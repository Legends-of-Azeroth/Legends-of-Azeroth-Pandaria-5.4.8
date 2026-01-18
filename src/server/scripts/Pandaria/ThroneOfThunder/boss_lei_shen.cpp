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
#include "throne_of_thunder.h"

enum Spells
{
    SPELL_DISCHARGED_ENERGY                      = 134821,
    SPELL_DISCHARGED_ENERGY_DEBUFF               = 137446, // client hide damage inc
    SPELL_DISCHARGED_ENERGY_AURA                 = 134820, // by encounter
    SPELL_ELECTRICAL_CONDUIT                     = 134800, // AreaTrigger
    SPELL_DECAPITATE_EFF                         = 134916,
    SPELL_DECAPITATE_SCRIPT                      = 134919, // triggered by 134912, possible planed like trigger for dest position of throw? but not used
    SPELL_DECAPITATE_MISSLE                      = 134990,
    SPELL_DECAPITATE_MARK                        = 134912,
    SPELL_DECAPITATE_MARK_2                      = 135000, // apply into mark 1
    SPELL_THUNDERSTRUCK_EFF                      = 135096,
    SPELL_THUNDERSTRUCK_SUMMON                   = 135098, // by player
    SPELL_THUNDERSTRUCK_MISSLE                   = 135095,
    SPELL_THUNDERSTRUCK_SELECTOR                 = 135091,
    SPELL_CRASHING_THUNDER                       = 135150,
    SPELL_CRASHING_THUNDER_AT_EFF                = 135153,
    SPELL_CRASHING_THUNDER_SELECTOR              = 135151,
    SPELL_CRASHING_THUNDER_EFF_PROC              = 135143, // on remove this - cast 135151 - > 135150 (priority tank, meelee, caster)
    SPELL_FUSION_SLASH                           = 136478,
    SPELL_LIGHTNING_WHIP                         = 136850,
    SPELL_LIGHTNING_WHIP_AT                      = 137499,
    SPELL_LIGHTNING_WHIP_SELECTOR                = 136845,
    SPELL_LIGHTNING_WHIP_SUMMON                  = 136846,
    SPELL_LIGHTNING_WHIP_AT_EFF                  = 136853,
    SPELL_SUMMON_BALL_LIGHTNING                  = 136543,
    SPELL_BALL_LIGHTNING_SELECTOR                = 136544,
    SPELL_BALL_LIGHTNING_JUMP                    = 136548,
    SPELL_BALL_LIGHTNIG_JUMP_EFF                 = 136620,
    SPELL_BALL_LIGHTNING_VISUAL                  = 136534,
    SPELL_OVERWHELMING_POWER                     = 136913,
    SPELL_OVERWHELMING_POWER_PROC                = 136914,
    SPELL_STATIC_SHOCK                           = 135695, // north
    SPELL_STATIC_SHOCK_EFF                       = 135703, // recalculate dmg at conduit level up
    SPELL_DIFFUSION_CHAIN_EFF                    = 135991, // east
    SPELL_DIFFUSION_CHAIN_SUMM_EFF_LVL_1         = 135992,
    SPELL_DIFFUSION_CHAIN_SUMM_EFF_LVL_2         = 135993,
    SPELL_DIFFUSION_CHAIN_SUMM_EFF_LVL_3         = 135994,
    SPELL_CHAIN_LIGHTNING_LVL_1                  = 136018,
    SPELL_CHAIN_LIGHTNING_LVL_2                  = 136019,
    SPELL_CHAIN_LIGHTNING_LVL_3                  = 136021,
    SPELL_DIFFUSION_CHAIN_SELECTOR               = 135990,
    SPELL_OVERCHARGED                            = 136295,
    SPELL_OVERCHARGED_HIT_EFF                    = 136326,
    SPELL_OVERCHARGED_AREATRIGGER                = 136322,
    SPELL_BOUNCING_BOLT_FALL_EFF                 = 136366,
    SPELL_BOUNCING_BOLT_MISSLE_1                 = 136361, // lvl 1
    SPELL_BOUNCING_BOLT_MISSLE_2                 = 138706, // lvl 2,3
    SPELL_BOUNCING_BOLT_SELECTOR                 = 136397,
    SPELL_BOUNCING_BOLT_SUMM_1                   = 136372,
    SPELL_BOUNCING_BOLT_SUMM_2                   = 136391, // dummy for extra hit?

    // Dark Winds
    SPELL_VIOLENT_GALE_WINDS                     = 136869,
    SPELL_VIOLENT_GALE_WINDS_AT_EFF              = 136889,
    SPELL_VIOLENT_GALE_WINDS_AREATRIGGER_NE      = 136867,
    SPELL_VIOLENT_GALE_WINDS_AREATRIGGER_NW      = 136876,
    SPELL_VIOLENT_GALE_WINDS_AREATRIGGER_SW      = 136877,
    SPELL_VIOLENT_GALE_WINDS_AREATRIGGER_SE      = 136879,

    // Supercharge Station
    SPELL_SUPERCHARGE_CONDUITS_EFF               = 137045,
    SPELL_SUPERCHARGE_CONDUITS_KNOCKBACK         = 138032,
    SPELL_SUPERCHARGE_CONDUITS                   = 137146,
    SPELL_SUPERCHARGE_CONDUITS_2                 = 137150,
                                                 
    // Misc | Cosmetic
    SPELL_DISPLACE                               = 136948, // teleport
    SPELL_COSMETIC_TELEPORT_EAST                 = 139102,
    SPELL_COSMETIC_TELEPORT_NORTH                = 139103,
    SPELL_COSMETIC_TELEPORT_WEST                 = 139104,
    SPELL_COSMETIC_TELEPORT_SOUTH                = 139105,
    SPELL_COSMETIC_VISUAL_OVERCHARGE             = 138275, // not in combat
    SPELL_CONDUIT_LEVEL_FORCE                    = 137606,
    SPELL_CONDUIT_LEVEL                          = 137607, // by player
    SPELL_CONDUIT_LEVEL_OVERRIDE                 = 137611,
    SPELL_BOUNCING_BOLT_CONDUIT_LEVEL            = 137581,
    SPELL_STATIC_SHOCK_CONDUIT_LEVEL             = 139271,
    SPELL_OVERCHARGE_CONDUIT_LEVEL               = 139272,
    SPELL_DIFFUSION_CHAIN_CONDUIT_LEVEL          = 139273,
    SPELL_METER_VISUAL_LEVEL_2                   = 138974, // 45s, target_unk_125
    SPELL_METER_VISUAL_LEVEL_3                   = 138975,
    SPELL_DESPAWN_PERSISTENT_AREA_AURAS          = 137182,
    SPELL_ELECTRICAL_CONDUIT_DUMMY               = 134803, // by conduit to encounter
    SPELL_OVERCHARGE_CONDUIT_AURA                = 135682,
    SPELL_BOUNCING_BOLT_CONDUIT_AURA             = 135683,
    SPELL_STATIC_SHOCK_CONDUIT_AURA              = 135680,
    SPELL_DIFFUSION_CHAIN_CONDUIT_AURA           = 135681,
    SPELL_IN_NORTH_QUADRANT                      = 139006, // some spells should`ve selected by quadrant side
    SPELL_IN_EAST_QUADRANT                       = 139007,
    SPELL_IN_SOUTH_QUADRANT                      = 139008,
    SPELL_IN_WEST_QUADRANT                       = 139009,
    SPELL_AMPLIFIER                              = 138070, // by 69133
    SPELL_OVERLOADED_CIRCUITS                    = 137176, // by player
    SPELL_OVERLOAD_CIRCUITS_NORTH                = 137153,
    SPELL_OVERLOAD_CIRCUITS_EAST                 = 137169,
    SPELL_OVERLOAD_CIRCUITS_SOUTH                = 137170,
    SPELL_OVERLOAD_CIRCUITS_WEST                 = 137171,

    // Heroic
    SPELL_HELM_OF_COMMAND                        = 139011,
};

enum Events
{
    EVENT_DECAPITATE = 1,
    EVENT_THUNDERSTRUCK,
    EVENT_CRASHING_THUNDER,
    EVENT_LIGHTNING_WHIP,
    EVENT_BALL_LIGHTNING,
    EVENT_FUSION_SLASH,
    EVENT_BERSERK,
    EVENT_STATIC_SHOCK,
    EVENT_DIFFUSION_CHAIN,
    EVENT_BOUNCING_BOLT,
    EVENT_OVERCHARGE,
    EVENT_VIOLENT_GALE_WINDS,
    EVENT_HELM_OF_COMMAND,
    EVENT_REMOVE_INACTIVITY,
};

enum Yells
{
    TALK_INTRO,
    TALK_AGGRO,
    TALK_THUNDERSTRUCK,
    TALK_PHASE_SWITCH_1,
    TALK_PHASE_SWITCH_DONE_1,
    TALK_LIGHTNING_WHIP,
    TALK_PHASE_SWITCH_2,
    TALK_PHASE_SWITCH_DONE_2,
    TALK_DEAD,
    TALK_THUNDERSTRUCK_ANN,
    TALK_SUPERCHARGED_ANN,
    TALK_LIGHTNING_WHIP_ANN,
    TALK_NW_WINDOW_ANN,
    TALK_NE_WINDOW_ANN,
    TALK_SE_WINDOW_ANN,
    TALK_SW_WINDOW_ANN,
};

enum Actions
{
    ACTION_CANCEL_OVERCHARGE_CONDUITS,
    ACTION_INIT_CONDUIT_FRAMES,
    ACTION_RESET_CONDUIT,
    ACTION_SET_CONDUIT_LEVEL,
    ACTION_RESET_CONDUIT_LEVEL,
    ACTION_OVERRIDE_THIS_CONDUIT,
    ACTION_LAST_PHASE,
};

enum Phases
{
    PHASE_NONE,
    PHASE_DEFAULT,
    PHASE_OVERCHARGE_CONDUITS,
    PHASE_LORD_OF_THUNDER,
};

const std::vector<uint32> conduitsEntry = 
{
    NPC_STATIC_SHOCK_CONDUIT,
    NPC_BOUNCING_BOLT_CONDUIT,
    NPC_OVERCHARGE_CONDUIT,
    NPC_DIFFUSION_CHAIN_CONDUIT,
};

const std::map<uint32, uint32> invCosmeticType =
{
    { SPELL_COSMETIC_TELEPORT_EAST,  NPC_DIFFUSION_CHAIN_CONDUIT },
    { SPELL_COSMETIC_TELEPORT_NORTH, NPC_STATIC_SHOCK_CONDUIT    },
    { SPELL_COSMETIC_TELEPORT_WEST,  NPC_BOUNCING_BOLT_CONDUIT   },
    { SPELL_COSMETIC_TELEPORT_SOUTH, NPC_OVERCHARGE_CONDUIT      },
};

const std::map<uint32, uint32> invConduitType =
{
    { NPC_DIFFUSION_CHAIN_CONDUIT, SPELL_DIFFUSION_CHAIN_CONDUIT_AURA },
    { NPC_STATIC_SHOCK_CONDUIT,    SPELL_STATIC_SHOCK_CONDUIT_AURA    },
    { NPC_BOUNCING_BOLT_CONDUIT,   SPELL_BOUNCING_BOLT_CONDUIT_AURA   },
    { NPC_OVERCHARGE_CONDUIT,      SPELL_OVERCHARGE_CONDUIT_AURA      },
};

const std::map<uint32, uint32> invConduitLevelType =
{
    { NPC_DIFFUSION_CHAIN_CONDUIT, SPELL_DIFFUSION_CHAIN_CONDUIT_LEVEL },
    { NPC_STATIC_SHOCK_CONDUIT,    SPELL_STATIC_SHOCK_CONDUIT_LEVEL    },
    { NPC_BOUNCING_BOLT_CONDUIT,   SPELL_BOUNCING_BOLT_CONDUIT_LEVEL   },
    { NPC_OVERCHARGE_CONDUIT,      SPELL_OVERCHARGE_CONDUIT_LEVEL      },
};

const std::map<uint32, uint32> invConduitFloorType =
{
    { NPC_DIFFUSION_CHAIN_CONDUIT, GO_FLOOR_FX_EAST  },
    { NPC_STATIC_SHOCK_CONDUIT,    GO_FLOOR_FX_NORTH },
    { NPC_BOUNCING_BOLT_CONDUIT,   GO_FLOOR_FX_WEST  },
    { NPC_OVERCHARGE_CONDUIT,      GO_FLOOR_FX_SOUTH },
};

const std::map<uint32, uint32> invConduitGrantType =
{
    { SPELL_OVERCHARGE_CONDUIT_AURA,      EVENT_OVERCHARGE      },
    { SPELL_BOUNCING_BOLT_CONDUIT_AURA,   EVENT_BOUNCING_BOLT   },
    { SPELL_STATIC_SHOCK_CONDUIT_AURA,    EVENT_STATIC_SHOCK    },
    { SPELL_DIFFUSION_CHAIN_CONDUIT_AURA, EVENT_DIFFUSION_CHAIN },
};

const std::map<uint32, uint32> invConduitPillarType =
{
    { NPC_DIFFUSION_CHAIN_CONDUIT, GO_EAST_PILLAR  },
    { NPC_STATIC_SHOCK_CONDUIT,    GO_NORTH_PILLAR },
    { NPC_BOUNCING_BOLT_CONDUIT,   GO_WEST_PILLAR  },
    { NPC_OVERCHARGE_CONDUIT,      GO_SOUTH_PILLAR },
};

const std::map<uint32, uint32> invQuadrantSideType =
{
    { 8910, SPELL_IN_NORTH_QUADRANT },
    { 8911, SPELL_IN_EAST_QUADRANT },
    { 8912, SPELL_IN_SOUTH_QUADRANT },
    { 8913, SPELL_IN_WEST_QUADRANT },
};

const std::map<uint32, uint32> invOverrideConduitType =
{
    { NPC_DIFFUSION_CHAIN_CONDUIT, SPELL_OVERLOAD_CIRCUITS_EAST },
    { NPC_STATIC_SHOCK_CONDUIT,    SPELL_OVERLOAD_CIRCUITS_NORTH },
    { NPC_BOUNCING_BOLT_CONDUIT,   SPELL_OVERLOAD_CIRCUITS_WEST },
    { NPC_OVERCHARGE_CONDUIT,      SPELL_OVERLOAD_CIRCUITS_SOUTH },
};

const std::map<uint32, std::array<uint32, 3>> invViolentWindsType =
{
    { SPELL_VIOLENT_GALE_WINDS_AREATRIGGER_NW, { GO_WIND_FX_NW, GO_NW_WINDOW, TALK_NW_WINDOW_ANN } },
    { SPELL_VIOLENT_GALE_WINDS_AREATRIGGER_NE, { GO_WIND_FX_NE, GO_NE_WINDOW, TALK_NE_WINDOW_ANN } },
    { SPELL_VIOLENT_GALE_WINDS_AREATRIGGER_SE, { GO_WIND_FX_SE, GO_SE_WINDOW, TALK_SE_WINDOW_ANN } },
    { SPELL_VIOLENT_GALE_WINDS_AREATRIGGER_SW, { GO_WIND_FX_SW, GO_SW_WINDOW, TALK_SW_WINDOW_ANN } },
};

const std::map<uint32, uint32> invdiffusedLightningType =
{
    { NPC_LESSER_DIFFUSING_LIGHTNING,  SPELL_CHAIN_LIGHTNING_LVL_1 },
    { NPC_DIFFUSING_LIGHTNING,         SPELL_CHAIN_LIGHTNING_LVL_2 },
    { NPC_GREATER_DIFFUSING_LIGHTNING, SPELL_CHAIN_LIGHTNING_LVL_3 },
};

const std::vector<uint32> quadrants =
{
    SPELL_IN_NORTH_QUADRANT,
    SPELL_IN_EAST_QUADRANT,
    SPELL_IN_SOUTH_QUADRANT,
    SPELL_IN_WEST_QUADRANT,
};

const std::map<uint32, uint32> invdConduitAchievementType =
{
    { NPC_DIFFUSION_CHAIN_CONDUIT, WORLDSTATE_DIFFUSION_CHAIN_CONDUIT_FIRST },
    { NPC_STATIC_SHOCK_CONDUIT,    WORLDSTATE_STATIC_SHOCK_CONDUIT_FIRST    },
    { NPC_BOUNCING_BOLT_CONDUIT,   WORLDSTATE_BOUNCING_BOLT_CONDUIT_FIRST   },
    { NPC_OVERCHARGE_CONDUIT,      WORLDSTATE_OVERLOAD_CONDUIT_FIRST        },
};

// Lei Shen 68397
class boss_lei_shen : public CreatureScript
{
    public:
        boss_lei_shen() : CreatureScript("boss_lei_shen") { }
    
        struct boss_lei_shenAI : public BossAI
        {
            boss_lei_shenAI(Creature* creature) : BossAI(creature, DATA_LEI_SHEN) { }

            EventMap berserkEvents;
            ObjectGuid targetGUID;
            uint32 phaseId, galeWindsEntry;
            int32 healthPct;
            std::vector<uint32> stealedConduit;
            bool hasOverchargedFirst;

            void Reset() override
            {
                events.Reset();
                berserkEvents.Reset();
                SetEquipmentSlots(false, EQUIPMENT_AXE, 0);
                me->SetReactState(REACT_AGGRESSIVE);
                _Reset();
                DoCast(me, SPELL_COSMETIC_TELEPORT_SOUTH);
                targetGUID = ObjectGuid::Empty;
                galeWindsEntry = 0;
                healthPct = 65;
                phaseId = PHASE_NONE;
                stealedConduit.clear();
                hasOverchargedFirst = false;
                scheduler.CancelAll();

                me->GetMap()->SetWorldState(WORLDSTATE_DIFFUSION_CHAIN_CONDUIT_FIRST, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_OVERLOAD_CONDUIT_FIRST, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_STATIC_SHOCK_CONDUIT_FIRST, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_BOUNCING_BOLT_CONDUIT_FIRST, 0);

                berserkEvents.ScheduleEvent(EVENT_REMOVE_INACTIVITY, 2 * IN_MILLISECONDS); // scheduler stuck here...
            }
    
            void JustEngagedWith(Unit* who) override
            {
                _JustEngagedWith();
    
                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                Talk(TALK_AGGRO);
                events.ScheduleEvent(EVENT_DECAPITATE, 40 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_THUNDERSTRUCK, 25 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CRASHING_THUNDER, 7.5 * IN_MILLISECONDS);
                berserkEvents.ScheduleEvent(EVENT_BERSERK, 15 * MINUTE * IN_MILLISECONDS);

                // Conduit frames
                HandleSendActionToConduits(ACTION_INIT_CONDUIT_FRAMES);
                phaseId = PHASE_DEFAULT;
            }
    
            void EnterEvadeMode() override
            {
                _EnterEvadeMode();
                BossAI::EnterEvadeMode();
    
                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
    
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_LEI_SHEN, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CONDUIT_LEVEL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DECAPITATE_EFF);

                    // Deactivate visual station
                    if (GameObject* overchargedStation = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_CHARGING_STATION)))
                        overchargedStation->SetGoState(GO_STATE_READY);

                    // Remove Collision
                    if (GameObject* discCollision = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_THUNDER_KING_DISC_COLLISION)))
                        discCollision->Delete();

                    // Deactivate floors
                    for (auto&& itr : invConduitFloorType)
                    {
                        if (GameObject* floor = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(itr.second) : ObjectGuid::Empty))
                        {
                            floor->SetGoState(GO_STATE_READY);
                            floor->SetAnimKitId(0, false);
                        }
                    }

                    // Close Windows and Deactivate Winds
                    for (auto&& itr : invViolentWindsType)
                    {
                        if (GameObject* myWind = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(itr.second[0])))
                            myWind->SetGoState(GO_STATE_READY);

                        if (GameObject* myWindow = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(itr.second[1])))
                            myWindow->SetGoState(GO_STATE_READY);
                    }
                }

                // Conduits Reset
                HandleSendActionToConduits(ACTION_RESET_CONDUIT);
                me->RemoveAllAreasTrigger();

                scheduler.CancelAll();
                berserkEvents.Reset();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                _DespawnAtEvade();
                me->GetMotionMaster()->MoveTargetedHome();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case TYPE_LEI_SHEN_PHASE:
                        return phaseId;
                    case TYPE_VIOLENT_GALE_WINDS:
                        return galeWindsEntry;
                }

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_VIOLENT_GALE_WINDS)
                    galeWindsEntry = data;
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_CANCEL_OVERCHARGE_CONDUITS:
                    {
                        Talk(TALK_SUPERCHARGED_ANN);

                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    
                        // Deactivate visual station
                        if (GameObject* overchargedStation = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GO_CHARGING_STATION) : ObjectGuid::Empty))
                            overchargedStation->SetGoState(GO_STATE_READY);
                    
                        // Remove Collision
                        if (GameObject* discCollision = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GO_THUNDER_KING_DISC_COLLISION) : ObjectGuid::Empty))
                            discCollision->Delete();
                    
                        // Deactivate Visual Floors
                        for (auto&& itr : invConduitFloorType)
                            if (GameObject* myFloor = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(itr.second) : ObjectGuid::Empty))
                                myFloor->SetGoState(GO_STATE_READY);

                        // Remove Additional Events
                        for (auto&& itr : invConduitGrantType)
                            events.CancelEvent(itr.second);
                    
                        // Select Conduit with max power and override him
                        std::map <uint32, uint32> conduitsDict;
                    
                        for (auto&& itr : conduitsEntry)
                            if (Creature* conduit = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(itr) : ObjectGuid::Empty))
                                conduitsDict.insert(std::pair<uint32, uint32>(conduit->AI()->GetData(TYPE_CONDUIT_LEVEL) * 100 + conduit->AI()->GetData(TYPE_CONDUIT_LEVEL_POWER), conduit->GetEntry()));

                        auto key = std::max_element(conduitsDict.begin(), conduitsDict.end(), conduitsDict.value_comp());

                        if (Creature* overrideConduit = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(key->second) : ObjectGuid::Empty))
                        {
                            overrideConduit->AI()->DoAction(ACTION_OVERRIDE_THIS_CONDUIT);

                            // Achievement control
                            if (!hasOverchargedFirst)
                            {
                                hasOverchargedFirst = true;
                                me->GetMap()->SetWorldState(invdConduitAchievementType.find(overrideConduit->GetEntry())->second, 1);
                            }

                            // get this
                            if (IsHeroic())
                            {
                                auto spellKey = invConduitType.find(overrideConduit->GetEntry())->second; 
                                stealedConduit.push_back(invConduitGrantType.find(spellKey)->second);

                                for (auto&& itr : stealedConduit)
                                    events.ScheduleEvent(itr, urand(25 * IN_MILLISECONDS, 55 * IN_MILLISECONDS));
                            }
                        }

                        // Discharge Conduit Additional Level and Reset Power
                        HandleSendActionToConduits(ACTION_RESET_CONDUIT_LEVEL);
                    
                        DoCast(me, SPELL_CONDUIT_LEVEL_OVERRIDE, true);               
                        
                        scheduler
                            .SetValidator([this] { return instance && instance->GetBossState(DATA_LEI_SHEN) == IN_PROGRESS; })
                            .Schedule(Milliseconds(3000), [this](TaskContext /*context*/)
                        {
                            phaseId = me->HealthBelowPct(30) ? PHASE_LORD_OF_THUNDER : PHASE_DEFAULT;
                            me->RemoveChanneledCast(targetGUID);

                            if (phaseId == PHASE_LORD_OF_THUNDER)
                            {
                                Talk(TALK_PHASE_SWITCH_2);
                                SetEquipmentSlots(false, EQUIPMENT_AXE, EQUIPMENT_POLEARM);
                                DoCast(me, SPELL_OVERWHELMING_POWER);
                                return;
                            }

                            SetEquipmentSlots(false, EQUIPMENT_POLEARM, 0);
                            Talk(TALK_PHASE_SWITCH_1);
                            events.ScheduleEvent(EVENT_BALL_LIGHTNING, 14.5 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_LIGHTNING_WHIP, 29.5 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_FUSION_SLASH, 43.5 * IN_MILLISECONDS);
                            DoCast(me, SPELL_DISCHARGED_ENERGY_AURA);
                        });
                        break;
                    }
                    case SPELL_STATIC_SHOCK_CONDUIT_AURA:
                    case SPELL_DIFFUSION_CHAIN_CONDUIT_AURA:
                    case SPELL_OVERCHARGE_CONDUIT_AURA:
                    case SPELL_BOUNCING_BOLT_CONDUIT_AURA:
                        events.ScheduleEvent(invConduitGrantType.find(actionId)->second, 9.5 * IN_MILLISECONDS);
                        break;
                    case SPELL_STATIC_SHOCK_CONDUIT_AURA - 4:
                    case SPELL_DIFFUSION_CHAIN_CONDUIT_AURA - 4:
                    case SPELL_OVERCHARGE_CONDUIT_AURA - 4:
                    case SPELL_BOUNCING_BOLT_CONDUIT_AURA - 4:
                        events.CancelEvent(invConduitGrantType.find(actionId + 4)->second);
                        break;
                    case ACTION_LAST_PHASE:
                        events.ScheduleEvent(EVENT_VIOLENT_GALE_WINDS, 14 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_BALL_LIGHTNING, 35.5 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_LIGHTNING_WHIP, 15.5 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_THUNDERSTRUCK, 30 * IN_MILLISECONDS);
                        break;
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->setActive(true);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                // Prevent teleport if we casting anything
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (healthPct > 0 && HealthBelowPct((uint32)healthPct))
                {
                    Talk(healthPct > 31 ? TALK_PHASE_SWITCH_1 : TALK_PHASE_SWITCH_2);

                    healthPct -= 35;

                    if (Unit* target = me->GetVictim())
                        targetGUID = target->GetGUID();

                    events.Reset();
                    me->PrepareChanneledCast(me->GetOrientation());
                    phaseId = PHASE_OVERCHARGE_CONDUITS;
                    me->RemoveAurasDueToSpell(SPELL_DISCHARGED_ENERGY_AURA);
                    me->RemoveAreaTrigger(SPELL_DISCHARGED_ENERGY_AURA);
                    DoCast(me, SPELL_DISPLACE);

                    // Should`ve triggered from displace, but wrong synch (knockback before teleport)
                    scheduler
                        .SetValidator([this] { return instance && instance->GetBossState(DATA_LEI_SHEN) == IN_PROGRESS; })
                        .Schedule(Seconds(1), [this](TaskContext /*context*/)
                    {
                        DoCast(me, SPELL_SUPERCHARGE_CONDUITS_KNOCKBACK, true);

                        events.ScheduleEvent(EVENT_DIFFUSION_CHAIN, 5.8 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_OVERCHARGE, 5.9 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_BOUNCING_BOLT, 17.5 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_STATIC_SHOCK, 18.5 * IN_MILLISECONDS);

                        if (IsHeroic())
                            events.ScheduleEvent(EVENT_HELM_OF_COMMAND, 7.5 * IN_MILLISECONDS);
                    });
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                Talk(TALK_DEAD);
                // Conduits Reset
                HandleSendActionToConduits(ACTION_RESET_CONDUIT);

                // override visual station
                if (GameObject* overchargedStation = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GO_CHARGING_STATION) : ObjectGuid::Empty))
                    overchargedStation->SetGoState(GO_STATE_READY);

                // Remove Collision
                if (GameObject* discCollision = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GO_THUNDER_KING_DISC_COLLISION) : ObjectGuid::Empty))
                    discCollision->Delete();

                // override conduits
                for (auto&& itr : invConduitPillarType)
                    if (GameObject* pillar = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(itr.second) : ObjectGuid::Empty))
                        pillar->SetAnimKitId(1681, false);

                // destroy floors
                for (auto&& itr : invConduitFloorType)
                    if (GameObject* floor = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(itr.second) : ObjectGuid::Empty))
                        floor->SetAnimKitId(1681, false);

                me->RemoveAllAreasTrigger();
                berserkEvents.Reset();

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CONDUIT_LEVEL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DECAPITATE_EFF);

                    if (!IsHeroic())
                        return;

                    for (auto&& itr : instance->instance->GetPlayers())
                        if (Player* player = itr.GetSource())
                            player->SendCinematicStart(CINEMATIC_RADEN_INTRO);

                    // Close Windows and Deactivate Winds
                    for (auto&& itr : invViolentWindsType)
                    {
                        if (GameObject* myWind = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(itr.second[0])))
                            myWind->SetGoState(GO_STATE_READY);

                        if (GameObject* myWindow = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(itr.second[1])))
                            myWindow->SetGoState(GO_STATE_READY);
                    }
                }
            }
    
            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);
                berserkEvents.Update(diff);

                while (uint32 eventId = berserkEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            break;
                        case EVENT_REMOVE_INACTIVITY:
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                            break;
                    }
                }

                if (!UpdateVictim())
                    return;
    
                events.Update(diff);
    
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DECAPITATE:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_DECAPITATE_MARK);

                            events.ScheduleEvent(EVENT_DECAPITATE, urand(48 * IN_MILLISECONDS, 50 * IN_MILLISECONDS));
                            break;
                        case EVENT_THUNDERSTRUCK:
                            Talk(TALK_THUNDERSTRUCK);
                            Talk(TALK_THUNDERSTRUCK_ANN);
                            DoCast(me, SPELL_THUNDERSTRUCK_SELECTOR);
                            events.ScheduleEvent(EVENT_THUNDERSTRUCK, 45 * IN_MILLISECONDS);
                            break;
                        case EVENT_CRASHING_THUNDER:
                            DoCast(me, SPELL_CRASHING_THUNDER_SELECTOR);
                            events.ScheduleEvent(EVENT_CRASHING_THUNDER, urand(30.5 * IN_MILLISECONDS, 31.5 * IN_MILLISECONDS));
                            break;
                        case EVENT_STATIC_SHOCK:
                            DoCast(me, SPELL_STATIC_SHOCK);
                            events.ScheduleEvent(EVENT_STATIC_SHOCK, phaseId == PHASE_OVERCHARGE_CONDUITS ? 18.5 * IN_MILLISECONDS : 40 * IN_MILLISECONDS);
                            break;
                        case EVENT_OVERCHARGE:
                            DoCast(me, SPELL_OVERCHARGED);
                            events.ScheduleEvent(EVENT_OVERCHARGE, phaseId == PHASE_OVERCHARGE_CONDUITS ? 23.3 * IN_MILLISECONDS : 40 * IN_MILLISECONDS);
                            break;
                        case EVENT_DIFFUSION_CHAIN:
                            DoCast(me, SPELL_DIFFUSION_CHAIN_SELECTOR);
                            events.ScheduleEvent(EVENT_DIFFUSION_CHAIN, phaseId == PHASE_OVERCHARGE_CONDUITS ? 23.4 * IN_MILLISECONDS : 40 * IN_MILLISECONDS);
                            break;
                        case EVENT_BOUNCING_BOLT:
                            DoCast(me, SPELL_BOUNCING_BOLT_SELECTOR, true);
                            events.ScheduleEvent(EVENT_BOUNCING_BOLT, phaseId == PHASE_OVERCHARGE_CONDUITS ? 17.5 * IN_MILLISECONDS : 40 * IN_MILLISECONDS);
                            break;
                        case EVENT_BALL_LIGHTNING:
                            DoCast(me, SPELL_BALL_LIGHTNING_SELECTOR);
                            events.ScheduleEvent(EVENT_BALL_LIGHTNING, 45.2 * IN_MILLISECONDS);
                            break;
                        case EVENT_LIGHTNING_WHIP:
                            Talk(TALK_LIGHTNING_WHIP);
                            Talk(TALK_LIGHTNING_WHIP_ANN);

                            if (Unit* vict = me->GetVictim())
                                targetGUID = vict->GetGUID();

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                me->PrepareChanneledCast(me->GetAngle(target), SPELL_LIGHTNING_WHIP_SELECTOR);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                me->PrepareChanneledCast(me->GetAngle(target), SPELL_LIGHTNING_WHIP_SELECTOR);
                            else if (Unit* target = me->GetVictim())
                                me->PrepareChanneledCast(me->GetAngle(target), SPELL_LIGHTNING_WHIP_SELECTOR);

                            scheduler
                                .SetValidator([this] { return instance && instance->GetBossState(DATA_LEI_SHEN) == IN_PROGRESS; })
                                .Schedule(Milliseconds(4500), [this](TaskContext /*context*/)
                            {
                                me->RemoveChanneledCast(targetGUID);
                            });

                            events.ScheduleEvent(EVENT_LIGHTNING_WHIP, urand(43 * IN_MILLISECONDS, 45 * IN_MILLISECONDS));
                            break;
                        case EVENT_FUSION_SLASH:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_FUSION_SLASH);

                            events.ScheduleEvent(EVENT_FUSION_SLASH, 42 * IN_MILLISECONDS);
                            break;
                        case EVENT_VIOLENT_GALE_WINDS:
                            DoCast(me, SPELL_VIOLENT_GALE_WINDS);
                            events.ScheduleEvent(EVENT_VIOLENT_GALE_WINDS, 30 * IN_MILLISECONDS);
                            break;
                        case EVENT_HELM_OF_COMMAND:
                            if (phaseId != PHASE_OVERCHARGE_CONDUITS)
                                break;

                            DoCast(me, SPELL_HELM_OF_COMMAND);
                            events.ScheduleEvent(EVENT_HELM_OF_COMMAND, 22.5 * IN_MILLISECONDS);
                            break;
                    }
                }
    
                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 170.0f);
            }

            private:
                void HandleSendActionToConduits(uint32 action_id)
                {
                    for (auto&& itr : conduitsEntry)
                        if (Creature* conduit = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(itr) : ObjectGuid::Empty))
                            conduit->AI()->DoAction(action_id);
                }

                uint64 getQuadrantStalkerGuidBySide(uint32 sideSpell)
                {
                    std::list<Creature*> quadrantStalkersList;
                    GetCreatureListWithEntryInGrid(quadrantStalkersList, me, NPC_QUADRANT_STALKER, 200.0f);

                    for (auto&& itr : quadrantStalkersList)
                        if (itr->HasAura(sideSpell))
                            return itr->GetGUID();

                    return 0;
                }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_lei_shenAI(creature);
        }
};

// Thunderstruck 68574
struct npc_thunderstruck : public ScriptedAI
{
    npc_thunderstruck(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        // Talk(TALK_ANN);

        if (Creature* leiShen = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_LEI_SHEN) : ObjectGuid::Empty))
            leiShen->CastSpell(me, SPELL_THUNDERSTRUCK_MISSLE, false);

        me->SendPlaySpellVisual(29524, me->GetGUID(), 0.1f); // need slow, doesn`t matter - will remove at missle hit
    }

    void UpdateAI(uint32 diff) override { }
};

// Electric conduit 68398, 68698, 68697, 68696
struct npc_electric_conduit : public ScriptedAI
{
    npc_electric_conduit(Creature* creature) : ScriptedAI(creature) 
    {
        instance = me->GetInstanceScript();
    }

    InstanceScript* instance;
    TaskScheduler scheduler;
    uint32 levelPower, level;
    bool isDisabled;

    void Reset() override
    {
        me->RemoveAurasDueToSpell(SPELL_ELECTRICAL_CONDUIT_DUMMY);
        scheduler.CancelAll();
        level      = 1;
        levelPower = 0;
        isDisabled = false;

        // set our quadrant validate
        if (Creature* quadrant = me->FindNearestCreature(NPC_QUADRANT_STALKER, 42.0f, true))
            quadrant->AI()->SetData(TYPE_CONDUIT_OVERRIDE, 0);
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_INIT_CONDUIT_FRAMES:
                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                DoCast(me, invConduitLevelType.find(me->GetEntry())->second, true);

                scheduler
                    .SetValidator([this] { return !isDisabled; })
                    .Schedule(Milliseconds(500), [this](TaskContext context)
                {
                    if (Creature* leiShen = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_LEI_SHEN) : ObjectGuid::Empty))
                    {
                        // Skip if last phase
                        if (leiShen->AI()->GetData(TYPE_LEI_SHEN_PHASE) == PHASE_LORD_OF_THUNDER)
                            return;

                        // Skip if leishen so far away
                        if (me->GetExactDist2d(leiShen) > 30.0f)
                        {
                            if (leiShen->HasAura(SPELL_ELECTRICAL_CONDUIT_DUMMY, me->GetGUID()))
                            {
                                if (GameObject* myFloor = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(invConduitFloorType.find(me->GetEntry())->second) : ObjectGuid::Empty))
                                    myFloor->SetGoState(GO_STATE_READY);

                                if (GameObject* myPillar = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(invConduitPillarType.find(me->GetEntry())->second) : ObjectGuid::Empty))
                                    myPillar->SetGoState(GO_STATE_READY);

                                leiShen->RemoveAurasDueToSpell(SPELL_ELECTRICAL_CONDUIT_DUMMY);
                                leiShen->RemoveAurasDueToSpell(SPELL_CONDUIT_LEVEL_FORCE);
                                me->RemoveAurasDueToSpell(SPELL_ELECTRICAL_CONDUIT_DUMMY);

                                for (auto&& itr : instance->instance->GetPlayers())
                                {
                                    if (Player* player = itr.GetSource())
                                        if (player->IsAlive() && !player->IsGameMaster())
                                            player->RemoveAurasDueToSpell(SPELL_CONDUIT_LEVEL);
                                }
                            }

                            context.Repeat(Milliseconds(500));
                            return;
                        }

                        // Skip if we active
                        if (leiShen->HasAura(SPELL_ELECTRICAL_CONDUIT_DUMMY, me->GetGUID()))
                        {
                            context.Repeat(Milliseconds(500));
                            return;
                        }

                        DoCast(leiShen, SPELL_ELECTRICAL_CONDUIT_DUMMY);

                        if (GameObject* myFloor = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(invConduitFloorType.find(me->GetEntry())->second) : ObjectGuid::Empty))
                            myFloor->SetGoState(GO_STATE_ACTIVE);

                        if (GameObject* myPillar = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(invConduitPillarType.find(me->GetEntry())->second) : ObjectGuid::Empty))
                            myPillar->SetGoState(GO_STATE_ACTIVE);

                        leiShen->CastSpell(leiShen, SPELL_CONDUIT_LEVEL_FORCE, true);
                    }
                    context.Repeat(Milliseconds(500));
                });
                break;
            case ACTION_RESET_CONDUIT:
                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->RemoveAurasDueToSpell(invConduitLevelType.find(me->GetEntry())->second);
                me->RemoveAllAreasTrigger();
                Reset();
                break;
            case ACTION_SET_CONDUIT_LEVEL:
                SetData(TYPE_CONDUIT_LEVEL, GetData(TYPE_CONDUIT_LEVEL) + 1);
                break;
            case ACTION_RESET_CONDUIT_LEVEL:
                levelPower = 0;
                break;
            case ACTION_OVERRIDE_THIS_CONDUIT:
                Talk(TALK_INTRO);
                isDisabled = true;
                levelPower = 0;

                // set our quadrant not validate
                if (Creature* quadrant = me->FindNearestCreature(NPC_QUADRANT_STALKER, 42.0f, true))
                    quadrant->AI()->SetData(TYPE_CONDUIT_OVERRIDE, 1);

                DoCast(me, invOverrideConduitType.find(me->GetEntry())->second, true);

                if (GameObject* floor = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(invConduitFloorType.find(me->GetEntry())->second) : ObjectGuid::Empty))
                    floor->SetAnimKitId(1677, false);
                break;
        }
    }

    void SetData(uint32 type, uint32 data) override
    {
        switch (type)
        {
            case TYPE_CONDUIT_LEVEL_POWER:
                if (levelPower + data >= 100)
                {
                    SetData(TYPE_CONDUIT_LEVEL, GetData(TYPE_CONDUIT_LEVEL) + 1);

                    levelPower = levelPower + data - 100;

                    if (instance)
                    {
                        for (auto&& itr : instance->instance->GetPlayers())
                        {
                            if (Player* player = itr.GetSource())
                                if (player->IsAlive() && !player->IsGameMaster())
                                    player->SetPower(POWER_ALTERNATE_POWER, levelPower);
                        }
                    }
                }
                else
                {
                    levelPower += data;

                    if (instance)
                    {
                        for (auto&& itr : instance->instance->GetPlayers())
                        {
                            if (Player* player = itr.GetSource())
                                if (player->IsAlive() && !player->IsGameMaster())
                                    player->SetPower(POWER_ALTERNATE_POWER, levelPower);
                        }
                    }
                }
                break;
            case TYPE_CONDUIT_LEVEL:
                if (level > 2)
                    break;

                level = data;
                me->SetPower(POWER_ALTERNATE_POWER, level);
                break;
        }
    }

    uint32 GetData(uint32 type) const override
    {
        switch (type)
        {
            case TYPE_CONDUIT_LEVEL_POWER:
                return levelPower;
            case TYPE_CONDUIT_LEVEL:
                return level;
        }

        return 0;
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);
    }
};

// Ball Lightning 69232
struct npc_ball_lightning : public ScriptedAI
{
    npc_ball_lightning(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    InstanceScript* instance;

    void IsSummonedBy(Unit* summoner) override
    {
        instance = me->GetInstanceScript();

        DoCast(me, SPELL_BALL_LIGHTNING_VISUAL, true);

        if (Creature* leiShen = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_LEI_SHEN) : ObjectGuid::Empty))
            leiShen->AI()->JustSummoned(me);

        scheduler
            .Schedule(Seconds(1), [this](TaskContext /*context*/)
        {
            me->SetInCombatWithZone();
            me->AttackStop();
            me->SetReactState(REACT_PASSIVE);
        });

        scheduler
            .SetValidator([this] { return !me->HasUnitState(UNIT_STATE_CASTING); })
            .Schedule(Milliseconds(1500), [this](TaskContext context)
        {
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                DoCast(target, SPELL_BALL_LIGHTNING_JUMP);
            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                DoCast(target, SPELL_BALL_LIGHTNING_JUMP);

            context.Repeat(Milliseconds(4500));
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->DisappearAndDie();
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);
    }
};

// Unharnessed Power 69133
struct npc_unharnessed_power : public ScriptedAI
{
    npc_unharnessed_power(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    InstanceScript* instance;

    void IsSummonedBy(Unit* summoner) override
    {
        instance = me->GetInstanceScript();

        if (instance && instance->GetBossState(DATA_LEI_SHEN) != IN_PROGRESS)
        {
            me->DespawnOrUnsummon();
            return;
        }

        if (Creature* leiShen = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_LEI_SHEN) : ObjectGuid::Empty))
            leiShen->AI()->JustSummoned(me);

        // Continue bouncing cicle if our pull weren`t avoid
        if (Creature* eastConduit = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_DIFFUSION_CHAIN_CONDUIT) : ObjectGuid::Empty))
            if (Creature* nearQuadrant = me->FindNearestCreature(NPC_QUADRANT_STALKER, 40.0f, true))
                DoCast(nearQuadrant, eastConduit->AI()->GetData(TYPE_CONDUIT_LEVEL) > 1 ? SPELL_BOUNCING_BOLT_MISSLE_2 : SPELL_BOUNCING_BOLT_MISSLE_1, true);

        scheduler
            .SetValidator([this] { return IsHeroic(); })
            .Schedule(Seconds(15), [this](TaskContext context)
        {
            DoCast(me, SPELL_AMPLIFIER);

            context.Repeat(Seconds(15));
        });
    }

    void Reset() override
    {
        me->SetInCombatWithZone();
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->DisappearAndDie();
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Diffused Lightning 69012, 69013, 69014
struct npc_diffused_lightning : public ScriptedAI
{
    npc_diffused_lightning(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    InstanceScript* instance;

    void IsSummonedBy(Unit* summoner) override
    {
        instance = me->GetInstanceScript();

        if (Creature* leiShen = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_LEI_SHEN) : ObjectGuid::Empty))
            leiShen->AI()->JustSummoned(me);

        scheduler
            .Schedule(Seconds(1), [this](TaskContext /*context*/)
        {
            me->SetInCombatWithZone();
        });

        scheduler
            .SetValidator([this] { return !me->HasUnitState(UNIT_STATE_CASTING); })
            .Schedule(Seconds(3), Seconds(9), [this](TaskContext context)
        {
            if (Unit* target = me->GetVictim())
                DoCast(target, invdiffusedLightningType.find(me->GetEntry())->second);

            context.Repeat(Seconds(5), Seconds(15));
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->DisappearAndDie();
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Quadrant Stalker 69024
struct npc_quadrant_stalker : public ScriptedAI
{
    npc_quadrant_stalker(Creature* creature) : ScriptedAI(creature) { }

    uint32 conduitOverride;

    void Reset() override
    {
        conduitOverride = 0;
    }

    // Any doesn`t kill this npc. Never.
    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        damage = 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_CONDUIT_OVERRIDE)
            conduitOverride = data;
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_CONDUIT_OVERRIDE)
            return conduitOverride;

        return 0;
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Cosmetic Teleport Side 139102, 139103, 139104, 139105
class spell_cosmetic_teleport_side : public SpellScript
{
    PrepareSpellScript(spell_cosmetic_teleport_side);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Creature* conduit = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(invCosmeticType.find(GetSpellInfo()->Id)->second) : ObjectGuid::Empty))
                caster->CastSpell(conduit, SPELL_COSMETIC_VISUAL_OVERCHARGE, false);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_cosmetic_teleport_side::HandleEffectHit, EFFECT_0, SPELL_EFFECT_TELEPORT_UNITS);
    }
};

// Cosmetic Visual Overcharge 138275
class spell_cosmetic_visual_overcharge : public AuraScript
{
    PrepareAuraScript(spell_cosmetic_visual_overcharge);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* caster = GetCaster()->ToUnit())
        {
            if (Creature* owner = GetOwner()->ToCreature())
            {
                // break if in combat
                if (caster->IsInCombat())
                    return;

                // Select next conduit
                uint32 nextConduitSpell = 0;

                for (auto&& itr : invCosmeticType)
                    if (itr.second == owner->GetEntry())
                        nextConduitSpell = itr.first;

                if (nextConduitSpell)
                    caster->CastSpell(caster, nextConduitSpell > SPELL_COSMETIC_TELEPORT_WEST ? SPELL_COSMETIC_TELEPORT_EAST : ++nextConduitSpell, false);
            }
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_cosmetic_visual_overcharge::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Decapitate 134912
class spell_decapitate : public AuraScript
{
    PrepareAuraScript(spell_decapitate);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* caster = GetCaster()->ToCreature())
            if (Unit* owner = GetOwner()->ToUnit())
                caster->CastSpell(owner, SPELL_DECAPITATE_MARK_2, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_decapitate::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Decapitate Mark 135000
class spell_decapitate_mark : public AuraScript
{
    PrepareAuraScript(spell_decapitate_mark);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* caster = GetCaster())
            if (Unit* owner = GetOwner()->ToUnit())
                caster->CastSpell(owner, SPELL_DECAPITATE_MISSLE, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_decapitate_mark::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Decapitate Eff 134916
class spell_decapitate_eff : public SpellScript
{
    PrepareSpellScript(spell_decapitate_eff);

    void HandleEffectHitTarget(SpellEffIndex /*eff_idx*/)
    {
        if (Unit* unit = GetHitUnit())
        {
            // 75% damage reduction after 30 yards distance
            float dist = unit->GetExactDist2d(GetCaster());

            int32 reduction = dist >= 30 ? GetHitDamage() * 0.25 : GetHitDamage() * (1 - ((dist * 2.5) / 100));
            SetHitDamage(reduction);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_decapitate_eff::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Thunderstruck Selector 135091
class spell_thunderstruck_selector : public SpellScript
{
    PrepareSpellScript(spell_thunderstruck_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_THUNDERSTRUCK_SUMMON, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove meelee, tanks and affected by select aura if possible
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if(MeeleSpecTargetSelector());

            // if we have enough players without tanks, meelee and affected by aura 
            if (!pList.empty())
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (pList.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);

                return;
            }

            // if we haven`t enough players without tanks, meelee and aura affected, then just select anyone
            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_thunderstruck_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thunderstruck_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Thunderstruck Eff 135096
class spell_thunderstruck_eff : public SpellScript
{
    PrepareSpellScript(spell_thunderstruck_eff);

    void HandleEffectHitTarget(SpellEffIndex /*eff_idx*/)
    {
        if (Unit* unit = GetHitUnit())
        {
            if (WorldLocation const* targetDest = GetSpell()->m_targets.GetDstPos())
            {
                // 85% damage reduction after 30 yards distance
                float dist = unit->GetExactDist2d(targetDest);

                int32 reduction = dist >= 30 ? GetHitDamage() * 0.15 : GetHitDamage() * (1 - ((dist * 2.8) / 100));
                SetHitDamage(reduction);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_thunderstruck_eff::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Crashing Thunder Eff Proc 135143
class spell_crashing_thunder_eff_proc : public AuraScript
{
    PrepareAuraScript(spell_crashing_thunder_eff_proc);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_CRASHING_THUNDER_SELECTOR, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_crashing_thunder_eff_proc::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Crashing Thunder Selector 135151
class spell_crashing_thunder_selector : public SpellScript
{
    PrepareSpellScript(spell_crashing_thunder_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_CRASHING_THUNDER, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove whole exclude tanks and meele
            pList.remove_if(CasterSpecTargetSelector());

            // if we have enough players without tanks, meelee and affected by aura 
            if (pList.size() >= 3)
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (pList.size() > 3)
                    Trinity::Containers::RandomResizeList(targets, 3);

                return;
            }

            // if we haven`t enough players without casters then just select anyone
            if (targets.size() > 3)
                Trinity::Containers::RandomResizeList(targets, 3);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_crashing_thunder_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_crashing_thunder_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Lightning Whip Selector 136845
class spell_lightning_whip_selector : public SpellScript
{
    PrepareSpellScript(spell_lightning_whip_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_LIGHTNING_WHIP, false);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove tanks
            pList.remove_if(TankSpecTargetSelector());

            // if we have enough players without tanks, meelee and affected by aura 
            if (!pList.empty())
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (pList.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);

                return;
            }

            // if we haven`t enough players without tanks when push something
            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_lightning_whip_selector::HandleEffectHitTarget, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lightning_whip_selector::SelectTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Ball Lightning Selector 136544
class spell_ball_lightning_selector : public SpellScript
{
    PrepareSpellScript(spell_ball_lightning_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_SUMMON_BALL_LIGHTNING, true);
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

            uint32 minCount = caster->GetMap()->Is25ManRaid() ? 7 : 3;

            // if we have enough players without tanks, meelee
            if (pList.size() >= minCount)
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                return;
            }

            // if we haven`t enough players
            if (targets.size() > minCount + 3)
                Trinity::Containers::RandomResizeList(targets, minCount + 3);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_ball_lightning_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ball_lightning_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Displace 136948
class spell_displace : public SpellScript
{
    PrepareSpellScript(spell_displace);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            // Activate visual station
            if (InstanceScript* instance = caster->GetInstanceScript())
                if (GameObject* overchargedStation = ObjectAccessor::GetGameObject(*caster, instance->GetGuidData(GO_CHARGING_STATION)))
                    overchargedStation->SetGoState(GO_STATE_ACTIVE);

            // Set Collision after knockback
            caster->SummonGameObject(GO_THUNDER_KING_DISC_COLLISION, discCollision.GetPositionX(), discCollision.GetPositionY(), discCollision.GetPositionZ(), discCollision.GetOrientation(), { }, 0);

            // Visual Anim
            caster->CastSpell(caster, SPELL_SUPERCHARGE_CONDUITS_EFF, false);
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_displace::HandleEffectHit, EFFECT_1, SPELL_EFFECT_TELEPORT_UNITS);
    }
};

// Supercharge Conduits 137045
class spell_supercharge_conduits : public AuraScript
{
    PrepareAuraScript(spell_supercharge_conduits);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            owner->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            // Activate Whole Floors
            for (auto&& itr : invConduitFloorType)
                if (GameObject* myFloor = ObjectAccessor::GetGameObject(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(itr.second) : ObjectGuid::Empty))
                    myFloor->SetGoState(GO_STATE_ACTIVE);
        }
    }

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_CANCEL_OVERCHARGE_CONDUITS);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_supercharge_conduits::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_supercharge_conduits::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Electrical Conduit Charge 134803
class spell_electrical_conduit_charge : public AuraScript
{
    PrepareAuraScript(spell_electrical_conduit_charge);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* caster = GetCaster())
        {
            if (Creature* owner = GetOwner()->ToCreature())
            {
                owner->CastSpell(owner, invConduitType.find(caster->GetEntry())->second, true);
                owner->RemoveAurasDueToSpell(SPELL_DISCHARGED_ENERGY_AURA);
                owner->RemoveAreaTrigger(SPELL_DISCHARGED_ENERGY_AURA);
            }
        }
    }

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* caster = GetCaster())
        {
            if (Creature* owner = GetOwner()->ToCreature())
            {
                owner->RemoveAurasDueToSpell(invConduitType.find(caster->GetEntry())->second);

                if (owner->AI()->GetData(TYPE_LEI_SHEN_PHASE) != PHASE_OVERCHARGE_CONDUITS)
                    owner->CastSpell(owner, SPELL_DISCHARGED_ENERGY_AURA, true);
            }
        }
    }

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->SetData(TYPE_CONDUIT_LEVEL_POWER, 2); // each 900 ms

    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_electrical_conduit_charge::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_electrical_conduit_charge::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_electrical_conduit_charge::OnTrigger, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Conduit Grant Ability 135682, 135683, 135680, 135681
class spell_conduit_grant_ability : public AuraScript
{
    PrepareAuraScript(spell_conduit_grant_ability);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(GetSpellInfo()->Id);
    }

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(GetSpellInfo()->Id - 4); // just misc value for remove
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_conduit_grant_ability::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_conduit_grant_ability::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Static Shock 135695
class spell_static_shock : public SpellScript
{
    PrepareSpellScript(spell_static_shock);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            uint32 targetCount = 1;

            if (Creature* conduit = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(NPC_STATIC_SHOCK_CONDUIT) : ObjectGuid::Empty))
                targetCount += conduit->AI()->GetData(TYPE_CONDUIT_LEVEL) - 1;

            // If we cast it while lei shen not in station - then we should cast in ONLY IN DPS
            if (!caster->HasAura(SPELL_SUPERCHARGE_CONDUITS_EFF))
            {
                targets.remove_if(TankSpecTargetSelector());

                if (targets.size() > targetCount)
                    Trinity::Containers::RandomResizeList(targets, targetCount);

                m_targets = targets;
            }
            else // in another case same - doesn`t matter
            {
                targets.remove_if(TankSpecTargetSelector());

                if (targets.size() > targetCount + 1)
                    Trinity::Containers::RandomResizeList(targets, targetCount + 1);

                m_targets = targets;
            }
        }
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_static_shock::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_static_shock::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Static Shock Eff 135703
class spell_static_shock_eff : public SpellScript
{
    PrepareSpellScript(spell_static_shock_eff);

    uint8 targetsCount;

    bool Load() override
    {
        targetsCount = 1;
        return true;
    }

    void RecalculateDamage(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Creature* conduit = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(NPC_STATIC_SHOCK_CONDUIT) : ObjectGuid::Empty))
            {
                int32 damage = GetHitDamage();
                int32 newDamage = damage * conduit->AI()->GetData(TYPE_CONDUIT_LEVEL);

                if (targetsCount > 0)
                    SetHitDamage(newDamage / targetsCount);
            }
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targetsCount = targets.size();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_static_shock_eff::RecalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_static_shock_eff::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Overcharged 136295
class spell_overcharged : public SpellScript
{
    PrepareSpellScript(spell_overcharged);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            uint32 targetCount = caster->GetMap()->Is25ManRaid() ? 2 : 1;

            if (Creature* conduit = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(NPC_OVERCHARGE_CONDUIT) : ObjectGuid::Empty))
                targetCount += conduit->AI()->GetData(TYPE_CONDUIT_LEVEL) - 1;

            // If we cast it while lei shen not in station - then we should cast in ONLY IN DPS
            if (!caster->HasAura(SPELL_SUPERCHARGE_CONDUITS_EFF))
            {
                targets.remove_if(TankSpecTargetSelector());

                if (targets.size() > targetCount)
                    Trinity::Containers::RandomResizeList(targets, targetCount);

                m_targets = targets;
            }
            else // in another case same - doesn`t matter
            {
                targets.remove_if(TankSpecTargetSelector());

                if (targets.size() > targetCount + 1)
                    Trinity::Containers::RandomResizeList(targets, targetCount + 1);

                m_targets = targets;
            }
        }
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_overcharged::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_overcharged::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_overcharged::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_overcharged::CopyTargets, EFFECT_3, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Overcharged 136295
class spell_overcharged_aura : public AuraScript
{
    PrepareAuraScript(spell_overcharged_aura);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_OVERCHARGED_AREATRIGGER, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_overcharged_aura::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
    }
};

// Diffusion Chain Selector 135990
class spell_diffusion_chain_selector : public SpellScript
{
    PrepareSpellScript(spell_diffusion_chain_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_DIFFUSION_CHAIN_EFF, true);
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
        OnEffectHitTarget += SpellEffectFn(spell_diffusion_chain_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_diffusion_chain_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Diffusion Chain Eff 135991
class spell_diffusion_chain_eff : public SpellScript
{
    PrepareSpellScript(spell_diffusion_chain_eff);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            if (Creature* eastConduit = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(NPC_DIFFUSION_CHAIN_CONDUIT) : ObjectGuid::Empty))
            {
                uint32 spell_id = 0;

                switch (eastConduit->AI()->GetData(TYPE_CONDUIT_LEVEL))
                {
                    case 1:
                        spell_id = SPELL_DIFFUSION_CHAIN_SUMM_EFF_LVL_1;
                        break;
                    case 2:
                        spell_id = SPELL_DIFFUSION_CHAIN_SUMM_EFF_LVL_2;
                        break;
                    case 3:
                        spell_id = SPELL_DIFFUSION_CHAIN_SUMM_EFF_LVL_3;
                        break;
                }

                target->CastSpell(target, spell_id, true);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_diffusion_chain_eff::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Bouncing Bolt Selector 136397
class spell_bouncing_bolt_selector : public SpellScript
{
    PrepareSpellScript(spell_bouncing_bolt_selector);

    std::list<WorldObject*> m_targets;

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                if (Creature* eastConduit = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(NPC_DIFFUSION_CHAIN_CONDUIT) : ObjectGuid::Empty))
                {
                    uint32 spell_id = 0;
                    uint32 targetCount = target->GetMap()->Is25ManRaid() ? 4 : 2;

                    switch (eastConduit->AI()->GetData(TYPE_CONDUIT_LEVEL))
                    {
                        case 1:
                            spell_id = SPELL_BOUNCING_BOLT_MISSLE_1;
                            break;
                        case 2:
                        case 3:
                            spell_id = SPELL_BOUNCING_BOLT_MISSLE_2;
                            targetCount += eastConduit->AI()->GetData(TYPE_CONDUIT_LEVEL) - 1;
                            break;
                    }

                    // this is dest random of caster
                    for (uint32 i = 0; i < targetCount; i++)
                        caster->CastSpell(target, spell_id, true);
                }
            }
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        if (Creature* caster = GetCaster()->ToCreature())
        {
            // If we cast it while lei shen not in station - then we should cast in ONLY IN quadrant with same type
            if (!caster->HasAura(SPELL_SUPERCHARGE_CONDUITS_EFF))
            {
                // Couldn`t use both in same remove
                targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_QUADRANT_STALKER; });
                targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && !target->ToCreature()->HasAura(SPELL_IN_WEST_QUADRANT); });

                // Remove Disabled for HC
                targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && target->ToCreature()->AI()->GetData(TYPE_CONDUIT_OVERRIDE); });

                // our main conduit overrided - in case
                if (targets.empty() && !m_targets.empty())
                {
                    targets.clear();

                    for (auto&& itr : m_targets)
                        targets.push_back(itr);

                    targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_QUADRANT_STALKER; });

                    // Remove Disabled
                    targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && target->ToCreature()->AI()->GetData(TYPE_CONDUIT_OVERRIDE); });

                    if (targets.size() > 1)
                        Trinity::Containers::RandomResizeList(targets, 1);
                }
            }
            else // in another case - select 2 quadrants around of conduit
            {
                targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_QUADRANT_STALKER; });

                // Remove Disabled
                targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && target->ToCreature()->AI()->GetData(TYPE_CONDUIT_OVERRIDE); });
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_bouncing_bolt_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_bouncing_bolt_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Conduit Level Override 137611
class spell_conduit_level_override : public SpellScript
{
    PrepareSpellScript(spell_conduit_level_override);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Creature* target = GetHitUnit()->ToCreature())
            target->AI()->DoAction(ACTION_SET_CONDUIT_LEVEL);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_DIFFUSION_CHAIN_CONDUIT && target->GetEntry() != NPC_OVERCHARGE_CONDUIT && target->GetEntry() != NPC_BOUNCING_BOLT_CONDUIT && target->GetEntry() != NPC_STATIC_SHOCK_CONDUIT; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_conduit_level_override::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_conduit_level_override::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Bouncing Bolt Eff 136366
class spell_bouncing_bolt_eff : public SpellScript
{
    PrepareSpellScript(spell_bouncing_bolt_eff);

    uint8 targetsCount;

    bool Load() override
    {
        targetsCount = 1;
        return true;
    }

    void FilterTargets(std::list<WorldObject*> &targets)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (targets.empty())
            {
                if (WorldLocation const* targetDest = GetSpell()->m_targets.GetDstPos())
                    caster->CastSpell(targetDest->GetPositionX(), targetDest->GetPositionY(), targetDest->GetPositionZ(), SPELL_BOUNCING_BOLT_SUMM_1, true);
            }
            else
                targetsCount = targets.size();
        }
    }

    void HandleDamage(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            if (Creature* conduit = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(NPC_BOUNCING_BOLT_CONDUIT) : ObjectGuid::Empty))
            {
                int32 damage = GetHitDamage();
                uint32 levelPower = conduit->AI()->GetData(TYPE_CONDUIT_LEVEL_POWER);

                if (targetsCount > 0)
                    SetHitDamage((damage * conduit->AI()->GetData(TYPE_CONDUIT_LEVEL) + AddPct(damage, levelPower)) / targetsCount);
                else // in case
                    SetHitDamage(damage * conduit->AI()->GetData(TYPE_CONDUIT_LEVEL) + AddPct(damage, levelPower));
            }
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_bouncing_bolt_eff::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_bouncing_bolt_eff::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Overwhelming Power 136913
class spell_overwhelming_power : public AuraScript
{
    PrepareAuraScript(spell_overwhelming_power);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_LAST_PHASE);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_overwhelming_power::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Violent Gale Winds Eff 136889
class spell_violent_gale_winds_eff : public AuraScript
{
    PrepareAuraScript(spell_violent_gale_winds_eff);

    Position temPos; // need save last sent movement force for remove

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Player* target = GetOwner()->ToPlayer())
        {
            if (Creature* leiShen = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(DATA_LEI_SHEN) : ObjectGuid::Empty))
            {
                float sOri = 0.0f;

                switch (leiShen->AI()->GetData(TYPE_VIOLENT_GALE_WINDS))
                {
                    case SPELL_VIOLENT_GALE_WINDS_AREATRIGGER_SW:
                        sOri = 5.46f;
                        break;
                    case SPELL_VIOLENT_GALE_WINDS_AREATRIGGER_SE:
                        sOri = 0.77f;
                        break;
                    case SPELL_VIOLENT_GALE_WINDS_AREATRIGGER_NE:
                        sOri = 2.34f;
                        break;
                    case SPELL_VIOLENT_GALE_WINDS_AREATRIGGER_NW:
                        sOri = 3.91f;
                        break;
                }

                GetPositionWithDistInOrientation(target, 130.0f, sOri, x, y);
                temPos = { x, y, target->GetPositionZ(), 0.0f };
                target->SendApplyMovementForce(true, temPos, 3.9f);
            }
        }
    }

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Player* target = GetOwner()->ToPlayer())
            target->SendApplyMovementForce(false, temPos);
    }

    private:
        float x, y;

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_violent_gale_winds_eff::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        OnEffectRemove += AuraEffectRemoveFn(spell_violent_gale_winds_eff::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Violent Gale Winds 136869
class spell_violent_gale_winds : public SpellScript
{
    PrepareSpellScript(spell_violent_gale_winds);

    void HandleScriptEff(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            std::vector<uint32> windsComboSpellStorage = { SPELL_VIOLENT_GALE_WINDS_AREATRIGGER_NE, SPELL_VIOLENT_GALE_WINDS_AREATRIGGER_NW, SPELL_VIOLENT_GALE_WINDS_AREATRIGGER_SE, SPELL_VIOLENT_GALE_WINDS_AREATRIGGER_SW };

            prevSpellId = caster->AI()->GetData(TYPE_VIOLENT_GALE_WINDS);

            // Remove From Temp container value, that was used before
            if (prevSpellId)
                windsComboSpellStorage.erase(std::find(windsComboSpellStorage.begin(), windsComboSpellStorage.end(), prevSpellId));

            // Select new spell from updated container
            prevSpellId = Trinity::Containers::SelectRandomContainerElement(windsComboSpellStorage);
            caster->AI()->SetData(TYPE_VIOLENT_GALE_WINDS, prevSpellId);

            caster->CastSpell(caster, prevSpellId, true);

            // Open Windows and Visual Floor by spellId
            if (GameObject* myWind = ObjectAccessor::GetGameObject(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(invViolentWindsType.find(prevSpellId)->second[0]) : ObjectGuid::Empty))
                myWind->SetGoState(GO_STATE_ACTIVE);

            if (GameObject* myWind = ObjectAccessor::GetGameObject(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(invViolentWindsType.find(prevSpellId)->second[1]) : ObjectGuid::Empty))
                myWind->SetGoState(GO_STATE_ACTIVE);

            // Announce
            caster->AI()->Talk(invViolentWindsType.find(prevSpellId)->second[2]);
        }
    }

    private:
        uint32 prevSpellId;

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_violent_gale_winds::HandleScriptEff, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Violent Gale Winds 136867, 136876, 136877, 136879
class spell_violent_gale_winds_at_aura : public AuraScript
{
    PrepareAuraScript(spell_violent_gale_winds_at_aura);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            uint32 sSpellId = owner->AI()->GetData(TYPE_VIOLENT_GALE_WINDS);

            // Close Windows and Visual Floor by spellId
            if (GameObject* myWind = ObjectAccessor::GetGameObject(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(invViolentWindsType.find(sSpellId)->second[0]) : ObjectGuid::Empty))
                myWind->SetGoState(GO_STATE_READY);

            if (GameObject* myWindow = ObjectAccessor::GetGameObject(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(invViolentWindsType.find(sSpellId)->second[1]) : ObjectGuid::Empty))
                myWindow->SetGoState(GO_STATE_READY);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_violent_gale_winds_at_aura::HandleAuraEffectRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Discharged Energy Aura 134820
class spell_discharged_energy_aura : public AuraScript
{
    PrepareAuraScript(spell_discharged_energy_aura);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            // Not sure why it not work in sat...
            if (InstanceScript* instance = owner->GetInstanceScript())
            {
                for (auto&& itr : instance->instance->GetPlayers())
                    if (Player* player = itr.GetSource())
                        player->RemoveAurasDueToSpell(SPELL_DISCHARGED_ENERGY);
            }
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_discharged_energy_aura::HandleAuraEffectRemove, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Ball Lightning Jump 136548
class spell_ball_lightning_jump : public SpellScript
{
    PrepareSpellScript(spell_ball_lightning_jump);

    void HandleDamage(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_BALL_LIGHTNIG_JUMP_EFF, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_ball_lightning_jump::HandleDamage, EFFECT_0, SPELL_EFFECT_JUMP);
    }
};

// Helm of Command 139011
class spell_helm_of_command : public SpellScript
{
    PrepareSpellScript(spell_helm_of_command);

    std::list<WorldObject*> m_targets, m_copy;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        if (Unit* caster = GetCaster())
        {
            uint8 targetCount = caster->GetMap()->Is25ManRaid() ? 2 : 1;
            targets.remove_if(TankSpecTargetSelector());

            if (!targets.empty())
            {
                if (targets.size() > targetCount)
                    Trinity::Containers::RandomResizeList(targets, targetCount);

                m_copy = targets;
                return;
            }

            if (!m_targets.empty())
            {
                targets.clear();

                for (auto&& itr : m_targets)
                    targets.push_back(itr);

                if (targets.size() > targetCount)
                    Trinity::Containers::RandomResizeList(targets, targetCount);

                m_copy = targets;
            }
        }
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_copy;
    }

    uint32 getTargetQuadrant(Unit* target)
    {
        for (auto&& itr : quadrants)
            if (target->HasAura(itr))
                return itr;

        return 0;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_helm_of_command::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_helm_of_command::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Helm of Command 139011
class spell_helm_of_command_aura : public AuraScript
{
    PrepareAuraScript(spell_helm_of_command_aura);

    Position temPos; // need save last sent movement force for remove

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Player* target = GetOwner()->ToPlayer())
        {
            if (Creature* leiShen = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(DATA_LEI_SHEN) : ObjectGuid::Empty))
            {
                GetPositionWithDistInOrientation(target, 150.0f, leiShen->GetAngle(target), x, y);
                temPos = { x, y, target->GetPositionZ(), 0.0f };
                target->SendApplyMovementForce(true, temPos, 12.0f);
            }
        }
    }

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Player* target = GetOwner()->ToPlayer())
            target->SendApplyMovementForce(false, temPos);
    }

    private:
        float x, y;

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_helm_of_command_aura::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_helm_of_command_aura::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// Overcharge 137252
class spell_leishen_overcharge : public SpellScript
{
    PrepareSpellScript(spell_leishen_overcharge);

    void SelectDest(SpellDestination& dest)
    {
        dest._position.RelocateOffset(0.0f, GetSpellInfo()->Effects[EFFECT_0].MaxRadiusEntry->RadiusMax);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_leishen_overcharge::SelectDest, EFFECT_0, TARGET_DEST_CASTER);
    }
};

// Overcharge Eff 136326
class spell_leishen_overcharge_eff : public SpellScript
{
    PrepareSpellScript(spell_leishen_overcharge_eff);

    void HandleDamage(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            if (Creature* conduit = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(NPC_OVERCHARGE_CONDUIT) : ObjectGuid::Empty))
            {
                int32 damage = GetHitDamage();
                uint32 levelPower = conduit->AI()->GetData(TYPE_CONDUIT_LEVEL_POWER);
                SetHitDamage(damage * conduit->AI()->GetData(TYPE_CONDUIT_LEVEL) + CalculatePct(damage, levelPower));
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_leishen_overcharge_eff::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 613. Summoned by 135150 - Crashing Thunder
class sat_crashing_thunder : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, SPELL_CRASHING_THUNDER_AT_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_CRASHING_THUNDER_AT_EFF);
    }
};

// 710. Summoned by 137499 - Lightning Whip
class sat_lightning_whip : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && !triggering->ToPlayer()->HasUnitMovementFlag(MOVEMENTFLAG_FALLING);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, SPELL_LIGHTNING_WHIP_AT_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_LIGHTNING_WHIP_AT_EFF);
    }
};

// 620. Summoned by 134820 - Discharged Energy
class sat_discharged_energy : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, SPELL_DISCHARGED_ENERGY, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_DISCHARGED_ENERGY);
    }
};

// 676. Summoned by 136322 - Overcharged
class sat_overcharged : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* object) override
    {
        if (!object->ToPlayer() || !object->ToPlayer()->IsAlive())
            return false;

        if (std::abs(object->GetPositionZ() - GetTarget()->GetPositionZ()) > 10.0f)
            return false;

        float coeff = (35.0f * 1000.0f) / GetAreaTrigger()->GetMaxDuration();
        float dist = (GetAreaTrigger()->GetMaxDuration() - GetAreaTrigger()->GetDuration()) * coeff / 1000.0f;

        if (GetTarget()->GetExactDist2d(object) - object->GetObjectSize() > dist)
            return false;

        if (!GetCaster()->CanSeeOrDetect(object))
            return false;

        if (GetCaster() && GetCaster()->GetGUID() == object->ToPlayer()->GetGUID() || dist < 6.1f) // also not hit self & targets around 6y
            return false;

        float hitDest = dist - 2.5f;
        return GetTarget()->GetExactDist2d(object) - object->GetObjectSize() > hitDest && hitDest > 6.0f;
    }

    void OnTriggeringApply(WorldObject* object) override
    {
        if (Player* target = object->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(target, SPELL_OVERCHARGED_HIT_EFF, true);
    }
};

// 727. Summoned by 137153, 137169, 137170, 137171 - Overloaded Circuits
class sat_overloaded_circuits : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, SPELL_OVERLOADED_CIRCUITS, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_OVERLOADED_CIRCUITS);
    }
};

// 712. Summoned by 136867, 136876, 136877, 136879 - Violent Gale Winds
class sat_violent_gale_winds : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, SPELL_VIOLENT_GALE_WINDS_AT_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_VIOLENT_GALE_WINDS_AT_EFF);
    }

};

// AreaTrigger 8910, 8911, 8912, 8913
class AreaTrigger_in_quadrant_lei_shen : public AreaTriggerScript
{
    public:
        AreaTrigger_in_quadrant_lei_shen() : AreaTriggerScript("AreaTrigger_in_quadrant_lei_shen") { }
    
        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (!player->HasAura(invQuadrantSideType.find(trigger->ID)->second))
            {
                // we haven`t hook on default areatrigger remove - just remove whole spells if not find currently
                for (auto&& itr : invQuadrantSideType)
                    player->RemoveAurasDueToSpell(itr.second);

                player->CastSpell(player, invQuadrantSideType.find(trigger->ID)->second, true);
            }
    
            return false;
        }
};

// AreaTrigger 8888
// Players should die from AT, not drop damage, cuz for example priest could use levitation and don`t die
class AreaTrigger_down_lei_shen : public AreaTriggerScript
{
    public:
        AreaTrigger_down_lei_shen() : AreaTriggerScript("AreaTrigger_down_lei_shen") { }
    
        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            player->Kill(player);
            return false;
        }
};

void AddSC_boss_lei_shen()
{
    new boss_lei_shen();
    new creature_script<npc_thunderstruck>("npc_thunderstruck");
    new creature_script<npc_electric_conduit>("npc_electric_conduit");
    new creature_script<npc_ball_lightning>("npc_ball_lightning");
    new creature_script<npc_unharnessed_power>("npc_unharnessed_power");
    new creature_script<npc_diffused_lightning>("npc_diffused_lightning");
    new creature_script<npc_quadrant_stalker>("npc_quadrant_stalker");

    new spell_script<spell_cosmetic_teleport_side>("spell_cosmetic_teleport_side");
    new aura_script<spell_cosmetic_visual_overcharge>("spell_cosmetic_visual_overcharge");
    new aura_script<spell_decapitate>("spell_decapitate");
    new aura_script<spell_decapitate_mark>("spell_decapitate_mark");
    new spell_script<spell_decapitate_eff>("spell_decapitate_eff");
    new spell_script<spell_thunderstruck_selector>("spell_thunderstruck_selector");
    new spell_script<spell_thunderstruck_eff>("spell_thunderstruck_eff");
    new aura_script<spell_crashing_thunder_eff_proc>("spell_crashing_thunder_eff_proc");
    new spell_script<spell_crashing_thunder_selector>("spell_crashing_thunder_selector");
    new spell_script<spell_lightning_whip_selector>("spell_lightning_whip_selector");
    new spell_script<spell_ball_lightning_selector>("spell_ball_lightning_selector");
    new spell_script<spell_displace>("spell_displace");
    new aura_script<spell_supercharge_conduits>("spell_supercharge_conduits");
    new aura_script<spell_electrical_conduit_charge>("spell_electrical_conduit_charge");
    new aura_script<spell_conduit_grant_ability>("spell_conduit_grant_ability");
    new spell_script<spell_static_shock>("spell_static_shock");
    new spell_script<spell_static_shock_eff>("spell_static_shock_eff");
    new spell_script<spell_overcharged>("spell_overcharged");
    new aura_script<spell_overcharged_aura>("spell_overcharged_aura");
    new spell_script<spell_diffusion_chain_selector>("spell_diffusion_chain_selector");
    new spell_script<spell_diffusion_chain_eff>("spell_diffusion_chain_eff");
    new spell_script<spell_bouncing_bolt_selector>("spell_bouncing_bolt_selector");
    new spell_script<spell_conduit_level_override>("spell_conduit_level_override");
    new aura_script<spell_overwhelming_power>("spell_overwhelming_power");
    new spell_script<spell_bouncing_bolt_eff>("spell_bouncing_bolt_eff");
    new aura_script<spell_violent_gale_winds_eff>("spell_violent_gale_winds_eff");
    new spell_script<spell_violent_gale_winds>("spell_violent_gale_winds");
    new aura_script<spell_violent_gale_winds_at_aura>("spell_violent_gale_winds_at_aura");
    new aura_script<spell_discharged_energy_aura>("spell_discharged_energy_aura");
    new spell_script<spell_ball_lightning_jump>("spell_ball_lightning_jump");
    new spell_script<spell_helm_of_command>("spell_helm_of_command");
    new aura_script<spell_helm_of_command_aura>("spell_helm_of_command_aura");
    new spell_script<spell_leishen_overcharge>("spell_leishen_overcharge");
    new spell_script<spell_leishen_overcharge_eff>("spell_leishen_overcharge_eff");
    new atrigger_script<sat_crashing_thunder>("sat_crashing_thunder");
    new atrigger_script<sat_lightning_whip>("sat_lightning_whip");
    new atrigger_script<sat_discharged_energy>("sat_discharged_energy");
    new atrigger_script<sat_overcharged>("sat_overcharged");
    new atrigger_script<sat_overloaded_circuits>("sat_overloaded_circuits");
    new atrigger_script<sat_violent_gale_winds>("sat_violent_gale_winds");
    new AreaTrigger_in_quadrant_lei_shen();
    new AreaTrigger_down_lei_shen();
}
