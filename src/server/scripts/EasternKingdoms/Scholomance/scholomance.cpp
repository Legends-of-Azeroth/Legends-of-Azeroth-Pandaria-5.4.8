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
#include "scholomance.h"

enum ScholoEvents
{
    // Neophyte
    EVENT_CAST_REND_FLESH                  = 100,
    EVENT_CAST_SHADOW_INCINERATION         = 101,

    // acolyte                             
    EVENT_CAST_SPIRIT_BARRAGE              = 102,
    EVENT_CAST_UNBOUND                     = 103,
    EVENT_CAST_SHADOW_BOLT                 = 104,
    EVENT_CAST_SHATTER_SOUL                = 105,

    // Risen Guard                         
    EVENT_IMPALE                           = 106,
    EVENT_UNHOLY_WEAPON                    = 107,

    // Candleflickring mages               
    EVENT_FLICKERING_FLAME                 = 108,
    EVENT_SKIN_LIKE_WAX                    = 109,

    // Bored Student                       
    EVENT_FIRE_BREATH                      = 110,
    EVENT_SHADOW_NOVA                      = 111,

    // Professor Slate                     
    EVENT_TOXIC_POTION                     = 112,
    EVENT_POTION_BRUTE_FORCE               = 113,
    EVENT_BRUTISH_FORCE                    = 114,

    // BoneWeaver                          
    EVENT_BONE_SHARDS                      = 115,
    EVENT_SUMMON_WOVEN_BONEGUARD           = 116,

    // Lilian and Gandling
    EVENT_TALK_1                           = 117,
    EVENT_TALK_2                           = 118,
    EVENT_TALK_3                           = 119,
    EVENT_TALK_4                           = 120,
    EVENT_TALK_5                           = 121,
    EVENT_TALK_6                           = 122,
    EVENT_TALK_7                           = 123,
    EVENT_TALK_8                           = 124,
    EVENT_TALK_9                           = 125,
    EVENT_SPECIAL                          = 126,
    EVENT_CHECK_BONEWEAVER                 = 127,
    EVENT_ESCORT                           = 128,
    EVENT_FINALY_FORM                      = 129,
};

enum Yells
{
    TALK_1                                 = 0,
    TALK_2                                 = 1,
    TALK_3                                 = 2,
    TALK_4                                 = 3,
    TALK_5                                 = 4,
    TALK_6                                 = 5,
    TALK_7                                 = 6,
    TALK_8                                 = 7,
    TALK_9                                 = 8,
    TALK_10                                = 9,
    TALK_11                                = 10,
    TALK_12                                = 11,
};

enum Spells
{
    SPELL_SHADOW_FORM_VISUAL               = 51126,
    SPELL_VISUAL_STRANGULATE_EMOTE         = 78037,
    SPELL_UNHOLY_WEAPON                    = 111801,
    SPELL_FLICKERING_FLAME                 = 114474,
    SPELL_SKIN_LIKE_WAX                    = 114479,
    SPELL_TWILING_CHANNEL                  = 78198,
    SPELL_CANDLE_EURPTION_VISUAL           = 39180,
    SPELL_SHADOW_BOLT_BORED_STUDENT        = 114859,
    SPELL_SHADOW_NOVA_BORED_STUDENT        = 114864,
    SPELL_FIRE_BREATH_POTION_BORED_STUDENT = 114872,
    SPELL_POTION_OF_BRUTISH_FORCE          = 114874,
    SPELL_TOXIC_POTION                     = 114873,
    SPELL_SUMMON_BLACK_CANDLE              = 114473,
    SPELL_BLACK_CANDLE_SPELL               = 114400,
    SPELL_SPIRIT_BARRAGE                   = 111774,
    SPELL_SHADOW_BOLT                      = 111599,
    SPELL_SHATTER_SOUL                     = 111594,
    SPELL_SHATTER_SOUL_SUMMON              = 111596,
    SPELL_IMPALE                           = 111813,
    SPELL_BOLIDING_BLOODTHIRST             = 114141,
    SPELL_BOILING_BLOODTHIRST              = 114155,
    SPELL_BOILING_BLOODTHIRST_AURA         = 114141,
    SPELL_REND_FLESH_LOW_LEVEL             = 120787,
    SPELL_REND_FLESH_HIGH_LEVEL            = 111762,
    SPELL_SHADOW_INCINERATION              = 111752,
    SPELL_DRINK_VIAL                       = 114800,
    SPELL_DRINK_VIAL_PROFESSOR_MORPH       = 124934,
    SPELL_CHAIN                            = 83487,
    SPELL_CLOTTING                         = 114176,
    SPELL_KRASTINOV_ACHIEVEMENT_SPELL      = 115427,

};

// Neophyte
#define REND_FLESH_INTERVAL urand(10000, 16000)
#define SHADOW_INCINERATION_INTERVAL 6500
// Acolyte
//#define UNBOUND_INTERVAL 
#define SHADOW_BOLT_INTERVAL 6000
#define SHATTER_SOUL_INTERVAL urand(14000, 20000)

#define EVENT_UNHOLY_WEAPON_INTERVAL urand(20000, 26000)
#define EVENT_IMPALE_INTERVAL urand(6000, 14000)

#define EVENT_FLICKERING_FLAME_INTERVAL urand(6000, 14000)
#define EVENT_SKIN_LIKE_WAX_INTERVAL 10000
#define firebreathinterval urand(8000, 30000)
#define toxicinterval urand(5000, 20000)
#define brutishforceinterval 25000
#define shadowbreathinterval urand(5000, 25000)

uint32 GetWovenGuardCount(WorldObject* owner)
{
    std::list <Creature*> WovenBoneguards;
    GetCreatureListWithEntryInGrid(WovenBoneguards, owner, NPC_WOVEN_BONEGUARD, 80.0f);

    if (WovenBoneguards.empty())
        return 0;
    return WovenBoneguards.size();
}

// Chillheart intro door 211259
class go_chillheart_iron_door : public GameObjectScript
{
    public:
        go_chillheart_iron_door() : GameObjectScript("go_chillheart_iron_door") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (Creature* Chillheart = GetClosestCreatureWithEntry(go, NPC_INSTRUCTOR_CHILLHEART, 150.0f, true))
                Chillheart->AI()->DoAction(ACTION_INTRO);

            if (Creature* TalkingSkull = GetClosestCreatureWithEntry(go, NPC_TALKING_SKULL, 150.0f, true))
                TalkingSkull->AI()->DoAction(ACTION_INTRO);

            go->UseDoorOrButton();
            return true;
        }
};

// Polyformic acid potion 211669
class go_polyformic_acid_potion : public GameObjectScript
{
    public:
        go_polyformic_acid_potion() : GameObjectScript("go_alchemy_bottle_white") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (player)
                player->CastSpell(player, SPELL_DRINK_VIAL, false);

            if (go)
                go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);

            return true;
        }
};

// Boneweaver 59193
class npc_boneweaver : public CreatureScript
{
    public:
        npc_boneweaver() : CreatureScript("npc_boneweaver") { }

        struct npc_boneweaverAI : public ScriptedAI
        {
            npc_boneweaverAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

            EventMap events, nonCombatEvents;
            float PosY;

            void IsSummonedBy(Unit* summoner) override { }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->CastSpell(me, SPELL_BONEWEAVING, false);
                nonCombatEvents.ScheduleEvent(EVENT_SUMMON_WOVEN_BONEGUARD, 3000);
            }

            void InitializeAI() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->CastSpell(me, SPELL_BONEWEAVING, false);
                nonCombatEvents.ScheduleEvent(EVENT_SUMMON_WOVEN_BONEGUARD, 3000);
            }

            void JustSummoned(Creature* summon) override 
            {
                if (Creature* Lilian = GetClosestCreatureWithEntry(me, NPC_LILIAN_VOSS, 60.0f, true))
                    summon->GetMotionMaster()->MoveChase(Lilian);
            }

            void DoAction(int32 actionId) override { }

            void DamageTaken(Unit* attacker, uint32& damage) override { }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_BONE_SHARDS, 3000);
                events.ScheduleEvent(EVENT_SUMMON_WOVEN_BONEGUARD, urand(8000, 9000));
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);
                nonCombatEvents.Update(diff);

                if (uint32 eventId = nonCombatEvents.ExecuteEvent())
                    if (eventId == EVENT_SUMMON_WOVEN_BONEGUARD)
                    {
                        if (GetWovenGuardCount(me) > 6)
                            return;
                        me->SummonCreature(NPC_WOVEN_BONEGUARD, me->GetPositionX() + frand(-5.0f, 5.0f), me->GetPositionY() + frand(-6.0f, 6.0f), me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN);
                    }

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BONE_SHARDS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                                me->CastSpell(target, SPELL_BONE_SHARDS, false);
                            events.ScheduleEvent(EVENT_BONE_SHARDS, urand(3000, 4000));
                            break;
                        case EVENT_SUMMON_WOVEN_BONEGUARD:
                            me->SummonCreature(NPC_WOVEN_BONEGUARD, me->GetPositionX() + frand(-5.0f, 5.0f), me->GetPositionY() + frand(-6.0f, 6.0f), me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN);
                            events.ScheduleEvent(EVENT_SUMMON_WOVEN_BONEGUARD, urand(8000, 16000));
                            break;
                    }
                }
            }

        private:
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_boneweaverAI(creature);
        }
};

// Acolyte 58757
class npc_scholomance_acolyte : public CreatureScript
{
    public:
        npc_scholomance_acolyte() : CreatureScript("npc_scholomance_acolyte") { }

        struct npc_scholomance_acolyteAI : public ScriptedAI
        {
            npc_scholomance_acolyteAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                me->AddAura(SPELL_SHADOW_FORM_VISUAL, me);
                me->SetReactState(REACT_DEFENSIVE);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (me->HasAura(SPELL_VISUAL_STRANGULATE_EMOTE))
                    me->RemoveAura(SPELL_VISUAL_STRANGULATE_EMOTE);

                events.ScheduleEvent(EVENT_CAST_SHADOW_BOLT, SHADOW_BOLT_INTERVAL);
                events.ScheduleEvent(EVENT_CAST_SHATTER_SOUL, SHATTER_SOUL_INTERVAL);
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
                    case EVENT_CAST_SHADOW_BOLT:
                        if (Unit* target = me->GetVictim())
                            me->CastSpell(target, SPELL_SHADOW_BOLT);
                        events.ScheduleEvent(EVENT_CAST_SHADOW_BOLT, SHADOW_BOLT_INTERVAL);
                        break;
                    case EVENT_CAST_SHATTER_SOUL:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0F, true))
                            me->CastSpell(target, SPELL_SHATTER_SOUL);
                        events.ScheduleEvent(EVENT_CAST_SHATTER_SOUL, SHATTER_SOUL_INTERVAL);
                        break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scholomance_acolyteAI(creature);
        }
};

// Neophyte 58823
class npc_scholomance_neophyte : public CreatureScript
{
    public:
        npc_scholomance_neophyte() : CreatureScript("npc_scholomance_neophyte") { }

        struct npc_scholomance_neophyteAI : public ScriptedAI
        {
            npc_scholomance_neophyteAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (me->HasAura(SPELL_VISUAL_STRANGULATE_EMOTE))
                    me->RemoveAura(SPELL_VISUAL_STRANGULATE_EMOTE);

                if (me->HasAura(SPELL_SHADOW_FORM_VISUAL))
                    me->RemoveAura(SPELL_SHADOW_FORM_VISUAL);

                me->SetReactState(REACT_DEFENSIVE);

                events.ScheduleEvent(EVENT_CAST_REND_FLESH, REND_FLESH_INTERVAL);
                events.ScheduleEvent(EVENT_CAST_SHADOW_INCINERATION, SHADOW_INCINERATION_INTERVAL);
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
                        case EVENT_CAST_REND_FLESH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0F, true))
                                me->CastSpell(target, SPELL_REND_FLESH_HIGH_LEVEL);
                            events.ScheduleEvent(EVENT_CAST_REND_FLESH, REND_FLESH_INTERVAL);
                            break;
                        case EVENT_CAST_SHADOW_INCINERATION:
                            if (Unit* target = me->GetVictim())
                                me->CastSpell(target, SPELL_SHADOW_INCINERATION);
                            events.ScheduleEvent(EVENT_CAST_SHADOW_INCINERATION, SHADOW_INCINERATION_INTERVAL);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scholomance_neophyteAI(creature);
        }
};

// Risen Guard 58822
class npc_scholomance_risen_guard : public CreatureScript
{
    public:
        npc_scholomance_risen_guard() : CreatureScript("npc_scholomance_risen_guard") { }

        struct npc_scholomance_risen_guardAI : public ScriptedAI
        {
            npc_scholomance_risen_guardAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                SetEquipmentSlots(false, RisenGuardSword, 0, EQUIP_NO_CHANGE);
            }

            void InitializeAI() override
            {
                SetEquipmentSlots(false, RisenGuardSword, 0, EQUIP_NO_CHANGE);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_IMPALE, EVENT_IMPALE_INTERVAL);
                events.ScheduleEvent(EVENT_UNHOLY_WEAPON, EVENT_UNHOLY_WEAPON_INTERVAL);
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
                        case EVENT_IMPALE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0F, true))
                                me->CastSpell(target, SPELL_IMPALE);

                            events.ScheduleEvent(EVENT_IMPALE, EVENT_IMPALE_INTERVAL);
                            break;
                        case EVENT_UNHOLY_WEAPON:
                            if (Unit* target = me->GetVictim())
                                me->CastSpell(target, SPELL_UNHOLY_WEAPON);

                            events.ScheduleEvent(EVENT_UNHOLY_WEAPON, EVENT_UNHOLY_WEAPON_INTERVAL);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scholomance_risen_guardAI(creature);
        }
};

// Shatter soul fragment 58758
class npc_shatter_soul_fragment : public CreatureScript
{
    public:
        npc_shatter_soul_fragment() : CreatureScript("npc_shatter_soul_fragment") { }

        struct npc_shatter_soul_fragmentAI : public ScriptedAI
        {
            npc_shatter_soul_fragmentAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                me->SetFaction(16);

                me->DespawnOrUnsummon(8000);
            }

            void IsSummonedBy(Unit* summoner) override 
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                me->SetFaction(16);

                me->DespawnOrUnsummon(8000);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (Unit* target = me->GetVictim())
                    me->CastSpell(target, SPELL_SPIRIT_BARRAGE);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shatter_soul_fragmentAI(creature);
        }
};

// World trigger candle 59481
struct npc_scholomance_dark_candle : public ScriptedAI
{
    npc_scholomance_dark_candle(Creature* creature) : ScriptedAI(creature) {}

    uint32 candleCount;

    void InitializeAI() override
    {
        SetCombatMovement(false);

        switch (me->GetMapId())
        {
            case 1007: // Scholomance
                candleCount = 0;
                me->CastSpell(me, SPELL_SUMMON_BLACK_CANDLE);
                me->CastSpell(me, SPELL_BLACK_CANDLE_SPELL);
                break;
            case 1009: // Heart of Fear
                DoCast(me, 125270);
                break;
        }
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_CANDLESTICK_DEATH)
        {
            candleCount++;

            if (candleCount >= 2)
            {
                me->RemoveAurasDueToSpell(SPELL_SUMMON_BLACK_CANDLE);
                me->RemoveAurasDueToSpell(SPELL_BLACK_CANDLE_SPELL);
            }
        }
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Candlestick mage 59467
class npc_scholomance_candlestick_mage : public CreatureScript
{
    public:
        npc_scholomance_candlestick_mage() : CreatureScript("npc_scholomance_candlestick_mage") { }

        enum iEvents
        {
            EVENT_INIT = 1,
        };

        struct npc_scholomance_candlestick_mageAI : public ScriptedAI
        {
            npc_scholomance_candlestick_mageAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events, nonCombatEvents;
            ObjectGuid CandleGUID;

            void InitializeAI() override
            {
                CandleGUID.Clear();
                nonCombatEvents.ScheduleEvent(EVENT_INIT, 2 * IN_MILLISECONDS);
                Reset();
            }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_FLICKERING_FLAME, EVENT_FLICKERING_FLAME_INTERVAL);
                events.ScheduleEvent(EVENT_SKIN_LIKE_WAX, EVENT_SKIN_LIKE_WAX_INTERVAL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* candle = ObjectAccessor::GetCreature(*me, CandleGUID))
                    candle->AI()->DoAction(ACTION_CANDLESTICK_DEATH);
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_INIT)
                    {
                        if (Creature* candle = me->FindNearestCreature(NPC_CANDLE, 10.0f, true))
                        {
                            CandleGUID = candle->GetGUID();
                            me->SetFacingToObject(candle);
                            me->CastSpell(me, SPELL_TWILING_CHANNEL);
                        }
                    }
                    break;
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FLICKERING_FLAME:
                            me->CastSpell(me->GetVictim(), SPELL_FLICKERING_FLAME);
                            events.ScheduleEvent(EVENT_FLICKERING_FLAME, EVENT_FLICKERING_FLAME_INTERVAL);
                            break;
                        case EVENT_SKIN_LIKE_WAX:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0F, true))
                                me->CastSpell(target, SPELL_SKIN_LIKE_WAX);

                            events.ScheduleEvent(EVENT_SKIN_LIKE_WAX, EVENT_SKIN_LIKE_WAX_INTERVAL);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scholomance_candlestick_mageAI(creature);
        }
};

enum KrastTexts
{
    TALK_KRAST_AGGRO = 0,
    TALK_KRAST_DEAD,
};

// Krastinoc carvers 59368
class npc_scholomance_krastinoc_carvers : public CreatureScript
{
    public:
        npc_scholomance_krastinoc_carvers() : CreatureScript("npc_scholomance_krastinoc_carvers") { }

        struct npc_scholomance_krastinoc_carversAI : public ScriptedAI
        {
            npc_scholomance_krastinoc_carversAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                switch (urand(0, 5))
                {
                    case 0:
                        me->CastSpell(me, 124064); // book
                        break;
                }

                me->CastSpell(me, SPELL_BOLIDING_BLOODTHIRST);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(TALK_KRAST_AGGRO);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(TALK_KRAST_DEAD);
                DoCast(me, SPELL_BOILING_BLOODTHIRST);

                if (Aura* BloodAura = me->GetAura(SPELL_BOILING_BLOODTHIRST_AURA))
                    if (BloodAura->GetStackAmount() >= 99)
                        DoCast(me, SPELL_KRASTINOV_ACHIEVEMENT_SPELL);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                DoMeleeAttackIfReady();
            }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scholomance_krastinoc_carversAI(creature);
        }
};

// Flesh horror 59359
class npc_scholomance_flesh_horror : public CreatureScript
{
    public:
        npc_scholomance_flesh_horror() : CreatureScript("npc_scholomance_flesh_horror") { }

        struct npc_scholomance_flesh_horrorAI : public ScriptedAI
        {
            npc_scholomance_flesh_horrorAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            std::list<ObjectGuid> meat;
            uint32 VehPos;

            void Reset() override
            {
                RemoveCraftMeat();
                VehPos = 0;
            }

            void InitializeAI() override 
            {
                VehPos = 0;
            }

            void RemoveCraftMeat()
            {
                for (auto&& itr : meat)
                    if (Creature* CraftMeat = ObjectAccessor::GetCreature(*me, itr))
                        CraftMeat->DespawnOrUnsummon();

                meat.clear();
            }

            bool HasGraftsDeath()
            {
                for (auto&& itr : meat)
                    if (Creature* graft = ObjectAccessor::GetCreature(*me, itr))
                        if (graft->IsAlive()) // we need check once
                            return false;
                
                return true;
            }

            void JustEngagedWith(Unit* /*who*/) override 
            {
                Position pos = me->GetRandomNearPosition(8.0f);

                for (uint8 i = 0; i < 4; i++)
                {
                    if (Creature* graft = me->SummonCreature(NPC_MEAT_GRAFT, pos, TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        if (!IsHeroic())
                        {
                            me->SetMaxHealth(9800);
                            me->SetHealth(9800);
                            graft->SetLevel(39);
                        }

                        graft->EnterVehicle(me, VehPos);
                        graft->SetReactState(REACT_PASSIVE);
                        graft->AddUnitState(UNIT_STATE_CANNOT_AUTOATTACK);

                        meat.push_back(graft->GetGUID());
                        VehPos++;
                    }
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                RemoveCraftMeat();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->GetHealthPct() < 95 && !HasGraftsDeath())
                    me->CastSpell(me, SPELL_CLOTTING, false);

                if (HasGraftsDeath())
                {
                    me->Kill(me);
                    me->DespawnOrUnsummon(10000);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scholomance_flesh_horrorAI(creature);
        }
};

// Bored student 59614
class npc_scholomance_bored_student : public CreatureScript
{
    public:
        npc_scholomance_bored_student() : CreatureScript("npc_scholomance_bored_student") { }

        struct npc_scholomance_bored_studentAI : public ScriptedAI
        {
            npc_scholomance_bored_studentAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override { }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SHADOW_NOVA, firebreathinterval);
                events.ScheduleEvent(EVENT_FIRE_BREATH, shadowbreathinterval);
            }
            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHADOW_NOVA:
                            me->CastSpell(me->GetVictim(), SPELL_SHADOW_NOVA_BORED_STUDENT);
                            events.ScheduleEvent(EVENT_SHADOW_NOVA, shadowbreathinterval);
                            break;
                        case EVENT_FIRE_BREATH:
                            me->CastSpell(me->GetVictim(), SPELL_FIRE_BREATH_POTION_BORED_STUDENT);
                            events.ScheduleEvent(EVENT_FIRE_BREATH, firebreathinterval);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scholomance_bored_studentAI(creature);
        }
};

// Professor Slait 59613
class npc_scholomance_professor : public CreatureScript
{
public:
        npc_scholomance_professor() : CreatureScript("npc_scholomance_professor") { }

        struct npc_scholomance_professorAI : public ScriptedAI
        {
            npc_scholomance_professorAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override { }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_BRUTISH_FORCE, brutishforceinterval);
                events.ScheduleEvent(EVENT_TOXIC_POTION, toxicinterval);
                events.ScheduleEvent(EVENT_FIRE_BREATH, firebreathinterval);
            }
            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                {
                    switch (urand(0, 20))
                    {
                        case 0:
                            me->CastSpell(me, 79506); // emote talk
                            break;
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FIRE_BREATH:
                            me->CastSpell(me->GetVictim(), SPELL_FIRE_BREATH_POTION_BORED_STUDENT);
                            events.ScheduleEvent(EVENT_FIRE_BREATH, firebreathinterval);
                            break;
                        case EVENT_TOXIC_POTION:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                                me->CastSpell(target, SPELL_TOXIC_POTION);
                            events.ScheduleEvent(EVENT_FIRE_BREATH, toxicinterval);
                            break;
                        case EVENT_BRUTISH_FORCE:
                            me->CastSpell(me, SPELL_POTION_OF_BRUTISH_FORCE);
                            events.ScheduleEvent(EVENT_BRUTISH_FORCE, brutishforceinterval);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scholomance_professorAI(creature);
        }
};

// Gandling at Rattlegore/lilian 58875
class npc_gandling_at_rattlegore : public CreatureScript
{
    public:
        npc_gandling_at_rattlegore() : CreatureScript("npc_gandling_at_rattlegore") { }

        struct npc_gandling_at_rattlegoreAI : public ScriptedAI
        {
            npc_gandling_at_rattlegoreAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

            EventMap nonCombatEvents;
            float PosY;
            bool AtRattlegore, _introDone;

            void IsSummonedBy(Unit* summoner) override { }

            void Reset() override
            {
                me->AddAura(SPELL_BONE_ARMOR_VISUAL, me);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                SetEquipmentSlots(false, GandlingStaff, 0, EQUIP_NO_CHANGE);
            }

            void InitializeAI() override
            {
                me->AddAura(SPELL_BONE_ARMOR_VISUAL, me);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                SetEquipmentSlots(false, GandlingStaff, 0, EQUIP_NO_CHANGE);
                AtRattlegore = false;
                _introDone = false;
            }

            void JustSummoned(Creature* summon) override { }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_INTRO:
                        Talk(TALK_1);
                        if (Creature* Lilian = GetClosestCreatureWithEntry(me, NPC_LILIAN_VOSS, 50.0f, true))
                            Lilian->AI()->DoAction(ACTION_INTRO);
                        nonCombatEvents.ScheduleEvent(EVENT_TALK_1, 6000);
                        break;
                    case ACTION_DIFF_INTRO:
                        Talk(TALK_10);
                        break;
                    case ACTION_REMOVE_LILIAN_SOUL:
                        Talk(TALK_11);
                        nonCombatEvents.ScheduleEvent(EVENT_TALK_9, 4500);
                        break;
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override { }

            void JustEngagedWith(Unit* who) override { }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case 0:
                        Talk(TALK_7);
                        me->CastSpell(me, SPELL_TELEPORT_VISUAL, false);
                        me->SetAnimTier(AnimTier::Fly);
                        me->OverrideInhabitType(INHABIT_AIR);
                        me->UpdateMovementFlags();
                        me->CastSpell(me, SPELL_SOUL_RIP_VISUAL, false);
                        me->NearTeleportTo(me->GetPositionX(), me->GetPositionY() + 10.0f, me->GetPositionZ(), me->GetOrientation());
                        nonCombatEvents.ScheduleEvent(EVENT_SPECIAL, 1500);
                        break;
                    case 1:
                        me->CastSpell(me, SPELL_TELEPORT_VISUAL, false);
                        me->DespawnOrUnsummon();
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                if (!me->HasAura(SPELL_BONE_ARMOR_VISUAL) && !AtRattlegore)
                    me->AddAura(SPELL_BONE_ARMOR_VISUAL, me);

                if (AtRattlegore && me->FindNearestPlayer(25.0f) && me->FindNearestPlayer(25.0f)->GetPositionZ() <= 109.0f)
                {
                    AtRattlegore = false;
                    Talk(TALK_8);
                    nonCombatEvents.ScheduleEvent(EVENT_TALK_8, 8000);
                    if (Creature* Lilian = GetClosestCreatureWithEntry(me, NPC_LILIAN_VOSS, 50.0f, true))
                        Lilian->AI()->DoAction(ACTION_DIFF_INTRO);

                    if (Creature* Skull = GetClosestCreatureWithEntry(me, NPC_TALKING_SKULL, 80.0f, true))
                        Skull->AI()->DoAction(ACTION_COUNTINUE_MOVE);
                }

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TALK_1:
                            Talk(TALK_2);
                            nonCombatEvents.ScheduleEvent(EVENT_TALK_2, 10000);
                            break;
                        case EVENT_TALK_2:
                            Talk(TALK_3);
                            nonCombatEvents.ScheduleEvent(EVENT_TALK_3, 10000);
                            break;
                        case EVENT_TALK_3:
                            Talk(TALK_4);
                            nonCombatEvents.ScheduleEvent(EVENT_TALK_4, 10000);
                            break;
                        case EVENT_TALK_4:
                            Talk(TALK_5);
                            nonCombatEvents.ScheduleEvent(EVENT_TALK_5, 10000);
                            break;
                        case EVENT_TALK_5:
                            Talk(TALK_6);
                            nonCombatEvents.ScheduleEvent(EVENT_TALK_6, 6000);
                            break;
                        case EVENT_TALK_6:
                            me->SetAnimTier(AnimTier::Ground);
                            me->OverrideInhabitType(INHABIT_GROUND);
                            me->UpdateMovementFlags();
                            me->RemoveAurasDueToSpell(SPELL_SOUL_RIP_VISUAL);
                            me->GetMotionMaster()->MovePoint(0, GandlinglilianDoorPoint);
                            break;
                        case EVENT_TALK_8:
                            Talk(TALK_9);
                            break;
                        case EVENT_TALK_9:
                            Talk(TALK_12);
                            if (Unit* TalkingSkull = ObjectAccessor::GetUnit(*me, _instance->GetGuidData(NPC_TALKING_SKULL)))
                                TalkingSkull->ToCreature()->AI()->Talk(TALK_GANDLING_LEAVE);

                            me->SetAnimTier(AnimTier::Ground);
                            me->OverrideInhabitType(INHABIT_GROUND);
                            me->UpdateMovementFlags();
                            me->RemoveAurasDueToSpell(SPELL_SOUL_RIP_VISUAL);
                            me->GetMotionMaster()->MovePoint(1, GandlingLeavePoint);
                            break;
                        case EVENT_SPECIAL:
                            me->NearTeleportTo(GandlingTeleportPos.GetPositionX(), GandlingTeleportPos.GetPositionY(), GandlingTeleportPos.GetPositionZ(), GandlingTeleportPos.GetOrientation());
                            me->SetHomePosition(GandlingTeleportPos.GetPositionX(), GandlingTeleportPos.GetPositionY(), GandlingTeleportPos.GetPositionZ(), GandlingTeleportPos.GetOrientation());
                            AtRattlegore = true;
                            me->CastSpell(me, SPELL_GANDLING_INTRO_CHANNEL, false);
                            break;
                    }
                }
            }

        private:
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_gandling_at_rattlegoreAI(creature);
        }
};

// Talking Skull 64562
class npc_talking_skull : public CreatureScript
{
    public:
        npc_talking_skull() : CreatureScript("npc_talking_skull") { }

        struct npc_talking_skullAI : public ScriptedAI
        {
            npc_talking_skullAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap nonCombatEvents;
            uint32 wp;
            bool AtGandling;

            void Reset() override { }

            void InitializeAI() override
            {
                wp = 0;
                AtGandling = false;
                me->SetWalk(true);
                me->SetSpeed(MOVE_WALK, 2.3f);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_COUNTINUE_MOVE:
                    case ACTION_INTRO:
                        if (!AtGandling)
                            nonCombatEvents.ScheduleEvent(EVENT_ESCORT, urand(1000, 2000));
                        break;
                    case ACTION_ALEXEI:
                        //Talk(...)
                        nonCombatEvents.ScheduleEvent(EVENT_FINALY_FORM, urand(3000, 5000));
                        break;
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                wp++;

                switch (pointId)
                {
                    case 0:
                        Talk(TALK_CHILLHEART_ENTRANCE);
                        Talk(TALK_CHILLHEART);
                        me->GetMotionMaster()->Clear();
                        break;
                    case 9:
                        Talk(TALK_JANDICE_ENTRANCE);
                        me->GetMotionMaster()->Clear();
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 4000);
                        break;
                    case 10:
                        me->GetMotionMaster()->Clear();
                        break;
                    case 11:
                        Talk(TALK_RATTLEGORE_ENTRANCE);
                        me->GetMotionMaster()->Clear();
                        break;
                    case 14:
                        me->GetMotionMaster()->Clear();
                        break;
                    case 16:
                        Talk(TALK_LILIAN_ENTRANCE);
                        me->GetMotionMaster()->Clear();
                        break;
                    case 29:
                        Talk(TALK_GANDLING_ENTRANCE);
                        AtGandling = true;
                        me->GetMotionMaster()->Clear();
                        break;
                    default:
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, urand(100, 200));
                        break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override { }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                    case EVENT_ESCORT:
                        me->GetMotionMaster()->MovePoint(wp, SkullPath[wp]);
                        break;
                    case EVENT_FINALY_FORM:
                        if (Creature* Alexi = me->SummonCreature(NPC_ALEXIY_BAROV, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                            me->DespawnOrUnsummon();
                        break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_talking_skullAI(creature);
        }
};

// Polymorfic acid 114800
class spell_polymorfic_acid_potion : public SpellScriptLoader
{
    public:
        spell_polymorfic_acid_potion() : SpellScriptLoader("spell_polymorfic_acid_potion") { }

        class spell_polymorfic_acid_potion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_polymorfic_acid_potion_AuraScript);

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                if (Unit* owner = GetOwner()->ToUnit())
                    owner->CastSpell(owner, SPELL_DRINK_VIAL_PROFESSOR_MORPH, false);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* owner = GetOwner()->ToUnit())
                    if (owner->HasAura(SPELL_DRINK_VIAL_PROFESSOR_MORPH))
                        owner->RemoveAura(SPELL_DRINK_VIAL_PROFESSOR_MORPH);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_polymorfic_acid_potion_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_polymorfic_acid_potion_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_polymorfic_acid_potion_AuraScript();
        }
};

// Boiling Bloodthirst 114155
class spell_boiling_bloodthirst : public SpellScriptLoader
{
    public:
        spell_boiling_bloodthirst() : SpellScriptLoader("spell_boiling_bloodthirst") { }

        class spell_boiling_bloodthirst_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_boiling_bloodthirst_SpellScript);

            void CheckTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_KRASTINOV_CARVER; });
            }

            void HandleOnHitEffect(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetHitUnit())
                        if (Aura* CasterBloodAura = caster->GetAura(SPELL_BOILING_BLOODTHIRST_AURA))
                            if (Aura* TargetBloodAura = target->GetAura(SPELL_BOILING_BLOODTHIRST_AURA))
                                TargetBloodAura->SetStackAmount(TargetBloodAura->GetStackAmount() + CasterBloodAura->GetStackAmount() - 1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_boiling_bloodthirst_SpellScript::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnEffectHitTarget += SpellEffectFn(spell_boiling_bloodthirst_SpellScript::HandleOnHitEffect, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_boiling_bloodthirst_SpellScript();
        }
};

void AddSC_scholomance()
{
    new go_chillheart_iron_door();
    new go_polyformic_acid_potion();
    new npc_boneweaver();
    new npc_scholomance_acolyte();
    new npc_scholomance_neophyte();
    new npc_scholomance_risen_guard();
    new npc_shatter_soul_fragment();
    new creature_script<npc_scholomance_dark_candle>("npc_scholomance_dark_candle");
    new npc_scholomance_candlestick_mage();
    new npc_scholomance_krastinoc_carvers();
    new npc_scholomance_flesh_horror();
    new npc_scholomance_bored_student();
    new npc_scholomance_professor();
    new npc_gandling_at_rattlegore();
    new npc_talking_skull();
    new spell_polymorfic_acid_potion();
    new spell_boiling_bloodthirst();
}
