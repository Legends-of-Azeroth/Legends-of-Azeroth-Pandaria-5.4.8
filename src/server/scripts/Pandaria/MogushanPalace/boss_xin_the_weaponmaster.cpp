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
#include "ScriptedCreature.h"
#include "mogu_shan_palace.h"
#include "Log.h"
#include "Pet.h"

enum Spells
{
    SPELL_BLADES_1           = 124531,
    SPELL_BLADES_2           = 124534,

    SPELL_TURN_ON_BLADES     = 119315,
    SPELL_TURN_OFF_BLADES    = 124540,
    SPELL_BLADE_TRAP_VISUAL  = 119388,

    SPELL_SUMMON_STAFF       = 120109,
    SPELL_SUMMON_AXES        = 120083,

    SPELL_STAFF_FIREBALL     = 119528,

    SPELL_SUMMON_FIRE_RING   = 119541,
    SPELL_FIRE_RING_AURA     = 119544,
    SPELL_FIRE_RING_VISUAL   = 119549,
    SPELL_EXPLODE_FIRE_RING  = 119590,

    SPELL_GROUND_SLAM        = 119684,

    SPELL_INCITING_ROAR      = 122959,

    SPELL_DART_AURA          = 120143,
    SPELL_DART_DAMAGE        = 120142
};

enum Creatures
{
    CREATURE_SWORD_LAUNCHER  = 59481,
};

enum Actions
{
    ACTION_ACTIVATE_BUTTON   = 0,
    ACTION_DEACTIVATE_BUTTON = 1,
};

// Constants to define which world triggers can be chosen as blade launchers

static const float m_fLessThanYPos = -2647.7f;
static const float m_fMoreThanYPos = -2578.6f;

static const Position m_fCenterPos = { -4632.8f, -2615.f, 21.8f, 0.0f };

static const uint32 aBladeSpells[2] =
{
    SPELL_BLADES_1,
    SPELL_BLADES_2
};

class boss_xin_the_weaponmaster : public CreatureScript
{
    public:
        boss_xin_the_weaponmaster() : CreatureScript("boss_xin_the_weaponmaster") { }

        enum eEvents
        {
            EVENT_RING_OF_FIRE          = 1,
            EVENT_GROUND_SLAM           = 2,
            EVENT_INCITING_ROAR         = 3,
            EVENT_AXES_ACTIVATE         = 4,
            EVENT_CHECK_SWORDS          = 5,
            EVENT_CHECK_CROSSBOW        = 6,
            EVENT_GROUND_SLAM_CANCEL    = 7,
        };

        enum eTalks
        {
            TALK_INTRO,
            TALK_AGGRO,
            TALK_AXES,
            EMOTE_AXES,
            EMOTE_BLADES,
            TALK_BLADES,
            TALK_CROSSBOW,
            EMOTE_CROSSBOW,
            TALK_DEATH,
            EMOTE_DEATH,
            TALK_SLAY,
            TALK_RESET
        };

        struct boss_xin_the_weaponmaster_AI : public BossAI
        {
            boss_xin_the_weaponmaster_AI(Creature* creature) : BossAI(creature, DATA_XIN_THE_WEAPONMASTER) { }

            bool m_bHasYelled;
            std::list <Creature*> gems;
            ObjectGuid targetGUID;

            void InitializeAI() override
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL_TOWARDS, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL_TOWARDS_DEST, true);
                me->HandleEmoteStateCommand(EMOTE_STATE_SIT_CHAIR_HIGH);
                targetGUID = ObjectGuid::Empty;
            }

            void InitializeGems()
            {
                gems.clear();
                GetCreatureListWithEntryInGrid(gems, me, CREATURE_FAINTLY_GLOWING_GEM, 200.0f);
            }

            void ActivateGem()
            {
                if (gems.empty())
                    return;

                if (Creature* gem = Trinity::Containers::SelectRandomContainerElement(gems))
                {
                    gem->AI()->DoAction(ACTION_ACTIVATE_BUTTON);
                    gems.erase(std::find(gems.begin(), gems.end(), gem));
                }
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
                InitializeGems();
                targetGUID = ObjectGuid::Empty;

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                for (auto&& itr : gems)
                    itr->AI()->DoAction(ACTION_DEACTIVATE_BUTTON);
            }

            void JustReachedHome() override
            {
                me->HandleEmoteStateCommand(EMOTE_STATE_SIT_CHAIR_HIGH);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!m_bHasYelled && who && who->GetTypeId() == TYPEID_PLAYER)
                {
                    m_bHasYelled = true;
                    Talk(TALK_INTRO);
                }

                ScriptedAI::MoveInLineOfSight(who);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                _EnterEvadeMode();

                if (me->HasUnitState(UNIT_STATE_CANNOT_TURN))
                    me->ClearUnitState(UNIT_STATE_CANNOT_TURN);

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->GetMotionMaster()->MoveTargetedHome();

                DeactivateWeapons();
                Talk(TALK_RESET);

                if (auto const script = me->GetInstanceScript())
                    script->HandleGameObject(ObjectGuid::Empty, true, ObjectAccessor::GetGameObject(*me, script->GetGuidData(GO_DOOR_BEFORE_KING)));
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(TALK_SLAY);
            }

            void DeactivateWeapons()
            {
                DeactivateAxes();
                DeactivateStaves();
                DeactivateCrossbows();
                DeactivateSwordLaunchers();
            }

            void DeactivateAxes()
            {
                if (instance)
                    instance->SetData(TYPE_ACTIVATE_ANIMATED_AXE, 0);
            }

            void DeactivateStaves()
            {
                std::list<Creature*> m_lStaveList;

                GetCreatureListWithEntryInGrid(m_lStaveList, me, CREATURE_ANIMATED_STAFF, 200.0f);

                for (auto&& creature : m_lStaveList)
                {
                    creature->CombatStop(true);

                    if (creature->AI())
                        creature->AI()->EnterEvadeMode();
                }
            }


            bool DoActivateSwordLaunchers()
            {
                if (me->GetHealthPct() > 66.5f)
                    return false;

                ActivateGem();

                std::list<Creature*>m_lLaunchersList;

                GetCreatureListWithEntryInGrid(m_lLaunchersList, me, CREATURE_SWORD_LAUNCHER, 200.0f);
                m_lLaunchersList.remove_if ([=](WorldObject* obj) { return obj->GetPositionY() > m_fLessThanYPos && obj->GetPositionY() < m_fMoreThanYPos; });

                if (m_lLaunchersList.empty())
                {
                    TC_LOG_ERROR("scripts", "Xin the Weaponmaster found no world triggers for blade launcher in instance %u", me->GetInstanceId());
                    return true;
                }

                for (auto&& pTrap : m_lLaunchersList)
                {
                    me->AddAura(SPELL_BLADE_TRAP_VISUAL, pTrap);
                    me->AddAura(SPELL_TURN_ON_BLADES, pTrap);
                }

                Talk(EMOTE_BLADES);
                Talk(TALK_BLADES);

                return true;
            }

            void DeactivateSwordLaunchers()
            {
                std::list<Creature*>m_lLaunchersList;

                GetCreatureListWithEntryInGrid(m_lLaunchersList, me, CREATURE_SWORD_LAUNCHER, 200.0f);

                for (auto&& creature : m_lLaunchersList)
                    creature->RemoveAllAuras();
            }

            bool DoActivateCrossbows()
            {
                if (me->GetHealthPct() > 35.5f)
                    return false;

                ActivateGem();

                std::list<Creature*>m_lCrossbowList;

                GetCreatureListWithEntryInGrid(m_lCrossbowList, me, CREATURE_CROSSBOW, 200.0f);
                if (m_lCrossbowList.size() > 3)
                    m_lCrossbowList.resize(3);

                for (auto&& pCrossbow : m_lCrossbowList)
                {
                    pCrossbow->CastSpell(pCrossbow, SPELL_DART_AURA, true);
                    pCrossbow->RemoveAurasDueToSpell(SPELL_PERMANENT_FEIGN_DEATH);
                }

                Talk(EMOTE_CROSSBOW);
                Talk(TALK_CROSSBOW);

                return true;
            }

            void DeactivateCrossbows()
            {
                std::list<Creature*>m_lCrossbowList;

                GetCreatureListWithEntryInGrid(m_lCrossbowList, me, CREATURE_CROSSBOW, 200.0f);

                for (auto&& pCrossbow : m_lCrossbowList)
                {
                    pCrossbow->RemoveAurasDueToSpell(SPELL_DART_AURA);
                    pCrossbow->CastSpell(pCrossbow, SPELL_PERMANENT_FEIGN_DEATH, true);
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_RING_OF_FIRE, 15000);
                events.ScheduleEvent(EVENT_GROUND_SLAM, urand(10000, 15000));
                events.ScheduleEvent(EVENT_INCITING_ROAR, urand(15000, 25000));
                events.ScheduleEvent(EVENT_AXES_ACTIVATE, urand(15000, 25000));
                events.ScheduleEvent(EVENT_CHECK_SWORDS, 1000);
                events.ScheduleEvent(EVENT_CHECK_CROSSBOW, 1000);
                InitializeGems();
                me->HandleEmoteStateCommand(EMOTE_STATE_STAND);

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                Talk(TALK_AGGRO);

                if (auto const script = me->GetInstanceScript())
                    script->HandleGameObject(ObjectGuid::Empty, false, ObjectAccessor::GetGameObject(*me, script->GetGuidData(GO_DOOR_BEFORE_KING)));
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                Talk(TALK_DEATH);
                Talk(EMOTE_DEATH);

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                DeactivateWeapons();

                if (auto const script = me->GetInstanceScript())
                    script->HandleGameObject(ObjectGuid::Empty, true, ObjectAccessor::GetGameObject(*me, script->GetGuidData(GO_DOOR_BEFORE_KING)));
            }

            void DoAction(int32 actionId) override
            {
                DeactivateCrossbows();
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
                        case EVENT_RING_OF_FIRE:
                            if (instance)
                                instance->SetData(TYPE_ACTIVATE_ANIMATED_STAFF, 0);
                            events.ScheduleEvent(EVENT_RING_OF_FIRE, 35000);
                            break;
                        case EVENT_GROUND_SLAM:
                            if (Unit* vict = me->GetVictim())
                            {
                                targetGUID = vict->GetGUID();
                                me->PrepareChanneledCast(me->GetAngle(vict), SPELL_GROUND_SLAM);
                            }

                            events.ScheduleEvent(EVENT_GROUND_SLAM_CANCEL, 3500);
                            events.ScheduleEvent(EVENT_GROUND_SLAM, urand(10000, 15000));
                            break;
                        case EVENT_INCITING_ROAR:
                            DoCast(SPELL_INCITING_ROAR);
                            events.ScheduleEvent(EVENT_INCITING_ROAR, 30000);
                            break;
                        case EVENT_AXES_ACTIVATE:
                            if (instance)
                                instance->SetData(TYPE_ACTIVATE_ANIMATED_AXE, 1);
                            Talk(EMOTE_AXES);
                            Talk(TALK_AXES);
                            break;
                        case EVENT_CHECK_SWORDS:
                            if (!DoActivateSwordLaunchers())
                                events.ScheduleEvent(EVENT_CHECK_SWORDS, 1000);
                            break;
                        case EVENT_CHECK_CROSSBOW:
                            if (!DoActivateCrossbows())
                                events.ScheduleEvent(EVENT_CHECK_CROSSBOW, 1000);
                            break;
                        case EVENT_GROUND_SLAM_CANCEL:
                            me->RemoveChanneledCast(targetGUID);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_xin_the_weaponmaster_AI(creature);
        }
};

class npc_animated_staff : public CreatureScript
{
    public:
        npc_animated_staff() : CreatureScript("npc_animated_staff") { }

        enum eActions
        {
            ACTION_ACTIVATE,
        };

        enum eEvents
        {
            EVENT_SUMMON_RING_OF_FIRE = 1,
            EVENT_FIREBALL = 2,
            EVENT_MOVE = 3,
            EVENT_FIRE_RING_VISUAL = 4
        };

        enum eCreatures
        {
            CREATURE_RING_OF_FIRE = 61499,
        };

        struct npc_animated_staff_AI : public ScriptedAI
        {
            npc_animated_staff_AI(Creature* creature) : ScriptedAI(creature) {}

            EventMap events;

            void InitializeAI() override
            {
                DoCast(SPELL_PERMANENT_FEIGN_DEATH);
                me->SetDisplayId(42195);
                me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, 76364);
                me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                SetCombatMovement(false);
            }

            void MovementInform(uint32 type, uint32 /*pointId*/) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                me->SetVisible(true);
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_ACTIVATE)
                {
                    me->RemoveAurasDueToSpell(SPELL_PERMANENT_FEIGN_DEATH);
                    events.ScheduleEvent(EVENT_SUMMON_RING_OF_FIRE, urand(9000, 14000));
                    events.ScheduleEvent(EVENT_FIREBALL, urand(2000, 3000));
                    events.ScheduleEvent(EVENT_MOVE, 200);
                    me->GetMotionMaster()->MovePoint(1, m_fCenterPos);
                    DoZoneInCombat();
                }
            }

            void Move()
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                {
                    float x, y;
                    // attempt fix crash when frand called by inversed params...
                    me->GetDistance(target) < 3.0f ? GetPositionWithDistInOrientation(me, frand(me->GetDistance(target), 3.0f), me->GetAngle(target), x, y) : GetPositionWithDistInOrientation(me, frand(3.0f, me->GetDistance(target)), me->GetAngle(target), x, y);
                    me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ());
                }

                events.ScheduleEvent(EVENT_MOVE, urand(4000, 7000));
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                _EnterEvadeMode();

                me->SetVisible(false);
                me->SetReactState(REACT_PASSIVE);
                me->CombatStop(true);
                //Add aura here to prevent our function cancelling movement
                me->AddAura(SPELL_PERMANENT_FEIGN_DEATH, me);

                me->GetMotionMaster()->MovePoint(0, me->GetHomePosition());
            }

            void HandleRing()
            {
                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);

                if (!target)
                    return;

                me->GetMotionMaster()->MovePoint(0, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
                me->CastSpell(target, SPELL_SUMMON_FIRE_RING, false);
                events.ScheduleEvent(EVENT_FIRE_RING_VISUAL, 600);
                events.ScheduleEvent(EVENT_SUMMON_RING_OF_FIRE, urand(16000, 22000));
            }

            void HandleFireball()
            {
                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);

                if (target)
                    DoCast(target, SPELL_STAFF_FIREBALL);
                events.ScheduleEvent(EVENT_FIREBALL, urand(4000, 8000));
                events.RescheduleEvent(EVENT_MOVE, urand(1000, 2000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SUMMON_RING_OF_FIRE:
                            HandleRing();
                            break;
                        case EVENT_FIREBALL:
                            HandleFireball();
                            break;
                        case EVENT_MOVE:
                            Move();
                            break;
                        case EVENT_FIRE_RING_VISUAL:
                            DoCast(SPELL_FIRE_RING_VISUAL);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_animated_staff_AI(creature);
        }
};

class npc_fire_ring_trigger : public CreatureScript
{
    public:
        npc_fire_ring_trigger() : CreatureScript("npc_fire_ring_trigger") { }

        enum eEvents
        {
            EVENT_UNSUMMON = 1,
            EVENT_MOVE     = 2,
            EVENT_MOVE_MID = 3
        };

        struct npc_fire_ring_triggerAI : public ScriptedAI
        {
            npc_fire_ring_triggerAI(Creature* creature) : ScriptedAI(creature) {}

            float x, y;
            float m_fArcPoint;
            bool m_bCounterClockwise;
            bool m_bFirstPoint;
            EventMap events;

            void InitializeAI() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                me->GetMotionMaster()->Clear();

                events.ScheduleEvent(EVENT_MOVE, 500);
                x = me->GetPositionX();
                y = me->GetPositionY();
                m_fArcPoint = 0.0f;

                m_bCounterClockwise = urand(0, 1);

                m_bFirstPoint = true;
                me->SetDisplayId(39810);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                {
                    if (m_fArcPoint < 11)
                        events.ScheduleEvent(EVENT_MOVE, 200);
                    else
                    {
                        events.CancelEvent(EVENT_MOVE);
                        events.ScheduleEvent(EVENT_MOVE_MID, 200);
                    }
                }
                else
                {
                    DoCast(SPELL_EXPLODE_FIRE_RING);
                    events.ScheduleEvent(EVENT_UNSUMMON, 2500);
                }
            }

            void Move()
            {
                if (!me->HasAura(SPELL_FIRE_RING_AURA) && !m_bFirstPoint)
                    DoCast(me, SPELL_FIRE_RING_AURA, true);

                float newX = x + 5.0f * cos(m_fArcPoint * M_PI / 5);
                float newY = m_bCounterClockwise ? y - 5.0f * sin(m_fArcPoint * M_PI / 5) : y + 5.0f * sin(m_fArcPoint * M_PI / 5);
                me->GetMotionMaster()->MovePoint(0, newX, newY, me->GetPositionZ());

                ++m_fArcPoint;

                m_bFirstPoint = false;
            }

            void MoveMiddle()
            {
                me->RemoveAllAuras();
                me->GetMotionMaster()->MovePoint(1, x, y, me->GetPositionZ());
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOVE:
                            Move();
                            break;
                        case EVENT_UNSUMMON:
                            me->DespawnOrUnsummon();
                            break;
                        case EVENT_MOVE_MID:
                            MoveMiddle();
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_fire_ring_triggerAI(creature);
        }
};

class npc_animated_axe : public CreatureScript
{
    public:
        npc_animated_axe() : CreatureScript("npc_animated_axe") { }

        enum eActions : int32
        {
            ACTION_START_COMBAT,
            ACTION_STOP_COMBAT
        };

        struct npc_animated_axeAI : public ScriptedAI
        {
            npc_animated_axeAI(Creature* creature) : ScriptedAI(creature) {}

            void InitializeAI() override
            {
                me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, 30316);
                DoCast(SPELL_PERMANENT_FEIGN_DEATH);
                me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());

                me->SetReactState(REACT_PASSIVE);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_START_COMBAT)
                {
                    me->RemoveAurasDueToSpell(SPELL_PERMANENT_FEIGN_DEATH);
                    DoCast(SPELL_WHIRLWIND);

                    // could possibly be written in a better way
                    // will recheck
                    me->GetMotionMaster()->MoveRandom(50.f);
                }
                else if (actionId == ACTION_STOP_COMBAT)
                {
                    DeactivateAxe();
                }
            }

            void MovementInform(uint32 type, uint32 /*pointId*/) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                me->SetVisible(true);
            }

            void DeactivateAxe()
            {
                me->SetVisible(false);
                me->CombatStop(true);
                me->RemoveAurasDueToSpell(SPELL_WHIRLWIND);
                DoCast(SPELL_PERMANENT_FEIGN_DEATH);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(0, me->GetHomePosition());
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_animated_axeAI(creature);
        }
};

// Faintly glowing gem 63808
class npc_faintly_glowing_gem : public CreatureScript
{
    public:
        npc_faintly_glowing_gem() : CreatureScript("npc_faintly_glowing_gem") { }

        enum eSpells
        {
            SPELL_GLOWING_GEM  = 124524,
        };

        enum eEvents
        {
            EVENT_SPECIAL_BEAM = 1,
        };

        struct npc_faintly_glowing_gemAI : public ScriptedAI
        {
            npc_faintly_glowing_gemAI(Creature* creature) : ScriptedAI(creature) {}

            EventMap events;

            void InitializeAI() override
            {
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetDisplayId(35408);

                if (me->HasAura(SPELL_GLOWING_GEM))
                    me->RemoveAura(SPELL_GLOWING_GEM);
            }

            void RemoveBladesAura() // template for achivment with red beam (What does this button do?)
            {
                std::list<Player*> PlayersInArea;
                GetPlayerListInGrid(PlayersInArea, me, 200.0f);

                if (!PlayersInArea.empty())
                    return;

                for (auto&& itr : PlayersInArea)
                    if (itr->HasAura(SPELL_BLADES_1))
                        itr->RemoveAura(SPELL_BLADES_1);
            }

            void DoAction(int32 actionId) override 
            {
                switch (actionId)
                {
                case ACTION_ACTIVATE_BUTTON:
                    if (!me->HasAura(SPELL_GLOWING_GEM))
                        me->AddAura(SPELL_GLOWING_GEM, me);

                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    //me->CastSpell(me, SPELL_BLADES_1, false);
                    //RemoveBladesAura();
                    events.ScheduleEvent(EVENT_SPECIAL_BEAM, 6 * IN_MILLISECONDS);
                    break;
                case ACTION_DEACTIVATE_BUTTON:
                    if (me->HasAura(SPELL_GLOWING_GEM))
                        me->RemoveAura(SPELL_GLOWING_GEM);

                    me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    break;
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override { }

            void OnSpellClick(Unit* clicker, bool& result) override
            {
                me->AI()->DoAction(ACTION_DEACTIVATE_BUTTON);

                if (Creature* Xin = GetClosestCreatureWithEntry(me, CREATURE_XIN_THE_WEAPONMASTER, 200.0f, true))
                    Xin->AI()->DoAction(ACTION_ACTIVATE_BUTTON);
            }

            void UpdateAI(uint32 diff) override 
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                    if (eventId == EVENT_SPECIAL_BEAM)
                        me->RemoveAura(SPELL_BLADES_1);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_faintly_glowing_gemAI(creature);
        }
};

// Quilen Guardian - 61387
class npc_mp_quilen_guardian : public CreatureScript
{
    public:
        npc_mp_quilen_guardian() : CreatureScript("npc_mp_quilen_guardian") { }

        enum eSpells
        {
            SPELL_CARNIVOROUS_BITE = 122962,
            SPELL_LEAPING_RUSH     = 121190,
        };

        enum eEvents
        {
            EVENT_CARNIVOROUS_BITE = 1,
            EVENT_LEAPING_RUSH     = 2,
        };

        struct npc_mp_quilen_guardianAI : public ScriptedAI
        {
            npc_mp_quilen_guardianAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void InitializeAI() override { }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_LEAPING_RUSH, urand(7000, 9000));
                events.ScheduleEvent(EVENT_CARNIVOROUS_BITE, 3000);
                CallQuilensForHelp(who->GetGUID());
            }

            void CallQuilensForHelp(ObjectGuid attacker)
            {
                std::list<Creature*> Quilens;
                GetCreatureListWithEntryInGrid(Quilens, me, CREATURE_QUILEN_GUARDIAN, 50.0f);

                if (Quilens.empty())
                    return;

                for (auto&& itr : Quilens)
                {
                    if (!itr->IsInCombat())
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, attacker))
                        {
                            itr->Attack(target, true);
                            itr->CastSpell(target, SPELL_LEAPING_RUSH, false);
                        }
                    }
                }
            }

            void DoAction(int32 actionId) override { }

            void MovementInform(uint32 type, uint32 pointId) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_LEAPING_RUSH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, LeapingRushPredicate(me)))
                                me->CastSpell(target, SPELL_LEAPING_RUSH, false);

                            events.ScheduleEvent(EVENT_LEAPING_RUSH, urand(7000, 9000));
                            break;
                        case EVENT_CARNIVOROUS_BITE:
                            if (Unit* target = me->GetVictim())
                                me->CastSpell(target, SPELL_CARNIVOROUS_BITE, false);

                            events.ScheduleEvent(EVENT_CARNIVOROUS_BITE, urand(8000, 12000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            struct LeapingRushPredicate 
            {
                public:
                    LeapingRushPredicate(Creature const* me) : _me(me) { }
                    bool operator()(Unit const* target) const
                    {
                        return target && target->GetTypeId() == TYPEID_PLAYER && target->GetDistance2d(_me) > 5.0f;
                    }

                private:
                    Creature const* _me;
            };

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_mp_quilen_guardianAI(creature);
        }
};

class OnlyTriggerInFrontPredicate
{
    public:
        OnlyTriggerInFrontPredicate(Unit* caster) : _caster(caster) { }

        bool operator()(WorldObject* target) const
        {
            return !_caster->isInFront(target, M_PI / 5) || target->GetGUID() == _caster->GetGUID() || target->GetDistance2d(_caster) > 80.f;
        }

    private:
        Unit* _caster;
};

struct CheckIfValidTarget
{
    bool operator ()(WorldObject const* target) const
    {
        return target->GetEntry() != 59481;
    }
};

class InBetweenCheck
{
    public:
        InBetweenCheck(Unit* caster, WorldObject* trigger) : _caster(caster), _trigger(trigger) { }

        bool operator ()(WorldObject const* target) const
        {
            return !target->IsInBetween(_caster, _trigger, 5.0f);
        }

    private:
        Unit const* _caster;
        WorldObject const* _trigger;
};

class spell_dart_damage : public SpellScriptLoader
{
    public:
        spell_dart_damage() : SpellScriptLoader("spell_dart_damage") { }

        class spell_dart_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dart_damage_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                return true;
            }

            void SelectTarget(std::list<WorldObject*>& targetList)
            {
                if (!GetCaster())
                    return;

                if (targetList.empty())
                {
                    FinishCast(SPELL_FAILED_NO_VALID_TARGETS);
                    return;
                }

                std::list<Creature*> targets;
                std::list<WorldObject*> finalTargets;
                GetCreatureListWithEntryInGrid(targets, GetCaster(), CREATURE_SWORD_LAUNCHER, 200.0f);

                targets.remove_if (OnlyTriggerInFrontPredicate(GetCaster()));

                for (auto&& target : targetList)
                {
                    for (auto&& trigger : targets)
                    {
                        if (target->IsInBetween(GetCaster(), trigger, 2.0f))
                            finalTargets.push_back(target);
                    }
                }

                targetList = finalTargets;
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dart_damage_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dart_damage_SpellScript();
        }
};

class spell_dart : public SpellScriptLoader
{
    public:
        spell_dart() : SpellScriptLoader("spell_dart") { }

        class spell_dart_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dart_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                return true;
            }

            // set up initial variables and check if caster is creature
            // this will let use safely use ToCreature() casts in entire script
            bool Load() override
            {
                return true;
            }

            void SelectTarget(std::list<WorldObject*>& targetList)
            {
                if (!GetCaster())
                    return;

                if (targetList.empty())
                {
                    FinishCast(SPELL_FAILED_NO_VALID_TARGETS);
                    return;
                }

                //Select the two targets.
                std::list<WorldObject*> targets = targetList;
                targetList.remove_if (OnlyTriggerInFrontPredicate(GetCaster()));
                targetList.remove_if (CheckIfValidTarget());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dart_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dart_SpellScript();
        }
};

class TargetCheck
{
    public:
        TargetCheck(Unit* caster) : _caster(caster) { }

        bool operator()(WorldObject* target) const
        {
            return !target->ToCreature() || target->GetEntry() != 59481 || target->GetGUID() == _caster->GetGUID();
        }
    private:
        Unit* _caster;
};

class ActiveTrapCheck
{
    public:
        ActiveTrapCheck(uint32 aura) : _aura(aura) { }

        bool operator()(WorldObject* target) const
        {
            return target->ToCreature() && target->ToCreature()->HasAura(_aura);
        }

    private:
        uint32 _aura;
};

class spell_activate_blades : public SpellScriptLoader
{
    public:
        spell_activate_blades() : SpellScriptLoader("spell_activate_blades") { }

        class spell_activate_blades_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_activate_blades_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BLADES_1) || !sSpellMgr->GetSpellInfo(SPELL_BLADES_2))
                    return false;
                return true;
            }

            void SelectTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if (TargetCheck(GetCaster()));
                targets.remove_if (ActiveTrapCheck(SPELL_BLADES_1));
                targets.remove_if (ActiveTrapCheck(SPELL_BLADES_2));

                if (targets.size() > 1)
                    targets.resize(1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_activate_blades_SpellScript::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_activate_blades_SpellScript();
        }
};

class spell_turn_off_blades : public SpellScriptLoader
{
    public:
        spell_turn_off_blades() : SpellScriptLoader("spell_turn_off_blades") { }

        class spell_turn_off_blades_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_turn_off_blades_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TURN_OFF_BLADES))
                    return false;
                return true;
            }

            void HandlePreCast()
            {
                if (GetCaster())
                {
                    GetCaster()->CastSpell(GetCaster(), SPELL_BLADES_1, false);
                    GetCaster()->CastSpell(GetCaster(), SPELL_BLADES_2, false);
                }
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_turn_off_blades_SpellScript::HandlePreCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_turn_off_blades_SpellScript();
        }
};

class spell_groundslam_xin : public SpellScriptLoader
{
    public:
        spell_groundslam_xin() : SpellScriptLoader("spell_groundslam_xin") { }

        class spell_groundslam_xin_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_groundslam_xin_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_GROUND_SLAM))
                    return false;
                return true;
            }

            void HandlePrecast()
            {
                if (GetCaster())
                    GetCaster()->AddUnitState(UNIT_STATE_CANNOT_TURN);
            }

            void HandleAftercast()
            {
                if (GetCaster())
                    GetCaster()->ClearUnitState(UNIT_STATE_CANNOT_TURN);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_groundslam_xin_SpellScript::HandlePrecast);
                AfterCast += SpellCastFn(spell_groundslam_xin_SpellScript::HandleAftercast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_groundslam_xin_SpellScript;
        }
};

class spell_crossbow_xin : public SpellScriptLoader
{
    public:
        spell_crossbow_xin() : SpellScriptLoader("spell_crossbow_xin") { }

        class spell_crossbow_xin_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_crossbow_xin_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DART_DAMAGE))
                    return false;
                return true;
            }

            void SelectTarget(std::list<WorldObject*>& targets)
            {
                if (targets.empty())
                    return;

                std::list<WorldObject*> finalTarget;

                finalTarget.push_back(Trinity::Containers::SelectRandomContainerElement(targets));

                if (!finalTarget.empty())
                    targets = finalTarget;
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_crossbow_xin_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_crossbow_xin_SpellScript();
        }
};

void AddSC_boss_xin_the_weaponmaster()
{
    new boss_xin_the_weaponmaster();
    new npc_animated_axe();
    new npc_animated_staff();
    new npc_fire_ring_trigger();
    new npc_faintly_glowing_gem();
    new npc_mp_quilen_guardian();
    new spell_dart();
    new spell_dart_damage();
    new spell_activate_blades();
    new spell_turn_off_blades();
    new spell_groundslam_xin();
    new spell_crossbow_xin();
}
