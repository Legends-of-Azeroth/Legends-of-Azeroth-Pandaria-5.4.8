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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellAuras.h"
#include "icecrown_citadel.h"

enum ScriptTexts
{
    SAY_STINKY_DEAD             = 0,
    SAY_AGGRO                   = 1,
    EMOTE_GAS_SPORE             = 2,
    EMOTE_WARN_GAS_SPORE        = 3,
    SAY_PUNGENT_BLIGHT          = 4,
    EMOTE_WARN_PUNGENT_BLIGHT   = 5,
    SAY_KILL                    = 7,
    SAY_BERSERK                 = 8,
    SAY_DEATH                   = 9,
};

enum Spells
{
    // Festergut
    SPELL_INHALE_BLIGHT         = 69165,
    SPELL_PUNGENT_BLIGHT        = 69195,
    SPELL_GASTRIC_BLOAT         = 72219, // 72214 is the proper way (with proc) but atm procs can't have cooldown for creatures
    SPELL_GASTRIC_EXPLOSION     = 72227,
    SPELL_GAS_SPORE             = 69278,
    SPELL_VILE_GAS              = 69240,
    SPELL_INOCULATED            = 69291,

    // Stinky
    SPELL_MORTAL_WOUND          = 71127,
    SPELL_DECIMATE              = 71123,
    SPELL_PLAGUE_STENCH         = 71805,
};

uint32 const gaseousBlight[3]        = {69157, 69162, 69164};
uint32 const gaseousBlightVisual[3]  = {69126, 69152, 69154};

enum Events
{
    EVENT_BERSERK = 1,
    EVENT_INHALE_BLIGHT,
    EVENT_VILE_GAS,
    EVENT_GAS_SPORE,
    EVENT_GASTRIC_BLOAT,

    EVENT_DECIMATE,
};

#define DATA_INOCULATED_STACK 69291

class boss_festergut : public CreatureScript
{
    public:
        boss_festergut() : CreatureScript("boss_festergut") { }

        struct boss_festergutAI : public BossAI
        {
            boss_festergutAI(Creature* creature) : BossAI(creature, DATA_FESTERGUT)
            {
                //_fuckup = 0;
                _maxInoculatedStack = 0;
                _inhaleCounter = 0;
                _gasDummyGUID = ObjectGuid::Empty;
            }

            void Reset() override
            {
                _Reset();
                events.ScheduleEvent(EVENT_BERSERK, 300000);
                events.ScheduleEvent(EVENT_INHALE_BLIGHT, urand(25000, 30000));
                events.ScheduleEvent(EVENT_GAS_SPORE, urand(20000, 25000));
                events.ScheduleEvent(EVENT_GASTRIC_BLOAT, urand(12500, 15000));
                _maxInoculatedStack = 0;
                _inhaleCounter = 0;
                //_fuckup = 0;
                me->RemoveAurasDueToSpell(SPELL_BERSERK2);
                if (Creature* gasDummy = me->FindNearestCreature(NPC_GAS_DUMMY, 100.0f, true))
                {
                    _gasDummyGUID = gasDummy->GetGUID();
                    for (uint8 i = 0; i < 3; ++i)
                    {
                        me->RemoveAurasDueToSpell(gaseousBlight[i]);
                        gasDummy->RemoveAurasDueToSpell(gaseousBlightVisual[i]);
                    }
                }

                me->GetMap()->SetWorldState(WORLDSTATE_FLU_SHOT_SHORTAGE, 1);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!instance->CheckRequiredBosses(DATA_FESTERGUT, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoCastSpellOnPlayers(LIGHT_S_HAMMER_TELEPORT);
                    return;
                }

                me->setActive(true);
                Talk(SAY_AGGRO);
                if (Creature* gasDummy = me->FindNearestCreature(NPC_GAS_DUMMY, 100.0f, true))
                    _gasDummyGUID = gasDummy->GetGUID();
                if (Creature* professor = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_PROFESSOR_PUTRICIDE)))
                if (professor->IsAlive())
                    professor->AI()->DoAction(ACTION_FESTERGUT_COMBAT);
                DoZoneInCombat();
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->RemoveAurasDueToSpell(1130);
                me->RemoveAurasDueToSpell(14323);
                me->RemoveAurasDueToSpell(14324);
                me->RemoveAurasDueToSpell(14325);
                me->RemoveAurasDueToSpell(53338);
                  _JustDied();
                Talk(SAY_DEATH);
                if (Creature* professor = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_PROFESSOR_PUTRICIDE)))
                if (professor->IsAlive())
                    professor->AI()->DoAction(ACTION_FESTERGUT_DEATH);

              // Remove Gastric Bloat on all players
                if (instance)
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GASTRIC_BLOAT);
                RemoveBlight();
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                instance->SetBossState(DATA_FESTERGUT, FAIL);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                ScriptedAI::EnterEvadeMode();
                if (Creature* professor = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_PROFESSOR_PUTRICIDE)))
                    professor->AI()->EnterEvadeMode();
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_PUNGENT_BLIGHT)
                    target->RemoveAurasDueToSpell(SPELL_INOCULATED);
            }

            /*void MoveInLineOfSight(Unit* who) override
            {
                if (me->IsWithinDistInMap(who, 20.0f, false))
                {
                me->SetReactState(REACT_AGGRESSIVE);
                DoZoneInCombat(me, 20.0f);
                    //me->SetInCombatWithZone();
                }
            }*/

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || !CheckInRoom())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INHALE_BLIGHT:
                        {
                            RemoveBlight();
                            if (_inhaleCounter == 3)
                            {
                                Talk(EMOTE_WARN_PUNGENT_BLIGHT);
                                Talk(SAY_PUNGENT_BLIGHT);
                                DoCast(me, SPELL_PUNGENT_BLIGHT);
                                _inhaleCounter = 0;
                                if (Creature* professor = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_PROFESSOR_PUTRICIDE)))
                                 //if (professor->IsAlive())
                                 //{
                                 //    if (instance->GetBossState(DATA_FESTERGUT) != DONE);
                                    professor->AI()->DoAction(ACTION_FESTERGUT_GAS);
                                events.RescheduleEvent(EVENT_GAS_SPORE, urand(20000, 25000));
                                //}
                            }
                            else
                            {
                                DoCast(me, SPELL_INHALE_BLIGHT);
                                // just cast and dont bother with target, conditions will handle it
                                ++_inhaleCounter;
                                if (_inhaleCounter < 3)
                                    me->CastSpell(me, gaseousBlight[_inhaleCounter], true, nullptr, nullptr, me->GetGUID());
                            }

                            events.ScheduleEvent(EVENT_INHALE_BLIGHT, urand(33500, 35000));
                            break;
                        }
                        case EVENT_VILE_GAS:
                        {
                            std::list<Unit*> ranged, melee;
                            uint32 minTargets = RAID_MODE<uint32>(3, 8, 3, 8);
                            SelectTargetList(ranged, 25, SELECT_TARGET_RANDOM, -5.0f, true);
                            SelectTargetList(melee, 25, SELECT_TARGET_RANDOM, 5.0f, true);
                            while (ranged.size() < minTargets)
                            {
                                if (melee.empty())
                                    break;

                                Unit* target = Trinity::Containers::SelectRandomContainerElement(melee);
                                ranged.push_back(target);
                                melee.remove(target);
                            }

                            if (!ranged.empty())
                            {
                                Trinity::Containers::RandomResizeList(ranged, RAID_MODE<uint32>(1, 3, 1, 3));
                                for (std::list<Unit*>::iterator itr = ranged.begin(); itr != ranged.end(); ++itr)
                                    DoCast(*itr, SPELL_VILE_GAS);
                            }

                            events.ScheduleEvent(EVENT_VILE_GAS, urand(28000, 35000));
                            break;
                        }
                        case EVENT_GAS_SPORE:
                            Talk(EMOTE_WARN_GAS_SPORE);
                            Talk(EMOTE_GAS_SPORE);
                            me->CastCustomSpell(SPELL_GAS_SPORE, SPELLVALUE_MAX_TARGETS, RAID_MODE<int32>(2, 3, 2, 3), me);
                            events.ScheduleEvent(EVENT_GAS_SPORE, urand(40000, 45000));
                            events.RescheduleEvent(EVENT_VILE_GAS, urand(28000, 35000));
                            break;
                        case EVENT_GASTRIC_BLOAT:
                            DoCastVictim(SPELL_GASTRIC_BLOAT);
                            events.ScheduleEvent(EVENT_GASTRIC_BLOAT, urand(15000, 17500));
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK2);
                            Talk(SAY_BERSERK);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_INOCULATED_STACK && data > _maxInoculatedStack)
                {
                    _maxInoculatedStack = data;

                    if (data > 2) // fail
                        me->GetMap()->SetWorldState(WORLDSTATE_FLU_SHOT_SHORTAGE, 0);
                }
                //if (_maxInoculatedStack == 3)
                //    _fuckup = 3;
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_INOCULATED_STACK)
                    return uint32(_maxInoculatedStack);

                return 0;
            }

            void RemoveBlight()
            {
                if (Creature* gasDummy = ObjectAccessor::GetCreature(*me, _gasDummyGUID))
                    for (uint8 i = 0; i < 3; ++i)
                    {
                        me->RemoveAurasDueToSpell(gaseousBlight[i]);
                        gasDummy->RemoveAurasDueToSpell(gaseousBlightVisual[i]);
                    }
            }

        private:
            ObjectGuid _gasDummyGUID;
            uint32 _maxInoculatedStack;
            uint32 _inhaleCounter;
            //uint8 _fuckup;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<boss_festergutAI>(creature);
        }
};

class npc_stinky_icc : public CreatureScript
{
    public:
        npc_stinky_icc() : CreatureScript("npc_stinky_icc") { }

        struct npc_stinky_iccAI : public ScriptedAI
        {
            npc_stinky_iccAI(Creature* creature) : ScriptedAI(creature)
            {
                _instance = creature->GetInstanceScript();
            }

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_DECIMATE, urand(20000, 25000));
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoCast(me, SPELL_PLAGUE_STENCH);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

               _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DECIMATE:
                            DoCastVictim(SPELL_DECIMATE);
                            _events.ScheduleEvent(EVENT_DECIMATE, urand(20000, 25000));
                            break;
                        default:
                            break;
                    }
                }

                if (!me->GetCurrentSpell(CURRENT_MELEE_SPELL))
                    DoCastVictim(SPELL_MORTAL_WOUND);

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* festergut = me->GetCreature(*me, _instance->GetGuidData(DATA_FESTERGUT)))
                    if (festergut->IsAlive())
                        festergut->AI()->Talk(SAY_STINKY_DEAD);
            }

        private:
            EventMap _events;
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_stinky_iccAI>(creature);
        }
};

class spell_festergut_pungent_blight : public SpellScriptLoader
{
    public:
        spell_festergut_pungent_blight() : SpellScriptLoader("spell_festergut_pungent_blight") { }

        class spell_festergut_pungent_blight_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_festergut_pungent_blight_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_UNIT;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                // remove it
                GetCaster()->RemoveAurasDueToSpell(uint32(GetEffectValue()));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_festergut_pungent_blight_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_festergut_pungent_blight_SpellScript();
        }
};

class spell_festergut_gastric_bloat : public SpellScriptLoader
{
    public:
        spell_festergut_gastric_bloat() : SpellScriptLoader("spell_festergut_gastric_bloat") { }

        class spell_festergut_gastric_bloat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_festergut_gastric_bloat_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_GASTRIC_EXPLOSION))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Aura const* aura = GetHitUnit()->GetAura(GetSpellInfo()->Id);
                if (!(aura && aura->GetStackAmount() == 10))
                    return;

                GetHitUnit()->RemoveAurasDueToSpell(GetSpellInfo()->Id);
                GetHitUnit()->CastSpell(GetHitUnit(), SPELL_GASTRIC_EXPLOSION, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_festergut_gastric_bloat_SpellScript::HandleScript, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_festergut_gastric_bloat_SpellScript();
        }
};

class spell_festergut_blighted_spores : public SpellScriptLoader
{
    public:
        spell_festergut_blighted_spores() : SpellScriptLoader("spell_festergut_blighted_spores") { }

        class spell_festergut_blighted_spores_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_festergut_blighted_spores_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_INOCULATED))
                    return false;
                return true;
            }

            void ExtraEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->CastSpell(GetTarget(), SPELL_INOCULATED, true);
                if (InstanceScript* instance = GetTarget()->GetInstanceScript())
                    if (Creature* festergut = ObjectAccessor::GetCreature(*GetTarget(), instance->GetGuidData(DATA_FESTERGUT)))
                        festergut->AI()->SetData(DATA_INOCULATED_STACK, GetTarget()->GetAuraCount(SPELL_INOCULATED));
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_festergut_blighted_spores_AuraScript::ExtraEffect, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_festergut_blighted_spores_AuraScript();
        }
};

void AddSC_boss_festergut()
{
    new boss_festergut();
    new npc_stinky_icc();
    new spell_festergut_pungent_blight();
    new spell_festergut_gastric_bloat();
    new spell_festergut_blighted_spores();
}
