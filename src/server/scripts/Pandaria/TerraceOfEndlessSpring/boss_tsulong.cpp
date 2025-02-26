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
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "terrace_of_endless_spring.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Spell.h"
#include "Pet.h"

enum TsulongEvents
{
    EVENT_NONE,
    EVENT_FLY,
    EVENT_WAYPOINT_FIRST,
    EVENT_WAYPOINT_SECOND,
    EVENT_SWITCH_TO_NIGHT_PHASE,
    EVENT_BERSERK,
    // night phase
    EVENT_SPAWN_SUNBEAM,
    EVENT_SHADOW_BREATH,
    EVENT_DARK_OF_NIGHT,
    EVENT_NIGHTMARES,
    // day phase
    EVENT_SUMMON_TERROR,
    EVENT_SUMMON_SHA,
    EVENT_SUN_BREATH,
    EVENT_EVADE_CHECK,
    EVENT_COSMETIC_BATTLE,
};

enum TsulongSpells
{
    SPELL_DREAD_SHADOWS                 = 122767,
    SPELL_DREAD_SHADOWS_DEBUFF          = 122768,
    SPELL_SUNBEAM_DUMMY                 = 122782,
    SPELL_SUNBEAM_PROTECTION            = 122789,
    SPELL_NIGHT_PHASE_EFFECT            = 122841,
    SPELL_SHADOW_BREATH                 = 122752,
    SPELL_NIGHTMARES                    = 122775,
    SPELL_NIGHTMARES_EFF                = 122770,
    SPELL_SHA_ACTIVE                    = 122438, // night phase regen
    SPELL_TSULONG_KILL_CREDIT           = 123901,
    SPELL_WHOS_GOT_TWO_GREEN_THUMBS     = 125827,
          
    // day phase
    SPELL_GOLD_ACTIVE                   = 122453, // day phase regen and model
    SPELL_SUN_BREATH                    = 122855,
    SPELL_SUMMON_EMBODIED_TERROR        = 122995, // 62969
    SPELL_SUMMON_SHA_PERIODIC           = 122946,
    SPELL_SUMMON_SHA_PERIODIC_EFF       = 122952,
    SPELL_SUMMON_SHA_MISSILE            = 122953, // 62919
    SPELL_UNLEASHED_SHA_EXPLOSION       = 130008,
    SPELL_BATHED_IN_LIGHT               = 122858,

    // Heroic mode
    SPELL_THE_DARK_OF_NIGHT             = 123739,
    SPELL_DARK_FIXATE_AURA              = 123740,
    SPELL_DARK_EXPLOSION                = 130013,

    SPELL_LIGHT_OF_DAY_BUFF             = 123716,
    SPELL_LIGHT_OF_DAY_PERIODIC_SUMM    = 123816,
    SPELL_LIGHT_OF_DAY_SUMM             = 123717, // triggered by 123816, interval 25s
    SPELL_LIGHT_OF_DAY_COSMETIC         = 131988, // beam

};

enum TsulongTimers
{
    TIMER_FIRST_WAYPOINT = 5000, // 5 secs for test, live : 120 000
};

enum TsulongPhase
{
    PHASE_NONE,
    PHASE_FLY,
    PHASE_DAY,
    PHASE_NIGHT
};

enum TsulongWaypoints
{
    WAYPOINT_FIRST  = 10001,
    WAYPOINT_SECOND = 10002,
    WAYPOINT_DAY    = 10003,
};

enum TsulongDisplay
{
    DISPLAY_TSULON_NIGHT = 42532,
    DISPLAY_TSULON_DAY   = 42533,

    FACTION_DAY          = 2104,
    FACTION_NIGHT        = 1965,
};

enum TsulongActions
{
    ACTION_SPAWN_SUNBEAM = 10,
    ACTION_REGEN_COMPLETE,
    ACTION_DARK_NIGHT_SWITCH,
};

enum TsulongCreatures
{
    SUNBEAM_DUMMY_ENTRY    = 62849,
    DARK_OF_NIGHT_ENTRY    = 63346,
};

enum
{
    SPELL_TERRORIZE                 = 123011,
    SPELL_TERRORIZE_TSULONG         = 123012,
    SPELL_TINY_TERROR               = 123038,
    SPELL_TINY_TERROR_EFF           = 123026,
    SPELL_SUMMON_TINY_TERROR        = 123508,
    SPELL_UNSTABLE_BOLT_SACRIFICE   = 122907,

    WORLD_STATE_NO_LFR                      = 12246,
    WORLD_STATE_WHOS_GOT_TWO_GREEN_THUMBS   = 12630,
};

class boss_tsulong : public CreatureScript
{
    enum Quotes
    {
        EMOTE_SUMMON_TERROR         = 1,
        EMOTE_TERRORIZE,
        SAY_INTRO,
        SAY_AGGRO,
        SAY_NIGHTMARES,
        SAY_NIGHT_TO_DAY,
        SAY_DAY_TO_NIGHT,
        SAY_SLAY_DAY,
        SAY_SLAY_NIGHT,
        SAY_DEATH,
        SAY_WIPE,
        EMOTE_SUNBEAM,
    };

    public:
        boss_tsulong() : CreatureScript("boss_tsulong") { }

        struct boss_tsulongAI : public BossAI
        {
            boss_tsulongAI(Creature* creature) : BossAI(creature, DATA_TSULONG) { }

            EventMap m_oocEvents, berserkEvents;
            uint8 phase;
            ObjectGuid victimGUID;
            bool firstSpecialEnabled;
            bool secondSpecialEnabled;
            bool inFly;
            bool hasBeenDefeated;
            bool landing;

            void InitializeAI() override
            {
                landing = false;
                hasBeenDefeated = false;
                me->setActive(true);

                if (instance && instance->GetBossState(DATA_PROTECTORS) < DONE)
                    m_oocEvents.ScheduleEvent(EVENT_COSMETIC_BATTLE, 1000);

                Reset();
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_NIGHTMARES:
                        DoCast(target, SPELL_NIGHTMARES_EFF, true);
                        break;
                    case SPELL_SUMMON_SHA_PERIODIC_EFF:
                        DoCast(target, SPELL_SUMMON_SHA_MISSILE, true);
                        break;
                }
            }

            void SpellHit(Unit* , SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_TERRORIZE_TSULONG)
                    Talk(EMOTE_TERRORIZE);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() == NPC_EMBODIED_TERROR)
                {
                    if (summon->AI())
                        summon->AI()->DoZoneInCombat();
                }
            }

            void Reset() override
            {
                m_oocEvents.RescheduleEvent(1, 2000);

                dayPhaseOri = 0.0f;
                victimGUID = ObjectGuid::Empty;
                _Reset();
                events.Reset();
                berserkEvents.Reset();
                summons.DespawnAll();

                inFly = false;

                me->SetDisableGravity(true);
                me->SetCanFly(true);
                me->RemoveAurasDueToSpell(SPELL_DREAD_SHADOWS);
                me->RemoveAurasDueToSpell(SPELL_BERSERK);
                me->SetPowerType(POWER_ENERGY);
                me->SetPower(POWER_ENERGY, 0);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                
                if (hasBeenDefeated)
                    return;

                _Reset();

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    if (instance->GetData(TYPE_PROTECTORS) == DONE && instance->GetData(TYPE_TSULONG) != DONE && !landing)
                        DoAction(ACTION_START_TSULONG_WAYPOINT);
                    else if (instance->GetData(TYPE_PROTECTORS) != DONE)
                    {
                        phase = PHASE_DAY;
                        me->SetDisplayId(DISPLAY_TSULON_DAY);
                        me->SetFaction(35);
                        me->HandleEmoteStateCommand(35);
                    }
                    else if (landing)
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveAurasDueToSpell(SPELL_GOLD_ACTIVE);
                        me->RemoveAurasDueToSpell(SPELL_SUMMON_SHA_PERIODIC);
                        me->SetFaction(FACTION_NIGHT);
                        me->SetHealth(me->GetMaxHealth());
                        events.SetPhase(PHASE_NONE);
                        phase = PHASE_NONE;
                    }
                }

                if (Creature* plant = GetEnchantedPlant(true))
                    plant->Respawn();
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_TSULONG, FAIL);
                }

                summons.DespawnAll();
            }

            void MoveInLineOfSight(Unit* who) override
            {
                CreatureAI::MoveInLineOfSight(who);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (instance && instance->GetData(DATA_TSULONG) == DONE)
                    return;

                _JustEngagedWith();
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                Talk(SAY_AGGRO);
                me->SetPower(POWER_ENERGY, 0);
                me->SetHealth(me->GetMaxHealth());
                SetPhase(PHASE_NIGHT);
                berserkEvents.ScheduleEvent(EVENT_BERSERK, 8 * MINUTE * IN_MILLISECONDS + 10 * IN_MILLISECONDS);

                if (Creature* plant = GetEnchantedPlant())
                    plant->SetInCombatWith(me);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk((phase == PHASE_DAY) ? SAY_SLAY_DAY : SAY_SLAY_NIGHT);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (me->IsInEvadeMode())
                    damage = 0;
                else
                if (phase == PHASE_DAY)
                {
                    if (me->GetHealth() <= damage)
                    {
                        DoCast(me, SPELL_UNLEASHED_SHA_EXPLOSION, true);
                        damage = 0;
                        summons.DespawnAll();
                        EnterEvadeMode();
                    }
                }
                else if (phase == PHASE_NIGHT)
                {
                    if (me->GetHealthPct() < 1.1f || me->GetHealth() <= damage)
                    {
                        damage = 0;
                        HandleDefeat(attacker);
                    }
                }
            }

            void HealReceived(Unit* doneBy, uint32& heal) override
            {
                uint32 pct = me->GetMaxHealth() / 100;

                // If heal more than 5% of total HP then prevent it
                if (heal > pct * 5)
                    heal = pct;

                if (instance && instance->GetBossState(DATA_TSULONG) != IN_PROGRESS)
                    return;

                // victory if healed during the day phase
                if (me->HealthAbovePctHealed(99, heal))
                {
                    if (phase == PHASE_DAY)
                    {
                        HandleDefeat(doneBy);
                    }
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                _JustDied();
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_TSULONG_WAYPOINT:
                        SetPhase(PHASE_FLY);
                        Talk(SAY_INTRO);
                        break;
                    case ACTION_SPAWN_SUNBEAM:
                        events.ScheduleEvent(EVENT_SPAWN_SUNBEAM, 0, 0, PHASE_NIGHT);
                        break;
                    case ACTION_REGEN_COMPLETE:
                        me->SetPower(POWER_ENERGY, 0);
                        SetPhase((phase == PHASE_DAY) ? PHASE_NIGHT : PHASE_DAY);
                        break;
                }
            }

            void SetPhase(uint8 newPhase)
            {
                uint8 oldPhase = phase;
                phase = newPhase;
                events.SetPhase(newPhase);
                switch (newPhase)
                {
                    case PHASE_FLY:
                        me->RemoveAurasDueToSpell(SPELL_GOLD_ACTIVE);
                        me->RemoveAurasDueToSpell(SPELL_SHA_ACTIVE);
                        events.ScheduleEvent(EVENT_FLY, 5000, 0, PHASE_FLY);
                        break;
                    case PHASE_DAY:
                    {
                        if (Unit* victim = me->GetVictim())
                            victimGUID = victim->GetGUID();

                        me->SetReactState(REACT_PASSIVE);
                        me->SetFaction(FACTION_DAY);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveIdle();
                        me->AttackStop();
                        me->StopMoving();
                        me->SetTarget(ObjectGuid::Empty);
                        me->RemoveAurasDueToSpell(SPELL_SHA_ACTIVE);
                        me->RemoveAurasDueToSpell(SPELL_DREAD_SHADOWS);
                        me->RemoveAurasDueToSpell(SPELL_TERRORIZE_TSULONG);
                        DoCast(me, SPELL_GOLD_ACTIVE, true);

                        events.Reset();
                        me->GetMotionMaster()->MovePoint(WAYPOINT_DAY, { -1017.841f, -3049.621f, 12.823f, 4.72f });
                        events.ScheduleEvent(EVENT_SUN_BREATH, 6000, 0, PHASE_DAY);
                        events.ScheduleEvent(EVENT_SUMMON_TERROR, 12000, 0, PHASE_DAY);
                        events.ScheduleEvent(EVENT_EVADE_CHECK, 2000, 0, PHASE_DAY);

                        if (oldPhase == PHASE_NIGHT)
                        {
                            Talk(SAY_NIGHT_TO_DAY);
                            me->SetHealth(me->GetMaxHealth() - me->GetHealth());
                        }
                    }
                        break;
                    case PHASE_NIGHT:
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveAurasDueToSpell(SPELL_GOLD_ACTIVE);
                        me->RemoveAurasDueToSpell(SPELL_SUMMON_SHA_PERIODIC);
                        DoCast(me, SPELL_SHA_ACTIVE, true);
                        me->SetFaction(FACTION_NIGHT);

                        events.Reset();
                        events.ScheduleEvent(EVENT_SWITCH_TO_NIGHT_PHASE, 0, 0, PHASE_NIGHT);
                        events.ScheduleEvent(EVENT_SHADOW_BREATH, 10000, 0, PHASE_NIGHT);
                        events.ScheduleEvent(EVENT_NIGHTMARES, urand(15000, 16000), 0, PHASE_NIGHT);
                        events.ScheduleEvent(EVENT_SPAWN_SUNBEAM, 2000, 0, PHASE_NIGHT);

                        if (IsHeroic())
                        {
                            events.ScheduleEvent(EVENT_DARK_OF_NIGHT, 10000, 0, PHASE_NIGHT);
                            me->RemoveAurasDueToSpell(SPELL_LIGHT_OF_DAY_PERIODIC_SUMM);
                        }

                        if (oldPhase == PHASE_DAY)
                        {
                            Talk(SAY_DAY_TO_NIGHT);
                            me->SetHealth(me->GetMaxHealth() - me->GetHealth());
                        }
                        break;
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case WAYPOINT_FIRST:
                        events.ScheduleEvent(EVENT_WAYPOINT_FIRST, 0, 0, PHASE_FLY);
                        break;
                    case WAYPOINT_SECOND:
                        events.ScheduleEvent(EVENT_WAYPOINT_SECOND, 0, 0, PHASE_FLY);
                        break;
                    case WAYPOINT_DAY:
                        if (Unit* vict = ObjectAccessor::GetUnit(*me, victimGUID))
                            dayPhaseOri = me->GetAngle(vict);

                        me->SetFacingTo(dayPhaseOri);

                        DoCast(me, SPELL_SUMMON_SHA_PERIODIC, true);

                        if (IsHeroic())
                            DoCast(me, SPELL_LIGHT_OF_DAY_PERIODIC_SUMM, false);
                        break;
                }
            }

            void HandleDefeat(Unit* attacker)
            {
                if (hasBeenDefeated)
                    return;

                hasBeenDefeated = true;

                EnterEvadeMode();

                Talk(SAY_DEATH);
                
                me->CombatStop();
                me->SetReactState(REACT_PASSIVE);
                me->SetFaction(FACTION_DAY);
                me->RemoveAllAuras();

                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();

                events.Reset();
                HandleRemoveFrightSpawn();

                me->setRegeneratingHealth(false);
                me->setRegeneratingMana(false);

                DoCast(me, SPELL_GOLD_ACTIVE, true);
                me->SetDisplayId(DISPLAY_TSULON_DAY);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);

                DoCastAOE(SPELL_TSULONG_KILL_CREDIT, true);

                if (Player* player = attacker->GetCharmerOrOwnerPlayerOrPlayerItself())
                {
                    me->GetMap()->ToInstanceMap()->PermBindAllPlayers(player);
                    player->RewardPersonalLootAndCurrency(me);
                }

                if (instance)
                {
                    if (GetEnchantedPlant())
                        instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_WHOS_GOT_TWO_GREEN_THUMBS);

                    instance->SetBossState(DATA_TSULONG, DONE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                }

                events.Reset();

                summons.DespawnAll();

                Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();

                for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                {
                    if (auto const player = itr->GetSource())
                    {
                        player->CombatStop();
                    }
                }
                
                me->StopMoving();
                me->OverrideInhabitType(INHABIT_AIR);
                me->UpdateMovementFlags();

                Movement::MoveSplineInit init(me);
                init.MoveTo(TsulongLeavePos.GetPositionX(), TsulongLeavePos.GetPositionY(), TsulongLeavePos.GetPositionZ());
                init.SetSmooth();
                init.Launch();
                me->DespawnOrUnsummon(me->GetSplineDuration());
            }

            void HandleEvadeDuringDay()
            {
                if (me->IsInEvadeMode())
                    return;

                Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();

                for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                {
                    if (Player* player = itr->GetSource())
                    {
                        if (player->IsAlive() && player->IsWithinDist(me, 40.f))
                            return;
                    }
                }

                summons.DespawnAll();
                EnterEvadeMode();
            }

            void EnterEvadeMode() override
            {
                _EnterEvadeMode();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->GetMotionMaster()->Clear();

                if (!hasBeenDefeated)
                {
                    Talk(SAY_WIPE);
                    Reset();
                    _DespawnAtEvade();
                    me->GetMap()->CreatureRelocation(me, -1017.841f, -3049.621f, 12.823f, 4.72f);

                    if (instance)
                        instance->SetBossState(DATA_TSULONG, FAIL);
                }

                if (instance)
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                HandleRemoveFrightSpawn();

                if (Creature* plant = GetEnchantedPlant())
                    plant->AI()->EnterEvadeMode();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->IsInEvadeMode() && !me->IsInCombat() && !inFly)
                {
                    m_oocEvents.Update(diff);

                    while (uint32 eventId = m_oocEvents.ExecuteEvent())
                    {
                        switch (eventId)
                        {
                            case EVENT_FLY:
                                summons.DespawnAll();
                                m_oocEvents.ScheduleEvent(EVENT_FLY, 1500);
                                break;
                            case EVENT_COSMETIC_BATTLE:
                                if (instance->GetBossState(DATA_PROTECTORS) < DONE)
                                {
                                    me->HandleEmoteCommand(urand(0, 1) ? EMOTE_ONESHOT_ATTACK1H : EMOTE_ONESHOT_ATTACK_UNARMED);
                                    m_oocEvents.ScheduleEvent(EVENT_COSMETIC_BATTLE, urand(1500, 2000));
                                }
                                break;
                        }
                    }
                }

                events.Update(diff);

                {
                    if (phase == PHASE_FLY)
                    {
                        while (uint32 eventId = events.ExecuteEvent())
                        {
                            switch (eventId)
                            {
                                case EVENT_FLY:
                                    me->setActive(true);
                                    me->UpdateObjectVisibility(true);
                                    me->SetFaction(FACTION_NIGHT);
                                    me->SetReactState(REACT_PASSIVE);
                                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                                    me->HandleEmoteStateCommand(EMOTE_STATE_NONE);
                                    me->SetDisplayId(DISPLAY_TSULON_NIGHT);
                                    me->GetMotionMaster()->MovePoint(WAYPOINT_FIRST, -1018.10f, -2947.431f, 50.12f);
                                    inFly = true;
                                    break;
                                case EVENT_WAYPOINT_FIRST:
                                    me->GetMotionMaster()->Clear();
                                    me->GetMotionMaster()->MovePoint(WAYPOINT_SECOND, -1017.841f, -3049.621f, 12.823f);
                                    break;
                                case EVENT_WAYPOINT_SECOND:
                                    landing = true;
                                    me->SetHomePosition(-1017.841f, -3049.621f, 12.823f, 4.72f);
                                    me->SetReactState(REACT_AGGRESSIVE);
                                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                                    me->OverrideInhabitType(INHABIT_GROUND);
                                    me->UpdateMovementFlags();
                                    inFly = false;
                                    events.SetPhase(PHASE_NONE);
                                    phase = PHASE_NONE;
                                    break;
                            }
                        }
                    }
                }

                berserkEvents.Update(diff);

                while (uint32 eventId = berserkEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_BERSERK)
                        DoCast(me, SPELL_BERSERK, true);
                    break;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (phase == PHASE_NIGHT)
                {
                    if (!UpdateVictim())
                        return;

                    while (uint32 eventId = events.ExecuteEvent())
                    {
                        Position pos;
                        switch (eventId)
                        {
                            case EVENT_SWITCH_TO_NIGHT_PHASE:
                                //me->SetDisplayId(DISPLAY_TSULON_NIGHT);
                                me->SetFaction(FACTION_NIGHT);
                                me->CastSpell(me, SPELL_DREAD_SHADOWS, true);
                                break;
                            case EVENT_SPAWN_SUNBEAM:
                                if (Creature* pSunbeam = GetClosestCreatureWithEntry(me, SUNBEAM_DUMMY_ENTRY, 200.0f))
                                    pSunbeam->DisappearAndDie();

                                Talk(EMOTE_SUNBEAM, me);
                                pos = me->GetRandomNearPosition(30.0f);
                                me->SummonCreature(SUNBEAM_DUMMY_ENTRY, pos);

                                // dark night should switch
                                if (IsHeroic())
                                {
                                    std::list<Creature*> cList;
                                    GetCreatureListWithEntryInGrid(cList, me, NPC_DARK_OF_NIGHT, 200.0f);

                                    for (auto&& itr : cList)
                                        itr->AI()->DoAction(ACTION_DARK_NIGHT_SWITCH);
                                }
                                events.ScheduleEvent(EVENT_SPAWN_SUNBEAM, 40000, 0, PHASE_NIGHT);
                                break;
                            case EVENT_NIGHTMARES:
                                Talk(SAY_NIGHTMARES);
                                DoCast(me, SPELL_NIGHTMARES, true);
                                events.ScheduleEvent(EVENT_NIGHTMARES, 15000, 0, PHASE_NIGHT);
                                break;
                            case EVENT_SHADOW_BREATH:
                                DoCastVictim(SPELL_SHADOW_BREATH, false);
                                events.ScheduleEvent(EVENT_SHADOW_BREATH, urand(25000, 30000), 0, PHASE_NIGHT);
                                break;
                            case EVENT_DARK_OF_NIGHT:
                                if (Creature* Sunbeam = GetClosestCreatureWithEntry(me, SUNBEAM_DUMMY_ENTRY, 200.0f, true))
                                    GenerateDarkOfNight(Sunbeam);

                                events.ScheduleEvent(EVENT_DARK_OF_NIGHT, urand(25000, 45000), 0, PHASE_NIGHT);
                                break;
                        }
                    }

                    DoMeleeAttackIfReady();
                }
                else if (phase == PHASE_DAY)
                {
                    while (uint32 eventId = events.ExecuteEvent())
                    {
                        switch (eventId)
                        {
                            case EVENT_SUMMON_TERROR:
                                Talk(EMOTE_SUMMON_TERROR, me);
                                DoCast(me, SPELL_SUMMON_EMBODIED_TERROR, true);
                                events.ScheduleEvent(EVENT_SUMMON_TERROR, 30000, 0, PHASE_DAY);
                                break;
                            case EVENT_SUN_BREATH:
                                me->SetFacingTo(dayPhaseOri);
                                me->SetOrientation(dayPhaseOri);
                                DoCast(me, SPELL_SUN_BREATH, false);
                                events.ScheduleEvent(EVENT_SUN_BREATH, 29000, 0, PHASE_DAY);
                                break;
                            case EVENT_SUMMON_SHA:
                                //events.ScheduleEvent(EVENT_SUN_BREATH, 30000, 0, PHASE_DAY);
                                break;
                            case EVENT_EVADE_CHECK:
                                HandleEvadeDuringDay();
                                events.ScheduleEvent(EVENT_EVADE_CHECK, 2000, 0, PHASE_DAY);
                                break;
                        }
                    }
                }

            }

            Creature* GetEnchantedPlant(bool ignoreDeathState = false)
            {
                if (!instance)
                    return nullptr;

                Creature* plant = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ENCHANTED_PLANT));
                if (plant)
                {
                    if (!plant->IsAlive() && !ignoreDeathState)
                        return nullptr;
                }
                return plant;
            }

            private:
                /* Main idea that we`ll get any positions in area 
                like points in static range with any orientation*/
                void GenerateDarkOfNight(Creature* sunbeam)
                {
                    uint32 val = Is25ManRaid() ? 9 : 4;

                    for (uint32 i = 0; i < val; i++)
                    {
                        float x = 0, y = 0, o = Position::NormalizeOrientation(frand(0.0f, 2 * M_PI));
                        GetPositionWithDistInOrientation(sunbeam, DARK_OF_NIGHT_RANGE, o, x, y);

                        me->SummonCreature(DARK_OF_NIGHT_ENTRY, x, y, sunbeam->GetPositionZ(), o);
                    }
                }
                float dayPhaseOri;

                void HandleRemoveFrightSpawn()
                {
                    std::list<Creature*> cList;
                    GetCreatureListWithEntryInGrid(cList, me, NPC_FRIGHT_SPAWN, 200.0f);

                    for (auto&& itr : cList)
                        itr->DespawnOrUnsummon();
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_tsulongAI(creature);
        }
};

class npc_sunbeam : public CreatureScript
{
    public:
        npc_sunbeam() : CreatureScript("npc_sunbeam") { }

        struct npc_sunbeamAI : public CreatureAI
        {
            npc_sunbeamAI(Creature* creature) : CreatureAI(creature)
            {
                instance = creature->GetInstanceScript();
                creature->SetObjectScale(5.0f);
                creature->SetReactState(REACT_PASSIVE);
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                creature->CastSpell(creature, SPELL_SUNBEAM_DUMMY, true);
            }

            InstanceScript* instance;

            void UpdateAI(uint32 /*diff*/) override
            {
                float scale = me->GetFloatValue(OBJECT_FIELD_SCALE);
                if (scale <= 1.0f)
                {
                    Talk(0);
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sunbeamAI(creature);
        }
};

// Embodied Terror - 62969
class npc_embodied_terror : public CreatureScript
{
    public:
        npc_embodied_terror() : CreatureScript("npc_embodied_terror") { }

        struct npc_embodied_terrorAI : public CreatureAI
        {
            npc_embodied_terrorAI(Creature* creature) : CreatureAI(creature) { }

            void Reset() override
            {
                instance = me->GetInstanceScript();

                scheduler
                    .SetValidator([this] { return !died && instance && instance->GetBossState(DATA_TSULONG) == IN_PROGRESS; })
                    .Schedule(Seconds(3), Seconds(6), [this](TaskContext context)
                {
                    DoCast(me, SPELL_TERRORIZE, true);
                    DoCast(me, SPELL_TERRORIZE_TSULONG, true);

                    context.Repeat(Seconds(11), Seconds(25));
                });

                died = false;
            }

            InstanceScript* instance;
            TaskScheduler scheduler;

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (died)
                {
                    damage = 0;
                    return;
                }

                if (me->GetHealth() <= damage && !died)
                {
                    died = true;
                    me->DespawnOrUnsummon(5000);
                    DoCast(me, SPELL_TINY_TERROR, true);
                }
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_TINY_TERROR_EFF)
                    for (int i = 0; i < 5; ++i)
                        DoCast(me, SPELL_SUMMON_TINY_TERROR, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                scheduler.Update(diff);

                DoMeleeAttackIfReady();
            }

        private:
            uint32 terrorizeTimer;
            bool died;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_embodied_terrorAI(creature);
        }
};

// Terrorize - 123018
class spell_terrorize_periodic_player : public SpellScriptLoader
{
    public:
        spell_terrorize_periodic_player() : SpellScriptLoader("spell_terrorize_periodic_player") { }

        class spell_terrorize_periodic_player_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_terrorize_periodic_player_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit*  target = GetHitUnit())
                    SetHitDamage(target->CountPctFromCurHealth(GetHitDamage()));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_terrorize_periodic_player_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_terrorize_periodic_player_SpellScript();
        }
};

 // Fright Spawn - 62977
class npc_fright_spawn : public CreatureScript
{
    enum
    {
        SPELL_FRIGHT        = 123036
    };

    public:
        npc_fright_spawn() : CreatureScript("npc_fright_spawn") { }

        struct npc_fright_spawnAI : public CreatureAI
        {
            npc_fright_spawnAI(Creature* creature) : CreatureAI(creature) { }

            void Reset() override
            {
                frightTimer = urand(3000, 5000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (frightTimer <= diff)
                {
                    DoCast(me, SPELL_FRIGHT, false);
                    frightTimer = urand(8000, 10000);
                } else frightTimer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            uint32 frightTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_fright_spawnAI(creature);
        }
};

// Unstable Sha - 62962
class npc_unstable_sha : public CreatureScript
{
    enum
    {
        SPELL_INSTABILITY_RIDE          = 122928,
        SPELL_INSTABILITY_TRANSFORM     = 122930,
        SPELL_INSTABILITY_DAMAGE        = 123697,
        SPELL_INSTABILITY_HEAL          = 130078,
        SPELL_UNSTABLE_BOLT             = 122881
    };

    public:
        npc_unstable_sha() : CreatureScript("npc_unstable_sha") { }

        struct npc_unstable_shaAI : public ScriptedAI
        {
            npc_unstable_shaAI(Creature* creature) : ScriptedAI(creature)
            {
                summonerGUID = ObjectGuid::Empty;
                riding = false;
                cast = false;
            }

            EventMap events;

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetSpeed(MOVE_RUN, 0.54f);
                DoZoneInCombat(me, 200.0f);
                SetGazeOn(summoner);
                summonerGUID = summoner->GetGUID();
                events.ScheduleEvent(1, urand(2000, 6000));
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (!cast && target == me && spell->Id == SPELL_INSTABILITY_TRANSFORM)
                    if (Unit* summoner = Unit::GetUnit(*me, summonerGUID))
                    {
                        DoCast(summoner, (summoner->GetFaction() == FACTION_DAY) ? SPELL_INSTABILITY_DAMAGE : SPELL_INSTABILITY_HEAL);
                        cast = true;
                        me->DespawnOrUnsummon(500);
                    }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictimWithGaze() || riding)
                    return;

                if (Unit* summoner = Unit::GetUnit(*me, summonerGUID))
                {
                    if (me->GetExactDist2d(summoner) <= 5.5f)
                    {
                        riding = true;
                        DoCast(summoner, SPELL_INSTABILITY_RIDE);
                    }
                }

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case 1:
                            if (Unit* unit = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            {
                                DoCast(unit, SPELL_UNSTABLE_BOLT);
                                events.ScheduleEvent(1, urand(8000, 16000));
                            }
                            break;
                    }
                }
            }

            bool riding;
            bool cast;
            ObjectGuid summonerGUID;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_unstable_shaAI(creature);
        }
};

// 125843, jam spell ?
class spell_dread_shadows_damage : public SpellScriptLoader
{
    public:
        spell_dread_shadows_damage() : SpellScriptLoader("spell_dread_shadows_damage") { }

        class spell_dread_shadows_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dread_shadows_damage_SpellScript);

            void RemoveInvalidTargets(std::list<WorldObject*>& targets)
            {
                targets.clear();
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dread_shadows_damage_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dread_shadows_damage_SpellScript::RemoveInvalidTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dread_shadows_damage_SpellScript();
        }
};

class DreadShadowsTargetCheck
{
    public:
        bool operator()(WorldObject* object) const
        {
            // check Sunbeam protection
            if (object->ToUnit() && object->ToUnit()->HasAura(122789))
                return true;

            return false;
        }
};

// 122768
class spell_dread_shadows_malus : public SpellScriptLoader
{
    public:
        spell_dread_shadows_malus() : SpellScriptLoader("spell_dread_shadows_malus") { }

        class spell_dread_shadows_malus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dread_shadows_malus_SpellScript);

            std::list<WorldObject*> m_targets;

            void RemoveInvalidTargets(std::list<WorldObject*>& targets)
            {
                if (Unit* caster = GetCaster())
                    targets.remove(caster);

                targets.remove_if (DreadShadowsTargetCheck());
                if (Creature* flower = GetCaster()->FindNearestCreature(NPC_ENCHANTED_PLANT, 200.0f))
                    targets.push_back(flower);
                m_targets = targets;
            }

            void CopyTargets(std::list<WorldObject*>& targets)
            {
                targets = m_targets;
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dread_shadows_malus_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dread_shadows_malus_SpellScript::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dread_shadows_malus_SpellScript();
        }
};

// Sunbeam 122789
class spell_sunbeam : public SpellScript
{
    PrepareSpellScript(spell_sunbeam);

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        targets.clear();
        Map::PlayerList const& players = GetCaster()->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            Player* plr = itr->GetSource();
            if (!plr)
                continue;

            if (Unit* caster = GetCaster())
            {
                float scale = caster->GetFloatValue(OBJECT_FIELD_SCALE);

                if (plr->GetExactDist2d(caster) <= scale)
                    targets.push_back(plr);
            }
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sunbeam::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

class spell_sunbeam_aura : public AuraScript
{
    PrepareAuraScript(spell_sunbeam_aura);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
        {
            if (target->GetTypeId() == TYPEID_PLAYER)
            {
                if (Pet* pet = target->ToPlayer()->GetPet())
                    pet->AddAura(SPELL_SUNBEAM_PROTECTION, pet);

                if (Unit* caster = GetCaster())
                {
                    float scale = caster->GetFloatValue(OBJECT_FIELD_SCALE);
                    float scaleDiff = caster->GetInstanceScript()->instance->Is25ManRaid() ? 0.1f : 0.2f;

                    if (scale > scaleDiff)
                        caster->SetObjectScale(scale - scaleDiff);
                }
            }

            target->RemoveAurasDueToSpell(SPELL_DREAD_SHADOWS_DEBUFF);
        }
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
        {
            if (target->GetTypeId() == TYPEID_PLAYER)
            {
                if (Pet* pet = target->ToPlayer()->GetPet())
                    pet->RemoveAurasDueToSpell(SPELL_SUNBEAM_PROTECTION);
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sunbeam_aura::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_sunbeam_aura::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Day and Night regen auras - 122438 and 122453
class spell_tsulong_sha_regen : public SpellScriptLoader
{
    public:
        spell_tsulong_sha_regen() : SpellScriptLoader("spell_tsulong_sha_regen") { }

        class spell_tsulong_sha_regen_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tsulong_sha_regen_AuraScript);

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (Unit* target = GetUnitOwner())
                {
                    if (target->GetPower(POWER_ENERGY) == 99)
                    {
                        if (target->ToCreature() && target->ToCreature()->AI())
                            target->ToCreature()->AI()->DoAction(ACTION_REGEN_COMPLETE);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tsulong_sha_regen_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tsulong_sha_regen_AuraScript();
        }
};

class spell_tsulong_nightmares : public SpellScriptLoader
{
    public:
        spell_tsulong_nightmares() : SpellScriptLoader("spell_tsulong_nightmares") { }

        class spell_tsulong_nightmares_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tsulong_nightmares_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                return true;
            }

            void SelectTargets(std::list<WorldObject*>& targets)
            {
                if (Unit* caster = GetCaster())
                {
                    // Melee targets should be low priority
                    targets.sort(Trinity::ObjectDistanceOrderPred(caster, false));

                    uint32 m_maxTargets = (caster->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || caster->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC) ? 3 : 1;

                    if (targets.size() > m_maxTargets)
                        Trinity::Containers::RandomResizeList(targets, m_maxTargets);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tsulong_nightmares_SpellScript::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tsulong_nightmares_SpellScript();
        }
};

class spell_dark_of_night_fixate : public SpellScript
{
    PrepareSpellScript(spell_dark_of_night_fixate);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        if (targets.size() > 1)
        {
            if (Unit* caster = GetCaster())
                targets.sort(Trinity::ObjectDistanceOrderPred(caster));

            if (targets.size() > 1)
                targets.resize(1);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dark_of_night_fixate::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

class spell_tsulong_terrorize : public SpellScriptLoader
{
    public:
        spell_tsulong_terrorize() : SpellScriptLoader("spell_tsulong_terrorize") { }

        class spell_tsulong_terrorize_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tsulong_terrorize_SpellScript);

            void SelectTargets(std::list<WorldObject*>&targets)
            {
                if (Unit* caster = GetCaster())
                {
                    uint32 m_maxTargets = (caster->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || caster->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC) ? 5 : 2;
      
                    if (targets.size() > m_maxTargets)
                        Trinity::Containers::RandomResizeList(targets, m_maxTargets);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tsulong_terrorize_SpellScript::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tsulong_terrorize_SpellScript();
        }
};

// Dark of night 63346
class npc_dark_of_night : public CreatureScript
{
    public:
        npc_dark_of_night() : CreatureScript("npc_dark_of_night") { }

        struct npc_dark_of_nightAI : public ScriptedAI
        {
            npc_dark_of_nightAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid sunbeamGUID;
            bool hasDied;

            void IsSummonedBy(Unit* summoner) override
            {
                // Apply taunt immune
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

                hasDied = false;

                DoZoneInCombat(me, 200.0f);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                DoCast(me, SPELL_DARK_FIXATE_AURA, true);
                me->SetSpeed(MOVE_RUN, 0.35f);
                me->PrepareChanneledCast(me->GetOrientation());

                if (Creature* sunbeam = GetClosestCreatureWithEntry(me, SUNBEAM_DUMMY_ENTRY, 200.0f, true))
                {
                    sunbeamGUID = sunbeam->GetGUID();

                    Movement::MoveSplineInit init(me);
                    init.MoveTo(sunbeam->GetPositionX(), sunbeam->GetPositionY(), sunbeam->GetPositionZ());
                    init.SetUncompressed();
                    init.Launch();
                }

                uint32 delay = 0;
                me->m_Events.Schedule(delay += me->GetSplineDuration(), 5, [this]()
                {
                    if (!hasDied)
                        DoCast(me, SPELL_DARK_EXPLOSION, true);

                    if (Creature* sunbeam = ObjectAccessor::GetCreature(*me, sunbeamGUID))
                        sunbeam->DespawnOrUnsummon();;

                    me->DespawnOrUnsummon();
                });
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->StopMoving();
                hasDied = true;
                me->DespawnOrUnsummon();
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_DARK_NIGHT_SWITCH)
                {
                    me->StopMoving();
                    me->m_Events.KillAllEvents(true);

                    if (Creature* sunbeam = GetClosestCreatureWithEntry(me, SUNBEAM_DUMMY_ENTRY, 200.0f, true))
                    {
                        sunbeamGUID = sunbeam->GetGUID();

                        Movement::MoveSplineInit init(me);
                        init.MoveTo(sunbeam->GetPositionX(), sunbeam->GetPositionY(), sunbeam->GetPositionZ());
                        init.SetUncompressed();
                        init.Launch();
                    }

                    uint32 delay = 0;
                    me->m_Events.Schedule(delay += me->GetSplineDuration(), 5, [this]()
                    {
                        if (!hasDied)
                            DoCast(me, SPELL_DARK_EXPLOSION, true);

                        if (Creature* sunbeam = ObjectAccessor::GetCreature(*me, sunbeamGUID))
                            sunbeam->DespawnOrUnsummon();;

                        me->DespawnOrUnsummon();
                    });
                }
            }
        
            void UpdateAI(uint32 /*diff*/) override { }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dark_of_nightAI(creature);
        }
};

// Light of Day 63337
class npc_light_of_day : public CreatureScript
{
    public:
        npc_light_of_day() : CreatureScript("npc_light_of_day") { }

        struct npc_light_of_dayAI : public ScriptedAI
        {
            npc_light_of_dayAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->CastSpell(me, SPELL_LIGHT_OF_DAY_COSMETIC, false);
            }

            void OnSpellClick(Unit* clicker, bool& /*result*/) override
            {
                clicker->CastSpell(clicker, SPELL_LIGHT_OF_DAY_BUFF, false);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->DespawnOrUnsummon(1500);
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_light_of_dayAI(creature);
        }
};

// 64443 - Enchanted Plant
struct npc_enchanted_plant : public ScriptedAI
{
    npc_enchanted_plant(Creature* c) : ScriptedAI(c) { }

    void Reset() override
    {
        if (me->GetMap()->GetDifficulty() != RAID_DIFFICULTY_25MAN_LFR)
            me->GetMap()->SetWorldState(WORLD_STATE_NO_LFR, 1);
        me->GetMap()->SetWorldState(WORLD_STATE_WHOS_GOT_TWO_GREEN_THUMBS, 1);
    }

    void UpdateAI(uint32 diff) override { }

    void JustDied(Unit*) override
    {
        me->GetMap()->SetWorldState(WORLD_STATE_WHOS_GOT_TWO_GREEN_THUMBS, 0);
    }
};

// Dark of night cosmetic 123742
class spell_tsulong_dark_of_night : public SpellScriptLoader
{
    public:
        spell_tsulong_dark_of_night() : SpellScriptLoader("spell_tsulong_dark_of_night") { }

        class spell_tsulong_dark_of_night_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tsulong_dark_of_night_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if([](WorldObject* target) -> bool
                {
                    if (Creature* creature = target->ToCreature())
                        return creature->GetEntry() != SUNBEAM_DUMMY_ENTRY;

                    return true;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tsulong_dark_of_night_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tsulong_dark_of_night_SpellScript();
        }
};

class SunBreathTargetSelector
{
    public:
        SunBreathTargetSelector() { }

        bool operator()(WorldObject* object)
        {
            return object && object->ToPlayer();
        }
};

// Sun Breath 122855
class spell_tsulong_sun_breath : public SpellScriptLoader
{
    public:
        spell_tsulong_sun_breath() : SpellScriptLoader("spell_tsulong_sun_breath") { }

        class spell_tsulong_sun_breath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tsulong_sun_breath_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (target->ToPlayer())
                        {
                            PreventHitAura();
                            caster->AddAura(SPELL_BATHED_IN_LIGHT, target);
                        }
                    }
                }
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(SunBreathTargetSelector());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_tsulong_sun_breath_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tsulong_sun_breath_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_110);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tsulong_sun_breath_SpellScript();
        }
};

class SummonShaTargetSelector
{
    public:
        SummonShaTargetSelector() { }

        bool operator()(WorldObject* object)
        {
            return object && object->GetEntry() != NPC_UNSTABLE_SHA_TARGET;
        }
};

// Summon Sha Periodic Effect 122952
class spell_tsulong_summon_sha_periodic_effect : public SpellScriptLoader
{
    public:
        spell_tsulong_summon_sha_periodic_effect() : SpellScriptLoader("spell_tsulong_summon_sha_periodic_effect") { }

        class spell_tsulong_summon_sha_periodic_effect_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tsulong_summon_sha_periodic_effect_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(SummonShaTargetSelector());

                if (Unit* caster = GetCaster())
                {
                    if (InstanceScript* instance = caster->GetInstanceScript())
                    {
                        uint32 maxVictims = instance->instance->Is25ManRaid() ? 8 : 3;

                        if (targets.size() > maxVictims)
                            Trinity::Containers::RandomResizeList(targets, maxVictims);
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tsulong_summon_sha_periodic_effect_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tsulong_summon_sha_periodic_effect_SpellScript();
        }
};

// Terrorize Tsulong 123012
class spell_terrorize_tsulong : public SpellScript
{
    PrepareSpellScript(spell_terrorize_tsulong);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            if (!target->HasAura(SPELL_GOLD_ACTIVE))
                PreventHitAura();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_terrorize_tsulong::HandleDummy, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Unstable Bolt 122881
class spell_tsulong_unstable_bolt : public SpellScript
{
    PrepareSpellScript(spell_tsulong_unstable_bolt);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_UNSTABLE_BOLT_SACRIFICE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_tsulong_unstable_bolt::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Unstable Bolt Sacrifice 122907
class spell_tsulong_unstable_bolt_sacrifice : public SpellScript
{
    PrepareSpellScript(spell_tsulong_unstable_bolt_sacrifice);

    void DealDamage()
    {
        if (Unit* caster = GetCaster())
        {
            if (const SpellInfo* sInfo = GetSpellInfo())
            {
                int32 bp = sInfo->Effects[EFFECT_0].BasePoints;
                SetHitDamage((caster->GetMaxHealth() * bp) / 100);
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_tsulong_unstable_bolt_sacrifice::DealDamage);
    }
};

void AddSC_boss_tsulong()
{
    new boss_tsulong();
    new npc_sunbeam();
    new spell_dread_shadows_damage();
    new spell_dread_shadows_malus();
    new spell_script<spell_sunbeam>("spell_sunbeam");
    new aura_script<spell_sunbeam_aura>("spell_sunbeam_aura");
    new spell_tsulong_sha_regen();
    new spell_terrorize_periodic_player();
    new spell_tsulong_terrorize();
    new spell_tsulong_nightmares();
    new spell_script<spell_dark_of_night_fixate>("spell_dark_of_night_fixate");
    new npc_embodied_terror();
    new npc_fright_spawn();
    new npc_unstable_sha();
    new npc_dark_of_night();
    new npc_light_of_day();
    new creature_script<npc_enchanted_plant>("npc_enchanted_plant");
    new spell_tsulong_dark_of_night();
    new spell_tsulong_sun_breath();
    new spell_tsulong_summon_sha_periodic_effect();
    new spell_script<spell_terrorize_tsulong>("spell_terrorize_tsulong");
    new spell_script<spell_tsulong_unstable_bolt>("spell_tsulong_unstable_bolt");
    new spell_script<spell_tsulong_unstable_bolt_sacrifice>("spell_tsulong_unstable_bolt_sacrifice");
}
