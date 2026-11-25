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

/* ScriptDataSDName: WanderingIslandNorth
SD%Complete: 100
SDComment: Quest support: 29408, 29409, 29414, 29419, 29420, 29421, 29664
SDCategory: WanderingIsland
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "SpellScript.h"
#include "Object.h"

// NPC 53566/54608/54609/54784/54786/55586/55672/56159/56686/64530 - Master Shang Xi | Quest 29408 - The Lesson of the Burning Scroll
struct npc_master_shang_xiAI : public ScriptedAI
{
    npc_master_shang_xiAI(Creature* creature) : ScriptedAI(creature), _resetTimer(10000) { }

    enum master_shang
    {
        SPELL_MASTERS_FLAME         = 114610,
        SPELL_CREATE_MASTERS_FLAME  = 114611,
        SPELL_SNATCH_MASTERS_FLAME  = 114746,

        ITEM_MASTERS_FLAME          = 80212,

        QUEST_LESSONS_OF_BURNING_SCROLL = 29408,
    };

    void OnQuestAccept(Player* /*player*/, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_LESSONS_OF_BURNING_SCROLL) // The Lesson of the Burning Scroll
        {
            me->AddAura(SPELL_MASTERS_FLAME, me);
            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
        }
    }

    void SpellHit(Unit* caster, const SpellInfo* spell) override
    {
        if (spell->Id == SPELL_SNATCH_MASTERS_FLAME)
        {
            if (caster->GetTypeId() == TYPEID_PLAYER)
            {
                if (caster->ToPlayer()->GetQuestStatus(QUEST_LESSONS_OF_BURNING_SCROLL) == QUEST_STATUS_INCOMPLETE)
                {
                    me->CastSpell(caster, SPELL_CREATE_MASTERS_FLAME, true);
                    me->RemoveAurasDueToSpell(SPELL_MASTERS_FLAME);
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    Talk(0);
                }
            }
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        auto const player = who->ToPlayer();
        if (!player)
            return;

        if (player->GetQuestStatus(QUEST_LESSONS_OF_BURNING_SCROLL) == QUEST_STATUS_INCOMPLETE && !player->HasItemCount(ITEM_MASTERS_FLAME) && !me->HasAura(SPELL_MASTERS_FLAME))
            me->AddAura(SPELL_MASTERS_FLAME, me);
    }

    void UpdateAI(uint32 diff) override
    {
        // In case noone used spellclick - reset questgiver flag in periodic way
        if (me->HasFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
            return;

        if (_resetTimer <= diff)
        {
            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            _resetTimer = 10000;
        }
        else
            _resetTimer -= diff;
    }

private:
    uint32 _resetTimer;
};

// NPC 53565/65469 - Aspiring Trainee | Quest 29409
struct npc_training_target : public ScriptedAI
{
    npc_training_target(Creature* creature) : ScriptedAI(creature), _resetTimer(0) { }

    void Reset() override
    {
        me->SetReactState(REACT_PASSIVE);
        // disable rotate
        me->SetControlled(true, UNIT_STATE_STUNNED);
        // immune to knock aways like blast wave
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);

        _scheduler.Schedule(500ms, [this](TaskContext /*task*/)
        {
            std::list<Creature*> trainees;
            me->GetCreaturesWithEntryInRange(trainees, 4.0f, 65469);
            me->GetCreaturesWithEntryInRange(trainees, 4.0f, 53565);
            for (auto unit : trainees)
                unit->AI()->SetData(1, 1);
        });

        _resetTimer = 5 * IN_MILLISECONDS;
    }

    void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
    {
        if (_EnterEvadeMode())
            Reset();
    }

    void JustDied(Unit* /*killer*/) override
    {
        std::list<Creature*> trainees;
        me->GetCreaturesWithEntryInRange(trainees, 4.0f, 65469);
        me->GetCreaturesWithEntryInRange(trainees, 4.0f, 53565);
        for (auto unit : trainees)
            unit->AI()->SetData(2, 2);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/) override
    {
        _resetTimer = 5 * IN_MILLISECONDS;
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        if (_resetTimer <= diff)
        {
            EnterEvadeMode();
            _resetTimer = 5000;
        }
        else
            _resetTimer -= diff;
    }

private:
    uint32 _resetTimer;
    TaskScheduler _scheduler;
};

// NPC 54611/57750 - Jaomin Ro | Quest 29409
struct npc_jaomin_roAI : public ScriptedAI
{
    npc_jaomin_roAI(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(29409) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* vehicle = player->SummonCreature(55685, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 20000ms))
            {
                vehicle->AI()->Reset();
            }
        }
        return true;
    }
};

// NPC 54611/57750 - Jaomin Ro AI | Quest 29409
struct boss_jaomin_roAI : public ScriptedAI
{
    boss_jaomin_roAI(Creature* creature) : ScriptedAI(creature) { }

    enum eEvents
    {
        EVENT_JAOMIN_JUMP   = 1,
        EVENT_HIT_CIRCLE    = 2,
        EVENT_FALCON        = 3,
        EVENT_RESET         = 4,
        EVENT_CHECK_AREA    = 5,
    };

    EventMap events;
    bool isInFalcon = false;

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_JAOMIN_JUMP, 1000);
        events.ScheduleEvent(EVENT_HIT_CIRCLE, 2000);
        events.ScheduleEvent(EVENT_CHECK_AREA, 2500);
    }

    void Reset() override
    {
        isInFalcon = false;
        me->SetDisplayId(39755);
        me->SetFaction(2357); // mechant!
    }

    void MoveInLineOfSight(Unit* who) override
    {
        auto const player = who->ToPlayer();
        if (!player)
            return;

        if (player->GetQuestStatus(29409) != QUEST_STATUS_INCOMPLETE)
            return;

        if (who->GetDistance(me) < 15.f)
        {
            if (me->GetStandState() != UNIT_STAND_STATE_STAND)
            {
                Talk(0);
                me->SetStandState(UNIT_STAND_STATE_STAND);
            }
        }
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (me->HealthBelowPctDamaged(30, damage) && !isInFalcon)
        {
            isInFalcon = true;
            me->SetDisplayId(39796); // faucon
            events.ScheduleEvent(EVENT_FALCON, 1000);
            events.CancelEvent(EVENT_JAOMIN_JUMP);
            events.CancelEvent(EVENT_HIT_CIRCLE);
        }

        if (me->HealthBelowPctDamaged(5, damage))
        {
            me->SetDisplayId(39755);

            std::list<Player*> playerList;
            GetPlayerListInGrid(playerList, me, 10.0f);
            for (auto&& player : playerList)
                player->KilledMonsterCredit(me->GetEntry(), ObjectGuid::Empty);

            Talk(1);
            EnterEvadeMode();
            me->SetFaction(35);
            me->SetFullHealth();
            me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
            events.Reset();
            events.ScheduleEvent(EVENT_RESET, 5000);
            damage = 0;
        }

        if (damage > me->GetHealth())
            damage = 0;
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (auto const eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_JAOMIN_JUMP:
                    if (me->GetVictim())
                        me->CastSpell(me->GetVictim(), 108938, true);
                    events.ScheduleEvent(EVENT_JAOMIN_JUMP, 30000);
                    break;
                case EVENT_HIT_CIRCLE:
                    if (me->GetVictim())
                        me->CastSpell(me->GetVictim(), 119301, true);
                    events.ScheduleEvent(EVENT_HIT_CIRCLE, 3000);
                    break;
                case EVENT_FALCON:
                    if (me->GetVictim())
                        me->CastSpell(me->GetVictim(), 108935, true);
                    events.ScheduleEvent(EVENT_FALCON, 4000);
                    break;
                case EVENT_RESET:
                    Reset();
                    break;
                case EVENT_CHECK_AREA:
                    if (me->GetAreaId() != 5843) // Grotte Paisible
                        Reset();
                    break;
            }
        }

        if (UpdateVictim())
            DoMeleeAttackIfReady();
    }
};

// NPC 56503 - Min Dimwind (Attacker) | Quest 29419
struct npc_attacker_dimwindAI : public ScriptedAI
{
    npc_attacker_dimwindAI(Creature* creature) : ScriptedAI(creature) { }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (me->GetHealthPct() < 90 && attacker && attacker->ToCreature() && attacker->ToCreature()->GetEntry() == 54785)
            damage = 0;
    }
};

// NPC 54785 - Min Dimwind | Quest 29419
struct npc_min_dimwindAI : public ScriptedAI
{
    npc_min_dimwindAI(Creature* creature) : ScriptedAI(creature), summons(creature) { }

    EventMap events;
    SummonList summons;

    enum eEvents
    {
        EVENT_RESET    = 1,
    };

    void Reset() override
    {
        ResetMobs();
        me->HandleEmoteCommand(EMOTE_STATE_READY2H);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (me->GetHealthPct() < 25 && attacker && attacker->ToCreature() && attacker->ToCreature()->GetEntry() == 54130)
            damage = 0;
    }

    void JustSummoned(Creature* summon) override
    {
        if (summon->GetEntry() == 54130)
        {
            summons.Summon(summon);

            summon->SetFacingToObject(me);
            summon->HandleEmoteCommand(EMOTE_STATE_READY2H);
        }
    }

    void ResetMobs()
    {
        events.CancelEvent(EVENT_RESET);
        me->HandleEmoteCommand(EMOTE_STATE_READY2H);

        summons.DespawnAll();

        for (int i = 0; i < 4; ++i)
            me->SummonCreature(54130, me->GetPositionX()-3+rand()%6, me->GetPositionY() + 4 + rand()%4, me->GetPositionZ()+2, 4.9f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000ms);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        auto const player = who->ToPlayer();
        if (!player || player->GetQuestStatus(29419) != QUEST_STATUS_INCOMPLETE)
            return;

        if (me->GetDistance(who) < 15.f)
        {
            me->HandleEmoteCommand(EMOTE_STATE_STAND);
            Talk(0);
            player->KilledMonsterCredit(54855);
            events.ScheduleEvent(EVENT_RESET, 6000);

            for (auto&& guid : summons)
                if (Creature* const creature = Unit::GetCreature(*me, guid))
                    creature->GetMotionMaster()->MoveFleeing(me, 5000);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);
        while (auto const eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_RESET:
                {
                    ResetMobs();
                }
            }
        }
    }
};

// NPC 56661 - Aysa Cloudsinger (Lake Escort) | Quest 29414 - The Way of the Tushui
struct npc_aysa_lake_escortAI : public EscortAI
{
    npc_aysa_lake_escortAI(Creature* creature) : EscortAI(creature) { }

    uint32 IntroTimer = 0;

    void Reset() override
    {
        me->SetReactState(REACT_PASSIVE);
        IntroTimer = 2500;
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        EscortAI::MovementInform(type, pointId);

        if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
            return;

        switch (pointId)
        {
            case 10:
                me->GetMotionMaster()->MoveJump(1227.11f, 3489.73f, 100.37f, 10, 20, 11);
                break;
            case 11:
                me->GetMotionMaster()->MoveJump(1236.68f, 3456.68f, 102.58f, 10, 20, 12);
                break;
            case 12:
                SetRun(true); Start(false);
                break;
            default:
                break;
        }
    }

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        if (waypointId == 4)
            me->DespawnOrUnsummon(500);
    }

    void UpdateAI(uint32 diff) override
    {
        if (IntroTimer)
        {
            if (IntroTimer <= diff)
            {
                Talk(0);
                IntroTimer = 0;
                me->GetMotionMaster()->MoveJump(1216.78f, 3499.44f, 91.15f, 10, 20, 10);
            }
            else
                IntroTimer -= diff;
        }

        EscortAI::UpdateAI(diff);
    }
};

// NPC 54567 - Aysa Cloudsinger | Quest 29414 - The Way of the Tushui
struct npc_aysaAI : public ScriptedAI
{
    enum
    {
        QUEST_WAY_OF_THE_TUSHUI = 29414,
        NPC_AYSA_LAKE_ESCORT    = 56661,
        NPC_MASTER_LI_FEI       = 54856,
        SPELL_MEDITATION_BAR    = 116421,
        NPC_TROUBLEMAKER        = 59637,

        SAY_INTRO               = 0,
        SAY_END
    };

    npc_aysaAI(Creature* creature) : ScriptedAI(creature)
    {
        if (me->GetAreaId() == 5848) // Cave of Meditation
            events.ScheduleEvent(EVENT_START, 600); // Begin script
        inCombat = false;
        timer = 0;
        lifeiGUID = ObjectGuid::Empty;
        me->SetReactState(REACT_DEFENSIVE);
        me->SetFaction(2263);
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_WAY_OF_THE_TUSHUI)
            if (Creature* tempSummon = me->SummonCreature(NPC_AYSA_LAKE_ESCORT, *me, TEMPSUMMON_MANUAL_DESPAWN, 0ms, player->GetGUID()))
                tempSummon->SetPhaseMask(1, true);
    }

    enum eEvents
    {
        EVENT_START      = 1,
        EVENT_SPAWN_MOBS = 2,
        EVENT_PROGRESS   = 3,
        EVENT_END        = 4,
    };

    EventMap events;
    std::vector<Player*> playersInvolved;
    ObjectGuid lifeiGUID;
    bool inCombat = false;
    uint32 timer = 0;

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (me->HealthBelowPctDamaged(5, damage))
        {
            DespawnLifei();
            damage = 0;
            me->SetFullHealth();
            me->SetReactState(REACT_DEFENSIVE);

            std::list<Creature*> unitlist;
            GetCreatureListWithEntryInGrid(unitlist, me, NPC_TROUBLEMAKER, 50.0f);
            for (auto&& creature : unitlist)
                me->Kill(creature);

            events.ScheduleEvent(EVENT_START, 20000);
            events.CancelEvent(EVENT_SPAWN_MOBS);
            events.CancelEvent(EVENT_PROGRESS);
            events.CancelEvent(EVENT_END);
        }
    }

    void UpdatePlayerList()
    {
        playersInvolved.clear();

        std::list<Player*> PlayerList;
        GetPlayerListInGrid(PlayerList, me, 20.0f);

        for (auto&& player : PlayerList)
            if (!player->IsGameMaster() && player->GetQuestStatus(QUEST_WAY_OF_THE_TUSHUI) == QUEST_STATUS_INCOMPLETE)
                playersInvolved.push_back(player);
    }

    Creature* GetLifei()
    {
        if (lifeiGUID != 0)
            return Creature::GetCreature(*me, lifeiGUID);
        else
        {
            TempSummon* temp = me->SummonCreature(NPC_MASTER_LI_FEI, 1130.162231f, 3435.905518f, 105.496597f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
            temp->GetMotionMaster()->Clear();
            temp->GetMotionMaster()->MoveRandom(10.0f);
            lifeiGUID = temp->GetGUID();
            return temp;
        }
    }

    void DespawnLifei()
    {
        if (lifeiGUID)
        {
            if (Creature* lifei = GetLifei())
            {
                lifei->DespawnOrUnsummon();
                lifeiGUID = ObjectGuid::Empty;
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);
        while (auto const eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_START: // Begin script if playersInvolved is not empty
                {
                    UpdatePlayerList();
                    if (playersInvolved.empty())
                        events.ScheduleEvent(EVENT_START, 600); // reschedule
                    else
                    {
                        Talk(SAY_INTRO);
                        me->SetReactState(REACT_PASSIVE);
                        timer = 0;
                        events.ScheduleEvent(EVENT_SPAWN_MOBS, 5000); // spawn mobs
                        events.ScheduleEvent(EVENT_PROGRESS, 1000); // update time
                        events.ScheduleEvent(EVENT_END, 90000); // end quest
                    }
                    break;
                }
                case EVENT_SPAWN_MOBS: // Spawn mobs
                {
                    UpdatePlayerList();

                    auto const maxSize = std::min<size_t>(playersInvolved.size(), 5);
                    auto const maxSpawns = std::max<size_t>(maxSize * 3, 3);

                    for (size_t i = 0; i < maxSpawns; ++i)
                        if (TempSummon* temp = me->SummonCreature(NPC_TROUBLEMAKER, 1155.625f, 3438.559f, 104.97f, 3.3f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000ms))
                        {
                            if (temp->AI())
                                temp->AI()->AttackStart(me);

                            temp->GetThreatManager().AddThreat(me, 250.0f);
                            temp->GetMotionMaster()->Clear();
                            temp->GetMotionMaster()->MoveChase(me);
                        }

                    events.ScheduleEvent(EVENT_SPAWN_MOBS, 20000);
                    break;
                }
                case EVENT_PROGRESS: // update energy
                {
                    timer++;

                    uint8 lafeiTalkThreshold[7] = { 25, 30, 42, 54, 66, 78, 85 };

                    for (int i = 0; i < 7; ++i)
                        if (timer == lafeiTalkThreshold[i])
                        {
                            Creature* lifei = GetLifei();
                            lifei->AI()->Talk(i);

                            if (i == 6)
                            {
                                lifei->DespawnOrUnsummon(500);
                                lifeiGUID = ObjectGuid::Empty;
                            }
                            break;
                        }

                    UpdatePlayerList();
                    for (auto&& player : playersInvolved)
                    {
                        if (!player->HasAura(SPELL_MEDITATION_BAR))
                            player->CastSpell(player, SPELL_MEDITATION_BAR);

                        player->SetPower(POWER_ALTERNATE_POWER, timer);
                        player->SetMaxPower(POWER_ALTERNATE_POWER, 90);
                    }

                    events.ScheduleEvent(EVENT_PROGRESS, 1000);
                    break;
                }
                case EVENT_END: // script end
                {
                    if (Creature* lifei = GetLifei())
                    {
                        lifei->DespawnOrUnsummon();
                        lifeiGUID = ObjectGuid::Empty;
                    }

                    events.ScheduleEvent(EVENT_START, 10000);
                    events.CancelEvent(EVENT_SPAWN_MOBS);
                    events.CancelEvent(EVENT_PROGRESS);
                    Talk(SAY_END);
                    me->SetReactState(REACT_DEFENSIVE);

                    UpdatePlayerList();
                    for (auto&& player : playersInvolved)
                    {
                        player->KilledMonsterCredit(NPC_MASTER_LI_FEI, ObjectGuid::Empty);
                        player->RemoveAura(SPELL_MEDITATION_BAR);
                    }
                    break;
                }
            }
        }
    }
};

// NPC 54856 - Master Li Fei | Quest 29421 - Only the Worthy Shall Pass
struct boss_li_feiAI : public ScriptedAI
{
    boss_li_feiAI(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == 29421)
        {
            if (Creature* tempSummon = me->SummonCreature(54856, me->GetPositionX() + 19, me->GetPositionY() + 4, me->GetPositionZ() - 1, me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0ms, player->GetGUID()))
            {
                tempSummon->SetPhaseMask(1024, true);
                tempSummon->AI()->SetGUID(player->GetGUID());
                tempSummon->AI()->Reset();
                tempSummon->AI()->AttackStart(player);
            }
        }
    }
};

// NPC 54734 - Master Li Fei (Fight) | Quest 29421 - Only the Worthy Shall Pass
struct boss_li_fei_fightAI : public ScriptedAI
{
    boss_li_fei_fightAI(Creature* creature) : ScriptedAI(creature) { }

    enum
    {
        EVENT_FEET_OF_FURY      = 1,
        EVENT_SHADOW_KICK       = 2,
        EVENT_SHADOW_KICK_STUN  = 3,

        QUEST_ONLY_THE_WORTHY_SHALL_PASS = 29421,
    };

    EventMap events;

    void Reset() override
    {
        me->SetFullHealth();

        events.Reset();
        events.ScheduleEvent(EVENT_FEET_OF_FURY, 5000);
        events.ScheduleEvent(EVENT_SHADOW_KICK,  1000);
    }

    // Li Fei does not aggro himself
    void MoveInLineOfSight(Unit* /*who*/) override { }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (!me->HealthBelowPctDamaged(10, damage))
            return;

        damage = 0;

        // copy, just to make sure that kill credit that phases players out
        // does not modify this threat list
        auto const threatList = me->GetThreatManager().GetUnsortedThreatList();
        for (auto&& hostileRef : threatList)
        {
            auto unit = hostileRef->GetVictim();
            if (unit && unit->GetTypeId() == TYPEID_PLAYER)
                unit->ToPlayer()->KilledMonsterCredit(54734, ObjectGuid::Empty);
        }

        // TODO: it seems that spell 106275 is used to display a text message
        // after fight is over

        me->GetThreatManager().RemoveMeFromThreatLists();
        me->GetThreatManager().ClearAllThreat();
        me->CombatStop(true);

        Reset();
    }

    void KilledUnit(Unit* victim) override
    {
        // Player managed to die with Blessings of Fire on
        if (auto const player = victim->ToPlayer())
            player->FailQuest(QUEST_ONLY_THE_WORTHY_SHALL_PASS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (auto const eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_FEET_OF_FURY:
                    if (auto const victim = me->GetVictim())
                        me->CastSpell(victim, 108958);
                    events.ScheduleEvent(EVENT_FEET_OF_FURY, 13000);
                    break;
                case EVENT_SHADOW_KICK:
                    if (auto const victim = me->GetVictim())
                        me->CastSpell(victim, 108936);
                    events.ScheduleEvent(EVENT_SHADOW_KICK_STUN, 2500);
                    events.ScheduleEvent(EVENT_SHADOW_KICK, 30000);
                    break;
                case EVENT_SHADOW_KICK_STUN:
                    if (auto const victim = me->GetVictim())
                        me->CastSpell(victim, 108944);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// NPC 54586/65470 - Huojin Trainee (Nim) | Quest 29409
struct npc_trainee_nimAI : public ScriptedAI
{
    npc_trainee_nimAI(Creature* creature) : ScriptedAI(creature) { }
    std::set<uint64> guids;

    void MoveInLineOfSight(Unit* who) override
    {
        auto const player = who->ToPlayer();
        if (!player)
            return;

        if (player->GetQuestStatus(29409) != QUEST_STATUS_INCOMPLETE)
            return;

        if (who->GetDistance(me) < 20.f)
        {
            if (guids.find(player->GetGUID()) == guids.end())
            {
                Talk(0, player);
                guids.insert(player->GetGUID());
            }
        }
    }
};

// NPC 54943 - Merchant Lorvo | Quest 29410
struct npc_merchant_lorvoAI : public ScriptedAI
{
    npc_merchant_lorvoAI(Creature* creature) : ScriptedAI(creature) { }
    std::set<uint64> guids;

    void MoveInLineOfSight(Unit* who) override
    {
        auto const player = who->ToPlayer();
        if (!player)
            return;

        if (player->GetQuestStatus(29410) == QUEST_STATUS_COMPLETE)
        {
            if (who->GetDistance(me) < 20.f)
            {
                if (guids.find(player->GetGUID()) == guids.end())
                {
                    Talk(0, player);
                    guids.insert(player->GetGUID());
                }
            }
        }
    }
};

// Spell 102630 - Huo Benediction (summons/despawns Huo 54958)
class spell_huo_benediction : public SpellScriptLoader
{
    public:
        spell_huo_benediction() : SpellScriptLoader("spell_huo_benediction") { }

        class spell_huo_benediction_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_huo_benediction_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();

                if (!target)
                    return;

                std::list<Creature*> huoList;
                GetCreatureListWithEntryInGrid(huoList, target, 54958, 20.0f);

                for (auto&& huo : huoList)
                    if (huo->ToTempSummon())
                        if (huo->ToTempSummon()->GetOwnerGUID() == target->GetGUID())
                            return;

                if (TempSummon* tempHuo = target->SummonCreature(54958, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0ms))
                {
                    tempHuo->SetExplicitSeerGuid(target->GetGUID());
                    tempHuo->SetOwnerGUID(target->GetGUID());
                    tempHuo->GetMotionMaster()->MoveFollow(target, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();

                if (!target)
                    return;

                std::list<Creature*> huoList;
                GetCreatureListWithEntryInGrid(huoList, target, 54958, 20.0f);

                for (auto&& huo : huoList)
                    if (huo->ToTempSummon())
                        if (huo->ToTempSummon()->GetOwnerGUID() == target->GetGUID())
                            huo->DespawnOrUnsummon();
            }

            void Register() override
            {
                OnEffectApply  += AuraEffectApplyFn(spell_huo_benediction_AuraScript::OnApply,  EFFECT_0, SPELL_AURA_MOD_INCREASE_SPEED, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectRemove += AuraEffectRemoveFn(spell_huo_benediction_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_INCREASE_SPEED, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_huo_benediction_AuraScript();
        }
};

// AT 29423 - Temple Entrance | Quest 29423
class AreaTrigger_at_temple_entrance : public AreaTriggerScript
{
    public:
        AreaTrigger_at_temple_entrance() : AreaTriggerScript("AreaTrigger_at_temple_entrance") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (player->GetQuestStatus(29423) == QUEST_STATUS_INCOMPLETE)
            {
                player->KilledMonsterCredit(61128, ObjectGuid::Empty);

                std::list<Creature*> huoList;
                GetCreatureListWithEntryInGrid(huoList, player, 54958, 20.0f);

                for (auto&& huo : huoList)
                {
                    if (huo->ToTempSummon())
                    {
                        if (huo->ToTempSummon()->GetOwnerGUID() == player->GetGUID())
                        {
                            huo->GetMotionMaster()->Clear();
                            huo->GetMotionMaster()->MovePoint(1, 950.0f, 3601.0f, 203.0f);
                            huo->DespawnOrUnsummon(5000);
                        }
                    }
                }
            }

            return true;
        }
};

// Spell 29664 - Lit Brazier of Flame | Quest 29664 - The Challenger's Fires
class spell_lit_brazier_of_flame : public SpellScript
{
    PrepareSpellScript(spell_lit_brazier_of_flame)

    SpellCastResult CheckCast()
    {
        auto const caster = GetCaster()->ToPlayer();
        if (!caster)
            return SPELL_FAILED_DONT_REPORT;
        switch (caster->GetQuestStatus(29664))
        {
            case QUEST_STATUS_NONE:
                return SPELL_FAILED_DONT_REPORT;
            case QUEST_STATUS_COMPLETE:
            case QUEST_STATUS_REWARDED:
                return SPELL_CAST_OK;
            default:
                // For spell_area casts only
                if (GetSpell()->IsTriggered())
                {
                    auto const killCredit = GetSpellInfo()->Effects[EFFECT_2].MiscValue;
                    if (caster->GetReqKillOrCastCurrentCount(29664, killCredit) != 0)
                        return SPELL_FAILED_DONT_REPORT;
                }
                return SPELL_CAST_OK;
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_lit_brazier_of_flame::CheckCast);
    }
};

// Spell 108958 - Feet of Fury | Quest 29421
class spell_feet_of_fury : public SpellScriptLoader
{
    public:
        spell_feet_of_fury() : SpellScriptLoader("spell_feet_of_fury") { }

        class spell_feet_of_fury_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_feet_of_fury_AuraScript)

            ObjectGuid targetGuid;

            bool Load() override
            {
                targetGuid = ObjectGuid::Empty;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                auto const caster = GetCaster();
                if (!caster)
                    return;

                if (auto const target = caster->GetVictim())
                    targetGuid = target->GetGUID();
            }

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();

                if (targetGuid == 0)
                    return;

                auto const caster = GetCaster();
                auto const target = ObjectAccessor::GetUnit(*caster, targetGuid);

                if (!target)
                {
                    targetGuid = ObjectGuid::Empty;
                    return;
                }

                auto const spellId = GetSpellInfo()->Effects[EFFECT_0].TriggerSpell;
                caster->CastSpell(target, spellId, true);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_feet_of_fury_AuraScript::OnApply,  EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_feet_of_fury_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_feet_of_fury_AuraScript;
        }
};

// Spell 114684 - Flame Spout | Quest 29420 - The Spirit's Guardian
class spell_flame_spout : public SpellScriptLoader
{
    public:
        spell_flame_spout() : SpellScriptLoader("spell_flame_spout") { }

        class spell_flame_spout_AuraScript : public AuraScript
        {
            public:
                PrepareAuraScript(spell_flame_spout_AuraScript);

                void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
                {
                    GetTarget()->RemoveAurasDueToSpell(114686); // Flame Spout Visual
                }

                void Register() override
                {
                    AfterEffectRemove += AuraEffectRemoveFn(spell_flame_spout_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_flame_spout_AuraScript;
        }
};

// Spell 106299 - Summon Living Air | Quest 29523 - Fanning the Flames
Position const LivingAirSummonPos = { 1224.9202f, 3727.2014f, 92.447205f, 0.0f };
class spell_gen_summon_living_air : public SpellScript
{
    PrepareSpellScript(spell_gen_summon_living_air)

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Creature* chickenTrigger = GetCaster()->FindNearestCreature(41200, 15.0f, true))
            chickenTrigger->SummonCreature(54631, LivingAirSummonPos, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 30000ms);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_gen_summon_living_air::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// NPC 57208 - Delivery Cart (Singing Pools) | Quest 29680
// NPC 59496 - Delivery Cart (Farmstead) | Quest 29775
// NPC 57740 - Delivery Cart (Forbidden Forest) | Quest 29800
struct npc_delivery_cart : public ScriptedAI
{
    struct CartData
    {
        uint32 Entry;
        uint32 PathId;
        int32 EjectNodeId;
        uint32 CreditNPC;
        uint32 QuestId;
    };

    static CartData const CartDataTable[];

    npc_delivery_cart(Creature* creature) : ScriptedAI(creature), _data(_GetCartData(creature->GetEntry())) { }

    static CartData _GetCartData(uint32 entry)
    {
        for (size_t i = 0; i < 3; ++i)
            if (CartDataTable[i].Entry == entry)
                return CartDataTable[i];
        return {};
    }

    void Reset() override
    {
        _events.Reset();
    }

    void PassengerBoarded(Unit* passenger, int8 /*seat*/, bool apply) override
    {
        if (!apply)
            return;

        auto const player = passenger->ToPlayer();
        if (!player)
            return;

        me->CastSpell(player, 46598); // Force Vehicle Ride

        _events.ScheduleEvent(1, 1800);
        _events.ScheduleEvent(2, 1000);

        if (_data.QuestId && player->hasQuest(_data.QuestId) && _data.CreditNPC)
            player->KilledMonsterCredit(_data.CreditNPC, player->GetGUID());
    }

    void WaypointReached(uint32 nodeId, uint32 /*pathId*/)
    {
        if (_data.EjectNodeId >= 0 && nodeId == static_cast<uint32>(_data.EjectNodeId))
            me->CastSpell(me, 50630); // Eject Passengers
    }

    void WaypointPathEnded(uint32 /*nodeId*/, uint32 /*pathId*/)
    {
        me->DespawnOrUnsummon(1000);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (auto const eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case 1: // Start path
                    me->GetMotionMaster()->MovePath(_data.PathId, false);
                    break;
                case 2: // Cart ropes visual
                    me->CastSpell(me, 108627); // OxCartRopeLeft
                    break;
                default:
                    break;
            }
        }
    }

private:
    EventMap _events;
    CartData _data;
};

npc_delivery_cart::CartData const npc_delivery_cart::CartDataTable[] =
{
    { 57208, 5720800, 28, 57710, 29680 }, // Singing Pools
    { 59496, 5949600, 28, 59497, 29775 }, // Farmstead
    { 57740, 5774000, 34, 57741, 29800 }, // Forbidden Forest
};

// NPC 54587/65471 - Tushui Trainee | Quest 29409
struct npc_tushui_traineeAI : public ScriptedAI
{
    npc_tushui_traineeAI(Creature* creature) : ScriptedAI(creature) { }

    bool isInCombat = false;
    uint32 punch1 = 1000;
    uint32 punch2 = 3500;
    uint32 punch3 = 6000;

    void Reset() override
    {
        punch1 = 1000;
        punch2 = 3500;
        punch3 = 6000;
        isInCombat = false;
        me->SetReactState(REACT_DEFENSIVE);
        me->SetFaction(7);
        me->SetFullHealth();
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (me->HealthBelowPctDamaged(16.67f, damage))
        {
            me->SetFaction(35);

            if (attacker && attacker->GetTypeId() == TYPEID_PLAYER)
                attacker->ToPlayer()->KilledMonsterCredit(54586, ObjectGuid::Empty);

            damage = 0;
            me->CombatStop();
            isInCombat = false;
            me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
            Talk(urand(0, 7));
            me->GetMotionMaster()->MovePoint(0, 1446.322876f, 3389.027588f, 173.782471f);
        }
    }

    void EnterCombat(Unit* /*unit*/) override
    {
        isInCombat = true;
    }

    void JustRespawned()
    {
        Reset();
    }

    void UpdateAI(uint32 diff) override
    {
        if (isInCombat)
        {
            DoMeleeAttackIfReady();
            return;
        }

        if (punch1 <= diff)
        {
            me->HandleEmoteCommand(35);
            punch1 = 7500;
        }
        else
            punch1 -= diff;

        if (punch2 <= diff)
        {
            me->HandleEmoteCommand(36);
            punch2 = 7500;
        }
        else
            punch2 -= diff;

        if (punch3 <= diff)
        {
            me->HandleEmoteCommand(37);
            punch3 = 7500;
        }
        else
            punch3 -= diff;

        if (me->GetPositionX() == 1446.322876f && me->GetPositionY() == 3389.027588f && me->GetPositionZ() == 173.782471f)
            me->ForcedDespawn(1000);
    }
};

// NPC 65470 - Huojin Trainee | Quest 29409
struct npc_huojin_traineeAI : public ScriptedAI
{
    npc_huojin_traineeAI(Creature* creature) : ScriptedAI(creature) { }

    uint32 punch = urand(500, 3000);
    bool isInCombat = false;

    void Reset() override
    {
        punch = urand(500, 3000);
        isInCombat = false;
        me->SetReactState(REACT_DEFENSIVE);
        me->SetFullHealth();
        me->SetFaction(7);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (me->HealthBelowPctDamaged(16.67f, damage))
        {
            damage = 0;
            if (attacker && attacker->GetTypeId() == TYPEID_PLAYER)
                attacker->ToPlayer()->KilledMonsterCredit(54586, ObjectGuid::Empty);
            me->CombatStop();
            me->SetFaction(35);
            isInCombat = false;
            me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
            Talk(urand(0, 7));
            me->GetMotionMaster()->MovePoint(0, 1446.322876f, 3389.027588f, 173.782471f);
        }
    }

    void EnterCombat(Unit* /*unit*/) override
    {
        isInCombat = true;
    }

    void JustRespawned()
    {
        Reset();
    }

    void UpdateAI(uint32 diff) override
    {
        if (isInCombat)
        {
            DoMeleeAttackIfReady();
            return;
        }

        if (punch <= diff)
        {
            me->HandleEmoteCommand(35);
            punch = urand(500, 3000);
        }
        else
            punch -= diff;

        if (me->GetPositionX() == 1446.322876f && me->GetPositionY() == 3389.027588f && me->GetPositionZ() == 173.782471f)
            me->ForcedDespawn(1000);
    }
};

// NPC 61411 - Instructor Zhi | Quest 29409
struct npc_instructorsAI : public ScriptedAI
{
    npc_instructorsAI(Creature* creature) : ScriptedAI(creature) { }

    uint32 punch1 = 300;
    uint32 punch2 = 2800;
    uint32 punch3 = 5300;

    void UpdateAI(uint32 diff) override
    {
        if (punch1 <= diff)
        {
            me->HandleEmoteCommand(35);
            punch1 = 7500;
        }
        else
            punch1 -= diff;

        if (punch2 <= diff)
        {
            me->HandleEmoteCommand(36);
            punch2 = 7500;
        }
        else
            punch2 -= diff;

        if (punch3 <= diff)
        {
            me->HandleEmoteCommand(37);
            punch3 = 7500;
        }
        else
            punch3 -= diff;
    }
};

// NPC 53565 - Aspiring Trainee | Quest 29409
struct npc_aspiring_traineeAI : public ScriptedAI
{
    npc_aspiring_traineeAI(Creature* creature) : ScriptedAI(creature) { }

    uint32 punch1 = 1000;
    uint32 punch2 = 3500;
    uint32 punch3 = 6000;

    void UpdateAI(uint32 diff) override
    {
        if (punch1 <= diff)
        {
            me->HandleEmoteCommand(35);
            punch1 = 7500;
        }
        else
            punch1 -= diff;

        if (punch2 <= diff)
        {
            me->HandleEmoteCommand(36);
            punch2 = 7500;
        }
        else
            punch2 -= diff;

        if (punch3 <= diff)
        {
            me->HandleEmoteCommand(37);
            punch3 = 7500;
        }
        else
            punch3 -= diff;
    }
};

// NPC 54568 - Ji Firepaw | Quest 29522
struct npc_ji_firepaw_talkAI : public ScriptedAI
{
    npc_ji_firepaw_talkAI(Creature* creature) : ScriptedAI(creature) { }

    bool hasSaid1 = false;

    void Reset() override
    {
        hasSaid1 = false;
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        std::list<Player*> playerList;
        GetPlayerListInGrid(playerList, me, 3.0f);
        for (Player* player : playerList)
        {
            if (!hasSaid1 && player->GetQuestStatus(29522) == QUEST_STATUS_COMPLETE)
            {
                Talk(0);
                hasSaid1 = true;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// NPC 60176 - Huojin Monk | Quest 29420
struct npc_huojin_monkAI : public ScriptedAI
{
    npc_huojin_monkAI(Creature* creature) : ScriptedAI(creature) { }

    bool hasSaid1 = false;

    void Reset() override
    {
        hasSaid1 = false;
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        std::list<Player*> playerList;
        GetPlayerListInGrid(playerList, me, 10.0f);
        for (Player* player : playerList)
        {
            if (!hasSaid1 && player->GetQuestStatus(29420) == QUEST_STATUS_COMPLETE)
            {
                Talk(0);
                hasSaid1 = true;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// NPC 60248 - Chia-hui Autumnleaf | Quest 29423
struct npc_chia_hui_autumnleafAI : public ScriptedAI
{
    npc_chia_hui_autumnleafAI(Creature* creature) : ScriptedAI(creature) { }

    bool hasSaid1 = false;

    void Reset() override
    {
        hasSaid1 = false;
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        std::list<Player*> playerList;
        GetPlayerListInGrid(playerList, me, 12.0f);
        for (Player* player : playerList)
        {
            if (!hasSaid1 && player->GetQuestStatus(29423) == QUEST_STATUS_INCOMPLETE)
            {
                Talk(0);
                hasSaid1 = true;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// NPC 60253 - Brewer Lin | Quest 29423
struct npc_brewer_linAI : public ScriptedAI
{
    npc_brewer_linAI(Creature* creature) : ScriptedAI(creature) { }

    bool hasSaid1 = false;

    void Reset() override
    {
        hasSaid1 = false;
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        std::list<Player*> playerList;
        GetPlayerListInGrid(playerList, me, 4.0f);
        for (Player* player : playerList)
        {
            if (!hasSaid1 && player->GetQuestStatus(29423) == QUEST_STATUS_INCOMPLETE)
            {
                Talk(0);
                hasSaid1 = true;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// NPC 54631 - Living Air | Quest 29523
struct boss_living_airAI : public ScriptedAI
{
    boss_living_airAI(Creature* creature) : ScriptedAI(creature)
    {
        me->SetReactState(REACT_AGGRESSIVE);
    }

    uint32 cast1Timer = 3000;
    uint32 cast2Timer = 5000;

    void EnterCombat(Unit* /*unit*/) override
    {
        cast1Timer = 3000;
        cast2Timer = 5000;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (cast1Timer <= diff)
        {
            me->CastSpell(me->GetVictim(), 108693);
            cast1Timer = 10000;
        }
        else
            cast1Timer -= diff;

        if (cast2Timer <= diff)
        {
            me->CastSpell(me->GetVictim(), 73212);
            cast2Timer = 5000;
        }
        else
            cast2Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

// AreaTrigger 7835 - Temple of Five Dawns
class AreaTrigger_at_temple_of_five_dawns : public AreaTriggerScript
{
public:
    AreaTrigger_at_temple_of_five_dawns() : AreaTriggerScript("AreaTrigger_at_temple_of_five_dawns") { }

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
    {
        if (player->GetQuestStatus(29423) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* huo = player->FindNearestCreature(54958, 15.0f))
            {
                if (Creature* master = player->FindNearestCreature(54786, 25.0f))
                {
                    master->Say("Welcome, Huo. The people have missed your warmth.", LANG_UNIVERSAL, player);

                    if (Creature* aysa = master->SummonCreature(61126, 968.2688f, 3602.207f, 196.6442f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN))
                        aysa->GetMotionMaster()->MovePoint(0, 966.3715f, 3602.764f, 196.47968f);

                    if (Creature* ji = master->SummonCreature(61127, 971.5566f, 3607.8015f, 195.71495f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN))
                        ji->GetMotionMaster()->MovePoint(0, 966.1493f, 3607.0894f, 196.51373f);

                    player->KilledMonsterCredit(61128, ObjectGuid::Empty);
                    huo->GetMotionMaster()->MovePoint(0, 955.11584f, 3604.04f, 200.71805f);
                    huo->DeleteCharmInfo();
                    huo->DespawnOrUnsummon(60000);
                    return true;
                }
            }
        }
        return false;
    }
};

// NPC 54787/57779/60917 - Huo
struct npc_huoAI : public ScriptedAI
{
    npc_huoAI(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* /*quest*/) override
    {
        player->CastSpell(player, 102630); // blessing of huo
        player->CastSpell(player, 128700);
    }
};

// Spell 102522 - Huo's Offerings
class spell_item_huo_offering : public SpellScriptLoader
{
public:
    spell_item_huo_offering() : SpellScriptLoader("spell_item_huo_offering") { }

    class spell_item_huo_offering_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_huo_offering_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            Unit* target = GetHitUnit();

            if (!caster || !target || target->GetEntry() != 54787)
                return;

            if (Player* player = caster->ToPlayer())
                player->KilledMonsterCredit(54787, target->GetGUID());
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_item_huo_offering_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_item_huo_offering_SpellScript();
    }
};

// NPC 54856 - Master Li Fei (Cave of Meditation) | Quest 29414
struct npc_aysa_meditationAI : public ScriptedAI
{
    npc_aysa_meditationAI(Creature* creature) : ScriptedAI(creature) { }

    uint32 power = 0;
    bool started = false;
    uint32 powerTimer = 1000;
    uint32 addsTimer = 60000;
    std::vector<Player*> playersParticipate;

    void Reset() override
    {
        power = 0;
        started = false;
        powerTimer = 1000;
        addsTimer = 60000;
    }

    void UpdatePlayerList()
    {
        playersParticipate.clear();

        std::list<Player*> playerList;
        GetPlayerListInGrid(playerList, me, 20.0f);
        for (Player* player : playerList)
            if (!player->IsGameMaster() && player->GetQuestStatus(29414) == QUEST_STATUS_INCOMPLETE)
                playersParticipate.push_back(player);
    }

    void UpdateAI(uint32 diff) override
    {
        if (playersParticipate.empty())
        {
            UpdatePlayerList();
            return;
        }

        if (!started)
        {
            for (Player* player : playersParticipate)
            {
                if (player->GetQuestStatus(29414) == QUEST_STATUS_INCOMPLETE && me->GetAreaId() == 5848)
                {
                    started = true;
                    powerTimer = 1000;
                    addsTimer = 60000;
                    break;
                }
            }
        }

        if (!started)
            return;

        if (powerTimer <= diff)
        {
            power++;

            if (power == 1)
            {
                Talk(0);
                me->CastSpell(me, 102445); // Summon Li Fei
            }

            for (Player* player : playersParticipate)
            {
                player->SetPower(POWER_ALTERNATE_POWER, power);
                player->SetMaxPower(POWER_ALTERNATE_POWER, 90);
            }

            if (power >= 90)
            {
                Talk(1);
                for (Player* player : playersParticipate)
                {
                    player->CastSpell(player, 102396);
                    player->KilledMonsterCredit(54856, ObjectGuid::Empty);
                    player->RemoveAura(116421);
                }
                Reset();
                return;
            }

            powerTimer = 1000;
        }
        else
            powerTimer -= diff;

        if (addsTimer <= diff)
        {
            static const Position addSpawnPos[2] =
            {
                { 1184.7f, 3448.3f, 102.5f, 0.0f },
                { 1186.7f, 3439.8f, 102.5f, 0.0f },
            };
            static const Position addPointPos[2] =
            {
                { 1145.13f, 3432.88f, 105.268f, 0.0f },
                { 1143.36f, 3437.39f, 104.973f, 0.0f },
            };

            for (uint8 i = 0; i < 2; ++i)
            {
                if (Creature* troublemaker = me->SummonCreature(61801, addSpawnPos[i].GetPositionX(), addSpawnPos[i].GetPositionY(), addSpawnPos[i].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN))
                    troublemaker->GetMotionMaster()->MovePoint(0, addPointPos[i].GetPositionX(), addPointPos[i].GetPositionY(), addPointPos[i].GetPositionZ());
            }
            addsTimer = 60000;
        }
        else
            addsTimer -= diff;
    }
};

void AddSC_wandering_island_north()
{
    RegisterCreatureAI(npc_master_shang_xiAI);
    RegisterCreatureAI(npc_training_target);
    RegisterCreatureAI(npc_jaomin_roAI);
    RegisterCreatureAI(boss_jaomin_roAI);
    RegisterCreatureAI(npc_tushui_traineeAI);
    RegisterCreatureAI(npc_huojin_traineeAI);
    RegisterCreatureAI(npc_instructorsAI);
    RegisterCreatureAI(npc_attacker_dimwindAI);
    RegisterCreatureAI(npc_min_dimwindAI);
    RegisterCreatureAI(npc_aysa_lake_escortAI);
    RegisterCreatureAI(npc_aysaAI);
    RegisterCreatureAI(boss_li_feiAI);
    RegisterCreatureAI(boss_li_fei_fightAI);
    RegisterCreatureAI(npc_trainee_nimAI);
    RegisterCreatureAI(npc_merchant_lorvoAI);
    RegisterCreatureAI(npc_aspiring_traineeAI);
    RegisterCreatureAI(npc_ji_firepaw_talkAI);
    RegisterCreatureAI(npc_huojin_monkAI);
    RegisterCreatureAI(npc_chia_hui_autumnleafAI);
    RegisterCreatureAI(npc_brewer_linAI);
    RegisterCreatureAI(boss_living_airAI);
    RegisterCreatureAI(npc_huoAI);
    RegisterCreatureAI(npc_aysa_meditationAI);
    new spell_huo_benediction();
    new AreaTrigger_at_temple_entrance();
    new AreaTrigger_at_temple_of_five_dawns();
    RegisterSpellScript(spell_lit_brazier_of_flame);
    new spell_feet_of_fury();
    new spell_flame_spout();
    RegisterSpellScript(spell_gen_summon_living_air);
    new spell_item_huo_offering();
    RegisterCreatureAI(npc_delivery_cart);
}
