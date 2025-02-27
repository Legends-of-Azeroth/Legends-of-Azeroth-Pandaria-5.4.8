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

#include "RealmList.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "Resolver.h"
#include "Util.h"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/system_timer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/as_tuple.hpp>

RealmList::RealmList() :
    _updateInterval(0)
{}

RealmList* RealmList::instance()
{
    static RealmList instance;
    return &instance;
}

// Load the realm list from the database
//void RealmList::Initialize(uint32 updateInterval)
void RealmList::Initialize(boost::asio::any_io_executor executor, uint32 updateInterval)
{
    _updateInterval = updateInterval;
    _executor = &executor;
    _resolver = std::make_unique<Trinity::Asio::Resolver>(executor);

    LoadBuildInfo();

    // Get the content of the realmlist table in the database
    UpdateRealms();
}

void RealmList::Close()
{
}

void RealmList::LoadBuildInfo()
{
    //                                                              0             1              2              3      4                5                6
    if (QueryResult result = LoginDatabase.Query("SELECT majorVersion, minorVersion, bugfixVersion, hotfixVersion, build, winChecksumSeed, macChecksumSeed FROM build_info ORDER BY build ASC"))
    {
        do
        {
            Field* fields = result->Fetch();
            RealmBuildInfo& build = _builds.emplace_back();
            build.MajorVersion = fields[0].GetUInt32();
            build.MinorVersion = fields[1].GetUInt32();
            build.BugfixVersion = fields[2].GetUInt32();
            std::string hotfixVersion = fields[3].GetString();

            if (hotfixVersion.length() < build.HotfixVersion.size())
            {
                std::copy(hotfixVersion.begin(), hotfixVersion.end(), build.HotfixVersion.begin());
            }
            else
            {
                std::fill(hotfixVersion.begin(), hotfixVersion.end(), '\0');
            }

            build.Build = fields[4].GetUInt32();
            std::string windowsHash = fields[5].GetString();

            if (windowsHash.length() == build.WindowsHash.size() * 2)
            {
                HexStrToByteArray(windowsHash, build.WindowsHash);
            }

            std::string macHash = fields[6].GetString();

            if (macHash.length() == build.MacHash.size() * 2)
            {
                HexStrToByteArray(macHash, build.MacHash);
            }
        } while (result->NextRow());
    }
}

void RealmList::UpdateRealm(RealmHandle const& id, uint32 build, std::string const& name,
    boost::asio::ip::address&& address, boost::asio::ip::address&& localAddr, boost::asio::ip::address&& localSubmask,
    uint16 port, uint8 icon, RealmFlags flag, uint8 timezone, AccountTypes allowedSecurityLevel, float population)
{
    // Create new if not exist or update existed
    Realm& realm = _realms[id];

    realm.Id = id;
    realm.Build = build;
    realm.Name = name;
    realm.Type = icon;
    realm.Flags = flag;
    realm.Timezone = timezone;
    realm.AllowedSecurityLevel = allowedSecurityLevel;
    realm.PopulationLevel = population;
    if (!realm.ExternalAddress || *realm.ExternalAddress != address)
        realm.ExternalAddress = std::make_unique<boost::asio::ip::address>(std::move(address));
    if (!realm.LocalAddress || *realm.LocalAddress != localAddr)
        realm.LocalAddress = std::make_unique<boost::asio::ip::address>(std::move(localAddr));
    if (!realm.LocalSubnetMask || *realm.LocalSubnetMask != localSubmask)
        realm.LocalSubnetMask = std::make_unique<boost::asio::ip::address>(std::move(localSubmask));

    realm.Port = port;
}

void RealmList::UpdateRealms()
{
    boost::asio::co_spawn(*_executor, [this]() -> boost::asio::awaitable<void>
    {
        boost::asio::system_timer timer(*_executor);
        while (42)
        {
            TC_LOG_INFO("server.authserver", "Updating Realm List...");

            LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_REALMLIST);
            PreparedQueryResult result = LoginDatabase.Query(stmt);

            std::map<RealmHandle, std::string> existingRealms;
            for (auto const& [handle, realm] : _realms)
            {
                existingRealms[handle] = realm.Name;
            }

            _realms.clear();

            // Circle through results and add them to the realm map
            if (result)
            {
                do
                {
                    Field* fields = result->Fetch();
                    uint32 realmId = fields[0].GetUInt32();
                    std::string name = fields[1].GetString();
                    std::string externalAddressString = fields[2].GetString();
                    std::string localAddressString = fields[3].GetString();
                    std::string localSubmaskString = fields[4].GetString();
                    uint16 port = fields[5].GetUInt16();

                    Optional<boost::asio::ip::tcp::endpoint> externalAddress = _resolver->Resolve(boost::asio::ip::tcp::v4(), externalAddressString, "");
                    if (!externalAddress)
                    {
                        TC_LOG_ERROR("server.authserver", "Could not resolve address %s for realm \"%s\" id %u", externalAddressString.c_str(), name.c_str(), realmId);
                        continue;
                    }

                    Optional<boost::asio::ip::tcp::endpoint> localAddress = _resolver->Resolve(boost::asio::ip::tcp::v4(), localAddressString, "");
                    if (!localAddress)
                    {
                        TC_LOG_ERROR("server.authserver", "Could not resolve localAddress %s for realm \"%s\" id %u", localAddressString.c_str(), name.c_str(), realmId);
                        continue;
                    }

                    Optional<boost::asio::ip::tcp::endpoint> localSubmask = _resolver->Resolve(boost::asio::ip::tcp::v4(), localSubmaskString, "");
                    if (!localSubmask)
                    {
                        TC_LOG_ERROR("server.authserver", "Could not resolve localSubnetMask %s for realm \"%s\" id %u", localSubmaskString.c_str(), name.c_str(), realmId);
                        continue;
                    }

                    uint8 icon = fields[6].GetUInt8();
                    if (icon == REALM_TYPE_FFA_PVP)
                        icon = REALM_TYPE_PVP;
                    if (icon >= MAX_CLIENT_REALM_TYPE)
                        icon = REALM_TYPE_NORMAL;

                    RealmFlags flag = RealmFlags(fields[7].GetUInt8());
                    uint8 timezone = fields[8].GetUInt8();
                    uint8 allowedSecurityLevel = fields[9].GetUInt8();
                    float pop = fields[10].GetFloat();
                    uint32 build = fields[11].GetUInt32();

                    RealmHandle id{ realmId };

                    UpdateRealm(id, build, name, externalAddress->address(), localAddress->address(), localSubmask->address(), port, icon, flag,
                        timezone, (allowedSecurityLevel <= SEC_ADMINISTRATOR ? AccountTypes(allowedSecurityLevel) : SEC_ADMINISTRATOR), pop);
                    if (!existingRealms.count(id))
                    {
                        TC_LOG_INFO("server.authserver", "Added realm \"%s\" at %s:%u.", name.c_str(), externalAddressString.c_str(), port);
                    }
                    else
                    {
                        TC_LOG_DEBUG("server.authserver", "Updating realm \"%s\" at %s:%u.", name.c_str(), externalAddressString.c_str(), port);
                    }

                    existingRealms.erase(id);

                } while (result->NextRow());
            }

            for (auto itr = existingRealms.begin(); itr != existingRealms.end(); ++itr)
                TC_LOG_INFO("server.authserver", "Removed realm \"%s\".", itr->second.c_str());

            timer.expires_after(std::chrono::seconds(_updateInterval));
            auto [err] = co_await timer.async_wait(boost::asio::as_tuple(boost::asio::use_awaitable));
            if (err) break;
        }
    }, boost::asio::detached);
}

Realm const* RealmList::GetRealm(RealmHandle const& id) const
{
    auto itr = _realms.find(id);
    if (itr != _realms.end())
    {
        return &itr->second;
    }

    return nullptr;
}

RealmBuildInfo const* RealmList::GetBuildInfo(uint32 build) const
{
    for (RealmBuildInfo const& clientBuild : _builds)
    {
        if (clientBuild.Build == build)
        {
            return &clientBuild;
        }
    }

    return nullptr;
}