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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"
#include "icecrown_citadel.h"

enum Texts
{
    SAY_AGGRO                   = 0,
    SAY_VAMPIRIC_BITE           = 1,
    SAY_MIND_CONTROL            = 2,
    EMOTE_BLOODTHIRST           = 3,
    SAY_SWARMING_SHADOWS        = 4,
    EMOTE_SWARMING_SHADOWS      = 5,
    SAY_PACT_OF_THE_DARKFALLEN  = 6,
    SAY_AIR_PHASE               = 7,
    SAY_KILL                    = 8,
    SAY_WIPE                    = 9,
    SAY_BERSERK                 = 10,
    SAY_DEATH                   = 11,
    EMOTE_BERSERK_RAID          = 12
};

enum Spells
{
    SPELL_SHROUD_OF_SORROW                  = 70986,
    SPELL_FRENZIED_BLOODTHIRST_VISUAL       = 71949,
    SPELL_VAMPIRIC_BITE                     = 71726,
    SPELL_VAMPIRIC_BITE_PLR                 = 70946,
    SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_PLR    = 70879,
    SPELL_FRENZIED_BLOODTHIRST              = 70877,
    SPELL_UNCONTROLLABLE_FRENZY             = 70923,
    SPELL_UNCONTROLLABLE_FRENZY_1           = 70924,
    SPELL_UNCONTROLLABLE_FRENZY_2           = 73015,
    SPELL_PRESENCE_OF_THE_DARKFALLEN        = 71952,
    SPELL_PRESENCE_OF_THE_DARKFALLEN_TRIG   = 70995,
    SPELL_BLOOD_MIRROR_DAMAGE               = 70821,
    SPELL_BLOOD_MIRROR_VISUAL               = 71510,
    SPELL_BLOOD_MIRROR_DUMMY                = 70838,
    SPELL_DELIRIOUS_SLASH                   = 71623,
    SPELL_PACT_OF_THE_DARKFALLEN_TARGET     = 71336,
    SPELL_PACT_OF_THE_DARKFALLEN            = 71340,
    SPELL_PACT_OF_THE_DARKFALLEN_DAMAGE     = 71341,
    SPELL_SWARMING_SHADOWS                  = 71264,
    SPELL_TWILIGHT_BLOODBOLT_TARGET         = 71445,
    SPELL_TWILIGHT_BLOODBOLT                = 71446,
    SPELL_INCITE_TERROR                     = 73070,
    SPELL_BLOODBOLT_WHIRL                   = 71772,
    SPELL_ANNIHILATE                        = 71322,

    SPELL_BLOOD_BEAM_VISUAL_RLEG            = 72301,
    SPELL_BLOOD_BEAM_VISUAL_LLEG            = 72302,
    SPELL_BLOOD_BEAM_VISUAL_LHAND           = 72303,
    SPELL_BLOOD_BEAM_VISUAL_RHAND           = 72304,

    // Misc
    SPELL_BLOOD_INFUSION_CREDIT             = 72934,
};

enum Shadowmourne
{
    QUEST_BLOOD_INFUSION                    = 24756,
    ITEM_SHADOW_S_EDGE                      = 49888,

    SPELL_GUSHING_WOUND                     = 72132,
    SPELL_THIRST_QUENCHED                   = 72154,
};

uint32 const vampireAuras[3] = { 70867, 70879, 70877 };

enum VampExploit
{
     SPELL_VAMPIRIC0_10N = 70867,
     SPELL_VAMPIRIC1_10N = 70871,
     SPELL_VAMPIRIC2_10N = 70879,
     SPELL_VAMPIRIC0_10H = 70877,
     SPELL_VAMPIRIC1_10H = 71265,
     SPELL_VAMPIRIC2_10H = 72649,
     SPELL_VAMPIRIC3_10H = 72985,
     SPELL_VAMPIRIC5_10H = 72151,
};

enum Events
{
    EVENT_BERSERK                   = 1,
    EVENT_VAMPIRIC_BITE             = 2,
    EVENT_BLOOD_MIRROR              = 3,
    EVENT_DELIRIOUS_SLASH           = 4,
    EVENT_PACT_OF_THE_DARKFALLEN    = 5,
    EVENT_SWARMING_SHADOWS          = 6,
    EVENT_TWILIGHT_BLOODBOLT        = 7,
    EVENT_AIR_PHASE                 = 8,
    EVENT_AIR_START_FLYING          = 9,
    EVENT_AIR_FLY_DOWN              = 10,

    EVENT_GROUP_NORMAL              = 1,
    EVENT_GROUP_CANCELLABLE         = 2,
};

enum Guids
{
    GUID_VAMPIRE    = 1,
    GUID_BLOODBOLT  = 2,
};

enum Points
{
    POINT_CENTER    = 1,
    POINT_AIR       = 2,
    POINT_GROUND    = 3,
    POINT_MINCHAR   = 4,
};

Position const centerPos  = {4595.7090f, 2769.4190f, 400.6368f, 0.000000f};
Position const airPos     = {4595.7090f, 2769.4190f, 422.3893f, 0.000000f};
Position const mincharPos = {4629.3711f, 2782.6089f, 424.6390f, 0.000000f};

bool IsVampire(Unit const* unit)
{
    for (uint8 i = 0; i < 3; ++i)
        if (unit && unit->HasAura(vampireAuras[i]))
            return true;
    return false;
}

class boss_blood_queen_lana_thel : public CreatureScript
{
    public:
        boss_blood_queen_lana_thel() : CreatureScript("boss_blood_queen_lana_thel") { }

        struct boss_blood_queen_lana_thelAI : public BossAI
        {
            boss_blood_queen_lana_thelAI(Creature* creature) : BossAI(creature, DATA_BLOOD_QUEEN_LANA_THEL) { }

            void Cleanup()
            {
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VAMPIRIC0_10N); // ESSENCE_OF_BLOOD_QUEEN
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VAMPIRIC_BITE);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VAMPIRIC0_10N);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VAMPIRIC1_10N);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VAMPIRIC2_10N);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VAMPIRIC0_10H);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VAMPIRIC1_10H);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VAMPIRIC2_10H);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VAMPIRIC3_10H);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VAMPIRIC5_10H);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_PLR);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VAMPIRIC2_10N); // ESSENCE_OF_BLOOD_QUEEN_PLR
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VAMPIRIC0_10H); // FRENZIED_BLOODTHIRST
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNCONTROLLABLE_FRENZY);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOOD_MIRROR_DAMAGE);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOOD_MIRROR_VISUAL);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOOD_MIRROR_DUMMY);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DELIRIOUS_SLASH);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PACT_OF_THE_DARKFALLEN);
            }

            void Reset() override
            {
                _Reset();
                events.ScheduleEvent(EVENT_BERSERK, 330000);
                events.ScheduleEvent(EVENT_VAMPIRIC_BITE, 15000, EVENT_GROUP_NORMAL);
                events.ScheduleEvent(EVENT_BLOOD_MIRROR, 2500, EVENT_GROUP_CANCELLABLE);
                events.ScheduleEvent(EVENT_DELIRIOUS_SLASH, urand(20000, 24000), EVENT_GROUP_NORMAL);
                events.ScheduleEvent(EVENT_PACT_OF_THE_DARKFALLEN, 15000, EVENT_GROUP_NORMAL);
                events.ScheduleEvent(EVENT_SWARMING_SHADOWS, 30500, EVENT_GROUP_NORMAL);
                events.ScheduleEvent(EVENT_TWILIGHT_BLOODBOLT, urand(20000, 25000), EVENT_GROUP_NORMAL);
                events.ScheduleEvent(EVENT_AIR_PHASE, 124000 + uint32(Is25ManRaid() ? 3000 : 0));
                Cleanup();
                _offtankGUID = ObjectGuid::Empty;
                _victimGUID = ObjectGuid::Empty;
                _vampires.clear();
                _creditBloodQuickening = false;
                _killMinchar = false;

                // Achievement (condition controller in script)
                me->GetMap()->SetWorldState(WORLDSTATE_ONCE_BITTEN_10, 1);

                if (Is25ManRaid())
                    me->GetMap()->SetWorldState(WORLDSTATE_ONCE_BITTEN_25, 1);
            }

            void JustEngagedWith(Unit* who) override
            {
                DoCastAOE(SPELL_REMOVE_EMPOWERED_BLOOD, true);

                if (!instance->CheckRequiredBosses(DATA_BLOOD_QUEEN_LANA_THEL, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoCastSpellOnPlayers(LIGHT_S_HAMMER_TELEPORT);
                    return;
                }

                me->setActive(true);
                DoZoneInCombat();
                Talk(SAY_AGGRO);
                instance->SetBossState(DATA_BLOOD_QUEEN_LANA_THEL, IN_PROGRESS);
                Cleanup();
                DoCast(me, SPELL_SHROUD_OF_SORROW, true);
                DoCast(me, SPELL_FRENZIED_BLOODTHIRST_VISUAL, true);
                _creditBloodQuickening = instance->GetData(DATA_BLOOD_QUICKENING_STATE) == IN_PROGRESS;
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();
                Talk(SAY_DEATH);
                Cleanup();

                // Blah, credit the quest
                if (_creditBloodQuickening)
                {
                    instance->SetData(DATA_BLOOD_QUICKENING_STATE, DONE);
                    if (Creature* minchar = me->FindNearestCreature(NPC_INFILTRATOR_MINCHAR_BQ, 200.0f))
                        minchar->AI()->DoAction(0);
                }

                DoCast(me, SPELL_BLOOD_INFUSION_CREDIT);
            }

            void DoAction(int32 action) override
            {
                if (action != ACTION_KILL_MINCHAR)
                    return;

                if (instance->GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == IN_PROGRESS)
                    _killMinchar = true;
                else
                {
                    me->SetFlying(true);
                    me->OverrideInhabitType(INHABIT_AIR);
                    me->GetMotionMaster()->MovePoint(POINT_MINCHAR, mincharPos);
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                _EnterEvadeMode();
                Cleanup();
                if (_killMinchar)
                {
                    _killMinchar = false;
                    me->SetFlying(true);
                    me->OverrideInhabitType(INHABIT_AIR);
                    me->GetMotionMaster()->MovePoint(POINT_MINCHAR, mincharPos);
                }
                else
                {
                    me->GetMotionMaster()->MoveTargetedHome();
                    Reset();
                }
            }

            void JustReachedHome() override
            {
                me->SetFlying(false);
                me->ResetInhabitTypeOverride();
                me->SetReactState(REACT_AGGRESSIVE);
                _JustReachedHome();
                Talk(SAY_WIPE);
                instance->SetBossState(DATA_BLOOD_QUEEN_LANA_THEL, FAIL);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void SetGUID(ObjectGuid guid, int32 type = 0) override
            {
                switch (type)
                {
                    case GUID_VAMPIRE:
                        _vampires.insert(guid);
                        break;
                    case GUID_BLOODBOLT:
                        _bloodboltedPlayers.insert(guid);
                        break;
                    default:
                        break;
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case POINT_CENTER:
                        DoCast(me, SPELL_INCITE_TERROR);
                        events.ScheduleEvent(EVENT_AIR_PHASE, 100000 + uint32(Is25ManRaid() ? 0 : 20000));
                        events.RescheduleEvent(EVENT_SWARMING_SHADOWS, 30500, EVENT_GROUP_NORMAL);
                        events.RescheduleEvent(EVENT_PACT_OF_THE_DARKFALLEN, 25500, EVENT_GROUP_NORMAL);
                        events.ScheduleEvent(EVENT_AIR_START_FLYING, 5000);
                        break;
                    case POINT_AIR:
                        _bloodboltedPlayers.clear();
                        DoCast(me, SPELL_BLOODBOLT_WHIRL);
                        Talk(SAY_AIR_PHASE);
                        events.ScheduleEvent(EVENT_AIR_FLY_DOWN, 10000);
                        break;
                    case POINT_GROUND:
                        me->SetFlying(false);
                        me->ResetInhabitTypeOverride();
                        me->SetReactState(REACT_AGGRESSIVE);
                        if (Unit *victim = me->SelectVictim())
                            AttackStart(victim);
                        events.ScheduleEvent(EVENT_BLOOD_MIRROR, 2500, EVENT_GROUP_CANCELLABLE);
                        break;
                    case POINT_MINCHAR:
                        if (Creature* minchar = me->FindNearestCreature(NPC_INFILTRATOR_MINCHAR_BQ, 200.0f))
                            minchar->CastSpell(minchar, SPELL_ANNIHILATE, true);
                        // already in evade mode
                        me->GetMotionMaster()->MoveTargetedHome();
                        Reset();
                    default:
                        break;
                }
            }

            void MoveInLineOfSight(Unit* who) override
            {
                // Only aggro by proximity on players, flying pets and guardians can raise Kinetic Bombs into Lana'thel's room
                if (who->GetTypeId() == TYPEID_PLAYER)
                    BossAI::MoveInLineOfSight(who);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || !CheckInRoom())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BERSERK:
                            Talk(EMOTE_BERSERK_RAID);
                            Talk(SAY_BERSERK);
                            DoCast(me, SPELL_BERSERK);
                            break;
                        case EVENT_VAMPIRIC_BITE:
                        {
                            events.ScheduleEvent(EVENT_VAMPIRIC_BITE, 10000, EVENT_GROUP_NORMAL);
                            bool doNewTarget = true;

                            if (Map* map = me->GetMap())
                            {
                                if (map->IsDungeon())
                                {
                                    Map::PlayerList const &players = map->GetPlayers();

                                    if (!players.isEmpty())
                                    {
                                        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                                        {
                                            if (Player* player = itr->GetSource())
                                            {
                                                if (IsVampire(player))
                                                {
                                                    doNewTarget = false;
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            if (!doNewTarget)
                                break;

                            std::list<Player*> targets;
                            SelectRandomTarget(false, &targets);

                            Unit* target = nullptr;
                            if (!targets.empty())
                                target = targets.front();
                            else if (me->GetVictim() && !IsVampire(me->GetVictim()))
                                target = me->GetVictim();

                            if (target)
                            {
                                DoCast(target, SPELL_VAMPIRIC_BITE);
                                if (IsHeroic())
                                    me->AddAura(SPELL_PRESENCE_OF_THE_DARKFALLEN_TRIG, me);
                                Talk(SAY_VAMPIRIC_BITE);
                                _vampires.insert(target->GetGUID());
                            }
                            break;
                        }
                        case EVENT_BLOOD_MIRROR:
                        {
                            bool foundTargets = false;

                            // victim can be nullptr when this is processed in the same update tick as EVENT_AIR_PHASE
                            if (me->GetVictim())
                            {
                                if (Player* newOfftankPointer = SelectRandomTarget(true))
                                {
                                    foundTargets = true;

                                    // Either victim or offtank changed, recalculate the whole aura linking
                                    if (me->GetVictim()->GetGUID() != _victimGUID || newOfftankPointer->GetGUID() != _offtankGUID || !me->GetVictim()->HasAura(SPELL_BLOOD_MIRROR_DAMAGE) || !newOfftankPointer->HasAura(SPELL_BLOOD_MIRROR_DUMMY))
                                    {
                                        _victimGUID = me->GetVictim()->GetGUID();
                                        _offtankGUID = newOfftankPointer->GetGUID();

                                        if (instance)
                                        {
                                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOOD_MIRROR_DAMAGE);
                                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOOD_MIRROR_VISUAL);
                                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOOD_MIRROR_DUMMY);
                                        }

                                        // both spells have SPELL_ATTR5_SINGLE_TARGET_SPELL, no manual removal needed
                                        newOfftankPointer->CastSpell(me->GetVictim(), SPELL_BLOOD_MIRROR_DAMAGE, true);
                                        me->GetVictim()->CastSpell(newOfftankPointer, SPELL_BLOOD_MIRROR_DUMMY, true);
                                        DoCastVictim(SPELL_BLOOD_MIRROR_VISUAL);
                                        if (Item* shadowsEdge = newOfftankPointer->GetWeaponForAttack(BASE_ATTACK, true))
                                            if (!newOfftankPointer->HasAura(SPELL_THIRST_QUENCHED) && shadowsEdge->GetEntry() == ITEM_SHADOW_S_EDGE && !newOfftankPointer->HasAura(SPELL_GUSHING_WOUND) && newOfftankPointer->GetQuestStatus(QUEST_BLOOD_INFUSION) == QUEST_STATUS_INCOMPLETE)
                                                newOfftankPointer->CastSpell(newOfftankPointer, SPELL_GUSHING_WOUND, true);
                                    }
                                }
                            }

                            // Required targets not found, remove effects
                            if (!foundTargets)
                            {
                                if (instance)
                                {
                                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOOD_MIRROR_DAMAGE);
                                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOOD_MIRROR_VISUAL);
                                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOOD_MIRROR_DUMMY);
                                }
                            }

                            events.ScheduleEvent(EVENT_BLOOD_MIRROR, 2500, EVENT_GROUP_CANCELLABLE);
                            break;
                        }
                        case EVENT_DELIRIOUS_SLASH:
                            if (_offtankGUID)
                                if (Player* offtankPointer = Unit::GetPlayer(*me, _offtankGUID))
                                    if (offtankPointer && me->GetAnimTier() != AnimTier::Fly)
                                        DoCast(offtankPointer, SPELL_DELIRIOUS_SLASH);
                            events.ScheduleEvent(EVENT_DELIRIOUS_SLASH, urand(20000, 24000), EVENT_GROUP_NORMAL);
                            break;
                        case EVENT_PACT_OF_THE_DARKFALLEN:
                        {
                            std::list<Player*> targets;
                            SelectRandomTarget(false, &targets);
                            Trinity::Containers::RandomResizeList<Player*>(targets, RAID_MODE(2, 3, 2, 3));
                            if (targets.size() > 1)
                            {
                                Talk(SAY_PACT_OF_THE_DARKFALLEN);
                                for (std::list<Player*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                                    DoCast(*itr, SPELL_PACT_OF_THE_DARKFALLEN);
                            }
                            events.ScheduleEvent(EVENT_PACT_OF_THE_DARKFALLEN, 30500, EVENT_GROUP_NORMAL);
                            break;
                        }
                        case EVENT_SWARMING_SHADOWS:
                            if (Player* target = SelectRandomTarget(false))
                            {
                                Talk(EMOTE_SWARMING_SHADOWS, target);
                                Talk(SAY_SWARMING_SHADOWS);
                                DoCast(target, SPELL_SWARMING_SHADOWS);
                            }
                            events.ScheduleEvent(EVENT_SWARMING_SHADOWS, 30500, EVENT_GROUP_NORMAL);
                            break;
                        case EVENT_TWILIGHT_BLOODBOLT:
                        {
                            std::list<Player*> targets;
                            SelectRandomTarget(false, &targets);
                            uint32 requiredTargets = uint32(Is25ManRaid() ? 4 : 2);
                            uint32 meleeTargets = std::count_if(targets.begin(), targets.end(), BloodboltDistanceCheck(me));
                            if (meleeTargets && (targets.size() - meleeTargets) >= requiredTargets)
                                targets.remove_if(BloodboltDistanceCheck(me));
                            Trinity::Containers::RandomResizeList<Player*>(targets, requiredTargets);
                            for (std::list<Player*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                                DoCast(*itr, SPELL_TWILIGHT_BLOODBOLT);
                            DoCast(me, SPELL_TWILIGHT_BLOODBOLT_TARGET);
                            events.ScheduleEvent(EVENT_TWILIGHT_BLOODBOLT, urand(10000, 15000), EVENT_GROUP_NORMAL);
                            break;
                        }
                        case EVENT_AIR_PHASE:
                            DoStopAttack();
                            me->SetReactState(REACT_PASSIVE);
                            //events.DelayEvents(10000, EVENT_GROUP_NORMAL);
                            events.CancelEventGroup(EVENT_GROUP_CANCELLABLE);
                            me->GetMotionMaster()->MovePoint(POINT_CENTER, centerPos);
                            break;
                        case EVENT_AIR_START_FLYING:
                            events.DelayEvents(20000, EVENT_GROUP_NORMAL);
                            me->SetFlying(true);
                            me->OverrideInhabitType(INHABIT_AIR);
                            me->GetMotionMaster()->MoveTakeoff(POINT_AIR, airPos, 4.5f);
                            break;
                        case EVENT_AIR_FLY_DOWN:
                            me->GetMotionMaster()->MoveLand(POINT_GROUND, centerPos, 4.5f);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();

                EnterEvadeIfOutOfCombatArea(diff);
            }

            bool WasVampire(uint64 guid)
            {
                return _vampires.count(guid) != 0;
            }

            bool WasBloodbolted(uint64 guid)
            {
                return _bloodboltedPlayers.count(guid) != 0;
            }

        private:
            // offtank for this encounter is the player standing closest to main tank
            Player* SelectRandomTarget(bool includeOfftank, std::list<Player*>* targetList = nullptr)
            {
                std::list<HostileReference*> const& threatlist = me->GetThreatManager().getThreatList();
                std::list<Player*> tempTargets;

                if (threatlist.empty())
                    return nullptr;

                for (std::list<HostileReference*>::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
                    if (Unit* refTarget = (*itr)->getTarget())
                        if (refTarget != me->GetVictim() && refTarget->GetTypeId() == TYPEID_PLAYER && (includeOfftank ? true : (refTarget->GetGUID() != _offtankGUID)) && me->IsValidAttackTarget(refTarget))
                            tempTargets.push_back(refTarget->ToPlayer());

                if (tempTargets.empty())
                    return nullptr;

                if (targetList)
                {
                    *targetList = tempTargets;
                    return nullptr;
                }

                if (includeOfftank)
                {
                    tempTargets.sort(Trinity::ObjectDistanceOrderPred(me->GetVictim()));
                    return tempTargets.front();
                }

                //std::list<Player*>::iterator itr = tempTargets.begin();
                //std::advance(itr, urand(0, tempTargets.size() - 1));
                //return *itr;

               return Trinity::Containers::SelectRandomContainerElement(tempTargets);
            }

            struct BloodboltDistanceCheck
            {
                BloodboltDistanceCheck(Unit const* me) : _me(me) { }
                bool operator()(Unit* u) { return _me->GetDistance2d(u) < 5.0f; }
                Unit const* _me;
            };

            std::set<uint64> _vampires;
            std::set<uint64> _bloodboltedPlayers;
            ObjectGuid _offtankGUID;
            ObjectGuid _victimGUID;
            bool _creditBloodQuickening;
            bool _killMinchar;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<boss_blood_queen_lana_thelAI>(creature);
        }
};

class npc_infiltrator_minchar : public CreatureScript
{
    public:
        npc_infiltrator_minchar() : CreatureScript("npc_infiltrator_minchar") { }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_infiltrator_mincharAI>(creature);
        }

        struct npc_infiltrator_mincharAI : public ScriptedAI
        {
            npc_infiltrator_mincharAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                if (!rescued)
                    if (InstanceScript* instance = me->GetInstanceScript())
                        if (instance->GetData(DATA_BLOOD_QUICKENING_STATE) == DONE)
                            DoAction(1);
            }

            void DoAction(int32 action) override
            {
                rescued = true;
                me->HandleEmoteStateCommand(0);
                me->SetFlying(false);
                me->RemoveAllAuras();
                if (action)
                    me->NearTeleportTo(4629.3711f, 2782.6089f, 401.5301f, me->GetOrientation());
                else
                    me->GetMotionMaster()->MoveCharge(4629.3711f, 2782.6089f, 401.5301f, SPEED_CHARGE/3.0f);
            }

            void UpdateAI(uint32 diff) override
            {
                if (me->isDead() || rescued)
                    return;

                if (beamTimer <= diff)
                {
                    std::list<Creature*> stalkers;
                    GetCreatureListWithEntryInGrid(stalkers, me, NPC_MINCHAR_BEAM_STALKER, 100.0f);
                    for (auto&& stalker : stalkers)
                    {
                        if (stalker->GetPositionZ() > 420.0f)
                        {
                            if (stalker->GetPositionX() > 4635.0f)
                                stalker->CastSpell(me, SPELL_BLOOD_BEAM_VISUAL_LHAND);
                            else
                                stalker->CastSpell(me, SPELL_BLOOD_BEAM_VISUAL_RHAND);
                        }
                        else
                        {
                            if (stalker->GetPositionX() > 4635.0f)
                                stalker->CastSpell(me, SPELL_BLOOD_BEAM_VISUAL_LLEG);
                            else
                                stalker->CastSpell(me, SPELL_BLOOD_BEAM_VISUAL_RLEG);
                        }
                    }
                    beamTimer = 10000;
                }
                else
                    beamTimer -= diff;
            }

        private:
            uint32 beamTimer = 1000;
            bool rescued = false;
        };
};

// helper for shortened code
typedef boss_blood_queen_lana_thel::boss_blood_queen_lana_thelAI LanaThelAI;

class spell_blood_queen_vampiric_bite : public SpellScriptLoader
{
    public:
        spell_blood_queen_vampiric_bite() : SpellScriptLoader("spell_blood_queen_vampiric_bite") { }

        class spell_blood_queen_vampiric_bite_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_blood_queen_vampiric_bite_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_PLR))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_FRENZIED_BLOODTHIRST))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_PRESENCE_OF_THE_DARKFALLEN_TRIG))
                    return false;
                return true;
            }

            SpellCastResult CheckTarget()
            {
                bool bloodqueenFound = false;

                if (GetCaster())
                {
                    if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                        if (Creature* bloodQueen = ObjectAccessor::GetCreature(*GetCaster(), instance->GetGuidData(DATA_BLOOD_QUEEN_LANA_THEL)))
                            bloodqueenFound = true;

                    if (GetCaster()->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (Player* casterPlayer = GetCaster()->ToPlayer())
                        {
                            if (casterPlayer->GetSelectedUnit())
                            {
                                if (casterPlayer->GetSelectedUnit()->GetTypeId() == TYPEID_PLAYER)
                                {
                                    if (Player* targetPlayer = casterPlayer->GetSelectedUnit()->ToPlayer())
                                    {
                                        if (!IsVampire(targetPlayer) && bloodqueenFound)
                                        {
                                            if (GetExplTargetUnit()->GetGUID() != targetPlayer->GetGUID())
                                            {
                                                GetCaster()->CastSpell(targetPlayer, SPELL_VAMPIRIC_BITE_PLR, false);
                                                SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_NONE);
                                                return SPELL_FAILED_CUSTOM_ERROR;
                                            }

                                            return SPELL_CAST_OK;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_CANT_TARGET_VAMPIRES);
                return SPELL_FAILED_CUSTOM_ERROR;
            }

            void OnCast()
            {
                if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                    return;

                InstanceScript* instance = GetCaster()->GetInstanceScript();
                if (!instance)
                    return;

                GetCaster()->RemoveAura(SPELL_FRENZIED_BLOODTHIRST, 0, 0, AURA_REMOVE_BY_ENEMY_SPELL);
                GetCaster()->CastSpell(GetCaster(), SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_PLR, true);
                // Presence of the Darkfallen buff on Blood-Queen
                //if (GetCaster()->GetMap()->IsHeroic())
                //    GetCaster()->CastSpell(GetCaster(), SPELL_PRESENCE_OF_THE_DARKFALLEN, true); // TODO: remove
                // Shadowmourne questline
                if (GetCaster()->ToPlayer()->GetQuestStatus(QUEST_BLOOD_INFUSION) == QUEST_STATUS_INCOMPLETE)
                {
                    if (Aura* aura = GetCaster()->GetAura(SPELL_GUSHING_WOUND))
                    {
                        if (aura->GetStackAmount() == 3)
                        {
                            GetCaster()->CastSpell(GetCaster(), SPELL_THIRST_QUENCHED, true);
                            GetCaster()->RemoveAura(aura);
                        }
                        else
                            GetCaster()->CastSpell(GetCaster(), SPELL_GUSHING_WOUND, true);
                    }
                }
                    if (Creature* bloodQueen = ObjectAccessor::GetCreature(*GetCaster(), instance->GetGuidData(DATA_BLOOD_QUEEN_LANA_THEL)))
            {
                        bloodQueen->AI()->SetGUID(GetHitUnit()->GetGUID(), GUID_VAMPIRE);
                    // Presence of the Darkfallen buff on Blood-Queen
                    if (GetCaster()->GetMap()->IsHeroic())
                        bloodQueen->AddAura(SPELL_PRESENCE_OF_THE_DARKFALLEN_TRIG, bloodQueen);
                    }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_blood_queen_vampiric_bite_SpellScript::CheckTarget);
                BeforeHit += SpellHitFn(spell_blood_queen_vampiric_bite_SpellScript::OnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_blood_queen_vampiric_bite_SpellScript();
        }
};

class spell_blood_queen_frenzied_bloodthirst : public SpellScriptLoader
{
    public:
        spell_blood_queen_frenzied_bloodthirst() : SpellScriptLoader("spell_blood_queen_frenzied_bloodthirst") { }

        class spell_blood_queen_frenzied_bloodthirst_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_blood_queen_frenzied_bloodthirst_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (InstanceScript* instance = GetTarget()->GetInstanceScript())
                    if (Creature* bloodQueen = ObjectAccessor::GetCreature(*GetTarget(), instance->GetGuidData(DATA_BLOOD_QUEEN_LANA_THEL)))
                        bloodQueen->AI()->Talk(EMOTE_BLOODTHIRST, GetTarget());
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                    if (InstanceScript* instance = target->GetInstanceScript())
                        if (Creature* bloodQueen = ObjectAccessor::GetCreature(*target, instance->GetGuidData(DATA_BLOOD_QUEEN_LANA_THEL)))
                        {
                            // this needs to be done BEFORE charm aura or we hit an assert in Unit::SetCharmedBy
                            if (target->GetVehicleKit())
                                target->RemoveVehicleKit();

                            bloodQueen->AI()->Talk(SAY_MIND_CONTROL);
                            bloodQueen->CastSpell(target, SPELL_UNCONTROLLABLE_FRENZY, true);
                        }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_blood_queen_frenzied_bloodthirst_AuraScript::OnApply, EFFECT_0, SPELL_AURA_OVERRIDE_SPELLS, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_blood_queen_frenzied_bloodthirst_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_OVERRIDE_SPELLS, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_blood_queen_frenzied_bloodthirst_AuraScript();
        }
};

class BloodboltHitCheck
{
    public:
        explicit BloodboltHitCheck(LanaThelAI* ai) : _ai(ai) { }

        bool operator()(WorldObject* object) const
        {
            return _ai->WasBloodbolted(object->GetGUID());
        }

    private:
        LanaThelAI* _ai;
};

class spell_blood_queen_bloodbolt : public SpellScriptLoader
{
    public:
        spell_blood_queen_bloodbolt() : SpellScriptLoader("spell_blood_queen_bloodbolt") { }

        class spell_blood_queen_bloodbolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_blood_queen_bloodbolt_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TWILIGHT_BLOODBOLT))
                    return false;
                return true;
            }

            bool Load() override
            {
                return GetCaster()->GetEntry() == NPC_BLOOD_QUEEN_LANA_THEL;
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                uint32 targetCount = (targets.size() + 2) / 3;
                targets.remove_if(BloodboltHitCheck(static_cast<LanaThelAI*>(GetCaster()->GetAI())));
                Trinity::Containers::RandomResizeList(targets, targetCount);
                // mark targets now, effect hook has missile travel time delay (might cast next in that time)
                for (std::list<WorldObject*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                    GetCaster()->GetAI()->SetGUID((*itr)->GetGUID(), GUID_BLOODBOLT);
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetCaster()->CastSpell(GetHitUnit(), SPELL_TWILIGHT_BLOODBOLT, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_blood_queen_bloodbolt_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_blood_queen_bloodbolt_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_blood_queen_bloodbolt_SpellScript();
        }
};

class spell_blood_queen_pact_of_the_darkfallen : public SpellScriptLoader
{
    public:
        spell_blood_queen_pact_of_the_darkfallen() : SpellScriptLoader("spell_blood_queen_pact_of_the_darkfallen") { }

        class spell_blood_queen_pact_of_the_darkfallen_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_blood_queen_pact_of_the_darkfallen_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(Trinity::UnitAuraCheck(false, SPELL_PACT_OF_THE_DARKFALLEN));

                bool remove = true;
                std::list<WorldObject*>::const_iterator itrEnd = targets.end(), itr, itr2;
                // we can do this, unitList is MAX 4 in size
                for (itr = targets.begin(); itr != itrEnd && remove; ++itr)
                {
                    if (!GetCaster()->IsWithinDist(*itr, 5.0f, false))
                        remove = false;

                    for (itr2 = targets.begin(); itr2 != itrEnd && remove; ++itr2)
                        if (itr != itr2 && !(*itr2)->IsWithinDist(*itr, 5.0f, false))
                            remove = false;
                }

                if (remove)
                {
                    if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                    {
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PACT_OF_THE_DARKFALLEN);
                        targets.clear();
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_blood_queen_pact_of_the_darkfallen_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_blood_queen_pact_of_the_darkfallen_SpellScript();
        }
};

class spell_blood_queen_pact_of_the_darkfallen_dmg : public SpellScriptLoader
{
    public:
        spell_blood_queen_pact_of_the_darkfallen_dmg() : SpellScriptLoader("spell_blood_queen_pact_of_the_darkfallen_dmg") { }

        class spell_blood_queen_pact_of_the_darkfallen_dmg_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_blood_queen_pact_of_the_darkfallen_dmg_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PACT_OF_THE_DARKFALLEN_DAMAGE))
                    return false;
                return true;
            }

            // this is an additional effect to be executed
            void PeriodicTick(AuraEffect const* aurEff)
            {
                SpellInfo const* damageSpell = sSpellMgr->GetSpellInfo(SPELL_PACT_OF_THE_DARKFALLEN_DAMAGE);
                int32 damage = damageSpell->Effects[EFFECT_0].CalcValue();
                float multiplier = 0.3375f + 0.1f * uint32(aurEff->GetTickNumber()/10); // do not convert to 0.01f - we need tick number/10 as INT (damage increases every 10 ticks)
                damage = int32(damage * multiplier);
                GetTarget()->CastCustomSpell(SPELL_PACT_OF_THE_DARKFALLEN_DAMAGE, SPELLVALUE_BASE_POINT0, damage, GetTarget(), true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_blood_queen_pact_of_the_darkfallen_dmg_AuraScript::PeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_blood_queen_pact_of_the_darkfallen_dmg_AuraScript();
        }
};

class spell_blood_queen_pact_of_the_darkfallen_dmg_target : public SpellScriptLoader
{
    public:
        spell_blood_queen_pact_of_the_darkfallen_dmg_target() : SpellScriptLoader("spell_blood_queen_pact_of_the_darkfallen_dmg_target") { }

        class spell_blood_queen_pact_of_the_darkfallen_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_blood_queen_pact_of_the_darkfallen_dmg_SpellScript);

            void FilterTargets(std::list<WorldObject*>& unitList)
            {
                unitList.remove_if(Trinity::UnitAuraCheck(true, SPELL_PACT_OF_THE_DARKFALLEN));
                unitList.push_back(GetCaster());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_blood_queen_pact_of_the_darkfallen_dmg_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_blood_queen_pact_of_the_darkfallen_dmg_SpellScript();
        }
};

class achievement_once_bitten_twice_shy_n : public AchievementCriteriaScript
{
    public:
        achievement_once_bitten_twice_shy_n() : AchievementCriteriaScript("achievement_once_bitten_twice_shy_n") { }

        bool OnCheck(Player* source, Unit* target) override
        {
            if (!target)
                return false;

            if (LanaThelAI* lanaThelAI = CAST_AI(LanaThelAI, target->GetAI()))
                if (lanaThelAI->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL || lanaThelAI->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC)
                    return !lanaThelAI->WasVampire(source->GetGUID());

            return false;
        }
};

class achievement_once_bitten_twice_shy_v : public AchievementCriteriaScript
{
    public:
        achievement_once_bitten_twice_shy_v() : AchievementCriteriaScript("achievement_once_bitten_twice_shy_v") { }

        bool OnCheck(Player* source, Unit* target) override
        {
            if (!target)
                return false;

            if (LanaThelAI* lanaThelAI = CAST_AI(LanaThelAI, target->GetAI()))
                if (lanaThelAI->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL || lanaThelAI->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC)
                    return lanaThelAI->WasVampire(source->GetGUID());

            return false;
        }
};

class achievement_once_bitten_twice_shy_n25 : public AchievementCriteriaScript
{
    public:
        achievement_once_bitten_twice_shy_n25() : AchievementCriteriaScript("achievement_once_bitten_twice_shy_n25") { }

        bool OnCheck(Player* source, Unit* target) override
        {
            if (!target)
                return false;

            if (LanaThelAI* lanaThelAI = CAST_AI(LanaThelAI, target->GetAI()))
                if (lanaThelAI->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || lanaThelAI->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
                    return !lanaThelAI->WasVampire(source->GetGUID());

            return false;
        }
};

class achievement_once_bitten_twice_shy_v25 : public AchievementCriteriaScript
{
    public:
        achievement_once_bitten_twice_shy_v25() : AchievementCriteriaScript("achievement_once_bitten_twice_shy_v25") { }

        bool OnCheck(Player* source, Unit* target) override
        {
            if (!target)
                return false;

            if (LanaThelAI* lanaThelAI = CAST_AI(LanaThelAI, target->GetAI()))
                if (lanaThelAI->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || lanaThelAI->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
                    return lanaThelAI->WasVampire(source->GetGUID());

             return false;
        }
};

class spell_blood_queen_uncontrollable_frenzy : public SpellScriptLoader
{
    public:
        spell_blood_queen_uncontrollable_frenzy() : SpellScriptLoader("spell_blood_queen_uncontrollable_frenzy") { }

        class spell_blood_queen_uncontrollable_frenzy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_blood_queen_uncontrollable_frenzy_AuraScript);

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetTarget())
                    return;

                bool valid = false;

                if (InstanceScript* instance = GetTarget()->GetInstanceScript())
                    if (instance->GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == IN_PROGRESS)
                        if (GetTarget()->GetCharmerGUID() == instance->GetGuidData(DATA_BLOOD_QUEEN_LANA_THEL))
                            valid = true;

                if (!valid)
                {
                    GetTarget()->RemoveAurasDueToSpell(SPELL_UNCONTROLLABLE_FRENZY);
                    GetTarget()->RemoveAurasDueToSpell(SPELL_UNCONTROLLABLE_FRENZY_1);
                    GetTarget()->RemoveAurasDueToSpell(SPELL_UNCONTROLLABLE_FRENZY_2);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_blood_queen_uncontrollable_frenzy_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_APPLY_CREATURE_IMMUNITIES, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_blood_queen_uncontrollable_frenzy_AuraScript();
        }
};

void AddSC_boss_blood_queen_lana_thel()
{
    new boss_blood_queen_lana_thel();
    new npc_infiltrator_minchar();
    new spell_blood_queen_vampiric_bite();
    new spell_blood_queen_frenzied_bloodthirst();
    new spell_blood_queen_bloodbolt();
    new spell_blood_queen_pact_of_the_darkfallen();
    new spell_blood_queen_pact_of_the_darkfallen_dmg();
    new spell_blood_queen_pact_of_the_darkfallen_dmg_target();
    new achievement_once_bitten_twice_shy_n();
    new achievement_once_bitten_twice_shy_v();
    new achievement_once_bitten_twice_shy_n25();
    new achievement_once_bitten_twice_shy_v25();
    new spell_blood_queen_uncontrollable_frenzy();
}
