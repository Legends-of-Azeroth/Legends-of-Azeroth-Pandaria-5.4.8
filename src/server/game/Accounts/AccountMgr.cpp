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

#include "AccountMgr.h"
#include "Config.h"
#include "DatabaseEnv.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Util.h"
#include "WorldSession.h"
#include "CryptoHash.h"

AccountMgr* AccountMgr::instance()
{
    static AccountMgr instance;
    return &instance;
}

AccountOpResult AccountMgr::CreateAccount(std::string username, std::string password, std::string email /*= ""*/, uint32 /*bnetAccountId*/ /*= 0*/, uint8 /*bnetIndex*/ /*= 0*/)
{
    if (utf8length(username) > MAX_ACCOUNT_STR)
        return AccountOpResult::AOR_NAME_TOO_LONG;                           // username's too long

    normalizeString(username);
    normalizeString(password);
    normalizeString(email);

    if (GetId(username))
        return AccountOpResult::AOR_NAME_ALREADY_EXIST;                       // username does already exist

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_ACCOUNT);

    stmt->setString(0, username);
    stmt->setString(1, CalculateShaPassHash(username, password));
    stmt->setString(2, email);

    LoginDatabase.DirectExecute(stmt); // Enforce saving, otherwise AddGroup can fail

    stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_REALM_CHARACTERS_INIT);
    LoginDatabase.Execute(stmt);

    return AccountOpResult::AOR_OK;                                          // everything's fine
}

AccountOpResult AccountMgr::DeleteAccount(uint32 accountId)
{
    // Check if accounts exists
    LoginDatabasePreparedStatement* loginStmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_BY_ID);
    loginStmt->setUInt32(0, accountId);
    PreparedQueryResult result = LoginDatabase.Query(loginStmt);

    if (!result)
        return AccountOpResult::AOR_NAME_NOT_EXIST;

    // Obtain accounts characters
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARS_BY_ACCOUNT_ID);

    stmt->setUInt32(0, accountId);

    result = CharacterDatabase.Query(stmt);

    if (result)
    {
        do
        {
            ObjectGuid guid(HighGuid::Player, (*result)[0].GetUInt32());

            // Kick if player is online
            if (Player* p = ObjectAccessor::FindConnectedPlayer(guid))
            {
                WorldSession* s = p->GetSession();
                s->KickPlayer();                            // mark session to remove at next session list update
                s->LogoutPlayer(false);                     // logout player without waiting next session list update
            }

            Player::DeleteFromDB(guid, accountId, false);       // no need to update realm characters
        } while (result->NextRow());
    }

    // table realm specific but common for all characters of account for realm
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_TUTORIALS);
    stmt->setUInt32(0, accountId);
    CharacterDatabase.Execute(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ACCOUNT_DATA);
    stmt->setUInt32(0, accountId);
    CharacterDatabase.Execute(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_BAN);
    stmt->setUInt32(0, accountId);
    CharacterDatabase.Execute(stmt);

    LoginDatabaseTransaction trans = LoginDatabase.BeginTransaction();

    loginStmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_ACCOUNT);
    loginStmt->setUInt32(0, accountId);
    trans->Append(loginStmt);

    loginStmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_ACCOUNT_ACCESS);
    loginStmt->setUInt32(0, accountId);
    trans->Append(loginStmt);

    loginStmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_REALM_CHARACTERS);
    loginStmt->setUInt32(0, accountId);
    trans->Append(loginStmt);

    loginStmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_ACCOUNT_BANNED);
    loginStmt->setUInt32(0, accountId);
    trans->Append(loginStmt);

    LoginDatabase.CommitTransaction(trans);

    return AccountOpResult::AOR_OK;
}

AccountOpResult AccountMgr::ChangePassword(uint32 accountId, std::string newPassword)
{
    std::string username;

    if (!GetName(accountId, username))
        return AccountOpResult::AOR_NAME_NOT_EXIST;                          // account doesn't exist

    if (utf8length(newPassword) > MAX_ACCOUNT_STR)
        return AccountOpResult::AOR_PASS_TOO_LONG;

    normalizeString(username);
    normalizeString(newPassword);

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_PASSWORD);

    stmt->setString(0, CalculateShaPassHash(username, newPassword));
    stmt->setUInt32(1, accountId);

    LoginDatabase.Execute(stmt);

    stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_VS);

    stmt->setString(0, "");
    stmt->setString(1, "");
    stmt->setString(2, username);

    LoginDatabase.Execute(stmt);

    return AccountOpResult::AOR_OK;
}

AccountOpResult AccountMgr::ChangeEmail(uint32 accountId, std::string newEmail)
{
    std::string username;

    if (!GetName(accountId, username))
        return AccountOpResult::AOR_NAME_NOT_EXIST;                          // account doesn't exist

    if (utf8length(newEmail) > MAX_EMAIL_STR)
        return AccountOpResult::AOR_EMAIL_TOO_LONG;

    normalizeString(username);
    normalizeString(newEmail);

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_EMAIL);

    stmt->setString(0, newEmail);
    stmt->setUInt32(1, accountId);

    LoginDatabase.Execute(stmt);

    return AccountOpResult::AOR_OK;
}

uint32 AccountMgr::GetId(std::string const& username)
{
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_GET_ACCOUNT_ID_BY_USERNAME);
    stmt->setString(0, username);
    PreparedQueryResult result = LoginDatabase.Query(stmt);

    return (result) ? (*result)[0].GetUInt32() : 0;
}

uint32 AccountMgr::GetSecurity(uint32 accountId)
{
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_GET_ACCOUNT_ACCESS_GMLEVEL);
    stmt->setUInt32(0, accountId);
    PreparedQueryResult result = LoginDatabase.Query(stmt);

    return (result) ? (*result)[0].GetUInt8() : uint32(SEC_PLAYER);
}

uint32 AccountMgr::GetSecurity(uint32 accountId, int32 realmId)
{
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_GET_GMLEVEL_BY_REALMID);
    stmt->setUInt32(0, accountId);
    stmt->setInt32(1, realmId);
    PreparedQueryResult result = LoginDatabase.Query(stmt);

    return (result) ? (*result)[0].GetUInt8() : uint32(SEC_PLAYER);
}

bool AccountMgr::GetName(uint32 accountId, std::string& name)
{
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_GET_USERNAME_BY_ID);
    stmt->setUInt32(0, accountId);
    PreparedQueryResult result = LoginDatabase.Query(stmt);

    if (result)
    {
        name = (*result)[0].GetString();
        return true;
    }

    return false;
}

bool AccountMgr::GetEmail(uint32 accountId, std::string& email)
{
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_GET_EMAIL_BY_ID);
    stmt->setUInt32(0, accountId);
    PreparedQueryResult result = LoginDatabase.Query(stmt);

    if (result)
    {
        email = (*result)[0].GetString();
        return true;
    }

    return false;
}

bool AccountMgr::CheckPassword(uint32 accountId, std::string password)
{
    std::string username;

    if (!GetName(accountId, username))
        return false;

    normalizeString(username);
    normalizeString(password);

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_CHECK_PASSWORD);
    stmt->setUInt32(0, accountId);
    stmt->setString(1, CalculateShaPassHash(username, password));
    PreparedQueryResult result = LoginDatabase.Query(stmt);

    return (result) ? true : false;
}

bool AccountMgr::CheckEmail(uint32 accountId, std::string newEmail)
{
    std::string oldEmail;

    // We simply return false for a non-existing email
    if (!GetEmail(accountId, oldEmail))
        return false;

    normalizeString(oldEmail);
    normalizeString(newEmail);

    if (strcmp(oldEmail.c_str(), newEmail.c_str()) == 0)
        return true;

    return false;
}

uint32 AccountMgr::GetCharactersCount(uint32 accountId)
{
    // check character count
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_SUM_CHARS);
    stmt->setUInt32(0, accountId);
    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    return (result) ? (*result)[0].GetUInt64() : 0;
}

bool AccountMgr::normalizeString(std::string& utf8String)
{
    wchar_t buffer[MAX_ACCOUNT_STR+1];

    size_t maxLength = MAX_ACCOUNT_STR;
    if (!Utf8toWStr(utf8String, buffer, maxLength))
        return false;
#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif
    std::transform(&buffer[0], buffer+maxLength, &buffer[0], wcharToUpperOnlyLatin);
#ifdef _MSC_VER
#pragma warning(default: 4996)
#endif

    return WStrToUtf8(buffer, maxLength, utf8String);
}

std::string AccountMgr::CalculateShaPassHash(std::string const& name, std::string const& password)
{
    Trinity::Crypto::SHA1 sha;
    sha.UpdateData(name);
    sha.UpdateData(":");
    sha.UpdateData(password);
    sha.Finalize();

    return ByteArrayToHexStr(sha.GetDigest());
}

bool AccountMgr::IsPlayerAccount(uint32 gmlevel)
{
    return gmlevel == SEC_PLAYER;
}

bool AccountMgr::IsAdminAccount(uint32 gmlevel)
{
    return gmlevel >= SEC_ADMINISTRATOR && gmlevel <= SEC_CONSOLE;
}

bool AccountMgr::IsConsoleAccount(uint32 gmlevel)
{
    return gmlevel == SEC_CONSOLE;
}

void AccountMgr::UpdateAccountAccess(uint32 accountId, uint8 securityLevel, int32 realmId)
{
    // Delete old security level from DB
    if (realmId == -1)
    {
        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_ACCOUNT_ACCESS);
        stmt->setUInt32(0, accountId);
        LoginDatabase.Execute(stmt);
    }
    else
    {
        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_ACCOUNT_ACCESS_BY_REALM);
        stmt->setUInt32(0, accountId);
        stmt->setUInt32(1, realmId);
        LoginDatabase.Execute(stmt);
    }

    // Add new security level
    if (securityLevel)
    {
        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_ACCOUNT_ACCESS);
        stmt->setUInt32(0, accountId);
        stmt->setUInt8(1, securityLevel);
        stmt->setInt32(2, realmId);
        LoginDatabase.Execute(stmt);
    }
}

using GameAccountMgr = AccountMgr;

AccountOpResult Battlenet::AccountMgr::CreateBattlenetAccount(std::string email, std::string password, bool withGameAccount /*= true*/)
{
    if (utf8length(email) > MAX_BNET_EMAIL_STR)
        return AccountOpResult::AOR_NAME_TOO_LONG;

    if (utf8length(password) > MAX_PASS_STR)
        return AccountOpResult::AOR_PASS_TOO_LONG;

    Utf8ToUpperOnlyLatin(email);
    Utf8ToUpperOnlyLatin(password);

    if (GetId(email))
        return AccountOpResult::AOR_NAME_ALREADY_EXIST;

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_BNET_ACCOUNT);
    stmt->setString(0, email);
    stmt->setString(1, CalculateShaPassHash(email, password));
    LoginDatabase.DirectExecute(stmt);

    uint32 newAccountId = GetId(email);
    ASSERT(newAccountId);

    if (withGameAccount)
        GameAccountMgr::instance()->CreateAccount(std::to_string(newAccountId) + "#1", password, email, newAccountId, 1);

    return AccountOpResult::AOR_OK;
}

AccountOpResult Battlenet::AccountMgr::ChangePassword(uint32 accountId, std::string newPassword)
{
    std::string username;
    if (!GetName(accountId, username))
        return AccountOpResult::AOR_NAME_NOT_EXIST;                          // account doesn't exist

    Utf8ToUpperOnlyLatin(username);
    Utf8ToUpperOnlyLatin(newPassword);
    if (utf8length(newPassword) > MAX_PASS_STR)
        return AccountOpResult::AOR_PASS_TOO_LONG;

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_BNET_PASSWORD);
    stmt->setString(0, CalculateShaPassHash(username, newPassword));
    stmt->setUInt32(1, accountId);
    LoginDatabase.Execute(stmt);

    return AccountOpResult::AOR_OK;
}

AccountOpResult Battlenet::AccountMgr::LinkWithGameAccount(std::string const& /*email*/, std::string const& /*gameAccountName*/)
{
    /*uint32 bnetAccountId = GetId(email);
    if (!bnetAccountId)
    return AccountOpResult::AOR_NAME_NOT_EXIST;

    uint32 gameAccountId = GameAccountMgr::GetId(gameAccountName);
    if (!gameAccountId)
    return AccountOpResult::AOR_NAME_NOT_EXIST;

    if (GetIdByGameAccount(gameAccountId))
    return AccountOpResult::AOR_ACCOUNT_BAD_LINK;

    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_BNET_GAME_ACCOUNT_LINK);
    stmt->setUInt32(0, bnetAccountId);
    stmt->setUInt8(1, GetMaxIndex(bnetAccountId) + 1);
    stmt->setUInt32(2, gameAccountId);
    LoginDatabase.Execute(stmt);*/
    return AccountOpResult::AOR_OK;
}

AccountOpResult Battlenet::AccountMgr::UnlinkGameAccount(std::string const& /*gameAccountName*/)
{
    /*uint32 gameAccountId = GameAccountMgr::GetId(gameAccountName);
    if (!gameAccountId)
    return AccountOpResult::AOR_NAME_NOT_EXIST;

    if (!GetIdByGameAccount(gameAccountId))
    return AccountOpResult::AOR_ACCOUNT_BAD_LINK;

    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_BNET_GAME_ACCOUNT_LINK);
    stmt->setNull(0);
    stmt->setNull(1);
    stmt->setUInt32(2, gameAccountId);
    LoginDatabase.Execute(stmt);*/
    return AccountOpResult::AOR_OK;
}

uint32 Battlenet::AccountMgr::GetId(std::string const& username)
{
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_BNET_ACCOUNT_ID_BY_EMAIL);
    stmt->setString(0, username);
    if (PreparedQueryResult result = LoginDatabase.Query(stmt))
        return (*result)[0].GetUInt32();

    return 0;
}

bool Battlenet::AccountMgr::GetName(uint32 accountId, std::string& name)
{
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_BNET_ACCOUNT_EMAIL_BY_ID);
    stmt->setUInt32(0, accountId);
    if (PreparedQueryResult result = LoginDatabase.Query(stmt))
    {
        name = (*result)[0].GetString();
        return true;
    }

    return false;
}

uint8 Battlenet::AccountMgr::GetMaxIndex(uint32 /*accountId*/)
{
    /*PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_BNET_MAX_ACCOUNT_INDEX);
    stmt->setUInt32(0, accountId);
    PreparedQueryResult result = LoginDatabase.Query(stmt);
    if (result)
    return (*result)[0].GetUInt8();
    */
    return 0;
}

std::string Battlenet::AccountMgr::CalculateShaPassHash(std::string const& name, std::string const& password)
{

    Trinity::Crypto::SHA256 email;
    email.UpdateData(name);
    email.Finalize();

    Trinity::Crypto::SHA256 sha;
    sha.UpdateData(ByteArrayToHexStr(email.GetDigest()));
    sha.UpdateData(":");
    sha.UpdateData(password);
    sha.Finalize();

    return ByteArrayToHexStr(sha.GetDigest(), true);
}
