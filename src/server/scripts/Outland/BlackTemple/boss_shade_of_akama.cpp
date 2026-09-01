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
SDName: Boss_Shade_of_Akama
SD%Complete: 90
SDComment: Seems to be complete.
SDCategory: Black Temple
EndScriptData */

#include "ScriptPCH.h"
#include "black_temple.h"

enum Says
{
    // Akama Ending cinematic text
    SAY_BROKEN_FREE_0  = 0,
    SAY_BROKEN_FREE_1  = 1,

    SAY_FREE           = 0,
    SAY_LOW_HEALTH     = 1,
    SAY_DEATH          = 2,
};

#define GOSSIP_ITEM 8654

struct Location
{
    float x, y, o, z;
};

/* Not used
static Location ChannelerLocations[]=
{
    { 463.161285f, 401.219757f, 3.141592f, 0.0f },
    { 457.377625f, 391.227661f, 2.106461f, 0.0f },
    { 446.012421f, 391.227661f, 1.071904f, 0.0f },
    { 439.533783f, 401.219757f, 0.000000f, 0.0f },
    { 446.012421f, 411.211853f, 5.210546f, 0.0f },
    { 457.377625f, 411.211853f, 4.177494f, 0.0f }
};*/

static Location SpawnLocations[]=
{
    { 498.652740f, 461.728119f, 0.0f, 0.0f },
    { 498.505003f, 339.619324f, 0.0f, 0.0f }
};

static Location AkamaWP[]=
{
    { 482.352448f, 401.162720f, 0.0f, 112.783928f },
    { 469.597443f, 402.264404f, 0.0f, 118.537910f }
};

static Location BrokenCoords[]=
{
    { 541.375916f, 401.439575f, M_PI,      112.783997f }, // The place where Akama channels
    { 534.130005f, 352.394531f, 2.164150f, 112.783737f }, // Behind a 'pillar' which is behind the east alcove
    { 499.621185f, 341.534729f, 1.652856f, 112.783730f }, // East Alcove
    { 499.151093f, 461.036438f, 4.770888f, 112.783700f }, // West Alcove
};

static Location BrokenWP[]=
{
    { 492.491638f, 400.744690f, 3.122336f, 112.783737f },
    { 494.335724f, 382.221771f, 2.676230f, 112.783737f },
    { 489.555939f, 373.507202f, 2.416263f, 112.783737f },
    { 491.136353f, 427.868774f, 3.519748f, 112.783737f },
};

// Spells
enum Spells
{
    SPELL_VERTEX_SHADE_BLACK   = 39833,
    SPELL_SHADE_SOUL_CHANNEL   = 40401,
    SPELL_DESTRUCTIVE_POISON   = 40874,
    SPELL_LIGHTNING_BOLT       = 42024,
    SPELL_AKAMA_SOUL_CHANNEL   = 40447,
    SPELL_AKAMA_SOUL_RETRIEVE  = 40902,
    AKAMA_SOUL_EXPEL           = 40855,
    SPELL_SHADE_SOUL_CHANNEL_2 = 40520,
};

enum Creatures
{
    NPC_ASHTONGUE_CHANNELER          = 23421,
    NPC_ASHTONGUE_SORCERER           = 23215,
    NPC_ASHTONGUE_DEFENDER           = 23216,
    NPC_ASHTONGUE_BROKEN             = 23319,
};

// Locations
#define Z1              118.543144f
#define Z2              120.783768f
#define Z_SPAWN         113.537949f
#define AGGRO_X         482.793182f
#define AGGRO_Y         401.270172f
#define AGGRO_Z         112.783928f
#define AKAMA_X         514.583984f
#define AKAMA_Y         400.601013f
#define AKAMA_Z         112.783997f

const uint32 spawnEntries[4]= { 23523, 23318, 23524 };

class npc_ashtongue_channeler : public CreatureScript
{
    public:
        npc_ashtongue_channeler() : CreatureScript("npc_ashtongue_channeler") { }

        struct npc_ashtongue_channelerAI : public ScriptedAI
        {
            npc_ashtongue_channelerAI(Creature* creature) : ScriptedAI(creature)
            {
                ShadeGUID = ObjectGuid::Empty;
            }

            ObjectGuid ShadeGUID;

            void Reset() override 
            {
                me->setActive(true);
            }

            void JustDied(Unit* /*killer*/) override;
            void JustEngagedWith(Unit* /*who*/) override { }
            void AttackStart(Unit* /*who*/) override { }
            void MoveInLineOfSight(Unit* /*who*/) override { }
            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ashtongue_channelerAI(creature);
        }
};

class npc_ashtongue_sorcerer : public CreatureScript
{
    public:
        npc_ashtongue_sorcerer() : CreatureScript("npc_ashtongue_sorcerer") { }

        struct npc_ashtongue_sorcererAI : public ScriptedAI
        {
            npc_ashtongue_sorcererAI(Creature* creature) : ScriptedAI(creature)
            {
                ShadeGUID = ObjectGuid::Empty;
            }

            ObjectGuid ShadeGUID;
            uint32 CheckTimer;
            bool StartBanishing;

            void Reset() override
            {
                StartBanishing = false;
                CheckTimer = 5000;
                me->setActive(true);
            }

            void JustDied(Unit* /*killer*/) override;
            void JustEngagedWith(Unit* /*who*/) override { }
            void AttackStart(Unit* /*who*/) override { }
            void MoveInLineOfSight(Unit* /*who*/) override { }
            void UpdateAI(uint32 diff) override
            {
                if (StartBanishing)
                    return;

                if (CheckTimer <= diff)
                {
                    Creature* Shade = Unit::GetCreature(*me, ShadeGUID);
                    if (Shade && Shade->IsAlive() && me->IsAlive())
                    {
                        if (me->IsWithinDist(Shade, 20, false))
                        {
                            me->GetMotionMaster()->Clear(false);
                            me->GetMotionMaster()->MoveIdle();
                            DoCast(Shade, SPELL_SHADE_SOUL_CHANNEL, true);
                            //DoCast(Shade, SPELL_SHADE_SOUL_CHANNEL_2, true);

                            StartBanishing = true;
                        }
                    }
                    CheckTimer = 2000;
                } else CheckTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ashtongue_sorcererAI(creature);
        }
};

class boss_shade_of_akama : public CreatureScript
{
    public:
        boss_shade_of_akama() : CreatureScript("boss_shade_of_akama") { }

        struct boss_shade_of_akamaAI : public ScriptedAI
        {
            boss_shade_of_akamaAI(Creature* creature) : ScriptedAI(creature), summons(me), HasKilledAkamaAndReseting(false)
            {
                instance = creature->GetInstanceScript();
                AkamaGUID = instance ? instance->GetGuidData(DATA_AKAMA_SHADE) : ObjectGuid::Empty;
                me->setActive(true); // if view distance is too low
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, true);
            }

            InstanceScript* instance;
            std::list<ObjectGuid> Channelers;
            std::list<ObjectGuid> Sorcerers;
            ObjectGuid AkamaGUID;
            uint32 SorcererCount;
            uint32 DeathCount;
            uint32 ReduceHealthTimer;
            uint32 SummonTimer;
            uint32 ResetTimer;
            uint32 DefenderTimer; // They are on a flat 15 second timer, independant of the other summon Creature timer.
            bool IsBanished;
            bool HasKilledAkama;
            bool reseting;
            bool GridSearcherSucceeded;
            bool HasKilledAkamaAndReseting;
            bool StartCombat;
            SummonList summons;

            void Reset() override
            {
                reseting = true;
                StartCombat = false;
                HasKilledAkamaAndReseting = false;

                GridSearcherSucceeded = false;

                Sorcerers.clear();
                summons.DespawnAll(); // despawn all adds

                if (Creature* Akama = Unit::GetCreature(*me, AkamaGUID))
                {
                    Akama->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP); // turn gossip on so players can restart the event
                    if (Akama->isDead())
                    {
                        Akama->Respawn(); // respawn akama if dead
                        Akama->AI()->EnterEvadeMode();
                    }
                }
                SorcererCount = 0;
                DeathCount = 0;

                SummonTimer = 10000;
                ReduceHealthTimer = 0;
                ResetTimer = 60000;
                DefenderTimer = 15000;

                IsBanished = true;
                HasKilledAkama = false;

                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                //me->GetMotionMaster()->Clear();
                //me->GetMotionMaster()->MoveIdle();
                me->HandleEmoteStateCommand(EMOTE_STATE_STUN);

                if (instance && me->IsAlive())
                    instance->SetData(DATA_SHADE_OF_AKAMA_EVENT, NOT_STARTED);

                reseting = false;
                me->setActive(true);
            }

            void JustDied(Unit* /*killer*/) override
            {
                summons.DespawnAll();
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_ASHTONGUE_DEFENDER || summon->GetEntry() == 23523 || summon->GetEntry() == 23318 || summon->GetEntry() == 23524)
                    summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_ASHTONGUE_DEFENDER || summon->GetEntry() == 23523 || summon->GetEntry() == 23318 || summon->GetEntry() == 23524)
                    summons.Despawn(summon);
            }

            void MoveInLineOfSight(Unit* /*who*/) override
            {
                if (!GridSearcherSucceeded)
                {
                    FindChannelers();

                    if (!Channelers.empty())
                    {
                        for (std::list<ObjectGuid>::const_iterator itr = Channelers.begin(); itr != Channelers.end(); ++itr)
                        {
                            Creature* Channeler = (Unit::GetCreature(*me, *itr));
                            if (Channeler)
                            {
                                if (Channeler->isDead())
                                {
                                    Channeler->RemoveCorpse();
                                    Channeler->Respawn();
                                    Channeler->InterruptNonMeleeSpells(true);
                                    Channeler->RemoveAurasDueToSpell(SPELL_SHADE_SOUL_CHANNEL);
                                }

                                if (Channeler->IsAlive())
                                {
                                Channeler->CastSpell(me, SPELL_SHADE_SOUL_CHANNEL, true);
                                //Channeler->CastSpell(me, SPELL_SHADE_SOUL_CHANNEL_2, true);  // this one triggered
                                Channeler->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                GridSearcherSucceeded = true;
                            }
                        }
                      }
                    } else TC_LOG_ERROR("misc", "SD2 ERROR: No Channelers are stored in the list. This encounter will not work properly");
                }
            }

            void JustEngagedWith(Unit* who) override { }

            void AttackStart(Unit* who) override
            {
                if (!who || IsBanished)
                    return;

                if (who->isTargetableForAttack() && who != me)
                    DoStartMovement(who);
            }

            void IncrementDeathCount(ObjectGuid guid = ObjectGuid::Empty)               // If guid is set, will remove it from list of sorcerer
            {
                if (reseting)
                    return;

                TC_LOG_DEBUG("misc", "TSCR: Increasing Death Count for Shade of Akama encounter");
                ++DeathCount;
                me->RemoveAuraFromStack(SPELL_SHADE_SOUL_CHANNEL_2);
                if (guid)
                {
                    if (Sorcerers.empty())
                        TC_LOG_ERROR("misc", "SD2 ERROR: Shade of Akama - attempt to remove guid " UI64FMTD " from Sorcerers list but list is already empty", guid.GetRawValue());
                    else
                        Sorcerers.remove(guid);
                }
            }

            void SummonCreature()
            {
                uint32 random = rand() % 2;
                float X = SpawnLocations[random].x;
                float Y = SpawnLocations[random].y;
                // max of 6 sorcerers can be summoned
                if ((rand() % 3 == 0) && (DeathCount > 0) && (SorcererCount < 7))
                {
                    Creature* sorcerer = me->SummonCreature(NPC_ASHTONGUE_SORCERER, X, Y, Z_SPAWN, 0, TEMPSUMMON_DEAD_DESPAWN, 0ms);
                    if (sorcerer)
                    {
                        CAST_AI(npc_ashtongue_sorcerer::npc_ashtongue_sorcererAI, sorcerer->AI())->ShadeGUID = me->GetGUID();
                        sorcerer->SetWalk(false);
                        sorcerer->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                        sorcerer->SetTarget(me->GetGUID());
                        Sorcerers.push_back(sorcerer->GetGUID());
                        --DeathCount;
                        ++SorcererCount;
                    }
                }
                //else
                //{
                for (uint8 pos = 0; pos < 2; ++pos)
                {
                    X = SpawnLocations[pos].x;
                    Y = SpawnLocations[pos].y;

                    for (uint8 i = 0; i < 3; ++i)
                    {
                        Creature* spawn = me->SummonCreature(spawnEntries[i], X, Y, Z_SPAWN, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000ms);
                        if (spawn)
                        {
                            spawn->SetWalk(false);
                            spawn->GetMotionMaster()->MovePoint(0, AGGRO_X, AGGRO_Y, AGGRO_Z);
                            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1);
                            spawn->AI()->AttackStart(target);
                            DoZoneInCombat(spawn);
                        }
                    }
                }
            }

            void FindChannelers()
            {
                std::list<Creature*> channelerList;
                me->GetCreatureListWithEntryInGrid(channelerList, NPC_ASHTONGUE_CHANNELER, 50.0f);

                if (!channelerList.empty())
                {
                    for (std::list<Creature*>::const_iterator itr = channelerList.begin(); itr != channelerList.end(); ++itr)
                    {
                        CAST_AI(npc_ashtongue_channeler::npc_ashtongue_channelerAI, (*itr)->AI())->ShadeGUID = me->GetGUID();
                        Channelers.push_back((*itr)->GetGUID());
                        TC_LOG_DEBUG("misc", "TSCR: Shade of Akama Grid Search found channeler " UI64FMTD ". Adding to list", (*itr)->GetGUID().GetRawValue());
                    }
                }
                else
                    TC_LOG_ERROR("misc", "SD2 ERROR: Grid Search was unable to find any channelers. Shade of Akama encounter will be buggy");
            }

            void SetSelectableChannelers()
            {
                if (Channelers.empty())
                {
                    TC_LOG_ERROR("misc", "SD2 ERROR: Channeler List is empty, Shade of Akama encounter will be buggy");
                    return;
                }

                for (std::list<ObjectGuid>::const_iterator itr = Channelers.begin(); itr != Channelers.end(); ++itr)
                    if (Creature* Channeler = (Unit::GetCreature(*me, *itr)))
                        Channeler->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void SetAkamaGUID(ObjectGuid guid) { AkamaGUID = guid; }

            void UpdateAI(uint32 diff) override
            {
                //if (!me->IsInCombat())
                if (!StartCombat)
                    return;

                if (IsBanished)
                {
                    // Akama is set in the threatlist so when we reset, we make sure that he is not included in our check
                    if (me->GetThreatManager().getThreatList().size() < 2)
                    {
                        EnterEvadeMode();
                        return;
                    }

                    if (DefenderTimer <= diff)
                    {
                        uint32 ran = rand() % 2;
                        Creature* defender = me->SummonCreature(NPC_ASHTONGUE_DEFENDER, SpawnLocations[ran].x, SpawnLocations[ran].y, Z_SPAWN, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 25000ms);
                        if (defender)
                        {
                            defender->SetWalk(false);
                            bool move = true;
                            if (AkamaGUID)
                            {
                                if (Creature* Akama = Unit::GetCreature(*me, AkamaGUID))
                                {
                                    float x, y, z;
                                    Akama->GetPosition(x,y,z);
                                    // They move towards AKama
                                    defender->GetMotionMaster()->MovePoint(0, x, y, z);
                                    defender->AI()->AttackStart(Akama);
                                } else move = false;
                            } else move = false;
                            if (!move)
                                defender->GetMotionMaster()->MovePoint(0, AKAMA_X, AKAMA_Y, AKAMA_Z);
                        }
                        DefenderTimer = 15000;
                    } else DefenderTimer -= diff;

                    if (SummonTimer <= diff)
                    {
                        SummonCreature();
                        SummonTimer = 35000;
                    } else SummonTimer -= diff;

                    if (DeathCount >= 6)
                    {
                        if (AkamaGUID)
                        {
                            Creature* Akama = Unit::GetCreature(*me, AkamaGUID);
                            if (Akama && Akama->IsAlive())
                            {
                                IsBanished = false;
                                me->GetMotionMaster()->Clear(false);
                                me->GetMotionMaster()->MoveChase(Akama);
                                Akama->GetMotionMaster()->Clear();
                                // Shade should move to Akama, not the other way around
                                Akama->GetMotionMaster()->MoveIdle();
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                // Crazy amount of threat
                                me->AddThreat(Akama, 10000000.0f);
                                Akama->AddThreat(me, 10000000.0f);
                                me->Attack(Akama, true);
                                Akama->Attack(me, true);
                            }
                        }
                    }
                }
                else                                                // No longer banished, let's fight Akama now
                {
                    if (ReduceHealthTimer <= diff)
                    {
                        if (AkamaGUID)
                        {
                            Creature* Akama = Unit::GetCreature(*me, AkamaGUID);
                            if (Akama && Akama->IsAlive())
                            {
                                //10 % less health every few seconds.
                                me->DealDamage(Akama, Akama->GetMaxHealth() / 10, nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
                                ReduceHealthTimer = 12000;
                            }
                        }
                    } else ReduceHealthTimer -= diff;

                    if (HasKilledAkama)
                    {
                        if (!HasKilledAkamaAndReseting) //do not let players kill Shade if Akama is dead and Shade is waiting for ResetTimer!! event would bug
                        {
                            HasKilledAkamaAndReseting = true;
                            me->RemoveAllAuras();
                            me->DeleteThreatList();
                            me->CombatStop();
                            //me->SetFullHealth();
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            me->GetMotionMaster()->MoveTargetedHome();
                        }
                        if (ResetTimer <= diff)
                        {
                            EnterEvadeMode(); // Reset a little while after killing Akama, evade and respawn Akama
                            return;
                        } else ResetTimer -= diff;
                    }

                    DoMeleeAttackIfReady();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_shade_of_akamaAI(creature);
        }
};

void npc_ashtongue_channeler::npc_ashtongue_channelerAI::JustDied(Unit* /*killer*/)
{
    Creature* shade = (Unit::GetCreature(*me, ShadeGUID));
    if (shade && shade->IsAlive())
        CAST_AI(boss_shade_of_akama::boss_shade_of_akamaAI, shade->AI())->IncrementDeathCount();
    else
        TC_LOG_ERROR("misc", "SD2 ERROR: Channeler dead but unable to increment DeathCount for Shade of Akama.");
}

void npc_ashtongue_sorcerer::npc_ashtongue_sorcererAI::JustDied(Unit* /*killer*/)
{
    Creature* shade = (Unit::GetCreature(*me, ShadeGUID));
    if (shade && shade->IsAlive())
        CAST_AI(boss_shade_of_akama::boss_shade_of_akamaAI, shade->AI())->IncrementDeathCount(me->GetGUID());
    else
        TC_LOG_ERROR("misc", "SD2 ERROR: Sorcerer dead but unable to increment DeathCount for Shade of Akama.");
}

class npc_akama_shade : public CreatureScript
{
    public:
        npc_akama_shade() : CreatureScript("npc_akama_shade") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1) // Fight time
            {
                player->CLOSE_GOSSIP_MENU();
                CAST_AI(npc_akama_shade::npc_akamaAI, creature->AI())->BeginEvent(player);
            }

            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            InstanceScript* instance = creature->GetInstanceScript();

            if (instance && instance->GetData(DATA_SHADE_OF_AKAMA_EVENT) == NOT_STARTED && player->IsAlive() && instance->CheckRequiredBosses(DATA_SHADE_OF_AKAMA_EVENT, player))
                player->ADD_GOSSIP_ITEM_DB(GOSSIP_ITEM, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(10866, creature->GetGUID());

            return true;
        }

        struct npc_akamaAI : public ScriptedAI
        {
            npc_akamaAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                ShadeHasDied = false;
                StartCombat = false;
                instance = creature->GetInstanceScript();
                if (instance)
                    ShadeGUID = instance->GetGuidData(DATA_SHADE_OF_AKAMA);
                else
                    ShadeGUID = ObjectGuid(uint64(NOT_STARTED));
                me->setActive(true);
                EventBegun = false;
                CastSoulRetrieveTimer = 0;
                SoulRetrieveTimer = 0;
                SummonBrokenTimer = 0;
                EndingTalkCount = 0;
                WayPointId = 0;
                BrokenSummonIndex = 0;
                BrokenList.clear();
                HasYelledOnce = false;
            }

            InstanceScript* instance;
            ObjectGuid ShadeGUID;
            uint32 DestructivePoisonTimer;
            uint32 LightningBoltTimer;
            uint32 CheckTimer;
            uint32 CastSoulRetrieveTimer;
            uint32 SoulRetrieveTimer;
            uint32 SummonBrokenTimer;
            uint32 EndingTalkCount;
            uint32 WayPointId;
            uint32 BrokenSummonIndex;
            std::list<ObjectGuid> BrokenList;
            bool EventBegun;
            bool ShadeHasDied;
            bool StartCombat;
            bool HasYelledOnce;
            SummonList summons;

            void Reset() override
            {
                DestructivePoisonTimer = 15000;
                LightningBoltTimer = 10000;
                CheckTimer = 2000;

                if (!EventBegun)
                {
                    me->SetUInt32Value(UNIT_FIELD_NPC_FLAGS, 0);      // Database sometimes has very very strange values
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                }
                summons.DespawnAll();
                me->setActive(true);
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_ASHTONGUE_BROKEN)
                    summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_ASHTONGUE_BROKEN)
                    summons.Despawn(summon);
            }

            void AttackStart(Unit* who) override { }
            void MoveInLineOfSight(Unit* who) override { }
            void JustEngagedWith(Unit* /*who*/) override { }

            void BeginEvent(Player* player)
            {
                if (!instance)
                    return;

                ShadeGUID = instance->GetGuidData(DATA_SHADE_OF_AKAMA);
                if (!ShadeGUID)
                    return;

                Creature* shade = (Unit::GetCreature(*me, ShadeGUID));
                if (shade)
                {
                    instance->SetData(DATA_SHADE_OF_AKAMA_EVENT, IN_PROGRESS);
                    // Prevent players from trying to restart event
                    me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    CAST_AI(boss_shade_of_akama::boss_shade_of_akamaAI, shade->AI())->SetAkamaGUID(me->GetGUID());
                    CAST_AI(boss_shade_of_akama::boss_shade_of_akamaAI, shade->AI())->SetSelectableChannelers();
                    CAST_AI(boss_shade_of_akama::boss_shade_of_akamaAI, shade->AI())->StartCombat = true;
                    me->CombatStart(shade);
                    shade->AddThreat(me, 1000000.0f);
                    //shade->AddThreat(me, 1000000.0f);
                    //me->CombatStart(shade);
                    shade->HandleEmoteStateCommand(EMOTE_STATE_NONE);
                    shade->SetTarget(me->GetGUID());
                    if (player)
                        shade->AddThreat(player, 1.0f);
                    DoZoneInCombat(shade);
                    EventBegun = true;
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case 0:
                        ++WayPointId;
                        break;
                    case 1:
                        if (Creature* Shade = Unit::GetCreature(*me, ShadeGUID))
                        {
                            me->SetTarget(ShadeGUID);
                            DoCast(Shade, SPELL_AKAMA_SOUL_RETRIEVE);
                            EndingTalkCount = 0;
                            SoulRetrieveTimer = 16000;
                        }
                        break;
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);
                EventBegun = false;
                ShadeHasDied = false;
                StartCombat = false;
                CastSoulRetrieveTimer = 0;
                SoulRetrieveTimer = 0;
                SummonBrokenTimer = 0;
                EndingTalkCount = 0;
                WayPointId = 0;
                BrokenSummonIndex = 0;
                BrokenList.clear();
                HasYelledOnce = false;
                Creature* Shade = Unit::GetCreature(*me, ShadeGUID);
                if (Shade && Shade->IsAlive())
                    CAST_AI(boss_shade_of_akama::boss_shade_of_akamaAI, Shade->AI())->HasKilledAkama = true;
                summons.DespawnAll();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!EventBegun)
                    return;

                if (HealthBelowPct(15) && !HasYelledOnce)
                {
                    Talk(SAY_LOW_HEALTH);
                    HasYelledOnce = true;
                }

                if (ShadeGUID && !StartCombat)
                {
                    Creature* shade = (Unit::GetCreature(*me, ShadeGUID));
                    if (shade && shade->IsAlive())
                    {
                        if (CAST_AI(boss_shade_of_akama::boss_shade_of_akamaAI, shade->AI())->IsBanished)
                        {
                            //if (CastSoulRetrieveTimer <= diff)
                            //{
                            //    DoCast(shade, SPELL_AKAMA_SOUL_CHANNEL);
                            //    CastSoulRetrieveTimer = 500;
                            //} else CastSoulRetrieveTimer -= diff;
                            if (!me->HasAura(SPELL_AKAMA_SOUL_CHANNEL))
                                DoCast(shade, SPELL_AKAMA_SOUL_CHANNEL);
                        }
                        else
                        {
                            me->InterruptNonMeleeSpells(false);
                            StartCombat = true;
                        }
                    }
                }

                if (ShadeHasDied && (WayPointId == 1))
                {
                    if (instance)
                        instance->SetData(DATA_SHADE_OF_AKAMA_EVENT, DONE);
                    me->GetMotionMaster()->MovePoint(WayPointId, AkamaWP[1].x, AkamaWP[1].y, AkamaWP[1].z);
                    ++WayPointId;
                }

                if (!ShadeHasDied && StartCombat)
                {
                    if (CheckTimer <= diff)
                    {
                        if (ShadeGUID)
                        {
                            Creature* Shade = Unit::GetCreature(*me, ShadeGUID);
                            if (Shade && !Shade->IsAlive())
                            {
                                ShadeHasDied = true;
                                WayPointId = 0;
                                me->SetWalk(true);
                                me->GetMotionMaster()->MovePoint(WayPointId, AkamaWP[0].x, AkamaWP[0].y, AkamaWP[0].z);
                            }
                            if (Shade && Shade->IsAlive())
                            {
                                if (Shade->GetThreatManager().getThreatList().size() < 2)
                                    Shade->AI()->EnterEvadeMode();
                            }
                        }
                        CheckTimer = 5000;
                    } else CheckTimer -= diff;
                }

                if (SummonBrokenTimer && BrokenSummonIndex < 4)
                {
                    if (SummonBrokenTimer <= diff)
                    {
                        for (uint8 i = 0; i < 4; ++i)
                        {
                            float x = BrokenCoords[BrokenSummonIndex].x + (i*5);
                            float y = BrokenCoords[BrokenSummonIndex].y + (1*5);
                            float z = BrokenCoords[BrokenSummonIndex].z;
                            float o = BrokenCoords[BrokenSummonIndex].o;
                            Creature* broken = me->SummonCreature(NPC_ASHTONGUE_BROKEN, x, y, z, o, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 360000ms);
                            if (broken)
                            {
                                float wx = BrokenWP[BrokenSummonIndex].x + (i*5);
                                float wy = BrokenWP[BrokenSummonIndex].y + (i*5);
                                float wz = BrokenWP[BrokenSummonIndex].z;
                                broken->GetMotionMaster()->MovePoint(0, wx, wy, wz);
                                broken->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                BrokenList.push_back(broken->GetGUID());
                            }
                        }
                        ++BrokenSummonIndex;
                        SummonBrokenTimer = 1000;
                    } else SummonBrokenTimer -= diff;
                }

                if (SoulRetrieveTimer)
                {
                    if (SoulRetrieveTimer <= diff)
                    {
                        switch (EndingTalkCount)
                        {
                            case 0:
                                me->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                                ++EndingTalkCount;
                                SoulRetrieveTimer = 2000;
                                SummonBrokenTimer = 1;
                                break;
                            case 1:
                                Talk(SAY_FREE);
                                ++EndingTalkCount;
                                SoulRetrieveTimer = 25000;
                                break;
                            case 2:
                                if (!BrokenList.empty())
                                {
                                    bool yelled = false;
                                    for (std::list<ObjectGuid>::const_iterator itr = BrokenList.begin(); itr != BrokenList.end(); ++itr)
                                        if (Creature* unit = Unit::GetCreature(*me, *itr))
                                        {
                                            if (!yelled)
                                            {
                                                unit->AI()->Talk(SAY_BROKEN_FREE_0);
                                                yelled = true;
                                            }
                                            unit->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                                        }
                                }
                                ++EndingTalkCount;
                                SoulRetrieveTimer = 1500;
                                break;
                            case 3:
                                if (!BrokenList.empty())
                                {
                                    for (std::list<ObjectGuid>::const_iterator itr = BrokenList.begin(); itr != BrokenList.end(); ++itr)
                                        if (Creature* unit = Unit::GetCreature(*me, *itr))
                                            unit->CastSpell(unit, 39656, true); // This is the incorrect spell, but can't seem to find the right one.
                                }
                                ++EndingTalkCount;
                                SoulRetrieveTimer = 5000;
                                break;
                            case 4:
                                if (!BrokenList.empty())
                                {
                                    for (std::list<ObjectGuid>::const_iterator itr = BrokenList.begin(); itr != BrokenList.end(); ++itr)
                                        if (Creature* unit = Unit::GetCreature(*me, *itr))
                                            unit->AI()->Talk(SAY_BROKEN_FREE_1);
                                }
                                SoulRetrieveTimer = 0;
                                break;
                        }
                    } else SoulRetrieveTimer -= diff;
                }

                if (!UpdateVictim())
                    return;

                if (DestructivePoisonTimer <= diff)
                {
                    Creature* shade = Unit::GetCreature(*me, ShadeGUID);
                    if (shade && shade->IsAlive())
                        DoCast(shade, SPELL_DESTRUCTIVE_POISON);
                    DestructivePoisonTimer = 15000;
                } else DestructivePoisonTimer -= diff;

                if (LightningBoltTimer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_LIGHTNING_BOLT);
                    LightningBoltTimer = 10000;
                } else LightningBoltTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_akamaAI(creature);
        }
};

void AddSC_boss_shade_of_akama()
{
    new boss_shade_of_akama();
    new npc_ashtongue_channeler();
    new npc_ashtongue_sorcerer();
    new npc_akama_shade();
}
