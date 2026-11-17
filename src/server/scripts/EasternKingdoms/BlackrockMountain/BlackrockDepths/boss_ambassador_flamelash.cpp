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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum Spells
{
    SPELL_FIREBLAST      = 15573,
    SPELL_BURNING_SPIRIT = 14744,
};

enum Events
{
    EVENT_FIREBLAST = 1,
    EVENT_SPIRITS   = 2,
};

enum Yells
{
    TALK_INTRO = 0,
};

enum Creatures
{
    NPC_BURNING_SPIRIT = 9178,
};

const Position FlamelashSpiritsPos[7] =
{
    { 978.52f,  -241.02f, -61.89f, 6.26f },
    { 995.44f,  -258.08f, -61.90f, 1.04f },
    { 1012.35f, -275.11f, -61.88f, 0.24f },
    { 1029.31f, -257.87f, -61.88f, 6.03f },
    { 1046.42f, -241.13f, -61.88f, 3.10f },
    { 1029.31f, -223.92f, -61.89f, 3.00f },
    { 1012.45f, -207.31f, -61.90f, 4.45f },
};

class boss_ambassador_flamelash : public CreatureScript
{
    public:
        boss_ambassador_flamelash() : CreatureScript("boss_ambassador_flamelash") { }

        struct boss_ambassador_flamelashAI : public ScriptedAI
        {
            boss_ambassador_flamelashAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            EventMap events;
            SummonList summons;
            std::list<uint8> SpiritsPos;

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
                SpiritsPos.clear();
                SpiritsPos = { 0, 1, 2, 3, 4, 5, 6 };
            }

            void JustEngagedWith(Unit* /*who*/) override 
            {
                Talk(TALK_INTRO);
                SummonSpirits();
                events.ScheduleEvent(EVENT_FIREBLAST, 2 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SPIRITS, 24 * IN_MILLISECONDS);
            }

            void SummonSpirits()
            {
                if (!SpiritsPos.empty())
                    for (auto sPos : SpiritsPos)
                        me->SummonCreature(NPC_BURNING_SPIRIT, FlamelashSpiritsPos[sPos], TEMPSUMMON_MANUAL_DESPAWN);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
            }

            void UpdateAI(uint32 diff) override
            {
                //Return since we have no target
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FIREBLAST:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_FIREBLAST);

                            events.ScheduleEvent(EVENT_FIREBLAST, 7 * IN_MILLISECONDS);
                            break;
                        case EVENT_SPIRITS:
                            SpiritsPos.clear();
                            SpiritsPos = { 0, 1, 2, 3, 4, 5, 6 };
                            Trinity::Containers::RandomResizeList(SpiritsPos, 3);
                            SummonSpirits();
                            events.ScheduleEvent(EVENT_SPIRITS, 30 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_ambassador_flamelashAI(creature);
        }
};

// Burning Spirit 9178
class npc_blackrock_burning_spirit : public CreatureScript
{
    public:
        npc_blackrock_burning_spirit() : CreatureScript("npc_blackrock_burning_spirit") { }

        struct npc_blackrock_burning_spiritAI : public ScriptedAI
        {
            npc_blackrock_burning_spiritAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid targetGUID;

            void IsSummonedBy(Unit* summoner) override
            {
                targetGUID = summoner->GetGUID();
                me->GetMotionMaster()->MoveFollow(summoner, 0.0f, me->GetAngle(summoner));
            }

            void UpdateAI(uint32 /*diff*/) override 
            {
                if (Unit* ambassador = ObjectAccessor::GetCreature(*me, targetGUID))
                {
                    if (ambassador->IsWithinDist(me, 1.0f, false))
                    {
                        ambassador->CastSpell(ambassador, SPELL_BURNING_SPIRIT, true);
                        me->DespawnOrUnsummon();
                    }
                    else
                        me->GetMotionMaster()->MoveFollow(ambassador, 0.0f, me->GetAngle(ambassador));
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_blackrock_burning_spiritAI(creature);
        }
};

void AddSC_boss_ambassador_flamelash()
{
    new boss_ambassador_flamelash();
    new npc_blackrock_burning_spirit();
}
