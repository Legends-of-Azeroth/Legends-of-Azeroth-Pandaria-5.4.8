/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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
#include "gundrak.h"

enum Texts
{
    // Drakkari Elemental
    EMOTE_MOJO                                    = 0,
    EMOTE_ACTIVATE_ALTAR                          = 1
};

enum Spells
{
    SPELL_EMERGE                                  = 54850,
    SPELL_EMERGE_SUMMON                           = 54851,
    SPELL_MIGHTY_BLOW                             = 54719,
    SPELL_MORTAL_STRIKES                          = 54715,
    SPELL_MORTAL_STRIKES_H                        = 59454,
    SPELL_MERGE                                   = 54878,
    SPELL_SURGE_VISUAL                            = 54827,
    SPELL_SURGE                                   = 54801,
    SPELL_SURGE_TARGETING                         = 54816,
    SPELL_FREEZE_ANIM                             = 16245,
    SPELL_MOJO_VOLLEY                             = 54849,
    SPELL_MOJO_VOLLEY_H                           = 59453,
    SPELL_MOJO_PUDDLE                             = 55627,
    SPELL_MOJO_WAVE                               = 55626,
    SPELL_ELEMENTAL_SPAWN_EFFECT                  = 54888,
    SPELL_COSMETIC_STUN_PERMANENT_FREEZE_ANIM     = 52656,
    SPELL_FACE_ME                                 = 54991,
    SPELL_DRENCHED_IN_MOJO                        = 54819,
    SPELL_DRENCHED_IN_MOJO_CLEAR                  = 59461,
};

enum Creatures
{
    NPC_MOJO                                      = 29830
};

enum Action
{
    ACTION_START = 1,
    ACTION_UNFREEZE,
    DATA_EMERGED
};

static Position SpawnLoc[] =
{
    { 1669.98f, 753.686f, 143.074f, 4.95674f },
    { 1680.67f, 737.104f, 143.083f, 2.53073f },
    { 1680.63f, 750.682f, 143.074f, 3.87463f },
    { 1663.1f,  743.665f, 143.078f, 6.19592f },
    { 1670.39f, 733.493f, 143.073f, 1.27409f }
};

class boss_drakkari_colossus : public CreatureScript
{
    public:
        boss_drakkari_colossus() : CreatureScript("boss_drakkari_colossus") { }

        struct boss_drakkari_colossusAI : public ScriptedAI
        {
            boss_drakkari_colossusAI(Creature* creature) : ScriptedAI(creature)
            {
                _instance = creature->GetInstanceScript();

                for (uint8 i = 0; i < 5; ++i)
                    _mojoGUID[i] = ObjectGuid::Empty;
            }

            void Reset() override
            {
                if (_instance)
                    _instance->SetData(DATA_DRAKKARI_COLOSSUS_EVENT, NOT_STARTED);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                DoDespawnMojo();
                DoSpawnMojo();

                _phase = 0;
                _mightyBlowTimer = 10 * IN_MILLISECONDS;
            }

            void JustReachedHome() override
            {
                DoCast(me, SPELL_FREEZE_ANIM);
            }

            void DoSpawnMojo()
            {
                started = false;
                for (uint8 i = 0; i < 5; ++i)
                    if (Creature* mojo = me->SummonCreature(NPC_MOJO, SpawnLoc[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, Milliseconds(1 * IN_MILLISECONDS)))
                        _mojoGUID[i] = mojo->GetGUID();
            }

            void DoDespawnMojo()
            {
                for (uint8 i = 0; i < 5; ++i)
                {
                    if (_mojoGUID[i])
                        if (Creature* mojo = ObjectAccessor::GetCreature(*me, _mojoGUID[i]))
                            if (mojo->IsAlive())
                                mojo->DespawnOrUnsummon();
                    _mojoGUID[i] = ObjectGuid::Empty;
                }
            }

            void DoMoveMojo()
            {
                for (uint8 i = 0; i < 5; ++i)
                    if (_mojoGUID[i])
                    {
                        Creature* mojo = Unit::GetCreature(*me, _mojoGUID[i]);
                        if (mojo && mojo->IsAlive())
                        {
                            mojo->SetReactState(REACT_PASSIVE);
                            mojo->GetMotionMaster()->Clear();
                            mojo->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                        }
                    }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                ScriptedAI::EnterEvadeMode();
                me->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM); // Will be applied in JustReachedHome
            }

            void AttackStart(Unit* who) override
            {
                if (!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                    ScriptedAI::AttackStart(who);
            }

            uint32 GetData(uint32 data) const override
            {
                if (data == DATA_EMERGED)
                    return (_phase == 1) ? 1 : 0;

                return 0;
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_START:
                        if (started)
                            return;
                        started = true;
                        DoMoveMojo();
                        me->m_Events.Schedule(2000, [this]()
                        {
                            DoDespawnMojo();
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                            me->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                        });
                        break;
                    case ACTION_UNFREEZE:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        me->RemoveAurasDueToSpell(SPELL_COSMETIC_STUN_PERMANENT_FREEZE_ANIM);
                        if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            AttackStart(target);
                        break;
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (_instance)
                    _instance->SetData(DATA_DRAKKARI_COLOSSUS_EVENT, IN_PROGRESS);

                _phase = 0;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (damage >= me->GetHealth())
                    damage = me->GetHealth() - 1;

                if (_phase == 0 && me->HealthBelowPctDamaged(50, damage) || _phase == 1 && me->HealthBelowPctDamaged(5, damage))
                {
                    ++_phase;
                    me->RemoveAllAuras();
                    me->AddAura(DUNGEON_MODE(SPELL_MORTAL_STRIKES, SPELL_MORTAL_STRIKES_H), me);
                    me->InterruptNonMeleeSpells(false);
                    me->SetFacingTo(me->GetOrientation());
                    DoCast(me, SPELL_EMERGE_SUMMON);
                    DoCast(me, SPELL_EMERGE);
                    me->AttackStop();
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (_instance)
                    _instance->SetData(DATA_DRAKKARI_COLOSSUS_EVENT, DONE);
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() != NPC_DRAKKARI_ELEMENTAL)
                    return;

                if (_phase == 2)
                    summon->SetHealth(summon->CountPctFromMaxHealth(50));

                DoZoneInCombat(summon);
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_EMERGE)
                {
                    DoCast(me, SPELL_COSMETIC_STUN_PERMANENT_FREEZE_ANIM);
                    DoCast(me, SPELL_FACE_ME);
                }
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_FACE_ME)
                    target->SetFacingToObject(me);
            }

            void UpdateAI(uint32 diff) override
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE) || !UpdateVictim())
                    return;

                if (_mightyBlowTimer <= diff)
                {
                    DoCastVictim(SPELL_MIGHTY_BLOW);
                    _mightyBlowTimer = urand(7000, 12000);
                }
                else
                    _mightyBlowTimer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            InstanceScript* _instance;
            uint8 _phase;
            uint32 _mightyBlowTimer;
            ObjectGuid _mojoGUID[5];
            bool started = false;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_drakkari_colossusAI(creature);
        }
};

class boss_drakkari_elemental : public CreatureScript
{
    public:
        boss_drakkari_elemental() : CreatureScript("boss_drakkari_elemental") { }

        struct boss_drakkari_elementalAI : public ScriptedAI
        {
            boss_drakkari_elementalAI(Creature* creature) : ScriptedAI(creature)
            {
                _instance = creature->GetInstanceScript();
                DoCast(me, SPELL_ELEMENTAL_SPAWN_EFFECT);
                DoCast(me, 65208, true); // Self Stun (3 secs)
            }

            void Reset() override
            {
                _surgeTimer = 15000;
                _disappearTimer = 4000;
                _merging = false;
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                if (Creature* colossus = Unit::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_DRAKKARI_COLOSSUS) : ObjectGuid::Empty))
                    colossus->AI()->DoAction(ACTION_UNFREEZE);

                me->DespawnOrUnsummon();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoCast(me, DUNGEON_MODE<uint32>(SPELL_MOJO_VOLLEY, SPELL_MOJO_VOLLEY_H), true);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->RemoveAurasDueToSpell(DUNGEON_MODE<uint32>(SPELL_MOJO_VOLLEY, SPELL_MOJO_VOLLEY_H));

                DoCastAOE(SPELL_DRENCHED_IN_MOJO_CLEAR, true);

                Talk(EMOTE_ACTIVATE_ALTAR);

                if (Creature* colossus = Unit::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_DRAKKARI_COLOSSUS) : ObjectGuid::Empty))
                {
                    colossus->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                    colossus->LowerPlayerDamageReq(colossus->GetMaxHealth());
                    colossus->Kill(colossus);
                }
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_DRENCHED_IN_MOJO_CLEAR)
                    target->RemoveAurasDueToSpell(SPELL_DRENCHED_IN_MOJO);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (damage >= me->GetHealth())
                    if (Creature* colossus = Unit::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_DRAKKARI_COLOSSUS) : ObjectGuid::Empty))
                        if (colossus->AI()->GetData(DATA_EMERGED))
                            damage = me->GetHealth() - 1;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!_merging && !UpdateVictim())
                    return;

                if (!_merging && HealthBelowPct(50))
                    if (Creature* colossus = Unit::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_DRAKKARI_COLOSSUS) : ObjectGuid::Empty))
                        if (colossus->AI()->GetData(DATA_EMERGED))
                        {
                            me->AttackStop();
                            me->InterruptNonMeleeSpells(true);
                            me->StopMoving();
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveIdle();
                            DoCast(me, SPELL_SURGE_VISUAL);
                            colossus->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE); // Hack, SPELL_MERGE doesn't have SPELL_ATTR6_CAN_TARGET_UNTARGETABLE
                            DoCast(me, SPELL_MERGE);
                            colossus->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                            me->RemoveAurasDueToSpell(DUNGEON_MODE<uint32>(SPELL_MOJO_VOLLEY, SPELL_MOJO_VOLLEY_H));
                            _merging = true;
                        }

                if (_merging)
                {
                    if (_disappearTimer <= diff)
                    {
                        if (Creature* colossus = Unit::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_DRAKKARI_COLOSSUS) : ObjectGuid::Empty))
                            colossus->AI()->DoAction(ACTION_UNFREEZE);

                        me->DespawnOrUnsummon();
                    }
                    else
                        _disappearTimer -= diff;

                    return;
                }

                if (_surgeTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                    {
                        Talk(EMOTE_MOJO);
                        DoCast(me, SPELL_SURGE_VISUAL);
                        DoCast(target, SPELL_SURGE);
                        DoCastAOE(SPELL_SURGE_TARGETING, true);
                    }
                    _surgeTimer = RAND(7000, 11000, 16000); // Seems that way :D
                }
                else
                    _surgeTimer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            InstanceScript* _instance;
            uint32 _surgeTimer;
            uint32 _disappearTimer;
            bool _merging;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_drakkari_elementalAI(creature);
        }
};

class npc_living_mojo : public CreatureScript
{
    public:
        npc_living_mojo() : CreatureScript("npc_living_mojo") { }

        struct npc_living_mojoAI : public ScriptedAI
        {
            npc_living_mojoAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            void Reset() override
            {
                _mojoWaveTimer = 5 * IN_MILLISECONDS;
                _mojoPuddleTimer = 10 * IN_MILLISECONDS;
            }

            void AttackStart(Unit* who) override
            {
                if (!me->IsSummon() || !instance)
                {
                    ScriptedAI::AttackStart(who);
                    return;
                }

                if (Creature* boss = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_DRAKKARI_COLOSSUS)))
                    boss->AI()->DoAction(ACTION_START);
            }

            void UpdateAI(uint32 diff) override
            {
                if (me->IsSummon() || !UpdateVictim())
                    return;

                if (_mojoWaveTimer <= diff)
                {
                    DoCastVictim(SPELL_MOJO_WAVE);
                    _mojoWaveTimer = 10 * IN_MILLISECONDS;
                } else _mojoWaveTimer -= diff;

                if (_mojoPuddleTimer <= diff)
                {
                    DoCastVictim(SPELL_MOJO_PUDDLE);
                    _mojoPuddleTimer = DUNGEON_MODE<uint32>(20 * IN_MILLISECONDS, 10 * IN_MILLISECONDS);
                } else _mojoPuddleTimer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            InstanceScript* instance;
            uint32 _mojoWaveTimer;
            uint32 _mojoPuddleTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_living_mojoAI(creature);
        }
};

class spell_mojo_volley_targeting : public SpellScriptLoader
{
    public:
        spell_mojo_volley_targeting() : SpellScriptLoader("spell_mojo_volley_targeting") { }

        class spell_mojo_volley_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mojo_volley_targeting_SpellScript);

            void FilterTargetsInitial(std::list<WorldObject*>& unitList)
            {
                sharedUnitList = unitList;
            }

            void FilterTargetsSubsequent(std::list<WorldObject*>& unitList)
            {
                unitList = sharedUnitList;
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mojo_volley_targeting_SpellScript::FilterTargetsInitial, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mojo_volley_targeting_SpellScript::FilterTargetsSubsequent, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            }

            std::list<WorldObject*> sharedUnitList;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mojo_volley_targeting_SpellScript();
        }
};

class spell_mojo_volley_trigger : public SpellScriptLoader
{
    public:
        spell_mojo_volley_trigger() : SpellScriptLoader("spell_mojo_volley_trigger") { }

        class spell_mojo_volley_trigger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mojo_volley_trigger_AuraScript);

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
                uint32 triggerSpellId = GetSpellInfo()->Effects[EFFECT_0].TriggerSpell;

                if (Unit* caster = GetCaster())
                    caster->CastCustomSpell(triggerSpellId, SPELLVALUE_MAX_TARGETS, irand(1, 2), caster, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mojo_volley_trigger_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mojo_volley_trigger_AuraScript();
        }
};

class spell_surge : public SpellScriptLoader
{
    public:
        spell_surge() : SpellScriptLoader("spell_surge") { }

        class spell_surge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_surge_SpellScript);

            void HandleCast()
            {
                if (Unit* caster = GetCaster())
                    if (InstanceScript* instance = caster->GetInstanceScript())
                        caster->CastSpell(caster, SPELL_DRENCHED_IN_MOJO, true, NULL, NULL, instance->GetGuidData(DATA_DRAKKARI_COLOSSUS));
            }

            void TargetSelect(SpellDestination& dest)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                Position pos = caster->GetPosition();
                dest._position = WorldLocation(caster->GetMapId(), pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
                for (int32 dist = GetSpellInfo()->Effects[EFFECT_1].CalcRadius(caster); dist > 0; dist -= 5)
                    caster->MovePositionToFirstCollision(dest._position, 5.0f, 0);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_surge_SpellScript::HandleCast);
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_surge_SpellScript::TargetSelect, EFFECT_1, TARGET_DEST_CASTER_FRONT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_surge_SpellScript();
        }
};

void AddSC_boss_drakkari_colossus()
{
    new boss_drakkari_colossus();
    new boss_drakkari_elemental();
    new npc_living_mojo();
    new spell_mojo_volley_targeting();
    new spell_mojo_volley_trigger();
    new spell_surge();
}
