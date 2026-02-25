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
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "CreatureAI.h"
#include "MoveSplineInit.h"
#include "SpellScript.h"
#include "Config.h"

enum eSpells
{
    SPELL_SPIRIT_BOLT                          = 121224,
    SPELL_GROUND_SLAM                          = 121087,
    SPELL_PETRIFICATION                        = 125090,
    SPELL_PETRIFIED                            = 125092,
    SPELL_FULLY_PETRIFIED                      = 115877,
    SPELL_MONSTROUS_BITE                       = 125096,
    SPELL_SUNDERING_BITE                       = 116970,
    SPELL_PROTECTIVE_FENZY                     = 116982,
    SPELL_SHATTERING_STONE                     = 116977,
    SPELL_FORTIFY_STONE_AURA                   = 116980,
    SPELL_FOCUSED_ASSAULT                      = 116990,
    SPELL_POSE_1                               = 120650,
    SPELL_POSE_2                               = 120613,
    SPELL_IMPALE                               = 121247,
    SPELL_CURSE_OF_VITALITY                    = 121245,
    SPELL_GHOST_ESSENCE                        = 120764,
    SPELL_INACTIVE                             = 118205,
    SPELL_ACTIVATION_VISUAL                    = 118212,
    SPELL_CHARGED_SHADOWS                      = 117685,
    SPELL_SHADOW_BLAST                         = 119365,
    SPELL_ANNIHILATE                           = 119521,
    SPELL_COWARDICE                            = 119635,
    SPELL_VOLLEY                               = 119554,
    SPELL_TROLL_RUSH                           = 116606,
    SPELL_TROLL_RUSH_APPLY_AURA                = 116607,
    SPELL_CRUSHING_ATTACKS                     = 119514,
    SPELL_SHOOT                                = 119524,
    SPELL_ENRAGE                               = 119629,
    SPELL_REFLECTIVE_SHIELDS                   = 119630,
    SPELL_FLESH_TO_STONE                       = 118552,
    SPELL_STONE_BLOCK                          = 118529,
    SPELL_FORCEFUL_SWING                       = 126955,
    SPELL_TOUCH_OF_NALAK_SHA                   = 126958,
    SPELL_WARDEN_S_FURY                        = 126970,
    SPELL_ENHANCED_RECONSTRUCTION              = 126980,
    SPELL_RECONSTRUCTING                       = 126985,
    SPELL_SPIRIT_KINGS_EVENT_INACTIVE          = 118205,
    SPELL_SPIRIT_KINGS_EVENT_ACTIVATION_VISUAL = 118212,
    SPELL_RAW_ENERGY                           = 126973,
    SPELL_STORY_TIME                           = 131378,
    SPELL_KG_ENRAGE                            = 119629,
    SPELL_MA_SHOT                              = 119524,
    SPELL_MM_CRUSHING_ATTACK                   = 119513,

    // Achievement
    SPELL_COALESCING_ENERGIZE                  = 135250,
};

enum eEvents
{
    EVENT_CURSED_MOGU_SPIRIT_BOLT              = 1,
    EVENT_CURSED_MOGU_GROUND_SLAM              = 2,
    EVENT_ENORMOUS_QUILEN_BITE                 = 3,
    EVENT_QUILEN_SUNDERING_BITE                = 4,
    EVENT_QUILEN_SHATTERING_STONE              = 5,
    EVENT_QUILEN_FOCUSED_ASSAULT               = 6,
    EVENT_QUILEN_PROTECTIVE_FRENZY             = 7,
    EVENT_ZANDALARI_TROLL_RUSH                 = 8,
    EVENT_ZIAN_CHARGED_SHADOWS                 = 9,
    EVENT_SORCERER_SHADOW_BLAST                = 10,
    EVENT_QIANG_ANNIHILATE                     = 11,
    EVENT_QIANG_START_SECOND_FIGHT             = 12,
    EVENT_MOUNTED_MOGU_CRUSHING_ATTACKS        = 13,
    EVENT_SUBETAI_VOLLEY                       = 14,
    EVENT_SUBETAI_START_THIRD_COMBAT           = 15,
    EVENT_MOGU_ARCHER_SHOOT                    = 16,
    EVENT_MENG_START_FOURTH_COMBAT             = 17,
    EVENT_MENG_COWARDICE                       = 18,
    EVENT_KINGS_GUARD_ENRAGE                   = 19,
    EVENT_KINGS_GUARD_REFLECTIVE_SHIELDS       = 20,
    EVENT_SECRET_FLESH_TO_STONE                = 21,
    EVENT_SECRET_STONE_BLOCK                   = 22,
    EVENT_WARDEN_FORCEFUL_SWING                = 23,
    EVENT_WARDEN_TOUCH_OF_NALAK_SHA            = 24,
    EVENT_WARDEN_WARDEN_S_FURY                 = 25,
    EVENT_KEEPER_ENHANCED_RECONSTRUCTION       = 26,
    EVENT_ARCANIST_RAW_ENERGY                  = 27,
    EVENT_IMPALE                               = 28,
    EVENT_CURSE_OF_VITALITY                    = 29,

    // Lorewalker Cho
    EVENT_ESCORT                               = 1,
    EVENT_INTRO                                = 2,
    EVENT_DOOR_OPENED                          = 3,
    EVENT_CHECK_CURSED_MOGU_1                  = 4,
    EVENT_CHECK_CURSED_MOGU_2                  = 5,
    EVENT_BEHIND_GARAJAL                       = 6,

    // Spirit Kings Event
    EVENT_ACTIVATED                            = 1,
    EVENT_SPIRIT_MOGU_COMBAT                   = 2,
};

enum eTrashsActions
{
    ACTION_SPIRIT_KINGS_EVENT                  = 1,
    ACTION_SPIRIT_KINGS_EVENT_ACTIVE           = 2,
    ACTION_SPIRIT_KINGS_NEXT                   = 3,
};

enum eTalk
{
    LOREWALKER_CHO_INTRO_1                     = 0,
    LOREWALKER_CHO_INTRO_2                     = 1,
    LOREWALKER_CHO_DOGS                        = 2,
    LOREWALKER_CHO_DOGS_DIE                    = 3,
    LOREWALKER_CHO_DOGS_DOOR_EVENT             = 4,
    LOREWALKER_CHO_FENG_1                      = 5,
    LOREWALKER_CHO_FENG_2                      = 6,
    LOREWALKER_CHO_FENG_3                      = 7,
    LOREWALKER_CHO_FENG_PAST_1                 = 8,
    LOREWALKER_CHO_FENG_PAST_2                 = 9,
    LOREWALKER_CHO_GARAJAL_1                   = 10,
    LOREWALKER_CHO_GARAJAL_2                   = 11,
    LOREWALKER_CHO_GARAJAL_3                   = 12,
    LOREWALKER_CHO_SPIRIT_KINGS_EVENT          = 13,
    LOREWALKER_CHO_SPIRIT_KINGS_EVENT_ZIAN     = 14,
    LOREWALKER_CHO_SPIRIT_KINGS_EVENT_QIANG    = 15,
    LOREWALKER_CHO_SPIRIT_KINGS_EVENT_SUBETAI  = 16,
    LOREWALKER_CHO_SPIRIT_KINGS_EVENT_MENG     = 17,
};

class npc_cursed_mogu_sculpture : public CreatureScript
{
    public:
        npc_cursed_mogu_sculpture() : CreatureScript("npc_cursed_mogu_sculture") { }

        struct npc_cursed_mogu_sculptureAI : public ScriptedAI
        {
            npc_cursed_mogu_sculptureAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            EventMap events;
            bool activationDone;
            ObjectGuid playerActivate;

            void InitializeAI() override
            {
                switch (me->GetEntry())
                {
                    case NPC_CURSED_MOGU_SCULPTURE_1:
                        me->AddAura(SPELL_POSE_1, me);
                        me->AddAura(SPELL_STONE, me);
                        break;
                    case NPC_CURSED_MOGU_SCULPTURE_2:
                        me->AddAura(SPELL_POSE_2, me);
                        me->AddAura(SPELL_BRONZE, me);
                        break;
                }

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
                playerActivate = ObjectGuid::Empty;
                activationDone = false;
            }

            void Reset() override
            {
                events.Reset();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
                playerActivate = ObjectGuid::Empty;
                activationDone = false;
            }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_CURSED_MOGU_SPIRIT_BOLT, urand(1.5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_IMPALE, urand(2000, 3000));

                switch (me->GetEntry())
                {
                    case NPC_CURSED_MOGU_SCULPTURE_1:
                        events.ScheduleEvent(EVENT_CURSE_OF_VITALITY, 13 * IN_MILLISECONDS);
                        break;
                    case NPC_CURSED_MOGU_SCULPTURE_2:
                        events.ScheduleEvent(EVENT_CURSED_MOGU_GROUND_SLAM, 9 * IN_MILLISECONDS);
                        break;
                }
                me->AI()->AttackStart(who);
            }

            void MoveInLineOfSight(Unit* who) override { }

            void JustDied(Unit* /*killer*/) override
            {
                if (!GetClosestCreatureWithEntry(me, me->GetEntry(), 50.0f, true))
                {
                    Creature* Feng = GetClosestCreatureWithEntry(me, NPC_FENG, 100.0f, true);
                    if (Feng && Feng->AI())
                        Feng->AI()->Talk(7);
                }
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_CURSED_MOGU_ATTACK_PLAYER)
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    me->RemoveAurasDueToSpell(SPELL_BRONZE);
                    me->RemoveAurasDueToSpell(SPELL_POSE_2);
                    me->RemoveAurasDueToSpell(SPELL_POSE_1);
                    me->RemoveAurasDueToSpell(SPELL_STONE);

                    if (Player* plr = ObjectAccessor::GetPlayer(*me, playerActivate))
                        me->AI()->AttackStart(plr);
                    else if (Player* plr = me->FindNearestPlayer(15.0f))
                        me->AI()->AttackStart(plr);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!activationDone)
                {
                    if (Player* itr = me->FindNearestPlayer(15.0f))
                    {
                        if (Creature* ghostEssence = instance->instance->GetCreature(instance->GetGuidData(NPC_GHOST_ESSENCE)))
                        {
                            if (ghostEssence->AI()->GetData(TYPE_GHOST_ESSENCE_STATE))
                            {
                                ghostEssence->CastSpell(ghostEssence, SPELL_GHOST_ESSENCE, false);

                                playerActivate = itr->GetGUID();
                                activationDone = true;
                            }
                        }
                    }
                }

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CURSED_MOGU_SPIRIT_BOLT:
                            DoCast(me, SPELL_SPIRIT_BOLT);
                            events.ScheduleEvent(EVENT_CURSED_MOGU_SPIRIT_BOLT, urand(8 * IN_MILLISECONDS, 23 * IN_MILLISECONDS));
                            break;
                        case EVENT_CURSED_MOGU_GROUND_SLAM:
                            if (Unit* target = me->GetVictim())
                                me->CastSpell(target, SPELL_GROUND_SLAM, false);

                            events.ScheduleEvent(EVENT_CURSED_MOGU_GROUND_SLAM, 9 * IN_MILLISECONDS);
                            break;
                        case EVENT_IMPALE:
                            if (Unit* target = me->GetVictim())
                                me->CastSpell(target, SPELL_IMPALE, false);

                            events.ScheduleEvent(EVENT_IMPALE, urand(6000, 7000));
                            break;
                        case EVENT_CURSE_OF_VITALITY:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CurseOfVitalityPredicate(me)))
                                me->CastSpell(target, SPELL_CURSE_OF_VITALITY, false);

                            events.ScheduleEvent(EVENT_CURSE_OF_VITALITY, 9 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                struct CurseOfVitalityPredicate 
                {
                    public:
                        CurseOfVitalityPredicate(Creature const* me) : _me(me) { }
                        bool operator()(Unit const* target) const
                        {
                            return target && target->GetTypeId() == TYPEID_PLAYER && !target->HasAura(SPELL_CURSE_OF_VITALITY);
                        }

                    private:
                        Creature const* _me;
                };

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cursed_mogu_sculptureAI(creature);
        }
};

class GhostEssenceTargetSelector
{
    public:
        GhostEssenceTargetSelector() { }

        bool operator()(WorldObject* object)
        {
            if (Creature* cre = object->ToCreature())
                if (cre->GetEntry() == NPC_CURSED_MOGU_SCULPTURE_2 || cre->GetEntry() == NPC_CURSED_MOGU_SCULPTURE_1)
                    return false;

                return true;
        }
};

// Ghost Essence - 120764
class spell_ghost_essence : public SpellScriptLoader
{
    public:
        spell_ghost_essence() : SpellScriptLoader("spell_ghost_essence") { }

        class spell_ghost_essence_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ghost_essence_SpellScript);

            void HandleOnHit()
            {
                if (Creature* target = GetHitCreature())
                    target->AI()->DoAction(ACTION_CURSED_MOGU_ATTACK_PLAYER);
            }

            void FilterTargets(std::list<WorldObject*>& targetList)
            {
                targetList.remove_if (GhostEssenceTargetSelector());
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_ghost_essence_SpellScript::HandleOnHit);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ghost_essence_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ghost_essence_SpellScript();
        }
};

float quilenNewY[2] = { 1170.0f, 1240.0f };

class npc_enormous_stone_quilen : public CreatureScript
{
    public:
        npc_enormous_stone_quilen() : CreatureScript("npc_enormous_stone_quilen") { }

        struct npc_enormous_stone_quilenAI : public ScriptedAI
        {
            npc_enormous_stone_quilenAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                prevPosition = 1;

                if (me->GetPositionX() > 3900)
                    prevPosition = 2;

                nextMovementTimer = 0;
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(prevPosition, me->GetPositionX(), quilenNewY[prevPosition - 1], me->GetPositionZ());
            }

            InstanceScript* instance;
            EventMap events;
            uint32 nextMovementTimer;
            uint8 prevPosition;

            void Reset() override
            {
                events.Reset();
            }

            void JustReachedHome() override
            {
                prevPosition = 1;

                if (me->GetPositionX() > 3900)
                    prevPosition = 2;

                nextMovementTimer = 0;
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(prevPosition, me->GetPositionX(), quilenNewY[prevPosition - 1], me->GetPositionZ());
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (me->IsInCombat())
                    return;

                prevPosition = pointId;
                nextMovementTimer = 500;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->SetWalk(false);
                events.ScheduleEvent(EVENT_ENORMOUS_QUILEN_BITE, urand(3000, 5000));

                DoCast(me, SPELL_PETRIFICATION);
            }

            void UpdateAI(uint32 diff) override
            {
                if (nextMovementTimer)
                {
                    if (nextMovementTimer <= diff)
                    {
                        me->GetMotionMaster()->MovePoint(prevPosition == 2 ? 1 : 2, me->GetPositionX(), quilenNewY[prevPosition == 2 ? 0 : 1], me->GetPositionZ());
                        nextMovementTimer = 0;
                    }
                    else
                        nextMovementTimer -= diff;
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_ENORMOUS_QUILEN_BITE)
                    {
                        if (Unit* target = me->GetVictim())
                            me->CastSpell(target, SPELL_MONSTROUS_BITE, true);

                        events.ScheduleEvent(EVENT_ENORMOUS_QUILEN_BITE, urand(6000, 8000));
                    }
                    break;
                }

                if (Unit* vict = me->GetVictim())
                    if (vict->HasAura(SPELL_FULLY_PETRIFIED))
                        me->Kill(vict);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_enormous_stone_quilenAI(creature);
        }
};

// Ghost Essence 62004
class npc_mogu_ghost_essence : public CreatureScript
{
    public:
        npc_mogu_ghost_essence() : CreatureScript("npc_mogu_ghost_essence") { }

        struct npc_mogu_ghost_essenceAI : public ScriptedAI
        {
            npc_mogu_ghost_essenceAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            EventMap events;
            bool aliveMoguAround, lastMoguAround;

            void InitializeAI() override
            {
                aliveMoguAround = false;
                lastMoguAround = false;
                instance = me->GetInstanceScript();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case 0:
                        lastMoguAround = true;
                        break;
                    case 1:
                        if (Unit* Feng = ObjectAccessor::GetUnit(*me, instance->GetGuidData(NPC_FENG)))
                            Feng->ToCreature()->AI()->DoAction(ACTION_CURSED_MOGU_ATTACK_PLAYER);

                        instance->SetData(DATA_FENG, SPECIAL);
                        me->DespawnOrUnsummon(3000);
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_GHOST_ESSENCE_STATE)
                    if (!aliveMoguAround || lastMoguAround)
                        return 1;

                return 0;
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (!GetClosestCreatureWithEntry(me, NPC_CURSED_MOGU_SCULPTURE_2, 150.0f, true) && !aliveMoguAround)
                {
                    aliveMoguAround = true;
                    me->GetMotionMaster()->MovePoint(0, GhostEssenceMovePos);
                }

                if (!GetClosestCreatureWithEntry(me, NPC_CURSED_MOGU_SCULPTURE_1, 150.0f, true) && lastMoguAround)
                {
                    lastMoguAround = false;
                    me->GetMotionMaster()->MovePoint(1, GhostEssenceFeng);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_mogu_ghost_essenceAI(creature);
        }
};

class npc_stone_quilen : public CreatureScript
{
    public:
        npc_stone_quilen() : CreatureScript("npc_stone_quilen") { }

        enum eActions
        {
            ACTION_KILLED = 1
        };

        struct npc_stone_quilenAI : public ScriptedAI
        {
            npc_stone_quilenAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            EventMap events;
            bool Frenzy;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                Frenzy = false;
            }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->CallForHelp(9.6f);
                events.ScheduleEvent(EVENT_QUILEN_SUNDERING_BITE, urand(5000, 6000));
                events.ScheduleEvent(EVENT_QUILEN_FOCUSED_ASSAULT, urand(500, 5000));

                if (Frenzy)
                    events.ScheduleEvent(EVENT_QUILEN_PROTECTIVE_FRENZY, 10 * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override
            {
                Frenzy = true;
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
                        case EVENT_QUILEN_SUNDERING_BITE:
                            if (Unit* target = me->GetVictim())
                                me->CastSpell(target, SPELL_SUNDERING_BITE, true);
                            events.ScheduleEvent(EVENT_QUILEN_SUNDERING_BITE, urand(5000, 6000));
                            break;
                        case EVENT_QUILEN_SHATTERING_STONE:
                            DoCast(me, SPELL_SHATTERING_STONE, true);
                            events.ScheduleEvent(EVENT_QUILEN_SHATTERING_STONE, urand(10000, 12000));
                            break;
                        case EVENT_QUILEN_FOCUSED_ASSAULT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                            {
                                me->AddAura(SPELL_FOCUSED_ASSAULT, target);
                                me->FixateOnTarget(target->GetGUID(), 3 * IN_MILLISECONDS);
                            }
                            events.ScheduleEvent(EVENT_QUILEN_FOCUSED_ASSAULT, urand(4000, 15000));
                            break;
                        case EVENT_QUILEN_PROTECTIVE_FRENZY:
                            me->CastSpell(me, SPELL_PROTECTIVE_FENZY, true);
                            me->CastSpell(me, SPELL_FORTIFY_STONE_AURA, true);

                            events.ScheduleEvent(EVENT_QUILEN_SHATTERING_STONE, urand(4000, 4500));
                            break;
                    }
                }

                if (Unit* vict = me->GetVictim())
                    if (vict->HasAura(SPELL_FULLY_PETRIFIED))
                        me->Kill(vict);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_stone_quilenAI(creature);
        }
};

const Position skullChargerPath_1[3]
{
    { 4183.11f, 1343.65f, 461.959f, 0.0f },
    { 4128.99f, 1343.57f, 461.886f, 0.0f },
    { 4124.77f, 1347.34f, 461.905f, 0.0f },
};

const Position skullChargerPath_2[3]
{
    { 4183.21f, 1339.52f, 461.961f, 0.0f},
    { 4128.95f,  1339.6f, 461.885f, 0.0f },
    { 4124.71f,  1335.8f, 461.905f, 0.0f },
};

// Pls fix Movement...
const std::map<uint64, uint32> skullchargersMatchPath =
{
    { 552288, 0 },
    { 552289, 1 },
};

// Zandalari Skullcharger 60375
class npc_zandalari_skullcharger : public CreatureScript
{
    public:
        npc_zandalari_skullcharger() : CreatureScript("npc_zandalari_skullcharger") { }

        struct npc_zandalari_skullchargerAI : public ScriptedAI
        {
            npc_zandalari_skullchargerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_ZANDALARI_TROLL_RUSH, urand(5000, 6000));
            }

            void DoAction(int32 actionId) override
            {
                for (auto&& it : skullchargersMatchPath)
                {
                    if (it.first == me->GetDBTableGUIDLow())
                    {
                        Movement::MoveSplineInit init(me);
                        for (auto itr : it.second ? skullChargerPath_2 : skullChargerPath_1)
                            init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                        init.SetUncompressed();
                        init.Launch();

                        uint32 delay = 0;
                        me->m_Events.Schedule(delay += me->GetSplineDuration(), 2, [this]()
                        {
                            me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                        });
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_ZANDALARI_TROLL_RUSH)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        {
                            me->CastSpell(target, SPELL_TROLL_RUSH, true);
                            me->ClearUnitState(UNIT_STATE_CASTING);
                            me->FixateOnTarget(target->GetGUID(), 10 * IN_MILLISECONDS);
                        }
                        events.ScheduleEvent(EVENT_ZANDALARI_TROLL_RUSH, urand(15000, 18000));
                    }
                    break;
                }

                if (!me->HasAura(SPELL_TROLL_RUSH_APPLY_AURA))
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zandalari_skullchargerAI(creature);
        }
};

// Zandalari Pterror wing 60384
class npc_zandalari_pterror_wing : public CreatureScript
{
    public:
        npc_zandalari_pterror_wing() : CreatureScript("npc_zandalari_pterror_wing") { }

        struct npc_zandalari_pterror_wingAI : public ScriptedAI
        {
            npc_zandalari_pterror_wingAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            EventMap events;
            ObjectGuid riderGUID;
            bool intro;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->SetSpeed(MOVE_FLIGHT, 1.8f);
                me->SetReactState(REACT_PASSIVE);
                intro = false;

                if (Creature* TerrorRider = me->SummonCreature(NPC_ZANDALARI_TERROR_RIDER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                    riderGUID = TerrorRider->GetGUID();
            }

            void Reset() override
            {
                events.Reset();
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Unit* Rider = ObjectAccessor::GetUnit(*me, riderGUID))
                    Rider->ToCreature()->AI()->DoAction(1);
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void DoAction(int32 /*actionId*/) override { }

            void MovementInform(uint32 /*type*/, uint32 /*pointId*/) override { }

            void UpdateAI(uint32 /*diff*/) override 
            {
                if (!intro && me->FindNearestPlayer(80.0f))
                {
                    intro = true;
                    if (Unit* Rider = ObjectAccessor::GetUnit(*me, riderGUID))
                        Rider->ToCreature()->AI()->DoAction(2);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zandalari_pterror_wingAI(creature);
        }
};

// Zandalari Terror Rider 60386
class npc_zandalari_terror_rider : public CreatureScript
{
    public:
        npc_zandalari_terror_rider() : CreatureScript("npc_zandalari_terror_rider") { }

        enum eSpells
        {
            SPELL_DEVASTATE        = 78660,
            SPELL_SUNDER_ARMOR     = 76622,
            SPELL_SHIELD_SLAM      = 49863,
            SPELL_TROLL_EXPLOSIVES = 116545,
            SPELL_VEHICLE_RIDE     = 46598,
        };

        enum eEvents
        {
            EVENT_DEVASTATE        = 1,
            EVENT_SUNDER_ARMOR     = 2,
            EVENT_SHIELD_SLAM      = 3,
            EVENT_EXPLOSIVE_R      = 4,
            EVENT_VEHICLE          = 5,
        };

        struct npc_zandalari_terror_riderAI : public ScriptedAI
        {
            npc_zandalari_terror_riderAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            EventMap events, terrorwingEvents;
            bool InTerrorWing;
            ObjectGuid TerrorWingGUID;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                me->SetReactState(REACT_PASSIVE);
                InTerrorWing = true;
            }

            void Reset() override
            {
                events.Reset();
            }

            void IsSummonedBy(Unit* summoner) override
            {
                TerrorWingGUID = summoner->GetGUID();
                terrorwingEvents.ScheduleEvent(EVENT_VEHICLE, 1000);
            }

            void JustDied(Unit* /*killer*/) override 
            {
                me->DespawnOrUnsummon(5 * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override 
            {
                switch (actionId)
                {
                    case 1:
                    {
                        if (me->IsOnVehicle())
                            me->ExitVehicle();

                        me->GetMotionMaster()->MoveFall();
                        me->SetReactState(REACT_AGGRESSIVE);
                        InTerrorWing = false;
                        terrorwingEvents.Reset();

                        events.ScheduleEvent(EVENT_DEVASTATE, 7 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_SUNDER_ARMOR, 8 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_SHIELD_SLAM, urand(4000, 11000));

                        if (Player* itr = me->FindNearestPlayer(45.0f))
                            me->AI()->AttackStart(itr);
                        break;
                    }
                    case 2:
                    {
                        terrorwingEvents.ScheduleEvent(EVENT_EXPLOSIVE_R, urand(1500, 4500));
                        break;
                    }
                }
            }

            void MovementInform(uint32 /*type*/, uint32 /*pointId*/) override { }

            void VictimEvents(uint32 spellId, uint32 eventId, uint32 timer)
            {
                if (Unit* vict = me->GetVictim())
                    me->CastSpell(vict, spellId, false);

                events.ScheduleEvent(eventId, timer);
            }

            void UpdateAI(uint32 diff) override 
            {
                terrorwingEvents.Update(diff);

                while (uint32 eventId = terrorwingEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_EXPLOSIVE_R:
                            if (TempSummon* expl = me->SummonCreature(NPC_TROLL_EXPLOSIVES, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                expl->GetMotionMaster()->MoveFall();
                                me->AddAura(SPELL_TROLL_EXPLOSIVES, expl);
                            }

                            terrorwingEvents.ScheduleEvent(EVENT_EXPLOSIVE_R, urand(5000, 9500));
                            break;
                        case EVENT_VEHICLE:
                            if (Unit* TerrowWing = ObjectAccessor::GetUnit(*me, TerrorWingGUID))
                                me->CastSpell(TerrowWing, SPELL_VEHICLE_RIDE, true);
                            break;
                    }
                }

                if (!UpdateVictim() || InTerrorWing)
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DEVASTATE:
                            VictimEvents(SPELL_DEVASTATE, EVENT_DEVASTATE, 7 * IN_MILLISECONDS);
                            break;
                        case EVENT_SUNDER_ARMOR:
                            VictimEvents(SPELL_SUNDER_ARMOR, EVENT_SUNDER_ARMOR, urand(8000, 16000));
                            break;
                        case EVENT_SHIELD_SLAM:
                            VictimEvents(SPELL_SHIELD_SLAM, EVENT_SHIELD_SLAM, 6 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zandalari_terror_riderAI(creature);
        }
};

class spell_mogu_petrification : public SpellScriptLoader
{
    public:
        spell_mogu_petrification() : SpellScriptLoader("spell_mogu_petrification") { }

        class spell_mogu_petrification_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mogu_petrification_AuraScript);

            uint32 stack;

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                {
                    if (Unit* target = GetTarget())
                    {
                        if (target->HasAura(SPELL_PETRIFIED))
                        {
                            stack = GetTarget()->GetAura(SPELL_PETRIFIED)->GetStackAmount();

                            if (stack >= 100 && !target->HasAura(SPELL_FULLY_PETRIFIED))
                                target->AddAura(SPELL_FULLY_PETRIFIED, target);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_mogu_petrification_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAPPLY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mogu_petrification_AuraScript();
        }
};

// Petrification 125090
class spell_mogu_petrification_periodic_aura : public SpellScriptLoader
{
    public:
        spell_mogu_petrification_periodic_aura() : SpellScriptLoader("spell_mogu_petrification_periodic_aura") { }

        class spell_mogu_petrification_periodic_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mogu_petrification_periodic_aura_AuraScript);

            void PeriodicTick(AuraEffect const* aurEff)
            {
                PreventDefaultAction();

                Unit* owner = GetOwner()->ToUnit();

                if (!owner)
                    return;

                std::list<Player*> PetrificatedPlayers;
                GetPlayerListInGrid(PetrificatedPlayers, owner, 200.0f);

                for (auto&& itr : PetrificatedPlayers)
                {
                    if (Aura* affect = itr->GetAura(SPELL_PETRIFIED))
                    {
                        affect->SetStackAmount(affect->GetStackAmount() + 5);
                        affect->RefreshDuration();
                    }
                    else
                        if (Aura* affect = owner->AddAura(SPELL_PETRIFIED, itr))
                            affect->SetStackAmount(5);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mogu_petrification_periodic_aura_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mogu_petrification_periodic_aura_AuraScript();
        }
};

class npc_mogu_secret_keeper : public CreatureScript
{
    public:
        npc_mogu_secret_keeper() : CreatureScript("npc_mogu_secret_keeper") { }

        struct npc_mogu_secret_keeperAI : public ScriptedAI
        {
            npc_mogu_secret_keeperAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SECRET_FLESH_TO_STONE, urand(5000, 12000));
                events.ScheduleEvent(EVENT_SECRET_STONE_BLOCK, urand(15000, 20000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SECRET_FLESH_TO_STONE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_FLESH_TO_STONE, false);
                            events.ScheduleEvent(EVENT_SECRET_FLESH_TO_STONE, 8 * IN_MILLISECONDS);
                            break;
                        case EVENT_SECRET_STONE_BLOCK:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_STONE_BLOCK, false);
                            events.ScheduleEvent(EVENT_SECRET_STONE_BLOCK, urand(15000, 20000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_mogu_secret_keeperAI(creature);
        }
};

class npc_mogu_warden : public CreatureScript
{
    public:
        npc_mogu_warden() : CreatureScript("npc_mogu_warden") { }

        struct npc_mogu_wardenAI : public ScriptedAI
        {
            npc_mogu_wardenAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_WARDEN_FORCEFUL_SWING, urand(5000, 12000));
                events.ScheduleEvent(EVENT_WARDEN_TOUCH_OF_NALAK_SHA, urand(15000, 20000));
                events.ScheduleEvent(EVENT_WARDEN_WARDEN_S_FURY, urand(22000, 28000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_WARDEN_FORCEFUL_SWING:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_FORCEFUL_SWING, false);
                            events.ScheduleEvent(EVENT_WARDEN_FORCEFUL_SWING, 30000);
                            break;
                        case EVENT_WARDEN_TOUCH_OF_NALAK_SHA:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_TOUCH_OF_NALAK_SHA, false);
                            events.ScheduleEvent(EVENT_WARDEN_TOUCH_OF_NALAK_SHA, 30000);
                            break;
                        case EVENT_WARDEN_WARDEN_S_FURY:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_WARDEN_S_FURY, false);
                            events.ScheduleEvent(EVENT_WARDEN_WARDEN_S_FURY, 30000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_mogu_wardenAI(creature);
        }
};

class npc_mogu_engine_keeper : public CreatureScript
{
    public:
        npc_mogu_engine_keeper() : CreatureScript("npc_mogu_engine_keeper") { }

        struct npc_mogu_engine_keeperAI : public ScriptedAI
        {
            npc_mogu_engine_keeperAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_KEEPER_ENHANCED_RECONSTRUCTION, urand(5000, 12000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                    if (eventId == EVENT_KEEPER_ENHANCED_RECONSTRUCTION)
                    {
                            if (Unit* target = GetCreatureWithLowHp())
                                me->CastSpell(target, SPELL_ENHANCED_RECONSTRUCTION, false);
                            events.ScheduleEvent(EVENT_KEEPER_ENHANCED_RECONSTRUCTION, 30000);
                            break;
                    }

                DoMeleeAttackIfReady();
            }
        private:
            Creature* GetCreatureWithLowHp()
            {
                std::list<Creature*> MogushanKeepers;

                GetCreatureListWithEntryInGrid(MogushanKeepers,me,NPC_MOGUSHAN_ARCANIST,150.0f);
                GetCreatureListWithEntryInGrid(MogushanKeepers, me, NPC_MOGUSHAN_WARDEN_T, 150.0f);

                float pct = 100.0f;
                Creature* cur = NULL;

                for (auto&& itr : MogushanKeepers)
                {
                    if (itr->GetHealthPct() < pct)
                    {
                        pct = itr->GetHealthPct();
                        cur = itr;
                    }
                }

                return cur;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_mogu_engine_keeperAI(creature);
        }
};

class npc_shan_xi_watershaper: public CreatureScript
{
    public:
        npc_shan_xi_watershaper() : CreatureScript("npc_shan_xi_watershaper") { }

        enum eSpells
        {
            SPELL_FROST_CHANNELING = 45846,
            SPELL_LIFESTREAM       = 118564
        };

        enum eEvents
        {
            EVENT_FROST_CHANNELING = 1,
            EVENT_LIFESTEAM        = 2
        };

        struct npc_shan_xi_watershaperAI : public ScriptedAI
        {
            npc_shan_xi_watershaperAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            EventMap cosmeticEvents;

            void Reset() override
            {
                events.Reset();
                cosmeticEvents.Reset();

                cosmeticEvents.ScheduleEvent(EVENT_FROST_CHANNELING, 1 * IN_MILLISECONDS);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->RemoveAura(SPELL_FROST_CHANNELING);

                events.ScheduleEvent(EVENT_LIFESTEAM, urand(7 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (cosmeticEvents.ExecuteEvent() == EVENT_FROST_CHANNELING)
                {
                    me->CastSpell(me, SPELL_FROST_CHANNELING, false);
                }

                cosmeticEvents.Update(diff);
                events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (events.ExecuteEvent() == EVENT_LIFESTEAM)
                {
                    me->CastSpell(me, SPELL_LIFESTREAM, false);
                    events.ScheduleEvent(EVENT_LIFESTEAM, urand(7 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shan_xi_watershaperAI(creature);
        }
};

class npc_zandalari_fire_dancer : public CreatureScript
{
    public:
        npc_zandalari_fire_dancer() : CreatureScript("npc_zandalari_fire_dancer") { }

        enum eSpells
        {
            SPELL_PYROBLAST = 120670,
            SPELL_BLAZING_SPEED = 116592,
            SPELL_FIREBOLT = 15228,
            SPELL_COSMETIC_FIREBALL = 117822,
            SPELL_FIRE_CHANNELING = 117826,
            SPELL_DETONATE = 117241
        };

        enum eEvents
        {
            EVENT_PYROBLAST = 1,
            EVENT_BLAZING_SPEED = 2,
            EVENT_FIREBOLT = 3,
            EVENT_DETONATE = 4,
            EVENT_COSMETIC_FIREBALL = 5,
            EVENT_FIRE_CHANNELING = 6
        };

        struct npc_zandalari_fire_dancerAI : public ScriptedAI
        {
            npc_zandalari_fire_dancerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            EventMap cosmeticEvents;

            void Reset() override
            {
                events.Reset();
                cosmeticEvents.Reset();

                cosmeticEvents.ScheduleEvent(urand(EVENT_COSMETIC_FIREBALL, EVENT_FIRE_CHANNELING), 1 * IN_MILLISECONDS);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                cosmeticEvents.Reset();

                events.ScheduleEvent(EVENT_PYROBLAST, urand(5 * IN_MILLISECONDS, 9 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_BLAZING_SPEED, urand(12 * IN_MILLISECONDS, 17 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DETONATE, urand(12 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FIREBOLT, 1 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (uint32 eventId = cosmeticEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FIRE_CHANNELING:
                            me->CastSpell(me, SPELL_FIRE_CHANNELING, false);
                            cosmeticEvents.ScheduleEvent(EVENT_COSMETIC_FIREBALL, 16 * IN_MILLISECONDS);
                            break;
                        case EVENT_COSMETIC_FIREBALL:
                            me->CastSpell(me, SPELL_COSMETIC_FIREBALL, false);
                            cosmeticEvents.ScheduleEvent(EVENT_FIRE_CHANNELING, 3 * IN_MILLISECONDS);
                            break;
                    }
                }

                cosmeticEvents.Update(diff);
                events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PYROBLAST:
                            me->CastSpell((Unit*)NULL, SPELL_PYROBLAST, false);
                            events.ScheduleEvent(EVENT_PYROBLAST, urand(5 * IN_MILLISECONDS, 9 * IN_MILLISECONDS));
                            break;
                        case EVENT_BLAZING_SPEED:
                            me->CastSpell(me, SPELL_BLAZING_SPEED, false);
                            events.ScheduleEvent(EVENT_BLAZING_SPEED, urand(41 * IN_MILLISECONDS, 47 * IN_MILLISECONDS));
                            break;
                        case EVENT_DETONATE:
                            me->CastSpell(me, SPELL_DETONATE, false);
                            events.ScheduleEvent(EVENT_DETONATE, urand(12 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
                            break;
                        case EVENT_FIREBOLT:
                            me->CastSpell((Unit*)NULL, SPELL_FIREBOLT, false);
                            events.ScheduleEvent(EVENT_FIREBOLT, 3 * IN_MILLISECONDS);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zandalari_fire_dancerAI(creature);
        }
};

class npc_zandalari_infiltrator : public CreatureScript
{
    public:
        npc_zandalari_infiltrator() : CreatureScript("npc_zandalari_infiltrator") { }

        enum eSpells
        {
            SPELL_SMOKE_BOMB = 116595
        };

        enum eEvents
        {
            EVENT_SMOKE_BOMB = 1
        };

        struct npc_zandalari_infiltratorAI : public ScriptedAI
        {
            npc_zandalari_infiltratorAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SMOKE_BOMB, urand(4 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_SMOKE_BOMB)
                {
                    me->CastSpell(me, SPELL_SMOKE_BOMB, false);
                    events.ScheduleEvent(EVENT_SMOKE_BOMB, urand(24 * IN_MILLISECONDS, 28 * IN_MILLISECONDS));
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zandalari_infiltratorAI(creature);
        }
};

class npc_troll_explosives : public CreatureScript
{
    public:
        npc_troll_explosives() : CreatureScript("npc_troll_explosives") { }

        enum eSpells
        {
            SPELL_EXPLOSIOIN_AURA = 116508
        };

        struct npc_troll_explosivesAI : public ScriptedAI
        {
            npc_troll_explosivesAI(Creature* creature) : ScriptedAI(creature) { }

            float orientation;

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                orientation = 0.0f;

                switch (me->GetEntry())
                {
                    case NPC_EXPLOSION_BUNNY_N_M:
                        orientation = 0.0f;
                        break;
                    case NPC_EXPLOSION_BUNNY_N_P:
                        orientation = M_PI / 4;
                        break;
                    case NPC_EXPLOSION_BUNNY_S_M:
                        orientation = M_PI / 2;
                        break;
                    case NPC_EXPLOSION_BUNNY_S_P:
                        orientation = 3 * M_PI / 4;
                        break;
                    case NPC_EXPLOSION_BUNNY_E_M:
                        orientation = M_PI;
                        break;
                    case NPC_EXPLOSION_BUNNY_E_P:
                        orientation = 5 * M_PI / 4;
                        break;
                    case NPC_EXPLOSION_BUNNY_W_M:
                        orientation = 3 * M_PI / 2;
                        break;
                    case NPC_EXPLOSION_BUNNY_W_P:
                        orientation = 7 * M_PI / 4;
                        break;
                }

                float x, y = 0.0f;
                GetPositionWithDistInOrientation(me, 40.0f, orientation, x, y);
                me->GetMotionMaster()->MovePoint(1, x, y, me->GetPositionZ());

                me->AddAura(SPELL_EXPLOSIOIN_AURA, me);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                damage = 0;
            }

            void MovementInform(uint32 /*type*/, uint32 pointId) override
            {
                if (pointId == 1)
                    me->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_troll_explosivesAI(creature);
        }
};

// Lorewalker Cho 61348
class npc_msv_lorewalker_cho : public CreatureScript
{
    public:
        npc_msv_lorewalker_cho() : CreatureScript("npc_msv_lorewalker_cho") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
                creature->AI()->DoAction(ACTION_SPIRIT_KINGS_INTRO);

            player->CLOSE_GOSSIP_MENU();

            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (instance->GetBossState(DATA_GARAJAL) == DONE)
                {
                    player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                }
            return true;
        }

        struct npc_msv_lorewalker_choAI : public ScriptedAI
        {
            npc_msv_lorewalker_choAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript())
            {
                Start = false;
                wp = 0;

                if (me->GetDBTableGUIDLow())
                    me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                if (_instance->GetData(DATA_LFR))
                {
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    wp = 17;
                    SetRun();
                }
            }

            EventMap events, nonCombatEvents;
            InstanceScript* _instance;
            std::list<Creature*> SpiritKingsSeq;
            bool Start; uint32 wp;

            void IsSummonedBy(Unit* summoner) override 
            {
                switch (summoner->GetEntry())
                {
                    case NPC_STONE_GUARD_CONTROLLER:
                        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        wp = 7;
                        break;
                    case NPC_FENG:
                        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        wp = 14;
                        SetWalk();
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 10000);
                        me->GetMotionMaster()->MovePoint(wp, ChoWaypoints[wp]);
                        break;
                    default:
                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        wp = 17;
                        SetRun();
                        break;
                }
            }

            void JustDied(Unit* /*killer*/) override { }

            void DoAction(int32 actionId) override 
            {
                switch (actionId)
                {
                    case ACTION_CHO_DOGS_DIE:
                        SetRun();
                        me->GetMotionMaster()->MovePoint(wp, ChoWaypoints[wp]);
                        break;
                    case ACTION_SPIRIT_KINGS_INTRO:
                        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        SetWalk();
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, urand(100, 200));
                        break;
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override { }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                wp++;

                switch (pointId)
                {
                    case 2: // Quilens
                        Talk(LOREWALKER_CHO_DOGS);
                        me->GetMotionMaster()->MovementExpired();
                        me->GetMotionMaster()->Clear();
                        break;
                    case 7: // Door opened to Feng
                        SetWalk();
                        Talk(LOREWALKER_CHO_DOGS_DIE);
                        nonCombatEvents.ScheduleEvent(EVENT_DOOR_OPENED, urand(3500, 5000));
                        break;
                    case 10: // First cursed mogu die
                        Talk(LOREWALKER_CHO_FENG_1);
                        nonCombatEvents.ScheduleEvent(EVENT_CHECK_CURSED_MOGU_1, urand(100, 200));
                        break;
                    case 13: // Second cursed mogu die and u behind Feng
                        Talk(LOREWALKER_CHO_FENG_3);
                        me->GetMotionMaster()->MovementExpired();
                        me->GetMotionMaster()->Clear();
                        break;
                    case 14:
                        Talk(LOREWALKER_CHO_FENG_PAST_2);
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, urand(100, 200));
                        break;
                    case 15:
                        Talk(LOREWALKER_CHO_GARAJAL_1);
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, urand(12000, 19000));
                        break;
                    case 16:
                        me->GetMotionMaster()->MovementExpired();
                        me->GetMotionMaster()->Clear();
                        Talk(LOREWALKER_CHO_GARAJAL_2);
                        nonCombatEvents.ScheduleEvent(EVENT_BEHIND_GARAJAL, urand(5000, 7000));
                        break;
                    case 19: // Spirit kings event
                        DoCast(me, SPELL_STORY_TIME);
                        Talk(LOREWALKER_CHO_SPIRIT_KINGS_EVENT);
                        SendActiveTrueOnSpiritKings();
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 7500);
                        break;
                    case 20: // Zian stoty
                        Talk(LOREWALKER_CHO_SPIRIT_KINGS_EVENT_ZIAN);
                        if (Creature* Zian = GetClosestCreatureWithEntry(me, NPC_ZIAN_THE_ENDLESS_SHADOW, 100.0f, true))
                            Zian->AI()->DoAction(ACTION_SPIRIT_KINGS_EVENT_ACTIVE);
                        break;
                    case 21: // Qiang story
                        Talk(LOREWALKER_CHO_SPIRIT_KINGS_EVENT_QIANG);
                        if (Creature* Qiang = GetClosestCreatureWithEntry(me, NPC_QIANG_THE_MERCILESS, 100.0f, true))
                            Qiang->AI()->DoAction(ACTION_SPIRIT_KINGS_EVENT_ACTIVE);
                        break;
                    case 22: // Subetai story
                        Talk(LOREWALKER_CHO_SPIRIT_KINGS_EVENT_SUBETAI);
                        if (Creature* Subetai = GetClosestCreatureWithEntry(me, NPC_SUBETAI_THE_SWIFT, 150.0f, true))
                            Subetai->AI()->DoAction(ACTION_SPIRIT_KINGS_EVENT_ACTIVE);
                        break; 
                    case 23: // Meng story
                        Talk(LOREWALKER_CHO_SPIRIT_KINGS_EVENT_MENG);
                        if (Creature* Meng = GetClosestCreatureWithEntry(me, NPC_MENG_THE_DEMENTED, 100.0f, true))
                            Meng->AI()->DoAction(ACTION_SPIRIT_KINGS_EVENT_ACTIVE);
                        break;
                    case 25: // Behind Spirit Kings Encounter
                        me->GetMotionMaster()->MovementExpired();
                        me->GetMotionMaster()->Clear();

                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);

                        _instance->SetData(DATA_SPIRIT_KINGS_STATE, DONE);

                        if (Creature* Qiang = GetClosestCreatureWithEntry(me, NPC_QIANG, 150.0f, true))
                        {
                            Qiang->AI()->DoAction(ACTION_FIRST_FIGHT);
                            Qiang->GetMotionMaster()->MovePath(Qiang->GetDBTableGUIDLow(), true);
                        }
                        break;
                    default:
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, urand(100, 200));
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);
                nonCombatEvents.Update(diff);

                if (!Start&&_instance->GetBossState(DATA_STONE_GUARD)!=DONE)
                    if (Player* itr = me->FindNearestPlayer(30.0f))
                    {
                        Start = true;
                        Talk(LOREWALKER_CHO_INTRO_1);
                        nonCombatEvents.ScheduleEvent(EVENT_INTRO, 11000);
                        SetWalk();

                    }

                if (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ESCORT:
                            me->GetMotionMaster()->MovementExpired();
                            me->GetMotionMaster()->MovePoint(wp, ChoWaypoints[wp]);
                            break;
                        case EVENT_INTRO:
                            Talk(LOREWALKER_CHO_INTRO_2);
                            me->GetMotionMaster()->MovePoint(wp, ChoWaypoints[wp]);
                            break;
                        case EVENT_DOOR_OPENED:
                            Talk(LOREWALKER_CHO_DOGS_DOOR_EVENT);
                            nonCombatEvents.ScheduleEvent(EVENT_CHECK_CURSED_MOGU_2, urand(100, 200));
                            break;
                        case EVENT_CHECK_CURSED_MOGU_2:
                            if (Creature* CursedMogu = GetClosestCreatureWithEntry(me, NPC_CURSED_MOGU_SCULPTURE_2, 90.0f, true))
                            {
                                nonCombatEvents.ScheduleEvent(EVENT_CHECK_CURSED_MOGU_2, urand(100, 200));
                                return;
                            }
                            me->GetMotionMaster()->MovePoint(wp, ChoWaypoints[wp]);
                            break;
                        case EVENT_CHECK_CURSED_MOGU_1:
                            if (Creature* CursedMogu = GetClosestCreatureWithEntry(me, NPC_CURSED_MOGU_SCULPTURE_1, 95.0f, true))
                            {
                                nonCombatEvents.ScheduleEvent(EVENT_CHECK_CURSED_MOGU_1, urand(100, 200));
                                return;
                            }
                            Talk(LOREWALKER_CHO_FENG_2);
                            me->GetMotionMaster()->MovePoint(wp, ChoWaypoints[wp]);
                            break;
                        case EVENT_BEHIND_GARAJAL:
                            Talk(LOREWALKER_CHO_GARAJAL_3);
                            break;
                    }
                }
            }

        private:
            void SetRun()
            {
                if (me->GetUnitMovementFlags() == MOVEMENTFLAG_WALKING)
                    me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                me->SetSpeed(MOVE_RUN, 2.8f);
            }

            void SetWalk()
            {
                me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                me->SetSpeed(MOVE_WALK, 1.4f);
            }

            void SendActiveTrueOnSpiritKings()
            {
                std::list<Creature*> SpiritKings;

                GetCreatureListWithEntryInGrid(SpiritKings, me, NPC_ZIAN_THE_ENDLESS_SHADOW, 100.0f);
                GetCreatureListWithEntryInGrid(SpiritKings, me, NPC_QIANG_THE_MERCILESS, 100.0f);
                GetCreatureListWithEntryInGrid(SpiritKings, me, NPC_SUBETAI_THE_SWIFT, 100.0f);
                GetCreatureListWithEntryInGrid(SpiritKings, me, NPC_MENG_THE_DEMENTED, 100.0f);

                for (auto&& itr : SpiritKings)
                    itr->AI()->DoAction(ACTION_SPIRIT_KINGS_EVENT);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_msv_lorewalker_choAI(creature);
        }
};

// Spirit kings guardian 61250, 61341, 61345, 61347
class npc_spirit_kings_guardian : public CreatureScript
{
    public:
        npc_spirit_kings_guardian() : CreatureScript("npc_spirit_kings_guardian") { }

        struct npc_spirit_kings_guardianAI : public ScriptedAI
        {
            npc_spirit_kings_guardianAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events, nonCombatEvents;
            uint32 MainSpell, SpellTimer;

            void IsSummonedBy(Unit* summoner) override { }

            void InitializeAI() override
            {
                me->SetDisplayId(11686);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                me->SetVisible(false);
            }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SPIRIT_MOGU_COMBAT, SpellTimer);
            }

            void JustDied(Unit* /*killer*/) override { }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_SPIRIT_KINGS_EVENT:
                        if (!me->IsAlive())
                            me->Respawn();

                        switch (me->GetEntry())
                        {
                            case NPC_SORCERER_MOGU:
                                me->SetDisplayId(40138);
                                SetCombatMovement(false);
                                MainSpell = SPELL_SHADOW_BLAST;
                                SpellTimer = 5000;
                                break;
                            case NPC_KINGSGUARD:
                                me->SetDisplayId(41498);
                                MainSpell = SPELL_KG_ENRAGE;
                                SpellTimer = urand(7000, 14000);
                                break;
                            case NPC_MOGU_ARCHER:
                                me->SetDisplayId(me->GetNativeDisplayId());
                                SetCombatMovement(false);
                                MainSpell = SPELL_MA_SHOT;
                                SpellTimer = 3000;
                                break;
                            case NPC_MOUNTED_MOGU:
                                me->SetDisplayId(me->GetNativeDisplayId());
                                MainSpell = SPELL_MM_CRUSHING_ATTACK;
                                SpellTimer = 5000;
                                break;
                        }
                        me->CastSpell(me, SPELL_INACTIVE, false);
                        me->SetVisible(true);
                        break;
                    case ACTION_SPIRIT_KINGS_EVENT_ACTIVE:
                        me->AddAura(SPELL_SPIRIT_KINGS_EVENT_ACTIVATION_VISUAL, me);
                        nonCombatEvents.ScheduleEvent(EVENT_ACTIVATED, urand(3500, 6000));
                        break;
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override { }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_ACTIVATED)
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED | UNIT_FLAG_NOT_SELECTABLE);
                        me->SetReactState(REACT_AGGRESSIVE);

                        if (me->HasAura(SPELL_SPIRIT_KINGS_EVENT_ACTIVATION_VISUAL))
                            me->RemoveAura(SPELL_ACTIVATION_VISUAL);

                        if (Player* itr = me->FindNearestPlayer(80.0f))
                            me->AI()->AttackStart(itr);
                    }
                    break;
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_SPIRIT_MOGU_COMBAT)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                            me->CastSpell(target, MainSpell, false);

                        events.ScheduleEvent(EVENT_SPIRIT_MOGU_COMBAT, SpellTimer);
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_spirit_kings_guardianAI(creature);
        }
};

// Spirit kings story 61421, 61423, 61427, 61429
class npc_spirit_kings_story : public CreatureScript
{
    public:
        npc_spirit_kings_story() : CreatureScript("npc_spirit_kings_story") { }

        struct npc_spirit_kings_storyAI : public ScriptedAI
        {
            npc_spirit_kings_storyAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events, nonCombatEvents;

            void IsSummonedBy(Unit* summoner) override { }

            void InitializeAI() override
            {
                me->SetDisplayId(11686);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                me->SetVisible(false);
            }

            void JustDied(Unit* /*killer*/) override { }

            void DoAction(int32 actionId) override 
            {
                switch (actionId)
                {
                    case ACTION_SPIRIT_KINGS_EVENT:
                        switch (me->GetEntry())
                        {
                            case NPC_ZIAN_THE_ENDLESS_SHADOW:
                                SendActiveOnMyMinions(NPC_SORCERER_MOGU, ACTION_SPIRIT_KINGS_EVENT);
                                break;
                            case NPC_QIANG_THE_MERCILESS:
                                SendActiveOnMyMinions(NPC_MOUNTED_MOGU, ACTION_SPIRIT_KINGS_EVENT);
                                break;
                            case NPC_SUBETAI_THE_SWIFT:
                                SendActiveOnMyMinions(NPC_MOGU_ARCHER, ACTION_SPIRIT_KINGS_EVENT);
                                break;
                            case NPC_MENG_THE_DEMENTED:
                                SendActiveOnMyMinions(NPC_KINGSGUARD, ACTION_SPIRIT_KINGS_EVENT);
                                break;
                        }

                        if (!me->IsAlive())
                            me->Respawn();

                        me->SetDisplayId(me->GetNativeDisplayId());
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        me->CastSpell(me, SPELL_SPIRIT_KINGS_EVENT_INACTIVE, false);
                        me->SetVisible(true);
                        break;
                    case ACTION_SPIRIT_KINGS_EVENT_ACTIVE:
                        switch (me->GetEntry())
                        {
                            case NPC_ZIAN_THE_ENDLESS_SHADOW:
                                SendActiveOnMyMinions(NPC_SORCERER_MOGU, ACTION_SPIRIT_KINGS_EVENT_ACTIVE);
                                break;
                            case NPC_QIANG_THE_MERCILESS:
                                SendActiveOnMyMinions(NPC_MOUNTED_MOGU, ACTION_SPIRIT_KINGS_EVENT_ACTIVE);
                                break;
                            case NPC_SUBETAI_THE_SWIFT:
                                SendActiveOnMyMinions(NPC_MOGU_ARCHER, ACTION_SPIRIT_KINGS_EVENT_ACTIVE);
                                break;
                            case NPC_MENG_THE_DEMENTED:
                                SendActiveOnMyMinions(NPC_KINGSGUARD, ACTION_SPIRIT_KINGS_EVENT_ACTIVE);
                                break;
                        }
                        nonCombatEvents.ScheduleEvent(EVENT_ACTIVATED, 1000);
                        break;
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override { }

            void UpdateAI(uint32 diff) override
            {

                events.Update(diff);
                nonCombatEvents.Update(diff);

                if (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_ACTIVATED)
                    {
                        if (!MyMinionsHasAlive(me->GetEntry()))
                        {
                            if (Creature* LoreWalkerCho = GetClosestCreatureWithEntry(me, NPC_LOREWALKER_CHO, 150.0f, true))
                                LoreWalkerCho->AI()->DoAction(ACTION_SPIRIT_KINGS_INTRO);
                            me->DespawnOrUnsummon();
                        }
                        else nonCombatEvents.ScheduleEvent(EVENT_ACTIVATED, urand(100, 200));                       
                    }

                }
            }

        private:
            void SendActiveOnMyMinions(uint32 entry, uint32 action)
            {
                std::list<Creature*> Minions;
                GetCreatureListWithEntryInGrid(Minions, me, entry, 100.0f);

                for (auto&& itr : Minions)
                    itr->AI()->DoAction(action);
            }

            bool MyMinionsHasAlive(uint32 entry)
            {
                switch (entry)
                {
                    case NPC_ZIAN_THE_ENDLESS_SHADOW:
                        if (Creature* Sorcerer = GetClosestCreatureWithEntry(me, NPC_SORCERER_MOGU, 50.0f, true))
                            return true;
                        break;
                    case NPC_QIANG_THE_MERCILESS:
                        if (Creature* MounterMogu = GetClosestCreatureWithEntry(me, NPC_MOUNTED_MOGU, 50.0f, true))
                            return true;
                        break;
                    case NPC_SUBETAI_THE_SWIFT:
                        if (Creature* Archer = GetClosestCreatureWithEntry(me, NPC_MOGU_ARCHER, 50.0f, true))
                            return true;
                        break;
                    case NPC_MENG_THE_DEMENTED:
                        if (Creature* Kingsguard = GetClosestCreatureWithEntry(me, NPC_KINGSGUARD, 50.0f, true))
                            return true;
                        break;
                }

                return false;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_spirit_kings_storyAI(creature);
        }
};

// Mogushan Arcanist 64063
class npc_mogushan_arcanist : public CreatureScript
{
    public:
        npc_mogushan_arcanist() : CreatureScript("npc_mogushan_arcanist") { }

        struct npc_mogushan_arcanistAI : public ScriptedAI
        {
            npc_mogushan_arcanistAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void IsSummonedBy(Unit* summoner) override { }

            void JustDied(Unit* /*killer*/) override { }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_ARCANIST_RAW_ENERGY, urand(5000, 12000));
            }

            void DoAction(int32 actionId) override { }

            void DamageTaken(Unit* attacker, uint32& damage) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                    if (eventId == EVENT_ARCANIST_RAW_ENERGY)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                            me->CastSpell(target, SPELL_RAW_ENERGY, false);
                        events.ScheduleEvent(EVENT_ARCANIST_RAW_ENERGY, urand(5000, 12000));
                    }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_mogushan_arcanistAI(creature);
        }
};

// Leng Windstaff 64825
class npc_mogushan_vaults_leng_windstaff : public CreatureScript
{
    public:
        npc_mogushan_vaults_leng_windstaff() : CreatureScript("npc_mogushan_vaults_leng_windstaff") { }
    
        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
    
            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    
                if (creature->GetInstanceScript())
                    creature->GetInstanceScript()->DoRemoveDampeningFromCreatures();
            }
    
            player->CLOSE_GOSSIP_MENU();
    
            return true;
        }
    
        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());
                
            if(sConfigMgr->GetIntDefault("FirstOfTheKingdom.MogushanVaults", 1))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Turn off dampening?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }
    
        struct npc_mogushan_vaults_leng_windstaffAI : public ScriptedAI
        {
            npc_mogushan_vaults_leng_windstaffAI(Creature* creature) : ScriptedAI(creature) { }
    
            void InitializeAI() override
            {
                if (me->GetMap()->GetDifficulty() != RAID_DIFFICULTY_25MAN_LFR)
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_mogushan_vaults_leng_windstaffAI(creature);
        }
};

class DetonateTargetSelector
{
    public:
        DetonateTargetSelector() { }

        enum eNpc
        {
            NPC_TROLL_EXPLOSIVES = 60644
        };

        bool operator()(WorldObject* object)
        {
            if (Creature* cre = object->ToCreature())
                if (cre->GetEntry() == NPC_TROLL_EXPLOSIVES)
                    return false;

                return true;
        }
};

class spell_mvault_detonate : public SpellScriptLoader
{
    public:
        spell_mvault_detonate() : SpellScriptLoader("spell_mvault_detonate") { }

        enum eSpells
        {
            SPELL_TROLL_EXPLOSIVES = 116545
        };

        class spell_mvault_detonate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mvault_detonate_SpellScript)

            void FilterTargets(std::list<WorldObject*>& targetList)
            {
                targetList.remove_if (DetonateTargetSelector());

                if (targetList.size() > 1)
                {
                    targetList.sort(Trinity::ObjectDistanceOrderPred(GetCaster()));
                    targetList.resize(1);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mvault_detonate_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        class spell_mvault_detonate_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mvault_detonate_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Creature* owner = GetOwner()->ToCreature();
                if (!owner)
                    return;

                owner->CastSpell(owner, SPELL_TROLL_EXPLOSIVES, false);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_mvault_detonate_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mvault_detonate_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_mvault_detonate_AuraScript();
        }
};

class spell_troll_explosives : public SpellScriptLoader
{
    public:
        spell_troll_explosives() : SpellScriptLoader("spell_troll_explosives") { }

        enum eSpells
        {
            SPELL_EXPLOSION = 116493
        };

        class spell_troll_explosives_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_troll_explosives_AuraScript);

            void OnRemove(AuraEffect const* , AuraEffectHandleModes)
            {
                if (Creature* owner = GetOwner()->ToCreature())
                {
                    if (Unit* caster = GetCaster())
                    {
                        if (owner->GetDBTableGUIDLow())
                            owner->CastSpell(owner, SPELL_EXPLOSION, false);
                        else
                            owner->CastSpell(owner, SPELL_EXPLOSION, false, NULL, NULL, caster->GetGUID());

                        owner->DespawnOrUnsummon();
                    }
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_troll_explosives_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_troll_explosives_AuraScript();
        }
};

class ExplosionTargetSelector
{
    public:
        ExplosionTargetSelector() { }

        bool operator()(WorldObject* object)
        {
            if (object->GetTypeId() == TYPEID_PLAYER)
                return false;

            return true;
        }
};

class spell_mvault_explosion : public SpellScriptLoader
{
    public:
        spell_mvault_explosion() : SpellScriptLoader("spell_mvault_explosion") { }

        class spell_mvault_explosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mvault_explosion_SpellScript)

            void FilterTargets(std::list<WorldObject*>& targetList)
            {
                targetList.remove_if (ExplosionTargetSelector());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mvault_explosion_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mvault_explosion_SpellScript();
        }
};

// Impale 121247
class spell_msv_impale : public SpellScriptLoader
{
    public:
        spell_msv_impale() : SpellScriptLoader("spell_msv_impale") { }

        class spell_msv_impale_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_msv_impale_SpellScript);

            void RecalculateDamage(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();

                if (!caster)
                    return;

                SetHitDamage(frand(1.95f, 2.05f)*urand(15068, 21738)); 
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_msv_impale_SpellScript::RecalculateDamage, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_msv_impale_SpellScript();
        }
};

// Fortify Stone Aura 116980
class spell_fortify_stone_aura : public SpellScriptLoader
{
    public:
        spell_fortify_stone_aura() : SpellScriptLoader("spell_fortify_stone_aura") { }

        class spell_fortify_stone_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fortify_stone_aura_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                std::list<Creature*> Quilens;
                GetCreatureListWithEntryInGrid(Quilens, GetUnitOwner(), NPC_STONE_QUILEN, 80.0f);

                for (auto&& itr : Quilens)
                    if (itr->HasAura(SPELL_FORTIFY_STONE_AURA))
                        itr->RemoveAura(SPELL_FORTIFY_STONE_AURA);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_fortify_stone_aura_AuraScript::OnRemove, EFFECT_0,SPELL_AURA_MOD_AOE_DAMAGE_AVOIDANCE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fortify_stone_aura_AuraScript();
        }
};

// Areatrigger 8750
class AreaTrigger_at_msv_engine_of_nalaksha : public AreaTriggerScript
{
    public:
        AreaTrigger_at_msv_engine_of_nalaksha() : AreaTriggerScript("AreaTrigger_at_msv_engine_of_nalaksha") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            player->CastSpell(player, SPELL_COALESCING_ENERGIZE, true);

            return true;
        }
};

void AddSC_mogu_shan_vault()
{
    new npc_cursed_mogu_sculpture();
    new npc_enormous_stone_quilen();
    new npc_mogu_ghost_essence();
    new npc_stone_quilen();
    new npc_zandalari_skullcharger();
    new npc_zandalari_pterror_wing();
    new npc_zandalari_terror_rider();
    new npc_mogu_secret_keeper();
    new npc_mogu_warden();
    new npc_mogu_engine_keeper();
    new npc_shan_xi_watershaper();
    new npc_zandalari_fire_dancer();
    new npc_zandalari_infiltrator();
    new npc_troll_explosives();
    new npc_msv_lorewalker_cho();
    new npc_spirit_kings_guardian();
    new npc_spirit_kings_story();
    new npc_mogushan_arcanist();
    new npc_mogushan_vaults_leng_windstaff();
    new spell_ghost_essence();
    new spell_mvault_detonate();
    new spell_troll_explosives();
    new spell_mvault_explosion();
    new spell_mogu_petrification();
    new spell_mogu_petrification_periodic_aura();
    new spell_msv_impale();
    new spell_fortify_stone_aura();
    new AreaTrigger_at_msv_engine_of_nalaksha();
}
