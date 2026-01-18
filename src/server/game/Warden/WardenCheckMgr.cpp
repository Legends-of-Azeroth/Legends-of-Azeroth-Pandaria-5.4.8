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
#include "WorldSession.h"
#include "Log.h"
#include "DatabaseEnv.h"
#include "Util.h"
#include "WardenCheckMgr.h"
#include "Warden.h"

WardenCheckMgr::WardenCheckMgr() { }

WardenCheckMgr::~WardenCheckMgr()
{
    for (uint16 i = 0; i < CheckStore.size(); ++i)
        delete CheckStore[i];

    for (CheckResultContainer::iterator itr = CheckResultStore.begin(); itr != CheckResultStore.end(); ++itr)
        delete itr->second;
}

WardenCheckMgr* WardenCheckMgr::instance()
{
    static WardenCheckMgr instance;
    return &instance;
}

void WardenCheckMgr::LoadWardenChecks(bool reload)
{
    // Check if Warden is enabled by config before loading anything
    if (!sWorld->getBoolConfig(CONFIG_WARDEN_ENABLED))
    {
        TC_LOG_INFO("warden", ">> Warden disabled, loading checks skipped.");
        return;
    }

    if (reload)
    {
        for (auto&& check : CheckStore)
            delete check;
        for (auto&& result : CheckResultStore)
            delete result.second;

        CheckStore.clear();
        CheckResultStore.clear();
        for (auto&& locale : MemChecksIdPool)
            locale.clear();
        for (auto&& locale : OtherChecksIdPool)
            locale.clear();
    }

    QueryResult result = WorldDatabase.Query("SELECT MAX(id) FROM warden_checks");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 Warden checks. DB table `warden_checks` is empty!");
        return;
    }

    Field* fields = result->Fetch();

    uint16 maxCheckId = fields[0].GetUInt16();

    CheckStore.resize(maxCheckId + 1);

    //                                    0    1     2     3        4       5      6      7       8       9         10        11
    result = WorldDatabase.Query("SELECT id, type, data, result, address, length, str, comment, action, `group`, locale+0, negative FROM warden_checks ORDER BY id ASC");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 warden checks.");
        return;
    }

    uint32 count = 0;
    do
    {
        fields = result->Fetch();

        uint16 id               = fields[0].GetUInt16();
        uint8 checkType         = fields[1].GetUInt8();
        std::string data        = fields[2].GetString();
        std::string checkResult = fields[3].GetString();
        uint32 address          = fields[4].GetUInt32();
        uint8 length            = fields[5].GetUInt8();
        std::string str         = fields[6].GetString();
        std::string comment     = fields[7].GetString();
        uint8 action            = fields[8].IsNull() ? sWorld->getIntConfig(CONFIG_WARDEN_CLIENT_FAIL_ACTION) : fields[8].GetUInt8();
        std::string group       = fields[9].GetString();
        uint32 locale           = fields[10].GetUInt32();
        uint8 negative          = fields[11].GetUInt8();

        // Check if action value is in range (0-3, see WardenActions enum)
        if (action >= MAX_WARDEN_ACTION)
        {
            TC_LOG_INFO("server.loading", "Warden check action out of range (ID: %u, action: %u)", id, uint32(action));
            continue;
        }

        WardenCheck* wardenCheck = new WardenCheck();
        wardenCheck->Type = checkType;
        wardenCheck->CheckId = id;
        wardenCheck->Group = group;
        wardenCheck->Locale = locale ? LocaleConstant(locale - 1) : LocaleConstant(TOTAL_LOCALES);
        wardenCheck->Negative = negative;

        // Initialize action with default action from config
        wardenCheck->Action = WardenActions(action);

        if (checkType == PAGE_CHECK_A || checkType == PAGE_CHECK_B || checkType == DRIVER_CHECK)
        {
            wardenCheck->Data.SetHexStr(data.c_str());
            int len = data.size() / 2;

            if (wardenCheck->Data.GetNumBytes() < len)
            {
                uint8 temp[24];
                memset(temp, 0, len);
                memcpy(temp, wardenCheck->Data.AsByteArray(), wardenCheck->Data.GetNumBytes());
                std::reverse(temp, temp + len);
                wardenCheck->Data.SetBinary((uint8*)temp, len);
            }
        }

        if (checkType == MEM_CHECK || checkType == MODULE_CHECK)
        {
            if (wardenCheck->Locale != TOTAL_LOCALES)
                MemChecksIdPool[wardenCheck->Locale].push_back(id);
            else
                for (auto&& locale : MemChecksIdPool)
                    locale.push_back(id);
        }
        else
        {
            if (wardenCheck->Locale != TOTAL_LOCALES)
                OtherChecksIdPool[wardenCheck->Locale].push_back(id);
            else
                for (auto&& locale : OtherChecksIdPool)
                    locale.push_back(id);
        }

        if (checkType == MEM_CHECK || checkType == PAGE_CHECK_A || checkType == PAGE_CHECK_B || checkType == PROC_CHECK)
        {
            wardenCheck->Address = address;
            wardenCheck->Length = length;
        }

        // PROC_CHECK support missing
        if (checkType == MEM_CHECK || checkType == MPQ_CHECK || checkType == LUA_STR_CHECK || checkType == DRIVER_CHECK || checkType == MODULE_CHECK)
            wardenCheck->Str = str;

        CheckStore[id] = wardenCheck;

        if (checkType == MPQ_CHECK || checkType == MEM_CHECK)
        {
            WardenCheckResult* wr = new WardenCheckResult();
            wr->Result.SetHexStr(checkResult.c_str());
            int len = checkResult.size() / 2;
            if (wr->Result.GetNumBytes() < len)
            {
                uint8 *temp = new uint8[len];
                memset(temp, 0, len);
                memcpy(temp, wr->Result.AsByteArray(), wr->Result.GetNumBytes());
                std::reverse(temp, temp + len);
                wr->Result.SetBinary((uint8*)temp, len);
                delete [] temp;
            }
            CheckResultStore[id] = wr;
        }

        if (comment.empty())
            wardenCheck->Comment = "Undocumented Check";
        else
            wardenCheck->Comment = comment;

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u warden checks.", count);
}

WardenCheck* WardenCheckMgr::GetWardenDataById(uint16 Id)
{
    if (Id < CheckStore.size())
        return CheckStore[Id];

    return NULL;
}

WardenCheckResult* WardenCheckMgr::GetWardenResultById(uint16 Id)
{
    CheckResultContainer::const_iterator itr = CheckResultStore.find(Id);
    if (itr != CheckResultStore.end())
        return itr->second;
    return NULL;
}
