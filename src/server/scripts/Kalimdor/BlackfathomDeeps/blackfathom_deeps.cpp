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
#include "ScriptedGossip.h"
#include "blackfathom_deeps.h"
#include "ScriptedEscortAI.h"
#include "Player.h"

enum Spells
{
    SPELL_BLESSING_OF_BLACKFATHOM                           = 8733,
    SPELL_RAVAGE                                            = 8391,
    SPELL_FROST_NOVA                                        = 865,
    SPELL_FROST_BOLT_VOLLEY                                 = 8398,
    SPELL_TELEPORT_DARNASSUS                                = 9268
};

const Position HomePosition = {-815.817f, -145.299f, -25.870f, 0};

class go_blackfathom_altar : public GameObjectScript
{
    public:
        go_blackfathom_altar() : GameObjectScript("go_blackfathom_altar") { }

        bool OnGossipHello(Player* player, GameObject* /*go*/) override
        {
            if (!player->HasAura(SPELL_BLESSING_OF_BLACKFATHOM))
                player->AddAura(SPELL_BLESSING_OF_BLACKFATHOM, player);
            return true;
        }
};

class go_blackfathom_fire : public GameObjectScript
{
    public:
        go_blackfathom_fire() : GameObjectScript("go_blackfathom_fire") { }

        bool OnGossipHello(Player* /*player*/, GameObject* go) override
        {
            InstanceScript* instance = go->GetInstanceScript();

            if (instance)
            {
                go->SetGoState(GO_STATE_ACTIVE);
                go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                instance->SetData(DATA_FIRE, instance->GetData(DATA_FIRE) + 1);
                return true;
            }
            return false;
        }
};

class npc_blackfathom_deeps_event : public CreatureScript
{
    public:
        npc_blackfathom_deeps_event() : CreatureScript("npc_blackfathom_deeps_event") { }

        struct npc_blackfathom_deeps_eventAI : public ScriptedAI
        {
            npc_blackfathom_deeps_eventAI(Creature* creature) : ScriptedAI(creature)
            {
                if (creature->IsSummon())
                {
                    creature->SetHomePosition(HomePosition);
                    AttackPlayer();
                }

                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 ravageTimer;
            uint32 frostNovaTimer;
            uint32 frostBoltVolleyTimer;

            bool Flee;

            void Reset() override
            {
                Flee = false;

                ravageTimer = urand(5000, 8000);
                frostNovaTimer = urand(9000, 12000);
                frostBoltVolleyTimer = urand(2000, 4000);
            }

            void AttackPlayer()
            {
                Map::PlayerList const &PlList = me->GetMap()->GetPlayers();

                if (PlList.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                {
                    if (Player* player = i->GetSource())
                    {
                        if (player->IsGameMaster())
                            continue;

                        if (player->IsAlive())
                        {
                            me->SetInCombatWith(player);
                            player->SetInCombatWith(me);
                            me->AddThreat(player, 0.0f);
                        }
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                switch (me->GetEntry())
                {
                    case NPC_AKU_MAI_SNAPJAW:
                    {
                        if (ravageTimer <= diff)
                        {
                            DoCastVictim(SPELL_RAVAGE);
                            ravageTimer = urand(9000, 14000);
                        }
                        else ravageTimer -= diff;
                        break;
                    }
                    case NPC_MURKSHALLOW_SOFTSHELL:
                    case NPC_BARBED_CRUSTACEAN:
                    {
                        if (!Flee && HealthBelowPct(15))
                        {
                            Flee = true;
                            me->DoFleeToGetAssistance();
                        }
                        break;
                    }
                    case NPC_AKU_MAI_SERVANT:
                    {
                        if (frostBoltVolleyTimer <= diff)
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_FROST_BOLT_VOLLEY);
                            frostBoltVolleyTimer = urand(5000, 8000);
                        }
                        else frostBoltVolleyTimer -= diff;

                        if (frostNovaTimer <= diff)
                        {
                            DoCastAOE(SPELL_FROST_NOVA, false);
                            frostNovaTimer = urand(25000, 30000);
                        }
                        else frostNovaTimer -= diff;
                        break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (me->IsSummon()) //we are not a normal spawn.
                    if (instance)
                        instance->SetData(DATA_EVENT, instance->GetData(DATA_EVENT) + 1);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_blackfathom_deeps_eventAI(creature);
        }
};

enum Morridune
{
    SAY_MORRIDUNE_1 = 0,
    SAY_MORRIDUNE_2 = 1
};

class npc_morridune : public CreatureScript
{
    public:
        npc_morridune() : CreatureScript("npc_morridune") { }

        struct npc_morriduneAI : public npc_escortAI
        {
            npc_morriduneAI(Creature* creature) : npc_escortAI(creature)
            {
                Talk(SAY_MORRIDUNE_1);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                Start(false, false, ObjectGuid::Empty);
            }

            void WaypointReached(uint32 waypointId) override
            {
                switch (waypointId)
                {
                    case 4:
                        SetEscortPaused(true);
                        me->SetFacingTo(1.775791f);
                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        Talk(SAY_MORRIDUNE_2);
                        break;
                }
            }

            bool OnGossipSelect(Player* player, uint32 /*sender*/, uint32 /*action*/) override
            {
                DoCast(player, SPELL_TELEPORT_DARNASSUS);
                return true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_morriduneAI(creature);
        }
};

void AddSC_blackfathom_deeps()
{
    new go_blackfathom_altar();
    new go_blackfathom_fire();
    new npc_blackfathom_deeps_event();
    new npc_morridune();
}
