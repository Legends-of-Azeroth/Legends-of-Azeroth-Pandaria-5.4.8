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

#ifndef SF_ACCMGR_H
#define SF_ACCMGR_H

#include "Define.h"
#include <string>
#include "Util.h"

enum class AccountOpResult : uint8
{
    AOR_OK,
    AOR_NAME_TOO_LONG,
    AOR_PASS_TOO_LONG,
    AOR_EMAIL_TOO_LONG,
    AOR_NAME_ALREADY_EXIST,
    AOR_NAME_NOT_EXIST,
    AOR_DB_INTERNAL_ERROR,
    AOR_ACCOUNT_BAD_LINK,
};

enum PasswordChangeSecurity
{
    PW_NONE,
    PW_EMAIL,
    PW_RBAC
};

#define MAX_PASS_STR 16
#define MAX_ACCOUNT_STR 16
#define MAX_EMAIL_STR 64

class AccountMgr
{
    private:
        AccountMgr() = default;
        ~AccountMgr() = default;

    public:
        static AccountMgr* instance();

        AccountOpResult CreateAccount(std::string username, std::string password, std::string email = "", uint32 bnetAccountId = 0, uint8 bnetIndex = 0);
        static AccountOpResult DeleteAccount(uint32 accountId);
        static AccountOpResult ChangeUsername(uint32 accountId, std::string newUsername, std::string newPassword);
        static AccountOpResult ChangePassword(uint32 accountId, std::string newPassword);
        static AccountOpResult ChangeEmail(uint32 accountId, std::string newEmail);
        static bool CheckPassword(uint32 accountId, std::string password);
        static bool CheckEmail(uint32 accountId, std::string newEmail);

        static uint32 GetId(std::string const& username);
        static uint32 GetSecurity(uint32 accountId);
        static uint32 GetSecurity(uint32 accountId, int32 realmId);
        static bool GetName(uint32 accountId, std::string& name);
        static bool GetEmail(uint32 accountId, std::string& email);
        static uint32 GetCharactersCount(uint32 accountId);

        static std::string CalculateShaPassHash(std::string const& name, std::string const& password);
        static bool normalizeString(std::string& utf8String);
        static bool IsPlayerAccount(uint32 gmlevel);
        static bool IsAdminAccount(uint32 gmlevel);
        static bool IsConsoleAccount(uint32 gmlevel);

        void UpdateAccountAccess(uint32 accountId, uint8 securityLevel, int32 realmId);
};

#define sAccountMgr AccountMgr::instance()

#define MAX_BNET_EMAIL_STR 320

namespace Battlenet
{
    static bool StringToBool(std::string const& str)
    {
        std::string lowerStr = str;
        std::transform(str.begin(), str.end(), lowerStr.begin(), [](char c) { return char(::tolower(c)); });
        return lowerStr == "1" || lowerStr == "true" || lowerStr == "yes";
    }

    static bool Utf8ToUpperOnlyLatin(std::string& utf8String)
    {
        std::wstring wstr;
        if (!Utf8toWStr(utf8String, wstr))
            return false;

        std::transform(wstr.begin(), wstr.end(), wstr.begin(), wcharToUpperOnlyLatin);
        return WStrToUtf8(wstr, utf8String);
    }

namespace AccountMgr
    {
        AccountOpResult CreateBattlenetAccount(std::string email, std::string password, bool withGameAccount = true);
        AccountOpResult ChangePassword(uint32 accountId, std::string newPassword);
        AccountOpResult LinkWithGameAccount(std::string const& email, std::string const& gameAccountName);
        AccountOpResult UnlinkGameAccount(std::string const& gameAccountName);

        uint32 GetId(std::string const& username);
        bool GetName(uint32 accountId, std::string& name);
        uint8 GetMaxIndex(uint32 accountId);

        std::string CalculateShaPassHash(std::string const& name, std::string const& password);
    }
}

#endif
