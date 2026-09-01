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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "ulduar.h"
#include "Vehicle.h"

enum Yells
{
    SAY_AGGRO       = 0,
    SAY_SLAY        = 1,
    SAY_DEATH       = 2,
    SAY_SUMMON      = 3,
    SAY_SLAG_POT    = 4,
    SAY_SCORCH      = 5,
    SAY_BERSERK     = 6,

    EMOTE_JETS      = 7,
};

enum Spells
{
    // Ignis
    SPELL_FLAME_JETS            = 62680,
    SPELL_SCORCH                = 62546,
    SPELL_SLAG_POT              = 62717,
    SPELL_SLAG_POT_DAMAGE_N     = 65722,
    SPELL_SLAG_POT_DAMAGE_H     = 65723,
    SPELL_SLAG_IMBUED           = 62836,
    SPELL_SLAG_IMBUED_25        = 63536,
    SPELL_ACTIVATE_CONSTRUCT    = 62488,
    SPELL_STRENGHT              = 64473,
    SPELL_GRAB                  = 62707,
    SPELL_BERSERK               = 47008,

    // Iron Construct
    SPELL_FREEZE_ANIM           = 63354,
    SPELL_HEAT                  = 65667,
    SPELL_MOLTEN                = 62373,
    SPELL_BRITTLE               = 62382,
    SPELL_SHATTER               = 62383,
    SPELL_GROUND                = 62548,
};


enum Events
{
    EVENT_FLAME_JETS    = 1,
    EVENT_SCORCH        = 2,
    EVENT_SLAG_POT      = 3,
    EVENT_GRAB_POT      = 4,
    EVENT_CHANGE_POT    = 5,
    EVENT_END_POT       = 6,
    EVENT_CONSTRUCT     = 7,
    EVENT_BERSERK       = 8,
    EVENT_SCORCH_SPAWN,
    EVENT_SCORCH_END,
};

enum Actions
{
    ACTION_REMOVE_BUFF  = 20,
};

enum AchievementData
{
    ACHIEVEMENT_IGNIS_START_EVENT   = 20951,
};

#define CONSTRUCT_SPAWN_POINTS 20

Position const ConstructSpawnPosition[CONSTRUCT_SPAWN_POINTS] =
{
    {630.366f, 216.772f, 360.891f, 3.001970f},
    {630.594f, 231.846f, 360.891f, 3.124140f},
    {630.435f, 337.246f, 360.886f, 3.211410f},
    {630.493f, 313.349f, 360.886f, 3.054330f},
    {630.444f, 321.406f, 360.886f, 3.124140f},
    {630.366f, 247.307f, 360.888f, 3.211410f},
    {630.698f, 305.311f, 360.886f, 3.001970f},
    {630.500f, 224.559f, 360.891f, 3.054330f},
    {630.668f, 239.840f, 360.890f, 3.159050f},
    {630.384f, 329.585f, 360.886f, 3.159050f},
    {543.220f, 313.451f, 360.886f, 0.104720f},
    {543.356f, 329.408f, 360.886f, 6.248280f},
    {543.076f, 247.458f, 360.888f, 6.213370f},
    {543.117f, 232.082f, 360.891f, 0.069813f},
    {543.161f, 305.956f, 360.886f, 0.157080f},
    {543.277f, 321.482f, 360.886f, 0.052360f},
    {543.316f, 337.468f, 360.886f, 6.195920f},
    {543.280f, 239.674f, 360.890f, 6.265730f},
    {543.265f, 217.147f, 360.891f, 0.174533f},
    {543.256f, 224.831f, 360.891f, 0.122173f},
};

class boss_ignis : public CreatureScript
{
    public:
        boss_ignis() : CreatureScript("boss_ignis") { }

        struct boss_ignis_AI : public BossAI
        {
            boss_ignis_AI(Creature* creature) : BossAI(creature, BOSS_IGNIS)
            {
                ASSERT(me->GetVehicleKit());
            }

            void Reset() override
            {
                _Reset();

                if (Unit* slagPotTarget = ObjectAccessor::GetUnit(*me, _slagPotGUID))
                {
                    slagPotTarget->ExitVehicle();
                    slagPotTarget->RemoveAurasDueToSpell(SPELL_SLAG_POT);
                    _slagPotGUID = ObjectGuid::Empty;
                }

                ClearSummons(false);
                for (uint8 i = 0; i < CONSTRUCT_SPAWN_POINTS; ++i)
                    if (Creature* construct = me->SummonCreature(NPC_IRON_CONSTRUCT, ConstructSpawnPosition[i]))
                        _creatureList.push_back(construct->GetGUID());

                instance->DoStopTimedAchievement(CRITERIA_START_TYPE_EVENT, ACHIEVEMENT_IGNIS_START_EVENT);
            }

            void ClearSummons(bool onBossDie)
            {
                while (!_creatureList.empty())
                {
                    Creature* summon = Unit::GetCreature(*me, *(_creatureList.begin()));
                    if (!summon)
                        _creatureList.erase(_creatureList.begin());
                    else
                    {
                        _creatureList.erase(_creatureList.begin());
                        if (!onBossDie)
                            summon->DespawnOrUnsummon();
                        else
                            me->Kill(summon);
                    }
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_FLAME_JETS, 30000);
                events.ScheduleEvent(EVENT_SCORCH, 25000);
                events.ScheduleEvent(EVENT_SLAG_POT, 35000);
                events.ScheduleEvent(EVENT_CONSTRUCT, 15000);
                events.ScheduleEvent(EVENT_END_POT, 40000);
                events.ScheduleEvent(EVENT_BERSERK, 480000);
                _slagPotGUID = ObjectGuid::Empty;
                me->GetMap()->SetWorldState(WORLDSTATE_SHATTERED, 0);
                _firstConstructKill = 0;
                instance->DoStartCriteria(CRITERIA_START_TYPE_EVENT, ACHIEVEMENT_IGNIS_START_EVENT);
            }

            void JustDied(Unit* killer) override
            {
                if (Unit* slagPotTarget = ObjectAccessor::GetUnit(*me, _slagPotGUID))
                {
                    slagPotTarget->ExitVehicle();
                    slagPotTarget->RemoveAurasDueToSpell(SPELL_SLAG_POT);
                    _slagPotGUID = ObjectGuid::Empty;
                }
                
                ClearSummons(true);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FLAME_JETS);

                _JustDied();
                Talk(SAY_DEATH);
            }

            void KilledUnit(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_SLAY);
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_GROUND_SCORCH)
                    summons.Summon(summon);
            }

            void DoAction(int32 action) override
            {
                if (action != ACTION_REMOVE_BUFF)
                    return;

                me->RemoveAuraFromStack(SPELL_STRENGHT);
                // Shattered Achievement
                time_t secondKill = sWorld->GetGameTime();
                if ((secondKill - _firstConstructKill) < 5)
                    me->GetMap()->SetWorldState(WORLDSTATE_SHATTERED, 1);
                _firstConstructKill = secondKill;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FLAME_JETS:
                            Talk(EMOTE_JETS);
                            DoCast(me, SPELL_FLAME_JETS);
                            events.ScheduleEvent(EVENT_FLAME_JETS, urand(35000, 40000));
                            break;
                        case EVENT_SLAG_POT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me, true)))
                            {
                                Talk(SAY_SLAG_POT);
                                _slagPotGUID = target->GetGUID();
                                DoCast(target, SPELL_GRAB);
                                events.DelayEvents(3000);
                                events.ScheduleEvent(EVENT_GRAB_POT, 500);
                            }
                            events.ScheduleEvent(EVENT_SLAG_POT, RAID_MODE(30000, 15000));
                            break;
                        case EVENT_GRAB_POT:
                            if (Unit* slagPotTarget = ObjectAccessor::GetUnit(*me, _slagPotGUID))
                            {
                                slagPotTarget->EnterVehicle(me, 0);
                                events.ScheduleEvent(EVENT_CHANGE_POT, 1000);
                            }
                            break;
                        case EVENT_CHANGE_POT:
                            if (Unit* slagPotTarget = ObjectAccessor::GetUnit(*me, _slagPotGUID))
                            {
                                slagPotTarget->ChangeSeat(1);
                                DoCast(slagPotTarget, SPELL_SLAG_POT);
                                events.ScheduleEvent(EVENT_END_POT, 10000);
                            }
                            break;
                        case EVENT_END_POT:
                            if (Unit* slagPotTarget = ObjectAccessor::GetUnit(*me, _slagPotGUID))
                            {
                                slagPotTarget->ExitVehicle();
                                slagPotTarget = nullptr;
                                _slagPotGUID = ObjectGuid::Empty;
                            }
                            break;
                        case EVENT_SCORCH:
                            Talk(SAY_SCORCH);
                            me->SetReactState(REACT_PASSIVE);
                            me->AttackStop();
                            DoCast(SPELL_SCORCH);
                            events.ScheduleEvent(EVENT_SCORCH_SPAWN, 2000);
                            events.ScheduleEvent(EVENT_SCORCH_END, 4000);
                            events.ScheduleEvent(EVENT_SCORCH, 25000);
                            break;
                        case EVENT_SCORCH_SPAWN:
                        {
                            Position pos = me->GetFirstCollisionPosition(me->GetObjectSize(), 0.0f);
                            me->SummonCreature(NPC_GROUND_SCORCH, pos, TEMPSUMMON_TIMED_DESPAWN, 45000ms);
                            break;
                        }
                        case EVENT_SCORCH_END:
                            me->SetReactState(REACT_AGGRESSIVE);
                            if (!me->GetVictim())
                                if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                    AttackStart(target);
                            break;
                        case EVENT_CONSTRUCT:
                            Talk(SAY_SUMMON);
                            if (!_creatureList.empty())
                            {
                                std::list<ObjectGuid>::iterator itr = _creatureList.begin();
                                std::advance(itr, urand(0, _creatureList.size() - 1));
                                if (Creature* construct = Unit::GetCreature(*me, *itr))
                                {
                                    construct->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                                    construct->SetReactState(REACT_AGGRESSIVE);
                                    construct->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_STUNNED | UNIT_FLAG_DISABLE_MOVE);
                                    construct->AI()->AttackStart(me->GetVictim());
                                    construct->AI()->DoZoneInCombat();
                                    DoCast(me, SPELL_STRENGHT, true);
                                    _creatureList.erase(itr);
                                    summons.Summon(construct);
                                }
                            }
                            DoCast(me, SPELL_ACTIVATE_CONSTRUCT);
                            events.ScheduleEvent(EVENT_CONSTRUCT, RAID_MODE(40000, 30000));
                            break;
                        case EVENT_BERSERK:
                            Talk(SAY_BERSERK);
                            DoCast(me, SPELL_BERSERK, true);
                            break;
                    }
                }

                DoMeleeAttackIfReady();

                EnterEvadeIfOutOfCombatArea(diff);
            }

        private:
            ObjectGuid _slagPotGUID;
            time_t _firstConstructKill;
            std::list<ObjectGuid> _creatureList;

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_ignis_AI>(creature);
        }
};

class npc_iron_construct : public CreatureScript
{
    public:
        npc_iron_construct() : CreatureScript("npc_iron_construct") { }

        struct npc_iron_constructAI : public ScriptedAI
        {
            npc_iron_constructAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript())
            {
                me->AddAura(SPELL_FREEZE_ANIM, me);
                creature->SetReactState(REACT_PASSIVE);
                needDamage = Is25ManRaid() ? 3000 : 5000;
            }

            void Reset() override
            {
                _brittled = false;
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                if (me->HasAura(SPELL_BRITTLE) && damage >= needDamage)
                {
                    DoCastAOE(SPELL_SHATTER, true);
                    if (Creature* ignis = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_IGNIS)))
                        if (ignis->AI())
                            ignis->AI()->DoAction(ACTION_REMOVE_BUFF);

                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->DespawnOrUnsummon(1000);
                }
            }

            void JustReachedHome() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_STUNNED | UNIT_FLAG_DISABLE_MOVE);
                DoCast(me, SPELL_FREEZE_ANIM, true);
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasAura(SPELL_MOLTEN) && me->HasAura(SPELL_HEAT))
                    me->RemoveAura(SPELL_HEAT);

                if (Aura* aur = me->GetAura(SPELL_HEAT))
                {
                    if (aur->GetStackAmount() >= 10)
                    {
                        me->RemoveAura(SPELL_HEAT);
                        DoCast(SPELL_MOLTEN);
                        _brittled = false;
                    }
                }

                // Water pools
                if (me->IsInWater() && !_brittled && me->HasAura(SPELL_MOLTEN))
                {
                    DoCast(SPELL_BRITTLE);
                    me->RemoveAura(SPELL_MOLTEN);
                    _brittled = true;
                }

                DoMeleeAttackIfReady();
            }

        private:
            InstanceScript* instance;
            bool _brittled;
            uint32 needDamage;

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_iron_constructAI>(creature);
        }
};

class npc_scorch_ground : public CreatureScript
{
    public:
        npc_scorch_ground() : CreatureScript("npc_scorch_ground") { }

        struct npc_scorch_groundAI : public ScriptedAI
        {
            npc_scorch_groundAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE |UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                creature->SetDisplayId(16925); //model 2 in db cannot overwrite wdb fields
            }

            void MoveInLineOfSight(Unit* unit) override
            {
                if (!_heat)
                {
                    if (unit->GetEntry() == NPC_IRON_CONSTRUCT)
                    {
                        if (!unit->HasAura(SPELL_HEAT) || !unit->HasAura(SPELL_MOLTEN))
                        {
                            _constructGUID = unit->GetGUID();
                            _heat = true;
                        }
                    }
                }
            }

            void Reset() override
            {
                _heat = false;
                DoCast(me, SPELL_GROUND);
                _constructGUID = ObjectGuid::Empty;
                _heatTimer = 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (_heat)
                {
                    if (_heatTimer <= diff)
                    {
                        Creature* construct = me->GetCreature(*me, _constructGUID);
                        if (construct && !construct->HasAura(SPELL_MOLTEN))
                        {
                            me->AddAura(SPELL_HEAT, construct);
                            _heatTimer = 1000;
                        }
                    }
                    else
                        _heatTimer -= diff;
                }
            }

        private:
            ObjectGuid _constructGUID;
            uint32 _heatTimer;
            bool _heat;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_scorch_groundAI>(creature);
        }
};

class spell_ignis_slag_pot : public SpellScriptLoader
{
    public:
        spell_ignis_slag_pot() : SpellScriptLoader("spell_ignis_slag_pot") { }

        class spell_ignis_slag_pot_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ignis_slag_pot_AuraScript);

            bool Validate(SpellInfo const* /*spellEntry*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SLAG_POT_DAMAGE_N))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_SLAG_POT_DAMAGE_H))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_SLAG_IMBUED))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* aurEff)
            {
                Unit* aurEffCaster = aurEff->GetCaster();
                if (!aurEffCaster)
                    return;

                uint32 spellId = GetSpellInfo()->Id;
                if (Unit* target = GetTarget())
                    aurEffCaster->CastSpell(target, spellId == SPELL_SLAG_POT ? SPELL_SLAG_POT_DAMAGE_N : SPELL_SLAG_POT_DAMAGE_H, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget() && GetTarget()->ToPlayer() && GetTarget()->IsAlive())
                {
                    GetTarget()->CastSpell(GetTarget(), SPELL_SLAG_IMBUED, true);

                    uint32 achievRequired = GetTarget()->GetMap()->Is25ManRaid() ? SPELL_SLAG_IMBUED_25 : SPELL_SLAG_IMBUED;
                    GetTarget()->ToPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2, achievRequired);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ignis_slag_pot_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_ignis_slag_pot_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ignis_slag_pot_AuraScript();
        }
};

void AddSC_boss_ignis()
{
    new boss_ignis();
    new npc_iron_construct();
    new npc_scorch_ground();
    new spell_ignis_slag_pot();

    if (VehicleSeatEntry* vehSeat = const_cast<VehicleSeatEntry*>(sVehicleSeatStore.LookupEntry(3206)))
        vehSeat->m_flags |= 0x400;
}
