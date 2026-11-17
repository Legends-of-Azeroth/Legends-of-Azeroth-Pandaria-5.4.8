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

#include "siege_of_niuzao_temple.h"
#include "MoveSplineInit.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Map.h"
#include "Vehicle.h"
#include "CreatureTextMgr.h"

/*

Patch 5.4.7 - Siege of Niuzao Temple: A Challenge Gong has been added, allowing players the option to trigger waves of attackers more quickly during the Commander Vo'jak encounter.

*/

enum
{
    NPC_LI_CHU              = 61812, // Left side
    NPC_LO_CHU              = 62794, // right side
    NPC_YANG_IRONCLAW       = 61620,
    NPC_SIKTHIK_WARDEN      = 62795,
    NPC_MANTID_TAR_KEG      = 61817,
    NPC_AMBERWING           = 61699,
    NPC_BREACHING_CHARGE    = 65168,

    // Adds
    ACTION_BOMBARDMENT      = 0,
    ACTION_ENGAGE_COMBAT    = 1, // DNC
    ACTION_RESET_COMBAT,
    ACTION_CALL_WAVE        = 10,
    WAVE_COUNT              = 4,
    LEFT_SIDE_MASK          = (1 << 10),

    POINT_INTRO             = 10,
    POINT_BOMB,

};

enum WaveCreatures
{
    NPC_SIKTHIK_SWARMER         = 63106,
    NPC_SIKTHIK_DEMOLISHER      = 61670,
    NPC_SIKTHIK_WARRIOR         = 61701,
    NPC_BARREL_TARGET           = 62684,
    NPC_INVISIBLE_STALKER       = 57478
};

enum
{
    SPELL_CARRYING_CAUSTIC_TAR      = 123032,
    SPELL_KEG_ACTIVE                = 123215, // keg sparkle visual
    SPELL_KEG_INACTIVE              = 123218,
    SPELL_MAKESHIFT_CHARGE          = 127408,
    SPELL_BREACH_DOOR_TARGET        = 127418,
    SPELL_BREACH_DOOR_EFFECT        = 127417,
};

enum Yells
{
    SAY_INTRO_1                 = 0,
    SAY_INTRO_2,
    SAY_INTRO_3,
    SAY_INTRO_4,
    SAY_AGGRO_1,
    SAY_AGGRO_2,
    SAY_AGGRO_3,
    SAY_AGGRO_4,
    SAY_AGGRO_5,
    EMOTE_WAVE_ONE,
    SAY_WAVE_ONE,
    EMOTE_WAVE_TWO,
    SAY_WAVE_TWO,
    EMOTE_WAVE_THREE,
    SAY_WAVE_THREE,
    EMOTE_WAVE_FOUR,
    EMOTE_BOMBARD,
    SAY_AMBERWING,
    SAY_OUTRO_1,
    SAY_OUTRO_2,
    SAY_GATE_1,
    SAY_GATE_2,
    SAY_GATE_3,
    SAY_GATE_4,

    SAY_VOJAK_AGGRO             = 0,
    SAY_VOJAK_WAVE_ONE,
    SAY_VOJAK_WAVE_TWO,
    SAY_VOJAK_WAVE_THREE,
    SAY_VOJAK_WAVE_FOUR,
    SAY_VOJAK_AIR_SUPPORT,
    SAY_VOJAK_ENGAGE_1,
    SAY_VOJAK_ENGAGE_2,
    SAY_VOJAK_AMBERWING,
    SAY_VOJAK_DEATH,
    SAY_VOJAK_SLAY,
};

struct SpawnData
{
    uint32 entry;
    Position spawnPos;
};

std::vector<SpawnData> LeftSpawns[WAVE_COUNT] =
{
    // row 1
    {
        { NPC_SIKTHIK_SWARMER, { 1492.31f, 5381.32f, 139.523f, 5.21757f } },
        { NPC_SIKTHIK_SWARMER, { 1497.94f, 5382.99f, 139.517f, 5.08107f } },
        { NPC_SIKTHIK_SWARMER, { 1503.62f, 5384.27f, 138.987f, 4.98993f } },
    },
    // row 2
    {
        { NPC_SIKTHIK_DEMOLISHER, { 1485.81f, 5387.15f, 139.537f, 5.13307f } },
        { NPC_SIKTHIK_DEMOLISHER, { 1490.2f, 5388.31f, 139.501f, 5.07617f } },
        { NPC_SIKTHIK_DEMOLISHER, { 1494.62f, 5389.79f, 139.373f, 5.21757f } },
        { NPC_SIKTHIK_DEMOLISHER, { 1499.56f, 5390.67f, 139.093f, 5.08107f } },
        { NPC_SIKTHIK_DEMOLISHER, { 1504.1f, 5391.9f, 138.702f, 4.98993f } }
    },
    // row 3
    {
        { NPC_SIKTHIK_SWARMER, { 1490.23f, 5395.73f, 140.125f, 5.1752f } },
        { NPC_SIKTHIK_SWARMER, { 1496.01f, 5397.63f, 140.189f, 5.0387f } },
        { NPC_SIKTHIK_SWARMER, { 1501.61f, 5398.63f, 139.612f, 4.94756f } },
    },
    // row 4
    {
        { NPC_SIKTHIK_DEMOLISHER, { 1486.66f, 5399.95f, 142.134f, 5.1069f } },
        { NPC_SIKTHIK_DEMOLISHER, { 1490.58f, 5401.87f, 142.353f, 5.0562f } },
        { NPC_SIKTHIK_DEMOLISHER, { 1495.18f, 5403.71f, 142.512f, 5.1752f } },
        { NPC_SIKTHIK_DEMOLISHER, { 1499.73f, 5404.56f, 141.805f, 5.0387f } },
        { NPC_SIKTHIK_DEMOLISHER, { 1504.15f, 5404.81f, 140.792f, 4.94756f } }
    }
};

std::vector<SpawnData> RightSpawns[WAVE_COUNT] =
{
    // row 1
    {
        { NPC_SIKTHIK_SWARMER, { 1518.57f, 5386.58f, 138.899f, 4.9688f } },
        { NPC_SIKTHIK_SWARMER, { 1524.63f, 5385.99f, 139.761f, 4.63027f } },
        { NPC_SIKTHIK_SWARMER, { 1529.82f, 5384.92f, 139.999f, 4.53913f } },
    },
    // row 2
    {
        { NPC_SIKTHIK_DEMOLISHER, {1517.42f, 5394.66f, 139.189f, 4.50366f} },
        { NPC_SIKTHIK_DEMOLISHER, {1522.11f, 5394.43f, 139.199f, 4.59403f} },
        { NPC_SIKTHIK_DEMOLISHER, {1526.45f, 5393.73f, 139.188f, 4.70598f} },
        { NPC_SIKTHIK_DEMOLISHER, {1531.14f, 5393.0f, 139.114f, 4.56948f} },
        { NPC_SIKTHIK_DEMOLISHER, {1534.96f, 5390.47f, 139.256f, 4.47834f} },
    },
    // row 3
    {
        { NPC_SIKTHIK_SWARMER, { 1519.44f, 5400.56f, 139.6f, 4.66401f } },
        { NPC_SIKTHIK_SWARMER, { 1534.09f, 5398.92f, 139.13f, 4.43637f } },
        { NPC_SIKTHIK_SWARMER, { 1526.4f, 5400.53f, 139.564f, 4.52751f } },
    },
    // row 4
    {
        { NPC_SIKTHIK_DEMOLISHER, { 1517.7f, 5406.8f, 140.215f, 4.67264f } },
        { NPC_SIKTHIK_DEMOLISHER, { 1522.76f, 5407.42f, 140.269f, 4.71994f } },
        { NPC_SIKTHIK_DEMOLISHER, { 1527.97f, 5406.99f, 140.135f, 4.66401f } },
        { NPC_SIKTHIK_DEMOLISHER, { 1533.31f, 5406.38f, 139.854f, 4.52751f } },
        { NPC_SIKTHIK_DEMOLISHER, { 1538.09f, 5404.21f, 139.43f, 4.43637f } },
    }
};

static const Position bombsPos[] = 
{
    { 1621.851f, 5411.984f, 138.73839f, 6.125f },
    { 1621.851f, 5408.082f, 138.74924f, 6.126f },
    { 1621.229f, 5404.804f, 138.73306f, 6.176f },
    { 1620.734f, 5400.718f, 138.73073f, 6.141f },
    { 1620.033f, 5397.146f, 139.07486f, 6.090f }
};

uint8 const loChuPathLenght = 26;
const Position loChuPath[loChuPathLenght] =
{
    { 1494.23f,  5287.28f, 184.718f, 0.0f },
    { 1483.31f,  5286.88f, 179.771f, 0.0f },
    { 1476.42f,  5292.71f, 179.706f, 0.0f },
    { 1474.88f,  5301.14f, 176.123f, 0.0f },
    { 1479.86f,  5321.44f, 176.123f, 0.0f },
    { 1550.88f,  5329.24f, 161.349f, 0.0f },
    { 1557.13f,  5332.74f, 161.849f, 0.0f },
    { 1562.88f,  5337.99f, 161.349f, 0.0f },
    { 1561.38f,  5346.24f, 161.349f, 0.0f },
    { 1557.13f, 5350.49f, 161.349f, 0.0f },
    { 1549.88f, 5351.74f, 161.349f, 0.0f },
    { 1537.63f, 5350.99f, 154.099f, 0.0f },
    { 1525.13f, 5349.99f, 146.599f, 0.0f },
    { 1520.63f, 5349.74f, 146.599f, 0.0f },
    { 1516.63f, 5351.74f, 146.599f, 0.0f },
    { 1513.38f, 5354.99f, 146.599f, 0.0f },
    { 1513.13f, 5359.24f, 146.599f, 0.0f },
    { 1511.88f, 5371.99f, 139.849f, 0.0f },
    { 1512.13f, 5381.74f, 139.349f, 0.0f },
    { 1520.88f, 5386.74f, 139.099f, 0.0f },
    { 1534.38f, 5388.99f, 139.849f, 0.0f },
    { 1549.38f, 5389.49f, 139.349f, 0.0f },
    { 1568.38f, 5390.99f, 140.099f, 0.0f },
    { 1585.38f, 5388.74f, 139.849f, 0.0f },
    { 1589.13f, 5387.99f, 139.849f, 0.0f },
    { 1591.88f, 5385.49f, 140.099f, 0.0f },
};

uint8 const liChuPathLenght = 30;
Position const liChuPath[liChuPathLenght] =
{
    { 1494.23f,  5287.28f, 184.718f, 0.0f },
    { 1483.31f,  5286.88f, 179.771f, 0.0f },
    { 1476.42f,  5292.71f, 179.706f, 0.0f },
    { 1474.88f,  5301.14f, 176.123f, 0.0f },
    { 1479.86f,  5321.44f, 176.123f, 0.0f },
    { 1521.96f,  5324.58f, 161.355f, 0.0f },
    { 1540.71f,  5326.33f, 161.355f, 0.0f },
    { 1550.96f,  5328.08f, 161.355f, 0.0f },
    { 1556.96f,  5330.08f, 161.855f, 0.0f },
    { 1561.46f, 5333.58f, 161.355f, 0.0f },
    { 1564.71f, 5337.58f, 161.355f, 0.0f },
    { 1565.21f, 5342.58f, 161.355f, 0.0f },
    { 1562.71f, 5347.58f, 161.355f, 0.0f },
    { 1558.96f, 5350.83f, 161.355f, 0.0f },
    { 1554.71f, 5352.83f, 161.355f, 0.0f },
    { 1550.21f, 5353.58f, 161.355f, 0.0f },
    { 1537.71f, 5352.83f, 154.105f, 0.0f },
    { 1525.21f, 5351.58f, 146.605f, 0.0f },
    { 1520.21f, 5351.33f, 146.605f, 0.0f },
    { 1516.96f, 5354.58f, 146.605f, 0.0f },
    { 1515.46f, 5358.83f, 146.605f, 0.0f },
    { 1514.46f, 5365.83f, 143.605f, 0.0f },
    { 1514.46f, 5371.83f, 139.855f, 0.0f },
    { 1517.71f, 5380.83f, 139.355f, 0.0f },
    { 1523.21f, 5384.08f, 139.855f, 0.0f },
    { 1535.71f, 5385.33f, 140.105f, 0.0f },
    { 1548.71f, 5385.83f, 139.605f, 0.0f },
    { 1564.46f, 5386.08f, 140.355f, 0.0f },
    { 1574.96f, 5384.83f, 140.605f, 0.0f },
    { 1582.21f, 5381.83f, 140.105f, 0.0f },
};

uint8 const yangIronclawPathLenght = 18;
Position const yangIronclawPath[yangIronclawPathLenght] =
{
    { 1494.23f, 5287.28f, 184.718f, 0.0f },
    { 1483.31f, 5286.88f, 179.771f, 0.0f },
    { 1476.42f, 5292.71f, 179.706f, 0.0f },
    { 1474.88f, 5301.14f, 176.123f, 0.0f },
    { 1479.86f, 5321.44f, 176.123f, 0.0f },
    { 1506.89f, 5326.1f,  161.209f, 0.0f },
    { 1556.49f, 5335.32f, 161.656f, 0.0f },
    { 1561.72f, 5345.77f, 161.21f,  0.0f },
    { 1549.24f, 5350.51f, 161.009f, 0.0f },
    { 1524.46f, 5348.87f, 146.23f,  0.0f },
    { 1512.02f, 5359.53f, 146.237f, 0.0f },
    { 1511.78f, 5372.53f, 139.081f, 0.0f },
    { 1518.06f, 5398.04f, 139.317f, 0.0f },
    { 1543.72f, 5404.46f, 139.159f, 0.0f },
    { 1565.09f, 5410.88f, 137.895f, 0.0f },
    { 1583.2f,  5415.66f, 138.417f, 0.0f },
    { 1602.04f, 5416.52f, 138.472f, 0.0f },
    { 1612.17f, 5415.12f, 138.691f, 0.0f },
};

/*
    Koz:
    This is a script for Yang Ironclaw
    It functions as controller NPC:
    - Instance State
    - NPC spawns / resets
    - Wave Generation / Timing
    - Exploit checks
    - Boss Summoning
*/

class npc_yang_ironclaw : public CreatureScript
{
    class CreatureTalkEvent final : public BasicEvent
    {
        public:
            explicit CreatureTalkEvent(Creature* speaker, uint32 textId) : _speaker(speaker), _textId(textId) { }

            bool Execute(uint64, uint32)
            {
                sCreatureTextMgr->SendChat(_speaker, _textId, 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_NORMAL, 0, TEAM_OTHER, false);
                return true;
            }

        private:
            Creature* _speaker;
            uint32 _textId;
    };

    enum 
    {
        EVENT_START_EVENT       = 1,
        EVENT_CALL_FIRST_WAVE,
        EVENT_CALL_SECOND_WAVE,
        EVENT_CALL_THIRD_WAVE,
        EVENT_CALL_FOURTH_WAVE,
        EVENT_CALL_BOSS,
        EVENT_PLAYER_CHECK,
        EVENT_INTRO_MOVE,
        EVENT_AMBERWING,
        EVENT_OUTRO_MOVE,
        EVENT_PLACE_BOMBS,
        EVENT_DETONATE_SPEECH,
        EVENT_DETONATE_BOMBS,

        PATH_OUTRO              = 1
    };

   //enum Quotes
   //{
   //    EMOTE_FIRST_WAVE,
   //    EMOTE_SECOND_WAVE,
   //    EMOTE_THIRD_WAVE,
   //    EMOTE_FOURTH_WAVE,
   //};

    enum Waves
    {
        WAVE_ONE,
        WAVE_TWO,
        WAVE_THREE,
        WAVE_FOUR,
        WAVE_BOSS
    };

    enum Misc
    {
        ACTION_START_EVENT      = 1,
    };

    public:
        npc_yang_ironclaw() : CreatureScript("npc_yang_ironclaw") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "We're ready to defend!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "We will wait", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

            // "Thanks for saving us! Are you ready to get this party started?"
            player->SEND_GOSSIP_MENU(2475, creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF + 1:
                {
                    if (creature->IsAIEnabled && !creature->GetVictim())
                        creature->GetAI()->DoAction(ACTION_START_EVENT);
                }
                    break;
                default:
                    break;
            }

            player->CLOSE_GOSSIP_MENU();

            return true;
        }

        struct npc_yang_ironclawAI : public ScriptedAI
        {
            npc_yang_ironclawAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                encounterInProgress = false;
                AmberWingGUID = ObjectGuid::Empty;
                me->SetFaction(1665);
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                ResetEncounter();
            }

            void Reset() override
            {
                me->SetFaction(35);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                if (instance->GetBossState(BOSS_VOJAK) == DONE)
                    me->DespawnOrUnsummon(500);
                else
                {
                    ResetEncounter();
                    events.Reset();
                }
            }

            void SummonWaveNPC(uint32 entry, const Position  &spawnPos, uint8 waveId, uint8 addNumber, bool isLeftSide)
            {
                // send everything in a bitmask (8 bit / value)
                uint32 mask = (waveId << 0);
                mask |= (addNumber << 8);
                mask |= (isLeftSide << 16);

                if (Creature* creature = me->SummonCreature(entry, spawnPos))
                    creature->AI()->SetData(0, mask);
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_BREACH_DOOR_TARGET) // fix crash when it did try despawn player...
                    if (target->ToCreature())
                    {
                        target->CastSpell(target, SPELL_BREACH_DOOR_EFFECT, true);
                        target->ToCreature()->DespawnOrUnsummon(300);
                    }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                damage = 0;
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_START_EVENT)
                    if (instance->GetBossState(BOSS_VOJAK) != DONE)
                        StartEncounter();
            }

            void ResetEncounter()
            {
                me->GetMotionMaster()->MoveTargetedHome();
                instance->SetBossState(BOSS_VOJAK, NOT_STARTED);
                encounterInProgress = false;
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                summons.DespawnAll();
                events.Reset();
                bombCnt = 0;

                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CARRYING_CAUSTIC_TAR);


                std::list<Creature*> cList;
                me->GetCreatureListWithEntryInGrid(cList, NPC_BARREL_TARGET, 500.0f);
                for (auto&& itr : cList)
                    itr->RemoveAllAuras();

                cList.clear();

                me->GetCreatureListWithEntryInGrid(cList, NPC_MANTID_TAR_KEG, 500.0f);
                for (auto&& itr : cList)
                {
                    itr->ExitVehicle();
                    itr->DespawnOrUnsummon(200);
                }

                if (Creature* loChu = me->FindNearestCreature(NPC_LO_CHU, 500.0f))
                    loChu->AI()->DoAction(ACTION_RESET_COMBAT);

                if (Creature* liChu = me->FindNearestCreature(NPC_LI_CHU, 500.0f))
                    liChu->AI()->DoAction(ACTION_RESET_COMBAT);

                // Summon waves

                uint32 spawnCounter = 0;

                me->GetMap()->LoadGrid(1509.67f, 5424.98f);

                for (int wave = 0; wave < WAVE_COUNT; ++wave)
                {
                    // left side spawns
                    spawnCounter = 0;
                    for (auto itr = LeftSpawns[wave].begin(); itr != LeftSpawns[wave].end(); ++itr, ++spawnCounter)
                        SummonWaveNPC(itr->entry, itr->spawnPos, wave, spawnCounter, true);

                    // right side spawns
                    spawnCounter = 0;
                    for (auto itr = RightSpawns[wave].begin(); itr != RightSpawns[wave].end(); ++itr, ++spawnCounter)
                        SummonWaveNPC(itr->entry, itr->spawnPos, wave, spawnCounter, false);

                }

                if (Creature* creature = me->SummonCreature(NPC_SIKTHIK_WARRIOR, 1498.52f, 5411.15f, 144.389f, 4.91132f))
                    creature->AI()->SetData(0, 3 | (1 << 16)); // join the fourth wave

                if (Creature* creature = me->SummonCreature(NPC_SIKTHIK_WARRIOR, 1523.97f, 5414.13f, 141.615f, 4.91132f))
                    creature->AI()->SetData(0, 2 | (1 << 16)); // join the third wave

                // boss spawn

                if (Creature* vojak = Creature::GetCreature(*me, vojakGUID))
                    vojak->DespawnOrUnsummon();

                if (Creature* creature = me->SummonCreature(NPC_VOJAK, 1509.67f, 5424.98f, 145.687f, 5.07204f))
                {
                    vojakGUID = creature->GetGUID();
                    creature->GetMotionMaster()->MoveRandom(5.0f);
                }
            }

            Creature* GetVojak() const
            {
                ASSERT(vojakGUID);

                if (Creature* vojak = Creature::GetCreature(*me, vojakGUID))
                    return vojak;

                return NULL;
            }

            void DelayedTalk(Creature* speaker, uint32 id, uint32 timer)
            {
                if (!speaker)
                    return;
                 // Should be safe to pass a pointer here
                CreatureTalkEvent * talkEvent = new CreatureTalkEvent(speaker, id);
                speaker->m_Events.AddEvent(talkEvent, speaker->m_Events.CalculateTime(timer));
            }

            void JustSummoned(Creature* summon) override
            {
                summon->setActive(true);
                if (summon->GetEntry() != NPC_VOJAK)
                    summons.Summon(summon);
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
            {
                if (summon->GetEntry() == NPC_VOJAK)
                {
                    instance->SetBossState(BOSS_VOJAK, DONE);
                    summons.DespawnAll();
                    events.Reset();

                    if (Creature* loChu = me->FindNearestCreature(NPC_LO_CHU, 500.0f))
                        loChu->AI()->DoAction(ACTION_RESET_COMBAT);

                    if (Creature* liChu = me->FindNearestCreature(NPC_LI_CHU, 500.0f))
                        liChu->AI()->DoAction(ACTION_RESET_COMBAT);

                    DelayedTalk(me, SAY_OUTRO_1, 2000);
                    DelayedTalk(me, SAY_OUTRO_2, 5000);
                    events.ScheduleEvent(EVENT_OUTRO_MOVE, 6000);

                    std::list<Creature*> cList;
                    me->GetCreatureListWithEntryInGrid(cList, NPC_MANTID_TAR_KEG, 500.0f);

                    for (auto&& itr : cList)
                    {
                        itr->SetRespawnDelay(DAY);
                        itr->DespawnOrUnsummon();
                    }
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE)
                {
                    if (pointId == POINT_INTRO)
                    {
                        Talk(SAY_AGGRO_3);
                        DelayedTalk(me, SAY_AGGRO_4, 3000);
                        DelayedTalk(me, SAY_AGGRO_5, 7000);
                    }
                    else if (pointId == POINT_BOMB)
                    {
                        if (bombCnt < 5)
                        {
                            DoCast(me, SPELL_MAKESHIFT_CHARGE, false);
                            events.ScheduleEvent(EVENT_PLACE_BOMBS, 1000);
                        }
                        else
                        {
                            DoCast(me, SPELL_MAKESHIFT_CHARGE, false);
                            events.ScheduleEvent(EVENT_DETONATE_SPEECH, 1000);
                        }
                    }
                }
            }

            void StartEncounter()
            {
                if (encounterInProgress)
                    return;

                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                encounterInProgress = true;
                instance->SetBossState(BOSS_VOJAK, IN_PROGRESS);
                events.ScheduleEvent(EVENT_CALL_FIRST_WAVE, 20000);
                events.ScheduleEvent(EVENT_CALL_SECOND_WAVE, 60000);
                events.ScheduleEvent(EVENT_CALL_THIRD_WAVE, 100000);
                events.ScheduleEvent(EVENT_CALL_FOURTH_WAVE, 140000);
                events.ScheduleEvent(EVENT_CALL_BOSS, 180000);
                events.ScheduleEvent(EVENT_PLAYER_CHECK, 5000);

                Talk(SAY_AGGRO_1);
                DelayedTalk(me, SAY_AGGRO_2, 3000);
                events.ScheduleEvent(EVENT_INTRO_MOVE, 5000);

                if (Creature* loChu = me->FindNearestCreature(NPC_LO_CHU, 500.0f))
                    loChu->AI()->DoAction(ACTION_ENGAGE_COMBAT);

                if (Creature* liChu = me->FindNearestCreature(NPC_LI_CHU, 500.0f))
                    liChu->AI()->DoAction(ACTION_ENGAGE_COMBAT);

                me->SetReactState(REACT_PASSIVE);
                me->SetFaction(35);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);
                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CALL_FIRST_WAVE: // Swarmers
                        {
                            Talk(EMOTE_WAVE_ONE);
                            DelayedTalk(GetVojak(), SAY_VOJAK_WAVE_ONE, 500);
                            DelayedTalk(me, SAY_WAVE_ONE, 6000);
                            EntryCheckPredicate pred(NPC_SIKTHIK_SWARMER);
                            summons.DoAction((ACTION_ENGAGE_COMBAT << WAVE_ONE), pred);

                            if (Creature* AmberWing = me->SummonCreature(NPC_AMBERWING, AmberWingSpawnPos.GetPositionX(), AmberWingSpawnPos.GetPositionY(), AmberWingSpawnPos.GetPositionZ(), AmberWingSpawnPos.GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                                AmberWingGUID = AmberWing->GetGUID();

                            DelayedTalk(me, EMOTE_BOMBARD, 4000);
                            DelayedTalk(me, SAY_AMBERWING, 4500);
                            break;
                        }
                        case EVENT_CALL_SECOND_WAVE: // Demolishers
                        {
                            Talk(EMOTE_WAVE_TWO);
                            DelayedTalk(GetVojak(), SAY_VOJAK_WAVE_TWO, 500);
                            DelayedTalk(me, SAY_WAVE_TWO, 5000);
                            EntryCheckPredicate pred(NPC_SIKTHIK_DEMOLISHER);
                            summons.DoAction((ACTION_ENGAGE_COMBAT << WAVE_TWO), pred);

                            if (Creature* AmberWing = ObjectAccessor::GetCreature(*me, AmberWingGUID))
                                if (AmberWing->IsAlive())
                                    AmberWing->AI()->DoAction(ACTION_BOMBARDMENT);

                            DelayedTalk(me, SAY_AMBERWING, 4500);
                            break;
                        }
                        case EVENT_CALL_THIRD_WAVE: // Swarmers + Warrior
                        {
                            Talk(EMOTE_WAVE_THREE);
                            DelayedTalk(GetVojak(), SAY_VOJAK_WAVE_THREE, 500);
                            DelayedTalk(me, SAY_WAVE_THREE, 5000);
                            EntryCheckPredicate pred(NPC_SIKTHIK_WARRIOR);
                            EntryCheckPredicate pred2(NPC_SIKTHIK_SWARMER);
                            summons.DoAction((ACTION_ENGAGE_COMBAT << WAVE_THREE), pred);
                            summons.DoAction((ACTION_ENGAGE_COMBAT << WAVE_THREE), pred2);

                            if (Creature* AmberWing = ObjectAccessor::GetCreature(*me, AmberWingGUID))
                                if (AmberWing->IsAlive())
                                    AmberWing->AI()->DoAction(ACTION_BOMBARDMENT);

                            DelayedTalk(me, SAY_AMBERWING, 4500);
                            break;
                        }
                        case EVENT_CALL_FOURTH_WAVE: // Demolishers + Warrior
                        {
                            Talk(EMOTE_WAVE_FOUR);
                            DelayedTalk(GetVojak(), SAY_VOJAK_WAVE_FOUR, 500);
                            EntryCheckPredicate pred(NPC_SIKTHIK_WARRIOR);
                            EntryCheckPredicate pred2(NPC_SIKTHIK_DEMOLISHER);
                            summons.DoAction((ACTION_ENGAGE_COMBAT << WAVE_FOUR), pred);
                            summons.DoAction((ACTION_ENGAGE_COMBAT << WAVE_FOUR), pred2);

                            if (Creature* AmberWing = ObjectAccessor::GetCreature(*me, AmberWingGUID))
                                if (AmberWing->IsAlive())
                                    AmberWing->DespawnOrUnsummon();
                            break;
                        }
                        case EVENT_CALL_BOSS: // Vo'Jak
                            if (Creature* vojak = Creature::GetCreature(*me, vojakGUID))
                                vojak->AI()->DoAction(ACTION_ENGAGE_COMBAT);
                            break;
                        case EVENT_INTRO_MOVE:
                            me->GetMotionMaster()->MovePoint(POINT_INTRO, 1523.673584f, 5313.781250f, 185.226746f);
                            break;
                        case EVENT_PLAYER_CHECK:
                        {
                            bool playersAlive = false;
                            Map::PlayerList const &players = me->GetMap()->GetPlayers();
                            for (auto itr = players.begin(); itr != players.end(); ++itr)
                            {
                                if (Player* player = itr->GetSource())
                                    if (player->IsAlive() && !player->IsGameMaster() && player->GetWMOAreaId() == WMO_REAR_STAGING_AREA)
                                    {
                                        playersAlive = true;
                                        break;
                                    }
                            }

                            if (!playersAlive)
                                ResetEncounter();
                            else
                                events.ScheduleEvent(EVENT_PLAYER_CHECK, 5000);
                            break;
                        }
                        case EVENT_OUTRO_MOVE:
                            if (Creature* loChu = me->FindNearestCreature(NPC_LO_CHU, 500.0f))
                                loChu->GetMotionMaster()->MoveSplinePath(loChuPath, loChuPathLenght, false, false, 8.0f, false, false, false);
                            if (Creature* liChu = me->FindNearestCreature(NPC_LI_CHU, 500.0f))
                                liChu->GetMotionMaster()->MoveSplinePath(liChuPath, liChuPathLenght, false, false, 8.0f, false, false, false);
                            me->GetMotionMaster()->MoveSplinePath(yangIronclawPath, yangIronclawPathLenght, false, false, 5.0f, false, false, false);
                            DelayedTalk(me, SAY_GATE_1, me->GetSplineDuration());
                            events.ScheduleEvent(EVENT_PLACE_BOMBS, me->GetSplineDuration() + 5000);
                            break;
                        case EVENT_DETONATE_SPEECH:
                            Talk(SAY_GATE_2);
                            me->GetMotionMaster()->MovePoint(0, 1615.413f, 5407.448f, 138.68094f);
                            DelayedTalk(me, SAY_GATE_3, 5000);
                            events.ScheduleEvent(EVENT_DETONATE_BOMBS, 7500);
                            break;
                        case EVENT_PLACE_BOMBS:
                            me->GetMotionMaster()->MovePoint(POINT_BOMB, bombsPos[bombCnt++]);
                            break;
                        case EVENT_DETONATE_BOMBS:
                            instance->SetData(DATA_VOJAK_DOOR, 0);
                            DoCast(me, SPELL_BREACH_DOOR_TARGET, true);
                            me->GetMotionMaster()->MovePoint(0, 1615.413f, 5407.448f, 138.68094f);
                            DelayedTalk(me, SAY_GATE_4, 4000);
                            break;
                        default:
                            break;
                    }
                }
            }

        private:
            uint8 bombCnt;
            EventMap events;
            SummonList summons;
            InstanceScript * instance;
            ObjectGuid vojakGUID;
            ObjectGuid AmberWingGUID;
            bool encounterInProgress;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_yang_ironclawAI(creature);
        }
};


class npc_sikthik_warden : public CreatureScript
{
    public:
        npc_sikthik_warden() : CreatureScript("npc_sikthik_warden") { }

        struct npc_sikthik_wardenAI : public ScriptedAI
        {
            npc_sikthik_wardenAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                if (Creature* Yang = GetClosestCreatureWithEntry(me, NPC_YANG_IRONCLAW, 50.0f, true))
                    me->Attack(Yang, true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sikthik_wardenAI(creature);
        }
};

static const Position PATH_POINTS[] =
{
    
    {1511.885f, 5378.232f, 138.95970f, 4.761f},
    {1512.162f, 5372.485f, 139.08124f, 4.765f},
    {1512.879f, 5359.946f, 146.22418f, 4.816f},
    {1524.942f, 5348.655f, 146.23088f, 0.053f},
    {1549.231f, 5350.165f, 160.99088f, 0.037f},
    {1567.434f, 5341.903f, 161.21361f, 3.485f},
    {1555.366f, 5329.321f, 161.66699f, 3.151f},
    {1507.551f, 5326.483f, 161.21100f, 3.233f},
    {1480.644f, 5324.358f, 176.12379f, 3.237f},
    {1470.566f, 5301.831f, 176.12379f, 4.769f},
    {1471.052f, 5292.643f, 179.70631f, 4.789f},
    {1483.235f, 5286.341f, 179.77177f, 0.159f},
    {1493.571f, 5287.627f, 184.71986f, 0.127f}
};

/*
    Koz:
    This is a generic AI for the Demolishers, Swarmers and Warriors
    It handles wave movement:
    - wave assignment (side -> movement timer)
    - randomized path generation
    - engaging when platform is reached
*/

struct npc_vojak_addAI : public ScriptedAI
{
    enum
    {
        ADD_STEP_DELAY          = 1000, // delay per add number
        RIGHT_SIDE_DELAY        = 3000  // added delay if on right side
    };

    npc_vojak_addAI(Creature* creature) : ScriptedAI(creature)
    {
        triggerTimer = 0;
        waveNumber = 0;
        summonerGUID = ObjectGuid::Empty;
        addCounter = 0;
        TRIGGER_TIMER = 0;
        aggrotimer = 0;
    }

    void DoAction(int32 actionId) override
    {
        if (actionId & (ACTION_ENGAGE_COMBAT << waveNumber))
        {
            me->SetReactState(REACT_PASSIVE);
            triggerTimer = TRIGGER_TIMER;
            //TC_LOG_FATAL("script", "Triggering {}, {} seconds", me->GetName().c_str(), triggerTimer);
        }
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == 0)
        {
            // read the mask (left side, add number, wave Id)
            bool leftSideAdd = (data >> 16) & 0xFF;
            addCounter = (data >> 8) & 0xFF;
            waveNumber = data & 0xFF;
            uint32 entry = me->GetEntry();
            
            TRIGGER_TIMER = ADD_STEP_DELAY + addCounter * ADD_STEP_DELAY + (leftSideAdd ? 0 : RIGHT_SIDE_DELAY);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (triggerTimer)
        {
            if (triggerTimer <= diff)
            {
                // move towards the upper platform
                Movement::MoveSplineInit init(me);
                for (auto itr : PATH_POINTS)
                {
                    float x, y, z;
                    me->GetRandomPoint(itr, 5.0f, x, y, z);
                    init.Path().push_back(G3D::Vector3(x, y, z));
                }
                
                init.SetSmooth();
                init.SetUncompressed();
                init.Launch();
                //me->GetMotionMaster()->MovePath(6162001, false);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                me->SetReactState(REACT_PASSIVE);
                aggrotimer = me->GetSplineDuration();
                triggerTimer = 0;
            } else triggerTimer -= diff;
        }

        if (aggrotimer)
        {
            if (aggrotimer <= diff)
            {
                me->SetReactState(REACT_AGGRESSIVE);
                DoZoneInCombat(me, 500.0f);
                aggrotimer = 0;
            } else aggrotimer -= diff;
        }

        if (me->HasReactState(REACT_PASSIVE))
            return;

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

    void IsSummonedBy(Unit* summoner) override { }

private:
    uint8 waveNumber;
    ObjectGuid summonerGUID;
    uint32 triggerTimer;
    uint32 TRIGGER_TIMER;
    uint32 aggrotimer;
    //bool leftSideAdd;
    uint8 addCounter;
};

class boss_commander_vojak : public CreatureScript
{
    enum Spells
    {
        SPELL_FRANTIC_FIGHTER       = 120757,
        SPELL_DASHING_STRIKE        = 120789,
        SPELL_THOUSAND_BLADES       = 120759
    };

    enum Events
    {
        EVENT_ENGAGE_COMBAT     = 1,
        EVENT_RISING_SPEED,
        EVENT_DASHING_STRIKE,
        EVENT_THOUSAND_BLADES,
        EVENT_THOUSAND_BLADES_END
    };

    public:
        boss_commander_vojak() : CreatureScript("boss_commander_vojak") { }


        struct boss_commander_vojakAI : public ScriptedAI
        {
            boss_commander_vojakAI(Creature* creature) : ScriptedAI(creature)
            {
                moving = true;
            }

            void Reset() override
            {
                events.Reset();
                me->GetMap()->SetWorldState(WORLDSTATE_WHERE_IS_MY_AIR_SUPPORT, 0);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_ENGAGE_COMBAT)
                {
                    moving = true;
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_UNK_6 | UNIT_FLAG_CAN_SWIM);
                    me->SetReactState(REACT_PASSIVE);
                    DoZoneInCombat(me);

                    // move towards the upper platform
                    Movement::MoveSplineInit init(me);
                    for (auto itr : PATH_POINTS)
                    {
                        float x, y, z;
                        me->GetRandomPoint(itr, 5.0f, x, y, z);
                        init.Path().push_back(G3D::Vector3(x, y, z));
                    }

                    init.SetSmooth();
                    init.SetUncompressed();
                    init.Launch();

                    events.ScheduleEvent(EVENT_RISING_SPEED, 3 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_ENGAGE_COMBAT, me->GetSplineDuration());
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!moving)
                    if (!UpdateVictimWithGaze() || !UpdateVictim())
                        return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ENGAGE_COMBAT:
                            moving = false;
                            me->SetReactState(REACT_AGGRESSIVE);
                            events.ScheduleEvent(EVENT_DASHING_STRIKE, 5000);
                            break;
                        case EVENT_RISING_SPEED:
                            DoCast(me, SPELL_FRANTIC_FIGHTER, true);
                            break;
                        case EVENT_DASHING_STRIKE:
                            if (Unit*  target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                            {
                                SetGazeOn(target);
                                DoCast(target, SPELL_DASHING_STRIKE, false);
                                dashingTargetGUID = target->GetGUID();
                            }
                            events.ScheduleEvent(EVENT_THOUSAND_BLADES, 3000);
                            events.ScheduleEvent(EVENT_DASHING_STRIKE, 13500);
                            break;
                        case EVENT_THOUSAND_BLADES:
                            DoCast(me, SPELL_THOUSAND_BLADES, false);
                            events.ScheduleEvent(EVENT_THOUSAND_BLADES_END, 4000);
                            break;
                        case EVENT_THOUSAND_BLADES_END:
                            me->SetReactState(REACT_AGGRESSIVE);
                            break;
                        default:
                            break;
                    }
                }

                if (!moving)
                    DoMeleeAttackIfReady();
            }

        private:
            bool moving;
            EventMap events;
            ObjectGuid dashingTargetGUID;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_commander_vojakAI(creature);
        }
};

// 63106
class npc_sikthik_swarmer : public CreatureScript
{
    public:
        npc_sikthik_swarmer() : CreatureScript("npc_sikthik_swarmer") { }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_vojak_addAI(creature);
        }
};

// 61670
class npc_sikthik_demolisher : public CreatureScript
{
    public:
        npc_sikthik_demolisher() : CreatureScript("npc_sikthik_demolisher") { }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_vojak_addAI(creature);
        }
};

// 61701
class npc_sikthik_warrior : public CreatureScript
{
    public:
        npc_sikthik_warrior() : CreatureScript("npc_sikthik_warrior") { }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_vojak_addAI(creature);
        }
};

/*
    Koz:
    This is a script for Lo Chu and Li Chu
    Keg Rotation:
        1. NPC runs towards the keg pool
        2. NPC casts Assignment Spell (periodic trigger) on self when arrived at the pool
    If Assignment spell hits an NPC (Barrel Target without "Has Barrel" aura):
        3. NPC picks up a random Keg in 10yd radius (Grab Barrel spell)
        4. NPC moves (WALKS!) to an invisible stalker near the said target NPC
        5. When arrived, NPC ejects the keg (jumps to targeted Barrel Target NPC)
        6. NPC moves (RUNS!) back to the keg pool
*/

enum HelperData
{
    SIDE_LEFT           = 0,
    SIDE_RIGHT,

    POINT_KEG_POOL      = 0,
    POINT_KEG,
    POINT_KEG_MID,
    POINT_KEG_SIDE,

};

static const Position helperPositions[2][3] = 
{
    // SIDE_LEFT
    {
        {1488.913f, 5299.012f, 184.64816f, 4.785f}, // Keg pool
        {1513.255f, 5309.223f, 184.64906f, 1.741f},
        {1488.649f, 5307.495f, 184.64957f, 1.564f}
    },
    // SIDE_RIGHT
    {
        {1548.154f, 5287.516f, 184.74574f, 4.749f}, // keg pool
        {1540.258f, 5310.451f, 184.65018f, 1.600f},
        {1556.005f, 5312.284f, 184.65018f, 1.639f}
    }
};

enum 
{
    SPELL_GRAB_BARREL           = 120405, // condition target keg, triggers SPELL_KEG_ENTER_VEHICLE
    SPELL_KEG_ENTER_VEHICLE     = 120402,
    SPELL_ASSIGNMENT            = 122347, // periodic, find nearest keg target
    SPELL_ASSIGNMENT_EFF        = 122346, // condition target Barrel Target, triggers SPELL_HAS_BARREL
    SPELL_HAS_BARREL            = 122345,
    SPELL_CLEAR_HAS_BARREL      = 122518, // 5yd aoe
    SPELL_EJECT_PASSENGERS      = 79737,
    SPELL_BARREL_DROP_FORCE     = 122385,
    SPELL_BARREL_JUMP           = 122376, // condition target Barrel Target?
};

class npc_chu_helper : public CreatureScript
{
    enum
    {
        EVENT_PICK_UP_KEG           = 1,
        EVENT_MOVE_TO_TARGET,
        EVENT_PLACE_KEG,
        EVENT_MOVE_TO_POOL,

    };

    public:
        npc_chu_helper() : CreatureScript("npc_chu_helper") { }

        struct npc_chu_helperAI : public ScriptedAI
        {
            npc_chu_helperAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                side = (me->GetEntry() == NPC_LO_CHU) ? SIDE_RIGHT : SIDE_LEFT;
                me->SetFaction(1665);
                barrelTargetGUID = ObjectGuid::Empty;
                barrelGUID = ObjectGuid::Empty;
                eventInProgress = false;
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                barrelTargetGUID = ObjectGuid::Empty;
                barrelGUID = ObjectGuid::Empty;
                eventInProgress = false;
                events.Reset();
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (!eventInProgress)
                    return;

                switch (spell->Id)
                {
                    case SPELL_GRAB_BARREL:
                        barrelGUID = target->GetGUID();
                        target->CastSpell(me, SPELL_KEG_ENTER_VEHICLE, true);
                        events.ScheduleEvent(EVENT_MOVE_TO_TARGET, 1000);
                        break;
                    case SPELL_ASSIGNMENT_EFF:
                        if (barrelTargetGUID)
                            break;
                        barrelTargetGUID = target->GetGUID();
                        me->RemoveAurasDueToSpell(SPELL_ASSIGNMENT);
                        DoCast(target, SPELL_HAS_BARREL, true);
                        DoCast(me, SPELL_GRAB_BARREL, true);
                        break;
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                damage = 0;
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_ENGAGE_COMBAT:
                        eventInProgress = true;
                        events.ScheduleEvent(EVENT_MOVE_TO_POOL, side ? 1000 : 2000);
                        barrelTargetGUID = ObjectGuid::Empty;
                        me->SetReactState(REACT_PASSIVE);
                        me->SetFaction(35);
                        break;
                    case ACTION_RESET_COMBAT:
                        //me->RemoveAllAuras();
                        //me->GetVehicleKit()->RemoveAllPassengers();
                        events.Reset();
                        eventInProgress = false;
                        barrelTargetGUID = ObjectGuid::Empty;
                        EnterEvadeMode();
                        break;
                    default:
                        break;
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE)
                {
                    switch (pointId)
                    {
                        case POINT_KEG_POOL:
                            events.ScheduleEvent(EVENT_PICK_UP_KEG, 1000);
                            break;
                        case POINT_KEG:
                            //DoCast(me, SPELL_EJECT_PASSENGERS, true);
                            me->GetVehicleKit()->RemoveAllPassengers();
                            if (Creature* barrel = Creature::GetCreature(*me, barrelGUID))
                                if (Creature* barrelTarget = Creature::GetCreature(*me, barrelTargetGUID))
                                {
                                    barrel->ExitVehicle();
                                    //barrel->CastSpell(barrelTarget, SPELL_BARREL_JUMP, true);
                                    barrel->AI()->SetGUID(barrelTargetGUID);
                                }
                            barrelTargetGUID = ObjectGuid::Empty;

                            events.ScheduleEvent(EVENT_MOVE_TO_POOL, 1000);
                            break;
                    }
                }

            }

            void UpdateAI(uint32 diff) override
            {
                if (UpdateVictim())
                    DoMeleeAttackIfReady();

                if (!eventInProgress)
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PICK_UP_KEG:
                            DoCast(me, SPELL_ASSIGNMENT, true);
                            break;
                        case EVENT_MOVE_TO_TARGET:
                        {
                            me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                            if (Creature* target = Creature::GetCreature(*me, barrelTargetGUID))
                                if (Creature* stalker = target->FindNearestCreature(NPC_INVISIBLE_STALKER, 10.0f, true))
                                    me->GetMotionMaster()->MovePoint(POINT_KEG, *stalker);
                            break;
                        }
                        case EVENT_PLACE_KEG:
                            break;
                        case EVENT_MOVE_TO_POOL:
                            me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                            me->GetMotionMaster()->MovePoint(POINT_KEG_POOL, helperPositions[side][POINT_KEG_POOL]);
                            break;
                    }
                }
            }

        private:
            ObjectGuid barrelTargetGUID;
            ObjectGuid barrelGUID;
            uint8 side;
            bool eventInProgress;
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_chu_helperAI(creature);
        }
};

// 120405
class spell_grab_barrel : public SpellScriptLoader
{
    public:
        spell_grab_barrel() : SpellScriptLoader("spell_grab_barrel") { }

        class spell_grab_barrel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_grab_barrel_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if (Trinity::UnitAuraCheck(true, SPELL_KEG_ACTIVE));
                Trinity::Containers::RandomResizeList(targets, 1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_grab_barrel_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_grab_barrel_SpellScript();
        }
};

// 122346
class spell_barrel_assignment : public SpellScriptLoader
{
    public:
        spell_barrel_assignment() : SpellScriptLoader("spell_barrel_assignment") { }

        class spell_barrel_assignment_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_barrel_assignment_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (Unit*  caster = GetCaster())
                {
                    targets.remove_if (Trinity::UnitAuraCheck(true, SPELL_HAS_BARREL));
                    Trinity::Containers::RandomResizeList(targets, 1);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_barrel_assignment_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_barrel_assignment_SpellScript();
        }
};

/*
    Koz:
    This is the script for Mantid Tar Keg
    We handle respawn on wipe aswell as 
    jumping after delivery here.
*/

class npc_mantid_tar_keg : public CreatureScript
{
    public:
        npc_mantid_tar_keg() : CreatureScript("npc_mantid_tar_keg") { }

        struct npc_mantid_tar_kegAI : public ScriptedAI
        {
            npc_mantid_tar_kegAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                me->SetCorpseDelay(1);
                me->SetRespawnDelay(5);
                jumptimer = 0;
            }

            void SetGUID(ObjectGuid guid, int32 /*type*/) override
            {
                targetGUID = guid;
                jumptimer = 250;
                if (Creature* target = Creature::GetCreature(*me, targetGUID))
                {
                    Position pos = target->GetPosition();
                    target->UpdateGroundPositionZ(pos.GetPositionX(), pos.GetPositionY(), pos.m_positionZ);
                    me->GetMotionMaster()->MoveJump(pos, 10.0f, 1.0f);
                }
            }

            void Reset() override
            {
                me->SetPosition(me->GetHomePosition());
                if (me->m_movementInfo.transport.guid)
                    me->m_movementInfo.transport.guid = ObjectGuid::Empty;
                me->SendMovementFlagUpdate();
                targetGUID = ObjectGuid::Empty;
                me->GetMotionMaster()->MoveIdle();
                me->SetReactState(REACT_PASSIVE);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                DoCast(me, SPELL_KEG_INACTIVE, true);
            }

            bool CanRespawn() override
            {
                return instance->GetBossState(BOSS_VOJAK) == NOT_STARTED;
            }

            void OnSpellClick(Unit* clicker, bool& /*result*/) override
            {
                //clicker->CastSpell(clicker, SPELL_CARRYING_CAUSTIC_TAR, true);
                if (Creature* target = Creature::GetCreature(*me, targetGUID))
                    target->RemoveAllAuras();
                me->DisappearAndDie();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == EFFECT_MOTION_TYPE && pointId == EVENT_JUMP)
                {
                    me->RemoveAurasDueToSpell(SPELL_KEG_INACTIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    DoCast(me, SPELL_KEG_ACTIVE, true);
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!jumptimer)
                    return;

                if (jumptimer <= diff)
                {

                    jumptimer = 0;
                } else jumptimer -= diff;
            }

        private:
            uint32 jumptimer;
            ObjectGuid targetGUID;
            InstanceScript * instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_mantid_tar_kegAI(creature);
        }
};

class npc_sap_puddle_vojak : public CreatureScript
{
    enum
    {
        SPELL_DRAIN_BARREL_TOP          = 122522, // Areatrigger 349
        SPELL_DRAIN_BARREL_BASE         = 120473  // Areatrigger 325, 359
    };

    public:
        npc_sap_puddle_vojak() : CreatureScript("npc_sap_puddle_vojak") { }

        struct npc_sap_puddle_vojakAI : public ScriptedAI
        {
            npc_sap_puddle_vojakAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                aoeTimer = 500;

                uint32 spellId = SPELL_DRAIN_BARREL_BASE;
                if (fabs(me->GetPositionZ() - 185.22f) < INTERACTION_DISTANCE)
                    spellId = SPELL_DRAIN_BARREL_TOP;

                DoCast(me, spellId, true);

                me->DespawnOrUnsummon(15000);
            }

            void MoveInLineOfSight(Unit* who) override { }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                damage = 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (aoeTimer <= diff)
                {
                    DoCast(me, 120270, true);
                    aoeTimer = 500;
                } else aoeTimer -= diff;
            }

        private:
            uint32 aoeTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sap_puddle_vojakAI(creature);
        }
};

// Sik'Thik amberwing - 61699
class npc_sikthik_amberwing : public CreatureScript
{
    enum eSpells
    {
        SPELL_GREEN_WINGS           = 126316,
        SPELL_BOMBARD               = 120199,
        SPELL_BOMBARD_DUMMY         = 120202,
        SPELL_BOMBARD_PROTECTION    = 120561,
    };

    enum eEvents
    {
        EVENT_BOMBARDMENT           = 1,
        EVENT_MOVE                  = 2,
    };

    public:
        npc_sikthik_amberwing() : CreatureScript("npc_sikthik_amberwing") { }

        struct npc_sikthik_amberwingAI : public ScriptedAI
        {
            npc_sikthik_amberwingAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            uint32 wp;

            void InitializeAI() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetDisplayId(43955); // idk why, but this value not got from db...
                // Set Flying
                me->SetCanFly(true);
                me->SetSpeed(MOVE_FLIGHT, 5.5f, true);
                me->SendMovementFlagUpdate();
                me->AddUnitMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING);
                me->SetAnimTier(AnimTier::Hover);
                me->SetDisableGravity(true);
                DoCast(me, SPELL_GREEN_WINGS, true);
                wp = 0;
                SetCombatMovement(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                events.ScheduleEvent(EVENT_MOVE, urand(100, 200));
            }

            void FlyToPlatform()
            {
                float x, y, z;
                Position pos;
                me->GetRandomPoint(PATH_AMBERWING[wp], 2.0f, x, y,z);
                pos.Relocate(x, y, PATH_AMBERWING[wp].GetPositionZ());
                me->GetMotionMaster()->MovePoint(wp, pos);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_BOMBARDMENT)
                    FlyToPlatform();
            }

            void JustDied(Unit* killer) override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_WHERE_IS_MY_AIR_SUPPORT, 1);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                wp++;

                switch (pointId)
                {
                    case 1:
                        if (Player* itr = me->FindNearestPlayer(80.0f))
                            me->CombatStart(itr, true);
                        me->GetMotionMaster()->Clear();
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                        events.ScheduleEvent(EVENT_BOMBARDMENT, 500);
                        events.ScheduleEvent(EVENT_MOVE, 10 * IN_MILLISECONDS);
                        break;
                    case 2:
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                        me->GetMotionMaster()->Clear();
                        me->NearTeleportTo(AmberWingSpawnPos.GetPositionX(), AmberWingSpawnPos.GetPositionY(), AmberWingSpawnPos.GetPositionZ(), AmberWingSpawnPos.GetOrientation());
                        wp = 0;
                        break;
                    default:
                        events.ScheduleEvent(EVENT_MOVE, urand(100, 200));
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BOMBARDMENT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true))
                                me->CastSpell(target, SPELL_BOMBARD, false);

                            events.ScheduleEvent(EVENT_BOMBARDMENT, urand(1500, 3000));
                            break;
                        case EVENT_MOVE:
                            events.CancelEvent(EVENT_BOMBARDMENT);
                            FlyToPlatform();
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sikthik_amberwingAI(creature);
        }
};

class BombardmentPredicate
{
    public:
        BombardmentPredicate() { }

        bool operator()(WorldObject* object)
        {
            return object && (object->GetEntry() == NPC_LI_CHU || object->GetEntry() == NPC_LO_CHU || object->GetEntry() == NPC_INVISIBLE_STALKER || object->GetEntry() == NPC_BARREL_TARGET || object->GetEntry() == NPC_MANTID_TAR_KEG);
        }
};

// Bombard 120200
class spell_amberwing_bombard : public SpellScriptLoader
{
    public:
        spell_amberwing_bombard() : SpellScriptLoader("spell_amberwing_bombard") { }

        class spell_amberwing_bombard_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_amberwing_bombard_SpellScript);

            void CheckTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(BombardmentPredicate());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_amberwing_bombard_SpellScript::CheckTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_amberwing_bombard_SpellScript::CheckTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_amberwing_bombard_SpellScript();
        }
};

// Caustic Tar 120270
class spell_vojak_caustic_tar : public SpellScriptLoader
{
    public:
        spell_vojak_caustic_tar() : SpellScriptLoader("spell_vojak_caustic_tar") { }

        class spell_vojak_caustic_tar_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_vojak_caustic_tar_SpellScript);

            void CheckTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(BombardmentPredicate());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_vojak_caustic_tar_SpellScript::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_vojak_caustic_tar_SpellScript::CheckTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_vojak_caustic_tar_SpellScript();
        }
};

// Unstable Blast 121982
class spell_vojak_unstable_blast : public SpellScriptLoader
{
    public:
        spell_vojak_unstable_blast() : SpellScriptLoader("spell_vojak_unstable_blast") { }

        class spell_vojak_unstable_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_vojak_unstable_blast_SpellScript);

            void CheckTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(BombardmentPredicate());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_vojak_unstable_blast_SpellScript::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_vojak_unstable_blast_SpellScript();
        }
};

void AddSC_commander_vojak()
{
    new boss_commander_vojak();
    new npc_yang_ironclaw();

    new npc_sikthik_demolisher();
    new npc_sikthik_swarmer();
    new npc_sikthik_warden();
    new npc_sikthik_warrior();
    new npc_chu_helper();

    new npc_mantid_tar_keg();
    new spell_barrel_assignment();
    new spell_grab_barrel();
    new npc_sap_puddle_vojak();

    new npc_sikthik_amberwing();
    new spell_amberwing_bombard();
    new spell_vojak_caustic_tar();
    new spell_vojak_unstable_blast();
}