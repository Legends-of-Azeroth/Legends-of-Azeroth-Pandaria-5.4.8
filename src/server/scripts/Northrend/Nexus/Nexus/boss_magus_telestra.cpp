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
#include "nexus.h"

enum Spells
{
    SPELL_ICE_NOVA                                = 47772,
    SPELL_ICE_NOVA_H                              = 56935,
    SPELL_FIREBOMB                                = 47773,
    SPELL_FIREBOMB_H                              = 56934,
    SPELL_GRAVITY_WELL                            = 47756,
    SPELL_TELESTRA_BACK                           = 47714,

    SPELL_FIRE_MAGUS_VISUAL                       = 47705,
    SPELL_FROST_MAGUS_VISUAL                      = 47706,
    SPELL_ARCANE_MAGUS_VISUAL                     = 47704,

    SPELL_CRITTER                                 = 47731,
    SPELL_TIMESTOP                                = 47736
};

enum Creatures
{
    NPC_FIRE_MAGUS                                = 26928,
    NPC_FROST_MAGUS                               = 26930,
    NPC_ARCANE_MAGUS                              = 26929
};

enum Yells
{
    SAY_AGGRO                                     = 0,
    SAY_KILL                                      = 1,
    SAY_DEATH                                     = 2,
    SAY_MERGE                                     = 3,
    SAY_SPLIT                                     = 4,
};

enum Misc
{
    ACTION_MAGUS_DEAD                             = 1,

    WORLD_STATE_SPLIT_PERSONALITY                 = 6421,
};

const Position CenterOfRoom = { 504.80f, 89.07f, -16.12f, 6.27f };

class boss_magus_telestra : public CreatureScript
{
    public:
        boss_magus_telestra() : CreatureScript("boss_magus_telestra") { }

        struct boss_magus_telestraAI : public ScriptedAI
        {
            boss_magus_telestraAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            ObjectGuid uiFireMagusGUID;
            ObjectGuid uiFrostMagusGUID;
            ObjectGuid uiArcaneMagusGUID;

            bool bFireMagusDead;
            bool bFrostMagusDead;
            bool bArcaneMagusDead;
            bool bIsWaitingToAppear;

            uint32 uiIsWaitingToAppearTimer;
            uint32 uiIceNovaTimer;
            uint32 uiFireBombTimer;
            uint32 uiGravityWellTimer;
            uint32 uiCooldown;

            uint8 Phase;
            uint32 personalitiesKilled;
            TimeValue lastPertsonalityKilledTime;

            void Reset() override
            {
                Phase = 0;
                //These times are probably wrong
                uiIceNovaTimer =  7 * IN_MILLISECONDS;
                uiFireBombTimer =  0;
                uiGravityWellTimer = 15 * IN_MILLISECONDS;
                uiCooldown = 0;

                uiFireMagusGUID = ObjectGuid::Empty;
                uiFrostMagusGUID = ObjectGuid::Empty;
                uiArcaneMagusGUID = ObjectGuid::Empty;

                bIsWaitingToAppear = false;

                personalitiesKilled = 0;
                lastPertsonalityKilledTime = TimeValue::zero();

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetVisible(true);

                if (instance)
                    instance->SetData(DATA_MAGUS_TELESTRA_EVENT, NOT_STARTED);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoZoneInCombat();
                Talk(SAY_AGGRO);

                if (instance)
                    instance->SetData(DATA_MAGUS_TELESTRA_EVENT, IN_PROGRESS);
            }

            void AttackStart(Unit* who) override
            {
                if (who && me->Attack(who, true))
                    casterMovement.Chase(who);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);

                if (instance)
                    instance->SetData(DATA_MAGUS_TELESTRA_EVENT, DONE);
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_MAGUS_DEAD)
                {
                    if (!lastPertsonalityKilledTime)
                        me->GetMap()->SetWorldState(WORLD_STATE_SPLIT_PERSONALITY, 1);

                    auto now = TimeValue::Now();
                    if (personalitiesKilled)
                    {
                        if ((now - lastPertsonalityKilledTime) > Seconds(5))
                            me->GetMap()->SetWorldState(WORLD_STATE_SPLIT_PERSONALITY, 0);
                    }

                    ++personalitiesKilled;
                    lastPertsonalityKilledTime = now;
                }
            }

            ObjectGuid SplitPersonality(uint32 entry)
            {
                if (Creature* Summoned = me->SummonCreature(entry, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, Milliseconds(1 * IN_MILLISECONDS)))
                {
                    switch (entry)
                    {
                        case NPC_FIRE_MAGUS:
                            Summoned->CastSpell(Summoned, SPELL_FIRE_MAGUS_VISUAL, false);
                            break;
                        case NPC_FROST_MAGUS:
                            Summoned->CastSpell(Summoned, SPELL_FROST_MAGUS_VISUAL, false);
                            break;
                        case NPC_ARCANE_MAGUS:
                            Summoned->CastSpell(Summoned, SPELL_ARCANE_MAGUS_VISUAL, false);
                            break;
                    }

                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        Summoned->AI()->AttackStart(target);

                    DoZoneInCombat(Summoned);

                    return Summoned->GetGUID();
                }
                return ObjectGuid::Empty;
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                if (summon->IsAlive())
                    return;

                if (summon->GetGUID() == uiFireMagusGUID)
                {
                    me->AI()->DoAction(ACTION_MAGUS_DEAD);
                    bFireMagusDead = true;
                }
                else if (summon->GetGUID() == uiFrostMagusGUID)
                {
                    me->AI()->DoAction(ACTION_MAGUS_DEAD);
                    bFrostMagusDead = true;
                }
                else if (summon->GetGUID() == uiArcaneMagusGUID)
                {
                    me->AI()->DoAction(ACTION_MAGUS_DEAD);
                    bArcaneMagusDead = true;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    casterMovement.Update(diff);

                if (bIsWaitingToAppear)
                {
                    me->StopMoving();
                    me->AttackStop();
                    if (uiIsWaitingToAppearTimer <= diff)
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        bIsWaitingToAppear = false;
                    } else uiIsWaitingToAppearTimer -= diff;
                    return;
                }

                if (Phase == 1 || Phase == 3)
                {
                    if (bFireMagusDead && bFrostMagusDead && bArcaneMagusDead)
                    {
                        personalitiesKilled = 0;

                        me->GetMotionMaster()->Clear();
                        me->SetPosition(CenterOfRoom.GetPositionX(), CenterOfRoom.GetPositionY(), CenterOfRoom.GetPositionZ(), CenterOfRoom.GetOrientation());
                        DoCast(me, SPELL_TELESTRA_BACK);
                        me->SetVisible(true);
                        if (Phase == 1)
                            Phase = 2;
                        if (Phase == 3)
                            Phase = 4;
                        uiFireMagusGUID = ObjectGuid::Empty;
                        uiFrostMagusGUID = ObjectGuid::Empty;
                        uiArcaneMagusGUID = ObjectGuid::Empty;
                        bIsWaitingToAppear = true;
                        uiIsWaitingToAppearTimer = 4 * IN_MILLISECONDS;
                        Talk(SAY_MERGE);
                    }
                    else
                        return;
                }

                if (Phase == 0 && HealthBelowPct(50))
                {
                    Phase = 1;
                    me->CastStop();
                    me->RemoveAllAuras();
                    me->SetVisible(false);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    uiFireMagusGUID = SplitPersonality(NPC_FIRE_MAGUS);
                    uiFrostMagusGUID = SplitPersonality(NPC_FROST_MAGUS);
                    uiArcaneMagusGUID = SplitPersonality(NPC_ARCANE_MAGUS);
                    bFireMagusDead = false;
                    bFrostMagusDead = false;
                    bArcaneMagusDead = false;
                    Talk(SAY_SPLIT);
                    return;
                }

                if (IsHeroic() && Phase == 2 && HealthBelowPct(10))
                {
                    Phase = 3;
                    me->CastStop();
                    me->RemoveAllAuras();
                    me->SetVisible(false);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    uiFireMagusGUID = SplitPersonality(NPC_FIRE_MAGUS);
                    uiFrostMagusGUID = SplitPersonality(NPC_FROST_MAGUS);
                    uiArcaneMagusGUID = SplitPersonality(NPC_ARCANE_MAGUS);
                    bFireMagusDead = false;
                    bFrostMagusDead = false;
                    bArcaneMagusDead = false;
                    Talk(SAY_SPLIT);
                    return;
                }

                if (uiCooldown)
                {
                    if (uiCooldown <= diff)
                        uiCooldown = 0;
                    else
                    {
                        uiCooldown -= diff;
                        return;
                    }
                }

                if (uiIceNovaTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    {
                        DoCast(target, SPELL_ICE_NOVA, false);
                        uiCooldown = 1500;
                    }
                    uiIceNovaTimer = 15 * IN_MILLISECONDS;
                } else uiIceNovaTimer -= diff;

                if (uiGravityWellTimer <= diff)
                {
                    if (Unit* target = me->GetVictim())
                    {
                        DoCast(target, SPELL_GRAVITY_WELL);
                        uiCooldown = 6 * IN_MILLISECONDS;
                    }
                    uiGravityWellTimer = 15 * IN_MILLISECONDS;
                } else uiGravityWellTimer -= diff;

                if (uiFireBombTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    {
                        DoCast(target, SPELL_FIREBOMB, false);
                        uiCooldown = 2 * IN_MILLISECONDS;
                    }
                    uiFireBombTimer = 2 * IN_MILLISECONDS;
                } else uiFireBombTimer -=diff;

                if (casterMovement.IsUnableToCast())
                    DoMeleeAttackIfReady();
            }

        private:
            CasterMovement casterMovement = CasterMovement(me).Spell(SPELL_FIREBOMB);
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_magus_telestraAI(creature);
        }
};

class boss_magus_telestra_arcane : public CreatureScript
{
    public:
        boss_magus_telestra_arcane() : CreatureScript("boss_magus_telestra_arcane") { }

        struct boss_magus_telestra_arcaneAI : public ScriptedAI
        {
            boss_magus_telestra_arcaneAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 uiCritterTimer;
            uint32 uiTimeStopTimer;

            void Reset() override
            {
                uiCritterTimer = urand(3000, 6000);
                uiTimeStopTimer = urand(7000, 10000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (uiCritterTimer <= diff)
                {
                    DoCast(SPELL_CRITTER);
                    uiCritterTimer = urand(5000, 8000);
                }else uiCritterTimer -= diff;

                if (uiTimeStopTimer <= diff)
                {
                    DoCastAOE(SPELL_TIMESTOP);
                    uiTimeStopTimer = urand(15000, 18000);
                } else uiTimeStopTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_magus_telestra_arcaneAI(creature);
        }
};

class spell_nexus_critter_targeting : public SpellScriptLoader
{
    public:
        spell_nexus_critter_targeting() : SpellScriptLoader("spell_nexus_critter_targeting") { }

        class spell_nexus_critter_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nexus_critter_targeting_SpellScript);

            void FilterTargetsInitial(std::list<WorldObject*>& unitList)
            {
                if (unitList.empty())
                    return;

                sharedUnitList = unitList;
            }

            void FilterTargetsSubsequent(std::list<WorldObject*>& unitList)
            {
                unitList = sharedUnitList;
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nexus_critter_targeting_SpellScript::FilterTargetsInitial, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nexus_critter_targeting_SpellScript::FilterTargetsSubsequent, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nexus_critter_targeting_SpellScript::FilterTargetsSubsequent, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
            }

            std::list<WorldObject*> sharedUnitList;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nexus_critter_targeting_SpellScript();
        }
};

void AddSC_boss_magus_telestra()
{
    new boss_magus_telestra();
    new boss_magus_telestra_arcane();
    new spell_nexus_critter_targeting();
}
