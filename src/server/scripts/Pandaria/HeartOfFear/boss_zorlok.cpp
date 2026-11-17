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
#include "heart_of_fear.h"
#include "PassiveAI.h"

enum Spells
{
    SPELL_ATTENUATION           = 122440,
    SPELL_CONVERT               = 122740,
    SPELL_INHALE                = 122852,
    SPELL_EXHALE                = 122761,
    SPELL_EXHALE_DMG            = 122760,
    SPELL_PHEROMONES_OF_ZEAL    = 123812,
    SPELL_PHEROMONES_CLOUD      = 123811,
    SPELL_FORCE_AND_VERVE       = 122713,
    SPELL_NOISE_CANCELLING      = 122706,
    SPELL_MISSILE_NOISE_CANC    = 122707,
    SPELL_SONG_OF_THE_EMPRESS   = 123791,
    SPELL_SE_MELEE              = 132236,
    SPELL_SONIC_RING_VISUAL     = 122334,
    SPELL_SONIC_RING_AURA       = 122336,
    SPELL_INHALE_PHEROMONES     = 124018,
    SPELL_REINFORCE             = 123833,
    SPELL_BERSERK               = 47008,  // 120207,
    SPELL_KICK_PLAYER_INTO_PLAY = 129353,
    SPELL_ZEALOUS_PARASITE      = 125785,
    SPELL_OVERZEALOUS_CRITERIA  = 125782,

    //Heroic
    SPELL_SONIC_PULSE           = 124668, // aura with proc
    SPELL_ECHO_OF_ZORLOK        = 127496,
    SPELL_ECHO_OF_ATTENUATION   = 127541,
    SPELL_ECHO_OF_FAV           = 127542,
    SPELL_ECHO_OF_ATTENUATION_2 = 127543, // same like 127541 but for 4td phase
};

enum Events
{
    EVENT_INHALE                = 1,
    EVENT_EXHALE                = 2,
    EVENT_BERSERK               = 3,
    EVENT_ATTENUATION           = 4,
    EVENT_SUMMON_RINGS          = 5,
    EVENT_SUMMON_RINGS1         = 6,
    EVENT_SUMMON_RINGS2         = 7,
    EVENT_PHEROMONES_CLOUD      = 8,
    EVENT_FORCE_AND_VERVE       = 9,
    EVENT_CAST_FANDV            = 10,
    EVENT_CONVERT               = 11,
    EVENT_PULL_RAID             = 12,
    EVENT_SONIC_CHECK_POSITION  = 13,
    EVENT_SONIC_MOVE            = 14,
    EVENT_ATTENUATION_CANCEL    = 15,
    EVENT_SONG_OF_EMPRESS_MELEE = 16, // if zorlok at platform, and players leave melee range, he must begun cast Song of Empress
    EVENT_SUMMON_ZEALOUS_BUGS   = 17,
    EVENT_REMOVE_INHALE_STACK   = 18,
};

enum ePlatforms
{
    PLATFORM_ZORLOK_SW          = 1,    // Platform South-west (Force and verve)
    PLATFORM_ZORLOK_NE          = 2,    // Platform North-east (attenuation)
    PLATFORM_ZORLOK_SE          = 3,    // Platform South-east (Convert)
};

enum eTalk
{
    TALK_AGGRO                  = 1,
    TALK_DEATH                  = 2,
    TALK_EVENT_IDLE_1A          = 3,    // 1st phase of trash mobs
    TALK_EVENT_IDLE_1B          = 4,
    TALK_EVENT_IDLE_1C          = 5,
    TALK_EVENT_IDLE_2           = 6,    // 2nd phase of trash mobs
    TALK_EVENT_IDLE_3           = 7,    // 3rd phase of trash mobs
    TALK_EVENT_IDLE_4           = 8,    // 4th phase of trash mobs
    TALK_EVENT_PHASE_1          = 9,
    TALK_EVENT_PHASE_2          = 10,
    TALK_EVENT_PHASE_3          = 11,
    TALK_EVENT_TRASH_A_COMBAT   = 12,
    TALK_EVENT_TRASH_A_DIES     = 13,
    TALK_EVENT_TRASH_B_COMBAT   = 14,
    TALK_EVENT_TRASH_B_DIES     = 15,
    TALK_EVENT_TRASH_C_COMBAT   = 16,
    TALK_EVENT_TRASH_C_DIES     = 17,
    TALK_SLAY_01                = 18,   // Killing a player
    TALK_SLAY_02                = 19,
    TALK_EXHALE                 = 20,
    TALK_INHALE                 = 21,
    TALK_CONVERT                = 22,
    TALK_PITCH                  = 23,   // Echoes of power
    TALK_FLY_ANN                = 24,
    TALK_INHALE_ZEAL_ANN        = 25,
    TALK_INHALE_ANN             = 26,
    TALK_CONVERT_ANN            = 27,
    TALK_FORCE_AND_VERVE_ANN    = 28,
};

enum Actions
{
    ACTION_SUMMON_SONIC_RINGS   = 1,
    ACTION_SONIC_CARD           = 2,
    ACTION_SONIC_QUAD1          = 3,
    ACTION_SONIC_QUAD2          = 4,
    ACTION_INHALE_PHEROMONES    = 5,
    ACTION_WIPE                 = 6,
    ACTION_NEXT_POS             = 7, // using by pre combat event
    ACTION_SECOND_PHASE         = 8,
    ACTION_REMOVE_SONIC         = 9,
};

enum Phases
{
    PHASE_ZORLOK1               = 1,
    PHASE_ZORLOK2               = 4, // value '4' needed, don't change it!
};

enum
{
    NPC_ZEALOUS_BUG     = 64405,
    ZEALOUS_BUGS_COUNT  = 8,
};

static Position const zealousBugPosition[ZEALOUS_BUGS_COUNT]
{
    { -2277.438f, 295.3924f, 406.6288f },
    { -2268.050f, 297.6649f, 406.6288f },
    { -2236.003f, 265.5764f, 406.6288f },
    { -2237.828f, 254.5243f, 406.6288f },
    { -2269.385f, 223.0087f, 406.6288f },
    { -2280.071f, 222.0573f, 406.6288f },
    { -2313.325f, 253.0087f, 406.6287f },
    { -2309.958f, 262.9740f, 406.6287f },
};

bool HasCorrectRangeForNoise(Creature* me, float x, float y, float z, std::list<Position> prev, ObjectGuid m_owner)
{
    Position pos = { x, y, z, frand(0, 2 * M_PI) };
    bool m_available = true;
    Unit* caster = ObjectAccessor::GetUnit(*me, m_owner);

    if (!caster)
        return m_available;

    std::list <Creature*> Wall;
    GetCreatureListWithEntryInGrid(Wall, caster, NPC_GENERAL_BUNNY_JMF, 100.0f);

    if (Wall.empty())
    {
        for (auto&& itr : prev)
            if (itr.GetExactDist2d(pos.GetPositionX(), pos.GetPositionY()) < 7.5f)
                m_available = false;

        return m_available;
    }

    for (auto&& itr : Wall)
    {
        for (auto&& jitr : prev)
        {
            if (itr->GetExactDist2d(pos.GetPositionX(), pos.GetPositionY()) < 1.0f || jitr.GetExactDist2d(pos.GetPositionX(), pos.GetPositionY()) < 7.5f)
                m_available = false;
        }
    }

    return m_available;
}

// like converter from degrees to rad 
float CalculateRealEquation(float angle)
{
    return (float)(angle / M_PI_DEGREES)*(M_PI);
}

// Zorlok - 62980
// Echo of Attenuation - 65173
// Echo of Force and Verve - 65174
class boss_zorlok : public CreatureScript
{
    public:
        boss_zorlok() : CreatureScript("boss_zorlok") { }
    

        struct boss_zorlokAI : public BossAI
        {
            boss_zorlokAI(Creature* creature) : BossAI(creature, DATA_ZORLOK) { }

            EventMap events, nonCombatEvents;
            uint32 wp, phase, CurrentEvent, exhaleTarget, PlatformMod, sonicSpirals, delay, sonicValue;
            bool PreCombatPhase, CanCombat, clocksideRings, TargetInLos, InFlyOrAfterLanding, lastEcho, inPulse;
            std::map <uint32, uint32> PlatformDict, m_RepEvents;
            std::list<Position> PredNoise;
            ObjectGuid targetGUID;
            float NoiseMod, m_pulse_ori_right, m_pulse_ori_left, sonicStep;

            ObjectGuid zealousBugs[ZEALOUS_BUGS_COUNT];

            void InitializeAI() override
            {
                me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, ZARTHIK_2H_WEAPON);
                // Set Flying
                me->SetCanFly(true);
                me->SetSpeed(MOVE_FLIGHT, 4.5f);
                me->SetAnimTier(AnimTier::Hover);
                me->SetDisableGravity(true);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                PreCombatPhase = false;
                CanCombat = false;
                TargetInLos = false;
                InFlyOrAfterLanding = true;
                lastEcho = false;
                inPulse = false;
                wp          = 4;
                PlatformMod = 0;
                targetGUID = ObjectGuid::Empty;
                delay       = 0;
                NoiseMod    = 2.5f;
                // Platform Dictionary
                PlatformDict.insert(std::pair<uint32, uint32>(PLATFORM_ZORLOK_SW, EVENT_FORCE_AND_VERVE));
                PlatformDict.insert(std::pair<uint32, uint32>(PLATFORM_ZORLOK_NE, EVENT_ATTENUATION));
                PlatformDict.insert(std::pair<uint32, uint32>(PLATFORM_ZORLOK_SE, EVENT_CONVERT));
                // Repeatable events (second phase)
                m_RepEvents.insert(std::pair<uint32, uint32>(EVENT_FORCE_AND_VERVE, 0));
                m_RepEvents.insert(std::pair<uint32, uint32>(EVENT_ATTENUATION,     0));
                m_RepEvents.insert(std::pair<uint32, uint32>(EVENT_CONVERT,         0));
                SetCombatMovement(false); // zorlok doesn`t move at platform

                delay = 0;
                me->m_Events.Schedule(delay += 2500, 20, [this]()
                {
                    // If PreEvent has Done
                    if (instance->GetData(DATA_ZORLOK) > DONE && me->IsAlive())
                    {
                        PreCombatPhase = true;

                        Movement::MoveSplineInit init(me);
                        init.MoveTo(ZorlokWaypoints[wp - 1].GetPositionX(), ZorlokWaypoints[wp - 1].GetPositionY(), ZorlokWaypoints[wp - 1].GetPositionZ());

                        init.SetUncompressed();
                        init.Launch();

                        delay = 0;
                        me->m_Events.Schedule(delay += me->GetSplineDuration(), 30, [this]()
                        {
                            me->StopMoving();
                            PreCombatPhase = false;
                            CanCombat = true;
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->SetFacingTo(Position::NormalizeOrientation(me->GetOrientation() - M_PI));
                            me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());

                            if (instance->GetData(DATA_ZORLOK) != SPECIAL) // save state if event done
                                instance->SetData(DATA_ZORLOK, SPECIAL);
                        });
                    }
                });

                // Temp Hackfix
                if (instance && instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
                {
                    // should be 174.5m in 10 normal
                    me->SetCreateHealth(174500000);
                    me->SetMaxHealth(174500000);
                    me->SetHealth(174500000);
                    me->ResetPlayerDamageReq();
                }

                std::fill(std::begin(zealousBugs), std::end(zealousBugs), ObjectGuid::Empty);
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
                // Make sure we can target zorlok
                me->SetCanFly(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                me->SetAnimTier(AnimTier::Hover);
                me->SetDisableGravity(true);
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveAllAreasTrigger();
                RemoveWalls();
                exhaleTarget = 0;
                sonicSpirals = 0;
                CurrentEvent = 0;
                targetGUID = ObjectGuid::Empty;
                delay        = 0;
                clocksideRings = true;
                lastEcho = false;
                inPulse = false;

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                // Temp Hackfix
                if (instance && instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
                {
                    // should be 174.5m in 10 normal
                    me->SetCreateHealth(174500000);
                    me->SetMaxHealth(174500000);
                    me->SetHealth(174500000);
                    me->ResetPlayerDamageReq();
                }

                // Platform Dictionary
                PlatformDict.clear();
                PlatformDict.insert(std::pair<uint32, uint32>(PLATFORM_ZORLOK_SW, EVENT_FORCE_AND_VERVE));
                PlatformDict.insert(std::pair<uint32, uint32>(PLATFORM_ZORLOK_NE, EVENT_ATTENUATION));
                PlatformDict.insert(std::pair<uint32, uint32>(PLATFORM_ZORLOK_SE, EVENT_CONVERT));
                // Repeatable events (second phase)
                m_RepEvents.clear();
                m_RepEvents.insert(std::pair<uint32, uint32>(EVENT_FORCE_AND_VERVE, 0));
                m_RepEvents.insert(std::pair<uint32, uint32>(EVENT_ATTENUATION, 0));
                m_RepEvents.insert(std::pair<uint32, uint32>(EVENT_CONVERT, 0));
                PlatformMod = 0;

                // Remove charmed players
                HandleRemoveCharmedPlayers();
                HandlePullPlayers(true);
            }

            // --- Specific Functions ---
            void RemoveWalls()
            {
                std::list<GameObject*> m_wallList;

                GetGameObjectListWithEntryInGrid(m_wallList, me, GO_ARENA_WALLS, 200.0f);
                GetGameObjectListWithEntryInGrid(m_wallList, me, GO_FINAL_PHASE_WALLS, 200.0f);

                for (auto&& m_wall : m_wallList)
                    me->RemoveGameObject(m_wall, true);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void HandleRemoveCharmedPlayers(bool death = true)
            {
                std::list<Player*> m_targets;
                GetPlayerListInGrid(m_targets, me, 300.0f);

                for (auto&& itr : m_targets)
                    if (itr->IsCharmed())
                        death ? me->Kill(itr) : itr->RemoveCharmAuras();
            }

            void SetFlying(uint32 point, bool last = false)
            {
                me->AttackStop();
                me->SetCanFly(true);
                me->SetReactState(REACT_PASSIVE);
                me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                me->SetDisableGravity(true);
                me->SendMovementFlagUpdate();
                me->SetAnimTier(AnimTier::Hover);
                me->AddUnitMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);

                if (last)
                {
                    me->GetMotionMaster()->MovePoint(point, oratiumCenter[0]);
                    return;
                }
                me->GetMotionMaster()->MovePoint(point, zorlokPlatforms[point - 1]);
            }

            void SetLanding(bool choise = false)
            {
                me->SetCanFly(false);
                me->SetDisableGravity(false);
                me->RemoveUnitMovementFlag(MOVEMENTFLAG_FLYING);
                me->SetAnimTier(AnimTier::Ground);
                me->SendMovementFlagUpdate();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                me->SetReactState(REACT_AGGRESSIVE);

                if (!choise)
                {
                    events.ScheduleEvent(EVENT_INHALE, 15000);
                    events.ScheduleEvent(CurrentEvent, urand(15000, 25000));
                    if (!me->FindNearestPlayer(5.0f) && !me->HasUnitState(UNIT_STATE_CASTING))
                        me->CastSpell(me, SPELL_SONG_OF_THE_EMPRESS, false);
                }
            }

            bool HasAnyTrashInRange()
            {
                if (instance->GetData(DATA_ZORLOK) > DONE && me->IsAlive())
                    return true;

                Creature* ShieldMaster = GetClosestCreatureWithEntry(me, NPC_SRATHIK_SHIELD_MASTER, 20.0f, true);
                Creature* Zealok = GetClosestCreatureWithEntry(me, NPC_ZARTHIK_ZEALOT, 20.0f, true);
                Creature* Fanatic = GetClosestCreatureWithEntry(me, NPC_SETTHIK_FANATIC, 20.0f, true);
                Creature* BoneSmasher = GetClosestCreatureWithEntry(me, NPC_ENSLAVED_BONESMASHER, 20.0f, true);
                Creature* Supplicant = GetClosestCreatureWithEntry(me, NPC_ZARTHIK_SUPPLICANT, 20.0f, true);
                Creature* Supplicant2 = GetClosestCreatureWithEntry(me, NPC_ZARTHIK_SUPPLICANT_2, 20.0f, true);
                Creature* Supplicant3 = GetClosestCreatureWithEntry(me, NPC_ZARTHIK_SUPPLICANT_3, 20.0f, true);

                if (Supplicant3 || Supplicant2 || Supplicant || BoneSmasher || Fanatic || Zealok || ShieldMaster)
                    return true;

                return false;
            }

            void AreaAuraController()
            {
                std::list<Player*> PlayersInArea;
                GetPlayerListInGrid(PlayersInArea, me, 200.0f);
                for (auto&& itr : PlayersInArea)
                {
                    // Remove convert aura for players who have less than 50% remaining health
                    if (itr->HasAura(SPELL_CONVERT) && itr->HealthBelowPct(51))
                        itr->RemoveAurasDueToSpell(SPELL_CONVERT);
                }
            }

            void HandlePullPlayers(bool remove = false)
            {
                std::list<Creature*> PlayerKickerList;
                GetCreatureListWithEntryInGrid(PlayerKickerList, me, NPC_PLAYER_KICKER, 200.0f);

                for (auto&& m_kicker : PlayerKickerList)
                {
                    if (remove)
                        m_kicker->RemoveAurasDueToSpell(SPELL_KICK_PLAYER_INTO_PLAY);
                    else
                        m_kicker->AddAura(SPELL_KICK_PLAYER_INTO_PLAY, m_kicker);
                }
            }

            void SelectNewPlatform()
            {
                if (PlatformDict.empty())
                    return;

                if (IsHeroic())
                {
                    uint32 val = PlatformDict.begin()->first; // blizz hotfix sw->ne->se, same heroic

                    if (PlatformMod > 1)
                        CreateEchoOfType(CurrentEvent);
                    CurrentEvent = PlatformDict.find(val)->second;

                    PlatformDict.erase(val);
                    SetFlying(val);

                }
                else
                {
                    auto itr = Trinity::Containers::SelectRandomContainerElement(PlatformDict);
                    uint32 val = itr.first;
                    CurrentEvent = PlatformDict.find(val)->second;
                    PlatformDict.erase(val);
                    SetFlying(val);
                }

            }

            // Select type and summon Echo of Power on 1st and 2td platforms
            void CreateEchoOfType(uint32 m_type)
            {
                uint32 m_echo = 0;

                switch (m_type)
                {
                    case EVENT_ATTENUATION:
                        m_echo = SPELL_ECHO_OF_ATTENUATION;
                        break;
                    case EVENT_FORCE_AND_VERVE:
                        m_echo = SPELL_ECHO_OF_FAV;
                        break;
                }

                if (m_echo == 0)
                    return;

                DoCast(me, m_echo);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                _EnterEvadeMode();
                BossAI::EnterEvadeMode();
                summons.DespawnAll();
                me->m_Events.KillAllEvents(true);
                HandleRemoveCharmedPlayers();
                HandlePullPlayers(true);

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CONVERT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FORCE_AND_VERVE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ZEALOUS_PARASITE);

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_ZORLOK, FAIL);
                }

                _DespawnAtEvade();
                me->GetMap()->CreatureRelocation(me, ZorlokWaypoints[3].GetPositionX(), ZorlokWaypoints[3].GetPositionY(), ZorlokWaypoints[3].GetPositionZ(), Position::NormalizeOrientation(ZorlokWaypoints[3].GetOrientation() + M_PI));

                DespawnZealousBugs();
            }

            void JustDied(Unit* /*killer*/) override
            {
                HandleAchievementOverzealous();
                DespawnZealousBugs();

                events.Reset();
                summons.DespawnAll();
                me->RemoveAllAreasTrigger();

                me->SetCanFly(false);
                me->SetDisableGravity(true);
                RemoveWalls();
                // Remove charmed players
                HandleRemoveCharmedPlayers(false);
                HandlePullPlayers(true);

                Talk(TALK_DEATH);

                if (instance)
                {
                    instance->SetBossState(DATA_ZORLOK, DONE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CONVERT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FORCE_AND_VERVE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ZEALOUS_PARASITE);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                _JustDied();
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                if (instance)
                    instance->SetBossState(DATA_ZORLOK, FAIL);
            }

            void JustEngagedWith(Unit* who) override
            {
                _JustEngagedWith();
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                Talk(TALK_AGGRO);
                me->CastSpell(tabCenter.GetPositionX(), tabCenter.GetPositionY(), tabCenter.GetPositionZ(), SPELL_PHEROMONES_CLOUD, false);
                Talk(TALK_EVENT_IDLE_3);
                phase = PHASE_ZORLOK1;
                // not require to damage taken from player for init boss actions
                DamageTaken(who, phase);
                // 10 minutes enrage timer
                events.ScheduleEvent(EVENT_BERSERK, 600000);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(TALK_SLAY_01 + urand(0, 1));
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                // Removing song of the empress
                if (me->GetDistance(attacker) < 5.0f)
                {
                    uint32 spell = me->GetCurrentSpell(CURRENT_CHANNELED_SPELL) ? me->GetCurrentSpell(CURRENT_CHANNELED_SPELL)->m_spellInfo->Id : 0;
                    if (me->HasAura(SPELL_SONG_OF_THE_EMPRESS) || spell == SPELL_SONG_OF_THE_EMPRESS || spell == SPELL_SE_MELEE)
                    {
                        if (Unit* victim = me->GetVictim())
                            if (!victim->IsWithinMeleeRange(me) && TargetInLos) // interrupt only if our victim available for attacks
                                return;

                        me->RemoveAurasDueToSpell(SPELL_SONG_OF_THE_EMPRESS);
                        me->InterruptNonMeleeSpells(true, SPELL_SONG_OF_THE_EMPRESS);
                        me->CastStop(SPELL_SONG_OF_THE_EMPRESS);
                        AttackStart(attacker);
                        me->SetInCombatWith(attacker);
                        TargetInLos = false;
                        InFlyOrAfterLanding = false;
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                // Hotfix was remove
                if (me->HealthBelowPctDamaged(20, damage) && IsHeroic() && !lastEcho)
                {
                    lastEcho = true;
                    DoCast(me, SPELL_ECHO_OF_ATTENUATION_2);
                }

                if (me->HealthBelowPctDamaged(100 - 20 * PlatformMod, damage) && phase == PHASE_ZORLOK1)
                {
                    PlatformMod++;
                    events.CancelEvent(CurrentEvent);
                    events.CancelEvent(EVENT_INHALE);
                    SelectNewPlatform();
                    InFlyOrAfterLanding = true;
                }

                if (me->HealthBelowPct(40) && phase == PHASE_ZORLOK1)
                {
                    phase = PHASE_ZORLOK2;
                    SetFlying(8, true);
                    events.ScheduleEvent(EVENT_SUMMON_ZEALOUS_BUGS, 1);
                }
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_NEXT_POS:
                    {
                        PreCombatPhase = true;

                        delay = 0;
                        Movement::MoveSplineInit init(me);
                        init.MoveTo(ZorlokWaypoints[wp - 4].GetPositionX(), ZorlokWaypoints[wp - 4].GetPositionY(), ZorlokWaypoints[wp - 4].GetPositionZ());

                        init.SetUncompressed();
                        init.Launch();

                        delay = 0;
                        me->m_Events.Schedule(delay += me->GetSplineDuration(), 30, [this]()
                        {
                            me->StopMoving();
                            PreCombatPhase = false;
                            wp++;

                            if (wp == 8)
                            {
                                CanCombat = true;
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                me->SetReactState(REACT_AGGRESSIVE);
                                me->SetFacingTo(Position::NormalizeOrientation(me->GetOrientation() - M_PI));
                                me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());

                                if (instance->GetData(DATA_ZORLOK) != SPECIAL) // save state if event done
                                    instance->SetData(DATA_ZORLOK, SPECIAL);
                                return;
                            }
                            else
                                Talk(wp - 2);

                            if (Player* itr = me->FindNearestPlayer(150.0f))
                                me->SetFacingTo(me->GetAngle(itr));
                        });
                        break;
                    }
                    case ACTION_SONIC_CARD:
                    {
                        inPulse = true;
                        sonicStep = 0.0f;
                        sonicValue = 0;

                        scheduler
                            .CancelAll()
                            .SetValidator([this] { return inPulse; })
                            .Schedule(Milliseconds(10), [this](TaskContext ringEvent)
                        {
                            for (uint32 i = 0; i < 5; i++)
                                me->SummonCreature(NPC_SONIC_RING_FORWARD, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), Position::NormalizeOrientation(CalculateRealEquation((float)72.0f * i + sonicStep)), TEMPSUMMON_MANUAL_DESPAWN);

                            if (clocksideRings)
                                sonicStep += 45.0f;
                            else
                                sonicStep -= 45.0f;

                            ringEvent.Repeat(Milliseconds(800));
                        });

                        // Only in heroic
                        if (IsHeroic())
                        {
                            inPulse = true;

                            if (clocksideRings)
                            {
                                m_pulse_ori_left = -30.0f;
                                m_pulse_ori_right = 150.0f;
                            }
                            else
                            {
                                m_pulse_ori_left = 30.0f;
                                m_pulse_ori_right = -150.0f;
                            }

                            scheduler
                                .SetValidator([this] { return inPulse; })
                                .Schedule(Milliseconds(500), [this](TaskContext pulseEvent)
                            {
                                if (clocksideRings)
                                {
                                    m_pulse_ori_left -= 30.0f;
                                    m_pulse_ori_right -= 30.0f;
                                }
                                else
                                {
                                    m_pulse_ori_left += 30.0f;
                                    m_pulse_ori_right += 30.0f;
                                }
                                me->SummonCreature(NPC_SONIC_PULSE_40LEFT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), Position::NormalizeOrientation(CalculateRealEquation(m_pulse_ori_left)), TEMPSUMMON_MANUAL_DESPAWN);
                                me->SummonCreature(NPC_SONIC_PULSE_40LEFT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), Position::NormalizeOrientation(CalculateRealEquation(m_pulse_ori_right)), TEMPSUMMON_MANUAL_DESPAWN);
                                pulseEvent.Repeat(Milliseconds(700));
                            });
                        }
                        break;
                    }
                    case ACTION_INHALE_PHEROMONES:
                    {
                        me->RemoveAreaTrigger(SPELL_PHEROMONES_CLOUD);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PHEROMONES_OF_ZEAL);
                        break;
                    }
                    case ACTION_SECOND_PHASE:
                    {
                        events.CancelEvent(EVENT_INHALE);
                        Talk(TALK_EVENT_PHASE_2);
                        events.ScheduleEvent(ChooseAction(), urand(25000, 35000));
                        events.ScheduleEvent(EVENT_PULL_RAID, 5000);
                        me->GetMotionMaster()->MoveLand(PHASE_ZORLOK2 + 10, oratiumCenter[1]);
                        SetLanding(true);
                        break;
                    }
                    case ACTION_WIPE:
                    {
                        events.Reset();
                        summons.DespawnAll();

                        me->RemoveAllAreasTrigger();
                        me->RemoveAllAuras();
                        me->SetFullHealth();
                        me->SetReactState(REACT_PASSIVE);

                        PlatformDict.clear();
                        sonicSpirals = 0;
                        exhaleTarget = 0;
                        clocksideRings = true;
                        RemoveWalls();

                        if (instance)
                        {
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PHEROMONES_CLOUD);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CONVERT);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_NOISE_CANCELLING);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FORCE_AND_VERVE);

                            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                        }

                        //SetFlying();
                        break;
                    }
                    case ACTION_REMOVE_SONIC:
                    {
                        events.CancelEvent(EVENT_SUMMON_RINGS);
                        events.CancelEvent(EVENT_SUMMON_RINGS1);
                        events.CancelEvent(EVENT_SUMMON_RINGS2);
                        inPulse = false;
                        break;
                    }
                    default:
                        break;
                }
            }

            // At second phase we should select random events, but each event couldn`t repeat more then 2 times
            uint32 ChooseAction()
            {
                std::vector<uint32> choice = { EVENT_FORCE_AND_VERVE, EVENT_ATTENUATION, EVENT_CONVERT };
                uint32 m_action;

                for (auto&& itr : choice)
                {
                    if (m_RepEvents.find(itr)->second >= 2)
                    {
                        choice.erase(std::find(choice.begin(), choice.end(), itr));
                        m_RepEvents.find(itr)->second = 0;
                    }
                }

                m_action = Trinity::Containers::SelectRandomContainerElement(choice);
                m_RepEvents.find(m_action)->second++;

                return m_action;
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                wp++;

                switch (pointId)
                {
                    case 1:
                    case 2:
                    case 3:
                        SetLanding();
                        break;
                    case 8:
                        me->CastSpell(me, SPELL_INHALE_PHEROMONES, true);
                        Talk(TALK_INHALE_ZEAL_ANN);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PHEROMONES_CLOUD);
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_EXHALE_TARGET)
                    return exhaleTarget;

                switch (type)
                {
                    case TYPE_EXHALE_TARGET:
                        return exhaleTarget;
                    case TYPE_ZORLOK_LAST_ECHO:
                        return lastEcho ? 1 : 0;
                }

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_EXHALE_TARGET)
                    exhaleTarget = data;
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                if (!PreCombatPhase && !HasAnyTrashInRange() && !CanCombat)
                    me->AI()->DoAction(ACTION_NEXT_POS);

                if (!UpdateVictim() || !CanCombat)
                    return;

                AreaAuraController();
                events.Update(diff);
                scheduler.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        // All-time events
                        case EVENT_INHALE:
                        {
                            // Can't inhale if already casting
                            if (me->HasUnitState(UNIT_STATE_CASTING))
                                events.RescheduleEvent(EVENT_INHALE, 5000);
                            else
                            {
                                // Inhale (Exhale is triggered when Zor'lok has 3-4 stacks of inhale)
                                Aura* inhale = me->GetAura(SPELL_INHALE);
                                if (!inhale || inhale->GetStackAmount() < 3 || !urand((inhale->GetStackAmount() < 4 ? 0 : 1), 1))
                                {
                                    Talk(TALK_INHALE);
                                    Talk(TALK_INHALE_ANN);
                                    me->CastSpell(me, SPELL_INHALE, false);
                                }
                                // Exhale
                                else
                                {
                                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                    {
                                        exhaleTarget = target->GetGUID().GetCounter();
                                        Talk(TALK_EXHALE);

                                        if (Unit* vict = me->GetVictim())
                                            targetGUID = vict->GetGUID();

                                        me->PrepareChanneledCast(me->GetAngle(target), SPELL_EXHALE, true);

                                        delay = 0;
                                        me->m_Events.Schedule(delay += 6500, 11, [this]()
                                        {
                                            me->RemoveChanneledCast(targetGUID);
                                        });
                                        events.ScheduleEvent(EVENT_REMOVE_INHALE_STACK, 6000);
                                    }
                                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, TankSpecTargetSelector()))
                                    {
                                        exhaleTarget = target->GetGUID().GetCounter();
                                        Talk(TALK_EXHALE);

                                        if (Unit* vict = me->GetVictim())
                                            targetGUID = vict->GetGUID();

                                        me->PrepareChanneledCast(me->GetAngle(target), SPELL_EXHALE, true);

                                        delay = 0;
                                        me->m_Events.Schedule(delay += 6500, 11, [this]()
                                        {
                                            me->RemoveChanneledCast(targetGUID);
                                        });
                                        events.ScheduleEvent(EVENT_REMOVE_INHALE_STACK, 6000);
                                    }
                                }
                                events.ScheduleEvent(EVENT_INHALE, urand(25000, 50000));
                            }
                            break;
                        }
                        // Attenuation platform
                        case EVENT_ATTENUATION:
                        {
                            if (me->HasUnitState(UNIT_STATE_CASTING))
                            {
                                events.RescheduleEvent(EVENT_ATTENUATION, 10000);
                                return;
                            }

                            if (Unit* target = me->GetVictim())
                                targetGUID = target->GetGUID();

                            clocksideRings = clocksideRings ? false : true;

                            me->PrepareChanneledCast(0.0f);
                            me->CastSpell(me, SPELL_ATTENUATION, false);
                            me->AddUnitState(UNIT_STATE_CASTING);

                            uint32 action = (phase == PHASE_ZORLOK1 ? EVENT_ATTENUATION : ChooseAction());
                            events.ScheduleEvent(EVENT_ATTENUATION_CANCEL, 14500);
                            events.ScheduleEvent(action, 40000);
                            break;
                        }
                        // Convert platform
                        case EVENT_CONVERT:
                        {
                            if (me->HasUnitState(UNIT_STATE_CASTING))
                            {
                                events.RescheduleEvent(EVENT_CONVERT, 10000);
                                return;
                            }
                            Talk(TALK_CONVERT_ANN);
                            // Creating target list
                            Talk(TALK_CONVERT);
                            DoCast(SPELL_CONVERT);

                            uint32 action = (phase == PHASE_ZORLOK1 ? EVENT_CONVERT : ChooseAction());
                            events.ScheduleEvent(action, 40000);

                            break;
                        }
                        // Force and Verve platform
                        case EVENT_FORCE_AND_VERVE:
                        {
                            if (me->HasUnitState(UNIT_STATE_CASTING))
                            {
                                events.RescheduleEvent(EVENT_FORCE_AND_VERVE, 10000);
                                return;
                            }
                            // Creating Noise Cancelling zones
                            float x, y; // make first calculation
                            NoiseMod = 2.5f;
                            x = me->GetPositionX() + frand(-NoiseMod * 2.5f * 1, NoiseMod * 2.5f * 1); // less random spawn in melee zone
                            y = me->GetPositionY() + frand(-NoiseMod * 2.5f * 1, NoiseMod * 2.5f * 1);
                            me->CastSpell(x, y, me->GetPositionZ(), SPELL_MISSILE_NOISE_CANC, false);

                            PredNoise.push_back({ x, y, me->GetPositionZ(), me->GetOrientation() }); // save pos 

                            uint8 count = IsHeroic() ? 5 : 4;

                            for (int i = 2; i < count; ++i)
                            {
                                do 
                                {
                                    x = me->GetPositionX() + frand(-NoiseMod * 2.5f * i, NoiseMod * 2.5f * i); // less random spawn in melee zone
                                    y = me->GetPositionY() + frand(-NoiseMod * 2.5f * i, NoiseMod * 2.5f * i);
                                } 
                                // then check if our planned pos not in wall/texture or not beside prev noise cancelling.
                                // now we`ll check each used position over current.
                                while (!HasCorrectRangeForNoise(me, x, y, me->GetPositionZ(), PredNoise, me->GetGUID()));

                                PredNoise.push_back({ x, y, me->GetPositionZ(), me->GetOrientation() }); // save next position
                                me->CastSpell(x, y, me->GetPositionZ(), SPELL_MISSILE_NOISE_CANC, false);
                            }

                            NoiseMod = 2.5f;
                            me->AddUnitState(UNIT_STATE_CASTING);
                            events.ScheduleEvent(EVENT_CAST_FANDV, 2000);
                            PredNoise.clear(); // remove all pos for next schedule
                            break;
                        }
                        case EVENT_CAST_FANDV:
                        {
                            me->SetReactState(REACT_PASSIVE);
                            Talk(TALK_FORCE_AND_VERVE_ANN);
                            me->CastSpell(me, SPELL_FORCE_AND_VERVE, false);
                            uint32 action = (phase == PHASE_ZORLOK1 ? EVENT_FORCE_AND_VERVE : ChooseAction());
                            events.ScheduleEvent(action, 40000);
                            break;
                        }
                        case EVENT_BERSERK:
                        {
                            me->CastSpell(me, SPELL_BERSERK, false);
                            break;
                        }
                        case EVENT_PULL_RAID:
                        {
                            // Pulling far players
                            HandlePullPlayers();

                            // Creating Walls
                            for (uint8 i = 0; i < 3; ++i)
                            {
                                // Arena walls
                                me->SummonGameObject(GO_ARENA_WALLS, finalPhaseWalls2[i].GetPositionX(), finalPhaseWalls2[i].GetPositionY(), finalPhaseWalls2[i].GetPositionZ(), finalPhaseWalls2[i].GetOrientation(), { }, 7200);

                                // Final phase Doors
                                me->SummonGameObject(GO_FINAL_PHASE_WALLS, finalPhaseWalls1[i].GetPositionX(), finalPhaseWalls1[i].GetPositionY(), finalPhaseWalls1[i].GetPositionZ(), finalPhaseWalls1[i].GetOrientation(), { }, 7200);
                            }
                            break;
                        }
                        case EVENT_ATTENUATION_CANCEL:
                        {
                            me->SetReactState(REACT_AGGRESSIVE);

                            if (Unit* itr = ObjectAccessor::GetUnit(*me, targetGUID))
                                me->Attack(itr, true);
                            else if (Player* itr = me->FindNearestPlayer(100.0f))
                                me->Attack(itr, true);
                            break;
                        }
                        case EVENT_SONG_OF_EMPRESS_MELEE:
                        {
                            if (Unit* victim = me->GetVictim())
                            {
                                if (victim->IsWithinMeleeRange(me)) // double check, maybe player back to melee range?
                                {
                                    TargetInLos = false;
                                    return;
                                }
                                else
                                    me->CastSpell(me, SPELL_SE_MELEE, true); // if not, then cast song of empress
                            }
                            break;
                        }
                        case EVENT_SUMMON_ZEALOUS_BUGS:
                            SummonZealousBugs();
                            events.ScheduleEvent(EVENT_SUMMON_ZEALOUS_BUGS, 50 * IN_MILLISECONDS);
                            break;
                        case EVENT_REMOVE_INHALE_STACK:
                            me->RemoveAurasDueToSpell(SPELL_INHALE);
                            break;
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (Unit* victim = me->GetVictim())
                {
                    if (!victim->IsWithinMeleeRange(me) && !TargetInLos && !InFlyOrAfterLanding)
                    {
                        TargetInLos = true;
                        events.ScheduleEvent(EVENT_SONG_OF_EMPRESS_MELEE, 3500);
                    }
                }

                DoMeleeAttackIfReady();
            }

            void HandleAchievementOverzealous()
            {
                if (!instance)
                    return;

                for (auto&& itr : me->GetMap()->GetPlayers())
                    if (!itr.GetSource()->HasAura(SPELL_ZEALOUS_PARASITE))
                        return;

                instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2, SPELL_OVERZEALOUS_CRITERIA);
            }

            void SummonZealousBugs()
            {
                for (uint32 i = 0; i < ZEALOUS_BUGS_COUNT; ++i)
                    if (!zealousBugs[i])
                        me->SummonCreature(NPC_ZEALOUS_BUG, zealousBugPosition[i]);
            }

            void DespawnZealousBugs()
            {
                for (uint32 i = 0; i < ZEALOUS_BUGS_COUNT; ++i)
                {
                    if (!zealousBugs[i])
                        continue;

                    if (Creature* bug = ObjectAccessor::GetCreature(*me, zealousBugs[i]))
                        bug->DespawnOrUnsummon();
                    zealousBugs[i] = ObjectGuid::Empty;
                }
            }

            void OnZealousBugUsed(ObjectGuid bugGuid)
            {
                for (uint32 i = 0; i < ZEALOUS_BUGS_COUNT; ++i)
                    if (zealousBugs[i] == bugGuid)
                        zealousBugs[i] = ObjectGuid::Empty;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_zorlokAI(creature);
        }
};

// Sonic Ring - 62689, 62694, 62696, 62716, 62717, 62718, 62719, 62726, 62727, 62743, 62744, 62746
// Sonic Pulse - 63835, 63836, 63837
class npc_sonic_ring : public CreatureScript
{
    public:
        npc_sonic_ring() : CreatureScript("npc_sonic_ring") { }

        struct npc_sonic_ringAI : public ScriptedAI
        {
            npc_sonic_ringAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            float rateOri;
            float orientation;
            EventMap events;

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetDisplayId(DISPLAYID_INVISIBLE);
                me->GetEntry() >= 63835 ? me->CastSpell(me, SPELL_SONIC_PULSE, false) : me->CastSpell(me, SPELL_SONIC_RING_VISUAL, false);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                me->SetDisableGravity(true);
                me->SetCanFly(true);

                events.ScheduleEvent(EVENT_SONIC_CHECK_POSITION, 10000);

                if (Unit* zorlok = summoner)
                {
                    float zorOri = me->GetOrientation();
                    
                    // If needed, reset orientation into valid range [0, M_PI * 2]
                    me->SetFacingTo(Position::NormalizeOrientation(zorOri));

                    // Go to virtual destination
                    Position reachPoint = GetTargetPoint(zorOri, me->GetPositionZ());

                    me->SetWalk(true);
                    me->GetEntry() >= 63835 ? me->SetSpeed(MOVE_WALK, 1.25f) : me->SetSpeed(MOVE_WALK, 2.2f);
                    me->GetMotionMaster()->MovePoint(1, reachPoint);
                }
                else
                    me->DespawnOrUnsummon();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 1)
                    me->DespawnOrUnsummon();
            }

            Position GetTargetPoint(float orientation, float posZ)
            {
                /*
                * The main idea is : a circle has 4 quarters; the principle is to define a point at the limit of the area the sonic ring can move,
                * and use this point as a destination for MovePoint(). To calculate this point, we use the orientation to get a ratio between X and Y:
                * if orientation is 0, we make 100% on x-axis, if orientation is pi/2, we make 100% on y-axis, and if orientation is pi/4, we make
                * 50% on x-axis and 50% on y-axis.
                *
                * The range orientation from 0 to pi/2 represents a quarter circle where x and y will be both positives, and we use this quarter circle
                * to determine general ratio between x and y. Then, we just have to "rotate" to apply this to the right orientation. According to this
                * initial orientation, we may need to switch x and y ratio (when turned on left or right, moving forward is moving on y-axis, and not
                * on x-axis, for instance), and/or to apply negatives values (if orientation is pi, we're moving backwards, so the x-value decreases,
                * while if orientation is 0.0, we're moving forwards, and so, the x-value increases, but we're still on the same axis).
                */
                
                float pointX = 0, pointY = 0;
                GetPositionWithDistInOrientation(me, 80.0f, orientation, pointX, pointY);

                Position reachPoint = { pointX, pointY, posZ, orientation };
                return reachPoint;
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_SONIC_CHECK_POSITION:
                    {
                        me->GetEntry() >= 63835 ? me->RemoveAura(SPELL_SONIC_PULSE) : me->RemoveAura(SPELL_SONIC_RING_VISUAL);
                        me->DespawnOrUnsummon(1000);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sonic_ringAI(creature);
        }
};

// Echo of Power - 65173, 65174
class npc_zorlok_echo_of_power : public CreatureScript
{
    public:
        npc_zorlok_echo_of_power() : CreatureScript("npc_zorlok_echo_of_power") { }

        struct npc_zorlok_echo_of_powerAI : public ScriptedAI
        {
            npc_zorlok_echo_of_powerAI(Creature* creature) : ScriptedAI(creature) 
            {
                instance = me->GetInstanceScript();

                // Last echo should move
                if (Creature* zorlok = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_ZORLOK) : ObjectGuid::Empty))
                    if (!zorlok->AI()->GetData(TYPE_ZORLOK_LAST_ECHO))
                        SetCombatMovement(false);

                TargetInLos = false;
            }

            InstanceScript* instance;
            TaskScheduler scheduler;
            EventMap events;
            uint32 m_eventType, sonicValue;
            bool clocksideRings, TargetInLos, inPulse;
            ObjectGuid targetGUID;
            float NoiseMod, m_pulse_ori_right, m_pulse_ori_left, sonicStep;
            std::list<Position> PredNoise;

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, ZARTHIK_2H_WEAPON);
                me->CastSpell(me, SPELL_ECHO_OF_ZORLOK, false);
                DoZoneInCombat();
                inPulse = false;

                switch (me->GetEntry())
                {
                    case NPC_ECHO_OF_ATTENUATION:
                        m_eventType = EVENT_ATTENUATION;
                        break;
                    case NPC_ECHO_OF_FORCE_AND_VERVE:
                        m_eventType = EVENT_FORCE_AND_VERVE;
                        break;
                }

                events.ScheduleEvent(m_eventType, 25 * IN_MILLISECONDS);

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_SONIC_CARD:
                    {
                        inPulse = true;
                        sonicStep = 0.0f;
                        sonicValue = 0;

                        scheduler
                            .CancelAll()
                            .SetValidator([this] { return inPulse; })
                            .Schedule(Milliseconds(10), [this](TaskContext ringEvent)
                        {
                            for (uint32 i = 0; i < 5; i++)
                                me->SummonCreature(NPC_SONIC_RING_FORWARD, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), Position::NormalizeOrientation(CalculateRealEquation((float)72.0f * i + sonicStep)), TEMPSUMMON_MANUAL_DESPAWN);

                            if (clocksideRings)
                                sonicStep += 45.0f;
                            else
                                sonicStep -= 45.0f;

                            ringEvent.Repeat(Milliseconds(800));
                        });

                        // Only Heroic mode
                        if (IsHeroic())
                        {
                            inPulse = true;

                            if (clocksideRings)
                            {
                                m_pulse_ori_left = -30.0f;
                                m_pulse_ori_right = 150.0f;
                            }
                            else
                            {
                                m_pulse_ori_left = 30.0f;
                                m_pulse_ori_right = -150.0f;
                            }

                            scheduler
                                .SetValidator([this] { return inPulse; })
                                .Schedule(Milliseconds(500), [this](TaskContext pulseEvent)
                            {
                                if (clocksideRings)
                                {
                                    m_pulse_ori_left -= 30.0f;
                                    m_pulse_ori_right -= 30.0f;
                                }
                                else
                                {
                                    m_pulse_ori_left += 30.0f;
                                    m_pulse_ori_right += 30.0f;
                                }
                                me->SummonCreature(NPC_SONIC_PULSE_40LEFT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), Position::NormalizeOrientation(CalculateRealEquation(m_pulse_ori_left)), TEMPSUMMON_MANUAL_DESPAWN);
                                me->SummonCreature(NPC_SONIC_PULSE_40LEFT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), Position::NormalizeOrientation(CalculateRealEquation(m_pulse_ori_right)), TEMPSUMMON_MANUAL_DESPAWN);
                                pulseEvent.Repeat(Milliseconds(700));
                            });
                        }
                        break;
                    }
                    case ACTION_REMOVE_SONIC:
                    {
                        inPulse = false;
                        events.CancelEvent(EVENT_SUMMON_RINGS);
                        events.CancelEvent(EVENT_SUMMON_RINGS1);
                        events.CancelEvent(EVENT_SUMMON_RINGS2);
                        break;
                    }
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                // Removing song of the empress
                if (me->GetDistance(attacker) < 5.0f)
                {
                    uint32 spell = me->GetCurrentSpell(CURRENT_CHANNELED_SPELL) ? me->GetCurrentSpell(CURRENT_CHANNELED_SPELL)->m_spellInfo->Id : 0;
                    if (me->HasAura(SPELL_SONG_OF_THE_EMPRESS) || spell == SPELL_SONG_OF_THE_EMPRESS || spell == SPELL_SE_MELEE)
                    {
                        if (Unit* victim = me->GetVictim())
                            if (!victim->IsWithinMeleeRange(me) && TargetInLos) // interrupt only if our victim available for attacks
                                return;

                        me->RemoveAurasDueToSpell(SPELL_SONG_OF_THE_EMPRESS);
                        me->InterruptNonMeleeSpells(true, SPELL_SONG_OF_THE_EMPRESS);
                        me->CastStop(SPELL_SONG_OF_THE_EMPRESS);
                        AttackStart(attacker);
                        me->SetInCombatWith(attacker);
                        TargetInLos = false;
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                scheduler.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ATTENUATION:
                        {
                            if (me->HasUnitState(UNIT_STATE_CASTING))
                            {
                                events.RescheduleEvent(EVENT_ATTENUATION, 10000);
                                return;
                            }

                            if (Unit* target = me->GetVictim())
                                targetGUID = target->GetGUID();

                            clocksideRings = clocksideRings ? false : true;

                            me->PrepareChanneledCast(0.0f);
                            me->CastSpell(me, SPELL_ATTENUATION, false);
                            me->AddUnitState(UNIT_STATE_CASTING);

                            events.ScheduleEvent(EVENT_ATTENUATION_CANCEL, 14500);
                            events.ScheduleEvent(m_eventType, 40 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_ATTENUATION_CANCEL:
                        {
                            me->SetReactState(REACT_AGGRESSIVE);

                            if (Unit* itr = ObjectAccessor::GetUnit(*me, targetGUID))
                                me->Attack(itr, true);
                            else if (Player* itr = me->FindNearestPlayer(100.0f))
                                me->Attack(itr, true);
                            break;
                        }
                        case EVENT_FORCE_AND_VERVE:
                        {
                            if (me->HasUnitState(UNIT_STATE_CASTING))
                            {
                                events.RescheduleEvent(EVENT_FORCE_AND_VERVE, 10000);
                                return;
                            }
                            // Creating Noise Cancelling zones
                            float x, y; // make first calculation
                            NoiseMod = 2.5f;
                            uint8 count = 5;

                            for (int i = 1; i < count - 2; ++i)
                            {
                                do
                                {
                                    x = me->GetPositionX() + frand(-NoiseMod * 2.5f * i, NoiseMod * 2.5f * i); // less random spawn in melee zone
                                    y = me->GetPositionY() + frand(-NoiseMod * 2.5f * i, NoiseMod * 2.5f * i);
                                }
                                // then check if our planned pos not in wall/texture or not beside prev noise cancelling.
                                // now we`ll check each used position over current.
                                while (!HasCorrectRangeForNoise(me, x, y, me->GetPositionZ(), PredNoise, me->GetGUID()));

                                PredNoise.push_back({ x, y, me->GetPositionZ(), me->GetOrientation() }); // save next position
                                me->CastSpell(x, y, me->GetPositionZ(), SPELL_MISSILE_NOISE_CANC, false);
                            }

                            // When Echo cast Force and Verve, it should spawn noise in zorlok current area also
                            PredNoise.clear(); // remove all pos for next schedule

                            if (Creature* zorlok = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_ZORLOK) : ObjectGuid::Empty))
                            {
                                for (int i = 1; i < count - 2; ++i)
                                {
                                    do
                                    {
                                        x = zorlok->GetPositionX() + frand(-NoiseMod * 2.5f * i, NoiseMod * 2.5f * i); // less random spawn in melee zone
                                        y = zorlok->GetPositionY() + frand(-NoiseMod * 2.5f * i, NoiseMod * 2.5f * i);
                                    }
                                    // then check if our planned pos not in wall/texture or not beside prev noise cancelling.
                                    // now we`ll check each used position over current.
                                    while (!HasCorrectRangeForNoise(me, x, y, zorlok->GetPositionZ(), PredNoise, zorlok->GetGUID()));

                                    PredNoise.push_back({ x, y, zorlok->GetPositionZ(), zorlok->GetOrientation() }); // save next position
                                    me->CastSpell(x, y, zorlok->GetPositionZ(), SPELL_MISSILE_NOISE_CANC, false);
                                }
                            }

                            NoiseMod = 2.5f;
                            me->AddUnitState(UNIT_STATE_CASTING);
                            events.ScheduleEvent(EVENT_CAST_FANDV, 2000);
                            PredNoise.clear(); // remove all pos for next schedule
                            break;
                        }
                        case EVENT_CAST_FANDV:
                        {
                            me->SetReactState(REACT_PASSIVE);
                            //Talk(TALK_FORCE_AND_VERVE_ANN);
                            me->CastSpell(me, SPELL_FORCE_AND_VERVE, false);
                            events.ScheduleEvent(m_eventType, 40 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_SONG_OF_EMPRESS_MELEE:
                        {
                            if (Unit* victim = me->GetVictim())
                            {
                                if (victim->IsWithinMeleeRange(me)) // double check, maybe player back to melee range?
                                {
                                    TargetInLos = false;
                                    return;
                                }
                                else
                                    me->CastSpell(me, SPELL_SE_MELEE, true); // if not, then cast song of empress
                            }
                            break;
                        }
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (Unit* victim = me->GetVictim())
                {
                    if (!victim->IsWithinMeleeRange(me) && !TargetInLos)
                    {
                        TargetInLos = true;
                        events.ScheduleEvent(EVENT_SONG_OF_EMPRESS_MELEE, 3500);
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zorlok_echo_of_powerAI(creature);
        }
};

// Inhale - 122852
class spell_inhale : public SpellScriptLoader
{
    public:
        spell_inhale() : SpellScriptLoader("spell_inhale") { }

        class spell_inhale_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_inhale_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                if (Unit* caster = GetCaster())
                    caster->CastSpell(caster, GetSpellInfo()->Effects[effIndex].BasePoints, false);
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_inhale_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_inhale_SpellScript();
        }
};
    

// Attenuation - 122440
class spell_attenuation : public SpellScriptLoader
{
    public:
        spell_attenuation() : SpellScriptLoader("spell_attenuation") { }

        class spell_attenuation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_attenuation_SpellScript);

            void Apply()
            {
                if (Creature* zorlok = GetCaster()->ToCreature())
                    zorlok->AI()->DoAction(ACTION_SONIC_CARD);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_attenuation_SpellScript::Apply);
            }
        };

        class spell_attenuation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_attenuation_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (caster->ToCreature())
                        caster->ToCreature()->AI()->DoAction(ACTION_REMOVE_SONIC);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_attenuation_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_attenuation_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_attenuation_AuraScript();
        }
};

// Force and Verve (Aura) - 122718
class spell_force_verve : public SpellScriptLoader
{
    public:
        spell_force_verve() : SpellScriptLoader("spell_force_verve") { }

        class spell_force_verve_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_force_verve_SpellScript);

            void ApplyEffect()
            {
                Player* target = GetHitPlayer();
                if (target)
                {
                    if (target->GetMap())
                    {
                        Difficulty diff = target->GetMap()->GetDifficulty();
                        if (target->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_LFR)
                            SetHitDamage(urand(55200, 64800));
                        else
                            SetHitDamage(urand(111000, 129000));

                        if (target->GetMap()->IsHeroic())
                            SetHitDamage(urand(138750, 161250));
                    }

                    if (target->HasAura(SPELL_NOISE_CANCELLING))
                        SetHitDamage(GetHitDamage() * 0.4);
                }
            }

            void SetReact()
            {
                if (Creature* caster = GetCaster()->ToCreature())
                    caster->SetReactState(REACT_DEFENSIVE);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_force_verve_SpellScript::ApplyEffect);
                AfterCast += SpellCastFn(spell_force_verve_SpellScript::SetReact);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_force_verve_SpellScript();
        }
};

// Sonic Ring (Aura) - 122336
class spell_sonic_ring : public SpellScriptLoader
{
    public:
        spell_sonic_ring() : SpellScriptLoader("spell_sonic_ring") { }

        class spell_sonic_ring_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sonic_ring_AuraScript);

            void ApplyAura(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    caster->AddAura(SPELL_SONIC_RING_AURA, caster);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_sonic_ring_AuraScript::ApplyAura, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectApply += AuraEffectApplyFn(spell_sonic_ring_AuraScript::ApplyAura, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        class spell_sonic_ring_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sonic_ring_SpellScript);

            void Effect()
            {
                if (Player* target = GetHitPlayer())
                    if (target->HasAura(SPELL_NOISE_CANCELLING))
                        SetHitDamage(GetHitDamage() * 0.4);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_sonic_ring_SpellScript::Effect);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sonic_ring_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_sonic_ring_SpellScript();
        }
};

class PheromonesEffectTargetSelector
{
    public:
        PheromonesEffectTargetSelector() { }

        bool operator()(WorldObject* object)
        {
            return object && object->ToCreature() && object->ToCreature()->GetEntry() != NPC_INHALE_POINT;
        }
};

// Pheromones of Zeal - 124018
class spell_pheromones_of_zeal : public SpellScriptLoader
{
    public:
        spell_pheromones_of_zeal() : SpellScriptLoader("spell_pheromones_of_zeal") { }

        class spell_pheromones_of_zeal_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pheromones_of_zeal_AuraScript);

            void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    caster->AddAura(SPELL_INHALE_PHEROMONES, caster);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_pheromones_of_zeal_AuraScript::Apply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        class spell_pheromones_of_zeal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pheromones_of_zeal_SpellScript);

            void Effect()
            {
                if (Creature* caster = GetCaster()->ToCreature())
                    caster->AI()->DoAction(ACTION_INHALE_PHEROMONES);
            }

            void HandleAfterCast()
            {
                if (Creature* caster = GetCaster()->ToCreature())
                    caster->AI()->DoAction(ACTION_SECOND_PHASE);
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(PheromonesEffectTargetSelector());
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_pheromones_of_zeal_SpellScript::Effect);
                AfterCast += SpellCastFn(spell_pheromones_of_zeal_SpellScript::HandleAfterCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pheromones_of_zeal_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pheromones_of_zeal_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_pheromones_of_zeal_SpellScript();
        }

};

class ExhaleTargetFilter 
{
    public:
        explicit ExhaleTargetFilter(Unit* caster) : _caster(caster) { }

        bool operator()(WorldObject* object) const
        {
            uint32 exhaleLowId = CAST_AI(boss_zorlok::boss_zorlokAI, _caster->GetAI())->GetData(TYPE_EXHALE_TARGET);
            Player* exhaleTarget = ObjectAccessor::FindPlayer(ObjectGuid(HighGuid::Player, exhaleLowId));

            if (!exhaleTarget)
                return false;

            // Remove all the other players (stun only Exhale target).
            return (object == exhaleTarget) ? false : true;
        }

    private:
        Unit* _caster;
};

// Exhale: 122761
class spell_zorlok_exhale : public SpellScriptLoader
{
    public:
        spell_zorlok_exhale() : SpellScriptLoader("spell_zorlok_exhale") { }

        class spell_zorlok_exhale_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_zorlok_exhale_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.clear();

                Unit* caster = GetCaster();

                if (!caster)
                    return;

                Player* target = ObjectAccessor::FindPlayer(ObjectGuid(HighGuid::Player, caster->GetAI()->GetData(TYPE_EXHALE_TARGET)));

                // No target? Then we pick a random one
                if (!target || !target->IsAlive())
                {
                    std::list<Player*> playerList;
                    GetPlayerListInGrid(playerList, caster, 60.0f);

                    for (auto&& itr : playerList)
                        if (!itr->IsAlive())
                            playerList.remove(itr);

                    if (!playerList.empty())
                        target = Trinity::Containers::SelectRandomContainerElement(playerList);
                }

                if (target)
                    targets.push_back(target);
            }

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                if (Unit* caster = GetCaster())
                    if (Player* target = GetHitPlayer())
                        caster->CastSpell(target, SPELL_EXHALE_DMG, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_zorlok_exhale_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_zorlok_exhale_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_zorlok_exhale_SpellScript::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_zorlok_exhale_SpellScript();
        }
};
    

// Exhale (damage): 122760
class spell_zorlok_exhale_damage : public SpellScriptLoader
{
    public:
        spell_zorlok_exhale_damage() : SpellScriptLoader("spell_zorlok_exhale_damage") { }

        class spell_zorlok_exhale_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_zorlok_exhale_damage_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                Unit* caster = GetCaster();

                if (targets.empty() || !caster)
                    return;

                Unit* currentTarget = ObjectAccessor::FindPlayer(ObjectGuid(HighGuid::Player, caster->GetAI()->GetData(TYPE_EXHALE_TARGET)));
                if (!currentTarget)
                    return;

                // Remove players not between Zorlok and his target.
                std::list<WorldObject*>::iterator itr, next;
                for (itr = targets.begin(); itr != targets.end(); itr = next)
                {
                    next = itr;
                    ++next;

                    // Keeping current target
                    if ((*itr) == currentTarget)
                        continue;

                    if ((*itr)->GetTypeId() != TYPEID_PLAYER || !(*itr)->IsInBetween(caster, currentTarget))
                        targets.remove(*itr);
                }
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* Caster = GetCaster();
                Unit* Target = GetHitUnit();

                if (!Caster || !Target)
                    return;

                if (HasAnyBetweenCasterAndTarget()) // no dmg current target if anyone between caster and current target
                    SetHitDamage(0);
                else
                    if (Aura* inhale = Caster->GetAura(SPELL_INHALE))
                        SetHitDamage(uint32(GetHitDamage() * (inhale->GetStackAmount() * 0.5f)));
            }

            bool HasAnyBetweenCasterAndTarget()
            {
                std::list<Player*> PlayersBetweenZorlok;
                GetPlayerListInGrid(PlayersBetweenZorlok, GetCaster(), 200.0f);

                for (auto&& itr : PlayersBetweenZorlok)
                {
                    if (itr->IsInBetween(GetCaster(), GetHitUnit()) && itr->GetGUID() != GetHitUnit()->GetGUID())
                        return true; // check only one
                }

                return false;
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_zorlok_exhale_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_129); // TARGET_UNIT_CONE_ENEMY_129
                OnEffectHitTarget += SpellEffectFn(spell_zorlok_exhale_damage_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_zorlok_exhale_damage_SpellScript();
        }
};

// 122740 - Convert
class spell_convert : public SpellScript
{
    PrepareSpellScript(spell_convert);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove tanks and converted players if possible
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && (!target->ToPlayer()->IsAlive() || target->ToPlayer()->HasAura(SPELL_CONVERT)); });

            // Get real victim size
            uint32 maxVictims = caster->GetInstanceScript()->instance->Is25ManRaid() ? 5 : 2;

            // if we have enough players without tank
            if (pList.size() >= maxVictims)
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (pList.size() > maxVictims)
                    Trinity::Containers::RandomResizeList(targets, maxVictims);

                m_targets = targets;

                return;
            }

            // if we haven`t enough players without tank, then just push whole raid to container exclude already converted players
            targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && (!target->ToPlayer()->IsAlive() || target->ToPlayer()->HasAura(SPELL_CONVERT)); });

            if (targets.size() > maxVictims)
                Trinity::Containers::RandomResizeList(targets, maxVictims);

            m_targets = targets;
        }
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_convert::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_convert::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_convert::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_convert::CopyTargets, EFFECT_3, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_convert::CopyTargets, EFFECT_4, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// 125784 - Heart of Fear - Zor'lok - Achievement - Loot Bug
class spell_zorlok_achievement_loot_bug : public SpellScript
{
    PrepareSpellScript(spell_zorlok_achievement_loot_bug);

    void HandleHit()
    {
        if (GetHitUnit()->HasAura(SPELL_ZEALOUS_PARASITE))
            return;

        Unit* bug = GetCaster();

        bug->CastSpell(GetHitUnit(), SPELL_ZEALOUS_PARASITE, true);
        if (auto* instance = bug->GetInstanceScript())
            if (Creature* zorlok = ObjectAccessor::GetCreature(*bug, instance->GetGuidData(DATA_ZORLOK)))
                if (auto script = dynamic_cast<boss_zorlok::boss_zorlokAI*>(zorlok->AI()))
                    script->OnZealousBugUsed(bug->GetGUID());
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_zorlok_achievement_loot_bug::HandleHit);
    }
};

// 363. Summoned by 123811 - Pheromones of Zeal
class sat_pheromones_of_zeal : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && triggering->ToPlayer()->GetPositionZ() < 408.5f;
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* pItr = triggering->ToPlayer())
            m_caster->CastSpell(pItr, SPELL_PHEROMONES_OF_ZEAL, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* pItr = triggering->ToPlayer())
            pItr->RemoveAurasDueToSpell(SPELL_PHEROMONES_OF_ZEAL);
    }
};

// 351, 429, 431, 428, 430. Summoned by 122731 - Create Noise Cancelling
class sat_zorlok_create_noise_cancelling : public IAreaTriggerAura
{

    void OnInit() override
    {
        if (AreaTrigger* at = GetAreaTrigger())
        {
            // depend of difficulty - 3/9 in 10/25 mode
            MaxAffectedPlayers = 3; // default
            DiffBetweenAffectedPlayers = 0;

            if (at->GetMap()->Is25ManRaid())
                MaxAffectedPlayers += 6;
        }
    }

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && MaxAffectedPlayers >= DiffBetweenAffectedPlayers;
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
        {
            DiffBetweenAffectedPlayers++;
            itr->CastSpell(itr, SPELL_NOISE_CANCELLING, true);
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
        {
            DiffBetweenAffectedPlayers--;
            itr->RemoveAurasDueToSpell(SPELL_NOISE_CANCELLING);
        }
    }

    private:
        uint32 MaxAffectedPlayers;
        uint32 DiffBetweenAffectedPlayers;
};

void AddSC_boss_zorlok()
{
    new boss_zorlok();
    new npc_sonic_ring();
    new npc_zorlok_echo_of_power();     // 65173, 65174 - Echo of Power
    new spell_inhale();                 // 122852 - Inhale
    new spell_attenuation();            // 122440 - Attenuation
    new spell_force_verve();            // 122718 - Force and verve
    new spell_sonic_ring();             // 122336 - Sonic Ring
    new spell_pheromones_of_zeal();     // 124018 - Pheromones of Zeal
    new spell_zorlok_exhale();          // 122761 - Exhale
    new spell_zorlok_exhale_damage();   // 122760 - Exhale (damage aura)
    new spell_script<spell_convert>("spell_convert");             // 122740 - Convert
    new spell_script<spell_zorlok_achievement_loot_bug>("spell_zorlok_achievement_loot_bug");
    new atrigger_script<sat_pheromones_of_zeal>("sat_pheromones_of_zeal");
    new atrigger_script<sat_zorlok_create_noise_cancelling>("sat_zorlok_create_noise_cancelling");
}
