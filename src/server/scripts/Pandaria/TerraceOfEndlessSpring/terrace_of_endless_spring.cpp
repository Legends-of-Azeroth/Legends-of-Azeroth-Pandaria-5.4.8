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

#include "ScriptedCreature.h"
#include "terrace_of_endless_spring.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "CreatureAI.h"
#include "MoveSplineInit.h"
#include "GridNotifiers.h"
#include "Config.h"

enum eAshaniGossips
{
    ASHANI_LAST_GOSSIP = 60010,
    ASHANI_LAST_OPTION = 51
};

// Springtender Ashani 64846
class npc_terrace_springtender_ashani : public CreatureScript
{
    public:
        npc_terrace_springtender_ashani() : CreatureScript("npc_terrace_springtender_ashani") { }
    
        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
    
            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    
                if (creature->GetInstanceScript())
                    creature->GetInstanceScript()->DoRemoveDampeningFromCreatures();
            }
    
            player->CLOSE_GOSSIP_MENU();
    
            return true;
        }
    
        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());
    
            if(sConfigMgr->GetIntDefault("FirstOfTheKingdom.TerraceSpring", 1))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Turn off dampening?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }
    
        struct npc_terrace_springtender_ashaniAI : public ScriptedAI
        {
            npc_terrace_springtender_ashaniAI(Creature* creature) : ScriptedAI(creature) { }
    
            void InitializeAI() override
            {
                if (me->GetMap()->GetDifficulty() != RAID_DIFFICULTY_25MAN_LFR)
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_terrace_springtender_ashaniAI(creature);
        }
};

enum eApparitionsSpells
{
    // Shared
    SPELL_CHANNELING_CORRUPTION     = 125706,
    SPELL_CORRUPTION_BOLT           = 125713,

    // Fear
    SPELL_OVERWHELMING_FEAR         = 125758,
    SPELL_NIGHT_TERRORS_MISSILE     = 125736,
    SPELL_NIGHT_TERRORS_PERIODIC    = 125745,
    SPELL_ENVELOPING_DARKNESS       = 125760,

    // Terror
    SPELL_GRIP_OF_FEAR              = 130115,
    SPELL_UNLEASHED_TERROR          = 130120
};

enum eApparitionsEvents
{
    // Fear
    EVENT_OVERWHELMING_FEAR     = 1,
    EVENT_NIGHT_TERRORS_MISSILE = 2,
    EVENT_NIGHT_TERRORS_AREA    = 3,
    EVENT_ENVELOPING_DARKNESS   = 4,

    // Terror
    EVENT_GRIP_OF_FEAR          = 5,
    EVENT_UNLEASHED_TERROR      = 6
};

// Apparition of Fear - 64368
class npc_apparition_of_fear : public CreatureScript
{
    public:
        npc_apparition_of_fear() : CreatureScript("npc_apparition_of_fear") { }

        struct npc_apparition_of_fearAI : public ScriptedAI
        {
            npc_apparition_of_fearAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                instance = creature->GetInstanceScript();
            }

            SummonList summons;
            InstanceScript* instance;
            EventMap events;

            uint32 corruptionBoltTimer;

            void Reset() override
            {
                me->CastSpell(me, SPELL_CHANNELING_CORRUPTION, true);

                events.Reset();

                corruptionBoltTimer = 3000;

                summons.DespawnAll();

                if (instance)
                    instance->SetData(INTRO_DONE, NOT_STARTED);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->RemoveAurasDueToSpell(SPELL_CHANNELING_CORRUPTION);

                // Schedule Combat event here
                events.Reset();

                events.ScheduleEvent(EVENT_OVERWHELMING_FEAR, 8000);
                events.ScheduleEvent(EVENT_NIGHT_TERRORS_MISSILE, 13000);
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
                std::list<Creature*> fear;
                me->GetCreatureListWithEntryInGrid(fear, NPC_APPARITION_OF_FEAR, 100.0f);
                std::list<Creature*> terror;
                me->GetCreatureListWithEntryInGrid(fear, NPC_APPARITION_OF_TERROR, 100.0f);

                bool done = true;
                for (auto&& itr : fear)
                {
                    if (itr->IsAlive())
                    {
                        done = false;
                        break;
                    }
                }

                for (auto&& itr : terror)
                {
                    if (itr->IsAlive())
                    {
                        done = false;
                        break;
                    }
                }

                if (done && instance)
                    instance->SetData(INTRO_DONE, DONE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                {
                    if (corruptionBoltTimer)
                    {
                        if (corruptionBoltTimer <= diff)
                        {
                            corruptionBoltTimer = 3000;

                            if (instance)
                            {
                                Creature* asani = instance->instance->GetCreature(instance->GetGuidData(NPC_ANCIENT_ASANI));
                                Creature* regail = instance->instance->GetCreature(instance->GetGuidData(NPC_ANCIENT_REGAIL));
                                Creature* kaolan = instance->instance->GetCreature(instance->GetGuidData(NPC_PROTECTOR_KAOLAN));
                                std::vector<ObjectGuid> targets;

                                if (asani)
                                    targets.push_back(asani->GetGUID());
                                if (regail)
                                    targets.push_back(regail->GetGUID());
                                if (kaolan)
                                    targets.push_back(kaolan->GetGUID());

                                if (targets.empty())
                                    return;
                                std::random_device rd;
                                std::mt19937 g(rd());
                                std::shuffle(targets.begin(), targets.end(), g);
                                if (Unit* target = ObjectAccessor::GetUnit(*me, (*targets.begin())))
                                    me->CastSpell(target, SPELL_CORRUPTION_BOLT, true);
                            }
                        }
                        else
                            corruptionBoltTimer -= diff;
                    }
                    return;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_OVERWHELMING_FEAR:
                        {
                            me->CastSpell(me, SPELL_OVERWHELMING_FEAR, false);
                            events.ScheduleEvent(EVENT_OVERWHELMING_FEAR, 12000);
                            break;
                        }
                        case EVENT_NIGHT_TERRORS_MISSILE:
                        {
                            me->CastSpell(me, SPELL_NIGHT_TERRORS_MISSILE, false);
                            events.ScheduleEvent(EVENT_NIGHT_TERRORS_MISSILE, 20000);
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_apparition_of_fearAI(creature);
        }
};

// Apparition of Terror - 66100
class npc_apparition_of_terror : public CreatureScript
{
    public:
        npc_apparition_of_terror() : CreatureScript("npc_apparition_of_terror") { }

        struct npc_apparition_of_terrorAI : public ScriptedAI
        {
            npc_apparition_of_terrorAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            EventMap events;
            SummonList summons;
            uint32 corruptionBoltTimer;

            void Reset() override
            {
                me->CastSpell(me, SPELL_CHANNELING_CORRUPTION, true);

                events.Reset();

                corruptionBoltTimer = 3000;

                summons.DespawnAll();

                if (instance)
                    instance->SetData(INTRO_DONE, NOT_STARTED);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->RemoveAurasDueToSpell(SPELL_CHANNELING_CORRUPTION);

                // Schedule Combat event here
                events.Reset();
                events.ScheduleEvent(EVENT_GRIP_OF_FEAR, 5000);
                events.ScheduleEvent(EVENT_UNLEASHED_TERROR, 9000);
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
                std::list<Creature*> fear;
                me->GetCreatureListWithEntryInGrid(fear, NPC_APPARITION_OF_FEAR, 100.0f);
                std::list<Creature*> terror;
                me->GetCreatureListWithEntryInGrid(fear, NPC_APPARITION_OF_TERROR, 100.0f);

                bool done = true;
                for (auto&& itr : fear)
                {
                    if (itr->IsAlive())
                    {
                        done = false;
                        break;
                    }
                }

                for (auto&& itr : terror)
                {
                    if (itr->IsAlive())
                    {
                        done = false;
                        break;
                    }
                }

                if (done && instance)
                    instance->SetData(INTRO_DONE, DONE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                {
                    if (corruptionBoltTimer)
                    {
                        if (corruptionBoltTimer <= diff)
                        {
                            corruptionBoltTimer = 3000;

                            if (instance)
                            {
                                Creature* asani = instance->instance->GetCreature(instance->GetGuidData(NPC_ANCIENT_ASANI));
                                Creature* regail = instance->instance->GetCreature(instance->GetGuidData(NPC_ANCIENT_REGAIL));
                                Creature* kaolan = instance->instance->GetCreature(instance->GetGuidData(NPC_PROTECTOR_KAOLAN));
                                std::vector<ObjectGuid> targets;

                                if (asani)
                                    targets.push_back(asani->GetGUID());
                                if (regail)
                                    targets.push_back(regail->GetGUID());
                                if (kaolan)
                                    targets.push_back(kaolan->GetGUID());

                                if (targets.empty())
                                    return;
                                std::random_device rd;
                                std::mt19937 g(rd());
                                std::shuffle(targets.begin(), targets.end(), g);
                                if (Unit* target = ObjectAccessor::GetUnit(*me, (*targets.begin())))
                                    me->CastSpell(target, SPELL_CORRUPTION_BOLT, true);
                            }
                        }
                        else
                            corruptionBoltTimer -= diff;
                    }
                    return;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_GRIP_OF_FEAR:
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                me->CastSpell(target, SPELL_GRIP_OF_FEAR, false);

                            events.ScheduleEvent(EVENT_GRIP_OF_FEAR, 12000);
                            break;
                        }
                        case EVENT_UNLEASHED_TERROR:
                        {
                            me->CastSpell(me, SPELL_UNLEASHED_TERROR, false);
                            events.ScheduleEvent(EVENT_UNLEASHED_TERROR, 18000);
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_apparition_of_terrorAI(creature);
        }
};

// Night Terrors - 64390
class npc_night_terrors : public CreatureScript
{
    public:
        npc_night_terrors() : CreatureScript("npc_night_terrors") { }

        struct npc_night_terrorsAI : public ScriptedAI
        {
            npc_night_terrorsAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->CastSpell(me, SPELL_NIGHT_TERRORS_PERIODIC, true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_night_terrorsAI(creature);
        }
};

// Night Terror (summon) - 64393
class npc_night_terror_summon : public CreatureScript
{
    public:
        npc_night_terror_summon() : CreatureScript("npc_night_terror_summon") { }

        struct npc_night_terror_summonAI : public ScriptedAI
        {
            npc_night_terror_summonAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_ENVELOPING_DARKNESS, 2000);

                me->SetReactState(REACT_AGGRESSIVE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_NEAREST))
                        AttackStart(target);

                    return;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_ENVELOPING_DARKNESS)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            me->CastSpell(target, SPELL_ENVELOPING_DARKNESS, false);

                        events.ScheduleEvent(EVENT_ENVELOPING_DARKNESS, 8000);
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_night_terror_summonAI(creature);
        }
};

// Night Terrors (missile) - 125736
class spell_night_terrors_missile : public SpellScriptLoader
{
    public:
        spell_night_terrors_missile() : SpellScriptLoader("spell_night_terrors_missile") { }

        class spell_night_terrors_missile_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_night_terrors_missile_SpellScript);

            void SummonNightTerrors(SpellEffIndex /*effIndex*/)
            {
                PreventHitDefaultEffect(EFFECT_0);

                if (!GetExplTargetDest())
                    return;

                if (Unit* caster = GetCaster())
                    caster->SummonCreature(NPC_NIGHT_TERRORS, GetExplTargetDest()->GetPositionX(), GetExplTargetDest()->GetPositionY(),
                    GetExplTargetDest()->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 15000);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_night_terrors_missile_SpellScript::SummonNightTerrors, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_night_terrors_missile_SpellScript();
        }
};

// Night Terrors (periodic) - 125745
class spell_night_terrors_periodic : public SpellScriptLoader
{
    public:
        spell_night_terrors_periodic() : SpellScriptLoader("spell_night_terrors_periodic") { }

        class spell_night_terrors_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_night_terrors_periodic_AuraScript);

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                if (Unit* caster = GetCaster())
                {
                    Position pos = caster->GetPosition();

                    caster->SummonCreature(NPC_NIGHT_TERROR_SUMMON, pos);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_night_terrors_periodic_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_night_terrors_periodic_AuraScript();
        }
};

void AddSC_terrace_of_endless_spring()
{
    new npc_terrace_springtender_ashani();
    new npc_apparition_of_fear();
    new npc_apparition_of_terror();
    new npc_night_terrors();
    new npc_night_terror_summon();
    new spell_night_terrors_missile();
    new spell_night_terrors_periodic();
}
