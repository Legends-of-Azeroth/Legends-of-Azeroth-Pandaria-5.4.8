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

/* ScriptData
SDName: Trial Of the Champion
EndScriptData */

/* ContentData
npc_announcer_toc5
EndContentData */

#include "ScriptPCH.h"
#include "Group.h"
#include "trial_of_the_champion.h"

#define GOSSIP_START_EVENT1     "Я готов к старту испытания."
#define GOSSIP_START_EVENT2     "Я готов к следующему испытанию."

#define ORIENTATION             4.714f

Position const SpawnPosition = { 746.261f, 657.401f, 411.681f, 4.65f };

enum IntroPhase
{
    IDLE,
    INTRO,
    FINISHED
};

enum Creatures
{
    NPC_TRALL             = 34994,
    NPC_GARROSH           = 34995,
    NPC_KING              = 34990,
    NPC_LADY              = 34992,
    NPC_HIGHLORD          = 34996,
    NPC_ANNOUNCER         = 35004
};

class npc_anstart : public CreatureScript
{
    public:
        npc_anstart() : CreatureScript("npc_anstart") { }

        struct npc_anstartAI : public ScriptedAI
        {
            npc_anstartAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint32 introTimer;
            uint8 introPhase;

            IntroPhase Phase;

            Creature* Trall;
            Creature* Garrosh;
            Creature* King;
            Creature* Lady;
            Creature* Highlord;

            InstanceScript* instance;

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                Phase = IDLE;
                introTimer = 0;
                introPhase = 0;
                Trall = NULL;
                Garrosh = NULL;
                King = NULL;
                Lady = NULL;
                Highlord = NULL;
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!who)
                    return;

                if (Phase == IDLE && who->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(who, 20.0))
                {
                    Phase = INTRO;
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                    if (Trall = me->SummonCreature(NPC_TRALL, 685.569f, 615.103f, 435.396f, 6.23544f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000ms))
                    {
                        Trall->SetReactState(REACT_PASSIVE);
                        Trall->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }
                    if (Garrosh = me->SummonCreature(NPC_GARROSH, 685.7f, 621.134f, 435.396f, 6.259f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000ms))
                    {
                        Garrosh->SetReactState(REACT_PASSIVE);
                        Garrosh->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }
                    if (King = me->SummonCreature(NPC_KING, 807.724f, 617.9f, 435.396f, 3.18416f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000ms))
                    {
                        King->SetReactState(REACT_PASSIVE);
                        King->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }
                    if (Lady = me->SummonCreature(NPC_LADY, 807.401f, 613.667f, 435.397f, 3.0585f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000ms))
                    {
                        Lady->SetReactState(REACT_PASSIVE);
                        Lady->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }
                    if (Highlord = me->SummonCreature(NPC_HIGHLORD, 746.482f, 556.857f, 435.396f, 1.5898f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000ms))
                    {
                        Highlord->SetReactState(REACT_PASSIVE);
                        Highlord->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }
                }
            }

            void AttackStart(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {

                if (Phase != INTRO)
                    return;

                if (introTimer <= diff)
                {
                    if (!Trall || !Garrosh || !King || !Lady || !Highlord)
                        return;

                    switch (introPhase)
                    {
                        case 0:
                            ++introPhase;
                            introTimer = 4000;
                            break;
                        case 1:
                            me->Say("The Sunreavers are proud to present their representatives in this trial by combat.", LANG_UNIVERSAL, 0); // AN_1
                            ++introPhase;
                            introTimer = 10000;
                            break;
                        case 2:
                            me->Say("Welcome, champions. Today, before the eyes of your leeders and peers, you will prove youselves worthy combatants.", LANG_UNIVERSAL, 0); // AN_2
                            ++introPhase;
                            introTimer = 13000;
                            break;
                        case 3:
                            Trall->Yell("Fight well, Horde! Lok'tar Ogar!", LANG_UNIVERSAL, 0); // AN_3
                            ++introPhase;
                            introTimer = 8000;
                            break;
                        case 4:
                            Garrosh->Yell("Finally, a fight worth watching.", LANG_UNIVERSAL, 0); // AN_4
                            ++introPhase;
                            introTimer = 6000;
                            break;
                        case 5:
                            King->Yell("I did not come here to watch animals tear at each other senselessly, Tirion.", LANG_UNIVERSAL, 0); // AN_5
                            ++introPhase;
                            introTimer = 8000;
                            break;
                        case 6:
                            Highlord->Yell("You will first be facing three of the Grand Champions of the Tournament! These fierce contenders have beaten out all others to reach the pinnacle of skill in the joust.", LANG_UNIVERSAL, 0); // AN_6
                            ++introPhase;
                            introTimer = 8000;
                            break;
                        case 7:
                            Highlord->Yell("Will tought! You next challenge comes from the Crusade's own ranks. You will be tested against their consederable prowess.", LANG_UNIVERSAL, 0); // AN_7
                            ++introPhase;
                            introTimer = 3000;
                            break;
                        case 8:
                            me->Say("You may begin!", LANG_UNIVERSAL, 0); // AN_8
                            ++introPhase;
                            introTimer = 4000;
                            break;
                        case 9:
                            if (Creature* announcertoc5 = me->SummonCreature(NPC_ANNOUNCER, 746.778f, 621.514f, 411.09f, 4.75104f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000ms))
                            {
                                me->DisappearAndDie();
                                announcertoc5->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                announcertoc5->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                                announcertoc5->SetReactState(REACT_PASSIVE);

                                Phase = FINISHED;
                            }
                            else
                                Reset();
                            return;
                    }
                }
                else
                    introTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_anstartAI(creature);
        }
};

class npc_announcer_toc5 : public CreatureScript
{
    bool OnGossipHello(Player* player, Creature* creature) override
    {
        InstanceScript* instance = creature->GetInstanceScript();

        if (instance &&
            instance->GetData(BOSS_GRAND_CHAMPIONS) == DONE &&
            instance->GetData(BOSS_BLACK_KNIGHT) == DONE &&
            (instance->GetData(BOSS_ARGENT_CHALLENGE_E) == DONE ||
            instance->GetData(BOSS_ARGENT_CHALLENGE_P) == DONE))
            return false;

        if (player->GetGroup() && !player->GetGroup()->IsLeader(player->GetGUID()))
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Sorry, I'm not the group leader", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            return true;
        }

        if (instance &&
            instance->GetData(BOSS_GRAND_CHAMPIONS) == NOT_STARTED &&
            instance->GetData(BOSS_ARGENT_CHALLENGE_E) == NOT_STARTED &&
            instance->GetData(BOSS_ARGENT_CHALLENGE_P) == NOT_STARTED &&
            instance->GetData(BOSS_BLACK_KNIGHT) == NOT_STARTED)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        else if (instance)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->CLOSE_GOSSIP_MENU();
            CAST_AI(npc_announcer_toc5::npc_announcer_toc5AI, creature->AI())->StartEncounter();
        }

        return true;
    }

    public:
        npc_announcer_toc5() : CreatureScript("npc_announcer_toc5") { }

        struct npc_announcer_toc5AI : public ScriptedAI
        {
            npc_announcer_toc5AI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();

                summonTimes = 0;
                lesserChampions = 0;
                defeatedGrandChampions = 0;

                firstBoss = 0;
                secondBoss = 0;
                thirdBoss = 0;

                argentChampion = 0;

                phase = 0;
                timer = 0;

                Champion1List.clear();
                Champion2List.clear();
                Champion3List.clear();

                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                SetGrandChampionsForEncounter();
                SetArgentChampion();
            }

            InstanceScript* instance;

            uint8 summonTimes;
            uint8 lesserChampions;
            uint8 defeatedGrandChampions;

            uint32 argentChampion;
            uint32 firstBoss;
            uint32 secondBoss;
            uint32 thirdBoss;

            uint32 phase;
            uint32 timer;
            ObjectGuid uiBlackKnightGUID;

            std::list<ObjectGuid> Champion1List;
            std::list<ObjectGuid> Champion2List;
            std::list<ObjectGuid> Champion3List;

            void NextStep(uint32 timerStep, bool nextStep = true, uint8 phaseStep = 0)
            {
                timer = timerStep;
                if (nextStep)
                    ++phase;
                else
                    phase = phaseStep;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (!instance)
                    return;

                switch (type)
                {
                    case DATA_START:
                        if (GameObject* go = GameObject::GetGameObject(*me, instance->GetGuidData(DATA_MAIN_GATE)))
                            instance->HandleGameObject(go->GetGUID(), true);
                        if (GameObject* go = GameObject::GetGameObject(*me, instance->GetGuidData(DATA_PORTCULLIS)))
                            instance->HandleGameObject(go->GetGUID(), false);
                        instance->SetData(DATA_MOVEMENT_DONE, 0);

                        me->Say("Coming out of the gate Grand Champions other faction.", LANG_UNIVERSAL, 0); // SAY_START
                        DoSummonGrandChampion(firstBoss);
                        NextStep(10000, false, 1);
                        break;
                    case DATA_IN_POSITION: // movement done.
                        me->GetMotionMaster()->MovePoint(1, 735.81f, 661.92f, 412.39f);
                        if (GameObject* go = GameObject::GetGameObject(*me, instance->GetGuidData(DATA_MAIN_GATE)))
                            instance->HandleGameObject(go->GetGUID(), false);
                        NextStep(10000, false, 3);
                        break;
                    case DATA_LESSER_CHAMPIONS_DEFEATED:
                    {
                        ++lesserChampions;
                        std::list<ObjectGuid> tempList;
                        if (lesserChampions == 3 || lesserChampions == 6)
                        {
                            switch (lesserChampions)
                            {
                                case 3:
                                    tempList = Champion2List;
                                    break;
                                case 6:
                                    tempList = Champion3List;
                                    break;
                            }

                            for (auto&& guid : tempList)
                                if (Creature* summon = Unit::GetCreature(*me, guid))
                                    AggroAllPlayers(summon);
                        }
                        else if (lesserChampions == 9)
                            StartGrandChampionsAttack();

                        break;
                    }
                    case DATA_GRAND_CHAMPIONS_DEFEATED:
                        defeatedGrandChampions = data;
                        if (defeatedGrandChampions == 3)
                        {
                            for (uint8 i = 0; i < 3; ++i)
                                if (Creature* GrandChampion = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_GRAND_CHAMPION_1 + i)))
                                {
                                    switch (i)
                                    {
                                        case 0: GrandChampion->SetHomePosition(739.678f, 662.541f, 412.393f, 4.6f); break;
                                        case 1: GrandChampion->SetHomePosition(746.71f, 661.02f, 411.69f, 4.6f); break;
                                        case 2: GrandChampion->SetHomePosition(754.34f, 660.70f, 412.39f, 4.6f); break;
                                    }
                                    GrandChampion->AI()->SetData(10, 0);
                                }

                            instance->SetData(BOSS_GRAND_CHAMPIONS, IN_PROGRESS);
                        }
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_GRAND_CHAMPIONS_DEFEATED)
                    return defeatedGrandChampions;

                return 0;
            }

            void StartGrandChampionsAttack()
            {
                for (uint8 i = 0; i < 3; ++i)
                    if (Creature* GrandChampion = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_GRAND_CHAMPION_1 + i) : ObjectGuid::Empty))
                        AggroAllPlayers(GrandChampion);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 1)
                    me->SetFacingTo(ORIENTATION);
            }

            void DoSummonGrandChampion(uint32 bossId)
            {
                ++summonTimes;
                uint32 NPC_TO_SUMMON_1 = 0; // Grand Champion
                uint32 NPC_TO_SUMMON_2 = 0; // Faction Champions
                switch (bossId)
                {
                    case 0:
                        NPC_TO_SUMMON_1 = NPC_MOKRA;
                        NPC_TO_SUMMON_2 = NPC_ORGRIMMAR_CHAMPION;
                        break;
                    case 1:
                        NPC_TO_SUMMON_1 = NPC_ERESSEA;
                        NPC_TO_SUMMON_2 = NPC_SILVERMOON_CHAMPION;
                        break;
                    case 2:
                        NPC_TO_SUMMON_1 = NPC_RUNOK;
                        NPC_TO_SUMMON_2 = NPC_THUNDER_CHAMPION;
                        break;
                    case 3:
                        NPC_TO_SUMMON_1 = NPC_ZULTORE;
                        NPC_TO_SUMMON_2 = NPC_TROLL_CHAMPION;
                        break;
                    case 4:
                        NPC_TO_SUMMON_1 = NPC_VISCERI;
                        NPC_TO_SUMMON_2 = NPC_UNDERCITY_CHAMPION;
                        break;
                    default:
                        return;
                }

                if (Creature* boss = me->SummonCreature(NPC_TO_SUMMON_1, SpawnPosition))
                {
                    if (instance)
                    {
                        instance->SetData64(DATA_GRAND_CHAMPION_1 - 1 + summonTimes, boss->GetGUID());
                        instance->SetData(DATA_GRAND_CHAMPION_ENTRY, boss->GetEntry());
                    }

                    boss->AI()->SetData(summonTimes, 0);

                    for (uint8 i = 0; i < 3; ++i)
                    {
                        if (Creature* add = me->SummonCreature(NPC_TO_SUMMON_2, SpawnPosition, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000ms))
                        {
                            switch (summonTimes)
                            {
                                case 1:
                                    Champion1List.push_back(add->GetGUID());
                                    break;
                                case 2:
                                    Champion2List.push_back(add->GetGUID());
                                    break;
                                case 3:
                                    Champion3List.push_back(add->GetGUID());
                                    break;
                            }

                            switch (i)
                            {
                                case 0:
                                    add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI);
                                    break;
                                case 1:
                                    add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI / 2);
                                    break;
                                case 2:
                                    add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI / 2 + M_PI);
                                    break;
                            }
                        }
                    }
                }
            }

            void DoStartArgentChampionEncounter()
            {
                me->Say("Coming out of the gate Crusader's Coliseum Champion.", LANG_UNIVERSAL, 0); // SAY_START3
                me->GetMotionMaster()->MovePoint(1, 735.81f, 661.92f, 412.39f);

                if (me->SummonCreature(argentChampion, SpawnPosition))
                {
                    for (uint8 i = 0; i < 3; ++i)
                    {
                        if (Creature* trash = me->SummonCreature(NPC_ARGENT_LIGHWIELDER, SpawnPosition, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7000ms))
                            trash->AI()->SetData(i, 0);
                        if (Creature* trash = me->SummonCreature(NPC_ARGENT_MONK, SpawnPosition, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7000ms))
                            trash->AI()->SetData(i, 0);
                        if (Creature* trash = me->SummonCreature(NPC_PRIESTESS, SpawnPosition, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7000ms))
                            trash->AI()->SetData(i, 0);
                    }
                }
            }

            void SetGrandChampionsForEncounter()
            {
                firstBoss = urand(0, 4);

                while (secondBoss == firstBoss || thirdBoss == firstBoss || thirdBoss == secondBoss)
                {
                    secondBoss = urand(0, 4);
                    thirdBoss = urand(0, 4);
                }
            }

            void SetArgentChampion()
            {
                argentChampion = urand(0, 1) ? NPC_PALETRESS : NPC_EADRIC;
            }

            void StartEncounter()
            {
                if (!instance)
                    return;

                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                if (GameObject* go = GameObject::GetGameObject(*me, instance->GetGuidData(DATA_PORTCULLIS)))
                    instance->HandleGameObject(go->GetGUID(), false);

                if (instance->GetData(BOSS_BLACK_KNIGHT) == NOT_STARTED)
                {
                    if (instance->GetData(BOSS_ARGENT_CHALLENGE_E) == NOT_STARTED && instance->GetData(BOSS_ARGENT_CHALLENGE_P) == NOT_STARTED)
                    {
                        if (instance->GetData(BOSS_GRAND_CHAMPIONS) == NOT_STARTED)
                            SetData(DATA_START, 0);

                        if (instance->GetData(BOSS_GRAND_CHAMPIONS) == DONE)
                            DoStartArgentChampionEncounter();
                    }

                    if (instance->GetData(BOSS_GRAND_CHAMPIONS) == DONE && (instance->GetData(BOSS_ARGENT_CHALLENGE_E) == DONE ||
                        instance->GetData(BOSS_ARGENT_CHALLENGE_P) == DONE))
                    {
                        //me->SummonCreature(VEHICLE_BLACK_KNIGHT, 769.834f, 651.915f, 447.035f, 0);
                        if (Unit* pBlackKnight = me->SummonCreature(VEHICLE_BLACK_KNIGHT,801.369507f, 640.574280f, 469.314362f, 3.97124f,TEMPSUMMON_DEAD_DESPAWN,180000ms))
                        {
                            uiBlackKnightGUID = pBlackKnight->GetGUID();
                            pBlackKnight->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            pBlackKnight->SetTarget(me->GetGUID());
                            me->SetTarget(uiBlackKnightGUID);
                        }
                        if (GameObject* go = GameObject::GetGameObject(*me, instance->GetGuidData(DATA_MAIN_GATE)))
                            instance->HandleGameObject(go->GetGUID(), false);

                        me->Yell("What''s that, up near the rafters?", LANG_UNIVERSAL, 0); // SAY_START5
                    }
                }
            }

            void AggroAllPlayers(Creature* temp)
            {
                Map::PlayerList const& playerList = me->GetMap()->GetPlayers();

                if (playerList.isEmpty())
                    return;

                for (auto&& ref : playerList)
                {
                    if (Player* player = ref.GetSource())
                    {
                        if (player->IsGameMaster())
                            continue;

                        if (player->GetVehicle())
                        {
                            if (Creature* creature = player->GetVehicleBase()->ToCreature())
                            {
                                temp->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                                temp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                temp->SetReactState(REACT_AGGRESSIVE);
                                temp->SetInCombatWith(creature);
                                player->SetInCombatWith(temp);
                                creature->SetInCombatWith(temp);
                                temp->AddThreat(creature, 0.0f);
                            }
                        }
                        else if (player->IsAlive())
                        {
                            temp->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                            temp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            temp->SetReactState(REACT_AGGRESSIVE);
                            temp->SetInCombatWith(player);
                            player->SetInCombatWith(temp);
                            temp->AddThreat(player, 0.0f);
                        }
                    }
                }
            }

           void UpdateAI(uint32 diff) override
            {
                if (timer <= diff)
                {
                    switch (phase)
                    {
                        case 1:
                            DoSummonGrandChampion(secondBoss);
                            NextStep(10000, true);
                            break;
                        case 2:
                            DoSummonGrandChampion(thirdBoss);
                            NextStep(0, false);
                            break;
                        case 3:
                            if (!Champion1List.empty())
                            {
                                for (auto&& guid : Champion1List)
                                    if (Creature* summon = Unit::GetCreature(*me, guid))
                                        AggroAllPlayers(summon);
                                NextStep(0, false);
                            }
                            break;
                    }
                }
                else
                    timer -= diff;

                if (!UpdateVictim())
                    return;
            }

            void JustSummoned(Creature* summon) override
            {
                if (instance && instance->GetData(BOSS_GRAND_CHAMPIONS) == NOT_STARTED)
                {
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    summon->SetReactState(REACT_PASSIVE);
                }
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                switch (summon->GetEntry())
                {
                    case NPC_DARNASSUS_CHAMPION:
                    case NPC_EXODAR_CHAMPION:
                    case NPC_STORMWIND_CHAMPION:
                    case NPC_GNOMEREGAN_CHAMPION:
                    case NPC_IRONFORGE_CHAMPION:
                    case NPC_UNDERCITY_CHAMPION:
                    case NPC_THUNDER_CHAMPION:
                    case NPC_ORGRIMMAR_CHAMPION:
                    case NPC_SILVERMOON_CHAMPION:
                    case NPC_TROLL_CHAMPION:
                        SetData(DATA_LESSER_CHAMPIONS_DEFEATED, 0);
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_announcer_toc5AI(creature);
        }
};

void AddSC_trial_of_the_champion()
{
    new npc_anstart();
    new npc_announcer_toc5();
}
