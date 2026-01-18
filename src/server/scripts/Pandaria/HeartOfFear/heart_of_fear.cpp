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

#include "ScriptedCreature.h"
#include "heart_of_fear.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "CreatureAI.h"
#include "MoveSplineInit.h"
#include "InstanceSaveMgr.h"
#include "Config.h"

enum eTalk
{
    MELJARAK_TALK_EVENT_1                   = 0,
    MELJARAK_TALK_EVENT_2                   = 1,
    MELJARAK_TALK_EVENT_SUMMON_GARALON      = 2,
    MELJARAK_TALK_EVENT_GARALON_IN_COMBAT   = 3,
    MELJARAK_TALK_EVENT_GARALON_DEATH       = 4,
};

enum eEvents
{
    EVENT_MELJARAK_TERRACE_IN_COMBAT        = 1,
    EVENT_MELJARAK_INTRO                    = 2,
    EVENT_MELJARAK_TERRACE_EMPTY            = 3,
    EVENT_MELJARAK_TERRACE_LEAVE            = 4,
    EVENT_ARTERIAL_SPIRIT                   = 5,
    EVENT_BLURRING_SLASH                    = 6,
    EVENT_BRAIN_FREEZE                      = 7,
    EVENT_BURST                             = 8,
    EVENT_DISMANTLED_ARMOR                  = 9,
    EVENT_GALE_FORCE_WINDS                  = 10,
    EVENT_GOUGE_THROAT                      = 11,
    EVENT_GUST                              = 12,
    EVENT_ICE_TRAP                          = 13,
    EVENT_JAWBONE_SLAM                      = 14,
    EVENT_MASH_AND_GNASH                    = 15,
    EVENT_MASS_SPELL_REFLEXION              = 16,
    EVENT_OVERWHELMING_ASSAULT              = 17,
    EVENT_RIPOSTE                           = 18,
    EVENT_SHIELD_SLAM                       = 19,
    EVENT_SLOW                              = 20,
    EVENT_STUNNING_STRIKE                   = 21,
    EVENT_SUMMON_ZEPHYR                     = 22,
    EVENT_SWIFT_STEP                        = 23,
    EVENT_UNDERWHELMING_ASSAULT             = 24,
    EVENT_UNDYING_DEVOTION                  = 25,
    EVENT_VITAL_STRIKES                     = 26,
    EVENT_WIND_SLASH                        = 27,
    EVENT_WIND_STEP_2                       = 28,
    EVENT_WIND_STEP                         = 29,
    EVENT_WINDBURST                         = 30,
    EVENT_ZEALOUS_RUSH                      = 31,
    EVENT_SWARMGUARDS_AEGIS                 = 32,
    EVENT_SEPARATION_ANXIETY                = 33,
    EVENT_CARAPASE                          = 34,
    EVENT_AMBER_INFUSION                    = 35,
    EVENT_MORTAL_REND                       = 36,
    EVENT_GRIEVOUS_WHIRL                    = 37,
    EVENT_AMBER_VOLLEY                      = 38,
    EVENT_BURNING_STING                     = 39,
    EVENT_SEARING_SLASH                     = 40,
    EVENT_FRENZIED_ASSAULT                  = 41,
    EVENT_CRY_HAVOC                         = 42,
    EVENT_TOXIC_HIVEBOMB                    = 43,
    EVENT_UNSEEN_STRIKE                     = 44,
    EVENT_UNSEEN_STRIKE_RETURN              = 45,
    EVENT_TEMPEST_SLASH                     = 46,
    EVENT_ESCORT                            = 47,
    EVENT_ESCORT_DIFF                       = 48,
    EVENT_AMBER_SPEW                        = 49,
    EVENT_SLAM                              = 50,
    EVENT_UNSEEN_STRIKE_CHASE               = 51,
    EVENT_SEARING_SLASH_REMOVE              = 52,
    EVENT_UNSEEN_STRIKE_PREPARE             = 53,
};

enum eSpells
{
    SPELL_ARTERIAL_SPIRIT                   = 123422,
    SPELL_GOUGE_THROAT                      = 123434,
    SPELL_SLOW                              = 31589,
    SPELL_VITAL_STRIKES                     = 123421,
    SPELL_UNDYING_DEVOTION                  = 123502,
    SPELL_BRAIN_FREEZE                      = 57761,
    SPELL_DISMANTLED_ARMOR                  = 123417,
    SPELL_MASS_SPELL_REFLEXION              = 124817,
    SPELL_SHIELD_SLAM                       = 124815,
    SPELL_STUNNING_STRIKE                   = 123420,
    SPELL_BLURRING_SLASH                    = 123437,
    SPELL_RIPOSTE                           = 123436,
    SPELL_ICE_TRAP                          = 135382,
    SPELL_MASH_AND_GNASH                    = 124809,
    SPELL_JAWBONE_SLAM                      = 123490,
    SPELL_WIND_SLASH                        = 125716,
    SPELL_GALE_FORCE_WINDS                  = 123497,
    SPELL_WINDBURST                         = 123499,
    SPELL_SUMMON_ZEPHYR                     = 124085,
    SPELL_SWIFT_STEP                        = 125742,
    SPELL_WIND_STEP                         = 123180,
    SPELL_ZEALOUS_RUSH                      = 123622,
    SPELL_UNDERWHELMING_ASSAULT             = 125728,
    SPELL_GUST                              = 124072,
    SPELL_BURST                             = 125054,
    SPELL_WIND_STEP_2                       = 123175,
    SPELL_OVERWHELMING_ASSAULT              = 123474,
    SPELL_SWARMGUARDS_AEGIS                 = 126926,
    SPELL_SEPARATION_ANXIETY                = 126933,
    SPELL_CARAPASE                          = 126928,
    SPELL_AMBER_INFUSION                    = 125432,
    SPELL_AMBER_EMANATION                   = 125426,
    SPELL_MORTAL_REND                       = 126901,
    SPELL_GRIEVOUS_WHIRL                    = 126907,
    SPELL_AMBER_VOLLEY                      = 126937,
    SPELL_BURNING_STING                     = 125490,
    SPELL_SEARING_SLASH                     = 125480,
    SPELL_FRENZIED_ASSAULT                  = 125900,
    SPELL_CRY_HAVOC                         = 125907,
    SPELL_TOXIC_HIVEBOMB                    = 125865,
    SPELL_UNSEEN_STRIKE_TR                  = 122949,
    SPELL_UNSEEN_STRIKE_MKR                 = 123017,
    SPELL_TEMP_SLASH_SUMM_V                 = 122842,
    SPELL_AGITATE_POOL                      = 124039,
    SPELL_AMBER_SPEW                        = 125069,
    SPELL_SLAM                              = 125081,
    SPELL_AMBER_VOLLEY_MISSLE               = 126941,
    SPELL_ZEPHYR_EFF                        = 124087,
};

bool HasTerraceInCombat(WorldObject* owner)
{
    std::list<Creature*> Terrace;
    GetCreatureListWithEntryInGrid(Terrace, owner, NPC_ZETTHIK_GUSTWIND, 120.0f);
    GetCreatureListWithEntryInGrid(Terrace, owner, NPC_ZETTHIK_ZEPHYRIAN, 120.0f);

    for (auto&& itr : Terrace)
        if (itr->IsInCombat())
            return true;

    return false;
}

bool HasTerraceEmpty(Unit* owner)
{
    Creature* Gustwind=GetClosestCreatureWithEntry(owner, NPC_ZETTHIK_GUSTWIND, 120.0f, true);
    Creature* Zephyrian = GetClosestCreatureWithEntry(owner, NPC_ZETTHIK_ZEPHYRIAN, 120.0f, true);

    if (!Gustwind && !Zephyrian)
        return true;

    return false;
}

// Wind lord meljarak intro 65501
class npc_wind_lord_meljarak_intro : public CreatureScript
{
    public:
        npc_wind_lord_meljarak_intro() : CreatureScript("npc_wind_lord_meljarak_intro") { }

        struct npc_wind_lord_meljarak_introAI : public ScriptedAI
        {
            npc_wind_lord_meljarak_introAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events, nonCombatEvents;
            uint32 timer;
            bool inCombat;
            InstanceScript* instance;

            void IsSummonedBy(Unit* summoner) override { }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->SetAnimTier(AnimTier::Hover);
                me->AddUnitMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING);
            }

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetCanFly(true); me->SetDisableGravity(true);
                me->SetAnimTier(AnimTier::Hover);
                me->AddUnitMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING);
                inCombat = false;

                // if Pre event has done or not
                if (instance->GetData(DATA_GARALON) > DONE)
                    nonCombatEvents.ScheduleEvent(EVENT_MELJARAK_TERRACE_EMPTY, 1500);
                else 
                    nonCombatEvents.ScheduleEvent(EVENT_MELJARAK_TERRACE_IN_COMBAT, urand(200, 300));
            }

            void SetFlyOnFamiliars()
            {
                std::list<Creature*> Familiars;
                me->GetCreatureListWithEntryInGrid(Familiars,NPC_SRATHIK_AMBER_TRAPPER, 100.0f);
                me->GetCreatureListWithEntryInGrid(Familiars, NPC_KORTHIK_ELITE_BLADEMASTER, 100.0f);
                me->GetCreatureListWithEntryInGrid(Familiars, NPC_ZARTHIK_BATTLE_MENDER, 100.0f);
                
                for (auto&& itr : Familiars)
                {
                    itr->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    itr->SetReactState(REACT_PASSIVE);
                    itr->SetCanFly(true); 
                    itr->SetDisableGravity(true);
                    itr->SetAnimTier(AnimTier::Hover);
                    itr->AddUnitMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING);
                }
            }


            void SetMoveOnMeOrFamiliars(uint32 point = 1, float range = 50.0f)
            {
                GetNewPositionMove(me->GetGUID(), point, range);
                std::list<Creature*> Familiars;
                me->GetCreatureListWithEntryInGrid(Familiars, NPC_SRATHIK_AMBER_TRAPPER, 100.0f);
                me->GetCreatureListWithEntryInGrid(Familiars, NPC_KORTHIK_ELITE_BLADEMASTER, 100.0f);
                me->GetCreatureListWithEntryInGrid(Familiars, NPC_ZARTHIK_BATTLE_MENDER, 100.0f);

                for (auto&& itr : Familiars)
                {
                    itr->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); // it`s only if were crash/reboot of server
                    itr->SetReactState(REACT_PASSIVE);
                    GetNewPositionMove(itr->GetGUID());
                }
            }

            void LeaveFromTerrace()
            {
                std::list<Creature*> Familiars;
                me->GetCreatureListWithEntryInGrid(Familiars, NPC_SRATHIK_AMBER_TRAPPER, 30.0f);
                me->GetCreatureListWithEntryInGrid(Familiars, NPC_KORTHIK_ELITE_BLADEMASTER, 30.0f);
                me->GetCreatureListWithEntryInGrid(Familiars, NPC_ZARTHIK_BATTLE_MENDER, 30.0f);

                for (auto&& itr : Familiars)
                    itr->DespawnOrUnsummon();
                me->DespawnOrUnsummon();
            }

            void GetNewPositionMove(ObjectGuid owner, uint32 point = 1, float range = 50.0f)
            {
                float x = 0, y = 0;
                if (Unit* MeljarakSwarm = ObjectAccessor::GetUnit(*me, owner))
                {
                    GetPositionWithDistInOrientation(MeljarakSwarm, 50.0f, me->GetOrientation(), x, y);

                    MeljarakSwarm->GetMotionMaster()->MoveCharge(x, y, MeljarakSwarm->GetPositionZ(), range, point);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == EVENT_CHARGE)
                    LeaveFromTerrace();

                if (type != POINT_MOTION_TYPE)
                    return;
            }

            void DoAction(int32 actionId) override 
            {
                switch (actionId)
                {
                    case ACTION_MELJARAK_GARALON_IN_COMBAT:
                        Talk(MELJARAK_TALK_EVENT_GARALON_IN_COMBAT);
                        break;
                    case ACTION_MELJARAK_LEAVE:
                        Talk(MELJARAK_TALK_EVENT_GARALON_DEATH);
                        nonCombatEvents.ScheduleEvent(EVENT_MELJARAK_TERRACE_LEAVE, 3000);
                        break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                damage = 0;
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MELJARAK_TERRACE_IN_COMBAT:
                            if (!HasTerraceInCombat(me))
                            {
                                nonCombatEvents.ScheduleEvent(EVENT_MELJARAK_TERRACE_IN_COMBAT, urand(200, 300));
                                return;
                            }
                            Talk(MELJARAK_TALK_EVENT_1);
                            SetFlyOnFamiliars();
                            nonCombatEvents.ScheduleEvent(EVENT_MELJARAK_INTRO, 5000);
                            break;
                        case EVENT_MELJARAK_INTRO:
                            Talk(MELJARAK_TALK_EVENT_2);
                            nonCombatEvents.ScheduleEvent(EVENT_MELJARAK_TERRACE_EMPTY, urand(200, 300));
                            break;
                        case EVENT_MELJARAK_TERRACE_EMPTY:
                            if (!HasTerraceEmpty(me))
                            { 
                                nonCombatEvents.ScheduleEvent(EVENT_MELJARAK_TERRACE_EMPTY, urand(200, 300));
                                return;
                            }
                            Talk(MELJARAK_TALK_EVENT_SUMMON_GARALON);
                            SetMoveOnMeOrFamiliars();

                            if (instance->GetData(DATA_GARALON) != SPECIAL)
                            {
                                instance->SetData(DATA_GARALON, SPECIAL);

                                if (Creature* Garalon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_GARALON) : ObjectGuid::Empty))
                                    Garalon->AI()->DoAction(ACTION_GARALON_INITIALIZE);
                            }
                            break;
                        case EVENT_MELJARAK_TERRACE_LEAVE:
                            SetMoveOnMeOrFamiliars(EVENT_CHARGE, 100.0f);
                            break;
                    }
                }

            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_wind_lord_meljarak_introAI(creature);
        }
};

// 63048 - Kor'thik Slicer
class npc_kor_thik_slicer : public CreatureScript
{
    public:
        npc_kor_thik_slicer() : CreatureScript("npc_kor_thik_slicer") { }

        struct npc_kor_thik_slicerAI : public ScriptedAI
        {
            npc_kor_thik_slicerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            bool eventScheduled;

            void Reset() override
            {
                events.Reset();
                eventScheduled = false;

                if (IsHeroic())
                    events.ScheduleEvent(EVENT_SLOW, 3000);

                events.ScheduleEvent(EVENT_ARTERIAL_SPIRIT, 5000);
                events.ScheduleEvent(EVENT_VITAL_STRIKES, 12000);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (!eventScheduled)
                {
                    if (me->HealthBelowPctDamaged(80, damage))
                    {
                        events.ScheduleEvent(EVENT_GOUGE_THROAT, 12000);
                        eventScheduled = true;
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventid = events.ExecuteEvent())
                {
                    switch (eventid)
                    {
                        case EVENT_ARTERIAL_SPIRIT:
                            if (Unit* target = me->GetVictim())
                            {
                                if (const SpellInfo* sInfo = sSpellMgr->GetSpellInfo(SPELL_ARTERIAL_SPIRIT))
                                {
                                    int32 m_bp = sInfo->Effects[EFFECT_0].BasePoints * me->GetInt32Value(UNIT_FIELD_ATTACK_POWER) / 3;
                                    me->CastCustomSpell(target, SPELL_ARTERIAL_SPIRIT, &m_bp, 0, 0, true);
                                }
                            }

                            events.ScheduleEvent(EVENT_ARTERIAL_SPIRIT, 20000);
                            break;
                        case EVENT_GOUGE_THROAT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                me->CastSpell(target, SPELL_GOUGE_THROAT, true);

                            events.ScheduleEvent(EVENT_GOUGE_THROAT, 16000);
                            break;
                        case EVENT_VITAL_STRIKES:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                me->CastSpell(target, SPELL_VITAL_STRIKES, true);

                            events.ScheduleEvent(EVENT_VITAL_STRIKES, 20000);
                            break;
                        case EVENT_SLOW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                me->CastSpell(target, SPELL_SLOW, true);

                            events.ScheduleEvent(EVENT_SLOW, 40000);
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
            return new npc_kor_thik_slicerAI(creature);
        }
};

// 63036 - Kor'thik extremist
class npc_kor_thik_extremist : public CreatureScript
{
    public:
        npc_kor_thik_extremist() : CreatureScript("npc_kor_thik_extremist") { }

        struct npc_kor_thik_extremistAI : public ScriptedAI
        {
            npc_kor_thik_extremistAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                if (IsHeroic())
                    events.ScheduleEvent(EVENT_SLOW, 3000);
                events.ScheduleEvent(EVENT_UNDYING_DEVOTION, 5000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 id = events.ExecuteEvent())
                {
                    switch (id)
                    {
                        case EVENT_UNDYING_DEVOTION:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                me->CastSpell(target, SPELL_UNDYING_DEVOTION, true);

                            events.ScheduleEvent(EVENT_UNDYING_DEVOTION, 20000);
                            break;
                        case EVENT_SLOW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                me->CastSpell(target, SPELL_SLOW, true);

                            events.ScheduleEvent(EVENT_SLOW, 40000);
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
            return new npc_kor_thik_extremistAI(creature);
        }
};

// 63032 - Sra'thik Shield Master
class npc_sra_thik_shield_master : public CreatureScript
{
    public:
        npc_sra_thik_shield_master() : CreatureScript("npc_sra_thik_shield_master") { }

        struct npc_sra_thik_shield_masterAI : public ScriptedAI
        {
            npc_sra_thik_shield_masterAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                if (IsHeroic())
                    events.ScheduleEvent(EVENT_SLOW, 3000);

                events.ScheduleEvent(EVENT_DISMANTLED_ARMOR, 12000);
                events.ScheduleEvent(EVENT_MASS_SPELL_REFLEXION, 19000);
                events.ScheduleEvent(EVENT_SHIELD_SLAM, 26000);
                events.ScheduleEvent(EVENT_STUNNING_STRIKE, 35000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 id = events.ExecuteEvent())
                {
                    switch (id)
                    {
                        case EVENT_DISMANTLED_ARMOR:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_DISMANTLED_ARMOR, true);
                            events.ScheduleEvent(EVENT_DISMANTLED_ARMOR, 40000);
                            break;
                        case EVENT_MASS_SPELL_REFLEXION:
                            me->CastSpell(me, SPELL_MASS_SPELL_REFLEXION, true);
                            events.ScheduleEvent(EVENT_MASS_SPELL_REFLEXION, 40000);
                            break;
                        case EVENT_SHIELD_SLAM:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_SHIELD_SLAM, true);
                            events.ScheduleEvent(EVENT_SHIELD_SLAM, 40000);
                            break;
                        case EVENT_STUNNING_STRIKE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_STUNNING_STRIKE, true);
                            events.ScheduleEvent(EVENT_STUNNING_STRIKE, 40000);
                            break;
                        case EVENT_SLOW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                me->CastSpell(target, SPELL_SLOW, true);
                            events.ScheduleEvent(EVENT_SLOW, 40000);
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
            return new npc_sra_thik_shield_masterAI(creature);
        }
};

// 63049 - Set'thik Swiftblade
class npc_set_thik_swiftblade : public CreatureScript
{
    public:
        npc_set_thik_swiftblade() : CreatureScript("npc_set_thik_swiftblade") { }

        struct npc_set_thik_swiftbladeAI : public ScriptedAI
        {
            npc_set_thik_swiftbladeAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                if (!IsHeroic())
                    events.ScheduleEvent(EVENT_SLOW, 3000);
                events.ScheduleEvent(EVENT_BLURRING_SLASH, 7000);
                events.ScheduleEvent(EVENT_RIPOSTE, 12000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 id = events.ExecuteEvent())
                {
                    switch (id)
                    {
                        case EVENT_BLURRING_SLASH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_BLURRING_SLASH, true);
                            events.ScheduleEvent(EVENT_BLURRING_SLASH, 22000);
                            break;
                        case EVENT_RIPOSTE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_RIPOSTE, true);
                            events.ScheduleEvent(EVENT_RIPOSTE, 22000);
                            break;
                        case EVENT_SLOW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                me->CastSpell(target, SPELL_SLOW, true);
                            events.ScheduleEvent(EVENT_SLOW, 40000);
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
            return new npc_set_thik_swiftbladeAI(creature);
        }
};

class npc_zar_thik_supplicant : public CreatureScript
{
    public:
        npc_zar_thik_supplicant() : CreatureScript("npc_zar_thik_supplicant") { }

        struct npc_zar_thik_supplicantAI : public ScriptedAI
        {
            npc_zar_thik_supplicantAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_MASH_AND_GNASH, urand(4.5 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
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
                    if (eventId == EVENT_MASH_AND_GNASH)
                    {
                        if (Unit* target = me->GetVictim())
                            DoCast(target, SPELL_MASH_AND_GNASH);

                        events.ScheduleEvent(EVENT_MASH_AND_GNASH, urand(4.5 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zar_thik_supplicantAI(creature);
        }
};

class npc_enslaved_bonesmasher : public CreatureScript
{
    public:
        npc_enslaved_bonesmasher() : CreatureScript("npc_enslaved_bonesmasher") { }

        struct npc_enslaved_bonesmasherAI : public ScriptedAI
        {
            npc_enslaved_bonesmasherAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_JAWBONE_SLAM, urand(7 * IN_MILLISECONDS, 28 * IN_MILLISECONDS));

                if (IsHeroic())
                    events.ScheduleEvent(EVENT_SLOW, urand(3 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
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
                        case EVENT_JAWBONE_SLAM:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_JAWBONE_SLAM);

                            events.ScheduleEvent(EVENT_JAWBONE_SLAM, urand(12.5 * IN_MILLISECONDS, 32 * IN_MILLISECONDS));
                            break;
                        case EVENT_SLOW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                DoCast(target, SPELL_SLOW);

                            events.ScheduleEvent(EVENT_SLOW, urand(3 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
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
            return new npc_enslaved_bonesmasherAI(creature);
        }
};

class npc_set_thik_tempest : public CreatureScript
{
    public:
        npc_set_thik_tempest() : CreatureScript("npc_set_thik_tempest") { }

        struct npc_set_thik_tempestAI : public ScriptedAI
        {
            npc_set_thik_tempestAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_WIND_SLASH, urand(4 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
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
                    if (eventId == EVENT_WIND_SLASH)
                    {
                        if (Unit* target = me->GetVictim())
                            DoCast(target, SPELL_WIND_SLASH);

                        events.ScheduleEvent(EVENT_WIND_SLASH, urand(4 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_set_thik_tempestAI(creature);
        }
};

class npc_set_thik_fanatic : public CreatureScript
{
    public:
        npc_set_thik_fanatic() : CreatureScript("npc_set_thik_fanatic") { }

        struct npc_set_thik_fanaticAI : public ScriptedAI
        {
            npc_set_thik_fanaticAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_GALE_FORCE_WINDS, urand(7 * IN_MILLISECONDS, 27 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_WINDBURST, urand(15 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));

                if (!IsHeroic())
                    events.ScheduleEvent(EVENT_SLOW, 3000);
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
                        case EVENT_GALE_FORCE_WINDS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                DoCast(target, SPELL_GALE_FORCE_WINDS);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                DoCast(target, SPELL_GALE_FORCE_WINDS);

                            events.ScheduleEvent(EVENT_GALE_FORCE_WINDS, urand(7 * IN_MILLISECONDS, 27 * IN_MILLISECONDS));
                            break;
                        case EVENT_WINDBURST:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                DoCast(target, SPELL_WINDBURST);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                DoCast(target, SPELL_WINDBURST);

                            events.ScheduleEvent(EVENT_WINDBURST, urand(17 * IN_MILLISECONDS, 28.5 * IN_MILLISECONDS));
                            break;
                        case EVENT_SLOW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                DoCast(target, SPELL_SLOW);

                            events.ScheduleEvent(EVENT_SLOW, urand(3 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
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
            return new npc_set_thik_fanaticAI(creature);
        }
};

// Set`hik Zephyrian 63593
class npc_set_thik_zephyrian : public CreatureScript
{
    public:
        npc_set_thik_zephyrian() : CreatureScript("npc_set_thik_zephyrian") { }

        struct npc_set_thik_zephyrianAI : public ScriptedAI
        {
            npc_set_thik_zephyrianAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            EventMap events;
            SummonList summons;

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SUMMON_ZEPHYR, urand(4 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->CastSpell(summon, SPELL_ZEPHYR_EFF, true);
            }

            void JustDied(Unit* /*killer*/) override
            {
                summons.DespawnAll();
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
                    if (eventId == EVENT_SUMMON_ZEPHYR)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                            DoCast(target, SPELL_SUMMON_ZEPHYR);
                        else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                            DoCast(target, SPELL_SUMMON_ZEPHYR);

                        events.ScheduleEvent(EVENT_SUMMON_ZEPHYR, urand(11.5 * IN_MILLISECONDS, 42 * IN_MILLISECONDS));
                        break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_set_thik_zephyrianAI(creature);
        }
    };

// Set`hik Gale Slicer 64353
class npc_set_thik_gale_slicer : public CreatureScript
{
    public:
        npc_set_thik_gale_slicer() : CreatureScript("npc_set_thik_gale_slicer") { }

        struct npc_set_thik_gale_slicerAI : public ScriptedAI
        {
            npc_set_thik_gale_slicerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SWIFT_STEP, urand(4 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
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
                    if (eventId == EVENT_SWIFT_STEP)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                            DoCast(target, SPELL_SWIFT_STEP);

                        events.ScheduleEvent(EVENT_SWIFT_STEP, urand(5.5 * IN_MILLISECONDS, 28.5 * IN_MILLISECONDS));
                        break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_set_thik_gale_slicerAI(creature);
        }
};

void CallForHelpMyStudients(Creature* me, ObjectGuid instructorGUID)
{
    Unit* InstructorType = ObjectAccessor::GetUnit(*me, instructorGUID);

    if (!InstructorType)
        return;

    std::list<Creature*> Studients;

    switch (InstructorType->GetEntry())
    {
        case NPC_INSTRUCTOR_MALTIK:
            GetCreatureListWithEntryInGrid(Studients, InstructorType, NPC_KORTHIK_SILENTWING, VISIBLE_RANGE);
            break;
        case NPC_INSTRUCTOR_KLITHAK:
            GetCreatureListWithEntryInGrid(Studients, InstructorType, NPC_SETTHIK_GALESLICER, VISIBLE_RANGE);
            break;
        case NPC_INSTRUCTOR_ZARIK:
            GetCreatureListWithEntryInGrid(Studients, InstructorType, NPC_SETTHIK_TEMPEST, VISIBLE_RANGE);
            break;
        case NPC_INSTRUCTOR_TAKTHOK:
            GetCreatureListWithEntryInGrid(Studients, InstructorType, NPC_KORTHIK_SWARMER, VISIBLE_RANGE);
            break;
    }

    for (auto&& itr : Studients)
    {
        if (Unit* InstructorTarget = InstructorType->GetVictim())
        {
            itr->SetInCombatWithZone();
            itr->AI()->AttackStart(InstructorTarget);
        }
        else if (Player* jitr = itr->FindNearestPlayer(VISIBLE_RANGE))
        {
            itr->SetInCombatWithZone();
            itr->AI()->AttackStart(jitr);
        }
    }
}

class npc_instructor_kli_thak : public CreatureScript
{
    public:
        npc_instructor_kli_thak() : CreatureScript("npc_instructor_kli_thak") { }

        struct npc_instructor_kli_thakAI : public ScriptedAI
        {
            npc_instructor_kli_thakAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;
            uint32 delay;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                Reset();
                delay = 0;
            }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_WIND_STEP, 7000);
                events.ScheduleEvent(EVENT_WIND_STEP_2, 15000);

                CallForHelpMyStudients(me, me->GetGUID());
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* tayak = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_TAYAK) : ObjectGuid::Empty))
                    tayak->ToCreature()->AI()->DoAction(ACTION_TAYAK_TALK_TRASH);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_INSTRUCTORS_MOVE)
                {
                    Movement::MoveSplineInit init(me);
                    for (uint8 i = 0; i < 3; ++i)
                        init.Path().push_back(G3D::Vector3(KlithakMaltikWaypoints[i].GetPositionX() + frand(-10.0f, 10.0f), KlithakMaltikWaypoints[i].GetPositionY() + frand(-10.0f, 10.0f), KlithakMaltikWaypoints[i].GetPositionZ()));

                    init.Path().push_back(G3D::Vector3(KlithakMaltikWaypoints[3].GetPositionX(), KlithakMaltikWaypoints[3].GetPositionY(), KlithakMaltikWaypoints[3].GetPositionZ()));
                    init.SetUncompressed();
                    init.Launch();

                    delay = 0;
                    me->m_Events.Schedule(delay += me->GetSplineDuration(), 5, [this]()
                    {
                        me->StopMoving();
                        me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                    });
                }
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
                        case EVENT_WIND_STEP:
                            if (Unit* target = me->GetVictim())
                                me->AddAura(SPELL_WIND_STEP, target);

                            events.ScheduleEvent(EVENT_WIND_STEP, urand(18000,29000));
                            break;
                        case EVENT_WIND_STEP_2:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                me->CastSpell(target, SPELL_WIND_STEP_2, true);

                            events.ScheduleEvent(EVENT_WIND_STEP_2, 16 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_instructor_kli_thakAI(creature);
        }
};

class npc_instructor_tak_thok : public CreatureScript
{
    public:
        npc_instructor_tak_thok() : CreatureScript("npc_instructor_tak_thok") { }

        struct npc_instructor_tak_thokAI : public ScriptedAI
        {
            npc_instructor_tak_thokAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;
            uint32 delay;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                delay = 0;
                Reset();
            }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_OVERWHELMING_ASSAULT, 4000);

                CallForHelpMyStudients(me, me->GetGUID());
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* tayak = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_TAYAK) : ObjectGuid::Empty))
                    tayak->ToCreature()->AI()->DoAction(ACTION_TAYAK_TALK_TRASH);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_INSTRUCTORS_MOVE)
                {
                    Movement::MoveSplineInit init(me);
                    for (uint8 i = 0; i < 3; ++i)
                        init.Path().push_back(G3D::Vector3(TakthokZarikWaypoints[i].GetPositionX() + frand(-10.0f, 10.0f), TakthokZarikWaypoints[i].GetPositionY() + frand(-10.0f, 10.0f), TakthokZarikWaypoints[i].GetPositionZ()));

                    init.Path().push_back(G3D::Vector3(TakthokZarikWaypoints[4].GetPositionX(), TakthokZarikWaypoints[4].GetPositionY(), TakthokZarikWaypoints[4].GetPositionZ()));
                    init.SetUncompressed();
                    init.Launch();

                    delay = 0;
                    me->m_Events.Schedule(delay += me->GetSplineDuration(), 5, [this]()
                    {
                        me->StopMoving();
                        me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                    });
                }
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
                    if (eventId == EVENT_OVERWHELMING_ASSAULT)
                    {
                        if (Unit* target = me->GetVictim())
                            me->CastSpell(target, SPELL_OVERWHELMING_ASSAULT, true);

                        events.ScheduleEvent(EVENT_OVERWHELMING_ASSAULT, 22000);
                        break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_instructor_tak_thokAI(creature);
        }
};

class npc_instructor_maltik : public CreatureScript
{
    public:
        npc_instructor_maltik() : CreatureScript("npc_instructor_maltik") { }

        struct npc_instructor_maltikAI : public ScriptedAI
        {
            npc_instructor_maltikAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            ObjectGuid unseenTank, unseenTarget;
            bool unseenReturn, evadeModeEnabled;
            uint32 delay;
            InstanceScript* instance;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                delay = 0;
                Reset();
            }

            void Reset() override
            {
                events.Reset();
                unseenReturn     = false;
                evadeModeEnabled = false;
                unseenTank       = ObjectGuid::Empty;
                unseenTarget     = ObjectGuid::Empty;
                me->setRegeneratingHealth(true);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->setRegeneratingHealth(false);
                events.ScheduleEvent(EVENT_UNSEEN_STRIKE, urand(18500, 21500));
                evadeModeEnabled = true;
                CallForHelpMyStudients(me, me->GetGUID());
            }

            void EnterEvadeMode() override
            {
                me->GetMotionMaster()->MoveTargetedHome();
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* tayak = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_TAYAK) : ObjectGuid::Empty))
                    tayak->ToCreature()->AI()->DoAction(ACTION_TAYAK_TALK_TRASH);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_INSTRUCTORS_MOVE)
                {
                    Movement::MoveSplineInit init(me);
                    for (uint8 i = 0; i < 3; ++i)
                        init.Path().push_back(G3D::Vector3(KlithakMaltikWaypoints[i].GetPositionX() + frand(-10.0f, 10.0f), KlithakMaltikWaypoints[i].GetPositionY() + frand(-10.0f, 10.0f), KlithakMaltikWaypoints[i].GetPositionZ()));

                    init.Path().push_back(G3D::Vector3(KlithakMaltikWaypoints[4].GetPositionX(), KlithakMaltikWaypoints[4].GetPositionY(), KlithakMaltikWaypoints[4].GetPositionZ()));
                    init.SetUncompressed();
                    init.Launch();

                    delay = 0;
                    me->m_Events.Schedule(delay += me->GetSplineDuration(), 5, [this]()
                    {
                        me->StopMoving();
                        me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                    });
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (evadeModeEnabled && instance && instance->IsWipe()) // handle evade cuz we can`t use default [hello unseen strike]
                {
                    me->RemoveAllAuras();
                    EnterEvadeMode();
                }

                if (!UpdateVictim() && !unseenReturn)
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_UNSEEN_STRIKE:
                        {
                            unseenTank = me->GetVictim() ? me->GetVictim()->GetGUID() : ObjectGuid::Empty;
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                            {
                                me->CastSpell(target, SPELL_UNSEEN_STRIKE_TR, true);
                                me->AddAura(SPELL_UNSEEN_STRIKE_MKR, target);
                                unseenTarget = target->GetGUID();
                                me->SetReactState(REACT_PASSIVE);
                                unseenReturn = true;
                            }
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                            {
                                me->CastSpell(target, SPELL_UNSEEN_STRIKE_TR, true);
                                me->AddAura(SPELL_UNSEEN_STRIKE_MKR, target);
                                unseenTarget = target->GetGUID();
                                me->SetReactState(REACT_PASSIVE);
                                unseenReturn = true;
                            }
                            events.ScheduleEvent(EVENT_UNSEEN_STRIKE_CHASE, 100);
                            events.ScheduleEvent(EVENT_UNSEEN_STRIKE_PREPARE, 4.9 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_UNSEEN_STRIKE_RETURN, 5500);
                            events.ScheduleEvent(EVENT_UNSEEN_STRIKE, urand(53000, 61000));
                            break;
                        }
                        case EVENT_UNSEEN_STRIKE_CHASE:
                        {
                            if (Unit* target = ObjectAccessor::GetUnit(*me, unseenTarget))
                                me->GetMotionMaster()->MoveChase(target);

                            events.ScheduleEvent(EVENT_UNSEEN_STRIKE_CHASE, urand(100, 200));
                            break;
                        }
                        case EVENT_UNSEEN_STRIKE_PREPARE:
                        {
                            events.CancelEvent(EVENT_UNSEEN_STRIKE_CHASE);

                            if (Unit* target = ObjectAccessor::GetUnit(*me, unseenTarget))
                                me->PrepareChanneledCast(me->GetAngle(target));
                            break;
                        }
                        case EVENT_UNSEEN_STRIKE_RETURN:
                        {
                            me->RemoveChanneledCast(unseenTank);
                            events.CancelEvent(EVENT_UNSEEN_STRIKE_CHASE);
                            break;
                        }
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_instructor_maltikAI(creature);
        }
};

class npc_instructor_zarik : public CreatureScript
{
    public:
        npc_instructor_zarik() : CreatureScript("npc_instructor_zarik") { }

        struct npc_instructor_zarikAI : public ScriptedAI
        {
            npc_instructor_zarikAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            EventMap events;
            InstanceScript* instance;
            SummonList summons;
            uint32 delay;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                Reset();
                delay = 0;
            }

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_TEMPEST_SLASH, urand(9500, 11000));

                CallForHelpMyStudients(me, me->GetGUID());
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* tayak = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_TAYAK) : ObjectGuid::Empty))
                    tayak->ToCreature()->AI()->DoAction(ACTION_TAYAK_TALK_TRASH);

                summons.DespawnAll();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_INSTRUCTORS_MOVE)
                {
                    Movement::MoveSplineInit init(me);
                    for (uint8 i = 0; i < 3; ++i)
                        init.Path().push_back(G3D::Vector3(TakthokZarikWaypoints[i].GetPositionX() + frand(-10.0f, 10.0f), TakthokZarikWaypoints[i].GetPositionY() + frand(-10.0f, 10.0f), TakthokZarikWaypoints[i].GetPositionZ()));

                    init.Path().push_back(G3D::Vector3(TakthokZarikWaypoints[3].GetPositionX(), TakthokZarikWaypoints[3].GetPositionY(), TakthokZarikWaypoints[3].GetPositionZ()));
                    init.SetUncompressed();
                    init.Launch();

                    delay = 0;
                    me->m_Events.Schedule(delay += me->GetSplineDuration(), 5, [this]()
                    {
                        me->StopMoving();
                        me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                    });
                }
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
                    if (eventId == EVENT_TEMPEST_SLASH)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true))
                            me->CastSpell(target, SPELL_TEMP_SLASH_SUMM_V, false);

                        events.ScheduleEvent(EVENT_TEMPEST_SLASH, urand(14500, 16500));
                        break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_instructor_zarikAI(creature);
        }
};

class npc_zar_thik_zealot : public CreatureScript
{
    public:
        npc_zar_thik_zealot() : CreatureScript("npc_zar_thik_zealot") { }

        struct npc_zar_thik_zealotAI : public ScriptedAI
        {
            npc_zar_thik_zealotAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                if (IsHeroic())
                    events.ScheduleEvent(EVENT_SLOW, 3000);

                events.ScheduleEvent(EVENT_ZEALOUS_RUSH, 7000);
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
                        case EVENT_ZEALOUS_RUSH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                me->CastSpell(target, SPELL_ZEALOUS_RUSH, true);

                            events.ScheduleEvent(EVENT_ZEALOUS_RUSH, 22000);
                            break;
                        case EVENT_SLOW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                me->CastSpell(target, SPELL_SLOW, true);

                            events.ScheduleEvent(EVENT_SLOW, 40000);
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
            return new npc_zar_thik_zealotAI(creature);
        }
};

class npc_kor_thik_swarmer : public CreatureScript
{
    public:
        npc_kor_thik_swarmer() : CreatureScript("npc_kor_thik_swarmer") { }

        struct npc_kor_thik_swarmerAI : public ScriptedAI
        {
            npc_kor_thik_swarmerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_UNDERWHELMING_ASSAULT, urand(4 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
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
                    if (eventId == EVENT_UNDERWHELMING_ASSAULT)
                    {
                        if (Unit* target = me->GetVictim())
                            me->CastSpell(target, SPELL_UNDERWHELMING_ASSAULT, true);

                        events.ScheduleEvent(EVENT_UNDERWHELMING_ASSAULT, urand(8 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                        break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_kor_thik_swarmerAI(creature);
        }
};

// Set`huj Gustwing 63592
class npc_set_thik_gustwing : public CreatureScript
{
    public:
        npc_set_thik_gustwing() : CreatureScript("npc_set_thik_gustwing") { }

        struct npc_set_thik_gustwingAI : public ScriptedAI
        {
            npc_set_thik_gustwingAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_GUST, urand(3 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
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
                    if (eventId == EVENT_GUST)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                            me->CastSpell(target, SPELL_GUST, true);

                        events.ScheduleEvent(EVENT_GUST, urand(7.5 * IN_MILLISECONDS, 32 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_set_thik_gustwingAI(creature);
        }
};

// Zephyr 63599
struct npc_zephyr_tornado : public ScriptedAI
{
    npc_zephyr_tornado(Creature* creature) : ScriptedAI(creature) { }

    float x, y;
    bool m_bCounterClockwise, m_bFirstPoint;

    void IsSummonedBy(Unit* summoner) override
    {
        m_bCounterClockwise = urand(0, 1);
        m_bFirstPoint = false;
        x = me->GetPositionX();
        y = me->GetPositionY();

        Movement::MoveSplineInit init(me);
        for (uint8 i = 1; i < 13; ++i)
        {
            float newX = x + 2.5f * cos(i * M_PI / 6);
            float newY = m_bCounterClockwise ? y - 2.5f * sin(i * M_PI / 6) : y + 2.5f * sin(i * M_PI / 6);
            init.Path().push_back(G3D::Vector3(newX, newY, me->GetPositionZ()));
        }

        init.SetUncompressed();
        init.SetCyclic();
        init.Launch();
    }

    void EnterEvadeMode() override { }
};

// Kaz`rik 64826
class npc_heart_of_fear_kazrik : public CreatureScript
{
    public:
        npc_heart_of_fear_kazrik() : CreatureScript("npc_heart_of_fear_kazrik") { }
    
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

            if(sConfigMgr->GetIntDefault("FirstOfTheKingdom.HeartOfFear", 1))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Turn off dampening?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }
    
        struct npc_heart_of_fear_kazrikAI : public ScriptedAI
        {
            npc_heart_of_fear_kazrikAI(Creature* creature) : ScriptedAI(creature) { }
    
            void InitializeAI() override
            {
                if (me->GetMap()->GetDifficulty() != RAID_DIFFICULTY_25MAN_LFR)
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_heart_of_fear_kazrikAI(creature);
        }
};

class npc_coagulated_amber : public CreatureScript
{
    public:
        npc_coagulated_amber() : CreatureScript("npc_coagulated_amber") { }

        struct npc_coagulated_amberAI : public ScriptedAI
        {
            npc_coagulated_amberAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events, nonCombatEvents;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->GetMotionMaster()->Clear();
                events.ScheduleEvent(EVENT_BURST, 4000);
            }

            ObjectGuid NextAmberPoolStalkerPath()
            {
                std::list<Creature*> AmberPoolStalker;
                GetCreatureListWithEntryInGrid(AmberPoolStalker, me, NPC_AMBER_POOL_STALKER, 200.0f);

                if (AmberPoolStalker.empty())
                    return ObjectGuid::Empty;

                if (Creature* m_stalker = Trinity::Containers::SelectRandomContainerElement(AmberPoolStalker))
                    return m_stalker->GetGUID();

                return ObjectGuid::Empty;
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_MISC)
                    nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 500);
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_ESCORT)
                    {
                        if (Unit* m_Pool = ObjectAccessor::GetUnit(*me, NextAmberPoolStalkerPath()))
                        {
                            Movement::MoveSplineInit init(me);
                            init.MoveTo(m_Pool->GetPositionX(), m_Pool->GetPositionY(), m_Pool->GetPositionZ());
                            init.SetVelocity(4.5f);
                            init.SetSmooth();
                            init.SetUncompressed();
                            init.Launch();

                            me->DespawnOrUnsummon(me->GetSplineDuration());
                        }
                    }
                    break;
                }

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_BURST)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(target, SPELL_BURST, true);

                        events.ScheduleEvent(EVENT_BURST, 22000);
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_coagulated_amberAI(creature);
        }
};

// Kor`thik swarmguard 64916
class npc_korthik_swarmguard : public CreatureScript
{
    public:
        npc_korthik_swarmguard() : CreatureScript("npc_korthik_swarmguard") { }

        struct npc_korthik_swarmguardAI : public ScriptedAI
        {
            npc_korthik_swarmguardAI(Creature* creature) : ScriptedAI(creature) { }
            EventMap events;
            ObjectGuid charmerByGUID;

            void InitializeAI() override
            {
                SetEquipmentSlots(false, MELJARAK_2H_WEAPON, 0, EQUIP_NO_CHANGE);
            }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_CARAPASE, urand(10000, 19000));
                events.ScheduleEvent(EVENT_SWARMGUARDS_AEGIS, urand(9000, 19000));
                events.ScheduleEvent(EVENT_SEPARATION_ANXIETY, 500);

                if (Unit* GetCharmerBy = GetClosestCreatureWithEntry(me, NPC_SRATHIK_AMBERCALLER, 50.0f, true))
                    charmerByGUID = GetCharmerBy->GetGUID();
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
                        case EVENT_CARAPASE:
                            DoCast(me, SPELL_CARAPASE);
                            events.ScheduleEvent(EVENT_CARAPASE, urand(19000, 29000));
                            break;
                        case EVENT_SWARMGUARDS_AEGIS:
                            if (Unit* charmer = ObjectAccessor::GetUnit(*me, charmerByGUID))
                                DoCast(charmer, SPELL_SWARMGUARDS_AEGIS);
                            break;
                        case EVENT_SEPARATION_ANXIETY:
                            if (Unit* charmer = ObjectAccessor::GetUnit(*me, charmerByGUID))
                            {
                                if (me->GetExactDist2d(charmer) >= 15)
                                    me->CastSpell(me, SPELL_SEPARATION_ANXIETY, false);
                                else if (me->HasAura(SPELL_SEPARATION_ANXIETY))
                                    me->RemoveAura(SPELL_SEPARATION_ANXIETY);

                                events.ScheduleEvent(EVENT_SEPARATION_ANXIETY, urand(500, 1000));
                            }
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_korthik_swarmguardAI(creature);
        }
};

// Sra`thik AmberCaller 64917
class npc_srathik_ambercaller : public CreatureScript
{
    public:
        npc_srathik_ambercaller() : CreatureScript("npc_srathik_ambercaller") { }

        struct npc_srathik_ambercallerAI : public ScriptedAI
        {
            npc_srathik_ambercallerAI(Creature* creature) : ScriptedAI(creature) { }
            EventMap events;

            void InitializeAI() override
            {
                SetEquipmentSlots(false, ZARTHIK_2H_WEAPON, 0, EQUIP_NO_CHANGE);
            }

            void JustEngagedWith(Unit* who) override
            {
                me->SetReactState(REACT_PASSIVE);
                me->AttackStop();
                me->CastSpell(me, SPELL_AMBER_VOLLEY, false);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_srathik_ambercallerAI(creature);
        }
};

// Kor`thik fleshrender 64902
class npc_korthik_fleshrender : public CreatureScript
{
    public:
        npc_korthik_fleshrender() : CreatureScript("npc_korthik_fleshrender") { }

        struct npc_korthik_fleshrenderAI : public ScriptedAI
        {
            npc_korthik_fleshrenderAI(Creature* creature) : ScriptedAI(creature) { }
            EventMap events;

            void InitializeAI() override
            {
                SetEquipmentSlots(false, MELJARAK_2H_WEAPON, 0, EQUIP_NO_CHANGE);
            }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_GRIEVOUS_WHIRL, urand(12000, 18000));
                events.ScheduleEvent(EVENT_MORTAL_REND, urand(5000, 9000));
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
                        case EVENT_GRIEVOUS_WHIRL:
                            DoCast(me, SPELL_GRIEVOUS_WHIRL);
                            events.ScheduleEvent(EVENT_GRIEVOUS_WHIRL, urand(19000, 27000));
                            break;
                        case EVENT_MORTAL_REND:
                            if (Unit* Victim = me->GetVictim())
                                me->AddAura(SPELL_MORTAL_REND, Victim);

                            events.ScheduleEvent(EVENT_MORTAL_REND, urand(5000, 9000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_korthik_fleshrenderAI(creature);
        }
};

// Sra`thik Pool-Tender 63570
class npc_srathik_pool_tender : public CreatureScript
{
    public:
        npc_srathik_pool_tender() : CreatureScript("npc_srathik_pool_tender") { }

        struct npc_srathik_pool_tenderAI : public ScriptedAI
        {
            npc_srathik_pool_tenderAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void InitializeAI() override
            {
                SetEquipmentSlots(false, ZARTHIK_2H_WEAPON, 0, EQUIP_NO_CHANGE);
                Reset();
            }

            void Reset() override
            {
                uint32 delay = 0;
                me->m_Events.Schedule(delay += 2000, 20, [this]()
                {
                    if (Creature* AmberPool = GetClosestCreatureWithEntry(me, NPC_AMBER_POOL_STALKER, 50.0f))
                        me->CastSpell(AmberPool, SPELL_AGITATE_POOL, false);
                });
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!me->HasAura(SPELL_AMBER_EMANATION))
                    me->AddAura(SPELL_AMBER_EMANATION, me);

                events.ScheduleEvent(EVENT_AMBER_INFUSION, urand(10000, 19000));
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
                    if (eventId == EVENT_AMBER_INFUSION)
                    {
                        if (Unit* PoolTender = GetClosestCreatureWithEntry(me, NPC_SRATHIK_POOL_TENDER, 50.0f, true))
                            me->CastSpell(PoolTender, SPELL_AMBER_INFUSION, false);

                        events.ScheduleEvent(EVENT_AMBER_INFUSION, urand(10000, 19000));
                    }
                }

                DoMeleeAttackIfReady();

            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_srathik_pool_tenderAI(creature);
        }
};

// Amber-Ridden Mushan 63568
class npc_amber_ridden_mushan : public CreatureScript
{
    public:
        npc_amber_ridden_mushan() : CreatureScript("npc_amber_ridden_mushan") { }

        struct npc_amber_ridden_mushanAI : public ScriptedAI
        {
            npc_amber_ridden_mushanAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            std::list<ObjectGuid> amber;
            uint32 VehPos;

            void Reset() override
            {
                RemoveAmberGrowth();
                VehPos = 0;
            }

            void InitializeAI() override
            {
                VehPos = 0;
            }

            void RemoveAmberGrowth()
            {
                for (auto&& itr : amber)
                    if (Creature* AmberGrowth = ObjectAccessor::GetCreature(*me, itr))
                        AmberGrowth->DespawnOrUnsummon();

                amber.clear();
            }

            bool HasAmberGrowthKilled()
            {
                for (auto&& itr : amber)
                    if (Creature* AmberGrowth = ObjectAccessor::GetCreature(*me, itr))
                        if (!AmberGrowth->IsAlive()) // we need check once
                            return true;

                return false;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Position pos = me->GetRandomNearPosition(4.0f);

                for (uint8 i = 0; i < 4; i++)
                {
                    if (Creature* AmberGrowth = me->SummonCreature(NPC_AMBER_GROWTH, pos, TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        AmberGrowth->EnterVehicle(me, VehPos);
                        AmberGrowth->SetReactState(REACT_PASSIVE);
                        AmberGrowth->AddUnitState(UNIT_STATE_CANNOT_AUTOATTACK);

                        amber.push_back(AmberGrowth->GetGUID());
                        VehPos++;
                    }
                }

                events.ScheduleEvent(EVENT_SLAM, 8 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_AMBER_SPEW, urand(6 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
            }

            void JustDied(Unit* /*killer*/) override
            {
                RemoveAmberGrowth();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (HasAmberGrowthKilled()) // if any amber growth were killed then we should kill our creature
                    me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_AMBER_SPEW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                me->CastSpell(target, SPELL_AMBER_SPEW, false);
                            else if (Unit* target=me->GetVictim())
                                me->CastSpell(target, SPELL_AMBER_SPEW, false);

                            events.ScheduleEvent(EVENT_AMBER_SPEW, urand(16 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
                            break;
                        case EVENT_SLAM:
                            DoCast(me, SPELL_SLAM);
                            events.ScheduleEvent(EVENT_SLAM, 16 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_amber_ridden_mushanAI(creature);
        }
};

// Amber Searstring 63569
class npc_amber_searsting : public CreatureScript
{
    public:
        npc_amber_searsting() : CreatureScript("npc_amber_searsting") { }

        struct npc_amber_searstingAI : public ScriptedAI
        {
            npc_amber_searstingAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            ObjectGuid targetGUID;

            void Reset() override
            {
                events.Reset();
                targetGUID = ObjectGuid::Empty;
            }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_BURNING_STING, urand(8000, 16000));
                events.ScheduleEvent(EVENT_SEARING_SLASH, urand(5000, 9000));
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
                        case EVENT_SEARING_SLASH:
                            if (Unit* target = me->GetVictim())
                            {
                                targetGUID = target->GetGUID();
                                me->PrepareChanneledCast(me->GetAngle(target), SPELL_SEARING_SLASH);
                            }
                            events.ScheduleEvent(EVENT_SEARING_SLASH, urand(5000, 9000));
                            events.ScheduleEvent(EVENT_SEARING_SLASH_REMOVE, 2 * IN_MILLISECONDS);
                            break;
                        case EVENT_BURNING_STING:
                            if (Unit* Victim = me->GetVictim())
                                me->CastSpell(Victim, SPELL_BURNING_STING, false);

                            events.ScheduleEvent(EVENT_BURNING_STING, urand(16000, 22000));
                            break;
                        case EVENT_SEARING_SLASH_REMOVE:
                            me->RemoveChanneledCast(targetGUID);
                            break;
                    }
                }

                DoMeleeAttackIfReady();

            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_amber_searstingAI(creature);
        }
};

// Kor`thik Warsinger 64458
class npc_korthik_warsinger : public CreatureScript
{
    public:
        npc_korthik_warsinger() : CreatureScript("npc_korthik_warsinger") { }

        struct npc_korthik_warsingerAI : public ScriptedAI
        {
            npc_korthik_warsingerAI(Creature* creature) : ScriptedAI(creature) { }
            EventMap events;

            void InitializeAI() override { SetEquipmentSlots(false, MELJARAK_2H_WEAPON, 0, EQUIP_NO_CHANGE); }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_FRENZIED_ASSAULT, urand(8000, 16000));
                events.ScheduleEvent(EVENT_CRY_HAVOC, urand(5000, 9000));
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
                        case EVENT_CRY_HAVOC:
                            DoCast(me, SPELL_CRY_HAVOC);
                            events.ScheduleEvent(EVENT_CRY_HAVOC, urand(15000, 20000));
                            break;
                        case EVENT_FRENZIED_ASSAULT:
                            if (Unit* Victim = me->GetVictim())
                                me->CastSpell(Victim, SPELL_FRENZIED_ASSAULT, false);

                            events.ScheduleEvent(EVENT_FRENZIED_ASSAULT, urand(14000, 22000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();

            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_korthik_warsingerAI(creature);
        }
};

// Zar`thik Augurer 64454
class npc_zarthik_augurer : public CreatureScript
{
    public:
        npc_zarthik_augurer() : CreatureScript("npc_zarthik_augurer") { }

        struct npc_zarthik_augurerAI : public ScriptedAI
        {
            npc_zarthik_augurerAI(Creature* creature) : ScriptedAI(creature) { }
            EventMap events;

            void InitializeAI() override
            {
                SetEquipmentSlots(false, ZARTHIK_2H_WEAPON, 0, EQUIP_NO_CHANGE);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_TOXIC_HIVEBOMB, urand(8000, 16000));
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
                    if (eventId == EVENT_TOXIC_HIVEBOMB)
                    {
                        if (Unit* Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                            me->CastSpell(Target, SPELL_TOXIC_HIVEBOMB, false);

                        events.ScheduleEvent(EVENT_TOXIC_HIVEBOMB, urand(8000, 16000));
                    }
                }

                DoMeleeAttackIfReady();

            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zarthik_augurerAI(creature);
        }
};

// Searing Slash 125480
class spell_searing_slash : public SpellScript
{
    PrepareSpellScript(spell_searing_slash);

    uint8 targetsCount;

    bool Load() override
    {
        targetsCount = 1;
        return true;
    }

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        targetsCount = targets.size();
    }

    void RecalculateDamage(SpellEffIndex /*effIndex*/)
    {
        if (targetsCount > 0)
            SetHitDamage(GetHitDamage() / targetsCount);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_searing_slash::CheckTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
        OnEffectHitTarget += SpellEffectFn(spell_searing_slash::RecalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Amber Volley 126938
class spell_amber_volley : public SpellScript
{
    PrepareSpellScript(spell_amber_volley);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_AMBER_VOLLEY_MISSLE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_amber_volley::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

void AddSC_heart_of_fear()
{
    new npc_wind_lord_meljarak_intro();
    new npc_kor_thik_slicer();
    new npc_kor_thik_extremist();
    new npc_sra_thik_shield_master();
    new npc_set_thik_swiftblade();
    new npc_zar_thik_supplicant();
    new npc_enslaved_bonesmasher();
    new npc_set_thik_tempest();
    new npc_set_thik_fanatic();
    new npc_set_thik_zephyrian();
    new npc_set_thik_gale_slicer();
    new npc_instructor_kli_thak();
    new npc_instructor_tak_thok();
    new npc_instructor_maltik();
    new npc_instructor_zarik();
    new npc_zar_thik_zealot();
    new npc_kor_thik_swarmer();
    new npc_set_thik_gustwing();
    new npc_coagulated_amber();
    new npc_korthik_swarmguard();
    new npc_srathik_ambercaller();
    new npc_korthik_fleshrender();
    new npc_srathik_pool_tender();
    new npc_amber_ridden_mushan();
    new npc_amber_searsting();
    new npc_korthik_warsinger();
    new npc_zarthik_augurer();
    new creature_script<npc_zephyr_tornado>("npc_zephyr_tornado");
    new npc_heart_of_fear_kazrik();
    new spell_script<spell_searing_slash>("spell_searing_slash");
    new spell_script<spell_amber_volley>("spell_amber_volley");
}
