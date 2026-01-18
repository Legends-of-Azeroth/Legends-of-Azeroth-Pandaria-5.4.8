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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "MapManager.h"
#include "Spell.h"
#include "Vehicle.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CreatureTextMgr.h"
#include "Unit.h"
#include "Player.h"
#include "Creature.h"
#include "InstanceScript.h"
#include "Map.h"
#include "VehicleDefines.h"
#include "SpellInfo.h"
#include "MoveSplineInit.h"
#include "siege_of_orgrimmar.h"

enum Yells
{
    // Amalgam of Corruption
    SAY_NORUSHEN_AGGRO_1         = 7,  // Very well, I will create a field to keep your corruption quarantined.
    SAY_NORUSHEN_AGGRO_2         = 8,  // Prove yourselves worthy and I will let you pass.
    SAY_NORUSHEN_AGGRO_3         = 9,  // The light cleanses; but it is not gentle. Gird yourselves and be purified!
    SAY_NORUSHEN_WIPE            = 13  // The light will not suffer your taint.
};

enum Spells
{
    // Norushen
    SPELL_TELEPORT_PLAYERS       = 145188,
    SPELL_SHA_PRESUMMON          = 145143, // Triggers 145144 visuals and some other stuff.
    SPELL_SHA_SUMMON             = 145149, // Summon spell.

    // Amalgam of Corruption
    SPELL_AMALGAM                = 145118, // Players with Corruption inflict less damage to this creature. 7 mins duration -> Berserk.

    SPELL_CORRUPTION_BAR         = 144421, // Apply bar + Aura 303 (-50%) dmg decrease to Amalgamation + Periodic Dummy (Effect 2) - 1 second.
    SPELL_CORRUPTION_BAR_HIGH    = 147800, // 50% less damage inflicted to the Amalgam of Corruption. Periodic dummy (Effect 0) - 1 second + Aura 303 (- 50%).

    SPELL_DESPAIR                = 145725, // In LFR, there are no Purifying Light orbs and players are randomly selected to do the trial. This is their aura.

    SPELL_UNLEASHED_ANGER_PRECST = 145214,
    SPELL_UNLEASHED_ANGER        = 145216, // Dummy 0.8 second cast, linked to above 145214 (no usage).
    SPELL_UNLEASHED_ANGER_DAMAGE = 145212, // Cast Time + Damage. This is the used spell.

    SPELL_SELF_DOUBT             = 146124, // Stacks, Infected mechanic + 50% per app.
    SPELL_FUSION                 = 145132,

    SPELL_ICY_FEAR_AURA          = 145733, // Periodic damage aura.
    SPELL_ICY_FEAR_DAMAGE        = 145735, // Damage triggered by above, calculation script.

    SPELL_FRAYED                 = 146179, // Dummy visual. Effect is handled through script.
    SPELL_UNLEASH_CORRUPTION_MIS = 145769, // Missile trigger.
    SPELL_UNLEASH_CORRUPTION_SUM = 145007, // Triggered by above, spawns the Manifestation of Corruption NPC.

    SPELL_UNCHECKED_CORRUPTION   = 145679,

    // Blind Hatred
    SPELL_BLIND_HATRED           = 145226, // Actual channeled spell, triggers 145573 periodic dummy check.
    SPELL_BLIND_HATRED_NPC_VIS   = 145571, // Sha visual on far end npc.
    SPELL_BLIND_HATRED_BEAM_VIS  = 145613, // Beam visual boss on far end npc.
    SPELL_BLIND_HATRED_DMG       = 145227, // Damage for players between boss - far end npc.
    SPELL_BLIND_HATRED_PERIODIC  = 145573,

    // Foul Link
    SPELL_UNLEASHED_ESSENCE      = 146173, // Casted by boss on Essence in Normal realm spawn. SE (Effect 0) for SPELL_FOUL_LINK_ADDS. Spawn Effect.
    SPELL_UNLEASHED_MANIFEST     = 146174, // Casted by boss on Manifestation in Normal realm spawn. SE (Effect 0). Spawn Effect. Anim replacements. Eff 160.

    SPELL_FOUL_LINK_TRIGGER      = 149189, // Triggered by Unleashed spell. Applies dummy aura, visual.
    SPELL_FOUL_LINK_ADDS         = 148974, // Applies Share Damage aura 300 between Amalgamation and Adds. Casted Boss -> Adds.

    // Creatures
    SPELL_SPAWN_ESSENCE_N        = 144490, // Spawn effect + trigger 145006 summoning Normal realm NPC_ESSENCE_OF_CORRUPTION_N. Creates Areatrigger 1081 (NN). 145768
    SPELL_SPAWN_MANIF_N          = 144491, // Spawn effect + trigger 145007 summoning Normal realm NPC_MANIFEST_OF_CORRUPTION_N. Creates Areatrigger 1082 (NN). 145769

    SPELL_MIS_SPAWN_ESSENCE_N    = 145768, // Missile. Actual spell to use.
    SPELL_MIS_SPAWN_MANIF_N      = 145769, // Missile. Actual spell to use.

    SPELL_SPAWN_ESSENCE_L        = 144733, // Summons Look Within realm NPC_ESSENCE_OF_CORRUPTION_L.
    SPELL_SPAWN_MANIF_L          = 144739, // Summons Look Within realm NPC_MANIFEST_OF_CORRUPTION_L.

    SPELL_SPAWN_TITANIC_CORR     = 144848, // Summons Look Within realm NPC_TITANIC_CORRUPTION.
    SPELL_SPAWN_GREATER_CORR     = 144980, // Summons Look Within realm NPC_GREATER_CORRUPTION.

    SPELL_QUARANTINE_MEASURES    = 145779, // Instakill in 500yd + some Dummy triggering.

    // Purifying Light (seems like 16 phases by one time)
    SPELL_LOOK_WITHIN_1          = 146827, // 1 minute duration and timer to stand in realm. Phase, Dummy (Effect 1), Screen Effect.
    SPELL_LOOK_WITHIN_2          = 144724,
    SPELL_LOOK_WITHIN_3          = 144725,
    SPELL_LOOK_WITHIN_4          = 144726,
    SPELL_LOOK_WITHIN_5          = 144727,
    SPELL_LOOK_WITHIN_6          = 146826,
    SPELL_LOOK_WITHIN_7          = 146828,
    SPELL_LOOK_WITHIN_8          = 146829,
    SPELL_LOOK_WITHIN_9          = 146830,
    SPELL_LOOK_WITHIN_10         = 146831,
    SPELL_LOOK_WITHIN_11         = 146832,
    SPELL_LOOK_WITHIN_12         = 146833,
    SPELL_LOOK_WITHIN_13         = 146834,
    SPELL_LOOK_WITHIN_14         = 146835,
    SPELL_LOOK_WITHIN_15         = 146836,
    SPELL_LOOK_WITHIN_16         = 146837,
    SPELL_LOOK_WITHIN_PERIODIC   = 144717, // Triggered by above. NPC visual.

    SPELL_TEST_OF_SERENITY_DPS   = 144849, // DPS spec test Look Within realm aura.
    SPELL_TEST_OF_RELIANCE_HEAL  = 144850, // Healer spec test Look Within realm aura.
    SPELL_TEST_OF_CONFIDENCE_TK  = 144851, // Tank spec test Look Within realm aura.

    // Trial adds - General.

    SPELL_MANIFEST_SPAWN_VIS     = 144778, // Spell used for Emerge visual.
    SPELL_CLEANSE_TRIAL          = 147657, // Used by Greater Corruption and Titanic Corruption.
    SPELL_PURIFIED_DMG_IMMUNE    = 146022, // Casted on the player when he leaves Look Within realm.
    SPELL_PURIFIED_DMG_INCREASE  = 144452, // Stays on the player from 0 - 25% Corruption, aura spell.

    SPELL_LINGERING_CORRUPTION   = 144514,
    SPELL_DISHEARTENING_LAUGH    = 146707, // Periodic damage aura apply, dmg each 2 s.
    SPELL_BOTTOMLESS_PIT_DUMMY   = 146701, // Dummy cast spell to create the Areatrigger.
    SPELL_BOTTOMLESS_PIT_EFF     = 146705, // To Players
    SPELL_BOTTOMLESS_PIT_AT      = 146793, // Creates Areatrigger 1257.
    SPELL_BOTTOMLESS_PIT_DMG     = 146703, // Applies periodic damage each 1 second.
    SPELL_CORRUPTION             = 144639,
    SPELL_HURL_CORRUPTION        = 144649, // Must be interrupted or tank dies.
    SPELL_BURST_OF_CORRUPTION    = 144654,

    SPELL_PIERCING_CORRUPTION    = 144657,
    SPELL_TITANIC_SMASH          = 144628,
    SPELL_CLEANSE_MANIF_L        = 144450, // Used by the manifestation when it dies, on the attacking player facing the trial.

    SPELL_TEAR_REALITY           = 144482, // Cone weapon dmg %. Nice visual :).

    // 2. For NPC_MANIFEST_OF_CORRUPTION_N:
    SPELL_BURST_OF_ANGER         = 147082,

    // Spawns NPC_RESIDUAL_CORRUPTION when it dies
    SPELL_CLEANSE_ESSENCE_L      = 144449, // Used by the essence when it dies, on the attacking player facing the trial.
    SPELL_ESSENCE_OF_CORRUPTION  = 148452, // Spawn visual, Stealth Detection, triggers Dark Bulwark.
    SPELL_DARK_BULWARK           = 149601, // Block / Parry / Dodge increase by 100% from the front.

    SPELL_EXPEL_CORRUPTION       = 144548, // Effect 0 Dummy. Effect 1 creates moving Areatrigger 1106.
    SPELL_EXPEL_CORRUPTION_REALM = 145064,
    SPELL_EXPELLED_CORRUPTION_EFF = 144547, // Damage to the target in the Expelled Corruption's path.

    SPELL_RESIDUAL_CORUPTION     = 145052, // Gives Corruption and triggers below.
    SPELL_RESIDUAL_CORRUPTION_A  = 145074, // Cast time, spawn effect on NPC and adds periodic dmg aura. Creates Areatrigger 1107, despawns NPC @ touch + 25 C.
    SPELL_RESIDUAL_CORRUPTION_D  = 145073,  // Damage to all players.

    // Friendly in Heal Phase
    SPELL_FIRE_BALL              = 144522,
    SPELL_THREATING_STRIKE       = 144527,
    SPELL_DAMAGE_DEALER          = 144521,

    // Misc
    SPELL_NONE_SHALL_PASS_ACHIEV = 146880,
};

enum Events
{
    // Norushen / General
    EVENT_INTRO_1                = 1,
    EVENT_INTRO_2,
    EVENT_INTRO_3,
    EVENT_INTRO_4,
    EVENT_INTRO_5,
    EVENT_INTRO_6,
    EVENT_INTRO_7,
    EVENT_INTRO_8,
    EVENT_INTRO_9,
    EVENT_INTRO_10,
    EVENT_INTRO_11,
    EVENT_INTRO_DONE,
    EVENT_ENCOUNTER_1,
    EVENT_ENCOUNTER_2,
    EVENT_ENCOUNTER_3,
    EVENT_START_COMBAT,
    EVENT_START_OUTRO,
    EVENT_OUTRO_SAY_OPEN_DOOR,
    EVENT_SPAWN_NORUSHEN_PRIDE,

    // Amalgam of Corruption
    EVENT_UNLEASHED_ANGER,
    EVENT_BLIND_HATRED,
    EVENT_CHECK_UNCHECKED_CORRUPTION,
    EVENT_SAFETY_MEASURES,
    EVENT_CHECK_EVADE,

    EVENT_SPAWN_PURIF_LIGHT_ORB,

    // Lorewalker Cho
    EVENT_CHO_MOVE_1,
    EVENT_CHO_MOVE_2,
    EVENT_CHO_MOVE_3,
    EVENT_CHO_MOVE_4,
    EVENT_CHO_MOVE_5,

    // Manifestation of Corruption
    EVENT_TEAR_REALITY,
    EVENT_BURST_OF_ANGER,

    // Essence of Corruption
    EVENT_EXPEL_CORRUPTION,

    // Greater Corruption
    EVENT_DISHEARTENING_LAUGH,
    EVENT_LINGERING_CORRUPTION,
    EVENT_BOTTOMLESS_PIT,

    // Titanic Corruption.
    EVENT_CORRUPTION,
    EVENT_HURL_CORRUPTION,
    EVENT_BURST_OF_CORRUPTION,
    EVENT_PIERCING_CORRUPTION,
    EVENT_TITANIC_SMASH,
    EVENT_ADDITIONAL_HURL,

    // Blind Hatred.
    EVENT_MOVE_CIRCLE,
    EVENT_HATRED_TARGET_CHECK,

    // Helpers
    EVENT_FIRE_BALL,
    EVENT_THREATING_STRIKE,
};

enum Actions
{
    // Norushen / General.
    ACTION_START_NORUSHEN_INTRO  = 1, // Intro with Norushen.
    ACTION_START_LOREWALKER_INTRO,    // Intro with Lorewalker Cho.
    ACTION_START_EVENT,               // Gossip select.
    ACTION_RESET_EVENT,               // Encounter failed / other reasons.
    ACTION_FINISHED_EVENT,            // Encounter succedeed.
    ACTION_HEAL_CHALLENGE_FAIL,

    // Quarantine Measures.
    ACTION_QUARANTINE_MEASURES
};

/*
    Players start the encounter with 75 points of Corruption. While at this level of Corruption they will inflict 50% less damage to the Amalgam of Corruption.
    When players reach 0 points of Corruption remaining, they become Purified and will inflict 25% more damage to the Amalgam. 
    With each 25 corruption their damage decreases by 25%.
*/
enum CorruptionLevels
{
    CORRUPTION_LEVEL_NONE =   0, // Purified, 125% damage done.
    CORRUPTION_LEVEL_LOW  =  25, // Purified removal, 100% damage done.
    CORRUPTION_LEVEL_MED  =  50, // 75% damage done.
    CORRUPTION_LEVEL_HIGH =  75, // Encounter start, 50% damage done.
    CORRUPTION_LEVEL_FULL = 100  // No side-effect, but 0 damage done.
};

enum MovementPoints
{
    // Lorewalker Cho (Intro).
    POINT_CHO_MOVE_1             = 1,
    POINT_CHO_MOVE_2             = 2,
    POINT_CHO_MOVE_3             = 3,
    POINT_CHO_MOVE_4             = 4,
    POINT_CHO_MOVE_5             = 5,

    // Norushen (Outro).
    POINT_NORUSHEN_OUTRO_MID     = 1
};

enum LookWithinRealmsCount
{
    // Max. 20% of the players at once, depending on difficulty.
    LW_REALM_COUNT_10MAN         = 2,
    LW_REALM_COUNT_25MAN         = 5,
};

const std::vector<uint32> realthWithin =
{
    SPELL_LOOK_WITHIN_1,
    SPELL_LOOK_WITHIN_2,
    SPELL_LOOK_WITHIN_3,
    SPELL_LOOK_WITHIN_4,
    SPELL_LOOK_WITHIN_5,
    SPELL_LOOK_WITHIN_6,
    SPELL_LOOK_WITHIN_7,
    SPELL_LOOK_WITHIN_8,
    SPELL_LOOK_WITHIN_9,
    SPELL_LOOK_WITHIN_10,
    SPELL_LOOK_WITHIN_11,
    SPELL_LOOK_WITHIN_12,
    SPELL_LOOK_WITHIN_13,
    SPELL_LOOK_WITHIN_14,
    SPELL_LOOK_WITHIN_15,
    SPELL_LOOK_WITHIN_16,
};

const std::map<uint32, uint32> invTestAmalgamType =
{
    { NPC_MANIFEST_OF_CORRUPTION_L, SPELL_MIS_SPAWN_MANIF_N   },
    { NPC_ESSENCE_OF_CORRUPTION_L,  SPELL_MIS_SPAWN_ESSENCE_N },
};

// values confirmed by sniffs (not sure, in heroic maybe changed)
const std::map<uint32, std::array<int32, 2>> invPurificationType =
{
    { ROLES_DPS,    { 3, 10  } },
    { ROLES_TANK,   { 1, -15 } },
    { ROLES_HEALER, { 0, 30  } },
};

// Norushen 71965.
class boss_norushen : public CreatureScript
{
    public:
        boss_norushen() : CreatureScript("boss_norushen") { }

        struct boss_norushenAI : public BossAI
        {
            boss_norushenAI(Creature* creature) : BossAI(creature, DATA_NORUSHEN)
            {
                choSummoned      = false;
                introStarted     = false;
                introDone        = false;
            }

            bool choSummoned;
            bool introStarted;
            bool introDone;
            bool eventStarted;

            void Reset() override
            {
                _Reset();

                events.Reset();
                summons.DespawnAll();

                eventStarted = false; // Actual event.

                if (instance && instance->GetData(DATA_NORUSHEN_PRE_EVENT_2) == DONE)
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->setActive(true);

                // Boss goes to combat aprox. 2 seconds later.
                if (summon->GetEntry() != NPC_AMALGAM_OF_CORRUPTION && summon->GetEntry() != NPC_LOREWALKER_CHO_NORUSHEN && summon->GetEntry() != NPC_NORUSHEN_2)
                    summon->SetInCombatWithZone();
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_NORUSHEN_INTRO:
                        events.ScheduleEvent(EVENT_INTRO_1, 100);
                        break;
                    case ACTION_START_EVENT:
                        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        events.ScheduleEvent(EVENT_ENCOUNTER_1, 100);
                        break;
                    case ACTION_RESET_EVENT:
                        EnterEvadeMode();
                        break;
                    case ACTION_FINISHED_EVENT:
                        events.Reset();
                        scheduler.CancelAll();
                        me->DeleteThreatList();
                        me->CombatStop(true);

                        if (instance)
                        {
                            instance->SetBossState(DATA_NORUSHEN, DONE);
                            instance->SaveToDB();
                        }
                        events.ScheduleEvent(EVENT_START_OUTRO, 100);
                        break;
                    case ACTION_START_INTRO:
                        if (instance && instance->GetData(DATA_NORUSHEN_PRE_EVENT_2) == DONE)
                            break;

                        if (instance)
                            instance->SetData(DATA_NORUSHEN_PRE_EVENT_2, DONE);

                        me->SummonCreature(NPC_LOREWALKER_CHO_NORUSHEN, choPreNorushenEvent[0], TEMPSUMMON_MANUAL_DESPAWN);
                        break;
                }
            }

            void EnterEvadeMode() override
            {
                Talk(SAY_NORUSHEN_WIPE);

                me->RemoveAllAuras();

                Reset();
                me->DeleteThreatList();
                me->CombatStop(true);

                me->GetMotionMaster()->MovementExpired();

                Movement::MoveSplineInit init(me);
                for (auto itr : norushenHomePath)
                    init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                init.SetUncompressed();
                init.Launch();

                scheduler
                    .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                {
                    me->SetFacingTo(Position::NormalizeOrientation(me->GetOrientation() - M_PI));
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                });

                if (instance)
                {
                    instance->SetBossState(DATA_NORUSHEN, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTION_BAR);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case POINT_NORUSHEN_OUTRO_MID: // Middle reached.
                        events.ScheduleEvent(EVENT_SPAWN_NORUSHEN_PRIDE, 100);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);
                scheduler.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO_DONE:
                            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            introDone = true;
                            break;
                        // Gossip selected, actual event starts.
                        case EVENT_ENCOUNTER_1:
                            Talk(SAY_NORUSHEN_AGGRO_1);
                            me->NearTeleportTo(choPreNorushenEvent[1].GetPositionX(), choPreNorushenEvent[1].GetPositionY(), choPreNorushenEvent[1].GetPositionZ(), choPreNorushenEvent[1].GetOrientation());
                            eventStarted = true;

                            if (instance)
                                instance->DoCastSpellOnPlayers(SPELL_TELEPORT_PLAYERS); // Teleport the players to the middle.

                            events.ScheduleEvent(EVENT_ENCOUNTER_2, 8000);
                            break;
                        case EVENT_ENCOUNTER_2:
                            Talk(SAY_NORUSHEN_AGGRO_2);

                            if (instance)
                                instance->SetBossState(DATA_NORUSHEN, IN_PROGRESS);

                            DoCast(me, SPELL_SHA_PRESUMMON);
                            events.ScheduleEvent(EVENT_ENCOUNTER_3, 10200);
                            break;
                        case EVENT_ENCOUNTER_3:
                            Talk(SAY_NORUSHEN_AGGRO_3);
                            DoCast(me, SPELL_SHA_SUMMON);
                            events.ScheduleEvent(EVENT_START_COMBAT, 6000);
                            break;
                        case EVENT_START_COMBAT:
                            if (Creature* amalgam = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
                            {
                                amalgam->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                                amalgam->SetReactState(REACT_AGGRESSIVE);
                                amalgam->SetInCombatWithZone();
                            }
                            break;
                        // Outro.
                        case EVENT_START_OUTRO:
                            if (Creature* quarantinMeasure = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_QUARANTINE_MEASURES) : ObjectGuid::Empty))
                                me->GetMotionMaster()->MovePoint(POINT_NORUSHEN_OUTRO_MID, *quarantinMeasure); // Move mid and talk.
                            break;
                        case EVENT_SPAWN_NORUSHEN_PRIDE:
                            me->SummonCreature(NPC_NORUSHEN_2, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
                            me->DespawnOrUnsummon();
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            player->ADD_GOSSIP_ITEM_DB(15762, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();

            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF + 1:
                    creature->AI()->DoAction(ACTION_START_EVENT);
                    creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    break;
                default:
                    break;
            }

            return true;
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_norushenAI(creature);
        }
};

// Amalgam of Corruption 72276.
class boss_amalgam_of_corruption : public CreatureScript
{
    public:
        boss_amalgam_of_corruption() : CreatureScript("boss_amalgam_of_corruption") { }

        struct boss_amalgam_of_corruptionAI : public ScriptedAI
        {
            boss_amalgam_of_corruptionAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                instance = me->GetInstanceScript();
            }

            EventMap events, berserkerEvents;
            InstanceScript* instance;
            SummonList summons;
            std::map<uint8, uint8> invOrbPosType;
            std::map <uint32, std::array<uint32, 2>> orbDictionary;
            float x, y;
            int32 lastReceive;
            uint32 hasAchieved;

            uint8 GetLookWithinRealmsCount()
            {
                return Is25ManRaid() ? LW_REALM_COUNT_25MAN : LW_REALM_COUNT_10MAN;
            }

            void Reset() override
            {
                SetCombatMovement(false);
                events.Reset();
                berserkerEvents.Reset();
                summons.DespawnAll();
                x = 0.0f; y = 0.0f;
                lastReceive = 40;
                hasAchieved = 1;

                invOrbPosType.clear();
                orbDictionary.clear();

                for (uint8 i = 0; i < 16; i++)
                    invOrbPosType.insert(std::pair<uint8, uint8>(i + 1, 0));

                uint32 phaseExp = 1;
                for (auto&& itr : realthWithin)
                {
                    orbDictionary.insert(std::pair<uint32, std::array<uint32, 2>>(itr, { uint32(1 << phaseExp), 0 }));
                    phaseExp++;
                }
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                Reset();
                DoCast(me, SPELL_AMALGAM); // Emerge + aura.

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                me->SetReactState(REACT_PASSIVE);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (Creature* quarantinMeasure = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_QUARANTINE_MEASURES) : ObjectGuid::Empty))
                    quarantinMeasure->SetInCombatWithZone();

                if (Creature* norushen = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_NORUSHEN) : ObjectGuid::Empty))
                    norushen->SetInCombatWithZone();

                DoCast(me, SPELL_ICY_FEAR_AURA, true);
                me->AddAura(SPELL_AMALGAM, me);

                uint8 orbSummonCount = GetLookWithinRealmsCount();

                if (instance && instance->GetData(DATA_FLEX))
                {
                    uint32 totalPlayersCount = instance->instance->GetPlayersCountExceptGMs();

                    if (totalPlayersCount > 14)
                        orbSummonCount++;

                    if (totalPlayersCount > 19)
                        orbSummonCount++;

                    if (totalPlayersCount > 24)
                        orbSummonCount++;
                }

                if (IsHeroic())
                    orbSummonCount += Is25ManRaid() ? 5 : 3;

                HandleSummonPurifyingOrbs(orbSummonCount);

                if (instance)
                {
                    instance->DoAddAuraOnPlayers(SPELL_CORRUPTION_BAR);
                    instance->DoSetAlternatePowerOnPlayers(CORRUPTION_LEVEL_HIGH);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me); // Add.
                }

                events.ScheduleEvent(EVENT_UNLEASHED_ANGER, 11000);
                events.ScheduleEvent(EVENT_BLIND_HATRED, 25000);
                events.ScheduleEvent(EVENT_CHECK_UNCHECKED_CORRUPTION, 6000);

                berserkerEvents.ScheduleEvent(EVENT_SPAWN_PURIF_LIGHT_ORB, 30 * IN_MILLISECONDS);
                berserkerEvents.ScheduleEvent(EVENT_SAFETY_MEASURES, 7 * MINUTE * IN_MILLISECONDS); // 7 minute berserk.
                berserkerEvents.ScheduleEvent(EVENT_CHECK_EVADE, 1 * IN_MILLISECONDS);

                me->setActive(true, ActiveFlags::InCombat);
                DoZoneInCombat();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->setActive(true);

                summon->SetInCombatWithZone();
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case TYPE_ORB_POS:
                        invOrbPosType.find(data)->second = 0;
                        break;
                    case TYPE_LIGHT_ORB:
                        orbDictionary.find(data)->second[1] = 1;
                        break;
                    case TYPE_LIGHT_ORB_SLOT:
                        orbDictionary.find(data)->second[1] = 0;
                        break;
                    case TYPE_NONE_SHALL_PASS:
                        hasAchieved = data;
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case TYPE_LIGHT_ORB:
                    {
                        for (auto&& itr : orbDictionary)
                            if (!itr.second[1])
                                return itr.first;
                    }
                    case TYPE_NONE_SHALL_PASS:
                        return hasAchieved;
                    default:
                        return orbDictionary.find(type)->second[0];
                }
                return 0;
            }

            void KilledUnit(Unit* victim) override { }

            void EnterEvadeMode() override
            {
                me->RemoveAllAuras();
                me->RemoveAllAreasTrigger();

                berserkerEvents.Reset();
                me->DeleteThreatList();
                me->CombatStop(true);
                summons.DespawnAll();

                _EnterEvadeMode();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTION_BAR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PURIFIED_DMG_INCREASE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SELF_DOUBT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TEST_OF_SERENITY_DPS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TEST_OF_RELIANCE_HEAL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TEST_OF_CONFIDENCE_TK);

                    for (auto&& itr : realthWithin)
                        instance->DoRemoveAurasDueToSpellOnPlayers(itr);

                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    if (Creature* quarantinMeasure = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_QUARANTINE_MEASURES)))
                    {
                        quarantinMeasure->DeleteThreatList();
                        quarantinMeasure->CombatStop(true);
                    }

                    // Restore Phase Mask
                    for (auto&& itr : instance->instance->GetPlayers())
                        if (Player* player = itr.GetSource())
                            player->SetPhaseMask(1, true);

                    instance->DoSetAlternatePowerOnPlayers(0);
                }

                if (Creature* norushen = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_NORUSHEN) : ObjectGuid::Empty))
                    norushen->AI()->DoAction(ACTION_RESET_EVENT);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->RemoveAllAreasTrigger();

                if (instance)
                {
                    if (GetData(TYPE_NONE_SHALL_PASS))
                        instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2, SPELL_NONE_SHALL_PASS_ACHIEV);

                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTION_BAR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PURIFIED_DMG_INCREASE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SELF_DOUBT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TEST_OF_SERENITY_DPS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TEST_OF_RELIANCE_HEAL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TEST_OF_CONFIDENCE_TK);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    for (auto&& itr : realthWithin)
                        instance->DoRemoveAurasDueToSpellOnPlayers(itr);

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove.

                    if (Creature* quarantinMeasure = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_QUARANTINE_MEASURES)))
                    {
                        quarantinMeasure->DeleteThreatList();
                        quarantinMeasure->CombatStop(true);
                    }

                    // Restore Phase Mask
                    for (auto&& itr : instance->instance->GetPlayers())
                        if (Player* player = itr.GetSource())
                            player->SetPhaseMask(1, true);

                    instance->DoSetAlternatePowerOnPlayers(0);
                }

                summons.DespawnAll();
                events.Reset();
                berserkerEvents.Reset();

                if (Creature* norushen = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_NORUSHEN) : ObjectGuid::Empty))
                    norushen->AI()->DoAction(ACTION_FINISHED_EVENT);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                // Handle Frayed aura addition and mechanic.
                if (me->HealthBelowPct(50) && !me->HasAura(SPELL_FRAYED))
                {
                    me->AddAura(SPELL_FRAYED, me);
                    DoCast(me, SPELL_UNLEASH_CORRUPTION_MIS, true);
                }

                if (me->HasAura(SPELL_FRAYED) && me->HealthBelowPct(lastReceive) && lastReceive > 9)
                {
                    lastReceive -= 10;
                    DoCast(me, SPELL_UNLEASH_CORRUPTION_MIS, true);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                berserkerEvents.Update(diff);

                while (uint32 eventId = berserkerEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SAFETY_MEASURES: // berserk
                            if (Creature* quarantinMeasure = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_QUARANTINE_MEASURES) : ObjectGuid::Empty))
                                quarantinMeasure->AI()->DoAction(ACTION_QUARANTINE_MEASURES);
                            break;
                            // Purifying Light orbs spawning.
                        case EVENT_SPAWN_PURIF_LIGHT_ORB:
                        {
                            uint8 orbCount = Is25ManRaid() ? 2 : 1;

                            if (instance && instance->GetData(DATA_FLEX) && instance->instance->GetPlayersCountExceptGMs() > 17)
                                orbCount++;

                            HandleSummonPurifyingOrbs(orbCount);
                            berserkerEvents.ScheduleEvent(EVENT_SPAWN_PURIF_LIGHT_ORB, 24 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_CHECK_EVADE:
                            if (instance && instance->IsWipe(58.5f, me))
                            {
                                EnterEvadeMode();
                                break;
                            }

                            berserkerEvents.ScheduleEvent(EVENT_CHECK_EVADE, 1 * IN_MILLISECONDS);
                            break;
                    }
                }

                if (me->HasAura(SPELL_BLIND_HATRED_PERIODIC))
                {
                    DoMeleeAttackIfReady();
                    return;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_UNLEASHED_ANGER:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_UNLEASHED_ANGER);

                            events.ScheduleEvent(EVENT_UNLEASHED_ANGER, 11000);
                            break;

                        case EVENT_BLIND_HATRED:
                        {
                            float x, y;
                            GetPositionWithDistInOrientation(me, 40.0f, frand(0.0f, 2 * M_PI), x, y);
                            me->SummonCreature(NPC_BLIND_HATRED, x, y, me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN);
                            DoCast(me, SPELL_BLIND_HATRED, true);

                            if (Creature* quarantinMeasure = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_QUARANTINE_MEASURES) : ObjectGuid::Empty))
                                quarantinMeasure->CastSpell(quarantinMeasure, SPELL_BLIND_HATRED, true);

                            events.ScheduleEvent(EVENT_BLIND_HATRED, 60000);
                            break;
                        }
                        case EVENT_CHECK_UNCHECKED_CORRUPTION:
                            if (me->GetVictim() && !me->IsWithinMeleeRange(me->GetVictim()))
                                DoCast(me, SPELL_UNCHECKED_CORRUPTION);

                            events.ScheduleEvent(EVENT_CHECK_UNCHECKED_CORRUPTION, 4000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                void HandleSummonPurifyingOrbs(uint8 count = 1)
                {
                    // Set the Look Within realms count and spawn the NPC's.
                    std::vector<uint32> choisePos = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

                    for (auto&& itr : choisePos)
                        if (invOrbPosType.find(itr)->second != 0 && std::find(choisePos.begin(), choisePos.end(), itr) != choisePos.end())
                            choisePos.erase(std::find(choisePos.begin(), choisePos.end(), itr));
                    std::random_device rd;
                    std::mt19937 g(rd());
                    std::shuffle(choisePos.begin(), choisePos.end(), g);

                    if (choisePos.size() > count)
                        choisePos.resize(count);

                    for (auto&& itr : choisePos)
                    {
                        invOrbPosType.find(itr)->second++;
                        GetPositionWithDistInOrientation(me, 32.5f, Position::NormalizeOrientation(((itr - 1) * M_PI / 8)), x, y);

                        if (Creature* orb = me->SummonCreature(NPC_PURIFYING_LIGHT, x, y, 356.339f, TEMPSUMMON_MANUAL_DESPAWN))
                            orb->AI()->SetData(TYPE_ORB_POS, itr);
                    }
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_amalgam_of_corruptionAI(creature);
        }
};

// Manifestation of Corruption 72264 71977.
struct npc_manifestation_of_corruption : public ScriptedAI
{
    npc_manifestation_of_corruption(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid targetGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        targetGUID = summoner->GetGUID();

        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);

        if (me->GetEntry() == NPC_MANIFEST_OF_CORRUPTION_L)
        {
            me->SetPhaseMask(summoner->GetPhaseMask(), true);

            if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
            {
                amalgam->AI()->JustSummoned(me);
                amalgam->CastSpell(me, SPELL_FOUL_LINK_ADDS, true);
            }

            DoCast(me, SPELL_MANIFEST_SPAWN_VIS); // Emerge.
            DoCast(me, SPELL_FOUL_LINK_TRIGGER, true);

            scheduler
                .Schedule(Milliseconds(1000), [this](TaskContext context)
            {
                me->RemoveAurasDueToSpell(SPELL_MANIFEST_SPAWN_VIS);
            });

            // Cone Eff to Summoner
            scheduler
                .Schedule(Seconds(4), [this](TaskContext context)
            {
                if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
                {
                    me->PrepareChanneledCast(me->GetAngle(target), SPELL_TEAR_REALITY);

                    scheduler
                        .Schedule(Milliseconds(3500), [this](TaskContext context)
                    {
                        me->RemoveChanneledCast(targetGUID);
                    });
                }

                context.Repeat(Seconds(6), Seconds(9));
            });
        }
        else // Foul Link handling.
        {
            DoCast(me, SPELL_FOUL_LINK_TRIGGER, true);

            if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
                amalgam->CastSpell(me, SPELL_FOUL_LINK_ADDS, true);

            me->ClearUnitState(UNIT_STATE_CASTING);
            DoZoneInCombat(me, 200.0f);

            scheduler
                .Schedule(Seconds(3), Seconds(6), [this](TaskContext context)
            {
                DoCast(me, SPELL_BURST_OF_ANGER, true);
                context.Repeat(Seconds(9), Seconds(12));
            });
        }
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return targetGUID;
    }

    void EnterEvadeMode() override
    {
        // fake death, vanish etc.
        if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
            if (target->IsAlive())
                return;

        me->DeleteThreatList();
        me->CombatStop(true);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (me->GetEntry() != NPC_MANIFEST_OF_CORRUPTION_L)
            return;

        if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
            if (damage < amalgam->GetHealth())
                amalgam->ModifyHealth(-int32(damage));
    }

    void JustDied(Unit* killer) override
    {
        if (me->GetEntry() == NPC_MANIFEST_OF_CORRUPTION_L)
        {
            if (killer)
                killer->CastSpell(killer, SPELL_CLEANSE_MANIF_L, true);

            // Have the Amalgam Unleash it when killed.
            if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
                amalgam->CastSpell(amalgam, SPELL_MIS_SPAWN_MANIF_N, true);
        }
        else // Summon Residual Corruption.
            me->SummonCreature(NPC_RESIDUAL_CORRUPTION, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);

        me->DespawnOrUnsummon(3 * IN_MILLISECONDS); // corpse
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        if (me->GetEntry() != NPC_MANIFEST_OF_CORRUPTION_L)
            DoMeleeAttackIfReady();
    }
};

// Essence of Corruption 72263 71976.
struct npc_essence_of_corruption : public ScriptedAI
{
    npc_essence_of_corruption(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid targetGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        targetGUID = summoner->GetGUID();
        me->SetReactState(REACT_PASSIVE);

        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);

        if (me->GetEntry() == NPC_ESSENCE_OF_CORRUPTION_L)
        {
            me->SetPhaseMask(summoner->GetPhaseMask(), true);

            if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
            {
                amalgam->AI()->JustSummoned(me);
                amalgam->CastSpell(me, SPELL_FOUL_LINK_ADDS, true);
            }

            DoCast(me, SPELL_ESSENCE_OF_CORRUPTION); // Emerge + Bulwark aura.
            DoCast(me, SPELL_FOUL_LINK_TRIGGER, true);

            // Expel Corruption to Summoner
            scheduler
                .Schedule(Seconds(2), [this](TaskContext context)
            {
                if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
                    me->PrepareChanneledCast(me->GetAngle(target), SPELL_EXPEL_CORRUPTION_REALM);

                context.Repeat(Seconds(6), Seconds(7));
            });
        }
        else // Foul Link handling.
        {
            DoCast(me, SPELL_FOUL_LINK_TRIGGER, true);

            if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
                amalgam->CastSpell(me, SPELL_FOUL_LINK_ADDS, true);

            // Expel Corruption to Summoner
            scheduler
                .Schedule(Seconds(2), [this](TaskContext context)
            {
                if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
                    me->PrepareChanneledCast(me->GetAngle(amalgam), SPELL_EXPEL_CORRUPTION);

                context.Repeat(Seconds(6), Seconds(7));
            });
        }

        DoZoneInCombat(me, 100.0f);
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return targetGUID;
    }

    void EnterEvadeMode() override
    {
        // fake death, vanish etc.
        if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
            if (target->IsAlive())
                return;

        me->DeleteThreatList();
        me->CombatStop(true);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (me->GetEntry() != NPC_ESSENCE_OF_CORRUPTION_L)
            return;

        if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
            if (damage < amalgam->GetHealth())
                amalgam->ModifyHealth(-int32(damage));
    }

    void JustDied(Unit* killer) override
    {
        if (me->GetEntry() == NPC_ESSENCE_OF_CORRUPTION_L)
        {
            if (killer)
                killer->CastSpell(killer, SPELL_CLEANSE_ESSENCE_L, true);

            // Have the Amalgam Unleash it when killed.
            if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
                amalgam->CastSpell(amalgam, SPELL_MIS_SPAWN_ESSENCE_N, true);
        }

        me->DespawnOrUnsummon(3 * IN_MILLISECONDS); // corpse
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Greater Corruption 72001.
struct npc_greater_corruption : public ScriptedAI
{
    npc_greater_corruption(Creature* creature) : ScriptedAI(creature), summons(me) { }

    SummonList summons;
    EventMap events;
    uint32 summonsDead;
    ObjectGuid targetGUID;
    bool hasFail;

    void Reset() override
    {
        events.Reset();
        summons.DespawnAll();

        summonsDead = 0;
        hasFail = false;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return targetGUID;
    }

    void IsSummonedBy(Unit* summoner) override
    {
        targetGUID = summoner->GetGUID();
        me->SetPhaseMask(summoner->GetPhaseMask(), true);
        Reset();
        DoCast(me, SPELL_MANIFEST_SPAWN_VIS); // Emerge.
        me->SetHealth((uint32)(me->GetMaxHealth() * 0.5f));

        if (Creature* sunGC = me->SummonCreature(NPC_SUN_TENDERHEART_GC, 781.99f, 953.02f, 356.33f, 1.79f, TEMPSUMMON_MANUAL_DESPAWN))
        {
            sunGC->SetPhaseMask(me->GetPhaseMask(), true);
            sunGC->SetHealth(sunGC->GetMaxHealth() * 0.5);
            sunGC->AI()->AttackStart(me);
        }
        if (Creature* rookGC = me->SummonCreature(NPC_ROOK_STONETOE_GC, me->GetPositionX() - 3.0f, me->GetPositionY() + 3.0f, me->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN))
        {
            rookGC->SetPhaseMask(me->GetPhaseMask(), true);
            rookGC->SetHealth(rookGC->GetMaxHealth() * 0.5);
            rookGC->AI()->AttackStart(me);
            SetGazeOn(rookGC);
        }
        if (Creature* levenGC = me->SummonCreature(NPC_LEVEN_DAWNBLADE_GC, me->GetPositionX() - 3.0f, me->GetPositionY() - 3.0f, me->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN))
        {
            levenGC->SetPhaseMask(me->GetPhaseMask(), true);
            levenGC->SetHealth(levenGC->GetMaxHealth() * 0.5);
            levenGC->AI()->AttackStart(me);
        }

        DoZoneInCombat(me, 100.0f);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        summon->setActive(true);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->RemoveAurasDueToSpell(SPELL_MANIFEST_SPAWN_VIS);

        events.ScheduleEvent(EVENT_DISHEARTENING_LAUGH, 12000);
        events.ScheduleEvent(EVENT_LINGERING_CORRUPTION, 16000);
        events.ScheduleEvent(EVENT_BOTTOMLESS_PIT, 20000);
    }

    void EnterEvadeMode() override
    {
        // fake death, vanish etc.
        if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
            if (target->IsAlive())
                return;

        Reset();
        me->RemoveAllAreasTrigger();
        me->DeleteThreatList();
        me->CombatStop(true);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_HEAL_CHALLENGE_FAIL && !hasFail)
        {
            hasFail = true;

            if (Player* target = ObjectAccessor::GetPlayer(*me, targetGUID))
            {
                std::list<TempSummon*> manifestations;
                target->GetSummons(manifestations, NPC_MANIFEST_OF_CORRUPTION_L);
                target->GetSummons(manifestations, NPC_ESSENCE_OF_CORRUPTION_L);
                target->GetSummons(manifestations, NPC_TITANIC_CORRUPTION);
                summons.DespawnAll();

                for (auto&& itr : manifestations)
                    itr->DespawnOrUnsummon();

                // Remove the realm auras and put the player in the basic phase, Corruption level is kept.
                for (auto&& itr : realthWithin)
                    target->RemoveAurasDueToSpell(itr);

                target->RemoveAurasDueToSpell(SPELL_TEST_OF_RELIANCE_HEAL);
                me->DespawnOrUnsummon(1 * IN_MILLISECONDS);

                if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
                    amalgam->CastSpell(amalgam, SPELL_MIS_SPAWN_MANIF_N, true);
            }
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
            if (damage < amalgam->GetHealth())
                amalgam->ModifyHealth(-int32(damage));
    }

    void JustDied(Unit* killer) override
    {
        me->RemoveAllAreasTrigger();

        if (killer)
            killer->CastSpell(killer, SPELL_CLEANSE_TRIAL, true);

        summons.DespawnAll();
        me->DespawnOrUnsummon(3 * IN_MILLISECONDS); // corpse
    }

    void SummonedCreatureDies(Creature* summon, Unit* killer) override
    {
        if (summon->GetEntry() == NPC_SUN_TENDERHEART_GC || summon->GetEntry() == NPC_ROOK_STONETOE_GC || summon->GetEntry() == NPC_LEVEN_DAWNBLADE_GC)
            summonsDead++;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
            return;

        // If all 3 mobs are dead evade and despawn.
        if (summonsDead == 3)
            EnterEvadeMode();

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_DISHEARTENING_LAUGH:
                    DoCast(me, SPELL_DISHEARTENING_LAUGH);
                    events.ScheduleEvent(EVENT_DISHEARTENING_LAUGH, urand(12000, 14000));
                    break;         
                case EVENT_LINGERING_CORRUPTION:
                    DoCast(me, SPELL_LINGERING_CORRUPTION);
                    events.ScheduleEvent(EVENT_LINGERING_CORRUPTION, urand(16500, 18500));
                    break;           
                case EVENT_BOTTOMLESS_PIT:
                    DoCast(me, SPELL_BOTTOMLESS_PIT_DUMMY);
                    events.ScheduleEvent(EVENT_LINGERING_CORRUPTION, urand(25000, 30000));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Titanic Corruption 72051.
struct npc_titanic_corruption : public ScriptedAI
{
    npc_titanic_corruption(Creature* creature) : ScriptedAI(creature)
    {
        instance = creature->GetInstanceScript();
    }

    InstanceScript* instance;
    EventMap events;
    TaskScheduler scheduler;
    ObjectGuid targetGUID;

    void Reset() override
    {
        events.Reset();
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return targetGUID;
    }

    void IsSummonedBy(Unit* summoner) override
    {
        targetGUID = summoner->GetGUID();

        // for warrior reflection @TODO
        /*if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
            me->SetOwnerGUID(amalgam->GetGUID());*/

        me->SetPhaseMask(summoner->GetPhaseMask(), true);
        DoCast(me, SPELL_MANIFEST_SPAWN_VIS); // Emerge.

        DoZoneInCombat(me, 100.0f);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->RemoveAurasDueToSpell(SPELL_MANIFEST_SPAWN_VIS);

        events.ScheduleEvent(EVENT_BURST_OF_CORRUPTION, 21000);
        events.ScheduleEvent(EVENT_HURL_CORRUPTION, 12000);
        events.ScheduleEvent(EVENT_PIERCING_CORRUPTION, 17000);
        events.ScheduleEvent(EVENT_TITANIC_SMASH, 7000);
    }

    void EnterEvadeMode() override
    {
        // fake death, vanish etc.
        if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
            if (target->IsAlive())
                return;

        Reset();
        me->DeleteThreatList();
        me->CombatStop(true);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
            if (damage < amalgam->GetHealth())
                amalgam->ModifyHealth(-int32(damage));
    }

    void JustDied(Unit* killer) override
    {
        if (killer)
            killer->CastSpell(killer, SPELL_CLEANSE_TRIAL, true);

        me->DespawnOrUnsummon(3 * IN_MILLISECONDS); // corpse
    }

    // Not used currently, blizzard forgot delete it from atlas...
    void DamageDealt(Unit* victim, uint32& /*damage*/, DamageEffectType damageType) override
    {
        /*if (victim && damageType == DIRECT_DAMAGE)
            DoCast(victim, SPELL_CORRUPTION);*/
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_HURL_CORRUPTION:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_HURL_CORRUPTION);

                    events.ScheduleEvent(EVENT_HURL_CORRUPTION, urand(20000, 23000));
                    break;
                case EVENT_BURST_OF_CORRUPTION:
                    DoCast(me, SPELL_BURST_OF_CORRUPTION);
                    events.ScheduleEvent(EVENT_BURST_OF_CORRUPTION, urand(20000, 23000));
                    break;
                case EVENT_PIERCING_CORRUPTION:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_PIERCING_CORRUPTION);

                    events.ScheduleEvent(EVENT_PIERCING_CORRUPTION, urand(17000, 20000));
                    break;
                case EVENT_TITANIC_SMASH:
                    if (Unit* target = me->GetVictim())
                        me->PrepareChanneledCast(me->GetAngle(target), SPELL_TITANIC_SMASH);

                    scheduler
                        .Schedule(Milliseconds(2500), [this](TaskContext context)
                    {
                        me->RemoveChanneledCast(targetGUID);
                    });

                    events.ScheduleEvent(EVENT_TITANIC_SMASH, urand(14000, 17000));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Blind Hatred 72565.
struct npc_blind_hatred : public ScriptedAI
{
    npc_blind_hatred(Creature* creature) : ScriptedAI(creature)
    {
        instance = creature->GetInstanceScript();
    }

    InstanceScript* instance;
    TaskScheduler scheduler;
    float x, y, dist;
    uint32 hasMoving;
    bool clockWise;

    void Reset() override
    {
        me->SetDisplayFromModel(0);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
        me->SetReactState(REACT_PASSIVE);
        DoCast(me, SPELL_BLIND_HATRED_NPC_VIS, true);
        clockWise = urand(0, 1);

        me->DespawnOrUnsummon(33200);
        hasMoving = 0;

        me->SetSpeed(MOVE_RUN, 0.72f);
        me->SetSpeed(MOVE_WALK, 0.72f);

        scheduler
            .Schedule(Seconds(3), [this](TaskContext context)
        {
            hasMoving = 1;

            if (Creature* amalgam = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
            {
                dist = me->GetExactDist2d(amalgam);

                Movement::MoveSplineInit init(me);
                for (uint8 i = 1; i < 25; ++i)
                {
                    float step = clockWise ? -(i * M_PI / 12) : (i * M_PI / 12);
                    x = amalgam->GetPositionX() + dist * cos(Position::NormalizeOrientation(me->GetAngle(amalgam->GetPositionX(), amalgam->GetPositionY()) + step - M_PI));
                    y = amalgam->GetPositionY() + dist * sin(Position::NormalizeOrientation(me->GetAngle(amalgam->GetPositionX(), amalgam->GetPositionY()) + step - M_PI));
                    init.Path().push_back(G3D::Vector3(x, y, me->GetPositionZ()));
                }

                init.SetUncompressed();
                init.SetCyclic();
                init.Launch();
            }
        });
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_BLIND_HATRED)
            return hasMoving;

        return 0;
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Residual Corruption 72550.
struct npc_residual_corruption : public ScriptedAI
{
    npc_residual_corruption(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* /*summoner*/) override
    {
        if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
            amalgam->AI()->JustSummoned(me);

        DoCast(me, SPELL_RESIDUAL_CORRUPTION_A);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Quarantine Measures 72669.
struct npc_quarantine_measures : public ScriptedAI
{
    npc_quarantine_measures(Creature* creature) : ScriptedAI(creature) { }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_QUARANTINE_MEASURES && me->GetInstanceScript())
        {
            for (auto&& itr : me->GetInstanceScript()->instance->GetPlayers())
                if (Player* player = itr.GetSource())
                    player->CastSpell(player, SPELL_QUARANTINE_MEASURES, true);
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        // In case
        damage = 0;
    }
};

// Purifying Light 72065.
struct npc_purifying_light_orb : public ScriptedAI
{
    npc_purifying_light_orb(Creature* creature) : ScriptedAI(creature) { }

    uint32 orbValue;
    bool hasClick;

    void Reset() override
    {
        hasClick = false;
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetFaction(35);
        me->SetReactState(REACT_PASSIVE);

        me->AddAura(SPELL_LOOK_WITHIN_PERIODIC, me);
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_ORB_POS)
            return orbValue;

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_ORB_POS)
            orbValue = data;
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        // Prevent Multy Clicking
        if (hasClick)
            return;

        hasClick = true;

        // Check spec and choose what spells to use.
        uint32 specSpell = 0;
        uint32 specRealm = 0;
        uint32 specPhase = 0;

        Player* clickerPlayer = clicker->ToPlayer();

        if (!clickerPlayer)
        {
            me->DespawnOrUnsummon();
            return;
        }

        uint32 playerRole = clickerPlayer->GetRoleForGroup(clickerPlayer->GetTalentSpecialization());

        if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
            specRealm = amalgam->AI()->GetData(TYPE_LIGHT_ORB);

        // not one phase is available
        if (!specRealm)
        {
            me->DespawnOrUnsummon();
            return;
        }

        // Remove spellclick flag.
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

        if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
        {
            amalgam->AI()->SetData(TYPE_LIGHT_ORB, specRealm);
            specPhase = amalgam->AI()->GetData(specRealm);
        }

        switch (playerRole)
        {
            case ROLES_DPS:
                specSpell = SPELL_TEST_OF_SERENITY_DPS;
                break;
            case ROLES_HEALER:
                specSpell = SPELL_TEST_OF_RELIANCE_HEAL;
                break;
            case ROLES_TANK:
                specSpell = SPELL_TEST_OF_CONFIDENCE_TK;
                break;
            default:
                break;
        }

        // Remove orb slot
        if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
            amalgam->AI()->SetData(TYPE_ORB_POS, GetData(TYPE_ORB_POS));

        if (clicker->ToPlayer())
            clicker->ToPlayer()->UnsummonPetTemporaryIfAny();

        // Add Look Within and spec-specific Test aura to the player.
        clicker->SetPhaseMask(specPhase, true);
        clicker->CastSpell(clicker, specRealm, true);
        clicker->CastSpell(clicker, specSpell, true);

        // Despawn.
        me->DespawnOrUnsummon(500);
    }

    void UpdateAI(uint32 diff) override { };
};

// Test of Reliance Helpers 72000, 71996, 71995
struct npc_test_of_reliance_helpers : public customCreatureAI
{
    npc_test_of_reliance_helpers(Creature* creature) : customCreatureAI(creature) 
    {
        if (me->GetEntry() == NPC_SUN_TENDERHEART_GC)
            SetCombatMovement(false);
    }

    ObjectGuid summonerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        if (Creature* amalgam = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
            amalgam->AI()->JustSummoned(me);

        summonerGUID = summoner->GetGUID();
    }

    void Reset() override
    {
        events.Reset();

        if (me->GetEntry() == NPC_SUN_TENDERHEART_GC || me->GetEntry() == NPC_LEVEN_DAWNBLADE_GC)
            DoCast(me, SPELL_DAMAGE_DEALER, true);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        if (me->GetEntry() == NPC_SUN_TENDERHEART_GC)
            events.ScheduleEvent(EVENT_FIRE_BALL, 2 * IN_MILLISECONDS);
        else if (me->GetEntry() == NPC_LEVEN_DAWNBLADE_GC)
            events.ScheduleEvent(EVENT_THREATING_STRIKE, 3 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* greaterCorruption = ObjectAccessor::GetCreature(*me, summonerGUID))
            greaterCorruption->AI()->DoAction(ACTION_HEAL_CHALLENGE_FAIL);
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
            ExecuteTargetEvent(SPELL_FIRE_BALL, urand(2.6 * IN_MILLISECONDS, 3 * IN_MILLISECONDS), EVENT_FIRE_BALL, eventId);
            ExecuteTargetEvent(SPELL_THREATING_STRIKE, urand(5 * IN_MILLISECONDS, 7 * IN_MILLISECONDS), EVENT_THREATING_STRIKE, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Corruption (Player bar) 144421.
class spell_amalgam_corruption_bar : public AuraScript
{
    PrepareAuraScript(spell_amalgam_corruption_bar);

    int32 basePoints;

    bool Load() override
    {
        basePoints = 0;
        return true;
    }

    void OnTick(AuraEffect const* aurEff)
    {
        Unit* target = GetTarget();

        if (!target || !GetAura())
            return;

        // Handle Tooltip display.
        basePoints = -target->GetPower(POWER_ALTERNATE_POWER);

        if (basePoints < -99) // not set -100% dmg cuz ability doesn`t affect on hit (only AA hit by 1 i.e.). this cause ignore effects from ability hit (for example - monk tank cannot use his def spell).
            basePoints = -99;

        if (AuraEffect* percent = target->GetAuraEffect(SPELL_CORRUPTION_BAR, EFFECT_1))
            percent->SetAmount(basePoints);

        // Handle Purified aura increase (127% damage dealt 0 - 25% Corruption).
        if (target->GetPower(POWER_ALTERNATE_POWER) < CORRUPTION_LEVEL_LOW && target->ToPlayer())
        {
            if (Aura* purification = target->GetAura(SPELL_PURIFIED_DMG_INCREASE))
            {
                auto key = invPurificationType.find(target->ToPlayer()->GetRoleForGroup(target->ToPlayer()->GetTalentSpecialization()));

                if (AuraEffect* purificationPct = target->GetAuraEffect(SPELL_PURIFIED_DMG_INCREASE, key->second[0]))
                    purificationPct->SetAmount(key->second[1]);
            }
            else if (Aura* purification = target->AddAura(SPELL_PURIFIED_DMG_INCREASE, target))
            {
                auto key = invPurificationType.find(target->ToPlayer()->GetRoleForGroup(target->ToPlayer()->GetTalentSpecialization()));

                if (AuraEffect* purificationPct = target->GetAuraEffect(SPELL_PURIFIED_DMG_INCREASE, key->second[0]))
                    purificationPct->SetAmount(key->second[1]);
            }
        }
        else
        {
            if (target->HasAura(SPELL_PURIFIED_DMG_INCREASE))
                target->RemoveAurasDueToSpell(SPELL_PURIFIED_DMG_INCREASE);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_amalgam_corruption_bar::OnTick, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Unleashed Anger (Boss cast) 145216.
class spell_amalgam_unleashed_anger : public SpellScript
{
    PrepareSpellScript(spell_amalgam_unleashed_anger);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->CastSpell(target, SPELL_UNLEASHED_ANGER_DAMAGE, true);
                caster->CastSpell(target, SPELL_SELF_DOUBT, true);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_amalgam_unleashed_anger::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Icy Fear (Damage spell) 145735.
class spell_amalgam_icy_fear : public SpellScript
{
    PrepareSpellScript(spell_amalgam_icy_fear);

    void CalculateDamage(SpellEffIndex /*effIndex*/)
    {
        float mod = (100.0f - GetCaster()->GetHealthPct()) / 100.0f;
        int32 recalculatedDamage = GetHitDamage() + (int32)(GetHitDamage() * mod); // Increases by 1% for each 1% boss HP lost.
        SetHitDamage(recalculatedDamage);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->GetTypeId() != TYPEID_PLAYER; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_amalgam_icy_fear::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_amalgam_icy_fear::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Test of Serenity 144849, Test of Reliance 144850, Test of Confidence 144851.
class spell_test_realm_amalgam : public AuraScript
{
    PrepareAuraScript(spell_test_realm_amalgam);

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Player* owner = GetOwner()->ToPlayer())
        {
            // Full clear of phase
            std::list<TempSummon*> manifestations, tempManifestations;
            owner->GetSummons(manifestations, NPC_MANIFEST_OF_CORRUPTION_L);
            owner->GetSummons(manifestations, NPC_ESSENCE_OF_CORRUPTION_L);
            owner->GetSummons(manifestations, NPC_GREATER_CORRUPTION);
            owner->GetSummons(manifestations, NPC_TITANIC_CORRUPTION);
            owner->GetSummons(manifestations, NPC_ROOK_STONETOE_GC);
            owner->GetSummons(manifestations, NPC_LEVEN_DAWNBLADE_GC);
            owner->GetSummons(manifestations, NPC_SUN_TENDERHEART_GC);
            tempManifestations = manifestations;

                // duration has end - fail or die (only for dps)
            if (owner->GetRoleForGroup(owner->GetTalentSpecialization()) == ROLES_DPS)
            {
                tempManifestations.remove_if([=](TempSummon* target) { return target && !target->IsAlive(); });

                if (Creature* amalgam = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
                    for (auto&& itr : tempManifestations)
                        amalgam->CastSpell(amalgam, invTestAmalgamType.find(itr->GetEntry())->second, true);

                // Clear dps cuz it require only Manifestation
                if (!std::any_of(tempManifestations.begin(), tempManifestations.end(), [](TempSummon* target) { return target->GetEntry() == NPC_MANIFEST_OF_CORRUPTION_L; }))
                {
                    owner->SetPower(POWER_ALTERNATE_POWER, 0);

                    // Purified aura addition.
                    owner->CastSpell(owner, SPELL_PURIFIED_DMG_INCREASE, true);
                    owner->CastSpell(owner, SPELL_PURIFIED_DMG_IMMUNE, true);
                }
            }

            // Heal spec should Kill OR stay alive for purification
            if ((owner->GetRoleForGroup(owner->GetTalentSpecialization()) == ROLES_HEALER || owner->GetRoleForGroup(owner->GetTalentSpecialization()) == ROLES_TANK) && aurEff->GetBase()->GetDuration() == 0)
            {
                owner->SetPower(POWER_ALTERNATE_POWER, 0);

                // Purified aura addition.
                owner->CastSpell(owner, SPELL_PURIFIED_DMG_INCREASE, true);
                owner->CastSpell(owner, SPELL_PURIFIED_DMG_IMMUNE, true);
            }

            // Remove npc from phase
            for (auto&& itr : manifestations)
                itr->DespawnOrUnsummon();

            // Remove the realm auras and put the player in the basic phase, Corruption level is kept.
            for (auto&& itr : realthWithin)
                owner->RemoveAurasDueToSpell(itr);

            owner->RemoveAurasDueToSpell(GetAura()->GetId());
        }
    }

    void OnTick(AuraEffect const* aurEff)
    {
        if (Player* owner = GetOwner()->ToPlayer())
        {
            // Not in phase yet
            if (owner->GetPhaseMask() < 2)
                return;

            std::list<TempSummon*> manifestations;
            owner->GetSummons(manifestations, NPC_MANIFEST_OF_CORRUPTION_L);
            owner->GetSummons(manifestations, NPC_ESSENCE_OF_CORRUPTION_L);
            owner->GetSummons(manifestations, NPC_GREATER_CORRUPTION);
            owner->GetSummons(manifestations, NPC_TITANIC_CORRUPTION);

            manifestations.remove_if([=](TempSummon* target) { return target && !target->IsAlive(); });

            // Challenge is complete
            if (manifestations.empty())
            {
                owner->UnsummonPetTemporaryIfAny();

                for (auto&& itr : realthWithin)
                    owner->RemoveAurasDueToSpell(itr);

                owner->RemoveAurasDueToSpell(GetAura()->GetId());
                owner->SetPower(POWER_ALTERNATE_POWER, 0);

                // Purified aura addition.
                owner->CastSpell(owner, SPELL_PURIFIED_DMG_INCREASE, true);
                owner->CastSpell(owner, SPELL_PURIFIED_DMG_IMMUNE, true);
            }
        }
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            SpawnLookWithinMobs(owner);
    }

    private:
        void SpawnLookWithinMobs(Player* target)
        {
            if (!target)
                return;

            uint32 playerRole = target->GetRoleForGroup(target->GetTalentSpecialization());
            switch (playerRole)
            {
                case ROLES_DPS: // Manifestation + Essence.
                case ROLES_DEFAULT:
                {
                    target->CastSpell(target, SPELL_SPAWN_MANIF_L, true);
                
                    uint32 altPower = target->GetPower(POWER_ALTERNATE_POWER);
                    // Calculate and spawn Essences of Corruption.

                    uint8 EssencesToSpawn = 1;

                    if (altPower > 55)
                        EssencesToSpawn += 1;
                    if (altPower > 70)
                        EssencesToSpawn += 1;
                    if (altPower > 85)
                        EssencesToSpawn += 1;

                    if (Creature* amalgam = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
                    {
                        for (uint8 i = 0; i < EssencesToSpawn; i++)
                        {
                            GetPositionWithDistInOrientation(amalgam, 23.5f, frand(0.0f, 2 * M_PI), x, y);
                            target->CastSpell(x, y, 356.339f, SPELL_SPAWN_ESSENCE_L, true);
                        }
                    }
                    break;
                }
                case ROLES_HEALER: // Greater Corruption.
                    target->CastSpell(target, SPELL_SPAWN_GREATER_CORR, true);
                    break;
                case ROLES_TANK: // Titanic Corruption.
                    target->CastSpell(target, SPELL_SPAWN_TITANIC_CORR, true);
                    break;
                default:
                    break;
            }
        }

        float x, y;

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_test_realm_amalgam::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_test_realm_amalgam::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectApply += AuraEffectApplyFn(spell_test_realm_amalgam::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Blind Hatred Periodic 145573
class spell_blind_hatred_periodic : public AuraScript
{
    PrepareAuraScript(spell_blind_hatred_periodic);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (owner->GetEntry() == NPC_AMALGAM_OF_CORRUPTION)
                owner->CastSpell(owner, SPELL_BLIND_HATRED_DMG, true);
    }

    void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->ClearUnitState(UNIT_STATE_CASTING);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_blind_hatred_periodic::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectApply += AuraEffectApplyFn(spell_blind_hatred_periodic::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class BlindHatredPredicate 
{
    public:
        BlindHatredPredicate(Unit* const m_caster) : _caster(m_caster) { }
    
        bool operator()(WorldObject* object)
        {
            if (Creature* blindHatred = _caster->FindNearestCreature(NPC_BLIND_HATRED, 100.0f, true))
                return object && object->ToPlayer() && (!object->ToPlayer()->IsInBetween(_caster, blindHatred, 2.5f) || object->ToPlayer()->HasAura(SPELL_PURIFIED_DMG_IMMUNE) || !blindHatred->AI()->GetData(TYPE_BLIND_HATRED));
    
            return false;
        }
    
    private:
        Unit const* _caster;
};

// Blind Hatred Eff 145227
class spell_blind_hatred_eff : public SpellScript
{
    PrepareSpellScript(spell_blind_hatred_eff);

    std::list<WorldObject*> m_targets;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(BlindHatredPredicate(GetCaster()));
        m_targets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_blind_hatred_eff::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_blind_hatred_eff::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Norushen Teleport 145188
class spell_norushen_teleport : public SpellScript
{
    PrepareSpellScript(spell_norushen_teleport);

    void HandleDummy(SpellEffIndex effIndex)
    {
        if (Creature* quarantinZone = ObjectAccessor::GetCreature(*GetCaster(), GetCaster()->GetInstanceScript() ? GetCaster()->GetInstanceScript()->GetGuidData(NPC_QUARANTINE_MEASURES) : ObjectGuid::Empty))
            if (GetHitUnit() && GetHitUnit()->GetExactDist2d(quarantinZone) <= 35.0f)
                PreventHitEffect(effIndex);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_norushen_teleport::HandleDummy, EFFECT_0, SPELL_EFFECT_TELEPORT_UNITS);
    }
};

// Unleash Corruption 145768
class spell_essence_unleash_corruption : public SpellScript
{
    PrepareSpellScript(spell_essence_unleash_corruption);

    void SelectTargets(SpellDestination& dest)
    {
        if (Creature* amalgam = ObjectAccessor::GetCreature(*GetCaster(), GetCaster()->GetInstanceScript() ? GetCaster()->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
        {
            GetPositionWithDistInOrientation(amalgam, 23.5f, frand(0.0f, 2 * M_PI), x, y);
            Position newPos = { x, y, 356.339f, 0.0f };
            dest.Relocate(newPos);
        }
    }

    private:
        float x, y;

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_essence_unleash_corruption::SelectTargets, EFFECT_0, TARGET_DEST_DEST_RADIUS);
    }
};

// Unleash Corruption 145769
class spell_manifestation_unleash_corruption : public SpellScript
{
    PrepareSpellScript(spell_manifestation_unleash_corruption);

    void SelectTargets(SpellDestination& dest)
    {
        if (Creature* amalgam = ObjectAccessor::GetCreature(*GetCaster(), GetCaster()->GetInstanceScript() ? GetCaster()->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
        {
            GetPositionWithDistInOrientation(amalgam, 37.5f, frand(0.0f, 2 * M_PI), x, y);
            Position newPos = { x, y, 356.339f, 0.0f };
            dest.Relocate(newPos);
        }
    }

    private:
        float x, y;

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_manifestation_unleash_corruption::SelectTargets, EFFECT_0, TARGET_DEST_DEST_RADIUS);
    }
};

// we should hit only our owner in same phase with another challengers
class TrialPhasePredicate 
{
    public:
        TrialPhasePredicate(Creature* const m_caster) : _caster(m_caster) { }
    
        bool operator()(WorldObject* object)
        {
            if (object && object->ToPlayer() && _caster->AI()->GetGUID() == object->ToPlayer()->GetGUID())
                return false;

            return true;
        }
    
    private:
        Creature const* _caster;
};

// Bottomless Pit Selector 146701
class spell_bottomless_pit : public SpellScript
{
    PrepareSpellScript(spell_bottomless_pit);

    void HandleDummy(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_SUN_TENDERHEART_GC && target->GetEntry() != NPC_LEVEN_DAWNBLADE_GC && target->GetEntry()!=NPC_ROOK_STONETOE_GC; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_bottomless_pit::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_bottomless_pit::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Bottomless Pit Selector 146705
class spell_bottomless_pit_player_selector : public SpellScript
{
    PrepareSpellScript(spell_bottomless_pit_player_selector);

    void HandleDummy(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            targets.remove_if(TrialPhasePredicate(caster));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_bottomless_pit_player_selector::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_bottomless_pit_player_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Lingering Corruption 144514
class spell_norushen_lingering_corruption : public SpellScript
{
    PrepareSpellScript(spell_norushen_lingering_corruption);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_SUN_TENDERHEART_GC && target->GetEntry() != NPC_LEVEN_DAWNBLADE_GC && target->GetEntry() != NPC_ROOK_STONETOE_GC 
            || GetCaster() && GetCaster()->ToCreature() && target && target->ToPlayer() && GetCaster()->ToCreature()->AI()->GetGUID() != target->ToPlayer()->GetGUID(); });

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_norushen_lingering_corruption::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Tear Reality 144482
class spell_tear_reality : public SpellScript
{
    PrepareSpellScript(spell_tear_reality);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            targets.remove_if(TrialPhasePredicate(caster));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tear_reality::FilterTargets, EFFECT_1, TARGET_UNIT_CONE_ENEMY_104);
    }
};

// Burst of Corruption 144654
class spell_titanic_burst_of_corruption : public SpellScript
{
    PrepareSpellScript(spell_titanic_burst_of_corruption);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            targets.remove_if(TrialPhasePredicate(caster));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_titanic_burst_of_corruption::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Titanic Smash 144628
class spell_titanic_titanic_smash : public SpellScript
{
    PrepareSpellScript(spell_titanic_titanic_smash);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            targets.remove_if(TrialPhasePredicate(caster));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_titanic_titanic_smash::FilterTargets, EFFECT_1, TARGET_UNIT_CONE_ENEMY_24);
    }
};

// Look Within 146827, 144724, 144725, 144726, 144727, 146826, 146828, 146829, 146830, 146831, 146832, 146833, 146834, 146835, 146836, 146837
class spell_look_within : public AuraScript
{
    PrepareAuraScript(spell_look_within);

    void HandleOnRemove(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (Creature* amalgam = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
                amalgam->AI()->SetData(TYPE_LIGHT_ORB_SLOT, GetSpellInfo()->Id);

            owner->SetPhaseMask(1, true);
        }
    }

    void HandleRessumon(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            owner->ResummonPetTemporaryUnSummonedIfAny();
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_look_within::HandleOnRemove, EFFECT_0, SPELL_AURA_PHASE, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_look_within::HandleRessumon, EFFECT_0, SPELL_AURA_PHASE, AURA_EFFECT_HANDLE_REAL);
        AfterEffectApply += AuraEffectApplyFn(spell_look_within::HandleRessumon, EFFECT_0, SPELL_AURA_PHASE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Unleash Corruption 145006 (Essence), 145007 (Manifest)
class spell_unleash_corruption_summ : public SpellScript
{
    PrepareSpellScript(spell_unleash_corruption_summ);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetBossState(DATA_NORUSHEN) != IN_PROGRESS)
            PreventHitEffect(eff_idx);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_unleash_corruption_summ::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SUMMON);
    }
};

// Purified 144452
class spell_norushen_purified : public AuraScript
{
    PrepareAuraScript(spell_norushen_purified);

    void OnUpdate(uint32 /*diff*/, AuraEffect* aurEff)
    {
        if (GetOwner() && GetOwner()->ToUnit() && GetOwner()->ToUnit()->GetInstanceScript() && GetOwner()->ToUnit()->GetInstanceScript()->GetBossState(DATA_NORUSHEN) != IN_PROGRESS)
            GetOwner()->ToUnit()->RemoveAurasDueToSpell(GetSpellInfo()->Id);
    }

    void Register() override
    {
        OnEffectUpdate += AuraEffectUpdateFn(spell_norushen_purified::OnUpdate, EFFECT_0, SPELL_AURA_MOD_HEALING_DONE_PERCENT);
    }
};

// 1084. Summoned by 144548 - Expel Corruption
class sat_expel_corruption : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && (object->ToPlayer() || object->ToCreature() && object->ToCreature()->GetEntry() == NPC_AMALGAM_OF_CORRUPTION && GetAreaTrigger()->GetExactDist2d(object->ToCreature()) < 1.0f);
    }

    void OnInit() override
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = ObjectAccessor::GetUnit(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(NPC_AMALGAM_OF_CORRUPTION) : ObjectGuid::Empty))
            {
                AreaTrigger* at = GetAreaTrigger();
                float dist = at->GetExactDist2d(target) + 5.0f;
                float x, y;

                std::vector<Position> path;

                for (uint32 i = 0; i < 16; ++i)
                {
                    x = at->GetPositionX() + ((dist / 16.0f) * i * cos(at->GetAngle(target)));
                    y = at->GetPositionY() + ((dist / 16.0f) * i * sin(at->GetAngle(target)));
                    path.push_back({ x, y, at->GetPositionZ() });
                }

                GetAreaTrigger()->InitSpline(path, at->GetDuration());
            }
        }
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
        {
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_EXPELLED_CORRUPTION_EFF, true);
        }
        else if (Creature* itr = target->ToCreature())
        {
            itr->CastSpell(itr, SPELL_FUSION, true);

            if (itr->AI())
                itr->AI()->SetData(TYPE_NONE_SHALL_PASS, 0);
        }
    }
};

// 1106. Summoned by 145064 - Expel Corruption 145064
class sat_expel_corruption_realm : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && GetCaster() && GetCaster()->ToCreature() && GetCaster()->ToCreature()->AI()->GetGUID() == object->ToPlayer()->GetGUID() && GetAreaTrigger()->GetExactDist2d(object->ToPlayer()) < 1.0f;
    }

    void OnInit() override
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            AreaTrigger* at = GetAreaTrigger();
            float dist = 80.0f;
            float x, y;

            std::vector<Position> path;

            for (uint32 i = 0; i < 20; ++i)
            {
                x = at->GetPositionX() + ((dist / 16.0f) * i * cos(caster->GetOrientation()));
                y = at->GetPositionY() + ((dist / 16.0f) * i * sin(caster->GetOrientation()));
                path.push_back({ x, y, at->GetPositionZ() });
            }

            GetAreaTrigger()->InitSpline(path, at->GetDuration());
        }
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_EXPELLED_CORRUPTION_EFF, true);
    }
};

// 1107. Summoned by 145074 - Residual Corruption
class sat_residual_corruption : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && object->ToPlayer()->GetPower(POWER_ALTERNATE_POWER) <= 75;
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
            itr->SetPower(POWER_ALTERNATE_POWER, itr->GetPower(POWER_ALTERNATE_POWER) + 25);
    }
};

// 1257. Summoned by 146793 - Bottomless Pit
class sat_bottomless_pit : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() || triggering->ToCreature() && (triggering->ToCreature()->GetEntry() == NPC_SUN_TENDERHEART_GC || triggering->ToCreature()->GetEntry() == NPC_LEVEN_DAWNBLADE_GC || triggering->ToCreature()->GetEntry() == NPC_ROOK_STONETOE_GC);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_BOTTOMLESS_PIT_DMG, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_BOTTOMLESS_PIT_DMG);
    }
};

class cond_blind_hatred : public ConditionScript
{
    public:
        cond_blind_hatred() : ConditionScript("cond_blind_hatred") { }

        bool OnConditionCheck(const Condition* cond, ConditionSourceInfo& source) override
        {
            if (source.mConditionTargets[0] && source.mConditionTargets[1])
            {
                if (Creature* caster = source.mConditionTargets[1]->ToCreature())
                {
                    if (Creature* target = source.mConditionTargets[0]->ToCreature())
                    {
                        if (caster->GetEntry() == NPC_QUARANTINE_MEASURES && target->GetEntry() == NPC_BLIND_HATRED)
                            return true;

                        if (caster->GetEntry() == NPC_AMALGAM_OF_CORRUPTION && target->GetEntry() == NPC_AMALGAM_OF_CORRUPTION) // should have only visual channeled for self
                            return true;
                    }
                }
            }

            return false;
        }
};

void AddSC_norushen()
{
    new boss_norushen();
    new boss_amalgam_of_corruption();

    new creature_script<npc_manifestation_of_corruption>("npc_manifestation_of_corruption");
    new creature_script<npc_essence_of_corruption>("npc_essence_of_corruption");
    new creature_script<npc_greater_corruption>("npc_greater_corruption");
    new creature_script<npc_titanic_corruption>("npc_titanic_corruption");
    new creature_script<npc_blind_hatred>("npc_blind_hatred");
    new creature_script<npc_residual_corruption>("npc_residual_corruption");
    new creature_script<npc_quarantine_measures>("npc_quarantine_measures");
    new creature_script<npc_purifying_light_orb>("npc_purifying_light_orb");
    new creature_script<npc_test_of_reliance_helpers>("npc_test_of_reliance_helpers");

    new aura_script<spell_amalgam_corruption_bar>("spell_amalgam_corruption_bar");
    new spell_script<spell_amalgam_unleashed_anger>("spell_amalgam_unleashed_anger");
    new spell_script<spell_amalgam_icy_fear>("spell_amalgam_icy_fear");
    new aura_script<spell_test_realm_amalgam>("spell_test_realm_amalgam");
    new aura_script<spell_blind_hatred_periodic>("spell_blind_hatred_periodic");
    new spell_script<spell_blind_hatred_eff>("spell_blind_hatred_eff");
    new spell_script<spell_norushen_teleport>("spell_norushen_teleport");
    new spell_script<spell_essence_unleash_corruption>("spell_essence_unleash_corruption");
    new spell_script<spell_manifestation_unleash_corruption>("spell_manifestation_unleash_corruption");
    new spell_script<spell_bottomless_pit>("spell_bottomless_pit");
    new spell_script<spell_bottomless_pit_player_selector>("spell_bottomless_pit_player_selector");
    new spell_script<spell_norushen_lingering_corruption>("spell_norushen_lingering_corruption");
    new spell_script<spell_tear_reality>("spell_tear_reality");
    new spell_script<spell_titanic_burst_of_corruption>("spell_titanic_burst_of_corruption");
    new spell_script<spell_titanic_titanic_smash>("spell_titanic_titanic_smash");
    new aura_script<spell_look_within>("spell_look_within");
    new spell_script<spell_unleash_corruption_summ>("spell_unleash_corruption_summ");
    new aura_script<spell_norushen_purified>("spell_norushen_purified");
    new atrigger_script<sat_expel_corruption>("sat_expel_corruption");
    new atrigger_script<sat_residual_corruption>("sat_residual_corruption");
    new atrigger_script<sat_expel_corruption_realm>("sat_expel_corruption_realm");
    new atrigger_script<sat_bottomless_pit>("sat_bottomless_pit");
    new cond_blind_hatred();
}
