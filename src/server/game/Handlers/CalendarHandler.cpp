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

/*
----- Opcodes Not Used yet -----

SMSG_CALENDAR_EVENT_INVITE_NOTES        [ packguid(Invitee), uint64(inviteId), string(Text), Boolean(Unk) ]
?CMSG_CALENDAR_EVENT_INVITE_NOTES       [ uint32(unk1), uint32(unk2), uint32(unk3), uint32(unk4), uint32(unk5) ]
SMSG_CALENDAR_EVENT_INVITE_NOTES_ALERT  [ uint64(inviteId), string(Text) ]
SMSG_CALENDAR_RAID_LOCKOUT_UPDATED      SendCalendarRaidLockoutUpdated(InstanceSave const* save)

@todo

Finish complains' handling - what to do with received complains and how to respond?
Find out what to do with all "not used yet" opcodes
Correct errors sending (event/invite not found, invites exceeded, event already passed, permissions etc.)
Fix locked events to be displayed properly and response time shouldn't be shown for people that haven't respond yet
Copied events should probably have a new owner

*/

#define SEND_SERVERSIDE_HOLIDAYS

#include "InstanceSaveMgr.h"
#include "Log.h"
#include "Opcodes.h"
#include "Player.h"
#include "SocialMgr.h"
#include "CalendarMgr.h"
#include "ObjectMgr.h"
#include "ObjectAccessor.h"
#include "DatabaseEnv.h"
#include "GuildMgr.h"
#include "WorldSession.h"
#include "GameEventMgr.h"
#include "Util.h"

void WorldSession::HandleCalendarGetCalendar(WorldPacket& /*recvData*/)
{
    ObjectGuid playerGuid = _player->GetGUID();
    TC_LOG_DEBUG("network", "CMSG_CALENDAR_GET_CALENDAR [" UI64FMTD "]", playerGuid.GetRawValue());

    time_t currTime = time(NULL);
    uint32 counter = 0;
    uint32 holidayCount = 0;
    CalendarInviteStore invites = sCalendarMgr->GetPlayerInvites(playerGuid);
    CalendarEventStore playerEvents = sCalendarMgr->GetPlayerEvents(playerGuid);
    ResetTimeByMapDifficultyMap const& resets = sInstanceSaveMgr->GetResetTimeMap();

    ByteBuffer lockoutInfoBuffer;
    ByteBuffer invitesInfoBuffer;
    ByteBuffer eventsInfoBuffer;
    ByteBuffer holidaysInfoBuffer;
    WorldPacket data(SMSG_CALENDAR_SEND_CALENDAR, 1000); // Average size if no instance
    size_t resetPos = data.bitwpos();
    data.WriteBits(0, 20); // Reset placeholder
    size_t hollidayPos = data.bitwpos();
    data.WriteBits(0, 16); // Hollidays placeholder
    size_t lockoutPos = data.bitwpos();
    data.WriteBits(0, 20); // Lockout placeholder

    for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
    {
        Player::BoundInstancesMap boundInstances = _player->GetBoundInstances(Difficulty(i));
        for (auto&& itr : boundInstances)
        {
            if (itr.second.perm)
            {
                InstanceSave const* save = itr.second.save;
                ObjectGuid guid(uint64(save->GetInstanceId()));
                data.WriteBit(guid[6]);
                data.WriteBit(guid[7]);
                data.WriteBit(guid[2]);
                data.WriteBit(guid[1]);
                data.WriteBit(guid[5]);
                data.WriteBit(guid[4]);
                data.WriteBit(guid[0]);
                data.WriteBit(guid[3]);

                lockoutInfoBuffer << uint32(save->GetDifficulty());
                lockoutInfoBuffer.WriteByteSeq(guid[3]);
                lockoutInfoBuffer.WriteByteSeq(guid[0]);
                lockoutInfoBuffer.WriteByteSeq(guid[1]);
                lockoutInfoBuffer.WriteByteSeq(guid[5]);
                lockoutInfoBuffer << uint32(save->GetResetTime() - currTime);
                lockoutInfoBuffer << uint32(save->GetMapId());
                lockoutInfoBuffer.WriteByteSeq(guid[2]);
                lockoutInfoBuffer.WriteByteSeq(guid[7]);
                lockoutInfoBuffer.WriteByteSeq(guid[6]);
                lockoutInfoBuffer.WriteByteSeq(guid[4]);

                ++counter;
            }
        }
    }

#ifdef SEND_SERVERSIDE_HOLIDAYS
    for (auto&& event : sGameEventMgr->GetEventMap())
    {
        if (!event.holiday_id)
            continue;

        HolidaysEntry const* holiday = sHolidaysStore.LookupEntry(event.holiday_id);
        if (!holiday)
            continue;

        bool original = holiday->CalendarFilterType == 0 || // Fishing (Weekly)
                        holiday->CalendarFilterType == 2;   // Call to Arms

        std::string holidayTexture = holiday->TextureFilename;

        std::function<void(uint32, ByteBuffer*)> appendDateIndex = [holiday, event, original](uint32 dateIndex, ByteBuffer* holidaysInfoBuffer)
        {
            if (dateIndex > 10) // we get broken packed time when index more than 10, idk why
                *holidaysInfoBuffer << uint32(0);
            else if (holiday->CalendarFilterType == 0 && !dateIndex) // Custom date for the first Fishing event occurrence
            {
                tm lt = tm();
                localtime_r(&event.start, &lt);
                *holidaysInfoBuffer << uint32(holiday->Date[dateIndex] & ~0x7FF | ((lt.tm_wday & 0x7) << 11) | ((lt.tm_hour & 0x1F) << 6) | (lt.tm_min & 0x3F)); // HH:mm and weekday custom, every other number is -1
            }
            else if (original)
                *holidaysInfoBuffer << uint32(holiday->Date[dateIndex]);
            else
                holidaysInfoBuffer->AppendPackedTime(event.start + event.occurence * MINUTE * dateIndex);
        };

        data.WriteBits(holidayTexture.size(), 6);

        appendDateIndex(7, &holidaysInfoBuffer);
        appendDateIndex(0, &holidaysInfoBuffer);
        holidaysInfoBuffer << uint32(original ? holiday->CalendarFlags[8] : 3);
        appendDateIndex(12, &holidaysInfoBuffer);
        appendDateIndex(16, &holidaysInfoBuffer);
        appendDateIndex(21, &holidaysInfoBuffer);
        appendDateIndex(14, &holidaysInfoBuffer);
        appendDateIndex(8, &holidaysInfoBuffer);
        holidaysInfoBuffer << uint32(holiday->CalendarFilterType);
        appendDateIndex(24, &holidaysInfoBuffer);
        holidaysInfoBuffer << uint32(original ? holiday->Duration[7] : 0);
        appendDateIndex(13, &holidaysInfoBuffer);
        holidaysInfoBuffer << uint32(original ? holiday->Duration[2] : 0);
        appendDateIndex(10, &holidaysInfoBuffer);
        holidaysInfoBuffer << uint32(original ? holiday->Duration[6] : 0);
        appendDateIndex(6, &holidaysInfoBuffer);
        appendDateIndex(23, &holidaysInfoBuffer);
        holidaysInfoBuffer << uint32(holiday->Priority);
        appendDateIndex(19, &holidaysInfoBuffer);
        holidaysInfoBuffer << uint32(original ? holiday->CalendarFlags[9] : 3);
        holidaysInfoBuffer << uint32(original ? holiday->Duration[8] : 0);
        appendDateIndex(9, &holidaysInfoBuffer);
        holidaysInfoBuffer << uint32(original ? holiday->Duration[1] : 0);
        holidaysInfoBuffer << uint32(original ? holiday->CalendarFlags[6] : 3);
        holidaysInfoBuffer << uint32(original ? holiday->CalendarFlags[1] : 3);
        appendDateIndex(4, &holidaysInfoBuffer);
        holidaysInfoBuffer << uint32(original ? holiday->Duration[0] : (event.length / MINUTE));
        appendDateIndex(20, &holidaysInfoBuffer);
        appendDateIndex(18, &holidaysInfoBuffer);
        appendDateIndex(2, &holidaysInfoBuffer);
        appendDateIndex(25, &holidaysInfoBuffer);
        holidaysInfoBuffer << uint32(original ? holiday->CalendarFlags[7] : 3);
        holidaysInfoBuffer << uint32(holiday->Id);
        holidaysInfoBuffer << uint32(original ? holiday->Duration[4] : 0);
        appendDateIndex(22, &holidaysInfoBuffer);
        holidaysInfoBuffer.WriteString(holidayTexture);
        appendDateIndex(1, &holidaysInfoBuffer);
        holidaysInfoBuffer << uint32(original ? holiday->CalendarFlags[2] : 3);
        holidaysInfoBuffer << uint32(original ? holiday->Duration[9] : 0);
        appendDateIndex(3, &holidaysInfoBuffer);
        appendDateIndex(15, &holidaysInfoBuffer);
        holidaysInfoBuffer << uint32(original ? holiday->CalendarFlags[3] : 3);
        holidaysInfoBuffer << uint32(original ? holiday->Duration[3] : 0);
        holidaysInfoBuffer << uint32(original ? holiday->CalendarFlags[4] : 3);
        appendDateIndex(5, &holidaysInfoBuffer);
        appendDateIndex(17, &holidaysInfoBuffer);
        holidaysInfoBuffer << uint32(original ? holiday->CalendarFlags[5] : 3);
        holidaysInfoBuffer << uint32(holiday->Region);
        holidaysInfoBuffer << uint32(original ? holiday->CalendarFlags[0] : 3);
        appendDateIndex(11, &holidaysInfoBuffer);
        holidaysInfoBuffer << uint32(holiday->Looping);
        holidaysInfoBuffer << uint32(original ? holiday->Duration[5] : 0);

        holidayCount++;
    }
#else
    for (auto&& event : sGameEventMgr->GetEventMap())
    {
        if (!event.holiday_id)
            continue;

        HolidaysEntry const* holiday = sHolidaysStore.LookupEntry(event.holiday_id);
        if (!holiday)
            continue;

        bool need_update = false;

        // ugly hack for compatibility with new dates
        switch (holiday->Id)
        {
            case 181: // Noblegarden
                const_cast<HolidaysEntry*>(holiday)->Date[10] = 288623232; // 17/04/2017 Mon (1)
                const_cast<HolidaysEntry*>(holiday)->Date[11] = 305154688; // 02/04/2018 Mon (1)
                const_cast<HolidaysEntry*>(holiday)->Date[12] = 322259584; // 22/04/2019 Mon (1)
                need_update = true;
                break;
            case 201: // Childrens Week
                const_cast<HolidaysEntry*>(holiday)->Date[12] = 272648832; // 02/05/2016 Mon (1)
                const_cast<HolidaysEntry*>(holiday)->Date[13] = 289409664; // 01/05/2017 Mon (1)
                const_cast<HolidaysEntry*>(holiday)->Date[14] = 305613440; // 30/04/2018 Mon (1)
                const_cast<HolidaysEntry*>(holiday)->Date[15] = 322374272; // 29/04/2019 Mon (1)
                need_update = true;
                break;
            case 321: // Harvest Festival
                const_cast<HolidaysEntry*>(holiday)->Date[14] = 327268992; // 07/092019 Sat (6)
                need_update = true;
                break;
            case 327: // Lunar Festival
                const_cast<HolidaysEntry*>(holiday)->Date[11] = 285555328; // 21/01/2017 Sat (6)
                const_cast<HolidaysEntry*>(holiday)->Date[12] = 303315584; // 17/02/2018 Sat (6)
                const_cast<HolidaysEntry*>(holiday)->Date[13] = 319212160; // 28/01/2019 Mon (1)
                need_update = true;
                break;
            case 404: // Pilgrim's Bounty
                const_cast<HolidaysEntry*>(holiday)->Date[8] = 296012416; // 20/11/2017 Mon (1)
                const_cast<HolidaysEntry*>(holiday)->Date[9] = 312773248; // 18/09/2018 Mon (1)
                need_update = true;
                break;
            case 423: // Love In The Air
                const_cast<HolidaysEntry*>(holiday)->Date[7] = 286364288; // 07/02/2017 Tue (2)
                const_cast<HolidaysEntry*>(holiday)->Date[8] = 303067776; // 02/02/2018 Fri (5)
                const_cast<HolidaysEntry*>(holiday)->Date[9] = 319885952; // 05/02/2019 Tue (2)
                need_update = true;
                break;
            case 479: // Darkmoon Faire
                // all dates shifted backwards on 3 days because fuck you
                const_cast<HolidaysEntry*>(holiday)->Date[0] = 309374976;  // 05/08/2018 Sun (7)
                const_cast<HolidaysEntry*>(holiday)->Date[1] = 309833728;  // 02/09/2018 Sun (7)
                const_cast<HolidaysEntry*>(holiday)->Date[2] = 311504896;  // 07/10/2018 Sun (7)
                const_cast<HolidaysEntry*>(holiday)->Date[3] = 312504320;  // 04/11/2018 Sun (7)
                const_cast<HolidaysEntry*>(holiday)->Date[4] = 312963072;  // 02/12/2018 Sun (7)
                const_cast<HolidaysEntry*>(holiday)->Date[5] = 318828544;  // 06/01/2019 Sun (7)
                const_cast<HolidaysEntry*>(holiday)->Date[6] = 319827968;  // 03/02/2019 Sun (7)
                const_cast<HolidaysEntry*>(holiday)->Date[7] = 320876544;  // 03/03/2019 Sun (7)
                const_cast<HolidaysEntry*>(holiday)->Date[8] = 321990656;  // 07/04/2019 Sun (7)
                const_cast<HolidaysEntry*>(holiday)->Date[9] = 323006464;  // 05/05/2019 Sun (7)
                const_cast<HolidaysEntry*>(holiday)->Date[10] = 323465216; // 02/06/2019 Sun (7)
                need_update = true;
                break;
        }

        if (!need_update)
            continue;

        std::string holidayTexture = holiday->TextureFilename[LOCALE_enUS];

        data.WriteBits(holidayTexture.size(), 6);

        holidaysInfoBuffer << uint32(holiday->Date[7]);
        holidaysInfoBuffer << uint32(holiday->Date[0]);
        holidaysInfoBuffer << uint32(holiday->CalendarFlags[8]);
        holidaysInfoBuffer << uint32(holiday->Date[12]);
        holidaysInfoBuffer << uint32(holiday->Date[16]);
        holidaysInfoBuffer << uint32(holiday->Date[21]);
        holidaysInfoBuffer << uint32(holiday->Date[14]);
        holidaysInfoBuffer << uint32(holiday->Date[8]);
        holidaysInfoBuffer << uint32(holiday->CalendarFilterType);
        holidaysInfoBuffer << uint32(holiday->Date[24]);
        holidaysInfoBuffer << uint32(holiday->Duration[7]);
        holidaysInfoBuffer << uint32(holiday->Date[13]);
        holidaysInfoBuffer << uint32(holiday->Duration[2]);
        holidaysInfoBuffer << uint32(holiday->Date[10]);
        holidaysInfoBuffer << uint32(holiday->Duration[6]);
        holidaysInfoBuffer << uint32(holiday->Date[6]);
        holidaysInfoBuffer << uint32(holiday->Date[23]);
        holidaysInfoBuffer << uint32(holiday->Priority);
        holidaysInfoBuffer << uint32(holiday->Date[19]);
        holidaysInfoBuffer << uint32(holiday->CalendarFlags[9]);
        holidaysInfoBuffer << uint32(holiday->Duration[8]);
        holidaysInfoBuffer << uint32(holiday->Date[9]);
        holidaysInfoBuffer << uint32(holiday->Duration[1]);
        holidaysInfoBuffer << uint32(holiday->CalendarFlags[6]);
        holidaysInfoBuffer << uint32(holiday->CalendarFlags[1]);
        holidaysInfoBuffer << uint32(holiday->Date[4]);
        holidaysInfoBuffer << uint32(holiday->Duration[0]);
        holidaysInfoBuffer << uint32(holiday->Date[20]);
        holidaysInfoBuffer << uint32(holiday->Date[18]);
        holidaysInfoBuffer << uint32(holiday->Date[2]);
        holidaysInfoBuffer << uint32(holiday->Date[25]);
        holidaysInfoBuffer << uint32(holiday->CalendarFlags[7]);
        holidaysInfoBuffer << uint32(holiday->Id);
        holidaysInfoBuffer << uint32(holiday->Duration[4]);
        holidaysInfoBuffer << uint32(holiday->Date[22]);
        holidaysInfoBuffer.WriteString(holidayTexture);
        holidaysInfoBuffer << uint32(holiday->Date[1]);
        holidaysInfoBuffer << uint32(holiday->CalendarFlags[2]);
        holidaysInfoBuffer << uint32(holiday->Duration[9]);
        holidaysInfoBuffer << uint32(holiday->Date[3]);
        holidaysInfoBuffer << uint32(holiday->Date[15]);
        holidaysInfoBuffer << uint32(holiday->CalendarFlags[3]);
        holidaysInfoBuffer << uint32(holiday->Duration[3]);
        holidaysInfoBuffer << uint32(holiday->CalendarFlags[4]);
        holidaysInfoBuffer << uint32(holiday->Date[5]);
        holidaysInfoBuffer << uint32(holiday->Date[17]);
        holidaysInfoBuffer << uint32(holiday->CalendarFlags[5]);
        holidaysInfoBuffer << uint32(holiday->Region);
        holidaysInfoBuffer << uint32(holiday->CalendarFlags[0]);
        holidaysInfoBuffer << uint32(holiday->Date[11]);
        holidaysInfoBuffer << uint32(holiday->Looping);
        holidaysInfoBuffer << uint32(holiday->Duration[5]);

        holidayCount++;
    }
#endif

    data.WriteBits(invites.size(), 19);
    for (auto&& calendarInvite : invites)
    {
        CalendarEvent* calendarEvent = sCalendarMgr->GetEvent(calendarInvite->GetEventId());
        ObjectGuid guid = calendarInvite->GetSenderGUID();
        data.WriteBit(guid[1]);
        data.WriteBit(guid[2]);
        data.WriteBit(guid[6]);
        data.WriteBit(guid[7]);
        data.WriteBit(guid[3]);
        data.WriteBit(guid[0]);
        data.WriteBit(guid[4]);
        data.WriteBit(guid[5]);

        invitesInfoBuffer.WriteByteSeq(guid[2]);
        invitesInfoBuffer << uint64(calendarInvite->GetInviteId());
        invitesInfoBuffer << uint8(calendarInvite->GetStatus());
        invitesInfoBuffer.WriteByteSeq(guid[6]);
        invitesInfoBuffer.WriteByteSeq(guid[3]);
        invitesInfoBuffer.WriteByteSeq(guid[4]);
        invitesInfoBuffer.WriteByteSeq(guid[1]);
        invitesInfoBuffer.WriteByteSeq(guid[0]);
        invitesInfoBuffer << uint64(calendarInvite->GetEventId());
        invitesInfoBuffer.WriteByteSeq(guid[7]);
        invitesInfoBuffer.WriteByteSeq(guid[5]);

        invitesInfoBuffer << uint8(calendarInvite->GetRank());
        invitesInfoBuffer << uint8((calendarEvent && calendarEvent->IsGuildEvent() && calendarEvent->GetGuildId() == _player->GetGuildId()) ? 1 : 0);
    }

    data.WriteBits(playerEvents.size(), 19);
    for (auto&& calendarEvent : playerEvents)
    {
        Guild* guild = sGuildMgr->GetGuildById(calendarEvent->GetGuildId());
        ObjectGuid guildGuid = guild ? guild->GetGUID() : ObjectGuid::Empty;
        ObjectGuid creatorGuid = calendarEvent->GetCreatorGUID();

        data.WriteBit(creatorGuid[2]);
        data.WriteBit(guildGuid[1]);
        data.WriteBit(guildGuid[7]);
        data.WriteBit(creatorGuid[4]);
        data.WriteBit(guildGuid[5]);
        data.WriteBit(guildGuid[6]);
        data.WriteBit(guildGuid[3]);
        data.WriteBit(guildGuid[4]);
        data.WriteBit(creatorGuid[7]);
        data.WriteBits(calendarEvent->GetTitle().size(), 8);
        data.WriteBit(creatorGuid[1]);
        data.WriteBit(guildGuid[2]);
        data.WriteBit(guildGuid[0]);
        data.WriteBit(creatorGuid[0]);
        data.WriteBit(creatorGuid[3]);
        data.WriteBit(creatorGuid[6]);
        data.WriteBit(creatorGuid[5]);

        eventsInfoBuffer.WriteByteSeq(creatorGuid[5]);
        eventsInfoBuffer.WriteByteSeq(guildGuid[3]);
        eventsInfoBuffer.WriteString(calendarEvent->GetTitle());
        eventsInfoBuffer.WriteByteSeq(guildGuid[7]);
        eventsInfoBuffer << int32(calendarEvent->GetDungeonId());
        eventsInfoBuffer.WriteByteSeq(creatorGuid[0]);
        eventsInfoBuffer.WriteByteSeq(creatorGuid[4]);
        eventsInfoBuffer.WriteByteSeq(guildGuid[2]);
        eventsInfoBuffer.WriteByteSeq(creatorGuid[7]);
        eventsInfoBuffer.WriteByteSeq(creatorGuid[2]);
        eventsInfoBuffer.AppendPackedTime(calendarEvent->GetEventTime());
        eventsInfoBuffer.WriteByteSeq(creatorGuid[3]);
        eventsInfoBuffer.WriteByteSeq(creatorGuid[1]);
        eventsInfoBuffer.WriteByteSeq(guildGuid[6]);
        eventsInfoBuffer.WriteByteSeq(guildGuid[1]);
        eventsInfoBuffer.WriteByteSeq(creatorGuid[6]);
        eventsInfoBuffer << uint32(calendarEvent->GetFlags());
        eventsInfoBuffer.WriteByteSeq(guildGuid[4]);
        eventsInfoBuffer.WriteByteSeq(guildGuid[5]);
        eventsInfoBuffer.WriteByteSeq(guildGuid[0]);
        eventsInfoBuffer << uint64(calendarEvent->GetEventId());
        eventsInfoBuffer << uint8(calendarEvent->GetType());
    }

    data.FlushBits();
    data.PutBits(lockoutPos, counter, 20);
    data.PutBits(hollidayPos, holidayCount, 16);
    data.append(eventsInfoBuffer);
    data.append(lockoutInfoBuffer);
    data.append(holidaysInfoBuffer);
    data.append(invitesInfoBuffer);

    data.AppendPackedTime(currTime);                       // zone time

    counter = 0;
    std::set<uint32> sentMaps;
    for (auto&& itr : resets)
    {
        uint32 mapId = PAIR32_LOPART(itr.first);
        if (sentMaps.find(mapId) != sentMaps.end())
            continue;

        MapEntry const* mapEntry = sMapStore.LookupEntry(mapId);
        if (!mapEntry || !mapEntry->IsRaid())
            continue;

        sentMaps.insert(mapId);

        data << int32(mapId);
        data << int32(itr.second - currTime);
        data << int32(0); // offset => found it different only once
        counter++;
    }

    data.PutBits(resetPos, counter, 20);
    data << uint32(1135753200); // Constant date, unk (28.12.2005 07:00)
    data << uint32(currTime);   // server time

    SendPacket(&data);
}

void WorldSession::HandleCalendarGetEvent(WorldPacket& recvData)
{
    uint64 eventId = recvData.read<uint64>();

    TC_LOG_DEBUG("network", "CMSG_CALENDAR_GET_EVENT. Player ["
        UI64FMTD "] Event [" UI64FMTD "]", _player->GetGUID().GetRawValue(), eventId);

    if (CalendarEvent* calendarEvent = sCalendarMgr->GetEvent(eventId))
        sCalendarMgr->SendCalendarEvent(_player->GetGUID(), *calendarEvent, CALENDAR_SENDTYPE_GET);
    else
        sCalendarMgr->SendCalendarCommandResult(_player->GetGUID(), CALENDAR_ERROR_EVENT_INVALID);
}

void WorldSession::HandleCalendarGuildFilter(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_CALENDAR_GUILD_FILTER [" UI64FMTD "]", _player->GetGUID().GetRawValue());

    uint8 maxLevel = recvData.read<uint8>();
    uint8 minRank = recvData.read<uint8>();
    uint8 minLevel = recvData.read<uint8>();

    if (Guild* guild = sGuildMgr->GetGuildById(_player->GetGuildId()))
        guild->MassInviteToEvent(this, minLevel, maxLevel, minRank);

    TC_LOG_DEBUG("network", "CMSG_CALENDAR_GUILD_FILTER: Min level [%d], Max level [%d], Min rank [%d]", minLevel, maxLevel, minRank);
}

void WorldSession::HandleCalendarAddEvent(WorldPacket& recvData)
{
    ObjectGuid guid = _player->GetGUID();

    uint32 maxInvites = recvData.read<uint32>();  // always 100, necesary? Not find the way how to change it
    uint32 flags = recvData.read<uint32>();
    int32 dungeonId = recvData.read<int32>();
    uint32 eventPackedTime = recvData.ReadPackedTime();

    // prevent events in the past
    // To Do: properly handle timezones and remove the "- time_t(86400L)" hack
    if (time_t(eventPackedTime) < (time(NULL) - time_t(86400L)))
    {
        recvData.rfinish();
        return;
    }

    uint8 type = recvData.read<uint8>();
    uint32 inviteeCount = recvData.ReadBits(22);
    uint16 descriptionLength = recvData.ReadBits(11);

    std::list<CalendarInvitePacketInfo> calendarInviteList;
    for (uint32 i = 0; i < inviteeCount; i++)
    {
        CalendarInvitePacketInfo info;
        info.Guid[7] = recvData.ReadBit();
        info.Guid[2] = recvData.ReadBit();
        info.Guid[6] = recvData.ReadBit();
        info.Guid[3] = recvData.ReadBit();
        info.Guid[5] = recvData.ReadBit();
        info.Guid[1] = recvData.ReadBit();
        info.Guid[0] = recvData.ReadBit();
        info.Guid[4] = recvData.ReadBit();
        calendarInviteList.push_back(info);
    }

    uint16 titleLength = recvData.ReadBits(8);

    for (auto&& itr : calendarInviteList)
    {
        recvData.ReadByteSeq(itr.Guid[4]);
        recvData.ReadByteSeq(itr.Guid[2]);
        recvData.ReadByteSeq(itr.Guid[3]);
        recvData.ReadByteSeq(itr.Guid[1]);
        recvData.ReadByteSeq(itr.Guid[0]);
        recvData.ReadByteSeq(itr.Guid[6]);
        recvData.ReadByteSeq(itr.Guid[7]);
        recvData >> itr.Status;
        recvData.ReadByteSeq(itr.Guid[5]);
        recvData >> itr.ModerationRank;
    }

    std::string title = recvData.ReadString(titleLength);
    std::string description = recvData.ReadString(descriptionLength);

    CalendarEvent* calendarEvent = new CalendarEvent(sCalendarMgr->GetFreeEventId(), guid, 0, CalendarEventType(type), dungeonId,
        time_t(eventPackedTime), flags, title, description);

    if (calendarEvent->IsGuildEvent() || calendarEvent->IsGuildAnnouncement())
        if (Player* creator = ObjectAccessor::FindPlayer(guid))
            calendarEvent->SetGuildId(creator->GetGuildId());

    if (calendarEvent->IsGuildAnnouncement())
    {
        CalendarInvite* invite = new CalendarInvite(0, calendarEvent->GetEventId(), 0, guid, 946684800, CALENDAR_STATUS_NOT_SIGNED_UP, CALENDAR_RANK_PLAYER, "");
        sCalendarMgr->AddInvite(calendarEvent, invite);
    }
    else
    {
        for (auto&& itr : calendarInviteList)
        {
            CalendarInvite* invite = new CalendarInvite(sCalendarMgr->GetFreeInviteId(), calendarEvent->GetEventId(), guid, guid, 946684800, CalendarInviteStatus(itr.Status), CalendarModerationRank(itr.ModerationRank), "");
            sCalendarMgr->AddInvite(calendarEvent, invite);
        }
    }

    sCalendarMgr->AddEvent(calendarEvent, CALENDAR_SENDTYPE_ADD);
}

void WorldSession::HandleCalendarUpdateEvent(WorldPacket& recvData)
{
    ObjectGuid guid = _player->GetGUID();
    time_t oldEventTime;

    ObjectGuid eventId, inviteId;
    uint32 maxInvites = recvData.read<uint32>();
    int32 dungeonId = recvData.read<int32>();
    uint32 eventPackedTime = recvData.ReadPackedTime();

    // prevent events in the past
    // To Do: properly handle timezones and remove the "- time_t(86400L)" hack
    if (time_t(eventPackedTime) < (time(NULL) - time_t(86400L)))
    {
        recvData.rfinish();
        return;
    }

    uint32 flags = recvData.read<uint32>();
    uint8 type = recvData.read<uint8>();
    eventId[4] = recvData.ReadBit();
    eventId[5] = recvData.ReadBit();
    eventId[2] = recvData.ReadBit();
    inviteId[4] = recvData.ReadBit();
    eventId[7] = recvData.ReadBit();
    eventId[0] = recvData.ReadBit();
    inviteId[5] = recvData.ReadBit();
    inviteId[3] = recvData.ReadBit();
    eventId[6] = recvData.ReadBit();
    eventId[1] = recvData.ReadBit();
    inviteId[6] = recvData.ReadBit();
    inviteId[2] = recvData.ReadBit();
    inviteId[7] = recvData.ReadBit();
    inviteId[1] = recvData.ReadBit();
    inviteId[0] = recvData.ReadBit();
    uint16 descriptionLength = recvData.ReadBits(11);
    uint16 titleLength = recvData.ReadBits(8);
    eventId[3] = recvData.ReadBit();


    recvData.ReadByteSeq(inviteId[6]);
    recvData.ReadByteSeq(eventId[0]);
    recvData.ReadByteSeq(inviteId[7]);
    recvData.ReadByteSeq(inviteId[3]);
    recvData.ReadByteSeq(eventId[6]);
    recvData.ReadByteSeq(inviteId[1]);
    recvData.ReadByteSeq(eventId[2]);
    std::string title = recvData.ReadString(titleLength);
    recvData.ReadByteSeq(inviteId[5]);
    recvData.ReadByteSeq(inviteId[4]);
    recvData.ReadByteSeq(eventId[5]);
    recvData.ReadByteSeq(eventId[3]);
    recvData.ReadByteSeq(inviteId[0]);
    recvData.ReadByteSeq(eventId[4]);
    std::string description = recvData.ReadString(descriptionLength);
    recvData.ReadByteSeq(eventId[1]);
    recvData.ReadByteSeq(inviteId[2]);
    recvData.ReadByteSeq(eventId[7]);

    TC_LOG_DEBUG("network", "CMSG_CALENDAR_UPDATE_EVENT [" UI64FMTD "] EventId [" UI64FMTD
        "], InviteId [" UI64FMTD "] Title %s, Description %s, type %u "
        "MaxInvites %u, Dungeon ID %d, Time %u, Flags %u",
        guid.GetRawValue(), (uint64)eventId, (uint64)inviteId, title.c_str(),
        description.c_str(), type, maxInvites, dungeonId,
        eventPackedTime, flags);

    if (CalendarEvent* calendarEvent = sCalendarMgr->GetEvent(eventId))
    {
        oldEventTime = calendarEvent->GetEventTime();

        calendarEvent->SetType(CalendarEventType(type));
        calendarEvent->SetFlags(flags);
        calendarEvent->SetEventTime(time_t(eventPackedTime));
        calendarEvent->SetDungeonId(dungeonId);
        calendarEvent->SetTitle(title);
        calendarEvent->SetDescription(description);

        sCalendarMgr->UpdateEvent(calendarEvent);
        sCalendarMgr->SendCalendarEventUpdateAlert(*calendarEvent, oldEventTime);
    }
    else
        sCalendarMgr->SendCalendarCommandResult(guid, CALENDAR_ERROR_EVENT_INVALID);
}

void WorldSession::HandleCalendarRemoveEvent(WorldPacket& recvData)
{
    ObjectGuid guid = _player->GetGUID();

    recvData.read_skip<uint64>(); // Skip InviteId
    uint64 eventId = recvData.read<uint64>();
    recvData.read_skip<uint32>(); // Skip flags

    sCalendarMgr->RemoveEvent(eventId, guid);
}

void WorldSession::HandleCalendarCopyEvent(WorldPacket& recvData)
{
    ObjectGuid guid = _player->GetGUID();

    uint64 eventId = recvData.read<uint64>();
    uint64 inviteId = recvData.read<uint64>();
    uint32 time = recvData.ReadPackedTime();

    TC_LOG_DEBUG("network", "CMSG_CALENDAR_COPY_EVENT [" UI64FMTD "], EventId [" UI64FMTD
        "] inviteId [" UI64FMTD "] Time: %u", guid.GetRawValue(), eventId, inviteId, time);

    if (CalendarEvent* oldEvent = sCalendarMgr->GetEvent(eventId))
    {
        CalendarEvent* newEvent = new CalendarEvent(*oldEvent, sCalendarMgr->GetFreeEventId(), guid);
        newEvent->SetEventTime(time_t(time));
        CalendarInviteStore invites = sCalendarMgr->GetEventInvites(eventId);

        for (auto&& itr : invites)
            sCalendarMgr->AddInvite(newEvent, new CalendarInvite(*itr, sCalendarMgr->GetFreeInviteId(), newEvent->GetEventId(), guid), false);

        sCalendarMgr->AddEvent(newEvent, CALENDAR_SENDTYPE_COPY);
    }
    else
        sCalendarMgr->SendCalendarCommandResult(guid, CALENDAR_ERROR_EVENT_INVALID);
}

void WorldSession::HandleCalendarEventInvite(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_CALENDAR_EVENT_INVITE");

    ObjectGuid playerGuid = _player->GetGUID();

    ObjectGuid inviteeGuid = ObjectGuid::Empty;
    uint32 inviteeTeam = 0;
    uint32 inviteeGuildId = 0;

    uint64 eventId = recvData.read<uint64>();
    uint64 inviteId = recvData.read<uint64>();
    bool isPreInvite = recvData.ReadBit();
    uint16 length = recvData.ReadBits(9);
    bool isGuildEvent = recvData.ReadBit();
    std::string name = recvData.ReadString(length);

    Player* player = ObjectAccessor::FindPlayerByName(name.c_str());
    if (player)
    {
        // Invitee is online
        inviteeGuid = player->GetGUID();
        inviteeTeam = player->GetTeam();
        inviteeGuildId = player->GetGuildId();
    }
    else
    {
        // Invitee offline, get data from database
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUID_RACE_ACC_BY_NAME);
        stmt->setString(0, name);
        if (PreparedQueryResult result = CharacterDatabase.Query(stmt))
        {
            Field* fields = result->Fetch();
            inviteeGuid = ObjectGuid(HighGuid::Player, fields[0].GetUInt32());
            inviteeTeam = Player::TeamForRace(fields[1].GetUInt8());
            inviteeGuildId = Player::GetGuildIdFromDB(inviteeGuid);
        }
    }

    if (!inviteeGuid)
    {
        sCalendarMgr->SendCalendarCommandResult(playerGuid, CALENDAR_ERROR_PLAYER_NOT_FOUND);
        return;
    }

    if (_player->GetTeam() != inviteeTeam && !sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CALENDAR))
    {
        sCalendarMgr->SendCalendarCommandResult(playerGuid, CALENDAR_ERROR_NOT_ALLIED);
        return;
    }

    if (QueryResult result = CharacterDatabase.PQuery("SELECT flags FROM character_social WHERE guid = " UI64FMTD " AND friend = " UI64FMTD, inviteeGuid, playerGuid))
    {
        Field* fields = result->Fetch();
        if (fields[0].GetUInt8() & SOCIAL_FLAG_IGNORED)
        {
            sCalendarMgr->SendCalendarCommandResult(playerGuid, CALENDAR_ERROR_IGNORING_YOU_S, name.c_str());
            return;
        }
    }

    if (!isPreInvite)
    {
        if (CalendarEvent* calendarEvent = sCalendarMgr->GetEvent(eventId))
        {
            if (calendarEvent->GetEventTime() < time(NULL))
            {
                sCalendarMgr->SendCalendarCommandResult(playerGuid, CALENDAR_ERROR_EVENT_PASSED);
                return;
            }

            // we probably can invite them, but we have to send them some different info about that invite
            if (calendarEvent->IsGuildEvent() && calendarEvent->GetGuildId() == inviteeGuildId)
            {
                sCalendarMgr->SendCalendarCommandResult(playerGuid, CALENDAR_ERROR_NO_GUILD_INVITES);
                return;
            }

            CalendarInvite* invite = new CalendarInvite(sCalendarMgr->GetFreeInviteId(), eventId, inviteeGuid, playerGuid, 0, CALENDAR_STATUS_INVITED, CALENDAR_RANK_PLAYER, "");
            sCalendarMgr->AddInvite(calendarEvent, invite, true, player && player->GetGuildId() != calendarEvent->GetGuildId());
        }
        else
            sCalendarMgr->SendCalendarCommandResult(playerGuid, CALENDAR_ERROR_EVENT_INVALID);
    }
    else
    {
        // we probably can invite them, but we have to send them some different info about that invite
        if (isGuildEvent && _player->GetGuildId() == inviteeGuildId)
        {
            sCalendarMgr->SendCalendarCommandResult(playerGuid, CALENDAR_ERROR_NO_GUILD_INVITES);
            return;
        }

        CalendarInvite invite(0, 0, inviteeGuid, playerGuid, 0, CALENDAR_STATUS_INVITED, CALENDAR_RANK_PLAYER, "");
        sCalendarMgr->SendCalendarEventInvite(invite);
    }
}

void WorldSession::HandleCalendarEventSignup(WorldPacket& recvData)
{
    ObjectGuid guid = _player->GetGUID();

    uint64 eventId = recvData.read<uint64>();
    bool tentative = recvData.ReadBit();

    TC_LOG_DEBUG("network", "CMSG_CALENDAR_EVENT_SIGNUP [" UI64FMTD "] EventId [" UI64FMTD "] Tentative %u", guid.GetRawValue(), eventId, tentative);

    if (CalendarEvent* calendarEvent = sCalendarMgr->GetEvent(eventId))
    {
        if (calendarEvent->IsGuildEvent() && calendarEvent->GetGuildId() != _player->GetGuildId())
        {
            sCalendarMgr->SendCalendarCommandResult(guid, CALENDAR_ERROR_GUILD_PLAYER_NOT_IN_GUILD);
            return;
        }

        CalendarInviteStatus status = tentative ? CALENDAR_STATUS_TENTATIVE : CALENDAR_STATUS_SIGNED_UP;
        CalendarInvite* invite = new CalendarInvite(sCalendarMgr->GetFreeInviteId(), eventId, guid, guid, time(NULL), status, CALENDAR_RANK_PLAYER, "");
        sCalendarMgr->AddInvite(calendarEvent, invite);
        sCalendarMgr->SendCalendarClearPendingAction(guid);
    }
    else
        sCalendarMgr->SendCalendarCommandResult(guid, CALENDAR_ERROR_EVENT_INVALID);
}

void WorldSession::HandleCalendarEventRsvp(WorldPacket& recvData)
{
    ObjectGuid guid = _player->GetGUID();

    uint64 eventId = recvData.read<uint64>();
    uint64 inviteId = recvData.read<uint64>();
    uint8 status = recvData.read<uint8>();

    TC_LOG_DEBUG("network", "CMSG_CALENDAR_EVENT_RSVP [" UI64FMTD "] EventId ["
        UI64FMTD "], InviteId [" UI64FMTD "], status %u", guid.GetRawValue(), eventId,
        inviteId, status);

    if (CalendarEvent* calendarEvent = sCalendarMgr->GetEvent(eventId))
    {
        // i think we still should be able to remove self from locked events
        if (status != CALENDAR_STATUS_REMOVED && calendarEvent->GetFlags() & CALENDAR_FLAG_INVITES_LOCKED)
        {
            sCalendarMgr->SendCalendarCommandResult(guid, CALENDAR_ERROR_EVENT_LOCKED);
            return;
        }

        if (CalendarInvite* invite = sCalendarMgr->GetInvite(inviteId))
        {
            invite->SetStatus(CalendarInviteStatus(status));
            invite->SetStatusTime(time(NULL));

            sCalendarMgr->UpdateInvite(invite);
            sCalendarMgr->SendCalendarEventStatus(*calendarEvent, *invite);
            sCalendarMgr->SendCalendarEventStatusAlert(*calendarEvent, *invite);
            sCalendarMgr->SendCalendarClearPendingAction(guid);
        }
        else
            sCalendarMgr->SendCalendarCommandResult(guid, CALENDAR_ERROR_NO_INVITE); // correct?
    }
    else
        sCalendarMgr->SendCalendarCommandResult(guid, CALENDAR_ERROR_EVENT_INVALID);
}

void WorldSession::HandleCalendarEventRemoveInvite(WorldPacket& recvData)
{
    ObjectGuid guid = _player->GetGUID();

    ObjectGuid invitee;
    uint64 inviteId = recvData.read<uint64>();
    uint64 senderId = recvData.read<uint64>();
    uint64 eventId = recvData.read<uint64>();
    invitee[6] = recvData.ReadBit();
    invitee[3] = recvData.ReadBit();
    invitee[2] = recvData.ReadBit();
    invitee[4] = recvData.ReadBit();
    invitee[5] = recvData.ReadBit();
    invitee[7] = recvData.ReadBit();
    invitee[0] = recvData.ReadBit();
    invitee[1] = recvData.ReadBit();
    recvData.ReadByteSeq(invitee[0]);
    recvData.ReadByteSeq(invitee[4]);
    recvData.ReadByteSeq(invitee[7]);
    recvData.ReadByteSeq(invitee[3]);
    recvData.ReadByteSeq(invitee[5]);
    recvData.ReadByteSeq(invitee[1]);
    recvData.ReadByteSeq(invitee[2]);
    recvData.ReadByteSeq(invitee[6]);

    TC_LOG_DEBUG("network", "CMSG_CALENDAR_EVENT_REMOVE_INVITE ["
        UI64FMTD "] EventId [" UI64FMTD "], senderId ["
        UI64FMTD "], Invitee ([" UI64FMTD "] id: [" UI64FMTD "])",
        guid.GetRawValue(), eventId, senderId, (uint64)invitee, inviteId);

    if (CalendarEvent* calendarEvent = sCalendarMgr->GetEvent(eventId))
    {
        if (calendarEvent->GetCreatorGUID() == invitee)
        {
            sCalendarMgr->SendCalendarCommandResult(guid, CALENDAR_ERROR_DELETE_CREATOR_FAILED);
            return;
        }

        sCalendarMgr->RemoveInvite(inviteId, eventId, guid);
    }
    else
        sCalendarMgr->SendCalendarCommandResult(guid, CALENDAR_ERROR_NO_INVITE);
}

void WorldSession::HandleCalendarEventStatus(WorldPacket& recvData)
{
    ObjectGuid guid = _player->GetGUID();

    ObjectGuid invitee;
    uint64 senderId = recvData.read<uint64>();
    uint64 eventId = recvData.read<uint64>();
    uint64 inviteId = recvData.read<uint64>();
    uint8 status = recvData.read<uint8>();
    invitee[4] = recvData.ReadBit();
    invitee[3] = recvData.ReadBit();
    invitee[7] = recvData.ReadBit();
    invitee[6] = recvData.ReadBit();
    invitee[2] = recvData.ReadBit();
    invitee[0] = recvData.ReadBit();
    invitee[5] = recvData.ReadBit();
    invitee[1] = recvData.ReadBit();
    recvData.ReadByteSeq(invitee[7]);
    recvData.ReadByteSeq(invitee[5]);
    recvData.ReadByteSeq(invitee[2]);
    recvData.ReadByteSeq(invitee[1]);
    recvData.ReadByteSeq(invitee[4]);
    recvData.ReadByteSeq(invitee[6]);
    recvData.ReadByteSeq(invitee[0]);
    recvData.ReadByteSeq(invitee[3]);

    TC_LOG_DEBUG("network", "CMSG_CALENDAR_EVENT_STATUS [" UI64FMTD"] EventId ["
        UI64FMTD "] senderId [" UI64FMTD "], Invitee ([" UI64FMTD "] id: ["
        UI64FMTD "], status %u", guid.GetRawValue(), eventId, senderId, (uint64)invitee, inviteId, status);

    if (CalendarEvent* calendarEvent = sCalendarMgr->GetEvent(eventId))
    {
        if (CalendarInvite* invite = sCalendarMgr->GetInvite(inviteId))
        {
            invite->SetStatus((CalendarInviteStatus)status);

            sCalendarMgr->UpdateInvite(invite);
            sCalendarMgr->SendCalendarEventStatus(*calendarEvent, *invite);
            sCalendarMgr->SendCalendarEventStatusAlert(*calendarEvent, *invite);
            sCalendarMgr->SendCalendarClearPendingAction(invitee);
        }
        else
            sCalendarMgr->SendCalendarCommandResult(guid, CALENDAR_ERROR_NO_INVITE); // correct?
    }
    else
        sCalendarMgr->SendCalendarCommandResult(guid, CALENDAR_ERROR_EVENT_INVALID);
}

void WorldSession::HandleCalendarEventModeratorStatus(WorldPacket& recvData)
{
    ObjectGuid guid = _player->GetGUID();

    ObjectGuid invitee;
    uint8 rank = recvData.read<uint8>();
    uint64 eventId = recvData.read<uint64>();
    uint64 senderId = recvData.read<uint64>();
    uint64 inviteId = recvData.read<uint64>();
    invitee[6] = recvData.ReadBit();
    invitee[5] = recvData.ReadBit();
    invitee[1] = recvData.ReadBit();
    invitee[3] = recvData.ReadBit();
    invitee[4] = recvData.ReadBit();
    invitee[7] = recvData.ReadBit();
    invitee[0] = recvData.ReadBit();
    invitee[2] = recvData.ReadBit();
    recvData.ReadByteSeq(invitee[7]);
    recvData.ReadByteSeq(invitee[5]);
    recvData.ReadByteSeq(invitee[0]);
    recvData.ReadByteSeq(invitee[4]);
    recvData.ReadByteSeq(invitee[1]);
    recvData.ReadByteSeq(invitee[3]);
    recvData.ReadByteSeq(invitee[2]);
    recvData.ReadByteSeq(invitee[6]);

    TC_LOG_DEBUG("network", "CMSG_CALENDAR_EVENT_MODERATOR_STATUS [" UI64FMTD "] EventId ["
        UI64FMTD "] senderId [" UI64FMTD "], Invitee ([" UI64FMTD "] id: ["
        UI64FMTD "], rank %u", guid.GetRawValue(), eventId, senderId, (uint64)invitee, inviteId, rank);

    if (CalendarEvent* calendarEvent = sCalendarMgr->GetEvent(eventId))
    {
        if (calendarEvent->IsGuildEvent())
        {
            sCalendarMgr->SendCalendarCommandResult(guid, CALENDAR_ERROR_NO_MODERATOR);
            return;
        }

        if (CalendarInvite* invite = sCalendarMgr->GetInvite(inviteId))
        {
            invite->SetRank(CalendarModerationRank(rank));
            sCalendarMgr->UpdateInvite(invite);
            sCalendarMgr->SendCalendarEventModeratorStatus(*calendarEvent, *invite);
        }
        else
            sCalendarMgr->SendCalendarCommandResult(guid, CALENDAR_ERROR_NO_INVITE); // correct?
    }
    else
        sCalendarMgr->SendCalendarCommandResult(guid, CALENDAR_ERROR_EVENT_INVALID);
}

void WorldSession::HandleCalendarComplain(WorldPacket& recvData)
{
    uint64 guid = _player->GetGUID();

    ObjectGuid complainGUID;
    uint64 inviteId = recvData.read<uint64>();
    uint64 eventId = recvData.read<uint64>();
    complainGUID[4] = recvData.ReadBit();
    complainGUID[6] = recvData.ReadBit();
    complainGUID[2] = recvData.ReadBit();
    complainGUID[7] = recvData.ReadBit();
    complainGUID[1] = recvData.ReadBit();
    complainGUID[5] = recvData.ReadBit();
    complainGUID[3] = recvData.ReadBit();
    complainGUID[0] = recvData.ReadBit();
    recvData.ReadByteSeq(complainGUID[6]);
    recvData.ReadByteSeq(complainGUID[7]);
    recvData.ReadByteSeq(complainGUID[1]);
    recvData.ReadByteSeq(complainGUID[0]);
    recvData.ReadByteSeq(complainGUID[4]);
    recvData.ReadByteSeq(complainGUID[2]);
    recvData.ReadByteSeq(complainGUID[3]);
    recvData.ReadByteSeq(complainGUID[5]);

    TC_LOG_DEBUG("network", "CMSG_CALENDAR_COMPLAIN [" UI64FMTD "] EventId ["
        UI64FMTD "] guid [" UI64FMTD "] InviteId [" UI64FMTD "]", guid, eventId, (uint64)complainGUID, inviteId);

    // what to do with complains?
}

void WorldSession::HandleCalendarGetNumPending(WorldPacket& /*recvData*/)
{
    ObjectGuid guid = _player->GetGUID();
    uint32 pending = sCalendarMgr->GetPlayerNumPending(guid);

    TC_LOG_DEBUG("network", "CMSG_CALENDAR_GET_NUM_PENDING: [" UI64FMTD
        "] Pending: %u", guid.GetRawValue(), pending);

    WorldPacket data(SMSG_CALENDAR_SEND_NUM_PENDING, 4);
    data << uint32(pending);
    SendPacket(&data);
}

void WorldSession::HandleSetSavedInstanceExtend(WorldPacket& recvData)
{
    uint32 mapId = recvData.read<uint32>();
    uint32 difficulty = recvData.read<uint32>();
    bool toggleExtend = recvData.ReadBit();

    TC_LOG_DEBUG("network", "CMSG_SET_SAVED_INSTANCE_EXTEND - MapId: %u, Difficulty: %u, ToggleExtend: %s", mapId, difficulty, toggleExtend ? "On" : "Off");

    /*
    InstancePlayerBind* instanceBind = _player->GetBoundInstance(mapId, Difficulty(difficulty));
    if (!instanceBind || !instanceBind->save)
        return;

    InstanceSave* save = instanceBind->save;
    // http://www.wowwiki.com/Instance_Lock_Extension
    // SendCalendarRaidLockoutUpdated(save);
    */
}

// ----------------------------------- SEND ------------------------------------

void WorldSession::SendCalendarRaidLockout(InstanceSave const* save, bool add)
{
    TC_LOG_DEBUG("network", "%s", add ? "SMSG_CALENDAR_RAID_LOCKOUT_ADDED" : "SMSG_CALENDAR_RAID_LOCKOUT_REMOVED");
    time_t currTime = time(NULL);

    ObjectGuid guid(uint64(save->GetInstanceId()));
    if (add)
    {
        WorldPacket data(SMSG_CALENDAR_RAID_LOCKOUT_ADDED, 4 + 4 + 4 + 4 + 8);

        data.WriteBit(guid[3]);
        data.WriteBit(guid[1]);
        data.WriteBit(guid[2]);
        data.WriteBit(guid[0]);
        data.WriteBit(guid[4]);
        data.WriteBit(guid[7]);
        data.WriteBit(guid[5]);
        data.WriteBit(guid[6]);
        data.WriteByteSeq(guid[6]);
        data << uint32(save->GetDifficulty());
        data.WriteByteSeq(guid[0]);
        data.WriteByteSeq(guid[4]);
        data.WriteByteSeq(guid[5]);
        data.WriteByteSeq(guid[3]);
        data.WriteByteSeq(guid[2]);
        data << uint32(save->GetMapId());
        data.WriteByteSeq(guid[1]);
        data.AppendPackedTime(currTime);
        data << uint32(save->GetResetTime() - currTime);
        data.WriteByteSeq(guid[7]);

        SendPacket(&data);
    }
    else
    {
        WorldPacket data(SMSG_CALENDAR_RAID_LOCKOUT_REMOVED, 4 + 4 + 8);

        data << uint32(save->GetDifficulty());
        data << uint32(save->GetMapId());
        data.WriteBit(guid[2]);
        data.WriteBit(guid[0]);
        data.WriteBit(guid[4]);
        data.WriteBit(guid[6]);
        data.WriteBit(guid[5]);
        data.WriteBit(guid[7]);
        data.WriteBit(guid[3]);
        data.WriteBit(guid[1]);
        data.WriteByteSeq(guid[6]);
        data.WriteByteSeq(guid[1]);
        data.WriteByteSeq(guid[7]);
        data.WriteByteSeq(guid[3]);
        data.WriteByteSeq(guid[4]);
        data.WriteByteSeq(guid[5]);
        data.WriteByteSeq(guid[0]);
        data.WriteByteSeq(guid[2]);

        SendPacket(&data);
    }
}

void WorldSession::SendCalendarRaidLockoutUpdated(InstanceSave const* save)
{
    if (!save)
        return;

    ObjectGuid guid = _player->GetGUID();
    TC_LOG_DEBUG("network", "SMSG_CALENDAR_RAID_LOCKOUT_UPDATED [" UI64FMTD
        "] Map: %u, Difficulty %u", guid.GetRawValue(), save->GetMapId(), save->GetDifficulty());

    time_t currTime = time(NULL);

    WorldPacket data(SMSG_CALENDAR_RAID_LOCKOUT_UPDATED, 4 + 4 + 4 + 4 + 8);
    data << uint32(save->GetResetTime() - currTime); // New remaining time? => found only same values as the possible old time
    data << uint32(save->GetMapId());
    data << uint32(save->GetDifficulty());
    data.AppendPackedTime(currTime);
    data << uint32(save->GetResetTime() - currTime); // Old remaining time? => found only same values as the possible new time
    SendPacket(&data);
}
