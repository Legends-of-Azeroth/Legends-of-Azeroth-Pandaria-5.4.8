/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
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

#include "Common.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Log.h"
#include "Opcodes.h"
#include "ByteBuffer.h"
#include "World.h"
#include "Player.h"
#include "Util.h"
#include "Warden.h"
#include "AccountMgr.h"
#include "CryptoHash.h"

Warden::Warden() : _checkTimer(10000/*10 sec*/), _clientResponseTimer(0), _dataSent(false), _initialized(false) { }

Warden::~Warden()
{
}

void Warden::SendModuleToClient()
{
    TC_LOG_DEBUG("warden", "Send module to client");

    // Create packet structure
    WardenModuleTransfer packet;

    uint32 sizeLeft = _module.data.second;
    uint32 pos = 0;
    uint16 burstSize;
    while (sizeLeft > 0)
    {
        burstSize = sizeLeft < 500 ? sizeLeft : 500;
        packet.Command = WARDEN_SMSG_MODULE_CACHE;
        packet.DataSize = burstSize;
        memcpy(packet.Data, &_module.data.first[pos], burstSize);
        sizeLeft -= burstSize;
        pos += burstSize;

        EncryptData((uint8*)&packet, burstSize + 3);
        SendPacket(SMSG_WARDEN_DATA, &packet, burstSize + 3);
    }
}

void Warden::RequestModule()
{
    TC_LOG_DEBUG("warden", "Request module");

    // Create packet structure
    WardenModuleUse request;
    request.Command = WARDEN_SMSG_MODULE_USE;

    memcpy(request.ModuleHash, _module.hash.first, _module.hash.second);
    memcpy(request.ModuleKey, _module.key.first, _module.key.second);
    request.Size = _module.data.second;

    // Encrypt with warden RC4 key.
    EncryptData((uint8*)&request, sizeof(WardenModuleUse));

    SendPacket(SMSG_WARDEN_DATA, &request, sizeof(request));
}

void Warden::Update()
{
    if (_initialized)
    {
        uint32 currentTimestamp = getMSTime();
        uint32 diff = currentTimestamp - _previousTimestamp;
        _previousTimestamp = currentTimestamp;

        if (_dataSent)
        {
            uint32 maxClientResponseDelay = sWorld->getIntConfig(CONFIG_WARDEN_CLIENT_RESPONSE_DELAY);

            if (maxClientResponseDelay > 0)
            {
                // Kick player if client response delays more than set in config
                if (_clientResponseTimer > maxClientResponseDelay * IN_MILLISECONDS)
                {
                    TC_LOG_WARN("warden", "{} (latency: {}, IP: {}) exceeded Warden module response delay for more than {} - disconnecting client",
                                   _session->GetPlayerInfo().c_str(), _session->GetLatency(), _session->GetRemoteAddress().c_str(), secsToTimeString(maxClientResponseDelay, true).c_str());

                    _clientResponseTimer = 0;
                    RecordFailedCheckGroup("!timeout", "");
                    //_session->KickPlayer();
                }
                else
                    _clientResponseTimer += diff;
            }
        }
        else if (_checkTimer)
        {
            if (diff >= _checkTimer)
            {
                RequestData();
            }
            else
                _checkTimer -= diff;
        }
    }
}

void Warden::SendPacket(OpcodeServer opcode, void const *data, size_t dataSize)
{
    WorldPacket packet(opcode, sizeof(uint32) + dataSize);
    packet << uint32(dataSize);
    packet.append(static_cast<uint8 const *>(data), dataSize);

    _session->SendPacket(&packet);
}

void Warden::DecryptData(uint8* buffer, uint32 length)
{
    _inputCrypto.UpdateData(buffer, length);
}

void Warden::EncryptData(uint8* buffer, uint32 length)
{
    _outputCrypto.UpdateData(buffer, length);
}

bool Warden::IsValidCheckSum(uint32 checksum, const uint8* data, const uint16 length)
{
    uint32 newChecksum = BuildChecksum(data, length);

    if (checksum != newChecksum)
    {
        TC_LOG_DEBUG("warden", "CHECKSUM IS NOT VALID");
        return false;
    }
    else
    {
        TC_LOG_DEBUG("warden", "CHECKSUM IS VALID");
        return true;
    }
}

union keyData
{
    std::array<uint8, 20> bytes;
    std::array<uint32, 5> ints;
};

uint32 Warden::BuildChecksum(const uint8* data, uint32 length)
{

    keyData hash;
    hash.bytes = Trinity::Crypto::SHA1::GetDigestOf(data, size_t(length));
    uint32 checkSum = 0;
    for (uint8 i = 0; i < 5; ++i)
        checkSum = checkSum ^ hash.ints[i];

    return checkSum;
}

std::string Warden::Penalty(WardenCheck* check /*= NULL*/)
{
    WardenActions action;

    if (check)
        action = check->Action;
    else
        action = WardenActions(sWorld->getIntConfig(CONFIG_WARDEN_CLIENT_FAIL_ACTION));

    switch (action)
    {
    case WARDEN_ACTION_LOG:
    case WARDEN_ACTION_LOG_GM:
        return "None";
        break;
    case WARDEN_ACTION_KICK:
        _session->KickPlayer();
        return "Kick";
        break;
    case WARDEN_ACTION_BAN:
        {
            std::stringstream duration;
            duration << sWorld->getIntConfig(CONFIG_WARDEN_CLIENT_BAN_DURATION) << "s";
            std::string accountName;
            AccountMgr::GetName(_session->GetAccountId(), accountName);
            std::stringstream banReason;
            banReason << "Warden Anticheat Violation";
            // Check can be NULL, for example if the client sent a wrong signature in the warden packet (CHECKSUM FAIL)
            if (check)
                banReason << ": " << check->Comment << " (CheckId: " << check->CheckId << ")";

            sWorld->BanAccount(BAN_ACCOUNT, accountName, duration.str(), banReason.str(), "Server");

            return "Ban";
        }
    default:
        break;
    }
    return "Undefined";
}

void Warden::Enable()
{
    if (_enabled)
        return;

    _enabled = true;
    _checkTimer = 1 * IN_MILLISECONDS;
}

void Warden::Disable()
{
    if (!_enabled)
        return;

    _enabled = false;
    _checkTimer = 0;
}

void WorldSession::HandleWardenDataOpcode(WorldPacket& recvData)
{
    uint32 len = recvData.read<uint32>();
    _warden->DecryptData(recvData.contents() + recvData.rpos(), len);
    uint8 opcode;
    recvData >> opcode;
    TC_LOG_DEBUG("warden", "Got packet, opcode {}, size {}", opcode, uint32(recvData.size()));
    recvData.hexlike();

    switch (opcode)
    {
        case WARDEN_CMSG_MODULE_MISSING:
            _warden->SendModuleToClient();
            break;
        case WARDEN_CMSG_MODULE_OK:
            _warden->RequestHash();
            break;
        case WARDEN_CMSG_CHEAT_CHECKS_RESULT:
            _warden->HandleData(recvData);
            break;
        case WARDEN_CMSG_MEM_CHECKS_RESULT:
            TC_LOG_DEBUG("warden", "NYI WARDEN_CMSG_MEM_CHECKS_RESULT received!");
            break;
        case WARDEN_CMSG_HASH_RESULT:
            recvData.rfinish(); // NYI opcode
            _warden->HandleHashResult(recvData);
            _warden->InitializeModule();
            break;
        case WARDEN_CMSG_MODULE_FAILED:
            TC_LOG_DEBUG("warden", "NYI WARDEN_CMSG_MODULE_FAILED received!");
            break;
        case WARDEN_CMSG_UNK:
            recvData.rfinish(); // NYI opcode
            break;
        default:
            TC_LOG_DEBUG("warden", "Got unknown warden opcode {} of size {}.", opcode, uint32(recvData.size() - 1));
            break;
    }
}
