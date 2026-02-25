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

#include "GameObjectAI.h"
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
#include "throne_of_thunder.h"

enum Spells
{
    /*Encounter*/
    SPELL_HARD_STARE                  = 133765,
    SPELL_GAZE                        = 134029,
    SPELL_BERSERK_KILLALL             = 137747,
    SPELL_DRAIN_LIFE_STUN             = 137727,
    SPELL_DRAIN_LIFE_CHANNEL          = 133795,
    SPELL_DRAIN_LIFE_DAMAGE           = 133798,
    SPELL_DRAIN_LIFE_HEAL             = 133807,
    SPELL_DESINTEGRATION_BEAM         = 133776,
    SPELL_DISINTEGRATION_BEAM_MOVER   = 133775,
    SPELL_DISINTEGRATION_BEAM_AT      = 133778,
    SPELL_DISINTEGRATION_BEAM_PRECAST = 134169,
    SPELL_MIND_DAGGERS_EFF            = 139107,
    SPELL_MIND_DAGGERS                = 139108,
    SPELL_EYE_SORE                    = 134755,

    /*Eyes*/
    SPELL_LINGERING_GAZE              = 138467, // no selector for this spell
    SPELL_LINGERING_GAZE_MISSLE       = 133792,
    SPELL_LINGERING_GAZE_AT           = 133793,
    SPELL_LINGERING_GAZE_AT_EFF       = 134040,
    SPELL_LINGERING_GAZE_EFF          = 134044, // On Missle Hit
    SPELL_LINGERING_GAZE_MARK         = 134626,
    SPELL_FORCE_OF_WILL               = 136932,
    SPELL_FORCE_OF_WILL_KNOCKBACK     = 136413,
                                      
    /*Misc | Shared*/                 
    SPELL_DURUMU_EYE_SPAWN            = 137674,
                                      
    // Beams                          
    SPELL_YELLOW_BEAM                 = 133740,
    SPELL_RED_BEAM                    = 133734,
    SPELL_BLUE_BEAM                   = 133672,
    SPELL_YELLOW_BEAM_DMG             = 133738,
    SPELL_RED_BEAM_DMG                = 133732,
    SPELL_BLUE_BEAM_DMG               = 133677,
    SPELL_YELLOW_BEAM_DMG_TO_ALL      = 133739,
    SPELL_RED_BEAM_DMG_TO_ALL         = 133733,
    SPELL_BLUE_BEAM_DMG_TO_ALL        = 133678,
    SPELL_INFARED_LIGHT_AURA_EFF      = 133732,

    SPELL_INFRARED_BEAM_FOCUS         = 134123,
    SPELL_BRIGHT_LIGHT_FOCUS          = 134124, // yellow
    SPELL_BLUE_RAYS_FOCUS             = 134122, // blue
                                      
    SPELL_FOG_INVISIBLE               = 137092,
                                      
    SPELL_CAUSTIC_SPIKE               = 136154,
    SPELL_ICY_GRASP                   = 136177,
    SPELL_CRIMSON_BLOOM               = 136122,
    SPELL_FLASH_FREEZE                = 136124,
                                      
    SPELL_SUMMON_CRIMSON_FOG          = 136128,
    SPELL_SUMMON_BLUE_FOG             = 136130,
    SPELL_INFRARED_LIGHT_ANN          = 133731,
    SPELL_BRIGHT_LIGHT_ANN            = 133737,
    SPELL_BLUE_RAYS_ANN               = 133675,
    SPELL_BLUE_RAY_TRACKING           = 139202,
    SPELL_INFARED_TRACKING            = 139204,

    // Disintegration phase
    // Those cumulated spells form a cone covering the whole platform, every visual lasts for 12 seconds.
    SPELL_WHOLE_ROOM_SLICE_ONE        = 136232, // 4 Visual dark balls in front of the caster / dist from caster : 0.0f->1.61f          = 1.61f
    SPELL_WHOLE_ROOM_SLICE_TWO        = 136233, // Visual farther than the precedent one      / dist from caster : 1.6f->7.21f          = 5.61f
    SPELL_WHOLE_ROOM_SLICE_THREE      = 136234, // Visual farther than the precedent one      / dist from caster : 7.21f->11.443f       = 4.233f
    SPELL_WHOLE_ROOM_SLICE_FOUR       = 136235, // Visual farther than the precedent one      / dist from caster : 11.443f->16.859f     = 5.416f
    SPELL_WHOLE_ROOM_SLICE_FIVE       = 136236, // Visual farther than the precedent one      / dist from caster : 16.859f->21.94f      = 5.081f
    SPELL_WHOLE_ROOM_SLICE_SIX        = 136237, // Visual farther than the precedent one      / dist from caster : 21.94f->26.504f      = 4.564f
    SPELL_WHOLE_ROOM_SLICE_SEVEN      = 136238, // Visual farther than the precedent one      / dist from caster : 26.504f->32.24f      = 5.736f
    SPELL_WHOLE_ROOM_SLICE_EIGHT      = 136239, // Visual farther than the precedent one      / dist from caster : 32.24f->37.896f      = 5.656f
    SPELL_WHOLE_ROOM_SLICE_NINE       = 136240, // Visual farther than the precedent one      / dist from caster : 37.896f->43.273f     = 5.378f
    SPELL_WHOLE_ROOM_SLICE_TEN        = 136241, // Visual farther than the precedent one      / dist from caster : 43.273f->47.631f     = 4.358f
    SPELL_WHOLE_ROOM_SLICE_ELEVEN     = 136242, // Visual farther than the precedent one      / dist from caster : 47.631f->52.853f     = 5.222f
    SPELL_WHOLE_ROOM_SLICE_TWELVE     = 136243, // Visual farther than the precedent one      / dist from caster : 52.853f->58.051f     = 5.198f
    SPELL_WHOLE_ROOM_SLICE_THIRTEEN   = 136244, // Visual farther than the precedent one      / dist from caster : 58.051f->62.973f     = 4.922f

    // Each spell forms a cone covering the whole platform, with +3k MS duration for each spellId incrementation.
    // Maybe be launched from the 10th to the 1st to form the whole labyrinth
    SPELL_WHOLE_ROOM_MAZE_ONE         = 136553, // Whole dark cone visual (M_PI / 6) (duration = 12500 MS)
    SPELL_WHOLE_ROOM_MAZE_TWO         = 136554, // Whole dark cone visual (M_PI / 6) (duration = 15500 MS)
    SPELL_WHOLE_ROOM_MAZE_THREE       = 136555, // Whole dark cone visual (M_PI / 6) (duration = 18500 MS)
    SPELL_WHOLE_ROOM_MAZE_FOUR        = 136556, // Whole dark cone visual (M_PI / 6) (duration = 21500 MS)
    SPELL_WHOLE_ROOM_MAZE_FIVE        = 136557, // Whole dark cone visual (M_PI / 6) (duration = 24500 MS)
    SPELL_WHOLE_ROOM_MAZE_SIX         = 136558, // Whole dark cone visual (M_PI / 6) (duration = 27500 MS)
    SPELL_WHOLE_ROOM_MAZE_SEVEN       = 136559, // Whole dark cone visual (M_PI / 6) (duration = 30500 MS)
    SPELL_WHOLE_ROOM_MAZE_EIGHT       = 136560, // Whole dark cone visual (M_PI / 6) (duration = 33500 MS)
    SPELL_WHOLE_ROOM_MAZE_NINE        = 136561, // Whole dark cone visual (M_PI / 6) (duration = 36500 MS)
    SPELL_WHOLE_ROOM_MAZE_TEN         = 136562, // Whole dark cone visual (M_PI / 6) (duration = 39500 MS)

    /*Heroic*/
    SPELL_AMBER_RETAILATION           = 136175, // each time when lost 10% from max HP
    SPELL_BURST_OF_AMBER              = 136123,
    SPELL_SUMMON_YELLOW_FOG_BEAST     = 136129,
    SPELL_ICE_WALL                    = 134587,
    SPELL_ICE_WALL_COLLISION          = 139293,
    SPELL_ICEWALL_SPAWN               = 137361,
    SPELL_ICEWALL_VISUAL              = 134576,
    SPELL_DARK_PARASITE               = 133597,
    SPELL_DARK_PLAGUE                 = 133598,
    SPELL_DEVOUR                      = 134007,
    SPELL_DEVOUR_EFF                  = 134010,
    SPELL_DEVOUR_SHARE                = 134005,

    // Misc
    SPELL_CROSSING_THE_STREAMS        = 139160, // BE_SPELL_TARGET_2
    SPELL_SUMMON_GREEN_FOG_BEAST      = 139156,
    SPELL_SUMMON_ORANGE_FOG_BEAST     = 139154,
    SPELL_SUMMON_PURPLE_FOG_BEAST     = 139155,
    SPELL_BLUE_RAY                    = 136119,
    SPELL_INFRARED_LIGHT              = 136120,
    SPELL_BRIGHT_LIGHT                = 136121,
};

enum Actions
{
    // Durumu
    ACTION_BEGIN_INTRO = 1,
    ACTION_FOG_KILLED,
    ACTION_SAY_FORCE_OF_WILL,
    ACTION_SAY_SPELL_2,
    ACTION_SAY_SPELL_3,
    ACTION_SAY_SPELL_4,
    ACTION_MAZE_START,
    ACTION_MAZE_END,
    ACTION_SUMMON_BLUE_FOG,

    // Fogs
    ACTION_FOG_ACTIVATE,
    ACTION_FOG_DEACTIVATE,

    // Eyes
    ACTION_EYE_DRAIN_LIFE,
    ACTION_DISINTEGRATION_BEAM,
    ACTION_SWITCH_BEAM_TARGET,
};

enum Events
{
    // Durumu
    EVENT_HARD_STARE = 1,
    EVENT_GAZE,
    EVENT_BERSERK,
    EVENT_LIGHT_SPECTRUM_SAY_SUMMON,
    EVENT_LIGHT_SPECTRUM,
    EVENT_MAZE_TIME,
    EVENT_MAZE_ORIENTATION,
    EVENT_MAZE_END,
    EVENT_CHECK_WIPE,
    EVENT_NEAR_CONE_SLICE,
    EVENT_SPECTRUM_LIMIT,
    EVENT_ICE_WALL,

    // Eyes
    EVENT_FORCE_OF_WILL,
    EVENT_LINGERING_GAZE,
    EVENT_DRAIN_LIFE,
    EVENT_EYES_ORIENTATION,

    // For roaming fogs and eyes
    EVENT_MOVE_RANDOM,

    // fogs
    EVENT_RED_FOG_WIND,
    EVENT_FOG_CAST,

    // Eye mover
    EVENT_INITIALIZE_TYPE_MOVE,
    EVENT_UNLOCK_AI,
    EVENT_CHECK_FOGS
};

enum Yells
{
    SAY_INTRO,
    SAY_AGGRO,
    SAY_DEATH,
    SAY_BERSERK,
    SAY_KILL1,
    SAY_KILL2,
    SAY_FORCE_OF_WILL,
    SAY_SUMMON_SPECTRUM,
    SAY_LINGERING_GAZE,
    SAY_SPELL_4, // idk for what spell this used
    SAY_SPECTRUM_INF,
};

enum miscPhase
{
    TYPE_PHASE,
    TYPE_LOW_MAZE_1,
    TYPE_LOW_MAZE_2,
    TYPE_HIGH_MAZE_1,
    TYPE_HIGH_MAZE_2,
    TYPE_BEAM_DURATION,
    TYPE_LIFE_DRAIN_DURATION,
};

const std::map<uint32, uint32> invBeamsType = 
{
    { SPELL_INFRARED_BEAM_FOCUS, SPELL_RED_BEAM  },
    { SPELL_BLUE_RAYS_FOCUS,     SPELL_BLUE_BEAM },
    { SPELL_BRIGHT_LIGHT_FOCUS,     0            },
};

const std::map<uint32, uint32> invCollorType =
{
    { NPC_RED_EYE,    SPELL_INFRARED_LIGHT_ANN },
    { NPC_BLUE_EYE,   SPELL_BLUE_RAYS_ANN      },
    { NPC_YELLOW_EYE, SPELL_BRIGHT_LIGHT_ANN   },
};

const std::map<uint32, uint32> invBeamEyeType =
{
    { NPC_RED_EYE,    SPELL_INFRARED_BEAM_FOCUS },
    { NPC_BLUE_EYE,   SPELL_BLUE_RAYS_FOCUS     },
    { NPC_YELLOW_EYE, SPELL_BRIGHT_LIGHT_FOCUS  },
};

const std::map<uint32, float> invMazeType =
{
    { SPELL_WHOLE_ROOM_SLICE_ONE,   1.0f    },
    { SPELL_WHOLE_ROOM_SLICE_TWO,   1.5f    },
    { SPELL_WHOLE_ROOM_SLICE_THREE, 2.25f   },
    { SPELL_WHOLE_ROOM_SLICE_FOUR,  3.0f    },
    { SPELL_WHOLE_ROOM_SLICE_FIVE,  3.75f   },
    { SPELL_WHOLE_ROOM_SLICE_SIX,   4.5f    },
    { SPELL_WHOLE_ROOM_SLICE_SEVEN, 5.25f   },
    { SPELL_WHOLE_ROOM_SLICE_EIGHT, 6.0f    },
    { SPELL_WHOLE_ROOM_SLICE_NINE,  6.75f   },
    { SPELL_WHOLE_ROOM_SLICE_TEN,   7.5f    },
    { SPELL_WHOLE_ROOM_SLICE_ELEVEN, 8.25f  },
    { SPELL_WHOLE_ROOM_SLICE_TWELVE, 9.0f   },
    { SPELL_WHOLE_ROOM_SLICE_THIRTEEN, 9.75 },
};

const std::map<uint32, std::array<uint32, 2>> invAchievFogsType =
{
    { NPC_ORANGE_FOG_BEAST, { SPELL_INFRARED_LIGHT, SPELL_BRIGHT_LIGHT,   } },
    { NPC_PURPLE_FOG_BEAST, { SPELL_BLUE_RAY,       SPELL_INFRARED_LIGHT, } },
    { NPC_GREEN_FOG_BEAST,  { SPELL_BLUE_RAY,       SPELL_BRIGHT_LIGHT,   } },
};

const std::map<uint32, uint32> invSpectrumMarkType =
{
    { 569, SPELL_INFRARED_LIGHT },
    { 565, SPELL_BLUE_RAY       },
    { 571, SPELL_BRIGHT_LIGHT   },
};

// Durumu 68036
class boss_durumu : public CreatureScript
{
    public:
        boss_durumu() : CreatureScript("boss_durumu") { }

        struct boss_durumuAI : public BossAI
        {
            boss_durumuAI(Creature* creature) : BossAI(creature, DATA_DURUMU_THE_FORGOTTEN)
            {
                me->setActive(true);
                SetCombatMovement(false);
            }

            EventMap nonCombatEvents;
            uint8 FogKilled; // used for summoned fogs in fighting
            uint32 phaseId;
            uint32 lowMaze_1;
            uint32 lowMaze_2;
            uint32 highMaze_1;
            uint32 highMaze_2;
            ObjectGuid lifeDrainEyeGUID;
            float x, y, ori;
            std::vector<uint32> achievementFogs;
            std::vector<uint32> achievementFogsEntry;

            void InitializeAI() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->SetReactState(REACT_DEFENSIVE);
                me->SetHomePosition(CenterDurumu);
                me->GetMap()->CreatureRelocation(me, CenterDurumu.GetPositionX(), CenterDurumu.GetPositionY(), CenterDurumu.GetPositionZ(), CenterDurumu.GetOrientation());
                //me->SetVisible(false);
                //me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                Reset();
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                if (instance)
                    instance->SetBossState(DATA_DURUMU_THE_FORGOTTEN, FAIL);
            }

            void Reset() override
            {
                // Temp Hackfix
                if (instance && instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
                {
                    // should be 261.7m in 10 normal
                    me->SetCreateHealth(261700000);
                    me->SetMaxHealth(261700000);
                    me->SetHealth(261700000);
                    me->ResetPlayerDamageReq();
                }

                me->SetReactState(REACT_DEFENSIVE);
                events.Reset();
                nonCombatEvents.Reset();
                me->RemoveAllAreasTrigger();
                _Reset();
                x = 0.0f; y = 0.0f;
                phaseId = 0;
                lifeDrainEyeGUID = ObjectGuid::Empty;

                if (!me->IsVisible())
                    nonCombatEvents.ScheduleEvent(EVENT_CHECK_FOGS, 1500);

                achievementFogs.clear();
                achievementFogsEntry.clear();

                achievementFogsEntry = { SPELL_SUMMON_GREEN_FOG_BEAST, SPELL_SUMMON_ORANGE_FOG_BEAST, SPELL_SUMMON_PURPLE_FOG_BEAST };

                scheduler
                    .Schedule(Seconds(2), [this](TaskContext context)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            void JustDied(Unit* killer) override
            {
                Talk(SAY_DEATH);

                _JustDied();

                nonCombatEvents.Reset();
                me->RemoveAllAreasTrigger();

                if (instance)
                {
                   instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove.
                   instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                   instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RED_BEAM);
                   instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RED_BEAM_DMG);
                   instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLUE_BEAM);
                   instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLUE_BEAM_DMG);
                   instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_YELLOW_BEAM);
                   instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_YELLOW_BEAM_DMG);
                   instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INFRARED_LIGHT_ANN);
                   instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BRIGHT_LIGHT_ANN);
                   instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLUE_RAYS_ANN);
                   instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DRAIN_LIFE_STUN);
                   instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INFARED_TRACKING);
                   instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLUE_RAY_TRACKING);

                   instance->DoUpdateWorldState(WORLDSTATE_FOG_BEASTS_REMAINING, 0);
                }

                // Remove Collision
                std::list<GameObject*> iceWallList;
                GetGameObjectListWithEntryInGrid(iceWallList, me, GO_ICE_WALL, 200.0f);

                for (auto&& itr : iceWallList)
                    itr->Delete();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_PHASE)
                    return phaseId;

                switch (type)
                {
                    case TYPE_PHASE:
                        return phaseId;
                    case TYPE_LOW_MAZE_1:
                        return lowMaze_1;
                    case TYPE_LOW_MAZE_2:
                        return lowMaze_2;
                    case TYPE_HIGH_MAZE_1:
                        return highMaze_1;
                    case TYPE_HIGH_MAZE_2:
                        return highMaze_2;
                }

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case TYPE_LOW_MAZE_1:
                        lowMaze_1 = data;
                        break;
                    case TYPE_LOW_MAZE_2:
                        lowMaze_2 = data;
                        break;
                    case TYPE_HIGH_MAZE_1:
                        highMaze_1 = data;
                        break;
                    case TYPE_HIGH_MAZE_2:
                        highMaze_2 = data;
                        break;
                }
            }

            void KilledUnit(Unit* victim) override
            {
                me->AI()->Talk(urand(0,1) == 0 ? SAY_KILL1 : SAY_KILL2);
            }

            void JustEngagedWith(Unit* who) override
            {
                Talk(SAY_AGGRO);
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_HARD_STARE, 5000);
                events.ScheduleEvent(EVENT_GAZE, 2000);
                events.ScheduleEvent(EVENT_BERSERK, 10 * MINUTE * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_LIGHT_SPECTRUM_SAY_SUMMON, 40000);
                events.ScheduleEvent(EVENT_MAZE_TIME, 2 * MINUTE * IN_MILLISECONDS + 15 * IN_MILLISECONDS);

                if (IsHeroic())
                    events.ScheduleEvent(EVENT_ICE_WALL, 2 * MINUTE * IN_MILLISECONDS + 8 * IN_MILLISECONDS);

                nonCombatEvents.ScheduleEvent(EVENT_CHECK_WIPE, 1 * IN_MILLISECONDS);
                SummonEyes();
            }


            void DespawnCreature(uint32 entry)
            {
                std::list<Creature*> creatureList;
                GetCreatureListWithEntryInGrid(creatureList, me, entry, 80.0f);

                for (auto&& NowCreature : creatureList)
                {
                    if (NowCreature->GetEntry() == NPC_BLUE_FOG && NowCreature->GetDisplayId() == NowCreature->GetNativeDisplayId()) // if blue fog was been activated not despawn him
                        continue;
                    NowCreature->DespawnOrUnsummon();
                }
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
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RED_BEAM);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RED_BEAM_DMG);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLUE_BEAM);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLUE_BEAM_DMG);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_YELLOW_BEAM);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_YELLOW_BEAM_DMG);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INFRARED_LIGHT_ANN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BRIGHT_LIGHT_ANN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLUE_RAYS_ANN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DRAIN_LIFE_STUN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INFARED_TRACKING);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLUE_RAY_TRACKING);

                    instance->DoUpdateWorldState(WORLDSTATE_FOG_BEASTS_REMAINING, 0);
                }

                summons.DespawnAll();
                scheduler.CancelAll();

                // Remove Collision
                std::list<GameObject*> iceWallList;
                GetGameObjectListWithEntryInGrid(iceWallList, me, GO_ICE_WALL, 200.0f);

                for (auto&& itr : iceWallList)
                    itr->Delete();

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                _DespawnAtEvade();
                me->GetMap()->CreatureRelocation(me, CenterDurumu.GetPositionX(), CenterDurumu.GetPositionY(), CenterDurumu.GetPositionZ(), CenterDurumu.GetOrientation());
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_FOGS:
                        {
                            std::list<Creature*> list;
                            bool _initiate = true;
                            GetCreatureListWithEntryInGrid(list, me, NPC_ROAMING_FOG, 100.0f);

                            for (auto&& itr : list)
                                if (itr->IsAlive())
                                {
                                    _initiate = false;
                                    break;
                                }
                        
                            if (_initiate)
                                DoAction(ACTION_BEGIN_INTRO);
                            else
                                nonCombatEvents.ScheduleEvent(EVENT_CHECK_FOGS, 1500);
                            break;
                        }
                        case EVENT_UNLOCK_AI:
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            break;
                        case EVENT_CHECK_WIPE:
                            if (instance && instance->IsWipe(75.0f, me))
                            {
                                EnterEvadeMode();
                                break;
                            }

                            nonCombatEvents.ScheduleEvent(EVENT_CHECK_WIPE, 1 * IN_MILLISECONDS);
                            break;
                        case EVENT_NEAR_CONE_SLICE:
                        {
                            HandleMindDaggers();
                            float sOri = M_PI / 4;

                            do
                            {
                                x = me->GetPositionX(), y = me->GetPositionY();

                                for (auto&& itr : invMazeType)
                                {
                                    x = x + (itr.second * cos(sOri));
                                    y = y + (itr.second * sin(sOri));
                                    me->CastSpell(x, y, -6.27f, itr.first, true);
                                }

                                sOri += M_PI / 8;
                            } 
                            while (sOri <= (3 * M_PI) / 8);

                            break;
                        }
                    }
                }

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
                        case EVENT_HARD_STARE:
                        {
                            DoPlaySoundToSet(me, 35334); // wound

                            if (Unit* vict = me->GetVictim())
                                DoCast(vict, SPELL_HARD_STARE);

                            events.ScheduleEvent(EVENT_HARD_STARE, 13000);
                            break;
                        }
                        case EVENT_GAZE:
                        {
                            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 2.0f, true);

                            if (!target && !me->HasUnitState(UNIT_STATE_CASTING))
                                DoCast(SPELL_GAZE);

                            events.ScheduleEvent(EVENT_GAZE, 2000);
                            break;
                        }
                        case EVENT_BERSERK:
                            events.CancelEventGroup(0); // clear all events
                            Talk(SAY_BERSERK);
                            DoCast(SPELL_BERSERK_KILLALL);
                            break;
                            // Spectrum phase start
                        case EVENT_LIGHT_SPECTRUM_SAY_SUMMON:
                        {
                            phaseId = 1;
                            FogKilled = 0;
                            Talk(SAY_SUMMON_SPECTRUM);
                            for (uint8 i = 0; i < 3; i++)
                            {
                                GetPositionWithDistInOrientation(me, frand(27.0f, 60.0f), frand(0.0f, 2 * M_PI), x, y);
                                me->CastSpell(x, y, me->GetPositionZ(), SPELL_SUMMON_CRIMSON_FOG, true);
                            }
                        
                            GetPositionWithDistInOrientation(me, frand(27.0f, 60.0f), frand(0.0f, 2 * M_PI), x, y);
                            me->CastSpell(x, y, me->GetPositionZ(), SPELL_SUMMON_BLUE_FOG, true);

                            // Yellow Fog
                            if (IsHeroic())
                            {
                                GetPositionWithDistInOrientation(me, frand(27.0f, 60.0f), frand(0.0f, 2 * M_PI), x, y);
                                me->CastSpell(x, y, me->GetPositionZ(), SPELL_SUMMON_YELLOW_FOG_BEAST, true);
                            }

                            // Achievement Fogs
                            if (!achievementFogsEntry.empty())
                            {
                                uint32 tempFog = Trinity::Containers::SelectRandomContainerElement(achievementFogsEntry);
                                DoCast(me, tempFog, true);

                                // Remove from list
                                if (std::find(achievementFogsEntry.begin(), achievementFogsEntry.end(), tempFog) != achievementFogsEntry.end())
                                    achievementFogsEntry.erase(std::find(achievementFogsEntry.begin(), achievementFogsEntry.end(), tempFog));
                            }
                        
                            if (instance)
                                instance->DoUpdateWorldState(WORLDSTATE_FOG_BEASTS_REMAINING, 3);

                            events.ScheduleEvent(EVENT_LIGHT_SPECTRUM, 3000);

                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_SPECTRUM_LIMIT, 80 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_LIGHT_SPECTRUM:
                            Talk(SAY_SPECTRUM_INF);
                            SummonBeamsEye();
                            break;
                            // Disintegration Beam phase start
                        case EVENT_MAZE_TIME:
                            if (GetData(TYPE_PHASE) == 1) // delay if in specturm phase yet
                            {
                                events.RescheduleEvent(EVENT_MAZE_TIME, 5 * IN_MILLISECONDS);
                                break;
                            }

                            phaseId = 2;
                            lowMaze_1 = 0;
                            lowMaze_2 = 0;
                            highMaze_1 = 0;
                            highMaze_2 = 0;
                            Maze(true);
                            events.ScheduleEvent(EVENT_MAZE_END, 47000);
                            break;
                        case EVENT_MAZE_END:
                            phaseId = 0;
                            Maze(false);
                            events.ScheduleEvent(EVENT_LIGHT_SPECTRUM_SAY_SUMMON, 40000);
                            events.ScheduleEvent(EVENT_MAZE_TIME, 2 * MINUTE * IN_MILLISECONDS + 15 * IN_MILLISECONDS);

                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_ICE_WALL, 2 * MINUTE * IN_MILLISECONDS + 8 * IN_MILLISECONDS);
                            break;
                        case EVENT_SPECTRUM_LIMIT:
                            Talk(SAY_BERSERK);
                            DoCast(SPELL_BERSERK_KILLALL);
                            break;
                        case EVENT_ICE_WALL:
                            phaseId = 2;

                            for (uint8 i = 0; i < 3; i++)
                            {
                                float mod = i > 1 ? M_PI / 4 : 0.0f;
                                GetPositionWithDistInOrientation(me, 1.0f, Position::NormalizeOrientation((7 * M_PI) / 4 + i * (M_PI / 2) + mod), x, y);
                                me->SummonCreature(NPC_ICE_WALL, x, y, -6.278f, Position::NormalizeOrientation((7 * M_PI) / 4 + i * (M_PI / 2) + mod), TEMPSUMMON_MANUAL_DESPAWN);
                            }
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_SUMMON_BLUE_FOG:
                        SummonSomeCreatures(NPC_BLUE_FOG, 1, 10.0f); // 1 blue fog
                        break;
                    case ACTION_BEGIN_INTRO: // When players kill roaming
                        //me->AI()->Talk(SAY_INTRO);
                        me->SetVisible(true);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        nonCombatEvents.ScheduleEvent(EVENT_UNLOCK_AI, 21000);
                        break;
                    case ACTION_FOG_KILLED: // red fog die
                        ++FogKilled;
                        if (FogKilled == 3)
                            EndBeamPhase();

                        if (instance)
                            instance->DoUpdateWorldState(WORLDSTATE_FOG_BEASTS_REMAINING, 3 - FogKilled);
                        break;
                    case ACTION_SAY_FORCE_OF_WILL: // force of will
                        me->AI()->Talk(SAY_FORCE_OF_WILL);
                        break;
                    case NPC_ORANGE_FOG_BEAST:
                    case NPC_PURPLE_FOG_BEAST:
                    case NPC_GREEN_FOG_BEAST:
                        if (std::find(achievementFogs.begin(), achievementFogs.end(), (uint32)actionId) == achievementFogs.end())
                            achievementFogs.push_back((uint32)actionId);

                        if (achievementFogs.size() >= 3 && instance)
                            instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2, SPELL_CROSSING_THE_STREAMS); // not requred to kill encounter

                        break;
                    default:
                        break;
                }
            }

            private:
                void SendMazeToEyes(bool start)
                {
                    std::list<Creature*> eyesList;
                    GetCreatureListWithEntryInGrid(eyesList, me, NPC_MIND_EYE, 200.0f);
                    GetCreatureListWithEntryInGrid(eyesList, me, NPC_HUNGRY_EYE, 200.0f);
                    GetCreatureListWithEntryInGrid(eyesList, me, NPC_APPRAYISYING_EYE, 200.0f);
                    for (auto&& eye : eyesList)
                        eye->AI()->DoAction(start == true ? ACTION_MAZE_START : ACTION_MAZE_END);
                }

                void SummonBeamsEye()
                {
                    // Init target list
                    std::list<Player*> pList;
                    GetPlayerListInGrid(pList, me, 200.0f);

                    pList.remove_if(TankSpecTargetSelector());

                    if (pList.empty())
                        return;

                    if (pList.size() > 3)
                        Trinity::Containers::RandomResizeList(pList, 3);

                    // Yellow Eye
                    if (Creature* eyeStatic = me->SummonCreature(NPC_YELLOW_EYE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()))
                    {
                        eyeStatic->AI()->SetGUID(ObjectGuid::Empty, 10 * IN_MILLISECONDS);

                        if (Player* target = pList.front())
                        {
                            if (pList.size() > 1)
                            {
                                eyeStatic->CastSpell(target, SPELL_BRIGHT_LIGHT_FOCUS, true);
                                pList.pop_front();
                            }
                            else
                                eyeStatic->CastSpell(target, SPELL_BRIGHT_LIGHT_FOCUS, true);
                        }
                    }

                    // Red Eye
                    if (Creature* eyeStatic = me->SummonCreature(NPC_RED_EYE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()))
                    {
                        eyeStatic->AI()->SetGUID(ObjectGuid::Empty, 10 * IN_MILLISECONDS);

                        if (Player* target = pList.front())
                        {
                            if (pList.size() > 1)
                            {
                                eyeStatic->CastSpell(target, SPELL_INFARED_TRACKING, true);
                                eyeStatic->CastSpell(target, SPELL_INFRARED_BEAM_FOCUS, true);
                                pList.pop_front();
                            }
                            else
                            {
                                eyeStatic->CastSpell(target, SPELL_INFARED_TRACKING, true);
                                eyeStatic->CastSpell(target, SPELL_INFRARED_BEAM_FOCUS, true);
                            }
                        }
                    }

                    // Blue Eye
                    if (Creature* eyeStatic = me->SummonCreature(NPC_BLUE_EYE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()))
                    {
                        eyeStatic->AI()->SetGUID(ObjectGuid::Empty, 10 * IN_MILLISECONDS);

                        if (Player* target = pList.front())
                        {
                            if (pList.size() > 1)
                            {
                                eyeStatic->CastSpell(target, SPELL_BLUE_RAY_TRACKING, true);
                                eyeStatic->CastSpell(target, SPELL_BLUE_RAYS_FOCUS, true);
                                pList.pop_front();
                            }
                            else
                            {
                                eyeStatic->CastSpell(target, SPELL_BLUE_RAY_TRACKING, true);
                                eyeStatic->CastSpell(target, SPELL_BLUE_RAYS_FOCUS, true);
                            }
                        }
                    }
                }

                void SummonEyes()
                {
                    me->SummonCreature(NPC_MIND_EYE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());

                    if (Creature* eye = me->SummonCreature(NPC_HUNGRY_EYE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()))
                        lifeDrainEyeGUID = eye->GetGUID();

                    me->SummonCreature(NPC_APPRAYISYING_EYE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                }

                void SummonSomeCreatures(uint32 entry, uint8 num, float minDist)
                {
                    for (uint8 i = 1; i <= num; ++i)
                    {
                        Position position = CenterDurumu;
                        while (me->GetDistance(position) <= minDist)
                            position = me->GetRandomPoint(CenterDurumu, 60.0f);
                        me->SummonCreature(entry, position);
                    }
                }

                void HandleMindDaggers(bool apply = true)
                {
                    std::list<Creature*> eyesList;
                    GetCreatureListWithEntryInGrid(eyesList, me, NPC_RED_EYE, 200.0f);
                    GetCreatureListWithEntryInGrid(eyesList, me, NPC_YELLOW_EYE, 200.0f);
                    GetCreatureListWithEntryInGrid(eyesList, me, NPC_BLUE_EYE, 200.0f);
                    GetCreatureListWithEntryInGrid(eyesList, me, NPC_MIND_EYE, 200.0f);
                    GetCreatureListWithEntryInGrid(eyesList, me, NPC_APPRAYISYING_EYE, 200.0f);

                    for (auto&& itr : eyesList)
                    {
                        if (apply)
                            DoCast(itr, SPELL_MIND_DAGGERS, true);
                        else
                            itr->RemoveAurasDueToSpell(SPELL_MIND_DAGGERS);

                    }
                }

                void UpdateOrientation()
                {
                    std::list<Creature*> beamTarList;
                    ObjectGuid tarGuid = ObjectGuid::Empty;
                    if (Unit* Beam = me->GetFirstMinionByEntry(NPC_YELLOW_EYE_MOVER))
                        tarGuid = Beam->GetGUID();

                    if (Creature* beamTar = ObjectAccessor::GetCreature(*me, tarGuid))
                        me->SetFacingToObject(beamTar);
                }

                void EndBeamPhase()
                {
                    phaseId = 0;
                    // RemoveBeamAuras
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLUE_BEAM);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RED_BEAM);

                    // Despawn creatures from phase
                    DespawnCreature(NPC_RED_EYE);
                    DespawnCreature(NPC_BLUE_EYE);
                    DespawnCreature(NPC_YELLOW_EYE);
                    DespawnCreature(NPC_YELLOW_EYE_MOVER);
                    DespawnCreature(NPC_BLUE_FOG);
                    DespawnCreature(NPC_YELLOW_FOG);

                    if (instance)
                    {
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INFRARED_LIGHT_ANN);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BRIGHT_LIGHT_ANN);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLUE_RAYS_ANN);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INFARED_TRACKING);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLUE_RAY_TRACKING);
                    }

                    events.CancelEvent(EVENT_SPECTRUM_LIMIT);
                }

                void Maze(bool start)
                {
                    me->SetReactState(start ? REACT_PASSIVE : REACT_AGGRESSIVE);
                    start ? events.CancelEvent(EVENT_HARD_STARE) : events.ScheduleEvent(EVENT_HARD_STARE, 2000);
                    start ? events.CancelEvent(EVENT_GAZE) : events.ScheduleEvent(EVENT_GAZE, 2000);

                    if (!start)
                    {
                        DespawnCreature(NPC_YELLOW_EYE_MOVER);
                        me->RemoveAllAreasTrigger();
                        HandleMindDaggers(false);

                        events.CancelEvent(EVENT_MAZE_ORIENTATION);

                        me->RemoveAurasDueToSpell(SPELL_DESINTEGRATION_BEAM);

                        if (Creature* eye = ObjectAccessor::GetCreature(*me, lifeDrainEyeGUID))
                            eye->AI()->DoAction(ACTION_EYE_DRAIN_LIFE); // find hungry eye to start drain life
                    }
                    else
                    {
                        scheduler.CancelAll();
                        me->PrepareChanneledCast(6.272f);

                        GetPositionWithDistInOrientation(me, 45.0f, 6.272f, x, y);

                        if (Creature* tarMover = me->SummonCreature(NPC_YELLOW_EYE_MOVER, x, y, me->GetPositionZ() + 3.0f))
                        {
                            me->CastSpell(tarMover, SPELL_DISINTEGRATION_BEAM_PRECAST, true);
                            me->SetGuidValue(UNIT_FIELD_CHANNEL_OBJECT, tarMover->GetGUID());
                            me->SetTarget(tarMover->GetGUID());
                        }

                        nonCombatEvents.ScheduleEvent(EVENT_NEAR_CONE_SLICE, 23.5 * IN_MILLISECONDS);

                        ori = M_PI / 2;

                        scheduler
                            .SetValidator([this] { return ori <= 9 * M_PI / 4; })
                            .Schedule(Milliseconds(500), [this](TaskContext context)
                        {
                            x = me->GetPositionX(), y = me->GetPositionY();

                            for (auto&& itr : invMazeType)
                            {
                                x = x + (itr.second * cos(Position::NormalizeOrientation(ori)));
                                y = y + (itr.second * sin(Position::NormalizeOrientation(ori)));
                                me->CastSpell(x, y, -6.27f, itr.first, true);
                            }

                            ori += M_PI / 8;

                            context.Repeat(Milliseconds(1000));
                        });
                    }
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_durumuAI(creature);
        }
};

// durumu fogs 69052, 69050, 69051
struct npc_durumu_fog : public ScriptedAI
{
    npc_durumu_fog(Creature* creature) : ScriptedAI(creature) 
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE);
        me->SetReactState(REACT_PASSIVE);
        me->SetVisible(false);
    }

    InstanceScript* instance;
    TaskScheduler scheduler;
    bool activate;
    uint32 healthPct;

    void IsSummonedBy(Unit* summoner) override
    {
        instance = me->GetInstanceScript();
        me->PrepareChanneledCast(me->GetOrientation());
        activate = false;
        healthPct = 90;
    }

    void JustDied(Unit* killer) override
    {
        if (Creature* durumu = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_DURUMU_THE_FORGOTTEN) : ObjectGuid::Empty))
        {
            if (me->GetEntry() == NPC_BLUE_FOG) // blue fog when die cast aoe and summon blue fog in another place
                DoCast(me, SPELL_FLASH_FREEZE, true);

            if (me->GetEntry() == NPC_RED_FOG)
                durumu->AI()->DoAction(ACTION_FOG_KILLED); // need 3 red fog killed for end phase
        }

        me->DespawnOrUnsummon(2.5 * IN_MILLISECONDS);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (me->GetEntry() == NPC_YELLOW_FOG && healthPct > 9 && HealthBelowPct(healthPct))
        {
            DoCast(me, SPELL_AMBER_RETAILATION, true);
            healthPct -= 10;
        }
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_FOG_ACTIVATE:
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE);
                me->SetVisible(true);
                activate = true;

                scheduler
                    .SetValidator([this] { return activate && me->GetEntry() != NPC_YELLOW_FOG; })
                    .Schedule(Milliseconds(500), [this](TaskContext context)
                {
                    DoCast(me, me->GetEntry() == NPC_BLUE_FOG ? SPELL_ICY_GRASP : SPELL_CAUSTIC_SPIKE);

                    context.Repeat(Seconds(2));
                });

                break;
            case ACTION_FOG_DEACTIVATE:
                activate = false;
                scheduler.CancelAll();
                me->InterruptNonMeleeSpells(true, SPELL_CAUSTIC_SPIKE);

                if (!me->IsAlive())
                    break;

                switch (me->GetEntry())
                {
                    case NPC_BLUE_FOG:
                        DoCast(me, SPELL_FLASH_FREEZE);
                        break;
                    case NPC_RED_FOG:
                        DoCast(me, SPELL_CRIMSON_BLOOM);
                        break;
                    case NPC_YELLOW_FOG:
                        DoCast(me, SPELL_BURST_OF_AMBER);
                        break;
                }

                me->SetVisible(false);
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// durumu fogs 69052, 69050, 69051
struct npc_durumu_fog_achievement : public ScriptedAI
{
    npc_durumu_fog_achievement(Creature* creature) : ScriptedAI(creature)
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetVisible(false);
    }

    InstanceScript* instance;
    uint32 requreHit;

    void IsSummonedBy(Unit* summoner) override
    {
        instance = me->GetInstanceScript();
        me->PrepareChanneledCast(me->GetOrientation());
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_FOG_ACTIVATE:
                me->SetVisible(true);

                if (Creature* durumu = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_DURUMU_THE_FORGOTTEN) : ObjectGuid::Empty))
                    durumu->AI()->DoAction(me->GetEntry());
                break;
            case ACTION_FOG_DEACTIVATE:
                me->SetVisible(false);
                break;
        }
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// durumu eyebeam mover 67829
struct npc_eyebeam_target : public ScriptedAI
{
    npc_eyebeam_target(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    float x, y;
    bool byDurumu;

    void IsSummonedBy(Unit* summoner) override
    {
        if (Creature* durumu = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_DURUMU_THE_FORGOTTEN) : ObjectGuid::Empty))
            durumu->AI()->JustSummoned(me);

        byDurumu = summoner->GetEntry() == NPC_DURUMU_THE_FORGOTTEN ? true : false;

        me->SetSpeed(MOVE_RUN, byDurumu ? 0.8f : 0.45f);
        x = 0.0f; y = 0.0f;
        me->SetDisplayId(11686);
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

        if (!byDurumu)
            InitializeMovement();
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_DISINTEGRATION_BEAM)
            InitializeMovement();
    }

    void UpdateAI(uint32 diff) override { }

    private:
        void InitializeMovement()
        {
            Movement::MoveSplineInit init(me);
            for (uint8 i = 1; i < 25; ++i)
            {
                x = CenterDurumu.GetPositionX() + 45.0f * cos(Position::NormalizeOrientation(me->GetAngle(CenterDurumu.GetPositionX(), CenterDurumu.GetPositionY()) + (i * M_PI / 12) - M_PI));
                y = CenterDurumu.GetPositionY() + 45.0f * sin(Position::NormalizeOrientation(me->GetAngle(CenterDurumu.GetPositionX(), CenterDurumu.GetPositionY()) + (i * M_PI / 12) - M_PI));
                init.Path().push_back(G3D::Vector3(x, y, me->GetPositionZ()));
            }

            init.SetUncompressed();
            init.SetCyclic();
            init.Launch();
        }
};

// Durumu Eyes 67875 ,67859, 67858, 67856, 67854, 67855
struct npc_durumu_eye : public ScriptedAI
{
    npc_durumu_eye(Creature* creature) : ScriptedAI(creature)
    {
        instance = creature->GetInstanceScript();
        me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
    }

    InstanceScript* instance;
    EventMap events;
    int32 beamDuration;
    uint32 lifeDrainLostDuration;
    ObjectGuid lifeDrainGUID;
    bool hasLifeDrainEyeActive;

    void Reset() override
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        InitializeEvents();
        lifeDrainGUID = ObjectGuid::Empty;
        lifeDrainLostDuration = 0;
        hasLifeDrainEyeActive = false;
    }

    void SetGUID(ObjectGuid guid, int32 type) override
    {
        if (type)
            beamDuration = type;
        else
            lifeDrainGUID = guid;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return lifeDrainGUID;
    }

    uint32 GetData(uint32 type) const override
    {
        switch (type)
        {
            case TYPE_BEAM_DURATION:
                return (uint32)beamDuration;
            case TYPE_LIFE_DRAIN_DURATION:
                return lifeDrainLostDuration;
        }

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_LIFE_DRAIN_DURATION)
            lifeDrainLostDuration = data;
    }

    void InitializeEvents()
    {
        switch (me->GetEntry())
        {
            case NPC_MIND_EYE:
                events.ScheduleEvent(EVENT_MOVE_RANDOM, 2000);
                events.ScheduleEvent(EVENT_FORCE_OF_WILL, 32000);
                break;
            case NPC_HUNGRY_EYE:
                me->PrepareChanneledCast(me->GetOrientation());
                events.ScheduleEvent(EVENT_MOVE_RANDOM, 2000);
                break;
            case NPC_APPRAYISYING_EYE:
                events.ScheduleEvent(EVENT_MOVE_RANDOM, 2000);
                events.ScheduleEvent(EVENT_LINGERING_GAZE, 15000);
                break;
            case NPC_YELLOW_EYE:
            case NPC_BLUE_EYE:
            case NPC_RED_EYE:
                events.ScheduleEvent(EVENT_EYES_ORIENTATION, 1000);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                break;
            default:
                break;
        }

        me->SetDisableGravity(true);
        me->SetCanFly(true);
        me->SetInCombatWithZone();
    }

    void RandomMove()
    {
        Position position = me->GetRandomPoint(CenterDurumu, 5.0f);

        if (!me->HasUnitState(UNIT_STATE_CASTING))
            me->GetMotionMaster()->MovePoint(0, position);
    }

    ObjectGuid SelectBeamTargetGuid()
    {
        if (me->GetEntry() == NPC_YELLOW_EYE)
        {
            std::list<Creature*> beamTarList;
            GetCreatureListWithEntryInGrid(beamTarList, me, NPC_YELLOW_EYE_MOVER, 80.0f);

            for (auto&& Beam : beamTarList)
                return Beam->GetGUID();
        }
        else // red or blue beam
        {
            std::list<Player*> PlayerList;
            GetPlayerListInGrid(PlayerList, me, 80.0f);

            for (auto&& itr : PlayerList)
                if (itr->HasAura(me->GetEntry() == NPC_RED_EYE ? SPELL_RED_BEAM : SPELL_BLUE_BEAM))
                    return itr->GetGUID();
        }

        return ObjectGuid::Empty;
    }

    void TrySearchFogs()
    {
        std::list<Creature*> fogList;
        GetCreatureListWithEntryInGrid(fogList, me, me->GetEntry() == NPC_RED_EYE ? NPC_RED_FOG : NPC_BLUE_FOG, 80.0f);

        for (auto&& fog : fogList)
        {
            if (me->isInFront(fog, M_PI / 7)) // if fog is front
                fog->AI()->DoAction(ACTION_FOG_ACTIVATE);
            else if (fog->IsVisible())
                fog->AI()->DoAction(ACTION_FOG_DEACTIVATE);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_MOVE_RANDOM:
                    RandomMove();
                    events.ScheduleEvent(EVENT_MOVE_RANDOM, urand(4000, 12000));
                    break;
                case EVENT_FORCE_OF_WILL:
                    if (HasInMazeOrSpectrumPhase(eventId))
                        break;

                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                    {
                        me->SetFacingTo(me->GetAngle(target));
                        DoCast(target, SPELL_FORCE_OF_WILL);
                    }
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                    {
                        me->SetFacingTo(me->GetAngle(target));
                        DoCast(target, SPELL_FORCE_OF_WILL);
                    }

                    if (Creature* durumu = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_DURUMU_THE_FORGOTTEN) : ObjectGuid::Empty))
                        durumu->GetAI()->DoAction(ACTION_SAY_FORCE_OF_WILL);

                    events.ScheduleEvent(EVENT_FORCE_OF_WILL, 20000);
                    break;
                case EVENT_LINGERING_GAZE:
                    if (HasInMazeOrSpectrumPhase(eventId))
                        break;

                    DoCast(me, SPELL_LINGERING_GAZE);
                    events.ScheduleEvent(EVENT_LINGERING_GAZE, 45000);
                    break;
                case EVENT_DRAIN_LIFE:
                    if (HasInMazeOrSpectrumPhase(eventId))
                        break;

                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                    {
                        me->SetFacingTo(me->GetAngle(target));
                        DoCast(target, SPELL_DRAIN_LIFE_CHANNEL);
                        lifeDrainGUID = target->GetGUID();
                    }
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                    {
                        me->SetFacingTo(me->GetAngle(target));
                        DoCast(target, SPELL_DRAIN_LIFE_CHANNEL);
                        lifeDrainGUID = target->GetGUID();
                    }

                    events.ScheduleEvent(EVENT_DRAIN_LIFE, urand(45 * IN_MILLISECONDS, 50 * IN_MILLISECONDS));
                    break;
                case EVENT_EYES_ORIENTATION:
                    if (Unit* target = ObjectAccessor::GetUnit(*me, SelectBeamTargetGuid()))
                        me->SetFacingToObject(target);
                
                    if (me->GetEntry() != NPC_YELLOW_EYE)
                        TrySearchFogs();
                
                    events.ScheduleEvent(EVENT_EYES_ORIENTATION, 200);
                    break;
            }
        }
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_EYE_DRAIN_LIFE:
                if (hasLifeDrainEyeActive)
                    break;

                hasLifeDrainEyeActive = true;
                events.ScheduleEvent(EVENT_DRAIN_LIFE, 12000);
                break;
            case ACTION_MAZE_START:
                events.CancelEvent(EVENT_LINGERING_GAZE);
                events.CancelEvent(EVENT_FORCE_OF_WILL);
                break;
            case ACTION_MAZE_END:
                switch (me->GetEntry())
                {
                    case NPC_MIND_EYE:
                        events.ScheduleEvent(EVENT_FORCE_OF_WILL, 25000);
                        break;
                    case NPC_APPRAYISYING_EYE:
                        events.ScheduleEvent(EVENT_LINGERING_GAZE, 19000);
                        break;
                }
                break;
            case ACTION_SWITCH_BEAM_TARGET:
            {
                std::list<Player*> pList, copyList;
                GetPlayerListInGrid(pList, me, 200.0f);
                copyList = pList;

                pList.remove_if(TankSpecTargetSelector());
                pList.remove_if([=](Player* target) { return target && (!target->IsAlive() || target->HasAura(SPELL_INFRARED_BEAM_FOCUS) || target->HasAura(SPELL_BLUE_RAYS_FOCUS) || target->HasAura(SPELL_BRIGHT_LIGHT_FOCUS)); });

                if (pList.empty())
                {
                    copyList.remove_if([=](Player* target) { return target && !target->IsAlive(); });

                    if (!copyList.empty())
                        if (Player* target = Trinity::Containers::SelectRandomContainerElement(copyList))
                            DoCast(target, invBeamEyeType.find(me->GetEntry())->second, true);

                    break;
                }

                if (Player* target = Trinity::Containers::SelectRandomContainerElement(pList))
                    DoCast(target, invBeamEyeType.find(me->GetEntry())->second, true);

                break;
            }
        }
    }

    private:
        bool HasInMazeOrSpectrumPhase(uint32 eventId)
        {
            if (Creature* durumu = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_DURUMU_THE_FORGOTTEN) : ObjectGuid::Empty))
            {
                switch (durumu->AI()->GetData(TYPE_PHASE))
                {
                    case 1:
                        if (eventId != EVENT_DRAIN_LIFE && eventId != EVENT_LINGERING_GAZE)
                        {
                            events.RescheduleEvent(eventId, urand(2.5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
                            return true;
                        }
                        else
                            return false;
                    case 2:
                        events.RescheduleEvent(eventId, urand(2.5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
                        return true;
                }
            }

            return false;
        }
};

// Ice Wall 69582
struct npc_durumu_ice_wall : public ScriptedAI
{
    npc_durumu_ice_wall(Creature* creature) : ScriptedAI(creature) { }

    InstanceScript* instance;
    std::vector<ObjectGuid> icewallGUIDs;
    float x, y;
    bool byEncounter;
    ObjectGuid ownerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        me->setRegeneratingHealth(false);
        // doesn`t know real value...
        me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 1.52f);
        me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 25.0f);

        instance = me->GetInstanceScript();
        DoCast(me, SPELL_ICEWALL_SPAWN, true);

        if (Creature* durumu = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_DURUMU_THE_FORGOTTEN) : ObjectGuid::Empty))
        {
            durumu->AI()->JustSummoned(me);
            me->SetFacingTo(Position::NormalizeOrientation(me->GetAngle(durumu) + M_PI));
        }

        if (summoner->GetEntry() == NPC_DURUMU_THE_FORGOTTEN)
        {
            for (uint32 i = 1; i < 10; i++)
            {
                GetPositionWithDistInOrientation(me, 10.0f * i, me->GetOrientation(), x, y);

                if (Creature* partIcewall = me->SummonCreature(NPC_ICE_WALL, x, y, -6.278f, TEMPSUMMON_MANUAL_DESPAWN))
                {
                    if (i == 4) // optimally
                    {
                        partIcewall->CastSpell(partIcewall, SPELL_ICEWALL_VISUAL, true);
                        partIcewall->CastSpell(partIcewall, SPELL_ICE_WALL_COLLISION, true);
                    }
                }
            }
        }
        else
            ownerGUID = summoner->GetGUID();
    }

    void JustSummoned(Creature* summon) override
    {
        icewallGUIDs.push_back(summon->GetGUID());
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (Creature* ownerWall = ObjectAccessor::GetCreature(*me, ownerGUID))
        {
            ownerWall->AI()->DamageTaken(me, damage);
            damage = 0;
            return;
        }

        if (me->GetHealth() <= damage)
        {
            me->RemoveAurasDueToSpell(SPELL_ICEWALL_VISUAL);

            for (auto&& itr : icewallGUIDs)
                if (Creature* icewall = ObjectAccessor::GetCreature(*me, itr))
                    me->Kill(icewall);

            me->Kill(me);
        }
        else
        {
            me->ModifyHealth(-int32(damage));

            for (auto&& itr : icewallGUIDs)
                if (Creature* icewall = ObjectAccessor::GetCreature(*me, itr))
                    icewall->ModifyHealth(-int32(damage));
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        // Remove Collision
        std::list<GameObject*> iceWallList;
        GetGameObjectListWithEntryInGrid(iceWallList, me, GO_ICE_WALL, 200.0f);

        for (auto&& itr : iceWallList)
            if (itr->GetOwnerGUID() == me->GetGUID())
                itr->Delete();

        me->DisappearAndDie();
    }

    void UpdateAI(uint32 diff) override { }
};

// Arterial Cut 133768
class spell_arterial_cut : public AuraScript
{
    PrepareAuraScript(spell_arterial_cut);

    void OnTick(AuraEffect const* aurEff)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (owner->GetHealth() >= owner->GetMaxHealth())
                owner->RemoveAurasDueToSpell(GetId());
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_arterial_cut::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// Drain life dmg calculate 133798
class spell_drain_life_damage : public SpellScriptLoader
{
    public:
        spell_drain_life_damage() : SpellScriptLoader("spell_drain_life_damage") { }

        class spell_drain_life_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_drain_life_damage_SpellScript);

            void CalculateDamage(SpellEffIndex /*effIndex*/)
            { 
                if (!GetCaster() || !GetHitUnit())
                    return;

                int32 amount = GetHitDamage();

                if (GetHitUnit()->HasAura(SPELL_DRAIN_LIFE_DAMAGE))
                    amount += (amount * 0.60)*GetHitUnit()->GetAura(SPELL_DRAIN_LIFE_DAMAGE)->GetStackAmount();

                int32 bp = amount * 25;
                std::list<Creature*> durumu;
                GetCreatureListWithEntryInGrid(durumu, GetCaster(), 68036, 100.0f);

                for (auto&& durumuTarget : durumu)
                    durumuTarget->CastCustomSpell(durumuTarget,SPELL_DRAIN_LIFE_HEAL,&bp,NULL,NULL,true);

                SetHitDamage(amount);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_drain_life_damage_SpellScript::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_drain_life_damage_SpellScript();
        }
};

// force of will removing 136932
class spell_force_of_will : public AuraScript
{
    PrepareAuraScript(spell_force_of_will);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_FORCE_OF_WILL_KNOCKBACK, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_force_of_will::HandleAuraEffectRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Beam damage 133732 133677 133738
class spell_beam_aoe_dmg : public SpellScript
{
    PrepareSpellScript(spell_beam_aoe_dmg);

    uint32 targetCount;

    bool Load() override
    {
        targetCount = 1;
        return true;
    }

    void HandleEffectHitTarget(SpellEffIndex /*eff_idx*/)
    {
        if (targetCount > 1)
            SetHitDamage(GetHitDamage() / targetCount);
    }

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        if (!GetCaster())
            return;

        targetCount = targets.size();

        if (targetCount == 0)
        {
            if (GetSpellInfo()->Id == SPELL_RED_BEAM_DMG) // redbeam dmg
                GetCaster()->CastSpell(GetCaster(), SPELL_RED_BEAM_DMG_TO_ALL, true); //aoe dmg to all
            if (GetSpellInfo()->Id == SPELL_BLUE_BEAM_DMG) // bluebeam dmg
                GetCaster()->CastSpell(GetCaster(), SPELL_BLUE_BEAM_DMG_TO_ALL, true); //aoe dmg to all
            if (GetSpellInfo()->Id == SPELL_YELLOW_BEAM_DMG) // yellowbeam dmg
                GetCaster()->CastSpell(GetCaster(), SPELL_YELLOW_BEAM_DMG_TO_ALL, true); //aoe dmg to all
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_beam_aoe_dmg::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_beam_aoe_dmg::SelectTarget, EFFECT_0, TARGET_UNIT_CONE_ENEMY_110);
    }
};

// Lingering Gaze 138467
class spell_lingering_gaze : public SpellScript
{
    PrepareSpellScript(spell_lingering_gaze);

    void HandleScriptEff(SpellEffIndex /*effIndex*/)
    {
        // No Selector for this spell
        if (Creature* caster = GetCaster()->ToCreature())
        {
            std::list<Player*> pList, tempList;
            GetPlayerListInGrid(pList, caster, 200.0f);
            tempList = pList;

            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if(MeeleSpecTargetSelector());

            uint32 targetCount = caster->GetMap()->Is25ManRaid() ? 5 : 2;

            if (pList.size() >= targetCount)
            {
                if (pList.size() > targetCount)
                    Trinity::Containers::RandomResizeList(pList, targetCount);

                for (auto&& itr : pList)
                    caster->CastSpell(itr, SPELL_LINGERING_GAZE_MARK, true);

                return;
            }

            // If not enough casters
            if (tempList.size() > targetCount)
                Trinity::Containers::RandomResizeList(tempList, targetCount);

            for (auto&& itr : tempList)
                caster->CastSpell(itr, SPELL_LINGERING_GAZE_MARK, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_lingering_gaze::HandleScriptEff, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Lingering Gaze Mark 134626
class spell_lingering_gaze_mark : public AuraScript
{
    PrepareAuraScript(spell_lingering_gaze_mark);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* caster = GetCaster())
            if (Unit* owner = GetOwner()->ToUnit())
                caster->CastSpell(owner, SPELL_LINGERING_GAZE_MISSLE, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_lingering_gaze_mark::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Focus Beam Precast 134123, 134124, 134122
class spell_focus_beam_precast : public AuraScript
{
    PrepareAuraScript(spell_focus_beam_precast);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetCaster() && GetCaster()->ToCreature())
            aurEff->GetBase()->SetDuration(GetCaster()->ToCreature()->AI()->GetData(TYPE_BEAM_DURATION));
    }

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* caster = GetCaster())
        {
            if (aurEff->GetBase()->GetDuration() > 0)
            {
                if (caster->ToCreature())
                {
                    caster->ToCreature()->AI()->SetGUID(ObjectGuid::Empty, aurEff->GetBase()->GetDuration());
                    caster->ToCreature()->AI()->DoAction(ACTION_SWITCH_BEAM_TARGET);
                }

                return;
            }

            if (Unit* owner = GetOwner()->ToUnit())
            {
                if (uint32 spellId = invBeamsType.find(GetSpellInfo()->Id)->second)
                    caster->CastSpell(owner, spellId, true);
                else // yellow beam doesn`t fixate to player
                {
                    x = CenterDurumu.GetPositionX() + (45.0f * cos(Position::NormalizeOrientation(owner->GetAngle(&CenterDurumu) - M_PI)));
                    y = CenterDurumu.GetPositionY() + (45.0f * sin(Position::NormalizeOrientation(owner->GetAngle(&CenterDurumu) - M_PI)));

                    if (Creature* tarMover = caster->SummonCreature(NPC_YELLOW_EYE_MOVER, x, y, owner->GetPositionZ())) // Eyebeam target(mover)
                        caster->CastSpell(tarMover, SPELL_YELLOW_BEAM, true);
                }
            }
        }
    }

    private:
        float x, y;

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_focus_beam_precast::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_focus_beam_precast::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Disintegration Beam Precast 134169
class spell_disintegration_beam_precast : public AuraScript
{
    PrepareAuraScript(spell_disintegration_beam_precast);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            if (Unit* caster = GetCaster())
            {
                caster->CastSpell(caster, SPELL_DESINTEGRATION_BEAM, true);
                caster->SetGuidValue(UNIT_FIELD_CHANNEL_OBJECT, owner->GetGUID());
                caster->SetTarget(owner->GetGUID());
            }

            owner->AI()->DoAction(ACTION_DISINTEGRATION_BEAM);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_disintegration_beam_precast::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Disintegration Beam 133776
class spell_disintegration_beam : public AuraScript
{
    PrepareAuraScript(spell_disintegration_beam);

    void OnUpdate(uint32 diff)
    {
        if (Unit* caster = GetCaster())
            if (Unit* owner = GetOwner()->ToUnit())
                caster->UpdateOrientation(caster->GetAngle(owner)); // Handle update server-side orientation
    }

    void Register() override
    {
        OnAuraUpdate += AuraUpdateFn(spell_disintegration_beam::OnUpdate);
    }
};

// Icy Grasp 136177
class spell_durumu_icy_grasp : public SpellScript
{
    PrepareSpellScript(spell_durumu_icy_grasp);

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && !target->ToUnit()->HasAura(SPELL_BLUE_RAYS_ANN); });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_durumu_icy_grasp::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Caustic Spike 136154
class spell_durumu_caustic_spike : public SpellScript
{
    PrepareSpellScript(spell_durumu_caustic_spike);

    void RecalculateDamage(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            uint32 stacks = target->GetAura(SPELL_INFARED_LIGHT_AURA_EFF) ? target->GetAura(SPELL_INFARED_LIGHT_AURA_EFF)->GetStackAmount() : 0;
            float mod = (stacks * 2.0f) / 100.0f; // 2% per stack
            int32 recalculatedDamage = GetHitDamage() + (int32)(GetHitDamage() * mod);

            SetHitDamage(recalculatedDamage);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_durumu_caustic_spike::RecalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Mind Daggers 139107
class spell_durumu_mind_daggers : public SpellScript
{
    PrepareSpellScript(spell_durumu_mind_daggers);

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
        {
            uint32 targetCount = caster->GetMap()->Is25ManRaid() ? 3 : 1;

            if (targets.size() > targetCount)
                Trinity::Containers::RandomResizeList(targets, targetCount);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_durumu_mind_daggers::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

class LifeDrainPredicate 
{
    public:
        LifeDrainPredicate(Creature* const m_caster) : _caster(m_caster) { }
    
        bool operator()(WorldObject* object)
        {
            if (Unit* currentTarget = ObjectAccessor::GetUnit(*_caster, _caster->AI()->GetGUID()))
                return object && object->ToPlayer() && (!object->ToPlayer()->IsInBetween(_caster, currentTarget, 3.0f) || currentTarget->GetGUID() == object->GetGUID());

            return false;
        }
    
    private:
        Creature const* _caster;
};

// Life Drain Eff 133798
class spell_durumu_life_drain_eff : public SpellScript
{
    PrepareSpellScript(spell_durumu_life_drain_eff);

    std::list<WorldObject*> m_targets;

    void HandleHealByDamage(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                // Remove Stun from prevTarget
                if (Unit* prevTarget = ObjectAccessor::GetUnit(*caster, caster->AI()->GetGUID()))
                    if (target->GetGUID() != prevTarget->GetGUID())
                        prevTarget->RemoveAurasDueToSpell(SPELL_DRAIN_LIFE_STUN);

                if (target->GetGUID() != caster->AI()->GetGUID())
                    caster->AddAura(SPELL_DRAIN_LIFE_STUN, target);

                caster->AI()->SetGUID(target->GetGUID());

                // Heal Encounter in 25 times more of damage value
                float mod = target->GetAura(GetSpellInfo()->Id) ? target->GetAura(GetSpellInfo()->Id)->GetStackAmount() - 1 : 0; // 60% per stack
                int32 bp = 25 * (GetHitDamage() + (int32)(GetHitDamage() * ((60.0f * mod) / 100.0f)));
                caster->CastCustomSpell(caster, SPELL_DRAIN_LIFE_HEAL, &bp, 0, 0, true);
            }
        }
    }

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            targets.remove_if(LifeDrainPredicate(caster));

            // If not any a cross - return old target
            if (targets.empty())
            {
                if (Unit* currentTarget = ObjectAccessor::GetUnit(*caster, caster->AI()->GetGUID()))
                    targets.push_back(currentTarget);
            }
            else
            {
                targets.sort(Trinity::ObjectDistanceOrderPred(caster, true));
                targets.resize(1);
            }

            m_targets = targets;
        }
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_durumu_life_drain_eff::HandleHealByDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_durumu_life_drain_eff::SelectTarget, EFFECT_0, TARGET_UNIT_CONE_ENEMY_110);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_durumu_life_drain_eff::CopyTargets, EFFECT_1, TARGET_UNIT_CONE_ENEMY_110);
    }
};

// Life Drain Eff 133798
class spell_durumu_life_drain_eff_aura : public AuraScript
{
    PrepareAuraScript(spell_durumu_life_drain_eff_aura);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* caster = GetCaster())
            // Save Last Duration on Remove
            if (GetCaster() && GetCaster()->ToCreature() && aurEff->GetBase()->GetDuration() > 0)
                GetCaster()->ToCreature()->AI()->SetData(TYPE_LIFE_DRAIN_DURATION, aurEff->GetBase()->GetDuration());
    }

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetCaster() && GetCaster()->ToCreature() && GetCaster()->ToCreature()->AI()->GetData(TYPE_LIFE_DRAIN_DURATION) > 0)
            SetDuration(GetCaster()->ToCreature()->AI()->GetData(TYPE_LIFE_DRAIN_DURATION));
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_durumu_life_drain_eff_aura::HandleAuraEffectRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectApply += AuraEffectApplyFn(spell_durumu_life_drain_eff_aura::HandleAuraEffectApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Life Drain Heal Eff 133807
class spell_durumu_life_drain_heal_eff : public SpellScript
{
    PrepareSpellScript(spell_durumu_life_drain_heal_eff);

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_DURUMU_THE_FORGOTTEN; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_durumu_life_drain_heal_eff::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Life Drain 133795
class spell_durumu_life_drain : public SpellScript
{
    PrepareSpellScript(spell_durumu_life_drain);

    void HandleHealByDamage(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->AddAura(SPELL_DRAIN_LIFE_STUN, target);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_durumu_life_drain::HandleHealByDamage, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Life Drain 133795
class spell_durumu_life_drain_aura : public AuraScript
{
    PrepareAuraScript(spell_durumu_life_drain_aura);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            // Handle Remove Stun Eff
            std::list<Player*> targets;
            GetPlayerListInGrid(targets, owner, 200.0f);

            for (auto&& itr : targets)
                itr->RemoveAurasDueToSpell(SPELL_DRAIN_LIFE_STUN);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_durumu_life_drain_aura::HandleAuraEffectRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 577. Summoned by 133793 - Lingering Gaze
class sat_lingering_gaze: public IAreaTriggerAura
{
    std::vector<ObjectGuid> affectedTargetGUIDs;

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* caster = GetCaster())
            if (Player* target = triggering->ToPlayer())
                caster->CastSpell(target, SPELL_LINGERING_GAZE_AT_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_LINGERING_GAZE_AT_EFF);
    }
};

// 566. Summoned by 133778 - Disintegration Beam
class sat_disintegration_beam : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer();
    }

    void OnTriggeringUpdate(WorldObject* triggering) override
    {
        if (Unit* caster = GetCaster())
            if (Player* target = triggering->ToPlayer())
                if (caster->HasInLine(target, 1.5f))
                    caster->Kill(target);
    }
};

// 569, 565, 571. Summoned By 133734, 133672, 133740 - Spectrum Beam
class sat_optic_spectrum : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && (triggering->ToPlayer() && GetCaster() && GetCaster()->HasInArc((55 * M_PI) / 180.0f, triggering->ToPlayer()) || triggering->ToCreature() && (triggering->ToCreature()->GetEntry() == NPC_ORANGE_FOG_BEAST || triggering->ToCreature()->GetEntry() == NPC_GREEN_FOG_BEAST || triggering->ToCreature()->GetEntry() == NPC_PURPLE_FOG_BEAST) && GetCaster()->HasInArc((55 * M_PI) / 180.0f, triggering->ToCreature()));
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Player* target = triggering->ToPlayer())
                caster->CastSpell(target, invCollorType.find(caster->GetEntry())->second, true);
            else if (Creature* fog = triggering->ToCreature())
            {
                // Cast spectrum visibility and check if colors is match
                if (AreaTrigger* at = GetAreaTrigger())
                    fog->CastSpell(fog, invSpectrumMarkType.find(at->GetEntry())->second, true);

                auto ptr = invAchievFogsType.find(fog->GetEntry());

                if (fog->HasAura(ptr->second[0]) && fog->HasAura(ptr->second[1]))
                    fog->AI()->DoAction(ACTION_FOG_ACTIVATE);
            }
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Player* target = triggering->ToPlayer())
                target->RemoveAurasDueToSpell(invCollorType.find(caster->GetEntry())->second);
            else if (Creature* fog = triggering->ToCreature())
            {
                // Remove our auras and send action
                if (AreaTrigger* at = GetAreaTrigger())
                    fog->RemoveAurasDueToSpell(invSpectrumMarkType.find(at->GetEntry())->second);

                fog->AI()->DoAction(ACTION_FOG_DEACTIVATE);
            }
        }
    }
};

// 660, 661, 662, 663, 664, 665, 666, 667, 668, 669. Summoned by 136232 - 136241 - Whole Slice Room
class sat_maze : public IAreaTriggerAura
{
    void OnInit() override 
    {
        uint32 sDuration = 0;

        if (AreaTrigger* at = GetAreaTrigger())
        {
            if (Creature* durumu = ObjectAccessor::GetCreature(*GetCaster(), GetCaster()->GetInstanceScript() ? GetCaster()->GetInstanceScript()->GetGuidData(DATA_DURUMU_THE_FORGOTTEN) : ObjectGuid::Empty))
            {
                switch (at->GetEntry())
                {
                    case 665:
                        at->SetDuration(18 * IN_MILLISECONDS + durumu->AI()->GetData(TYPE_LOW_MAZE_1) * (durumu->AI()->GetData(TYPE_LOW_MAZE_1) > 3 ? 2100 : 2800));
                        durumu->AI()->SetData(TYPE_LOW_MAZE_1, durumu->AI()->GetData(TYPE_LOW_MAZE_1) + 1);
                        break;
                    case 666:
                        at->SetDuration(18 * IN_MILLISECONDS + durumu->AI()->GetData(TYPE_LOW_MAZE_2) * (durumu->AI()->GetData(TYPE_LOW_MAZE_2) > 3 ? 2100 : 2800));
                        durumu->AI()->SetData(TYPE_LOW_MAZE_2, durumu->AI()->GetData(TYPE_LOW_MAZE_2) + 1);
                        break;
                    case 668:
                        at->SetDuration(18 * IN_MILLISECONDS + durumu->AI()->GetData(TYPE_HIGH_MAZE_1) * (durumu->AI()->GetData(TYPE_HIGH_MAZE_1) > 5 ? 2300 : 2700));
                        durumu->AI()->SetData(TYPE_HIGH_MAZE_1, durumu->AI()->GetData(TYPE_HIGH_MAZE_1) + 1);
                        break;
                    case 669:
                        at->SetDuration(18 * IN_MILLISECONDS + durumu->AI()->GetData(TYPE_HIGH_MAZE_2) * (durumu->AI()->GetData(TYPE_HIGH_MAZE_2) > 5 ? 2300 : 2700));
                        durumu->AI()->SetData(TYPE_HIGH_MAZE_2, durumu->AI()->GetData(TYPE_HIGH_MAZE_2) + 1);
                        break;
                    default:
                        at->SetDuration(3 * MINUTE * IN_MILLISECONDS);
                        break;
                }
            }
        }
    }

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (Player* target = triggering->ToPlayer())
                caster->CastSpell(target, SPELL_EYE_SORE, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (Player* target = triggering->ToPlayer())
                target->RemoveAura(SPELL_EYE_SORE, caster->GetGUID(), 0, AURA_REMOVE_BY_EXPIRE);
    }
};

// Areatrigger 8936
class AreaTrigger_down_durumu : public AreaTriggerScript
{
    public:
        AreaTrigger_down_durumu() : AreaTriggerScript("AreaTrigger_down_durumu") { }
    
        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            player->Kill(player);
            return false;
        }
};

void AddSC_boss_durumu()
{
    new boss_durumu();
    new creature_script<npc_eyebeam_target>("npc_eyebeam_target");
    new creature_script<npc_durumu_fog>("npc_durumu_fog");
    new creature_script<npc_durumu_fog_achievement>("npc_durumu_fog_achievement");
    new creature_script<npc_durumu_eye>("npc_durumu_eye");
    new creature_script<npc_durumu_ice_wall>("npc_durumu_ice_wall");

    new spell_drain_life_damage();
    new aura_script<spell_arterial_cut>("spell_arterial_cut");
    new aura_script<spell_force_of_will>("spell_force_of_will");
    new spell_script<spell_beam_aoe_dmg>("spell_beam_aoe_dmg");
    new spell_script<spell_lingering_gaze>("spell_lingering_gaze");
    new aura_script<spell_lingering_gaze_mark>("spell_lingering_gaze_mark");
    new aura_script<spell_focus_beam_precast>("spell_focus_beam_precast");
    new aura_script<spell_disintegration_beam_precast>("spell_disintegration_beam_precast");
    new aura_script<spell_disintegration_beam>("spell_disintegration_beam");
    new spell_script<spell_durumu_icy_grasp>("spell_durumu_icy_grasp");
    new spell_script<spell_durumu_caustic_spike>("spell_durumu_caustic_spike");
    new spell_script<spell_durumu_mind_daggers>("spell_durumu_mind_daggers");
    new spell_script<spell_durumu_life_drain_eff>("spell_durumu_life_drain_eff");
    new aura_script<spell_durumu_life_drain_eff_aura>("spell_durumu_life_drain_eff_aura");
    new spell_script<spell_durumu_life_drain_heal_eff>("spell_durumu_life_drain_heal_eff");
    new spell_script<spell_durumu_life_drain>("spell_durumu_life_drain");
    new aura_script<spell_durumu_life_drain_aura>("spell_durumu_life_drain_aura");
    new atrigger_script<sat_lingering_gaze>("sat_lingering_gaze");
    new atrigger_script<sat_disintegration_beam>("sat_disintegration_beam");
    new atrigger_script<sat_optic_spectrum>("sat_optic_spectrum");
    new atrigger_script<sat_maze>("sat_maze");
    new AreaTrigger_down_durumu();
}