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

#ifndef SF_WARDEN_BASE_H
#define SF_WARDEN_BASE_H

#include <map>
#include "Cryptography/ARC4.h"
#include "Cryptography/BigNumber.h"
#include "ByteBuffer.h"
#include "WardenCheckMgr.h"

enum WardenOpcodes
{
    // Client->Server
    WARDEN_CMSG_MODULE_MISSING                  = 0,
    WARDEN_CMSG_MODULE_OK                       = 1,
    WARDEN_CMSG_CHEAT_CHECKS_RESULT             = 2,
    WARDEN_CMSG_MEM_CHECKS_RESULT               = 3,        // only sent if MEM_CHECK bytes doesn't match
    WARDEN_CMSG_HASH_RESULT                     = 4,
    WARDEN_CMSG_MODULE_FAILED                   = 5,        // this is sent when client failed to load uploaded module due to cache fail
    WARDEN_CMSG_UNK                             = 10,

    // Server->Client
    WARDEN_SMSG_MODULE_USE                      = 0,
    WARDEN_SMSG_MODULE_CACHE                    = 1,
    WARDEN_SMSG_CHEAT_CHECKS_REQUEST            = 2,
    WARDEN_SMSG_MODULE_INITIALIZE               = 3,
    WARDEN_SMSG_MEM_CHECKS_REQUEST              = 4,        // byte len; while (!EOF) { byte unk(1); byte index(++); string module(can be 0); int offset; byte len; byte[] bytes_to_compare[len]; }
    WARDEN_SMSG_HASH_REQUEST                    = 5
};

enum WardenCheckType
{
    // end of checks packet
    CT_END                  = 0xFF,

    PAGE_CHECK_A            = 0x27, // (B2) 178: uint Seed + byte[20] SHA1 + uint Addr + byte Len (scans all pages for specified hash)
    PAGE_CHECK_B            = 0x68, // (BF) 191: uint Seed + byte[20] SHA1 + uint Addr + byte Len (scans only pages starts with MZ+PE headers for specified hash)
    MEM_CHECK               = 0x34, // 52: byte moduleNameIndex + uint Offset + byte Len (check to ensure memory isn't modified)
    MODULE_CHECK            = 0x4E, // 78: uint Seed + byte[20] SHA1 (check to ensure module isn't injected)

    // module info
    WOWWARDEN_INFO          = 0x75, // returns 3 64bit addresses: executable, warden, main warden struct

    // no data
    TIMING_CHECK            = 0xD0, // not sure

    MPQ_CHECK               = 0x98, // 152: byte fileNameIndex (check to ensure MPQ file isn't modified)
    LUA_STR_CHECK           = 0x8B, // 139: byte luaNameIndex (check to ensure LUA string isn't used)
    DRIVER_CHECK            = 0x71, // 113: uint Seed + byte[20] SHA1 + byte driverNameIndex (check to ensure driver isn't loaded)
    PROC_CHECK              = 0x7E, // 126: uint Seed + byte[20] SHA1 + byte moluleNameIndex + byte procNameIndex + uint Offset + byte Len (check to ensure proc isn't detoured)

};

#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

struct WardenModuleUse
{
    uint8 Command;
    uint8 ModuleHash[32];
    uint8 ModuleKey[16];
    uint32 Size;
};

struct WardenModuleTransfer
{
    uint8 Command;
    uint16 DataSize;
    uint8 Data[500];
};

struct WardenHashRequest
{
    uint8 Command;
    uint8 Seed[16];
};

#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

struct ClientWardenModule
{
    typedef std::pair<uint8 const*, size_t> MemChunk;

    MemChunk data;
    MemChunk key;
    MemChunk seed;
    MemChunk hash;
    MemChunk serverKeySeed;
    MemChunk clientKeySeed;
    MemChunk clientKeySeedHash;
};

struct WardenRequestContext
{
    ObjectGuid ResponseReceiverGUID = ObjectGuid::Empty;
    std::vector<uint16> MemCheckIDs;
    std::vector<uint16> OtherCheckIDs;
    uint32 CheckCount = 0;
    WardenCheck* CustomMemCheck = NULL;
    WardenCheck* CustomOtherCheck = NULL;
    WardenCheckResult* CustomMemResult = NULL;
    WardenCheckResult* CustomOtherResult = NULL;
};

struct WardenFailedCheckGroupRecord
{
    uint32 Count = 0;
    time_t LastFailTime;
    std::set<std::string> Comments;
};

class WorldSession;

class Warden
{
    friend class WardenWin;
    friend class WardenMac;

    public:
        Warden();
        virtual ~Warden();

        virtual void Init(WorldSession* session, BigNumber* k) = 0;
        virtual ClientWardenModule* GetModuleForClient() { return nullptr; }
        virtual void InitializeModule() { }
        virtual void RequestHash() = 0;
        virtual void HandleHashResult(ByteBuffer &buff) = 0;
        virtual void RequestData(WardenRequestContext* context = nullptr) = 0;
        virtual void HandleData(ByteBuffer &buff) = 0;

        virtual bool GetEndSceneAddress(int32&) { return false; }

        void SendModuleToClient();
        void RequestModule();
        void Update();
        void DecryptData(uint8* buffer, uint32 length);
        void EncryptData(uint8* buffer, uint32 length);

        virtual void Enable();
        virtual void Disable();

        bool IsBusy() const { return _dataSent; }
        bool IsEnabled() const { return _enabled; }

        void RecordFailedCheckGroup(std::string const& group, std::string const& comment)
        {
            WardenFailedCheckGroupRecord& record = _failedCheckGroups[group];
            ++record.Count;
            record.LastFailTime = time(nullptr);
            if (!comment.empty())
                record.Comments.insert(comment);
        }
        std::map<std::string, WardenFailedCheckGroupRecord> const& GetRecordedFailedCheckGroups() const { return _failedCheckGroups; }

        static bool IsValidCheckSum(uint32 checksum, const uint8 *data, const uint16 length);
        static uint32 BuildChecksum(const uint8 *data, uint32 length);

        // If no check is passed, the default action from config is executed
        std::string Penalty(WardenCheck* check = nullptr);

    private:
        void SendPacket(OpcodeServer opcode, void const *data, size_t dataSize);
    private:
        WorldSession* _session;
        uint8 _inputKey[16];
        uint8 _outputKey[16];
        uint8 _seed[16];
        Trinity::Crypto::ARC4 _inputCrypto;
        Trinity::Crypto::ARC4 _outputCrypto;
        uint32 _checkTimer;                          // Timer for sending check requests
        uint32 _clientResponseTimer;                 // Timer for client response delay
        bool _dataSent;
        uint32 _previousTimestamp;
        ClientWardenModule _module;
        bool _initialized;
        bool _enabled = true;

        ObjectGuid _responseReceiverGUID = ObjectGuid::Empty;
        uint32 _customChecks = 0;
        std::map<std::string, WardenFailedCheckGroupRecord> _failedCheckGroups;
};

#endif
