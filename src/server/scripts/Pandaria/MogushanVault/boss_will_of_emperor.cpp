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

#include "mogu_shan_vault.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "Vehicle.h"

enum eSpells
{
    // Jan Xi & Qin Xi
    SPELL_STOMP                 = 116969,
    SPELL_DEVAST_ARC            = 117006,
    SPELL_DEVAST_ARC_RIGHT      = 117005,
    SPELL_DEVAST_ARC_LEFT       = 117003,
    SPELL_DEVAST_ARC_2          = 116835,
    SPELL_ARC_VISUAL_LEFT       = 116968, // anim prepare left arc
    SPELL_ARC_VISUAL_RIGHT      = 116971, // anim prepare right arc
    SPELL_ARC_VISUAL_CENTER     = 116972, // anim prepare center arc
    SPELL_MAGNETIC_ARMOR_QIN    = 116815,
    SPELL_MAGNETIZED_QIN        = 116818,
    SPELL_MAGNETIC_PULL_QIN     = 116919,
    SPELL_MAGNETIC_ARMOR_JAN    = 117193,
    SPELL_MAGNETIZED_JAN        = 117195,
    SPELL_MAGNETIC_PULL_JAN     = 117194,
    SPELL_FREEZE_ANIM           = 16245,
    SPELL_OPPORTUNISTIC_STRIKE  = 116808,
    SPELL_GROWING_OPPORTUNITY   = 117854,

    // Ancient Mogu Machine
    SPELL_TITAN_GAS             = 116779,
    SPELL_TITAN_GAS_AURA        = 116803,
    SPELL_TITAN_GAS_AURA2       = 118327,
    SPELL_TITAN_GAS_HEROIC      = 118320,

    // Rage
    SPELL_FOCALISED_ASSAULT     = 116525,
    SPELL_WITHOUT_ARMOR         = 116535,

    // Courage
    SPELL_FOCALISED_DEFENSE     = 116778,
    SPELL_IMPEDING_THRUST       = 117485,
    SPELL_HALF_PLATE            = 116537,
    SPELL_PHALANX_WALL          = 116549,

    // Strength
    SPELL_ENERGIZING_SMASH      = 116550,
    SPELL_ENERGIZING_VISUAL     = 116556,
    SPELL_ENERGIZED             = 116605,
    SPELL_FULL_PLATE            = 116540,

    // Titan Spark - Heroic
    SPELL_SUMMON_TITAN_SPARK    = 117746,
    SPELL_FOCALISED_ENERGY      = 116829,
    SPELL_ENERGY_OF_CREATION    = 116805,
    SPELL_ENERGY_OF_CREATION_A  = 116673, // spark aura

    // Shared
    SPELL_TERRACOTTA_SKYBEAM_S  = 118063,
    SPELL_TERRACOTTA_SKYBEAM_M  = 118060,
    SPELL_TERRACOTTA_SPAWN      = 118165,
    SPELL_TERRACOTTA_JUMP       = 127708,

    // Visual
    SPELL_COSMETIC_LIGHTNING    = 127732
};

enum eEvents
{
    // Jan & Qin
    EVENT_DEVASTATING_COMBO     = 1,
    EVENT_CHECK_MAGNETIC_ARMOR  = 2,
    EVENT_SUMMON_RAGE           = 4,
    EVENT_SUMMON_STRENGTH       = 5,
    EVENT_SUMMON_COURAGE        = 6,
    EVENT_ARC_READY             = 7,
    EVENT_DEVASTATING_CANCEL    = 8,
    EVENT_RESTORE_MANA          = 9,
    EVENT_BOSS_CAST_SKYBEAM     = 100,
    EVENT_BOSS_CAST_SPAWNIN     = 101,
    EVENT_BOSS_WAIT             = 102,
    EVENT_BOSS_WAIT_VISIBLE     = 103,
    EVENT_BOSS_EMOTE            = 104,
    EVENT_BOSS_FREEZE           = 105,

    // Shared
    EVENT_CHECK_TARGET          = 10,
    EVENT_WAIT                  = 11,
    EVENT_WAIT_VISIBLE          = 12,
    EVENT_CAST_SKYBEAM          = 13,
    EVENT_CAST_SPAWNIN          = 14,
    EVENT_TITAN_GAS             = 16,
    EVENT_END_TITAN_GAS         = 17,

    // Courage
    EVENT_IMPEDING_THRUST       = 20,
    EVENT_CHECK_FOCDEF          = 21,
    EVENT_PHALANX_WALL          = 22,

    // Strenght
    EVENT_ENERGIZING_SMASH      = 30,
    EVENT_ENERGIZING_REMOVE     = 31,
    EVENT_SELECT_TARGET         = 32,
    EVENT_ENERGIZING_VISUAL     = 33,

    // Rage
    EVENT_RAGE_FIRST_ATTACK     = 34,
    EVENT_CHECK_WIPE            = 35,
    EVENT_LIGHTNING_COSMETIC    = 36,

    // Misc
    EVENT_PREPARE               = 37,
};

enum eAddActions
{
    // Bosses action
    ACTION_ACTIVATE             = 0,
    ACTION_WIPE                 = 1,

    // Adds actions
    ACTION_LAND                 = 2,
    ACTION_COSMECTIC            = 3,
    ACTION_MOGU_ACTIVATE        = 4,
    ACTION_DEV_ARC              = 5,
    ACTION_TITAN_GAS            = 6,
    ACTION_TITAN_GAS_END        = 7,

    TYPE_DEVASTATION_ARC        = 8,
    ACTION_CONSOLE_WIPE         = 9,
};

enum eDisplayID
{
    DISPLAY_BOSS_INVISIBLE      = 15880,
    DISPLAY_ADD_INVISIBLE       = 11686,
    // For each creature (Boss or adds), we have : entry - 19008 = displayId, so we can get the right modelId with me->SetDisplayID(me->GetEntry() - 19008);
    DISPLAY_VISIBLE             = 19008
};

enum eWeapons
{
    WEAPON_RAGE                 = 81169,
    WEAPON_STRENGTH             = 81170,
    WEAPON_COURAGE_LEFT         = 81171,
    WEAPON_COURAGE_RIGHT        = 81172,
    WEAPON_JAN_XI               = 80289,
    WEAPON_QIN_XI               = 80939
};

enum eTalk
{
    TALK_JAN_QIN_XI             = 0,
    TALK_RAGE                   = 1,
    TALK_STRENGTH               = 2,
    TALK_COURAGE                = 3,
    TALK_TITAN_GAS_START        = 4,
    TALK_TITAN_GAS_END          = 5,
    TALK_DEFEATED               = 6,
    TALK_CONSTRUCTS_APPEAR      = 7,
    TALK_EMPEROR_STRENGTH_ANN   = 8,
    TALK_EMPEROR_COURAGE_ANN    = 9,
    TALK_TITAN_GAS_ANN          = 10,
    TALK_TO_LOWER_LEVEL         = 11,
};

#define CENTER_X 3869.67f
#define CENTER_Y 1550.34f
#define CENTER_Z 362.282f

// Achievement
#define ACHIEVE 6455

// Starting positions for adds
const Position EmperorRage[6]
{
    { 3814.52f, 1537.23f, 398.33f, 0.19f },
    { 3813.38f, 1550.42f, 398.33f, 6.27f },
    { 3814.84f, 1563.42f, 398.33f, 6.08f },
    { 3814.64f, 1537.17f, 367.72f, 0.19f },
    { 3813.38f, 1550.36f, 367.72f, 6.27f },
    { 3814.87f, 1563.53f, 367.72f, 6.07f }
};

const Position EmperrorStrength[8]
{
    { 3835.68f, 1598.06f, 398.91f, 5.45f },
    { 3853.23f, 1608.38f, 398.91f, 5.13f },
    { 3835.51f, 1598.20f, 368.30f, 5.45f },
    { 3853.45f, 1607.81f, 368.30f, 5.13f },
    { 3835.31f, 1502.10f, 398.91f, 0.82f },
    { 3853.10f, 1491.93f, 398.91f, 1.14f },
    { 3835.22f, 1501.85f, 368.30f, 0.82f },
    { 3853.05f, 1492.02f, 368.30f, 1.14f }
};

const Position EmperrorCourage[8]
{
    { 3874.45f, 1612.35f, 398.91f, 4.79f },
    { 3895.20f, 1612.17f, 398.91f, 4.46f },
    { 3874.43f, 1611.79f, 368.30f, 4.79f },
    { 3895.31f, 1612.18f, 368.30f, 4.46f },
    { 3874.40f, 1488.01f, 398.91f, 1.48f },
    { 3895.32f, 1487.56f, 398.91f, 1.81f },
    { 3874.36f, 1487.67f, 368.30f, 1.48f },
    { 3895.39f, 1487.81f, 368.30f, 1.81f }
};

class CourageTargetSelector
{
    public:
        CourageTargetSelector() {}

        bool operator() (WorldObject* const& object)
        {
            return object && object->ToPlayer() && object->ToPlayer()->GetRoleForGroup(object->ToPlayer()->GetTalentSpecialization()) != ROLES_TANK;
        }
};

class RageTargetSelector
{
    public:
        RageTargetSelector() {}

        bool operator() (WorldObject* const& object)
        {
            return object && (object->ToPlayer() && object->ToPlayer()->HasAura(SPELL_FOCALISED_ASSAULT) || object->ToPlayer()->IsGameMaster());
        }
};

#define NB_ALCOVES_RAGE 6
#define NB_ALCOVES_STR_COUR 8

// Qin-Xi - 60399
// Jan-Xi - 60400
class boss_jin_qin_xi : public CreatureScript
{
    public:
        boss_jin_qin_xi() : CreatureScript("boss_jin_qin_xi") { }

        struct boss_jin_qin_xiAI : public BossAI
        {
            boss_jin_qin_xiAI(Creature* creature) : BossAI(creature, DATA_WILL_OF_EMPEROR) { }

            int sumCourage, comboArc;
            uint8 maxCombo, janHitCount, qinHitCount, devastatingComboPhase;
            ObjectGuid victimWithMagneticArmor, ArcDevastVictimGUID, TerracotaBossGUID;
            float moveTurn, moveWalk, moveRun, spellOri;
            bool isActive, achievement, nonArc;
            std::list<ObjectGuid> playerList;
            Position homePos;
            std::map<uint32, float> ArcComboAnim;
            uint32 prevSpellId, spellAnim;

            void InitializeAI() override
            {
                me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 10.0f);
                me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 10.0f);
                Reset();
            }

            void Reset() override
            {
                if (!me->IsAlive())
                    return;

                _Reset();

                me->SetFaction(35);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->SetDisplayId(DISPLAY_BOSS_INVISIBLE);
                me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, 0);
                me->SetReactState(REACT_AGGRESSIVE);

                isActive = false;
                nonArc = true;
                me->ResetLootMode();
                events.Reset();
                summons.DespawnAll();

                moveTurn = me->GetSpeed(MOVE_TURN_RATE);
                moveWalk = me->GetSpeed(MOVE_WALK);
                moveRun  = me->GetSpeed(MOVE_RUN);
                homePos  = me->GetHomePosition();

                victimWithMagneticArmor = ObjectGuid::Empty;

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGNETIC_ARMOR_QIN);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                playerList.clear();

                achievement = false;
                janHitCount = 0;
                qinHitCount = 0;
                sumCourage = 0;
                prevSpellId = 0;

                me->SetPowerType(POWER_MANA);
                me->SetMaxPower(POWER_MANA, IsHeroic() ? 40 : 20);
                me->SetPower(POWER_MANA, 0);
                me->setRegeneratingMana(false);

                ArcComboAnim.clear();

            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_DEVASTATION_ARC)
                    return prevSpellId;
                return 0;
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                // Applies only if hit by opportunistic strike
                if (spell->Id != SPELL_OPPORTUNISTIC_STRIKE)
                    return;

                // Increases the right counter as all the strikes must be performed on the same boss
                if (me->GetEntry() == NPC_JAN_XI)
                    ++janHitCount;
                else
                    ++qinHitCount;

                // Get the number of players
                uint8 team = me->GetMap()->GetPlayers().getSize();

                // If all the players have done the opportunistic strike on the same boss, the achievement is done
                if (janHitCount == team || qinHitCount == team)
                    if (me->GetInstanceScript()->GetData(ACHIEVE))
                        me->GetInstanceScript()->SetData(ACHIEVE, 1);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoZoneInCombat();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            }

            void EnterEvadeMode() override
            {
                summons.DespawnAll();

                _DespawnAtEvade();
                me->GetMotionMaster()->MoveTargetedHome();

                if (instance)
                {
                    instance->SetBossState(DATA_WILL_OF_EMPEROR, FAIL);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                }

                instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STOMP);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DEVAST_ARC);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DEVAST_ARC_2);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TITAN_GAS);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARC_VISUAL_CENTER);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARC_VISUAL_RIGHT);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARC_VISUAL_LEFT);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGNETIC_ARMOR_JAN);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGNETIC_ARMOR_QIN);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGNETIZED_JAN);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGNETIZED_QIN);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ENERGY_OF_CREATION);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGNETIZED_JAN);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FOCALISED_ASSAULT);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FOCALISED_ENERGY);

                if (Creature* anc_mogu_machine = GetClosestCreatureWithEntry(me, NPC_ANCIENT_MOGU_MACHINE, 200.0f))
                    anc_mogu_machine->AI()->Reset();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
                    return;

                if (pointId == 2)
                {
                    me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                    events.ScheduleEvent(EVENT_BOSS_EMOTE, 2000);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                events.Reset();
                summons.DespawnAll();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                if (me->GetEntry() == NPC_QIN_XI)
                {
                    if (Creature* anc_mogu_machine = GetClosestCreatureWithEntry(me, NPC_ANCIENT_MOGU_MACHINE, 200.0f))
                    {
                        anc_mogu_machine->AI()->Talk(TALK_DEFEATED);
                        me->Kill(anc_mogu_machine->ToUnit());
                    }
                }

                instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STOMP);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DEVAST_ARC);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DEVAST_ARC_2);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TITAN_GAS);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARC_VISUAL_CENTER);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARC_VISUAL_RIGHT);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARC_VISUAL_LEFT);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGNETIC_ARMOR_JAN);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGNETIC_ARMOR_QIN);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGNETIZED_JAN);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGNETIZED_QIN);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ENERGY_OF_CREATION);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGNETIZED_JAN);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FOCALISED_ASSAULT);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FOCALISED_ENERGY);

                instance->SetBossState(DATA_WILL_OF_EMPEROR, DONE);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_ACTIVATE:
                        if (isActive)
                            return;

                        // Appearing effect
                        isActive = true;
                        events.ScheduleEvent(EVENT_BOSS_CAST_SKYBEAM, 90000);

                        // 5 or 10 attacks, each combo is made of 2 phases - Using 10 by default
                        maxCombo = IsHeroic() ? 20 : 10;
                        devastatingComboPhase = 0;

                        // --- Summoning adds ---
                        events.ScheduleEvent(EVENT_SUMMON_RAGE, 11000);
                        break;
                    case ACTION_WIPE:
                        EnterEvadeMode();
                        break;
                    case ACTION_DEV_ARC:
                    {
                        uint8 ComboCount = IsHeroic() ? 10 : 5;
                        devastatingComboPhase >= ComboCount ? events.ScheduleEvent(EVENT_DEVASTATING_CANCEL, 100) : events.ScheduleEvent(EVENT_ARC_READY, 200);
                        break;
                    }
                    case ACTION_TITAN_GAS:
                        events.CancelEvent(EVENT_SUMMON_RAGE);
                        events.CancelEvent(EVENT_SUMMON_STRENGTH);
                        events.CancelEvent(EVENT_SUMMON_COURAGE);
                        break;
                    case ACTION_TITAN_GAS_END:
                        // --- Summoning adds ---
                        events.ScheduleEvent(EVENT_SUMMON_RAGE, 11000);
                        break;
                }
            }

            // Calculate Ori
            float GetOriForCurrentArcType(float ori, float mod)
            {
                float orientation = ori + mod;
                orientation = (orientation > (M_PI * 2)) ? (orientation - (M_PI * 2)) : (orientation < 0.0f ? (orientation + (M_PI * 2)) : orientation);

                return orientation;
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (spell->Id != SPELL_DEVAST_ARC && spell->Id != SPELL_STOMP)
                    return;

                for (ObjectGuid guid : playerList)
                {
                    Player* player = me->GetPlayer(*me, guid);
                    if (player && player->GetGUID() == target->GetGUID())
                    {
                        playerList.remove(guid);
                        break;
                    }
                }
            }

            Creature* getOtherBoss()
            {
                if (instance)
                    return instance->instance->GetCreature(instance->GetGuidData(me->GetEntry() == NPC_QIN_XI ? NPC_JAN_XI: NPC_QIN_XI));
                else
                    return NULL;
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                Creature* otherBoss = getOtherBoss();

                if (!otherBoss)
                    return;

                if (attacker != otherBoss)
                    me->DealDamage(otherBoss, damage);
                else
                    me->LowerPlayerDamageReq(damage);
            }

            void UpdateAI(uint32 diff) override
            {
                UpdateVictim();

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                // Anti bug safe check --> kill them all
                if (me->GetPositionX() > 3915.f)
                {
                    if (Creature* jan_xi = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_JAN_XI) : ObjectGuid::Empty))
                        jan_xi->AI()->DoAction(ACTION_WIPE);

                    if (Creature* qin_xi = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_QIN_XI) : ObjectGuid::Empty))
                        qin_xi->AI()->DoAction(ACTION_WIPE);

                    if (Creature* anc_mogu_machine = GetClosestCreatureWithEntry(me, NPC_ANCIENT_MOGU_MACHINE, 200.0f))
                        anc_mogu_machine->AI()->DoAction(ACTION_CONSOLE_WIPE);
                }

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BOSS_CAST_SKYBEAM:
                        {
                            // 1st halo, still invisible
                            me->CastSpell(me, SPELL_TERRACOTTA_SKYBEAM_S, false);
                            events.ScheduleEvent(EVENT_BOSS_CAST_SPAWNIN, 6000);
                            break;
                        }
                        case EVENT_BOSS_CAST_SPAWNIN:
                        {
                            // 2nd halo, still invisible
                            me->CastSpell(me, SPELL_TERRACOTTA_SPAWN, false);
                            if (me->GetEntry() == NPC_JAN_XI)
                                if (Creature* anc_mogu_machine = GetClosestCreatureWithEntry(me, NPC_ANCIENT_MOGU_MACHINE, 200.0f))
                                    anc_mogu_machine->AI()->Talk(TALK_CONSTRUCTS_APPEAR);
                            events.ScheduleEvent(EVENT_BOSS_WAIT, 500);
                            break;
                        }
                        case EVENT_BOSS_WAIT:
                        {
                            // Add weapon
                            me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 0, (me->GetEntry() == NPC_JAN_XI ? WEAPON_JAN_XI : WEAPON_QIN_XI));
                            // Become visible, but wait till the end of halos
                            me->SetDisplayId(me->GetEntry() - DISPLAY_VISIBLE);
                            // Only Qin-Xi makes the machine talk, to avoid "double voices"
                            if (me->GetEntry() == NPC_QIN_XI)
                                if (Creature* anc_mogu_machine = GetClosestCreatureWithEntry(me, NPC_ANCIENT_MOGU_MACHINE, 200.0f))
                                    anc_mogu_machine->AI()->Talk(TALK_JAN_QIN_XI);
                            events.ScheduleEvent(EVENT_BOSS_WAIT_VISIBLE, 2000);
                            break;
                        }
                        case EVENT_BOSS_WAIT_VISIBLE:
                        {
                            // Landing coords 1st boss
                            if (Creature* console = GetClosestCreatureWithEntry(me, NPC_ANCIENT_MOGU_MACHINE, 200.0f))
                            {
                                float x, y;
                                GetPositionWithDistInOrientation(me, 15.0f, me->GetAngle(console->GetPositionX(), console->GetPositionY()), x, y);
                                me->GetMotionMaster()->MoveJump(x, y, 362.19f, 20.0f, 20.0f, 2);
                                break;
                            }
                        }
                        case EVENT_BOSS_EMOTE:
                        {
                            // Turn into enemy and jump in the room
                            me->SetFaction(14);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);

                            if (Player* itr = me->FindNearestPlayer(VISIBLE_RANGE))
                                me->AI()->AttackStart(itr);

                            // --- Attacks ---
                            events.ScheduleEvent(EVENT_CHECK_MAGNETIC_ARMOR, 12 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_RESTORE_MANA, 0.5 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_SUMMON_RAGE:
                        {
                            // Jan-Xi is the only one who summons, to avoid double summon
                            if (me->GetEntry() == NPC_JAN_XI)
                            {
                                // Two Emperor's rage at each summon in 10, 4 in 25
                                std::list<uint32> alcovesPos = { 0, 1, 2, 3, 4, 5 };
                                Trinity::Containers::RandomResizeList(alcovesPos, Is25ManRaid() ? 4 : 2);

                                for (auto&& itr:alcovesPos)
                                    me->SummonCreature(NPC_EMPEROR_RAGE, EmperorRage[itr]);

                                // Summoning other add : Emperor's Strength (if sumCourage == 0) or Empreror's Courage (if sumCourage == 1)
                                uint32 delaySummon = 1000 * urand(5, 10);
                                events.ScheduleEvent(EVENT_SUMMON_STRENGTH + sumCourage, delaySummon);
                                // Changing sumCourage from 0 to 1 or from 1 to 0
                                sumCourage = 1 - (sumCourage % 2);

                                // Scheduling next summon phase
                                uint32 delay = 1000 * urand(32, 40);
                                events.ScheduleEvent(EVENT_SUMMON_RAGE, delay);
                            }
                            break;
                        }
                        case EVENT_SUMMON_STRENGTH:
                        {
                            // Choose an alcove
                            std::list<uint32> alcovesPos = { 0, 1, 2, 3, 4, 5, 6, 7 };
                            Trinity::Containers::RandomResizeList(alcovesPos, Is25ManRaid() ? 2 : 1);

                            for (auto&& itr : alcovesPos)
                                me->SummonCreature(NPC_EMPEROR_STRENGHT, EmperrorStrength[itr]);
                            break;
                        }
                        case EVENT_SUMMON_COURAGE:
                        {
                            // Choose an alcove
                            std::list<uint32> alcovesPos = { 0, 1, 2, 3, 4, 5, 6, 7 };
                            Trinity::Containers::RandomResizeList(alcovesPos, Is25ManRaid() ? 2 : 1);

                            for (auto&& itr : alcovesPos)
                                me->SummonCreature(NPC_EMPEROR_COURAGE, EmperrorStrength[itr]);
                            break;
                        }
                        /* DEVASTATING COMBO */
                        case EVENT_DEVASTATING_COMBO:
                        {
                            // Initializing Combo
                            devastatingComboPhase = 0;
                            nonArc = false;
                            // This npc`ll make our devast combo, we`ll make a visual effects
                            if (TempSummon* terracota = me->SummonCreature(NPC_TERRACOTA_BOSS, *me, TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                TerracotaBossGUID = terracota->GetGUID();
                                terracota->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);
                                DoZoneInCombat(terracota, 200.0f);
                            }

                            if (Unit* vict = me->GetVictim())
                                ArcDevastVictimGUID = vict->GetGUID();

                            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                            me->PrepareChanneledCast(me->GetOrientation());

                            // Set Turning Speed
                            me->SetSpeed(MOVE_TURN_RATE, 10.0f, true);

                            float m_ori = me->GetOrientation();

                            // initialize container that store spell of each anim type
                            ArcComboAnim.insert(std::pair<uint32, float>(SPELL_ARC_VISUAL_RIGHT,  GetOriForCurrentArcType(m_ori, -M_PI / 2)));
                            ArcComboAnim.insert(std::pair<uint32, float>(SPELL_ARC_VISUAL_LEFT,   GetOriForCurrentArcType(m_ori, M_PI / 2)));
                            ArcComboAnim.insert(std::pair<uint32, float>(SPELL_ARC_VISUAL_CENTER, GetOriForCurrentArcType(m_ori, 2 * M_PI)));
                            ArcComboAnim.insert(std::pair<uint32, float>(SPELL_STOMP,             GetOriForCurrentArcType(m_ori, 2 * M_PI)));

                            events.ScheduleEvent(EVENT_ARC_READY, 300);
                            break;
                        }
                        case EVENT_ARC_READY:
                        {
                            std::vector<uint32> ArcComboSpellStorage = { SPELL_ARC_VISUAL_LEFT, SPELL_ARC_VISUAL_RIGHT, SPELL_ARC_VISUAL_CENTER, SPELL_STOMP };

                            // Remove From Temp container value, that was used before (real random)
                            if (prevSpellId)
                                ArcComboSpellStorage.erase(std::find(ArcComboSpellStorage.begin(), ArcComboSpellStorage.end(), prevSpellId));

                            // Select new spell from updated container
                            prevSpellId = Trinity::Containers::SelectRandomContainerElement(ArcComboSpellStorage);

                            // Get Anim for selected spell and ori to cast
                            spellOri = ArcComboAnim.find(prevSpellId)->second;

                            ++devastatingComboPhase;


                            me->SetOrientation(spellOri);
                            me->UpdateOrientation(spellOri);
                            me->SetFacingTo(spellOri);
                            me->UpdatePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), spellOri);

                            if (Unit* target = ObjectAccessor::GetUnit(*me, ArcDevastVictimGUID))
                            {
                                if (target->IsAlive())
                                    me->CastSpell(target, prevSpellId, false);
                                else if (Unit* itr = ObjectAccessor::GetUnit(*me, GetAnyAlivePlayerInRange())) // without these it cause bug
                                {
                                    ArcDevastVictimGUID = itr->GetGUID();
                                    me->CastSpell(itr, prevSpellId, false);
                                }
                                else // it cause evade them if any can`t cast Devastation Arc
                                {
                                    if (Creature* jan_xi = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_JAN_XI) : ObjectGuid::Empty))
                                        jan_xi->AI()->DoAction(ACTION_WIPE);

                                    if (Creature* qin_xi = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_QIN_XI) : ObjectGuid::Empty))
                                        qin_xi->AI()->DoAction(ACTION_WIPE);
                                }
                            }
                            else if (Unit* itr = ObjectAccessor::GetUnit(*me, GetAnyAlivePlayerInRange())) // without these it cause bug
                            {
                                ArcDevastVictimGUID = itr->GetGUID();
                                me->CastSpell(itr, prevSpellId, false);
                            }
                            else // it cause evade them if any can`t cast Devastation Arc
                            {
                                if (Creature* jan_xi = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_JAN_XI) : ObjectGuid::Empty))
                                    jan_xi->AI()->DoAction(ACTION_WIPE);

                                if (Creature* qin_xi = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_QIN_XI) : ObjectGuid::Empty))
                                    qin_xi->AI()->DoAction(ACTION_WIPE);
                            }

                            // Send casting for us vehicle
                            if (Creature* Terracota = ObjectAccessor::GetCreature(*me, TerracotaBossGUID))
                                Terracota->AI()->DoAction(prevSpellId);
                            // Run!
                            break;
                        }
                        case EVENT_DEVASTATING_CANCEL:
                        {
                            me->RemoveChanneledCast(ArcDevastVictimGUID);
                            nonArc = true;
                            me->SetSpeed(MOVE_TURN_RATE, moveTurn, false);
                            // Reset attacking to normal
                            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                            events.ScheduleEvent(EVENT_RESTORE_MANA, urand(2, 4)*IN_MILLISECONDS);

                            // Remove our terracota trigger
                            if (Vehicle* Passengers = me->GetVehicleKit())
                                Passengers->RemoveAllPassengers(/*true*/);

                            if (Creature* Terracota = ObjectAccessor::GetCreature(*me, TerracotaBossGUID))
                                Terracota->DespawnOrUnsummon();

                            break;
                        }
                        case EVENT_RESTORE_MANA:
                        {
                            if (nonArc)
                                me->ModifyPower(POWER_MANA, 2);

                            uint8 manaType = IsHeroic() ? 40 : 20;
                            me->GetPower(POWER_MANA) >= manaType ? events.ScheduleEvent(EVENT_DEVASTATING_COMBO, 100) : events.ScheduleEvent(EVENT_RESTORE_MANA, IsHeroic() ? urand(1, 2) * IN_MILLISECONDS : urand(2, 4) * IN_MILLISECONDS);
                            break;
                        }
                        /* END OF DEVASTATING COMBO */
                        case EVENT_CHECK_MAGNETIC_ARMOR:
                        {
                            bool isQin = me->GetEntry() == NPC_QIN_XI;

                            if (!me->HasAura(isQin ? SPELL_MAGNETIC_ARMOR_QIN : SPELL_MAGNETIC_ARMOR_JAN))
                                DoCast(me, isQin ? SPELL_MAGNETIC_ARMOR_QIN : SPELL_MAGNETIC_ARMOR_JAN);

                            if (Unit* vict = me->GetVictim())
                            {
                                if (!vict->HasAura((isQin ? SPELL_MAGNETIZED_QIN : SPELL_MAGNETIZED_JAN)))
                                {
                                    if (Player* player = ObjectAccessor::FindPlayer(victimWithMagneticArmor))
                                        player->RemoveAurasDueToSpell(isQin ? SPELL_MAGNETIZED_QIN : SPELL_MAGNETIZED_JAN);

                                    me->AddAura(isQin ? SPELL_MAGNETIZED_QIN : SPELL_MAGNETIZED_JAN, vict);
                                    victimWithMagneticArmor = vict->GetGUID();
                                }
                            }

                            if (Player* itr = ObjectAccessor::GetPlayer(*me, victimWithMagneticArmor))
                            {
                                if (me->GetDistance2d(itr) > 16.0f)
                                {
                                    me->CastSpell(itr, isQin ? SPELL_MAGNETIC_PULL_QIN : SPELL_MAGNETIC_PULL_JAN, false);
                                    itr->GetMotionMaster()->MoveJump(me->GetPositionX() + frand(-2.5f, 2.5f), me->GetPositionY() + (-3.5f, 3.5f), me->GetPositionZ(), 35.0f, 15.0f);
                                }
                            }

                            events.ScheduleEvent(EVENT_CHECK_MAGNETIC_ARMOR, 1000);

                            break;
                        }
                        default:
                            break;
                    }
                }

                // Not in combo : attack players
                    DoMeleeAttackIfReady();
            }

            private:
                ObjectGuid GetAnyAlivePlayerInRange()
                {
                    std::list<Player*> pList;
                    GetPlayerListInGrid(pList, me, 99.0f);

                    pList.remove_if([=](Player* target) { return (target && !target->IsAlive()) || (target && target->IsGameMaster()); });

                    if (pList.empty())
                        return ObjectGuid::Empty;

                    return Trinity::Containers::SelectRandomContainerElement(pList)->GetGUID();

                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_jin_qin_xiAI(creature);
        }
};

// Emperor's Rage - 60396
// Emperor's Strength - 60397
// Emperor's Courage - 60398
class npc_woe_add_generic : public CreatureScript
{
    public:
        npc_woe_add_generic() : CreatureScript("npc_woe_add_generic") { }

        struct npc_woe_add_genericAI : public ScriptedAI
        {
            npc_woe_add_genericAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid targetGuid;
            EventMap events, nonCombatEvents;
            InstanceScript* instance;
            ObjectGuid targetGUID, smashTriggerGUID;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                me->setActive(true);
                Reset();
            }

            void Reset() override
            {
                events.Reset();

                // Won't attack
                me->SetFaction(35);
                // Invisible
                me->SetDisplayId(DISPLAY_ADD_INVISIBLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

                if (me->GetEntry() != NPC_EMPEROR_STRENGHT)
                {
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_THREAT, true);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL, true);
                }

                // Wait before casting
                nonCombatEvents.ScheduleEvent(EVENT_CAST_SKYBEAM, 1000);

                targetGuid = ObjectGuid::Empty;
                smashTriggerGUID = ObjectGuid::Empty;
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (IsHeroic())
                    me->CastSpell(me, SPELL_SUMMON_TITAN_SPARK, true);

                // Cancel aura for add which are spawning in when bosses die
                me->RemoveAllAuras();
                me->DespawnOrUnsummon(10 * IN_MILLISECONDS);

                // Interrupt our vehicle passenger if die
                if (Creature* smashTrigger = ObjectAccessor::GetCreature(*me, smashTriggerGUID))
                {
                    smashTrigger->InterruptNonMeleeSpells(true, SPELL_ENERGIZING_SMASH);
                    smashTrigger->DespawnOrUnsummon(1 * IN_MILLISECONDS);
                }

                if (me->GetEntry() == NPC_EMPEROR_RAGE)
                    if (Player* itr = ObjectAccessor::GetPlayer(*me, targetGUID))
                        itr->RemoveAurasDueToSpell(SPELL_FOCALISED_ASSAULT);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == EVENT_JUMP)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    // Don't remove Immune to PC/NPC for Rage & Stength
                    if (me->GetEntry() == NPC_EMPEROR_COURAGE)
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                    switch (me->GetEntry())
                    {
                        case NPC_EMPEROR_RAGE:
                        {
                            me->AddAura(SPELL_WITHOUT_ARMOR, me);
                            me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                            break;
                        }
                        case NPC_EMPEROR_COURAGE:
                        {
                            me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                            me->AddAura(SPELL_HALF_PLATE, me);
                            break;
                        }
                        case NPC_EMPEROR_STRENGHT:
                        {
                            me->AddAura(SPELL_FULL_PLATE, me);
                            break;
                        }
                    }

                    nonCombatEvents.ScheduleEvent(EVENT_PREPARE, 2 * IN_MILLISECONDS);
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                switch (me->GetEntry())
                {
                    case NPC_EMPEROR_STRENGHT:
                        events.ScheduleEvent(EVENT_ENERGIZING_SMASH, urand(5000, 10000));

                        if (TempSummon* eSmash = me->SummonCreature(NPC_EMPEROR_STRENGTH_TRIGGER, *me, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            smashTriggerGUID = eSmash->GetGUID();
                            eSmash->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);
                        }
                        break;
                    case NPC_EMPEROR_COURAGE:
                        events.ScheduleEvent(EVENT_IMPEDING_THRUST, 3000);
                        events.ScheduleEvent(EVENT_PHALANX_WALL, 1000);
                        break;
                }
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_LAND)
                {
                    std::list<Player*> PlayerOnTerracota;
                    GetPlayerListInGrid(PlayerOnTerracota, me, 200.0f);

                    if (PlayerOnTerracota.empty())
                        return;

                    switch (me->GetEntry())
                    {
                        case NPC_EMPEROR_RAGE:
                            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

                            PlayerOnTerracota.remove_if(RageTargetSelector());
                            PlayerOnTerracota.remove_if(TankSpecTargetSelector());
                           
                            if (!PlayerOnTerracota.empty())
                            {
                                if (Player* itr = Trinity::Containers::SelectRandomContainerElement(PlayerOnTerracota))
                                {
                                    me->AI()->AttackStart(itr);
                                    me->GetThreatManager().addThreat(itr, 10000.0f);

                                    me->CastSpell(itr, SPELL_FOCALISED_ASSAULT, true);
                                    me->GetMotionMaster()->MoveChase(itr);
                                    targetGUID = itr->GetGUID();
                                }
                            }
                            else
                            {
                                if (Player* itr = me->FindNearestPlayer(VISIBLE_RANGE))
                                {
                                    me->AI()->AttackStart(itr);
                                    me->GetThreatManager().addThreat(itr, 10000.0f);

                                    me->CastSpell(itr, SPELL_FOCALISED_ASSAULT, true);
                                    me->GetMotionMaster()->MoveChase(itr);
                                    targetGUID = itr->GetGUID();
                                }
                            }
                            break;
                        case NPC_EMPEROR_STRENGHT:
                        {
                            PlayerOnTerracota.remove_if(TankSpecTargetSelector());

                            if (!PlayerOnTerracota.empty())
                            {
                                if (Player* itr = Trinity::Containers::SelectRandomContainerElement(PlayerOnTerracota))
                                {
                                    me->AI()->AttackStart(itr);
                                    me->GetThreatManager().addThreat(itr, 300.0f);
                                }
                            }
                            else 
                                if (Player* itr = me->FindNearestPlayer(VISIBLE_RANGE))
                                {
                                    me->AI()->AttackStart(itr);
                                    me->GetThreatManager().addThreat(itr, 300.0f);
                                }

                            break;
                        }
                        case NPC_EMPEROR_COURAGE:
                        {
                            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

                            // Should take farthest tank
                            PlayerOnTerracota.remove_if(CourageTargetSelector());
                            PlayerOnTerracota.sort(Trinity::ObjectDistanceOrderPred(me));

                            if (!PlayerOnTerracota.empty())
                            {
                                if (Player* itr = PlayerOnTerracota.back())
                                {
                                    me->AI()->AttackStart(itr);
                                    me->GetThreatManager().addThreat(itr, 10000.0f);
                                }
                            }
                            else if (Player* itr = me->FindNearestPlayer(VISIBLE_RANGE))
                            {
                                me->AI()->AttackStart(itr);
                                me->GetThreatManager().addThreat(itr, 10000.0f);
                            }
                            break;
                        }
                    }

                    DoZoneInCombat();
                }
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CAST_SKYBEAM:
                        {
                            // Cast 1st halo
                            if (me->GetEntry() == NPC_EMPEROR_RAGE)
                                me->CastSpell(me, SPELL_TERRACOTTA_SKYBEAM_S, false);
                            else
                                me->CastSpell(me, SPELL_TERRACOTTA_SKYBEAM_M, false);
                            // Wait invisible
                            nonCombatEvents.ScheduleEvent(EVENT_CAST_SPAWNIN, 7000);
                            break;
                        }
                        case EVENT_CAST_SPAWNIN:
                        {
                            // Cast 2nd halo
                            me->CastSpell(me, SPELL_TERRACOTTA_SPAWN, false);

                            // Boss Emote - Not displayed if done by mob as he's invisible, so done by a random player
                            if (me->GetEntry() != NPC_EMPEROR_RAGE)
                                if (Creature* anc_mogu_machine = GetClosestCreatureWithEntry(me, NPC_ANCIENT_MOGU_MACHINE, 200.0f))
                                    anc_mogu_machine->AI()->Talk(me->GetEntry() == NPC_EMPEROR_STRENGHT ? TALK_EMPEROR_STRENGTH_ANN : TALK_EMPEROR_COURAGE_ANN);
                            // Wait invisible
                            nonCombatEvents.ScheduleEvent(EVENT_WAIT, 5000);
                            break;
                        }
                        case EVENT_WAIT:
                        {
                            // Add weapons
                            switch (me->GetEntry())
                            {
                                case NPC_EMPEROR_RAGE:
                                {
                                    me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 0, WEAPON_RAGE);
                                    me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1, WEAPON_RAGE);
                                    break;
                                }
                                case NPC_EMPEROR_STRENGHT:
                                {
                                    me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 0, WEAPON_STRENGTH);
                                    break;
                                }
                                case NPC_EMPEROR_COURAGE:
                                {
                                    me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 0, WEAPON_COURAGE_LEFT);
                                    me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1, WEAPON_COURAGE_RIGHT);
                                }
                            }
                            // Set visible
                            me->SetDisplayId(me->GetEntry() - DISPLAY_VISIBLE);
                            // Talk - Choose the right sentence according to the mob
                            int32 words = me->GetEntry() == NPC_EMPEROR_RAGE ? TALK_RAGE : (me->GetEntry() == NPC_EMPEROR_STRENGHT ? TALK_STRENGTH : TALK_COURAGE);
                            if (Creature* anc_mogu_machine = GetClosestCreatureWithEntry(me, NPC_ANCIENT_MOGU_MACHINE, 200.0f))
                                anc_mogu_machine->AI()->Talk(words);
                            // Wait til spell ends, before jumping
                            nonCombatEvents.ScheduleEvent(EVENT_WAIT_VISIBLE, 2000);
                            break;
                        }
                        case EVENT_WAIT_VISIBLE:
                        {
                            me->SetFaction(14);
                            // Landing coords
                            float x = me->GetPositionX() + (15 * cos(me->GetOrientation()));
                            float y = me->GetPositionY() + (15 * sin(me->GetOrientation()));
                            // Jump
                            me->GetMotionMaster()->MoveJump(x, y, 362.19f, 20.0f, 20.0f, EVENT_JUMP);
                            break;
                        }
                        case EVENT_PREPARE:
                        {
                            me->AI()->DoAction(ACTION_LAND);
                            break;
                        }
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId=events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PHALANX_WALL:
                            me->CastSpell(me, SPELL_PHALANX_WALL, false);
                            break;
                        case EVENT_IMPEDING_THRUST:
                            if (Unit* victim = me->GetVictim())
                                if (me->IsWithinMeleeRange(victim))
                                    me->CastSpell(victim, SPELL_IMPEDING_THRUST, false);

                            events.ScheduleEvent(EVENT_IMPEDING_THRUST, 2000);
                            break;
                        case EVENT_ENERGIZING_SMASH:
                            if (Vehicle* veh = me->GetVehicleKit())
                            {
                                if (Unit* eSmash = veh->GetPassenger(0))
                                {
                                    eSmash->CastSpell(eSmash, SPELL_ENERGIZING_SMASH, false);

                                    if (Unit* vict = me->GetVictim())
                                    {
                                        targetGUID = vict->GetGUID();
                                        me->PrepareChanneledCast(me->GetAngle(vict));
                                        DoCast(vict, SPELL_ENERGIZING_VISUAL, true);
                                    }
                                }
                            }

                            events.ScheduleEvent(EVENT_ENERGIZING_REMOVE, 2 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_ENERGIZING_SMASH, 8 * IN_MILLISECONDS);
                            break;
                        case EVENT_ENERGIZING_REMOVE:
                            me->RemoveChanneledCast(targetGUID);
                            break;
                    }
                }

                if (me->GetEntry() == NPC_EMPEROR_RAGE)
                {
                    if (Unit* RageTarget = ObjectAccessor::GetUnit(*me, targetGUID))
                    {
                        if (RageTarget->IsAlive())
                            me->GetMotionMaster()->MoveChase(RageTarget);
                        else if (Player* itr = me->FindNearestPlayer(VISIBLE_RANGE))
                            targetGUID = itr->GetGUID();
                    }
                    else if (targetGUID)
                        targetGUID = ObjectGuid::Empty;

                    DoMeleeAttackIfReady();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_woe_add_genericAI(creature);
        }
};

// Titan Spark - 60480
class npc_woe_titan_spark : public CreatureScript
{
    public:
        npc_woe_titan_spark() : CreatureScript("npc_woe_titan_spark") { }

        struct npc_woe_titan_sparkAI : public ScriptedAI
        {
            npc_woe_titan_sparkAI(Creature* creature) : ScriptedAI(creature) { }

            TaskScheduler scheduler;
            uint32 delay;
            ObjectGuid targetGuid;
            bool canExplode;

            void Reset() override
            {
                targetGuid = ObjectGuid::Empty;
                delay      = 0;
                canExplode = false;

                me->PrepareChanneledCast(me->GetOrientation());
                me->OverrideInhabitType(INHABIT_AIR);
                me->UpdateMovementFlags();

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_THREAT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL, true);

                me->CastSpell(me, SPELL_ENERGY_OF_CREATION_A, false);

                if (Unit* target = ObjectAccessor::GetUnit(*me, GetTargetGUID()))
                {
                    targetGuid = target->GetGUID();
                    DoCast(target, SPELL_FOCALISED_ENERGY, true);
                }

                me->SetSpeed(MOVE_RUN, 0.47f);

                Movement::MoveSplineInit init(me);
                init.MoveTo(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 7.8f);

                init.SetFly();
                init.SetSmooth();
                init.SetUncompressed();
                init.Launch();

                me->m_Events.Schedule(delay += me->GetSplineDuration(), 1, [this]()
                {
                    DoZoneInCombat(me, 300.0f);

                    // check our main target
                    if (Unit* target = ObjectAccessor::GetUnit(*me, targetGuid))
                    {
                        if (target->IsAlive())
                        {
                            me->RemoveChanneledCast(targetGuid);
                            me->GetThreatManager().addThreat(target, 3000.0f);
                        }
                        // If our main target not alive - too select new.
                        else if (Unit* newTarget = ObjectAccessor::GetUnit(*me, GetTargetGUID()))
                        {
                            targetGuid = newTarget->GetGUID();
                            me->RemoveChanneledCast(targetGuid);
                            me->GetThreatManager().addThreat(target, 3000.0f);
                        }
                    }
                    // If not found - select new
                    else if (Unit* target = ObjectAccessor::GetUnit(*me, GetTargetGUID()))
                    {
                        targetGuid = target->GetGUID();
                        me->RemoveChanneledCast(targetGuid);
                        me->GetThreatManager().addThreat(target, 3000.0f);
                    }

                    canExplode = true;
                });

                scheduler.Schedule(Milliseconds(500), [this](TaskContext context)
                {
                    // No any way - this should work like movement areaTrigger
                    if (canExplode)
                        if (Player* itr = me->FindNearestPlayer(7.15f))
                            me->Kill(me);

                    context.Repeat(Milliseconds(500));
                });
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->CastSpell(me, SPELL_ENERGY_OF_CREATION, false);
                me->SetCorpseDelay(3); // set delay

                // Remove aura from player
                if (Player* itr = ObjectAccessor::GetPlayer(*me, targetGuid))
                    itr->RemoveAurasDueToSpell(SPELL_FOCALISED_ENERGY);
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);
            }

            private:
                ObjectGuid GetTargetGUID()
                {
                    std::list<Player*> pList;
                    GetPlayerListInGrid(pList, me, 300.0f);
                    pList.remove_if(TankSpecTargetSelector());

                    // If not selected then return random player if it possible
                    if (pList.empty())
                    {
                        if (Player* itr = me->FindNearestPlayer(300.0f))
                            return itr->GetGUID();

                        return ObjectGuid::Empty;
                    }

                    return Trinity::Containers::SelectRandomContainerElement(pList)->GetGUID();
                }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_woe_titan_sparkAI(creature);
        }
};

// Terracota Boss - 60575
class npc_woe_terracota_boss : public CreatureScript
{
    public:
        npc_woe_terracota_boss() : CreatureScript("npc_woe_terracota_boss") { }

        struct npc_woe_terracota_bossAI : public ScriptedAI
        {
            npc_woe_terracota_bossAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            uint32 m_AnimType;

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case SPELL_ARC_VISUAL_LEFT:
                        m_AnimType = SPELL_DEVAST_ARC_LEFT;
                        break;
                    case SPELL_ARC_VISUAL_RIGHT:
                        m_AnimType = SPELL_DEVAST_ARC_RIGHT;
                        break;
                    case SPELL_ARC_VISUAL_CENTER:
                        m_AnimType = SPELL_DEVAST_ARC;
                        break;
                    default:
                        m_AnimType = 0;
                        break;
                }

                // Skip Stomp cuz he`s self-triggered animation
                if (m_AnimType)
                    me->CastSpell(me, m_AnimType, false);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_woe_terracota_bossAI(creature);
        }
};

// Ancient Mogu Machine - 60648
class npc_ancient_mogu_machine : public CreatureScript
{
    public:
        npc_ancient_mogu_machine() : CreatureScript("npc_ancient_mogu_machine") { }

        struct npc_ancient_mogu_machineAI : public ScriptedAI
        {
            npc_ancient_mogu_machineAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;
            bool activated;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);

                Reset();
            }

            void Reset() override
            {
                me->RemoveAurasDueToSpell(SPELL_TITAN_GAS_HEROIC);
                events.Reset();
                me->CombatStop(true);
                activated = false;
            }

            // Talk
            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_MOGU_ACTIVATE && !activated)
                {
                    activated = true;
                    events.ScheduleEvent(EVENT_TITAN_GAS, IsHeroic() ? 12000 : 225000);
                    events.ScheduleEvent(EVENT_CHECK_WIPE, 1 * IN_MILLISECONDS);

                    events.ScheduleEvent(EVENT_LIGHTNING_COSMETIC, 6 * IN_MILLISECONDS);
                }
                else if (actionId == ACTION_CONSOLE_WIPE)
                    Reset();
                else
                    Talk(actionId);
            }

            void AttackStart(Unit* target) override
            {
                if (target->GetTypeId() == TYPEID_PLAYER)
                    CreatureAI::AttackStart(target);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                            // Boss emote for the end of Titan Gas. Done by a player as Mogu Machine is invisible.
                        case EVENT_TITAN_GAS:
                        {
                            // Talk
                            Talk(TALK_TITAN_GAS_START);
                            Talk(TALK_TITAN_GAS_ANN);
                            me->CastSpell(CENTER_X, CENTER_Y, CENTER_Z, IsHeroic() ? SPELL_TITAN_GAS_HEROIC : SPELL_TITAN_GAS, false);
                        
                            // In Heroic Gas available all time
                            if (!IsHeroic())
                            {
                                // Reset spawning terracota while titan gas in use
                                if (Unit* QinXi = ObjectAccessor::GetUnit(*me, instance->GetGuidData(NPC_QIN_XI)))
                                    QinXi->ToCreature()->AI()->DoAction(ACTION_TITAN_GAS);
                        
                                if (Unit* JanXi = ObjectAccessor::GetUnit(*me, instance->GetGuidData(NPC_JAN_XI)))
                                    JanXi->ToCreature()->AI()->DoAction(ACTION_TITAN_GAS);
                        
                                events.ScheduleEvent(EVENT_END_TITAN_GAS, 30000);
                                events.ScheduleEvent(EVENT_TITAN_GAS, 210000);
                            }
                            break;
                        }
                        case EVENT_END_TITAN_GAS:
                        {
                            // Talk for Titan Gas ending
                            Talk(TALK_TITAN_GAS_END);
                        
                            // Set spawn terracota again
                            if (Unit* QinXi = ObjectAccessor::GetUnit(*me, instance->GetGuidData(NPC_QIN_XI)))
                                QinXi->ToCreature()->AI()->DoAction(ACTION_TITAN_GAS_END);
                        
                            if (Unit* JanXi = ObjectAccessor::GetUnit(*me, instance->GetGuidData(NPC_JAN_XI)))
                                JanXi->ToCreature()->AI()->DoAction(ACTION_TITAN_GAS_END);
                            break;
                        }
                        case EVENT_CHECK_WIPE:
                            if (instance && instance->IsWipe(81.5f, me))
                            {
                                Reset();
                        
                                if (Creature* jan_xi = instance->instance->GetCreature(me->GetInstanceScript()->GetGuidData(NPC_JAN_XI)))
                                    jan_xi->AI()->DoAction(ACTION_WIPE);
                        
                                if (Creature* qin_xi = instance->instance->GetCreature(me->GetInstanceScript()->GetGuidData(NPC_QIN_XI)))
                                    qin_xi->AI()->DoAction(ACTION_WIPE);
                        
                                break;
                            }
                            events.ScheduleEvent(EVENT_CHECK_WIPE, 1 * IN_MILLISECONDS);
                            break;
                        case EVENT_LIGHTNING_COSMETIC:
                            DoZoneInCombat();
                            me->CastSpell(me, SPELL_COSMETIC_LIGHTNING, false);
                        
                            for (auto&& pItr : instance->instance->GetPlayers())
                                if (Player* player = pItr.GetSource())
                                    if (player->IsAlive() && !player->IsGameMaster() && player->GetPositionZ() > 363.39f)
                                        player->CastSpell(player, SPELL_TERRACOTTA_JUMP, false);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ancient_mogu_machineAI(creature);
        }
};

// General Purpose Bunny JMF - 55091
class npc_general_purpose_bunnyJMF : public CreatureScript
{
    public:
        npc_general_purpose_bunnyJMF() : CreatureScript("npc_general_purpose_bunnyJMF") { }

        struct npc_general_purpose_bunnyJMFAI : public ScriptedAI
        {
            bool hasCast;

            npc_general_purpose_bunnyJMFAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
            }

            void Reset() override
            {
                me->RemoveAllAuras();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_general_purpose_bunnyJMFAI(creature);
        }
};

// Cosmetic Lightning Spell - 127732
class spell_cosmetic_lightning : public SpellScriptLoader
{
    public:
        spell_cosmetic_lightning() : SpellScriptLoader("spell_cosmetic_lightning") { }

        class spell_cosmetic_lightning_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cosmetic_lightning_SpellScript);

            void SelectTargets(std::list<WorldObject*>& targets)
            {
                if (Unit* caster = GetCaster())
                {
                    targets.clear();

                    std::list<Creature*> focus;
                    GetCreatureListWithEntryInGrid(focus, caster, NPC_GENERAL_PURPOSE_BUNNY_JMF, 500.0f);

                    focus.remove(caster->ToCreature());

                    for (auto&& cible: focus)
                    {
                        targets.push_back(cible);
                        caster->AddAura(SPELL_COSMETIC_LIGHTNING, cible);
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cosmetic_lightning_SpellScript::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cosmetic_lightning_SpellScript();
        }
};

//  Terracotta spawn visual spawn-in - 118165
class spell_terracota_spawn : public SpellScriptLoader
{
    public:
        spell_terracota_spawn() : SpellScriptLoader("spell_terracota_spawn") { }

        class spell_terracota_spawn_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_terracota_spawn_AuraScript);

            void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    caster->AddAura(SPELL_TERRACOTTA_SPAWN, GetCaster());
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_terracota_spawn_AuraScript::Apply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuracript() const
        {
            return new spell_terracota_spawn_AuraScript();
        }
};

// Magnetized (Qin-Xi) - 116818
class spell_magnetized_qin : public SpellScriptLoader
{
    public:
        spell_magnetized_qin() : SpellScriptLoader("spell_magnetized_qin") { }

        class spell_magnetized_qin_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_magnetized_qin_AuraScript);

            void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* player = GetTarget()->ToPlayer())
                {
                    player->AddAura(SPELL_MAGNETIZED_QIN, player);
                    targetGUID = player->GetGUID();
                }
            }

            private:
                ObjectGuid targetGUID;


            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_magnetized_qin_AuraScript::Apply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_magnetized_qin_AuraScript();
        }
};

// Magnetized (Jan-Xi) - 117195
class spell_magnetized_jan : public SpellScriptLoader
{
    public:
        spell_magnetized_jan() : SpellScriptLoader("spell_magnetized_jan") { }

        class spell_magnetized_jan_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_magnetized_jan_AuraScript);

            void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* player = GetTarget()->ToPlayer())
                {
                    player->AddAura(SPELL_MAGNETIZED_JAN, player);
                    targetGUID = player->GetGUID();
                }
            }

            private:
                ObjectGuid targetGUID;

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_magnetized_jan_AuraScript::Apply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_magnetized_jan_AuraScript();
        }
};

//  Devastating Arc - 117006
class spell_devastating_arc : public SpellScriptLoader
{
    public:
        spell_devastating_arc() : SpellScriptLoader("spell_devastating_arc") { }

        class spell_devastating_arc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_devastating_arc_AuraScript);

            void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    std::list<Player*> playerList;
                    GetPlayerListInGrid(playerList, caster, 25.0f);

                    for (auto&& target : playerList)
                        caster->AddAura(SPELL_DEVAST_ARC, target);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_devastating_arc_AuraScript::Apply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_devastating_arc_AuraScript();
        }
};

// Impeding Thrust - 117485
class spell_impeding_thrust : public SpellScriptLoader
{
    public:
        spell_impeding_thrust() : SpellScriptLoader("spell_impeding_thrust") { }

        class spell_impeding_thrust_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_impeding_thrust_AuraScript);

            void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* player = GetTarget()->ToPlayer())
                {
                    auto const impeding = player->GetAura(SPELL_IMPEDING_THRUST);
                    if (impeding->GetStackAmount() < 4)
                        impeding->SetStackAmount(impeding->GetStackAmount() + 1);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_impeding_thrust_AuraScript::Apply, EFFECT_0, SPELL_AURA_NONE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_impeding_thrust_AuraScript();
        }
};

struct TitanGasTargetSelector final
{
    bool operator ()(WorldObject const* object) const
    {
        if (object->GetTypeId() == TYPEID_PLAYER)
            return false;

        return true;
    }
};

// Titan gas - 116803 - triggered by Titan Gas (116779)
class spell_titan_gas final : public SpellScriptLoader
{
    public:
        spell_titan_gas() : SpellScriptLoader("spell_titan_gas") { }

        class spell_titan_gas_AuraScript final : public AuraScript
        {
            PrepareAuraScript(spell_titan_gas_AuraScript);

            void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
                    target->AddAura(SPELL_TITAN_GAS_AURA, target);
            }

            void Register() override final
            {
                OnEffectApply += AuraEffectApplyFn(spell_titan_gas_AuraScript::Apply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        class spell_titan_gas_SpellScript final : public SpellScript
        {
            PrepareSpellScript(spell_titan_gas_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if (TitanGasTargetSelector());
            }

            void Register() override final
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_titan_gas_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        AuraScript* GetAuraScript() const override final
        {
            return new spell_titan_gas_AuraScript();
        }

        SpellScript* GetSpellScript() const override final
        {
            return new spell_titan_gas_SpellScript();
        }
};

// Titan gas - 118327 - triggered by Titan Gas (116779)
class spell_titan_gas2 : public SpellScriptLoader
{
    public:
        spell_titan_gas2() : SpellScriptLoader("spell_titan_gas2") { }

        class spell_titan_gas2_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_titan_gas2_AuraScript);

            void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
                    target->AddAura(SPELL_TITAN_GAS_AURA2, target);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_titan_gas2_AuraScript::Apply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        class spell_titan_gas2_SpellScript final : public SpellScript
        {
            PrepareSpellScript(spell_titan_gas2_SpellScript);

            void FilterTargets(std::list<WorldObject*> &targets)
            {
                targets.remove_if (TitanGasTargetSelector());
            }

            void Register() override final
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_titan_gas2_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        AuraScript* GetAuraScript() const override final
        {
            return new spell_titan_gas2_AuraScript();
        }

        SpellScript* GetSpellScript() const override final
        {
            return new spell_titan_gas2_SpellScript();
        }
};

// Titan gas - 116782
class spell_titan_gas_players final : public SpellScript
{
    PrepareSpellScript(spell_titan_gas_players);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return !target->ToPlayer(); });
    }

    void Register() override final
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_titan_gas_players::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

class EnergizingSmashPredicate 
{
    public:
        EnergizingSmashPredicate(Unit* const m_owner) : _owner(m_owner) { }

        bool operator()(WorldObject* object)
        {
            // Reason why we should make it: not correct really position of vehicle
            x = _owner->GetPositionX() + (10.0f * cos(_owner->GetOrientation()));
            y = _owner->GetPositionY() + (10.0f * sin(_owner->GetOrientation()));
            pos = { x, y, _owner->GetPositionZ(), _owner->GetOrientation() };
            // increased by 1 yard pet stack
            if (Aura* energize = _owner->GetAura(SPELL_ENERGIZED))
                return object && object->ToPlayer() && object->ToPlayer()->GetExactDist2d(&pos) > (10.0f + (float)energize->GetStackAmount());

            return object && object->ToPlayer() && object->ToPlayer()->GetExactDist2d(&pos) > 10.0f;
        }

    private:
        Unit const* _owner;
        float x, y;
        Position pos;
};

// Energizing smash - 116550
class spell_energizing_smash : public SpellScript
{
    PrepareSpellScript(spell_energizing_smash);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        if (GetCaster() && GetCaster()->GetVehicleBase())
            targets.remove_if(EnergizingSmashPredicate(GetCaster()->GetVehicleBase()));

        m_targets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->SetObjectScale(caster->GetObjectScale() + 0.1f);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_energizing_smash::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_energizing_smash::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
        AfterCast += SpellCastFn(spell_energizing_smash::HandleAfterCast);
    }
};

struct FocusedTargetSelector final
{
    public:
        explicit FocusedTargetSelector(Unit* _caster) : caster(_caster) { }

        bool operator ()(WorldObject const* unit) const
        {
            if (caster->GetVictim() && caster->GetVictim()->GetTypeId() == TYPEID_PLAYER && unit == caster->GetVictim())
                return false;

            return true;
        }

    private:
        Unit* caster;
};

class spell_woe_focused final : public SpellScriptLoader
{
    public:
        spell_woe_focused() : SpellScriptLoader("spell_woe_focused") { }

        class spell_woe_focused_SpellScript final : public SpellScript
        {
            PrepareSpellScript(spell_woe_focused_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if (FocusedTargetSelector(GetCaster()));
            }

            void Register() override final
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_woe_focused_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript * GetSpellScript() const final
        {
            return new spell_woe_focused_SpellScript();
        }
};

// Devastation Arc Visual 116968, 116971, 116972
class spell_devastation_arc_visual : public SpellScriptLoader
{
    public:
        spell_devastation_arc_visual() : SpellScriptLoader("spell_devastation_arc_visual") { }

        class spell_devastation_arc_visual_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_devastation_arc_visual_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster() && GetCaster()->ToCreature())
                    GetCaster()->ToCreature()->AI()->DoAction(ACTION_DEV_ARC);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_devastation_arc_visual_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_devastation_arc_visual_AuraScript();
        }
};

// Stomp 116969
class spell_woe_stomp final : public SpellScript
{
    PrepareSpellScript(spell_woe_stomp);

    void HandleAfterCast()
    {
        if (GetCaster() && GetCaster()->ToCreature())
            GetCaster()->ToCreature()->AI()->DoAction(ACTION_DEV_ARC);
    }

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 16.0f);

            for (auto&& pItrPot : pList)
                if (!HasPlayerGotHit(caster, pItrPot->GetGUID(), targets))
                    pItrPot->CastSpell(pItrPot, SPELL_GROWING_OPPORTUNITY, true);
        }
    }

    private:
        bool HasPlayerGotHit(Unit* me, ObjectGuid targetGUID, std::list<WorldObject*>& pTargets)
        {
            if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
            {
                for (auto&& pItr : pTargets)
                    if (pItr->GetGUID() == target->GetGUID())
                        return true;
            }

            return false;
        }

    void Register() override final
    {
        AfterCast += SpellCastFn(spell_woe_stomp::HandleAfterCast);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_woe_stomp::SelectTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Energy of Creation 116805
class spell_woe_energy_of_creation final : public SpellScriptLoader
{
    public:
        spell_woe_energy_of_creation() : SpellScriptLoader("spell_woe_energy_of_creation") { }

        class spell_woe_energy_of_creation_SpellScript final : public SpellScript
        {
            PrepareSpellScript(spell_woe_energy_of_creation_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if([=](WorldObject* target) { return target && (target->ToPlayer() || target->ToUnit() && target->ToUnit()->IsPet()); });
            }

            void Register() override final
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_woe_energy_of_creation_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript * GetSpellScript() const final
        {
            return new spell_woe_energy_of_creation_SpellScript();
        }
};

// Energy of Creation Visual 127758
class spell_woe_energy_of_creation_visual final : public SpellScriptLoader
{
    public:
        spell_woe_energy_of_creation_visual() : SpellScriptLoader("spell_woe_energy_of_creation_visual") { }

        class spell_woe_energy_of_creation_visual_SpellScript final : public SpellScript
        {
            PrepareSpellScript(spell_woe_energy_of_creation_visual_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if([=](WorldObject* target) { return target && (target->ToPlayer() || target->ToUnit() && target->ToUnit()->IsPet()); });
            }

            void Register() override final
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_woe_energy_of_creation_visual_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript * GetSpellScript() const final
        {
            return new spell_woe_energy_of_creation_visual_SpellScript();
        }
};

// Devastating Arc Effect 116835
class spell_devastating_arc_eff : public SpellScript
{
    PrepareSpellScript(spell_devastating_arc_eff);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 16.0f);

            for (auto&& pItrPot : pList)
                if (!HasPlayerGotHit(caster, pItrPot->GetGUID(), targets))
                    pItrPot->CastSpell(pItrPot, SPELL_GROWING_OPPORTUNITY, true);
        }
    }

    private:
        bool HasPlayerGotHit(Unit* me, ObjectGuid targetGUID, std::list<WorldObject*>& pTargets)
        {
            if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
            {
                for (auto&& pItr : pTargets)
                    if (pItr->GetGUID() == target->GetGUID())
                        return true;
            }

            return false;
        }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_devastating_arc_eff::SelectTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_WITH_OFFSET);
    }
};

// Growing Opportunity 117854
class spell_growing_opportunity : public SpellScript
{
    PrepareSpellScript(spell_growing_opportunity);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            if (InstanceScript* instance = target->GetInstanceScript())
            {
                if (Aura* gOpp = target->GetAura(SPELL_GROWING_OPPORTUNITY))
                {
                    uint32 getReqStuck = instance->instance->IsHeroic() ? 9 : 4;

                    if (gOpp->GetStackAmount() > getReqStuck)
                    {
                        target->RemoveAurasDueToSpell(SPELL_GROWING_OPPORTUNITY);
                        target->CastSpell(target, SPELL_OPPORTUNISTIC_STRIKE, true);
                    }
                }
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_growing_opportunity::HandleEffectHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Ancient Control Console - 211584
class go_ancien_control_console : public GameObjectScript
{
    public:
        go_ancien_control_console() : GameObjectScript("go_ancien_control_console") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (!instance->CheckRequiredBosses(DATA_WILL_OF_EMPEROR, player))
                    return false;

                if (Creature* jan_xi = instance->instance->GetCreature(instance->GetGuidData(NPC_JAN_XI)))
                    jan_xi->AI()->DoAction(ACTION_ACTIVATE);

                if (Creature* qin_xi = instance->instance->GetCreature(instance->GetGuidData(NPC_QIN_XI)))
                    qin_xi->AI()->DoAction(ACTION_ACTIVATE);

                if (Creature* console = GetClosestCreatureWithEntry(go, NPC_ANCIENT_MOGU_MACHINE, 200.0f))
                {
                    console->AI()->Talk(TALK_TO_LOWER_LEVEL);
                    console->SetReactState(REACT_PASSIVE);
                    console->setActive(true);

                    uint32 delay = 0;
                    console->m_Events.Schedule(delay += 6000, 1, [this, instance, console]()
                    {
                        console->AI()->DoAction(ACTION_MOGU_ACTIVATE);
                        instance->SetBossState(DATA_WILL_OF_EMPEROR, IN_PROGRESS);
                    });
                }
            }

            return false;
        }
};

// Achievement - 6455
class achievement_show_me_you_moves : public AchievementCriteriaScript
{
    public:
        achievement_show_me_you_moves() : AchievementCriteriaScript("achievement_show_me_you_moves") { }

        bool OnCheck(Player* player, Unit* /*target*/) override
        {
            if (player->GetInstanceScript()->GetData(ACHIEVE))
                return true;

            return false;
        }
};

void AddSC_boss_will_of_emperor()
{
    new boss_jin_qin_xi();
    new npc_woe_add_generic();
    new npc_woe_titan_spark();
    new npc_woe_terracota_boss();
    new npc_general_purpose_bunnyJMF();
    new npc_ancient_mogu_machine();
    new spell_cosmetic_lightning();
    new spell_terracota_spawn();
    new spell_devastating_arc();
    new spell_impeding_thrust();
    new spell_magnetized_jan();
    new spell_magnetized_qin();
    new spell_titan_gas();
    new spell_titan_gas2();
    new spell_script<spell_titan_gas_players>("spell_titan_gas_players");
    new spell_script<spell_energizing_smash>("spell_energizing_smash");
    new spell_woe_focused();
    new spell_devastation_arc_visual();
    new spell_script<spell_woe_stomp>("spell_woe_stomp");
    new spell_woe_energy_of_creation();
    new spell_woe_energy_of_creation_visual();
    new spell_script<spell_devastating_arc_eff>("spell_devastating_arc_eff");
    new spell_script<spell_growing_opportunity>("spell_growing_opportunity");
    new go_ancien_control_console();
    new achievement_show_me_you_moves();
}
