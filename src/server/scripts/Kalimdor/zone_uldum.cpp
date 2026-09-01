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

#include "ScriptPCH.h"

enum Spells
{
    // Akma'hat
    SPELL_STONE_MANTLE              = 93561,
    SPELL_STONE_MANTLE_BUFF         = 94965,
    SPELL_SHOCKWAVE                 = 94968,
    SPELL_FURY_OF_THE_SANDS         = 94946,
    SPELL_FURY_OF_THE_SANDS_DUMMY   = 94941,
    SPELL_FURY_OF_THE_SANDS_SUMMON  = 93574,
    SPELL_SANDS_OF_TIME             = 93578,
    SPELL_TELEPORT_TO_RAMKAHEN      = 94564,
    SPELL_TAHET_KILL_CREDIT         = 86747,
};

enum Events
{
    // Akma'hat
    EVENT_SHOCKWAVE         = 1,
    EVENT_FURY_OF_THE_SANDS = 2,
    EVENT_STONE_MANTLE      = 3,
    EVENT_SANDS_OF_TIME     = 4,
};

enum CreatureIds
{
    NPC_AKMAHAT              = 50063,
    NPC_FURY_OF_THE_SANDS    = 51293,
    NPC_SUN_PRIEST_ASARIS    = 47715,
    NPC_ASAQ                 = 47930,
    NPC_NORMACH              = 46603,
    NPC_ORSIS_SURVIVOR       = 45715,
    NPC_COLOSSUS_OF_THE_MOON = 46042,
    NPC_COLOSSUS_OF_THE_SUN  = 46041,
    NPC_RAMKAHEN_PRISONER    = 46425,
    NPC_NEFERSET_ENFORCER    = 46432,
    NPC_NEFERSET_OVERLORD    = 46441,
    NPC_CAIMAS_PIT_MASTER    = 46276,
    NPC_TAHET                = 46496,
    NPC_GOREBITE             = 46278,
    NPC_THARTEP              = 46280,
    NPC_KHAMEN               = 46281,
};

enum Quests
{
    QUEST_EASY_MONEY                = 27003,
    QUEST_TRAITORS                  = 27922,
    QUEST_ESCAPE_FROM_THE_LOST_CITY = 28112,
    QUEST_IMPENDING_RETRIBUTION     = 28134,
    QUEST_COLOSSAL_GUARDIANS        = 27623,
    QUEST_NEFERSET_PRISON           = 27707,
    QUEST_THE_PIT_OF_SCALES         = 27738,
};

enum Credits
{
    CREDIT_SUN_PRIEST_ASARIS    = 47933,
    CREDIT_CONVERSATION_ASAQ    = 47936,
    CREDIT_CONVERSATION_NOMARCH = 47932,
    CREDIT_RAMKAHEN_PRISONER    = 46461,
};

enum Actions
{
    ACTION_COLOSSUS,
    ACTION_RAMKAHEN_PRISONER,
    ACTION_GOREBITE_DIED,
    ACTION_KROKOLISKS_DIED,
};

enum Talks
{
    TALK_INTRO,
    TALK_SPECIAL_1,
    TALK_SPECIAL_2,
    TALK_SPECIAL_3,
};

enum Types
{
    TYPE_NEFERSET_PRISON_PLAYER,
    TYPE_PIT_MASTER_PLAYER,
};

const std::map<uint32, uint32> CreditMatchType =
{
    { NPC_SUN_PRIEST_ASARIS, CREDIT_SUN_PRIEST_ASARIS },
    { NPC_ASAQ,              CREDIT_CONVERSATION_ASAQ },
    { NPC_NORMACH,           CREDIT_CONVERSATION_NOMARCH },
};

const Position RamkahenPrisonerEvent[3]
{
    { -11082.54f, -1941.22f, 4.06f, 4.56f },
    { -11105.21f, -1963.67f, 3.60f, 6.23f },
    { -11106.89f, -1937.87f, 2.46f, 5.49f },
};

const Position PitMasterPath[3]
{
    { -11449.53f, -1152.56f,  3.74f, 4.69f },
    { -11449.70f, -1165.49f,  4.04f, 4.69f },
    { -11450.00f, -1181.15f, -2.64f, 4.78f }, // jump
};

const Position KrokoSpawnPoints[3]
{
    { -11450.98f, -1211.39f, 1.07f, 1.53f}, // gorebite
    { -11424.02f, -1183.38f, 1.07f, 3.07f },
    { -11482.53f, -1167.26f, 1.03f, 5.35f },
};

class boss_akmahat : public CreatureScript
{
    public:
        boss_akmahat() : CreatureScript("boss_akmahat") { }

        struct boss_akmahatAI : public ScriptedAI
        {
            boss_akmahatAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->setActive(true);
            }

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SHOCKWAVE, 16000);
                events.ScheduleEvent(EVENT_FURY_OF_THE_SANDS, 9000);
                events.ScheduleEvent(EVENT_SANDS_OF_TIME, urand(20000, 30000));
                events.ScheduleEvent(EVENT_STONE_MANTLE, 22000);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }
            
            void JustDied(Unit* /*killer*/) override
            {
                events.Reset();
                summons.DespawnAll();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;
                
                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                
                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_STONE_MANTLE:
                            if (!me->HasAura(SPELL_STONE_MANTLE))
                            {
                                DoCast(me, SPELL_STONE_MANTLE);
                                events.ScheduleEvent(EVENT_STONE_MANTLE, urand(43000, 45000));
                            }
                            else
                                events.ScheduleEvent(EVENT_STONE_MANTLE, urand(10000, 15000));
                            break;
                        case EVENT_SHOCKWAVE:
                            DoCastVictim(SPELL_SHOCKWAVE);
                            events.ScheduleEvent(EVENT_SHOCKWAVE, 16000);
                            break;
                        case EVENT_FURY_OF_THE_SANDS:
                            for (uint8 i = 0; i < 9; ++i)
                                DoCast(me, SPELL_FURY_OF_THE_SANDS_SUMMON, true);
                            DoCast(me, SPELL_FURY_OF_THE_SANDS);
                            events.ScheduleEvent(EVENT_FURY_OF_THE_SANDS, 16000);
                            break;
                        case EVENT_SANDS_OF_TIME:
                            DoCastAOE(SPELL_SANDS_OF_TIME);
                            events.ScheduleEvent(EVENT_SANDS_OF_TIME, 25000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        private:
            EventMap events;
            SummonList summons;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_akmahatAI(creature);
        }
};  

class npc_akmahat_fury_of_the_sands : public CreatureScript
{
    public:
        npc_akmahat_fury_of_the_sands() : CreatureScript("npc_akmahat_fury_of_the_sands") { }

        struct npc_akmahat_fury_of_the_sandsAI : public ScriptedAI
        {
            npc_akmahat_fury_of_the_sandsAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                DoCast(me, SPELL_FURY_OF_THE_SANDS_DUMMY, true);
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_FURY_OF_THE_SANDS)
                    me->DespawnOrUnsummon(1000);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_akmahat_fury_of_the_sandsAI(creature);
        }
};

// Addarah 44833
class npc_addarah : public CreatureScript
{
    public:
        npc_addarah() : CreatureScript("npc_addarah") { }

        bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
        {
            player->CastSpell(player, 89328, true);

            return false;
        }
};

// Prince Nadun 46872
class npc_prince_nadun : public CreatureScript
{
    public:
        npc_prince_nadun() : CreatureScript("npc_prince_nadun") { }

        bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_ESCAPE_FROM_THE_LOST_CITY)
                player->CastSpell(player, SPELL_TELEPORT_TO_RAMKAHEN, true);

            return false;
        }
};

// Quest Impending Retribution 47715, 47930, 46603
class npc_quest_impending_retribution : public CreatureScript
{
    public:
        npc_quest_impending_retribution() : CreatureScript("npc_quest_impending_retribution") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
                player->KilledMonsterCredit(CreditMatchType.find(creature->GetEntry())->second);

            player->CLOSE_GOSSIP_MENU();

            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if (player->GetQuestStatus(QUEST_IMPENDING_RETRIBUTION) != QUEST_STATUS_INCOMPLETE)
                return false;

            switch (creature->GetEntry())
            {
                case NPC_SUN_PRIEST_ASARIS:
                case NPC_ASAQ:
                case NPC_NORMACH:
                    player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                    break;
            }

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }
};

// Sun Moon Colossus 46042, 46041
class npc_sun_moon_colossus : public CreatureScript
{
    public:
        npc_sun_moon_colossus() : CreatureScript("npc_sun_moon_colossus") { }

        struct npc_sun_moon_colossusAI : public ScriptedAI
        {
            npc_sun_moon_colossusAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
                me->SetFaction(35);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_COLOSSUS)
                {
                    me->ExitVehicle();
                    me->HandleEmoteStateCommand(EMOTE_STATE_NONE);
                    me->SetFaction(14);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sun_moon_colossusAI(creature);
        }
};

// Ramkahen Prisoner 46425
class npc_quest_ramkahen_prisoner : public CreatureScript
{
    public:
        npc_quest_ramkahen_prisoner() : CreatureScript("npc_quest_ramkahen_prisoner") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                if (TempSummon* sPrisoner = player->SummonCreature(NPC_RAMKAHEN_PRISONER, *creature, TEMPSUMMON_MANUAL_DESPAWN))
                    sPrisoner->SetPhaseMask(QUEST_NEFERSET_PRISON + 1, true);

                player->SetPhaseMask(QUEST_NEFERSET_PRISON + 1, true);
            }

            player->CLOSE_GOSSIP_MENU();

            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if (player->GetQuestStatus(QUEST_NEFERSET_PRISON) != QUEST_STATUS_INCOMPLETE)
                return false;

            player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        struct npc_quest_ramkahen_prisonerAI : public ScriptedAI
        {
            npc_quest_ramkahen_prisonerAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid ownerGUID;
            uint32 delay = 0;

            void IsSummonedBy(Unit* summoner) override
            {
                ownerGUID = summoner->GetGUID();
                Talk(TALK_INTRO);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                delay = 0;
                me->m_Events.Schedule(delay += 2000, 1, [this]()
                {
                    HandleSummonGuardians();
                });

                me->m_Events.Schedule(delay += urand(20 * IN_MILLISECONDS, 22 * IN_MILLISECONDS), 2, [this]()
                {
                    HandleSummonGuardians();
                });

                me->m_Events.Schedule(delay += urand(20 * IN_MILLISECONDS, 22 * IN_MILLISECONDS), 3, [this]()
                {
                    HandleSummonGuardians(true);
                });
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_NEFERSET_PRISON_PLAYER)
                    return ownerGUID;

                return 0;
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_RAMKAHEN_PRISONER)
                {
                    Talk(TALK_SPECIAL_1);

                    if (Player* pPlayer = ObjectAccessor::GetPlayer(*me, ownerGUID))
                    {
                        pPlayer->KilledMonsterCredit(CREDIT_RAMKAHEN_PRISONER);

                        delay = 0;
                        me->m_Events.Schedule(delay += 5000, 1, [this, pPlayer]()
                        {
                            pPlayer->SetPhaseMask(1, true);
                            me->DespawnOrUnsummon();
                        });
                    }

                }
            }

            void UpdateAI(uint32 /*diff*/) override { }

            private:
                void HandleSummonGuardians(bool boss = false)
                {
                    if (Player* pPlayer = ObjectAccessor::GetPlayer(*me, ownerGUID))
                        for (uint8 i = 0; i < 2; ++i)
                            pPlayer->SummonCreature(NPC_NEFERSET_ENFORCER, RamkahenPrisonerEvent[i], TEMPSUMMON_MANUAL_DESPAWN);

                    if (boss)
                        me->SummonCreature(NPC_NEFERSET_OVERLORD, RamkahenPrisonerEvent[2], TEMPSUMMON_MANUAL_DESPAWN);
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_quest_ramkahen_prisonerAI(creature);
        }
};

// Neferset Enforcer Quest 46432, 46441
class npc_neferset_enforcer_quest : public CreatureScript
{
    public:
        npc_neferset_enforcer_quest() : CreatureScript("npc_neferset_enforcer_quest") { }

        struct npc_neferset_enforcer_questAI : public ScriptedAI
        {
            npc_neferset_enforcer_questAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid prisonerGUID;

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetPhaseMask(QUEST_NEFERSET_PRISON + 1, true);

                if (me->GetEntry() == NPC_NEFERSET_OVERLORD)
                {
                    prisonerGUID = summoner->GetGUID();

                    if (uint32 targetLOW = summoner->ToCreature()->AI()->GetData(TYPE_NEFERSET_PRISON_PLAYER))
                    {
                        if (Player* pPlayer = ObjectAccessor::FindPlayer(ObjectGuid(HighGuid::Player, targetLOW)))
                        {
                            me->GetMotionMaster()->MoveChase(pPlayer);
                            me->Attack(pPlayer, true);
                        }
                    }
                }
                else
                {
                    me->GetMotionMaster()->MoveChase(summoner);
                    me->Attack(summoner, true);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (me->GetEntry() == NPC_NEFERSET_OVERLORD)
                    if (Creature* prisoner = ObjectAccessor::GetCreature(*me, prisonerGUID))
                        prisoner->AI()->DoAction(ACTION_RAMKAHEN_PRISONER);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_neferset_enforcer_questAI(creature);
        }
};

// Tahet 46496
class npc_tahet : public CreatureScript
{
    public:
        npc_tahet() : CreatureScript("npc_tahet") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if (player->GetQuestStatus(QUEST_THE_PIT_OF_SCALES) != QUEST_STATUS_INCOMPLETE)
                return false;

            player->SetPhaseMask(QUEST_THE_PIT_OF_SCALES, true);

            if (TempSummon* Tahet = player->SummonCreature(NPC_TAHET, *creature, TEMPSUMMON_MANUAL_DESPAWN))
            {
                Tahet->SetPhaseMask(QUEST_THE_PIT_OF_SCALES, true);

                uint32 delay = 0;
                creature->m_Events.Schedule(delay += 1000, 1, [creature, player, Tahet]()
                {
                    Tahet->Kill(Tahet);

                    if (TempSummon* PitMaster = player->SummonCreature(NPC_CAIMAS_PIT_MASTER, PitMasterPath[0], TEMPSUMMON_MANUAL_DESPAWN))
                        PitMaster->SetPhaseMask(QUEST_THE_PIT_OF_SCALES, true);
                });
                
            }

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }
};

// Caimat Pit Master 46276
class npc_caimat_pit_master : public CreatureScript
{
    public:
        npc_caimat_pit_master() : CreatureScript("npc_caimat_pit_master") { }

        struct npc_caimat_pit_masterAI : public ScriptedAI
        {
            npc_caimat_pit_masterAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid playerGUID;
            uint32 delay, krokoCount;

            void IsSummonedBy(Unit* summoner) override
            {
                playerGUID = summoner->GetGUID();
                krokoCount = 0;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);

                Movement::MoveSplineInit init(me);
                for (uint8 i = 0; i < 2; i++)
                    init.Path().push_back(G3D::Vector3(PitMasterPath[i].GetPositionX(), PitMasterPath[i].GetPositionY(), PitMasterPath[i].GetPositionZ()));

                init.SetSmooth();
                init.SetUncompressed();
                init.Launch();

                delay = 0;
                me->m_Events.Schedule(delay += me->GetSplineDuration(), 1, [this]()
                {
                    Talk(TALK_INTRO);
                });

                me->m_Events.Schedule(delay += 3500, 1, [this]()
                {
                    Talk(TALK_SPECIAL_1);

                    if (Creature* gorebite = me->SummonCreature(NPC_GOREBITE, KrokoSpawnPoints[0], TEMPSUMMON_MANUAL_DESPAWN))
                        gorebite->SetPhaseMask(QUEST_THE_PIT_OF_SCALES, true);
                });
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_PIT_MASTER_PLAYER)
                    return playerGUID;

                return 0;
            }

            void DoAction(int32 actionId) override
            {

                switch (actionId)
                {
                    case ACTION_GOREBITE_DIED:
                        Talk(TALK_SPECIAL_2);

                        delay = 0;
                        me->m_Events.Schedule(delay += 5000, 1, [this]()
                        {
                            for (uint8 i = 1; i < 3; i++)
                                if (Creature* kroko = me->SummonCreature(i == 1 ? NPC_THARTEP : NPC_KHAMEN, KrokoSpawnPoints[i], TEMPSUMMON_MANUAL_DESPAWN))
                                    kroko->SetPhaseMask(QUEST_THE_PIT_OF_SCALES, true);
                        });
                        break;
                    case ACTION_KROKOLISKS_DIED:
                        if (++krokoCount >= 2)
                            me->GetMotionMaster()->MoveJump(PitMasterPath[2], 20.0f, 10.0f, EVENT_JUMP);

                        break;
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == EVENT_JUMP)
                {
                    if (Player* pPlayer = ObjectAccessor::GetPlayer(*me, playerGUID))
                    {
                        Talk(TALK_SPECIAL_3);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                        me->GetMotionMaster()->MoveChase(pPlayer);
                        me->Attack(pPlayer, true);
                    }
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                DoCast(me, SPELL_TAHET_KILL_CREDIT);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_caimat_pit_masterAI(creature);
        }
};

// Kroko 46278, 46280, 46281
class npc_krokolisks_quest : public CreatureScript
{
    public:
        npc_krokolisks_quest() : CreatureScript("npc_krokolisks_quest") { }

        struct npc_krokolisks_questAI : public ScriptedAI
        {
            npc_krokolisks_questAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid summonerGUID;

            void IsSummonedBy(Unit* summoner) override
            {
                summonerGUID = summoner->GetGUID();

                if (uint32 targetLOW = summoner->ToCreature()->AI()->GetData(TYPE_PIT_MASTER_PLAYER))
                {
                    if (Player* pPlayer = ObjectAccessor::FindPlayer(ObjectGuid(HighGuid::Player, targetLOW)))
                    {
                        me->GetMotionMaster()->MoveChase(pPlayer);
                        me->Attack(pPlayer, true);
                    }
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* caimat = ObjectAccessor::GetCreature(*me, summonerGUID))
                    caimat->AI()->DoAction(me->GetEntry() == NPC_GOREBITE ? ACTION_GOREBITE_DIED : ACTION_KROKOLISKS_DIED);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_krokolisks_questAI(creature);
        }
};

// Neferset Frond 206579
class go_neferset_frond : public GameObjectScript
{
    public:
        go_neferset_frond() : GameObjectScript("go_neferset_frond") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (player && player->GetQuestStatus(QUEST_TRAITORS) == QUEST_STATUS_INCOMPLETE)
                player->CompleteQuest(QUEST_TRAITORS);

            return true;
        }
};

enum DormusEnums
{
    NPC_DORMUS                  = 50245,
    SPELL_SANDSTORM_TELEPORT    = 93471,
};

Position const dormusSummonPos = { -5753.08f, 601.24f, 165.94f, 1.456f };

class spell_dormus_rage : public SpellScriptLoader
{
    public:
        spell_dormus_rage() : SpellScriptLoader("spell_dormus_rage") { }

        class spell_dormus_rage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dormus_rage_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (!caster || !caster->IsInWorld())
                    return;

                caster->GetMap()->LoadGrid(dormusSummonPos.GetPositionX(), dormusSummonPos.GetPositionY());
                caster->SummonCreature(NPC_DORMUS, dormusSummonPos, TEMPSUMMON_TIMED_DESPAWN, 1200000ms);
                caster->CastSpell(caster, SPELL_SANDSTORM_TELEPORT, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dormus_rage_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dormus_rage_SpellScript();
        }
};

class spell_sandstorm : public SpellScriptLoader
{
    public:
        spell_sandstorm() : SpellScriptLoader("spell_sandstorm") { }

        class spell_sandstorm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sandstorm_SpellScript);

            void HandleExtraEffect()
            {
                if (GetCaster() && GetCaster()->ToCreature())
                    GetCaster()->ToCreature()->DisappearAndDie();
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_sandstorm_SpellScript::HandleExtraEffect);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sandstorm_SpellScript();
        }
};

class spell_transform_camel_dust_pile : public SpellScriptLoader
{
    public:
        spell_transform_camel_dust_pile() : SpellScriptLoader("spell_transform_camel_dust_pile") { }

        class spell_transform_camel_dust_pile_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_transform_camel_dust_pile_SpellScript);

            void HandleExtraEffect()
            {
                Unit* caster = GetCaster();
                Unit* target = GetOriginalCaster();
                if (!caster || caster->GetTypeId() != TYPEID_UNIT || !target || target->GetTypeId() != TYPEID_PLAYER)
                    return;

                caster->ToCreature()->SetLootRecipient(target);
                caster->ToCreature()->LowerPlayerDamageReq(caster->GetMaxHealth());
                target->Kill(caster);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_transform_camel_dust_pile_SpellScript::HandleExtraEffect);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_transform_camel_dust_pile_SpellScript();
        }
};

const Position uldumIntroPos = { -10994.88f, -1255.51f, 13.24f, 4.65f };

// Addarah Breakout Cage Master 89328
class spell_addarah_breakout_cage_master : public SpellScript
{
    PrepareSpellScript(spell_addarah_breakout_cage_master);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            caster->CompleteQuest(QUEST_EASY_MONEY);
            caster->TeleportTo(1, uldumIntroPos.GetPositionX(), uldumIntroPos.GetPositionY(), uldumIntroPos.GetPositionZ(), uldumIntroPos.GetOrientation(), TELE_TO_NOT_UNSUMMON_PET);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_addarah_breakout_cage_master::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Rescue Survivor 85372
class spell_rescue_survivor_quest : public SpellScript
{
    PrepareSpellScript(spell_rescue_survivor_quest);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->KilledMonsterCredit(NPC_ORSIS_SURVIVOR);

                if (target->ToCreature())
                    target->ToCreature()->DespawnOrUnsummon();
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_rescue_survivor_quest::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Thunder Clap 85840
class spell_thunder_clap_quest : public SpellScript
{
    PrepareSpellScript(spell_thunder_clap_quest);

    void HandleAfterCast()
    {
        if (Unit* caster = GetCaster())
        {
            if (Creature* sunGuardian = caster->FindNearestCreature(NPC_COLOSSUS_OF_THE_SUN, 5.0f, true))
                sunGuardian->AI()->DoAction(ACTION_COLOSSUS);

            if (Creature* moonGuardian = caster->FindNearestCreature(NPC_COLOSSUS_OF_THE_MOON, 5.0f, true))
                moonGuardian->AI()->DoAction(ACTION_COLOSSUS);
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_thunder_clap_quest::HandleAfterCast);
    }
};

// Stealing 89185
class spell_stealing_quest : public SpellScript
{
    PrepareSpellScript(spell_stealing_quest);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                if (target->ToCreature())
                    target->ToCreature()->AI()->DoAction(ACTION_COLOSSUS);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_stealing_quest::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

enum LittlePluckersType
{
    NPC_PLUCKER_CREDIT = 48105,
    NPC_PYGME_THIEF    = 48043,
    NPC_PYGME_SCOUT    = 48041,
    NPC_PYGME_OAF      = 48040,
};

// Mallet Eff 89445
class spell_uldum_mallet_eff : public SpellScript
{
    PrepareSpellScript(spell_uldum_mallet_eff);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->Kill(target);
                caster->KilledMonsterCredit(NPC_PLUCKER_CREDIT);
            }
        }
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_PYGME_THIEF && target->GetEntry() != NPC_PYGME_SCOUT && target->GetEntry() != NPC_PYGME_OAF; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_uldum_mallet_eff::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_uldum_mallet_eff::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENTRY);
    }
};

void AddSC_uldum()
{
    new boss_akmahat();
    new npc_akmahat_fury_of_the_sands();
    new npc_addarah();
    new npc_prince_nadun();
    new npc_quest_impending_retribution();
    new npc_sun_moon_colossus();
    new npc_quest_ramkahen_prisoner();
    new npc_neferset_enforcer_quest();
    new npc_tahet();
    new npc_caimat_pit_master();
    new npc_krokolisks_quest();
    new go_neferset_frond();
    new spell_dormus_rage();
    new spell_sandstorm();
    new spell_transform_camel_dust_pile();
    new spell_script<spell_addarah_breakout_cage_master>("spell_addarah_breakout_cage_master");
    new spell_script<spell_rescue_survivor_quest>("spell_rescue_survivor_quest");
    new spell_script<spell_thunder_clap_quest>("spell_thunder_clap_quest");
    new spell_script<spell_stealing_quest>("spell_stealing_quest");
    new spell_script<spell_uldum_mallet_eff>("spell_uldum_mallet_eff");
}