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

#include "WordFilterMgr.h"

bool NormalizeWord(std::string& utf8String)
{
    // remove spaces
    std::string::size_type n = 0;
    while ((n = utf8String.find(' ', n)) != utf8String.npos)
        utf8String.erase(n, 1);

    std::wstring utf16String;

    if (!Utf8toWStr(utf8String, utf16String))
        return false;

    std::transform(utf16String.begin(), utf16String.end(), utf16String.begin(), wcharToLower);

    return WStrToUtf8(utf16String, utf8String);
}

/*
// Find words by letters next to it.
bool FindWordInSequence(const std::string& str, const std::string& word, const size_t maxDistBethChars)
{
    size_t charsCount = 0, distBethCharsCount = 0;
    bool lastCharIsExist = false;

    for (size_t i = 0, lastI = 0; i < str.size(); ++i)
    {
        if (str[i] == word[charsCount])
        {
            if (!lastCharIsExist)
                lastI = i;

            ++charsCount;
            distBethCharsCount = 0;
            lastCharIsExist = true;
        }
        else if (lastCharIsExist)
            ++distBethCharsCount;

        if (distBethCharsCount > maxDistBethChars)
        {
            i = lastI; 
            charsCount = 0;
            distBethCharsCount = 0;
            lastCharIsExist = false;
        }

        if (charsCount >= word.size())
            return true;
    }

    return false;
}
*/

WordFilterMgr::WordFilterMgr() 
{
}

WordFilterMgr::~WordFilterMgr()
{
}

WordFilterMgr* WordFilterMgr::instance()
{
    static WordFilterMgr instance;
    return &instance;
}

void WordFilterMgr::LoadLetterAnalogs()
{
    uint32 oldMSTime = getMSTime();

    m_letterAnalogs.clear();

    QueryResult result = WorldDatabase.Query("SELECT letter, analogs FROM letter_analogs");
    if (!result)
    {
        TC_LOG_INFO("misc", ">> Loaded 0 letter analogs. DB table `letter_analogs` is empty!");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        char letter = fields[0].GetString()[0]; // !fields[0].GetInt8()
        std::string analogs = fields[1].GetString();

        NormalizeWord(analogs);
        m_letterAnalogs[letter] = analogs; 

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("misc", ">> Loaded {} letter analogs in {} ms", count, GetMSTimeDiffToNow(oldMSTime));
}


void WordFilterMgr::LoadBadWords()
{
    uint32 oldMSTime = getMSTime();

    m_badWords.clear();
    m_badWordsMail.clear();

    QueryResult result = WorldDatabase.Query("SELECT bad_word FROM bad_word");
    if (!result)
    {
        TC_LOG_INFO("misc", ">> Loaded 0 bad words. DB table `bad_word` is empty!");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        std::string analog = fields[0].GetString();

        AddBadWord(analog);

        ++count;
    }
    while (result->NextRow());

    result = WorldDatabase.Query("SELECT bad_word FROM bad_word_mail");
    if (!result)
    {
        TC_LOG_INFO("misc", ">> Loaded 0 bad words. DB table `bad_word_mail` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        std::string analog = fields[0].GetString();

        AddBadWordMail(analog);

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("misc", ">> Loaded {} bad words in {} ms", count, GetMSTimeDiffToNow(oldMSTime));
}

inline void WordFilterMgr::ConvertLettersToAnalogs(std::string& text)
{
    // todo: to change to STL algorithms (for optimize).
    for (std::string::iterator sit = text.begin(); sit != text.end(); ++sit)
        for (LetterAnalogMap::const_iterator mit = m_letterAnalogs.begin(); mit != m_letterAnalogs.end(); ++mit)
            if (mit->second.find(*sit) != std::string::npos)
            {
                *sit = mit->first;
                break;
            }
}

std::string WordFilterMgr::FindBadWord(const std::string& text, bool mail)
{
    std::string _text = text;
    
    if (_text.empty() || m_badWords.empty())
        return "";

    NormalizeWord(_text);
    ConvertLettersToAnalogs(_text);

    for (BadWordMap::const_iterator it = m_badWords.begin(); it != m_badWords.end(); ++it)
        if (_text.find(it->first) != std::string::npos)
            return it->second;

    if(mail)
    {
        for (BadWordMapMail::const_iterator it = m_badWordsMail.begin(); it != m_badWordsMail.end(); ++it)
            if (_text.find(it->first) != std::string::npos)
                return it->second;
    }
    /* 
    // At ~5 times slower.
    for (BadWordMap::const_iterator it = m_badWords.begin(); it != m_badWords.end(); ++it)
    {
        if (it->second.size() <= 3)
        {
            if (_text.find(it->first) != std::string::npos)
                return it->second;
        }
        else if (it->second.size() <= 7)
        {
            if(FindWordInSequence(_text, it->first, 1))
                return it->second;
        }
        else
        {
            if(FindWordInSequence(_text, it->first, 2))
                return it->second;
        }
    }
    */

    return "";
}

bool WordFilterMgr::AddBadWord(const std::string& badWord, bool toDB)
{
    std::string _badWord = badWord;
  
    NormalizeWord(_badWord);

    std::string convertedBadWord = _badWord;
    ConvertLettersToAnalogs(convertedBadWord);

    // is already exist
    if (m_badWords.find(convertedBadWord) != m_badWords.end())
        return false;

    m_badWords[convertedBadWord] = _badWord;

    if (toDB)
        WorldDatabase.PQuery("REPLACE INTO bad_word VALUES ('{}')", _badWord.c_str()); 

    return true;
}

bool WordFilterMgr::AddBadWordMail(const std::string& badWord, bool toDB)
{
    std::string _badWord = badWord;

    NormalizeWord(_badWord);

    std::string convertedBadWord = _badWord;
    ConvertLettersToAnalogs(convertedBadWord);

    // is already exist
    if (m_badWordsMail.find(convertedBadWord) != m_badWordsMail.end())
        return false;

    m_badWordsMail[convertedBadWord] = _badWord;

    if (toDB)
        WorldDatabase.PQuery("REPLACE INTO bad_word_mail VALUES ('{}')", _badWord.c_str());

    return true;
}

bool WordFilterMgr::RemoveBadWord(const std::string& badWord, bool fromDB)
{
    std::string _badWord = badWord;
    
    NormalizeWord(_badWord);

    std::string convertedBadWord = _badWord;
    ConvertLettersToAnalogs(convertedBadWord);

    // is not exist
    BadWordMap::iterator it = m_badWords.find(convertedBadWord);
    if (it == m_badWords.end())
        return false;

    m_badWords.erase(it);

    if (fromDB)
        WorldDatabase.PExecute("DELETE FROM bad_word WHERE `bad_word` = '{}'", _badWord.c_str()); 

    return true;
}
