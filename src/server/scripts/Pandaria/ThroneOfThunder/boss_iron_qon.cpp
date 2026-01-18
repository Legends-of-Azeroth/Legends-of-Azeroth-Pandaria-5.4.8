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
#include "Vehicle.h"
#include "throne_of_thunder.h"

enum spells
{
    /*Qon*/
    SPELL_THROW_SPEAR                 = 134925, // Misssle, summon  68480
    SPELL_SPEAR_AURA                  = 134927, // Just visual spear
    SPELL_IMPALE                      = 134691,
    SPELL_RISING_ANGER                = 136324,
    SPELL_RISING_ANGER_EFF            = 136323,
    SPELL_FIST_SMASH                  = 136146,

    /*Roshak Phase*/
    SPELL_BURNING_CINDERS_SPEAR_EFF   = 134758, /* visual ground rapture from spear. 
    at frist cast make a rapture with AT, next cast while rapure is active it make spawn only area trigger.
    Have a sin/cos model at ~15y spawn distance*/

    SPELL_BURNING_CINDERS_AURA        = 135052, // aura on player that have a stackable proc eff
    SPELL_BURNING_CINDERS_EFF         = 137668,
    SPELL_BURNING_CINDERS_MISSLE      = 136330, // throw spear -> delay... -> missle this ->periodic spawn attenuation AT
    SPELL_MOLTEN_OVERLOAD             = 137221, // at full energy
    SPELL_MOLTEN_INFERNO              = 134664, // it seems like sequence inferno->unleased->inferno...
    SPELL_UNLEASHED_FLAME             = 134628, // split
    SPELL_UNLEASHED_FLAME_MISSLE      = 134611,
    SPELL_UNLEASHED_FLAME_SELECTOR    = 139140,
    SPELL_UNLEASHED_FLAME_EFF         = 134628,
    SPELL_SCORCHED                    = 134647,
                                      
    /*Quenzal Phase*/                 
    SPELL_STORM_CLOUD_AURA            = 136425, // same like burning cinders aura
    SPELL_STORM_CLOUD_EFF             = 136421,
    SPELL_STORM_CLOUD_MISSLE          = 136419,
    SPELL_ELECTRIFIED                 = 136615,
    SPELL_WIND_STORM_AT_EFF           = 136577,
    SPELL_RUSHING_WINDS_PULL_EFF      = 136592, // by players
    SPELL_RUSHING_WINDS_PULL          = 136591,
    SPELL_RUSHING_WINDS_AT            = 136574,
    SPELL_RUSHING_WINDS_VISUAL        = 135570,
    SPELL_GUST_OF_WIND                = 135550,
    SPELL_FREEZING_WINDS_EFF          = 136260,
    SPELL_BURNING_WINDS_EFF           = 136261,
    SPELL_RUSHING_WINDS_VEHICLE       = 137654,
    SPELL_RUSHING_WINDS_TARGETING     = 135583,
    SPELL_RUSHING_WINDS_ADDITIONAL    = 136161, // knockback eff only for freezing/ignite
    SPELL_RUSHING_WINDS_SPEED         = 136590,
    SPELL_RUSHING_WINDS_MISC          = 136596,
    SPELL_FREEZE_CYCLONE              = 136153,
    SPELL_IGNITE_CYCLONE              = 136159,
    SPELL_FREEZE_CYCLONE_EFF          = 136892,

    SPELL_ARCING_LIGHTNING_PROC       = 138434,
    SPELL_ARCING_LIGHTNING_PROC_HIT   = 138435,
    SPELL_ARCING_LIGHTNING_PROC_HIT_2 = 138473,
    SPELL_ARCING_LIGHTNING_PROC_EFF   = 136193,
    SPELL_SUMMON_LIGHTNING_CLOUD      = 136198,
    SPELL_ARCING_LIGHTNING_VISUAL     = 136224,
    SPELL_ARCING_LIGHTNING_EFF        = 136192,
    SPELL_ARCING_LIGHTNING_SELECTOR   = 139106, // seems like 136192 selector?

    /*Damren Phase*/
    SPELL_FROZEN_BLOOD_AURA           = 136452,
    SPELL_FROZEN_BLOOD_EFF            = 136451,
    SPELL_FROZEN_BLOOD_MISSLE         = 136449,
    SPELL_FROZEN_RESILENCE            = 135144,
    SPELL_FROZEN_RESILENCE_EFF        = 135142,
    SPELL_SHATTER_EFF                 = 135146,
    SPELL_SHATTER_EFF_ALLY            = 137709,
    SPELL_FREEZE                      = 135145,
    SPELL_FREEZE_SELECTOR             = 139109,
    SPELL_DEAD_ZONE_LEFT              = 135199, // aa, okey, understand how it work
    SPELL_DEAD_ZONE_RIGHT             = 135223,
    SPELL_DEAD_ZONE_BACK              = 135225,
    SPELL_DEAD_ZONE_FRONT             = 135361,
    SPELL_DEAD_ZONE_FRONT_RIGHT       = 137226,
    SPELL_DEAD_ZONE_LEFT_RIGHT        = 137227,
    SPELL_DEAD_ZONE_FRONT_LEFT        = 137228,
    SPELL_DEAD_ZONE_FRONT_BACK        = 137229,
    SPELL_DEAD_ZONE_LEFT_BACK         = 137230,
    SPELL_DEAD_ZONE_BACK_RIGHT        = 137231,
    SPELL_DEAD_ZONE                   = 137232,
    SPELL_DEAD_ZONE_EFF               = 135147,
                                      
    /*Misc | Shared*/                 
    SPELL_NON_REGENERATE_POWER        = 72242,

    /*Heroic*/
    SPELL_WHIRLING_WINDS_EFF          = 139167,
    SPELL_WHIRLING_WINDS_AT           = 139166,
    SPELL_FROST_SPIKE_SELECTOR        = 139174,
    SPELL_FROST_SPIKE_MISSLE          = 139177,
    SPELL_FROST_SPIKE_EFF             = 139180,

    // Achievements
    SPELL_CHECK_BURNING_CINDERS       = 139065,
    SPELL_FAILED_BURNING_CINDERS      = 139054,
    SPELL_CHECK_STORM_CLOUD           = 139061,
    SPELL_FAILED_STORM_CLOUD          = 139056,
    SPELL_CHECK_FROZEN_BLOOD          = 139064,
    SPELL_FAILED_FROZEN_BLOOD         = 139055,
    SPELL_CHECK_FROZEN_SOLID          = 139062,
    SPELL_FAILED_FROZEN_SOLID         = 139057,
    SPELL_CHECK_RUSHING_WINDS         = 139063,
    SPELL_FAILED_RUSHING_WINDS        = 139058,
};

enum events
{
    EVENT_IMPALE = 1,
    EVENT_THROW_SPEAR,
    EVENT_FIST_SMASH,
    EVENT_BERSERK,
    EVENT_GAIN_ENERGY,
    EVENT_WIND_STORM,
    EVENT_ARCING_LIGHTNING,
    EVENT_FREEZE,
    EVENT_DEAD_ZONE,
    EVENT_ENCHANT_SPEAR,
    EVENT_ENCHANT_CYCLONE,
    
    // Heroic
    EVENT_WHIRLWIND,
    EVENT_FROSTSPIKE,
    EVENT_UNLIASHED_FLAME,
};

enum Yells
{
    TALK_MANCHU_INTRO,
    TALK_MANCHI_DIE,
    TALK_WEISHEN_INTRO,
    TALK_WEISHEN_DIE,
    TALK_AGGRO,
    TALK_QUILEN_DEATH,
    TALK_QUILEN_QUETZAL,
    TALK_QUILEN_DAMREN,
    TALK_LAST_PHASE,
    TALK_DEATH,
};

enum eSpears
{
    SPEAR_FLAME,
    SPEAR_STORM,
    SPEAR_FROST,
    SPEAR_NONE,
};

// Just match spear type to next quilen
const std::map<uint32, uint32> invSpearType =
{
    { SPEAR_FLAME, NPC_QUETZAL },
    { SPEAR_STORM, NPC_DAMREN  },
    { SPEAR_FROST, 0           },
    { SPEAR_NONE,  0           },
};

const std::map<uint32, uint32> invEnchantType =
{
    { NPC_ROSHAK,  SPELL_BURNING_CINDERS_MISSLE },
    { NPC_QUETZAL, SPELL_STORM_CLOUD_MISSLE     },
    { NPC_DAMREN,  SPELL_FROZEN_BLOOD_MISSLE    },
};

const std::vector<uint32> quelins
{
    NPC_ROSHAK,
    NPC_QUETZAL,
    NPC_DAMREN,
};

const std::map<uint32, uint32> invCallingType =
{
    { NPC_ROSHAK,  NPC_QUETZAL },
    { NPC_QUETZAL, NPC_DAMREN },
    { NPC_DAMREN,  NPC_ROSHAK },
};

class boss_iron_qon : public CreatureScript
{
    public:
        boss_iron_qon() : CreatureScript("boss_iron_qon") { }
    
        struct boss_iron_qonAI : public BossAI
        {
            boss_iron_qonAI(Creature* creature) : BossAI(creature, DATA_IRON_QON) 
            {
                me->setActive(true);
            }

            EventMap berserkEvents;
            uint32 spearType;
            float x, y;
            bool atEvade;
   
            void Reset() override
            {
                _Reset();
                berserkEvents.Reset();
                scheduler.CancelAll();
                spearType = SPEAR_FLAME;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                x = 0.0f; y = 0.0f;
                atEvade = false;

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                me->m_Events.Schedule(1500, 20, [this]()
                {
                    if (Creature* roshak = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ROSHAK) : ObjectGuid::Empty))
                    {
                        roshak->OverrideInhabitType(INHABIT_AIR);
                        roshak->UpdateMovementFlags();

                        DoCast(roshak, VEHICLE_SPELL_RIDE_HARDCODED, true);
                        roshak->GetMotionMaster()->MovePoint(0, roshak->GetPositionX(), roshak->GetPositionY(), roshak->GetPositionZ() + 10.0f);
                    }
                });
            }

            void JustEngagedWith(Unit* who) override
            {
                Talk(TALK_AGGRO);

                _JustEngagedWith();

                for (auto&& itr : quelins)
                    if (Creature* quelin = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(itr) : ObjectGuid::Empty))
                        quelin->AI()->JustEngagedWith(who);

                if (GameObject* go = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GO_IRON_QON_SEWER_GATE) : ObjectGuid::Empty))
                    go->SetGoState(GO_STATE_READY);

                events.ScheduleEvent(EVENT_IMPALE, 20 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_THROW_SPEAR, 30 * IN_MILLISECONDS);
                berserkEvents.ScheduleEvent(EVENT_BERSERK, 12 * MINUTE * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_SWITCH_QUELIN)
                {
                    me->ExitVehicle();

                    if (spearType >= SPEAR_FROST)
                    {
                        Talk(TALK_LAST_PHASE);

                        if (instance)
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                        DoCast(me, SPELL_RISING_ANGER, true);

                        // Init ignite Freeze Cyclones
                        if (Creature* windController = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_RUSHING_WINDS_CONTROLLER) : ObjectGuid::Empty))
                            for (uint32 i = 0; i < 5; i++)
                                windController->CastSpell(windController, SPELL_GUST_OF_WIND, true);

                        events.Reset();
                        events.ScheduleEvent(EVENT_IMPALE, 20 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_FIST_SMASH, IsHeroic() ? 60 * IN_MILLISECONDS : 20 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_ENCHANT_CYCLONE, 45 * IN_MILLISECONDS);
                        spearType++;

                        if (IsHeroic())
                            for (auto&& itr : quelins)
                                if (Creature* quilens = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(itr) : ObjectGuid::Empty))
                                    quilens->AI()->DoAction(ACTION_CALL_QUILENS);
                        return;
                    }

                    if (Creature* nextQuelin = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(invSpearType.find(spearType)->second) : ObjectGuid::Empty))
                    {
                        if (instance)
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, nextQuelin);

                        Talk(TALK_QUILEN_DEATH);
                        
                        GetPositionWithDistInOrientation(nextQuelin, 45.0f, nextQuelin->GetOrientation(), x, y);

                        Movement::MoveSplineInit init(nextQuelin);
                        init.MoveTo(x, y, 142.85f);

                        init.Launch();

                        ObjectGuid quelingGUID = nextQuelin->GetGUID();

                        scheduler
                            .SetValidator([this] { return instance && instance->GetBossState(DATA_IRON_QON) == IN_PROGRESS; })
                            .Schedule(Milliseconds(nextQuelin->GetSplineDuration()), [this, quelingGUID](TaskContext context)
                        {
                            if (Creature* quelin = ObjectAccessor::GetCreature(*me, quelingGUID))
                            {
                                Talk(quelin->GetEntry() == NPC_DAMREN ? TALK_QUILEN_DAMREN : TALK_QUILEN_QUETZAL);

                                DoCast(quelin, VEHICLE_SPELL_RIDE_HARDCODED, true);
                                quelin->StopMoving();
                                quelin->OverrideInhabitType(INHABIT_GROUND);
                                quelin->UpdateMovementFlags();
                                DoZoneInCombat(quelin, 300.0f);
                                quelin->RemoveChanneledCast();
                                quelin->HandleEmoteStateCommand(EMOTE_STATE_POINT);
                                quelin->AI()->DoAction(ACTION_SWITCH_QUELIN);
                                quelin->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);

                                if (IsHeroic())
                                    if (Creature* addQuilen = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(quelin->GetEntry() > NPC_QUETZAL ? quelin->GetEntry() - 2 : quelin->GetEntry() + 1) : ObjectGuid::Empty))
                                        addQuilen->AI()->DoAction(ACTION_ADDITIONAL_QUILEN);
                            }
                        });
                    }

                    spearType++;
                }
            }

            void EnterEvadeMode() override
            {
                if (atEvade)
                    return;

                atEvade = true;

                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                berserkEvents.Reset();

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_IRON_QON, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARCING_LIGHTNING_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FREEZE_CYCLONE_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARCING_LIGHTNING_PROC);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_IMPALE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ELECTRIFIED);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FAILED_BURNING_CINDERS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FAILED_STORM_CLOUD);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FAILED_FROZEN_BLOOD);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FAILED_FROZEN_SOLID);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FAILED_RUSHING_WINDS);

                    if (GameObject* go = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_IRON_QON_SEWER_GATE)))
                        go->SetGoState(GO_STATE_ACTIVE);
                }

                for (auto&& itr : quelins)
                    if (Creature* quelin = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(itr) : ObjectGuid::Empty))
                        quelin->AI()->EnterEvadeMode();

                _DespawnAtEvade();
                me->GetMotionMaster()->MoveTargetedHome();
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(TALK_DEATH);

                _JustDied();

                // Achievement Controller
                DoCast(me, SPELL_CHECK_BURNING_CINDERS, true);
                DoCast(me, SPELL_CHECK_STORM_CLOUD, true);
                DoCast(me, SPELL_CHECK_FROZEN_BLOOD, true);
                DoCast(me, SPELL_CHECK_FROZEN_SOLID, true);
                DoCast(me, SPELL_CHECK_RUSHING_WINDS, true);

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARCING_LIGHTNING_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FREEZE_CYCLONE_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARCING_LIGHTNING_PROC);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_IMPALE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ELECTRIFIED);

                    if (GameObject* go = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_IRON_QON_SEWER_GATE)))
                        go->SetGoState(GO_STATE_ACTIVE);
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_SELECTED_SPEAR)
                    return spearType;

                return 0;
            }
    
            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;
    
                events.Update(diff);
                scheduler.Update(diff);
                berserkEvents.Update(diff);

                while (uint32 eventId = berserkEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_BERSERK)
                        DoCast(me, SPELL_BERSERK);
                    break;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_IMPALE:
                            // We use not our threat list
                            if (Unit* quilen = me->GetVehicleBase())
                            {
                                if (Unit* target = ObjectAccessor::GetUnit(*me, quilen->GetTarget()))
                                    DoCast(target, SPELL_IMPALE);
                                else // Damren so often casting some spells and break his target
                                {
                                    events.RescheduleEvent(EVENT_IMPALE, urand(2 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                                    break;
                                }
                            }
                            else if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_IMPALE);

                            events.ScheduleEvent(EVENT_IMPALE, 20 * IN_MILLISECONDS);
                            break;
                        case EVENT_THROW_SPEAR:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                DoCast(target, SPELL_THROW_SPEAR);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                DoCast(target, SPELL_THROW_SPEAR);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                DoCast(target, SPELL_THROW_SPEAR);

                            events.ScheduleEvent(EVENT_ENCHANT_SPEAR, 3.6 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_THROW_SPEAR, urand(30 * IN_MILLISECONDS, 42 * IN_MILLISECONDS));
                            break;
                        case EVENT_FIST_SMASH:
                            DoCast(me, SPELL_FIST_SMASH);
                            events.ScheduleEvent(EVENT_FIST_SMASH, 20 * IN_MILLISECONDS);
                            break;
                        case EVENT_ENCHANT_SPEAR:
                            if (Unit* quilen = me->GetVehicleBase())
                                quilen->CastSpell(quilen, invEnchantType.find(quilen->GetEntry())->second, true);
                            break;
                        case EVENT_ENCHANT_CYCLONE:
                            DoCast(me, SPELL_RUSHING_WINDS_MISC, true); // ignite/freeze cyclones
                            events.ScheduleEvent(EVENT_ENCHANT_CYCLONE, 50 * IN_MILLISECONDS);
                            break;
                    }
                }
    
                if (spearType > SPEAR_FROST)
                    DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_iron_qonAI(creature);
        }
};

struct quelingBaseAI : public ScriptedAI
{
    quelingBaseAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

    InstanceScript* instance;
    EventMap events, additionalEvents;
    TaskScheduler scheduler;
    SummonList summons;
    uint32 windStormValue;
    uint32 prevDeadZone;
    ObjectGuid targetGUID;
    float x, y;
    bool inferno;
    bool atEvade;
    bool inCombat;
    bool hasLeave;
    bool hasInStorm;

    void InitializeAI() override
    {
        me->setActive(true);

        if (instance && instance->GetBossState(DATA_IRON_QON) != DONE)
            me->Respawn();

        Reset();
    }

    void Reset() override
    {
        events.Reset();
        additionalEvents.Reset();
        scheduler.CancelAll();
        me->SetReactState(REACT_PASSIVE);
        targetGUID = ObjectGuid::Empty;
        prevDeadZone = 0;
        windStormValue = 1;
        inferno = true;
        atEvade = false;
        inCombat = false;
        hasLeave = false;
        hasInStorm = false;
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);

        me->SetPowerType(POWER_MANA);
        me->SetMaxPower(POWER_MANA, 100);
        me->SetPower(POWER_MANA, 0);
        me->setRegeneratingMana(false);
        me->SetUInt32Value(UNIT_FIELD_OVERRIDE_DISPLAY_POWER_ID, 162);

        if (me->GetEntry() == NPC_ROSHAK)
        {
            me->ResetInhabitTypeOverride();
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
        }
        else
        {
            me->OverrideInhabitType(INHABIT_GROUND);
            me->UpdateMovementFlags();
        }
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_SWITCH_QUELIN:
                additionalEvents.Reset();
                scheduler.CancelAll();

                if (me->GetEntry() == NPC_QUETZAL)
                {
                    events.ScheduleEvent(EVENT_WIND_STORM, 45 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_ARCING_LIGHTNING, 13 * IN_MILLISECONDS);
                }
                else if (me->GetEntry() == NPC_DAMREN)
                {
                    events.ScheduleEvent(EVENT_FREEZE, 2 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_DEAD_ZONE, 5 * IN_MILLISECONDS);
                }
                break;
            case ACTION_ADDITIONAL_QUILEN:
            {
                me->StopMoving();

                Movement::MoveSplineInit init(me);
                for (auto&& itr : ironQonQuilensPath)
                    init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-2.5f, 2.5f), itr.GetPositionY() + frand(-1.5f, 1.5f), itr.GetPositionZ()));

                init.SetUncompressed();
                init.SetCyclic();
                init.SetFly();
                init.Launch();

                // init events
                switch (me->GetEntry())
                {
                    case NPC_ROSHAK:
                        additionalEvents.ScheduleEvent(EVENT_UNLIASHED_FLAME, 25 * IN_MILLISECONDS);
                        break;
                    case NPC_DAMREN:
                        additionalEvents.ScheduleEvent(EVENT_FROSTSPIKE, 20 * IN_MILLISECONDS);
                        break;
                    case NPC_QUETZAL:
                        additionalEvents.ScheduleEvent(EVENT_ARCING_LIGHTNING, 35 * IN_MILLISECONDS);
                        additionalEvents.ScheduleEvent(EVENT_WHIRLWIND, 15 * IN_MILLISECONDS);
                        break;
                }
                break;
            }
            case ACTION_REMOVE_ADDITIONAL_QUILEN:
            {
                if (Unit* target = me->GetVictim())
                    targetGUID = target->GetGUID();

                additionalEvents.Reset();
                me->PrepareChanneledCast(me->GetOrientation());
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                me->StopMoving();
                me->OverrideInhabitType(INHABIT_AIR);
                me->UpdateMovementFlags();

                Movement::MoveSplineInit init(me);
                for (auto&& itr : ironQonQuilensPath)
                    init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-2.5f, 2.5f), itr.GetPositionY() + frand(-1.5f, 1.5f), itr.GetPositionZ()));

                init.SetUncompressed();
                init.SetFly();
                init.SetCyclic();
                init.Launch();
                break;
            }
            case ACTION_CALL_QUILENS:
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                me->RemoveChanneledCast(targetGUID);

                me->OverrideInhabitType(INHABIT_GROUND);
                me->SetAnimTier(AnimTier::Ground);
                me->UpdateMovementFlags();

                switch (me->GetEntry())
                {
                    case NPC_ROSHAK:
                        inferno = true;
                        additionalEvents.Reset();

                        scheduler
                            .SetValidator([this] { return instance && instance->GetBossState(DATA_IRON_QON) == IN_PROGRESS; })
                            .Schedule(Milliseconds(me->GetSplineDuration() + 1 * IN_MILLISECONDS), [this](TaskContext context)
                        {
                            // Prevent gain energy while in this
                            if (!me->HasAura(SPELL_MOLTEN_OVERLOAD))
                            {
                                // calculate use spell like energize cuz it shouldn`t cast spell and energize in same time
                                if (!HandleCastSpellByEnergy(me->GetPower(POWER_MANA)))
                                    me->ModifyPower(POWER_MANA, 5);
                            }

                            context.Repeat(Milliseconds(900));
                        });
                        break;
                    case NPC_QUETZAL:
                        events.ScheduleEvent(EVENT_ARCING_LIGHTNING, 13 * IN_MILLISECONDS);
                        break;
                }

                break;
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (!IsHeroic())
            return;

        if (HealthBelowPct(25) && !hasLeave)
        {
            hasLeave = true;

            if (me->GetEntry() != NPC_DAMREN)
                events.Reset();

            me->SetPower(POWER_MANA, 0);
            scheduler.CancelAll();

            if (Creature* qon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_IRON_QON) : ObjectGuid::Empty))
                qon->AI()->DoAction(ACTION_SWITCH_QUELIN);

            if (me->GetEntry() != NPC_DAMREN)
                DoAction(ACTION_REMOVE_ADDITIONAL_QUILEN);
        }
    }

    void EnterEvadeMode() override
    {
        if (atEvade)
            return;

        atEvade = true;

        if (!me->IsAlive())
            me->Respawn();

        me->GetMotionMaster()->Clear();
        me->StopMoving();

        me->OverrideInhabitType(INHABIT_GROUND);
        me->UpdateMovementFlags();

        if (instance)
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

        uint32 corpseDelay = me->GetCorpseDelay();
        uint32 respawnDelay = me->GetRespawnDelay();

        me->SetCorpseDelay(1);
        me->SetRespawnDelay(29);

        me->DespawnOrUnsummon();

        me->SetCorpseDelay(corpseDelay);
        me->SetRespawnDelay(respawnDelay);
        me->GetMap()->CreatureRelocation(me, me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ(), me->GetHomePosition().GetOrientation());

        if (Creature* ironQon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_IRON_QON) : ObjectGuid::Empty))
            ironQon->AI()->EnterEvadeMode();
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (IsHeroic())
            return;

        if (Creature* qon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_IRON_QON) : ObjectGuid::Empty))
            qon->AI()->DoAction(ACTION_SWITCH_QUELIN);

        if (instance)
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
    }

    bool HandleCastSpellByEnergy(int32 energy)
    {
        if (inferno && energy > 10)
        {
            inferno = false;

            // Reset each 10s
            scheduler
                .SetValidator([this] { return instance && instance->GetBossState(DATA_IRON_QON) == IN_PROGRESS; })
                .Schedule(Seconds(8), [this](TaskContext context)
            {
                inferno = true;
            });

            DoCast(me, SPELL_MOLTEN_INFERNO);
            return true;
        }

        int32 energyRange = urand(0, 1) ? 30 : 35;
        if (energy >= energyRange) // sometimes it used at 30, sometimes at 35
        {
            DoCast(me, SPELL_UNLEASHED_FLAME_SELECTOR);
        }

        if (energy > 95)
        {
            DoCast(me, SPELL_MOLTEN_OVERLOAD);
        }

        return false;
    }
};

// Ro`shak 68079
class npc_roshak : public CreatureScript
{
    public:
        npc_roshak() : CreatureScript("npc_roshak") { }
    
        struct npc_roshakAI : public quelingBaseAI
        {
            npc_roshakAI(Creature* creature) : quelingBaseAI(creature) 
            {
                instance = me->GetInstanceScript();
            }
    
            void JustEngagedWith(Unit* who) override 
            {
                if (inCombat)
                    return;

                inCombat = true;

                me->setActive(true, ActiveFlags::InCombat);
                DoZoneInCombat();

                // Init another Quelins in combat and air
                for (auto&& itr : quelins)
                    if (Creature* quelin = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(itr) : ObjectGuid::Empty))
                        quelin->SetInCombatWithZone();

                // Init Qon in combat too
                if (Creature* qon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_IRON_QON) : ObjectGuid::Empty))
                {
                    qon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    qon->SetInCombatWithZone();
                }

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                targetGUID = who->GetGUID();
                GetPositionWithDistInOrientation(me, 45.0f, me->GetOrientation(), x, y);

                Movement::MoveSplineInit init(me);
                init.MoveTo(x, y, 142.85f);

                init.Launch();

                scheduler
                    .SetValidator([this] { return instance && instance->GetBossState(DATA_IRON_QON) == IN_PROGRESS; })
                    .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                {
                    me->StopMoving();
                    me->OverrideInhabitType(INHABIT_GROUND);
                    me->UpdateMovementFlags();
                    me->RemoveChanneledCast(targetGUID);

                    if (IsHeroic())
                        if (Creature* addQuilen = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(me->GetEntry() + 1) : ObjectGuid::Empty))
                            addQuilen->AI()->DoAction(ACTION_ADDITIONAL_QUILEN);
                });

                scheduler
                    .SetValidator([this] { return instance && instance->GetBossState(DATA_IRON_QON) == IN_PROGRESS; })
                    .Schedule(Milliseconds(me->GetSplineDuration() + 1 * IN_MILLISECONDS), [this](TaskContext context)
                {
                    // Prevent gain energy while in this
                    if (!me->HasAura(SPELL_MOLTEN_OVERLOAD))
                    {
                        // calculate use spell like energize cuz it shouldn`t cast spell and energize in same time
                        if (!HandleCastSpellByEnergy(me->GetPower(POWER_MANA)))
                            me->ModifyPower(POWER_MANA, 5);
                    }

                    context.Repeat(Milliseconds(900));
                });
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_IN_AIR)
                    return hasLeave;

                return 0;
            }
    
            void UpdateAI(uint32 diff) override
            {
                additionalEvents.Update(diff);

                while (uint32 eventId = additionalEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_UNLIASHED_FLAME)
                    {
                        DoCast(me, SPELL_UNLEASHED_FLAME_SELECTOR, true);
                        additionalEvents.ScheduleEvent(EVENT_UNLIASHED_FLAME, 32 * IN_MILLISECONDS);
                    }
                    break;
                }

                if (!UpdateVictim())
                    return;
    
                scheduler.Update(diff);
                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_roshakAI(creature);
        }
};

// Quet`zal 68080
class npc_quetzal : public CreatureScript
{
    public:
        npc_quetzal() : CreatureScript("npc_quetzal") { }
    
        struct npc_quetzalAI : public quelingBaseAI
        {
            npc_quetzalAI(Creature* creature) : quelingBaseAI(creature)
            {
                instance = me->GetInstanceScript();
            }
    
            void JustEngagedWith(Unit* who) override
            {
                if (inCombat)
                    return;

                inCombat = true;

                me->OverrideInhabitType(INHABIT_AIR);
                me->UpdateMovementFlags();
                me->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 10.0f); 
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                    me->HandleEmoteStateCommand(EMOTE_STATE_NONE);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_WIND_STORM)
                    return windStormValue;

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_WIND_STORM)
                    windStormValue = data;
            }
    
            void UpdateAI(uint32 diff) override
            {
                additionalEvents.Update(diff);

                while (uint32 eventId = additionalEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARCING_LIGHTNING:
                            DoCast(me, SPELL_ARCING_LIGHTNING_EFF);
                            additionalEvents.ScheduleEvent(EVENT_ARCING_LIGHTNING, 40 * IN_MILLISECONDS);
                            break;
                        case EVENT_WHIRLWIND:
                        {
                            // Drop down from current position
                            me->StopMoving();
                            me->GetMotionMaster()->MovePoint(1, me->GetPositionX(), me->GetPositionY(), 142.83f);

                            uint32 delay = me->GetSplineDuration();

                            scheduler
                                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                    me->SetFacingTo(me->GetAngle(target));
                                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                    me->SetFacingTo(me->GetAngle(target));

                                // Launch Areatrigger to target facing
                                DoCast(me, SPELL_WHIRLING_WINDS_AT);
                            });

                            // Back to fly path
                            scheduler
                                .Schedule(Milliseconds(delay += 1000), [this](TaskContext context)
                            {
                                me->StopMoving();
                                Movement::MoveSplineInit init(me);
                                for (auto&& itr : ironQonQuilensPath)
                                    init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-2.5f, 2.5f), itr.GetPositionY() + frand(-1.5f, 1.5f), itr.GetPositionZ()));

                                init.SetUncompressed();
                                init.SetCyclic();
                                init.SetFly();
                                init.Launch();

                                additionalEvents.ScheduleEvent(EVENT_WHIRLWIND, 25 * IN_MILLISECONDS);
                            });

                            break;
                        }
                    }
                }

                if (!UpdateVictim())
                    return;
    
                events.Update(diff);
                scheduler.Update(diff);

                if (hasInStorm)
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_WIND_STORM:
                            windStormValue = 1;
                            hasInStorm = true;

                            if (Unit* target = me->GetVictim())
                                targetGUID = target->GetGUID();

                            me->PrepareChanneledCast(me->GetOrientation());

                            me->OverrideInhabitType(INHABIT_AIR);
                            me->UpdateMovementFlags();
                            me->GetMotionMaster()->MovePoint(1, quetzalStormPos);

                            scheduler
                                .SetValidator([this] { return instance && instance->GetBossState(DATA_IRON_QON) == IN_PROGRESS; })
                                .Schedule(Milliseconds(20000), [this](TaskContext context)
                            {
                                hasInStorm = false;
                                me->RemoveChanneledCast(targetGUID);

                                me->OverrideInhabitType(INHABIT_GROUND);
                                me->SetAnimTier(AnimTier::Ground);
                                me->UpdateMovementFlags();
                            });

                            if (Creature* windController = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_RUSHING_WINDS_CONTROLLER) : ObjectGuid::Empty))
                            {
                                windController->CastSpell(windController, SPELL_RUSHING_WINDS_AT, true);
                                windController->CastSpell(windController, SPELL_RUSHING_WINDS_PULL, true);

                                for (uint32 i = 0; i < 10; i++)
                                    windController->CastSpell(windController, SPELL_GUST_OF_WIND, true);
                            }

                            events.ScheduleEvent(EVENT_WIND_STORM, 90 * IN_MILLISECONDS);
                            break;
                        case EVENT_ARCING_LIGHTNING:
                            DoCast(me, SPELL_ARCING_LIGHTNING_EFF);
                            events.ScheduleEvent(EVENT_ARCING_LIGHTNING, 20 * IN_MILLISECONDS);
                            break;
                    }
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_quetzalAI(creature);
        }
};

// Dam`ren 68081
class npc_damren : public CreatureScript
{
    public:
        npc_damren() : CreatureScript("npc_damren") { }
    
        struct npc_damrenAI : public quelingBaseAI
        {
            npc_damrenAI(Creature* creature) : quelingBaseAI(creature)
            {
                instance = me->GetInstanceScript();
            }
    
            void JustEngagedWith(Unit* who) override
            {
                if (inCombat)
                    return;

                inCombat = true;

                me->OverrideInhabitType(INHABIT_AIR);
                me->UpdateMovementFlags();
                me->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 10.0f);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                    me->HandleEmoteStateCommand(EMOTE_STATE_NONE);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_DEAD_ZONE)
                    return prevDeadZone;

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_DEAD_ZONE)
                    prevDeadZone = data;
            }
    
            void UpdateAI(uint32 diff) override
            {
                additionalEvents.Update(diff);

                while (uint32 eventId = additionalEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_FROSTSPIKE)
                    {
                        DoCast(me, SPELL_FROST_SPIKE_SELECTOR, true);
                        additionalEvents.ScheduleEvent(EVENT_FROSTSPIKE, 20 * IN_MILLISECONDS);
                    }
                    break;
                }

                if (!UpdateVictim())
                    return;
    
                events.Update(diff);
                scheduler.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DEAD_ZONE:
                            if (Unit* vict = me->GetVictim())
                                targetGUID = vict->GetGUID();

                            me->PrepareChanneledCast(me->GetOrientation(), SPELL_DEAD_ZONE);

                            scheduler
                                .SetValidator([this] { return instance && instance->GetBossState(DATA_IRON_QON) == IN_PROGRESS; })
                                .Schedule(Milliseconds(7500), [this](TaskContext context)
                            {
                                me->RemoveChanneledCast(targetGUID);
                            });

                            events.ScheduleEvent(EVENT_DEAD_ZONE, 15 * IN_MILLISECONDS);
                            break;
                        case EVENT_FREEZE:
                            DoCast(me, SPELL_FREEZE);
                            events.ScheduleEvent(EVENT_FREEZE, 16.5 * IN_MILLISECONDS);
                            break;
                    }
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_damrenAI(creature);
        }
};

// Iron Qon`s Spear 68480
class npc_iron_qons_spear : public CreatureScript
{
    public:
        npc_iron_qons_spear() : CreatureScript("npc_iron_qons_spear") { }
    
        struct npc_iron_qons_spearAI : public ScriptedAI
        {
            npc_iron_qons_spearAI(Creature* creature) : ScriptedAI(creature) { }
    
            InstanceScript* instance;
            TaskScheduler scheduler;
            float dist, modOri, startWave;
            uint32 spearEff;

            void IsSummonedBy(Unit* summoner) override
            {
                instance = me->GetInstanceScript();

                DoCast(me, SPELL_SPEAR_AURA, true);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_ENCHANT_SPEAR)
                {
                    if (Creature* qon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_IRON_QON) : ObjectGuid::Empty))
                    {
                        spearEff = 0;

                        switch (qon->AI()->GetData(TYPE_SELECTED_SPEAR))
                        {
                            case SPEAR_FLAME:
                                spearEff = SPELL_BURNING_CINDERS_SPEAR_EFF;
                                break;
                            case SPEAR_STORM:
                                spearEff = SPELL_STORM_CLOUD_EFF;
                                break;
                            case SPEAR_FROST:
                                spearEff = SPELL_FROZEN_BLOOD_EFF;
                                break;
                        }

                        DoCast(me, spearEff, true);
                        me->DespawnOrUnsummon(46 * IN_MILLISECONDS);

                        startWave = urand(0, 2 * M_PI);

                        // Model of multi Thread ?
                        dist = 3.0f;
                        modOri = (2305 * M_PI) / 576;

                        scheduler
                            .SetValidator([this] { return instance && instance->GetBossState(DATA_IRON_QON) == IN_PROGRESS && dist < 100.0f; })
                            .Schedule(Milliseconds(300), [this](TaskContext context)
                        {
                            modOri += M_PI / 3;

                            float attenuation = sin(modOri) / modOri;

                            float x, y;

                            for (uint32 i = 0; i < 5; i++)
                            {
                                GetPositionWithDistInOrientation(me, dist, Position::NormalizeOrientation(startWave + i * 72.0f + Position::NormalizeOrientation(attenuation)), x, y);
                                me->CastSpell(x, y, me->GetMap()->GetHeight(x, y, me->GetPositionZ(), true, 20.0f), spearEff, true);
                            }

                            dist += 3.0f;

                            context.Repeat(Milliseconds(300));
                        });
                    }
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                damage = 0;
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_iron_qons_spearAI(creature);
        }
};

// Rushing Wind 68852
struct npc_rushing_wind : public ScriptedAI
{
    npc_rushing_wind(Creature* creature) : ScriptedAI(creature) { }

    InstanceScript* instance;
    TaskScheduler scheduler;
    uint32 mod, passenger;
    float x, y;
    bool clockSide;

    void IsSummonedBy(Unit* summoner) override
    {
        instance = me->GetInstanceScript();
        me->SetDisplayFromModel(0);

        if (Creature* ironQon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_IRON_QON) : ObjectGuid::Empty))
        {
            ironQon->AI()->JustSummoned(me);

            if (ironQon->AI()->GetData(TYPE_SELECTED_SPEAR) >= SPEAR_FROST)
            {
                DoCast(me, SPELL_RUSHING_WINDS_ADDITIONAL);

                scheduler
                    .SetValidator([this] { return instance && instance->GetBossState(DATA_IRON_QON) == IN_PROGRESS; })
                    .Schedule(Milliseconds(200), [this](TaskContext context)
                {
                    Movement::MoveSplineInit init(me);
                    init.MoveTo(ironQonCenter.GetPositionX() + frand(-60.0f, 60.0f), ironQonCenter.GetPositionY() + frand(-33.0f, 33.0f), me->GetPositionZ());
                    init.Launch();

                    context.Repeat(Milliseconds(me->GetSplineDuration()));
                });
            }
            else
            {
                mod = 0;
                x = 0.0f; y = 0.0f;
                clockSide = urand(0, 1);
                me->DespawnOrUnsummon(25 * IN_MILLISECONDS);

                if (Creature* quetZal = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_QUETZAL) : ObjectGuid::Empty))
                {
                    quetZal->AI()->JustSummoned(me);

                    mod = quetZal->AI()->GetData(TYPE_WIND_STORM);
                    quetZal->AI()->SetData(TYPE_WIND_STORM, mod + 1);

                    if (mod > 5)
                        DoCast(me, SPELL_RUSHING_WINDS_SPEED, true);
                }

                Movement::MoveSplineInit init(me);
                GetPositionWithDistInOrientation(me, 6.0f * mod, frand(0, 2 * M_PI), x, y);
                init.MoveTo(x, y, me->GetPositionZ());
                init.Launch();

                scheduler
                    .SetValidator([this] { return instance && instance->GetBossState(DATA_IRON_QON) == IN_PROGRESS; })
                    .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                {
                    me->StopMoving();
                    DoCast(me, SPELL_RUSHING_WINDS_ADDITIONAL);

                    Movement::MoveSplineInit newInit(me);
                    for (uint8 i = 1; i < 25; ++i)
                    {
                        if (Creature* windController = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_RUSHING_WINDS_CONTROLLER) : ObjectGuid::Empty))
                        {
                            x = windController->GetPositionX() + 6.0f * mod * cos(Position::NormalizeOrientation(me->GetAngle(windController) + (i * M_PI / 12) - M_PI));
                            y = clockSide ? windController->GetPositionY() + 6.0f * mod * sin(Position::NormalizeOrientation(me->GetAngle(windController) + (i * M_PI / 12) - M_PI)) : windController->GetPositionY() - 6.0f * mod * sin(Position::NormalizeOrientation(me->GetAngle(windController) + (i * M_PI / 12) - M_PI));
                            newInit.Path().push_back(G3D::Vector3(x, y, me->GetPositionZ()));
                        }
                    }

                    newInit.SetUncompressed();
                    newInit.SetCyclic();
                    newInit.Launch();
                });
            }
        }
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_FREEZE_CYCLONE)
        {
            scheduler
                .SetValidator([this] { return instance && instance->GetBossState(DATA_IRON_QON) == IN_PROGRESS; })
                .Schedule(Milliseconds(200), [this](TaskContext context)
            {
                if (me->GetVehicleKit() && me->GetVehicleKit()->GetPassenger(0))
                {
                    context.Repeat(Milliseconds(500));
                    return;
                }

                std::list<Player*> pList;
                GetPlayerListInGrid(pList, me, 3.3f);

                for (auto&& itr : pList)
                {
                    // Not pull same target again
                    if (me->GetAI()->GetData(TYPE_TARGET_ID) == itr->GetGUID().GetCounter())
                        continue;

                    if (itr->IsOnVehicle())
                        continue;

                    itr->CastSpell(me, SPELL_RUSHING_WINDS_VEHICLE, true);
                    SetData(TYPE_TARGET_ID, itr->GetGUID().GetCounter());
                }

                context.Repeat(Milliseconds(500));
            });
        }
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_TARGET_ID)
            return passenger;

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_TARGET_ID)
            passenger = data;
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);
    }
};

// Lightning Cloud 70660
struct npc_lightnig_cloud : public ScriptedAI
{
    npc_lightnig_cloud(Creature* creature) : ScriptedAI(creature) { }

    ObjectGuid ownerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

        ownerGUID = summoner->GetGUID();
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (clicker->GetGUID() == ownerGUID)
            return;

        if (Unit* owner = ObjectAccessor::GetUnit(*me, ownerGUID))
        {
            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            owner->RemoveAurasDueToSpell(SPELL_ARCING_LIGHTNING_EFF);
            me->DespawnOrUnsummon();
        }
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Unleashed Flame Selector 139140
class spell_unleashed_flame_selector : public SpellScript
{
    PrepareSpellScript(spell_unleashed_flame_selector);

    std::map <uint32, WorldObject*> unleashedDict;

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_UNLEASHED_FLAME_MISSLE, true);

            // Handle cuz it triggered
            caster->ModifyPower(POWER_MANA, -30);
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        // We should select group with max order, but not less than 3(5) in 10/25.
        if (Creature* caster = GetCaster()->ToCreature())
        {
            uint32 minCount = caster->GetMap()->Is25ManRaid() ? 5 : 3;

            for (auto&& itr : targets)
            {
                std::list<Player*> pList;
                GetPlayerListInGrid(pList, itr, 9.2f);

                unleashedDict.insert(std::pair<uint32, WorldObject*>(pList.size(), itr));
            }

            auto key = std::max_element(unleashedDict.begin(), unleashedDict.end(), unleashedDict.value_comp());

            // Do anything only if min group not less than 3(5)
            if (key->first >= minCount)
            {
                targets.clear();
                targets.push_back(key->second);
            }
            else
            {
                if (caster->GetMap()->IsHeroic() && caster->AI()->GetData(TYPE_IN_AIR))
                    targets.push_back(key->second);
                else
                    targets.clear();
            }
        }

    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_unleashed_flame_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_unleashed_flame_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Unleashed Flame Eff 134628
class spell_unleashed_flame_eff : public SpellScript
{
    PrepareSpellScript(spell_unleashed_flame_eff);

    uint8 targetsCount;

    bool Load() override
    {
        targetsCount = 1;
        return true;
    }

    void RecalculateDamage(SpellEffIndex /*effIndex*/)
    {
        if (targetsCount > 0)
            SetHitDamage(GetHitDamage() / targetsCount);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targetsCount = targets.size();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_unleashed_flame_eff::RecalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_unleashed_flame_eff::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// Rushing Wind Pull 136591
class spell_rushing_wind_pull : public SpellScript
{
    PrepareSpellScript(spell_rushing_wind_pull);

    void HandleScriptEffHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                target->CastSpell(caster, SPELL_RUSHING_WINDS_PULL_EFF, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_rushing_wind_pull::HandleScriptEffHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Rushing Wind Targeting 135583
class spell_rushing_wind_targeting : public SpellScript
{
    PrepareSpellScript(spell_rushing_wind_targeting);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (!caster || !target)
            return;

        uint32 stormLowId = caster->GetAI()->GetData(TYPE_TARGET_ID);

        // Not pull same target again
        if (uint32 stormLowId = caster->GetAI()->GetData(TYPE_TARGET_ID))
            if (stormLowId == target->GetGUID().GetCounter())
                return;

        // Pull only if we haven`t anyone already in us
        if (!target->IsOnVehicle() && !target->HasAura(SPELL_RUSHING_WINDS_PULL) && caster->GetVehicleKit() && !caster->GetVehicleKit()->GetPassenger(0))
        {
            target->CastSpell(caster, SPELL_RUSHING_WINDS_VEHICLE, true);
            caster->GetAI()->SetData(TYPE_TARGET_ID, target->GetGUID().GetCounter());
            target->CastSpell(target, SPELL_FAILED_RUSHING_WINDS, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_rushing_wind_targeting::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Storm Cloud Eff 137669
class spell_storm_cloud_eff : public AuraScript
{
    PrepareAuraScript(spell_storm_cloud_eff);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetStackAmount() > 2)
            if (Unit* owner = GetUnitOwner())
                if (!owner->HasAura(SPELL_ELECTRIFIED))
                    owner->CastSpell(owner, SPELL_ELECTRIFIED, true);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_storm_cloud_eff::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAPPLY);
    }
};

// Arcing Lightning Selector 136192
class spell_arcing_lightning_selector : public SpellScript
{
    PrepareSpellScript(spell_arcing_lightning_selector);

    std::list<WorldObject*> m_targets;

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            target->CastSpell(target, SPELL_SUMMON_LIGHTNING_CLOUD, true);
            target->CastSpell(target, SPELL_ARCING_LIGHTNING_VISUAL, true);
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove whole exclude casters
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if(MeeleSpecTargetSelector());
            pList.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_ARCING_LIGHTNING_EFF); });

            // if we have enough players without tanks, meelee
            if (!pList.empty())
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (targets.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);

                m_targets = targets;

                return;
            }

            targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_ARCING_LIGHTNING_EFF); });

            // if we haven`t enough players
            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);

            m_targets = targets;
        }
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_arcing_lightning_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_arcing_lightning_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_arcing_lightning_selector::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_arcing_lightning_selector::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_arcing_lightning_selector::CopyTargets, EFFECT_3, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_arcing_lightning_selector::CopyTargets, EFFECT_4, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Arcing Lightning 136192
class spell_arcing_lightning : public AuraScript
{
    PrepareAuraScript(spell_arcing_lightning);

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        // proc on death
        if (Unit* owner = GetOwner()->ToUnit())
            if (Creature* quetZal = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(NPC_QUETZAL) : ObjectGuid::Empty))
                if (quetZal->IsAlive())
                    quetZal->CastSpell(quetZal, SPELL_ARCING_LIGHTNING_EFF, true);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->RemoveAurasDueToSpell(SPELL_ARCING_LIGHTNING_VISUAL);
    }

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_ARCING_LIGHTNING_PROC_HIT_2, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_arcing_lightning::HandleOnProc);
        OnEffectRemove += AuraEffectRemoveFn(spell_arcing_lightning::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_arcing_lightning::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

class ArcingLightningSwitchPredicate 
{
    public:
        ArcingLightningSwitchPredicate(Unit const* me) : _me(me) { }
    
        bool operator() (WorldObject* object) const
        {
            return object && object->ToPlayer() && (object->ToPlayer()->GetExactDist2d(_me) > 10.0f || object->ToPlayer()->HasAura(SPELL_ARCING_LIGHTNING_PROC_EFF) || object->ToPlayer()->HasAura(SPELL_WIND_STORM_AT_EFF));
        }
    
        private:
            Unit const* _me;
};

// Arcing Lightning Proc Selector 138473
class spell_arcing_lightning_proc_selector : public SpellScript
{
    PrepareSpellScript(spell_arcing_lightning_proc_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                if (Creature* quetzal = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(NPC_QUETZAL) : ObjectGuid::Empty))
                    caster->CastSpell(target, SPELL_ARCING_LIGHTNING_PROC_EFF, true, NULL, NULL, quetzal->GetGUID());
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
            targets.remove_if(ArcingLightningSwitchPredicate(caster));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_arcing_lightning_proc_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_arcing_lightning_proc_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Arcing Lightning Proc Eff 136193
class spell_arcing_lightning_proc_eff_aura : public AuraScript
{
    PrepareAuraScript(spell_arcing_lightning_proc_eff_aura);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_ARCING_LIGHTNING_PROC, true);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->RemoveAurasDueToSpell(SPELL_ARCING_LIGHTNING_PROC);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_arcing_lightning_proc_eff_aura::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_arcing_lightning_proc_eff_aura::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Arcing Lightning Proc Eff 136193
class spell_arcing_lightning_proc_eff : public SpellScript
{
    PrepareSpellScript(spell_arcing_lightning_proc_eff);

    void HandleEffectHitTarget(SpellEffIndex effIndex)
    {
        if (Unit* target = GetHitUnit())
            if (target->HasAura(SPELL_ARCING_LIGHTNING_PROC_EFF) || target->HasAura(SPELL_WIND_STORM_AT_EFF))
                PreventEffectForTarget(target, effIndex);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_arcing_lightning_proc_eff::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Arcing Lightning Proc Selector 2 138435
class spell_arcing_lightning_proc_selector_2 : public SpellScript
{
    PrepareSpellScript(spell_arcing_lightning_proc_selector_2);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                if (Creature* quetzal = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(NPC_QUETZAL) : ObjectGuid::Empty))
                    caster->CastSpell(target, SPELL_ARCING_LIGHTNING_PROC_EFF, true, NULL, NULL, quetzal->GetGUID());
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
            targets.remove_if(ArcingLightningSwitchPredicate(caster));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_arcing_lightning_proc_selector_2::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_arcing_lightning_proc_selector_2::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Freeze Selector 135145
class spell_freeze_selector : public SpellScript
{
    PrepareSpellScript(spell_freeze_selector);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove whole exclude casters
            pList.remove_if(TankSpecTargetSelector());

            // if we have enough players without tanks, meelee
            if (!pList.empty())
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (targets.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);

                return;
            }

            // if we haven`t enough players
            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_freeze_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Freeze Aura 135145
class spell_freeze_aura : public AuraScript
{
    PrepareAuraScript(spell_freeze_aura);

    void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Aura* aura = aurEff->GetBase())
        {
            uint32 duration = urand(1000, 5000);
            aura->SetDuration(duration);
            aura->SetMaxDuration(duration);
        }
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (Unit* caster = GetCaster())
                caster->CastSpell(owner, SPELL_SHATTER_EFF, true);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_freeze_aura::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_freeze_aura::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Frozen Resilence 135144
class spell_frozen_resilence : public AuraScript
{
    PrepareAuraScript(spell_frozen_resilence);

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (Unit* attacker = eventInfo.GetActor())
            {
                int32 damage = int32(0.1f * eventInfo.GetDamageInfo()->GetDamage());

                owner->CastCustomSpell(attacker, SPELL_FROZEN_RESILENCE_EFF, &damage, NULL, NULL, true);
            }
        }
    }

    void Register()
    {
        OnProc += AuraProcFn(spell_frozen_resilence::HandleOnProc);
    }
};

// Dead Zone 137232
class spell_dead_zone : public SpellScript
{
    PrepareSpellScript(spell_dead_zone);

    void HandleScriptEff(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            std::vector<uint32> deadZoneComboSpellStorage = { SPELL_DEAD_ZONE_FRONT_RIGHT, SPELL_DEAD_ZONE_LEFT_RIGHT, SPELL_DEAD_ZONE_FRONT_LEFT, SPELL_DEAD_ZONE_FRONT_BACK, SPELL_DEAD_ZONE_LEFT_BACK, SPELL_DEAD_ZONE_BACK_RIGHT };

            prevSpellId = caster->AI()->GetData(TYPE_DEAD_ZONE);

            // Remove From Temp container value, that was used before
            if (prevSpellId)
                deadZoneComboSpellStorage.erase(std::find(deadZoneComboSpellStorage.begin(), deadZoneComboSpellStorage.end(), prevSpellId));

            // Select new spell from updated container
            prevSpellId = Trinity::Containers::SelectRandomContainerElement(deadZoneComboSpellStorage);
            caster->AI()->SetData(TYPE_DEAD_ZONE, prevSpellId);

            caster->CastSpell(caster, prevSpellId, true);
        }
    }

    private:
        uint32 prevSpellId;

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_dead_zone::HandleScriptEff, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Molten Overload 137221
class spell_molten_overload : public AuraScript
{
    PrepareAuraScript(spell_molten_overload);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->ModifyPower(POWER_MANA, -10);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_molten_overload::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// Throw Spear Missle Eff 136330, 136419, 136449
class spell_throw_spear_missle_eff : public SpellScript
{
    PrepareSpellScript(spell_throw_spear_missle_eff);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Creature* target = GetHitCreature())
            target->AI()->DoAction(ACTION_ENCHANT_SPEAR);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_IRON_SPEAR; });
        targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && (target->ToCreature()->HasAura(SPELL_BURNING_CINDERS_SPEAR_EFF) || target->ToCreature()->HasAura(SPELL_STORM_CLOUD_EFF) || target->ToCreature()->HasAura(SPELL_FROZEN_BLOOD_EFF)); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_throw_spear_missle_eff::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_throw_spear_missle_eff::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Rushing Winds Misc 136596
class spell_rushing_winds_misc : public SpellScript
{
    PrepareSpellScript(spell_rushing_winds_misc);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, urand(0, 1) ? SPELL_FREEZE_CYCLONE : SPELL_IGNITE_CYCLONE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_rushing_winds_misc::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Enchant Cyclone Ignite 136159
class spell_enchant_cyclone_ignite : public SpellScript
{
    PrepareSpellScript(spell_enchant_cyclone_ignite);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_RUSHING_WINDS_2; });
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && (target->ToUnit()->HasAura(SPELL_IGNITE_CYCLONE) || target->ToUnit()->HasAura(SPELL_FREEZE_CYCLONE)); });

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);

        m_targets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_enchant_cyclone_ignite::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_enchant_cyclone_ignite::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_enchant_cyclone_ignite::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_enchant_cyclone_ignite::CopyTargets, EFFECT_3, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Enchant Cyclone Freeze 136153
class spell_enchant_cyclone_freeze : public SpellScript
{
    PrepareSpellScript(spell_enchant_cyclone_freeze);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_RUSHING_WINDS_2; });
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && (target->ToUnit()->HasAura(SPELL_IGNITE_CYCLONE) || target->ToUnit()->HasAura(SPELL_FREEZE_CYCLONE)); });

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);

        m_targets = targets;
    }


    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Creature* target = GetHitCreature())
            target->AI()->DoAction(ACTION_FREEZE_CYCLONE);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_enchant_cyclone_freeze::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_enchant_cyclone_freeze::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_enchant_cyclone_freeze::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_enchant_cyclone_freeze::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Rushing Winds Vehicle 137654
class spell_rushing_winds_vehicle_aura : public AuraScript
{
    PrepareAuraScript(spell_rushing_winds_vehicle_aura);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            caster->RemoveAurasDueToSpell(SPELL_FREEZE_CYCLONE_EFF);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_rushing_winds_vehicle_aura::OnRemove, EFFECT_0, SPELL_AURA_CONTROL_VEHICLE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Rushing Winds Vehicle 137654
class spell_rushing_winds_vehicle : public SpellScript
{
    PrepareSpellScript(spell_rushing_winds_vehicle);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (GetHitUnit() && GetHitUnit()->HasAura(SPELL_FREEZE_CYCLONE))
                caster->AddAura(SPELL_FREEZE_CYCLONE_EFF, caster);

            caster->CastSpell(caster, SPELL_FAILED_FROZEN_SOLID, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_rushing_winds_vehicle::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Frost Spike Selector 139174
class spell_frost_spike_selector : public SpellScript
{
    PrepareSpellScript(spell_frost_spike_selector);

    std::list<WorldObject*> m_targets;

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_FROST_SPIKE_MISSLE, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        // Remove whole exclude casters
        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(MeeleSpecTargetSelector());

        uint32 minCount = GetCaster()->GetMap()->Is25ManRaid() ? 5 : 2;

        // if we have enough players without tanks, meelee
        if (targets.size() >= minCount)
        {
            if (targets.size() > minCount)
                Trinity::Containers::RandomResizeList(targets, minCount);

            return;
        }

        if (!m_targets.empty())
        {
            targets.clear();

            for (auto&& itr : m_targets)
                targets.push_back(itr);

            if (targets.size() > minCount)
                Trinity::Containers::RandomResizeList(targets, minCount);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_frost_spike_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_frost_spike_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// 604. Summoned by 134758 - Burning Cinders
class sat_iron_qon_burning_cinders : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && !triggering->ToPlayer()->HasAura(SPELL_WIND_STORM_AT_EFF);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
        {
            if (!itr->HasAura(SPELL_BURNING_CINDERS_AURA))
                itr->CastSpell(itr, SPELL_BURNING_CINDERS_AURA, true);

            itr->CastSpell(itr, SPELL_FAILED_BURNING_CINDERS, true);
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        // Remove just periodic aura, it mean that aura with stacks not`ll remove till effect not done
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_BURNING_CINDERS_AURA);
    }
};

// 681. Summoned by 136421 - Storm Cloud
class sat_iron_qon_storm_cloud : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && !triggering->ToPlayer()->HasAura(SPELL_WIND_STORM_AT_EFF);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
        {
            if (!itr->HasAura(SPELL_STORM_CLOUD_AURA))
                itr->CastSpell(itr, SPELL_STORM_CLOUD_AURA, true);

            itr->CastSpell(itr, SPELL_FAILED_STORM_CLOUD, true);
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        // Remove just periodic aura, it mean that aura with stacks not`ll remove till effect not done
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_STORM_CLOUD_AURA);
    }
};

// 684. Summoned by 136451 - Frozen Blood
class sat_iron_qon_frozen_blood : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && !triggering->ToPlayer()->HasAura(SPELL_WIND_STORM_AT_EFF);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
        {
            if (!itr->HasAura(SPELL_FROZEN_BLOOD_AURA))
                itr->CastSpell(itr, SPELL_FROZEN_BLOOD_AURA, true);

            itr->CastSpell(itr, SPELL_FAILED_FROZEN_BLOOD, true);
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        // Remove just periodic aura, it mean that aura with stacks not`ll remove till effect not done
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_FROZEN_BLOOD_AURA);
    }
};

// 691. Summoned by 136574 - Rushing Winds
class sat_iron_qon_rushing_winds : public IAreaTriggerAura
{
    std::vector<ObjectGuid> affectedTargetGUIDs;

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                caster->CastSpell(itr, SPELL_WIND_STORM_AT_EFF, true);
                affectedTargetGUIDs.push_back(itr->GetGUID());
            }
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
        {
            itr->RemoveAurasDueToSpell(SPELL_WIND_STORM_AT_EFF);

            if (std::find(affectedTargetGUIDs.begin(), affectedTargetGUIDs.end(), itr->GetGUID()) != affectedTargetGUIDs.end())
                affectedTargetGUIDs.erase(std::find(affectedTargetGUIDs.begin(), affectedTargetGUIDs.end(), itr->GetGUID()));
        }
    }
};

std::map<uint32, float> invDeadZoneType =
{
    { 614,  M_PI / 2 },
    { 615, -M_PI / 2 },
    { 616,      M_PI },
    { 625,      0.0f },
};

// 614, 615, 616, 625. Summoned by 135199, 135223, 135225, 135361 - Dead Zone Side
class sat_iron_qon_dead_zone : public IAreaTriggerAura
{
    float casterOri;
    uint32 areaTriggerId;

    void OnInit() override
    {
        if (Unit* caster = GetCaster())
            casterOri = caster->GetOrientation();

        if (AreaTrigger* at = GetAreaTrigger())
            areaTriggerId = at->GetEntry();
    }

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && hasInConeWithAngle(triggering->ToPlayer());
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_DEAD_ZONE_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_DEAD_ZONE_EFF);
    }

    bool hasInConeWithAngle(Player* target)
    {
        float appAngle = Position::NormalizeOrientation(casterOri + invDeadZoneType.find(areaTriggerId)->second);
        float invertedAngle = Position::NormalizeOrientation(target->GetOrientation() - M_PI);

        return GetCaster()->GetAngle(target) >= appAngle - M_PI / 4 && GetCaster()->GetAngle(target) <= appAngle + M_PI / 4;
    }
};

// 833. Summoned by 139166 - Whirling Winds
class sat_iron_qon_whirling_winds : public IAreaTriggerAura
{
    void OnInit() override
    {
        if (Unit* caster = GetCaster())
        {
            if (AreaTrigger* at = GetAreaTrigger())
            {
                float dist = 100.0f;
                float x, y;

                std::vector<Position> path;

                for (uint32 i = 1; i < 17; ++i)
                {
                    x = at->GetPositionX() + (((dist * i) / 17.0f) * cos(caster->GetOrientation()));
                    y = at->GetPositionY() + (((dist * i) / 17.0f) * sin(caster->GetOrientation()));
                    path.push_back({ x, y, at->GetPositionZ() });
                }

                at->InitSpline(path, at->GetDuration());
            }
        }
    }

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_WHIRLING_WINDS_EFF, true);
    }
};

void AddSC_boss_iron_qon()
{
    new boss_iron_qon();
    new npc_roshak();
    new npc_quetzal();
    new npc_damren();
    new npc_iron_qons_spear();
    new creature_script<npc_rushing_wind>("npc_rushing_wind");
    new creature_script<npc_lightnig_cloud>("npc_lightnig_cloud");
    new spell_script<spell_unleashed_flame_selector>("spell_unleashed_flame_selector");
    new spell_script<spell_unleashed_flame_eff>("spell_unleashed_flame_eff");
    new spell_script<spell_rushing_wind_pull>("spell_rushing_wind_pull");
    new spell_script<spell_rushing_wind_targeting>("spell_rushing_wind_targeting");
    new aura_script<spell_storm_cloud_eff>("spell_storm_cloud_eff");
    new spell_script<spell_arcing_lightning_selector>("spell_arcing_lightning_selector");
    new aura_script<spell_arcing_lightning>("spell_arcing_lightning");
    new spell_script<spell_arcing_lightning_proc_selector>("spell_arcing_lightning_proc_selector");
    new aura_script<spell_arcing_lightning_proc_eff_aura>("spell_arcing_lightning_proc_eff_aura");
    new spell_script<spell_arcing_lightning_proc_eff>("spell_arcing_lightning_proc_eff");
    new spell_script<spell_arcing_lightning_proc_selector_2>("spell_arcing_lightning_proc_selector_2");
    new spell_script<spell_freeze_selector>("spell_freeze_selector");
    new aura_script<spell_freeze_aura>("spell_freeze_aura");
    new aura_script<spell_frozen_resilence>("spell_frozen_resilence");
    new spell_script<spell_dead_zone>("spell_dead_zone");
    new aura_script<spell_molten_overload>("spell_molten_overload");
    new spell_script<spell_throw_spear_missle_eff>("spell_throw_spear_missle_eff");
    new spell_script<spell_rushing_winds_misc>("spell_rushing_winds_misc");
    new spell_script<spell_enchant_cyclone_ignite>("spell_enchant_cyclone_ignite");
    new spell_script<spell_enchant_cyclone_freeze>("spell_enchant_cyclone_freeze");
    new aura_script<spell_rushing_winds_vehicle_aura>("spell_rushing_winds_vehicle_aura");
    new spell_script<spell_rushing_winds_vehicle>("spell_rushing_winds_vehicle");
    new spell_script<spell_frost_spike_selector>("spell_frost_spike_selector");
    new atrigger_script<sat_iron_qon_burning_cinders>("sat_iron_qon_burning_cinders");
    new atrigger_script<sat_iron_qon_storm_cloud>("sat_iron_qon_storm_cloud");
    new atrigger_script<sat_iron_qon_frozen_blood>("sat_iron_qon_frozen_blood");
    new atrigger_script<sat_iron_qon_rushing_winds>("sat_iron_qon_rushing_winds");
    new atrigger_script<sat_iron_qon_dead_zone>("sat_iron_qon_dead_zone");
    new atrigger_script<sat_iron_qon_whirling_winds>("sat_iron_qon_whirling_winds");
}
