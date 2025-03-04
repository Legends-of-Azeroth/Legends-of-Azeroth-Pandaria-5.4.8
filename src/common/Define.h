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

#ifndef TRINITY_DEFINE_H
#define TRINITY_DEFINE_H

#include "CompilerDefs.h"

#include <cinttypes>
#include <cstddef>
#include <climits>

#define TRINITY_LITTLEENDIAN 0
#define TRINITY_BIGENDIAN    1

#if !defined(TRINITY_ENDIAN)
#  if defined (BOOST_BIG_ENDIAN)
#    define TRINITY_ENDIAN TRINITY_BIGENDIAN
#  else
#    define TRINITY_ENDIAN TRINITY_LITTLEENDIAN
#  endif
#endif

#if PLATFORM == PLATFORM_WINDOWS
#  define TRINITY_PATH_MAX 260
#  ifndef DECLSPEC_NORETURN
#    define DECLSPEC_NORETURN __declspec(noreturn)
#  endif //DECLSPEC_NORETURN
#  ifndef DECLSPEC_DEPRECATED
#    define DECLSPEC_DEPRECATED __declspec(deprecated)
#  endif //DECLSPEC_DEPRECATED
#else //PLATFORM != PLATFORM_WINDOWS
#  define TRINITY_PATH_MAX PATH_MAX
#  define DECLSPEC_NORETURN
#  define DECLSPEC_DEPRECATED
#endif //PLATFORM

#if !defined(COREDEBUG)
#  define TRINITY_INLINE inline
#else //COREDEBUG
#  if !defined(TRINITY_DEBUG)
#    define TRINITY_DEBUG
#  endif //TRINITY_DEBUG
#  define TRINITY_INLINE
#endif //!COREDEBUG

#if TRINITY_COMPILER == TRINITY_COMPILER_GNU
#  define ATTR_NORETURN __attribute__((noreturn))
#  define ATTR_DEPRECATED __attribute__((deprecated))
#else //COMPILER != COMPILER_GNU
#  define ATTR_NORETURN
#  define ATTR_DEPRECATED
#endif //COMPILER == COMPILER_GNU

#if TRINITY_COMPILER == TRINITY_COMPILER_GNU
#  define ATTR_PRINTF(F, V) __attribute__ ((__format__ (__printf__, F, V)))
#else //TRINITY_COMPILER != TRINITY_COMPILER_GNU
#  define ATTR_PRINTF(F, V)
#endif //TRINITY_COMPILER == TRINITY_COMPILER_GNU

#ifdef TRINITY_API_USE_DYNAMIC_LINKING
#  if PLATFORM == PLATFORM_WINDOWS
#    define TC_API_EXPORT __declspec(dllexport)
#    define TC_API_IMPORT __declspec(dllimport)
#  elif COMPILER == COMPILER_GNU
#    define TC_API_EXPORT __attribute__((visibility("default")))
#    define TC_API_IMPORT
#  else
#    error compiler not supported!
#  endif
#else
#  define TC_API_EXPORT
#  define TC_API_IMPORT
#endif

#ifdef TRINITY_API_EXPORT_COMMON
#  define TC_COMMON_API TC_API_EXPORT
#else
#  define TC_COMMON_API TC_API_IMPORT
#endif

#ifdef TRINITY_API_EXPORT_DATABASE
#  define TC_DATABASE_API TC_API_EXPORT
#else
#  define TC_DATABASE_API TC_API_IMPORT
#endif

#ifdef TRINITY_API_EXPORT_SHARED
#  define TC_SHARED_API TC_API_EXPORT
#else
#  define TC_SHARED_API TC_API_IMPORT
#endif

#ifdef TRINITY_API_EXPORT_GAME
#  define TC_GAME_API TC_API_EXPORT
#else
#  define TC_GAME_API TC_API_IMPORT
#endif

#define UI64FMTD "%" PRIu64
#define UI64LIT(N) UINT64_C(N)

#define SI64FMTD "%" PRId64
#define SI64LIT(N) INT64_C(N)

#define SZFMTD "%" PRIuPTR

#define STRING_VIEW_FMT "%.*s"
#define STRING_VIEW_FMT_ARG(str) static_cast<int>((str).length()), (str).data()

typedef std::int64_t  int64;
typedef std::int32_t  int32;
typedef std::int16_t  int16;
typedef std::int8_t   int8;
typedef std::uint64_t uint64;
typedef std::uint32_t uint32;
typedef std::uint16_t uint16;
typedef std::uint8_t  uint8;

enum DBCFormer
{
    FT_NA = 'x',                                            //not used or unknown, 4 byte size
    FT_NA_BYTE = 'X',                                       //not used or unknown, byte
    FT_STRING = 's',                                        //char*
    FT_FLOAT = 'f',                                         //float
    FT_INT = 'i',                                           //uint32
    FT_BYTE = 'b',                                          //uint8
    FT_LONG = 'l',                                          //uint64
    FT_SORT = 'd',                                          //sorted by this field, field is not included
    FT_IND = 'n',                                           //the same, but parsed to data
    FT_SQL_PRESENT = 'p',                                   //Used in sql format to mark column present in sql dbc
    FT_SQL_ABSENT = 'a'                                     //Used in sql format to mark column absent in sql dbc
};

#endif //TRINITY_DEFINE_H
