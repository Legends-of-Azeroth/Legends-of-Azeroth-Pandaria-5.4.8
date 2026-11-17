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
#include "dragon_soul.h"

enum ScriptedTexts
{
    SAY_AGGRO       = 0,
    SAY_DEATH       = 1,
    SAY_GROUND1     = 6,
    SAY_GROUND2     = 7,
    SAY_CRYSTAL     = 9,
    SAY_KILL        = 10,
    SAY_KOHCROM     = 11,
    ANN_CRYSTAL     = 12,
};

enum Spells
{
    SPELL_BERSERK                           = 47008,
    SPELL_STOMP                             = 103414,
    SPELL_CRUSH_ARMOR                       = 103687,
    SPELL_RESONATING_CRYSTAL                = 103640,
    SPELL_RESONATING_CRYSTAL_SUMMON         = 103639,
    SPELL_RESONATING_CRYSTAL_DMG            = 103545,
    SPELL_RESONATING_CRYSTAL_AURA           = 103494,
    SPELL_TARGET_SELECTION                  = 103528,
    SPELL_DANGER                            = 103534,
    SPELL_WARNING                           = 103536,
    SPELL_SAFE                              = 103541,
    SPELL_FURIOUS                           = 103846,

    SPELL_BLACK_BLOOD_OF_THE_EARTH_DUMMY    = 103180,
    SPELL_BLACK_BLOOD_OF_THE_EARTH          = 103851,
    SPELL_BLACK_BLOOD_OF_THE_EARTH_DMG      = 103785,

    SPELL_EARTHEN_VORTEX                    = 103821,
    SPELL_EARTHEN_VORTEX_SUMMON             = 104512,
    SPELL_EARTHEN_VORTEX_VEHICLE            = 109615,

    SPELL_SUMMON_KOHCROM                    = 109017,
    SPELL_MORCHOK_JUMP                      = 109070,

    SPELL_MORCHOK_SIEGE_MISSILE_1           = 107541,
    SPELL_MORCHOK_SIEGE_MISSILE_2           = 110307,

    SPELL_EARTHS_VENGEANCE                  = 103176,
    SPELL_EARTHS_VENGEANCE_MISSILE          = 103177,
    SPELL_EARTHS_VENGEANCE_DMG              = 103178,
};

enum Events
{
    EVENT_EARTHS_VENGEANCE      = 1,
    EVENT_BLACK_BLOOD           = 2,
    EVENT_RESONATING_CRYSTAL    = 3,
    EVENT_BERSERK               = 4,
    EVENT_EARTHEN_VORTEX        = 5,
    EVENT_CONTINUE              = 6,
    EVENT_STOMP                 = 7,
    EVENT_CRUSH_ARMOR           = 8,
    EVENT_EXPLODE               = 9,
    EVENT_CHECK_PLAYERS         = 10,
    EVENT_CONTINUE_1            = 11,
    EVENT_UPDATE_HEALTH         = 12,
}; 

enum Adds
{
    NPC_EARTHEN_VORTEX      = 55723,
    NPC_RESONATING_CRYSTAL  = 55346,
};

enum Actions
{
    ACTION_KOHCROM_STOMP                = 1,
    ACTION_KOHCROM_RESONATING_CRYSTAL   = 2,
    ACTION_KOHCROM_EARTHEN_VORTEX       = 3,
};
enum MiscData
{
    DATA_GUID_1         = 1,
    DATA_GUID_2         = 2,
    DATA_KOHCROM_DONE   = 3,
    DATA_ALLOW_ACHIEV   = 4,
};

class boss_morchok: public CreatureScript
{
    public:
        boss_morchok() : CreatureScript("boss_morchok") { }

        struct boss_morchokAI : public BossAI
        {
            boss_morchokAI(Creature* creature) : BossAI(creature, DATA_MORCHOK)
            {             
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->setActive(true);
                pKohcrom = NULL;
            }

            void Reset() override
            {
                _Reset();
                
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DANGER);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WARNING);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SAFE);

                me->SetReactState(REACT_AGGRESSIVE);

                me->LowerPlayerDamageReq(me->GetHealth());

                _stompguid1 = ObjectGuid::Empty;
                _stompguid2 = ObjectGuid::Empty;
                bEnrage = false;
                bKohcrom = false;
                bFirstStomp = false;
                bFirstCrystal = false;
                pKohcrom = NULL;
                
                me->GetMap()->SetWorldState(WORLDSTATE_DONT_STAND_SO_CLOSE_TO_ME, 1);
                me->SetObjectScale(1.0f);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);

                events.ScheduleEvent(EVENT_STOMP, urand(12000, 14000));
                events.ScheduleEvent(EVENT_EARTHEN_VORTEX, urand(56000, 60000));
                events.ScheduleEvent(EVENT_RESONATING_CRYSTAL, urand(19000, 20000));
                events.ScheduleEvent(EVENT_BERSERK, 7 * MINUTE * IN_MILLISECONDS);
                if (!IsHeroic())
                    events.ScheduleEvent(EVENT_CRUSH_ARMOR, urand(6000, 12000));

                _stompguid1 = ObjectGuid::Empty;
                _stompguid2 = ObjectGuid::Empty;
                bEnrage = false;
                bKohcrom = false;
                bFirstStomp = false;
                bFirstCrystal = false;

                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DANGER);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WARNING);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SAFE);

                DoZoneInCombat();
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                instance->SetBossState(DATA_MORCHOK, IN_PROGRESS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                Talk(SAY_DEATH);

                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DANGER);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WARNING);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SAFE);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void SetGUID(ObjectGuid guid, int32 type) override
            {
                if (type == DATA_GUID_1)
                    _stompguid1 = guid;
                else if (type == DATA_GUID_2)
                    _stompguid2 = guid;
            }

            ObjectGuid GetGUID(int32 type) const override
            {
                if (type == DATA_GUID_1)
                    return _stompguid1;
                else if (type == DATA_GUID_2)
                    return _stompguid2;

                return ObjectGuid::Empty;
            }

            void JustSummoned(Creature* summon) override
            {
                BossAI::JustSummoned(summon);
                if (summon->GetEntry() == NPC_KOHCROM)
                {
                    pKohcrom = summon;
                    summon->SetMaxHealth(me->GetMaxHealth());
                    summon->SetHealth(me->GetHealth());
                }
            }
            void KilledUnit(Unit* victim) override
            {
                if (victim && victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_KOHCROM_DONE)
                    return uint32(bKohcrom);

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_ALLOW_ACHIEV)
                    me->GetMap()->SetWorldState(WORLDSTATE_DONT_STAND_SO_CLOSE_TO_ME, 0);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if ((me->GetDistance(me->GetHomePosition()) > 500.0f) || (me->GetPositionX() > -1915))
                {
                    EnterEvadeMode();
                    return;
                }

                if (IsHeroic() && pKohcrom && pKohcrom->GetPositionX() > -1915)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (IsHeroic() && me->HealthBelowPct(90) && !bKohcrom)
                {
                    bKohcrom = true;
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();

                    Talk(SAY_KOHCROM);

                    DoCast(me, SPELL_MORCHOK_JUMP);
                    DoCast(me, SPELL_SUMMON_KOHCROM, true);

                    bFirstCrystal = true;

                    events.DelayEvents(10000);
                    events.ScheduleEvent(EVENT_CONTINUE, 5000);
                    events.ScheduleEvent(EVENT_UPDATE_HEALTH, 2000);
                    return;
                }

                if (me->HealthBelowPct(20) && !bEnrage)
                {
                    bEnrage = true;
                    DoCast(me, SPELL_FURIOUS);
                    return;
                }

                if (IsHeroic() && pKohcrom)
                {
                    if (me->HealthBelowPct(30) && me->GetFloatValue(OBJECT_FIELD_SCALE) > 0.2f)
                    {
                        me->SetObjectScale(0.2f);
                        pKohcrom->SetObjectScale(0.2f);
                    }
                    else if (me->HealthBelowPct(40) && me->GetFloatValue(OBJECT_FIELD_SCALE) > 0.3f)
                    {
                        me->SetObjectScale(0.3f);
                        pKohcrom->SetObjectScale(0.3f);
                    }
                    else if (me->HealthBelowPct(50) && me->GetFloatValue(OBJECT_FIELD_SCALE) > 0.4f)
                    {
                        me->SetObjectScale(0.4f);
                        pKohcrom->SetObjectScale(0.4f);
                    }
                    else if (me->HealthBelowPct(60) && me->GetFloatValue(OBJECT_FIELD_SCALE) > 0.5f)
                    {
                        me->SetObjectScale(0.5f);
                        pKohcrom->SetObjectScale(0.5f);
                    }
                    else if (me->HealthBelowPct(70) && me->GetFloatValue(OBJECT_FIELD_SCALE) > 0.6f)
                    {
                        me->SetObjectScale(0.6f);
                        pKohcrom->SetObjectScale(0.6f);
                    }
                    else if (me->HealthBelowPct(80) && me->GetFloatValue(OBJECT_FIELD_SCALE) > 0.7f)
                    {
                        me->SetObjectScale(0.7f);
                        pKohcrom->SetObjectScale(0.7f);
                    }
                }

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_UPDATE_HEALTH:
                            if (me->IsAlive())
                            {
                                if (pKohcrom)
                                {
                                    if (!pKohcrom->IsAlive())
                                        break;

                                    if (me->GetHealth() < 500000 || pKohcrom->GetHealth() < 500000)
                                        break;

                                    uint32 new_health = (me->GetHealth() + pKohcrom->GetHealth()) / 2;
                                    new_health = std::min(new_health, me->GetMaxHealth());
                                    me->SetHealth(new_health);
                                    pKohcrom->SetHealth(new_health);
                                }
                                events.ScheduleEvent(EVENT_UPDATE_HEALTH, 3000);
                            }

                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            break;
                        case EVENT_CRUSH_ARMOR:
                            DoCastVictim(SPELL_CRUSH_ARMOR);
                            events.ScheduleEvent(EVENT_CRUSH_ARMOR, urand(12000, 15000));
                            break;
                        case EVENT_STOMP:
                        {
                            _stompguid1 = ObjectGuid::Empty;
                            _stompguid2 = ObjectGuid::Empty;

                            int32 tim = int32(events.GetNextEventTime(EVENT_EARTHEN_VORTEX)) - int32(events.GetTimer());
                            if (tim <= 7000)
                                break;

                            DoCast(me, SPELL_STOMP);
                            events.ScheduleEvent(EVENT_STOMP, urand(12000, 14000));
                            if (bKohcrom && !bFirstStomp && pKohcrom)
                                pKohcrom->AI()->DoAction(ACTION_KOHCROM_STOMP);
                            bFirstStomp = false;
                            break;
                        }
                        case EVENT_RESONATING_CRYSTAL:
                        {
                            int32 tim = int32(events.GetNextEventTime(EVENT_EARTHEN_VORTEX)) - int32(events.GetTimer());
                            if (tim <= 17000)
                                break;

                            Talk(ANN_CRYSTAL);
                            Talk(SAY_CRYSTAL);
                            Position pos = me->GetNearPosition(frand(25.0f, 45.0f), frand(0, 2 * M_PI));
                            me->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_RESONATING_CRYSTAL, true);
                            events.ScheduleEvent(EVENT_RESONATING_CRYSTAL, urand(12000, 14000));
                            if (bKohcrom && !bFirstCrystal && pKohcrom)
                                pKohcrom->AI()->DoAction(ACTION_KOHCROM_RESONATING_CRYSTAL);
                            bFirstCrystal = false;
                            break;
                        }
                        case EVENT_EARTHEN_VORTEX:
                            Talk(SAY_GROUND1);
                            me->SetReactState(REACT_PASSIVE);
                            me->AttackStop();

                            events.CancelEvent(EVENT_RESONATING_CRYSTAL);
                            events.CancelEvent(EVENT_STOMP);
                            events.CancelEvent(EVENT_CRUSH_ARMOR);

                            DoCastAOE(SPELL_EARTHEN_VORTEX, true);
                            DoCast(me, SPELL_EARTHS_VENGEANCE);

                            if (bKohcrom && pKohcrom)
                                pKohcrom->AI()->DoAction(ACTION_KOHCROM_EARTHEN_VORTEX);

                            events.ScheduleEvent(EVENT_BLACK_BLOOD, 5000);
                            events.ScheduleEvent(EVENT_EARTHEN_VORTEX, urand(94000, 97000));
                            break;
                        case EVENT_BLACK_BLOOD:
                            Talk(SAY_GROUND2);
                            DoCast(me, SPELL_BLACK_BLOOD_OF_THE_EARTH);
                            events.ScheduleEvent(EVENT_CONTINUE_1, 18000);
                            break;
                        case EVENT_CONTINUE:
                            me->SetReactState(REACT_AGGRESSIVE);
                            AttackStart(me->GetVictim());
                            break;
                        case EVENT_CONTINUE_1:
                            me->SetReactState(REACT_AGGRESSIVE);
                            AttackStart(me->GetVictim());
                            bFirstCrystal = true;
                            events.ScheduleEvent(EVENT_STOMP, 18000);
                            events.ScheduleEvent(EVENT_RESONATING_CRYSTAL, 25000);
                            if (!IsHeroic())
                                events.ScheduleEvent(EVENT_CRUSH_ARMOR, urand(6000, 12000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            Creature* pKohcrom;
            ObjectGuid _stompguid1;
            ObjectGuid _stompguid2;
            bool bEnrage;
            bool bKohcrom;
            bool bFirstStomp;
            bool bFirstCrystal;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_morchokAI>(creature);
        }
};

class npc_morchok_kohcrom: public CreatureScript
{
    public:
        npc_morchok_kohcrom() : CreatureScript("npc_morchok_kohcrom") { }

        struct npc_morchok_kohcromAI : public ScriptedAI
        {
            npc_morchok_kohcromAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->setActive(true);
                me->SetReactState(REACT_PASSIVE);
                instance = me->GetInstanceScript();
            }

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();

                _stompguid1 = ObjectGuid::Empty;
                _stompguid2 = ObjectGuid::Empty;
                bEnrage = false;
            }

            void SetGUID(ObjectGuid guid, int32 type) override
            {
                if (type == DATA_GUID_1)
                    _stompguid1 = guid;
                else if (type == DATA_GUID_2)
                    _stompguid2 = guid;
            }

            ObjectGuid GetGUID(int32 type) const override
            {
                if (type == DATA_GUID_1)
                    return _stompguid1;
                else if (type == DATA_GUID_2)
                    return _stompguid2;

                return ObjectGuid::Empty;
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                DoCast(me, SPELL_MORCHOK_JUMP, true);
                events.ScheduleEvent(EVENT_CONTINUE, 5000);
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_KOHCROM_STOMP:
                        events.ScheduleEvent(EVENT_STOMP, urand(4000, 6000));
                        break;
                    case ACTION_KOHCROM_RESONATING_CRYSTAL:
                        events.ScheduleEvent(EVENT_RESONATING_CRYSTAL, urand(4000, 6000));
                        break;
                    case ACTION_KOHCROM_EARTHEN_VORTEX:
                        events.ScheduleEvent(EVENT_EARTHEN_VORTEX, 1);
                        break;
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                if (me->IsInCombat())
                    DoZoneInCombat(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;
 
                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->HealthBelowPct(20) && !bEnrage)
                {
                    bEnrage = true;
                    DoCast(me, SPELL_FURIOUS);
                    return;
                }

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_STOMP:
                            _stompguid1 = ObjectGuid::Empty;
                            _stompguid2 = ObjectGuid::Empty;
                            DoCast(me, SPELL_STOMP);
                            break;
                        case EVENT_RESONATING_CRYSTAL:
                        {
                            Position pos = me->GetNearPosition(frand(25.0f, 45.0f), frand(0, 2 * M_PI));
                            me->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_RESONATING_CRYSTAL, true);
                            break;
                        }
                        case EVENT_EARTHEN_VORTEX:
                            me->SetReactState(REACT_PASSIVE);
                            me->AttackStop();
                            DoCastAOE(SPELL_EARTHEN_VORTEX, true);
                            DoCast(me, SPELL_EARTHS_VENGEANCE);
                            events.ScheduleEvent(EVENT_BLACK_BLOOD, 5000);
                            break;
                        case EVENT_BLACK_BLOOD:
                            DoCast(me, SPELL_BLACK_BLOOD_OF_THE_EARTH);
                            events.ScheduleEvent(EVENT_CONTINUE, 18000);
                            break;
                        case EVENT_CONTINUE:
                            me->SetReactState(REACT_AGGRESSIVE);
                            AttackStart(me->GetVictim());
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        private:
            EventMap events;
            SummonList summons;
            InstanceScript* instance;
            ObjectGuid _stompguid1;
            ObjectGuid _stompguid2;
            bool bEnrage;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_morchok_kohcromAI>(creature);
        }
};

class npc_morchok_resonating_crystal : public CreatureScript
{
    public:
        npc_morchok_resonating_crystal() : CreatureScript("npc_morchok_resonating_crystal") { }

        struct npc_morchok_resonating_crystalAI : public ScriptedAI
        {
            npc_morchok_resonating_crystalAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                SetCombatMovement(false);
            }

            EventMap events; 

            void Reset() override { }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                events.ScheduleEvent(EVENT_EXPLODE, 10000);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_EXPLODE)
                    {
                        me->RemoveAura(SPELL_RESONATING_CRYSTAL);
                        DoCastAOE(SPELL_RESONATING_CRYSTAL_DMG);
                        me->DespawnOrUnsummon(500);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_morchok_resonating_crystalAI>(creature);
        }
};

class spell_morchok_target_selected : public SpellScriptLoader
{ 
    public:
        spell_morchok_target_selected() : SpellScriptLoader("spell_morchok_target_selected") { }

        class spell_morchok_target_selected_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_morchok_target_selected_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (targets.empty())
                    return;

                uint32 max_size = (GetCaster()->GetMap()->Is25ManRaid() ? 7 : 3);

                if (targets.size() > max_size)
                {
                    targets.sort(DistanceOrderPred(GetCaster()));
                }

                max_size = std::min((uint32)targets.size(), max_size);

                uint32 count = 0;
                for (std::list<WorldObject*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                {
                    temp_targets.push_back((*itr));
                    count++;
                    if (count >= max_size)
                        break;
                }
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                bool bFound = false;
                for (std::vector<WorldObject*>::const_iterator itr = temp_targets.begin(); itr != temp_targets.end(); ++itr)
                    if ((*itr)->GetGUID() == GetHitUnit()->GetGUID())
                    {
                        bFound = true;
                        break;
                    }

                if (!bFound)
                {
                    GetHitUnit()->RemoveAura(SPELL_DANGER);
                    GetHitUnit()->RemoveAura(SPELL_WARNING);
                    GetHitUnit()->RemoveAura(SPELL_SAFE);
                }
                else
                {
                    float dist = GetCaster()->GetDistance(GetHitUnit());
                    if (dist > 30.0f) 
                        GetCaster()->CastSpell(GetHitUnit(), SPELL_DANGER, true);
                    else if (dist > 15.0f) 
                        GetCaster()->CastSpell(GetHitUnit(), SPELL_WARNING, true);
                    else 
                        GetCaster()->CastSpell(GetHitUnit(), SPELL_SAFE, true);
                }
            }
        
            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_morchok_target_selected_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_morchok_target_selected_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_DUMMY);
            }

        private:
            std::vector<WorldObject*> temp_targets;

            class DistanceOrderPred
            {
                public:
                    DistanceOrderPred(WorldObject* searcher) : _searcher(searcher) { }
                    bool operator() (WorldObject* a, WorldObject* b) const
                    {
                        float rA = _searcher->GetDistance(a);
                        float rB = _searcher->GetDistance(b);
                        return rA < rB;
                    }

                private:
                    WorldObject* _searcher;
            };
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_morchok_target_selected_SpellScript();
        }

};

class spell_morchok_resonating_crystal_dmg : public SpellScriptLoader
{
   
    public:
        spell_morchok_resonating_crystal_dmg() : SpellScriptLoader("spell_morchok_resonating_crystal_dmg") { }

        class spell_morchok_resonating_crystal_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_morchok_resonating_crystal_dmg_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (targets.empty())
                    return;

                targets.remove_if(AurasCheck());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_morchok_resonating_crystal_dmg_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_morchok_resonating_crystal_dmg_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_morchok_resonating_crystal_dmg_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_DEST_AREA_ENEMY);
            }

        private:
            class AurasCheck
            {
                public:
                    AurasCheck() { }

                    bool operator()(WorldObject* unit)
                    {
                        return (!unit->ToUnit() || !(unit->ToUnit()->HasAura(SPELL_DANGER) || unit->ToUnit()->HasAura(SPELL_WARNING) || unit->ToUnit()->HasAura(SPELL_SAFE)));
                    }
            };
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_morchok_resonating_crystal_dmg_SpellScript();
        }
};

class spell_morchok_black_blood_of_the_earth : public SpellScriptLoader
{
    public:
        spell_morchok_black_blood_of_the_earth() : SpellScriptLoader("spell_morchok_black_blood_of_the_earth") { }

        class spell_morchok_black_blood_of_the_earth_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_morchok_black_blood_of_the_earth_AuraScript);

            void HandlePeriodicTick(AuraEffect const* aurEff)
            {
                uint32 ticks = aurEff->GetTickNumber();

                std::list<GameObject*> goList;
                GetCaster()->GetGameObjectListWithEntryInGrid(goList, GO_INNER_WALL, 100.0f);

                for (uint32 j = 1; j <= ticks + 1; ++j)
                {
                    if ((j - ticks - 1) % 4)
                        continue;
                    uint32 steps = 5 * j;
                    float angle = 2 * M_PI / steps;
                    for (uint32 i = 0; i < steps; ++i)
                    {
                        Position pos = { GetCaster()->GetPositionX() + 4 * j * cosf(i * angle), GetCaster()->GetPositionY() + 4 * j * sinf(i * angle), GetCaster()->GetPositionZ() };
                        pos.m_positionZ = GetCaster()->GetMap()->GetHeight(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), false);

                        bool collided = false;
                        if (!goList.empty())
                            for (std::list<GameObject*>::const_iterator itr = goList.begin(); itr != goList.end(); ++itr)
                                if (WallCheck(GetCaster(), (*itr))(&pos) || (*itr)->GetExactDist2d(&pos) <= 4)
                                    collided = true;

                        if (!collided)
                            GetCaster()->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_BLACK_BLOOD_OF_THE_EARTH_DUMMY, true);
                        /*
                        // Simple version
                        Position pos;
                        GetCaster()->GetNearPosition(pos, 4 * j, i * angle);
                        GetCaster()->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_BLACK_BLOOD_OF_THE_EARTH_DUMMY, true);
                        */
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_morchok_black_blood_of_the_earth_AuraScript::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }

        private:
            class WallCheck
            {
                public:
                    WallCheck(Unit* searcher, GameObject* go) : _searcher(searcher), _go(go) { }

                    bool operator()(Position* unit)
                    {
                        return (_go->IsInBetween(_searcher, unit, 4.0f));
                    }

                private:
                    Unit* _searcher;
                    GameObject* _go;
            };
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_morchok_black_blood_of_the_earth_AuraScript();
        }
};

class spell_morchok_black_blood_of_the_earth_dmg : public SpellScriptLoader
{
   
    public:
        spell_morchok_black_blood_of_the_earth_dmg() : SpellScriptLoader("spell_morchok_black_blood_of_the_earth_dmg") { }

        class spell_morchok_black_blood_of_the_earth_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_morchok_black_blood_of_the_earth_dmg_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                if (targets.empty())
                    return;

                // Achievement Controll
                if (Creature* caster = GetCaster()->ToCreature())
                {
                    std::list<Player*> achievementTargets;
                    GetPlayerListInGrid(achievementTargets, caster, 200.0f);

                    for (auto&& itr : achievementTargets)
                    {
                        if (HasAnyAroundMe(itr))
                            caster->AI()->SetData(DATA_ALLOW_ACHIEV, 0);
                    }
                }

                if (AuraEffect const* aurEff = GetCaster()->GetAuraEffect(SPELL_BLACK_BLOOD_OF_THE_EARTH, EFFECT_0))
                {
                    uint32 ticks = aurEff->GetTickNumber() + 1;
                    targets.remove_if(DistanceCheck(GetCaster(), float(ticks * 4)));
                }

                std::list<GameObject*> goList;
                GetCaster()->GetGameObjectListWithEntryInGrid(goList, GO_INNER_WALL, 100.0f);
                if (!goList.empty())
                    for (std::list<GameObject*>::const_iterator itr = goList.begin(); itr != goList.end(); ++itr)
                        targets.remove_if(WallCheck(GetCaster(), (*itr)));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_morchok_black_blood_of_the_earth_dmg_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_morchok_black_blood_of_the_earth_dmg_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }

        private:
            class DistanceCheck
            {
                public:
                    DistanceCheck(Unit* searcher, float distance) : _searcher(searcher), _distance(distance) { }
            
                    bool operator()(WorldObject* unit)
                    {
                        return (_searcher->GetDistance2d(unit) > _distance);
                    }

                private:
                    Unit* _searcher;
                    float _distance;
            };

            class WallCheck
            {
                public:
                    WallCheck(Unit* searcher, GameObject* go) : _searcher(searcher), _go(go) { }
            
                    bool operator()(WorldObject* unit)
                    {
                        return (_go->IsInBetween(_searcher, unit, 4.0f));
                    }

                private:
                    Unit* _searcher;
                    GameObject* _go;
            };

            bool HasAnyAroundMe(Player* pTarget)
            {
                std::list<Player*> nearTargets;
                GetPlayerListInGrid(nearTargets, pTarget, 5.0f);

                nearTargets.remove_if([=](Player* target) { return target && target->GetGUID() == pTarget->GetGUID(); });

                uint32 allowNearTargets = pTarget->GetMap()->Is25ManRaid() ? 1 : 0;

                if (nearTargets.size() <= allowNearTargets)
                    return false;

                return true;
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_morchok_black_blood_of_the_earth_dmg_SpellScript();
        }
};

class spell_morchok_stomp : public SpellScriptLoader
{
    public:
        spell_morchok_stomp() : SpellScriptLoader("spell_morchok_stomp") { }

        class spell_morchok_stomp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_morchok_stomp_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                if (targets.empty())
                    return;

                if (Creature* pMorchok = GetCaster()->ToCreature())
                {
                    targets.sort(DistanceOrderPred(GetCaster()));
                    std::list<WorldObject*>::const_iterator itr = targets.begin();
                    pMorchok->AI()->SetGUID((*itr)->GetGUID(), DATA_GUID_1);
                    if (targets.size() > 1)
                    {
                        ++itr;
                        pMorchok->AI()->SetGUID((*itr)->GetGUID(), DATA_GUID_2);
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_morchok_stomp_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }

        private:
            class DistanceOrderPred
            {
                public:
                    DistanceOrderPred(WorldObject* searcher) : _searcher(searcher) { }
                    bool operator() (WorldObject* a, WorldObject* b) const
                    {
                        float rA = _searcher->GetExactDist(a);
                        float rB = _searcher->GetExactDist(b);
                        return rA < rB;
                    }

                private:
                    WorldObject* _searcher;
            };
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_morchok_stomp_SpellScript();
        }
};

void AddSC_boss_morchok()
{
    new boss_morchok();
    new npc_morchok_kohcrom();
    new npc_morchok_resonating_crystal();
    new spell_morchok_target_selected();
    new spell_morchok_resonating_crystal_dmg();
    new spell_morchok_black_blood_of_the_earth();
    new spell_morchok_black_blood_of_the_earth_dmg();
    new spell_morchok_stomp();
}
