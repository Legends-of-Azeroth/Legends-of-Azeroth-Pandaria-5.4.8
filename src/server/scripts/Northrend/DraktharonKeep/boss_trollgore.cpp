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
#include "SpellAuras.h"
#include "drak_tharon_keep.h"

enum Spells
{
    SPELL_INFECTED_WOUND                          = 49637,
    SPELL_CRUSH                                   = 49639,
    SPELL_CORPSE_EXPLODE                          = 49555,
    SPELL_CONSUME                                 = 49380,
    SPELL_CONSUME_AURA                            = 49381,
    //Heroic spells
    SPELL_CORPSE_EXPLODE_H                        = 59807,
    SPELL_CONSUME_H                               = 59803,
    SPELL_CONSUME_AURA_H                          = 59805,

    SPELL_SUMMON_INVADER_A                        = 49456,
    SPELL_SUMMON_INVADER_B                        = 49457,
    SPELL_SUMMON_INVADER_C                        = 49458,

    SPELL_INVADER_TAUNT_TRIGGER                   = 49405,
};

enum Yells
{
    SAY_AGGRO                                     = 0,
    SAY_KILL                                      = 1,
    SAY_CONSUME                                   = 2,
    SAY_EXPLODE                                   = 3,

    SOUND_DEATH                                   = 13183,
};

enum Creatures
{
    NPC_WORLD_TRIGGER                             = 22515,
    NPC_DRAKKARI_INVADER_A                        = 27709,
    NPC_DRAKKARI_INVADER_B                        = 27753,
    NPC_DRAKKARI_INVADER_C                        = 27754,
};

#define DATA_CONSUMPTION_JUNCTION                 1

class boss_trollgore : public CreatureScript
{
    public:
        boss_trollgore() : CreatureScript("boss_trollgore") { }

        struct boss_trollgoreAI : public ScriptedAI
        {
            boss_trollgoreAI(Creature* creature) : ScriptedAI(creature), lSummons(me)
            {
                instance = creature->GetInstanceScript();
                uiSpawnTimer = 30 * IN_MILLISECONDS;
            }

            uint32 uiConsumeTimer;
            uint32 uiAuraCountTimer;
            uint32 uiCrushTimer;
            uint32 uiInfectedWoundTimer;
            uint32 uiExplodeCorpseTimer;
            uint32 uiSpawnTimer;
            bool fightingPlayers = false;
            bool consumptionJunction;

            SummonList lSummons;

            InstanceScript* instance;

            void Reset() override
            {
                uiConsumeTimer = 15 * IN_MILLISECONDS;
                uiAuraCountTimer = 15500;
                uiCrushTimer = urand(1 * IN_MILLISECONDS, 5 * IN_MILLISECONDS);
                uiInfectedWoundTimer = urand(10 * IN_MILLISECONDS, 60 * IN_MILLISECONDS);
                uiExplodeCorpseTimer = 3 * IN_MILLISECONDS;

                consumptionJunction = true;

                lSummons.DespawnAll();

                me->RemoveAura(DUNGEON_MODE(SPELL_CONSUME_AURA, SPELL_CONSUME_AURA_H));

                if (fightingPlayers)
                {
                    fightingPlayers = false;
                    instance->SetData(DATA_TROLLGORE_EVENT, NOT_STARTED);
                }

                me->GetMap()->SetWorldState(WORLDSTATE_CONSUMPTION_JUNCTION, 1);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (who->IsControlledByPlayer())
                {
                    Talk(SAY_AGGRO);

                    fightingPlayers = true;
                    instance->SetData(DATA_TROLLGORE_EVENT, IN_PROGRESS);
                }
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_UNIT)
                {
                    switch (who->GetEntry())
                    {
                        case NPC_DRAKKARI_INVADER_A:
                        case NPC_DRAKKARI_INVADER_B:
                        case NPC_DRAKKARI_INVADER_C:
                            return;
                    }
                }

                ScriptedAI::MoveInLineOfSight(who);
            }

            void DamageTaken(Unit* attacker, uint32& /*damage*/) override
            {
                if (!fightingPlayers && attacker->IsControlledByPlayer())
                {
                    fightingPlayers = true;
                    instance->SetData(DATA_TROLLGORE_EVENT, IN_PROGRESS);
                }
            }

            void SpellHitTarget(Unit* /*who*/, SpellInfo const* spell) override
            {
                if (spell && spell->Id == DUNGEON_MODE(SPELL_CONSUME, SPELL_CONSUME_H))
                {
                    DoCast(me, SPELL_CONSUME_AURA, true);
                    if (me->GetAuraCount(DUNGEON_MODE(SPELL_CONSUME_AURA, SPELL_CONSUME_AURA_H)) > 9)
                        me->GetMap()->SetWorldState(WORLDSTATE_CONSUMPTION_JUNCTION, 0);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (uiSpawnTimer <= diff)
                {
                    std::list<Creature*> spawners;
                    GetCreatureListWithEntryInGrid(spawners, me, NPC_WORLD_TRIGGER, 100.0f);
                    spawners.remove_if([](Creature* spawner) { return spawner->GetPositionZ() < 30.0f; });
                    for (Creature* spawner : spawners)
                        spawner->CastSpell(spawner, spawner->GetPositionY() < -670.0f ? SPELL_SUMMON_INVADER_A : SPELL_SUMMON_INVADER_B, true);
                    uiSpawnTimer = 30 * IN_MILLISECONDS;
                } else uiSpawnTimer -= diff;

                if (!UpdateVictim())
                    return;

                // Don't use spells on event NPCs
                if (me->GetVictim() && me->GetVictim()->GetTypeId() == TYPEID_UNIT)
                {
                    switch (me->GetVictim()->GetEntry())
                    {
                        case NPC_DRAKKARI_INVADER_A:
                        case NPC_DRAKKARI_INVADER_B:
                        case NPC_DRAKKARI_INVADER_C:
                            DoMeleeAttackIfReady();
                            return;
                    }
                }

                if (!fightingPlayers && me->GetVictim() && me->GetVictim()->IsControlledByPlayer())
                {
                    fightingPlayers = true;
                    instance->SetData(DATA_TROLLGORE_EVENT, IN_PROGRESS);
                }

                if (uiConsumeTimer <= diff)
                {
                    Talk(SAY_CONSUME);
                    DoCast(SPELL_CONSUME);
                    if (GetDifficulty() == DUNGEON_DIFFICULTY_HEROIC)
                        DoCast(SPELL_CONSUME_AURA_H);
                    else
                        DoCast(SPELL_CONSUME_AURA);
                    uiConsumeTimer = 15 * IN_MILLISECONDS;
                } else uiConsumeTimer -= diff;

                if (consumptionJunction)
                {
                    Aura* ConsumeAura = me->GetAura(DUNGEON_MODE(SPELL_CONSUME_AURA, SPELL_CONSUME_AURA_H));
                    if (ConsumeAura && ConsumeAura->GetStackAmount() > 9)
                        consumptionJunction = false;
                }

                if (uiCrushTimer <= diff)
                {
                    DoCastVictim(SPELL_CRUSH);
                    uiCrushTimer = urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS);
                } else uiCrushTimer -= diff;

                if (uiInfectedWoundTimer <= diff)
                {
                    DoCastVictim(SPELL_INFECTED_WOUND);
                    uiInfectedWoundTimer = urand(25 * IN_MILLISECONDS, 35 * IN_MILLISECONDS);
                } else uiInfectedWoundTimer -= diff;

                if (uiExplodeCorpseTimer <= diff)
                {
                    DoCast(SPELL_CORPSE_EXPLODE);
                    Talk(SAY_EXPLODE);
                    uiExplodeCorpseTimer = urand(15 * IN_MILLISECONDS, 19 * IN_MILLISECONDS);
                } else uiExplodeCorpseTimer -= diff;

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->PlayDirectSound(SOUND_DEATH);

                lSummons.DespawnAll();

                if (!fightingPlayers)
                    instance->SetData(DATA_TROLLGORE_EVENT, IN_PROGRESS);

                fightingPlayers = false;
                instance->SetData(DATA_TROLLGORE_EVENT, DONE);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_CONSUMPTION_JUNCTION)
                    return consumptionJunction ? 1 : 0;

                return 0;
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL, victim);
            }

            void JustSummoned(Creature* summon) override
            {
                lSummons.Summon(summon);
                if (summon->AI())
                    summon->AI()->AttackStart(me);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_trollgoreAI(creature);
        }
};

class npc_drakkari_invader : public CreatureScript
{
    public:
        npc_drakkari_invader() : CreatureScript("npc_drakkari_invader") { }

        struct npc_drakkari_invaderAI : public ScriptedAI
        {
            npc_drakkari_invaderAI(Creature* creature) : ScriptedAI(creature) { }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                if (CreatureAddon const* addon = me->GetCreatureAddon())
                    if (addon->path_id)
                        me->GetMotionMaster()->MovePath(addon->path_id, false);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (me->GetDistance(who) <= 5.0f)
                    ScriptedAI::MoveInLineOfSight(who);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == WAYPOINT_MOTION_TYPE && pointId == 1)
                {
                    me->Dismount();
                    me->SetFlying(false);
                    DoCast(me, SPELL_INVADER_TAUNT_TRIGGER);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_drakkari_invaderAI(creature);
        }
};

class spell_invader_taunt_trigger : public SpellScriptLoader
{
    public:
        spell_invader_taunt_trigger() : SpellScriptLoader("spell_invader_taunt_trigger") { }

        class spell_invader_taunt_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_invader_taunt_trigger_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetHitUnit())
                        target->CastSpell(caster, GetEffectValue(), true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_invader_taunt_trigger_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_invader_taunt_trigger_SpellScript();
        }
};

class achievement_consumption_junction : public AchievementCriteriaScript
{
    public:
        achievement_consumption_junction() : AchievementCriteriaScript("achievement_consumption_junction") { }

        bool OnCheck(Player* /*source*/, Unit* target) override
        {
            if (!target)
                return false;

            if (Creature* Trollgore = target->ToCreature())
                if (Trollgore->AI()->GetData(DATA_CONSUMPTION_JUNCTION))
                    return true;

            return false;
        }
};

void AddSC_boss_trollgore()
{
    new boss_trollgore();
    new npc_drakkari_invader();
    new spell_invader_taunt_trigger();
    new achievement_consumption_junction();
}
