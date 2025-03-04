/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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

#ifndef _UTIL_H
#define _UTIL_H

#include "Duration.h"
#include "Errors.h"
#include "Optional.h"

#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <ctime>
#include <array>
#include <limits>

// Searcher for map of structs
template<typename T, class S> struct Finder
{
    T val_;
    T S::* idMember_;

    Finder(T val, T S::* idMember) : val_(val), idMember_(idMember) {}
    bool operator()(const std::pair<int, S> &obj) { return obj.second.*idMember_ == val_; }
};

class Tokenizer
{
public:
    typedef std::vector<char const*> StorageType;

    typedef StorageType::size_type size_type;

    typedef StorageType::const_iterator const_iterator;
    typedef StorageType::reference reference;
    typedef StorageType::const_reference const_reference;

public:
    Tokenizer(const std::string &src, char const sep, uint32 vectorReserve = 0);
    ~Tokenizer() { delete[] m_str; }

    const_iterator begin() const { return m_storage.begin(); }
    const_iterator end() const { return m_storage.end(); }

    size_type size() const { return m_storage.size(); }

    reference operator [] (size_type i) { return m_storage[i]; }
    const_reference operator [] (size_type i) const { return m_storage[i]; }

private:
    char* m_str;
    StorageType m_storage;
};

namespace Trinity
{
    TC_COMMON_API std::vector<std::string_view> Tokenize(std::string_view str, char sep, bool keepEmpty);

    /* this would return string_view into temporary otherwise */
    std::vector<std::string_view> Tokenize(std::string&&, char, bool) = delete;
    std::vector<std::string_view> Tokenize(std::string const&&, char, bool) = delete;

    /* the delete overload means we need to make this explicit */
    inline std::vector<std::string_view> Tokenize(char const* str, char sep, bool keepEmpty) { return Tokenize(std::string_view(str ? str : ""), sep, keepEmpty); }
}

void stripLineInvisibleChars(std::string &src);

int64 MoneyStringToMoney(const std::string& moneyString);

std::string secsToTimeString(uint64 timeInSecs, bool shortText = false, bool hoursOnly = false);
uint32 TimeStringToSecs(const std::string& timestring);
std::string TimeToTimestampStr(time_t t);

struct tm* localtime_r(time_t const* time, struct tm *result);

inline void ApplyPercentModFloatVar(float& var, float val, bool apply)
{
    if (val == -100.0f)     // prevent set var to zero
        val = -99.99f;
    var *= (apply ? (100.0f + val) / 100.0f : 100.0f / (100.0f + val));
}

// Percentage calculation
template <class T, class U>
inline T CalculatePct(T base, U pct)
{
    return T(base * static_cast<float>(pct) / 100.0f);
}

template <class T, class U>
inline T AddPct(T &base, U pct)
{
    return base += CalculatePct(base, pct);
}

template <class T, class U>
inline T ApplyPct(T &base, U pct)
{
    return base = CalculatePct(base, pct);
}

template <class T>
inline T RoundToInterval(T& num, T floor, T ceil)
{
    return num = std::min(std::max(num, floor), ceil);
}

bool WStrToUtf8(std::wstring wstr, std::string& utf8str);
// size==real string size
bool WStrToUtf8(wchar_t* wstr, size_t size, std::string& utf8str);

size_t utf8length(std::string& utf8str);                    // set string to "" if invalid utf8 sequence
void utf8truncate(std::string& utf8str, size_t len);

// UTF8 handling
TC_COMMON_API bool Utf8toWStr(std::string_view utf8str, std::wstring& wstr);

// in wsize==max size of buffer, out wsize==real string size
TC_COMMON_API bool Utf8toWStr(char const* utf8str, size_t csize, wchar_t* wstr, size_t& wsize);

inline bool Utf8toWStr(std::string_view utf8str, wchar_t* wstr, size_t& wsize)
{
    return Utf8toWStr(utf8str.data(), utf8str.size(), wstr, wsize);
}

TC_COMMON_API bool WStrToUtf8(std::wstring_view wstr, std::string& utf8str);

inline bool isBasicLatinCharacter(wchar_t wchar)
{
    if (wchar >= L'a' && wchar <= L'z')                      // LATIN SMALL LETTER A - LATIN SMALL LETTER Z
        return true;
    if (wchar >= L'A' && wchar <= L'Z')                      // LATIN CAPITAL LETTER A - LATIN CAPITAL LETTER Z
        return true;
    return false;
}

inline bool isExtendedLatinCharacter(wchar_t wchar)
{
    if (isBasicLatinCharacter(wchar))
        return true;
    if (wchar >= 0x00C0 && wchar <= 0x00D6)                  // LATIN CAPITAL LETTER A WITH GRAVE - LATIN CAPITAL LETTER O WITH DIAERESIS
        return true;
    if (wchar >= 0x00D8 && wchar <= 0x00DE)                  // LATIN CAPITAL LETTER O WITH STROKE - LATIN CAPITAL LETTER THORN
        return true;
    if (wchar == 0x00DF)                                     // LATIN SMALL LETTER SHARP S
        return true;
    if (wchar >= 0x00E0 && wchar <= 0x00F6)                  // LATIN SMALL LETTER A WITH GRAVE - LATIN SMALL LETTER O WITH DIAERESIS
        return true;
    if (wchar >= 0x00F8 && wchar <= 0x00FE)                  // LATIN SMALL LETTER O WITH STROKE - LATIN SMALL LETTER THORN
        return true;
    if (wchar >= 0x0100 && wchar <= 0x012F)                  // LATIN CAPITAL LETTER A WITH MACRON - LATIN SMALL LETTER I WITH OGONEK
        return true;
    if (wchar == 0x1E9E)                                     // LATIN CAPITAL LETTER SHARP S
        return true;
    return false;
}

inline bool isCyrillicCharacter(wchar_t wchar)
{
    if (wchar >= 0x0410 && wchar <= 0x044F)                  // CYRILLIC CAPITAL LETTER A - CYRILLIC SMALL LETTER YA
        return true;
    if (wchar == 0x0401 || wchar == 0x0451)                  // CYRILLIC CAPITAL LETTER IO, CYRILLIC SMALL LETTER IO
        return true;
    return false;
}

inline bool isEastAsianCharacter(wchar_t wchar)
{
    if (wchar >= 0x1100 && wchar <= 0x11F9)                  // Hangul Jamo
        return true;
    if (wchar >= 0x3041 && wchar <= 0x30FF)                  // Hiragana + Katakana
        return true;
    if (wchar >= 0x3131 && wchar <= 0x318E)                  // Hangul Compatibility Jamo
        return true;
    if (wchar >= 0x31F0 && wchar <= 0x31FF)                  // Katakana Phonetic Ext.
        return true;
    if (wchar >= 0x3400 && wchar <= 0x4DB5)                  // CJK Ideographs Ext. A
        return true;
    if (wchar >= 0x4E00 && wchar <= 0x9FC3)                  // Unified CJK Ideographs
        return true;
    if (wchar >= 0xAC00 && wchar <= 0xD7A3)                  // Hangul Syllables
        return true;
    if (wchar >= 0xFF01 && wchar <= 0xFFEE)                  // Halfwidth forms
        return true;
    return false;
}

inline bool isNumeric(wchar_t wchar)
{
    return (wchar >= L'0' && wchar <=L'9');
}

inline bool isNumeric(char c)
{
    return (c >= '0' && c <='9');
}

inline bool isNumeric(char const* str)
{
    for (char const* c = str; *c; ++c)
        if (!isNumeric(*c))
            return false;

    return true;
}

inline bool isNumericOrSpace(wchar_t wchar)
{
    return isNumeric(wchar) || wchar == L' ';
}

inline bool isBasicLatinString(const std::wstring &wstr, bool numericOrSpace)
{
    for (size_t i = 0; i < wstr.size(); ++i)
        if (!isBasicLatinCharacter(wstr[i]) && (!numericOrSpace || !isNumericOrSpace(wstr[i])))
            return false;
    return true;
}

inline bool isExtendedLatinString(const std::wstring &wstr, bool numericOrSpace)
{
    for (size_t i = 0; i < wstr.size(); ++i)
        if (!isExtendedLatinCharacter(wstr[i]) && (!numericOrSpace || !isNumericOrSpace(wstr[i])))
            return false;
    return true;
}

inline bool isCyrillicString(const std::wstring &wstr, bool numericOrSpace)
{
    for (size_t i = 0; i < wstr.size(); ++i)
        if (!isCyrillicCharacter(wstr[i]) && (!numericOrSpace || !isNumericOrSpace(wstr[i])))
            return false;
    return true;
}

inline bool isEastAsianString(const std::wstring &wstr, bool numericOrSpace)
{
    for (size_t i = 0; i < wstr.size(); ++i)
        if (!isEastAsianCharacter(wstr[i]) && (!numericOrSpace || !isNumericOrSpace(wstr[i])))
            return false;
    return true;
}

inline wchar_t wcharToUpper(wchar_t wchar)
{
    if (wchar >= L'a' && wchar <= L'z')                      // LATIN SMALL LETTER A - LATIN SMALL LETTER Z
        return wchar_t(uint16(wchar)-0x0020);
    if (wchar == 0x00DF)                                     // LATIN SMALL LETTER SHARP S
        return wchar_t(0x1E9E);
    if (wchar >= 0x00E0 && wchar <= 0x00F6)                  // LATIN SMALL LETTER A WITH GRAVE - LATIN SMALL LETTER O WITH DIAERESIS
        return wchar_t(uint16(wchar)-0x0020);
    if (wchar >= 0x00F8 && wchar <= 0x00FE)                  // LATIN SMALL LETTER O WITH STROKE - LATIN SMALL LETTER THORN
        return wchar_t(uint16(wchar)-0x0020);
    if (wchar >= 0x0101 && wchar <= 0x012F)                  // LATIN SMALL LETTER A WITH MACRON - LATIN SMALL LETTER I WITH OGONEK (only %2=1)
    {
        if (wchar % 2 == 1)
            return wchar_t(uint16(wchar)-0x0001);
    }
    if (wchar >= 0x0430 && wchar <= 0x044F)                  // CYRILLIC SMALL LETTER A - CYRILLIC SMALL LETTER YA
        return wchar_t(uint16(wchar)-0x0020);
    if (wchar == 0x0451)                                     // CYRILLIC SMALL LETTER IO
        return wchar_t(0x0401);

    return wchar;
}

inline wchar_t wcharToUpperOnlyLatin(wchar_t wchar)
{
    return isBasicLatinCharacter(wchar) ? wcharToUpper(wchar) : wchar;
}

inline wchar_t wcharToLower(wchar_t wchar)
{
    if (wchar >= L'A' && wchar <= L'Z')                      // LATIN CAPITAL LETTER A - LATIN CAPITAL LETTER Z
        return wchar_t(uint16(wchar)+0x0020);
    if (wchar >= 0x00C0 && wchar <= 0x00D6)                  // LATIN CAPITAL LETTER A WITH GRAVE - LATIN CAPITAL LETTER O WITH DIAERESIS
        return wchar_t(uint16(wchar)+0x0020);
    if (wchar >= 0x00D8 && wchar <= 0x00DE)                  // LATIN CAPITAL LETTER O WITH STROKE - LATIN CAPITAL LETTER THORN
        return wchar_t(uint16(wchar)+0x0020);
    if (wchar >= 0x0100 && wchar <= 0x012E)                  // LATIN CAPITAL LETTER A WITH MACRON - LATIN CAPITAL LETTER I WITH OGONEK (only %2=0)
    {
        if (wchar % 2 == 0)
            return wchar_t(uint16(wchar)+0x0001);
    }
    if (wchar == 0x1E9E)                                     // LATIN CAPITAL LETTER SHARP S
        return wchar_t(0x00DF);
    if (wchar == 0x0401)                                     // CYRILLIC CAPITAL LETTER IO
        return wchar_t(0x0451);
    if (wchar >= 0x0410 && wchar <= 0x042F)                  // CYRILLIC CAPITAL LETTER A - CYRILLIC CAPITAL LETTER YA
        return wchar_t(uint16(wchar)+0x0020);

    return wchar;
}

inline void wstrToUpper(std::wstring& str)
{
    std::transform( str.begin(), str.end(), str.begin(), wcharToUpper );
}

inline void wstrToLower(std::wstring& str)
{
    std::transform( str.begin(), str.end(), str.begin(), wcharToLower );
}

std::wstring GetMainPartOfName(std::wstring wname, uint32 declension);

bool utf8ToConsole(const std::string& utf8str, std::string& conStr);
bool consoleToUtf8(const std::string& conStr, std::string& utf8str);
bool Utf8FitTo(const std::string& str, std::wstring search);
void utf8printf(FILE* out, const char *str, ...);
void vutf8printf(FILE* out, const char *str, va_list* ap);
TC_COMMON_API bool Utf8ToUpperOnlyLatin(std::string& utf8String);

#if TRINITY_PLATFORM == TRINITY_PLATFORM_WINDOWS
TC_COMMON_API bool ReadWinConsole(std::string& str, size_t size = 256);
TC_COMMON_API bool WriteWinConsole(std::string_view str, bool error = false);
#endif

TC_COMMON_API Optional<std::size_t> RemoveCRLF(std::string& str);

TC_COMMON_API bool IsIPAddress(char const* ipaddress);

uint32 CreatePIDFile(const std::string& filename);

namespace Trinity::Impl
{
    TC_COMMON_API std::string ByteArrayToHexStr(uint8 const* bytes, size_t length, bool reverse = false);
    TC_COMMON_API void HexStrToByteArray(std::string_view str, uint8* out, size_t outlen, bool reverse = false);
}

template <typename Container>
std::string ByteArrayToHexStr(Container const& c, bool reverse = false)
{
    return Trinity::Impl::ByteArrayToHexStr(std::data(c), std::size(c), reverse);
}

template <size_t Size>
void HexStrToByteArray(std::string_view str, std::array<uint8, Size>& buf, bool reverse = false)
{
    Trinity::Impl::HexStrToByteArray(str, buf.data(), Size, reverse);
}
template <size_t Size>
std::array<uint8, Size> HexStrToByteArray(std::string_view str, bool reverse = false)
{
    std::array<uint8, Size> arr;
    HexStrToByteArray(str, arr, reverse);
    return arr;
}

inline std::vector<uint8> HexStrToByteVector(std::string_view str, bool reverse = false)
{
    std::vector<uint8> buf;
    size_t const sz = (str.size() / 2);
    buf.resize(sz);
    Trinity::Impl::HexStrToByteArray(str, buf.data(), sz, reverse);
    return buf;
}

//std::string ByteArrayToHexStr(uint8 const* bytes, uint32 length, bool reverse = false);
bool StringToBool(std::string const& str);
TC_COMMON_API bool StringEqualI(std::string_view str1, std::string_view str2);
#endif

//handler for operations on large flags
#ifndef _FLAG96
#define _FLAG96

// simple class for not-modifyable list
template <typename T>
class HookList
{
    typedef typename std::list<T>::iterator ListIterator;
    private:
        typename std::list<T> m_list;
    public:
        HookList<T> & operator+=(T t)
        {
            m_list.push_back(t);
            return *this;
        }
        HookList<T> & operator-=(T t)
        {
            m_list.remove(t);
            return *this;
        }
        size_t size()
        {
            return m_list.size();
        }
        ListIterator begin()
        {
            return m_list.begin();
        }
        ListIterator end()
        {
            return m_list.end();
        }
};

class flag128
{
private:
    uint32 part[4];

public:
    flag128(uint32 p1 = 0, uint32 p2 = 0, uint32 p3 = 0, uint32 p4 = 0)
    {
        part[0] = p1;
        part[1] = p2;
        part[2] = p3;
        part[3] = p4;
    }

    inline bool IsEqual(uint32 p1 = 0, uint32 p2 = 0, uint32 p3 = 0, uint32 p4 = 0) const
    {
        return (part[0] == p1 && part[1] == p2 && part[2] == p3 && part[3] == p4);
    }

    inline bool HasFlag(uint32 p1 = 0, uint32 p2 = 0, uint32 p3 = 0, uint32 p4 = 0) const
    {
        return (part[0] & p1 || part[1] & p2 || part[2] & p3 || part[3] & p4);
    }

    inline void Set(uint32 p1 = 0, uint32 p2 = 0, uint32 p3 = 0, uint32 p4 = 0)
    {
        part[0] = p1;
        part[1] = p2;
        part[2] = p3;
        part[3] = p4;
    }

    inline bool operator <(const flag128 &right) const
    {
        for (uint8 i = 4; i > 0; --i)
        {
            if (part[i - 1] < right.part[i - 1])
                return true;
            else if (part[i - 1] > right.part[i - 1])
                return false;
        }
        return false;
    }

    inline bool operator ==(const flag128 &right) const
    {
        return
        (
            part[0] == right.part[0] &&
            part[1] == right.part[1] &&
            part[2] == right.part[2] &&
            part[3] == right.part[3]
        );
    }

    inline bool operator !=(const flag128 &right) const
    {
        return !this->operator ==(right);
    }

    inline flag128 operator &(const flag128 &right) const
    {
        return flag128(part[0] & right.part[0], part[1] & right.part[1],
            part[2] & right.part[2], part[3] & right.part[3]);
    }

    inline flag128 & operator &=(const flag128 &right)
    {
        part[0] &= right.part[0];
        part[1] &= right.part[1];
        part[2] &= right.part[2];
        part[3] &= right.part[3];
        return *this;
    }

    inline flag128 operator |(const flag128 &right) const
    {
        return flag128(part[0] | right.part[0], part[1] | right.part[1],
            part[2] | right.part[2], part[3] | right.part[3]);
    }

    inline flag128 & operator |=(const flag128 &right)
    {
        part[0] |= right.part[0];
        part[1] |= right.part[1];
        part[2] |= right.part[2];
        part[3] |= right.part[3];
        return *this;
    }

    inline flag128 operator ~() const
    {
        return flag128(~part[0], ~part[1], ~part[2], ~part[3]);
    }

    inline flag128 operator ^(const flag128 &right) const
    {
        return flag128(part[0] ^ right.part[0], part[1] ^ right.part[1],
            part[2] ^ right.part[2], part[3] ^ right.part[3]);
    }

    inline flag128 & operator ^=(const flag128 &right)
    {
        part[0] ^= right.part[0];
        part[1] ^= right.part[1];
        part[2] ^= right.part[2];
        part[3] ^= right.part[3];
        return *this;
    }

    inline operator bool() const
    {
        return (part[0] != 0 || part[1] != 0 || part[2] != 0 || part[3] != 0);
    }

    inline bool operator !() const
    {
        return !this->operator bool();
    }

    inline uint32 & operator [](uint8 el)
    {
        return part[el];
    }

    inline const uint32 & operator [](uint8 el) const
    {
        return part[el];
    }
};

class flag96
{
private:
    uint32 part[3];

public:
    flag96(uint32 p1 = 0, uint32 p2 = 0, uint32 p3 = 0)
    {
        part[0] = p1;
        part[1] = p2;
        part[2] = p3;
    }

    flag96(uint64 p1, uint32 p2)
    {
        part[0] = (uint32)(p1 & uint64_t(0x00000000FFFFFFFF));
        part[1] = (uint32)((p1 >> 32) & uint64_t(0x00000000FFFFFFFF));
        part[2] = p2;
    }

    inline bool IsEqual(uint32 p1 = 0, uint32 p2 = 0, uint32 p3 = 0) const
    {
        return (part[0] == p1 && part[1] == p2 && part[2] == p3);
    }

    inline bool HasFlag(uint32 p1 = 0, uint32 p2 = 0, uint32 p3 = 0) const
    {
        return (part[0] & p1 || part[1] & p2 || part[2] & p3);
    }

    inline void Set(uint32 p1 = 0, uint32 p2 = 0, uint32 p3 = 0)
    {
        part[0] = p1;
        part[1] = p2;
        part[2] = p3;
    }

    inline bool operator <(const flag96 &right) const
    {
        for (uint8 i = 3; i > 0; --i)
        {
            if (part[i - 1] < right.part[i - 1])
                return true;
            else if (part[i - 1] > right.part[i - 1])
                return false;
        }
        return false;
    }

    inline bool operator ==(const flag96 &right) const
    {
        return
        (
            part[0] == right.part[0] &&
            part[1] == right.part[1] &&
            part[2] == right.part[2]
        );
    }

    inline bool operator !=(const flag96 &right) const
    {
        return !this->operator ==(right);
    }

    inline flag96 operator &(const flag96 &right) const
    {
        return flag96(part[0] & right.part[0], part[1] & right.part[1],
            part[2] & right.part[2]);
    }

    inline flag96 & operator &=(const flag96 &right)
    {
        part[0] &= right.part[0];
        part[1] &= right.part[1];
        part[2] &= right.part[2];
        return *this;
    }

    inline flag96 operator |(const flag96 &right) const
    {
        return flag96(part[0] | right.part[0], part[1] | right.part[1],
            part[2] | right.part[2]);
    }

    inline flag96 & operator |=(const flag96 &right)
    {
        part[0] |= right.part[0];
        part[1] |= right.part[1];
        part[2] |= right.part[2];
        return *this;
    }

    inline flag96 operator ~() const
    {
        return flag96(~part[0], ~part[1], ~part[2]);
    }

    inline flag96 operator ^(const flag96 &right) const
    {
        return flag96(part[0] ^ right.part[0], part[1] ^ right.part[1],
            part[2] ^ right.part[2]);
    }

    inline flag96 & operator ^=(const flag96 &right)
    {
        part[0] ^= right.part[0];
        part[1] ^= right.part[1];
        part[2] ^= right.part[2];
        return *this;
    }

    inline operator bool() const
    {
        return (part[0] != 0 || part[1] != 0 || part[2] != 0);
    }

    inline bool operator !() const
    {
        return !this->operator bool();
    }

    inline uint32 & operator [](uint8 el)
    {
        return part[el];
    }

    inline const uint32 & operator [](uint8 el) const
    {
        return part[el];
    }
};

class flag64
{
public:
    flag64()
    {
        m_flags[0] = 0;
        m_flags[1] = 0;
    }

    flag64(uint32 l, uint32 h)
    {
        m_flags[0] = l;
        m_flags[1] = h;
    }

    uint32 operator[](uint32 index) const
    {
        ASSERT(index < m_flags.size());
        return m_flags[index];
    }

    uint32& operator[](uint32 index)
    {
        ASSERT(index < m_flags.size());
        return m_flags[index];
    }

    void operator |= (flag64 const& other)
    {
        m_flags[0] |= other.m_flags[0];
        m_flags[1] |= other.m_flags[1];
    }

    flag64 operator & (flag64 const& other) const
    {
        return flag64{ m_flags[0] & other.m_flags[0], m_flags[1] & other.m_flags[1] };
    }

    operator bool() const
    {
        return m_flags[0] != 0 || m_flags[1] != 0;
    }

protected:
    std::array<uint32, 2> m_flags;
};


enum ComparisionType
{
    COMP_TYPE_EQ = 0,
    COMP_TYPE_HIGH,
    COMP_TYPE_LOW,
    COMP_TYPE_HIGH_EQ,
    COMP_TYPE_LOW_EQ,
    COMP_TYPE_MAX
};

template <class T>
bool CompareValues(ComparisionType type, T val1, T val2)
{
    switch (type)
    {
        case COMP_TYPE_EQ:
            return val1 == val2;
        case COMP_TYPE_HIGH:
            return val1 > val2;
        case COMP_TYPE_LOW:
            return val1 < val2;
        case COMP_TYPE_HIGH_EQ:
            return val1 >= val2;
        case COMP_TYPE_LOW_EQ:
            return val1 <= val2;
        default:
            // incorrect parameter
            ASSERT(false);
            return false;
    }
}

template<typename E>
constexpr typename std::underlying_type<E>::type AsUnderlyingType(E enumValue)
{
    static_assert(std::is_enum<E>::value, "AsUnderlyingType can only be used with enums");
    return static_cast<typename std::underlying_type<E>::type>(enumValue);
}

template <class... Args>
inline std::string Format(std::string const& msg, Args... args)
{
    char buff[4096];
    int32 n = snprintf(buff, sizeof(buff), msg.c_str(), args...);
    if (n < 0 || n >= sizeof(buff))
        return "BAD FORMAT BUFFER OVERFLOW";
    return std::string(buff);
}

template <class... Args>
inline std::string Format(char const* msg, Args... args)
{
    char buff[4096];
    int32 n = snprintf(buff, sizeof(buff), msg, args...);
    if (n < 0 || n >= sizeof(buff))
        return "BAD FORMAT BUFFER OVERFLOW";
    return std::string(buff);
}

inline std::string& TrimString(std::string& str, char const* tok = " ")
{
    std::string::size_type trimPos;
    if ((trimPos = str.find_first_not_of(tok)) != 0)
        str.erase(0, trimPos);
    if ((trimPos = str.find_last_not_of(tok)) != str.size() - 1)
        str.erase(trimPos + 1);

    return str;
}

inline std::string TimeStr(time_t toConvert, char const* fmt)
{
    char buff[100];
    struct tm timeInfo;
    localtime_r(&toConvert, &timeInfo);
    std::strftime(buff, 100, fmt, &timeInfo);
    return std::string(buff);
}

class AnyValue
{
    enum Type
    {
        None,
        Int,
        UInt,
        Float,
    };
public:
    AnyValue()
        : _type(AnyValue::None)
    {
    }

    template <class Rep>
    Rep Get() const;

    template <class Rep>
    void Set(Rep val);

private:
    union
    {
        int64  Int;
        uint64 UInt;
        double Float;
    } _val;
    Type _type;
};

template <> inline void AnyValue::Set(int32 val)  { _type = AnyValue::Int;  _val.Int = val;   }
template <> inline void AnyValue::Set(int64 val)  { _type = AnyValue::Int;  _val.Int = val;   }
template <> inline void AnyValue::Set(uint32 val) { _type = AnyValue::UInt; _val.UInt = val;  }
template <> inline void AnyValue::Set(uint64 val) { _type = AnyValue::UInt; _val.UInt = val;  }
template <> inline void AnyValue::Set(float val)  { _type = AnyValue::Float;_val.Float = val; }
template <> inline void AnyValue::Set(double val) { _type = AnyValue::Float;_val.Float = val; }

template <> inline int32  AnyValue::Get<int32>()  const { if (_type == AnyValue::None) return 0;    ASSERT(_type == AnyValue::Int);   return _val.Int;   }
template <> inline int64  AnyValue::Get<int64>()  const { if (_type == AnyValue::None) return 0LL;  ASSERT(_type == AnyValue::Int);   return _val.Int;   }
template <> inline uint32 AnyValue::Get<uint32>() const { if (_type == AnyValue::None) return 0U;   ASSERT(_type == AnyValue::UInt);  return _val.UInt;  }
template <> inline uint64 AnyValue::Get<uint64>() const { if (_type == AnyValue::None) return 0ULL; ASSERT(_type == AnyValue::UInt);  return _val.UInt;  }
template <> inline float  AnyValue::Get<float>()  const { if (_type == AnyValue::None) return 0.0f; ASSERT(_type == AnyValue::Float); return _val.Float; }
template <> inline double AnyValue::Get<double>() const { if (_type == AnyValue::None) return 0.0f; ASSERT(_type == AnyValue::Float); return _val.Float; }

inline uint32 NumberOfSetBits(uint32 i)
{
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

#endif
