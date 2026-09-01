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
#include "violet_hold.h"

enum Spells
{
    SPELL_ARCANE_BARRAGE_VOLLEY               = 54202,
    SPELL_ARCANE_BARRAGE_VOLLEY_H             = 59483,
    SPELL_SUMMON_ETHEREAL_SPHERE_1            = 54102,
    SPELL_SUMMON_ETHEREAL_SPHERE_2            = 54137,
    SPELL_SUMMON_ETHEREAL_SPHERE_3            = 54138,
    SPELL_ARCANE_BOLT                           = 13748
};

enum NPCs
{
    NPC_ETHEREAL_SPHERE                       = 29271,
    //NPC_ETHEREAL_SPHERE2                    = 32582, // heroic only?
};

enum CreatureSpells
{
    SPELL_ARCANE_POWER                             = 54160,
    SPELL_ARCANE_POWER_H                           = 59474,
    SPELL_ARCANE_BUFFED                            = 54226,
    SPELL_ARCANE_BUFFED_H                          = 59485,
    SPELL_SUMMON_PLAYERS                           = 54164,
    SPELL_POWER_BALL_VISUAL                        = 54141,
};

enum Yells
{
    // Xevozz
    SAY_AGGRO                                   = 0,
    SAY_SLAY                                    = 1,
    SAY_DEATH                                   = 2,
    SAY_CHARGED                                 = 4, // unused
    SAY_REPEAT_SUMMON                           = 5,
    SAY_SUMMON_ENERGY                           = 6,

    // Ethereal Sphere
    SAY_ETHEREAL_SPHERE_SUMMON                  = 0,
};

class boss_xevozz : public CreatureScript
{
    public:
        boss_xevozz() : CreatureScript("boss_xevozz") { }

        struct boss_xevozzAI : public ScriptedAI
        {
            boss_xevozzAI(Creature* creature) : ScriptedAI(creature)
            {
                instance  = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 uiSummonEtherealSphere_Timer;
            uint32 uiArcaneBarrageVolley_Timer;
            uint32 uiArcaneBuffet_Timer;
            uint32 uiArcaneBolt;

            void Reset() override
            {
                if (instance)
                {
                    if (instance->GetData(DATA_WAVE_COUNT) == 6)
                        instance->SetData(DATA_1ST_BOSS_EVENT, NOT_STARTED);
                    else if (instance->GetData(DATA_WAVE_COUNT) == 12)
                        instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);
                }

                uiSummonEtherealSphere_Timer = urand(10000, 12000);
                uiArcaneBarrageVolley_Timer = urand(20000, 22000);
                uiArcaneBolt = urand(2400, 3800);
                uiArcaneBuffet_Timer = urand(5000, 6000);
                DespawnSphere();
            }

            void DespawnSphere()
            {
                std::list<Creature*> assistList;
                GetCreatureListWithEntryInGrid(assistList,me, NPC_ETHEREAL_SPHERE ,150.0f);

                if (assistList.empty())
                    return;

                for (auto&& pSphere : assistList)
                    if (pSphere)
                        pSphere->Kill(pSphere, false);
            }

            void JustSummoned(Creature* summon) override
            {
                summon->SetSpeed(MOVE_RUN, 0.5f);
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                {
                    summon->AddThreat(target, 0.00f);
                    summon->AI()->AttackStart(target);
                }
            }

            void AttackStart(Unit* who) override
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC) || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (me->Attack(who, true))
                {
                    me->AddThreat(who, 0.0f);
                    me->SetInCombatWith(who);
                    who->SetInCombatWith(me);
                    DoStartMovement(who);
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                if (instance)
                {
                    if (GameObject* pDoor = instance->instance->GetGameObject(instance->GetGuidData(DATA_XEVOZZ_CELL)))
                        if (pDoor->GetGoState() == GO_STATE_READY)
                        {
                            EnterEvadeMode();
                            return;
                        }
                    if (instance->GetData(DATA_WAVE_COUNT) == 6)
                        instance->SetData(DATA_1ST_BOSS_EVENT, IN_PROGRESS);
                    else if (instance->GetData(DATA_WAVE_COUNT) == 12)
                        instance->SetData(DATA_2ND_BOSS_EVENT, IN_PROGRESS);
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                ScriptedAI::EnterEvadeMode();
                if (instance)
                    instance->SetData(DATA_WIPE, 1);
            }

            void MoveInLineOfSight(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (uiArcaneBarrageVolley_Timer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(me, SPELL_ARCANE_BARRAGE_VOLLEY);
                        uiArcaneBarrageVolley_Timer = urand(20000, 22000);
                    }
                }
                else uiArcaneBarrageVolley_Timer -= diff;

                if (uiArcaneBolt <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        DoCast(target, SPELL_ARCANE_BOLT);
                    uiArcaneBolt = urand(2400, 3800);
                }

                else uiArcaneBolt -= diff;

                if (uiArcaneBuffet_Timer)
                {
                    if (uiArcaneBuffet_Timer <= diff)
                    {
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            DoCast(me->GetVictim(), DUNGEON_MODE(SPELL_ARCANE_BUFFED,SPELL_ARCANE_BUFFED_H));
                            uiArcaneBuffet_Timer = urand(15000, 20000);
                        }
                    }
                    else uiArcaneBuffet_Timer -= diff;
                }

                if (uiSummonEtherealSphere_Timer <= diff)
                {
                    Talk(SAY_REPEAT_SUMMON);
                    DoCast(me, SPELL_SUMMON_ETHEREAL_SPHERE_1);
                    if (IsHeroic()) // extra one for heroic
                        me->SummonCreature(NPC_ETHEREAL_SPHERE, me->GetPositionX() - 5 + rand() % 10, me->GetPositionY() - 5 + rand() % 10, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 40000ms);

                    uiSummonEtherealSphere_Timer = urand(45000, 47000);
                }
                else uiSummonEtherealSphere_Timer -= diff;

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);

                DespawnSphere();

                if (instance)
                {
                    if (instance->GetData(DATA_WAVE_COUNT) == 6)
                    {
                        if (IsHeroic() && instance->GetData(DATA_1ST_BOSS_EVENT) == DONE)
                            me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                        instance->SetData(DATA_1ST_BOSS_EVENT, DONE);
                        instance->SetData(DATA_WAVE_COUNT, 7);
                    }
                    else if (instance->GetData(DATA_WAVE_COUNT) == 12)
                    {
                        if (IsHeroic() && instance->GetData(DATA_2ND_BOSS_EVENT) == DONE)
                            me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                        instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);
                        instance->SetData(DATA_WAVE_COUNT, 13);
                    }
                }
            }

            void SpellHit(Unit* /*who*/, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_ARCANE_POWER || spell->Id == SPELL_ARCANE_POWER_H)
                    Talk(SAY_SUMMON_ENERGY);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                Talk(SAY_SLAY);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_xevozzAI(creature);
        }
};

class npc_ethereal_sphere : public CreatureScript
{
    public:
        npc_ethereal_sphere() : CreatureScript("npc_ethereal_sphere") { }

        struct npc_ethereal_sphereAI : public ScriptedAI
        {
            npc_ethereal_sphereAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 uiSummonPlayers_Timer;
            uint32 uiRangeCheck_Timer;

            void Reset() override
            {
                uiSummonPlayers_Timer = urand(33000, 35000);
                uiRangeCheck_Timer = 1000;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!me->HasAura(SPELL_POWER_BALL_VISUAL))
                    DoCast(me, SPELL_POWER_BALL_VISUAL);

                if (uiRangeCheck_Timer < diff)
                {
                    if (instance)
                    {
                        if (Creature* pXevozz = Unit::GetCreature(*me, instance->GetGuidData(DATA_XEVOZZ)))
                        {
                            float fDistance = me->GetDistance2d(pXevozz);
                            if (fDistance <= 3)
                                DoCast(pXevozz, SPELL_ARCANE_POWER);
                            else
                                DoCast(me, 35845); // Is it blizzlike?
                        }
                    }
                    uiRangeCheck_Timer = 1000;
                }
                else uiRangeCheck_Timer -= diff;

                if (uiSummonPlayers_Timer < diff)
                {
                    DoCast(me, SPELL_SUMMON_PLAYERS); // not working right

                    Map* map = me->GetMap();
                    if (map && map->IsDungeon())
                    {
                        Map::PlayerList const& PlayerList = map->GetPlayers();
                        if (!PlayerList.isEmpty())
                            for (auto&& ref : PlayerList)
                                if (ref.GetSource()->IsAlive())
                                    DoTeleportPlayer(ref.GetSource(), me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), ref.GetSource()->GetOrientation());
                    }

                    uiSummonPlayers_Timer = urand(33000, 35000);
                }
                else uiSummonPlayers_Timer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ethereal_sphereAI(creature);
        }
};

void AddSC_boss_xevozz()
{
    new boss_xevozz();
    new npc_ethereal_sphere();
}
