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

/* ScriptDataSDName: WanderingIslandWest
SD%Complete: 100
SDComment: Quest support: 29776, 29779, 29780, 29781, 29785, 29786, 29787, 29790, 29791
SDCategory: WanderingIsland
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "SpellScript.h"
#include "Vehicle.h"
#include "CreatureTextMgr.h"



static Position const jiTempleMovePoint_1 = { 966.1493f, 3607.0894f, 196.51373f, 0.0f };
static Position const jiTempleMovePoint_2 = { 958.9819f, 3594.94f, 196.6083f, 0.0f };
static Position const aysaTempleMovePoint_1 = { 966.3715f, 3602.764f, 196.47968f, 0.0f };
static Position const aysaTempleMovePoint_2 = { 943.1327f, 3572.154f, 193.6543f, 0.0f };
static Position const jiTempleSpawnPos = { 971.5566f, 3607.8015f, 195.71495f, 0.0f };
static Position const aysaTempleSpawnPos = { 968.2688f, 3602.207f, 196.6442f, 0.0f };

// NPC 64530 - Master Shang Xi (Wind Temple) | Quest 29776
struct npc_master_shang_xi_temple : public ScriptedAI
{
    enum TempleEvents
    {
        EVENT_MASTER_SHANG_XI_TALK_1 = 1,
        EVENT_MASTER_SHANG_XI_TALK_2,
        EVENT_MASTER_SHANG_XI_TALK_3,
        EVENT_MASTER_SHANG_XI_TALK_4,
        EVENT_MASTER_SHANG_XI_TALK_5,
        EVENT_MASTER_SHANG_XI_TALK_6,
        EVENT_MASTER_SHANG_XI_TALK_7,
        EVENT_MASTER_SHANG_XI_TALK_8,
        EVENT_MASTER_SHANG_XI_TALK_9,
    };

    std::vector<Player*> players;
    EventMap events;
    bool started;

    npc_master_shang_xi_temple(Creature* creature) : ScriptedAI(creature), started(false) { }

    void Reset() override
    {
        started = false;
        events.Reset();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        Player* const player = who->ToPlayer();
        if (!player)
            return;

        if (player->GetQuestStatus(29423) != QUEST_STATUS_REWARDED || started)
            return;

        if (me->GetAreaId() == 5820) // Temple of Five Dawns Area
        {
            started = true;
            if (player->GetQuestStatus(29775) == QUEST_STATUS_REWARDED)
                events.ScheduleEvent(EVENT_MASTER_SHANG_XI_TALK_7, 1000);
            else
                events.ScheduleEvent(EVENT_MASTER_SHANG_XI_TALK_1, 1000);
        }
    }

    void UpdatePlayerList()
    {
        players.clear();
        std::list<Player*> playerList;
        GetPlayerListInGrid(playerList, me, 20.0f);
        for (Player* player : playerList)
            if (!player->IsGameMaster() && player->GetQuestStatus(29423) == QUEST_STATUS_REWARDED)
                players.push_back(player);
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == 29776) // Morning Breeze Village
        {
            if (Creature* vehicle = player->SummonCreature(55685, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 20000ms))
            {
                player->EnterVehicle(vehicle);
            }
        }
    }

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        if (player->GetQuestStatus(29776) != QUEST_STATUS_NONE)
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I would like to go back on the top of the temple", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        SendGossipMenuFor(player, 13728, me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 action) override
    {
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            if (Creature* vehicle = player->SummonCreature(55685, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 20000ms))
            {
                player->EnterVehicle(vehicle);
            }
        }

        CloseGossipMenuFor(player);
        return true;
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_MASTER_SHANG_XI_TALK_1:
                {
                    UpdatePlayerList();
                    for (Player* player : players)
                    {
                        sCreatureTextMgr->SendChat(me, 0, player);
                        events.ScheduleEvent(EVENT_MASTER_SHANG_XI_TALK_2, 10000);
                    }
                    break;
                }
                case EVENT_MASTER_SHANG_XI_TALK_2:
                {
                    sCreatureTextMgr->SendChat(me, 1);
                    events.ScheduleEvent(EVENT_MASTER_SHANG_XI_TALK_3, 10000);
                    break;
                }
                case EVENT_MASTER_SHANG_XI_TALK_3:
                {
                    sCreatureTextMgr->SendChat(me, 2);
                    events.ScheduleEvent(EVENT_MASTER_SHANG_XI_TALK_4, 10000);
                    break;
                }
                case EVENT_MASTER_SHANG_XI_TALK_4:
                {
                    sCreatureTextMgr->SendChat(me, 3);
                    events.ScheduleEvent(EVENT_MASTER_SHANG_XI_TALK_5, 10000);
                    if (Creature* ji = me->FindNearestCreature(61127, 20.0f, true))
                    {
                        sCreatureTextMgr->SendChat(ji, 0);
                        ji->GetMotionMaster()->MovePoint(1, jiTempleMovePoint_2);
                        ji->DespawnOrUnsummon(2500);
                    }
                    break;
                }
                case EVENT_MASTER_SHANG_XI_TALK_5:
                {
                    sCreatureTextMgr->SendChat(me, 4);
                    events.ScheduleEvent(EVENT_MASTER_SHANG_XI_TALK_6, 10000);
                    if (Creature* aysa = me->FindNearestCreature(61126, 20.0f, true))
                    {
                        sCreatureTextMgr->SendChat(aysa, 0);
                        aysa->GetMotionMaster()->MovePoint(2, aysaTempleMovePoint_2);
                        aysa->DespawnOrUnsummon(2500);
                    }
                    break;
                }
                case EVENT_MASTER_SHANG_XI_TALK_6:
                {
                    UpdatePlayerList();
                    for (Player* player : players)
                        sCreatureTextMgr->SendChat(me, 5, player);
                    break;
                }
                case EVENT_MASTER_SHANG_XI_TALK_7:
                {
                    sCreatureTextMgr->SendChat(me, 6);
                    events.ScheduleEvent(EVENT_MASTER_SHANG_XI_TALK_8, 10000);
                    break;
                }
                case EVENT_MASTER_SHANG_XI_TALK_8:
                {
                    sCreatureTextMgr->SendChat(me, 7);
                    events.ScheduleEvent(EVENT_MASTER_SHANG_XI_TALK_9, 10000);
                    break;
                }
                case EVENT_MASTER_SHANG_XI_TALK_9:
                {
                    sCreatureTextMgr->SendChat(me, 8);
                    break;
                }
            }
        }
    }
};

// NPC 55685 - Wind Vehicle (Escort)
struct npc_wind_vehicleAI : public EscortAI
{
    npc_wind_vehicleAI(Creature* creature) : EscortAI(creature) { }

    uint32 IntroTimer;

    void Reset() override
    {
        IntroTimer = 100;
    }

    void WaypointReached(uint32 waypointId, uint32 pathId) override
    {
        if (waypointId == 6)
        {
            if (me->GetVehicleKit())
                me->GetVehicleKit()->RemoveAllPassengers();

            me->DespawnOrUnsummon();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (IntroTimer)
        {
            if (IntroTimer <= diff)
            {
                SetRun(true); Start(false);
                IntroTimer = 0;
            }
            else
                IntroTimer -= diff;
        }

        EscortAI::UpdateAI(diff);
    }
};

// AT - Wind Temple Entrance (Quest 29785)
struct AreaTrigger_at_wind_temple_entrance : public AreaTriggerScript
{
    enum
    {
        SPELL_SUMMON_AYSA_MARKER = 104571
    };

    AreaTrigger_at_wind_temple_entrance() : AreaTriggerScript("AreaTrigger_at_wind_temple_entrance") { }

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
    {
        if (player->GetQuestStatus(29785) == QUEST_STATUS_INCOMPLETE && !player->HasAura(SPELL_SUMMON_AYSA_MARKER))
            player->CastSpell(player, SPELL_SUMMON_AYSA_MARKER, true);
        return true;
    }
};

// NPC 64543/64532 - Aysa/Dafeng (Wind Temple Escort) | Quest 29785
struct npc_aysa_wind_temple_escortAI : public EscortAI
{
    enum
    {
        SPELL_SUMMON_AYSA_MARKER = 104571
    };

    npc_aysa_wind_temple_escortAI(Creature* creature) : EscortAI(creature)
    {
        if (auto const summon = me->ToTempSummon())
            summon->SetExplicitSeerGuid(summon->GetSummonerGUID());
    }

    uint32 introTimer_;

    void Reset() override
    {
        introTimer_ = 100;
        me->SetReactState(REACT_PASSIVE);
    }

    void DoAction(int32 /*actionId*/) override
    {
        SetEscortPaused(false);
    }

    void WaypointReached(uint32 waypointId, uint32 pathId) override
    {
        switch (waypointId)
        {
            case 2:
                SetEscortPaused(true);
                me->SetFacingTo(2.38f);
                break;
            case 7:
                SetEscortPaused(true);
                break;
            case 10:
            {
                auto const summoner = me->ToTempSummon()->GetSummoner();
                if (summoner && summoner->GetTypeId() == TYPEID_PLAYER)
                {
                    auto const player = summoner->ToPlayer();

                    player->KilledMonsterCredit(55666);
                    player->AreaExploredOrEventHappens(29785);
                    player->RemoveAurasDueToSpell(SPELL_SUMMON_AYSA_MARKER);
                }

                SetEscortPaused(true);
                me->DespawnOrUnsummon(10000); // need check time in retail
                break;
            }
            default:
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (introTimer_)
        {
            if (introTimer_ <= diff)
            {
                SetRun(true); Start(false);
                introTimer_ = 0;
            }
            else
                introTimer_ -= diff;
        }

        EscortAI::UpdateAI(diff);
    }
};

// NPC - Frightened Wind (Wind Temple)
struct npc_frightened_windAI : public ScriptedAI
{
    npc_frightened_windAI(Creature* creature) : ScriptedAI(creature) { }

    uint32 tornadeTimer;

    enum Spells
    {
        SPELL_TORNADE    = 107278,
    };

    void Reset() override
    {
        me->SetReactState(REACT_PASSIVE);
        tornadeTimer = 8 * IN_MILLISECONDS;
    }

    void UpdateAI(uint32 diff) override
    {
        if (tornadeTimer <= diff)
        {
            if (me->HasAura(SPELL_TORNADE))
                me->RemoveAurasDueToSpell(SPELL_TORNADE);
            else
                me->AddAura(SPELL_TORNADE, me);

            if (!me->HasAura(SPELL_TORNADE))
            {
                std::list<Creature*> aysaList;
                GetCreatureListWithEntryInGrid(aysaList, me, 55744, 50.0f);

                for (auto aysa: aysaList)
                    aysa->AI()->DoAction(1);
            }
            tornadeTimer = 8 * IN_MILLISECONDS;
        }
    }
};

// NPC 64543 - Aysa (Wind Temple) | Quest 29786
struct npc_aysa_in_wind_templeAI : public ScriptedAI
{
    npc_aysa_in_wind_templeAI(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == 29786) // Bataille Pyrotechnique
        {
            if (Creature* aysa = player->SummonCreature(64543, 543.94f, 4317.31f, 212.24f, 1.675520f, TEMPSUMMON_TIMED_DESPAWN, 10000ms))
            {
                aysa->SetExplicitSeerGuid(player->GetGUID());
                aysa->GetMotionMaster()->MovePoint(1, 643.45f, 4228.66f, 202.90f);
            }

            if (Creature* dafeng = player->SummonCreature(64532, 543.56f, 4320.97f, 212.24f, 5.445430f, TEMPSUMMON_TIMED_DESPAWN, 10000ms))
            {
                dafeng->SetExplicitSeerGuid(player->GetGUID());
                dafeng->GetMotionMaster()->MovePoint(1, 643.45f, 4228.66f, 202.90f);
            }
        }
    }
};

enum Enums
{
    NPC_ROCKET_LAUNCHER = 64507,
    SPELL_ROCKET_LAUNCH = 104855,

    EVENT_NEXT_MOVEMENT = 1,
    EVENT_STUNNED       = 2,
    EVENT_LIGHTNING     = 3,

    POINT_STUN          = 100,

    SPELL_SERPENT_SWEEP = 125990,
    SPELL_STUNNED       = 125992,
    SPELL_LIGHTNING     = 126006,
};

static Position const ZhaoPos[] =
{
    {719.36f, 4164.60f, 216.06f, 0.0f}, // Center
    {745.91f, 4154.35f, 223.48f, 0.0f},
    {717.04f, 4141.16f, 219.83f, 0.0f},
    {689.62f, 4153.16f, 217.63f, 0.0f},
    {684.53f, 4173.24f, 216.98f, 0.0f},
    {704.77f, 4190.16f, 218.24f, 0.0f},
    {736.90f, 4183.85f, 221.41f, 0.0f}
};

static const Position zhaoStunPos = {723.163025f, 4163.799805f, 202.082993f, 0.0f };

// NPC 55786 - Zhao Ren (Boss) | Quest 29786
struct boss_zhao_renAI : public ScriptedAI
{
    boss_zhao_renAI(Creature* creature) : ScriptedAI(creature) { }

    EventMap _events;
    bool eventStarted;
    uint8 hitCount;
    uint8 currentPos;

    void Reset() override
    {
        _events.Reset();
        me->SetReactState(REACT_PASSIVE);
        me->SetDisableGravity(true);

        eventStarted = false;
        hitCount = 0;
        currentPos = 0;

        me->SetFullHealth();
        me->RemoveAurasDueToSpell(SPELL_STUNNED);

        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MovePoint(0, ZhaoPos[0].GetPositionX(), ZhaoPos[0].GetPositionY(), ZhaoPos[0].GetPositionZ());
    }

    void SpellHit(WorldObject* /*caster*/, const SpellInfo* spell) override
    {
        if (spell->Id == SPELL_ROCKET_LAUNCH)
        {
            if (++hitCount >= 5)
            {
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(POINT_STUN, zhaoStunPos);
                hitCount = 0;
            }
        }
    }

    bool checkPlayers()
    {
        std::list<Player*> playerList;
        GetPlayerListInGrid(playerList, me, 80.0f);

        for (auto player : playerList)
            if (player->GetQuestStatus(29786) == QUEST_STATUS_INCOMPLETE)
                if (player->IsAlive())
                    return true;

        return false;
    }

    void GoToNextPos()
    {
        if (++currentPos > 6)
            currentPos = 1;

        me->GetMotionMaster()->MovePoint(currentPos, ZhaoPos[currentPos].GetPositionX(), ZhaoPos[currentPos].GetPositionY(), ZhaoPos[currentPos].GetPositionZ());
    }

    Player* GetRandomPlayer()
    {
        std::list<Player*> playerList;
        GetPlayerListInGrid(playerList, me, 50.0f);

        if (playerList.empty())
            return NULL;

        Trinity::Containers::RandomResizeList(playerList, 1);

        return *playerList.begin();
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (!pointId)
            return;

        if (pointId == POINT_STUN)
        {
            me->CastSpell(me, SPELL_STUNNED, true);
            _events.RescheduleEvent(EVENT_LIGHTNING, 17000);
            _events.ScheduleEvent(EVENT_STUNNED, 12000);
        }
        else
            _events.ScheduleEvent(EVENT_NEXT_MOVEMENT, 200);
    }

    void JustDied(Unit* /*killer*/) override
    {
        std::list<Player*> playerList;
        GetPlayerListInGrid(playerList, me, 50.0f);

        for (auto player : playerList)
            if (player->GetQuestStatus(29786) == QUEST_STATUS_INCOMPLETE)
                if (player->IsAlive())
                    player->KilledMonsterCredit(me->GetEntry());
    }

    void UpdateAI(uint32 diff) override
    {
        if (checkPlayers())
        {
            if (!eventStarted)  // Event not started, player found
            {
                _events.ScheduleEvent(EVENT_NEXT_MOVEMENT, 1000);
                _events.ScheduleEvent(EVENT_LIGHTNING, 5000);
                eventStarted = true;
            }
        }
        else
        {
            if (eventStarted)  // Event started, no player found
                Reset();

            return;
        }

        _events.Update(diff);

        switch (_events.ExecuteEvent())
        {
            case EVENT_NEXT_MOVEMENT:
            {
                if (me->HasAura(SPELL_STUNNED))
                    _events.ScheduleEvent(EVENT_NEXT_MOVEMENT, 2000);

                GoToNextPos();
                break;
            }
            case EVENT_STUNNED:
            {
                me->RemoveAurasDueToSpell(SPELL_STUNNED);
                me->CastSpell(me, SPELL_SERPENT_SWEEP, false);
                me->SetDisableGravity(true);
                _events.ScheduleEvent(EVENT_NEXT_MOVEMENT, 4000);
                break;
            }
            case EVENT_LIGHTNING:
            {
                if (Player* player = GetRandomPlayer())
                    me->CastSpell(player, SPELL_LIGHTNING, false);

                _events.ScheduleEvent(EVENT_LIGHTNING, 5000);
                break;
            }
        }
    }
};

// NPC 64507 - Rocket Launcher (Wind Temple)
struct npc_rocket_launcherAI : public ScriptedAI
{
    npc_rocket_launcherAI(Creature* creature) : ScriptedAI(creature) { }

    uint32 cooldown;

    void Reset() override
    {
        cooldown = 0;
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    }

    void OnSpellClick(Unit* Clicker, bool& /*result*/) override
    {
        if (cooldown)
            return;

        Creature* const zhao = GetClosestCreatureWithEntry(me, 55786, 50.0f);

        if (zhao && zhao->IsWithinDist2d(Clicker, 10.0f))
            me->CastSpell(zhao, SPELL_ROCKET_LAUNCH, false);
        else
        {
            Clicker->ToPlayer()->GetSession()->SendNotification("Wait until the Onyx Serpent is directly overhead.");
            return;
        }

        cooldown = 5000;
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        return;
    }

    void UpdateAI(uint32 diff) override
    {
        if (cooldown)
        {
            if (cooldown <= diff)
            {
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                cooldown = 0;
            }
            else
                cooldown -= diff;
        }
    }
};

// NPC 56159 - Master Shang Xi (After Zhao) | Quest 29787
struct npc_master_shang_xi_after_zhaoAI : public ScriptedAI
{
    npc_master_shang_xi_after_zhaoAI(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == 29787) // Worthy of Passing
        {
            if (Creature* master = player->SummonCreature(56159, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0ms))
            {
                master->SetExplicitSeerGuid(player->GetGUID());
                master->AI()->SetGUID(player->GetGUID());
            }
        }
    }
};

// 56159 - Worthy of Passing(29787)
struct npc_master_shang_xi_after_zhao_escortAI : public EscortAI
{
    npc_master_shang_xi_after_zhao_escortAI(Creature* creature) : EscortAI(creature)
    {
        summonGUID = ObjectGuid::Empty;
    }

    uint32 IntroTimer;

    ObjectGuid playerGuid;

    void Reset() override
    {
        IntroTimer = 250;
        me->SetReactState(REACT_PASSIVE);
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        playerGuid = guid;
    }

    void WaypointReached(uint32 waypointId, uint32 pathId) override
    {
        switch (waypointId)
        {
            case 1:
                Talk(0);
                break;
            case 4:
                Talk(1);
                break;
            case 6:
                Talk(2);
                break;
            case 7:
                me->SetWalk(true);
                break;
            case 8:
                Talk(3);
                break;
            case 10:
                if (Creature* creature = me->SummonCreature(56274, 845.89f, 4372.62f, 223.98f, 4.78f, TEMPSUMMON_CORPSE_DESPAWN, 0ms))
                {
                    creature->SetExplicitSeerGuid(playerGuid);
                    summonGUID = creature->GetGUID();
                    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    creature->SetReactState(REACT_DEFENSIVE);
                }
                break;
            case 15:
                me->SetFacingTo(5.91f);
                if (Creature* creature = Creature::GetCreature(*me, summonGUID))
                    creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                SetEscortPaused(true);
                me->SetWalk(false);
                break;
            case 16:
                me->SetFacingTo(4.537860f);

                if (Player* owner = ObjectAccessor::GetPlayer(*me, playerGuid))
                    owner->AddAura(59074, owner);
                break;
            case 17:
                Talk(5);
                break;
            case 21:
                me->DespawnOrUnsummon(1000);
                break;
            default:
                break;
        }
    }

    void SummonedCreatureDespawn(Creature* summon) override
    {
        if (summon->GetEntry() == 56274)
        {
            SetEscortPaused(false);
            Talk(4);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (IntroTimer)
        {
            if (IntroTimer <= diff)
            {
                SetRun(true); Start(false);
                IntroTimer = 0;
            }
            else
                IntroTimer -= diff;
        }

        EscortAI::UpdateAI(diff);
    }
private:
    ObjectGuid summonGUID;
};

// NPC 56686 - Master Shang Xi (Thousand Staff) | Quest 29790
struct npc_master_shang_xi_thousand_staffAI : public ScriptedAI
{
    npc_master_shang_xi_thousand_staffAI(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == 29790) // Passing Wisdom
        {
            if (Creature* master = player->SummonCreature(56686, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0ms))
            {
                master->SetExplicitSeerGuid(player->GetGUID());
                master->AI()->SetGUID(player->GetGUID());
            }
        }
    }
};

// Passing Wisdom (29790)
struct npc_master_shang_xi_thousand_staff_escortAI : public ScriptedAI
{
    npc_master_shang_xi_thousand_staff_escortAI(Creature* creature) : ScriptedAI(creature) { }

    uint32 IntroTimer;
    uint8 phase;

    ObjectGuid playerGuid;

    void Reset() override
    {
        phase = 0;
        IntroTimer = 250;
        me->SetReactState(REACT_PASSIVE);
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        playerGuid = guid;
    }

    void UpdateAI(uint32 diff) override
    {
        if (IntroTimer < diff)
        {
            if (phase < 6)
                Talk(phase, ObjectAccessor::GetPlayer(*me, playerGuid));

            IntroTimer = 15000;
            ++phase;

            if (phase == 7)
            {
                auto const creature = me->SummonCreature(57874, 873.09f, 4462.25f, 241.27f, 3.80f, TEMPSUMMON_TIMED_DESPAWN, 60000ms);
                creature->SetExplicitSeerGuid(playerGuid);

                me->DespawnOrUnsummon();

                if (Player* owner = ObjectAccessor::GetPlayer(*me, playerGuid))
                    owner->KilledMonsterCredit(56688);
            }
        }
        else IntroTimer -= diff;
    }
};

// Grab Air Balloon - 95247
class spell_grab_air_balloon: public SpellScriptLoader
{
    public:
        spell_grab_air_balloon() : SpellScriptLoader("spell_grab_air_balloon") { }

        class spell_grab_air_balloon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_grab_air_balloon_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                PreventHitAura();

                if (Unit* caster = GetCaster())
                    if (Creature* balloon = caster->SummonCreature(55649, 915.55f, 4563.66f, 230.68f, 2.298090f, TEMPSUMMON_MANUAL_DESPAWN, 0ms))
                    {
                        balloon->SetExplicitSeerGuid(caster->GetGUID());
                        caster->EnterVehicle(balloon, 0);
                    }
            }

            void Register() override
            {
                OnEffectLaunchTarget += SpellEffectFn(spell_grab_air_balloon_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_grab_air_balloon_SpellScript();
        }
};

// NPC 55649 - Shang Xi Air Balloon (Vehicle)
struct npc_shang_xi_air_balloonAI : public EscortAI
{
    npc_shang_xi_air_balloonAI(Creature* creature) : EscortAI(creature) { }

    ObjectGuid playerGUID;
    uint32 eventTimer;
    uint32 phase;

    void Reset() override
    {
        playerGUID = ObjectGuid::Empty;
        eventTimer = 250;
        phase = 0;

        me->setActive(true);
        me->SetReactState(REACT_PASSIVE);
    }

    void RemoveNpcPassengers()
    {
        for (auto i = 1; i != 3; ++i)
        {
            auto const passenger = me->GetVehicleKit()->GetPassenger(i);
            if (!passenger)
                continue;

            passenger->ExitVehicle();
            passenger->ToCreature()->DespawnOrUnsummon(1000);
        }
    }

    void WaypointReached(uint32 waypointId, uint32 pathId) override
    {
        switch (waypointId)
        {
            case 11:
                RemoveNpcPassengers();
                break;
            case 12:
            {
                auto const passenger = me->GetVehicleKit()->GetPassenger(0);
                if (passenger && passenger->GetTypeId() == TYPEID_PLAYER)
                {
                    passenger->ToPlayer()->KilledMonsterCredit(55939);
                    passenger->AddAura(50550, passenger);
                }

                me->GetVehicleKit()->RemoveAllPassengers();
                break;
            }
        }
    }

    /*
        seat 0 = player
        seat 1 = Ji Firepaw
        seat 2 = Aysa Cloudsinger
    */

    void UpdateAI(uint32 diff) override final
    {
        if (playerGUID == 0)
        {
            RemoveNpcPassengers();
            me->DespawnOrUnsummon();
            return;
        }

        if (phase <= 24)
        {
            if (eventTimer <= diff)
            {
                if (phase == 0)
                {
                    SetRun(true); Start(false);
                    eventTimer = 2000;
                }
                else if (phase == 1)
                {
                    PassengerTalk(0, 1);
                    eventTimer = 5000;
                }
                else if (phase == 2)
                {
                    PassengerTalk(0, 2);
                    eventTimer = 8000;
                }
                else if (phase == 3)
                {
                    PassengerTalk(1, 1);
                    eventTimer = 6000;
                }
                else if (phase == 4)
                {
                    PassengerTalk(1, 2);
                    eventTimer = 6000;
                }
                else if (phase == 5)
                {
                    PassengerTalk(2, 1);
                    eventTimer = 6000;
                }
                else if (phase == 6)
                {
                    PassengerTalk(2, 2);
                    eventTimer = 7500;
                }
                else if (phase == 7)
                {
                    PassengerTalk(3, 2);
                    eventTimer = 6000;
                }
                else if (phase == 8)
                {
                    PassengerTalk(4, 2);
                    eventTimer = 6000;
                }
                // Need custom chat builder as creature is too far.
                else if (phase == 9)
                {
                    Talk(0); // I am in pain, but it warms my heart that Liu Lang's grandchildren have not forgotten me. 27822
                    eventTimer = 7000;
                }
                else if (phase == 10)
                {
                    Talk(1); // There is a thorn in my side. I cannot remove it. 27823
                    eventTimer = 5000;
                }
                else if (phase == 11)
                {
                    Talk(2); // The pain is unbearable, and I can no longer swim straight. 27824
                    eventTimer = 5000;
                }
                else if (phase == 12)
                {
                    Talk(3); // Please grandchildren, can you remove this thorn? I cannot do so on my own. 27825
                    eventTimer = 7000;
                }
                else if (phase == 13)
                {
                    PassengerTalk(5, 2);
                    eventTimer = 7000;
                }
                else if (phase == 14)
                {
                    Talk(4); // It is in the forest where your feet do not walk. Continue along the mountains and you will find it. 27826
                    eventTimer = 8000;
                }
                else if (phase == 15)
                {
                    PassengerTalk(6, 2);
                    eventTimer = 6000;
                }
                else if (phase == 16)
                {
                    me->SetSpeed(MOVE_FLIGHT, 3.0f, true);
                    Talk(5); // Thank you, grandchildren. 27827
                    eventTimer = 8000;
                }
                else if (phase == 17)
                {
                    PassengerTalk(3, 1);
                    eventTimer = 6000;
                }
                else if (phase == 18)
                {
                    PassengerTalk(4, 1);
                    eventTimer = 8000;
                    me->SetSpeed(MOVE_FLIGHT, 4.0f, true);
                }
                else if (phase == 19)
                {
                    PassengerTalk(7, 2);
                    eventTimer = 8000;
                }
                else if (phase == 20)
                {
                    PassengerTalk(5, 1);
                    eventTimer = 8000;
                }
                else if (phase == 21)
                {
                    PassengerTalk(8, 2);
                    eventTimer = 6000;
                }
                else if (phase == 22)
                {
                    PassengerTalk(6, 1);
                    eventTimer = 6000;
                }
                else if (phase == 23)
                {
                    PassengerTalk(9, 2);
                    eventTimer = 6000;
                }
                else if (phase == 24)
                {
                    PassengerTalk(10, 2);
                    eventTimer = 6000;
                }
                ++phase;
            }
            else
            {
                eventTimer -= diff;
            }
        }

        EscortAI::UpdateAI(diff);
    }

    void PassengerTalk(uint32 talkId, uint32 seatId)
    {
        if (Unit* unit = me->GetVehicleKit()->GetPassenger(seatId))
            if (Creature* creature = unit->ToCreature())
                creature->AI()->Talk(talkId, ObjectAccessor::GetPlayer(*me, playerGUID));
    }

    void PassengerBoarded(Unit* passenger, int8 seatId, bool apply) override
    {
        if (seatId != 0 || passenger->GetTypeId() != TYPEID_PLAYER)
            return;

        if (apply)
        {
            auto const player = passenger->ToPlayer();

            if (auto const firepaw = player->SummonCreature(56660, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0.f, TEMPSUMMON_TIMED_DESPAWN, 300000ms))
                firepaw->EnterVehicle(me, 1);

            if (auto const aysa = player->SummonCreature(56662, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0.f, TEMPSUMMON_TIMED_DESPAWN, 300000ms))
                aysa->EnterVehicle(me, 2);

            playerGUID = player->GetGUID();

            player->KilledMonsterCredit(56378);
        }
        else
            playerGUID = ObjectGuid::Empty;
    }
};

class spell_monkey_wisdom : public SpellScriptLoader
{
    public:
        spell_monkey_wisdom() : SpellScriptLoader("spell_monkey_wisdom") { }

        class spell_monkey_wisdom_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_monkey_wisdom_SpellScript);

            void HandleScript(SpellEffIndex /*eff*/)
            {
                uint32 wisdoms[9] = {
                    54074, // Peel banana first, eat second.
                    54077, // Wet fur not fun to sleep on.
                    54078, // Don't roll in own poo unless want to smell like poo all day.
                    54075, // Steal a banana from a hozen, expect a angry hozen.
                    54080, // Poo not good to eat, but very good to throw.
                    54073, // Don't throw banana peel where going to walk.
                    54076, // Firecracker for throwing, banana for eating.
                    54079  // Don't pull own tail when there are other tails to pull.
                };

                auto target = GetHitUnit();
                if (GetCaster() != target)
                {
                    BroadcastText const* bct = sObjectMgr->GetBroadcastText(wisdoms[urand(0, 7)]);
                    Player* _player = target->ToPlayer();
                    LocaleConstant loc_idx = _player->GetSession()->GetSessionDbLocaleIndex();
                    std::string baseText = "";
                    if (bct)
                        baseText = bct->GetText(loc_idx, _player->GetGender());

                    target->TextEmote(baseText.c_str(), nullptr); 
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_monkey_wisdom_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_KILL_CREDIT2);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_monkey_wisdom_SpellScript();
        }
};

// NPC 56660 - Ji Firepaw (Pre-Balloon Event) | Quest 29791
struct mob_aisa_pre_balon_eventAI : public ScriptedAI
{
    mob_aisa_pre_balon_eventAI(Creature* creature) : ScriptedAI(creature) { }

    bool justSpeaking;
    EventMap _events;
    GuidSet m_player_for_event;

    enum events
    {
        EVENT_1 = 1,
        EVENT_2 = 2,
        EVENT_3 = 3,

        NPC_FRIEND = 56663,
    };

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == 29791)
            sCreatureTextMgr->SendChat(me, 1);

        me->CastSpell(player, 95247, true);
        player->CastSpell(player, 95247, true);
    }

    void Reset() override
    {
        justSpeaking = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (justSpeaking || who->GetTypeId() != TYPEID_PLAYER || who->IsOnVehicle())
            return;

        GuidSet::iterator itr = m_player_for_event.find(who->GetGUID());

        if (itr != m_player_for_event.end())
            return;

        if (who->ToPlayer()->GetQuestStatus(29790) != QUEST_STATUS_COMPLETE)
            return;

        m_player_for_event.insert(who->GetGUID());
        justSpeaking = true;
        _events.ScheduleEvent(EVENT_1, 10000);
        sCreatureTextMgr->SendChat(me, 0);
        
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_1:
                {
                    _events.ScheduleEvent(EVENT_2, 8000);

                    if (Creature* f = me->FindNearestCreature(NPC_FRIEND, 100.0f, true))
                    {
                        sCreatureTextMgr->SendChat(f, 0);
                        f->SetFacingToObject(me);
                    }
                    break;
                }
                case EVENT_2:
                    sCreatureTextMgr->SendChat(me, 2);
                    justSpeaking = false;
                    break;
            }
        }
    }
};

// NPC 56660 - Ji Firepaw (MOP Air Balloon Escort)
struct mop_air_balloonAI : public EscortAI
{
    mop_air_balloonAI(Creature* creature) : EscortAI(creature) { }

    ObjectGuid playerGuid;
    ObjectGuid aisaGUID;
    ObjectGuid firepawGUID;
    ObjectGuid shenZiGUID;
    ObjectGuid headGUID;
    EventMap events;

    void Reset() override
    {
        me->SetWalk(false);
        me->SetSpeed(MOVE_FLIGHT, 10.0f, false);
        events.Reset();

        me->setActive(true);
        me->SetReactState(REACT_PASSIVE);
        me->m_invisibilityDetect.AddFlag(INVISIBILITY_UNK5);
        me->m_invisibilityDetect.AddValue(INVISIBILITY_UNK5, 999);
    }

    enum localdata
    {
        NPC_AISA = 56661,
        NPC_FIREPAW = 56660,
        NPC_SHEN_ZI_SU = 56676,
        NPC_TURTLE_HEAD = 57769,

        SPELL_HEAD_ANIM_RISE = 114888,
        SPELL_HEAD_ANIM_1 = 114898,
        SPELL_HEAD_ANIM_2 = 118571,
        SPELL_HEAD_ANIM_3 = 118572,
        SPELL_VOICE_ANIM = 106759,

        SPELL_AISA_ENTER_SEAT_2 = 63313, //106617

        SPELL_CREDIT_1 = 105895,
        SPELL_CREDIT_2 = 105010,
        SPELL_EJECT_PASSANGER = 60603,
        SPELL_PARASHUT = 45472,

        EVENT_1 = 1, // 17:24:47.000

        EVENT_AISA_TALK_0 = 2,  //17:24:51.000
        EVENT_AISA_TALK_1 = 3,  //17:25:07.000
        EVENT_AISA_TALK_2 = 4,  //17:25:18.000
        EVENT_AISA_TALK_3 = 5,  //17:25:31.000
        EVENT_AISA_TALK_4 = 6,  //17:25:38.000
        EVENT_AISA_TALK_5 = 7,  //17:26:40.000
        EVENT_AISA_TALK_6 = 8,  //17:27:02.000
        EVENT_AISA_TALK_7 = 9,  //17:27:29.000
        EVENT_AISA_TALK_8 = 10, //17:27:50.000
        EVENT_AISA_TALK_9 = 11, //17:28:04.000
        EVENT_AISA_TALK_10 = 12, //17:28:10.000

        EVENT_FIREPAW_TALK_0 = 13, //17:24:47.000
        EVENT_FIREPAW_TALK_1 = 14, //17:24:57.000
        EVENT_FIREPAW_TALK_2 = 15, //17:25:13.000
        EVENT_FIREPAW_TALK_3 = 16, //17:27:16.000
        EVENT_FIREPAW_TALK_4 = 17, //17:27:22.000
        EVENT_FIREPAW_TALK_5 = 18, //17:27:43.000
        EVENT_FIREPAW_TALK_6 = 19, //17:27:57.000

        EVENT_SHEN_ZI_SU_TALK_0 = 20, //17:25:44.000
        EVENT_SHEN_ZI_SU_TALK_1 = 21, //17:25:58.000
        EVENT_SHEN_ZI_SU_TALK_2 = 22, //17:26:12.000
        EVENT_SHEN_ZI_SU_TALK_3 = 23, //17:26:25.000
        EVENT_SHEN_ZI_SU_TALK_4 = 24, //17:26:47.000 
        EVENT_SHEN_ZI_SU_TALK_5 = 25, //17:27:09.000
    };

    void InitTalking(Player* player) 
    {
        me->GetMap()->LoadGrid(865.222f, 4986.84f); //voice
        me->GetMap()->LoadGrid(868.356f, 4631.19f); //head

        uint32 t = 3000;
        events.ScheduleEvent(EVENT_FIREPAW_TALK_0, t += 1000);       //17:24:47.000
        events.ScheduleEvent(EVENT_AISA_TALK_0, t += 4000);          //17:24:51.000
        events.ScheduleEvent(EVENT_FIREPAW_TALK_1, t += 6000);       //17:24:57.000
        events.ScheduleEvent(EVENT_AISA_TALK_1, t += 10000);         //17:25:07.000
        events.ScheduleEvent(EVENT_FIREPAW_TALK_2, t += 6000);       //17:25:13.000
        events.ScheduleEvent(EVENT_AISA_TALK_2, t += 5000);          //17:25:18.000
        events.ScheduleEvent(EVENT_AISA_TALK_3, t += 14000);         //17:25:31.000
        events.ScheduleEvent(EVENT_AISA_TALK_4, t += 6000);          //17:25:38.000
        events.ScheduleEvent(EVENT_SHEN_ZI_SU_TALK_0, t += 6000);    //17:25:44.000
        events.ScheduleEvent(EVENT_SHEN_ZI_SU_TALK_1, t += 14000);   //17:25:58.000
        events.ScheduleEvent(EVENT_SHEN_ZI_SU_TALK_2, t += 14000);   //17:26:12.000
        events.ScheduleEvent(EVENT_SHEN_ZI_SU_TALK_3, t += 13000);   //17:26:25.000
        events.ScheduleEvent(EVENT_AISA_TALK_5, t += 15000);         //17:26:40.000
        events.ScheduleEvent(EVENT_SHEN_ZI_SU_TALK_4, t += 7000);    //17:26:47.000 
        events.ScheduleEvent(EVENT_AISA_TALK_6, t += 15000);         //17:27:02.000
        events.ScheduleEvent(EVENT_SHEN_ZI_SU_TALK_5, t += 7000);    //17:27:09.000
        events.ScheduleEvent(EVENT_FIREPAW_TALK_3, t += 7000);       //17:27:16.000
        events.ScheduleEvent(EVENT_FIREPAW_TALK_4, t += 6000);       //17:27:22.000
        events.ScheduleEvent(EVENT_AISA_TALK_7, t += 7000);          //17:27:29.000
        events.ScheduleEvent(EVENT_FIREPAW_TALK_5, t += 14000);      //17:27:43.000
        events.ScheduleEvent(EVENT_AISA_TALK_8, t += 7000);          //17:27:50.000
        events.ScheduleEvent(EVENT_FIREPAW_TALK_6, t += 7000);       //17:27:57.000
        events.ScheduleEvent(EVENT_AISA_TALK_9, t += 7000);          //17:28:04.000
        events.ScheduleEvent(EVENT_AISA_TALK_10, t += 7000);         //17:28:10.000
    }

    void PassengerBoarded(Unit* passenger, int8 seatId, bool apply) override
    {
        if (!apply)
        {
            if (passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->DespawnOrUnsummon(1000);
                me->CastSpell(passenger, 45472, true);
                passenger->ToPlayer()->KilledMonsterCredit(55939);
            }
            else
                passenger->ToCreature()->DespawnOrUnsummon(1000);
            return;
        }

        if (seatId == 0)
        {
            if (Player* player = passenger->ToPlayer())
            {
                playerGuid = player->GetGUID();
                me->CastSpell(player, 105895, true);
                InitTalking(player);
            }
        }

        if (passenger->GetTypeId() != TYPEID_PLAYER)
        {
            passenger->m_invisibilityDetect.AddFlag(INVISIBILITY_UNK5);
            passenger->m_invisibilityDetect.AddValue(INVISIBILITY_UNK5, 999);

            switch (passenger->GetEntry())
            {
                case 56661:
                    aisaGUID = passenger->GetGUID();
                    break;
                case 56660:
                    firepawGUID = passenger->GetGUID();
                    break;
                default:
                    break;
            }
        }
    }

    void TalkShenZiSU(uint32 text)
    {
        Creature *shen = me->GetMap()->GetCreature(shenZiGUID);

        if (!shen)
            return;

        if (Player* plr = ObjectAccessor::FindPlayer(playerGuid))
        {
            Creature *head = me->GetMap()->GetCreature(headGUID);
            if (!head)
                return;

            switch (text)
            {
                //cast 114888                      //17:25:31.000
                case 0:                            //17:25:44.000
                    plr->CastSpell(shen, SPELL_HEAD_ANIM_1, false);
                    break;
                case 1:                            //17:25:58.000
                    plr->CastSpell(shen, SPELL_VOICE_ANIM, false);
                    break;
                case 2:                            //17:26:11.000
                case 3:                            //17:26:25.000
                case 5:                            //17:27:08.000
                    plr->CastSpell(shen, SPELL_HEAD_ANIM_2, false);
                    break;
                case 4:                            //17:26:47.000
                    plr->CastSpell(shen, SPELL_HEAD_ANIM_3, false);
                    break;
            }
            if (text == 5) // restore emote
            {
                head->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, ANIM_FLY_LAND);   //hack
                //plr->RemoveFromExtraLook(head->GetGUID());
            }
        }
        sCreatureTextMgr->SendChat(shen, text);
    }

    void IsSummonedBy(Unit* summoner) override
    {
        me->SetTarget(summoner->GetGUID());
        playerGuid = summoner->GetGUID();
        summoner->EnterVehicle(me, 0);
        events.ScheduleEvent(EVENT_1, 1000);
    }

    void WaypointReached(uint32 waypointId, uint32 pathId) override
    {
        switch (waypointId)
        {
            case 8:
                me->SetSpeed(MOVE_FLIGHT, 8.0f, false);
                break;
            case 15:
            {
                if (Player* plr = ObjectAccessor::FindPlayer(playerGuid))
                    me->CastSpell(plr, SPELL_CREDIT_2, true);
                break;
            }
            default:
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);
        EscortAI::UpdateAI(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_1:
                {
                    if (Creature* f = me->FindNearestCreature(NPC_AISA, 100.0f, true))
                        f->CastSpell(me, SPELL_AISA_ENTER_SEAT_2, true);

                    SetRun(true); Start(false);
                    break;
                }
                case EVENT_AISA_TALK_3:
                    if (Creature *head = me->GetMap()->GetCreature(headGUID))
                        if (Player* plr = ObjectAccessor::FindPlayer(playerGuid))
                        {
                            plr->CastSpell(plr, SPELL_HEAD_ANIM_RISE, false);    //17:25:31.000
                            head->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, 0);  //hack
                        }
                case EVENT_AISA_TALK_0:
                case EVENT_AISA_TALK_1:
                case EVENT_AISA_TALK_2:
                case EVENT_AISA_TALK_4:
                case EVENT_AISA_TALK_5:
                case EVENT_AISA_TALK_6:
                case EVENT_AISA_TALK_7:
                case EVENT_AISA_TALK_8:
                case EVENT_AISA_TALK_9:
                case EVENT_AISA_TALK_10:
                {
                    if (Creature *aisa = me->GetMap()->GetCreature(aisaGUID))
                        sCreatureTextMgr->SendChat(aisa, eventId - 2, 0);
                    break;
                }

                case EVENT_FIREPAW_TALK_0:
                case EVENT_FIREPAW_TALK_1:
                case EVENT_FIREPAW_TALK_2:
                case EVENT_FIREPAW_TALK_3:
                case EVENT_FIREPAW_TALK_4:
                case EVENT_FIREPAW_TALK_5:
                case EVENT_FIREPAW_TALK_6:
                {
                    if (Creature *paw = me->GetMap()->GetCreature(firepawGUID))
                        sCreatureTextMgr->SendChat(paw, eventId - 13, 0);
                    break;
                }
                case EVENT_SHEN_ZI_SU_TALK_0:   //114898
                case EVENT_SHEN_ZI_SU_TALK_1:   //cast 106759
                case EVENT_SHEN_ZI_SU_TALK_2:   //cast 118571
                case EVENT_SHEN_ZI_SU_TALK_3:   //118571
                case EVENT_SHEN_ZI_SU_TALK_4:   //118572
                case EVENT_SHEN_ZI_SU_TALK_5:   //118571
                    TalkShenZiSU(eventId - 20);
                    break;
            }
        }
    }
};

// NPC 59960 - Ji Firepaw (summoned helper) | Quests 29779/29780/29781
struct npc_ji_firepawAI : public ScriptedAI
{
    npc_ji_firepawAI(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        std::list<Creature*> summonList;
        GetCreatureListWithEntryInGrid(summonList, player, 59960, 6.0f);

        if (summonList.empty())
        {
            if (quest->GetQuestId() == 29779 || quest->GetQuestId() == 29780 || quest->GetQuestId() == 29781)
                player->SummonCreature(59960, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0ms, player->GetGUID());
        }
    }
};

struct npc_ji_firepaw_escortAI : public ScriptedAI
{
    npc_ji_firepaw_escortAI(Creature* creature) : ScriptedAI(creature) { }

    uint64 playerGuid = 0;

    void IsSummonedBy(Unit* summoner) override
    {
        playerGuid = summoner->GetGUID();
        me->GetMotionMaster()->MoveFollow(summoner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!playerGuid)
            return;

        Player* summoner = ObjectAccessor::FindPlayer(ObjectGuid(playerGuid));
        if (!summoner)
            return;

        if (Unit* target = summoner->GetVictim())
        {
            me->Attack(target, true);
            DoMeleeAttackIfReady();
            return;
        }

        if ((summoner->GetQuestStatus(29780) == QUEST_STATUS_COMPLETE || summoner->GetQuestStatus(29780) == QUEST_STATUS_REWARDED) &&
            (summoner->GetQuestStatus(29779) == QUEST_STATUS_COMPLETE || summoner->GetQuestStatus(29779) == QUEST_STATUS_REWARDED) &&
            (summoner->GetQuestStatus(29781) == QUEST_STATUS_COMPLETE || summoner->GetQuestStatus(29781) == QUEST_STATUS_REWARDED))
            me->DespawnOrUnsummon();
    }
};

// NPC 56274 - Guardian of the Elders
struct npc_guardian_of_the_eldersAI : public ScriptedAI
{
    npc_guardian_of_the_eldersAI(Creature* creature) : ScriptedAI(creature) { }

    void JustDied(Unit* /*killer*/) override
    {
        if (GameObject* door = me->FindNearestGameObject(209922, 40.0f))
        {
            door->UseDoorOrButton(60000);

            if (Creature* masterShangXi = me->FindNearestCreature(56159, 40.0f))
            {
                masterShangXi->SetWalk(false);
                masterShangXi->GetMotionMaster()->MovePoint(10, masterShangXi->GetPositionX(), masterShangXi->GetPositionY(), masterShangXi->GetPositionZ());
            }
        }
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        DoMeleeAttackIfReady();
    }
};

// NPC 54975 - Aysa (Chamber of Whispers)
struct npc_aysa_chamber_of_whispersAI : public ScriptedAI
{
    npc_aysa_chamber_of_whispersAI(Creature* creature) : ScriptedAI(creature)
    {
        me->Say("Wait!", LANG_UNIVERSAL);
        introTimer = 1000;
    }

    uint32 introTimer = 0;
    uint32 deactivate1Timer = 0;
    uint32 moveMidTimer = 0;
    uint32 movePos3Timer = 0;
    uint32 deactivate2Timer = 0;
    uint32 moveEndTimer = 0;
    uint32 outroTimer = 0;
    uint8 phase = 0;

    void UpdateAI(uint32 diff) override
    {
        switch (phase)
        {
            case 0:
                if (introTimer && introTimer <= diff)
                {
                    me->Say("We need to wait for the winds to settle, then make a break for the cover of the far hallway.", LANG_UNIVERSAL);
                    me->GetMotionMaster()->MovePoint(0, 647.493f, 4224.63f, 202.90865f);
                    deactivate1Timer = 2000;
                    phase = 1;
                }
                else if (introTimer)
                    introTimer -= diff;
                break;
            case 1:
                if (deactivate1Timer && deactivate1Timer <= diff)
                {
                    if (GameObject* cloud1 = me->FindNearestGameObject(209685, 30.0f))
                        cloud1->UseDoorOrButton(60000);
                    moveMidTimer = 5000;
                    phase = 2;
                }
                else if (deactivate1Timer)
                    deactivate1Timer -= diff;
                break;
            case 2:
                if (moveMidTimer && moveMidTimer <= diff)
                {
                    me->GetMotionMaster()->MovePoint(1, 598.57294f, 4266.661f, 206.54927f);
                    movePos3Timer = 10000;
                    phase = 3;
                }
                else if (moveMidTimer)
                    moveMidTimer -= diff;
                break;
            case 3:
                if (movePos3Timer && movePos3Timer <= diff)
                {
                    me->Say("Wait for another opening. I'll meet you on the far side.", LANG_UNIVERSAL);
                    me->GetMotionMaster()->MovePoint(2, 580.1649f, 4283.193f, 210.18248f);
                    std::list<GameObject*> clouds;
                    me->GetGameObjectListWithEntryInGrid(clouds, 209685, 60.0f);
                    for (GameObject* cloud : clouds)
                        if (cloud->GetGUID().GetCounter() == 88728)
                            cloud->UseDoorOrButton(60000);
                    moveEndTimer = 1000;
                    phase = 4;
                }
                else if (movePos3Timer)
                    movePos3Timer -= diff;
                break;
            case 4:
                if (moveEndTimer && moveEndTimer <= diff)
                {
                    me->GetMotionMaster()->MovePoint(3, 543.9549f, 4317.2744f, 212.22935f);
                    outroTimer = 10000;
                    phase = 5;
                }
                else if (moveEndTimer)
                    moveEndTimer -= diff;
                break;
            case 5:
                if (outroTimer && outroTimer <= diff)
                {
                    me->Say("Dafeng! What's wrong? Why are you hiding back here?", LANG_UNIVERSAL);
                    std::list<Player*> playerList;
                    GetPlayerListInGrid(playerList, me, 45.0f);
                    for (Player* player : playerList)
                        player->KilledMonsterCredit(55666, ObjectGuid::Empty);
                    me->DespawnOrUnsummon();
                    phase = 6;
                }
                else if (outroTimer)
                    outroTimer -= diff;
                break;
            default:
                break;
        }
    }
};

void AddSC_wandering_island_west()
{
    RegisterCreatureAI(npc_master_shang_xi_temple);
    RegisterCreatureAI(npc_wind_vehicleAI);
    new AreaTrigger_at_wind_temple_entrance();
    RegisterCreatureAI(npc_aysa_wind_temple_escortAI);
    RegisterCreatureAI(npc_frightened_windAI);
    RegisterCreatureAI(npc_aysa_in_wind_templeAI);
    RegisterCreatureAI(boss_zhao_renAI);
    RegisterCreatureAI(npc_rocket_launcherAI);
    RegisterCreatureAI(npc_master_shang_xi_after_zhaoAI);
    RegisterCreatureAI(npc_master_shang_xi_after_zhao_escortAI);
    RegisterCreatureAI(npc_master_shang_xi_thousand_staffAI);
    RegisterCreatureAI(npc_master_shang_xi_thousand_staff_escortAI);
    new spell_grab_air_balloon();
    RegisterCreatureAI(npc_shang_xi_air_balloonAI);
    new spell_monkey_wisdom();
    RegisterCreatureAI(mob_aisa_pre_balon_eventAI);
    RegisterCreatureAI(mop_air_balloonAI);
    RegisterCreatureAI(npc_ji_firepawAI);
    RegisterCreatureAI(npc_ji_firepaw_escortAI);
    RegisterCreatureAI(npc_guardian_of_the_eldersAI);
    RegisterCreatureAI(npc_aysa_chamber_of_whispersAI);
}
