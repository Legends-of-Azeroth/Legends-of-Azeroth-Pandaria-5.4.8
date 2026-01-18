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
#include "ScriptedGossip.h"
#include "Vehicle.h"
#include "ObjectMgr.h"
#include "ScriptedEscortAI.h"
#include "CombatAI.h"
#include "PassiveAI.h"
#include "Player.h"
#include "SpellInfo.h"
#include "CreatureTextMgr.h"

/*######
## Quest 25134: Lazy Peons
## npc_lazy_peon
######*/

enum eYells
{
    SAY_SPELL_HIT,

    SAY_INTRO = 0,
    SAY_SPECIAL_1,
    SAY_SPECIAL_2,
    SAY_SPECIAL_3,
    SAY_SPECIAL_4,
    SAY_SPECIAL_5,
    SAY_SPECIAL_6,
    SAY_SPECIAL_7,
    SAY_SPECIAL_8,
    SAY_SPECIAL_9,
    SAY_SPECIAL_10,
    SAY_SPECIAL_11,
    SAY_SPECIAL_12,
    SAY_SPECIAL_13,
    SAY_SPECIAL_14,
    SAY_SPECIAL_15,
    SAY_SPECIAL_16,
    SAY_SPECIAL_17,
    SAY_SPECIAL_18,
    SAY_SPECIAL_19,
    SAY_SPECIAL_20,
    SAY_SPECIAL_21,
    SAY_SPECIAL_22,
};

enum eSpells
{
    SPELL_BUFF_SLEEP                 = 17743,
    SPELL_AWAKEN_PEON                = 19938,
    SPELL_SUMMON_GARROSH_JI          = 121139,
    SPELL_SUMMON_JI                  = 120749,
                                     
    // Garrosh Abilities             
    SPELL_BERSERK_CHARGE             = 84742,
    SPELL_WHIRLWIND                  = 84724,
    SPELL_SHOCKWAVE                  = 84715,

    // Quest: Gift of Hellscream 
    // So, will use 6td phase for player and 2 for creatures because i didn`t find phase aura.
    SPELL_TELEPORT_OUT_BLACKOUT      = 121123,
    SPELL_RING_OF_VALOR_IN_THE_ARENA = 132633,
    SPELL_VISUAL_TELEPORT            = 132629, // for creatures
};

enum eEvents
{
    EVENT_CHARGE_B  = 1,
    EVENT_WHIRLWIND = 2,
    EVENT_SHOCKWAVE = 3,
};

enum eQuests
{
    QUEST_LAZY_PEONS                   = 25134,
    QUEST_THE_HORDE_WAY                = 31013,
    QUEST_GIFT_OF_HELLSCREAM           = 31014,
    QUEST_LOST_IN_THE_FLOODS           = 25187,
    QUEST_WATERSHED_PATROL             = 25188,
    QUEST_LOST_BUT_NOT_FORGOTTEN       = 25193, // trigger kc3
    QUEST_SPIRIT_BE_PRAISED            = 25189, // trigger kc1
    QUEST_RAGGARANS_FURY               = 25192, // trigger kc2
    QUEST_THATS_THE_END_OF_THIS_RAPTOR = 25195, // trigger kc4
};

enum eGameObjects
{
    GO_LUMBERPILE = 175784,
};

enum eCreatures
{
    NPC_GARROSH_HORDE_WAY        = 62087, // should be summoned to trigged (DEST_NEARBY_ENTRY) cuz used by same spell in prev quest
    NPC_JI_FIREPAWN_HORDE_WAY    = 62081, // Same like Garrosh
    NPC_GARROSH_HORDE_WAY_CREDIT = 62089,

    /*Lost in the Floods*/
    NPC_LOST_IN_THE_FLOOD_KC1    = 39357,
    NPC_LOST_IN_THE_FLOOD_KC2    = 39358,
    NPC_LOST_IN_THE_FLOOD_KC3    = 39359,
    NPC_LOST_IN_THE_FLOOD_KC4    = 39360,

    /*Watershed Patrol*/
    NPC_WATERSHED_PATROL_KC1     = 39331,
    NPC_WATERSHED_PATROL_KC2     = 39332,
    NPC_WATERSHED_PATROL_KC3     = 39333,
    NPC_WATERSHED_PATROL_KC4     = 39334,

    /*Gift of Hellscream*/
    NPC_ARENA_CREDIT             = 62209,
    NPC_GADOON                   = 62100,
    NPC_FROZENGORE               = 62097,
    NPC_OGGAX                    = 62102,
};

enum eActions
{
    ACTION_GIFT_OF_HELLSCREAM,
    ACTION_CREAURE_SLAIN,
    ACTION_INIT_TRIAL,
};

const Position GarroshHordeWayPath[6]
{
    { 1658.80f, -4348.18f, 26.35f, 3.61f },
    { 1662.11f, -4342.89f, 26.36f, 1.28f },
    { 1659.93f, -4337.30f, 26.45f, 1.19f },
    { 1646.26f, -4359.82f, 26.76f, 3.29f },
    { 1633.08f, -4362.12f, 26.77f, 3.52f },
    { 1624.11f, -4366.82f, 24.61f, 3.54f },
};

const Position TrialSpawnPoints[3]
{
    { 2001.87f, -4730.90f, 86.77f, 6.03f},
    { 1999.71f, -4739.68f, 86.77f, 6.23f },
    { 2000.45f, -4747.63f, 86.77f, 6.28f },
};

const Position GarroshArenaPos  = { 2030.34f, -4713.97f, 95.93f, 4.75f };
const Position RingOfValor      = { 2031.56f, -4753.59f, 86.77f, 1.56f };
const Position LeaveRingOfValor = { 2125.208f, -4731.869f, 50.304f, 2.581132f };

const std::map<uint32, uint32> trialMatchType =
{
    { NPC_GADOON,     0 },
    { NPC_FROZENGORE, 1 },
    { NPC_OGGAX,      2 },
};

const std::map<uint32, uint32> watershedMatchType =
{
    { QUEST_SPIRIT_BE_PRAISED,            NPC_WATERSHED_PATROL_KC1 },
    { QUEST_RAGGARANS_FURY,               NPC_WATERSHED_PATROL_KC2 },
    { QUEST_LOST_BUT_NOT_FORGOTTEN,       NPC_WATERSHED_PATROL_KC3 },
    { QUEST_THATS_THE_END_OF_THIS_RAPTOR, NPC_WATERSHED_PATROL_KC4 },
};

class npc_lazy_peon : public CreatureScript
{
    public:
        npc_lazy_peon() : CreatureScript("npc_lazy_peon") { }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lazy_peonAI(creature);
        }

        struct npc_lazy_peonAI : public ScriptedAI
        {
            npc_lazy_peonAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid PlayerGUID;

            uint32 RebuffTimer;
            bool work;

            void Reset() override
            {
                PlayerGUID = ObjectGuid::Empty;
                RebuffTimer = 0;
                work = false;
            }

            void MovementInform(uint32 /*type*/, uint32 id) override
            {
                if (id == 1)
                    work = true;
            }

            void SpellHit(Unit* caster, const SpellInfo* spell) override
            {
                if (spell->Id != SPELL_AWAKEN_PEON)
                    return;

                Player* player = caster->ToPlayer();
                if (player && player->GetQuestStatus(QUEST_LAZY_PEONS) == QUEST_STATUS_INCOMPLETE)
                {
                    player->KilledMonsterCredit(me->GetEntry(), me->GetGUID());
                    Talk(SAY_SPELL_HIT, caster);
                    me->RemoveAllAuras();
                    if (GameObject* Lumberpile = me->FindNearestGameObject(GO_LUMBERPILE, 20))
                        me->GetMotionMaster()->MovePoint(1, Lumberpile->GetPositionX() - 1, Lumberpile->GetPositionY(), Lumberpile->GetPositionZ());
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (work == true)
                    me->HandleEmoteCommand(EMOTE_ONESHOT_WORK_CHOPWOOD);
                if (RebuffTimer <= diff)
                {
                    DoCast(me, SPELL_BUFF_SLEEP);
                    RebuffTimer = 300000; //Rebuff agian in 5 minutes
                }
                else
                    RebuffTimer -= diff;
                if (!UpdateVictim())
                    return;
                DoMeleeAttackIfReady();
            }
        };
};

// Garrosh Hellscream warchief 39605
class npc_garrosh_hellscream_warchief : public CreatureScript
{
    public:
        npc_garrosh_hellscream_warchief() : CreatureScript("npc_garrosh_hellscream_warchief") { }

        bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_THE_HORDE_WAY)
                creature->CastSpell(player, SPELL_SUMMON_GARROSH_JI, true);

            return false;
        }

        struct npc_garrosh_hellscream_warchiefAI : public ScriptedAI
        {
            npc_garrosh_hellscream_warchiefAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_WHIRLWIND, 25 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CHARGE_B, urand(1 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SHOCKWAVE, 15 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHARGE_B:
                            if (HandleDelayIfInWhirlwind(eventId))
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_BERSERK_CHARGE, false);

                            events.ScheduleEvent(EVENT_CHARGE_B, urand(12.5 * IN_MILLISECONDS, 31 * IN_MILLISECONDS));
                            break;
                        case EVENT_WHIRLWIND:
                            DoCast(me, SPELL_WHIRLWIND);
                            events.ScheduleEvent(EVENT_WHIRLWIND, 50 * IN_MILLISECONDS);
                            me->ClearUnitState(UNIT_STATE_CASTING);
                            break;
                        case EVENT_SHOCKWAVE:
                            if (HandleDelayIfInWhirlwind(eventId))
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 180.0f, true))
                                DoCast(target, SPELL_SHOCKWAVE, false);

                            events.ScheduleEvent(EVENT_SHOCKWAVE, 15 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                bool HandleDelayIfInWhirlwind(uint32 eventID)
                {
                    if (me->HasAura(SPELL_WHIRLWIND))
                    {
                        events.RescheduleEvent(eventID, urand(1 * IN_MILLISECONDS, 2 * IN_MILLISECONDS));
                        return true;
                    }

                    return false;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_garrosh_hellscream_warchiefAI(creature);
        }
};

// Garrosh Horde Way 62087
class npc_garrosh_horde_way_quest : public CreatureScript
{
    public:
        npc_garrosh_horde_way_quest() : CreatureScript("npc_garrosh_horde_way_quest") { }

        struct npc_garrosh_horde_way_questAI : public ScriptedAI
        {
            npc_garrosh_horde_way_questAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid summonerGUID;
            uint32 delay, slainCount;
            std::vector<ObjectGuid> challengerGUIDs;

            void IsSummonedBy(Unit* summoner) override
            {
                summonerGUID = summoner->GetGUID();
                challengerGUIDs.clear();

                if (summoner->ToPlayer() && summoner->ToPlayer()->GetQuestStatus(QUEST_THE_HORDE_WAY) == QUEST_STATUS_REWARDED)
                {
                    slainCount = 0;
                    DoAction(ACTION_GIFT_OF_HELLSCREAM);
                    return;
                }
                me->SetFacingToObject(summoner);
                me->SetWalk(true);
                delay = 0;
                me->SetPhaseMask(2, true);
                Talk(SAY_INTRO);
                me->GetMotionMaster()->MovePoint(0, GarroshHordeWayPath[0]);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_GIFT_OF_HELLSCREAM:
                        me->SetPhaseMask(2, true);
                        delay = 0;

                        me->m_Events.Schedule(delay += 1000, 1, [this]()
                        {
                            Talk(SAY_SPECIAL_17);
                        });

                        me->m_Events.Schedule(delay += 10000, 2, [this]()
                        {
                            if (Unit* owner = ObjectAccessor::GetUnit(*me, summonerGUID))
                                owner->CastSpell(owner, SPELL_RING_OF_VALOR_IN_THE_ARENA, true);

                            Talk(SAY_SPECIAL_18);

                            for (auto&& itr : trialMatchType)
                                if (Creature* challenger = me->SummonCreature(itr.first, TrialSpawnPoints[itr.second], TEMPSUMMON_MANUAL_DESPAWN))
                                    challengerGUIDs.push_back(challenger->GetGUID());
                        });

                        me->m_Events.Schedule(delay += 11000, 3, [this]()
                        {
                            Talk(SAY_SPECIAL_19);
                        });

                        me->m_Events.Schedule(delay += 7000, 4, [this]()
                        {
                            Talk(SAY_SPECIAL_20);
                        });

                        me->m_Events.Schedule(delay += 11000, 5, [this]()
                        {
                            Talk(SAY_SPECIAL_21);

                            for (auto&& itr : challengerGUIDs)
                                if (Creature* challenger = ObjectAccessor::GetCreature(*me, itr))
                                    challenger->AI()->DoAction(ACTION_INIT_TRIAL);
                        });

                        me->m_Events.Schedule(delay += 4500, 6, [this]()
                        {
                            Talk(SAY_SPECIAL_22);
                        });
                        break;
                    case ACTION_CREAURE_SLAIN:
                        if (++slainCount >= 3)
                        {
                            delay = 0;
                            me->m_Events.Schedule(delay += 6000, 7, [this]()
                            {
                                if (Unit* owner = ObjectAccessor::GetUnit(*me, summonerGUID))
                                    owner->CastSpell(owner, SPELL_TELEPORT_OUT_BLACKOUT, true);

                                me->DespawnOrUnsummon();
                            });
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
                    case 0:
                        Talk(SAY_SPECIAL_1);
                        me->m_Events.Schedule(delay += 8000, 1, [this]()
                        {
                            if (Unit* owner = ObjectAccessor::GetUnit(*me, summonerGUID))
                                me->SetFacingTo(me->GetAngle(owner));

                            Talk(SAY_SPECIAL_2);
                        });

                        me->m_Events.Schedule(delay += 6000, 1, [this]()
                        {
                            Talk(SAY_SPECIAL_3);
                            me->GetMotionMaster()->MovePoint(1, GarroshHordeWayPath[1]);
                        });

                        me->m_Events.Schedule(delay += 3000, 1, [this]()
                        {
                            Talk(SAY_SPECIAL_4);
                            me->GetMotionMaster()->MovePoint(2, GarroshHordeWayPath[2]);
                        });
                        break;
                    case 2:
                        delay = 0;
                        me->m_Events.Schedule(delay += 4500, 1, [this]()
                        {
                            Talk(SAY_SPECIAL_5);
                            me->GetMotionMaster()->MovePoint(3, GarroshHordeWayPath[0]);
                        });
                        break;
                    case 3:
                        me->SetFacingTo(3.59f);
                        Talk(SAY_SPECIAL_6);

                        delay = 0;
                        me->m_Events.Schedule(delay += 9000, 1, [this]()
                        {
                            Talk(SAY_SPECIAL_7);
                        });

                        me->m_Events.Schedule(delay += 6500, 1, [this]()
                        {
                            Talk(SAY_SPECIAL_8);
                        });

                        me->m_Events.Schedule(delay += 4000, 1, [this]()
                        {
                            Talk(SAY_SPECIAL_9);
                            me->GetMotionMaster()->MovePoint(4, GarroshHordeWayPath[3]);
                        });
                        break;
                    case 4:
                        Talk(SAY_SPECIAL_10);

                        delay = 0;
                        me->m_Events.Schedule(delay += 5000, 1, [this]()
                        {
                            if (Unit* owner = ObjectAccessor::GetUnit(*me, summonerGUID))
                                me->SetFacingTo(me->GetAngle(owner));

                            Talk(SAY_SPECIAL_11);
                        });

                        me->m_Events.Schedule(delay += 3000, 1, [this]()
                        {
                            Talk(SAY_SPECIAL_12);
                        });

                        me->m_Events.Schedule(delay += 13500, 1, [this]()
                        {
                            Talk(SAY_SPECIAL_13);
                            me->GetMotionMaster()->MovePoint(5, GarroshHordeWayPath[4]);
                        });
                        break;
                    case 5:
                        me->SetFacingTo(Position::NormalizeOrientation(me->GetOrientation() + M_PI));

                        delay = 0;
                        me->m_Events.Schedule(delay += 6500, 1, [this]()
                        {
                            Talk(SAY_SPECIAL_14);
                        });

                        me->m_Events.Schedule(delay += 6500, 1, [this]()
                        {
                            me->SetFacingTo(Position::NormalizeOrientation(me->GetOrientation() + M_PI));
                            Talk(SAY_SPECIAL_15);
                        });

                        me->m_Events.Schedule(delay += 5500, 1, [this]()
                        {
                            Talk(SAY_SPECIAL_16);

                            if (Unit* owner = ObjectAccessor::GetUnit(*me, summonerGUID))
                                owner->RemoveAurasDueToSpell(SPELL_SUMMON_JI);

                            me->GetMotionMaster()->MovePoint(6, GarroshHordeWayPath[5]);
                        });
                        break;
                    case 6:
                        me->DespawnOrUnsummon();
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_garrosh_horde_way_questAI(creature);
        }
};

// Ji Firepawn Horde Way 62081
class npc_ji_firepaw_horde_way_quest : public CreatureScript
{
    public:
        npc_ji_firepaw_horde_way_quest() : CreatureScript("npc_ji_firepaw_horde_way_quest") { }

        struct npc_ji_firepaw_horde_way_questAI : public ScriptedAI
        {
            npc_ji_firepaw_horde_way_questAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid summonerGUID;
            uint32 delay;
            bool allowFollow;

            void IsSummonedBy(Unit* summoner) override
            {
                summonerGUID = summoner->GetGUID();
                me->SetPhaseMask(2, true);
                allowFollow = true;

                if (summoner->ToPlayer() && summoner->ToPlayer()->GetQuestStatus(QUEST_THE_HORDE_WAY) == QUEST_STATUS_REWARDED)
                {
                    allowFollow = false;
                    DoAction(ACTION_GIFT_OF_HELLSCREAM);
                    return;
                }

                me->DespawnOrUnsummon(1 * MINUTE * IN_MILLISECONDS + 50 * IN_MILLISECONDS);
                delay = 0;
                me->m_Events.Schedule(delay += 45000, 1, [this]()
                {
                    Talk(SAY_INTRO);
                });

                me->m_Events.Schedule(delay += 50000, 1, [this]()
                {
                    Talk(SAY_SPECIAL_1);
                });
            }

            void EnterEvadeMode() override 
            {
                me->HandleEmoteStateCommand(EMOTE_STATE_NONE);
                Talk(SAY_SPECIAL_2);
                me->DespawnOrUnsummon(6 * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_GIFT_OF_HELLSCREAM)
                {
                    me->SetPhaseMask(2, true);
                    me->HandleEmoteStateCommand(EMOTE_STATE_READY_UNARMED);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (allowFollow)
                    if (Unit* owner = ObjectAccessor::GetUnit(*me, summonerGUID))
                        me->GetMotionMaster()->MoveFollow(owner, 2.5f, me->GetAngle(owner));
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ji_firepaw_horde_way_questAI(creature);
        }
};

// Trial Challengers 62100, 62097, 62102
struct npc_trial_challengers : public ScriptedAI
{
    npc_trial_challengers(Creature* creature) : ScriptedAI(creature) { }

    ObjectGuid ownerGUID;
    uint32 delay;
    float x, y;

    void IsSummonedBy(Unit* summoner) override
    {
        ownerGUID = summoner->GetGUID();
        x = 0.0f; y = 0.0f;
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
        delay = 0;
    }

    // Idk how it should work? 8.4m HP...
    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        damage *= 10000;
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_INIT_TRIAL)
        {
            if (Unit* owner = ObjectAccessor::GetUnit(*me, ownerGUID))
            {
                GetPositionWithDistInOrientation(owner, 25.0f, owner->GetOrientation(), x, y);
                DoCast(me, SPELL_VISUAL_TELEPORT);
                me->NearTeleportTo(x + frand(-10.0f, 10.0f), y, 86.77f, owner->GetOrientation());

                me->m_Events.Schedule(delay += 2000, 1, [this]()
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
                });
            }
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* owner = ObjectAccessor::GetCreature(*me, ownerGUID))
            owner->AI()->DoAction(ACTION_CREAURE_SLAIN);

    }
};

// Watershed Patrol 3193, 39325, 39326, 39324
class npc_watershed_patrol : public CreatureScript
{
    public:
        npc_watershed_patrol() : CreatureScript("npc_watershed_patrol") { }

        bool OnQuestReward(Player* player, Creature* creature, const Quest *_Quest, uint32 /*slot*/) override
        {
            if (player->GetQuestStatus(QUEST_WATERSHED_PATROL) != QUEST_STATUS_INCOMPLETE)
                return false;

            for (auto iType : watershedMatchType)
            {
                if (_Quest->GetQuestId() == iType.first)
                {
                    player->KilledMonsterCredit(iType.second);

                    if (player->GetQuestStatus(QUEST_WATERSHED_PATROL) == QUEST_STATUS_INCOMPLETE)
                        player->CompleteQuest(QUEST_WATERSHED_PATROL);

                    return true;
                }
            }

            return true;
        }

        struct npc_watershed_patrolAI : public ScriptedAI
        {
            npc_watershed_patrolAI(Creature* creature) : ScriptedAI(creature) { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_watershed_patrolAI(creature);
        }
};

const Position questPos[6] =
{
    // Jailor 1
   { -1158.725464f, -5520.14746f, 12.086f, 0.0f },
   { -1153.468384f, -5519.44726f, 11.987f, 0.0f },
   // Jailor 2
   { -1137.682007f, -5428.35058f, 13.705f, 0.0f },
   { -1136.194702f, -5417.04003f, 13.269f, 0.0f },
   // Scout 1
   { -1150.157837f, -5526.10009f, 8.1050f, 0.0f },
   // Scout 2
   { -1142.019531f, -5415.30566f, 10.597f, 0.0f },
};

enum ProvingPit
{
    DATA_START_EVENT = 1,
    DATA_END_EVENT   = 1,

    NPC_JAILOR       = 39062,
    NPC_SCOUT        = 38142,
};

struct npc_darkspear_jailor : public ScriptedAI
{
    npc_darkspear_jailor(Creature* creature) : ScriptedAI(creature) { }

    bool firstJailor = false;

    bool OnGossipSelect(Player* player, uint32 /*sender*/, uint32 /*action*/) override
    {
        player->CLOSE_GOSSIP_MENU();
        player->KilledMonsterCredit(NPC_JAILOR);

        if (!me->FindNearestCreature(NPC_SCOUT, 30.0f)) // not ready yet
            return false;

        firstJailor = false;
        if (me->FindNearestCreature(38243, 30.0f, true))
            firstJailor = true;

        Talk(0, player);
        me->SetWalk(false);
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        me->GetMotionMaster()->MovePoint(1, questPos[firstJailor ? 0 : 2]);
        return true;
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        me->m_Events.Schedule(1000, [this, pointId]()
        {
            switch (pointId)
            {
                case 1:
                    me->GetMotionMaster()->MovePoint(2, questPos[firstJailor ? 1 : 3]);
                    break;
                case 2:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_USE_STANDING);
                    if (Creature* scout = me->FindNearestCreature(NPC_SCOUT, 30.0f))
                        scout->AI()->SetData(DATA_START_EVENT, firstJailor);
                    me->m_Events.Schedule(2000, [this]() { me->GetMotionMaster()->MovePoint(3, questPos[firstJailor ? 0 : 2]); });
                    break;
                case 3:
                    me->GetMotionMaster()->MovePoint(4, me->GetHomePosition());
                    break;
                case 4:
                    me->SetFacingTo(me->GetHomePosition().GetOrientation());
                    break;
            }
        });
    }

    void SetData(uint32 type, uint32 data) override
    {
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }
};

struct npc_captive_pitescale_scout : public ScriptedAI
{
    npc_captive_pitescale_scout(Creature* creature) : ScriptedAI(creature) { }

    void JustAppeared() override
    {
        if (GameObject* caje = me->FindNearestGameObject(201968, 30.0f))
            caje->ResetDoorOrButton();
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
        if (Creature* jailor = me->FindNearestCreature(NPC_JAILOR, 30.0f))
            jailor->AI()->SetData(DATA_END_EVENT, 0);
    }

    void JustReachedHome() override
    {
        if (GameObject* caje = me->FindNearestGameObject(201968, 30.0f))
            caje->ResetDoorOrButton();
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
        if (Creature* jailor = me->FindNearestCreature(NPC_JAILOR, 30.0f))
            jailor->AI()->SetData(DATA_END_EVENT, 0);
    }

    void SetData(uint32 data, uint32 type) override
    {
        if (GameObject* caje = me->FindNearestGameObject(201968, 30.0f))
            caje->UseDoorOrButton();
        me->GetMotionMaster()->MovePoint(1, questPos[type ? 4 : 5]);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        me->m_Events.Schedule(1000, [this, pointId]()
        {
            switch (pointId)
            {
                case 1:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    Talk(0);
                    if (Player* player = me->FindNearestPlayer(30.0f))
                        AttackStart(player);
                    break;
            }
        });
    }
};

enum VoodooSpells
{
    SPELL_BREW      = 16712, // Special Brew
    SPELL_GHOSTLY   = 16713, // Ghostly
    SPELL_HEX1      = 16707, // Hex
    SPELL_HEX2      = 16708, // Hex
    SPELL_HEX3      = 16709, // Hex
    SPELL_GROW      = 16711, // Grow
    SPELL_LAUNCH    = 16716, // Launch (Whee!)
};

// 17009
class spell_voodoo : public SpellScriptLoader
{
    public:
        spell_voodoo() : SpellScriptLoader("spell_voodoo") { }

        class spell_voodoo_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_voodoo_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BREW) || !sSpellMgr->GetSpellInfo(SPELL_GHOSTLY) ||
                    !sSpellMgr->GetSpellInfo(SPELL_HEX1) || !sSpellMgr->GetSpellInfo(SPELL_HEX2) ||
                    !sSpellMgr->GetSpellInfo(SPELL_HEX3) || !sSpellMgr->GetSpellInfo(SPELL_GROW) ||
                    !sSpellMgr->GetSpellInfo(SPELL_LAUNCH))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                uint32 spellid = RAND(SPELL_BREW, SPELL_GHOSTLY, RAND(SPELL_HEX1, SPELL_HEX2, SPELL_HEX3), SPELL_GROW, SPELL_LAUNCH);
                if (Unit* target = GetHitUnit())
                    GetCaster()->CastSpell(target, spellid, false);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_voodoo_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_voodoo_SpellScript();
        }
};

// Summon Garrosh horde way 120753
class spell_summ_garrosh_horde_way : public SpellScript
{
    PrepareSpellScript(spell_summ_garrosh_horde_way);

    void HandleAfterCast()
    {
        if (Player* pCaster = GetCaster()->ToPlayer())
        {
            pCaster->SetPhaseMask(3, true);
            pCaster->CastSpell(pCaster, SPELL_SUMMON_JI, true);
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_summ_garrosh_horde_way::HandleAfterCast);
    }
};

// Summon Ji horde way 120749
class spell_summ_ji_horde_way : public AuraScript
{
    PrepareAuraScript(spell_summ_ji_horde_way);

    void Remove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* pCaster = GetCaster()->ToPlayer())
        {
            pCaster->KilledMonsterCredit(NPC_GARROSH_HORDE_WAY_CREDIT);
            
            uint32 delay = 0;
            pCaster->m_Events.Schedule(delay += 3500, 1, [pCaster]()
            {
                pCaster->SetPhaseMask(1, true);
            });
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_summ_ji_horde_way::Remove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Place Territorial Fetish 72070
class spell_place_territorial_fetish : public SpellScript
{
    PrepareSpellScript(spell_place_territorial_fetish);

    SpellCastResult CheckCondition()
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            if (Creature* creature = player->FindNearestCreature(38560, 50.0f))
            {
                if (creature->HasAura(72072))
                    return SPELL_FAILED_BAD_TARGETS;
            }
            else
                return SPELL_FAILED_BAD_TARGETS;
        }

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_place_territorial_fetish::CheckCondition);
    }
};

enum ThonksSpyglassSpells
{
    SPELL_TELESCOPE_1        = 73741,
    SPELL_TELESCOPE_2        = 73763,
    SPELL_TELESCOPE_3        = 73764,
    SPELL_TELESCOPE_4        = 73765,

    OBJECTIVE_RAGGARAN       = 256157,
    OBJECTIVE_FLOODED_HUT    = 256158,
    OBJECTIVE_MISHA          = 256159,
    OBJECTIVE_ZEN_TAJI       = 256160,
};

// Thonk's Spyglass 73817
class spell_thonks_spyglass : public SpellScript
{
    PrepareSpellScript(spell_thonks_spyglass);

    void HandleAfterCast()
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_LOST_IN_THE_FLOODS) == QUEST_STATUS_INCOMPLETE && !player->GetQuestObjectiveCounter(OBJECTIVE_RAGGARAN))
            {
                player->CastSpell(player, SPELL_TELESCOPE_1);
                return;
            }
            else if (player->GetQuestStatus(QUEST_LOST_IN_THE_FLOODS) == QUEST_STATUS_INCOMPLETE && !player->GetQuestObjectiveCounter(OBJECTIVE_FLOODED_HUT))
            {
                player->CastSpell(player, SPELL_TELESCOPE_2);
                return;
            }
            else if (player->GetQuestStatus(QUEST_LOST_IN_THE_FLOODS) == QUEST_STATUS_INCOMPLETE && !player->GetQuestObjectiveCounter(OBJECTIVE_MISHA))
            {
                player->CastSpell(player, SPELL_TELESCOPE_3);
                return;
            }
            else if (player->GetQuestStatus(QUEST_LOST_IN_THE_FLOODS) == QUEST_STATUS_INCOMPLETE && !player->GetQuestObjectiveCounter(OBJECTIVE_ZEN_TAJI))
            {
                player->CastSpell(player, SPELL_TELESCOPE_4);
                return;
            }
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_thonks_spyglass::HandleAfterCast);
    }
};

// Shrink 72249
class spell_shrink : public SpellScript
{
    PrepareSpellScript(spell_shrink);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* obj) { return obj->GetEntry() != 38300 && obj->GetEntry() != 38301; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_shrink::FilterTargets, EFFECT_ALL, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Teleport Out Blackout 121123
class spell_teleport_out_blackout : public AuraScript
{
    PrepareAuraScript(spell_teleport_out_blackout);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            owner->KilledMonsterCredit(NPC_ARENA_CREDIT);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            owner->TeleportTo(1, LeaveRingOfValor.GetPositionX(), LeaveRingOfValor.GetPositionY(), LeaveRingOfValor.GetPositionZ(), LeaveRingOfValor.GetOrientation(), TELE_TO_NOT_UNSUMMON_PET);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_teleport_out_blackout::OnApply, EFFECT_1, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_teleport_out_blackout::OnRemove, EFFECT_1, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
    }
};

// Areatrigger 8730
class AreaTrigger_at_ring_of_valor_entrance : public AreaTriggerScript
{
    public:
        AreaTrigger_at_ring_of_valor_entrance() : AreaTriggerScript("AreaTrigger_at_ring_of_valor_entrance") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (player->GetQuestStatus(QUEST_GIFT_OF_HELLSCREAM) == QUEST_STATUS_INCOMPLETE && player->GetPhaseMask() != 6)
            {
                player->SetPhaseMask(6, true);

                player->SummonCreature(NPC_GARROSH_HORDE_WAY, GarroshArenaPos, TEMPSUMMON_MANUAL_DESPAWN);
                player->SummonCreature(NPC_JI_FIREPAWN_HORDE_WAY, RingOfValor.GetPositionX() + frand(-5.0f, 5.0f), RingOfValor.GetPositionY() + frand(-2.0f, 4.0f), RingOfValor.GetPositionZ(), RingOfValor.GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
                return true;
            }
            return false;
    }
};

void AddSC_durotar()
{
    new npc_lazy_peon();
    new npc_garrosh_hellscream_warchief();
    new npc_garrosh_horde_way_quest();
    new npc_ji_firepaw_horde_way_quest();
    new creature_script<npc_trial_challengers>("npc_trial_challengers");
    new npc_watershed_patrol();
    new creature_script<npc_darkspear_jailor>("npc_darkspear_jailor");
    new creature_script<npc_captive_pitescale_scout>("npc_captive_pitescale_scout");
    new spell_voodoo();
    new spell_script<spell_summ_garrosh_horde_way>("spell_summ_garrosh_horde_way");
    new aura_script<spell_summ_ji_horde_way>("spell_summ_ji_horde_way");
    new spell_script<spell_place_territorial_fetish>("spell_place_territorial_fetish");
    new spell_script<spell_thonks_spyglass>("spell_thonks_spyglass");
    new spell_script<spell_shrink>("spell_shrink");
    new aura_script<spell_teleport_out_blackout>("spell_teleport_out_blackout");
    new AreaTrigger_at_ring_of_valor_entrance();
}
