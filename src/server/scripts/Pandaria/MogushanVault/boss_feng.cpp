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

#include "mogu_shan_vault.h"
#include "GameObjectAI.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "Vehicle.h"

enum Spells
{
    // Shared
    SPELL_SPIRIT_BOLT                   = 118530,
    SPELL_STRENGHT_OF_SPIRIT            = 116363,
    SPELL_DRAW_ESSENCE                  = 121631,
    SPELL_BERSERK_FENG                  = 26662,

    // Visuals
    SPELL_SPIRIT_FIST                   = 115743,
    SPELL_SPIRIT_SPEAR                  = 115740,
    SPELL_SPIRIT_STAFF                  = 115742,
    SPELL_SPIRIT_SHIELD                 = 115741,

    // Spirit of the Fist
    SPELL_LIGHTNING_LASH                = 131788,
    SPELL_LIGHTNING_FISTS               = 116157,
    SPELL_EPICENTER                     = 116018,
    SPELL_EPICENTER_EFFECT              = 116040,

    // Spirit of the Spear
    SPELL_FLAMING_SPEAR                 = 116942,
    SPELL_WILDFIRE_SPARK                = 116784,
    SPELL_DRAW_FLAME                    = 116711,
    SPELL_WILDFIRE_INFUSION_STACK       = 116821,
    SPELL_WILDFIRE_INFUSION             = 116817,
    SPELL_WILDFIRE_INFUSION_AURA        = 116927,
    SPELL_ARCHIMONDES_FIRE              = 116787,

    // Spirit of the Staff
    SPELL_ARCANE_SHOCK                  = 131790,
    SPELL_ARCANE_VELOCITY               = 116364,
    SPELL_ARCANE_VELOCITY_DAMAGE        = 116365,
    SPELL_ARCANE_RESONANCE              = 116417,

    // Spirit of the Shield ( Heroic )
    SPELL_SIPHONING_SHIELD              = 118071,
    SPELL_CHAINS_OF_SHADOW              = 118783,
    SPELL_SHADOWBURN                    = 131792, // stuck this dot on current tank

    // Controler Visual
    SPELL_VISUAL_FIST                   = 105032,
    SPELL_VISUAL_SPEAR                  = 118485,
    SPELL_VISUAL_STAFF                  = 118486,
    SPELL_VISUAL_SHIELD                 = 117303,

    // Siphon Shield
    SPELL_SUMMON_SHIELD                 = 117203,
    SPELL_TOGGLE_SHIELD                 = 121296,
    SPELL_SHIELD_DISPLAYED              = 117763,
    SPELL_SHIELD_VISUAL                 = 117240,
    SPELL_SOUL_FRAGMENT                 = 45537,
    SPELL_SUMMON_SOUL_FRAGMENT          = 117717,
    SPELL_BACK_TO_FENG                  = 117781,
    SPELL_LINKED_SHIELD                 = 45537,

    // Soul Fragment
    SPELL_SOUL_DISPLAY                  = 32395,
    SPELL_ICE_TRAP                      = 135382,

    // Nullification Barrier
    SPELL_NULLIFICATION_BARRIER         = 115817,
    SPELL_NULLIFICATION_BARRIER_BUTTON  = 115811,
    SPELL_NULLIFICATION_BARRIER_EFFECT  = 115856,

    // Shroud of Reversal
    SPELL_SHROUD_OF_REVERSAL            = 115911,
    SPELL_SHROUD_OF_REVERASL_BUTTON     = 115972,

    SPELL_EPICENTER_BUTTON              = 118300,
    SPELL_LIGHTNING_FISTS_BUTTON        = 118302,
    SPELL_ARCANE_RESONANCE_BUTTON       = 118304,
    SPELL_ARCANE_VELOCITY_BUTTON        = 118305,
    SPELL_WILDFIRE_SPARK_BUTTON         = 118307,
    SPELL_FLAMING_SPEAR_BUTTON          = 118308,
    // Inversion bouclier siphon        = 118471,
    SPELL_SHADOWBURN_BUTTON             = 132296,
    SPELL_LIGHTNING_LASH_BUTTON         = 132297,
    SPELL_ARCANE_SHOCK_BUTTON           = 132298,

    SPELL_REVERSAL_EPICENTER            = 116936,
    SPELL_REVERSAL_LIGHTNING_FIRSTS     = 115730,
    SPELL_REVERSAL_ARCANE_RESONANCE     = 118194,
    SPELL_REVERSAL_ARCANE_VIOLCITY      = 116938,
    SPELL_REVERSAL_WILDFIRE_SPARK       = 116922,
    SPELL_REVERSAL_FLAMING_SPEAR        = 118197,
    SPELL_REVERSAL_SHADOWBURN           = 132292,
    SPELL_REVERSAL_LIGHTNING_LASH       = 132293,
    SPELL_REVERSAL_ARCANE_SHOCK         = 132294,
};

enum LightningFistSpell
{
    SPELL_FIST_BARRIER = 115856,
    SPELL_FIST_CHARGE  = 116374,
    SPELL_FIST_VISUAL  = 116225
};

enum Events
{
    EVENT_DOT_ATTACK            = 1,
    EVENT_RE_ATTACK             = 2,

    EVENT_LIGHTNING_FISTS       = 3,
    EVENT_EPICENTER             = 4,

    EVENT_WILDFIRE_SPARK        = 5,
    EVENT_DRAW_FLAME            = 6,

    EVENT_ARCANE_VELOCITY       = 7,
    EVENT_ARCANE_VELOCITY_END   = 8,
    EVENT_ARCANE_RESONANCE      = 9,
    EVENT_SPIRIT_BOLTS          = 10,

    EVENT_SIPHONING_SHIELD      = 11,
    EVENT_CHAINS_OF_SHADOW      = 12,

    EVENT_SHIELD_CASTSOULS      = 13,
    EVENT_SHIELD_CHECKSOULS     = 14,
    EVENT_SHIELD_BACK           = 15,
    EVENT_SHIELD_DESTROY        = 16,
    EVENT_SOUL_WALK             = 17,
    EVENT_SOUL_GROW             = 18,
    EVENT_BERSERK               = 19,
    EVENT_SHIELD_REFUND_HANDLE  = 20,
};

enum FengPhases
{
    PHASE_NONE                  = 0,
    PHASE_FIST                  = 1,
    PHASE_SPEAR                 = 2,
    PHASE_STAFF                 = 3,
    PHASE_SHIELD                = 4
};

enum Talk
{
    TALK_AGGRO          = 0,
    TALK_PHASE_1        = 1,
    TALK_PHASE_2        = 2,
    TALK_PHASE_3        = 3,
    TALK_PHASE_4        = 4,
    TALK_DEATH          = 5,
    TALK_SLAY           = 6,
    TALK_WILDFIRE_SPARK = 7,
    TALK_EPICENTER      = 8,
    TALK_DRAW_FLAME     = 9,
};

enum EquipId
{
    EQUIP_ID_FISTS      = 82769,
    EQUIP_ID_SPEAR      = 82770,
    EQUIP_ID_STAFF      = 82771,
    EQUIP_ID_HAXE       = 61887, // Not exactly the right weapon... - not found in [82760, 82780]
    EQUIP_ID_SHIELD     = 82767, // Shield
};

enum ShieldPhases
{
    PHASE_INACTIVE      = 0, // Feng isn't using Siphoning Shield
    PHASE_THROWN        = 1, // Feng has just thrown the shield, but it's not on the floor yet
    PHASE_LANDED        = 2, // Shield on the ground, casting
    PHASE_INTERRUPTED   = 3, // Shield has been recalled by Feng, stop casting on the ground
    PHASE_BACK          = 4, // Shield is flying back to Feng
};

enum SoulActions
{
    ACTION_SOUL_HOME    = 20,
    ACTION_SOUL_KILLED  = 21,
};

#define SHIELD_ON_FENG 5
#define DISPLAYID_SHIELD 11686

Position modPhasePositions[4] =
{
    {4063.26f, 1320.50f, 466.30f, 5.5014f}, // Phase Fist
    {4021.17f, 1320.50f, 466.30f, 3.9306f}, // Phase Spear
    {4021.17f, 1362.80f, 466.30f, 2.0378f}, // Phase Staff
    {4063.26f, 1362.80f, 466.30f, 0.7772f}, // Phase Shield
};

uint32 statueEntryInOrder[4] = {GO_FIST_STATUE,   GO_SPEAR_STATUE,   GO_STAFF_STATUE,   GO_SHIELD_STATUE};
uint32 controlerVisualId[4]  = {SPELL_VISUAL_FIST, SPELL_VISUAL_SPEAR, SPELL_VISUAL_STAFF, SPELL_VISUAL_SHIELD};
uint32 fengVisualId[4]       = {SPELL_SPIRIT_FIST, SPELL_SPIRIT_SPEAR, SPELL_SPIRIT_STAFF, SPELL_SPIRIT_SHIELD};

#define MAX_DIST    60
Position centerPos = {4041.979980f, 1341.859985f, 466.388000f, 3.140160f};

// Positions for Siphoning Shield
Position shieldPositions[3] =
{
    {4067.59f, 1326.71f, 466.30f, 2.579275f},
    {4020.77f, 1363.22f, 466.30f, 5.500957f},
    {4036.06f, 1362.29f, 466.30f, 4.253744f},
};

const std::vector<std::pair<uint32, uint32>> shroudOfReversalMap =
{
    { SPELL_EPICENTER_EFFECT, SPELL_EPICENTER_BUTTON        },
    { SPELL_FIST_CHARGE,      SPELL_LIGHTNING_FISTS_BUTTON  },
    { SPELL_ARCANE_RESONANCE, SPELL_ARCANE_RESONANCE_BUTTON },
    { SPELL_WILDFIRE_SPARK,   SPELL_WILDFIRE_SPARK_BUTTON   },
    { SPELL_FLAMING_SPEAR,    SPELL_FLAMING_SPEAR_BUTTON    },
    { SPELL_SHADOWBURN,       SPELL_SHADOWBURN_BUTTON       },
    { SPELL_LIGHTNING_LASH,   SPELL_LIGHTNING_LASH_BUTTON   },
    { SPELL_ARCANE_SHOCK,     SPELL_ARCANE_SHOCK_BUTTON     },
};

#define SHIELD_POSITIONS_COUNT 3;

enum AchievementWorldStates
{
    WORLD_STATE_LIGHTNING_FISTS_MIMICKED            = 11998,
    WORLD_STATE_EPICENTER_MIMICKED                  = 11997,
    WORLD_STATE_WILDFIRE_SPARK_MIMICKED             = 11999,
    WORLD_STATE_ARCANE_VIOLCITY_MIMICKED            = 12001,
    WORLD_STATE_ARCANE_RESONANCE_MIMICKED           = 12002,
    WORLD_STATE_ANYTHING_YOU_CAN_DO_I_CAN_DO_BETTER = 12630,
};

static std::map<uint32, uint32> const achievementWorldStates
{
    { SPELL_REVERSAL_LIGHTNING_FIRSTS, WORLD_STATE_LIGHTNING_FISTS_MIMICKED     },
    { SPELL_REVERSAL_EPICENTER,        WORLD_STATE_EPICENTER_MIMICKED           },
    { SPELL_REVERSAL_WILDFIRE_SPARK,   WORLD_STATE_WILDFIRE_SPARK_MIMICKED      },
    { SPELL_REVERSAL_ARCANE_VIOLCITY,  WORLD_STATE_ARCANE_VIOLCITY_MIMICKED     },
    { SPELL_REVERSAL_ARCANE_RESONANCE, WORLD_STATE_ARCANE_RESONANCE_MIMICKED    },
};

// Feng The Accursed - 60009
class boss_feng : public CreatureScript
{
    public:
        boss_feng() : CreatureScript("boss_feng") { }

        struct boss_fengAI : public BossAI
        {
            boss_fengAI(Creature* creature) : BossAI(creature, DATA_FENG) { }

            EventMap berserkEvents;
            bool isWaitingForPhase;
            uint8 actualPhase;
            uint32 nextPhasePct;
            uint32 dotSpellId;
            uint32 delay;
            std::list<uint32> phaseList;
            std::list<uint64> sparkList;
            uint8 availablePos[4];

            // Available positions -- Heroic mode purpose
            // uint8 availablePos[4];

            void InitializeAI() override
            {
                me->setActive(true);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->AddAura(SPELL_BRONZE, me);
                me->AddAura(SPELL_STONE, me);

                if (instance->GetData(DATA_FENG) >= DONE)
                    me->AI()->DoAction(ACTION_CURSED_MOGU_ATTACK_PLAYER);

                Reset();
            }

            void Reset() override
            {
                _Reset();

                me->SetReactState(REACT_DEFENSIVE);

                for (auto&& visualSpellId: fengVisualId)
                    me->RemoveAurasDueToSpell(visualSpellId);

                SetEquipmentSlots(false, 0, 0, EQUIP_NO_CHANGE);

                if (instance->GetBossState(DATA_FENG) != DONE)
                    instance->SetBossState(DATA_FENG, NOT_STARTED);

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FLAMING_SPEAR);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_NULLIFICATION_BARRIER_BUTTON);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHROUD_OF_REVERASL_BUTTON);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WILDFIRE_INFUSION_AURA);

                isWaitingForPhase = false;
                actualPhase  = PHASE_FIST;
                nextPhasePct = IsHeroic() ? 98 : 95;
                dotSpellId = 0;
                delay = 0;

                for (uint8 i = 1; i < 5; i++)
                    availablePos[i - 1] = i;

                // Desactivate old statue
                if (GameObject* oldStatue = instance->instance->GetGameObject(instance->GetGuidData(statueEntryInOrder[actualPhase - 1])))
                {
                    oldStatue->SetLootState(GO_READY);
                    oldStatue->UseDoorOrButton();
                }

                actualPhase = PHASE_NONE;

                me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_FIST_CHARGE, false); // should be stunable with this spell
                berserkEvents.Reset();

                for (auto&& itr : achievementWorldStates)
                    me->GetMap()->SetWorldState(itr.second, 0);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(TALK_DEATH);
                _JustDied();

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_NULLIFICATION_BARRIER_BUTTON);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHROUD_OF_REVERASL_BUTTON);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WILDFIRE_INFUSION_AURA);
                instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                if (GameObject* inversionGob = instance->instance->GetGameObject(instance->GetGuidData(GO_INVERSION)))
                    inversionGob->Delete();

                if (GameObject* cancelGob = instance->instance->GetGameObject(instance->GetGuidData(GO_CANCEL)))
                    cancelGob->Delete();

                if (Creature* Cho = GetClosestCreatureWithEntry(me, NPC_LOREWALKER_CHO, 200.0f, true))
                    Cho->DespawnOrUnsummon();

                me->SummonCreature(NPC_LOREWALKER_CHO, ChoPastFengSpawn, TEMPSUMMON_MANUAL_DESPAWN);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!instance->CheckRequiredBosses(DATA_FENG, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    who->NearTeleportTo(EncountersEntrance[0].GetPositionX(), EncountersEntrance[0].GetPositionY(), EncountersEntrance[0].GetPositionZ(), EncountersEntrance[0].GetOrientation());
                    return;
                }

                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                delay = 0;
                me->m_Events.Schedule(delay += 3000, 20, [this]()
                {
                    if (me->IsInCombat())
                        _JustEngagedWith();
                });
                Talk(TALK_AGGRO);
                berserkEvents.ScheduleEvent(EVENT_BERSERK, IsHeroic() ? 10 * MINUTE * IN_MILLISECONDS : 9 * MINUTE * IN_MILLISECONDS);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_FENG, FAIL);
                    summons.DespawnAll();

                    if (GameObject* inversionGob = instance->instance->GetGameObject(instance->GetGuidData(GO_INVERSION)))
                        inversionGob->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);

                    if (GameObject* cancelGob = instance->instance->GetGameObject(instance->GetGuidData(GO_CANCEL)))
                        cancelGob->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);

                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WILDFIRE_INFUSION_AURA);
                }

                _DespawnAtEvade();
                me->GetMotionMaster()->MoveTargetedHome();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId >= 1 && pointId <= 4)
                    PrepareNewPhase(pointId);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_CURSED_MOGU_ATTACK_PLAYER:
                        me->SetReactState(REACT_DEFENSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->RemoveAurasDueToSpell(SPELL_BRONZE);
                        me->RemoveAurasDueToSpell(SPELL_STONE);
                        break;
                    case ACTION_SCHEDULE_SHIELD:
                        events.ScheduleEvent(EVENT_SIPHONING_SHIELD, 30000);
                        break;
                    case ACTION_SPARK:
                        me->AddAura(SPELL_WILDFIRE_INFUSION_STACK, me);
                        break;
                }
            }

            void PrepareNewPhase(uint8 newPhase)
            {
                events.Reset();
                events.ScheduleEvent(EVENT_SPIRIT_BOLTS, 15 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_DOT_ATTACK, 15000);
                events.ScheduleEvent(EVENT_RE_ATTACK,  500);

                me->SetReactState(REACT_PASSIVE);
                me->GetMotionMaster()->Clear();

                if (IsHeroic())
                    me->CastSpell(me, SPELL_STRENGHT_OF_SPIRIT, false);

                if (Creature* controler = GetClosestCreatureWithEntry(me, NPC_PHASE_CONTROLLER, 20.0f))
                    controler->DespawnOrUnsummon();

                if (actualPhase != PHASE_NONE)
                {
                    // Desactivate old statue and enable the new one
                    if (GameObject* oldStatue = instance->instance->GetGameObject(instance->GetGuidData(statueEntryInOrder[actualPhase - 1])))
                    {
                        oldStatue->SetLootState(GO_READY);
                        oldStatue->UseDoorOrButton();
                    }
                }

                if (GameObject* newStatue = instance->instance->GetGameObject(instance->GetGuidData(statueEntryInOrder[newPhase - 1])))
                {
                    newStatue->SetLootState(GO_READY);
                    newStatue->UseDoorOrButton();
                }

                for (auto&& visualSpellId: fengVisualId)
                    me->RemoveAurasDueToSpell(visualSpellId);

                me->AddAura(fengVisualId[newPhase - 1], me);
                me->CastSpell(me, SPELL_DRAW_ESSENCE, true);

                switch (newPhase)
                {
                    case PHASE_FIST:
                    {
                        dotSpellId = SPELL_LIGHTNING_LASH;
                        events.ScheduleEvent(EVENT_LIGHTNING_FISTS,  15000, PHASE_FIST);
                        events.ScheduleEvent(EVENT_EPICENTER,        35000, PHASE_FIST);
                        SetEquipmentSlots(false, EQUIP_ID_FISTS, EQUIP_ID_FISTS, EQUIP_NO_CHANGE);
                        Talk(TALK_PHASE_1);
                        break;
                    }
                    case PHASE_SPEAR:
                    {
                        dotSpellId = SPELL_FLAMING_SPEAR;
                        events.ScheduleEvent(EVENT_WILDFIRE_SPARK,   10000, PHASE_SPEAR);
                        events.ScheduleEvent(EVENT_DRAW_FLAME,       36000, PHASE_SPEAR);
                        SetEquipmentSlots(false, EQUIP_ID_SPEAR, 0, EQUIP_NO_CHANGE);
                        Talk(TALK_PHASE_2);
                        break;
                    }
                    case PHASE_STAFF:
                    {
                        summons.DespawnAll();

                        me->RemoveAura(SPELL_WILDFIRE_INFUSION);
                        dotSpellId = SPELL_ARCANE_SHOCK;
                        events.ScheduleEvent(EVENT_ARCANE_VELOCITY, urand(17000, 18000), PHASE_STAFF);
                        events.ScheduleEvent(EVENT_ARCANE_RESONANCE, urand(14000, 15000), PHASE_STAFF);
                        SetEquipmentSlots(false, EQUIP_ID_STAFF, 0, EQUIP_NO_CHANGE);
                        Talk(TALK_PHASE_3);
                        break;
                    }
                    case PHASE_SHIELD:
                    {
                        dotSpellId = SPELL_SHADOWBURN;
                        SetEquipmentSlots(false, EQUIP_ID_HAXE, EQUIP_ID_SHIELD, EQUIP_NO_CHANGE);
                        events.ScheduleEvent(EVENT_SIPHONING_SHIELD, 5000, PHASE_SHIELD);
                        events.ScheduleEvent(EVENT_CHAINS_OF_SHADOW, 45000, PHASE_SHIELD);
                        Talk(TALK_PHASE_4);
                        break;
                    }
                    default:
                        break;
                }

                isWaitingForPhase = false;
                actualPhase = newPhase;

                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                sparkList.push_back(summon->GetGUID());
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);

                sparkList.remove(summon->GetGUID());
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(TALK_SLAY);
            }

            void OnAddThreat(Unit* /*victim*/, float& fThreat, SpellSchoolMask /*schoolMask*/, SpellInfo const* /*threatSpell*/)
            {
                if (isWaitingForPhase)
                    fThreat = 0;
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (!instance)
                    return;

                if (attacker->GetEntry() == NPC_WILDFIRE_SPARK)
                {
                    damage = 0;
                    return;
                }

                if (nextPhasePct)
                {
                    if (me->HealthBelowPctDamaged(nextPhasePct, damage))
                    {
                        events.Reset();

                        uint8 newPhase = 9;
                        isWaitingForPhase = true;

                        me->InterruptNonMeleeSpells(true);

                        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

                        // In normal mode, Feng reaches the statue in predetermined order
                        if (!IsHeroic())
                        {
                            newPhase = actualPhase + 1;
                            me->GetMotionMaster()->MovePoint(newPhase, modPhasePositions[newPhase - 1].GetPositionX(), modPhasePositions[newPhase - 1].GetPositionY(), modPhasePositions[newPhase - 1].GetPositionZ());
                        }
                        // In Heroic mode, Feng reaches the nearest statue he hasn't siphonned yet
                        else
                        {
                            float dist = 300.0f;
                            uint32 statue = 0;

                            // Looking for the closest available statue
                            for (uint8 i = 0; i < 4; i++)
                            {
                                if (availablePos[i])
                                {
                                    if (me->GetDistance(modPhasePositions[i]) < dist)
                                    {
                                        dist = me->GetDistance(modPhasePositions[i]);
                                        statue = i;
                                    }
                                }
                            }
                            // Statue selected
                            newPhase = statue + 1;
                            availablePos[statue] = 0;
                            me->GetMotionMaster()->MovePoint(newPhase, modPhasePositions[statue]);
                        }

                        if (Creature* controler = me->SummonCreature(NPC_PHASE_CONTROLLER, modPhasePositions[newPhase - 1].GetPositionX(), modPhasePositions[newPhase - 1].GetPositionY(), modPhasePositions[newPhase - 1].GetPositionZ()))
                            controler->AddAura(controlerVisualId[newPhase - 1], controler);

                        // Nefertum: Lo hago de este modo porque los timers entre normal/lfr y heroico varian bastante... No me rinde hacerlo con un ternario
                        uint32 reduction = 0;
                        if (IsHeroic() && nextPhasePct == 98)
                            reduction = 23;
                        else if (!IsHeroic() && nextPhasePct == 95)
                            reduction = 29;
                        else if (IsHeroic())
                            reduction = 25;
                        else
                            reduction = 33;
                        nextPhasePct >= reduction ? nextPhasePct-= reduction: nextPhasePct = 0;
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                CheckPlatform();

                if (isWaitingForPhase)
                    return;

                if (!UpdateVictim())
                    return;

                berserkEvents.Update(diff);

                while (uint32 eventId = berserkEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_BERSERK)
                    {
                        me->InterruptNonMeleeSpells(true);
                        DoCast(me, SPELL_BERSERK_FENG, true);
                    }
                    break;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                            // All Phases
                        case EVENT_DOT_ATTACK:
                        {
                            if (dotSpellId == SPELL_SHADOWBURN)
                            {
                                if (Unit* target = me->GetVictim())
                                    me->CastSpell(target, dotSpellId, false);
                            }
                            else if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, dotSpellId, false);
                            events.ScheduleEvent(EVENT_DOT_ATTACK, 12500);
                            break;
                        }
                        case EVENT_RE_ATTACK:
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->GetMotionMaster()->MoveChase(target);

                            me->SetReactState(REACT_AGGRESSIVE);
                            break;
                        }
                        // Fist Phase
                        case EVENT_LIGHTNING_FISTS:
                        {
                            me->CastSpell(me, SPELL_LIGHTNING_FISTS, false);
                            events.ScheduleEvent(EVENT_LIGHTNING_FISTS, 15000);
                            break;
                        }
                        case EVENT_EPICENTER:
                        {
                            Talk(TALK_EPICENTER);
                            me->CastSpell(me, SPELL_EPICENTER, false);
                            events.ScheduleEvent(EVENT_EPICENTER, 35000);
                            break;
                        }
                        // Spear Phase
                        case EVENT_WILDFIRE_SPARK:
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                            {
                                Talk(TALK_WILDFIRE_SPARK, target);
                                me->CastSpell(target, SPELL_WILDFIRE_SPARK, false);
                            }
                            events.ScheduleEvent(EVENT_WILDFIRE_SPARK, 10 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_DRAW_FLAME:
                        {
                            events.CancelEvent(EVENT_WILDFIRE_SPARK);
                            Talk(TALK_DRAW_FLAME);
                            me->CastSpell(me, SPELL_DRAW_FLAME, false);
                            events.ScheduleEvent(EVENT_DRAW_FLAME, 36000);
                            events.ScheduleEvent(EVENT_WILDFIRE_SPARK, 6500);
                            break;
                        }
                        // Staff Phase
                        case EVENT_ARCANE_VELOCITY:
                        {
                            me->CastSpell(me, SPELL_ARCANE_VELOCITY, false);
                            events.ScheduleEvent(EVENT_ARCANE_VELOCITY_END, 100);   // The eventmap don't update while the creature is casting
                            events.ScheduleEvent(EVENT_ARCANE_VELOCITY, 18 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_ARCANE_VELOCITY_END:
                        {
                            me->SetSpeed(MOVE_RUN, 1.14f);
                            break;
                        }
                        case EVENT_ARCANE_RESONANCE:
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                target->AddAura(SPELL_ARCANE_RESONANCE, target);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                target->AddAura(SPELL_ARCANE_RESONANCE, target);

                            events.ScheduleEvent(EVENT_ARCANE_RESONANCE, 15 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_SPIRIT_BOLTS:
                        {
                            me->CastSpell(me, SPELL_SPIRIT_BOLT, true);
                            events.ScheduleEvent(EVENT_SPIRIT_BOLTS, urand(8 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                            break;
                        }
                        // Shield Phase
                        case EVENT_SIPHONING_SHIELD:
                        {
                            me->CastSpell(me, SPELL_SUMMON_SHIELD, true);

                            if (Vehicle* Passengers = me->GetVehicleKit())
                                Passengers->RemoveAllPassengers(/*true*/);
                            break;
                        }
                        case EVENT_CHAINS_OF_SHADOW:
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                me->CastSpell(target, SPELL_CHAINS_OF_SHADOW, true);
                            events.ScheduleEvent(EVENT_CHAINS_OF_SHADOW, urand(30000, 40000));
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 40.0f);
            }

        private:
            void CheckPlatform()
            {
                if (!me->IsInCombat())
                    return;

                if ((me->GetPositionX() - centerPos.GetPositionX()) > 37.0f ||
                    (me->GetPositionX() - centerPos.GetPositionX()) < -37.0f ||
                    (me->GetPositionY() - centerPos.GetPositionY()) > 37.0f ||
                    (me->GetPositionY() - centerPos.GetPositionY()) < -37.0f)
                    me->AI()->EnterEvadeMode();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_fengAI(creature);
        }
};

// Lightning Charge - 60241
class npc_lightning_fist : public CreatureScript
{
    public:
        npc_lightning_fist() : CreatureScript("npc_lightning_fist") { }

        enum iEvents
        {
            EVENT_CHECK_NEAR_PLAYER = 1,
            EVENT_HEROIC_ARC        = 2,
        };

        struct npc_lightning_fistAI : public ScriptedAI
        {
            npc_lightning_fistAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetReactState(REACT_PASSIVE);
                me->AddAura(SPELL_FIST_BARRIER, me);
                me->AddAura(SPELL_FIST_VISUAL, me);

                float x = 0, y = 0;
                GetPositionWithDistInOrientation(me, 100.0f, me->GetOrientation(), x, y);

                me->GetMotionMaster()->MoveCharge(x, y, me->GetPositionZ(), 9.5f, 1);

                events.ScheduleEvent(EVENT_CHECK_NEAR_PLAYER, 500);
                
                if (IsHeroic() && summoner->GetEntry() == NPC_FENG)
                    events.ScheduleEvent(EVENT_HEROIC_ARC, 200);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 1)
                    me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_NEAR_PLAYER:
                        {
                            std::list<Player*> playerList;
                            GetPlayerListInGrid(playerList, me, 5.0f);

                            for (auto&& player : playerList)
                                me->CastSpell(player, SPELL_FIST_CHARGE, true);

                            events.ScheduleEvent(EVENT_CHECK_NEAR_PLAYER, 500);
                            break;
                        }
                        case EVENT_HEROIC_ARC:
                            for (uint8 i = 0; i < 2; i++)
                                me->SummonCreature(NPC_LIGHTNING_CHARGE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), i == 0 ? Position::NormalizeOrientation(me->GetOrientation() - M_PI / 4) : Position::NormalizeOrientation(me->GetOrientation() + M_PI / 4), TEMPSUMMON_MANUAL_DESPAWN);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lightning_fistAI(creature);
        }
};

// Siphoning Shield - 60627
class npc_siphon_shield : public CreatureScript
{
    public:
        npc_siphon_shield() : CreatureScript("npc_siphon_shield") { }

        struct npc_siphon_shieldAI : public ScriptedAI
        {
            npc_siphon_shieldAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;
            std::map<uint32, uint32> soulsMap;

            void Reset() override
            {
                instance = me->GetInstanceScript();
                // Set invisible
                me->SetDisplayId(DISPLAYID_SHIELD);

                if (Creature* feng = instance->instance->GetCreature(instance->GetGuidData(NPC_FENG)))
                    me->SetFacingToObject(feng);

                events.Reset();
                // Display shield
                me->AddAura(SPELL_SHIELD_DISPLAYED, me);
                // Activate Visual
                me->AddAura(SPELL_SHIELD_VISUAL, me);
                events.ScheduleEvent(EVENT_SHIELD_CASTSOULS, 2000);
                //events.ScheduleEvent(EVENT_SHIELD_REFUND_HANDLE, 30 * IN_MILLISECONDS);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_SOUL_HOME)
                    if (Creature* feng = instance->instance->GetCreature(instance->GetGuidData(NPC_FENG)))
                        feng->SetHealth(feng->GetHealth() + (uint32)feng->GetMaxHealth()*(Is25ManRaid() ? 0.05f : 0.1f)); // 10% at 10 mode for each soul and 5% at 25 
            }

            bool HasAnySpiritAlive()
            {
                std::list<Creature*> Spirits;
                GetCreatureListWithEntryInGrid(Spirits, me, NPC_SOUL_FRAGMENT, 100.0f);

                for (auto&& itr : Spirits)
                    if (itr->IsAlive())
                        return true;

                return false;
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHIELD_CASTSOULS:
                        {
                            // Retrieving targets
                            std::list<Player*> potenTargets;
                            GetPlayerListInGrid(potenTargets, me, 150.0f);

                            if (potenTargets.empty())
                            {
                                events.ScheduleEvent(EVENT_SHIELD_CHECKSOULS, 2000);
                                break;
                            }

                            uint8 maxTargets = Is25ManRaid() ? 10 : 5;
                            auto it = potenTargets.begin();
                            while (it != potenTargets.end())
                            {
                                if ((*it)->IsGameMaster())
                                    it = potenTargets.erase(it);
                                else
                                    it++;
                            }

                            // Selecting targets
                            if (potenTargets.size() > maxTargets)
                                Trinity::Containers::RandomResizeList(potenTargets, maxTargets);

                            // Attacking targets -- Souls are summoned by player
                            for (auto&& target : potenTargets)
                                target->CastSpell(target, SPELL_SUMMON_SOUL_FRAGMENT, false);

                            // Check if there are souls remainings
                            events.ScheduleEvent(EVENT_SHIELD_CHECKSOULS, 2000);
                            break;
                        }
                        case EVENT_SHIELD_CHECKSOULS:
                        {
                            if (!HasAnySpiritAlive())
                            {
                                // Remove visual effect
                                me->RemoveAura(SPELL_SHIELD_VISUAL);
                                // Scheduling end of action
                                events.ScheduleEvent(EVENT_SHIELD_BACK, 500);
                            }
                            else
                                events.ScheduleEvent(EVENT_SHIELD_CHECKSOULS, 1000);
                            break;
                        }
                        case EVENT_SHIELD_BACK:
                        {
                            if (Creature* feng = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_FENG)))
                            {
                                // Making Feng moves his arm
                                feng->CastSpell(me, SPELL_BACK_TO_FENG, true);
                                // Making shield returning to feng
                                me->CastSpell(feng, SPELL_BACK_TO_FENG, false);
                                // Visual effect
                                me->CastSpell(feng, SPELL_LINKED_SHIELD, false);
                            }
                            break;
                        }
                        // In Situation when group try endless control spirits
                        /*case EVENT_SHIELD_REFUND_HANDLE:
                        {
                            std::list<Creature*> soulList;
                            GetCreatureListWithEntryInGrid(soulList, me, NPC_SOUL_FRAGMENT, 200.0f);

                            for (auto&& itr : soulList)
                                itr->DespawnOrUnsummon();
                            break;
                        }*/
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_siphon_shieldAI(creature);
        }
};

// Soul Fragment - 60781
class npc_soul_fragment : public CreatureScript
{
    public:
        npc_soul_fragment() : CreatureScript("npc_soul_fragment") { }

        struct npc_soul_fragmentAI : public ScriptedAI
        {
            npc_soul_fragmentAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            EventMap events;
            Player* bound;
            float scale;
            bool CanMove;

            void Reset() override
            {
                // Set dark aspect
                me->AddAura(SPELL_SOUL_DISPLAY, me);
                bound = me->SelectNearestPlayerNotGM(0.0f);
                scale = 0.1f;
                me->SetObjectScale(scale);
                // Display with display of player
                me->SetDisplayId(bound->GetDisplayId());

                if (Creature* shield = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_SIPHONING_SHIELD)))
                    shield->CastSpell(me, SPELL_SOUL_FRAGMENT, false);

                events.ScheduleEvent(EVENT_SOUL_WALK, 2000);
                events.ScheduleEvent(EVENT_SOUL_GROW, 100);
                me->SetFaction(16);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                CanMove = false;
                DoZoneInCombat(me, 200.0f);
                me->SetSpeed(MOVE_RUN, 0.25f);

                uint32 delay = 0;
                me->m_Events.Schedule(delay += 2000, 1, [this]()
                {
                    CanMove = true;
                });
            }

            void JustDied(Unit* /*killer*/) override
            {
                // We send negative guid to tell shield the soul has been killed
                if (Creature* shield = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_SIPHONING_SHIELD)))
                    shield->AI()->DoAction(ACTION_SOUL_KILLED);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventID = events.ExecuteEvent())
                {
                    if (eventID == EVENT_SOUL_GROW)
                    {
                        if (scale < 1.5f)
                        {
                            scale += 0.1f;
                            me->SetObjectScale(scale);
                            events.ScheduleEvent(EVENT_SOUL_GROW, 100);
                        }
                    }
                    break;
                }

                if (!CanMove)
                    return;

                if (Creature* shield = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_SIPHONING_SHIELD)))
                {
                    if (shield->IsWithinDist(me, 1.5f, false))
                    {
                        shield->AI()->DoAction(ACTION_SOUL_HOME);
                        me->DespawnOrUnsummon();
                    }

                    if (shield)
                        me->GetMotionMaster()->MoveFollow(shield, 0.0f, me->GetAngle(shield));
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_soul_fragmentAI(creature);
        }
};

// Wildfire Spark - 60438
class npc_wild_spark : public CreatureScript
{
    public:
        npc_wild_spark() : CreatureScript("npc_wild_spark") { }

        struct npc_wild_sparkAI : public ScriptedAI
        {
            npc_wild_sparkAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                me->SetDisplayFromModel(0);
                Reset();
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->CastSpell(me, 116717, true); // Fire aura
                me->CastSpell(me, SPELL_ARCHIMONDES_FIRE, true); // Fire visual

                bool clockwise = urand(0, 1);
                float x = me->GetPositionX();
                float y = me->GetPositionY();

                Movement::MoveSplineInit init(me);
                for (uint8 i = 1; i < 13; ++i)
                {
                    float newX = x + 1.5f * cos(i * M_PI / 6);
                    float newY = clockwise ? y - 1.5f * sin(i * M_PI / 6) : y + 1.5f * sin(i * M_PI / 6);
                    init.Path().push_back(G3D::Vector3(newX, newY, me->GetPositionZ()));
                }

                init.SetUncompressed();
                init.SetCyclic();
                init.Launch();
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_DRAW_FLAME)
                {
                    me->StopMoving();
                    me->SetWalk(true);
                    me->SetSpeed(MOVE_WALK, 3.0f);
                    me->GetMotionMaster()->MovePoint(1, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ());
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 1)
                {
                    if (InstanceScript* instance = me->GetInstanceScript())
                    {
                        if (Creature* feng = instance->instance->GetCreature(instance->GetGuidData(NPC_FENG)))
                        {
                            feng->AI()->DoAction(ACTION_SPARK);
                            me->DespawnOrUnsummon(500);
                        }
                    }
                }
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (instance)
                    if (instance->GetBossState(DATA_FENG) != IN_PROGRESS)
                        me->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_wild_sparkAI(creature);
        }
};

// Mogu Epicenter - 116040
class spell_mogu_epicenter : public SpellScriptLoader
{
    public:
        spell_mogu_epicenter() : SpellScriptLoader("spell_mogu_epicenter") { }

        class spell_mogu_epicenter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mogu_epicenter_SpellScript);

            void DealDamage()
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                if (!caster || !target)
                    return;

                float distance = caster->GetExactDist2d(target);

                if (distance >= 0 && distance <= 60)
                    SetHitDamage(GetHitDamage() * (1 - (distance / MAX_DIST)));
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mogu_epicenter_SpellScript::DealDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mogu_epicenter_SpellScript();
        }
};

// Wildfire Spark - 116583
class spell_mogu_wildfire_spark : public SpellScriptLoader
{
    public:
        spell_mogu_wildfire_spark() : SpellScriptLoader("spell_mogu_wildfire_spark") { }

        class spell_mogu_wildfire_spark_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mogu_wildfire_spark_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                const uint8 maxSpark = 4;

                Unit* caster = GetCaster();

                if (!caster)
                    return;

                for (uint8 i = 0; i < maxSpark; ++i)
                {
                    float position_x = caster->GetPositionX() + frand(-3.0f, 3.0f);
                    float position_y = caster->GetPositionY() + frand(-3.0f, 3.0f);
                    caster->CastSpell(position_x, position_y, caster->GetPositionZ(), 116586, true);
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_mogu_wildfire_spark_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mogu_wildfire_spark_SpellScript();
        }
};

// Wildfire Infusion (stacks) - 116821
class spell_wildfire_infusion_stacks : public SpellScriptLoader
{
    public:
        spell_wildfire_infusion_stacks() : SpellScriptLoader("spell_wildfire_infusion_stacks") { }

        class spell_wildfire_infusion_stacks_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_wildfire_infusion_stacks_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget())
                    GetTarget()->AddAura(SPELL_WILDFIRE_INFUSION, GetTarget());
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget())
                    GetTarget()->RemoveAura(SPELL_WILDFIRE_INFUSION);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_wildfire_infusion_stacks_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_wildfire_infusion_stacks_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_wildfire_infusion_stacks_AuraScript();
        }
};

// Wildfire Infusion - 116816
class spell_mogu_wildfire_infusion : public SpellScriptLoader
{
    public:
        spell_mogu_wildfire_infusion() : SpellScriptLoader("spell_mogu_wildfire_infusion") { }

        class spell_mogu_wildfire_infusion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mogu_wildfire_infusion_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* caster = GetCaster())
                    if (auto const aura = caster->GetAura(SPELL_WILDFIRE_INFUSION_STACK))
                        aura->ModStackAmount(-1);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_mogu_wildfire_infusion_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mogu_wildfire_infusion_SpellScript();
        }
};

// Draw Flame - 116711
class spell_draw_flame : public SpellScriptLoader
{
    public:
        spell_draw_flame() : SpellScriptLoader("spell_draw_flame") { }

        class spell_draw_flame_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_draw_flame_SpellScript);

            void ReplaceTarget(std::list<WorldObject*>& targets)
            {
                targets.clear();

                if (Unit* feng = GetCaster())
                {
                    std::list<Creature*> wildfireSpark;
                    feng->GetCreatureListWithEntryInGrid(wildfireSpark, NPC_WILDFIRE_SPARK, 65.0f);

                    for (auto&& itr : wildfireSpark)
                        targets.push_back(itr);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_draw_flame_SpellScript::ReplaceTarget, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_draw_flame_SpellScript();
        }
};

// Arcane Velocity - 116365
class spell_mogu_arcane_velocity : public SpellScriptLoader
{
    public:
        spell_mogu_arcane_velocity() : SpellScriptLoader("spell_mogu_arcane_velocity") { }

        class spell_mogu_arcane_velocity_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mogu_arcane_velocity_SpellScript);

            void DealDamage()
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                if (!caster || !target)
                    return;

                // I don`t know why it happened. spell hasn`t any vulnerability attributes, but it make hurt in ice block...
                if (target->HasAuraType(SPELL_AURA_SCHOOL_IMMUNITY))
                {
                    SetHitDamage(0);
                    return;
                }

                float distance = caster->GetExactDist2d(target);

                uint8 mode = GetCaster()->GetInstanceScript()->instance->GetSpawnMode();
                uint32 mindmg = (mode == RAID_DIFFICULTY_10MAN_NORMAL ? 39000 : (mode == RAID_DIFFICULTY_25MAN_NORMAL ? 44850 : (mode == RAID_DIFFICULTY_10MAN_HEROIC ? 58500 : (mode == RAID_DIFFICULTY_25MAN_HEROIC ? 67275 : 16770))));
                uint32 range = (mode == RAID_DIFFICULTY_10MAN_NORMAL ? 1850 : (mode == RAID_DIFFICULTY_25MAN_NORMAL ? 2050 : (mode == RAID_DIFFICULTY_10MAN_HEROIC ? 2850 : (mode == RAID_DIFFICULTY_25MAN_HEROIC ? 3050 : 710))));

                if (distance >= 0 && distance <= 60)
                    SetHitDamage(mindmg + range * distance);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_mogu_arcane_velocity_SpellScript::DealDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mogu_arcane_velocity_SpellScript();
        }
};

// Mogu Inversion - 118300 / 118302 / 118304 / 118305 / 118307 / 118308 / 132296 / 132297 / 132298
class spell_mogu_inversion : public SpellScriptLoader
{
    public:
        spell_mogu_inversion() : SpellScriptLoader("spell_mogu_inversion") { }

        class spell_mogu_inversion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mogu_inversion_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget())
                    GetTarget()->RemoveAurasDueToSpell(SPELL_SHROUD_OF_REVERASL_BUTTON);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget())
                    GetTarget()->CastSpell(GetTarget(), SPELL_SHROUD_OF_REVERASL_BUTTON, true);
            }

            void Register() override
            {
                OnEffectApply     += AuraEffectApplyFn(spell_mogu_inversion_AuraScript::OnApply,   EFFECT_0, SPELL_AURA_OVERRIDE_SPELLS, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_mogu_inversion_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_OVERRIDE_SPELLS, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mogu_inversion_AuraScript();
        }
};

// GameObject - 211628 - Shroud of reversal
class go_inversion : public GameObjectScript
{
    public:
        go_inversion() : GameObjectScript("go_inversion") { }

        struct go_inversionAI : public GameObjectAI
        {
            go_inversionAI(GameObject* go) : GameObjectAI(go) { }

            bool OnGossipHello(Player* player, bool isUse) override
            {
                if (!isUse)
                    return true;

                if (!player->IsInCombat())
                    return true;

                if (player->GetRoleForGroup(player->GetTalentSpecialization()) != ROLES_TANK)
                    return true;

                // Check if player already have one of available spells
                if (player->HasAura(SPELL_SHROUD_OF_REVERASL_BUTTON) || player->HasAura(SPELL_NULLIFICATION_BARRIER_BUTTON))
                    return true;

                player->CastSpell(player, SPELL_SHROUD_OF_REVERASL_BUTTON, false);
                me->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);

                return false;
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new go_inversionAI(go);
        }
};

// GameObject - 211626 - Nullification Barrier
class go_cancel : public GameObjectScript
{
    public:
        go_cancel() : GameObjectScript("go_cancel") { }

        struct go_cancelAI : public GameObjectAI
        {
            go_cancelAI(GameObject* go) : GameObjectAI(go) { }

            bool OnGossipHello(Player* player, bool isUse) override
            {
                if (!isUse)
                    return true;

                if (!player->IsInCombat())
                    return true;

                if (player->GetRoleForGroup(player->GetTalentSpecialization()) != ROLES_TANK)
                    return true;

                if (player->HasAura(SPELL_SHROUD_OF_REVERASL_BUTTON) || player->HasAura(SPELL_NULLIFICATION_BARRIER_BUTTON))
                    return true;

                player->CastSpell(player, SPELL_NULLIFICATION_BARRIER_BUTTON, false);
                me->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);

                return false;
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new go_cancelAI(go);
        }
};

class spell_feng_spirit_bolts : public SpellScriptLoader
{
    public:
        spell_feng_spirit_bolts() : SpellScriptLoader("spell_feng_spirit_bolts") { }

        class spell_feng_spirit_bolts_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_feng_spirit_bolts_SpellScript)

            void FilterTargets(std::list<WorldObject*>& targetList)
            {
                size_t const maxTargets = GetCaster()->GetMap()->IsHeroic() ? 8 : 3;

                if (targetList.size() > maxTargets)
                    targetList.resize(maxTargets);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_feng_spirit_bolts_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_feng_spirit_bolts_SpellScript();
        }
};

struct PlayerTargetSelector
{
    bool operator ()(WorldObject const* object) const
    {
        if (object->GetTypeId() == TYPEID_PLAYER)
            return false;
        return true;
    }
};

class spell_feng_wildfire_spark : public SpellScriptLoader
{
    public:
        spell_feng_wildfire_spark() : SpellScriptLoader("spell_feng_wildfire_spark") { }

        class spell_feng_wildfire_spark_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_feng_wildfire_spark_SpellScript);

            std::list<WorldObject*> m_targets;

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(PlayerTargetSelector());
                targets.remove_if(TankSpecTargetSelector());
                Trinity::Containers::RandomResizeList(targets, 1);
                m_targets = targets;
            }

            void CopyTargets(std::list<WorldObject*>& targets)
            {
                targets = m_targets;
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_feng_wildfire_spark_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_feng_wildfire_spark_SpellScript::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_feng_wildfire_spark_SpellScript();
        }
};

// Arcane Resonance - 116434
class spell_mogu_arcane_resonance : public SpellScriptLoader
{
    public:
        spell_mogu_arcane_resonance() : SpellScriptLoader("spell_mogu_arcane_resonance") { }

        class spell_mogu_arcane_resonance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mogu_arcane_resonance_SpellScript);

            uint8 targetCount;

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if (PlayerTargetSelector());
                targetCount = targets.size();
            }

            void DealDamage()
            {
                if (targetCount > 25)
                    targetCount = 0;

                SetHitDamage(GetHitDamage() * targetCount);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mogu_arcane_resonance_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnHit += SpellHitFn(spell_mogu_arcane_resonance_SpellScript::DealDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mogu_arcane_resonance_SpellScript();
        }
};

// Siphoning Shield Apply Vehicle 117781
class spell_siphoning_shield_vehicle : public SpellScriptLoader
{
    public:
        spell_siphoning_shield_vehicle() : SpellScriptLoader("spell_siphoning_shield_vehicle") { }

        class spell_siphoning_shield_vehicle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_siphoning_shield_vehicle_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster() && GetHitUnit() && GetCaster()->ToCreature() && GetHitUnit()->ToCreature() && GetHitUnit()->ToCreature()->AI())
                {
                    // Become invisible
                    GetCaster()->RemoveAura(SPELL_SHIELD_DISPLAYED);
                    // Make the shield on Feng's arm
                    GetHitUnit()->RemoveAura(SPELL_TOGGLE_SHIELD);
                    // No more visual link between Feng & shield, as shield is on Feng
                    GetHitUnit()->RemoveAura(SPELL_LINKED_SHIELD);
                    // Scheduling next siphoning shield in 30 secs
                    GetHitUnit()->ToCreature()->AI()->DoAction(ACTION_SCHEDULE_SHIELD);
                }

            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_siphoning_shield_vehicle_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_siphoning_shield_vehicle_SpellScript();
        }
};

// 115911 - Shroud of Reversal
class spell_shroud_of_reversal : public AuraScript
{
    PrepareAuraScript(spell_shroud_of_reversal);

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        // TriggerSpell == 0, spam
        PreventDefaultAction();

        if (Unit* caster = GetCaster())
        {
            for (auto&& itr: shroudOfReversalMap)
            {
                if (GetUnitOwner()->HasAura(itr.first))
                {
                    caster->CastSpell(caster, itr.second, true);
                    Remove();
                    break;
                }
            }
        }
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (Unit* caster = GetCaster())
        {
            if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_ARCANE_VELOCITY_DAMAGE)
            {
                caster->CastSpell(caster, SPELL_ARCANE_VELOCITY_BUTTON, true);
                Remove();
            }
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_shroud_of_reversal::HandlePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        OnProc += AuraProcFn(spell_shroud_of_reversal::HandleProc);
    }
};

// Siphoning Shield Missle 117203
class spell_siphoning_shield_missle : public SpellScript
{
    PrepareSpellScript(spell_siphoning_shield_missle);

    void SelectTargets(SpellDestination& dest)
    {
        Position tarPos = { 0.f, 0.f, 466.31f, 0.f };
        float dist = frand(0.f, 28.1f);
        GetRandPosFromCenterInDist(centerPos.GetPositionX(), centerPos.GetPositionY(), dist, tarPos.m_positionX, tarPos.m_positionY);

        dest.Relocate(tarPos);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_siphoning_shield_missle::SelectTargets, EFFECT_0, TARGET_DEST_DEST_RANDOM);
    }
};

// 115730 - Reversal: Lightning Fists
// 116936 - Reversal: Epicenter
// 116922 - Reversal: Wildfire Spark
// 116938 - Reversal: Arcane Velocity
// 118194 - Reversal: Arcane Resonance
class spell_achievement_anything_you_can_do_i_can_do_better : public SpellScript
{
    PrepareSpellScript(spell_achievement_anything_you_can_do_i_can_do_better);

    void HandleHit()
    {
        Map* instance = GetCaster()->GetMap();
        auto itr = achievementWorldStates.find(GetSpellInfo()->Id);
        if (itr != achievementWorldStates.end())
        {
            instance->SetWorldState(itr->second, 1);
            for (auto&& pair : achievementWorldStates)
                if (pair.second != WORLD_STATE_ANYTHING_YOU_CAN_DO_I_CAN_DO_BETTER)
                    if (!instance->GetWorldState(pair.second))
                        return;
            instance->SetWorldState(WORLD_STATE_ANYTHING_YOU_CAN_DO_I_CAN_DO_BETTER, 1);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_achievement_anything_you_can_do_i_can_do_better::HandleHit);
    }
};

// 229
class sat_nullification_barrier : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* target)
    {
        return target->ToUnit() != nullptr;
    }

    void OnTriggeringApply(WorldObject* target)
    {
        Unit* unit = target->ToUnit();
        if (unit->IsInRaidWith(GetCaster()))
            unit->CastSpell(unit, SPELL_NULLIFICATION_BARRIER_EFFECT, true);
        else if (unit->GetTypeId() == TYPEID_UNIT)
        {
            if (unit->GetEntry() == NPC_WILDFIRE_SPARK || unit->GetEntry() == NPC_SOUL_FRAGMENT)
                unit->ToCreature()->DespawnOrUnsummon();
            else if (unit->GetEntry() == NPC_SIPHONING_SHIELD)
            {
                // Remove all souls
                std::list<Creature*> Spirits;
                GetCreatureListWithEntryInGrid(Spirits, GetCaster(), NPC_SOUL_FRAGMENT, 100.0f);

                for (auto&& itr : Spirits)
                    itr->DespawnOrUnsummon();
            }
        }
    }

    void OnTriggeringRemove(WorldObject* target)
    {
        Unit* unit = target->ToUnit();
        if (unit->IsInRaidWith(GetCaster()))
            unit->RemoveAurasDueToSpell(SPELL_NULLIFICATION_BARRIER_EFFECT);
    }
};

void AddSC_boss_feng()
{
    new boss_feng();
    new npc_lightning_fist();
    new npc_wild_spark();
    new npc_siphon_shield();
    new npc_soul_fragment();
    new spell_mogu_epicenter();
    new spell_mogu_wildfire_spark();
    new spell_wildfire_infusion_stacks();
    new spell_mogu_wildfire_infusion();
    new spell_draw_flame();
    new spell_mogu_arcane_velocity();
    new spell_mogu_arcane_resonance();
    new spell_mogu_inversion();
    new spell_feng_spirit_bolts();
    new spell_feng_wildfire_spark();
    new spell_siphoning_shield_vehicle();
    new aura_script<spell_shroud_of_reversal>("spell_shroud_of_reversal");
    new spell_script<spell_siphoning_shield_missle>("spell_siphoning_shield_missle");
    new spell_script<spell_achievement_anything_you_can_do_i_can_do_better>("spell_achievement_anything_you_can_do_i_can_do_better");
    new atrigger_script<sat_nullification_barrier>("sat_nullification_barrier");
    new go_inversion;
    new go_cancel;
}
