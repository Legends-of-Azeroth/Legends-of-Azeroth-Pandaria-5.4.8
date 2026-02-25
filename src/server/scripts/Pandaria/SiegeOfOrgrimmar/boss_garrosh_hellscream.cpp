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
#include "siege_of_orgrimmar.h"
#include "PlayerAI.h"
#include "TargetedMovementGenerator.h"

// Todo: find berserk info.

enum Yells
{
    TALK_AGGRO,
    TALK_SIEGE_ENGINEERS,
    TALK_WARSONG,
    TALK_FARSEERS,
    TALK_SWITCH_PHASE_2,
    TALK_PHASE_2_TRANSFORM,
    TALK_REALM,
    TALK_WHIRLWIND,
    TALK_WHIRLWIND_ANN,
    TALK_WHIRLWIND_EMP_ANN,
    TALK_WHIRLWIND_EMP,
    TALK_SWITCH_PHASE_3,

    // Heart Whispers
    TALK_HEART_WHISPER     = 0,

    // Heroic
    TALK_PHASE_4_BEGUN = 17,
    TALK_PHASE_4_PUSH,
    TALK_MALICE_ANN,
    TALK_BOMBARDMENT,
    TALK_IRON_STAR_LAUNCH_ANN,
    TALK_BOMBARDMENT_ANN,
    TALK_MANIFEST_RAGE, // only on cast success
};

enum Spells
{
    SPELL_BERSERK_RAID_FINDER             = 64238,
    SPELL_WEAK_MINDED                     = 145331,

    SPELL_DESECRATED                      = 144762, // periodic damage
    SPELL_DESECRATE_SELECTOR              = 144745,
    SPELL_DESECRATED_EMPOWER              = 144817,
    SPELL_DESECRATED_WEAPON_AREATRIGGER   = 144760,
    SPELL_EMPOWERED_DESECRATED_WEAPON_AT  = 144818,
    SPELL_DESECRATE_MISSLE                = 144748,
    SPELL_DESECRATE_EMPOWER_MISSLE        = 144749,
    SPELL_IRON_STAR_VISUAL                = 144645,
    SPELL_HELLSCREAMS_WARSONG             = 144821,
    SPELL_POWER_IRON_STAR                 = 144616,
    SPELL_IRON_STAR_IMPACT_EFF            = 144650,
    SPELL_IRON_STAR_IMPACT_EFF_2          = 144653,
    SPELL_ENTER_REALM_OF_YSHAARJ          = 144867,
    SPELL_REALM_OF_YSHAARJ                = 144954,
    SPELL_SHAARJS_PROTECTION              = 144945,
    SPELL_SHAARJS_PROTECTION_AURA         = 144920,
    SPELL_ANNIHILATE                      = 144969,
    SPELL_GRIPPING_DESPAIR                = 145183,
    SPELL_WHIRLING_CORRUPTION             = 144985,
    SPELL_EMPOWERED_WHIRLING_CORRUPTION   = 145037,
    SPELL_WHIRLING_CORRUPTION_VISUAL      = 144994,
    SPELL_TOUCH_OH_YSHAARJ                = 145065,
    SPELL_EMPOWERED_WHIRLING_MISSLE       = 145023, // summon minion of Yshaarj
    SPELL_EMPOWERED_GRIPPING_DESPAIR      = 145195,
    SPELL_EXPLOSIVE_DESPAIR               = 145199,
    SPELL_EXPLOSIVE_DESPAIR_AURA          = 145213,
    SPELL_EMPOWERED_TOUCH_OF_YSHAARJ      = 145171,
    SPELL_EMPOWERED_PROC                  = 145050, // death proc for minions of Y`shaarj
    SPELL_EMPOWERING_CORRUPTION           = 145043,
    SPELL_EMPOWERING_CORRUPTION_2         = 149536,
    SPELL_TOUCH_OF_YSHAARJ_TARGET_CHARM   = 145071,

    // Garrosh Misc
    SPELL_DESECRATED_EMPOWERED            = 145829, // at gain 75
    SPELL_WHIRLWIND_EMPOWERED             = 145833, // at gain 25
    SPELL_TOUCH_EMPOWERED                 = 145832, // at gain 50
    SPELL_GRIPPING_EMPOWERED              = 145831, // at gain 100
    SPELL_PHASE_THIRD_TRANSFORM           = 145598,
    SPELL_PHASE_THIRD_DE_TRANSFORM        = 146966,
    SPELL_TRANSITION_VISUAL               = 144852, // 8s timer on garrosh at phase change to 2td
    SPELL_TRANSITION_EFFECT               = 144842,
    SPELL_TANSITION_VISUAL_THIRD          = 145222, // same but for third phase
    SPELL_TRANSITION_THIRD_EFFECT         = 145246,
    SPELL_VISUAL_FLEE_SECOND_PHASE        = 146756,
    SPELL_VISUAL_FLEE_THIRD_PHASE         = 146845,
    SPELL_NON_REGENERATE_POWER            = 72242,
    SPELL_SUMMON_FADING_BREATH            = 147296,
    SPELL_HOPE_MISSLE                     = 149002,
    SPELL_COURAGE_AURA                    = 148983,
    SPELL_FAITH_AURA                      = 148994,
    SPELL_HOPE_AURA                       = 149004,
    SPELL_TOUCH_OF_YSHAARJ_PLAYER         = 145599, // Periodical cast
    SPELL_EMP_TOUCH_OF_YSHAARJ_PLAYER_EFF = 145175,
    SPELL_TOUCH_OF_YSHAARJ_PLAYER_EFF     = 145071,
    SPELL_FINAL_HORDE                     = 149978, // for HC 
    SPELL_FINALE_ALLIANCE                 = 149979,
    SPELL_FINALE_HORDE_SHARED             = 147301, // for another difficulty
    SPELL_FINALE_ALLIANCE_SHARED          = 147302,
    SPELL_ENTER_REALM_OF_YSHAARJ_UNK      = 144878, // by garrosh at teleport to realm
    SPELL_TELEPORT_TO_REALM_SERPENT       = 144880,
    SPELL_TELEPORT_TO_SERPENT             = 144881,
    SPELL_TELEPORT_TO_REALM_CRANE         = 144883,
    SPELL_TELEPORT_TO_CRANE               = 144884,
    SPELL_TELEPORT_TO_REALM_SPRING        = 144885,
    SPELL_TELEPORT_TO_SPRING              = 144886,
    SPELL_COSMETIC_CHANNEL                = 145431,
    SPELL_ABSORB_ENERGY                   = 144946,
    SPELL_GROWING_POWER                   = 144947,
    SPELL_HEARTBEAT_SOUND                 = 148574,
    SPELL_THROW_AXE_AT_HEART              = 145235,
    SPELL_REALM_OF_YSHAARJ_REMOVE         = 145647,
    SPELL_CONSUMED_COURAGE                = 149011,
    SPELL_CONSUMED_HOPE                   = 149032,
    SPELL_CONSUMED_FAITH                  = 149033,
    SPELL_GARROSH_ENERGY                  = 145801,
    SPELL_ZEN                             = 131221, // wrong id, no data in sniffs about prevent falling damage
    SPELL_JUMP_TO_GROUND                  = 144956,
    SPELL_JUMO_TO_GROUND_EFF              = 145231,

    // Yshaarj Realm Minions
    SPELL_CRUSHING_FEAR_EFF               = 147324,
    SPELL_CRUSHING_FEAR                   = 147320,
    SPELL_CRUSHING_FEAR_LAUNCHER          = 147327,
    SPELL_CRUSHING_FEAR_SELECTOR          = 147325,
    SPELL_EMBODIED_DOUBT                  = 149347,
    SPELL_ULTIMATE_DESPAIR                = 147341,
    SPELL_EMBODIED_DESPAIR                = 145276,

    // Heroic
    SPELL_CALL_BOMBARDEMENT_25            = 147120,
    SPELL_MALEFICE_25                     = 147209,
    SPELL_MANIFEST_RAGE_25                = 147011,

    // Minions
    SPELL_HAMSTRING                     = 144582,
    SPELL_CHAIN_LIGHTNING               = 144584,
    SPELL_ANCENTRAL_CHAIN_HEAL          = 144583,
    SPELL_FURY                          = 144588,
    SPELL_EXPLODING_IRON_STAR           = 144798,

    // Anim
    ANIM_AI_KIT_AROUND_HEART_TALK       = 4448,
    ANIM_AI_KIT_TRANSITION              = 4210,

    // Heroic
    SPELL_MALICE                        = 147209,
    SPELL_MALICE_EFF                    = 147229,
    SPELL_MALICIOUS_BLAST               = 147235,
    SPELL_MALICIOUS_ENERGY              = 147236, // in case if malice not affect 2(5) players - gain power for garrosh + aoe
    SPELL_MALICIOUS_ENERGY_EXPLOSION    = 147733,
    SPELL_CALL_BOMBARDMENT              = 147120,
    SPELL_BOMBARDMENT_SELECTOR_GUNSHIP  = 147131, // seems like should select any npc for next spell
    SPELL_BOMBARDMENT_SELECTOR          = 147132,
    SPELL_BOMBARDMENT_MISSLE_1          = 147133,
    SPELL_BOMBARDMENT_AT                = 147135,
    SPELL_CLUMP_CHECK                   = 147126,
    SPELL_CLUMP_CHECK_EFF               = 147130,
    SPELL_NAPALM                        = 147136,
    SPELL_IRON_STAR_FIXATE              = 147665,
    SPELL_IRON_STAR_FIXATE_SELECTOR     = 147712,
    SPELL_MANIFEST_RAGE                 = 147011,
    SPELL_MANIFESTATION_RAGE            = 147035,
    SPELL_MANIFESTATION_RAGE_EFF        = 147013,
    SPELL_UNSTABLE_IRON_STAR_EXPLOSION  = 147173,
    SPELL_FIRE_IRON_STAR                = 147053,
    SPELL_FIRE_UNSTABLE_IRON_STAR       = 147047,
    SPELL_IRON_STAR_CRATER              = 147048,
    SPELL_UNSTABLE_IRON_STAR_AT         = 147148,
    SPELL_UNSTABLE_IRON_STAR_AT_EFF     = 147179,
    SPELL_UNSTABLE_IRON_STAR_GARROSH    = 147177, // stun him
    SPELL_PHASE_FOUR_TRANSFORM          = 146987,
    SPELL_ENTER_REALM_OF_GARROSH        = 146984,
    SPELL_ENTER_REALM_OF_GARROSH_EFF    = 146985,
    SPELL_TELEPORT_TO_GARROSH_REALM     = 146986,
    SPELL_GARROSH_WEAK_MINDED           = 148440,
    SPELL_BLOOD_FRENZIED                = 147300,
    SPELL_BLOOD_FRENZIED_SELECTOR       = 149336,

    // Misc
    SPELL_WEAKENED_BLOWS                = 115798,
    SPELL_MORTAL_WOUNDS                 = 115804,
    SPELL_MASTER_POISONER               = 93068,
    SPELL_SUMMON_FADING_BREATH_MISSLE   = 147296,
    SPELL_SUMMON_FADING_BREATH_EFF      = 147295,
};

enum Events
{
    EVENT_DESECRATED = 1,
    EVENT_WARSONG,
    EVENT_ENGINEERS,
    EVENT_WOLF_RIDER,
    EVENT_WARBRINGER,
    EVENT_TRANSFORM_PHASE_SECOND,
    EVENT_ENTER_ANOTHER_REALM,
    EVENT_LEAVE_ANOTHER_REALM,
    EVENT_GAIN_ENERGY,
    EVENT_ANNIHILATE,
    EVENT_RETURN,
    EVENT_WHIRLWIND,
    EVENT_TOUCH,
    EVENT_GRIPPING,
    EVENT_TRANSFORM_PHASE_THIRD,
    EVENT_PREPUSH_4TD,
    EVENT_PHASE_FOUR,
    EVENT_MALICE,
    EVENT_BOMBARDMENT,
    EVENT_GAIN_POWER,
    EVENT_MANIFEST_RAGE,

    // misc
    EVENT_MOVE,
    EVENT_POWER_IRON_STAR,
    EVENT_HAMSTRING,
    EVENT_CHAIN_LIGHTNING,
    EVENT_CHAIN_HEAL,
    EVENT_COLLISION,
    EVENT_CRUSHING_FEAR,
    EVENT_EMBODIED_DOUBT,
    EVENT_EMBODIED_DESPAIR,
    EVENT_ALT_WORLD,
    EVENT_EVADE_CHECK,
    EVENT_TOUCH_OF_YSHAARJ_PLAYER,
};

enum GuardsType
{
    GUARD_TYPE_WOLF_RIDER,
    GUARD_TYPE_WARBRINGER,
    GUARD_TYPE_ENGINEER,
};

enum Phases
{
    PHASE_FIRST  = 1,
    PHASE_SECOND,
    PHASE_THIRD,
    PHASE_MY_WORLD,
    PHASE_ALT,
};

enum Realms
{
    REALM_TERRACE_OF_THE_ENDLESS_SPRING = 1,
    REALM_TEMPLE_OF_THE_JADE_SERPENT,
    REALM_TEMPLE_OF_THE_RED_CRANE,
};

const std::map<uint32, std::array<uint32, 3>> invGarroshRealmsType =
{
    { REALM_TERRACE_OF_THE_ENDLESS_SPRING, { SPELL_TELEPORT_TO_REALM_SPRING,  2, NPC_EMBODIED_FEAR      } },
    { REALM_TEMPLE_OF_THE_JADE_SERPENT,    { SPELL_TELEPORT_TO_REALM_SERPENT, 1, NPC_EMBODIED_DOUBT     } },
    { REALM_TEMPLE_OF_THE_RED_CRANE,       { SPELL_TELEPORT_TO_REALM_CRANE,   0, NPC_EMBODIED_DESPAIR_G } },
};

class TouchYshaarjAI : public PlayerAI
{
    public:
        TouchYshaarjAI(Player* player) : PlayerAI(player) { }

        EventMap playerEvents;
        int32 amount;

        void OnCharmed(bool apply) override
        {
            if (apply)
            {
                // Handle set health pct inc depend of mind control type (seems like spell for this doesn`t exist)
                if (Creature* garrosh = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GARROSH_HELLSCREAM) : ObjectGuid::Empty))
                {
                    float percent = me->GetHealthPct();
                    amount = garrosh->HasAura(SPELL_TOUCH_EMPOWERED) ? 150 : 50;

                    if (me->GetMap()->IsHeroic())
                        amount += 150;

                    me->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_PCT, float(amount), true);
                    if (me->IsAlive())
                        me->SetHealth(std::max(uint32(me->GetMaxHealth() * percent / 100.f), uint32(1)));
                }

                me->SetByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_UNK1);
                me->UnsummonPetTemporaryIfAny();
                me->CastStop();
                me->AttackStop();
                me->SendMovementFlagUpdate(true);
                playerEvents.ScheduleEvent(EVENT_TOUCH_OF_YSHAARJ_PLAYER, urand(2 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));

                // just move to near point for update movement for whole
                float angle = frand(0.0f, 2 * M_PI);
                float x = me->GetPositionX() + 0.5f * cos(angle);
                float y = me->GetPositionY() + 0.5f * sin(angle);
                float z = me->GetPositionZ();
                me->UpdateAllowedPositionZ(x, y, z);
                me->GetMotionMaster()->MovePoint(0, x, y, z, angle);
            }
            else
            {
                // Handle remove health pct inc depend of mind control type (seems like spell for this doesn`t exist)
                if (Creature* garrosh = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GARROSH_HELLSCREAM) : ObjectGuid::Empty))
                {
                    float percent = me->GetHealthPct();
                    uint32 currentHealth = me->GetHealth();

                    me->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_PCT, float(amount), false);

                    // Set Current health depend of improved health of Y`shaarj
                    if (me->IsAlive())
                        me->SetHealth(std::max(uint32(currentHealth), uint32(1)));
                }

                me->RemoveByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_UNK1);
                me->ResummonPetTemporaryUnSummonedIfAny();
                playerEvents.Reset();
                me->InterruptNonMeleeSpells(false, SPELL_TOUCH_OF_YSHAARJ_PLAYER);
                me->RemoveAurasDueToSpell(SPELL_WEAKENED_BLOWS);
                me->RemoveAurasDueToSpell(SPELL_MORTAL_WOUNDS);
                me->RemoveAurasDueToSpell(SPELL_MASTER_POISONER); // hotfixed 18.09.2013: "Removing the Touch of Y'Sharrj or Empowered Touch of Y'Shaarj debuff now also removes Weakened Blows, Mortal Wounds, and Master Poisoner effects from the player"

                me->CastStop();
                me->AttackStop();
                MovementGeneratorType mgType = me->GetMotionMaster()->GetMotionSlotType(MOTION_SLOT_ACTIVE);
                if (mgType == FOLLOW_MOTION_TYPE || mgType == CHASE_MOTION_TYPE)
                {
                    TargetedMovementGeneratorBase* movementGenerator = dynamic_cast<TargetedMovementGeneratorBase*>(me->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_ACTIVE));
                    if (movementGenerator && !(movementGenerator->GetTarget() && movementGenerator->GetTarget()->IsAlive()))
                    {
                        // Delete targeted movement generator if there is no target anymore or the target is dead
                        if (me->GetMotionMaster()->top() == me->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_CONTROLLED))
                            me->GetMotionMaster()->DirectDelete(MOTION_SLOT_ACTIVE);
                        else
                        {
                            me->GetMotionMaster()->MovementExpired();
                            me->StopMoving();
                        }
                    }
                }
            }
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage) override
        {
            if (me->HealthBelowPct(21) || damage >= me->GetHealth())
            {
                // Prevent killable from players
                if (damage >= me->GetHealth())
                    damage = 0;

                me->RemoveAurasDueToSpell(SPELL_EMPOWERED_TOUCH_OF_YSHAARJ);
                me->RemoveAurasDueToSpell(SPELL_TOUCH_OH_YSHAARJ);
                me->RemoveAurasDueToSpell(SPELL_TOUCH_OF_YSHAARJ_PLAYER_EFF);
                me->RemoveAurasDueToSpell(SPELL_EMP_TOUCH_OF_YSHAARJ_PLAYER_EFF);
            }
        }

        void UpdateAI(uint32 diff) override 
        {
            playerEvents.Update(diff);
    
            while (uint32 eventId = playerEvents.ExecuteEvent())
            {
                if (eventId == EVENT_TOUCH_OF_YSHAARJ_PLAYER)
                {
                    if (Player* target = ObjectAccessor::GetPlayer(*me, getNotCharmedPlayerGUID()))
                        me->CastSpell(target, SPELL_TOUCH_OF_YSHAARJ_PLAYER, false);

                    playerEvents.ScheduleEvent(EVENT_TOUCH_OF_YSHAARJ_PLAYER, 7 * IN_MILLISECONDS);
                }
                break;
            }
        }

    private:
        ObjectGuid getNotCharmedPlayerGUID()
        {
            std::list<Player*> targets;
            GetPlayerListInGrid(targets, me, 200.0f);
            targets.remove_if([=](Player* target) { return target && (target->HasAura(SPELL_TOUCH_OF_YSHAARJ_PLAYER_EFF) || target->HasAura(SPELL_EMP_TOUCH_OF_YSHAARJ_PLAYER_EFF) || target->HasAura(SPELL_TOUCH_OH_YSHAARJ) || target->HasAura(SPELL_EMPOWERED_TOUCH_OF_YSHAARJ)); });

            if (targets.empty())
                return ObjectGuid::Empty;

            return Trinity::Containers::SelectRandomContainerElement(targets)->GetGUID();
        }
};

class boss_garrosh_hellscream : public CreatureScript
{
    public:
        boss_garrosh_hellscream() : CreatureScript("boss_garrosh_hellscream") { }

        struct boss_garrosh_hellscreamAI : public BossAI
        {
            boss_garrosh_hellscreamAI(Creature* creature) : BossAI(creature, DATA_GARROSH_HELLSCREAM) 
            {
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_HEALING_PCT, true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
            }
            EventMap RealmEvents;
            bool wRiderGates;
            bool sPhase;
            bool inRealm;
            bool firstTransition;
            bool clumpCheckDone;
            bool lastPhaseTriggered;
            uint8 phase;
            uint32 currentEmboded, SelectedEmbodied;
            uint32 strikeCounter;
            uint32 despairCounter;
            uint32 touchCount;
            uint32 bombardCount;
            uint32 powerGainCount;
            ObjectGuid targetGUID;
            ObjectGuid despairTargetGUID;
            std::list<uint64> realmMinionGUIDs;

            void Reset() override
            {
                _Reset();
                events.Reset();
                RealmEvents.Reset();
                summons.DespawnAll();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_GARROSH_HELLSCREAM, NOT_STARTED);

                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_COURAGE_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FAITH_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HOPE_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DESECRATED);
                }

                me->SetPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 0);
                DoCast(me, SPELL_NON_REGENERATE_POWER, true);

                wRiderGates = false;
                phase = PHASE_FIRST;
                targetGUID = ObjectGuid::Empty;
                currentEmboded = 0;
                SelectedEmbodied = 0;
                despairTargetGUID = ObjectGuid::Empty;
                strikeCounter = 0;
                despairCounter = 0;
                bombardCount = 0;
                touchCount = 0;
                powerGainCount = 0;
                sPhase = false;
                inRealm = false;
                firstTransition = true;
                clumpCheckDone = false;
                lastPhaseTriggered = false;
                me->RemoveAurasDueToSpell(SPELL_TRANSITION_EFFECT);
                me->RemoveAurasDueToSpell(SPELL_SHAARJS_PROTECTION);
                me->RemoveAurasDueToSpell(SPELL_WHIRLWIND_EMPOWERED);
                me->RemoveAurasDueToSpell(SPELL_TOUCH_EMPOWERED);
                me->RemoveAurasDueToSpell(SPELL_DESECRATED_EMPOWERED);
                me->RemoveAurasDueToSpell(SPELL_GRIPPING_EMPOWERED);
                me->RemoveAurasDueToSpell(SPELL_TRANSITION_THIRD_EFFECT);
                me->HandleEmoteStateCommand(EMOTE_STATE_NONE);
                me->SetAIAnimKitId(0);
                me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, 101441);
                realmMinionGUIDs.clear();
                me->GetMap()->SetWorldState(WORLDSTATE_STRIKE, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_STRIKE_2, 0);

                scheduler
                    .Schedule(Seconds(1), [this](TaskContext context)
                {
                    if (instance && instance->GetData(DATA_GARROSH_HELLSCREAM_PREVE_EVENT) == DONE)
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case TYPE_GARROSH_FIRST_TRANSITION:
                        return firstTransition ? 1 : 0;
                    case TYPE_GARROSH_REALM:
                        return inRealm ? 1 : 0;
                    case TYPE_EXPLOSIVE_DESPAIR:
                        return despairCounter;
                    case TYPE_CLUMP_CHECK_DONE:
                        return clumpCheckDone;
                }

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case TYPE_GARROSH_FIRST_TRANSITION:
                        firstTransition = false;
                        break;
                    case TYPE_GARROSH_STRIKE_COUNT:
                        if (++strikeCounter >= 17)
                        {
                            me->GetMap()->SetWorldState(WORLDSTATE_STRIKE, 1);
                            me->GetMap()->SetWorldState(WORLDSTATE_STRIKE_2, 1);
                        }
                        break;
                    case TYPE_EXPLOSIVE_DESPAIR:
                        despairCounter = data;
                        break;
                }
            }

            void EnterEvadeMode() override
            {
                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                me->RemoveAllAreasTrigger();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_COURAGE_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FAITH_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HOPE_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DESECRATED);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ZEN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_REALM_OF_YSHAARJ);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_EXPLOSIVE_DESPAIR_AURA);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    if (Creature* YshaarjHear = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_HEART_OF_YSHAARJ)))
                        YshaarjHear->AI()->Reset();
                }

                summons.DespawnAll();
                me->RemoveAurasDueToSpell(SPELL_TRANSITION_EFFECT);
                me->RemoveAurasDueToSpell(SPELL_SHAARJS_PROTECTION);
                me->RemoveAurasDueToSpell(SPELL_WHIRLWIND_EMPOWERED);
                me->RemoveAurasDueToSpell(SPELL_TOUCH_EMPOWERED);
                me->RemoveAurasDueToSpell(SPELL_DESECRATED_EMPOWERED);
                me->RemoveAurasDueToSpell(SPELL_GRIPPING_EMPOWERED);
                me->RemoveAurasDueToSpell(SPELL_TRANSITION_THIRD_EFFECT);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                _DespawnAtEvade();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                    for (auto&& itr : instance->instance->GetPlayers())
                        if (Player* target = itr.GetSource())
                            target->CastSpell(target, SPELL_ZEN, true);
                }

                Talk(TALK_AGGRO);
                me->HandleEmoteStateCommand(EMOTE_STATE_READY2H);
                events.ScheduleEvent(EVENT_DESECRATED, 10.5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_WARSONG, 12 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_ENGINEERS, 15 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_WOLF_RIDER, 60 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_WARBRINGER, 1 * IN_MILLISECONDS);
                RealmEvents.ScheduleEvent(EVENT_EVADE_CHECK, 1 * IN_MILLISECONDS);
            }

            void SetGUID(ObjectGuid guid, int32 /*type*/) override
            {
                despairTargetGUID = guid;
            }

            ObjectGuid GetGUID(int32 /*type*/) const override
            {
                return despairTargetGUID;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (phase == PHASE_MY_WORLD && lastPhaseTriggered) // no case any
                    return; 

                // In Period when Garrosh Switch between phases (interract to heart)
                if (sPhase && damage >= me->GetHealth())
                {
                    damage = 0;
                    return;
                }

                // Not die if we in realm
                if (GetData(TYPE_GARROSH_REALM) && damage >= me->GetHealth())
                {
                    damage = 0;
                    return;
                }

                // Not switch while in realm or transition
                if (GetData(TYPE_GARROSH_REALM) && HealthBelowPct(10))
                    return;

                // Not interrupt if casting anything
                if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(SPELL_WHIRLING_CORRUPTION) || me->HasAura(SPELL_EMPOWERED_WHIRLING_CORRUPTION))
                {
                    if ((phase == PHASE_FIRST || phase == PHASE_SECOND || IsHeroic() && phase == PHASE_THIRD) && damage >= me->GetHealth())
                        damage = 0;

                    return;
                }

                if (HealthBelowPct(10) && phase == PHASE_FIRST && !sPhase)
                {
                    if (damage >= me->GetHealth())
                        damage = 0;

                    sPhase = true;
                    events.Reset();

                    if (Unit* vict = me->GetVictim())
                        targetGUID = vict->GetGUID();

                    me->PrepareChanneledCast(me->GetOrientation());

                    if (Creature* YshaarjHear = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_HEART_OF_YSHAARJ)))
                        me->GetMotionMaster()->MovePoint(0, YshaarjHear->GetPositionX(), YshaarjHear->GetPositionY(), me->GetPositionZ());
                }

                if (HealthBelowPct(10) && phase == PHASE_SECOND && !sPhase)
                {
                    if (damage >= me->GetHealth())
                        damage = 0;

                    sPhase = true;
                    events.Reset();

                    if (Unit* vict = me->GetVictim())
                        targetGUID = vict->GetGUID();

                    me->PrepareChanneledCast(me->GetOrientation());

                    if (Creature* YshaarjHear = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_HEART_OF_YSHAARJ)))
                    {
                        YshaarjHear->AI()->Reset();
                        me->GetMotionMaster()->MovePoint(2, YshaarjHear->GetPositionX(), YshaarjHear->GetPositionY(), me->GetPositionZ());
                    }
                }

                // Last phase only in Heroic Mode
                if (IsHeroic() && HealthBelowPct(2) && phase == PHASE_THIRD && !sPhase)
                {
                    if (damage >= me->GetHealth())
                        damage = 0;

                    sPhase = true;
                    events.Reset();

                    if (Unit* vict = me->GetVictim())
                        targetGUID = vict->GetGUID();

                    me->PrepareChanneledCast(me->GetOrientation());

                    phase = PHASE_MY_WORLD;
                    Talk(TALK_PHASE_4_BEGUN);

                    RealmEvents.ScheduleEvent(EVENT_PREPUSH_4TD, 13.5 * IN_MILLISECONDS);
                    RealmEvents.ScheduleEvent(EVENT_PHASE_FOUR, 19.5 * IN_MILLISECONDS);

                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                me->RemoveAllAreasTrigger();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_COURAGE_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FAITH_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HOPE_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DESECRATED);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ZEN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_REALM_OF_YSHAARJ);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_EXPLOSIVE_DESPAIR_AURA);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    uint32 sceneSpellId = instance->GetData(DATA_GROUP_FACTION) ? SPELL_FINALE_HORDE_SHARED : SPELL_FINALE_ALLIANCE_SHARED;
                    DoCast(me, sceneSpellId, true);

                    if (Creature* YshaarjHear = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_HEART_OF_YSHAARJ)))
                        YshaarjHear->AI()->Reset();
                }

                summons.DespawnAll();
                events.Reset();

                RewardPersonalLoot();
                DoCast(me, SPELL_SUMMON_FADING_BREATH_MISSLE);
            }

            void RewardPersonalLoot()
            {
                Difficulty difficulty = me->GetMap()->GetDifficulty();
                uint32 index;
                switch (difficulty)
                {
                    case RAID_DIFFICULTY_10MAN_NORMAL:
                    case RAID_DIFFICULTY_25MAN_NORMAL:
                        index = 0;
                        break;
                    case RAID_DIFFICULTY_10MAN_HEROIC:
                    case RAID_DIFFICULTY_25MAN_HEROIC:
                        index = 1;
                        break;
                    case RAID_DIFFICULTY_1025MAN_FLEX:
                        index = 2;
                        break;
                    default:
                        return;
                }

                static std::vector<uint32> const weapons[]
                {
                    { 104404, 104405, 104401, 104403, 104400, 104406, 104399, 104402, 104409, 104407, 104408 }, // Normal
                    { 105685, 105692, 105691, 105686, 105684, 105690, 105683, 105688, 105687, 105693, 105689 }, // Heroic
                    { 105672, 105679, 105678, 105673, 105671, 105677, 105670, 105675, 105674, 105680, 105676 }, // Flex
                };

                for (auto&& itr : me->GetMap()->GetPlayers())
                {
                    Player* player = itr.GetSource();
                    if (!player->HasLootLockout(LootLockoutType::HeirloomWeapon, me->GetEntry(), difficulty) && roll_chance_f(20))
                    {
                        uint32 itemId = Trinity::Containers::SelectRandomContainerElement(weapons[index]);
                        Item* item = Item::CreateItem(itemId, 1);
                        player->SendDisplayToast(item, itemId, 1, TOAST_TYPE_ITEM, TOAST_DISPLAY_TYPE_ITEM);
                        player->StoreNewItem(item);
                        player->AddLootLockout(LootLockoutType::HeirloomWeapon, me->GetEntry(), difficulty);
                    }
                }
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
            { 
                if (std::find(realmMinionGUIDs.begin(), realmMinionGUIDs.end(), summon->GetGUID()) != realmMinionGUIDs.end())
                    realmMinionGUIDs.erase(std::find(realmMinionGUIDs.begin(), realmMinionGUIDs.end(), summon->GetGUID()));

                if (realmMinionGUIDs.empty())
                    me->RemoveAurasDueToSpell(SPELL_SHAARJS_PROTECTION);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_GARROSH_PHASE_SECOND:
                        if (Creature* YshaarjHear = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_HEART_OF_YSHAARJ)))
                            YshaarjHear->AI()->DoAction(ACTION_GARROSH_ALT_PHASE);

                        me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, 101441);
                        Talk(TALK_PHASE_2_TRANSFORM);
                        sPhase = false;
                        phase = PHASE_SECOND;
                        me->HandleEmoteStateCommand(EMOTE_STATE_READY2H);
                        me->RemoveChanneledCast(targetGUID);
                        me->SetPowerType(POWER_ENERGY);
                        me->SetMaxPower(POWER_ENERGY, 100);
                        me->SetPower(POWER_ENERGY, 0);
                        DoCast(me, SPELL_NON_REGENERATE_POWER, true);
                        
                        if (Is25ManRaid() && IsHeroic())
                            me->SetHealth(1757632000); // temp
                        else
                            me->SetHealth(me->GetMaxHealth());

                        me->SetAIAnimKitId(0);
                        break;
                    case ACTION_GARROSH_ALT_PHASE:
                        inRealm = true;
                        RealmEvents.CancelEvent(EVENT_EVADE_CHECK);
                        me->InterruptNonMeleeSpells(false, SPELL_EMPOWERED_WHIRLING_CORRUPTION);
                        me->InterruptNonMeleeSpells(false, SPELL_WHIRLING_CORRUPTION);
                        realmMinionGUIDs.clear();
                        events.ScheduleEvent(EVENT_ENTER_ANOTHER_REALM, 2.5 * IN_MILLISECONDS);
                        break;
                    case ACTION_REMOVE_YSHAARJ_PROTECTION:
                        me->SetAIAnimKitId(0);
                        me->RemoveAurasDueToSpell(SPELL_ABSORB_ENERGY);

                        if (IsHeroic())
                        {
                            if (Creature* nearHeart = me->FindNearestCreature(NPC_HEART_OF_YSHAARJ_REALM, 100.0f, true))
                                nearHeart->RemoveAurasDueToSpell(SPELL_CRUSHING_FEAR_LAUNCHER);
                        }

                        if (GetData(TYPE_GARROSH_REALM))
                            RealmEvents.ScheduleEvent(EVENT_ANNIHILATE, 5 * IN_MILLISECONDS);
                        break;
                    case ACTION_GARROSH_PHASE_THIRD:
                        if (Creature* YshaarjHear = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_HEART_OF_YSHAARJ)))
                            YshaarjHear->AI()->DoAction(ACTION_GARROSH_PHASE_THIRD);

                        me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, 101441);
                        DoCast(me, SPELL_PHASE_THIRD_TRANSFORM, true);
                        sPhase = false;
                        phase = PHASE_THIRD;
                        me->SetDisableGravity(false);
                        me->HandleEmoteStateCommand(EMOTE_STATE_READY2H);

                        RealmEvents.ScheduleEvent(EVENT_RETURN, 7000);
                        break;
                    case ACTION_LAUNCH_IRON_STAR:
                        Talk(TALK_IRON_STAR_LAUNCH_ANN);
                        clumpCheckDone = true;
                        DoCast(me, SPELL_FIRE_IRON_STAR, true);
                        break;
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case 0:
                        Talk(TALK_SWITCH_PHASE_2);
                        me->SetAIAnimKitId(ANIM_AI_KIT_AROUND_HEART_TALK);

                        scheduler
                            .Schedule(Seconds(5), [this](TaskContext context)
                        {
                            me->SetAIAnimKitId(0);
                            DoCast(me, SPELL_THROW_AXE_AT_HEART);
                            me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, 0);
                        });

                        scheduler
                            .Schedule(Seconds(7), [this](TaskContext context)
                        {
                            DoCast(me, SPELL_VISUAL_FLEE_SECOND_PHASE, true);
                            me->SetAIAnimKitId(ANIM_AI_KIT_TRANSITION);

                            if (Creature* HeartOfYshaarj = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_HEART_OF_YSHAARJ)))
                                HeartOfYshaarj->CastSpell(HeartOfYshaarj, SPELL_TRANSITION_VISUAL, false);
                        });
                        break;
                    case 2:
                        Talk(TALK_SWITCH_PHASE_3);
                        me->SetAIAnimKitId(ANIM_AI_KIT_AROUND_HEART_TALK);

                        scheduler
                            .Schedule(Seconds(5), [this](TaskContext context)
                        {
                            DoCast(me, SPELL_REALM_OF_YSHAARJ_REMOVE);
                            me->SetAIAnimKitId(0);
                            me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, 0);
                        });

                        scheduler
                            .Schedule(Seconds(7), [this](TaskContext context)
                        {
                            DoCast(me, SPELL_VISUAL_FLEE_THIRD_PHASE, true);
                            me->SetAIAnimKitId(ANIM_AI_KIT_TRANSITION);

                            if (Creature* HeartOfYshaarj = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_HEART_OF_YSHAARJ)))
                                HeartOfYshaarj->CastSpell(HeartOfYshaarj, SPELL_TANSITION_VISUAL_THIRD, false);
                        });

                        RealmEvents.ScheduleEvent(EVENT_TRANSFORM_PHASE_THIRD, 2.5 * IN_MILLISECONDS);
                        break;
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() == NPC_MINION_OF_YSHAARJ)
                {
                    summon->SetReactState(REACT_AGGRESSIVE);
                    summon->SetInCombatWithZone();
                    summon->CastSpell(summon, SPELL_EMPOWERED_PROC, true);
                }

                if (summon->GetEntry() == NPC_MANIFESTATION_OF_RAGE)
                    summon->SetInCombatWithZone();
            }

            PlayerAI* GetAIForCharmedPlayer(Player* player) override
            {
                return new TouchYshaarjAI(player);
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);
                RealmEvents.Update(diff);

                while (uint32 eventId = RealmEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_LEAVE_ANOTHER_REALM:
                            inRealm = false;

                            for (auto&& itr : instance->instance->GetPlayers())
                            {
                                if (Player* target = itr.GetSource())
                                {
                                    target->CastSpell(target, SPELL_ZEN, true);
                                    target->UnsummonPetTemporaryIfAny();
                                }
                            }

                            me->RemoveAurasDueToSpell(SPELL_SHAARJS_PROTECTION);
                            me->InterruptNonMeleeSpells(true);
                            RealmEvents.Reset();
                            currentEmboded = 0;

                            if (Creature* HeartOfYshaarj = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_HEART_OF_YSHAARJ)))
                                me->NearTeleportTo(HeartOfYshaarj->GetPositionX(), HeartOfYshaarj->GetPositionY(), -317.39f, HeartOfYshaarj->GetOrientation());

                            RealmEvents.ScheduleEvent(EVENT_RETURN, 2 * IN_MILLISECONDS);
                            break;
                        case EVENT_ANNIHILATE:
                        {
                            if (!GetData(TYPE_GARROSH_REALM)) // check twice
                                break;

                            float sOri = frand(0, 2 * M_PI);
                            me->PrepareChanneledCast(sOri, SPELL_ANNIHILATE);
                            RealmEvents.ScheduleEvent(EVENT_ANNIHILATE, 3 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_RETURN:
                        {
                            me->CastSpell(garroshJumpPos.GetPositionX(), garroshJumpPos.GetPositionY(), garroshJumpPos.GetPositionZ(), SPELL_JUMP_TO_GROUND, false);

                            for (auto&& itr : instance->instance->GetPlayers())
                                if (Player* target = itr.GetSource())
                                    target->ResummonPetTemporaryUnSummonedIfAny();

                            // Set Active to Korkron Warriors in main phase
                            std::list<Creature*> korkronsList;
                            GetCreatureListWithEntryInGrid(korkronsList, me, NPC_WARBRINGER_KORKRON, 300.0f);
                            GetCreatureListWithEntryInGrid(korkronsList, me, NPC_WOLF_RIDER_FARSEER, 300.0f);
                            GetCreatureListWithEntryInGrid(korkronsList, me, NPC_MINION_OF_YSHAARJ, 300.0f);

                            korkronsList.remove_if([=](Creature* target) { return target && !target->IsAlive(); });

                            for (auto&& itr : korkronsList)
                                itr->RemoveChanneledCast(targetGUID);

                            if (sPhase) // we already switch to transition
                                break;

                            me->RemoveChanneledCast(targetGUID);
                            me->SetAIAnimKitId(0);
                            events.ScheduleEvent(EVENT_WHIRLWIND, me->HasAura(SPELL_TRANSITION_THIRD_EFFECT) ? 27 * IN_MILLISECONDS : 30 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_TOUCH, me->HasAura(SPELL_TRANSITION_THIRD_EFFECT) ? 13 * IN_MILLISECONDS : 15 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_DESECRATED, me->HasAura(SPELL_TRANSITION_THIRD_EFFECT) ? 3 * IN_MILLISECONDS : 10 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_GRIPPING, me->HasAura(SPELL_TRANSITION_THIRD_EFFECT) ? 5 * IN_MILLISECONDS : 12.5 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_EVADE_CHECK:
                            if (GetData(TYPE_GARROSH_REALM)) // in realm
                            {
                                if (instance && instance->IsWipe(300.0f, me))
                                {
                                    EnterEvadeMode();
                                    break;
                                }
                            }
                            else if (me->GetPositionZ() > -309.0f) // in main phase
                            {
                                EnterEvadeMode();
                                break;
                            }

                            RealmEvents.ScheduleEvent(EVENT_EVADE_CHECK, 1 * IN_MILLISECONDS);
                            break;
                        case EVENT_TRANSFORM_PHASE_THIRD:
                            if (Creature* YshaarjHear = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_HEART_OF_YSHAARJ)))
                                YshaarjHear->CastSpell(YshaarjHear, SPELL_TRANSITION_VISUAL, false);
                            break;
                        case EVENT_PREPUSH_4TD:
                            Talk(TALK_PHASE_4_PUSH);
                            break;
                        case EVENT_PHASE_FOUR:
                        {
                            inRealm = true;
                            RealmEvents.CancelEvent(EVENT_EVADE_CHECK);
                            DoCast(me, SPELL_ENTER_REALM_OF_GARROSH);

                            // Set alive minions to inactive state
                            std::list<Creature*> minionsList;
                            GetCreatureListWithEntryInGrid(minionsList, me, NPC_MINION_OF_YSHAARJ, 300.0f);

                            minionsList.remove_if([=](Creature* target) { return target && !target->IsAlive(); });

                            for (auto&& itr : minionsList)
                                itr->PrepareChanneledCast(itr->GetOrientation());

                            scheduler
                                .Schedule(Milliseconds(2750), [this](TaskContext context)
                            {
                                me->SetPower(POWER_ENERGY, 0);
                                me->RemoveAurasDueToSpell(SPELL_WHIRLWIND_EMPOWERED);
                                me->RemoveAurasDueToSpell(SPELL_TOUCH_EMPOWERED);
                                me->RemoveAurasDueToSpell(SPELL_DESECRATED_EMPOWERED);
                                me->RemoveAurasDueToSpell(SPELL_GRIPPING_EMPOWERED);

                                me->RemoveAllAreasTrigger();
                                DoCast(me, SPELL_PHASE_FOUR_TRANSFORM);
                                me->NearTeleportTo(myworldGarrosh.GetPositionX(), myworldGarrosh.GetPositionY(), myworldGarrosh.GetPositionZ(), myworldGarrosh.GetOrientation());

                                scheduler
                                    .Schedule(Milliseconds(17250), [this](TaskContext context)
                                {
                                    lastPhaseTriggered = true;
                                    RealmEvents.ScheduleEvent(EVENT_EVADE_CHECK, 1 * IN_MILLISECONDS);
                                    me->RemoveChanneledCast(targetGUID);
                                    events.ScheduleEvent(EVENT_MALICE, 9 * IN_MILLISECONDS);
                                    events.ScheduleEvent(EVENT_BOMBARDMENT, 50 * IN_MILLISECONDS);
                                    RealmEvents.ScheduleEvent(EVENT_GAIN_POWER, 1 * IN_MILLISECONDS);

                                });
                            });
                            break;
                        }
                        case EVENT_GAIN_POWER:
                            uint32 currentPower = me->GetPower(POWER_ENERGY);

                            if (++currentPower > 99)
                            {
                                me->SetPower(POWER_ENERGY, 100);
                                events.ScheduleEvent(EVENT_MANIFEST_RAGE, 0.5 * IN_MILLISECONDS);
                                RealmEvents.ScheduleEvent(EVENT_GAIN_POWER, 8 * IN_MILLISECONDS);
                                break;
                            }
                            else
                                me->SetPower(POWER_ENERGY, currentPower);

                            RealmEvents.ScheduleEvent(EVENT_GAIN_POWER, getPowerGainTimer(powerGainCount));
                            break;
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(SPELL_WHIRLING_CORRUPTION) || me->HasAura(SPELL_EMPOWERED_WHIRLING_CORRUPTION))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DESECRATED:
                            DoCast(me, SPELL_DESECRATE_SELECTOR);
                            events.ScheduleEvent(EVENT_DESECRATED, getDesecrateTimer());
                            break;
                        case EVENT_WARSONG:
                            Talk(TALK_WARSONG);
                            DoCast(me, SPELL_HELLSCREAMS_WARSONG);
                            events.ScheduleEvent(EVENT_WARSONG, 42.2 * IN_MILLISECONDS);
                            break;
                        case EVENT_ENGINEERS:
                            Talk(TALK_SIEGE_ENGINEERS);
                            HandleSummonGuards(GUARD_TYPE_ENGINEER);
                            events.ScheduleEvent(EVENT_ENGINEERS, urand(45 * IN_MILLISECONDS, 49 * IN_MILLISECONDS));
                            break;
                        case EVENT_WOLF_RIDER:
                            Talk(TALK_FARSEERS);
                            HandleSummonGuards(GUARD_TYPE_WOLF_RIDER);
                            events.ScheduleEvent(EVENT_WOLF_RIDER, 50 * IN_MILLISECONDS);
                            break;
                        case EVENT_WARBRINGER:
                            HandleSummonGuards(GUARD_TYPE_WARBRINGER);
                            events.ScheduleEvent(EVENT_WARBRINGER, urand(47 * IN_MILLISECONDS, 49 * IN_MILLISECONDS));
                            break;
                        case EVENT_ENTER_ANOTHER_REALM:
                            Talk(TALK_REALM);
                            me->InterruptNonMeleeSpells(true);
                            me->PrepareChanneledCast(me->GetOrientation());
                            events.Reset();

                            if (Creature* YshaarjHear = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_HEART_OF_YSHAARJ)))
                            {
                                uint32 realmData = YshaarjHear->AI()->GetData(TYPE_GARROSH_REALM);
                                uint32 transfertSpell = invGarroshRealmsType.find(realmData)->second[0];
                                uint32 realmId = invGarroshRealmsType.find(realmData)->second[1];

                                // Set Passive to Korkron Warriors until we`ll return in main phase
                                std::list<Creature*> korkronsList;
                                GetCreatureListWithEntryInGrid(korkronsList, me, NPC_WARBRINGER_KORKRON, 300.0f);
                                GetCreatureListWithEntryInGrid(korkronsList, me, NPC_WOLF_RIDER_FARSEER, 300.0f);
                                GetCreatureListWithEntryInGrid(korkronsList, me, NPC_MINION_OF_YSHAARJ, 300.0f);

                                korkronsList.remove_if([=](Creature* target) { return target && !target->IsAlive(); });

                                for (auto&& itr : korkronsList)
                                    itr->PrepareChanneledCast(itr->GetOrientation());

                                DoCast(me, transfertSpell, true);
                                me->PrepareChanneledCast(me->GetOrientation());

                                // Spawn Minions there
                                HandleSummonGuards(invGarroshRealmsType.find(realmData)->second[2]);

                                for (auto&& itr : instance->instance->GetPlayers())
                                    if (Player* target = itr.GetSource())
                                        target->UnsummonPetTemporaryIfAny();

                                me->NearTeleportTo(RealmWorldEGarroshPos[realmId].GetPositionX(), RealmWorldEGarroshPos[realmId].GetPositionY(), RealmWorldEGarroshPos[realmId].GetPositionZ(), RealmWorldEGarroshPos[realmId].GetOrientation());

                                scheduler
                                    .Schedule(Seconds(1), [this](TaskContext context)
                                {
                                    if (Creature* nearHeart = me->FindNearestCreature(NPC_HEART_OF_YSHAARJ_REALM, 100.0f, true))
                                        nearHeart->AI()->DoAction(0);

                                    DoCast(me, SPELL_REALM_OF_YSHAARJ, true);
                                    RealmEvents.ScheduleEvent(EVENT_LEAVE_ANOTHER_REALM, 60500);
                                });

                                scheduler
                                    .Schedule(Seconds(2), [this](TaskContext context)
                                {
                                    DoCast(me, SPELL_SHAARJS_PROTECTION, true);
                                    me->SetAIAnimKitId(ANIM_AI_KIT_TRANSITION);
                                    RealmEvents.ScheduleEvent(EVENT_EVADE_CHECK, 1 * IN_MILLISECONDS);

                                    for (auto&& itr : instance->instance->GetPlayers())
                                        if (Player* target = itr.GetSource())
                                            target->ResummonPetTemporaryUnSummonedIfAny();

                                    if (Creature* YshaarjHear = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_HEART_OF_YSHAARJ)))
                                    {
                                        uint32 realmData = YshaarjHear->AI()->GetData(TYPE_GARROSH_REALM);
                                        auto key = invGarroshRealmsType.find(realmData);

                                        if (key->second[2] == NPC_EMBODIED_FEAR)
                                        {
                                            if (IsHeroic())
                                                if (Creature* nearHeart = me->FindNearestCreature(NPC_HEART_OF_YSHAARJ_REALM, 100.0f, true))
                                                    nearHeart->CastSpell(nearHeart, SPELL_CRUSHING_FEAR_LAUNCHER, true);

                                            DoCast(me, SPELL_CONSUMED_COURAGE, true);
                                        }
                                        else if (key->second[2] == NPC_EMBODIED_DOUBT)
                                            DoCast(me, SPELL_CONSUMED_FAITH, true);
                                    }
                                });
                            }
                            break;
                        case EVENT_WHIRLWIND:
                            events.RescheduleEvent(EVENT_GRIPPING, 9.5 * IN_MILLISECONDS);
                            Talk(me->HasAura(SPELL_WHIRLWIND_EMPOWERED) ? TALK_WHIRLWIND_ANN : TALK_WHIRLWIND_EMP_ANN);
                            Talk(me->HasAura(SPELL_WHIRLWIND_EMPOWERED) ? TALK_WHIRLWIND : TALK_WHIRLWIND_EMP);
                            DoCast(me, me->HasAura(SPELL_WHIRLWIND_EMPOWERED) ? SPELL_EMPOWERED_WHIRLING_CORRUPTION : SPELL_WHIRLING_CORRUPTION);
                            events.ScheduleEvent(EVENT_WHIRLWIND, 49.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_TOUCH:
                            DoCast(me, me->HasAura(SPELL_TOUCH_EMPOWERED) ? SPELL_EMPOWERED_TOUCH_OF_YSHAARJ : SPELL_TOUCH_OH_YSHAARJ);
                            events.ScheduleEvent(EVENT_TOUCH, getTouchTimer());
                            break;
                        case EVENT_GRIPPING:
                            if (Unit* vict = me->GetVictim())
                                DoCast(vict, me->HasAura(SPELL_GRIPPING_EMPOWERED) ? SPELL_EMPOWERED_GRIPPING_DESPAIR : SPELL_GRIPPING_DESPAIR);

                            events.ScheduleEvent(EVENT_GRIPPING, 3.2 * IN_MILLISECONDS);
                            break;
                        case EVENT_MALICE:
                            DoCast(me, SPELL_MALICE);
                            events.ScheduleEvent(EVENT_MALICE, 29.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_BOMBARDMENT:
                            bombardCount++;
                            Talk(TALK_BOMBARDMENT);
                            Talk(TALK_BOMBARDMENT_ANN);
                            clumpCheckDone = false;
                            DoCast(me, SPELL_CALL_BOMBARDMENT);
                            events.ScheduleEvent(EVENT_BOMBARDMENT, getBombardTimer());
                            break;
                        case EVENT_MANIFEST_RAGE:
                            DoCast(me, SPELL_MANIFEST_RAGE);
                            break;
                    }
                }

                if (!inRealm || inRealm && phase == PHASE_MY_WORLD)
                    DoMeleeAttackIfReady();
            }

            private:
                void HandleSummonGuards(uint32 type)
                {
                    switch (type)
                    {
                        case GUARD_TYPE_WARBRINGER:
                            for (uint8 i = 0; i < 6; i++)
                                me->SummonCreature(NPC_WARBRINGER_KORKRON, WarbringersSpawnPos[i], TEMPSUMMON_MANUAL_DESPAWN);

                            if (GameObject* westGates = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GO_GARROSH_SOUTH_WEST_DOOR):ObjectGuid::Empty))
                                instance->HandleGameObject(westGates->GetGUID(), true, NULL);

                            if (GameObject* eastGates = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GO_GARROSH_SOUTH_EAST_DOOR) : ObjectGuid::Empty))
                                instance->HandleGameObject(eastGates->GetGUID(), true, NULL);
                            break;
                        case GUARD_TYPE_WOLF_RIDER:
                            me->SummonCreature(NPC_WOLF_RIDER_FARSEER, WolfridersSpawnPos[wRiderGates ? 0 : 1], TEMPSUMMON_MANUAL_DESPAWN);

                            if (GameObject* sideGates = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(wRiderGates ? GO_GARROSH_SOUTH_EAST_DOOR : GO_GARROSH_SOUTH_WEST_DOOR) : ObjectGuid::Empty))
                                instance->HandleGameObject(sideGates->GetGUID(), true, NULL);

                            wRiderGates = wRiderGates ? false : true;
                            break;
                        case GUARD_TYPE_ENGINEER:
                            for (uint8 i = 0; i < 2; i++)
                                me->SummonCreature(NPC_SIEGE_ENGINEER, SiegeEngineersSpawnPos[i], TEMPSUMMON_MANUAL_DESPAWN);

                            for (uint8 i = 0; i < 2; i++)
                                me->SummonCreature(NPC_IRON_STAR, IronStarSpawnPos[i], TEMPSUMMON_MANUAL_DESPAWN);
                            break;
                        case NPC_EMBODIED_DESPAIR_G:
                            for (auto&& itr : embodiedDespairSpawnPos)
                            {
                                if (Creature* despair = me->SummonCreature(type, itr, TEMPSUMMON_MANUAL_DESPAWN))
                                    realmMinionGUIDs.push_back(despair->GetGUID());
                            }
                            break;
                        case NPC_EMBODIED_FEAR:
                            for (auto&& itr : embodiedFearSpawnPos)
                            {
                                for (uint8 i = 0; i < 4; i++)
                                {
                                    if (Creature* fear = me->SummonCreature(type, itr.GetPositionX() + frand(-7.0f, 7.0f), itr.GetPositionY() + frand(-7.0f, 7.0f), itr.GetPositionZ(), itr.GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                                        realmMinionGUIDs.push_back(fear->GetGUID());

                                }
                            }
                            break;
                        case NPC_EMBODIED_DOUBT:
                            for (auto&& itr : embodiedDoubtSpawnPos)
                            {
                                for (uint8 i = 0; i < 3; i++)
                                {
                                    if (Creature* doubt = me->SummonCreature(type, itr.GetPositionX() + frand(-5.0f, 5.0f), itr.GetPositionY() + frand(-5.0f, 5.0f), itr.GetPositionZ(), itr.GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                                        realmMinionGUIDs.push_back(doubt->GetGUID());
                                }
                            }
                            break;
                    }
                }

                uint32 getBombardTimer()
                {
                    if (bombardCount > 3)
                        return 25 * IN_MILLISECONDS;

                    switch (bombardCount)
                    {
                        case 1:
                            return 55 * IN_MILLISECONDS;
                        case 2:
                        case 3:
                            return 40 * IN_MILLISECONDS;
                    }

                    return 0;
                }

                uint32 getDesecrateTimer()
                {
                    switch (phase)
                    {
                        case PHASE_FIRST:
                            return 41 * IN_MILLISECONDS;
                        case PHASE_SECOND:
                            return 35 * IN_MILLISECONDS;
                        case PHASE_THIRD:
                            return 25 * IN_MILLISECONDS;
                    }

                    return 35 * IN_MILLISECONDS; // by default
                }

                uint32 getTouchTimer()
                {
                    switch (phase)
                    {
                        case PHASE_SECOND:
                            return 45 * IN_MILLISECONDS;
                        case PHASE_THIRD:
                            return ++touchCount > 1 ? 42 * IN_MILLISECONDS : 34.5 * IN_MILLISECONDS;
                    }

                    return 45 * IN_MILLISECONDS;
                }

                uint32 getPowerGainTimer(uint32 &powerCount)
                {
                    if (++powerCount % 4 == 0)
                    {
                        powerCount = 0;
                        return 1.3 * IN_MILLISECONDS;
                    }

                    return 0.8 * IN_MILLISECONDS;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_garrosh_hellscreamAI(creature);
        }
};

struct soo_garrosh_guards_typeAI : public ScriptedAI
{
    soo_garrosh_guards_typeAI(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    InstanceScript* instance;
    EventMap events;
    ObjectGuid myStarGUID;
    ObjectGuid warbringerFixateGUID;
    float x, y;
    bool hasAffectCombat; // this creatures should move to same pos and affect by combat only after reach his pos. but in case damageTaken - stop moving and instant affect by combat

    void Reset() override
    {
        warbringerFixateGUID = ObjectGuid::Empty;
        myStarGUID = ObjectGuid::Empty;
        x = 0.0f; y = 0.0f;
        instance = me->GetInstanceScript();

        GetPositionWithDistInOrientation(me, me->GetEntry() == NPC_SIEGE_ENGINEER ? 14.3f : 45.0f, me->GetEntry() == NPC_SIEGE_ENGINEER ? me->GetOrientation() : GarroshGuardsOri, x, y);

        Movement::MoveSplineInit init(me);
        init.MoveTo(x, y, me->GetPositionZ());

        if (!IsHeroic() && me->GetEntry() == NPC_WOLF_RIDER_FARSEER)
            init.SetVelocity(6.7f);

        init.SetSmooth();
        init.Launch();

        if (me->GetEntry() == NPC_SIEGE_ENGINEER) // only for engineer
        {
            me->SetInCombatWithZone();

            if (instance)
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        }

        scheduler
            .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
        {
            if (!hasAffectCombat && me->GetEntry() != NPC_SIEGE_ENGINEER)
            {
                me->StopMoving();

                Movement::MoveSplineInit init(me);
                if (me->GetExactDist2d(&warbringerLastPosition[0]) > me->GetExactDist2d(&warbringerLastPosition[1]))
                    init.MoveTo(warbringerLastPosition[1].GetPositionX(), warbringerLastPosition[1].GetPositionY(), warbringerLastPosition[1].GetPositionZ());
                else
                    init.MoveTo(warbringerLastPosition[0].GetPositionX(), warbringerLastPosition[0].GetPositionY(), warbringerLastPosition[0].GetPositionZ());

                init.SetSmooth();
                init.Launch();

                scheduler
                    .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                {
                    if (!hasAffectCombat)
                    {
                        hasAffectCombat = true;
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                        me->SetInCombatWithZone();
                    }
                });
            }
            else if (me->GetEntry() == NPC_SIEGE_ENGINEER)
            {
                if (Creature* iStar = me->FindNearestCreature(NPC_IRON_STAR, 35.0f, true))
                {
                    myStarGUID = iStar->GetGUID();
                    me->StopMoving();
                    me->SetFacingTo(me->GetAngle(iStar));
                }

                DoCast(me, SPELL_POWER_IRON_STAR);
            }
        });
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (!hasAffectCombat && me->GetEntry() != NPC_KORKRON_SIEGEMASTER)
        {
            hasAffectCombat = true;
            me->StopMoving();
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
            me->SetInCombatWithZone();

            // Set near warbringers in combat too
            std::list<Creature*> warbringersList;
            GetCreatureListWithEntryInGrid(warbringersList, me, me->GetEntry(), 15.0f);

            for (auto&& itr : warbringersList)
                itr->AI()->DoAction(ACTION_START_INTRO);
        }
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_START_INTRO:
                if (hasAffectCombat)
                    break;

                hasAffectCombat = true;
                me->StopMoving();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                me->SetInCombatWithZone();
                break;
            case ACTION_GARROSH_EVADE:
                me->DespawnOrUnsummon();
                break;
        }
    }
};

// Siege Engineer 71984
struct npc_siege_engineer : public soo_garrosh_guards_typeAI
{
    npc_siege_engineer(Creature* creature) : soo_garrosh_guards_typeAI(creature) 
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (instance)
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
    }

    void Unsummoned() override
    {
        if (instance)
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Kor`kron Warbringer 71979
struct npc_korkron_warbringer : public soo_garrosh_guards_typeAI
{
    npc_korkron_warbringer(Creature* creature) : soo_garrosh_guards_typeAI(creature) 
    {
        hasAffectCombat = false;
        me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);

        if (IsHeroic())
            DoCast(me, SPELL_BLOOD_FRENZIED);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        if (IsHeroic())
            DoCast(me, SPELL_BLOOD_FRENZIED_SELECTOR);

        events.ScheduleEvent(EVENT_HAMSTRING, urand(2 * IN_MILLISECONDS, 5.5 *IN_MILLISECONDS));
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        warbringerFixateGUID = guid;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return warbringerFixateGUID;
    }

    void JustDied(Unit* killer) override
    {
        if (killer->GetGUID() != me->GetGUID())
            return;

        if (Creature* garrosh = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GARROSH_HELLSCREAM) : ObjectGuid::Empty))
            garrosh->AI()->SetData(TYPE_GARROSH_STRIKE_COUNT, 1);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_HAMSTRING)
            {
                if (Unit* vict = me->GetVictim())
                    DoCast(vict, SPELL_HAMSTRING);

                events.ScheduleEvent(EVENT_HAMSTRING, urand(6 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Wolf Rider Farseer 71983
struct npc_wolf_rider_farseer : public soo_garrosh_guards_typeAI
{
    npc_wolf_rider_farseer(Creature* creature) : soo_garrosh_guards_typeAI(creature) 
    { 
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        hasAffectCombat = false;
        me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(4 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_CHAIN_HEAL, urand(6.5 * IN_MILLISECONDS, 19.5 *IN_MILLISECONDS));
    }

    void CastInterrupted(SpellInfo const* spell) override
    {
        DoCast(me, SPELL_FURY, true);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasReactState(REACT_PASSIVE))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CHAIN_HEAL:
                    DoCast(me, SPELL_ANCENTRAL_CHAIN_HEAL);
                    events.ScheduleEvent(EVENT_CHAIN_HEAL, urand(7 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                    break;
                case EVENT_CHAIN_LIGHTNING:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                        DoCast(target, SPELL_CHAIN_LIGHTNING);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                        DoCast(target, SPELL_CHAIN_LIGHTNING);

                    events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(13.5 * IN_MILLISECONDS, 27 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Desecrated Weapon 72154, 72198
struct npc_soo_desecrated_weapon : public ScriptedAI
{
    npc_soo_desecrated_weapon(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->SetInCombatWithZone();
        DoCast(me, me->GetEntry() == NPC_DESECRATED_WEAPON_EMPOWERED ? SPELL_EMPOWERED_DESECRATED_WEAPON_AT : SPELL_DESECRATED_WEAPON_AREATRIGGER, true);
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->RemoveAllAreasTrigger();
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (me->GetEntry() == NPC_DESECRATED_WEAPON_EMPOWERED && damage >= me->GetHealth())
            damage = 0;

        if (AreaTrigger* myWeapon = me->GetAreaTrigger(me->GetEntry() == NPC_DESECRATED_WEAPON_EMPOWERED ? SPELL_EMPOWERED_DESECRATED_WEAPON_AT : SPELL_DESECRATED_WEAPON_AREATRIGGER))
        {
            float radius = (me->GetHealthPct() * 25.0f) / 100.0f;
            myWeapon->SetFloatValue(AREATRIGGER_FIELD_EXPLICIT_SCALE, (radius * 1.0f) / 25.0f);
            myWeapon->SetVisualRadius(radius);
            myWeapon->UpdateObjectVisibility();

            // Update new Radius Value
            myWeapon->SetRadius(radius);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Iron Star 71985
struct npc_soo_iron_star : public ScriptedAI
{
    npc_soo_iron_star(Creature* creature) : ScriptedAI(creature) { }

    EventMap nonCombatEvents;
    float x, y;

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_POWER_IRON_STAR)
        {
            me->SetInCombatWithZone();
            DoCast(me, SPELL_IRON_STAR_VISUAL);

            x = 0.0f; y = 0.0f;

            GetPositionWithDistInOrientation(me, 195.0f, me->GetOrientation(), x, y);

            Movement::MoveSplineInit init(me);
            init.MoveTo(x, y, me->GetPositionZ());
            init.Launch();
            nonCombatEvents.ScheduleEvent(EVENT_COLLISION, me->GetSplineDuration());
        }
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_COLLISION)
            {
                DoCast(me, SPELL_EXPLODING_IRON_STAR);
                me->Kill(me);
            }
            break;
        }
    }
};

// Embodied Minions 72236, 72237, 72238
struct npc_soo_embodied_minions : public ScriptedAI
{
    npc_soo_embodied_minions(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    InstanceScript* instance;

    void Reset() override
    {
        events.Reset();
        DoCast(me, SPELL_COSMETIC_CHANNEL);

        if (me->GetEntry() == NPC_EMBODIED_DOUBT || me->GetEntry() == NPC_EMBODIED_DESPAIR_G)
        {
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            SetCombatMovement(false);
        }

        instance = me->GetInstanceScript();
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetEntry() != NPC_EMBODIED_DESPAIR_G)
            return;

        if (!IsHeroic())
            return;

        std::list<Creature*> despairList;
        GetCreatureListWithEntryInGrid(despairList, me, me->GetEntry(), 100.0f);
        despairList.remove_if([=](Creature* target) { return target->GetGUID() == me->GetGUID(); });

        for (auto&& despair : despairList)
            despair->CastSpell(despair, SPELL_ULTIMATE_DESPAIR, true);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->InterruptNonMeleeSpells(false, SPELL_COSMETIC_CHANNEL);

        switch (me->GetEntry())
        {
            case NPC_EMBODIED_DOUBT:
                if (IsHeroic())
                    events.ScheduleEvent(EVENT_EMBODIED_DOUBT, urand(4 * IN_MILLISECONDS, 9.5*IN_MILLISECONDS));
                break;
            case NPC_EMBODIED_DESPAIR_G:
                events.ScheduleEvent(EVENT_EMBODIED_DESPAIR, urand(2.5 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_EMBODIED_DOUBT:
                    DoCastAOE(SPELL_EMBODIED_DOUBT);
                    events.ScheduleEvent(EVENT_EMBODIED_DOUBT, urand(4 * IN_MILLISECONDS, 9.5 * IN_MILLISECONDS));
                    break;
                case EVENT_EMBODIED_DESPAIR:
                    DoCast(me, SPELL_EMBODIED_DESPAIR);
                    events.ScheduleEvent(EVENT_EMBODIED_DESPAIR, 9 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Heart of Yshaarj 72215
struct npc_soo_heart_of_yshaarj : public ScriptedAI
{
    npc_soo_heart_of_yshaarj(Creature* creature) : ScriptedAI(creature) { }

    EventMap nonCombatEvents;
    InstanceScript* instance;
    float x, y;
    uint32 prevRealm;

    void Reset() override
    {
        prevRealm = 0;
        nonCombatEvents.Reset();
        instance = me->GetInstanceScript();
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_GARROSH_ALT_PHASE:
                me->RemoveAurasDueToSpell(SPELL_TRANSITION_VISUAL);
                nonCombatEvents.ScheduleEvent(EVENT_ALT_WORLD, 15 * IN_MILLISECONDS);
                break;
            case ACTION_GARROSH_PHASE_THIRD:
                me->RemoveAurasDueToSpell(SPELL_TRANSITION_VISUAL);
                break;
        }
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        damage = 0;
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_GARROSH_REALM)
            return prevRealm;

        return 0;
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_ALT_WORLD)
            {
                DoCast(me, SPELL_ENTER_REALM_OF_YSHAARJ);

                std::vector<uint32> RealmComboStorage = { REALM_TEMPLE_OF_THE_JADE_SERPENT, REALM_TERRACE_OF_THE_ENDLESS_SPRING, REALM_TEMPLE_OF_THE_RED_CRANE };

                if (!IsHeroic())
                {
                    // Remove From Temp container value, that was used before (real random)
                    if (prevRealm)
                        RealmComboStorage.erase(std::find(RealmComboStorage.begin(), RealmComboStorage.end(), prevRealm));

                    // Select new spell from updated container
                    prevRealm = Trinity::Containers::SelectRandomContainerElement(RealmComboStorage);
                }
                else // Fixated
                {
                    switch (prevRealm)
                    {
                        case 0:
                        case REALM_TEMPLE_OF_THE_RED_CRANE:
                            prevRealm = REALM_TEMPLE_OF_THE_JADE_SERPENT;
                            break;
                        case REALM_TEMPLE_OF_THE_JADE_SERPENT:
                            prevRealm = REALM_TERRACE_OF_THE_ENDLESS_SPRING;
                            break;
                        case REALM_TERRACE_OF_THE_ENDLESS_SPRING:
                            prevRealm = REALM_TEMPLE_OF_THE_RED_CRANE;
                            break;
                    }
                }

                if (Creature* Garrosh = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_GARROSH_HELLSCREAM) : ObjectGuid::Empty))
                    Garrosh->AI()->DoAction(ACTION_GARROSH_ALT_PHASE);

                nonCombatEvents.ScheduleEvent(EVENT_ALT_WORLD, 3 * MINUTE * IN_MILLISECONDS + 28 * IN_MILLISECONDS); // should be 2.25.5s but 2.5s time fo transition
            }
            break;
        }
    }
};

// Heart of Y`shaarj Realm 72228
struct npc_garrosh_heart_of_yshaarj_realm : public ScriptedAI
{
    npc_garrosh_heart_of_yshaarj_realm(Creature* creature) : ScriptedAI(creature) { }

    void DoAction(int32 actionId) override
    {
        me->SetInCombatWithZone();
        DoCast(me, SPELL_ABSORB_ENERGY);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        damage = 0;
    }
};

// Minions of Y`shaarj 72272
struct npc_garrosh_minion_of_yshaarj : public ScriptedAI
{
    npc_garrosh_minion_of_yshaarj(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        if (IsHeroic())
        {
            me->SetPowerType(POWER_ENERGY);
            me->SetMaxPower(POWER_ENERGY, 100);
            me->SetPower(POWER_ENERGY, 0);
            DoCast(me, SPELL_NON_REGENERATE_POWER, true);
        }
    }

    void DamageDealt(Unit* /*victim*/, uint32& /*damage*/, DamageEffectType damageType) override
    {
        if (IsHeroic() && damageType == DIRECT_DAMAGE) // generate 10 energy by each attack only in HC
        {
            uint32 currentPower = me->GetPower(POWER_ENERGY) + 10;

            if (currentPower > 99)
            {
                me->SetPower(POWER_ENERGY, 0);
                DoCast(me, SPELL_EMPOWERING_CORRUPTION_2);
            }
            else
                me->SetPower(POWER_ENERGY, currentPower);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Unstable Iron Star 73059
struct npc_garrosh_unstable_iron_star : public ScriptedAI
{
    npc_garrosh_unstable_iron_star(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid targetGUID;
    bool hasExplosive;
    bool allowExplosive;

    void Reset() override
    {
        if (Creature* garrosh = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GARROSH_HELLSCREAM) : ObjectGuid::Empty))
            garrosh->AI()->JustSummoned(me);

        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        targetGUID = ObjectGuid::Empty;
        hasExplosive = false;
        allowExplosive = false;
        me->SetInCombatWithZone();
        DoCast(me, SPELL_UNSTABLE_IRON_STAR_AT);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->PrepareChanneledCast(me->GetOrientation());

        scheduler
            .Schedule(Milliseconds(2000), [this](TaskContext context)
        {
            allowExplosive = true;
            DoCast(me, SPELL_IRON_STAR_FIXATE_SELECTOR);
        });
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            me->PrepareChanneledCast(me->GetOrientation());

            scheduler
                .Schedule(Milliseconds(500), [this](TaskContext context)
            {
                if (me->HasAura(SPELL_UNSTABLE_IRON_STAR_AT))
                    DoCast(me, SPELL_IRON_STAR_FIXATE_SELECTOR);
            });
        }
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        targetGUID = guid;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return targetGUID;
    }

    uint32 GetData(uint32 type) const override
    {
        switch (type)
        {
            case TYPE_IRON_STAR_EXPLOSIVE:
                return hasExplosive ? 1 : 0;
            case TYPE_ALLOW_IRON_STAR_EXPLOSIVE:
                return allowExplosive ? 1 : 0;
        }

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_IRON_STAR_EXPLOSIVE)
            hasExplosive = true;
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;
    }
};

// Hellscreams Warsong 144821
class spell_soo_hellscreams_warsong : public SpellScript
{
    PrepareSpellScript(spell_soo_hellscreams_warsong);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_WARBRINGER_KORKRON && target->GetEntry() != NPC_WOLF_RIDER_FARSEER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_hellscreams_warsong::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_hellscreams_warsong::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_hellscreams_warsong::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Power Iron Star 144616
class spell_soo_power_iron_star : public AuraScript
{
    PrepareAuraScript(spell_soo_power_iron_star);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (aurEff->GetBase()->GetDuration() > 0)
            return;

        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_POWER_IRON_STAR);

        if (GetCaster() && GetCaster()->ToCreature())
            GetCaster()->ToCreature()->DespawnOrUnsummon(3 * IN_MILLISECONDS);
    }

    void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (GetOwner() && GetOwner()->ToUnit() && GetOwner()->ToUnit()->GetMap()->IsHeroic())
        {
            SetDuration(10 * IN_MILLISECONDS);

            // For client-side visual
            GetAura()->SetMaxDuration(10 * IN_MILLISECONDS);
            GetAura()->RefreshDuration(false);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_soo_power_iron_star::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectApply += AuraEffectApplyFn(spell_soo_power_iron_star::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Realm of Yshaarj 144954
class spell_garrosh_realm_of_yshaarj : public AuraScript
{
    PrepareAuraScript(spell_garrosh_realm_of_yshaarj);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            owner->CastSpell(owner, SPELL_GARROSH_ENERGY, true);
    }

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Player* owner = GetOwner()->ToPlayer())
        {
            if (Creature* heartOfYshaarj = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(NPC_HEART_OF_YSHAARJ) : ObjectGuid::Empty))
            {
                owner->TeleportTo(owner->GetMapId(), heartOfYshaarj->GetPositionX() + frand(-4.5f, 4.5f), heartOfYshaarj->GetPositionY() + frand(-3.0f, 3.0f), heartOfYshaarj->GetPositionZ(), heartOfYshaarj->GetOrientation(), TELE_TO_NOT_UNSUMMON_PET | TELE_TO_NOT_LEAVE_COMBAT);

                // Required for dbm
                owner->m_Events.Schedule(1000, [=]()
                {
                    owner->CastSpell(garroshJumpPos.GetPositionX() + frand(-5.0f, 5.0f), garroshJumpPos.GetPositionY() + frand(-5.0f, 5.0f), garroshJumpPos.GetPositionZ(), SPELL_JUMP_TO_GROUND, true);
                });
            }

            owner->RemoveAurasDueToSpell(SPELL_COURAGE_AURA);
            owner->RemoveAurasDueToSpell(SPELL_FAITH_AURA);
            owner->RemoveAurasDueToSpell(SPELL_HOPE_AURA);
            owner->RemoveAurasDueToSpell(SPELL_GARROSH_ENERGY);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_garrosh_realm_of_yshaarj::OnAuraEffectApply, EFFECT_1, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_garrosh_realm_of_yshaarj::OnAuraEffectRemove, EFFECT_1, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
    }
};

// Yshaarjs Protection 144945
class spell_soo_yshaarjs_protection_aura : public AuraScript
{
    PrepareAuraScript(spell_soo_yshaarjs_protection_aura);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (InstanceScript* instance = owner->GetInstanceScript())
                instance->SetData(DATA_SHA_VORTEX, DONE);

            if (owner->ToCreature())
                owner->ToCreature()->AI()->DoAction(ACTION_REMOVE_YSHAARJ_PROTECTION);
        }
    }

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (InstanceScript* instance = owner->GetInstanceScript())
                instance->SetData(DATA_SHA_VORTEX, NOT_STARTED);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_soo_yshaarjs_protection_aura::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_DAMAGE_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
        OnEffectApply += AuraEffectApplyFn(spell_soo_yshaarjs_protection_aura::OnAuraEffectApply, EFFECT_0, SPELL_AURA_DAMAGE_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Yshaarjs Protection 144945
class spell_soo_yshaarjs_protection : public SpellScript
{
    PrepareSpellScript(spell_soo_yshaarjs_protection);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        // Not heal at first transition
        if (GetCaster() && GetCaster()->ToCreature() && GetCaster()->ToCreature()->AI()->GetData(TYPE_GARROSH_FIRST_TRANSITION))
        {
            PreventHitEffect(eff_idx);
            GetCaster()->ToCreature()->AI()->SetData(TYPE_GARROSH_FIRST_TRANSITION, 0);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_soo_yshaarjs_protection::HandleEffectHitTarget, EFFECT_2, SPELL_EFFECT_HEAL_PCT);
    }
};

// Whirling Corruption 144985, 145037
class spell_soo_whirling_corruption : public AuraScript
{
    PrepareAuraScript(spell_soo_whirling_corruption);

    bool Load() override
    {
        minionsCount = 0;
        return true;
    }

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        // Doesn`t matter who is target, but not caster
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (Player* pItr = owner->FindNearestPlayer(300.0f))
                owner->CastSpell(pItr, SPELL_WHIRLING_CORRUPTION_VISUAL, true);

            uint32 limitCount = owner->GetMap()->Is25ManRaid() ? 10 : 8;
            if (GetSpellInfo()->Id == SPELL_EMPOWERED_WHIRLING_CORRUPTION && minionsCount < limitCount) // limit is 8(10)
            {
                minionsCount++;

                // no spell selector for these
                std::list<Player*> targets, copyTargets;
                GetPlayerListInGrid(targets, owner, 200.0f);
                copyTargets = targets;

                targets.remove_if(TankSpecTargetSelector()); // allow target melee too

                if (targets.empty())
                {
                    targets.clear();

                    for (auto&& itr : copyTargets)
                        targets.push_back(itr);

                    if (targets.size() > 1)
                        Trinity::Containers::RandomResizeList(targets, 1);

                    if (Player* target = targets.front())
                        owner->CastSpell(target, SPELL_EMPOWERED_WHIRLING_MISSLE, true);

                    return;
                }

                if (targets.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);

                if (Player* target = targets.front())
                    owner->CastSpell(target, SPELL_EMPOWERED_WHIRLING_MISSLE, true);
            }
        }
    }

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->ClearUnitState(UNIT_STATE_CASTING);
    }

    private:
        uint8 minionsCount;

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_soo_whirling_corruption::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        AfterEffectApply += AuraEffectApplyFn(spell_soo_whirling_corruption::OnAuraEffectApply, EFFECT_1, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
    }
};

// Empowered Gripping Despair 145195
class spell_soo_empowered_gripping_despair : public AuraScript
{
    PrepareAuraScript(spell_soo_empowered_gripping_despair);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (owner->GetInstanceScript() && owner->GetInstanceScript()->GetBossState(DATA_GARROSH_HELLSCREAM) != IN_PROGRESS)
                return;

            if (GetCaster() && GetCaster()->ToCreature())
                GetCaster()->ToCreature()->AI()->SetData(TYPE_EXPLOSIVE_DESPAIR, GetStackAmount() + GetCaster()->ToCreature()->AI()->GetData(TYPE_EXPLOSIVE_DESPAIR));

            int32 bp = (int32)((GetStackAmount() * sSpellMgr->GetSpellInfo(GetSpellInfo()->Id, owner->GetMap()->GetDifficulty())->Effects[0].BasePoints) / 2);

            owner->CastCustomSpell(owner, SPELL_EXPLOSIVE_DESPAIR, &bp, nullptr, nullptr, true);

            if (owner->GetInstanceScript())
                owner->GetInstanceScript()->DoRemoveAurasDueToSpellOnPlayers(SPELL_EXPLOSIVE_DESPAIR_AURA);

            owner->CastSpell(owner, SPELL_EXPLOSIVE_DESPAIR_AURA, true);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_soo_empowered_gripping_despair::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Explosive Despair Eff 145213
class spell_garrosh_explosive_despair_eff : public AuraScript
{
    PrepareAuraScript(spell_garrosh_explosive_despair_eff);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (Creature* garrosh = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_GARROSH_HELLSCREAM) : ObjectGuid::Empty))
                SetStackAmount(garrosh->AI()->GetData(TYPE_EXPLOSIVE_DESPAIR));
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_garrosh_explosive_despair_eff::OnAuraEffectApply, EFFECT_0, SPELL_AURA_MOD_MECHANIC_DAMAGE_TAKEN_PERCENT, AURA_EFFECT_HANDLE_REAL);
    }
};

// Transition Phase Third Effect 145246
class spell_soo_transition_phase_third_effect : public AuraScript
{
    PrepareAuraScript(spell_soo_transition_phase_third_effect);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* m_owner = GetOwner()->ToCreature())
            m_owner->AI()->DoAction(ACTION_GARROSH_PHASE_THIRD);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_soo_transition_phase_third_effect::OnAuraEffectApply, EFFECT_1, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
    }
};

// Consumed Hope 149032
class spell_garrosh_consumed_hope : public AuraScript
{
    PrepareAuraScript(spell_garrosh_consumed_hope);

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_HOPE_MISSLE, true);
    }

    void Register()
    {
        OnProc += AuraProcFn(spell_garrosh_consumed_hope::HandleOnProc);
    }
};

// Desecrate Selector 144745
class spell_garrosh_desecrate_selector : public SpellScript
{
    PrepareSpellScript(spell_garrosh_desecrate_selector);

    std::list<WorldObject*> m_targets;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Player* target = GetHitPlayer())
            {
                caster->SetTarget(target->GetGUID());
                ObjectGuid targetGUID = target->GetGUID();
                uint32 spellId = GetSpellInfo()->Effects[caster->HasAura(SPELL_DESECRATED_EMPOWERED) ? EFFECT_1 : EFFECT_0].BasePoints;

                // Target Scanning 0.2s
                caster->m_Events.Schedule(250, [=]()
                {
                    if (Unit* desecrateTarget = ObjectAccessor::GetUnit(*caster, targetGUID))
                        caster->CastSpell(desecrateTarget, spellId, false);

                    if (Unit* victim = caster->GetVictim())
                        caster->SetTarget(victim->GetGUID());
                });
            }
        }
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // tanks not in priority
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && GetCaster() && target->ToPlayer()->GetExactDist2d(GetCaster()) < 12.5f; });

        uint32 requredInRangeCount = GetCaster()->GetMap()->Is25ManRaid() ? 7 : 3;

        if (targets.size() >= requredInRangeCount)
        {
            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);

            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_desecrate_selector::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_desecrate_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Phase Two Transform 144842
class spell_garrosh_phase_two_transform : public AuraScript
{
    PrepareAuraScript(spell_garrosh_phase_two_transform);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_GARROSH_PHASE_SECOND);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_garrosh_phase_two_transform::OnAuraEffectApply, EFFECT_0, SPELL_AURA_MOD_SCALE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Phase Two Transform 144842
class spell_garrosh_phase_two_transform_eff : public SpellScript
{
    PrepareSpellScript(spell_garrosh_phase_two_transform_eff);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (GetCaster() && GetCaster()->GetMap()->IsHeroic() && GetCaster()->GetMap()->Is25ManRaid())
            PreventHitEffect(eff_idx);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_phase_two_transform_eff::HandleEffectHitTarget, EFFECT_1, SPELL_EFFECT_HEAL_PCT);
    }
};

// Exploding Iron Star 144798
class spell_garrosh_exploding_iron_star : public SpellScript
{
    PrepareSpellScript(spell_garrosh_exploding_iron_star);

    void HandleEffectHitTarget(SpellEffIndex /*eff_idx*/)
    {
        if (Unit* unit = GetHitUnit())
        {
            // 85% damage reduction after 45 yards distance
            float dist = unit->GetExactDist2d(GetCaster());

            int32 reduction = dist >= 45 ? GetHitDamage() * 0.15 : GetHitDamage() * (1 - ((dist * 0.85) / 45.0f));
            SetHitDamage(reduction);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_exploding_iron_star::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Touch of Y`shaarj 145065
class spell_garrosh_touch_of_yshaarj : public SpellScript
{
    PrepareSpellScript(spell_garrosh_touch_of_yshaarj);

    std::list<WorldObject*> m_targets, copyTargets;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && (target->ToPlayer()->HasAura(SPELL_TOUCH_OH_YSHAARJ) || target->ToPlayer()->HasAura(SPELL_EMPOWERED_TOUCH_OF_YSHAARJ)); });
        targets.remove_if(TankSpecTargetSelector());

        uint32 targetsCount = sSpellMgr->GetSpellInfo(GetSpellInfo()->Id, GetCaster()->GetMap()->GetDifficulty())->MaxAffectedTargets;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 2; // min 2, max 4
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 15)
                targetsCount++;

            if (totalPlayersCount > 20)
                targetsCount++;
        }

        if (targets.size() < targetsCount)
        {
            targets.clear();

            for (auto&& itr : m_targets)
                targets.push_back(itr);

            targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && (target->ToPlayer()->HasAura(SPELL_TOUCH_OH_YSHAARJ) || target->ToPlayer()->HasAura(SPELL_EMPOWERED_TOUCH_OF_YSHAARJ)); });

            if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
            {
                // Calculate TargetsCount
                targetsCount = 2; // min 2, max 4
                uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

                if (totalPlayersCount > 15)
                    targetsCount++;

                if (totalPlayersCount > 20)
                    targetsCount++;
            }

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount);

            copyTargets = targets;
            return;
        }

        if (targets.size() > targetsCount)
            Trinity::Containers::RandomResizeList(targets, targetsCount);

        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_touch_of_yshaarj::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_touch_of_yshaarj::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_touch_of_yshaarj::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_touch_of_yshaarj::CopyTargets, EFFECT_3, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_touch_of_yshaarj::CopyTargets, EFFECT_4, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_touch_of_yshaarj::CopyTargets, EFFECT_5, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_touch_of_yshaarj::CopyTargets, EFFECT_6, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_touch_of_yshaarj::CopyTargets, EFFECT_7, TARGET_UNIT_SRC_AREA_ENEMY);
        /*OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_touch_of_yshaarj::CopyTargets, EFFECT_8, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_touch_of_yshaarj::CopyTargets, EFFECT_9, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_touch_of_yshaarj::CopyTargets, EFFECT_10, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_touch_of_yshaarj::CopyTargets, EFFECT_11, TARGET_UNIT_SRC_AREA_ENEMY);*/
    }
};

// Touch of Y`shaarj 145065
class spell_garrosh_touch_of_yshaarj_aura : public AuraScript
{
    PrepareAuraScript(spell_garrosh_touch_of_yshaarj_aura);

    void OnUpdate(uint32 /*diff*/, AuraEffect* aurEff)
    {
        if (GetOwner() && GetOwner()->ToUnit() && GetOwner()->ToUnit()->HealthBelowPct(20))
            GetOwner()->ToUnit()->RemoveAurasDueToSpell(GetSpellInfo()->Id);
    }

    void Absorb(AuraEffect*, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        if (dmgInfo.GetDamage() >= absorbAmount)
            absorbAmount = -1; // not remove
    }

    void Register() override
    {
        OnEffectUpdate += AuraEffectUpdateFn(spell_garrosh_touch_of_yshaarj_aura::OnUpdate, EFFECT_2, SPELL_AURA_MOD_SCALE);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_garrosh_touch_of_yshaarj_aura::Absorb, EFFECT_0);
    }
};

// Empowered Touch of Y`shaarj 145171
class spell_garrosh_empowered_touch_of_yshaarj : public SpellScript
{
    PrepareSpellScript(spell_garrosh_empowered_touch_of_yshaarj);

    std::list<WorldObject*> m_targets, copyTargets;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && (target->ToPlayer()->HasAura(SPELL_TOUCH_OH_YSHAARJ) || target->ToPlayer()->HasAura(SPELL_EMPOWERED_TOUCH_OF_YSHAARJ)); });
        targets.remove_if(TankSpecTargetSelector());

        uint32 targetsCount = sSpellMgr->GetSpellInfo(GetSpellInfo()->Id, GetCaster()->GetMap()->GetDifficulty())->MaxAffectedTargets;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 2; // min 2, max 4
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 15)
                targetsCount++;

            if (totalPlayersCount > 20)
                targetsCount++;
        }

        if (targets.size() < targetsCount)
        {
            targets.clear();

            for (auto&& itr : m_targets)
                targets.push_back(itr);

            targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && (target->ToPlayer()->HasAura(SPELL_TOUCH_OH_YSHAARJ) || target->ToPlayer()->HasAura(SPELL_EMPOWERED_TOUCH_OF_YSHAARJ)); });

            if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
            {
                // Calculate TargetsCount
                targetsCount = 2; // min 2, max 4
                uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

                if (totalPlayersCount > 15)
                    targetsCount++;

                if (totalPlayersCount > 20)
                    targetsCount++;
            }

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount);

            copyTargets = targets;
            return;
        }

        if (targets.size() > targetsCount)
            Trinity::Containers::RandomResizeList(targets, targetsCount);

        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowered_touch_of_yshaarj::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowered_touch_of_yshaarj::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowered_touch_of_yshaarj::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowered_touch_of_yshaarj::CopyTargets, EFFECT_3, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowered_touch_of_yshaarj::CopyTargets, EFFECT_4, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowered_touch_of_yshaarj::CopyTargets, EFFECT_5, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowered_touch_of_yshaarj::CopyTargets, EFFECT_6, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowered_touch_of_yshaarj::CopyTargets, EFFECT_7, TARGET_UNIT_SRC_AREA_ENEMY);
        /*OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowered_touch_of_yshaarj::CopyTargets, EFFECT_8, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowered_touch_of_yshaarj::CopyTargets, EFFECT_9, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowered_touch_of_yshaarj::CopyTargets, EFFECT_10, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowered_touch_of_yshaarj::CopyTargets, EFFECT_11, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowered_touch_of_yshaarj::CopyTargets, EFFECT_12, TARGET_UNIT_SRC_AREA_ENEMY);*/
    }
};

// Empowered Touch of Y`shaarj 145171
class spell_garrosh_empowered_touch_of_yshaarj_aura : public AuraScript
{
    PrepareAuraScript(spell_garrosh_empowered_touch_of_yshaarj_aura);

    void OnUpdate(uint32 /*diff*/, AuraEffect* aurEff)
    {
        if (GetOwner() && GetOwner()->ToUnit() && GetOwner()->ToUnit()->HealthBelowPct(20))
        {
            GetOwner()->ToUnit()->RemoveAurasDueToSpell(GetSpellInfo()->Id);
            GetOwner()->ToUnit()->InterruptNonMeleeSpells(false, SPELL_TOUCH_OF_YSHAARJ_TARGET_CHARM);
        }
    }

    void Absorb(AuraEffect*, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        if (dmgInfo.GetDamage() >= absorbAmount)
            absorbAmount = -1; // not remove
    }

    void Register() override
    {
        OnEffectUpdate += AuraEffectUpdateFn(spell_garrosh_empowered_touch_of_yshaarj_aura::OnUpdate, EFFECT_2, SPELL_AURA_TRANSFORM);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_garrosh_empowered_touch_of_yshaarj_aura::Absorb, EFFECT_0);
    }
};

// Touch of Y`shaarj Player Eff 145175, 145071
class spell_garrosh_touch_of_yshaarj_player_eff_aura : public AuraScript
{
    PrepareAuraScript(spell_garrosh_touch_of_yshaarj_player_eff_aura);

    void Absorb(AuraEffect*, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        if (dmgInfo.GetDamage() >= absorbAmount)
            absorbAmount = -1; // not remove
    }

    void Register() override
    {
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_garrosh_touch_of_yshaarj_player_eff_aura::Absorb, EFFECT_0);
    }
};

// Empowering Corruption 145043
class spell_garrosh_empowering_corruption : public SpellScript
{
    PrepareSpellScript(spell_garrosh_empowering_corruption)

    std::list<WorldObject*> copyTargets;

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_MINION_OF_YSHAARJ; });
        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowering_corruption::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowering_corruption::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowering_corruption::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowering_corruption::CopyTargets, EFFECT_3, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Empowering Corruption Heroic 149536
class spell_garrosh_empowering_corruption_heroic : public SpellScript
{
    PrepareSpellScript(spell_garrosh_empowering_corruption_heroic)

        std::list<WorldObject*> copyTargets;

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_MINION_OF_YSHAARJ; });
        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowering_corruption_heroic::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowering_corruption_heroic::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowering_corruption_heroic::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowering_corruption_heroic::CopyTargets, EFFECT_3, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_empowering_corruption_heroic::CopyTargets, EFFECT_4, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Enter Realm of Y`shaarj Launcher 144867
class spell_garrosh_enter_realm_of_yshaarj_launcher : public SpellScript
{
    PrepareSpellScript(spell_garrosh_enter_realm_of_yshaarj_launcher)

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && !target->ToPlayer(); });
    }

    void SelectGarrosh(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_GARROSH_HELLSCREAM; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_enter_realm_of_yshaarj_launcher::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_enter_realm_of_yshaarj_launcher::SelectGarrosh, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_enter_realm_of_yshaarj_launcher::SelectTarget, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_enter_realm_of_yshaarj_launcher::SelectTarget, EFFECT_3, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Teleport to Realms 144880, 144883, 144885
class spell_garrosh_teleport_to_realms : public SpellScript
{
    PrepareSpellScript(spell_garrosh_teleport_to_realms);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, GetSpellInfo()->Effects[eff_idx].BasePoints, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_teleport_to_realms::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Consumed Courage 149011
class spell_garrosh_consumed_courage : public SpellScript
{
    PrepareSpellScript(spell_garrosh_consumed_courage)

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_EMBODIED_FEAR; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_consumed_courage::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Consumed Faith 149033
class spell_garrosh_consumed_faith : public SpellScript
{
    PrepareSpellScript(spell_garrosh_consumed_faith)

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_EMBODIED_DOUBT; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_consumed_faith::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Whirling Corruption Eff 144989
class spell_garrosh_whirling_corruption : public SpellScript
{
    PrepareSpellScript(spell_garrosh_whirling_corruption);

    void HandleEffectHitTarget(SpellEffIndex /*eff_idx*/)
    {
        if (Unit* unit = GetHitUnit())
        {
            // 65% damage reduction after 30 yards distance
            float dist = unit->GetExactDist2d(GetCaster());
            int32 mod = 1;

            if (dist <= 8)
            {
                SetHitDamage(GetHitDamage() * mod);
                return;
            }

            int32 reduction = dist >= 30 ? mod * (GetHitDamage() * 0.35) : mod * (GetHitDamage() * (1 - ((0.65 * dist) / 30.0f)));
            SetHitDamage(reduction);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_whirling_corruption::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Garrosh Energy 145801
class spell_garrosh_garrosh_energy : public AuraScript
{
    PrepareAuraScript(spell_garrosh_garrosh_energy);

    void OnUpdate(uint32 /*diff*/, AuraEffect* aurEff)
    {
        if (Player* owner = GetOwner()->ToPlayer())
        {
            if (owner->GetInstanceScript() && owner->GetInstanceScript()->GetBossState(DATA_GARROSH_HELLSCREAM) != IN_PROGRESS)
            {
                owner->RemoveAurasDueToSpell(GetSpellInfo()->Id);
                return;
            }

            if (Creature* garrosh = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_GARROSH_HELLSCREAM) : ObjectGuid::Empty))
                owner->SetPower(POWER_ALTERNATE_POWER, garrosh->GetPower(POWER_ENERGY));
        }
    }

    void Register() override
    {
        OnEffectUpdate += AuraEffectUpdateFn(spell_garrosh_garrosh_energy::OnUpdate, EFFECT_0, SPELL_AURA_ENABLE_ALT_POWER);
    }
};

// Empowered Desecrate 144816, 144758
class spell_garrosh_empowered_desecrate_summon : public SpellScript
{
    PrepareSpellScript(spell_garrosh_empowered_desecrate_summon);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetBossState(DATA_GARROSH_HELLSCREAM) != IN_PROGRESS)
            PreventHitEffect(eff_idx);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_empowered_desecrate_summon::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectHit += SpellEffectFn(spell_garrosh_empowered_desecrate_summon::HandleEffectHitTarget, EFFECT_1, SPELL_EFFECT_SUMMON);
    }
};

// Growing Power 144947
class spell_garrosh_growing_power : public SpellScript
{
    PrepareSpellScript(spell_garrosh_growing_power);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Unit* owner = GetCaster())
        {
            if (owner->GetPower(POWER_ENERGY) >= 24 && !owner->HasAura(SPELL_WHIRLWIND_EMPOWERED))
                owner->CastSpell(owner, SPELL_WHIRLWIND_EMPOWERED, true);

            if (owner->GetPower(POWER_ENERGY) >= 49 && !owner->HasAura(SPELL_TOUCH_EMPOWERED))
                owner->CastSpell(owner, SPELL_TOUCH_EMPOWERED, true);

            if (owner->GetPower(POWER_ENERGY) >= 74 && !owner->HasAura(SPELL_DESECRATED_EMPOWERED))
                owner->CastSpell(owner, SPELL_DESECRATED_EMPOWERED, true);

            if (owner->GetPower(POWER_ENERGY) >= 99 && !owner->HasAura(SPELL_GRIPPING_EMPOWERED))
                owner->CastSpell(owner, SPELL_GRIPPING_EMPOWERED, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_growing_power::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_ENERGIZE);
    }
};

// Touch of Y`shaarj Player Launcher 145096
class spell_garrosh_touch_of_yshaarj_player_launcher : public SpellScript
{
    PrepareSpellScript(spell_garrosh_touch_of_yshaarj_player_launcher);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_TOUCH_OF_YSHAARJ_TARGET_CHARM, false);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_touch_of_yshaarj_player_launcher::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Ancestral Chain Heal 144583
class spell_garrosh_ancestral_chain_heal : public SpellScript
{
    PrepareSpellScript(spell_garrosh_ancestral_chain_heal);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (GetHitUnit() && GetHitUnit()->GetEntry() == NPC_GARROSH_HELLSCREAM)
            PreventHitEffect(eff_idx);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_ancestral_chain_heal::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_HEAL_PCT);
    }
};

// Power of the Old God 144820
class spell_garrosh_power_of_the_old_god : public SpellScript
{
    PrepareSpellScript(spell_garrosh_power_of_the_old_god);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (AreaTrigger* myWeapon = caster->GetAreaTrigger(SPELL_DESECRATED_WEAPON_AREATRIGGER))
            {
                float radius = ((caster->GetHealthPct() + 1.0f) * 20.0f) / 100.0f;
                myWeapon->SetFloatValue(AREATRIGGER_FIELD_EXPLICIT_SCALE, (radius * 1.0f) / 20.0f);
                myWeapon->SetVisualRadius(radius);
                myWeapon->UpdateObjectVisibility();

                // Update new Radius Value
                myWeapon->SetRadius(radius);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_power_of_the_old_god::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_HEAL_PCT);
    }
};

// Empowered Whirling Corruption Missle 145033
class spell_garrosh_empowered_whirling_missle : public SpellScript
{
    PrepareSpellScript(spell_garrosh_empowered_whirling_missle);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetBossState(DATA_GARROSH_HELLSCREAM) != IN_PROGRESS)
            PreventHitEffect(eff_idx);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_empowered_whirling_missle::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectHit += SpellEffectFn(spell_garrosh_empowered_whirling_missle::HandleEffectHitTarget, EFFECT_1, SPELL_EFFECT_SUMMON);
    }
};

// Touch of Y`shaarj Player Launcher 145599
class spell_garrosh_touch_of_yshaarj_player_launcher_2 : public SpellScript
{
    PrepareSpellScript(spell_garrosh_touch_of_yshaarj_player_launcher_2);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Unit* target = GetHitUnit())
            if (Creature* garrosh = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(DATA_GARROSH_HELLSCREAM) : ObjectGuid::Empty))
                garrosh->CastSpell(target, garrosh->HasAura(SPELL_TOUCH_EMPOWERED) ? SPELL_EMP_TOUCH_OF_YSHAARJ_PLAYER_EFF : SPELL_TOUCH_OF_YSHAARJ_PLAYER_EFF, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_touch_of_yshaarj_player_launcher_2::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Malice 147209
class spell_garrosh_malice : public SpellScript
{
    PrepareSpellScript(spell_garrosh_malice);

    std::list<WorldObject*> m_targets, copyTargets;

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (Unit* target = GetHitUnit())
                caster->AI()->Talk(TALK_MALICE_ANN, target);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;
        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_IRON_STAR_FIXATE); }); // exclude targets with iron star fixate

        if (targets.empty())
        {
            targets.clear();

            for (auto&& itr : m_targets)
                targets.push_back(itr);

            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);

            copyTargets = targets;
            return;
        }

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);

        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_malice::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_malice::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_malice::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Malice Selector 147229
class spell_garrosh_malice_selector : public SpellScript
{
    PrepareSpellScript(spell_garrosh_malice_selector);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, SPELL_MALICIOUS_BLAST, true);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        uint32 requiredTargets = sSpellMgr->GetSpellInfo(SPELL_MALICE, GetCaster()->GetMap()->GetDifficulty())->Effects[EFFECT_0].BasePoints;

        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->GetGUID() == GetCaster()->GetGUID(); }); // remove self

        if (targets.size() < requiredTargets)
        {
            uint32 targetsDiff = requiredTargets - targets.size(); // gain power for EACH not affected player
            targets.clear();

            if (targetsDiff > 1)
            {
                for (uint32 i = 0; i < targetsDiff; i++)
                    GetCaster()->CastSpell(GetCaster(), SPELL_MALICIOUS_ENERGY, true);
            }
            else
                GetCaster()->CastSpell(GetCaster(), SPELL_MALICIOUS_ENERGY, true); // gain power to garrosh and affect raid by damage
        }

        if (targets.size() > requiredTargets)
            Trinity::Containers::RandomResizeList(targets, requiredTargets);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_malice_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_malice_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Bombardment Selector 147132
class spell_garrosh_bombardment_selector : public SpellScript
{
    PrepareSpellScript(spell_garrosh_bombardment_selector);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, GetSpellInfo()->Effects[eff_idx].BasePoints, true);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.size() > 3)
            Trinity::Containers::RandomResizeList(targets, 3);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_bombardment_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_bombardment_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Manifest Rage 147011
class spell_garrosh_manifest_rage : public AuraScript
{
    PrepareAuraScript(spell_garrosh_manifest_rage);

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            owner->CastSpell(owner, SPELL_MANIFESTATION_RAGE, true);
            owner->CastSpell(owner, SPELL_MANIFESTATION_RAGE, true); // twice per tick
        }
    }

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            owner->SetPower(POWER_ENERGY, 0);
            owner->AI()->Talk(TALK_MANIFEST_RAGE);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_garrosh_manifest_rage::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectApply += AuraEffectApplyFn(spell_garrosh_manifest_rage::OnAuraEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Fire Iron Star Selector 147053
class spell_garrosh_fire_iron_star_selector : public SpellScript
{
    PrepareSpellScript(spell_garrosh_fire_iron_star_selector);

    std::list<WorldObject*> m_targets;

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Creature* target = GetHitCreature())
        {
            if (Creature* caster = GetCaster()->ToCreature())
            {
                if (Unit* starTarget = ObjectAccessor::GetUnit(*caster, caster->AI()->GetGUID()))
                    target->CastSpell(starTarget, GetSpellInfo()->Effects[eff_idx].BasePoints, true);
                else if (Unit* starTarget = caster->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                    target->CastSpell(starTarget, GetSpellInfo()->Effects[eff_idx].BasePoints, true);
            }
        }
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_GUNSHIP_MAIN_CANNON; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_fire_iron_star_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_fire_iron_star_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Enter Realm of Garrosh Eff 146984
class spell_garrosh_enter_realm_of_garrosh_eff : public AuraScript
{
    PrepareAuraScript(spell_garrosh_enter_realm_of_garrosh_eff);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            uint32 teleportSpell = sSpellMgr->GetSpellInfo(SPELL_ENTER_REALM_OF_GARROSH_EFF, GetCaster()->GetMap()->GetDifficulty())->Effects[EFFECT_0].BasePoints;
            owner->CastSpell(owner, teleportSpell, true);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_garrosh_enter_realm_of_garrosh_eff::OnAuraEffectRemove, EFFECT_1, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
    }
};

// Clump Check 147126
class spell_garrosh_clump_check : public SpellScript
{
    PrepareSpellScript(spell_garrosh_clump_check);

    bool Load() override
    {
        launched = false;
        return true;
    }

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (launched)
            return;

        // Try found 4/7 players around us depend of difficulty
        if (Unit* target = GetHitUnit())
        {
            std::list<Player*> playersList;
            GetPlayerListInGrid(playersList, target, 8.0f);

            playersList.remove_if([=](WorldObject* potentTarget) { return potentTarget && potentTarget->GetGUID() == target->GetGUID(); });

            uint32 requiredCount = target->GetMap()->Is25ManRaid() ? 6 : 3; // cuz exclude caster

            if (playersList.size() >= requiredCount)
            {
                launched = true;

                // Save target for launch iron star to group
                if (GetCaster() && GetCaster()->ToCreature())
                    GetCaster()->ToCreature()->AI()->SetGUID(target->GetGUID());

                if (Creature* caster = GetCaster()->ToCreature())
                    caster->AI()->DoAction(ACTION_LAUNCH_IRON_STAR);
            }
        }
    }

    private:
        bool launched;

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_clump_check::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
    }
};

// Call Bombardment 147120
class spell_garrosh_call_bombardment : public AuraScript
{
    PrepareAuraScript(spell_garrosh_call_bombardment);

    bool Load() override
    {
        procCount = 0; // limit is 4, so first trigger after 1s cast finishing bombardment. next - trigger each 3s
        bombardCounter = 0; // 30 per 10 sec, so trigger 3 each 1s, not 500ms
        hasFirstTrigger = false;
        return true;
    }

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (!hasFirstTrigger) // first triggering after 1s cast finishing
        {
            if (++procCount % 2 == 0)
            {
                procCount = 0;
                hasFirstTrigger = true;
                return;
            }

            PreventDefaultAction();
            return;
        }

        // Next triggering each 3s
        if (++procCount % 6 == 0)
            return;

        PreventDefaultAction();
    }

    void OnBombardCheck(AuraEffect const* /*aurEff*/)
    {
        if (++bombardCounter % 2 == 0)
            return;

        PreventDefaultAction();
    }

    private:
        uint32 procCount;
        uint32 bombardCounter;
        bool hasFirstTrigger;

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_garrosh_call_bombardment::OnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_garrosh_call_bombardment::OnBombardCheck, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// Bombardment Force 147131
class spell_garrosh_bombardment_force : public SpellScript
{
    PrepareSpellScript(spell_garrosh_bombardment_force);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_GUNSHIP_SMALL_CANNON; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_bombardment_force::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Iron Star Fixate Selector 147712
class spell_garrosh_iron_star_fixate_selector : public SpellScript
{
    PrepareSpellScript(spell_garrosh_iron_star_fixate_selector);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, GetSpellInfo()->Effects[eff_idx].BasePoints, true);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.empty())
            return;

        targets.sort(Trinity::ObjectDistanceOrderPred(GetCaster(), true));
        targets.resize(1);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_iron_star_fixate_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_iron_star_fixate_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Fixate 147665
class spell_garrosh_iron_star_fixate : public AuraScript
{
    PrepareAuraScript(spell_garrosh_iron_star_fixate);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (GetCaster() && GetCaster()->ToCreature())
            {
                GetCaster()->ToCreature()->AI()->SetGUID(owner->GetGUID());
                GetCaster()->ToCreature()->AI()->AttackStart(owner);
                GetCaster()->ToCreature()->SetReactState(REACT_PASSIVE);
                GetCaster()->ToCreature()->ClearUnitState(UNIT_STATE_CASTING);
            }
        }
    }

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetCaster() && GetCaster()->ToCreature())
            GetCaster()->ToCreature()->AI()->DoAction(ACTION_START_INTRO);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_garrosh_iron_star_fixate::OnAuraEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_garrosh_iron_star_fixate::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Unstable Iron Star Garrosh Eff 147177
class spell_garrosh_unstable_iron_star_garrosh_eff : public AuraScript
{
    PrepareAuraScript(spell_garrosh_unstable_iron_star_garrosh_eff);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            if (owner->GetPower(POWER_ENERGY) > 99)
                owner->SetPower(POWER_ENERGY, 0); // only on cast
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_garrosh_unstable_iron_star_garrosh_eff::OnAuraEffectApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// Blood Frenzied 149336
class spell_garrosh_blood_frenzied_selector : public SpellScript
{
    PrepareSpellScript(spell_garrosh_blood_frenzied_selector);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->AI()->SetGUID(target->GetGUID());
                caster->AI()->AttackStart(target);
                caster->SetReactState(REACT_PASSIVE);
            }
        }
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && (target->GetTypeId() != TYPEID_PLAYER || hasTargetFixated(target->GetGUID())); });

        if (targets.empty())
            return;

        targets.sort(Trinity::ObjectDistanceOrderPred(GetCaster(), true));

        if (targets.size() > 1)
            targets.resize(1);
    }

    private:
        bool hasTargetFixated(uint64 targetGUID)
        {
            std::list<Creature*> warbringersList;
            GetCreatureListWithEntryInGrid(warbringersList, GetCaster(), GetCaster()->GetEntry(), 100.0f);

            warbringersList.remove_if([=](Creature* korkronWarbringer) { return korkronWarbringer && !korkronWarbringer->IsAlive(); });

            for (auto&& itr : warbringersList)
                if (itr->AI()->GetGUID() == targetGUID)
                    return true;

            return false;
        }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_blood_frenzied_selector::HandleEffectHitTarget, EFFECT_1, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_blood_frenzied_selector::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Crushing Fear Selector 147325
class spell_garrosh_crushing_fear_selector : public SpellScript
{
    PrepareSpellScript(spell_garrosh_crushing_fear_selector);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, GetSpellInfo()->Effects[eff_idx].BasePoints, true);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && GetCaster() && GetCaster()->GetExactDist2d(target) > 225.0f; });

        uint32 allowTargets = GetCaster()->GetMap()->Is25ManRaid() ? 9 : 4;

        if (targets.size() > allowTargets)
            Trinity::Containers::RandomResizeList(targets, allowTargets);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_garrosh_crushing_fear_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_crushing_fear_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Manifistation of Rage Eff 147013
class spell_garrosh_manifistation_of_rage_eff : public SpellScript
{
    PrepareSpellScript(spell_garrosh_manifistation_of_rage_eff);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetBossState(DATA_GARROSH_HELLSCREAM) != IN_PROGRESS)
            PreventHitEffect(eff_idx);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_garrosh_manifistation_of_rage_eff::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SUMMON);
    }
};

// Iron Star Crater 147048
class spell_garrosh_iron_star_crater : public SpellScript
{
    PrepareSpellScript(spell_garrosh_iron_star_crater);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetBossState(DATA_GARROSH_HELLSCREAM) != IN_PROGRESS)
            PreventHitEffect(eff_idx);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_garrosh_iron_star_crater::HandleEffectHitTarget, EFFECT_2, SPELL_EFFECT_SUMMON);
    }
};

// Annihilate 144969
class spell_garrosh_annihilate : public SpellScript
{
    PrepareSpellScript(spell_garrosh_annihilate);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->GetTypeId() != TYPEID_PLAYER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_annihilate::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_garrosh_annihilate::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Faith Missle 148991
class spell_garrosh_faith_missle : public SpellScript
{
    PrepareSpellScript(spell_garrosh_faith_missle);

    void SelectDest(SpellDestination& dest)
    {
        Position pos = GetCaster()->GetNearPosition(frand(-GetSpellInfo()->Effects[EFFECT_0].MaxRadiusEntry->RadiusMax, GetSpellInfo()->Effects[EFFECT_0].MaxRadiusEntry->RadiusMax), frand(0.0f, 2 * M_PI));
        dest._position.Relocate(pos);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_garrosh_faith_missle::SelectDest, EFFECT_0, TARGET_DEST_CASTER);
    }
};

// Jump to Ground 144956
class spell_garrosh_jump_to_ground : public SpellScript
{
    PrepareSpellScript(spell_garrosh_jump_to_ground);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_JUMO_TO_GROUND_EFF, true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_garrosh_jump_to_ground::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_JUMP_DEST);
    }
};

// 1089. Summoned by 144645 - Iron Star Impact
class sat_garrosh_iron_star_impact : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && (triggering->ToCreature() && (triggering->ToCreature()->GetEntry() == NPC_WARBRINGER_KORKRON || triggering->ToCreature()->GetEntry() == NPC_WOLF_RIDER_FARSEER) || triggering->ToPlayer() && triggering->ToPlayer()->IsAlive()) && triggering->ToUnit() && IsInBox(triggering->ToUnit(), 29.0f, 17.0f, 54.0f);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
                caster->CastSpell(target, SPELL_IRON_STAR_IMPACT_EFF, true);
        }
        else if (Creature* target = triggering->ToCreature())
            target->CastSpell(target, SPELL_IRON_STAR_IMPACT_EFF_2, true);
    }
};

// 1091, 1093. Summoned by 144760, 144818 - Desecrated Weapon
class sat_garrosh_desecrated_weapon : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        if (Creature* garrosh = ObjectAccessor::GetCreature(*triggering, triggering->GetInstanceScript() ? triggering->GetInstanceScript()->GetGuidData(DATA_GARROSH_HELLSCREAM) : ObjectGuid::Empty))
            return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && !triggering->ToPlayer()->HasAura(SPELL_TOUCH_OH_YSHAARJ) && !triggering->ToPlayer()->HasAura(SPELL_TOUCH_OF_YSHAARJ_PLAYER_EFF) && !triggering->ToPlayer()->HasAura(SPELL_EMPOWERED_TOUCH_OF_YSHAARJ) && !triggering->ToPlayer()->HasAura(SPELL_EMP_TOUCH_OF_YSHAARJ_PLAYER_EFF) && !triggering->ToPlayer()->HasAura(SPELL_ENTER_REALM_OF_GARROSH) && !garrosh->AI()->GetData(TYPE_GARROSH_REALM); // prevent hit from AT if already in transition.

        return false;
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(target, SPELL_DESECRATED, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAura(SPELL_DESECRATED, GetCaster()->GetGUID(), 0, AURA_REMOVE_BY_EXPIRE);
    }
};

// 1347. Summoned by 148982 - Courage
class sat_garrosh_courage : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && object->ToPlayer()->IsAlive() && !object->ToPlayer()->HasAura(SPELL_COURAGE_AURA);
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
            itr->CastSpell(itr, SPELL_COURAGE_AURA, true);
    }
};

// 1350. Summoned by 149003 - Hope
class sat_garrosh_hope : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && object->ToPlayer()->IsAlive() && !object->ToPlayer()->HasAura(SPELL_HOPE_AURA);
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
            itr->CastSpell(itr, SPELL_HOPE_AURA, true);
    }
};

// 1349. Summoned by 148992 - Faith
class sat_garrosh_faith : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && object->ToPlayer()->IsAlive() && !object->ToPlayer()->HasAura(SPELL_FAITH_AURA) && GetAreaTrigger() && GetAreaTrigger()->GetDuration() < 14000;
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
            itr->CastSpell(itr, SPELL_FAITH_AURA, true);
    }
};

// 1275. Summoned by 147135 - Bombardment
class sat_garrosh_bombardment : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, SPELL_NAPALM, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_NAPALM);
    }
};

// 1300. Summoned by 147148 - Unstable Iron Star
class sat_garrosh_unstable_iron_star_garrosh_collision : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && GetCaster() && GetCaster()->ToCreature() && GetCaster()->ToCreature()->AI()->GetData(TYPE_ALLOW_IRON_STAR_EXPLOSIVE) && (triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() || triggering->ToCreature() && triggering->ToCreature()->IsAlive() && triggering->ToCreature()->GetEntry() == NPC_GARROSH_HELLSCREAM && GetCaster()->GetExactDist2d(triggering->ToCreature()) <= 10) && !GetCaster()->ToCreature()->AI()->GetData(TYPE_IRON_STAR_EXPLOSIVE);
    }

    void OnTrigger(WorldObject* triggering) override
    {
        if (GetCaster() && GetCaster()->ToCreature())
        {
            if (Creature* target = triggering->ToCreature())
            {
                GetCaster()->ToCreature()->AI()->SetData(TYPE_IRON_STAR_EXPLOSIVE, 1);
                GetCaster()->CastSpell(GetCaster(), SPELL_UNSTABLE_IRON_STAR_EXPLOSION, true);
                GetCaster()->CastSpell(GetCaster(), SPELL_UNSTABLE_IRON_STAR_GARROSH, true);
                GetCaster()->ToCreature()->PrepareChanneledCast(GetCaster()->GetOrientation());

                if (AreaTrigger* atrigger = GetAreaTrigger())
                    atrigger->SetDuration(0);

                GetCaster()->ToCreature()->DespawnOrUnsummon(1 * IN_MILLISECONDS);
            }
            else if (Player* target = triggering->ToPlayer())
            {
                GetCaster()->ToCreature()->AI()->SetData(TYPE_IRON_STAR_EXPLOSIVE, 1);
                GetCaster()->CastSpell(GetCaster(), SPELL_UNSTABLE_IRON_STAR_EXPLOSION, true);
                GetCaster()->ToCreature()->PrepareChanneledCast(GetCaster()->GetOrientation());

                if (AreaTrigger* atrigger = GetAreaTrigger())
                    atrigger->SetDuration(0);

                GetCaster()->ToCreature()->DespawnOrUnsummon(1 * IN_MILLISECONDS);
            }
        }
    }
};

void AddSC_garrosh_hellscream()
{
    new boss_garrosh_hellscream();
    new creature_script<npc_siege_engineer>("npc_siege_engineer");
    new creature_script<npc_korkron_warbringer>("npc_korkron_warbringer");
    new creature_script<npc_wolf_rider_farseer>("npc_wolf_rider_farseer");
    new creature_script<npc_soo_desecrated_weapon>("npc_soo_desecrated_weapon");
    new creature_script<npc_soo_iron_star>("npc_soo_iron_star");
    new creature_script<npc_soo_embodied_minions>("npc_soo_embodied_minions");
    new creature_script<npc_soo_heart_of_yshaarj>("npc_soo_heart_of_yshaarj");
    new creature_script<npc_garrosh_heart_of_yshaarj_realm>("npc_garrosh_heart_of_yshaarj_realm");
    new creature_script<npc_garrosh_minion_of_yshaarj>("npc_garrosh_minion_of_yshaarj");
    new creature_script<npc_garrosh_unstable_iron_star>("npc_garrosh_unstable_iron_star");

    new spell_script<spell_soo_hellscreams_warsong>("spell_soo_hellscreams_warsong");
    new aura_script<spell_soo_power_iron_star>("spell_soo_power_iron_star");
    new aura_script<spell_garrosh_realm_of_yshaarj>("spell_garrosh_realm_of_yshaarj");
    new aura_script<spell_soo_yshaarjs_protection_aura>("spell_soo_yshaarjs_protection_aura");
    new spell_script<spell_soo_yshaarjs_protection>("spell_soo_yshaarjs_protection");
    new aura_script<spell_soo_whirling_corruption>("spell_soo_whirling_corruption");
    new aura_script<spell_soo_empowered_gripping_despair>("spell_soo_empowered_gripping_despair");
    new aura_script<spell_garrosh_explosive_despair_eff>("spell_garrosh_explosive_despair_eff");
    new aura_script<spell_soo_transition_phase_third_effect>("spell_soo_transition_phase_third_effect");
    new aura_script<spell_garrosh_consumed_hope>("spell_garrosh_consumed_hope");
    new spell_script<spell_garrosh_desecrate_selector>("spell_garrosh_desecrate_selector");
    new aura_script<spell_garrosh_phase_two_transform>("spell_garrosh_phase_two_transform");
    new spell_script<spell_garrosh_phase_two_transform_eff>("spell_garrosh_phase_two_transform_eff");
    new spell_script<spell_garrosh_exploding_iron_star>("spell_garrosh_exploding_iron_star");
    new spell_script<spell_garrosh_touch_of_yshaarj>("spell_garrosh_touch_of_yshaarj");
    new aura_script<spell_garrosh_touch_of_yshaarj_aura>("spell_garrosh_touch_of_yshaarj_aura");
    new spell_script<spell_garrosh_empowered_touch_of_yshaarj>("spell_garrosh_empowered_touch_of_yshaarj");
    new aura_script<spell_garrosh_empowered_touch_of_yshaarj_aura>("spell_garrosh_empowered_touch_of_yshaarj_aura");
    new aura_script<spell_garrosh_touch_of_yshaarj_player_eff_aura>("spell_garrosh_touch_of_yshaarj_player_eff_aura");
    new spell_script<spell_garrosh_empowering_corruption>("spell_garrosh_empowering_corruption");
    new spell_script<spell_garrosh_empowering_corruption_heroic>("spell_garrosh_empowering_corruption_heroic");
    new spell_script<spell_garrosh_enter_realm_of_yshaarj_launcher>("spell_garrosh_enter_realm_of_yshaarj_launcher");
    new spell_script<spell_garrosh_teleport_to_realms>("spell_garrosh_teleport_to_realms");
    new spell_script<spell_garrosh_consumed_courage>("spell_garrosh_consumed_courage");
    new spell_script<spell_garrosh_consumed_faith>("spell_garrosh_consumed_faith");
    new spell_script<spell_garrosh_whirling_corruption>("spell_garrosh_whirling_corruption");
    new spell_script<spell_garrosh_empowered_desecrate_summon>("spell_garrosh_empowered_desecrate_summon");
    new aura_script<spell_garrosh_garrosh_energy>("spell_garrosh_garrosh_energy");
    new spell_script<spell_garrosh_growing_power>("spell_garrosh_growing_power");
    new spell_script<spell_garrosh_touch_of_yshaarj_player_launcher>("spell_garrosh_touch_of_yshaarj_player_launcher");
    new spell_script<spell_garrosh_ancestral_chain_heal>("spell_garrosh_ancestral_chain_heal");
    new spell_script<spell_garrosh_power_of_the_old_god>("spell_garrosh_power_of_the_old_god");
    new spell_script<spell_garrosh_empowered_whirling_missle>("spell_garrosh_empowered_whirling_missle");
    new spell_script<spell_garrosh_touch_of_yshaarj_player_launcher_2>("spell_garrosh_touch_of_yshaarj_player_launcher_2");
    new spell_script<spell_garrosh_malice>("spell_garrosh_malice");
    new spell_script<spell_garrosh_malice_selector>("spell_garrosh_malice_selector");
    new spell_script<spell_garrosh_bombardment_selector>("spell_garrosh_bombardment_selector");
    new aura_script<spell_garrosh_manifest_rage>("spell_garrosh_manifest_rage");
    new spell_script<spell_garrosh_fire_iron_star_selector>("spell_garrosh_fire_iron_star_selector");
    new aura_script<spell_garrosh_enter_realm_of_garrosh_eff>("spell_garrosh_enter_realm_of_garrosh_eff");
    new spell_script<spell_garrosh_clump_check>("spell_garrosh_clump_check");
    new aura_script<spell_garrosh_call_bombardment>("spell_garrosh_call_bombardment");
    new spell_script<spell_garrosh_bombardment_force>("spell_garrosh_bombardment_force");
    new spell_script<spell_garrosh_iron_star_fixate_selector>("spell_garrosh_iron_star_fixate_selector");
    new aura_script<spell_garrosh_iron_star_fixate>("spell_garrosh_iron_star_fixate");
    new aura_script<spell_garrosh_unstable_iron_star_garrosh_eff>("spell_garrosh_unstable_iron_star_garrosh_eff");
    new spell_script<spell_garrosh_blood_frenzied_selector>("spell_garrosh_blood_frenzied_selector");
    new spell_script<spell_garrosh_crushing_fear_selector>("spell_garrosh_crushing_fear_selector");
    new spell_script<spell_garrosh_manifistation_of_rage_eff>("spell_garrosh_manifistation_of_rage_eff");
    new spell_script<spell_garrosh_iron_star_crater>("spell_garrosh_iron_star_crater");
    new spell_script<spell_garrosh_annihilate>("spell_garrosh_annihilate");
    new spell_script<spell_garrosh_faith_missle>("spell_garrosh_faith_missle");
    new spell_script<spell_garrosh_jump_to_ground>("spell_garrosh_jump_to_ground");
    new atrigger_script<sat_garrosh_iron_star_impact>("sat_garrosh_iron_star_impact");
    new atrigger_script<sat_garrosh_desecrated_weapon>("sat_garrosh_desecrated_weapon");
    new atrigger_script<sat_garrosh_courage>("sat_garrosh_courage");
    new atrigger_script<sat_garrosh_hope>("sat_garrosh_hope");
    new atrigger_script<sat_garrosh_faith>("sat_garrosh_faith");
    new atrigger_script<sat_garrosh_bombardment>("sat_garrosh_bombardment");
    new atrigger_script<sat_garrosh_unstable_iron_star_garrosh_collision>("sat_garrosh_unstable_iron_star_garrosh_collision");
}
