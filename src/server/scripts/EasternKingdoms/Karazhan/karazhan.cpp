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

/* ScriptData
SDName: Karazhan
SD%Complete: 100
SDComment: Support for Barnes (Opera controller) and Berthold (Doorman), Support for Quest 9645.
SDCategory: Karazhan
EndScriptData */

/* ContentData
npc_barnes
npc_berthold
npc_image_of_medivh
EndContentData */

#include "ScriptPCH.h"
#include "karazhan.h"
#include "ScriptedEscortAI.h"

enum Spells
{
    SPELL_SPOTLIGHT     = 25824,
    SPELL_TUXEDO        = 32616,
};

enum Creatures
{
    NPC_SPOTLIGHT       = 19525
};

/*######
# npc_barnesAI
######*/

#define GOSSIP_READY        14052

#define BROADCAST_READY     14056
#define BROADCAST_OZ_INTRO1 14051
#define BROADCAST_OZ_INTRO2 14053

#define OZ_GOSSIP1          7421
#define OZ_GOSSIP2          7422

#define BROADCAST_RAJ_INTRO1 14119
#define RAJ_GOSSIP1         14121

#define OZ_GM_GOSSIP1       "[GM] Change event to EVENT_OZ"
#define OZ_GM_GOSSIP2       "[GM] Change event to EVENT_HOOD"
#define OZ_GM_GOSSIP3       "[GM] Change event to EVENT_RAJ"

struct Dialogue
{
    int32 textid;
    uint32 timer;
};

static Dialogue OzDialogue[]=
{
    {  0, 6000  },
    {  1, 18000 },
    {  2, 9000  },
    {  3, 15000 }
};

static Dialogue HoodDialogue[]=
{
    {  4, 6000  },
    {  5, 10000 },
    {  6, 14000 },
    {  7, 15000 }
};

static Dialogue RAJDialogue[]=
{
    {  8, 5000  },
    {  9, 7000  },
    { 10, 14000 },
    { 11, 14000 }
};

// Entries and spawn locations for creatures in Oz event
float Spawns[6][2]=
{
    { 17535, -10896 }, // Dorothee
    { 17546, -10891 }, // Roar
    { 17547, -10884 }, // Tinhead
    { 17543, -10902 }, // Strawman
    { 17603, -10892 }, // Grandmother
    { 17534, -10900 }, // julianne
};

#define SPAWN_Z             90.5f
#define SPAWN_Y             -1758
#define SPAWN_O             4.738f

class npc_barnes : public CreatureScript
{
    public:
        npc_barnes() : CreatureScript("npc_barnes") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (InstanceScript* instance = creature->GetInstanceScript())
            {
                // Check for death of Moroes and if opera event is not done already
                if (instance->GetData(TYPE_MOROES) == DONE && instance->GetData(TYPE_OPERA) == NOT_STARTED)
                {
                    player->ADD_GOSSIP_ITEM_DB(OZ_GOSSIP1, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

                    if (player->IsGameMaster())
                    {
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, OZ_GM_GOSSIP1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, OZ_GM_GOSSIP2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, OZ_GM_GOSSIP3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                    }

                    if (npc_barnesAI* barnesAI = CAST_AI(npc_barnes::npc_barnesAI, creature->AI()))
                    {
                        if (!barnesAI->RaidWiped)
                            player->SEND_GOSSIP_MENU(8970, creature->GetGUID());
                        else
                            player->SEND_GOSSIP_MENU(8975, creature->GetGUID());

                        return true;
                    }
                }
            }

            player->SEND_GOSSIP_MENU(8978, creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            npc_barnesAI* barnesAI = CAST_AI(npc_barnes::npc_barnesAI, creature->AI());

            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF + 1:
                    player->ADD_GOSSIP_ITEM_DB(OZ_GOSSIP2, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                    player->SEND_GOSSIP_MENU(8971, creature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF + 2:
                    player->CLOSE_GOSSIP_MENU();
                    barnesAI->StartEvent();
                    break;
                case GOSSIP_ACTION_INFO_DEF + 3:
                    player->CLOSE_GOSSIP_MENU();
                    barnesAI->m_uiEventId = EVENT_OZ;
                    TC_LOG_DEBUG("scripts", "TSCR: player (%s) manually set Opera event to EVENT_OZ", player->GetGUID().ToString().c_str());
                    break;
                case GOSSIP_ACTION_INFO_DEF + 4:
                    player->CLOSE_GOSSIP_MENU();
                    barnesAI->m_uiEventId = EVENT_HOOD;
                    TC_LOG_DEBUG("scripts", "TSCR: player (%s) manually set Opera event to EVENT_HOOD", player->GetGUID().ToString().c_str());
                    break;
                case GOSSIP_ACTION_INFO_DEF + 5:
                    player->CLOSE_GOSSIP_MENU();
                    barnesAI->m_uiEventId = EVENT_RAJ;
                    TC_LOG_DEBUG("scripts", "TSCR: player (%s) manually set Opera event to EVENT_RAJ", player->GetGUID().ToString().c_str());
                    break;
            }

            return true;
        }

        struct npc_barnesAI : public npc_escortAI
        {
            npc_barnesAI(Creature* creature) : npc_escortAI(creature)
            {
                RaidWiped = false;
                m_uiEventId = 0;
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            ObjectGuid m_uiSpotlightGUID;

            uint32 TalkCount;
            uint32 TalkTimer;
            uint32 WipeTimer;
            uint32 m_uiEventId;

            bool PerformanceReady;
            bool RaidWiped;

            void Reset() override
            {
                m_uiSpotlightGUID = ObjectGuid::Empty;

                TalkCount = 0;
                TalkTimer = 2000;
                WipeTimer = 5000;

                PerformanceReady = false;

                if (instance)
                    m_uiEventId = instance->GetData(DATA_OPERA_PERFORMANCE);
            }

            void StartEvent()
            {
                if (!instance)
                    return;

                instance->SetData(TYPE_OPERA, IN_PROGRESS);

                // resets count for this event, in case earlier failed
                if (m_uiEventId == EVENT_OZ)
                    instance->SetData(DATA_OPERA_OZ_DEATHCOUNT, IN_PROGRESS);

                Start(false, false);
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void WaypointReached(uint32 i) override
            {
                if (!instance)
                    return;

                switch (i)
                {
                    case 0:
                        DoCast(me, SPELL_TUXEDO, false);
                        instance->DoUseDoorOrButton(instance->GetGuidData(DATA_GO_STAGEDOORLEFT));
                        break;
                    case 4:
                        TalkCount = 0;
                        SetEscortPaused(true);

                        if (Creature* spotlight = me->SummonCreature(NPC_SPOTLIGHT, me->GetPosition(), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000ms))
                        {
                            spotlight->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            spotlight->CastSpell(spotlight, SPELL_SPOTLIGHT, false);
                            m_uiSpotlightGUID = spotlight->GetGUID();
                        }
                        break;
                    case 8:
                        instance->DoUseDoorOrButton(instance->GetGuidData(DATA_GO_STAGEDOORLEFT));
                        PerformanceReady = true;
                        break;
                    case 9:
                        PrepareEncounter();
                        instance->DoUseDoorOrButton(instance->GetGuidData(DATA_GO_CURTAINS));
                        break;
                }
            }

            void Talk(uint32 count)
            {
                uint32 text = 0;

                switch (m_uiEventId)
                {
                    case EVENT_OZ:
                        if (OzDialogue[count].textid)
                             text = OzDialogue[count].textid;
                        if (OzDialogue[count].timer)
                            TalkTimer = OzDialogue[count].timer;
                        break;
                    case EVENT_HOOD:
                        if (HoodDialogue[count].textid)
                            text = HoodDialogue[count].textid;
                        if (HoodDialogue[count].timer)
                            TalkTimer = HoodDialogue[count].timer;
                        break;
                    case EVENT_RAJ:
                         if (RAJDialogue[count].textid)
                             text = RAJDialogue[count].textid;
                        if (RAJDialogue[count].timer)
                            TalkTimer = RAJDialogue[count].timer;
                        break;
                }

                if (text)
                    CreatureAI::Talk(text);
            }

            void PrepareEncounter()
            {
                TC_LOG_DEBUG("scripts", "TSCR: Barnes Opera Event - Introduction complete - preparing encounter %d", m_uiEventId);
                uint8 index = 0;
                uint8 count = 0;

                switch (m_uiEventId)
                {
                    case EVENT_OZ:
                        index = 0;
                        count = 4;
                        break;
                    case EVENT_HOOD:
                        index = 4;
                        count = index+1;
                        break;
                    case EVENT_RAJ:
                        index = 5;
                        count = index+1;
                        break;
                }

                for (; index < count; ++index)
                {
                    uint32 entry = ((uint32)Spawns[index][0]);
                    float PosX = Spawns[index][1];

                    if (Creature* creature = me->SummonCreature(entry, PosX, SPAWN_Y, SPAWN_Z, SPAWN_O, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, Milliseconds(HOUR*2*IN_MILLISECONDS)))
                    {
                        // In case database has bad flags
                        creature->SetUInt32Value(UNIT_FIELD_FLAGS, 0);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }
                }

                RaidWiped = false;
            }

            void UpdateAI(uint32 diff) override
            {
                npc_escortAI::UpdateAI(diff);

                if (HasEscortState(STATE_ESCORT_PAUSED))
                {
                    if (TalkTimer <= diff)
                    {
                        if (TalkCount > 3)
                        {
                            if (Creature* spotlight = Unit::GetCreature(*me, m_uiSpotlightGUID))
                                spotlight->DespawnOrUnsummon();

                            SetEscortPaused(false);
                            return;
                        }

                        Talk(TalkCount);
                        ++TalkCount;
                    } else TalkTimer -= diff;
                }

                if (PerformanceReady)
                {
                    if (!RaidWiped)
                    {
                        if (WipeTimer <= diff)
                        {
                            Map* map = me->GetMap();
                            if (!map->IsDungeon())
                                return;

                            Map::PlayerList const& playerList = map->GetPlayers();
                            if (playerList.isEmpty())
                                return;

                            RaidWiped = true;
                            for (auto&& itr : playerList)
                            {
                                if (itr.GetSource()->IsAlive() && !itr.GetSource()->IsGameMaster())
                                {
                                    RaidWiped = false;
                                    break;
                                }
                            }

                            if (RaidWiped)
                            {
                                RaidWiped = true;
                                EnterEvadeMode();
                                return;
                            }

                            WipeTimer = 15000;
                        } else WipeTimer -= diff;
                    }

                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_barnesAI(creature);
        }
};

/*###
# npc_berthold
####*/

enum eBerthold
{
    SPELL_TELEPORT           = 39567
};

#define GOSSIP_ITEM_TELEPORT    7139

class npc_berthold : public CreatureScript
{
    public:
        npc_berthold() : CreatureScript("npc_berthold") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (InstanceScript* instance = creature->GetInstanceScript())
            {
                player->ADD_GOSSIP_ITEM_DB(GOSSIP_ITEM_TELEPORT, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->ADD_GOSSIP_ITEM_DB(GOSSIP_ITEM_TELEPORT, 1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->ADD_GOSSIP_ITEM_DB(GOSSIP_ITEM_TELEPORT, 2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                // Check if Shade of Aran event is done
                if (instance->GetData(TYPE_ARAN) == DONE)
                {
                    player->ADD_GOSSIP_ITEM_DB(GOSSIP_ITEM_TELEPORT, 3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                    player->SEND_GOSSIP_MENU(10741, creature->GetGUID());
                    return true;
                }
            }

            player->SEND_GOSSIP_MENU(8401, creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF + 1:
                    player->SEND_GOSSIP_MENU(10113, creature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF + 2:
                    player->SEND_GOSSIP_MENU(10114, creature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF + 3:
                    player->SEND_GOSSIP_MENU(10114, creature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF + 4:
                    player->CLOSE_GOSSIP_MENU();
                    player->CastSpell(player, SPELL_TELEPORT, true);
                    break;
            }
            return true;
        }
};


/*###
# npc_image_of_medivh
####*/

enum
{
    BROADCAST_DIALOG_MEDIVH_1       = 0,
    BROADCAST_DIALOG_MEDIVH_2       = 1,
    BROADCAST_DIALOG_MEDIVH_3       = 2,
    BROADCAST_DIALOG_MEDIVH_4       = 3,
    BROADCAST_DIALOG_MEDIVH_5       = 4,
    BROADCAST_DIALOG_ARCANAGOS_1    = 0,
    BROADCAST_DIALOG_ARCANAGOS_2    = 1,
    BROADCAST_DIALOG_ARCANAGOS_3    = 2,
    BROADCAST_DIALOG_ARCANAGOS_4    = 3,

    NPC_ARCANAGOS                   = 17652,
    SPELL_FIRE_BALL                 = 30967,
    SPELL_UBER_FIREBALL             = 30971,
    SPELL_CONFLAGRATION_BLAST       = 30977,
    SPELL_MANA_SHIELD               = 31635,
};

static float MedivPos[4]     = { -11161.49f, -1902.24f, 91.48f, 1.94f };
static float ArcanagosPos[4] = { -11169.75f, -1881.48f, 95.39f, 4.83f };

class npc_image_of_medivh : public CreatureScript
{
    public:
        npc_image_of_medivh() : CreatureScript("npc_image_of_medivh") { }

        struct npc_image_of_medivhAI : public ScriptedAI
        {
            npc_image_of_medivhAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            ObjectGuid ArcanagosGUID;

            uint32 YellTimer;
            uint32 Step;
            uint32 FireMedivhTimer;
            uint32 FireArcanagosTimer;

            bool EventStarted;

            void Reset() override
            {
                ArcanagosGUID = ObjectGuid::Empty;

                if (instance && instance->GetGuidData(DATA_IMAGE_OF_MEDIVH) == 0)
                {
                    instance->SetData64(DATA_IMAGE_OF_MEDIVH, me->GetGUID());
                    me->GetMotionMaster()->MovePoint(1, MedivPos[0], MedivPos[1], MedivPos[2]);
                    Step = 0;
                }
                else
                {
                    me->DealDamage(me,me->GetHealth(),nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
                    me->RemoveCorpse();
                }
            }
            void JustEngagedWith(Unit* /*who*/) override { }

            void MovementInform(uint32 type, uint32 id) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;
                if (id == 1)
                    StartEvent();
            }

            void StartEvent()
            {
                Step = 1;
                EventStarted = true;
                Creature* arcanagos = me->SummonCreature(NPC_ARCANAGOS, ArcanagosPos[0], ArcanagosPos[1], ArcanagosPos[2], 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000ms);
                if (!arcanagos)
                    return;
                ArcanagosGUID = arcanagos->GetGUID();
                arcanagos->SetFacingTo(ArcanagosPos[3]);
                me->SetFacingTo(MedivPos[3]);
                YellTimer = 10000;
            }

            uint32 NextStep(uint32 Step)
            {
                Creature* arca = Unit::GetCreature(*me, ArcanagosGUID);
                Map* map = me->GetMap();
                switch (Step)
                {
                    case 0: return 9999999;
                    case 1:
                        Talk(BROADCAST_DIALOG_MEDIVH_1);
                        return 10000;
                    case 2:
                        if (arca)
                            arca->AI()->Talk(BROADCAST_DIALOG_ARCANAGOS_1);
                        return 20000;
                    case 3:
                        Talk(BROADCAST_DIALOG_MEDIVH_2);
                        return 10000;
                    case 4:
                        if (arca)
                            arca->AI()->Talk(BROADCAST_DIALOG_ARCANAGOS_2);
                        return 20000;
                    case 5:
                        Talk(BROADCAST_DIALOG_MEDIVH_3);
                        return 20000;
                    case 6:
                        if (arca)
                            arca->AI()->Talk(BROADCAST_DIALOG_ARCANAGOS_3);
                        return 10000;
                    case 7:
                        FireArcanagosTimer = 500;
                        return 5000;
                    case 8:
                        FireMedivhTimer = 500;
                        DoCast(me, SPELL_MANA_SHIELD);
                        return 10000;
                    case 9:
                        Talk(BROADCAST_DIALOG_MEDIVH_4);
                        return 10000;
                    case 10:
                        if (arca)
                            DoCast(arca, SPELL_CONFLAGRATION_BLAST, false);
                        return 1000;
                    case 11:
                        if (arca)
                            arca->AI()->Talk(BROADCAST_DIALOG_ARCANAGOS_4);
                        return 5000;
                    case 12:
                        arca->GetMotionMaster()->MovePoint(0, -11010.82f,-1761.18f, 156.47f);
                        arca->setActive(true);
                        arca->InterruptNonMeleeSpells(true);
                        arca->SetSpeed(MOVE_RUN, 2.0f);
                        return 10000;
                    case 13:
                        Talk(BROADCAST_DIALOG_MEDIVH_5);
                        return 10000;
                    case 14:
                        me->SetVisible(false);
                        me->ClearInCombat();

                        if (map->IsDungeon())
                        {
                            for (auto&& itr : map->GetPlayers())
                            {
                                if (itr.GetSource()->IsAlive())
                                {
                                    if (itr.GetSource()->GetQuestStatus(9645) == QUEST_STATUS_INCOMPLETE)
                                        itr.GetSource()->CompleteQuest(9645);
                                }
                            }
                        }
                        return 50000;
                    case 15:
                        if (arca)
                            arca->DealDamage(arca, arca->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
                        return 5000;
                    default : return 9999999;
                }

            }

            void UpdateAI(uint32 diff) override
            {
                if (YellTimer <= diff)
                {
                    if (EventStarted)
                        YellTimer = NextStep(Step++);
                } else YellTimer -= diff;

                if (Step >= 7 && Step <= 12)
                {
                    Unit* arca = Unit::GetUnit(*me, ArcanagosGUID);

                    if (FireArcanagosTimer <= diff)
                    {
                        if (arca)
                            arca->CastSpell(me, SPELL_FIRE_BALL, false);
                        FireArcanagosTimer = 6000;
                    } else FireArcanagosTimer -= diff;

                    if (FireMedivhTimer <= diff)
                    {
                        if (arca)
                            DoCast(arca, SPELL_FIRE_BALL);
                        FireMedivhTimer = 5000;
                    } else FireMedivhTimer -= diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_image_of_medivhAI(creature);
        }
};

void AddSC_karazhan()
{
    new npc_barnes();
    new npc_berthold();
    new npc_image_of_medivh();
}
