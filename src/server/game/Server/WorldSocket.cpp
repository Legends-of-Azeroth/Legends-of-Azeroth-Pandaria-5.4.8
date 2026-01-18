/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "WorldSocket.h"
#include "AuthenticationPackets.h"
#include "BigNumber.h"
#include "DatabaseEnv.h"
#include "GameTime.h"
#include "CryptoHash.h"
#include "CryptoRandom.h"
#include "IPLocation.h"
#include "Opcodes.h"
#include "PacketLog.h"
#include "Random.h"
//#include "RBAC.h"
#include "Realm.h"
#include "ScriptMgr.h"
#include "World.h"
#include "WorldSession.h"
#include <memory>
#include <zlib.h>
//#include "ServerPktHeader.h"

#pragma pack(push, 1)

struct CompressedWorldPacket
{
    uint32 UncompressedSize;
    uint32 UncompressedAdler;
    uint32 CompressedAdler;
};

#pragma pack(pop)

std::string const WorldSocket::ServerConnectionInitialize("WORLD OF WARCRAFT CONNECTION - SERVER TO CLIENT");
std::string const WorldSocket::ClientConnectionInitialize("WORLD OF WARCRAFT CONNECTION - CLIENT TO SERVER");
uint32 const WorldSocket::MinSizeForCompression = 0x400;
uint32 const SizeOfHeader = sizeof(uint16) + sizeof(uint16);

struct ServerPktHeader
{
    ServerPktHeader(uint32 size, uint32 cmd, bool encrypt) : size(size)
    {
        if (encrypt)
        {
            uint32 data = (size << 13) | (cmd & MAX_OPCODE);
            memcpy(&header[0], &data, 4);
            //_authCrypt->EncryptSend(reinterpret_cast<uint8*>(&header[0]), getHeaderLength());
        }
        else
        {
            // Dynamic header size is not needed anymore, we are using not encrypted part for only the first few packets
            memcpy(&header[0], &size, 2);
            memcpy(&header[2], &cmd, 2);
        }
    }

    uint8 getHeaderLength()
    {
        return 4;
    }

    const uint32 size;
    uint8 header[4];
};

using boost::asio::ip::tcp;

WorldSocket::WorldSocket(tcp::socket&& socket)
    : Socket(std::move(socket)), _OverSpeedPings(0), _worldSession(nullptr), _authed(false), _sendBufferSize(4096), _compressionStream(nullptr)
{
    Trinity::Crypto::GetRandomBytes(_authSeed);
    _headerBuffer.Resize(sizeof(ClientPktHeader));
}

WorldSocket::~WorldSocket()
{
    if (_compressionStream)
    {
        deflateEnd(_compressionStream);
        delete _compressionStream;
    }

}

void WorldSocket::Start()
{
    std::string ip_address = GetRemoteIpAddress().to_string();
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_IP_INFO);
    stmt->setString(0, ip_address);

    _queryProcessor.AddCallback(LoginDatabase.AsyncQuery(stmt).WithPreparedCallback(std::bind(&WorldSocket::CheckIpCallback, this, std::placeholders::_1)));
}

void WorldSocket::CheckIpCallback(PreparedQueryResult result)
{
    if (result)
    {
        bool banned = false;
        do
        {
            Field* fields = result->Fetch();
            if (fields[0].GetUInt64() != 0)
                banned = true;

        } while (result->NextRow());

        if (banned)
        {
            SendAuthResponseError(AUTH_REJECT);
            TC_LOG_ERROR("network", "WorldSocket::CheckIpCallback: Sent Auth Response (IP %s banned).", GetRemoteIpAddress().to_string().c_str());
            DelayedCloseSocket();
            return;
        }
    }

    _packetBuffer.Resize(2 + ClientConnectionInitialize.length() + 1);

    AsyncReadWithCallback(&WorldSocket::InitializeHandler);    

    MessageBuffer initializer;
    ServerPktHeader header(ServerConnectionInitialize.size(), 0, false);
    initializer.Write(header.header, header.getHeaderLength() - 2);
    initializer.Write(ServerConnectionInitialize.c_str(), ServerConnectionInitialize.length());

    // - io_service.run thread, safe.
    QueuePacket(std::move(initializer));

}

void WorldSocket::InitializeHandler(boost::system::error_code error, std::size_t transferedBytes)
{
    if (error)
    {
        CloseSocket();
        return;
    }

    GetReadBuffer().WriteCompleted(transferedBytes);

    MessageBuffer& packet = GetReadBuffer();
    if (packet.GetActiveSize() > 0)
    {
        if (_packetBuffer.GetRemainingSpace() > 0)
        {
            // need to receive the header
            std::size_t readHeaderSize = std::min(packet.GetActiveSize(), _packetBuffer.GetRemainingSpace());
            _packetBuffer.Write(packet.GetReadPointer(), readHeaderSize);
            packet.ReadCompleted(readHeaderSize);

            if (_packetBuffer.GetRemainingSpace() > 0)
            {
                // Couldn't receive the whole header this time.
                ASSERT(packet.GetActiveSize() == 0);
                AsyncReadWithCallback(&WorldSocket::InitializeHandler);
                return;
            }

            std::string initializer(reinterpret_cast<char const*>(_packetBuffer.GetReadPointer() + 2), std::min(_packetBuffer.GetActiveSize() - 2, ClientConnectionInitialize.length()));

            if (initializer != ClientConnectionInitialize)
            {
                CloseSocket();
                return;
            }

            _compressionStream = new z_stream();
            _compressionStream->zalloc = (alloc_func)nullptr;
            _compressionStream->zfree = (free_func)nullptr;
            _compressionStream->opaque = (voidpf)nullptr;
            _compressionStream->avail_in = 0;
            _compressionStream->next_in = nullptr;
            int32 z_res = deflateInit2(_compressionStream, sWorld->getIntConfig(CONFIG_COMPRESSION), Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);
            if (z_res != Z_OK)
            {
                CloseSocket();
                TC_LOG_ERROR("network", "Can't initialize packet compression (zlib: deflateInit) Error code: %i (%s)", z_res, zError(z_res));
                return;
            }

            _packetBuffer.Reset();
            HandleSendAuthSession();
            AsyncRead();
            return;
        }
    }

    AsyncReadWithCallback(&WorldSocket::InitializeHandler);
}

bool WorldSocket::Update()
{
    EncryptablePacket* queued;
    MessageBuffer buffer(_sendBufferSize);
    while (_bufferQueue.Dequeue(queued))
    {

        uint32 packetSize = queued->size();
        if (packetSize > MinSizeForCompression && queued->NeedsEncryption())
            packetSize = compressBound(packetSize) + sizeof(CompressedWorldPacket);

        if (buffer.GetRemainingSpace() < SizeOfHeader + packetSize)
        {
            QueuePacket(std::move(buffer));
            buffer.Resize(_sendBufferSize);
        }

        if (buffer.GetRemainingSpace() >=  SizeOfHeader + packetSize)
        {
            WritePacketToBuffer(*queued, buffer);
        }
        else    // single packet larger than 4096 bytes
        {
            MessageBuffer packetBuffer(SizeOfHeader + packetSize);
            WritePacketToBuffer(*queued, packetBuffer);
            QueuePacket(std::move(packetBuffer));
        }

        delete queued;
    }

    if (buffer.GetActiveSize() > 0)
        QueuePacket(std::move(buffer));

    if (!BaseSocket::Update())
        return false;

    _queryProcessor.ProcessReadyCallbacks();

    return true;
}

void WorldSocket::HandleSendAuthSession()
{
    WorldPackets::Auth::AuthChallenge challenge;
    challenge.Challenge = _authSeed;
    memcpy(challenge.DosChallenge.data(), Trinity::Crypto::GetRandomBytes<32>().data(), 32);
    challenge.DosZeroBits = 1;

    SendPacketAndLogOpcode(*challenge.Write());
}

void WorldSocket::OnClose()
{
    {
        std::lock_guard<std::mutex> sessionGuard(_worldSessionLock);
        _worldSession = nullptr;
    }
}

void WorldSocket::ReadHandler()
{
    if (!IsOpen())
        return;

    MessageBuffer& packet = GetReadBuffer();
    while (packet.GetActiveSize() > 0)
    {
        
        if (_headerBuffer.GetRemainingSpace() > 0)
        {
            // need to receive the header
            std::size_t readHeaderSize = std::min(packet.GetActiveSize(), _headerBuffer.GetRemainingSpace());
            _headerBuffer.Write(packet.GetReadPointer(), readHeaderSize);
            packet.ReadCompleted(readHeaderSize);

            if (_headerBuffer.GetRemainingSpace() > 0)
            {
                // Couldn't receive the whole header this time.
                ASSERT(packet.GetActiveSize() == 0);
                break;
            }

            // We just received nice new header
            if (!ReadHeaderHandler())
            {
                CloseSocket();
                return;
            }
        }

        // We have full read header, now check the data payload
        if (_packetBuffer.GetRemainingSpace() > 0)
        {
            // need more data in the payload
            std::size_t readDataSize = std::min(packet.GetActiveSize(), _packetBuffer.GetRemainingSpace());
            _packetBuffer.Write(packet.GetReadPointer(), readDataSize);
            packet.ReadCompleted(readDataSize);

            if (_packetBuffer.GetRemainingSpace() > 0)
            {
                // Couldn't receive the whole data this time.
                ASSERT(packet.GetActiveSize() == 0);
                break;
            }
        }

        // just received fresh new payload
        ReadDataHandlerResult result = ReadDataHandler();
        _headerBuffer.Reset();
        if (result != ReadDataHandlerResult::Ok)
        {
            if (result != ReadDataHandlerResult::WaitingForQuery)
            {
                CloseSocket();
            }

            return;
        }
    }

    AsyncRead();
}

bool WorldSocket::ReadHeaderHandler()
{

    if (_authCrypt.IsInitialized())
    {
        
        uint8* uintHeader = (uint8*)_headerBuffer.GetReadPointer();
        _authCrypt.DecryptRecv(uintHeader, sizeof(WorldClientPktHeader)); 
        WorldClientPktHeader* header = reinterpret_cast<WorldClientPktHeader*>(_headerBuffer.GetReadPointer());

        uint32 value = *(uint32*)uintHeader;
        header->cmd = value & 0x1FFF;
        header->size = ((value & ~(uint32)0x1FFF) >> 13);

        // if (header->size > 10236)
        // {
        //     Player* _player = _worldSession ? _worldSession->GetPlayer() : nullptr; // Removed this because it crashes the server if the player disconnects right before the log
        //     return false;
        // }
        //OpcodeClient opcode = static_cast<OpcodeClient>(PacketFilter::DropHighBytes(header->cmd));
        _packetBuffer.Resize(header->size);
        
    }
    else
    {

        ClientPktHeader* header = reinterpret_cast<ClientPktHeader*>(_headerBuffer.GetReadPointer());
        // EndianConvertReverse(header->size);
        // EndianConvert(header->cmd);       
        
        // if ((header->size < 4) || (header->size > 10240))
        // {
        //     Player* _player = _worldSession ? _worldSession->GetPlayer() : nullptr; // Removed this because it crashes the server if the player disconnects right before the log
        //     return false;
        // }

        header->size -= sizeof(header->cmd);

        if (header->IsValidSize() && header->IsValidOpcode()) // header->size > 0
        {
            _packetBuffer.Resize(header->size);
        }
    }

    return true;
}

void WorldSocket::WritePacketToBuffer(EncryptablePacket const& packet, MessageBuffer& buffer)
{
    uint32 opcode = packet.GetOpcode();
    uint32 packetSize = packet.size();

    // Reserve space for buffer
    uint8* headerPos = buffer.GetWritePointer();
    buffer.WriteCompleted(SizeOfHeader);

    if (packetSize > MinSizeForCompression && packet.NeedsEncryption())
    {
        CompressedWorldPacket cmp;
        cmp.UncompressedSize = packetSize + 4;
        cmp.UncompressedAdler = adler32(adler32(0x9827D8F1, reinterpret_cast<Bytef*>(&opcode), 4), packet.contents(), packetSize);

        // Reserve space for compression info - uncompressed size and checksums
        uint8* compressionInfo = buffer.GetWritePointer();
        buffer.WriteCompleted(sizeof(CompressedWorldPacket));

        uint32 compressedSize = CompressPacket(buffer.GetWritePointer(), packet);

        cmp.CompressedAdler = adler32(0x9827D8F1, buffer.GetWritePointer(), compressedSize);

        memcpy(compressionInfo, &cmp, sizeof(CompressedWorldPacket));
        buffer.WriteCompleted(compressedSize);
        packetSize = compressedSize + sizeof(CompressedWorldPacket);

        opcode = SMSG_COMPRESSED_PACKET;
    }
    else if (!packet.empty())
        buffer.Write(packet.contents(), packet.size());

    // packetSize += 2 /*opcode*/;

    ServerPktHeader header(!packet.NeedsEncryption() ? packetSize + 2 : packetSize, opcode, packet.NeedsEncryption());
    if (packet.NeedsEncryption())
        _authCrypt.EncryptSend(reinterpret_cast<uint8*>(&header.header), 4);

    memcpy(headerPos, &header.header, SizeOfHeader);
}

uint32 WorldSocket::CompressPacket(uint8* buffer, WorldPacket const& packet)
{
    uint32 opcode = packet.GetOpcode();
    uint32 bufferSize = deflateBound(_compressionStream, packet.size() + sizeof(opcode));

    _compressionStream->next_out = buffer;
    _compressionStream->avail_out = bufferSize;
    _compressionStream->next_in = (Bytef*)&opcode;
    _compressionStream->avail_in = sizeof(uint32);

    int32 z_res = deflate(_compressionStream, Z_NO_FLUSH);
    if (z_res != Z_OK)
    {
        TC_LOG_ERROR("network", "Can't compress packet opcode (zlib: deflate) Error code: %i (%s, msg: %s)", z_res, zError(z_res), _compressionStream->msg);
        return 0;
    }

    _compressionStream->next_in = (Bytef*)packet.contents();
    _compressionStream->avail_in = packet.size();

    z_res = deflate(_compressionStream, Z_SYNC_FLUSH);
    if (z_res != Z_OK)
    {
        TC_LOG_ERROR("network", "Can't compress packet data (zlib: deflate) Error code: %i (%s, msg: %s)", z_res, zError(z_res), _compressionStream->msg);
        return 0;
    }

    return bufferSize - _compressionStream->avail_out;
}


struct AuthSession
{
    uint8 digest[20];
    uint32 clientSeed;
    uint16 clientBuild;
    std::string Account;
    WorldPacket addonsData;
};

struct AccountInfo
{
    uint32 Id;
    ::SessionKey SessionKey;
    std::string LastIP;
    bool IsLockedToIP;
    std::string LockCountry;
    uint8 Expansion;
    int64 MuteTime;
    LocaleConstant Locale;
    uint32 Recruiter;
    std::string OS;
    bool IsBanned;
    bool IsRecruiter;
    uint32 Flags;
    AccountTypes Security;
    bool HasBoost;
    std::string MutedBy;
    std::string MuteReason;
    bool MutedInPublicChannelsOnly;
    uint32 OnlineMuteTimer;

    explicit AccountInfo(Field* fields)
    {
        //         0          1       2            3         4                5           6         7          8           9        10         11      12                                                            13     14        15          16                   17                     18               
        // SELECT a.id, a.sessionkey, a.last_ip, a.locked, a.lock_country, a.expansion, a.mutetime, a.locale, a.recruiter, a.os, a.flags, aa.gmlevel, ab.unbandate > UNIX_TIMESTAMP() OR ab.unbandate = ab.bandate, r.id, abo.id, am.muted_by, am.mute_reason, am.public_channels_only, m.mute_timer FROM account a LEFT JOIN account r ON a.id = r.recruiter LEFT JOIN account_access aa ON a.id = aa.id AND aa.RealmID IN (-1, ?) LEFT JOIN account_banned ab ON a.id = ab.id AND ab.active = 1 LEFT JOIN account_boost abo ON a.id=abo.id AND abo.realmid IN (-1, ?) LEFT JOIN mute_active m ON m.realmid = ? AND m.account = a.id LEFT JOIN account_muted am ON m.mute_id = am.id AND m.realmid = am.realmid WHERE a.username = ?
        Id = fields[0].GetUInt32();
        SessionKey = HexStrToByteArray<SESSION_KEY_LENGTH>(std::string_view(fields[1].GetString())); 
        LastIP = fields[2].GetString();
        IsLockedToIP = fields[3].GetBool();
        LockCountry = fields[4].GetString();
        Expansion = fields[5].GetUInt8();
        MuteTime = fields[6].GetInt64();
        Locale = LocaleConstant(fields[7].GetUInt8());
        Recruiter = fields[8].GetUInt32();
        OS = fields[9].GetString();
        Flags = fields[10].GetUInt32();
        Security = AccountTypes(fields[11].GetUInt8());
        IsBanned = fields[12].GetUInt64() != 0;
        IsRecruiter = fields[13].GetUInt32() != 0;
        HasBoost = fields[14].GetUInt32() != 0;
        MutedBy = fields[15].GetString();
        MuteReason = fields[16].GetString();
        MutedInPublicChannelsOnly = fields[17].GetBool();
        OnlineMuteTimer = fields[18].GetUInt32();

        uint32 world_expansion = sWorld->getIntConfig(CONFIG_EXPANSION);
        if (Expansion > world_expansion)
            Expansion = world_expansion;

        if (Locale >= TOTAL_LOCALES)
            Locale = LOCALE_enUS;
    }
};

WorldSocket::ReadDataHandlerResult WorldSocket::ReadDataHandler()
{

    ClientPktHeader* header = reinterpret_cast<ClientPktHeader*>(_headerBuffer.GetReadPointer());
    OpcodeClient opcode = static_cast<OpcodeClient>(header->cmd);

    WorldPacket packet(opcode, std::move(_packetBuffer));
    WorldPacket* packetToQueue;

    if (sPacketLog->CanLogPacket())
        sPacketLog->LogPacket(packet, CLIENT_TO_SERVER, GetRemoteIpAddress(), GetRemotePort());

    std::unique_lock<std::mutex> sessionGuard(_worldSessionLock, std::defer_lock);

    switch (opcode)
    {
        case CMSG_PING:
        {
            LogOpcodeText(opcode, sessionGuard);
            WorldPackets::Auth::Ping ping(std::move(packet));
            if (!ping.ReadNoThrow())
            {   
                TC_LOG_ERROR("network", "WorldSocket::ReadDataHandler(): client %s sent malformed CMSG_PING", GetRemoteIpAddress().to_string().c_str());
                return ReadDataHandlerResult::Error;
            } 
            if (!HandlePing(ping))
                return ReadDataHandlerResult::Error;
            return ReadDataHandlerResult::Ok;
        }
        case CMSG_AUTH_SESSION:
        {
            LogOpcodeText(opcode, sessionGuard);
            if (_authed)
            {
                // locking just to safely log offending user is probably overkill but we are disconnecting him anyway
                if (sessionGuard.try_lock())
                    TC_LOG_ERROR("network", "WorldSocket::ProcessIncoming: received duplicate CMSG_AUTH_SESSION from %s", _worldSession->GetPlayerInfo().c_str());
                return ReadDataHandlerResult::Error;
            }

            try
            {
                HandleAuthSession(packet);
                return ReadDataHandlerResult::WaitingForQuery;
            }
            catch (ByteBufferException const&)
            {
            }
            TC_LOG_ERROR("network", "WorldSocket::ReadDataHandler(): client %s sent malformed CMSG_AUTH_SESSION", GetRemoteIpAddress().to_string().c_str());
            return ReadDataHandlerResult::Error;
        }
        case CMSG_AUTH_CONTINUED_SESSION:
        {
            LogOpcodeText(opcode, sessionGuard);
            if (_authed)
            {
                // locking just to safely log offending user is probably overkill but we are disconnecting him anyway
                if (sessionGuard.try_lock())
                    TC_LOG_ERROR("network", "WorldSocket::ProcessIncoming: received duplicate CMSG_AUTH_CONTINUED_SESSION from %s", _worldSession->GetPlayerInfo().c_str());
                return ReadDataHandlerResult::Error;
            }

            // std::shared_ptr<WorldPackets::Auth::AuthContinuedSession> authSession = std::make_shared<WorldPackets::Auth::AuthContinuedSession>(std::move(packet));
            // if (!authSession->ReadNoThrow())
            // {
            //     TC_LOG_ERROR("network", "WorldSocket::ReadDataHandler(): client %s sent malformed CMSG_AUTH_CONTINUED_SESSION", GetRemoteIpAddress().to_string().c_str());
            //     return ReadDataHandlerResult::Error;
            // }
            HandleAuthContinuedSession(packet);
            return ReadDataHandlerResult::WaitingForQuery;
        }            
        case CMSG_KEEP_ALIVE: // todo: handle this packet in the same way of CMSG_TIME_SYNC_RESP
            sessionGuard.lock();
            LogOpcodeText(opcode, sessionGuard);
            if (_worldSession)
            {
                _worldSession->ResetTimeOutTime(true);
                return ReadDataHandlerResult::Ok;
            }
            TC_LOG_ERROR("network", "WorldSocket::ReadDataHandler: client %s sent CMSG_KEEP_ALIVE without being authenticated", GetRemoteIpAddress().to_string().c_str());
            return ReadDataHandlerResult::Error;

        case CMSG_LOG_DISCONNECT:
            LogOpcodeText(opcode, sessionGuard);
            packet.rfinish(); // contains uint32 disconnectReason;
            return ReadDataHandlerResult::Ok;

        case CMSG_ENABLE_NAGLE:
            LogOpcodeText(opcode, sessionGuard);
            SetNoDelay(false);
            return ReadDataHandlerResult::Ok;

        case CMSG_TIME_SYNC_RESP:
            packet.SetReceiveTime(std::chrono::steady_clock::now());
            packetToQueue = new WorldPacket(std::move(packet));
            break;

        default:
            packetToQueue = new WorldPacket(std::move(packet));
            break;
    }

    sessionGuard.lock();

    LogOpcodeText(opcode, sessionGuard);

    if (!_worldSession)
    {
        TC_LOG_ERROR("network.opcode", "ProcessIncoming: Client not authed opcode = %u", uint32(opcode));
        delete packetToQueue;
        return ReadDataHandlerResult::Error;
    }

    ClientOpcodeHandler const* handler = opcodeTable[opcode];
    //if (!handler)
    if (!handler || handler->Status == STATUS_UNHANDLED)
    {
        TC_LOG_ERROR("network.opcode", "No defined handler for opcode %s sent by %s", GetOpcodeNameForLogging(static_cast<OpcodeClient>(packet.GetOpcode())).c_str(), _worldSession->GetPlayerInfo().c_str());
        delete packetToQueue;
        //return ReadDataHandlerResult::Error;
        return ReadDataHandlerResult::Ok;
    }

    // Our Idle timer will reset on any non PING opcodes on login screen, allowing us to catch people idling.
    _worldSession->ResetTimeOutTime(false);

    // Copy the packet to the heap before enqueuing
    _worldSession->QueuePacket(packetToQueue);
   
    return ReadDataHandlerResult::Ok;

}

void WorldSocket::LogOpcodeText(OpcodeClient opcode, std::unique_lock<std::mutex> const& guard) const
{
    if (!guard)
    {
        TC_LOG_TRACE("network.opcode", "C->S: %s %s", GetRemoteIpAddress().to_string().c_str(), GetOpcodeNameForLogging(static_cast<OpcodeClient>(opcode)).c_str());
    }
    else
    {
        TC_LOG_TRACE("network.opcode", "C->S: %s %s", (_worldSession ? _worldSession->GetPlayerInfo() : GetRemoteIpAddress().to_string()).c_str(),
            GetOpcodeNameForLogging(static_cast<OpcodeClient>(opcode)).c_str());
    }
}

void WorldSocket::SendPacketAndLogOpcode(WorldPacket const& packet)
{
    TC_LOG_TRACE("network.opcode", "S->C: %s %s", GetRemoteIpAddress().to_string().c_str(), GetOpcodeNameForLogging(static_cast<OpcodeServer>(packet.GetOpcode())).c_str());
    SendPacket(packet);
}

void WorldSocket::SendPacket(WorldPacket const& packet)
{
    if (!IsOpen())
        return;

    if (sPacketLog->CanLogPacket())
        sPacketLog->LogPacket(packet, SERVER_TO_CLIENT, GetRemoteIpAddress(), GetRemotePort());

    _bufferQueue.Enqueue(new EncryptablePacket(packet, _authCrypt.IsInitialized()));
}

void WorldSocket::HandleAuthSession(WorldPacket& recvPacket)
{
    // Client switches packet headers after sending CMSG_AUTH_SESSION
    _headerBuffer.Resize(sizeof(WorldClientPktHeader));

    std::shared_ptr<AuthSession> authSession = std::make_shared<AuthSession>();

    // recvPacket.read(authSession->AddonInfo.contents(), authSession->AddonInfo.size()); // .contents will throw if empty, thats what we want

    uint32 addonSize;

    recvPacket.read_skip<uint32>();
    recvPacket.read_skip<uint32>();
    recvPacket >> authSession->digest[18];
    recvPacket >> authSession->digest[14];
    recvPacket >> authSession->digest[3];
    recvPacket >> authSession->digest[4];
    recvPacket >> authSession->digest[0];
    recvPacket.read_skip<uint32>();
    recvPacket >> authSession->digest[11];
    recvPacket >> authSession->clientSeed;
    recvPacket >> authSession->digest[19];
    recvPacket.read_skip<uint8>();
    recvPacket.read_skip<uint8>();
    recvPacket >> authSession->digest[2];
    recvPacket >> authSession->digest[9];
    recvPacket >> authSession->digest[12];
    recvPacket.read_skip<uint64>();
    recvPacket.read_skip<uint32>();
    recvPacket >> authSession->digest[16];
    recvPacket >> authSession->digest[5];
    recvPacket >> authSession->digest[6];
    recvPacket >> authSession->digest[8];
    recvPacket >> authSession->clientBuild;
    recvPacket >> authSession->digest[17];
    recvPacket >> authSession->digest[7];
    recvPacket >> authSession->digest[13];
    recvPacket >> authSession->digest[15];
    recvPacket >> authSession->digest[1];
    recvPacket >> authSession->digest[10];
    recvPacket >> addonSize;

    authSession->addonsData.resize(addonSize);
    recvPacket.read((uint8*)authSession->addonsData.contents(), addonSize);

    recvPacket.ReadBit();
    uint32 accountNameLength = recvPacket.ReadBits(11);

    authSession->Account = recvPacket.ReadString(accountNameLength);

    // Get the account information from the realmd database
    //         0           1        2         3          4              5              6          7        8            9       10       11        12                                                            13    14
    // SELECT a.id, a.sessionkey, a.last_ip, a.locked, a.lock_country, a.expansion, a.mutetime, a.locale, a.recruiter, a.os, a.flags, aa.gmlevel, ab.unbandate > UNIX_TIMESTAMP() OR ab.unbandate = ab.bandate, r.id, abo.id, am.muted_by, am.mute_reason, am.public_channels_only, m.mute_timer FROM account a LEFT JOIN account r ON a.id = r.recruiter LEFT JOIN account_access aa ON a.id = aa.id AND aa.RealmID IN (-1, ?) LEFT JOIN account_banned ab ON a.id = ab.id AND ab.active = 1 LEFT JOIN account_boost abo ON a.id=abo.id AND abo.realmid IN (-1, ?) LEFT JOIN mute_active m ON m.realmid = ? AND m.account = a.id LEFT JOIN account_muted am ON m.mute_id = am.id AND m.realmid = am.realmid WHERE a.username = ?
    // size_t hashPos = authSession->account.find_last_of('#');
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_INFO_BY_NAME);
    stmt->setInt32(0, int32(realm.Id.Realm));
    stmt->setInt32(1, int32(realm.Id.Realm));
    stmt->setInt32(2, int32(realm.Id.Realm));
    stmt->setString(3, authSession->Account);

    _queryProcessor.AddCallback(LoginDatabase.AsyncQuery(stmt).WithPreparedCallback(std::bind(&WorldSocket::HandleAuthSessionCallback, this, authSession, std::placeholders::_1)));

}

void WorldSocket::HandleAuthSessionCallback(std::shared_ptr<AuthSession> authSession, PreparedQueryResult result)
{

    // // For hook purposes, we get Remoteaddress at this point.
    // std::string address = GetRemoteIpAddress().to_string();

    // LoginDatabasePreparedStatement* stmt = nullptr;

    // if (sWorld->getBoolConfig(CONFIG_ALLOW_LOGGING_IP_ADDRESSES_IN_DATABASE))
    // {
    //     // As we don't know if attempted login process by ip works, we update last_attempt_ip right away
    //     stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_LAST_ATTEMPT_IP);
    //     stmt->setString(0, address);
    //     stmt->setString(1, authSession->Account);
    //     LoginDatabase.Execute(stmt);
    //     // This also allows to check for possible "hack" attempts on account
    // }

    // // even if auth credentials are bad, try using the session key we have - client cannot read auth response error without it
    // _authCrypt.Init(account.SessionKey);

    // if (authSession->RealmID != realm.Id.Realm)
    // {
    //     SendAuthResponseError(REALM_LIST_REALM_NOT_FOUND);
    //     TC_LOG_ERROR("network", "WorldSocket::HandleAuthSession: Client %s requested connecting with realm id %u but this realm has id %u set in config.",
    //         GetRemoteIpAddress().to_string().c_str(), authSession->RealmID, realm.Id.Realm);
    //     DelayedCloseSocket();
    //     return;
    // }

    // // Must be done before WorldSession is created
    // bool wardenActive = sWorld->getBoolConfig(CONFIG_WARDEN_ENABLED);
    // if (wardenActive && account.OS != "Win" && account.OS != "OSX")
    // {
    //     SendAuthResponseError(AUTH_REJECT);
    //     TC_LOG_ERROR("network", "WorldSocket::HandleAuthSession: Client %s attempted to log in using invalid client OS (%s).", address.c_str(), account.OS.c_str());
    //     DelayedCloseSocket();
    //     return;
    // }

    // // Check that Key and account name are the same on client and server
    // uint8 t[4] = { 0x00,0x00,0x00,0x00 };

    // Trinity::Crypto::SHA1 sha;
    // sha.UpdateData(authSession->Account);
    // sha.UpdateData(t);
    // sha.UpdateData(authSession->LocalChallenge);
    // sha.UpdateData(_authSeed);
    // sha.UpdateData(account.SessionKey);
    // sha.Finalize();

    // if (sha.GetDigest() != authSession->Digest)
    // {
    //     SendAuthResponseError(AUTH_FAILED);
    //     TC_LOG_ERROR("network", "WorldSocket::HandleAuthSession: Authentication failed for account: %u ('%s') address: %s", account.Id, authSession->Account.c_str(), address.c_str());
    //     DelayedCloseSocket();
    //     return;
    // }

    // if (IpLocationRecord const* location = sIPLocation->GetLocationRecord(address))
    //     _ipCountry = location->CountryCode;

    // ///- Re-check ip locking (same check as in auth).
    // if (account.IsLockedToIP)
    // {
    //     if (account.LastIP != address)
    //     {
    //         SendAuthResponseError(AUTH_FAILED);
    //         TC_LOG_DEBUG("network", "WorldSocket::HandleAuthSession: Sent Auth Response (Account IP differs. Original IP: %s, new IP: %s).", account.LastIP.c_str(), address.c_str());
    //         // We could log on hook only instead of an additional db log, however action logger is config based. Better keep DB logging as well
    //         sScriptMgr->OnFailedAccountLogin(account.Id);
    //         DelayedCloseSocket();
    //         return;
    //     }
    // }
    // else if (!account.LockCountry.empty() && account.LockCountry != "00" && !_ipCountry.empty())
    // {
    //     if (account.LockCountry != _ipCountry)
    //     {
    //         SendAuthResponseError(AUTH_FAILED);
    //         TC_LOG_DEBUG("network", "WorldSocket::HandleAuthSession: Sent Auth Response (Account country differs. Original country: %s, new country: %s).", account.LockCountry.c_str(), _ipCountry.c_str());
    //         // We could log on hook only instead of an additional db log, however action logger is config based. Better keep DB logging as well
    //         sScriptMgr->OnFailedAccountLogin(account.Id);
    //         DelayedCloseSocket();
    //         return;
    //     }
    // }

    // int64 mutetime = account.MuteTime;
    // //! Negative mutetime indicates amount of seconds to be muted effective on next login - which is now.
    // if (mutetime < 0)
    // {
    //     mutetime = GameTime::GetGameTime() + llabs(mutetime);

    //     stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME_LOGIN);
    //     stmt->setInt64(0, mutetime);
    //     stmt->setUInt32(1, account.Id);
    //     LoginDatabase.Execute(stmt);
    // }

    // // Check locked state for server
    // AccountTypes allowedAccountType = sWorld->GetPlayerSecurityLimit();
    // TC_LOG_DEBUG("network", "Allowed Level: %u Player Level %u", allowedAccountType, account.Security);
    // if (allowedAccountType > SEC_PLAYER && account.Security < allowedAccountType)
    // {
    //     SendAuthResponseError(AUTH_UNAVAILABLE);
    //     TC_LOG_DEBUG("network", "WorldSocket::HandleAuthSession: User tries to login but his security level is not enough");
    //     sScriptMgr->OnFailedAccountLogin(account.Id);
    //     DelayedCloseSocket();
    //     return;
    // }

    // TC_LOG_DEBUG("network", "WorldSocket::HandleAuthSession: Client '%s' authenticated successfully from %s.", authSession->Account.c_str(), address.c_str());

    // if (sWorld->getBoolConfig(CONFIG_ALLOW_LOGGING_IP_ADDRESSES_IN_DATABASE))
    // {
    //     // Update the last_ip in the database as it was successful for login
    //     stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_LAST_IP);

    //     stmt->setString(0, address);
    //     stmt->setString(1, authSession->Account);

    //     LoginDatabase.Execute(stmt);
    // }

    // // At this point, we can safely hook a successful login
    // sScriptMgr->OnAccountLogin(account.Id);

    // _authed = true;
    // _worldSession = new WorldSession(account.Id, std::move(authSession->Account), shared_from_this(), account.Security,
    //     account.Expansion, mutetime, account.Locale, account.Recruiter, account.IsRectuiter);
    // _worldSession->ReadAddonsInfo(authSession->AddonInfo);

    // // Initialize Warden system only if it is enabled by config
    // if (wardenActive)
    //     _worldSession->InitWarden(account.SessionKey, account.OS);

    //_queryProcessor.AddCallback(_worldSession->LoadPermissionsAsync().WithPreparedCallback(std::bind(&WorldSocket::LoadSessionPermissionsCallback, this, std::placeholders::_1)));
    

    // Stop if the account is not found
    if (!result)
    {
        // We can not log here, as we do not know the account. Thus, no accountId.
        SendAuthResponseError(AUTH_UNKNOWN_ACCOUNT);
        TC_LOG_ERROR("network", "WorldSocket::HandleAuthSession: Sent Auth Response (unknown account).");
        DelayedCloseSocket();
        return;
    }

    AccountInfo account(result->Fetch());

    ///- Re-check ip locking (same check as in realmd).
    if (account.IsLockedToIP) // if ip is locked
    {
        if (strcmp (account.LastIP.c_str(), GetRemoteIpAddress().to_string().c_str()))
        {
            SendAuthResponseError(AUTH_FAILED);
            TC_LOG_DEBUG("network", "WorldSocket::HandleAuthSession: Sent Auth Response (Account IP differs).");
            DelayedCloseSocket();
            return;
        }
    }

    BigNumber k;
    // k.SetHexStr(fields[1].GetCString());

    // Must be done before WorldSession is created
    if (sWorld->getBoolConfig(CONFIG_WARDEN_ENABLED) && account.OS != "Win" && account.OS != "Wn64" && account.OS != "OSX")
    {
        SendAuthResponseError(AUTH_REJECT);
        TC_LOG_ERROR("network", "WorldSocket::HandleAuthSession: Client %s attempted to log in using invalid client OS (%s).", GetRemoteIpAddress().to_string().c_str(), account.OS.c_str());
        DelayedCloseSocket();
        return;
    }

    if (account.IsBanned) // if account banned
    {
        SendAuthResponseError(AUTH_BANNED);
        TC_LOG_ERROR("network", "WorldSocket::HandleAuthSession: Sent Auth Response (Account banned).");
        DelayedCloseSocket();
        return;
    }

    // Check locked state for server
    AccountTypes allowedAccountType = sWorld->GetPlayerSecurityLimit();
    TC_LOG_DEBUG("network", "Allowed Level: %u Player Level %u", allowedAccountType, account.Security);
    if (allowedAccountType > SEC_PLAYER && account.Security < allowedAccountType)
    {
        SendAuthResponseError(AUTH_UNAVAILABLE);
        TC_LOG_INFO("network", "WorldSocket::HandleAuthSession: User tries to login but his security level is not enough");
        DelayedCloseSocket();
        return;
    }

    // Check that Key and account name are the same on client and server
    uint8 t[4] = { 0x00,0x00,0x00,0x00 };

    Trinity::Crypto::SHA1 sha;
    sha.UpdateData(authSession->Account);
    sha.UpdateData(t);
    //sha.UpdateData(authSession->clientSeed);
    sha.UpdateData((uint8*)&authSession->clientSeed, 4);
    sha.UpdateData(_authSeed);
    sha.UpdateData(account.SessionKey);
    sha.Finalize();

    std::string address = GetRemoteIpAddress().to_string();

    if (memcmp(sha.GetDigest().data(), authSession->digest, 20)) //(sha.GetDigest() != authSession->digest) //if (memcmp(sha.GetDigest(), authSession->digest, 20))
    {
        SendAuthResponseError(AUTH_FAILED);
        TC_LOG_ERROR("network", "WorldSocket::HandleAuthSession: Authentication failed for account: %u ('%s') address: %s", account.Id, authSession->Account.c_str(), address.c_str());
        DelayedCloseSocket();
        return;
    }

    if (IpLocationRecord const* location = sIPLocation->GetLocationRecord(address))
        _ipCountry = location->CountryCode;


    TC_LOG_DEBUG("network", "WorldSocket::HandleAuthSession: Client '%s' authenticated successfully from %s.",
        authSession->Account.c_str(),
        address.c_str());

    // Update the last_ip in the database

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_LAST_IP);
    stmt->setString(0, address);
    stmt->setString(1, authSession->Account);
    LoginDatabase.Execute(stmt);

    _authCrypt.Init(account.SessionKey);

    // First reject the connection if packet contains invalid data or realm state doesn't allow logging in
    if (sWorld->IsClosed())
    {
        SendAuthResponseError(AUTH_REJECT);
        TC_LOG_ERROR("network", "WorldSocket::HandleAuthSession: World closed, denying client (%s).", GetRemoteIpAddress().to_string().c_str());
        DelayedCloseSocket();
        return;
    }

    _authed = true;
    _worldSession = new WorldSession(account.Id, shared_from_this(), account.Security, account.Expansion, account.MuteTime, account.Locale, account.Recruiter, account.Flags, account.IsRecruiter, account.HasBoost);
    _worldSession->SetMute({ account.OnlineMuteTimer, account.MutedBy, account.MuteReason, account.MutedInPublicChannelsOnly });
    _worldSession->ReadAddonsInfo(authSession->addonsData);
    sWorld->AddSession(_worldSession);

    // Initialize Warden system only if it is enabled by config
    if (sWorld->getBoolConfig(CONFIG_WARDEN_ENABLED))
        _worldSession->InitWarden(&k, account.OS);

    // Sleep this Network thread for
    // uint32 sleepTime = sWorld->getIntConfig(CONFIG_SESSION_ADD_DELAY);
    // std::this_thread::sleep_for(Microseconds(sleepTime));
    

    //_queryProcessor.AddCallback(_worldSession->LoadPermissionsAsync().WithPreparedCallback(std::bind(&WorldSocket::LoadSessionPermissionsCallback, this, std::placeholders::_1)));
    
    AsyncRead();
}

void WorldSocket::LoadSessionPermissionsCallback(PreparedQueryResult result)
{
    // RBAC must be loaded before adding session to check for skip queue permission
    //_worldSession->GetRBACData()->LoadFromDBCallback(result);

    //sWorld->AddSession(_worldSession);
}

void WorldSocket::HandleAuthContinuedSession(WorldPacket& recvPacket)
{

    uint64 DosResponse = 0;
    uint64 Key = 0;
    std::array<uint8, 20> Digest;

    recvPacket >> Key;
    recvPacket >> DosResponse;

    recvPacket >> Digest[1];
    recvPacket >> Digest[14];
    recvPacket >> Digest[9];
    recvPacket >> Digest[18];
    recvPacket >> Digest[17];
    recvPacket >> Digest[8];
    recvPacket >> Digest[6];
    recvPacket >> Digest[10];
    recvPacket >> Digest[3];
    recvPacket >> Digest[16];
    recvPacket >> Digest[4];
    recvPacket >> Digest[0];
    recvPacket >> Digest[15];
    recvPacket >> Digest[2];
    recvPacket >> Digest[19];
    recvPacket >> Digest[12];
    recvPacket >> Digest[13];
    recvPacket >> Digest[5];
    recvPacket >> Digest[11];
    recvPacket >> Digest[7];

    WorldSession::ConnectToKey key;
    key.Raw = Key;

    // _type = ConnectionType(key.Fields.ConnectionType);
    // if (_type != CONNECTION_TYPE_INSTANCE)
    // {
    //     SendAuthResponseError(AUTH_UNKNOWN_ACCOUNT);
    //     DelayedCloseSocket();
    //     return;
    // }

    // Client switches packet headers after sending CMSG_AUTH_CONTINUED_SESSION
    _headerBuffer.Resize(sizeof(ClientPktHeader));

    TC_LOG_DEBUG("network", "WorldSocket::HandleAuthContinuedSession: AccountId (%u).", uint32(key.Fields.AccountId));

    // uint32 accountId = uint32(key.Fields.AccountId);
    // LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_INFO_CONTINUED_SESSION);
    // stmt->setUInt32(0, accountId);

    //_queryProcessor.AddCallback(LoginDatabase.AsyncQuery(stmt).WithPreparedCallback(std::bind(&WorldSocket::HandleAuthContinuedSessionCallback, this, authSession, std::placeholders::_1)));
}

void WorldSocket::HandleAuthContinuedSessionCallback(WorldSession::ConnectToKey key, PreparedQueryResult result) // std::shared_ptr<WorldPackets::Auth::AuthContinuedSession> authSession
{
    if (!result)
    {
        SendAuthResponseError(AUTH_UNKNOWN_ACCOUNT);
        DelayedCloseSocket();
        return;
    }

    // WorldSession::ConnectToKey key;
    // key.Raw = authSession->Key;

    uint32 accountId = uint32(key.Fields.AccountId);
    Field* fields = result->Fetch();
    std::string login = fields[0].GetString();

    ::SessionKey SessionKey = HexStrToByteArray<SESSION_KEY_LENGTH>(std::string_view(fields[1].GetString())); 
    _authCrypt.Init(SessionKey);

    uint8 t[4] = { 0x00,0x00,0x00,0x00 };

    // Trinity::Crypto::SHA1 sha;
    // sha.UpdateData(authSession->Account);
    // sha.UpdateData(t);
    // //sha.UpdateData(authSession->clientSeed);
    // sha.UpdateData((uint8*)&authSession->clientSeed, 4);
    // sha.UpdateData(_authSeed);
    // sha.UpdateData(account.SessionKey);
    // sha.Finalize();

    // BigNumber k;
    // k.SetHexStr(fields[1].GetCString());

    // _authCrypt.Init(&k, _encryptSeed.AsByteArray().get(), _decryptSeed.AsByteArray().get());

    // SHA1Hash sha;
    // sha.UpdateData(login);
    // sha.UpdateBigNumbers(&k, nullptr);
    // sha.UpdateData((uint8*)&_authSeed, 4);
    // sha.Finalize();

    // if (memcmp(sha.GetDigest(), authSession->Digest.data(), sha.GetLength()))
    // {
    //     SendAuthResponseError(AUTH_UNKNOWN_ACCOUNT);
    //     TC_LOG_ERROR("network", "WorldSocket::HandleAuthContinuedSession: Authentication failed for account: %u ('%s') address: %s", accountId, login.c_str(), GetRemoteIpAddress().to_string().c_str());
    //     DelayedCloseSocket();
    //     return;
    // }

    // sWorld->AddInstanceSocket(shared_from_this(), authSession->Key);
    // AsyncRead();
}

void WorldSocket::SendAuthResponseError(uint8 code)
{
    WorldPacket packet(SMSG_AUTH_RESPONSE, 1);
    packet.WriteBit(0); // has account info
    packet.WriteBit(0); // has queue info
    packet.FlushBits();
    packet << uint8(code);

    SendPacketAndLogOpcode(packet);
}

bool WorldSocket::HandlePing(WorldPackets::Auth::Ping& ping)
{
    using namespace std::chrono;

    if (_LastPingTime == steady_clock::time_point())
    {
        _LastPingTime = steady_clock::now();
    }
    else
    {
        steady_clock::time_point now = steady_clock::now();

        steady_clock::duration diff = now - _LastPingTime;

        _LastPingTime = now;

        if (diff < seconds(27))
        {
            ++_OverSpeedPings;

            uint32 maxAllowed = sWorld->getIntConfig(CONFIG_MAX_OVERSPEED_PINGS);

            if (maxAllowed && _OverSpeedPings > maxAllowed)
            {
                std::unique_lock<std::mutex> sessionGuard(_worldSessionLock);

                //if (_worldSession && !_worldSession->HasPermission(rbac::RBAC_PERM_SKIP_CHECK_OVERSPEED_PING))
                if (_worldSession && _worldSession->GetSecurity() == SEC_PLAYER)
                {
                    TC_LOG_ERROR("network", "WorldSocket::HandlePing: %s kicked for over-speed pings (address: %s)",
                        _worldSession->GetPlayerInfo().c_str(), GetRemoteIpAddress().to_string().c_str());

                    return false;
                }
            }
        }
        else
            _OverSpeedPings = 0;
    }

    {
        std::lock_guard<std::mutex> sessionGuard(_worldSessionLock);

        if (_worldSession)
        {
            _worldSession->SetLatency(ping.Latency);
            _worldSession->ResetClientTimeDelay();
        }
        else
        {
            TC_LOG_ERROR("network", "WorldSocket::HandlePing: peer sent CMSG_PING, but is not authenticated or got recently kicked, address = %s", GetRemoteIpAddress().to_string().c_str());
            return false;
        }
    }

    SendPacketAndLogOpcode(*WorldPackets::Auth::Pong(ping.Serial).Write());
    return true;
}
