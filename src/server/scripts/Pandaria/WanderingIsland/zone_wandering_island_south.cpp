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
#include "ScriptedEscortAI.h"
#include "Vehicle.h"

class AreaTrigger_at_mandori : public AreaTriggerScript
{
    public:
        AreaTrigger_at_mandori() : AreaTriggerScript("AreaTrigger_at_mandori") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
           if (player->GetPositionX() < 710.0f)
               return true;

           if (player->GetQuestStatus(29792) != QUEST_STATUS_INCOMPLETE)
               return true;

           ObjectGuid playerGuid = player->GetGUID();

            auto const aysa = player->SummonCreature(59986, 698.04f, 3601.79f, 142.82f, 3.254830f, TEMPSUMMON_MANUAL_DESPAWN, 0ms); // Aysa
            auto const ji   = player->SummonCreature(59988, 698.06f, 3599.34f, 142.62f, 2.668790f, TEMPSUMMON_MANUAL_DESPAWN, 0ms); // Ji
            auto const jojo = player->SummonCreature(59989, 702.78f, 3603.58f, 142.01f, 3.433610f, TEMPSUMMON_MANUAL_DESPAWN, 0ms); // Jojo

            if (!aysa || !ji || !jojo)
                return true;

            aysa->SetExplicitSeerGuid(playerGuid);
            ji->SetExplicitSeerGuid(playerGuid);
            jojo->SetExplicitSeerGuid(playerGuid);

            aysa->AI()->SetGUID(playerGuid);
            ji->AI()->SetGUID(playerGuid);
            jojo->AI()->SetGUID(playerGuid);

            player->RemoveAurasDueToSpell(59073);
            player->RemoveAurasDueToSpell(59074);

            return true;
        }
};

class npc_mandori_escort : public CreatureScript
{
    public:
        npc_mandori_escort() : CreatureScript("npc_mandori_escort") { }

        struct npc_mandori_escortAI : public npc_escortAI
        {
            npc_mandori_escortAI(Creature* creature) : npc_escortAI(creature) { }

            enum escortEntry
            {
                NPC_AYSA    = 59986,
                NPC_JI      = 59988,
                NPC_JOJO    = 59989
            };

            uint32 IntroTimer;
            uint32 doorEventTimer;

            uint8  IntroState;
            uint8  doorEventState;

            ObjectGuid playerGuid;

            ObjectGuid mandoriDoorGuid;
            ObjectGuid peiwuDoorGuid;

            void Reset() override
            {
                IntroTimer      = 250;
                doorEventTimer  = 0;

                IntroState      = 0;
                doorEventState  = 0;

                playerGuid = ObjectGuid::Empty;
                mandoriDoorGuid = ObjectGuid::Empty;
                peiwuDoorGuid = ObjectGuid::Empty;

                me->SetReactState(REACT_PASSIVE);
            }

            void SetGUID(ObjectGuid guid, int32 /*type*/) override
            {
                playerGuid = guid;

                if (!Is(NPC_AYSA))
                    return;

                if (GameObject* mandoriDoor = me->SummonGameObject(211294, 695.26f, 3600.99f, 142.38f, 3.04f, { }, 0))
                {
                    mandoriDoor->SetExplicitSeerGuid(playerGuid);
                    mandoriDoorGuid = mandoriDoor->GetGUID();
                }

                if (GameObject* peiwuDoor = me->SummonGameObject(211298, 566.52f, 3583.46f, 92.16f, 3.14f, { }, 0))
                {
                    peiwuDoor->SetExplicitSeerGuid(playerGuid);
                    peiwuDoorGuid = peiwuDoor->GetGUID();
                }
            }

            bool Is(uint32 npc_entry)
            {
                return me->GetEntry() == npc_entry;
            }

            void WaypointReached(uint32 waypointId) override
            {
                switch (waypointId)
                {
                    case 7:
                        SetEscortPaused(true);

                        // Jojo reach the waypoint 1 sec after the others
                        if (!Is(NPC_JOJO))
                            doorEventTimer = 2000;
                        else
                            doorEventTimer = 1000;
                        break;
                    default:
                        break;
                }
            }

            void LastWaypointReached()
            {
                if (Is(NPC_JI))
                    if (Player* player = ObjectAccessor::FindPlayer(playerGuid))
                        player->AddAura(68482, player); // Phase 8192

                if (Is(NPC_AYSA))
                {
                    if (GameObject* mandoriDoor = me->GetMap()->GetGameObject(mandoriDoorGuid))
                        mandoriDoor->Delete();
                    if (GameObject* peiwuDoor = me->GetMap()->GetGameObject(peiwuDoorGuid))
                        peiwuDoor->Delete();
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (IntroTimer)
                {
                    if (IntroTimer <= diff)
                    {
                        switch (++IntroState)
                        {
                            case 1:
                                if (Is(NPC_AYSA))
                                    Talk(0, ObjectAccessor::FindPlayer(playerGuid));
                                IntroTimer = 1000;
                                break;
                            case 2:
                                if (Is(NPC_AYSA))
                                {
                                    if (GameObject* mandoriDoor = me->GetMap()->GetGameObject(mandoriDoorGuid))
                                        mandoriDoor->SetGoState(GO_STATE_ACTIVE);

                                    if (Player* player = ObjectAccessor::FindPlayer(playerGuid))
                                        player->KilledMonsterCredit(59946);
                                }
                                IntroTimer = 1000;
                                break;
                            case 3:
                                Start(false, true);
                                IntroTimer = 0;
                                break;
                        }
                    }
                    else
                        IntroTimer -= diff;
                }

                if (doorEventTimer)
                {
                    if (doorEventTimer <= diff)
                    {
                        switch (++doorEventState)
                        {
                            case 1:
                                if (Is(NPC_AYSA))
                                    Talk(1);
                                doorEventTimer = 2000;
                                break;
                            case 2:
                                if (Is(NPC_AYSA))
                                    Talk(2);
                                doorEventTimer = 4000;
                                break;
                            case 3:
                                if (Is(NPC_JI))
                                    Talk(0);
                                doorEventTimer = 3000;
                                break;
                            case 4:
                                if (Is(NPC_JI))
                                    Talk(1);
                                doorEventTimer = 4000;
                                break;
                            case 5:
                                if (Is(NPC_JOJO))
                                    me->GetMotionMaster()->MoveCharge(567.99f, 3583.41f, 94.74f);
                                doorEventTimer = 150;
                                break;
                            case 6:
                                if (Is(NPC_AYSA))
                                    if (GameObject* peiwuDoor = me->GetMap()->GetGameObject(peiwuDoorGuid))
                                        peiwuDoor->SetGoState(GO_STATE_ACTIVE);
                                doorEventTimer = 2000;
                                break;
                            case 7:
                                if (Is(NPC_JI))
                                    Talk(2);
                                if (Is(NPC_AYSA))
                                    if (Player* player = ObjectAccessor::FindPlayer(playerGuid))
                                        player->KilledMonsterCredit(59947);
                                if (!Is(NPC_JOJO))
                                   SetEscortPaused(false);
                                doorEventTimer = 2000;
                                break;
                            case 8:
                               if (Is(NPC_JOJO))
                                   SetEscortPaused(false);
                                doorEventTimer = 0;
                                break;
                        }
                    }
                    else
                        doorEventTimer -= diff;
                }

                npc_escortAI::UpdateAI(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_mandori_escortAI(creature);
        }
};

class npc_korga : public CreatureScript
{
    public:
        npc_korga() : CreatureScript("npc_korga") { }

        bool OnQuestAccept(Player* player, Creature* /*creature*/, Quest const* quest) override
        {
            if (quest->GetQuestId() == 30589) // Wrecking the Wreck
                if (Creature* jiEscort = player->SummonCreature(60900, 424.71f, 3635.59f, 92.70f, 2.498430f, TEMPSUMMON_MANUAL_DESPAWN, 0ms))
                {
                    jiEscort->SetExplicitSeerGuid(player->GetGUID());
                    jiEscort->SetReactState(REACT_PASSIVE);
                    jiEscort->AI()->SetGUID(player->GetGUID());
                }

            return true;
        }
};

class npc_ji_forest_escort : public CreatureScript
{
    public:
        npc_ji_forest_escort() : CreatureScript("npc_ji_forest_escort") { }

        struct npc_ji_forest_escortAI : public npc_escortAI
        {
            npc_ji_forest_escortAI(Creature* creature) : npc_escortAI(creature) { }

            ObjectGuid playerGuid;
            uint32 IntroTimer;

            void Reset() override
            {
                playerGuid = ObjectGuid::Empty;
                IntroTimer      = 2000;
            }

            void SetGUID(ObjectGuid guid, int32 /*type*/) override
            {
                Talk(0);
                playerGuid = guid;
            }

            void WaypointReached(uint32 /*waypointId*/) override { }

            void LastWaypointReached()
            {
                if (Player* player = ObjectAccessor::FindPlayer(playerGuid))
                    player->AddAura(68483, player); // Phase 16384
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == 100)
                    Start(false, true);
                else
                {
                    npc_escortAI::MovementInform(type, pointId);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (IntroTimer)
                {
                    if (IntroTimer <= diff)
                    {
                        me->GetMotionMaster()->MoveJump(429.1f, 3674.f, 78.6f, 20.f, 20.f, 100);
                        IntroTimer = 0;
                    }
                    else
                        IntroTimer -= diff;
                }

                npc_escortAI::UpdateAI(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ji_forest_escortAI(creature);
        }
};

struct AreaTrigger_at_rescue_soldiers final : public AreaTriggerScript
{
    AreaTrigger_at_rescue_soldiers() : AreaTriggerScript("AreaTrigger_at_rescue_soldiers") { }

    bool OnTrigger(Player *player, AreaTriggerEntry const* /*trigger*/) override
    {
        if (player->GetQuestStatus(29794) != QUEST_STATUS_INCOMPLETE)
            return true;

        if (!player->HasAura(129340))
            return true;

        auto const vehicleKit = player->GetVehicleKit();
        if (!vehicleKit)
            return true;

        auto const soldierUnit = vehicleKit->GetPassenger(0);
        if (!soldierUnit)
            return true;

        if (auto const solider = soldierUnit->ToCreature())
        {
            solider->AI()->DoAction(1);

            player->RemoveAurasDueToSpell(129340);
            player->KilledMonsterCredit(55999);
        }

        return true;
    }
};

enum
{
    QUEST_AN_ANCIENT_EVIL   = 29798,
    NPC_VORDRAKA            = 56009,
};

class npc_jojo_ironbrow : public CreatureScript
{
public:
    npc_jojo_ironbrow() : CreatureScript("npc_jojo_ironbrow") { }

    bool OnQuestAccept(Player* player, Creature* /*creature*/, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_AN_ANCIENT_EVIL)
        {
            if (Creature* const vordraka = player->SummonCreature(NPC_VORDRAKA, 281.74f, 4003.40f, 72.96f, 0.05f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000ms))
            {
                vordraka->SetExplicitSeerGuid(player->GetGUID());
                vordraka->AI()->SetGUID(player->GetGUID());
                vordraka->SetPhaseMask(12, true);
            }
        }

        return true;
    }
};

class boss_vordraka : public CreatureScript
{
public:
    boss_vordraka() : CreatureScript("boss_vordraka") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_vordrakaAI(creature);
    }

    struct boss_vordrakaAI : public ScriptedAI
    {
        boss_vordrakaAI(Creature* creature) : ScriptedAI(creature) { }

        ObjectGuid summonerGUID;
        bool summonedAllies;
        EventMap _events;

        enum eEnums
        {
            EVENT_DEEP_ATTACK       = 1,
            EVENT_DEEP_SEA_RUPTURE  = 2,

            SPELL_DEEP_ATTACK       = 117287,
            SPELL_DEEP_SEA_RUPTURE  = 117456,

            NPC_DEEPSCALE_AGGRESSOR = 60685,
            NPC_AYSA                = 56416,
        };

        void Reset() override
        {
            summonedAllies = false;
            _events.ScheduleEvent(EVENT_DEEP_ATTACK, 10000);
            _events.ScheduleEvent(SPELL_DEEP_SEA_RUPTURE, 12500);
        }

        void SetGUID(ObjectGuid guid, int32 /*type*/) override
        {
            summonerGUID = guid;
        }

        void DamageTaken(Unit* attacker, uint32& damage) override
        {
            if (attacker->GetEntry() == NPC_AYSA)
                damage /= 2;
        }

        void MoveInLineOfSight(Unit* who) override
        {
            Player * const player = who->ToPlayer();
            if (!player || me->GetVictim())
                return;

            if (player->GetQuestStatus(QUEST_AN_ANCIENT_EVIL) == QUEST_STATUS_INCOMPLETE)
                if (Creature* const aysa = GetClosestCreatureWithEntry(me, NPC_AYSA, 30.f))
                {
                    aysa->AI()->AttackStart(me);
                    AttackStart(aysa);
                    me->AddThreat(aysa, 1000.0f);
                }

            ScriptedAI::MoveInLineOfSight(who);
        }

        void JustDied(Unit* /*killer*/) override
        {
            std::list<Player*> playerList;
            GetPlayerListInGrid(playerList, me, 30.0f);

            for (auto player : playerList)
                if (player->GetQuestStatus(QUEST_AN_ANCIENT_EVIL) == QUEST_STATUS_INCOMPLETE)
                    if (player->IsAlive())
                        player->KilledMonsterCredit(me->GetEntry());
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            if (!summonedAllies && me->HealthBelowPct(50))
            {
                if (Creature* const aysa = GetClosestCreatureWithEntry(me, NPC_AYSA, 30.f))
                {
                    aysa->AI()->Talk(0);
                    me->AddThreat(aysa, 1000.0f);
                }

                for (int i = 0; i < 3; ++i)
                {
                    Position pos = me->GetRandomNearPosition(20.f);
                    if (Creature* const summon = me->SummonCreature(NPC_DEEPSCALE_AGGRESSOR, pos, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000ms))
                        if (Player * const player = Unit::GetPlayer(*me, summonerGUID))
                            summon->AI()->AttackStart(player);
                }
                summonedAllies = true;
            }

            _events.Update(diff);

            switch (_events.ExecuteEvent())
            {
                case EVENT_DEEP_ATTACK:
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 25.0f, true))
                        me->CastSpell(target, SPELL_DEEP_ATTACK, false);

                    _events.ScheduleEvent(EVENT_DEEP_ATTACK, 10000);
                    break;
                }
                case EVENT_DEEP_SEA_RUPTURE:
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 25.0f, true))
                        me->CastSpell(target, SPELL_DEEP_SEA_RUPTURE, false);

                    _events.ScheduleEvent(EVENT_DEEP_ATTACK, 10000);
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }
    };
};

enum
{
    QUEST_RISKING_IT_ALL        = 30767
};

class npc_aysa_gunship_crash : public CreatureScript
{
    public:
        npc_aysa_gunship_crash() : CreatureScript("npc_aysa_gunship_crash") { }

        bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_RISKING_IT_ALL)
            {
                if (Creature* aysa = player->SummonCreature(60729, creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ(), creature->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0ms))
                {
                    aysa->SetExplicitSeerGuid(player->GetGUID());
                    aysa->AI()->SetGUID(player->GetGUID());
                }
            }

            return true;
        }

        struct npc_aysa_gunship_crashAI : public ScriptedAI
        {
            npc_aysa_gunship_crashAI(Creature* creature) : ScriptedAI(creature)
            { }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (HealthBelowPct(70))
                    damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_aysa_gunship_crashAI(creature);
        }
};

class npc_aysa_gunship_crash_escort : public CreatureScript
{
    enum
    {
        NPC_JI_FIREPAW      = 60741,
        GO_EXPLOSIVES       = 215344,
        NPC_CREDIT          = 60727,

        EVENT_AYSA_START    = 1,
        EVENT_AYSA_TALK_1,
        EVENT_JI_TALK_1,
        EVENT_AYSA_TALK_2,
        EVENT_JI_TALK_2,
        EVENT_AYSA_TALK_3,
        EVENT_JI_TALK_3,
        EVENT_AYSA_TALK_4,
        EVENT_AYSA_SIGH,
        EVENT_JI_TURN,
        EVENT_JI_USE,
        EVENT_RUN,
        EVENT_START_CINEMATIC,
        EVENT_TELEPORT_PLAYER,

    };
public:
    npc_aysa_gunship_crash_escort() : CreatureScript("npc_aysa_gunship_crash_escort") { }

    struct npc_aysa_gunship_crash_escortAI : public npc_escortAI
    {
        npc_aysa_gunship_crash_escortAI(Creature* creature) : npc_escortAI(creature) { }

        ObjectGuid playerGuid;
        ObjectGuid jiGuid;
        ObjectGuid fireGuid;

        uint32 IntroTimer;
        uint32 discussTimer;

        uint8  discussEvent;

        EventMap events;

        void Reset() override
        {
            playerGuid = ObjectGuid::Empty;
            jiGuid = ObjectGuid::Empty;
            fireGuid = ObjectGuid::Empty;

            IntroTimer      = 100;
            discussTimer    = 0;

            discussEvent    = 0;

            events.Reset();
        }

        void SetGUID(ObjectGuid guid, int32 /*type*/) override
        {
            playerGuid = guid;

            if (Creature* ji = me->SummonCreature(NPC_JI_FIREPAW, 230.31f, 4006.67f, 87.27f, 3.38f, TEMPSUMMON_MANUAL_DESPAWN, 0ms))
            {
                ji->SetExplicitSeerGuid(playerGuid);
                jiGuid = ji->GetGUID();
            }

            if (GameObject* gob = me->SummonGameObject(GO_EXPLOSIVES, 227.75f, 4006.38f, 87.06f, 0.0f, { }, 0))
            {
                gob->SetExplicitSeerGuid(playerGuid);
                fireGuid = gob->GetGUID();
            }

            events.ScheduleEvent(EVENT_AYSA_START, 1000);
        }

        void MovementInform(uint32 type, uint32 pointId) override
        {
            if (type == POINT_MOTION_TYPE)
            {
                if (pointId == 100)
                {
                    getJi()->GetMotionMaster()->MoveJump(231.176f, 3976.017f, 87.80869f, 10.0f, 15.0f, 5);
                    me->GetMotionMaster()->MoveJump(237.478f, 3973.933f, 87.96973f, 10.0f, 15.0f, 5);
                }
            }
            else if (type == EFFECT_MOTION_TYPE)
            {
                if (pointId == 5)
                    events.ScheduleEvent(EVENT_START_CINEMATIC, 200);
            }

            npc_escortAI::MovementInform(type, pointId);
        }

        Creature* getJi()
        {
            return me->GetMap()->GetCreature(jiGuid);
        }

        void UpdateAI(uint32 diff) override
        {
            npc_escortAI::UpdateAI(diff);
            events.Update(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_AYSA_START:
                        Talk(0, Player::GetPlayer(*me, playerGuid));
                        Start(false, true);
                        break;
                    case EVENT_AYSA_TALK_1:
                        Talk(1);
                        getJi()->SetFacingToObject(me);
                        events.ScheduleEvent(EVENT_JI_TALK_1, 4000);
                        break;
                    case EVENT_JI_TALK_1:
                        getJi()->AI()->Talk(1);
                        events.ScheduleEvent(EVENT_AYSA_TALK_2, 11000);
                        break;
                    case EVENT_AYSA_TALK_2:
                        Talk(2);
                        events.ScheduleEvent(EVENT_JI_TALK_2, 3000);
                        break;
                    case EVENT_JI_TALK_2:
                        getJi()->AI()->Talk(2);
                        events.ScheduleEvent(EVENT_AYSA_TALK_3, 10000);
                        break;
                    case EVENT_AYSA_TALK_3:
                        Talk(3);
                        events.ScheduleEvent(EVENT_JI_TALK_3, 4000);
                        break;
                    case EVENT_JI_TALK_3:
                        getJi()->AI()->Talk(3);
                        events.ScheduleEvent(EVENT_AYSA_TALK_4, 7000);
                        break;
                    case EVENT_AYSA_TALK_4:
                        Talk(4);
                        events.ScheduleEvent(EVENT_AYSA_SIGH, 5000);
                        break;
                    case EVENT_AYSA_SIGH:
                        Talk(5);
                        events.ScheduleEvent(EVENT_JI_TURN, 2000);
                        break;
                    case EVENT_JI_TURN:
                        getJi()->SetFacingTo(me->GetHomePosition().GetOrientation());
                        events.ScheduleEvent(EVENT_JI_USE, 2000);
                        break;
                    case EVENT_JI_USE:
                        getJi()->HandleEmoteCommand(EMOTE_ONESHOT_USE_STANDING);
                        events.ScheduleEvent(EVENT_RUN, 2000);
                        break;
                    case EVENT_RUN:
                        getJi()->GetMotionMaster()->MovePoint(100, 232.039f, 3981.756f, 85.9347f);
                        me->GetMotionMaster()->MovePoint(100, 236.940f, 3982.803f, 86.68575f);
                        break;
                    case EVENT_START_CINEMATIC:

                        if (Player* player = Player::GetPlayer(*me, playerGuid))
                        {
                            player->KilledMonsterCredit(NPC_CREDIT);
                            player->AreaExploredOrEventHappens(QUEST_RISKING_IT_ALL);
                            player->SendMovieStart(117);
                        }
                        events.ScheduleEvent(EVENT_TELEPORT_PLAYER, 500);
                        break;
                    case EVENT_TELEPORT_PLAYER:
                        getJi()->DespawnOrUnsummon(2000);
                        me->DespawnOrUnsummon(2000);

                        if (GameObject * fires = GameObject::GetGameObject(*me, fireGuid))
                            fires->Delete();

                        if (Player* player = Player::GetPlayer(*me, playerGuid))
                            player->NearTeleportTo(249.38f, 3939.55f, 65.61f, 1.501471f);
                        break;
                    default:
                        break;
                }
            }

        }

        void WaypointReached(uint32 waypointId) override
        {
            if (waypointId == 18)
            {
                SetEscortPaused(true);
                events.ScheduleEvent(EVENT_AYSA_TALK_1, 1000);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_aysa_gunship_crash_escortAI(creature);
    }

};

#define MAX_ENNEMIES_POS   2
#define MAX_HEALER_COUNT   12
#define UPDATE_POWER_TIMER 3000

Position const ennemiesPositions[MAX_ENNEMIES_POS] =
{
    {215.0f, 3951.0f, 71.4f, 0.0f},
    {290.0f, 3939.0f, 86.7f, 0.0f}
};

enum eEnums
{
    QUEST_HEALING_SHEN      = 29799,

    NPC_HEALER_A            = 60878,
    NPC_HEALER_H            = 60896,
    NPC_ENNEMY              = 60858,

    NPC_SHEN_HEAL_CREDIT    = 56011,

    EVENT_CHECK_PLAYERS     = 1,
    EVENT_UPDATE_POWER      = 2,
    EVENT_SUMMON_ENNEMY     = 3,
    EVENT_SUMMON_HEALER     = 4,

    SPELL_SHEN_HEALING      = 117783,
    SPELL_HEALER_A          = 117784,
    SPELL_HEALER_H          = 117932,
};

class npc_ji_end_event : public CreatureScript
{
    public:
        npc_ji_end_event() : CreatureScript("npc_ji_end_event") { }

        struct npc_ji_end_eventAI : public ScriptedAI
        {
            npc_ji_end_eventAI(Creature* creature) : ScriptedAI(creature), _summons(creature) { }

            EventMap   _events;
            SummonList _summons;

            bool       inProgress;
            uint8      healerCount;
            uint8      ennemiesCount;
            uint16     actualPower;

            void Reset() override
            {
                _summons.DespawnAll();

                healerCount   = 0;
                ennemiesCount = 0;
                actualPower   = 0;

                inProgress = false;

                _events.Reset();
                _events.ScheduleEvent(EVENT_CHECK_PLAYERS, 5000);
            }

            bool CheckPlayers()
            {
                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 100.0f);

                for (auto player : playerList)
                    if (player->GetQuestStatus(QUEST_HEALING_SHEN) == QUEST_STATUS_INCOMPLETE)
                        if (player->IsAlive())
                            return true;

                return false;
            }

            void UpdatePower()
            {
                actualPower = (actualPower + healerCount <= 700) ? actualPower + healerCount: 700;

                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 100.0f);

                for (auto player : playerList)
                {
                    if (player->GetQuestStatus(QUEST_HEALING_SHEN) == QUEST_STATUS_INCOMPLETE)
                    {
                        if (player->IsAlive())
                        {
                            if (actualPower < 700) // IN_PROGRESS
                            {
                                if (!player->HasAura(SPELL_SHEN_HEALING))
                                    player->CastSpell(player, SPELL_SHEN_HEALING, true);

                                player->SetPower(POWER_ALTERNATE_POWER, actualPower);
                            }
                            else
                            {
                                if (player->HasAura(SPELL_SHEN_HEALING))
                                    player->RemoveAurasDueToSpell(SPELL_SHEN_HEALING);

                                player->KilledMonsterCredit(NPC_SHEN_HEAL_CREDIT);
                            }
                        }
                    }
                }

                if (actualPower >= 700)
                    Reset();
            }

            void SummonEnnemy()
            {
                uint8 pos = rand() % MAX_ENNEMIES_POS;
                float posJumpX = frand(228.0f, 270.0f);
                float posJumpY = frand(3949.0f, 3962.0f);

                if (Creature* ennemy = me->SummonCreature(NPC_ENNEMY, ennemiesPositions[pos].GetPositionX(), ennemiesPositions[pos].GetPositionY(), ennemiesPositions[pos].GetPositionZ(), TEMPSUMMON_CORPSE_DESPAWN))
                    ennemy->GetMotionMaster()->MoveJump(posJumpX, posJumpY, me->GetMap()->GetHeight(me->GetPhaseMask(), posJumpX, posJumpY, 100.0f), 20.0f, 20.0f);
            }

            void SummonHealer()
            {
                uint32 entry = rand() % 2 ? NPC_HEALER_A: NPC_HEALER_H;
                float posX = frand(228.0f, 270.0f);
                float posY = frand(3949.0f, 3962.0f);

                me->SummonCreature(entry, posX, posY, me->GetMap()->GetHeight(me->GetPhaseMask(), posX, posY, 100.0f), 1.37f, TEMPSUMMON_CORPSE_DESPAWN);
            }

            void JustSummoned(Creature* summon) override
            {
                _summons.Summon(summon);

                switch (summon->GetEntry())
                {
                    case NPC_HEALER_A:
                    case NPC_HEALER_H:
                        ++healerCount;
                        break;
                    case NPC_ENNEMY:
                        ++ennemiesCount;
                        break;
                }
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
            {
                _summons.Despawn(summon);

                switch (summon->GetEntry())
                {
                    case NPC_HEALER_A:
                    case NPC_HEALER_H:
                        --healerCount;
                        break;
                    case NPC_ENNEMY:
                        --ennemiesCount;
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                _events.Update(diff);

                switch (_events.ExecuteEvent())
                {
                    case EVENT_CHECK_PLAYERS:
                    {
                        bool playerNearWithQuest = CheckPlayers();

                        if (inProgress && !playerNearWithQuest)
                        {
                            inProgress = false;
                            Reset();
                        }
                        else if (!inProgress && playerNearWithQuest)
                        {
                            inProgress = true;
                            _events.ScheduleEvent(EVENT_UPDATE_POWER,  UPDATE_POWER_TIMER);
                            _events.ScheduleEvent(EVENT_SUMMON_ENNEMY, 5000);
                            _events.ScheduleEvent(EVENT_SUMMON_HEALER, 5000);
                        }
                        _events.ScheduleEvent(EVENT_CHECK_PLAYERS, 5000);
                        break;
                    }
                    case EVENT_UPDATE_POWER:
                        UpdatePower();
                        _events.ScheduleEvent(EVENT_UPDATE_POWER, UPDATE_POWER_TIMER);
                        break;
                    case EVENT_SUMMON_ENNEMY:
                        if (ennemiesCount < (healerCount / 2))
                        {
                            SummonEnnemy();
                            _events.ScheduleEvent(EVENT_SUMMON_ENNEMY, 5000);
                        }
                        else
                            _events.ScheduleEvent(EVENT_SUMMON_ENNEMY, 7500);
                        break;
                    case EVENT_SUMMON_HEALER:
                        if (healerCount < MAX_HEALER_COUNT)
                            SummonHealer();

                        _events.ScheduleEvent(EVENT_SUMMON_HEALER, 12500);
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ji_end_eventAI(creature);
        }
};

class npc_shen_healer : public CreatureScript
{
    public:
        npc_shen_healer() : CreatureScript("npc_shen_healer") { }

        struct npc_shen_healerAI : public ScriptedAI
        {
            npc_shen_healerAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->CastSpell(me, me->GetEntry() == NPC_HEALER_A ? SPELL_HEALER_A: SPELL_HEALER_H, true);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                return;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shen_healerAI(creature);
        }
};

#define GOSSIP_CHOOSE_FACTION     "I'm ready to choose my destiny."
#define GOSSIP_TP_STORMIND        "I would like to go to Stormwind"
#define GOSSIP_TP_ORGRI           "I would like to go to Orgrimmar"

class npc_shang_xi_choose_faction : public CreatureScript
{
    public:
        npc_shang_xi_choose_faction() : CreatureScript("npc_shang_xi_choose_faction") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if (player->GetRace() == RACE_PANDAREN_NEUTRAL)
            {
                if (player->GetQuestStatus(31450) == QUEST_STATUS_INCOMPLETE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_CHOOSE_FACTION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            }
            else if (player->GetRace() == RACE_PANDAREN_ALLIANCE)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TP_STORMIND, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            else if (player->GetRace() == RACE_PANDAREN_HORDE)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TP_ORGRI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

            player->PlayerTalkClass->SendGossipMenu(1, creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action) override
        {
            if (action == GOSSIP_ACTION_INFO_DEF + 1)
                player->ShowNeutralPlayerFactionSelectUI();
            else if (action == GOSSIP_ACTION_INFO_DEF + 2)
                player->TeleportTo(0, -8866.55f, 671.93f, 97.90f, 5.31f);
            else if (action == GOSSIP_ACTION_INFO_DEF + 3)
                player->TeleportTo(1, 1577.30f, -4453.64f, 15.68f, 1.84f);

            player->PlayerTalkClass->SendCloseGossip();
            return true;
        }
};

void AddSC_wandering_island_south()
{
    new AreaTrigger_at_mandori();
    new npc_mandori_escort();
    new npc_korga();
    new npc_ji_forest_escort();
    new AreaTrigger_at_rescue_soldiers();
    new npc_jojo_ironbrow();
    new boss_vordraka();
    //new npc_aysa_gunship_crash();
    //new npc_aysa_gunship_crash_escort();
    new npc_ji_end_event();
    new npc_shen_healer();
    new npc_shang_xi_choose_faction();
}
