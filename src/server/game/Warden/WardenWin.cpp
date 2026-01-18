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

#include "Cryptography/HMAC.h"
#include "Cryptography/CryptoRandom.h"
#include "Cryptography/WardenKeyGeneration.h"
#include "Common.h"
#include "WorldSession.h"
#include "Log.h"
#include "Opcodes.h"
#include "ByteBuffer.h"
#include "World.h"
#include "Player.h"
#include "Util.h"
#include "WardenWin.h"
#include "WardenModuleWin.h"
#include "WardenCheckMgr.h"
#include "Chat.h"
#include "Language.h"

WardenWin::WardenWin() : Warden() { }

WardenWin::~WardenWin() { }

void WardenWin::Init(WorldSession* session, BigNumber* k)
{
    _session = session;
    // Generate Warden Key
    SHA1Randx WK(k->AsByteArray(), k->GetNumBytes());
    WK.Generate(_inputKey, 16);
    WK.Generate(_outputKey, 16);

    // Initial encryption is done with session key
    uint8 inputKey[16]; // MD5_DIGEST_LENGTH
    uint8 outputKey[16];

    auto const inputKeySize = sizeof(inputKey);
    auto const outputKeySize = sizeof(outputKey);

    WK.Generate(inputKey, inputKeySize);
    WK.Generate(outputKey, outputKeySize);

    _inputCrypto.Init(inputKey);
    _outputCrypto.Init(outputKey);

    _module.data = std::make_pair(winWardenModule.Module, sizeof(winWardenModule.Module));
    _module.key = std::make_pair(winWardenModule.ModuleKey, sizeof(winWardenModule.ModuleKey));
    _module.seed = std::make_pair(winWardenModule.Seed, sizeof(winWardenModule.Seed));
    _module.hash = std::make_pair(winWardenModule.Hash, sizeof(winWardenModule.Hash));
    _module.serverKeySeed = std::make_pair(winWardenModule.ServerKeySeed, sizeof(winWardenModule.ServerKeySeed));
    _module.clientKeySeed = std::make_pair(winWardenModule.ClientKeySeed, sizeof(winWardenModule.ClientKeySeed));
    _module.clientKeySeedHash = std::make_pair(winWardenModule.ClientKeySeedHash, sizeof(winWardenModule.ClientKeySeedHash));

    TC_LOG_DEBUG("warden", "%u - Server side warden initializing...", session->GetAccountId());
    TC_LOG_DEBUG("warden", "%u - C->S Key: %s", session->GetAccountId(), Trinity::Impl::ByteArrayToHexStr(inputKey, inputKeySize).c_str());
    TC_LOG_DEBUG("warden", "%u - S->C Key: %s", session->GetAccountId(), Trinity::Impl::ByteArrayToHexStr(outputKey, outputKeySize).c_str());
    TC_LOG_DEBUG("warden", "%u - Module Key: %s", session->GetAccountId(), Trinity::Impl::ByteArrayToHexStr(_module.key.first, _module.key.second).c_str());
    TC_LOG_DEBUG("warden", "%u - Module Hash: %s", session->GetAccountId(), Trinity::Impl::ByteArrayToHexStr(_module.hash.first, _module.hash.second).c_str());

    RequestModule();
    DoCustomMemCheck(12967816, 4);
    _state = State::EndSceneRequestStep1;
}

void WardenWin::RequestHash()
{
    TC_LOG_DEBUG("warden", "%u - Request hash", _session->GetAccountId());

    // Create packet structure
    WardenHashRequest request;
    request.Command = WARDEN_SMSG_HASH_REQUEST;
    memcpy(request.Seed, _module.seed.first, _module.seed.second);

    // Encrypt with warden RC4 key.
    EncryptData((uint8*)&request, sizeof(WardenHashRequest));

    // Send packet
    SendPacket(SMSG_WARDEN_DATA, &request, sizeof(WardenHashRequest));
}

void WardenWin::HandleHashResult(ByteBuffer &buff)
{
    // Verify length
    if (buff.wpos() - buff.rpos() < _module.clientKeySeedHash.second)
    {
        TC_LOG_INFO("warden", "%u - Request hash truncated packet", _session->GetAccountId());
        return;
    }

    // Verify key
    if (std::memcmp(buff.contents() + buff.rpos(), _module.clientKeySeedHash.first, _module.clientKeySeedHash.second) != 0)
    {
        TC_LOG_INFO("warden", "%u - Request hash reply failed", _session->GetAccountId());
        return;
    }

    buff.rpos(buff.wpos());

    TC_LOG_DEBUG("warden", "%u - Request hash reply: succeed", _session->GetAccountId());

    // reinit crypto keys
    _inputCrypto.Init(_module.clientKeySeed.first, _module.clientKeySeed.second);
    _outputCrypto.Init(_module.serverKeySeed.first, _module.serverKeySeed.second);

    _initialized = true;
    _previousTimestamp = getMSTime();
}

void WardenWin::RequestData(WardenRequestContext* context)
{
    if (context)
        _responseReceiverGUID = context->ResponseReceiverGUID;
    else if (!_customChecks)
        _responseReceiverGUID.Clear();

    if (context)
        _customChecks = context->CheckCount;

    TC_LOG_DEBUG("warden", "Request data");

    if (context)
    {
        _memChecksTodo.clear();
        if (!context->MemCheckIDs.empty())
            _memChecksTodo.assign(context->MemCheckIDs.begin(), context->MemCheckIDs.end());
        _otherChecksTodo.clear();
        if (!context->OtherCheckIDs.empty())
            _otherChecksTodo.assign(context->OtherCheckIDs.begin(), context->OtherCheckIDs.end());
    }
    else
    {
        if (_memChecksTodo.empty() && _otherChecksTodo.empty())
        {
            _customChecks = 0;
            _responseReceiverGUID.Clear();
        }

        // If all checks were done, fill the todo list again
        if (_memChecksTodo.empty() && !_customChecks)
            _memChecksTodo.assign(sWardenCheckMgr->MemChecksIdPool[_session->GetSessionDbLocaleIndex()].begin(), sWardenCheckMgr->MemChecksIdPool[_session->GetSessionDbLocaleIndex()].end());

        if (_otherChecksTodo.empty() && !_customChecks)
            _otherChecksTodo.assign(sWardenCheckMgr->OtherChecksIdPool[_session->GetSessionDbLocaleIndex()].begin(), sWardenCheckMgr->OtherChecksIdPool[_session->GetSessionDbLocaleIndex()].end());
    }

    _serverTicks = getMSTime();

    uint16 id;
    uint8 type;
    WardenCheckResult* ws;
    WardenCheck* wd;
    _currentChecks.clear();

    uint32 customChecksLeft = _customChecks;

    // Build check request
    if (context && context->CustomMemCheck)
        _currentChecks.push_back(std::make_pair(*context->CustomMemCheck, context->CustomMemResult ? *context->CustomMemResult : WardenCheckResult()));

    for (uint32 i = 0; _customChecks ? customChecksLeft : i < sWorld->getIntConfig(CONFIG_WARDEN_NUM_MEM_CHECKS); ++i)
    {
        // If todo list is done break loop (will be filled on next Update() run)
        if (_memChecksTodo.empty())
            break;

        --customChecksLeft;

        // Get check id from the end and remove it from todo
        id = _memChecksTodo.back();
        _memChecksTodo.pop_back();

        wd = sWardenCheckMgr->GetWardenDataById(id);
        ws = sWardenCheckMgr->GetWardenResultById(id);
 
        if (!wd)
            continue;

        // Add the id to the list sent in this cycle
        _currentChecks.push_back(std::make_pair(*wd, ws ? *ws : WardenCheckResult()));
    }

    ByteBuffer buff;
    buff << uint8(WARDEN_SMSG_CHEAT_CHECKS_REQUEST);

    // always "wow.exe"
    std::string exename = "@exe";
    buff << uint8(exename.size());
    buff.append(exename.c_str(), exename.size());

    if (context && context->CustomOtherCheck)
    {
        _currentChecks.push_back(std::make_pair(*context->CustomOtherCheck, context->CustomOtherResult ? *context->CustomOtherResult : WardenCheckResult()));

        switch (context->CustomOtherCheck->Type)
        {
            case MPQ_CHECK:
            case LUA_STR_CHECK:
            case DRIVER_CHECK:
                buff << uint8(context->CustomOtherCheck->Str.size());
                buff.append(context->CustomOtherCheck->Str.c_str(), context->CustomOtherCheck->Str.size());
                break;
            default:
                break;
        }
    }

    for (uint32 i = 0; _customChecks ? customChecksLeft : i < sWorld->getIntConfig(CONFIG_WARDEN_NUM_OTHER_CHECKS); ++i)
    {
        // If todo list is done break loop (will be filled on next Update() run)
        if (_otherChecksTodo.empty())
            break;

        --customChecksLeft;

        // Get check id from the end and remove it from todo
        id = _otherChecksTodo.back();
        _otherChecksTodo.pop_back();

        wd = sWardenCheckMgr->GetWardenDataById(id);
        ws = sWardenCheckMgr->GetWardenResultById(id);

        if (!wd)
            continue;
 
        // Add the id to the list sent in this cycle
        _currentChecks.push_back(std::make_pair(*wd, ws ? *ws : WardenCheckResult()));

        switch (wd->Type)
        {
            case MPQ_CHECK:
            case LUA_STR_CHECK:
            case DRIVER_CHECK:
                buff << uint8(wd->Str.size());
                buff.append(wd->Str.c_str(), wd->Str.size());
                break;
            default:
                break;
        }
    }

    // string with len 0, finish of string checks
    buff << uint8(0x00);

    uint8 const xorByte = _module.clientKeySeed.first[0];

    // Add TIMING_CHECK
    buff << uint8(TIMING_CHECK ^ xorByte);

    for (auto&& check : _currentChecks)
    {
        wd = &check.first;

        type = wd->Type;
        switch (type)
        {
            case MEM_CHECK:
            {
                buff << uint8(type ^ xorByte);
                buff << uint8(0x01);

                buff << uint8(0x0E);
                for (uint32 i = 0; i < 4; ++i)
                    buff << uint8((wd->Address >> (i * 8)) & 0xFF);

                buff << uint8(wd->Length);
                break;
            }
            case PAGE_CHECK_A:
            case PAGE_CHECK_B:
            {
                buff.append(wd->Data.AsByteArray(0, false), wd->Data.GetNumBytes());
                buff << uint32(wd->Address);
                buff << uint8(wd->Length);
                break;
            }
            /*case MPQ_CHECK:
            case LUA_STR_CHECK:
            {
                buff << uint8(index++);
                break;
            }
            case DRIVER_CHECK:
            {
                buff.append(wd->Data.AsByteArray(0, false), wd->Data.GetNumBytes());
                buff << uint8(index++);
                break;
            }*/
            case MODULE_CHECK:
            {
                std::array<uint8, 4> seed = Trinity::Crypto::GetRandomBytes<4>();
                buff.append(seed);
                buff.append(Trinity::Crypto::HMAC_SHA1::GetDigestOf(seed, wd->Str));
                break;                
            }
            /*case PROC_CHECK:
            {
                buff.append(wd->i.AsByteArray(0, false).get(), wd->i.GetNumBytes());
                buff << uint8(index++);
                buff << uint8(index++);
                buff << uint32(wd->Address);
                buff << uint8(wd->Length);
                break;
            }*/
            default:
                break;                                      // Should never happen
        }
    }

    buff << uint8(CT_END ^ xorByte);

    // Encrypt with warden RC4 key
    EncryptData(buff.contents(), buff.size());

    SendPacket(SMSG_WARDEN_DATA, buff.contents(), buff.size());

    _dataSent = true;

    std::stringstream stream;
    stream << "Sent checks: ";
    for (auto&& check : _currentChecks)
    {
        if (check.first.CheckId)
            stream << check.first.CheckId << " ";
        else
            stream << "custom (type: " << uint32(check.first.Type) << ", data: " << check.first.Data.AsHexStr() << ", str: " << check.first.Str << ", address: 0x" << std::hex << std::uppercase << check.first.Address << std::nouppercase << std::dec << ", length: " << uint32(check.first.Length) << ", result: " << check.second.Result.AsHexStr() << ") ";
    }

    TC_LOG_DEBUG("warden", "%s", stream.str().c_str());

    if (_responseReceiverGUID)
        if (Player* receiver = ObjectAccessor::FindConnectedPlayer(_responseReceiverGUID))
            ChatHandler(receiver->GetSession()).SendSysMessage(stream.str().c_str());
}

void WardenWin::HandleData(ByteBuffer &buff)
{
    TC_LOG_DEBUG("warden", "Handle data");

    Player* receiver = NULL;
    if (_responseReceiverGUID)
        if (receiver = ObjectAccessor::FindConnectedPlayer(_responseReceiverGUID))
            ChatHandler(receiver->GetSession()).PSendSysMessage("Received warden check response from player %s (guid: %u, account: %u, map: %d). Handling...", _session->GetPlayerName().c_str(), _session->GetGuidLow(), _session->GetAccountId(), _session->GetPlayer() ? _session->GetPlayer()->GetMapId() : -1);

    _dataSent = false;
    _clientResponseTimer = 0;

    uint16 length;
    buff >> length;
    uint32 checksum;
    buff >> checksum;

    if (buff.wpos() - buff.rpos() < length)
    {
        TC_LOG_INFO("warden", "%u - Length is too large", _session->GetAccountId());
        buff.rpos(buff.wpos());
        return;
    }


    if (!IsValidCheckSum(checksum, buff.contents() + buff.rpos(), length))
    {
        buff.rpos(buff.wpos());
        std::string penalty = Penalty();
        TC_LOG_WARN("warden", "%s failed checksum. Action: %s, map: %d", _session->GetPlayerInfo().c_str(), Penalty().c_str(), _session->GetPlayer() ? _session->GetPlayer()->GetMapId() : -1); // , penalty.c_str()

        if (receiver)
            ChatHandler(receiver->GetSession()).PSendSysMessage("Failed checksum. Action: %s", penalty.c_str());

        sWorld->SendGMText(LANG_ICORE_GUARD, _session->GetPlayerName().c_str(), _session->GetPlayerName().c_str(), _session->GetGuidLow(), _session->GetAccountId(), _session->GetPlayer() ? _session->GetPlayer()->GetMapId() : -1, 0, "internal", "Failed checksum", penalty.c_str());
        return;
    }

    if (length == 0)
        return;

    // TIMING_CHECK
    {
        uint8 result;
        buff >> result;
        /// @todo test it.
        if (result == 0x00)
        {
            std::string penalty = Penalty();
            TC_LOG_DEBUG("warden", "TIMING CHECK FAIL result 0x00");
            TC_LOG_WARN("warden", "player %s (guid: %u, account: %u, map: %d) failed timing check. Action: %s",
                _session->GetPlayerInfo().c_str(), _session->GetGuidLow(), _session->GetAccountId(), _session->GetPlayer() ? _session->GetPlayer()->GetMapId() : -1, Penalty().c_str());

            if (receiver)
                ChatHandler(receiver->GetSession()).PSendSysMessage("Failed timing check. Action: %s", penalty.c_str());

            //sWorld->SendGMText(LANG_ICORE_GUARD, _session->GetPlayerName().c_str(), _session->GetPlayerName().c_str(), _session->GetGuidLow(), _session->GetAccountId(), _session->GetPlayer() ? _session->GetPlayer()->GetMapId() : -1, 0, "internal", "Failed timing check", penalty.c_str());
            //return;
        }

        uint32 newClientTicks;
        buff >> newClientTicks;

        uint32 ticksNow = getMSTime();
        uint32 ourTicks = newClientTicks + (ticksNow - _serverTicks);

        TC_LOG_DEBUG("warden", "ServerTicks %u", ticksNow);         // Now
        TC_LOG_DEBUG("warden", "RequestTicks %u", _serverTicks);    // At request
        TC_LOG_DEBUG("warden", "Ticks %u", newClientTicks);         // At response
        TC_LOG_DEBUG("warden", "Ticks diff %u", ourTicks - newClientTicks);
    }

    WardenCheckResult* rs;
    WardenCheck *rd;
    uint8 type;
    uint16 checkFailed = 0;
    std::vector<std::tuple<WardenCheck*, bool, std::string>> checksFailed;

    for (auto&& check : _currentChecks)
    {
        rd = &check.first;
        rs = &check.second;

        if (!rd)
            return;

        type = rd->Type;
        switch (type)
        {
            case MEM_CHECK:
            {
                uint8 Mem_Result;
                buff >> Mem_Result;

                if (Mem_Result != 0)
                {
                    TC_LOG_DEBUG("warden", "RESULT MEM_CHECK not 0x00, CheckId %u account Id %u", rd->CheckId, _session->GetAccountId());
                    //checkFailed = rd->CheckId;
                    checksFailed.push_back(std::make_tuple(rd, false, ""));
                    _state = State::Normal;     // If failed on endscene request.
                    continue;
                }

                if (memcmp(buff.contents() + buff.rpos(), rs->Result.AsByteArray(0, false), rd->Length) != 0)
                {
                    TC_LOG_DEBUG("warden", "RESULT MEM_CHECK fail CheckId %u account Id %u", rd->CheckId, _session->GetAccountId());
                    checkFailed = rd->CheckId;
                    checksFailed.push_back(std::make_tuple(rd, true, Trinity::Impl::ByteArrayToHexStr(buff.contents() + buff.rpos(), rd->Length)));
                    buff.rpos(buff.rpos() + rd->Length);
                    continue;
                }

                buff.rpos(buff.rpos() + rd->Length);
                TC_LOG_DEBUG("warden", "RESULT MEM_CHECK passed CheckId %u account Id %u", rd->CheckId, _session->GetAccountId());
                break;
            }
            case PAGE_CHECK_A:
            case PAGE_CHECK_B:
            case DRIVER_CHECK:
            {
                const uint8 byte = 0xE9;
                if ((memcmp(buff.contents() + buff.rpos(), &byte, sizeof(uint8)) != 0) ^ rd->Negative)
                {
                    if (type == PAGE_CHECK_A || type == PAGE_CHECK_B)
                        TC_LOG_DEBUG("warden", "RESULT PAGE_CHECK fail, CheckId %u account Id %u", rd->CheckId, _session->GetAccountId());
                    if (type == DRIVER_CHECK)
                        TC_LOG_DEBUG("warden", "RESULT DRIVER_CHECK fail, CheckId %u account Id %u", rd->CheckId, _session->GetAccountId());
                    checkFailed = rd->CheckId;
                    checksFailed.push_back(std::make_tuple(rd, true, Trinity::Impl::ByteArrayToHexStr(buff.contents() + buff.rpos(), sizeof(uint8))));
                    buff.rpos(buff.rpos() + 1);
                    continue;
                }

                buff.rpos(buff.rpos() + 1);
                if (type == PAGE_CHECK_A || type == PAGE_CHECK_B)
                    TC_LOG_DEBUG("warden", "RESULT PAGE_CHECK passed CheckId %u account Id %u", rd->CheckId, _session->GetAccountId());
                else if (type == DRIVER_CHECK)
                    TC_LOG_DEBUG("warden", "RESULT DRIVER_CHECK passed CheckId %u account Id %u", rd->CheckId, _session->GetAccountId());
                break;
            }
            case MODULE_CHECK:
            {
                uint8 const modulePresentMarker = 0x4A;
                if ((buff.read<uint8>() == modulePresentMarker) ^ rd->Negative)
                {
                    TC_LOG_INFO("warden", "%u - RESULT MODULE_CHECK fail, CheckId %u", _session->GetAccountId(), rd->CheckId);
                    checkFailed = rd->CheckId;
                    checksFailed.push_back(std::make_tuple(rd, true, Trinity::Impl::ByteArrayToHexStr(buff.contents() + buff.rpos(), sizeof(uint8))));
                    continue;
                }

                TC_LOG_DEBUG("warden", "%u - RESULT MODULE_CHECK passed CheckId %u", _session->GetAccountId(), rd->CheckId);
                break;
            }
            case LUA_STR_CHECK:
            {
                uint8 Lua_Result;
                buff >> Lua_Result;

                if ((Lua_Result != 0) ^ rd->Negative)
                {
                    TC_LOG_DEBUG("warden", "RESULT LUA_STR_CHECK fail, CheckId %u account Id %u", rd->CheckId, _session->GetAccountId());
                    //checkFailed = rd->CheckId;
                    checksFailed.push_back(std::make_tuple(rd, false, ""));
                    continue;
                }

                uint8 luaStrLen;
                buff >> luaStrLen;

                if (luaStrLen != 0)
                {
                    char *str = new char[luaStrLen + 1];
                    memcpy(str, buff.contents() + buff.rpos(), luaStrLen);
                    str[luaStrLen] = '\0'; // null terminator
                    TC_LOG_DEBUG("warden", "Lua string: %s", str);
                    checkFailed = rd->CheckId;     // _sessiion->Kick even solo kick for addOns ; checkFailed = global sanctions.
                    checksFailed.push_back(std::make_tuple(rd, true, ""));
                    delete [] str;
                    buff.rpos(buff.rpos() + luaStrLen);
                }
                buff.rpos(buff.rpos() + luaStrLen);         // Skip string
                TC_LOG_DEBUG("warden", "RESULT LUA_STR_CHECK passed, CheckId %u account Id %u", rd->CheckId, _session->GetAccountId());
                break;
            }
            case MPQ_CHECK:
            {
                uint8 Mpq_Result;
                buff >> Mpq_Result;

                if (Mpq_Result != 0)
                {
                    TC_LOG_DEBUG("warden", "RESULT MPQ_CHECK not 0x00 account id %u", _session->GetAccountId());
                    //checkFailed = rd->CheckId;
                    checksFailed.push_back(std::make_tuple(rd, false, ""));
                    continue;
                }

                if (memcmp(buff.contents() + buff.rpos(), rs->Result.AsByteArray(0, false), 20) != 0) // SHA1
                {
                    TC_LOG_DEBUG("warden", "RESULT MPQ_CHECK fail, CheckId %u account Id %u", rd->CheckId, _session->GetAccountId());
                    checkFailed = rd->CheckId; // _session->kick for solo kicker Model edits maps.
                    checksFailed.push_back(std::make_tuple(rd, true, ""));
                    buff.rpos(buff.rpos() + 20);            // 20 bytes SHA1
                    continue;
                }

                buff.rpos(buff.rpos() + 20);                // 20 bytes SHA1
                TC_LOG_DEBUG("warden", "RESULT MPQ_CHECK passed, CheckId %u account Id %u", rd->CheckId, _session->GetAccountId());
                break;
            }
            default:                                        // Should never happen
                break;
        }
    }

    if (checksFailed.empty())
    {
        if (receiver)
            ChatHandler(receiver->GetSession()).PSendSysMessage("All checks passed successfully");
    }
    else
    {
        if (receiver)
            ChatHandler(receiver->GetSession()).PSendSysMessage("Failed warden checks:");

        // First - find failed check with highest penalty
        WardenActions action = WardenActions(0);
        for (auto&& check : checksFailed)
        {
            WardenCheck* checkData = std::get<0>(check);
            bool allowPenalty = std::get<1>(check);
            std::string clientResult = std::get<2>(check);
            uint16 id = checkData->CheckId;

            if (allowPenalty && checkData->Action > action)
                checkFailed = id;
        }

        // Then - record all failed checks and report about all except for the one with highest penalty - report about it at the end
        for (auto&& check : checksFailed)
        {
            WardenCheck* checkData      = std::get<0>(check);
            bool allowPenalty           = std::get<1>(check);
            std::string clientResult    = std::get<2>(check);
            uint16 id = checkData->CheckId;

            if (!checkData->Group.empty())
                RecordFailedCheckGroup(checkData->Group, checkData->Comment);

            if (receiver)
            {
                char const* penaltyString;
                switch (checkData->Action)
                {
                    case WARDEN_ACTION_LOG:         penaltyString = "log";      break;
                    case WARDEN_ACTION_LOG_GM:      penaltyString = "log+gm";   break;
                    //case WARDEN_ACTION_ITEMLOG:     penaltyString = ".itemlog"; break;
                    case WARDEN_ACTION_KICK:        penaltyString = "kick";     break;
                    case WARDEN_ACTION_BAN:         penaltyString = "ban";      break;
                    default:                        penaltyString = "?";        break;
                }
                if (id)
                    ChatHandler(receiver->GetSession()).PSendSysMessage("%u = %s - [%s] %s (%s)%s", uint32(id), clientResult.c_str(), checkData->Group.c_str(), checkData->Comment.c_str(), penaltyString, allowPenalty ? "" : " (skipped)");
                else
                    ChatHandler(receiver->GetSession()).PSendSysMessage("Custom check = %s", clientResult.c_str());
            }

            // Only report about checks the penalty won't be given for
            if (id != checkFailed)
            {
                TC_LOG_DEBUG("warden", "Player %s (guid: %u, account: %u, map: %d) failed Guard check %u. Action: %s",
                    _session->GetPlayerInfo().c_str(), _session->GetGuidLow(), _session->GetAccountId(), _session->GetPlayer() ? _session->GetPlayer()->GetMapId() : -1, id, "Override");

                if (checkData->Action >= WARDEN_ACTION_LOG_GM)
                    sWorld->SendGMText(LANG_ICORE_GUARD, _session->GetPlayerName().c_str(), _session->GetPlayerName().c_str(), _session->GetGuidLow(), _session->GetAccountId(), _session->GetPlayer() ? _session->GetPlayer()->GetMapId() : -1, checkFailed, checkData->Group.c_str(), checkData->Comment.c_str(), "Override");
            }
        }
    }

    // Now report about the check the penalty is given for
    if (WardenCheck* check = checkFailed ? sWardenCheckMgr->GetWardenDataById(checkFailed) : nullptr)
    {       
        std::string penalty = Penalty(check);
        TC_LOG_WARN("warden", "I-Core GUARD: Player %s (guid: %u, account: %u, map: %d) failed Guard check %u. Action: %s",
            _session->GetPlayerInfo().c_str(), _session->GetGuidLow(), _session->GetAccountId(), _session->GetPlayer() ? _session->GetPlayer()->GetMapId() : -1, checkFailed, penalty.c_str());

        if (receiver)
            ChatHandler(receiver->GetSession()).PSendSysMessage("Given penalty for failed warden check %u. Action: %s", uint32(checkFailed), penalty.c_str());

        if (check->Action >= WARDEN_ACTION_LOG_GM)
            sWorld->SendGMText(LANG_ICORE_GUARD, _session->GetPlayerName().c_str(), _session->GetPlayerName().c_str(), _session->GetGuidLow(), _session->GetAccountId(), _session->GetPlayer() ? _session->GetPlayer()->GetMapId() : -1, checkFailed, check->Group.c_str(), check->Comment.c_str(), penalty.c_str());
    }

    if (_state != State::Normal)
    {
        ContinueCheck();
        return;
    }

    // Set hold off timer, minimum timer should at least be 1 second
    uint32 holdOff = sWorld->getIntConfig(CONFIG_WARDEN_CLIENT_CHECK_HOLDOFF);
    _checkTimer = (holdOff < 1 ? 1 : holdOff) * IN_MILLISECONDS;

    if (!_enabled)
        _checkTimer = 0;

    if (_customChecks && !(_memChecksTodo.empty() && _otherChecksTodo.empty()))
        _checkTimer = 1 * IN_MILLISECONDS;
}

void WardenWin::Disable()
{
    Warden::Disable();

    _memChecksTodo.clear();
    _otherChecksTodo.clear();
}

bool WardenWin::GetEndSceneAddress(int32& addr)
{
    addr = _endSceneAddress;
    return _endSceneCaptured;
}

void WardenWin::DoCustomMemCheck(uint32 addr, uint32 len, ObjectGuid guid)
{
    WardenRequestContext ctx;
    WardenCheck check;
    WardenCheckResult result;
    check.CheckId = 0;
    check.Type = MEM_CHECK;
    check.Action = WARDEN_ACTION_LOG;
    check.Address = addr;
    check.Length = len;
    check.Locale = LocaleConstant(TOTAL_LOCALES);
    check.Negative = false;
    ctx.ResponseReceiverGUID = guid;
    ctx.CustomMemCheck = &check;
    ctx.CustomMemResult = &result;
    RequestData(&ctx);
}

void WardenWin::HandleEndSceneResult(ByteBuffer& buff, WardenCheck* check)
{
    auto begin = buff.rpos();
    auto end = begin + check->Length;

    switch (_state)
    {
        case State::EndSceneRequestStep1:
            _endSceneAddress = buff.read<int32>();
            _endSceneAddress += 0x397C;
            break;
        case State::EndSceneRequestStep2:
            _endSceneAddress = buff.read<int32>();
            break;
        case State::EndSceneRequestStep3:
            _endSceneAddress = buff.read<int32>();
            _endSceneAddress += 0xA8;
            break;
        case State::EndSceneRequestStep4:
            _endSceneAddress = buff.read<int32>();
            _endSceneCaptured = true;
            _state = State::Normal;
            break;
        default:
            std::abort();
    }
    buff.rpos(end);
}

void WardenWin::ContinueCheck()
{
    switch (_state)
    {
        case State::EndSceneRequestStep1:
            DoCustomMemCheck(_endSceneAddress, 4);
            _state = State::EndSceneRequestStep2;
            break;
        case State::EndSceneRequestStep2:
            DoCustomMemCheck(_endSceneAddress, 4);
            _state = State::EndSceneRequestStep3;
            break;
        case State::EndSceneRequestStep3:
            DoCustomMemCheck(_endSceneAddress, 4);
            _state = State::EndSceneRequestStep4;
            break;
        case State::EndSceneRequestStep4:
            break;
        default:
            std::abort();
    }
}
