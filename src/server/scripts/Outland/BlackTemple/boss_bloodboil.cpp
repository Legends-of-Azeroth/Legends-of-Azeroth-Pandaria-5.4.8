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
#include "black_temple.h"

enum Bloodboil
{
    // Speech'n'Sound
    SAY_AGGRO                   = 0,
    SAY_SLAY                    = 1,
    SAY_SPECIAL                 = 2,
    SAY_ENRAGE                  = 3,

    // Spells
    SPELL_ACID_GEYSER           = 40630,
    SPELL_ACIDIC_WOUND          = 40481,
    SPELL_ARCING_SMASH          = 40599,
    SPELL_BLOODBOIL             = 42005, // This spell is AoE whereas it shouldn't be
    SPELL_FEL_ACID              = 40508,
    SPELL_FEL_RAGE_SELF         = 40594,
    SPELL_FEL_RAGE_TARGET       = 40604,
    SPELL_FEL_RAGE_2            = 40616,
    SPELL_FEL_RAGE_3            = 41625,
    SPELL_FEL_RAGE_SCALE        = 46787,
    SPELL_BEWILDERING_STRIKE    = 40491,
    SPELL_EJECT1                = 40486, // 1000 Physical damage + knockback + script effect (should handle threat reduction I think)
    SPELL_EJECT2                = 40597, // 1000 Physical damage + Stun (used in phase 2?)
    SPELL_TAUNT_GURTOGG         = 40603,
    SPELL_INSIGNIFIGANCE        = 40618,
    SPELL_BERSERK               = 45078,

    SPELL_FEL_GEYSER_DAMAGE     = 40593,
    SPELL_FEL_GEYSER            = 40569,
};

class boss_gurtogg_bloodboil : public CreatureScript
{
    public:
        boss_gurtogg_bloodboil() : CreatureScript("boss_gurtogg_bloodboil") { }

        struct boss_gurtogg_bloodboilAI : public ScriptedAI
        {
            boss_gurtogg_bloodboilAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            ObjectGuid TargetGUID;

            float TargetThreat;

            uint32 BloodboilTimer;
            uint32 BloodboilCount;
            uint32 AcidGeyserTimer;
            uint32 AcidicWoundTimer;
            uint32 ArcingSmashTimer;
            uint32 EnrageTimer;
            uint32 FelAcidTimer;
            uint32 EjectTimer;
            uint32 BewilderingStrikeTimer;
            uint32 PhaseChangeTimer;

            bool Phase1;

            void Reset() override
            {
                if (instance)
                    instance->SetData(DATA_GURTOGG_BLOOD_BOIL_EVENT, NOT_STARTED);

                TargetGUID = ObjectGuid::Empty;

                TargetThreat = 0;

                BloodboilTimer = 10000;
                BloodboilCount = 0;
                AcidGeyserTimer = 1000;
                AcidicWoundTimer = 6000;
                ArcingSmashTimer = 19000;
                EnrageTimer = 600000;
                FelAcidTimer = 25000;
                EjectTimer = 10000;
                BewilderingStrikeTimer = 15000;
                PhaseChangeTimer = 60000;

                Phase1 = true;

                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, false);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!instance || !instance->CheckRequiredBosses(DATA_GURTOGG_BLOOD_BOIL_EVENT, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->SetData(DATA_KICK_PLAYERS, 1);
                    return;
                }

                DoZoneInCombat();
                Talk(SAY_AGGRO);
                if (instance)
                    instance->SetData(DATA_GURTOGG_BLOOD_BOIL_EVENT, IN_PROGRESS);
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_SLAY);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                    instance->SetData(DATA_GURTOGG_BLOOD_BOIL_EVENT, DONE);

                DoPlaySoundToSet(me, 11439);
            }

            void RevertThreatOnTarget(ObjectGuid guid)
            {
                Unit* unit = nullptr;
                unit = Unit::GetUnit(*me, guid);
                if (unit)
                {
                    if (DoGetThreat(unit))
                        DoModifyThreatPercent(unit, -100);
                    if (TargetThreat)
                        me->GetThreatManager().AddThreat(unit, TargetThreat);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (ArcingSmashTimer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_ARCING_SMASH);
                    ArcingSmashTimer = 10000;
                } else ArcingSmashTimer -= diff;

                if (FelAcidTimer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_FEL_ACID);
                    FelAcidTimer = 25000;
                } else FelAcidTimer -= diff;

                if (!me->HasAura(SPELL_BERSERK))
                {
                    if (EnrageTimer <= diff)
                    {
                        DoCast(me, SPELL_BERSERK);
                        Talk(SAY_ENRAGE);
                    } else EnrageTimer -= diff;
                }

                if (Phase1)
                {
                    if (BewilderingStrikeTimer <= diff)
                    {
                        DoCast(me->GetVictim(), SPELL_BEWILDERING_STRIKE);
                        float mt_threat = DoGetThreat(me->GetVictim());
                        if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 1))
                            me->GetThreatManager().AddThreat(target, mt_threat);
                        BewilderingStrikeTimer = 20000;
                    } else BewilderingStrikeTimer -= diff;

                    if (EjectTimer <= diff)
                    {
                        DoCast(me->GetVictim(), SPELL_EJECT1);
                        DoModifyThreatPercent(me->GetVictim(), -40);
                        EjectTimer = 15000;
                    } else EjectTimer -= diff;

                    if (AcidicWoundTimer <= diff)
                    {
                        //DoCast(me->GetVictim(), SPELL_ACIDIC_WOUND);
                        DoCast(me, SPELL_ACIDIC_WOUND);
                        AcidicWoundTimer = 10000;
                    } else AcidicWoundTimer -= diff;

                    if (BloodboilTimer <= diff)
                    {
                        if (BloodboilCount < 5)                      // Only cast it five times.
                        {
                            DoCastAOE(SPELL_BLOODBOIL);
                            ++BloodboilCount;
                            BloodboilTimer = 10000*BloodboilCount;
                        }
                    } else BloodboilTimer -= diff;
                }

                if (!Phase1)
                {
                    if (AcidGeyserTimer <= diff)
                    {
                        DoCast(me->GetVictim(), SPELL_ACID_GEYSER);
                        AcidGeyserTimer = 30000;
                    } else AcidGeyserTimer -= diff;

                    if (EjectTimer <= diff)
                    {
                        DoCast(me->GetVictim(), SPELL_EJECT2);
                        EjectTimer = 15000;
                    } else EjectTimer -= diff;
                }

                if (PhaseChangeTimer <= diff)
                {
                    if (Phase1)
                    {
                        Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true);
                        if (target && target->IsAlive())
                        {
                            Phase1 = false;

                            TargetThreat = DoGetThreat(target);
                            TargetGUID = target->GetGUID();
                            target->CastSpell(me, SPELL_TAUNT_GURTOGG, true);
                            if (DoGetThreat(target))
                                DoModifyThreatPercent(target, -100);
                            me->GetThreatManager().AddThreat(target, 50000000.0f);
                            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                            me->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, true);
                                                                    // If VMaps are disabled, this spell can call the whole instance
                            DoCast(me, SPELL_INSIGNIFIGANCE, true);
                            DoCast(target, SPELL_FEL_RAGE_TARGET, true);
                            DoCast(target, SPELL_FEL_RAGE_2, true);
                            DoCast(target, SPELL_FEL_RAGE_3, true);
                            DoCast(target, SPELL_FEL_RAGE_SCALE, true);
                            DoCast(target,SPELL_FEL_GEYSER,true);

                            // Cast this without triggered so that it appears in combat logs and shows visual.
                            DoCast(me, SPELL_FEL_RAGE_SELF);

                            Talk(SAY_SPECIAL);

                            AcidGeyserTimer = 1000;
                            PhaseChangeTimer = 30000;
                        }
                    }
                    else // Encounter is a loop pretty much. Phase 1 -> Phase 2 -> Phase 1 -> Phase 2 till death or enrage
                    {
                        if (TargetGUID)
                            RevertThreatOnTarget(TargetGUID);
                        TargetGUID = ObjectGuid::Empty;
                        Phase1 = true;
                        BloodboilTimer = 10000;
                        BloodboilCount = 0;
                        AcidicWoundTimer += 2000;
                        ArcingSmashTimer += 2000;
                        FelAcidTimer += 2000;
                        EjectTimer += 2000;
                        PhaseChangeTimer = 60000;
                        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, false);
                    }
                } else PhaseChangeTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_gurtogg_bloodboilAI(creature);
        }
};

class npc_fel_geyser : public CreatureScript
{
    public:
        npc_fel_geyser() : CreatureScript("npc_fel_geyser") { }

        struct npc_fel_geyserAI : public ScriptedAI
        {
            npc_fel_geyserAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            uint32 damage_timer;

            void Reset() override
            {
                damage_timer = 1000;
            }

            void UpdateAI(uint32 diff) override
            {
                if (damage_timer < diff)
                {
                    DoCastAOE(SPELL_FEL_GEYSER_DAMAGE);
                    damage_timer = 60000;
                }else damage_timer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_fel_geyserAI(creature);
        }
};

class spell_bloodboil : public SpellScriptLoader
{
    public:
        spell_bloodboil() : SpellScriptLoader("spell_bloodboil") { }

        class spell_bloodboil_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_bloodboil_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (targets.size() > 5)
                {
                    targets.sort(Trinity::ObjectDistanceOrderPred(GetCaster(), false));
                    targets.resize(5);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_bloodboil_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_bloodboil_SpellScript();
        }
};

void AddSC_boss_gurtogg_bloodboil()
{
    new boss_gurtogg_bloodboil();
    new npc_fel_geyser();
    new spell_bloodboil();
}
