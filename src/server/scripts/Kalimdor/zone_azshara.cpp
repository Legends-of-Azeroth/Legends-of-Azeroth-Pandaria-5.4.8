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
#include "SpellScript.h"
#pragma execution_character_set("UTF-8")

enum groundTroopsTypes
{
    NPC_AWOL_GRUNT_DESCIPLINED = 42893,
    NPC_AWOL_GRUNT             = 42646,

    TALK_ENRAGE = 0,
    TALK_DEFEAT,
    TALK_APOLOGY,

    ACTION_CHOISE = 0,
};

// AWOL Grunt 42646
struct npc_azshara_awol_grunt : public CreatureAI
{
    npc_azshara_awol_grunt(Creature* creature) : CreatureAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid targetGUID;
    bool hasDefeat;

    void Reset() override
    {
        targetGUID = ObjectGuid::Empty;
        hasDefeat = false;
        me->SetFaction(125);
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        targetGUID = guid;
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_CHOISE)
        {
            uint32 ourChoise = urand(0, 1);

            if (ourChoise)
            {
                if (Player* target = ObjectAccessor::GetPlayer(*me, targetGUID))
                {
                    Talk(TALK_APOLOGY, target);
                    target->KilledMonsterCredit(NPC_AWOL_GRUNT_DESCIPLINED);
                }

                scheduler
                    .Schedule(Seconds(2), [this](TaskContext /*context*/)
                {
                    MoveAway();
                });
            }
            else
            {
                me->SetFaction(16);

                if (Player* target = ObjectAccessor::GetPlayer(*me, targetGUID))
                {
                    Talk(TALK_ENRAGE, target);
                    AttackStart(target);
                }
            }
        }
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (hasDefeat)
            damage = 0;

        if (damage >= me->GetHealth() && !hasDefeat)
        {
            damage = 0;
            hasDefeat = true;
            me->SetFaction(125);
            me->PrepareChanneledCast(me->GetOrientation());
            Talk(TALK_DEFEAT);

            if (Player* target = ObjectAccessor::GetPlayer(*me, targetGUID))
                target->KilledMonsterCredit(NPC_AWOL_GRUNT_DESCIPLINED);

            scheduler
                .Schedule(Seconds(2), [this](TaskContext /*context*/)
            {
                MoveAway();
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

    private:
        void MoveAway()
        {
            Position pos = me->GetNearPosition(15.0f, frand(0.0f, 2 * M_PI));
            me->GetMotionMaster()->MovePoint(0, pos);
            me->DespawnOrUnsummon(me->GetSplineDuration());
        }
};

// Chewing Out 80017
class spell_azshara_chewing_out : public SpellScript
{
    PrepareSpellScript(spell_azshara_chewing_out);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            if (Creature* grunt = GetHitCreature())
            {
                grunt->AI()->SetGUID(caster->GetGUID());
                grunt->AI()->DoAction(ACTION_CHOISE);
            }
        }
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_AWOL_GRUNT; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_azshara_chewing_out::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_azshara_chewing_out::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

/*
Quest: An Ancient Enemy -- needs some corrections
*/

static const Position ancientEnemyPos[3] =
{
    {-716.50f, -5597.82f, 25.56f, 1.23f}, // Vanira
    {-730.68f, -5592.52f, 25.5f, 0.52f}, // Zuni
    {-722.39f, -5597.65f, 25.5f, 0.89f}, // Vol'jin
};

enum ancientEnemy
{
        NPC_ZUNI                    = 38423,
        NPC_ZARJIRA                 = 38306,
        NPC_VANIRA                  = 38437,
        NPC_VOLJIN                  = 38225,
        EVENT_INTRO                 = 1,
        SPELL_FIRES                 = 72250,
        SPELL_FIRES_BEAM            = 73294,
};

#define VOLJIN_GOSSIP       "我准备好了."
static const uint32 npcId[3] = { NPC_ZUNI, NPC_VANIRA, NPC_VOLJIN };

class npc_voljin_ancient_enemy : public CreatureScript
{
    enum
    {
        QUEST_ANCIENT_ENEMY         = 24814,
        GOSSIP_VOLJIN               = 15318,

        SPELL_VOLJIN_CREDIT         = 73589,
        SPELL_SHADOW_SHOCK          = 73087,
        SPELL_SHOOT                 = 20463,

        EVENT_SHOOT                 = 2,
        EVENT_SHADOW_SHOCK          = 3,

        SAY_INTRO1                   = 0,
        SAY_INTRO2                   = 1,
        SAY_BRAZIERS                = 2,
        SAY_OUTRO_1                 = 3,
        SAY_OUTRO_2                 = 4,
        SAY_OUTRO_3                 = 5,
    };

    struct npc_voljin_ancient_enemyAI : public ScriptedAI
    {
        npc_voljin_ancient_enemyAI(Creature* c) : ScriptedAI(c) {}

        void Reset() override
        {
            EventInProgress = false;
            events.Reset();
            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }

        void AttackStart(Unit * who) override
        {
            ScriptedAI::AttackStartNoMove(who);
        }

        void DoAction(int32 /*action*/) override
        {
            if(EventInProgress)
                return;
            EventInProgress = true;

            for (int i=0; i<=2; ++i)
            {
                if(Creature * creature = i == 2 ? me : me->FindNearestCreature(i ? NPC_ZUNI : NPC_VANIRA, 20.0f))
                {
                    creature->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                    creature->SetReactState(REACT_PASSIVE);
                    creature->GetMotionMaster()->MovePoint(0, ancientEnemyPos[i]);
                }
            }

            events.ScheduleEvent(EVENT_INTRO, 10000);
        }

        void JustEngagedWith(Unit * /*who*/) override
        {
            events.ScheduleEvent(EVENT_SHOOT, 2000);
            events.ScheduleEvent(EVENT_SHADOW_SHOCK, 5000);
        }

        void UpdateAI(uint32 diff) override
        {
            if(!EventInProgress)
                return;

            events.Update(diff);
            {
                if(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                    case EVENT_INTRO:
                        Talk(SAY_INTRO1);
                        if (Creature * zarjira = me->FindNearestCreature(NPC_ZARJIRA, 30.0f))
                            zarjira->AI()->DoAction(1);
                        break;
                    case EVENT_SHOOT:
                        DoCastVictim(SPELL_SHOOT);
                        events.ScheduleEvent(EVENT_SHOOT, urand(2000, 2500));
                        break;
                    case EVENT_SHADOW_SHOCK:
                        DoCastVictim(SPELL_SHADOW_SHOCK);
                        events.ScheduleEvent(EVENT_SHADOW_SHOCK, urand(8000, 10000));
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    private:
        bool EventInProgress;
        EventMap events;
    };

public:

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if(player->GetQuestStatus(QUEST_ANCIENT_ENEMY) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, VOLJIN_GOSSIP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(GOSSIP_VOLJIN, creature->GetGUID());            
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        player->CLOSE_GOSSIP_MENU();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
            creature->AI()->DoAction(1);
        player->KilledMonsterCredit(NPC_VOLJIN);
        creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        return true;
    }

    npc_voljin_ancient_enemy() : CreatureScript("npc_voljin_ancient_enemy") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_voljin_ancient_enemyAI (creature);
    }
};

class npc_zarjira : public CreatureScript
{
    enum
    {
        NPC_FIRE_OF_THE_SEAS        = 38542,
        SPELL_FROSTBOLT             = 46987,
        SPELL_SUMMON_MANIFESTATION  = 73295,
        SPELL_DELUGE_OF_SHADOW      = 72044,

        EVENT_COMBAT                = 2,
        EVENT_FROSTBOLT             = 3,
        EVENT_MANIFESTATION         = 4,
        EVENT_FIRES                 = 5,

        SAY_INTRO                   = 1,
        SAY_FIRES                   = 2,
    };

    struct npc_zarjiraAI : public ScriptedAI
    {
        npc_zarjiraAI(Creature* c) : ScriptedAI(c) {}

        void Reset() override
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void AttackStart(Unit * who) override
        {
            ScriptedAI::AttackStartNoMove(who);
        }

        void DoAction(int32 action) override
        {
            if(action == 1)
                events.ScheduleEvent(EVENT_INTRO, 12000);
            if(action == 2)
            {
                Talk(SAY_FIRES);
                if(Creature * voljin = me->FindNearestCreature(NPC_VOLJIN, 30.0f))
                    voljin->RemoveAllAuras();
            }
        }

        void JustDied(Unit* /*killer*/) override
        {
            for(int i=0;i<=2;++i)
                if(Creature * creature = me->FindNearestCreature(npcId[i], 30.0f))
                    creature->AI()->EnterEvadeMode();

            std::list<Player*> playerList;
            GetPlayerListInGrid(playerList, me, 50.0f);
            for (auto&& player : playerList)
                player->KilledMonsterCredit(NPC_ZARJIRA, ObjectGuid::Empty);
        }

        void JustEngagedWith(Unit *) override
        {
            events.ScheduleEvent(EVENT_FROSTBOLT, 3000);
            events.ScheduleEvent(EVENT_MANIFESTATION, 10000);
            events.ScheduleEvent(EVENT_FIRES, 60000);
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);
            if(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case EVENT_INTRO:
                    Talk(1);
                    events.ScheduleEvent(EVENT_COMBAT, 10000);
                    break;
                case EVENT_COMBAT:
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        if(Creature * creature = me->FindNearestCreature(NPC_VOLJIN, 30.0f))
                            creature->CastSpell(me, SPELL_DELUGE_OF_SHADOW, true);

                        for(int i=0;i<=2;++i)
                        {
                            if(Creature * creature = me->FindNearestCreature(npcId[i], 30.0f))
                            {
                                me->SetInCombatWith(creature);
                                if(i == 2)
                                {
                                    me->Attack(creature, false);
                                    creature->Attack(me, false);
                                }
                                else
                                {
                                    creature->Attack(me, true);
                                    creature->GetMotionMaster()->MoveChase(me);
                                }

                            }
                        }
                        me->SetReactState(REACT_PASSIVE);
                    }
                    break;
                case EVENT_FROSTBOLT:
                    if(me->GetPower(POWER_MANA) <= me->GetMaxPower(POWER_MANA) / 4)
                        me->SetPower(POWER_MANA, me->GetMaxPower(POWER_MANA));
                    DoCastVictim(SPELL_FROSTBOLT);
                    events.ScheduleEvent(EVENT_FROSTBOLT, urand(2500, 3000));
                    break;
                case EVENT_MANIFESTATION:
                    if(Unit * target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                        target->CastSpell(target, SPELL_SUMMON_MANIFESTATION, true);
                    events.ScheduleEvent(EVENT_MANIFESTATION, urand(10000, 15000));
                    break;
                case EVENT_FIRES:
                    {
                        if (Creature* ancientenemy = me->FindNearestCreature(38225, 100.0f, true))
                            ancientenemy->AI()->Talk(SAY_INTRO, me);
                        std::list<Creature*> cList;
                        me->GetCreatureListWithEntryInGrid(cList, NPC_FIRE_OF_THE_SEAS, 30.0f);
                        for(std::list<Creature*>::const_iterator i = cList.begin(); i != cList.end(); ++i)
                        {
                            (*i)->CastSpell((*i), SPELL_FIRES, true);
                            (*i)->CastSpell(me, SPELL_FIRES_BEAM, true);
                            (*i)->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);                            
                        }
                        events.ScheduleEvent(EVENT_FIRES, urand(60000, 80000));
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap events;
    };

public:
    npc_zarjira() : CreatureScript("npc_zarjira") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_zarjiraAI (creature);
    }
};

class npc_fire_of_the_seas : public CreatureScript
{
    enum
    {
        SPELL_STAMP_OUT_FIRES       = 73296,
    };

    struct npc_fire_of_the_seasAI : public ScriptedAI
    {
        npc_fire_of_the_seasAI(Creature* c) : ScriptedAI(c) {}

        void Reset() override
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void SpellHit(Unit * /*caster*/, const SpellInfo * spell) override
        {
            if(spell->Id == SPELL_STAMP_OUT_FIRES)
            {
                me->RemoveAllAuras();
                me->InterruptNonMeleeSpells(true);
                if(Creature * zarjira = me->FindNearestCreature(NPC_ZARJIRA, 30.0f))
                    if(!zarjira->HasAura(SPELL_FIRES_BEAM))
                        zarjira->AI()->DoAction(2);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }
        }
    private:
    };

public:
    npc_fire_of_the_seas() : CreatureScript("npc_fire_of_the_seas") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_fire_of_the_seasAI (creature);
    }
};

void AddSC_azshara()
{
    new creature_script<npc_azshara_awol_grunt>("npc_azshara_awol_grunt");
    new spell_script<spell_azshara_chewing_out>("spell_azshara_chewing_out");
    new npc_voljin_ancient_enemy();
    new npc_zarjira();
    new npc_fire_of_the_seas();
}
