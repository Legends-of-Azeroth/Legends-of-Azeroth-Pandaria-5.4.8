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

#ifndef TRINITYCORE_ERRORS_H
#define TRINITYCORE_ERRORS_H

#include "Define.h"
#include <string>

namespace Trinity
{
    [[noreturn]] TC_COMMON_API void Assert(char const* file, int line, char const* function, std::string debugInfo, char const* message);
    [[noreturn]] TC_COMMON_API void Assert(char const* file, int line, char const* function, std::string debugInfo, char const* message, char const* format, ...) ATTR_PRINTF(6, 7);
    [[noreturn]] TC_COMMON_API void Fatal(char const* file, int line, char const* function, char const* message, ...) ATTR_PRINTF(4, 5);
    [[noreturn]] TC_COMMON_API void Error(char const* file, int line, char const* function, char const* message);

    [[noreturn]] TC_COMMON_API void Abort(char const* file, int line, char const* function);
    [[noreturn]] TC_COMMON_API void Abort(char const* file, int line, char const* function, char const* message, ...);

    TC_COMMON_API void Warning(char const* file, int line, char const* function, char const* message);

    [[noreturn]] TC_COMMON_API void AbortHandler(int sigval);

} // namespace Trinity

TC_COMMON_API std::string GetDebugInfo();

#if TRINITY_COMPILER == TRINITY_COMPILER_MICROSOFT
#define ASSERT_BEGIN __pragma(warning(push)) __pragma(warning(disable: 4127))
#define ASSERT_END __pragma(warning(pop))
#else
#define ASSERT_BEGIN
#define ASSERT_END
#endif

#if TRINITY_PLATFORM == TRINITY_PLATFORM_WINDOWS
#define EXCEPTION_ASSERTION_FAILURE 0xC0000420L
#endif

#define WPAssert(cond, ...) do { if (!(cond)) Trinity::Assert(__FILE__, __LINE__, __FUNCTION__, GetDebugInfo(), #cond, ##__VA_ARGS__); } while(0) ASSERT_END
#define WPAssert_NODEBUGINFO(cond, ...) do { if (!(cond)) Trinity::Assert(__FILE__, __LINE__, __FUNCTION__, "", #cond, ##__VA_ARGS__); } while(0) ASSERT_END
#define WPFatal(cond, ...) ASSERT_BEGIN do { if (!(cond)) Trinity::Fatal(__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); } while(0) ASSERT_END
#define WPError(cond, msg) do { if (!(cond)) Trinity::Error(__FILE__, __LINE__, __FUNCTION__, (msg)); } while (0) ASSERT_END
#define WPWarning(cond, msg) do { if (!(cond)) Trinity::Warning(__FILE__, __LINE__, __FUNCTION__, (msg)); } while (0) ASSERT_END
#define WPAbort() do { Trinity::Abort(__FILE__, __LINE__, __FUNCTION__); } while(0) ASSERT_END
#define WPAbort_MSG(msg, ...) ASSERT_BEGIN do { Trinity::Abort(__FILE__, __LINE__, __FUNCTION__, (msg), ##__VA_ARGS__); } while(0) ASSERT_END

#define STR(x) #x
#define STR_(x) STR(x)
#define STR_LINE STR_(__LINE__)
#define DEBUG_STR(x) ("ASSERTION FAILED: " #x " at " __FILE__ ":" STR_LINE )

#define ASSERT WPAssert
#define ABORT WPAbort
#define ABORT_MSG WPAbort_MSG

template <typename T>
inline T* ASSERT_NOTNULL_IMPL(T* pointer, char const* expr)
{
    ASSERT(pointer, "%s", expr);
    return pointer;
}

#define ASSERT_NOTNULL(pointer) ASSERT_NOTNULL_IMPL(pointer, #pointer)

void LogNotImplementedCall(char const* name);

#define CALL_NOT_IMPLEMENTED() LogNotImplementedCall(__FUNCTION__)

class Exception : public std::exception
{
public:
    Exception() { }
    Exception(std::string const& what) : _what(what) { }
    Exception(char const* what) : _what(what) { }
    Exception(Exception const& other) : _what(other._what) { }
    ~Exception() { }

    char const* what() const throw()
    {
        return _what.c_str();
    }

private:
    std::string _what;
};

#endif
