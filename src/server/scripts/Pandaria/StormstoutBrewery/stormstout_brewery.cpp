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

#include "stormstout_brewery.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"

enum eHabaneroBeer
{
    NPC_BARREL              = 56731,

    SPELL_PROC_EXPLOSION    = 106787
};

static const Position aPartyWps[]=
{
    { -766.2f,  1390.8f,  146.7f, 1.8f  },
    { -775.55f, 1422.47f, 139.5f, 3.4f  },
    { -801.81f, 1415.04f, 139.7f, 3.38f },
    { -814.9f,  1411.7f,  134.6f, 3.38f },
    { -811.6f,  1399.9f,  132.3f, 4.98f },
    { -804.4f,  1383.6f,  126.7f, 5.9f  },
    { -783.2f,  1383.4f,  126.7f, 4.9f  }
};

class spell_spicy_explosion : public SpellScriptLoader
{
    public:
        spell_spicy_explosion() : SpellScriptLoader("spell_spicy_explosion") { }

        class spell_spicy_explosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_spicy_explosion_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SPICY_EXPLOSION))
                    return false;

                return true;
            }

            void SelectTargets(std::list<WorldObject*>&targets)
            {
                targets.remove_if([=](WorldObject* obj) { return  obj && obj->GetTypeId() == TYPEID_PLAYER; });
            }

            void OnAfterCast()
            {
                if (GetCaster()->ToCreature())
                    GetCaster()->ToCreature()->DespawnOrUnsummon(1000);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_spicy_explosion_SpellScript::OnAfterCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_spicy_explosion_SpellScript::SelectTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_spicy_explosion_SpellScript::SelectTargets, EFFECT_3, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_spicy_explosion_SpellScript();
        }
};

class npc_chen_stormstout : public CreatureScript
{
    public:
        npc_chen_stormstout() : CreatureScript("npc_chen_stormstout") { }

        enum Talks
        {
            TALK_NOT_ZAN,
            TALK_HAPPENED,
            TALK_WHERE,
            TALK_ABANDONED,
            TALK_GAO,
            TALK_COOKIES,
            TALK_GHOST
        };

        enum Events
        {
            EVENT_NONE,
            EVENT_TALK_1,
            EVENT_TALK_2,
            EVENT_TALK_3,
            EVENT_TALK_4,
            EVENT_TALK_5,
            EVENT_TALK_6,
            EVENT_TALK_7,
        };

        struct npc_chen_stormstoutAI : public ScriptedAI
        {
            npc_chen_stormstoutAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void GetAuntieAndDoAction(int8 action)
            {
                if (Creature* auntie = GetClosestCreatureWithEntry(me, NPC_AUNTIE_STORMSTOUT, 12.f))
                    auntie->AI()->DoAction(action);
            }

            void DoAction(int32 actionId) override
            {
                events.Reset();

                switch (actionId)
                {
                    case 0:
                        events.ScheduleEvent(EVENT_TALK_1, 8000);
                        break;
                    case 1:
                        events.ScheduleEvent(EVENT_TALK_2, 6000);
                        break;
                    case 2:
                        events.ScheduleEvent(EVENT_TALK_3, 7400);
                        break;
                    case 3:
                        events.ScheduleEvent(EVENT_TALK_4, 7000);
                        break;
                    case 4:
                        events.ScheduleEvent(EVENT_TALK_5, 13000);
                        break;
                    case 5:
                        events.ScheduleEvent(EVENT_TALK_6, 4000);
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
                        case EVENT_TALK_1:
                            Talk(TALK_NOT_ZAN);
                            GetAuntieAndDoAction(1);
                            break;
                        case EVENT_TALK_2:
                            Talk(TALK_HAPPENED);
                            GetAuntieAndDoAction(2);
                            break;
                        case EVENT_TALK_3:
                            Talk(TALK_WHERE);
                            GetAuntieAndDoAction(3);
                            break;
                        case EVENT_TALK_4:
                            Talk(TALK_ABANDONED);
                            GetAuntieAndDoAction(4);
                            break;
                        case EVENT_TALK_5:
                            Talk(TALK_GAO);
                            GetAuntieAndDoAction(5);
                            break;
                        case EVENT_TALK_6:
                            Talk(TALK_COOKIES);
                            events.ScheduleEvent(EVENT_TALK_7, 7000);
                            break;
                        case EVENT_TALK_7:
                            Talk(TALK_GHOST);
                            break;
                    }
                }

            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_chen_stormstoutAI(creature);
        }
};

class npc_auntie_stormstout : public CreatureScript
{
    public:
        npc_auntie_stormstout() : CreatureScript("npc_auntie_stormstout") { }

        enum Events
        {
            EVENT_NONE,
            EVENT_TALK_1,
            EVENT_TALK_2,
            EVENT_TALK_3,
            EVENT_TALK_4,
            EVENT_TALK_5,
            EVENT_TALK_6
        };

        enum Talks
        {
            TALK_HELLO,
            TALK_ZAN,
            TALK_DAY,
            TALK_SIZE,
            TALK_ABANDONED,
            TALK_COOKIES
        };

        struct npc_auntie_stormstoutAI : public ScriptedAI
        {
            npc_auntie_stormstoutAI(Creature* creature) : ScriptedAI(creature)
            {
                eventStarted = false;
            }

            bool eventStarted;
            EventMap events;

            void GetChenAndDoAction(int8 action)
            {
                if (Creature* pChen = GetClosestCreatureWithEntry(me, NPC_CHEN_STORMSTOUT, 12.f))
                    pChen->AI()->DoAction(action);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!eventStarted && who && who->ToPlayer())
                {
                    HandleEventStart();
                    eventStarted = true;
                }
            }

            void HandleEventStart()
            {
                if (eventStarted)
                    return;

                events.ScheduleEvent(EVENT_TALK_1, 5000);
            }

            void DoAction(int32 actionId) override
            {
                events.Reset();

                switch (actionId)
                {
                    case 1:
                        events.ScheduleEvent(EVENT_TALK_2, 6000);
                        break;
                    case 2:
                        events.ScheduleEvent(EVENT_TALK_3, 4500);
                        break;        
                    case 3:
                        events.ScheduleEvent(EVENT_TALK_4, 7000);
                        break;
                    case 4:
                        events.ScheduleEvent(EVENT_TALK_5, 6500);
                        break;
                    case 5:
                        events.ScheduleEvent(EVENT_TALK_6, 5000);
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
                        case EVENT_TALK_1:
                            Talk(TALK_HELLO);
                            GetChenAndDoAction(0);
                            break;
                        case EVENT_TALK_2:
                            Talk(TALK_ZAN);
                            GetChenAndDoAction(1);
                            break;
                        case EVENT_TALK_3:
                            Talk(TALK_DAY);
                            GetChenAndDoAction(2);
                            break;
                        case EVENT_TALK_4:
                            Talk(TALK_SIZE);
                            GetChenAndDoAction(3);
                            break;
                        case EVENT_TALK_5:
                            Talk(TALK_ABANDONED);
                            GetChenAndDoAction(4);
                            break;
                        case EVENT_TALK_6:
                            Talk(TALK_COOKIES);
                            GetChenAndDoAction(5);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_auntie_stormstoutAI(creature);
        }
};


class npc_aqua_dancer : public CreatureScript
{
    public:
        npc_aqua_dancer() : CreatureScript("npc_aqua_dancer") { }

        enum Spells
        {
            SPELL_SPLASH                = 107030,
            SPELL_AQUATIC_ILLUSION      = 107044
        };

        enum Talks
        {
            TALK_DEATH
        };

        struct npc_aqua_dancerAI : public ScriptedAI
        {
            npc_aqua_dancerAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid hozenGuid;

            void Reset() override { }

            void InitializeAI() override
            {
                me->SetUnitMovementFlags(MOVEMENTFLAG_DISABLE_GRAVITY);

                if (Creature* hozen = GetClosestCreatureWithEntry(me, NPC_SODDEN_HOZEN_BRAWLER, 30.f))
                {
                    DoCast(hozen, SPELL_AQUATIC_ILLUSION, true);
                    hozenGuid = hozen->GetGUID();
                }

                me->SetReactState(REACT_PASSIVE);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                damage = me->GetHealth();
            }

            void JustDied(Unit* killer) override
            {
                DoCast(me, SPELL_SPLASH, true);

                if (Creature* hozen = ObjectAccessor::GetCreature(*me, hozenGuid))
                {
                    Talk(TALK_DEATH);

                    me->DealDamage(hozen, (uint32)hozen->GetMaxHealth() / 2, 0, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_FROST);

                    if (hozen->AI())
                        hozen->AI()->DoZoneInCombat();
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
            }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_aqua_dancerAI(creature);
        }
};

class npc_fiery_trickster : public CreatureScript
{
    public:
        npc_fiery_trickster() : CreatureScript("npc_fiery_trickster") { }

        enum Spells
        {
            SPELL_FIERY_ILLUSION        = 107175,
            SPELL_BLAZING_SPARK         = 107071
        };

        enum Events
        {
            EVENT_NONE,
            EVENT_INIT,
            EVENT_FIRE_SPARK
        };

        enum Talks
        {
            TALK_DEATH
        };

        struct npc_fiery_tricksterAI : public ScriptedAI
        {
            npc_fiery_tricksterAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetUnitMovementFlags(MOVEMENTFLAG_DISABLE_GRAVITY);
                events.ScheduleEvent(EVENT_INIT, urand(400, 800));
                events.ScheduleEvent(EVENT_FIRE_SPARK, urand(4000, 9000));
            }

            ObjectGuid hozenGuid;
            EventMap events;

            void Initialize()
            {
                Creature* hozen = GetClosestCreatureWithEntry(me, NPC_INFLAMED_HOZEN_BRAWLER, 39.f);

                if (hozen && !hozen->HasAura(SPELL_FIERY_ILLUSION))
                {
                    DoCast(hozen, SPELL_FIERY_ILLUSION, true);
                    hozenGuid = hozen->GetGUID();
                }

                me->SetReactState(REACT_PASSIVE);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                damage = me->GetHealth();
            }

            void JustDied(Unit* killer) override
            {
                if (Creature* hozen = ObjectAccessor::GetCreature(*me, hozenGuid))
                {
                    Talk(TALK_DEATH);
                    me->DealDamage(hozen, (uint32)hozen->GetMaxHealth() / 2, 0, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_FIRE);

                    if (hozen->AI())
                        hozen->AI()->DoZoneInCombat();
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INIT:
                            Initialize();
                            break;
                        case EVENT_FIRE_SPARK:
                            DoCast(me, SPELL_BLAZING_SPARK, true);
                            events.ScheduleEvent(EVENT_FIRE_SPARK, urand(6000, 11000));
                            break;
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_fiery_tricksterAI(creature);
        }
};

static const uint32 auiEmotes[] =
{
    EMOTE_ONESHOT_TALK,
    EMOTE_ONESHOT_CHEER,
    EMOTE_ONESHOT_EAT,
    EMOTE_ONESHOT_LAUGH,
};

class npc_hozen_party_animal : public CreatureScript
{
    public:
        npc_hozen_party_animal() : CreatureScript("npc_hozen_party_animal") { }

        enum Events
        {
            EVENT_NONE,
            EVENT_MOVE,
            EVENT_FIREWORK,
        };

        struct npc_hozen_party_animalAI : public ScriptedAI
        {
            npc_hozen_party_animalAI(Creature* creature) : ScriptedAI(creature) { }

            bool canAggroMore;
            uint32 waypoint;
            EventMap events;

            void Initialize()
            {
                me->CombatStop(true);
                me->AttackStop();
                me->SetReactState(REACT_PASSIVE);
                canAggroMore = true;
                me->HandleEmoteCommand(auiEmotes[(urand(0, 3))]);

                me->GetMotionMaster()->Clear(false);

                waypoint = (me->GetDistance(aPartyWps[0]) > me->GetDistance(aPartyWps[1])) ? 1 : 0;

                events.ScheduleEvent(EVENT_MOVE, 100);
                events.ScheduleEvent(EVENT_FIREWORK, urand(2, 28) * IN_MILLISECONDS);

            }

            void Reset() override
            {
                events.ScheduleEvent(EVENT_FIREWORK, urand(2, 28)*IN_MILLISECONDS);

                if (uint32 hatId = Trinity::Containers::SelectRandomContainerElement(hats))
                    me->CastSpell(me, hatId, false);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == 0)
                    Initialize();
                else if (actionId == 1)
                    canAggroMore = false;
            }

            void Move()
            {
                me->GetMotionMaster()->MovePoint(waypoint, aPartyWps[waypoint]);
            }

            void JustEngagedWith(Unit* who) override
            {
                std::list<Creature*> temp;
                GetCreatureListWithEntryInGrid(temp, me, me->GetEntry(), 15.f);

                if (canAggroMore)
                {
                    for (auto creature : temp)
                    {
                        if (creature->AI() && creature->IsAlive() && !creature->IsInCombat())
                        {
                            creature->AI()->DoAction(1);
                            creature->AI()->DoZoneInCombat();
                        }
                    }
                }

                events.CancelEvent(EVENT_FIREWORK);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                        events.ScheduleEvent(EVENT_MOVE, 100);
                        break;
                    case 6:
                        me->DespawnOrUnsummon();
                        break;
                }

                waypoint++;
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOVE:
                            Move();
                            break;
                        case EVENT_FIREWORK:
                            if (uint32 fireworkId = Trinity::Containers::SelectRandomContainerElement(fireworks))
                                me->CastSpell(me, fireworkId, false);

                            events.ScheduleEvent(EVENT_FIREWORK, urand(2, 28)*IN_MILLISECONDS);
                            break;
                    }
                }

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_hozen_party_animalAI(creature);
        }
};

class npc_hozen_bouncer : public CreatureScript
{
    public:
        npc_hozen_bouncer() : CreatureScript("npc_hozen_bouncer") { }

        enum Events
        {
            EVENT_NONE,
            EVENT_TALK
        };

        enum Talks
        {
            TALK_DOWN,
            TALK_MEAN,
            TALK_NEW,
            TALK_PARTY
        };

        enum Spells
        {
            SPELL_BOUNCE    = 107019
        };

        struct npc_hozen_bouncerAI : public ScriptedAI
        {
            npc_hozen_bouncerAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();

                if (instance && instance->GetBossState(DATA_OOK_OOK) == DONE)
                    me->AddAura(SPELL_BOUNCE, me);

                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
            }

            uint32 talk;
            EventMap events;
            InstanceScript* instance;

            void DoAction(int32 actionId) override
            {
                talk = actionId;
                me->RemoveAllAuras();

                float x, y;

                GetPositionWithDistInOrientation(me, 18.f, me->GetOrientation(), x, y);
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(0, x, y, me->GetMap()->GetHeight(x, y, me->GetPositionZ()));
            }

            void MovementInform(uint32 type, uint32 /*pointId*/) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                me->SetFacingTo(1.83f);
                events.ScheduleEvent(EVENT_TALK, 2000*(talk+1)*2);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TALK:
                            Talk(talk);
                            if (talk < 2)
                                events.ScheduleEvent(EVENT_TALK, 5000);
                            talk += 2;
                            break;
                    }
                    
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_hozen_bouncerAI(creature);
        }
};

class npc_controlled_hozen : public CreatureScript
{
    public:
        npc_controlled_hozen() : CreatureScript("npc_controlled_hozen") { }

        struct npc_controlled_hozenAI : public ScriptedAI
        {
            npc_controlled_hozenAI(Creature* creature) : ScriptedAI(creature) { }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                me->CombatStop(true);
                me->AttackStop();
                me->RemoveAllAurasExceptType(SPELL_AURA_PROC_TRIGGER_SPELL);
                me->GetMotionMaster()->MoveTargetedHome();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                std::list<Creature*> temp;
                GetCreatureListWithEntryInGrid(temp, me, me->GetEntry(), 60.f);

                for (auto&& creature : temp)
                    if (creature->IsAlive() && !creature->IsInCombat())
                        creature->AI()->DoZoneInCombat();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_controlled_hozenAI(creature);
        }
};

// Habanero Brew 56731
class npc_habanero_brew : public CreatureScript
{
    public:
        npc_habanero_brew() : CreatureScript("npc_habanero_brew") { }

        struct npc_habanero_brewAI : public ScriptedAI
        {
            npc_habanero_brewAI(Creature* creature) : ScriptedAI(creature) {}

            void InitializeAI() override
            {
                if (!me->HasAura(SPELL_PROC_EXPLOSION))
                    me->AddAura(SPELL_PROC_EXPLOSION, me);

                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->DespawnOrUnsummon(1000);
            }

            void DoAction(int32 /*actionId*/) override { }

            void OnSpellClick(Unit* /*clicker*/, bool& /*result*/) override
            {
                me->CastSpell(me, SPELL_SPICY_EXPLOSION, false);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_habanero_brewAI(creature);
        }
};

void AddSC_stormstout_brewery()
{
    new spell_spicy_explosion();
    new npc_chen_stormstout();
    new npc_auntie_stormstout();
    new npc_aqua_dancer();
    new npc_fiery_trickster();
    new npc_hozen_party_animal();
    new npc_hozen_bouncer();
    new npc_controlled_hozen();
    new npc_habanero_brew();
}