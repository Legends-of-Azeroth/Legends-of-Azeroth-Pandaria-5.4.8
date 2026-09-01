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
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "ulduar.h"
#include "Vehicle.h"

enum Spells
{
    SPELL_TYMPANIC_TANTRUM                      = 62776,
    SPELL_SEARING_LIGHT_10                      = 63018,
    SPELL_SEARING_LIGHT_25                      = 65121,

    SPELL_SUMMON_LIFE_SPARK                     = 64210,
    SPELL_SUMMON_VOID_ZONE                      = 64203,

    SPELL_GRAVITY_BOMB_10                       = 63024,
    SPELL_GRAVITY_BOMB_25                       = 64234,

    SPELL_HEARTBREAK_10                         = 65737,
    SPELL_HEARTBREAK_25                         = 64193,

    // Cast by 33337 at Heartbreak:
    SPELL_RECHARGE_PUMMELER                     = 62831,    // Summons 33344
    SPELL_RECHARGE_SCRAPBOT                     = 62828,    // Summons 33343
    SPELL_RECHARGE_BOOMBOT                      = 62835,    // Summons 33346

    // Cast by 33329 on 33337 (visual?)
    SPELL_ENERGY_ORB                            = 62790,    // Triggers 62826 - needs spellscript for periodic tick to cast one of the random spells above

    SPELL_HEART_HEAL_TO_FULL                    = 17683,
    SPELL_HEART_OVERLOAD                        = 62789,

    SPELL_HEART_LIGHTNING_TETHER                = 64799,    // Cast on self?
    SPELL_HEART_RIDE_VEHICLE                    = 63313,
    SPELL_ENRAGE                                = 26662,
    SPELL_STAND                                 = 37752,
    SPELL_SUBMERGE                              = 37751,

    // Void Zone
    SPELL_VOID_ZONE_10                          = 64203,
    SPELL_VOID_ZONE_25                          = 64235,
    SPELL_CONSUMPTION_10                        = 64208,
    SPELL_CONSUMPTION_25                        = 64206,

    // Life Spark
    SPELL_STATIC_CHARGED_10                     = 64227,
    SPELL_STATIC_CHARGED_25                     = 64236,
    SPELL_SHOCK                                 = 64230,

    // XT-002 Heart
    SPELL_EXPOSED_HEART                         = 63849,
    SPELL_HEART_RIDE_XT002                      = 63852,

    // XM-024 Pummeller
    SPELL_ARCING_SMASH                          = 8374,
    SPELL_TRAMPLE                               = 5568,
    SPELL_UPPERCUT                              = 10966,

    // Scrapbot:
    SPELL_SCRAPBOT_RIDE_VEHICLE                 = 47020,
    SPELL_SUICIDE                               = 7,

    // Boombot
    SPELL_AURA_BOOMBOT                          = 65032,
    SPELL_BOOM                                  = 62834,

    // Scrapbot
    SPELL_HEAL_XT002                            = 62832,

    // Achievement-related spells
    SPELL_ACHIEVEMENT_CREDIT_NERF_SCRAPBOTS     = 65037
};

#define SPELL_SEARING_LIGHT RAID_MODE(SPELL_SEARING_LIGHT_10, SPELL_SEARING_LIGHT_25)
#define SPELL_GRAVITY_BOMB RAID_MODE(SPELL_GRAVITY_BOMB_10, SPELL_GRAVITY_BOMB_25)
#define SPELL_HEARTBREAK RAID_MODE(SPELL_HEARTBREAK_10, SPELL_HEARTBREAK_25)
#define SPELL_CONSUMPTION RAID_MODE(SPELL_CONSUMPTION_10, SPELL_CONSUMPTION_25)
#define SPELL_STATIC_CHARGED RAID_MODE(SPELL_STATIC_CHARGED_10, SPELL_STATIC_CHARGED_25)

enum Creatures
{
    NPC_VOID_ZONE                               = 34001,
    NPC_LIFE_SPARK                              = 34004,
    NPC_XT002_HEART                             = 33329,
    NPC_XS013_SCRAPBOT                          = 33343,
    NPC_XM024_PUMMELLER                         = 33344,
    NPC_XE321_BOOMBOT                           = 33346,
};

enum Actions
{
    ACTION_ENTER_HARD_MODE,
    ACTION_XT002_REACHED
};

enum XT002Data
{
    DATA_TRANSFERED_HEALTH,
    DATA_HARD_MODE,
    DATA_HEALTH_RECOVERED,
    DATA_GRAVITY_BOMB_CASUALTY,
};

enum Yells
{
    // XT-002
    SAY_AGGRO               = 0,
    SAY_HEART_OPENED        = 1,
    SAY_HEART_CLOSED        = 2,
    SAY_TYMPANIC_TANTRUM    = 3,
    SAY_SLAY                = 4,
    SAY_BERSERK             = 5,
    SAY_DEATH               = 6,
    EMOTE_TYMPANIC          = 8,
    EMOTE_HEART             = 9,
    
    // XS-013 Scrapbot
    EMOTE_REPAIR            = 0
};

enum AchievementCredits 
{ 
    ACHIEV_MUST_DECONSTRUCT_FASTER = 21027 
};

enum Events
{
    EVENT_TYMPANIC_TANTRUM = 1,
    EVENT_SEARING_LIGHT,
    EVENT_GRAVITY_BOMB,
    EVENT_HEART_PHASE,
    EVENT_ENERGY_ORB,
    EVENT_DISPOSE_HEART,
    EVENT_ENRAGE,
    EVENT_ENTER_HARD_MODE,
    EVENT_ARCING_SMASH, 
    EVENT_TRAMPLE, 
    EVENT_UPPERCUT,
};

enum Timers
{
    TIMER_TYMPANIC_TANTRUM_MIN = 32000,
    TIMER_TYMPANIC_TANTRUM_MAX = 36000,
    TIMER_SEARING_LIGHT        = 20000,
    TIMER_GRAVITY_BOMB         = 20000,
    TIMER_HEART_PHASE          = 30000,
    TIMER_ENERGY_ORB_MIN       = 9000,
    TIMER_ENERGY_ORB_MAX       = 10000,
    TIMER_ENRAGE               = 600000,
    TIMER_VOID_ZONE            = 3000,
    TIMER_SPAWN_ADD            = 12000,
    TIMER_SHOCK                = 3000,
    TIMER_ARCING_SMASH         = 7000,
    TIMER_TRAMPLE              = 2000,
    TIMER_UPPERCUT             = 7000,
};

enum XT002Phase 
{ 
    PHASE_ONE = 1, 
    PHASE_TWO 
};

#define HEART_VEHICLE_SEAT 0

struct SpawnVariation
{
    uint32 SpellID;
    uint32 RandomMin, RandomMax;
};

std::vector<std::vector<SpawnVariation>> SpawnVariations =
{
    { { SPELL_RECHARGE_PUMMELER, 1, 1 } },
    { { SPELL_RECHARGE_BOOMBOT,  1, 1 }, { SPELL_RECHARGE_SCRAPBOT, 5, 5 } },
    { { SPELL_RECHARGE_BOOMBOT,  1, 1 }, { SPELL_RECHARGE_SCRAPBOT, 5, 5 } },
};

class boss_xt002 : public CreatureScript
{
    public:
        boss_xt002() : CreatureScript("boss_xt002") { }

        struct boss_xt002_AI : public BossAI
        {
            boss_xt002_AI(Creature* creature) : BossAI(creature, BOSS_XT002)
            {
                gravityBombCasualty = false;
                hardMode = false;

                heartExposed = 0;
            }

            void Reset() override
            {
                _Reset();

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DISABLE_MOVE);  // Just for the case they are still present
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->ResetLootMode();

                me->GetMap()->SetWorldState(WORLDSTATE_NERF_ENGINEERING, 1);
                me->GetMap()->SetWorldState(WORLDSTATE_NERF_GRAVITY_BOMBS, 1);
                gravityBombCasualty = false;
                hardMode = false;

                heartExposed = 0;

                phase = PHASE_ONE;
                events.Reset();
                events.SetPhase(PHASE_ONE);

                if (instance)
                    instance->DoStopTimedAchievement(CRITERIA_START_TYPE_EVENT, ACHIEV_MUST_DECONSTRUCT_FASTER);

                DespawnSpawns();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                _JustEngagedWith();

                events.ScheduleEvent(EVENT_ENRAGE, TIMER_ENRAGE, 0, PHASE_ONE);
                events.ScheduleEvent(EVENT_GRAVITY_BOMB, TIMER_GRAVITY_BOMB, 0, PHASE_ONE);
                events.ScheduleEvent(EVENT_SEARING_LIGHT, TIMER_SEARING_LIGHT, 0, PHASE_ONE);
                // Due to Hordeguides, the first Tympanic Tantrum gets scheduled after ~1min.
                events.ScheduleEvent(EVENT_TYMPANIC_TANTRUM, urand(TIMER_TYMPANIC_TANTRUM_MIN, TIMER_TYMPANIC_TANTRUM_MAX) * 2, 0, PHASE_ONE);

                if (instance)
                    instance->DoStartCriteria(CRITERIA_START_TYPE_EVENT, ACHIEV_MUST_DECONSTRUCT_FASTER);

                std::list<Creature*> trashmobs;
                GetCreatureListWithEntryInGrid(trashmobs, me, 34269, 100);
                GetCreatureListWithEntryInGrid(trashmobs, me, 34267, 100);
                GetCreatureListWithEntryInGrid(trashmobs, me, 34273, 100);
                GetCreatureListWithEntryInGrid(trashmobs, me, 34271, 100);
                for (auto&& trash : trashmobs)
                    if (trash->IsAlive())
                        trash->SetInCombatWithZone();
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_ENTER_HARD_MODE:
                        events.ScheduleEvent(EVENT_ENTER_HARD_MODE, 1);
                        break;
                    case ACTION_XT002_REACHED:
                        me->GetMap()->SetWorldState(WORLDSTATE_NERF_ENGINEERING, 0);
                        break;
                }
            }

            void KilledUnit(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_SLAY);
            }

            void JustDied(Unit* /*killer*/) override
            {
                DespawnSpawns();

                Talk(SAY_DEATH);
                me->RemoveAllAuras();
                _JustDied();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            }

            void DamageTaken(Unit* /*who*/, uint32& /*damage*/) override
            {
                if (!hardMode && phase == PHASE_ONE && !HealthAbovePct(100 - 25 * (heartExposed+1)))
                {
                    ExposeHeart();
                    ++heartExposed;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_HARD_MODE:
                        return hardMode ? 1 : 0;
                    case DATA_GRAVITY_BOMB_CASUALTY:
                        return gravityBombCasualty ? 1 : 0;
                }

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_TRANSFERED_HEALTH:
                        _transferHealth = data;
                        break;
                    case DATA_GRAVITY_BOMB_CASUALTY:
                        gravityBombCasualty = (data > 0) ? true : false;
                        me->GetMap()->SetWorldState(WORLDSTATE_NERF_GRAVITY_BOMBS, 0);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || !CheckInRoom())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING)) // Still channeling, do nothing
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SEARING_LIGHT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_SEARING_LIGHT);

                            events.ScheduleEvent(EVENT_SEARING_LIGHT, TIMER_SEARING_LIGHT, 0, PHASE_ONE);
                            break;
                        case EVENT_GRAVITY_BOMB:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_GRAVITY_BOMB);

                            events.ScheduleEvent(EVENT_GRAVITY_BOMB, TIMER_GRAVITY_BOMB, 0, PHASE_ONE);
                            break;
                        case EVENT_TYMPANIC_TANTRUM:
                            Talk(SAY_TYMPANIC_TANTRUM);
                            Talk(EMOTE_TYMPANIC);
                            DoCast(SPELL_TYMPANIC_TANTRUM);
                            events.ScheduleEvent(EVENT_TYMPANIC_TANTRUM, urand(TIMER_TYMPANIC_TANTRUM_MIN, TIMER_TYMPANIC_TANTRUM_MAX), 0, PHASE_ONE);
                            break;
                        case EVENT_DISPOSE_HEART:
                            SetPhaseOne();
                            break;
                        case EVENT_ENRAGE:
                            Talk(SAY_BERSERK);
                            DoCast(me, SPELL_ENRAGE);
                            break;
                        case EVENT_ENTER_HARD_MODE:
                            me->SetFullHealth();
                            DoCast(me, SPELL_HEARTBREAK, true);
                            me->AddLootMode(LOOT_MODE_HARD_MODE_1);
                            hardMode = true;
                            SetPhaseOne();
                            break;
                    }
                }

                if (phase == PHASE_ONE)
                    DoMeleeAttackIfReady();
            }

            void ExposeHeart()
            {
                Talk(SAY_HEART_OPENED);
                Talk(EMOTE_HEART);
                me->InterruptSpell(CURRENT_CHANNELED_SPELL, false, true);
                me->RemoveAurasDueToSpell(SPELL_TYMPANIC_TANTRUM);

                DoCast(me, SPELL_SUBMERGE);  // WIll make creature untargetable

                me->AttackStop();
                me->SetReactState(REACT_PASSIVE);

                _transferHealth = 0;
                if (Creature* heart = me->SummonCreature(NPC_XT002_HEART, *me, TEMPSUMMON_MANUAL_DESPAWN, Milliseconds(60*IN_MILLISECONDS)))
                {
                    heart->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    heart->EnterVehicle(me, 1);
                    heart->ClearUnitState(UNIT_STATE_FOLLOW_FORMATION);
                    heart->SetInCombatWithZone();
                    heart->CastSpell(heart, SPELL_EXPOSED_HEART, true);
                    heart->CastSpell(heart, SPELL_HEART_OVERLOAD, true);
                }
                // If that does not work... uh-oh

                events.CancelEvent(EVENT_SEARING_LIGHT);
                events.CancelEvent(EVENT_GRAVITY_BOMB);
                events.CancelEvent(EVENT_TYMPANIC_TANTRUM);

                // Phase 2 has officially started
                phase = PHASE_TWO;
                // Start "end of phase 2 timer"
                events.SetPhase(PHASE_TWO);
                events.ScheduleEvent(EVENT_DISPOSE_HEART, TIMER_HEART_PHASE, 0, PHASE_TWO);
            }

            void SetPhaseOne()
            {
                Talk(SAY_HEART_CLOSED);
                if (me->HasAura(SPELL_SUBMERGE))
                    me->RemoveAurasDueToSpell(SPELL_SUBMERGE);
                DoCast(me, SPELL_STAND);
                // Just for the case this isn't done by the spell above.

                phase = PHASE_ONE;
                events.SetPhase(PHASE_ONE);
                
                events.RescheduleEvent(EVENT_SEARING_LIGHT, TIMER_SEARING_LIGHT / 2, 0, PHASE_ONE);
                events.RescheduleEvent(EVENT_GRAVITY_BOMB, TIMER_GRAVITY_BOMB, 0, PHASE_ONE);
                events.RescheduleEvent(EVENT_TYMPANIC_TANTRUM, urand(TIMER_TYMPANIC_TANTRUM_MIN, TIMER_TYMPANIC_TANTRUM_MAX), 0, PHASE_ONE);

                if (Unit* heart = me->GetVehicleKit() ? me->GetVehicleKit()->GetPassenger(HEART_VEHICLE_SEAT) : nullptr)
                {
                    heart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                    heart->RemoveAurasDueToSpell(SPELL_EXPOSED_HEART);
                }

                if (!hardMode)
                {
                    if (me->GetHealth() <= _transferHealth)
                        _transferHealth = me->GetHealth() > 10000 ? me->GetHealth() - 10000 : 0;

                    me->ModifyHealth(-((int32)_transferHealth));
                    me->LowerPlayerDamageReq(_transferHealth);
                }

                float healthPct = me->GetHealthPct();
                if (healthPct <= 25)
                    heartExposed = 3;
                else if (healthPct <= 50)
                    heartExposed = 2;

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_AGGRESSIVE);
            }

            private:
                // Achievement related
                bool hardMode;              // Are we in hard mode? Or: was the heart killed during phase 2?
                bool gravityBombCasualty;   // Did someone die because of Gravity Bomb damage?

                XT002Phase phase;
                uint8 heartExposed;
                uint32 _transferHealth;

                void DespawnSpawns()
                {
                    if (Unit* heart = me->GetVehicleKit() ? me->GetVehicleKit()->GetPassenger(HEART_VEHICLE_SEAT) : nullptr)
                        heart->InterruptNonMeleeSpells(true);

                    std::list<Creature*> spawns;
                    GetCreatureListWithEntryInGrid(spawns, me, NPC_XT002_HEART, 250);
                    GetCreatureListWithEntryInGrid(spawns, me, NPC_VOID_ZONE, 250);
                    GetCreatureListWithEntryInGrid(spawns, me, NPC_LIFE_SPARK, 250);
                    GetCreatureListWithEntryInGrid(spawns, me, NPC_XS013_SCRAPBOT, 250);
                    GetCreatureListWithEntryInGrid(spawns, me, NPC_XM024_PUMMELLER, 250);
                    GetCreatureListWithEntryInGrid(spawns, me, NPC_XE321_BOOMBOT, 250);
                    for (auto&& spawn : spawns)
                        if (spawn)
                            spawn->DespawnOrUnsummon();
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_xt002_AI>(creature);
        }
};

class npc_xt002_heart : public CreatureScript
{
    public:
        npc_xt002_heart() : CreatureScript("npc_xt002_heart") { }        

        struct npc_xt002_heartAI : public ScriptedAI
        {
            npc_xt002_heartAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE |  UNIT_FLAG_PACIFIED);
                SetCombatMovement(false);

                me->m_Events.Schedule(30000, [this]()
                {
                    if (me->IsAlive())
                    {
                        failed = true;
                        me->Kill(me);
                        me->DespawnOrUnsummon(10000);
                        me->m_Events.Schedule(1000, [this]() { me->SetVisible(false); });
                    }
                });
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (failed)
                    return;

                if (instance)
                    if (Creature* XT002 = me->GetCreature(*me, instance->GetGuidData(BOSS_XT002)))
                        if (XT002->AI())
                            XT002->AI()->DoAction(ACTION_ENTER_HARD_MODE);

                // removes the aura
                me->RemoveAurasDueToSpell(SPELL_EXPOSED_HEART);
                me->RemoveAurasDueToSpell(SPELL_HEART_OVERLOAD);
                me->DespawnOrUnsummon(10000);
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                if (Creature* XT002 = me->GetCreature(*me, instance->GetGuidData(BOSS_XT002)))
                    if (XT002->AI())
                    {
                        uint32 health = me->GetHealth();
                        if (health <= damage)
                            health = 0;
                        else
                            health -= damage;
                        XT002->AI()->SetData(DATA_TRANSFERED_HEALTH, me->GetMaxHealth() - health);
                    }
            }

            private:
                InstanceScript* instance;
                uint32 damageTaken;
                bool failed = false;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_xt002_heartAI>(creature);
        }
};

class npc_scrapbot : public CreatureScript
{
    public:
        npc_scrapbot() : CreatureScript("npc_scrapbot") { }        

        struct npc_scrapbotAI : public ScriptedAI
        {
            npc_scrapbotAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
                casted = false;
            }

            void Reset() override
            {
                DoZoneInCombat(nullptr, 250.0f);
                me->SetReactState(REACT_PASSIVE);

                rangeCheckTimer = 500;

                if (Creature* pXT002 = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_XT002)))
                    me->GetMotionMaster()->MoveFollow(pXT002, 0.1f, 0.0f);
            }

            void IsSummonedBy(Unit* summoner) override
            {
                DoCast(me, 65208, true); // Self Stun (3 secs)
            }

            void UpdateAI(uint32 diff) override
            {
                if (rangeCheckTimer <= diff)
                {
                    if (Creature* xt002 = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_XT002)))
                    {
                        if (!casted && xt002->IsAlive())
                            if (me->IsWithinMeleeRange(xt002))
                            {
                                casted = true;
                                Talk(EMOTE_REPAIR);
                                xt002->CastSpell(xt002, SPELL_HEAL_XT002, true);
                                xt002->AI()->DoAction(ACTION_XT002_REACHED);
                                /*
                                DoCast(xt002, SPELL_SCRAPBOT_RIDE_VEHICLE);
                                // Unapply vehicle aura again
                                xt002->RemoveAurasDueToSpell(SPELL_SCRAPBOT_RIDE_VEHICLE);
                                */
                                me->DespawnOrUnsummon();
                            }
                    }
                }
                else
                    rangeCheckTimer -= diff;
            }

            private:
                InstanceScript* instance;
                uint32 rangeCheckTimer;
                bool casted;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_scrapbotAI>(creature);
        }
};

class npc_pummeller : public CreatureScript
{
    public:
        npc_pummeller() : CreatureScript("npc_pummeller") { }

        struct npc_pummellerAI : public ScriptedAI
        {
            npc_pummellerAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            void Reset() override
            {
                DoZoneInCombat(nullptr, 250.0f);

                events.ScheduleEvent(EVENT_ARCING_SMASH, TIMER_ARCING_SMASH);
                events.ScheduleEvent(EVENT_TRAMPLE, TIMER_TRAMPLE);
                events.ScheduleEvent(EVENT_UPPERCUT, TIMER_UPPERCUT);
            }

            void IsSummonedBy(Unit* summoner) override
            {
                DoCast(me, 65208, true); // Self Stun (3 secs)
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->IsWithinMeleeRange(me->GetVictim()))
                {
                    events.Update(diff);
                    while (uint32 event = events.ExecuteEvent())
                    {
                        switch (event)
                        {
                            case EVENT_ARCING_SMASH:
                                DoCast(me->GetVictim(), SPELL_ARCING_SMASH);
                                events.ScheduleEvent(EVENT_ARCING_SMASH, TIMER_ARCING_SMASH);
                                break;

                            case EVENT_TRAMPLE:
                                DoCast(me->GetVictim(), SPELL_TRAMPLE);
                                events.ScheduleEvent(EVENT_TRAMPLE, TIMER_TRAMPLE);
                                break;

                            case EVENT_UPPERCUT:
                                DoCast(me->GetVictim(), SPELL_UPPERCUT);
                                events.ScheduleEvent(EVENT_UPPERCUT, TIMER_UPPERCUT);
                                break;
                        }                        
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
                InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_pummellerAI>(creature);
        }
};

class npc_boombot : public CreatureScript
{
    public:
        npc_boombot() : CreatureScript("npc_boombot") { }        

        struct npc_boombotAI : public ScriptedAI
        {
            npc_boombotAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            void Reset() override
            {
                DoZoneInCombat(nullptr, 250.0f);

                boomed = false;

                DoCast(SPELL_AURA_BOOMBOT); // For achievement

                // HACK/workaround:
                // these values aren't confirmed - lack of data - and the values in DB are incorrect
                // these values are needed for correct damage of Boom spell
                me->SetFloatValue(UNIT_FIELD_MIN_RANGED_DAMAGE, 15000.0f);
                me->SetFloatValue(UNIT_FIELD_MAX_RANGED_DAMAGE, 18000.0f);
                me->SetDisplayId(19139);
                me->SetSpeed(MOVE_RUN, 0.5f, true);
            }

            void IsSummonedBy(Unit* summoner) override
            {
                DoCast(me, 65208, true); // Self Stun (3 secs)
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                if (damage >= me->GetHealth() && !boomed)
                {
                    boomed = true; // Prevent recursive calls

                    ObjectGuid targetguid = me->GetGUID();
                    ObjectGuid casterguid = me->GetGUID();
                    WorldPacket data(SMSG_SPELLINSTAKILLLOG, 8+8+4);
                    data.WriteBit(casterguid[6]);
                    data.WriteBit(targetguid[0]);
                    data.WriteBit(casterguid[7]);
                    data.WriteBit(targetguid[2]);
                    data.WriteBit(casterguid[3]);
data.WriteBit(casterguid[1]);
data.WriteBit(casterguid[2]);
data.WriteBit(casterguid[0]);
data.WriteBit(casterguid[4]);
                    data.WriteBit(targetguid[4]);
data.WriteBit(targetguid[7]);
data.WriteBit(targetguid[1]);
data.WriteBit(targetguid[6]);
data.WriteBit(targetguid[5]);
                    data.WriteBit(casterguid[5]);
                    data.WriteBit(targetguid[3]);

                    data.WriteByteSeq(casterguid[0]);
                    data.WriteByteSeq(targetguid[1]);
                    data.WriteByteSeq(casterguid[3]);
data.WriteByteSeq(casterguid[4]);
data.WriteByteSeq(casterguid[5]);
data.WriteByteSeq(casterguid[7]);
                    data.WriteByteSeq(targetguid[0]);
                    data.WriteByteSeq(casterguid[6]);
                    data.WriteByteSeq(targetguid[2]);
data.WriteByteSeq(targetguid[4]);
                    data.WriteByteSeq(casterguid[1]);
                    data << int32(SPELL_BOOM);
                    data.WriteByteSeq(targetguid[3]);
                    data.WriteByteSeq(casterguid[2]);
                    data.WriteByteSeq(targetguid[7]);
data.WriteByteSeq(targetguid[6]);
data.WriteByteSeq(targetguid[5]);
                    me->SendMessageToSet(&data, false);

                    me->DealDamage(me, me->GetHealth(), nullptr, NODAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);

                    damage = 0;

                    // Visual only seems to work if the instant kill event is delayed or the spell itself is delayed
                    // Casting done from player and caster source has the same targetinfo flags,
                    // so that can't be the issue
                    // See BoomEvent class
                    // Schedule 1s delayed
                    me->m_Events.Schedule(1 * IN_MILLISECONDS, [this]()
                    {
                        // This hack is here because we suspect our implementation of spell effect execution on targets
                        // is done in the wrong order. We suspect that EFFECT_0 needs to be applied on all targets,
                        // then EFFECT_1, etc - instead of applying each effect on target1, then target2, etc.
                        // The above situation causes the visual for this spell to be bugged, so we remove the instakill
                        // effect and implement a script hack for that.
                        me->RemoveAurasDueToSpell(65208);
                        DoCastAOE(SPELL_BOOM, false);
                    });
                }
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (!UpdateVictim())
                    return;

                // No melee attack
            }

           private:
                InstanceScript* instance;
                bool boomed;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_boombotAI>(creature);
        }
};

class npc_life_spark : public CreatureScript
{
    public:
        npc_life_spark() : CreatureScript("npc_life_spark") { }

        struct npc_life_sparkAI : public ScriptedAI
        {
            npc_life_sparkAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                DoCast(me, SPELL_STATIC_CHARGED);
                shockTimer = 0; // first one is immediate.
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (shockTimer <= diff)
                {
                    DoCastVictim(SPELL_SHOCK);
                    shockTimer = TIMER_SHOCK;
                }
                else shockTimer -= diff;
                
                DoMeleeAttackIfReady();
            }

            private:
                uint32 shockTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_life_sparkAI>(creature);
        }
};

class npc_void_zone : public CreatureScript
{
    public:
        npc_void_zone() : CreatureScript("npc_void_zone") { }

        struct npc_void_zoneAI : public ScriptedAI
        {
            npc_void_zoneAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                SetCombatMovement(false);
            }

            void Reset() override
            {
                consumptionTimer = 3*IN_MILLISECONDS;
            }

            void UpdateAI(uint32 diff) override
            {
                if (consumptionTimer <= diff)
                {
                    int32 dmg = RAID_MODE<uint32>(5000, 7500);
                    me->CastCustomSpell(me, SPELL_CONSUMPTION, &dmg, 0, 0, false);
                    consumptionTimer = 3*IN_MILLISECONDS;
                }
                else
                    consumptionTimer -= diff;
            }

        private:
            uint32 consumptionTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_void_zoneAI>(creature);
        }
};

class spell_xt002_searing_light_spawn_life_spark : public SpellScriptLoader
{
    public:
        spell_xt002_searing_light_spawn_life_spark() : SpellScriptLoader("spell_xt002_searing_light_spawn_life_spark") { }

        class spell_xt002_searing_light_spawn_life_spark_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_xt002_searing_light_spawn_life_spark_AuraScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_LIFE_SPARK))
                    return false;
                return true;
            }

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Player* player = GetOwner()->ToPlayer())
                    if (Unit* xt002 = GetCaster())
                        if (Creature* xt002c = xt002->ToCreature())
                            if (xt002c->IsAIEnabled)
                                if (xt002c->AI()->GetData(DATA_HARD_MODE))
                                    xt002->ToCreature()->CastSpell(player, SPELL_SUMMON_LIFE_SPARK, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_xt002_searing_light_spawn_life_spark_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_xt002_searing_light_spawn_life_spark_AuraScript();
        }
};

class spell_xt002_gravity_bomb_aura : public SpellScriptLoader
{
    public:
        spell_xt002_gravity_bomb_aura() : SpellScriptLoader("spell_xt002_gravity_bomb_aura") { }

        class spell_xt002_gravity_bomb_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_xt002_gravity_bomb_aura_AuraScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_VOID_ZONE))
                    return false;
                return true;
            }

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Player* player = GetOwner()->ToPlayer())
                    if (Unit* xt002 = GetCaster())
                        if (Creature* xt002c = xt002->ToCreature())
                            if (xt002c->IsAIEnabled)
                                if (xt002c->AI()->GetData(DATA_HARD_MODE))
                                    xt002->ToCreature()->CastSpell(player, SPELL_SUMMON_VOID_ZONE, true);    // TODO: Check if casting this spell works as intended, may have problems due to target selection
            }

            void OnPeriodic(AuraEffect const* aurEff)
            {
                Unit* xt002 = GetCaster();
                if (!xt002)
                    return;

                Unit* owner = GetOwner()->ToUnit();
                if (!owner)
                    return;

                if (aurEff->GetAmount() >= int32(owner->GetHealth()))
                    if (xt002->GetAI())
                        xt002->GetAI()->SetData(DATA_GRAVITY_BOMB_CASUALTY, 1);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_xt002_gravity_bomb_aura_AuraScript::OnPeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE);
                AfterEffectRemove += AuraEffectRemoveFn(spell_xt002_gravity_bomb_aura_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };
        AuraScript* GetAuraScript() const override
        {
            return new spell_xt002_gravity_bomb_aura_AuraScript();
        }
};

class BombTargetSelector 
{
    public:
        BombTargetSelector(Creature* me, const Unit* victim) : _me(me), _victim(victim) { }

        bool operator()(WorldObject* object) const
        {
            if (Unit* target = object->ToUnit())
                if (target == _victim && _me->GetThreatManager().getThreatList().size() > 1)
                    return true;

            return false;
        }

        Creature* _me;
        Unit const* _victim;
};

class spell_xt002_gravity_bomb_aura_target : public SpellScriptLoader
{
    public:
        spell_xt002_gravity_bomb_aura_target() : SpellScriptLoader("spell_xt002_gravity_bomb_aura_target") { }

        class spell_xt002_gravity_bomb_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xt002_gravity_bomb_targeting_SpellScript);

            bool Load() override
            {
                _target = nullptr;
                return GetCaster()->GetTypeId() == TYPEID_UNIT;
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(BombTargetSelector(GetCaster()->ToCreature(), GetCaster()->GetVictim()));

                if (targets.empty())
                    return;

                _target = Trinity::Containers::SelectRandomContainerElement(targets);
                targets.clear();
                targets.push_back(_target);
            }

            void SetTarget(std::list<WorldObject*>& targets)
            {
                targets.clear();
                if (_target)
                    targets.push_back(_target);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_xt002_gravity_bomb_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_xt002_gravity_bomb_targeting_SpellScript::SetTarget, EFFECT_2, TARGET_UNIT_DEST_AREA_ENEMY);
            }

            WorldObject* _target;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_xt002_gravity_bomb_targeting_SpellScript();
        }
};

class spell_xt002_gravity_bomb_damage : public SpellScriptLoader
{
    public:
        spell_xt002_gravity_bomb_damage() : SpellScriptLoader("spell_xt002_gravity_bomb_damage") { }

        class spell_xt002_gravity_bomb_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xt002_gravity_bomb_damage_SpellScript);

            void HandleScript(SpellEffIndex /*eff*/)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                if (GetHitDamage() >= int32(GetHitUnit()->GetHealth()))
                    if (caster->GetAI())
                        caster->GetAI()->SetData(DATA_GRAVITY_BOMB_CASUALTY, 1);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_xt002_gravity_bomb_damage_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_xt002_gravity_bomb_damage_SpellScript();
        }
};

class spell_xt002_heart_overload_periodic : public SpellScriptLoader
{
    public:
        spell_xt002_heart_overload_periodic() : SpellScriptLoader("spell_xt002_heart_overload_periodic") { }

        class spell_xt002_heart_overload_periodic_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xt002_heart_overload_periodic_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ENERGY_ORB))
                    return false;

                if (!sSpellMgr->GetSpellInfo(SPELL_RECHARGE_BOOMBOT))
                    return false;

                if (!sSpellMgr->GetSpellInfo(SPELL_RECHARGE_PUMMELER))
                    return false;

                if (!sSpellMgr->GetSpellInfo(SPELL_RECHARGE_SCRAPBOT))
                    return false;

                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (InstanceScript* instance = caster->GetInstanceScript())
                    {
                        std::vector<Unit*> piles;
                        for (uint32 i = 0; i < 4; ++i)
                            if (Unit* toyPile = ObjectAccessor::GetUnit(*caster, instance->GetGuidData(DATA_TOY_PILE_0 + i)))
                                if (caster->GetVehicleBase()->GetExactDist2d(toyPile) > 30.0f)
                                    piles.push_back(toyPile);

                        if (!piles.empty())
                            if (Unit* toyPile = Trinity::Containers::SelectRandomContainerElement(piles))
                                caster->CastSpell(toyPile, SPELL_ENERGY_ORB, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_xt002_heart_overload_periodic_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_xt002_heart_overload_periodic_SpellScript();
        }
};

class spell_xt002_energy_orb : public SpellScriptLoader
{
    public:
        spell_xt002_energy_orb() : SpellScriptLoader("spell_xt002_energy_orb") { }

        class spell_xt002_energy_orb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xt002_energy_orb_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    for (auto&& spawn : Trinity::Containers::SelectRandomContainerElement(SpawnVariations))
                        target->CastCustomSpell(spawn.SpellID, SPELLVALUE_BASE_POINT0, urand(spawn.RandomMin, spawn.RandomMax), target, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_xt002_energy_orb_SpellScript::HandleDummy, EFFECT_2, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_xt002_energy_orb_SpellScript();
        }
};

class spell_xt002_recharge_robot : public SpellScriptLoader
{
    public:
        spell_xt002_recharge_robot() : SpellScriptLoader("spell_xt002_recharge_robot") { }

        class spell_xt002_recharge_robot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xt002_recharge_robot_SpellScript);

            void SelectDest(SpellDestination& dest)
            {
                dest._position.m_positionZ = 410;
            }

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_xt002_recharge_robot_SpellScript::SelectDest, EFFECT_0, TARGET_DEST_DEST_RANDOM);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_xt002_recharge_robot_SpellScript();
        }
};

class spell_xt002_tympanic_tantrum : public SpellScriptLoader
{
    public:
        spell_xt002_tympanic_tantrum() : SpellScriptLoader("spell_xt002_tympanic_tantrum") { }

        class spell_xt002_tympanic_tantrum_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xt002_tympanic_tantrum_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if (PlayerOrPetCheck());
            }
            
            void HandleDamageCalc(SpellEffIndex effIndex)
            {
                if (Unit* target = GetHitUnit())
                {
                    uint8 healthPct = GetSpellInfo()->Effects[effIndex].CalcValue(target);
                    SetHitDamage(target->GetMaxHealth() / healthPct);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_xt002_tympanic_tantrum_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_xt002_tympanic_tantrum_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_xt002_tympanic_tantrum_SpellScript::HandleDamageCalc, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_xt002_tympanic_tantrum_SpellScript();
        }
};

class spell_xt002_submerged : public SpellScriptLoader
{
    public:
        spell_xt002_submerged() : SpellScriptLoader("spell_xt002_submerged") { }

        class spell_xt002_submerged_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xt002_submerged_SpellScript);

            void HandleScript(SpellEffIndex /*eff*/)
            {
                Creature* target = GetHitCreature();
                if (!target)
                    return;

                target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                target->SetStandState(UNIT_STAND_STATE_SUBMERGED);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_xt002_submerged_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_xt002_submerged_SpellScript();
        }
};

class spell_xt002_stand : public SpellScriptLoader
{
    public:
        spell_xt002_stand() : SpellScriptLoader("spell_xt002_stand") { }

        class spell_xt002_stand_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_xt002_stand_SpellScript);

            void HandleScript(SpellEffIndex /*eff*/)
            {
                Creature* target = GetHitCreature();
                if (!target)
                    return;

                target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                target->SetStandState(UNIT_STAND_STATE_STAND);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_xt002_stand_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_xt002_stand_SpellScript();
        }
};

class achievement_heartbreaker : public AchievementCriteriaScript
{
    public:
        achievement_heartbreaker() : AchievementCriteriaScript("achievement_heartbreaker") { }

        bool OnCheck(Player* /*source*/, Unit* target)
        {
            if (!target || !target->GetAI())
                return false;

            return target->GetAI()->GetData(DATA_HARD_MODE);
        }
};

void AddSC_boss_xt002()
{
    new npc_xt002_heart();
    new npc_scrapbot();
    new npc_pummeller();
    new npc_boombot();

    new npc_life_spark();
    new npc_void_zone();
    new boss_xt002();

    new spell_xt002_searing_light_spawn_life_spark();
    new spell_xt002_gravity_bomb_aura();
    new spell_xt002_gravity_bomb_aura_target();
    new spell_xt002_gravity_bomb_damage();
    new spell_xt002_heart_overload_periodic();
    new spell_xt002_energy_orb();
    new spell_xt002_recharge_robot();
    new spell_xt002_tympanic_tantrum();
    new spell_xt002_submerged();
    new spell_xt002_stand();

    new achievement_heartbreaker();
}

#undef SPELL_SEARING_LIGHT
#undef SPELL_GRAVITY_BOMB
#undef SPELL_HEARTBREAK
#undef SPELL_CONSUMPTION
#undef SPELL_STATIC_CHARGED
