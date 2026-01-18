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

#include "ScriptPCH.h"
#include "Player.h"
#include "Group.h"
#include "SpellInfo.h"
#include "LFGMgr.h"
#include "SpellAuraEffects.h"
#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "World.h"
#include "CreatureTextMgr.h"

enum Spells
{
    // Coren Direbrew
    SPELL_DISARM                            = 47310,
    SPELL_DISARM_PRECAST                    = 47407,
    SPELL_MOLE_MACHINE_EMERGE               = 50313,

    // Ilsa Direbrew and Ursula Direbrew
    SPELL_SEND_FIRST_MUG                    = 47333,
    SPELL_SEND_SECOND_MUG                   = 47339,
    SPELL_HAS_BREW_BUFF                     = 47376,
    SPELL_CONSUME_BREW                      = 47377,
    SPELL_BARRELED                          = 47442,
    SPELL_CHUCK_MUG                         = 50276,
};

enum MiscData
{
    GOSSIP_ID_COREN       = 23872,
    GOSSIP_ITEM_START     = 0,

    NPC_ILSA_DIREBREW     = 26764,
    NPC_URSULA_DIREBREW   = 26822,
    NPC_DIREBREW_MINION   = 26776,

    EQUIP_ID_TANKARD      = 48663,
    FACTION_HOSTILE_COREN = 736
};

const Position AddSpawn[3] =
{
    {890.87f, -133.95f, -48.0f, 1.53f},
    {892.47f, -133.26f, -48.0f, 2.16f},
    {893.54f, -131.81f, -48.0f, 2.75f}
};

// 23872
class npc_coren_direbrew : public CreatureScript
{
    public:
        npc_coren_direbrew() : CreatureScript("npc_coren_direbrew") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            player->ADD_GOSSIP_ITEM_DB(GOSSIP_ID_COREN, GOSSIP_ITEM_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                creature->SetFaction(FACTION_HOSTILE_COREN);
                creature->AI()->AttackStart(player);
                creature->AI()->DoZoneInCombat();

                creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);

                player->CLOSE_GOSSIP_MENU();
            }

            return true;
        }

        struct npc_coren_direbrewAI : public ScriptedAI
        {
            npc_coren_direbrewAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            SummonList summons;

            void Reset()
            {
                me->RestoreFaction();
                me->SetCorpseDelay(90);

                summons.DespawnAll();

                _disarmTimer = urand(10, 15) * IN_MILLISECONDS;
                _addTimer = 20 * IN_MILLISECONDS;

                _spawnedIlsa = false;
                _spawnedUrsula = false;

                for (uint8 i = 0; i < 3; ++i)
                    if (Creature* creature = me->SummonCreature(NPC_DIREBREW_MINION, AddSpawn[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15 * IN_MILLISECONDS))
                        _add[i] = creature->GetGUID();

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
            }

            void JustEngagedWith(Unit* /*who*/)
            {
                SetEquipmentSlots(false, EQUIP_ID_TANKARD, EQUIP_ID_TANKARD, EQUIP_NO_CHANGE);

                for (uint8 i = 0; i < 3; ++i)
                {
                    if (_add[i])
                    {
                        Creature* creature = ObjectAccessor::GetCreature((*me), _add[i]);
                        if (creature && creature->IsAlive())
                        {
                            creature->SetFaction(FACTION_HOSTILE_COREN);
                            creature->SetInCombatWithZone();
                        }
                        _add[i] = ObjectGuid::Empty;
                    }
                }

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (_disarmTimer)
                {
                    if (_disarmTimer <= diff)
                    {
                        DoCast(SPELL_DISARM_PRECAST);
                        DoCastVictim(SPELL_DISARM, false);
                        _disarmTimer = urand(20, 25) * IN_MILLISECONDS;
                    }
                    else
                        _disarmTimer -= diff;
                }

                if (_addTimer)
                {
                    if (_addTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        {
                            float posX, posY, posZ;
                            target->GetPosition(posX, posY, posZ);
                            target->CastSpell(target, SPELL_MOLE_MACHINE_EMERGE, true, 0, 0, me->GetGUID());
                            me->SummonCreature(NPC_DIREBREW_MINION, posX, posY, posZ, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15 * IN_MILLISECONDS);

                            _addTimer = 15 * IN_MILLISECONDS;
                            if (_spawnedIlsa)
                                _addTimer -= 3 * IN_MILLISECONDS;

                            if (_spawnedUrsula)
                                _addTimer -= 3 * IN_MILLISECONDS;
                        }
                    }
                    else
                        _addTimer -= diff;
                }

                if (!_spawnedIlsa && HealthBelowPct(66))
                {
                    DoSpawnCreature(NPC_ILSA_DIREBREW, 0, 0, 0, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15 * IN_MILLISECONDS);
                    _spawnedIlsa = true;
                }

                if (!_spawnedUrsula && HealthBelowPct(33))
                {
                    DoSpawnCreature(NPC_URSULA_DIREBREW, 0, 0, 0, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15 * IN_MILLISECONDS);
                    _spawnedUrsula = true;
                }

                DoMeleeAttackIfReady();
            }

            void JustSummoned(Creature* summon)
            {
                if (me->GetFaction() == FACTION_HOSTILE_COREN)
                {
                    summon->SetFaction(FACTION_HOSTILE_COREN);

                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        summon->AI()->AttackStart(target);
                }

                summons.Summon(summon);
            }

            void JustDied(Unit* /*killer*/)
            {
                summons.DespawnAll();

                Map* map = me->GetMap();
                if (map && map->IsDungeon())
                {
                    if (Group* group = me->GetMap()->GetInstanceGroup())
                        sLFGMgr->FinishDungeon(group->GetGUID(), 287, me->GetMap());
                }
            }

            private:
                ObjectGuid _add[3];
                uint32 _addTimer;
                uint32 _disarmTimer;
                bool _spawnedIlsa;
                bool _spawnedUrsula;
        };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coren_direbrewAI(creature);
    }
};

// 26764, 26822
struct npc_brewmaiden : public ScriptedAI
{
    npc_brewmaiden(Creature* creature) : ScriptedAI(creature) { }

    void Reset()
    {
        brewTimer = 2 * IN_MILLISECONDS;
        chuckMugTimer = 10 * IN_MILLISECONDS;
        barrelTimer = 5 * IN_MILLISECONDS;
    }

    void JustEngagedWith(Unit* /*who*/)
    {
        me->SetInCombatWithZone();
    }

    void AttackStart(Unit* who)
    {
        if (!who)
            return;

        if (me->Attack(who, true))
        {
            me->AddThreat(who, 1.0f);
            me->SetInCombatWith(who);
            who->SetInCombatWith(me);

            if (me->GetEntry() == NPC_URSULA_DIREBREW)
                me->GetMotionMaster()->MoveFollow(who, 10.0f, 0.0f);
            else
                me->GetMotionMaster()->MoveChase(who);
        }
    }

    void SpellHitTarget(Unit* target, SpellInfo const* spell)
    {
        if (spell->Id == SPELL_SEND_FIRST_MUG)
            target->CastSpell(target, SPELL_HAS_BREW_BUFF, true);

        if (spell->Id == SPELL_SEND_SECOND_MUG)
            target->CastSpell(target, SPELL_CONSUME_BREW, true);
    }

    void UpdateAI(uint32 const diff)
    {
        if (!UpdateVictim())
            return;

        if (brewTimer)
        {
            if (brewTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);

                    if (target && me->GetDistance(target) > 5.0f)
                    {
                        DoCast(target, SPELL_SEND_FIRST_MUG);
                        brewTimer = 12 * IN_MILLISECONDS;
                    }
                }
            }
            else
                brewTimer -= diff;
        }

        if (chuckMugTimer)
        {
            if (chuckMugTimer <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    DoCast(target, SPELL_CHUCK_MUG);

                chuckMugTimer = 15 * IN_MILLISECONDS;
            }
            else
                chuckMugTimer -= diff;
        }

        if (me->GetEntry() == NPC_URSULA_DIREBREW)
        {
            if (barrelTimer)
            {
                if (barrelTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCastVictim(SPELL_BARRELED);
                        barrelTimer = urand(15, 18) * IN_MILLISECONDS;
                    }
                }
                else
                    barrelTimer -= diff;
            }
        }
        else
            DoMeleeAttackIfReady();
    }

private:
    uint32 brewTimer;
    uint32 barrelTimer;
    uint32 chuckMugTimer;
};


void AddSC_boss_coren_direbrew()
{
    new npc_coren_direbrew();
    new creature_script<npc_brewmaiden>("npc_brewmaiden");
}
