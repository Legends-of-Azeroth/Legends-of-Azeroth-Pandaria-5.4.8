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

/**
* @file main.cpp
* @brief Authentication Server main program
*
* This file contains the main program for the
* authentication server
*/

#include "AppenderDB.h"
#include "AuthSocketMgr.h"
#include "Banner.h"
#include "Configuration/Config.h"
#include "DatabaseEnv.h"
#include "DatabaseLoader.h"
#include "GitRevision.h"
#include "Log.h"
#include "MySQLThreading.h"
#include "OpenSSLCrypto.h"
#include "ProcessPriority.h"
#include "RealmList.h"
#include "SignalHandler.h"
#include "Util.h"

#include <boost/asio/signal_set.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/system_timer.hpp>
#include <boost/asio/as_tuple.hpp>

#include <openssl/crypto.h>
#include <openssl/opensslv.h>

#include <iostream>
#include <csignal>

using namespace boost::program_options;
namespace fs = boost::filesystem;

template <typename T>
using Async = boost::asio::awaitable<T>;

#ifndef _TRINITY_REALM_CONFIG
# define _TRINITY_REALM_CONFIG  "authserver.conf"
#endif

#if TRINITY_PLATFORM == TRINITY_PLATFORM_WINDOWS
#include "ServiceWin32.h"
char serviceName[] = "authserver";
char serviceLongName[] = "TrinityCore auth service";
char serviceDescription[] = "TrinityCore World of Warcraft emulator auth service";
/*
* -1 - not in service mode
*  0 - stopped
*  1 - running
*  2 - paused
*/
int m_ServiceStatus = -1;
#endif

bool StartDB();
void StopDB();

variables_map GetConsoleArguments(int argc, char** argv, fs::path& configFile, std::string& configService);

/// Initialize connection to the database
bool StartDB()
{
    MySQL::Library_Init();

    std::string dbstring = sConfigMgr->GetStringDefault("LoginDatabaseInfo", "");
    if (dbstring.empty())
    {
        TC_LOG_ERROR("server.authserver", "Database not specified");
        return false;
    }

    int32 worker_threads = sConfigMgr->GetIntDefault("LoginDatabase.WorkerThreads", 1);
    if (worker_threads < 1 || worker_threads > 32)
    {
        TC_LOG_ERROR("server.authserver", "Improper value specified for LoginDatabase.WorkerThreads, defaulting to 1.");
        worker_threads = 1;
    }

    int32 synch_threads = sConfigMgr->GetIntDefault("LoginDatabase.SynchThreads", 1);
    if (synch_threads < 1 || synch_threads > 32)
    {
        TC_LOG_ERROR("server.authserver", "Improper value specified for LoginDatabase.SynchThreads, defaulting to 1.");
        synch_threads = 1;
    }

    DatabaseLoader loader("server.authserver", DatabaseLoader::DATABASE_NONE);
    loader
        .AddDatabase(LoginDatabase, "Login");

    if (!loader.Load())
        return false;

    TC_LOG_INFO("server.authserver", "Started auth database connection pool.");        

    // // NOTE: While authserver is singlethreaded you should keep synch_threads == 1. Increasing it is just silly since only 1 will be used ever.
    // if (!LoginDatabase.Open(dbstring, uint8(worker_threads), uint8(synch_threads)))
    // {
    //     TC_LOG_ERROR("server.authserver", "Cannot connect to database");
    //     return false;
    // }

    TC_LOG_INFO("server.authserver", "Started auth database connection pool.");
    sLog->SetRealmId(0); // Enables DB appenders when realm is set.
    return true;
}

/// Close the connection to the database
void StopDB()
{
    LoginDatabase.Close();
    MySQL::Library_End();
}

Async<void> KeepDatabaseAliveHandler(boost::asio::any_io_executor exec, int32 dbPingInterval)
{
    // Self managed

    boost::asio::system_timer timer(exec);
    while (42)
    {
        TC_LOG_INFO("server.authserver", "Ping MySQL to keep connection alive");
        LoginDatabase.KeepAlive();
        timer.expires_after(std::chrono::minutes(dbPingInterval));
        auto [err] = co_await timer.async_wait(boost::asio::as_tuple(boost::asio::use_awaitable));
        if (err) break;
    }

    co_return;
}

Async<void> BanExpiryHandler(boost::asio::any_io_executor exec, int32 banExpiryCheckInterval)
{
    // Self managed

    boost::asio::system_timer timer(exec);
    while (42)
    {
        LoginDatabase.Execute(LoginDatabase.GetPreparedStatement(LOGIN_DEL_EXPIRED_IP_BANS));
        LoginDatabase.Execute(LoginDatabase.GetPreparedStatement(LOGIN_UPD_EXPIRED_ACCOUNT_BANS));
        timer.expires_after(std::chrono::minutes(banExpiryCheckInterval));
        auto [err] = co_await timer.async_wait(boost::asio::as_tuple(boost::asio::use_awaitable));
        if (err) break;
    }

    co_return;
}

#if TRINITY_PLATFORM == TRINITY_PLATFORM_WINDOWS
Async<void> ServiceStatusWatcher(boost::asio::thread_pool& pool)
{
    boost::asio::system_timer timer(pool.get_executor());
    while (42)
    {
        if (m_ServiceStatus == 0)
            pool.stop();

        timer.expires_after(std::chrono::seconds(1));
        auto [err] = co_await timer.async_wait(boost::asio::as_tuple(boost::asio::use_awaitable));
        if (err)
        {
            pool.stop();
            break;
        }
    }

    co_return;
}
#endif

variables_map GetConsoleArguments(int argc, char** argv, fs::path& configFile, std::string& configService)
{
    options_description all("Allowed options");
    all.add_options()
        ("help,h", "print usage message")
        ("version,v", "print version build info")
        ("config,c", value<fs::path>(&configFile)->default_value(fs::absolute(_TRINITY_REALM_CONFIG)),
                     "use <arg> as configuration file")
        ;
#if TRINITY_PLATFORM == TRINITY_PLATFORM_WINDOWS
    options_description win("Windows platform specific options");
    win.add_options()
        ("service,s", value<std::string>(&configService)->default_value(""), "Windows service options: [install | uninstall]")
        ;

    all.add(win);
#else
    (void)configService;
#endif
    variables_map variablesMap;
    try
    {
        store(command_line_parser(argc, argv).options(all).allow_unregistered().run(), variablesMap);
        notify(variablesMap);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << "\n";
    }

    if (variablesMap.count("help"))
        std::cout << all << "\n";
    else if (variablesMap.count("version"))
        std::cout << GitRevision::GetFullVersion() << "\n";

    return variablesMap;
}

Async<int> async_main(boost::asio::thread_pool& pool)
{
    // small hack without rewritting everything
    // await 2 seconds for the boost::threading pool to start
    boost::asio::system_timer timer(pool.get_executor());
    timer.expires_after(std::chrono::seconds(2));
    co_await timer.async_wait(boost::asio::use_awaitable);

    if (sRealmList->GetRealms().empty())
    {
        TC_LOG_ERROR("server.authserver", "No valid realms specified.");
        pool.stop();
        co_return 1;
    }

    // Launch the listening network socket
    int32 port = sConfigMgr->GetIntDefault("RealmServerPort", 3724);
    if (port < 0 || port > 0xFFFF)
    {
        TC_LOG_ERROR("server.authserver", "Specified port out of allowed range (1-65535)");
        co_return 1;
    }

    std::string bind_ip = sConfigMgr->GetStringDefault("BindIP", "0.0.0.0");
    if (!sAuthSocketMgr.StartNetwork(pool.get_executor(), bind_ip, port))
    {
        TC_LOG_ERROR("server.authserver", "Failed to initialize network");
        co_return 1;
    }

    // Set process priority according to configuration settings
    SetProcessPriority("server.authserver", sConfigMgr->GetIntDefault(CONFIG_PROCESSOR_AFFINITY, 0), sConfigMgr->GetBoolDefault(CONFIG_HIGH_PRIORITY, false));

    // Enabled a timed callback for handling the database keep alive ping
    int32 dbPingInterval = sConfigMgr->GetIntDefault("MaxPingTime", 30);
    boost::asio::co_spawn(pool.get_executor(), KeepDatabaseAliveHandler(pool.get_executor(), dbPingInterval), boost::asio::detached);


    int32 banExpiryCheckInterval = sConfigMgr->GetIntDefault("BanExpiryCheckInterval", 60);
    boost::asio::co_spawn(pool.get_executor(), BanExpiryHandler(pool.get_executor(), banExpiryCheckInterval), boost::asio::detached);

#if TRINITY_PLATFORM == TRINITY_PLATFORM_WINDOWS
    if (m_ServiceStatus != -1)
    {
        boost::asio::co_spawn(pool.get_executor(), ServiceStatusWatcher(pool), boost::asio::detached);
    }
#endif

    co_return 0;
}

int main(int argc, char** argv)
{
    signal(SIGABRT, &Trinity::AbortHandler);
    // Command line parsing to get the configuration file name
    auto configFile = fs::absolute(_TRINITY_REALM_CONFIG);
    std::string configService;
    auto vm = GetConsoleArguments(argc, argv, configFile, configService);
    // exit if help or version is enabled
    if (vm.count("help") || vm.count("version"))
        return 0;

    std::string configError;
    if (!sConfigMgr->LoadInitial(configFile.generic_string(),
        std::vector<std::string>(argv, argv + argc),
        configError))
    {
        printf("Error in config file: %s\n", configError.c_str());
        return 1;
    }

    std::vector<std::string> overriddenKeys = sConfigMgr->OverrideWithEnvVariablesIfAny();

    sLog->RegisterAppender<AppenderDB>();
    sLog->Initialize(nullptr);

    Trinity::Banner::Show("authserver",
        [](char const* text)
        {
            TC_LOG_INFO("server.authserver", "%s", text);
        },
        []()
        {
            TC_LOG_INFO("server.authserver", "Using configuration file %s.", sConfigMgr->GetFilename().c_str());
            TC_LOG_INFO("server.authserver", "Using SSL version: %s (library: %s)", OPENSSL_VERSION_TEXT, OpenSSL_version(OPENSSL_VERSION));
            TC_LOG_INFO("server.authserver", "Using Boost version: %i.%i.%i", BOOST_VERSION / 100000, BOOST_VERSION / 100 % 1000, BOOST_VERSION % 100);
        }
    );

    for (std::string const& key : overriddenKeys)
        TC_LOG_INFO("server.authserver", "Configuration field '%s' was overridden with environment variable.", key.c_str());

    OpenSSLCrypto::threadsSetup(boost::dll::program_location().remove_filename());


    // authserver PID file creation
    std::string pidFile = sConfigMgr->GetStringDefault("PidFile", "");
    if (!pidFile.empty())
    {
        if (uint32 pid = CreatePIDFile(pidFile))
            TC_LOG_INFO("server.authserver", "Daemon PID: %u\n", pid);
        else
        {
            TC_LOG_ERROR("server.authserver", "Cannot create PID file %s.\n", pidFile.c_str());
            return 1;
        }
    }

    // Initialize the database connection
    if (!StartDB())
        return 1;

    boost::asio::thread_pool pool(4);
    // Set signal handlers
    boost::asio::signal_set signals(pool.get_executor(), SIGINT, SIGTERM);
    signals.async_wait([&pool](boost::system::error_code ec, int /*signal_number*/)
    {
        if (!ec)
        {
            std::cout << "Signal catch, pool.stop()\n";
            pool.stop();
        }
    });

    // Get the list of realms for the server
    sRealmList->Initialize(pool.get_executor(), sConfigMgr->GetIntDefault("RealmsStateUpdateDelay", 20));
    boost::asio::co_spawn(pool.get_executor(), async_main(pool), boost::asio::detached);

    // Start the io service worker loop
    pool.join();
    StopDB();
    sRealmList->Close();
    sAuthSocketMgr.StopNetwork();
    OpenSSLCrypto::threadsCleanup();

    TC_LOG_INFO("server.authserver", "Halting process...");
    return 0;
}