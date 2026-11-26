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

#ifndef CREATUREAIIMPL_H
#define CREATUREAIIMPL_H

#include "Common.h"
#include "Define.h"
#include "Duration.h"
#include "TemporarySummon.h"
#include "CreatureAI.h"
#include "SpellMgr.h"
#include <initializer_list>
#include <map>
#include <vector>

template<class T>
inline
const T& RAND(const T& v1, const T& v2)
{
    return (urand(0, 1)) ? v1 : v2;
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3)
{
    switch (urand(0, 2))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4)
{
    switch (urand(0, 3))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5)
{
    switch (urand(0, 4))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6)
{
    switch (urand(0, 5))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7)
{
    switch (urand(0, 6))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8)
{
    switch (urand(0, 7))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9)
{
    switch (urand(0, 8))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10)
{
    switch (urand(0, 9))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11)
{
    switch (urand(0, 10))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11, const T& v12)
{
    switch (urand(0, 11))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
        case 11: return v12;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11, const T& v12, const T& v13)
{
    switch (urand(0, 12))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
        case 11: return v12;
        case 12: return v13;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11, const T& v12, const T& v13, const T& v14)
{
    switch (urand(0, 13))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
        case 11: return v12;
        case 12: return v13;
        case 13: return v14;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11, const T& v12, const T& v13, const T& v14, const T& v15)
{
    switch (urand(0, 14))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
        case 11: return v12;
        case 12: return v13;
        case 13: return v14;
        case 14: return v15;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11, const T& v12, const T& v13, const T& v14, const T& v15, const T& v16)
{
    switch (urand(0, 15))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
        case 11: return v12;
        case 12: return v13;
        case 13: return v14;
        case 14: return v15;
        case 15: return v16;
    }
}

class EventMap
{
    /**
    * Internal storage type.
    * Key: Time as uint32 when the event should occur.
    * Value: The event data as uint32.
    *
    * Structure of event data:
    * - Bit  0 - 15: Event Id.
    * - Bit 16 - 23: Group
    * - Bit 24 - 31: Phase
    * - Pattern: 0xPPGGEEEE
    */
    typedef std::multimap<uint32, uint32> EventStore;
    typedef std::map<uint32, std::vector<Milliseconds>> EventSeriesStore;

    public:
        using EventId = uint32;
        using GroupIndex = uint8;
        using GroupMask = uint8;
        using PhaseIndex = uint8;
        using PhaseMask = uint8;

        EventMap() : _time(0), _phase(0), _lastEvent(0) { }

        /**
        * @name Reset
        * @brief Removes all scheduled events and resets time and phase.
        */
        void Reset()
        {
            _eventMap.clear();
            _timerSeries.clear();
            _time = 0;
            _phase = 0;
        }

        /**
         * @name Update
         * @brief Updates the timer of the event map.
         * @param time Value in ms to be added to time.
         */
        void Update(uint32 time)
        {
            _time += time;
        }

        /**
        * @name Update
        * @brief Updates the timer of the event map.
        * @param time Value as std::chrono::duration to be added to time.
        */
        void Update(Milliseconds const& time)
        {
            Update(time.count());
        }

        /**
        * @name GetTimer
        * @return Current timer in ms value.
        */
        uint32 GetTimer() const
        {
            return _time;
        }

        /**
        * @name GetPhaseMask
        * @return Active phases as mask.
        */
        uint8 GetPhaseMask() const
        {
            return _phase;
        }

        /**
        * @name Empty
        * @return True, if there are no events scheduled.
        */
        bool Empty() const
        {
            return _eventMap.empty();
        }

        /**
        * @name SetPhase
        * @brief Sets the phase of the map (absolute).
        * @param phase Phase which should be set. Values: 1 - 8. 0 resets phase.
        */
        void SetPhase(uint8 phase)
        {
            if (!phase)
                _phase = 0;
            else if (phase <= 8)
                _phase = (1 << (phase - 1));
        }

        /**
        * @name AddPhase
        * @brief Activates the given phase (bitwise).
        * @param phase Phase which should be activated. Values: 1 - 8
        */
        void AddPhase(uint8 phase)
        {
            if (phase && phase <= 8)
                _phase |= (1 << (phase - 1));
        }

        /**
        * @name RemovePhase
        * @brief Deactivates the given phase (bitwise).
        * @param phase Phase which should be deactivated. Values: 1 - 8.
        */
        void RemovePhase(uint8 phase)
        {
            if (phase && phase <= 8)
                _phase &= ~(1 << (phase - 1));
        }

        /**
        * @name ScheduleEvent
        * @brief Creates new event entry in map.
        * @param eventId The id of the new event.
        * @param time The time in milliseconds as std::chrono::duration until the event occurs.
        * @param group The group which the event is associated to. Has to be between 1 and 8. 0 means it has no group.
        * @param phase The phase in which the event can occur. Has to be between 1 and 8. 0 means it can occur in all phases.
        */
        void ScheduleEvent(uint32 eventId, Milliseconds const& time, uint32 group = 0, uint8 phase = 0)
        {
            ScheduleEvent(eventId, time.count(), group, phase);
        }

        /**
        * @name ScheduleEvent
        * @brief Creates new event entry in map.
        * @param eventId The id of the new event.
        * @param time The time in milliseconds until the event occurs.
        * @param group The group which the event is associated to. Has to be between 1 and 8. 0 means it has no group.
        * @param phase The phase in which the event can occur. Has to be between 1 and 8. 0 means it can occur in all phases.
        */
        void ScheduleEvent(uint32 eventId, uint32 time, uint32 group = 0, uint8 phase = 0)
        {
            if (group && group <= 8)
                eventId |= (1 << (group + 15));

            if (phase && phase <= 8)
                eventId |= (1 << (phase + 23));

            _eventMap.insert(EventStore::value_type(_time + time, eventId));
        }

        /**
        * @name ScheduleEvent
        * @brief Schedules a new event. An existing event is not canceled.
        * @param eventId The id of the new event.
        * @param minTime The minimum time until the event occurs as std::chrono type.
        * @param maxTime The maximum time until the event occurs as std::chrono type.
        * @param group The group which the event is associated to. Has to be between 1 and 8. 0 means it has no group.
        * @param phase The phase in which the event can occur. Has to be between 1 and 8. 0 means it can occur in all phases.
        */
        void ScheduleEvent(uint32 eventId, Milliseconds minTime, Milliseconds maxTime, uint32 group = 0, uint32 phase = 0)
        {
            ScheduleEvent(eventId, randtime(minTime, maxTime), group, phase);
        }

        /**
        * @name RescheduleEvent
        * @brief Cancels the given event and reschedules it.
        * @param eventId The id of the event.
        * @param time The time in milliseconds as std::chrono::duration until the event occurs.
        * @param group The group which the event is associated to. Has to be between 1 and 8. 0 means it has no group.
        * @param phase The phase in which the event can occur. Has to be between 1 and 8. 0 means it can occur in all phases.
        */
        void RescheduleEvent(uint32 eventId, Milliseconds const& time, uint32 group = 0, uint8 phase = 0)
        {
            RescheduleEvent(eventId, time.count(), group, phase);
        }

        /**
        * @name RescheduleEvent
        * @brief Cancels the given event and reschedules it.
        * @param eventId The id of the event.
        * @param minTime The minimum time until the event occurs as std::chrono type.
        * @param maxTime The maximum time until the event occurs as std::chrono type.
        * @param group The group which the event is associated to. Has to be between 1 and 8. 0 means it has no group.
        * @param phase The phase in which the event can occur. Has to be between 1 and 8. 0 means it can occur in all phases.
        */
        void RescheduleEvent(uint32 eventId, Milliseconds minTime, Milliseconds maxTime, uint32 group = 0, uint32 phase = 0)
        {
            RescheduleEvent(eventId, randtime(minTime, maxTime), group, phase);
        }

        /**
        * @name RescheduleEvent
        * @brief Cancels the given event and reschedules it.
        * @param eventId The id of the event.
        * @param time The time in milliseconds until the event occurs.
        * @param group The group which the event is associated to. Has to be between 1 and 8. 0 means it has no group.
        * @param phase The phase in which the event can occur. Has to be between 1 and 8. 0 means it can occur in all phases.
        */
        void RescheduleEvent(uint32 eventId, uint32 time, uint32 group = 0, uint8 phase = 0)
        {
            CancelEvent(eventId);
            ScheduleEvent(eventId, time, group, phase);
        }

        /**
        * @name RepeatEvent
        * @brief Repeats the mostly recently executed event.
        * @param time Time until in milliseconds as std::chrono::duration the event occurs.
        */
        void RepeatEvent(Milliseconds const& time)
        {
            RepeatEvent(time.count());
        }

        /**
        * @name RepeatEvent
        * @brief Cancels the closest event and reschedules it.
        * @param time Time until the event occurs.
        */
        void RepeatEvent(uint32 time)
        {
            if (Empty())
                return;

            uint32 eventId = _eventMap.begin()->second;
            _eventMap.erase(_eventMap.begin());
            ScheduleEvent(eventId, time);
        }

        /**
        * @name Repeat
        * @brief Repeats the most recently executed event.
        * @param time Time until in milliseconds as std::chrono::duration the event occurs.
        */
        void Repeat(Milliseconds const& time)
        {
            _eventMap.insert(EventStore::value_type(_time + time.count(), _lastEvent));
        }

        /**
        * @name RepeatEvent
        * @brief Repeats the most recently executed event.
        * @param minTime The minimum time until the event occurs as std::chrono type.
        * @param maxTime The maximum time until the event occurs as std::chrono type.
        */
        void Repeat(Milliseconds minTime, Milliseconds maxTime)
        {
            Repeat(randtime(minTime, maxTime));
        }

        /**
        * @name PopEvent
        * @brief Remove the first event in the map.
        */
        void PopEvent()
        {
            if (!Empty())
                _eventMap.erase(_eventMap.begin());
        }

        /**
        * @name ExecuteEvent
        * @brief Returns the next event to execute and removes it from map.
        * @return Id of the event to execute.
        */
        uint32 ExecuteEvent()
        {
            while (!Empty())
            {
                EventStore::iterator itr = _eventMap.begin();

                if (itr->first > _time)
                    return 0;
                else if (_phase && (itr->second & 0xFF000000) && !((itr->second >> 24) & _phase))
                    _eventMap.erase(itr);
                else
                {
                    uint32 eventId = (itr->second & 0x0000FFFF);
                    uint32 eventData = itr->second;
                    _lastEvent = eventData;
                    _eventMap.erase(itr);
                    ScheduleNextFromSeries(eventData);
                    return eventId;
                }
            }

            return 0;
        }

        /**
        * @name GetEvent
        * @brief Returns the next event to execute.
        * @return Id of the event to execute.
        */
        uint32 GetEvent()
        {
            while (!Empty())
            {
                EventStore::iterator itr = _eventMap.begin();

                if (itr->first > _time)
                    return 0;
                else if (_phase && (itr->second & 0xFF000000) && !(itr->second & (_phase << 24)))
                    _eventMap.erase(itr);
                else
                    return (itr->second & 0x0000FFFF);
            }

            return 0;
        }

        /**
        * @name DelayEvents
        * @brief Delays all events in the map. If delay is greater than or equal internal timer, delay will be 0.
        * @param delay Amount of delay in ms as std::chrono::duration.
        */
        void DelayEvents(Milliseconds const& delay)
        {
            DelayEvents(delay.count());
        }

        /**
        * @name DelayEvents
        * @brief Delays all events in the map. If delay is greater than or equal internal timer, delay will be 0.
        * @param delay Amount of delay.
        */
        void DelayEvents(uint32 delay)
        {
            _time = delay < _time ? _time - delay : 0;
        }

        /**
        * @name DelayEvents
        * @brief Delay all events of the same group.
        * @param delay Amount of delay in ms as std::chrono::duration.
        * @param group Group of the events.
        */
        void DelayEvents(Milliseconds const& delay, uint32 group)
        {
            DelayEvents(delay.count(), group);
        }

        /**
        * @name DelayEvents
        * @brief Delay all events of the same group.
        * @param delay Amount of delay.
        * @param group Group of the events.
        */
        void DelayEvents(uint32 delay, uint32 group)
        {
            if (!group || group > 8 || Empty())
                return;

            EventStore delayed;

            for (EventStore::iterator itr = _eventMap.begin(); itr != _eventMap.end();)
            {
                if (itr->second & (1 << (group + 15)))
                {
                    delayed.insert(EventStore::value_type(itr->first + delay, itr->second));
                    _eventMap.erase(itr++);
                }
                else
                    ++itr;
            }

            _eventMap.insert(delayed.begin(), delayed.end());
        }

        /**
        * @name SetMinimalDelay
        * @brief Increase event delay if smaller than given delay.
        * @param eventId The id of the event.
        * @param delay Minimum delay for given event.
        */
        void SetMinimalDelay(EventId eventId, Milliseconds const& delay)
        {
            if (Empty())
                return;

            uint32 time = _time + delay.count();

            for (EventStore::iterator itr = _eventMap.begin(); itr != _eventMap.end();)
            {
                if (eventId == (itr->second & 0x0000FFFF))
                {
                    if (itr->first < time)
                    {
                        _eventMap.insert(EventStore::value_type(time, itr->second));
                        itr = _eventMap.erase(itr);
                        continue;
                    }
                }

                ++itr;
            }
        }

        /**
        * @name CancelEvent
        * @brief Cancels all events of the specified id.
        * @param eventId Event id to cancel.
        */
        void CancelEvent(uint32 eventId)
        {
            if (Empty())
                return;

            for (EventStore::iterator itr = _eventMap.begin(); itr != _eventMap.end();)
            {
                if (eventId == (itr->second & 0x0000FFFF))
                    _eventMap.erase(itr++);
                else
                    ++itr;
            }

            for (EventSeriesStore::iterator itr = _timerSeries.begin(); itr != _timerSeries.end();)
            {
                if (eventId == (itr->first & 0x0000FFFF))
                    _timerSeries.erase(itr++);
                else
                    ++itr;
            }
        }

        /**
        * @name CancelEventGroup
        * @brief Cancel events belonging to specified group.
        * @param group Group to cancel.
        */
        void CancelEventGroup(uint32 group)
        {
            if (!group || group > 8 || Empty())
                return;

            for (EventStore::iterator itr = _eventMap.begin(); itr != _eventMap.end();)
            {
                if (itr->second & (1 << (group + 15)))
                    _eventMap.erase(itr++);
                else
                    ++itr;
            }

            for (EventSeriesStore::iterator itr = _timerSeries.begin(); itr != _timerSeries.end();)
            {
                if (itr->first & (1 << (group + 15)))
                    _timerSeries.erase(itr++);
                else
                    ++itr;
            }
        }

        /**
        * @name GetNextEventTime
        * @brief Returns closest occurence of specified event.
        * @param eventId Wanted event id.
        * @return Time of found event.
        */
        uint32 GetNextEventTime(uint32 eventId) const
        {
            if (Empty())
                return 0;

            for (EventStore::const_iterator itr = _eventMap.begin(); itr != _eventMap.end(); ++itr)
                if (eventId == (itr->second & 0x0000FFFF))
                    return itr->first;

            return 0;
        }

        /**
         * @name GetNextEventTime
         * @return Time of next event.
         */
        uint32 GetNextEventTime() const
        {
            return Empty() ? 0 : _eventMap.begin()->first;
        }

        /**
        * @name IsInPhase
        * @brief Returns wether event map is in specified phase or not.
        * @param phase Wanted phase.
        * @return True, if phase of event map contains specified phase.
        */
        bool IsInPhase(uint8 phase)
        {
            return phase <= 8 && (!phase || _phase & (1 << (phase - 1)));
        }

        /**
        * @name GetTimeUntilEvent
        * @brief Returns time in milliseconds until next event.
        * @param Id of the event.
        * @return Time of next event.
        */
        Milliseconds GetTimeUntilEvent(uint32 eventId) const
        {
            for (EventStore::const_iterator itr = _eventMap.begin(); itr != _eventMap.end(); ++itr)
                if (eventId == (itr->second & 0x0000FFFF))
                    return Milliseconds(int64(itr->first) - int64(_time));

            return Milliseconds::max();
        }

        uint32 IsScheduled(uint32 eventId) const
        {
            for (EventStore::const_iterator itr = _eventMap.begin(); itr != _eventMap.end(); ++itr)
                if (eventId == (itr->second & 0x0000FFFF))
                    return true;

            return false;
        }

        /**
        * @name HasEventScheduled
        * @brief Returns whether an event is scheduled.
        * @param eventId The id of the event.
        * @return True if event is scheduled.
        */
        bool HasEventScheduled(EventId eventId) const
        {
            return IsScheduled(eventId) != 0;
        }

        /**
        * @name ScheduleNextFromSeries
        * @brief Schedules packed event data with next timer from series.
        * @param eventData Full event data, including group and phase.
        */
        void ScheduleNextFromSeries(uint32 eventData)
        {
            EventSeriesStore::iterator itr = _timerSeries.find(eventData);
            if (itr == _timerSeries.end() || itr->second.empty())
                return;

            ScheduleEvent(eventData, itr->second.front());

            if (itr->second.size() > 1)
                itr->second.erase(itr->second.begin());
            else
                _timerSeries.erase(itr);
        }

        /**
        * @name ScheduleEventSeries
        * @brief Schedules event with first value of series and requeues with the next.
        * @param eventId Id of the event.
        * @param group Group of the event.
        * @param phase Phase of the event.
        * @param timeSeries Times the event should be scheduled after each trigger.
        */
        void ScheduleEventSeries(EventId eventId, GroupIndex group, PhaseIndex phase, std::initializer_list<Milliseconds> timeSeries)
        {
            if (group > 8 || phase > 8 || !timeSeries.size())
                return;

            uint32 eventData = PackEventId(eventId, group, phase);
            std::vector<Milliseconds>& series = _timerSeries[eventData];
            series.insert(series.end(), timeSeries.begin(), timeSeries.end());
            ScheduleNextFromSeries(eventData);
        }

        /**
        * @name ScheduleEventSeries
        * @brief Schedules event with first value of series and requeues with the next.
        * @param eventId Id of the event.
        * @param timeSeries Times the event should be scheduled after each trigger.
        */
        void ScheduleEventSeries(EventId eventId, std::initializer_list<Milliseconds> timeSeries)
        {
            ScheduleEventSeries(eventId, 0, 0, timeSeries);
        }

    private:
        static uint32 PackEventId(EventId eventId, GroupIndex group, PhaseIndex phase)
        {
            if (group && group <= 8)
                eventId |= (1 << (group + 15));

            if (phase && phase <= 8)
                eventId |= (1 << (phase + 23));

            return eventId;
        }

        /**
        * @name _time
        * @brief Internal timer.
        *
        * This does not represent the real date/time value.
        * It's more like a stopwatch: It can run, it can be stopped,
        * it can be resetted and so on. Events occur when this timer
        * has reached their time value. Its value is changed in the
        * Update method.
        */
        uint32 _time;

        /**
        * @name _phase
        * @brief Phase mask of the event map.
        *
        * Contains the phases the event map is in. Multiple
        * phases from 1 to 8 can be set with SetPhase or
        * AddPhase. RemovePhase deactives a phase.
        */
        uint8 _phase;

        /**
        * @name _eventMap
        * @brief Internal event storage map. Contains the scheduled events.
        *
        * See typedef at the beginning of the class for more
        * details.
        */
        EventStore _eventMap;

        /**
        * @name _lastEvent
        * @brief Stores information on the most recently executed event.
        */
        uint32 _lastEvent;

        /**
        * @name _timerSeries
        * @brief Time series which requeue themselves until the series is empty.
        */
        EventSeriesStore _timerSeries;
};

enum AITarget
{
    AITARGET_SELF,
    AITARGET_VICTIM,
    AITARGET_ENEMY,
    AITARGET_ALLY,
    AITARGET_BUFF,
    AITARGET_DEBUFF
};

enum AICondition
{
    AICOND_AGGRO,
    AICOND_COMBAT,
    AICOND_DIE
};

#define AI_DEFAULT_COOLDOWN 5000

struct AISpellInfoType
{
    AISpellInfoType() : target(AITARGET_SELF), condition(AICOND_COMBAT),
        cooldown(AI_DEFAULT_COOLDOWN), realCooldown(0), maxRange(0.0f) { }
    AITarget target;
    AICondition condition;
    uint32 cooldown;
    uint32 realCooldown;
    float maxRange;
};

AISpellInfoType* GetAISpellInfo(uint32 i);

#endif
