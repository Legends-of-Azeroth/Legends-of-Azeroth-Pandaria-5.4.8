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
SDName: Silverpine_Forest
SD%Complete: 100
SDComment: Quest support: 435
SDCategory: Silverpine Forest
EndScriptData */

/* ContentData
npc_deathstalker_erland
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "Player.h"

/*######
## npc_deathstalker_erland
######*/

enum eErland
{
    SAY_QUESTACCEPT     = 0,
    SAY_START           = 1,
    SAY_AGGRO           = 2,
    SAY_PROGRESS        = 3,
    SAY_LAST            = 4,

    SAY_RANE            = 0,
    SAY_RANE_ANSWER     = 5,
    SAY_MOVE_QUINN      = 6,

    SAY_QUINN           = 7,
    SAY_QUINN_ANSWER    = 0,
    SAY_BYE             = 8,

    QUEST_ESCORTING     = 435,
    NPC_RANE            = 1950,
    NPC_QUINN           = 1951
};

class npc_deathstalker_erland : public CreatureScript
{
public:
    npc_deathstalker_erland() : CreatureScript("npc_deathstalker_erland") { }

    struct npc_deathstalker_erlandAI : public EscortAI
    {
        npc_deathstalker_erlandAI(Creature* creature) : EscortAI(creature) { }

        void WaypointReached(uint32 waypointId, uint32 pathId) override
        {
            Player* player = GetPlayerForEscort();
            if (!player)
                return;

            switch (waypointId)
            {
                case 1:
                    Talk(SAY_START, player);
                    break;
                case 10:
                    Talk(SAY_PROGRESS);
                    break;
                case 13:
                    Talk(SAY_LAST, player);
                    player->GroupEventHappens(QUEST_ESCORTING, me);
                    break;
                case 15:
                    if (Creature* rane = me->FindNearestCreature(NPC_RANE, 20.0f))
                        rane->AI()->Talk(SAY_RANE);
                    break;
                case 16:
                    Talk(SAY_RANE_ANSWER);
                    break;
                case 17:
                    Talk(SAY_MOVE_QUINN);
                    break;
                case 24:
                    Talk(SAY_QUINN);
                    break;
                case 25:
                    if (Creature* quinn = me->FindNearestCreature(NPC_QUINN, 20.0f))
                        quinn->AI()->Talk(SAY_QUINN_ANSWER);
                    break;
                case 26:
                    Talk(SAY_BYE);
                    break;
            }
        }

        void Reset() override { }

        void JustEngagedWith(Unit* who) override
        {
            Talk(SAY_AGGRO, who);
        }
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_ESCORTING)
        {
            creature->AI()->Talk(SAY_QUESTACCEPT, player);

            if (EscortAI* pEscortAI = CAST_AI(npc_deathstalker_erland::npc_deathstalker_erlandAI, creature->AI()))
                pEscortAI->Start(true, player->GetGUID());
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_deathstalker_erlandAI(creature);
    }
};

enum FenwickThatros
{
    SAY_FENWICK_AGGRO          = 0,

    SPELL_DRINK_HEALING_POTION = 54633,
    SPELL_POTION_OF_STRENGTH   = 80471,
    SPELL_ELIXIR_OF_AGILITY    = 80472,
    SPELL_TONIC_OF_HEALTH      = 80473,

    EVENT_BUFF_STRENGTH        = 1,
    EVENT_BUFF_AGILITY         = 2,
    EVENT_BUFF_STAMINA         = 3,
    EVENT_HEALING              = 4
};

struct npc_fenwick_thatros : public ScriptedAI
{
    npc_fenwick_thatros(Creature* creature) : ScriptedAI(creature) {}

    void Reset() override
    {
        half_hp_reached_once = false;
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(SAY_FENWICK_AGGRO);
        events.ScheduleEvent(EVENT_BUFF_STRENGTH, 3000);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/) override
    {
        if (!half_hp_reached_once && !HealthAbovePct(50))
        {
            half_hp_reached_once = true;
            me->CastSpell(me, SPELL_DRINK_HEALING_POTION);
            events.ScheduleEvent(EVENT_HEALING, 10000);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BUFF_STRENGTH:
                    me->CastSpell(me, SPELL_POTION_OF_STRENGTH);
                    events.ScheduleEvent(EVENT_BUFF_AGILITY, 3000);
                    break;
                case EVENT_BUFF_AGILITY:
                    me->CastSpell(me, SPELL_ELIXIR_OF_AGILITY);
                    events.ScheduleEvent(EVENT_BUFF_STAMINA, 3000);
                    break;
                case EVENT_BUFF_STAMINA:
                    me->CastSpell(me, SPELL_TONIC_OF_HEALTH);
                    break;
                case EVENT_HEALING:
                    if (!HealthAbovePct(50))
                    {
                        me->CastSpell(me, SPELL_DRINK_HEALING_POTION);
                        events.ScheduleEvent(EVENT_HEALING, 10000);
                    }
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
private:
    EventMap events;
    bool half_hp_reached_once = false;
};

/*######
## AddSC
######*/

void AddSC_silverpine_forest()
{
    new npc_deathstalker_erland();
    new creature_script<npc_fenwick_thatros>("npc_fenwick_thatros");
}
