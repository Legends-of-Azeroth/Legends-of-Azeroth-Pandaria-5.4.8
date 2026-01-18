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

#ifndef TRINITYCORE_COMMON_H
#define TRINITYCORE_COMMON_H

#include "Define.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <cmath>
#include <ctgmath>

#if PLATFORM == PLATFORM_WINDOWS
#define STRCASECMP stricmp
#else
#define STRCASECMP strcasecmp
#endif

#include <set>
#include <list>
#include <string>
#include <map>
#include <queue>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#if PLATFORM == PLATFORM_WINDOWS
// XP winver - needed to compile with standard leak check in MemoryLeaks.h
// uncomment later if needed
//#define _WIN32_WINNT 0x0501
#  include <ws2tcpip.h>
//#undef WIN32_WINNT
#else
#  include <sys/types.h>
#  include <sys/ioctl.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <unistd.h>
#  include <netdb.h>
#endif

#if TRINITY_COMPILER == TRINITY_COMPILER_MICROSOFT

#define atoll _atoi64
#define llabs _abs64

#else

#define stricmp strcasecmp
#define strnicmp strncasecmp

#endif

inline unsigned long atoul(char const* str) { return strtoul(str, nullptr, 10); }
inline unsigned long long atoull(char const* str) { return strtoull(str, nullptr, 10); }

#define STRINGIZE(a) #a

enum TimeConstants : uint32
{
    MINUTE          = 60,
    HOUR            = MINUTE*60,
    DAY             = HOUR*24,
    WEEK            = DAY*7,
    MONTH           = DAY*30,
    YEAR            = MONTH*12,
    IN_MILLISECONDS = 1000
};

enum AccountTypes
{
    SEC_PLAYER          = 0,
    SEC_MODERATOR       = 1,
    SEC_GAMEMASTER      = 2,
    SEC_ADMINISTRATOR   = 3,
    SEC_CONSOLE         = 4,    // must be always last in list, accounts must have less security level always also
};

enum LocaleConstant : uint8
{
    LOCALE_enUS = 0,
    LOCALE_koKR = 1,
    LOCALE_frFR = 2,
    LOCALE_deDE = 3,
    LOCALE_zhCN = 4,
    LOCALE_zhTW = 5,
    LOCALE_esES = 6,
    LOCALE_esMX = 7,
    LOCALE_ruRU = 8,
    LOCALE_itIT = 9,
    LOCALE_ptBR = 10,
    LOCALE_ptPT = 11,
    TOTAL_LOCALES
};

#define DEFAULT_LOCALE LOCALE_enUS

#define MAX_LOCALES 11
#define MAX_ACCOUNT_TUTORIAL_VALUES 8

TC_COMMON_API extern char const* localeNames[TOTAL_LOCALES];

TC_COMMON_API LocaleConstant GetLocaleByName(const std::string& name);

typedef std::vector<std::string> StringVector;

#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

struct DbcStr
{
    char const* operator[](size_t i) const
    {
        if (i >= TOTAL_LOCALES || !m_impl[i])
            return "";
        return m_impl[i];
    }

    char const* m_impl[TOTAL_LOCALES];
};

#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

// we always use stdlibc++ std::max/std::min, undefine some not C++ standard defines (Win API and some other platforms)
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#ifndef M_PI
#define M_PI            3.14159265358979323846f
#define M_PI_2          1.57079632679489661923f   // pi/2
#endif

#define MAX_QUERY_LEN 32*1024

#endif
