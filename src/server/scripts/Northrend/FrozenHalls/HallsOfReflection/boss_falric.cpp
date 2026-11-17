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

 /* ScriptData
 SDName: boss_falric
 SD%Complete: 
 SDCategory: Halls of Reflection
 EndScriptData */

#include "ScriptPCH.h"
#include "halls_of_reflection.h"

enum Yells
{
    SAY_AGGRO                                     = 0,
    SAY_SLAY                                      = 1,
    SAY_DEATH                                     = 2,
    SAY_INTRO_2                                   = 3,
    SAY_IMPENDING_DESPAIR                         = 4,
    SAY_DEFILING_HORROR                           = 5,
    SAY_INTRO_1                                   = 6,
};

enum Spells
{
    SPELL_QUIVERING_STRIKE                        = 72422,
    SPELL_IMPENDING_DESPAIR                       = 72426,
    SPELL_DEFILING_HORROR                         = 72435,
    SPELL_HOPELESSNESS_1                          = 72395,
    SPELL_HOPELESSNESS_2                          = 72396,
    SPELL_HOPELESSNESS_3                          = 72397,
    SPELL_HOPELESSNESS_1_H                        = 72390,
    SPELL_HOPELESSNESS_2_H                        = 72391,
    SPELL_HOPELESSNESS_3_H                        = 72393,
};

uint32 hopelessnessNormal[] = { SPELL_HOPELESSNESS_1,   SPELL_HOPELESSNESS_2,   SPELL_HOPELESSNESS_3   };
uint32 hopelessnessHeroic[] = { SPELL_HOPELESSNESS_1_H, SPELL_HOPELESSNESS_2_H, SPELL_HOPELESSNESS_3_H };

enum Events
{
    EVENT_NONE,
    EVENT_QUIVERING_STRIKE,
    EVENT_IMPENDING_DESPAIR,
    EVENT_DEFILING_HORROR,
};

class boss_falric : public CreatureScript
{
    public:
        boss_falric() : CreatureScript("boss_falric") { }

        struct boss_falricAI : public boss_horAI
        {
            boss_falricAI(Creature* creature) : boss_horAI(creature) { }

            uint8 uiHopelessnessCount;

            void Reset() override
            {
                boss_horAI::Reset();

                uiHopelessnessCount = 0;

                if (instance)
                    instance->SetData(DATA_FALRIC_EVENT, NOT_STARTED);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (instance)
                    instance->SetData(DATA_FALRIC_EVENT, IN_PROGRESS);

                events.ScheduleEvent(EVENT_QUIVERING_STRIKE, 23000);
                events.ScheduleEvent(EVENT_IMPENDING_DESPAIR, 9000);
                events.ScheduleEvent(EVENT_DEFILING_HORROR, urand(25000, 45000)); // TODO adjust timer.
            }

            void JustDied(Unit* killer) override
            {
                Talk(SAY_DEATH, killer);

                if (instance)
                {
                    instance->SetData(DATA_FALRIC_EVENT, DONE);

                    if (Creature* marwyn = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_MARWYN)))
                        marwyn->m_Events.Schedule(60000, [marwyn]() { marwyn->AI()->DoAction(ACTION_SAY_GAUNTLET_RESUME); });
                }
            }

            void KilledUnit(Unit* victim) override
            {
                Talk(SAY_SLAY, victim);
            }

            void DoAction(int32 action) override
            {
                ObjectGuid playerGuid = ObjectGuid::Empty;
                if (Player* player = me->FindNearestPlayer(100.0f))
                    playerGuid = player->GetGUID();

                switch (action)
                {
                    case ACTION_ENTER_COMBAT:  // called by InstanceScript when boss shall enter in combat.
                        // Just in case. Should have been done by InstanceScript
                        me->SetVisible(true);

                        Talk(SAY_AGGRO, ObjectAccessor::GetPlayer(*me, playerGuid));
                        me->HandleEmoteStateCommand(EMOTE_STATE_TALK);

                        me->m_Events.Schedule(6000, [this]()
                        {
                            me->HandleEmoteCommand(EMOTE_ONESHOT_POINT_NO_SHEATHE);
                            me->HandleEmoteStateCommand(0);
                        });
                        me->m_Events.Schedule(8000, [this]()
                        {
                            // Reset flags
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                            me->SetReactState(REACT_AGGRESSIVE);

                            if (Unit* pUnit = me->SelectNearestTarget())
                                AttackStart(pUnit);

                            DoZoneInCombat();

                            if (!me->IsInCombat())
                                EnterEvadeMode();
                        });
                        break;
                    case ACTION_SAY_INTRO_1:
                        Talk(SAY_INTRO_1, ObjectAccessor::GetPlayer(*me, playerGuid));
                        break;
                    case ACTION_SAY_INTRO_2:
                        Talk(SAY_INTRO_2, ObjectAccessor::GetPlayer(*me, playerGuid));
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_QUIVERING_STRIKE:
                        DoCast(SPELL_QUIVERING_STRIKE);
                        events.ScheduleEvent(EVENT_QUIVERING_STRIKE, 10000);
                        break;
                    case EVENT_IMPENDING_DESPAIR:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        {
                            Talk(SAY_IMPENDING_DESPAIR);
                            DoCast(target, SPELL_IMPENDING_DESPAIR);
                        }
                        events.ScheduleEvent(EVENT_IMPENDING_DESPAIR, 13000);
                        break;
                    case EVENT_DEFILING_HORROR:
                        Talk(SAY_DEFILING_HORROR);
                        DoCast(SPELL_DEFILING_HORROR);
                        events.ScheduleEvent(EVENT_DEFILING_HORROR, urand(25000, 45000)); // TODO adjust timer.
                        break;
                }

                if ((uiHopelessnessCount < 1 && HealthBelowPct(66))
                    || (uiHopelessnessCount < 2 && HealthBelowPct(33))
                    || (uiHopelessnessCount < 3 && HealthBelowPct(10)))
                {
                    for (auto&& spell : DUNGEON_MODE(hopelessnessNormal, hopelessnessHeroic))
                        me->RemoveOwnedAura(spell);

                    DoCast(me, DUNGEON_MODE(hopelessnessNormal, hopelessnessHeroic)[uiHopelessnessCount], true);
                    uiHopelessnessCount++;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_falricAI(creature);
        }
};

void AddSC_boss_falric()
{
    new boss_falric();
}
