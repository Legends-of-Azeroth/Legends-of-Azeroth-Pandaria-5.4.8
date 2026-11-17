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
#include "Vehicle.h"
#include "scarlet_halls.h"

enum Spells
{
    SPELL_HUNGRY_DOG          = 111582,
    SPELL_INQUISITION         = 111341,
    SPELL_SEAL_OF_BLOOD       = 111351,
    SPELL_RAZOR_EDGE          = 84146,
    SPELL_UNFLINCHING_RESOLVE = 111418,
    SPELL_HOLY_SMITE_N        = 9734,
    SPELL_HOLY_SMITE_H        = 128233,
    SPELL_HOLY_FIRE_N         = 17140,
    SPELL_HOLY_FIRE_H         = 128232,
    SPELL_FANATICISM          = 111376,
    SPELL_COMMANDING_SHOUT    = 80983,
    SPELL_DISARM              = 6713,
    SPELL_SHIELD_BARRIER      = 111324,
    SPELL_FIREBALL            = 128249,
    SPELL_FLAMESTRIKE         = 111667,
    SPELL_JOLT                = 114000,
    SPELL_SLASH_ARMOR         = 114011,
    SPELL_TOSS_STATUE         = 114022,
    SPELL_SCARLET_PROTECTION  = 114185,
    SPELL_FIREBALL_VALLEY     = 114182,
    SPELL_LEASHED             = 103169,
    SPELL_ARCHERY_VEHICLE     = 114877,
    SPELL_SUMMON_CANNONEER    = 113933,
    SPELL_FIRE_CANNON_DUMMY   = 113932,
    SPELL_CANNON_BLAST        = 113719,
    SPELL_PIERCING_ARROW      = 113564,
    SPELL_SHIELD_BREAKER      = 125492,
    SPELL_CARRYING_BUCKET     = 128164,
    SPELL_BEATDOWN            = 114184,
    SPELL_BLEEDING_WOUND      = 113855,
    SPELL_DOG_FOOD            = 111894,
    SPELL_EXPLODING_SHOT      = 113644,
    SPELL_EXPLODING_GROUND    = 114861,
    SPELL_DOG_LEAP            = 122929,
    SPELL_EATEN               = 122916,
    SPELL_PLAYER_VEHICLE_AURA = 113399,
};

enum Events
{
    EVENT_HUNGRY_DOG          = 1,
    EVENT_RAZOR_EDGE          = 2,
    EVENT_HOLY_SMITE          = 3,
    EVENT_HOLY_FIRE           = 4,
    EVENT_DISARM              = 5,
    EVENT_FIREBALL            = 6,
    EVENT_FLAMESTRIKE         = 7,
    EVENT_JOLT                = 8,
    EVENT_SLASH_ARMOR         = 9,
    EVENT_TOSS_STATUE         = 10,
    EVENT_SCARLET_PROTECTION  = 11,
    EVENT_FIREBALL_VALLEY     = 12,
    EVENT_CANNON_READY        = 13,
    EVENT_ESCORT              = 14,
    EVENT_FIRE_AT_LINE        = 15,
    EVENT_SHIELD_BREAKER      = 16,
    EVENT_BEATDOWN            = 17,
    EVENT_BLEEDING_WOUND      = 18,
    EVENT_EXPLODING_GROUND    = 19,
};

enum Weapons
{
    W_DRAGON_REACH            = 19353,
    W_REMORSE                 = 45142,
    W_MOULDERING_CORPSE       = 49793,
    W_MAGIC_SHIELD            = 45682,
    W_COURAGE                 = 37401,
};

// Starving hound 58876
class npc_starving_hound : public CreatureScript
{
    public:
        npc_starving_hound() : CreatureScript("npc_starving_hound") { }

        struct npc_starving_houndAI : public ScriptedAI
        {
            npc_starving_houndAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            bool attacked;

            void Reset() override
            {
                events.Reset();
                attacked = false;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_HUNGRY_DOG, urand(8000, 15000));
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_HUMANE_SOCIETY, 0);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (CanAttackMyOwner() && !attacked)
                {
                    attacked = true;
                    me->SetFaction(15);

                    if (Unit* owner = GetClosestCreatureWithEntry(me, NPC_VIGILANT_WATCHMAN, 30.0f, true))
                        me->Attack(owner, true);
                }

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_HUNGRY_DOG)
                    {
                        if (Unit* Vict = me->GetVictim())
                            me->CastSpell(Vict, SPELL_HUNGRY_DOG, false);
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }

        private:
            bool CanAttackMyOwner()
            {
                if (Unit* owner = GetClosestCreatureWithEntry(me, NPC_VIGILANT_WATCHMAN, 30.0f, true))
                    if (owner->HasAura(SPELL_DOG_FOOD))
                        return true;

                return false;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_starving_houndAI(creature);
        }
};

// Angry Hound 58674
class npc_angry_hound : public CreatureScript
{
    public:
        npc_angry_hound() : CreatureScript("npc_angry_hound") { }

        enum eEvents
        {
            EVENT_LEASHED = 1,
        };

        struct npc_angry_houndAI : public ScriptedAI
        {
            npc_angry_houndAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap nonCombatEvents;
            bool AnyUnitAtStake;

            void Reset() override
            {
                if (Creature* stake = GetClosestCreatureWithEntry(me, NPC_STAKE, 20.0f, true))
                    stake->CastSpell(me, SPELL_LEASHED, false);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                AnyUnitAtStake = false;
            }

            void InitializeAI() override
            {
                if (Creature* stake = GetClosestCreatureWithEntry(me, NPC_STAKE, 20.0f, true)) // we need solve problem when creature create is comes later then we are.
                    stake->CastSpell(me, SPELL_LEASHED, false);
                else nonCombatEvents.ScheduleEvent(EVENT_LEASHED, 1000);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);;
                AnyUnitAtStake = false;
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_HUMANE_SOCIETY, 0);
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                if (!AnyUnitAtStake)
                    if (Player* itr = me->FindNearestPlayer(10.0f))
                    {
                        if (me->GetDistance2d(itr) < 4.0f)
                        {
                            AnyUnitAtStake = true;
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                            me->Attack(itr, true);
                            me->GetMotionMaster()->MoveChase(itr);
                        }
                    }

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_LEASHED)
                    {
                        if (Creature* stake = GetClosestCreatureWithEntry(me, NPC_STAKE, 20.0f, true)) // till creature not`ll load
                            stake->CastSpell(me, SPELL_LEASHED, false);

                        else nonCombatEvents.ScheduleEvent(EVENT_LEASHED, 1000);
                    }
                    break;
                }

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 4.0f);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_angry_houndAI(creature);
        }
};

// Bucket of Meaty Dog Food 65379
class npc_bucket_of_meaty_dog_food : public CreatureScript
{
    public:
        npc_bucket_of_meaty_dog_food() : CreatureScript("npc_bucket_of_meaty_dog_food") { }

        struct npc_bucket_of_meaty_dog_foodAI : public ScriptedAI
        {
            npc_bucket_of_meaty_dog_foodAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

            EventMap events;

            void IsSummonedBy(Unit* summoner) override { }

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            }

            void OnSpellClick(Unit* clicker, bool& /*result*/) override
            {
                clicker->CastSpell(clicker, SPELL_CARRYING_BUCKET, false);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->DespawnOrUnsummon(1500);
            }

            void UpdateAI(uint32 diff) override { }

            private:
                InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_bucket_of_meaty_dog_foodAI(creature);
        }
};

// Reinforced  archery target 59163
class npc_reinforced_archery_target : public CreatureScript
{
    public:
        npc_reinforced_archery_target() : CreatureScript("npc_reinforced_archery_target") { }

        enum eTalks
        {
            TALK_INTRO = 1,
        };

        struct npc_reinforced_archery_targetAI : public ScriptedAI
        {
            npc_reinforced_archery_targetAI(Creature* creature) : ScriptedAI(creature) { }

            bool hasRider;

            void Reset() override 
            {
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                DoCast(me, SPELL_ARCHERY_VEHICLE, true);
                hasRider = false;
            }

            void OnSpellClick(Unit* clicker, bool& /*result*/) override
            {
                if (clicker->HasAura(SPELL_HEROIC_DEFENSE) || hasRider)
                    return;

                hasRider = true;
                Talk(TALK_INTRO);
                DoCast(clicker, SPELL_HEROIC_DEFENSE);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (damage >= me->GetHealth())
                    damage = 0;
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_reinforced_archery_targetAI(creature);
        }
};

// Commander Lindon 59191
class npc_commander_lindon : public CreatureScript
{
    public:
        npc_commander_lindon() : CreatureScript("npc_commander_lindon") { }

        enum eWeapon
        {
            WEAP_BLOW  = 55306,
            WEAP_SWORD = 56696,
        };

        enum eTalks
        {
            TALK_INTRO       = 1,
            TALK_RANGE_REACH = 2,
            TALK_MELEE_REACH = 3,
        };

        struct npc_commander_lindonAI : public BossAI
        {
            npc_commander_lindonAI(Creature* creature) : BossAI(creature, DATA_COMANDER_LINDON) { }

            EventMap nonCombatEvents;
            bool Melee, initCombat, explosion;

            void InitializeAI() override 
            {
                Melee      = false;
                initCombat = false;
                explosion  = false;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!initCombat && who && who->GetTypeId() == TYPEID_PLAYER)
                {
                    initCombat = true;
                    Talk(TALK_INTRO);
                    nonCombatEvents.ScheduleEvent(EVENT_FIRE_AT_LINE, urand(1000, 2000));
                    nonCombatEvents.ScheduleEvent(EVENT_SHIELD_BREAKER, urand(5000, 6000));
                    DoZoneInCombat();
                }
                ScriptedAI::MoveInLineOfSight(who);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_GROUND_EXPLOSION && !explosion)
                {
                    explosion = true;
                    Talk(TALK_RANGE_REACH);
                    nonCombatEvents.ScheduleEvent(EVENT_EXPLODING_GROUND, 500);
                }
            }

            void Reset() override 
            {
                _Reset();
                Melee = false;
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override 
            {
                _JustEngagedWith();
                HandleDoors();
                if (instance)
                    instance->SetData(DATA_COMANDER_LINDON, IN_PROGRESS);
            }

            void HandleDoors(bool reset = false)
            {
                if (instance)
                    if (GameObject* Idoor = GetClosestGameObjectWithEntry(me, GO_COMANDER_LINDON_EXIT, 150.0f))
                        instance->HandleGameObject(ObjectGuid::Empty, reset, Idoor);
            }

            void JustDied(Unit* /*killer*/) override 
            {
                _JustDied();
                if (instance)
                    instance->SetData(DATA_COMANDER_LINDON, DONE);

                HandleDoors(true);
            }

            void JustSummoned(Creature* summon) override
            {
                summon->SetDisplayFromModel(0);
            }

            void UpdateAI(uint32 diff) override 
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FIRE_AT_LINE:
                            SendArchersInCombat(ACTION_FIRE_AT_LINE);
                            break;
                        case EVENT_SHIELD_BREAKER:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, ShieldBreakerPredicate(me)))
                                me->CastSpell(target, SPELL_SHIELD_BREAKER, false);

                            nonCombatEvents.ScheduleEvent(EVENT_SHIELD_BREAKER, urand(2000, 3000));
                            break;
                        case EVENT_EXPLODING_GROUND:
                            float itter = 0.0f;
                            const Position currentPos = urand(0, 1) ? ExplodingStartPos1 : ExplodingStartPos2;

                            for (uint8 i = 0; i < 3; i++) // 3 pos for fire
                            {
                                me->CastSpell(currentPos.GetPositionX() + itter, currentPos.GetPositionY(), currentPos.GetPositionZ(), SPELL_EXPLODING_SHOT, true);
                                itter += 12.0f;
                            }
                            break;
                    }
                }

                if (!Melee)
                    if (Player* itr = me->FindNearestPlayer(10.0f))
                    {
                        Talk(TALK_MELEE_REACH);
                        nonCombatEvents.Reset();
                        Melee = true;
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                        me->AI()->AttackStart(itr);
                        me->GetMotionMaster()->MoveChase(itr);
                        SendArchersInCombat(ACTION_MELEE);
                        RemoveShieldFromPlayers();
                    }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                DoMeleeAttackIfReady();
            }

        private:
            void SendArchersInCombat(uint32 action)
            {
                std::list<Creature*> Archers;
                GetCreatureListWithEntryInGrid(Archers, me, NPC_MASTER_ARCHER, 30.0f);

                for (auto&& itr : Archers)
                    if (itr->IsAlive())
                        itr->AI()->DoAction(action);
            }

            struct ShieldBreakerPredicate 
            {
                public:
                    ShieldBreakerPredicate(Creature const* me) : _me(me) { }
                    bool operator()(Unit const* target) const
                    {
                        return target && target->GetTypeId() == TYPEID_PLAYER && target->GetDistance2d(_me) < 18.0f;
                    }

                private:
                    Creature const* _me;
            };

            void RemoveShieldFromPlayers()
            {
                std::list<Player*> PlayersWithShield;
                GetPlayerListInGrid(PlayersWithShield, me, 100.0f);

                for (auto&& itr : PlayersWithShield)
                    if (itr->HasAura(SPELL_HEROIC_DEFENSE))
                        itr->RemoveAura(SPELL_HEROIC_DEFENSE);
            }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_commander_lindonAI(creature);
        }
};

// Master Archer 59175
class npc_scarlet_master_archer : public CreatureScript
{
    public:
        npc_scarlet_master_archer() : CreatureScript("npc_scarlet_master_archer") { }

        enum eWeapon
        {
            WEAP_BLOW = 55306,
            WEAP_SWORD = 56696,
        };

        enum eEvents
        {
            EVENT_FIRE_AT_ROW    = 1,
            EVENT_BLEEDING_WOUND = 2,
        };

        struct npc_scarlet_master_archerAI : public ScriptedAI
        {
            npc_scarlet_master_archerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events, nonCombatEvents;

            void Reset() override
            {
                nonCombatEvents.Reset();
                events.Reset();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_BLEEDING_WOUND, urand(2500, 4500));
            }

            void DoAction(int32 actionId) override 
            {
                switch (actionId)
                {
                    case ACTION_FIRE_AT_LINE:
                        nonCombatEvents.ScheduleEvent(EVENT_FIRE_AT_ROW, urand(1000, 2000));
                        break;
                    case ACTION_MELEE:
                    {
                        if (Player* itr = me->FindNearestPlayer(10.0f))
                        {
                            nonCombatEvents.Reset();
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                            me->AI()->AttackStart(itr);
                            me->GetMotionMaster()->MoveChase(itr);
                        }
                    }
                    break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_FIRE_AT_ROW)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                            me->CastSpell(target, SPELL_PIERCING_ARROW, false);

                        nonCombatEvents.ScheduleEvent(EVENT_FIRE_AT_ROW, urand(1000, 2500));
                    }
                    break;
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_BLEEDING_WOUND)
                    {
                        if (Unit* vict = me->GetVictim())
                        {
                            if (me->GetDistance2d(vict) < 5.0f)
                                me->CastSpell(vict, SPELL_BLEEDING_WOUND, false);

                            events.ScheduleEvent(EVENT_BLEEDING_WOUND, urand(5500, 6500));
                        }
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scarlet_master_archerAI(creature);
        }
};

// Scarlet Scourge Hewer 58684
class npc_scarlet_scourge_hewer : public CreatureScript
{
    public:
        npc_scarlet_scourge_hewer() : CreatureScript("npc_scarlet_scourge_hewer") { }

        struct npc_scarlet_scourge_hewerAI : public ScriptedAI
        {
            npc_scarlet_scourge_hewerAI(Creature* creature) : ScriptedAI(creature) { }

            bool triggered;

            void Reset() override
            {
                triggered = false;
                SetEquipmentSlots(false, W_DRAGON_REACH, 0, EQUIP_NO_CHANGE);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->CastSpell(me, SPELL_SEAL_OF_BLOOD, false);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/) override
            {
                if (HealthBelowPct(50) && !triggered)
                {
                    triggered = true;
                    me->CastSpell(me, SPELL_INQUISITION, false);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scarlet_scourge_hewerAI(creature);
        }
};

// Scarlet Myrmidon 58683
class npc_scarlet_myrmidon : public CreatureScript
{
    public:
        npc_scarlet_myrmidon() : CreatureScript("npc_scarlet_myrmidon") { }

        struct npc_scarlet_myrmidonAI : public ScriptedAI
        {
            npc_scarlet_myrmidonAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            bool triggered;

            void Reset() override
            {
                triggered = false;
                events.Reset();
                SetEquipmentSlots(false, W_REMORSE, 0, EQUIP_NO_CHANGE);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_RAZOR_EDGE, urand(10000, 19000));
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/) override
            {
                if (HealthBelowPct(60) && !triggered)
                {
                    triggered = true;
                    me->CastSpell(me, SPELL_UNFLINCHING_RESOLVE, false);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                    if (eventId == EVENT_RAZOR_EDGE)
                    {
                        me->CastSpell(me, SPELL_RAZOR_EDGE, false);
                        events.ScheduleEvent(EVENT_RAZOR_EDGE, urand(10000, 19000));
                    }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scarlet_myrmidonAI(creature);
        }
};

// Scarlet Evangelist 58685
class npc_scarlet_evangelist : public CreatureScript
{
    public:
        npc_scarlet_evangelist() : CreatureScript("npc_scarlet_evangelist") { }

        struct npc_scarlet_evangelistAI : public ScriptedAI
        {
            npc_scarlet_evangelistAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            bool triggered;

            void Reset() override
            {
                triggered = false;
                events.Reset();
                SetEquipmentSlots(false, W_MOULDERING_CORPSE, 0, EQUIP_NO_CHANGE);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_HOLY_FIRE, urand(5000, 7000));
                events.ScheduleEvent(EVENT_HOLY_SMITE, urand(1500, 2500));
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (HealthBelowPct(60) && !triggered)
                {
                    triggered = true;
                    me->CastSpell(me, SPELL_FANATICISM, false);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_HOLY_FIRE:
                            if (Unit* vict = me->GetVictim())
                                IsHeroic() ? me->CastSpell(vict, SPELL_HOLY_FIRE_H, false) : me->CastSpell(vict, SPELL_HOLY_FIRE_N, false);
                            events.ScheduleEvent(EVENT_HOLY_FIRE, urand(5000, 7000));
                            break;
                        case EVENT_HOLY_SMITE:
                            if (Unit* vict = me->GetVictim())
                                IsHeroic() ? me->CastSpell(vict, SPELL_HOLY_SMITE_H, false) : me->CastSpell(vict, SPELL_HOLY_SMITE_N, false);
                            events.ScheduleEvent(EVENT_HOLY_SMITE, urand(1500, 2500));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scarlet_evangelistAI(creature);
        }
};

// Scarlet Cannoneer 59293
class npc_scarlet_cannoneer : public CreatureScript
{
    public:
        npc_scarlet_cannoneer() : CreatureScript("npc_scarlet_cannoneer") { }

        struct npc_scarlet_cannoneerAI : public ScriptedAI
        {
            npc_scarlet_cannoneerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            bool triggered;
            ObjectGuid CannonGuid;

            void IsSummonedBy(Unit* summoner) override 
            {
                if (!IsHeroic())
                {
                    me->SetLevel(28);
                    me->SetMaxHealth(1890);
                    me->SetHealth(1890);
                }

                if (summoner)
                    CannonGuid = summoner->GetGUID();

                triggered = false;
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                events.Reset();
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (!triggered)
                    if (Player* itr = me->FindNearestPlayer(20.0f))
                    {
                        triggered = true;
                        events.ScheduleEvent(EVENT_CANNON_READY, urand(1500, 4000));
                        me->Attack(itr, false);
                    }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_CANNON_READY)
                    {
                        me->CastSpell(me, SPELL_FIRE_CANNON_DUMMY, true);

                        if (Creature* CannonStalker = ObjectAccessor::GetCreature(*me, CannonGuid))
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true))
                                CannonStalker->CastSpell(target, SPELL_CANNON_BLAST, true);

                        events.ScheduleEvent(EVENT_CANNON_READY, urand(1500, 4000));
                    }
                }

            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scarlet_cannoneerAI(creature);
        }
};

// Scarlet Cannon stalker 58726
class npc_cannon_stalker : public CreatureScript
{
    public:
        npc_cannon_stalker() : CreatureScript("npc_cannon_stalker") { }

        struct npc_cannon_stalkerAI : public ScriptedAI
        {
            npc_cannon_stalkerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void InitializeAI() override
            {
                me->CastSpell(me, SPELL_SUMMON_CANNONEER, false);
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cannon_stalkerAI(creature);
        }
};

// Scarlet Defender 58676
class npc_scarlet_halls_defender : public CreatureScript
{
    public:
        npc_scarlet_halls_defender() : CreatureScript("npc_scarlet_halls_defender") { }

        struct npc_scarlet_halls_defenderAI : public ScriptedAI
        {
            npc_scarlet_halls_defenderAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            bool triggered;

            void InitializeAI() override
            {
                triggered = false;
                SetEquipmentSlots(false, W_MOULDERING_CORPSE, 0, EQUIP_NO_CHANGE);
            }

            void Reset() override
            {
                triggered = false;
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_DISARM, urand(9000, 18000));
                me->CastSpell(me, SPELL_COMMANDING_SHOUT, false);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/) override
            {
                if (HealthBelowPct(60) && !triggered)
                {
                    triggered = true;
                    me->CastSpell(me, SPELL_SHIELD_BARRIER, false);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                    if (eventId == EVENT_DISARM)
                        if (Unit* vict = me->GetVictim())
                            me->CastSpell(vict, SPELL_DISARM, false);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scarlet_halls_defenderAI(creature);
        }
};

// Scarlet Evoker 58756
class npc_scarlet_evoker : public CreatureScript
{
    public:
        npc_scarlet_evoker() : CreatureScript("npc_scarlet_evoker") { }

        struct npc_scarlet_evokerAI : public ScriptedAI
        {
            npc_scarlet_evokerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override 
            {
                SetEquipmentSlots(false, W_MOULDERING_CORPSE, 0, EQUIP_NO_CHANGE);
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_FIREBALL, urand(3000, 5000));
                events.ScheduleEvent(EVENT_FLAMESTRIKE, urand(7000, 15500));
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FLAMESTRIKE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                me->CastSpell(target, SPELL_FLAMESTRIKE, false);
                            events.ScheduleEvent(EVENT_FLAMESTRIKE, urand(7000, 15500));
                            break;
                        case EVENT_FIREBALL:
                            if (Unit* vict = me->GetVictim())
                                me->CastSpell(vict, SPELL_FIREBALL, false);
                            events.ScheduleEvent(EVENT_FIREBALL, urand(3000, 5000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scarlet_evokerAI(creature);
        }
};

// Scarlet Hall Guardian 59240
class npc_scarlet_hall_guardian : public CreatureScript
{
    public:
        npc_scarlet_hall_guardian() : CreatureScript("npc_scarlet_hall_guardian") { }

        struct npc_scarlet_hall_guardianAI : public ScriptedAI
        {
            npc_scarlet_hall_guardianAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
                SetEquipmentSlots(false, W_REMORSE, W_MAGIC_SHIELD, EQUIP_NO_CHANGE);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_JOLT, urand(1500, 3000));
                events.ScheduleEvent(EVENT_SLASH_ARMOR, urand(7000, 14000));
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_JOLT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                me->CastSpell(target, SPELL_JOLT, false);
                            events.ScheduleEvent(EVENT_JOLT, urand(11000, 17000));
                            break;
                        case EVENT_SLASH_ARMOR:
                            if (Unit* vict = me->GetVictim())
                                me->CastSpell(vict, SPELL_SLASH_ARMOR, false);
                            events.ScheduleEvent(EVENT_SLASH_ARMOR, urand(7000, 14000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scarlet_hall_guardianAI(creature);
        }
};

// Scarlet Treasurer 59241
class npc_scarlet_treasurer : public CreatureScript
{
    public:
        npc_scarlet_treasurer() : CreatureScript("npc_scarlet_treasurer") { }

        struct npc_scarlet_treasurerAI : public ScriptedAI
        {
            npc_scarlet_treasurerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override 
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_TOSS_STATUE, urand(3 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (GameObject* gCoins = me->SummonGameObject(GO_GOLD_COINS, me->GetPositionX() + frand(-3.0f, 3.0f), me->GetPositionY() + frand(-1.9f, 1.9f), me->GetPositionZ(), me->GetOrientation(), { }, 0))
                    gCoins->SetFaction(0);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_TOSS_STATUE)
                    {
                        if (Unit* target = me->GetVictim())
                            DoCast(target, SPELL_TOSS_STATUE);

                        events.ScheduleEvent(EVENT_TOSS_STATUE, urand(5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scarlet_treasurerAI(creature);
        }
};

// Hooded Crusader 64738
class npc_scarlet_hooded_crusader : public CreatureScript
{
    public:
        npc_scarlet_hooded_crusader() : CreatureScript("npc_scarlet_hooded_crusader") { }

        struct npc_scarlet_hooded_crusaderAI : public ScriptedAI
        {
            npc_scarlet_hooded_crusaderAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap nonCombatEvents;
            uint32 wp;

            void DoAction(int32 actionId) override 
            {
                if (actionId == ACTION_KODEX)
                    nonCombatEvents.ScheduleEvent(EVENT_ESCORT, urand(100, 200));
            }

            void InitializeAI() override 
            {
                wp = 0;
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                wp++;

                switch (pointId)
                {
                    case 0:
                        Talk(TALK_INTRO_H);
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 6000);
                        break;
                    case 1:
                        me->GetMotionMaster()->MovementExpired();
                        me->GetMotionMaster()->Clear();
                        Talk(TALK_CODEX);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_ESCORT)
                    {
                        me->GetMotionMaster()->MovementExpired();
                        me->GetMotionMaster()->MovePoint(wp, HoodedCrusaderWp[wp]);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scarlet_hooded_crusaderAI(creature);
        }
};

// Scarlet Scholar 59372
class npc_scarlet_scholar : public CreatureScript
{
    public:
        npc_scarlet_scholar() : CreatureScript("npc_scarlet_scholar") { }

        struct npc_scarlet_scholarAI : public ScriptedAI
        {
            npc_scarlet_scholarAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override 
            {
                events.Reset();
                SetEquipmentSlots(false, W_MOULDERING_CORPSE, 0, EQUIP_NO_CHANGE);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SCARLET_PROTECTION, urand(3000, 8000));
                events.ScheduleEvent(EVENT_FIREBALL_VALLEY, urand(9000, 18000));
                events.ScheduleEvent(EVENT_FIREBALL, 1000);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SCARLET_PROTECTION:
                            me->InterruptNonMeleeSpells(true);
                            DoCast(me, SPELL_SCARLET_PROTECTION);
                            events.ScheduleEvent(EVENT_SCARLET_PROTECTION, urand(34 * IN_MILLISECONDS, 65 * IN_MILLISECONDS));
                            break;
                        case EVENT_FIREBALL:
                            if (Unit* target = me->GetVictim())
                                me->CastSpell(target, SPELL_FIREBALL, false);

                            events.ScheduleEvent(EVENT_FIREBALL, urand(3000, 6500));
                            break;
                        case EVENT_FIREBALL_VALLEY:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                me->CastSpell(target, SPELL_FIREBALL_VALLEY, false);

                            events.ScheduleEvent(EVENT_FIREBALL_VALLEY, urand(9000, 18000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scarlet_scholarAI(creature);
        }
};

// Scarlet Pupil 59373
class npc_scarlet_pupil : public CreatureScript
{
    public:
        npc_scarlet_pupil() : CreatureScript("npc_scarlet_pupil") { }

        struct npc_scarlet_pupilAI : public ScriptedAI
        {
            npc_scarlet_pupilAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override 
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_BEATDOWN, urand(2500, 6000));
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_BEATDOWN)
                    {
                        if (Unit* vict = me->GetVictim())
                            me->CastSpell(vict, SPELL_BEATDOWN, false);

                        events.ScheduleEvent(EVENT_BEATDOWN, urand(5000, 6500));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scarlet_pupilAI(creature);
        }
};

// Throw bucket meaty dog 113029
class spell_throw_bucket_meaty_dog : public SpellScriptLoader
{
    public:
        spell_throw_bucket_meaty_dog() : SpellScriptLoader("spell_throw_bucket_meaty_dog") { }

        class spell_throw_bucket_meaty_dog_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_throw_bucket_meaty_dog_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* caster = GetCaster();

                if (caster->IsInCombat())
                {
                    caster->RemoveAura(SPELL_CARRYING_BUCKET);
                    return SPELL_FAILED_AFFECTING_COMBAT;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_throw_bucket_meaty_dog_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_throw_bucket_meaty_dog_SpellScript();
        }
};

class EatenPredicate
{
    public:
        EatenPredicate() { }

        bool operator()(WorldObject* object)
        {
            return object && object->ToCreature() && object->ToCreature()->GetEntry() != NPC_VIGILANT_WATCHMAN;
        }
};

// Dog Food 111894
class spell_scarlet_halls_dog_food : public SpellScriptLoader
{
    public:
        spell_scarlet_halls_dog_food() : SpellScriptLoader("spell_scarlet_halls_dog_food") { }

        class spell_scarlet_halls_dog_food_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_scarlet_halls_dog_food_SpellScript);

            void CheckTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(EatenPredicate());
            }

            void HandleHitEffect(SpellEffIndex /*effIndex*/)
            {
                if (Creature* target = GetHitUnit()->ToCreature())
                {
                    target->StopMoving();
                    target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                    target->AddAura(SPELL_EATEN, target);

                    std::list<Creature*> Hounds;
                    GetCreatureListWithEntryInGrid(Hounds, target, NPC_STARVING_HOUND, 10.0f);

                    for (auto&& sHound : Hounds)
                    {
                        sHound->StopMoving();
                        sHound->CastSpell(target, SPELL_DOG_LEAP, false);
                        sHound->HandleEmoteStateCommand(EMOTE_STATE_EAT);
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_scarlet_halls_dog_food_SpellScript::CheckTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_scarlet_halls_dog_food_SpellScript::HandleHitEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_scarlet_halls_dog_food_SpellScript();
        }
};

// Archery Target Vehicle Override 114877
class spell_scarlet_halls_archery_target_vehicle_override : public AuraScript
{
    PrepareAuraScript(spell_scarlet_halls_archery_target_vehicle_override);

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit()) // not triggering by default for unk reason...
            owner->CastSpell(owner, SPELL_PLAYER_VEHICLE_AURA, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_scarlet_halls_archery_target_vehicle_override::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// Archery Target Vehicle Override Eff 113399
class spell_scarlet_halls_archery_target_vehicle_override_eff : public SpellScript
{
    PrepareSpellScript(spell_scarlet_halls_archery_target_vehicle_override_eff);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && GetCaster() && target->ToPlayer()->HasAura(GetSpellInfo()->Id) && !target->ToPlayer()->HasAura(GetSpellInfo()->Id, GetCaster()->GetGUID()); });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_scarlet_halls_archery_target_vehicle_override_eff::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Heroic Defense 113436
class spell_scarlet_halls_heroic_defense : public AuraScript
{
    PrepareAuraScript(spell_scarlet_halls_heroic_defense);

    void HandleOnRemove(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetCaster() && GetCaster()->ToCreature())
            GetCaster()->ToCreature()->DespawnOrUnsummon();
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_scarlet_halls_heroic_defense::HandleOnRemove, EFFECT_0, SPELL_AURA_CONTROL_VEHICLE, AURA_EFFECT_HANDLE_REAL);
    }
};

// AreaTrigger 7532
class AreaTrigger_at_commander_lindon : public AreaTriggerScript
{
    public:
        AreaTrigger_at_commander_lindon() : AreaTriggerScript("AreaTrigger_at_commander_lindon") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (Creature* lindon = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(DATA_COMANDER_LINDON) : ObjectGuid::Empty))
                lindon->AI()->DoAction(ACTION_GROUND_EXPLOSION);

            if (player->HasAura(SPELL_HEROIC_DEFENSE) && player->GetPositionX() > 950.0f)
                player->RemoveAurasDueToSpell(SPELL_HEROIC_DEFENSE);

            return false;
        }
};

void AddSC_scarlet_halls()
{
    new npc_starving_hound();
    new npc_angry_hound();
    new npc_bucket_of_meaty_dog_food();
    new npc_reinforced_archery_target();
    new npc_commander_lindon();
    new npc_scarlet_master_archer();
    new npc_scarlet_scourge_hewer();
    new npc_scarlet_myrmidon();
    new npc_scarlet_evangelist();
    new npc_scarlet_cannoneer();
    new npc_cannon_stalker();
    new npc_scarlet_halls_defender();
    new npc_scarlet_evoker();
    new npc_scarlet_hall_guardian();
    new npc_scarlet_treasurer();
    new npc_scarlet_scholar();
    new npc_scarlet_hooded_crusader();
    new npc_scarlet_pupil();
    new spell_throw_bucket_meaty_dog();
    new spell_scarlet_halls_dog_food();
    new aura_script<spell_scarlet_halls_archery_target_vehicle_override>("spell_scarlet_halls_archery_target_vehicle_override");
    new spell_script<spell_scarlet_halls_archery_target_vehicle_override_eff>("spell_scarlet_halls_archery_target_vehicle_override_eff");
    new aura_script<spell_scarlet_halls_heroic_defense>("spell_scarlet_halls_heroic_defense");
    new AreaTrigger_at_commander_lindon();
}
