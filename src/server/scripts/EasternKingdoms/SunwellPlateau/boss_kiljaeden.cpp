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
SDName: Boss_Kiljaeden
SD%Complete: 80
SDComment: Sinister Reflection Model, Armageddon Visual, SAY_KJ_SHADOWSPIKE3, Emote, End Sequence
SDCategory: Sunwell_Plateau
EndScriptData */

// TODO rewrite Armageddon

#include "ScriptPCH.h"
#include "sunwell_plateau.h"
#include <math.h>

// Speech and sounds
enum Yells
{
    SAY_KJ_OFFCOMBAT                            = 0,

    SAY_KALECGOS_ENCOURAGE                      = 0,
    SAY_KALECGOS_READY1                         = 1,
    SAY_KALECGOS_READY2                         = 2,
    SAY_KALECGOS_READY3                         = 3,
    SAY_KALECGOS_READY4                         = 4,
    SAY_KALECGOS_AWAKEN                         = 5,
    SAY_KALECGOS_LETGO                          = 6,
    SAY_KALECGOS_FOCUS                          = 7,
    SAY_KALECGOS_FATE                           = 8,
    SAY_KALECGOS_GOODBYE                        = 9,
    SAY_KALECGOS_JOIN                           = 10,

    SAY_KJ_DEATH                                = 0,
    SAY_KJ_SLAY                                 = 1,
    SAY_KJ_REFLECTION                           = 2,
    SAY_KJ_EMERGE                               = 3,
    SAY_KJ_DARKNESS                             = 4,
    SAY_KJ_PHASE3                               = 5,
    SAY_KJ_PHASE4                               = 6,
    SAY_KJ_PHASE5                               = 7,
    EMOTE_KJ_DARKNESS                           = 8,

    SAY_ANVEENA_IMPRISONED                      = 0,
    SAY_ANVEENA_LOST                            = 1,
    SAY_ANVEENA_KALEC                           = 2,
    SAY_ANVEENA_GOODBYE                         = 3,
};

// Spells used during the encounter
enum Spells
{
    // Hand of the Deceiver's spells and cosmetics
    SPELL_SHADOW_BOLT_VOLLEY                    = 45770, // ~30 yard range Shadow Bolt Volley for ~2k(?) damage
    SPELL_SHADOW_INFUSION                       = 45772, // They gain this at 20% - Immunity to Stun/Silence and makes them look angry!
    SPELL_FELFIRE_PORTAL                        = 46875, // Creates a portal that spawns Felfire Fiends (LIVE FOR THE SWARM!1 FOR THE OVERMIND!)
    SPELL_SHADOW_CHANNELING                     = 46757, // Channeling animation out of combat

    // Volatile Felfire Fiend's spells
    SPELL_FELFIRE_FISSION                       = 45779, // Felfire Fiends explode when they die or get close to target.

    // Kil'Jaeden's spells and cosmetics
    SPELL_TRANS                                 = 23188, // Surprisingly, this seems to be the right spell.. (Where is it used?)
    SPELL_REBIRTH                               = 44200, // Emerge from the Sunwell
    SPELL_SOUL_FLAY                             = 45442, // 9k Shadow damage over 3 seconds. Spammed throughout all the fight.
    SPELL_SOUL_FLAY_SLOW                        = 47106,
    SPELL_LEGION_LIGHTNING                      = 45664, // Chain Lightning, 4 targets, ~3k Shadow damage, 1.5fk mana burn
    SPELL_FIRE_BLOOM                            = 45641, // Places a debuff on 5 raid members, which causes them to deal 2k Fire damage to nearby allies and selves. MIGHT NOT WORK
    SPELL_DESTROY_ALL_DRAKES                    = 46707, // when he use it?

    SPELL_SINISTER_REFLECTION_CLONE             = 45785,
    SPELL_SINISTER_REFLECTION_SUMMON            = 45891,
    SPELL_SINISTER_REFLECTION                   = 45892, // Summon shadow copies of 5 raid members that fight against KJ's enemies
    SPELL_SINISTER_REFLECTION_CLASS             = 45893,
    SPELL_COPY_WEAPON                           = 41055, // }
    SPELL_COPY_WEAPON2                          = 41054, // }
    SPELL_COPY_OFFHAND                          = 45206, // }- Spells used in Sinister Reflection creation
    SPELL_COPY_OFFHAND_WEAPON                   = 45205, // }

    SPELL_SHADOW_SPIKE                          = 46680, // Bombard random raid members with Shadow Spikes (Very similar to Void Reaver orbs)
    SPELL_FLAME_DART                            = 45737, // Bombards the raid with flames every 3(?) seconds
    SPELL_DARKNESS_OF_A_THOUSAND_SOULS          = 46605, // Begins a 8-second channeling, after which he will deal 50'000 damage to the raid
    SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE   = 45657,

    // Armageddon spells wrong visual
    SPELL_ARMAGEDDON_TRIGGER                    = 45909, // Meteor spell trigger missile should cast Creature on himself
    SPELL_ARMAGEDDON_VISUAL                     = 45911, // Does the hellfire visual to indicate where the meteor missle lands
    SPELL_ARMAGEDDON_VISUAL2                    = 45914, // Does the light visual to indicate where the meteor missle lands
    SPELL_ARMAGEDDON_VISUAL3                    = 24207, // This shouldn't correct but same as seen on the movie
    SPELL_ARMAGEDDON_SUMMON_TRIGGER             = 45921, // summons the triggers that cast the spells on himself need random target select
    SPELL_ARMAGEDDON_DAMAGE                     = 45915, // This does the area damage

    // Shield Orb Spells*/
    SPELL_SHADOW_BOLT                           = 45679,

    // Anveena's spells and cosmetics (Or, generally, everything that has "Anveena" in name)
    SPELL_ANVEENA_PRISON                        = 46367, // She hovers locked within a bubble
    SPELL_ANVEENA_ENERGY_DRAIN                  = 46410, // Sunwell energy glow animation (Control mob uses this)
    SPELL_SACRIFICE_OF_ANVEENA                  = 46474, // This is cast on Kil'Jaeden when Anveena sacrifices herself into the Sunwell

    // Sinister Reflection Spells
    SPELL_SR_CURSE_OF_AGONY                     = 46190,
    SPELL_SR_SHADOW_BOLT                        = 47076,

    SPELL_SR_EARTH_SHOCK                        = 47071,

    SPELL_SR_FIREBALL                           = 47074,

    SPELL_SR_HEMORRHAGE                         = 45897,

    SPELL_SR_HOLY_SHOCK                         = 38921,
    SPELL_SR_HAMMER_OF_JUSTICE                  = 37369,

    SPELL_SR_HOLY_SMITE                         = 47077,
    SPELL_SR_RENEW                              = 47079,

    SPELL_SR_SHOOT                              = 16496,
    SPELL_SR_MULTI_SHOT                         = 48098,
    SPELL_SR_WING_CLIP                          = 40652,

    SPELL_SR_WHIRLWIND                          = 17207,

    SPELL_SR_MOONFIRE                           = 47072,
    //SPELL_SR_PLAGU STRIKE                       = 58843, Dk spell!

    // Other Spells (used by players, etc)
    SPELL_SUMMON_BLUE_DRAKE                     = 45836,
    SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT          = 45839, // Possess the blue dragon from the orb to help the raid.
    SPELL_ENTROPIUS_BODY                        = 46819, // Visual for Entropius at the Epilogue
    SPELL_RING_OF_BLUE_FLAMES                   = 45825, //Cast this spell when the go is activated
    SPELL_SUNWELL_IGNITION                      = 46822,
};

// Error messages
#define ERROR_KJ_NOT_SUMMONED "TSCR ERROR: Unable to summon Kil'Jaeden for some reason"

// Others
#define FLOOR_Z         28.050388f
#define SHIELD_ORB_Z    45.000f

enum phase
{
    PHASE_DECEIVERS     = 1, // Fight 3 adds
    PHASE_NORMAL        = 2, // Kil'Jaeden emerges from the sunwell
    PHASE_DARKNESS      = 3, // At 85%, he gains few abilities; kalecgos joins the fight
    PHASE_ARMAGEDDON    = 4, // At 55%, he gains even more abilities
    PHASE_SACRIFICE     = 5, // At 25%, Anveena sacrifices herself into the Sunwell; at this point he becomes enraged and has *significally* shorter cooldowns.
};

//Timers
enum KilJaedenTimers
{
    TIMER_SPEECH           = 0,

    //phase 2 timer
    TIMER_SOUL_FLAY        = 1,
    TIMER_LEGION_LIGHTNING = 2,
    TIMER_FIRE_BLOOM       = 3,
    TIMER_SUMMON_SHILEDORB = 4,

    //phase 3 timer
    TIMER_SHADOW_SPIKE     = 5,
    TIMER_FLAME_DART       = 6,
    TIMER_DARKNESS         = 7,
    TIMER_ORBS_EMPOWER     = 8,

    //phase 4 timer
    TIMER_ARMAGEDDON       = 9
};

// Locations of the Hand of Deceiver adds
Position DeceiverLocations[3]=
{
    { 1682.045f, 631.299f, 28.0f, 5.936f },
    { 1684.099f, 618.848f, 28.0f, 0.589f },
    { 1694.170f, 612.272f, 28.0f, 1.416f },
};

// Locations, where Shield Orbs will spawn
float ShieldOrbLocations[4][2]=
{
    { 1698.900f, 627.870f     }, // Middle pont of Sunwell
    { 12,        3.14f        }, // First one spawns northeast of KJ
    { 12,        3.14f / 0.7f }, // Second one spawns southeast
    { 12,        3.14f * 3.8f }  // Third one spawns (?)
};

struct Speech
{
    int32 textid;
    uint32 creature, timer;
};

// Timers
static Speech Speeches[]=
{
    // Kil phase 1 -> phase 2
    { SAY_KJ_EMERGE,             DATA_KILJAEDEN,     0     },
    { SAY_KALECGOS_JOIN,         DATA_KALECGOS_KJ,   26000 },
    // Kil phase 2 -> phase 3
    { SAY_KALECGOS_AWAKEN,       DATA_KALECGOS_KJ,   10000 },
    { SAY_ANVEENA_IMPRISONED,    DATA_ANVEENA,       5000  },
    { SAY_KJ_PHASE3,             DATA_KILJAEDEN,     5000  },
    // Kil phase 3 -> phase 4
    { SAY_KALECGOS_LETGO,        DATA_KALECGOS_KJ,   10000 },
    { SAY_ANVEENA_LOST,          DATA_ANVEENA,       8000  },
    { SAY_KJ_PHASE4,             DATA_KILJAEDEN,     7000  },
    // Kil phase 4 -> phase 5
    { SAY_KALECGOS_FOCUS,        DATA_KALECGOS_KJ,   4000  },
    { SAY_ANVEENA_KALEC,         DATA_ANVEENA,       11000 },
    { SAY_KALECGOS_FATE,         DATA_KALECGOS_KJ,   2000  },
    { SAY_ANVEENA_GOODBYE,       DATA_ANVEENA,       6000  },
    { SAY_KJ_PHASE5,             DATA_KILJAEDEN,     5500  },

    // Use in End sequence?
    { SAY_KALECGOS_GOODBYE,      DATA_KALECGOS_KJ,   12000 },
};

//AI for kalecgos
class boss_kalecgos_kj : public CreatureScript
{
    public:
        boss_kalecgos_kj() : CreatureScript("boss_kalecgos_kj") { }

        struct boss_kalecgos_kjAI : public ScriptedAI
        {
            boss_kalecgos_kjAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            uint8 empowerCount;

            void Reset() override
            {
                empowerCount = 0;
                me->SetFlying(true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->setActive(true);

                ResetOrbs();
            }

            GameObject* GetOrb(int32 index)
            {
                if (!instance)
                    return nullptr;

                switch (index)
                {
                    case 0:
                        return instance->instance->GetGameObject(instance->GetGuidData(DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_1));
                    case 1:
                        return instance->instance->GetGameObject(instance->GetGuidData(DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_2));
                    case 2:
                        return instance->instance->GetGameObject(instance->GetGuidData(DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_3));
                    case 3:
                        return instance->instance->GetGameObject(instance->GetGuidData(DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_4));
                }

                return nullptr;
            }

            void ResetOrbs()
            {
                me->RemoveDynObject(SPELL_RING_OF_BLUE_FLAMES);
                for (uint8 i = 0; i < 4; ++i)
                {
                    if (GameObject* orb = GetOrb(i))
                    {
                        orb->Respawn();
                        orb->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        orb->UpdateObjectVisibility();
                    }
                }
            }

            void EmpowerOrb(bool all)
            {
                ResetOrbs();
                if (all)
                {
                    me->RemoveDynObject(SPELL_RING_OF_BLUE_FLAMES);
                    for (uint8 i = 0; i < 4; ++i)
                    {
                        if (GameObject* orb = GetOrb(i))
                        {
                            orb->CastSpell(me, SPELL_RING_OF_BLUE_FLAMES);
                            orb->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        }
                    }
                    Talk(SAY_KALECGOS_ENCOURAGE);
                }
                else
                {
                    if (GameObject* orb = GetOrb(urand(0, 3)))
                    {
                        orb->Respawn();
                        orb->CastSpell(me, SPELL_RING_OF_BLUE_FLAMES);
                        orb->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        orb->UpdateObjectVisibility();

                        ++empowerCount;
                        switch (empowerCount)
                        {
                            case 1: Talk(SAY_KALECGOS_READY1); break;
                            case 2: Talk(SAY_KALECGOS_READY2); break;
                            case 3: Talk(SAY_KALECGOS_READY3); break;
                            case 4: Talk(SAY_KALECGOS_READY4); break;
                        }
                    }
                }
            }

            void UpdateAI(uint32 /*diff*/) override { }

            void SetRingOfBlueFlames()
            {
                me->RemoveDynObject(SPELL_RING_OF_BLUE_FLAMES);
                for (uint8 i = 0; i < 4; ++i)
                    if (GameObject* orb = GetOrb(i))
                        if (!orb->HasFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE))
                            orb->CastSpell(me, SPELL_RING_OF_BLUE_FLAMES);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_kalecgos_kjAI(creature);
        }
};

// AI for Kil'jaeden Event Controller
class npc_kiljaeden_controller : public CreatureScript
{
    public:
        npc_kiljaeden_controller() : CreatureScript("npc_kiljaeden_controller") { }

        struct npc_kiljaeden_controllerAI : public ScriptedAI
        {
            npc_kiljaeden_controllerAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                instance = creature->GetInstanceScript();
                SetCombatMovement(false);
            }

            InstanceScript* instance;
            SummonList summons;
            uint32 randomSayTimer;
            uint32 phase;
            uint8 deceiverDeathCount;
            bool summonedDeceivers;
            bool kiljaedenDeath;

            void InitializeAI() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->AddUnitState(UNIT_STATE_STUNNED);

                ScriptedAI::InitializeAI();
            }

            void Reset() override
            {
                phase = PHASE_DECEIVERS;

                if (instance)
                    if (Creature* kalecKJ = Unit::GetCreature((*me), instance->GetGuidData(DATA_KALECGOS_KJ)))
                        CAST_AI(boss_kalecgos_kj::boss_kalecgos_kjAI, kalecKJ->AI())->ResetOrbs();
                deceiverDeathCount = 0;
                summonedDeceivers = false;
                kiljaedenDeath = false;
                randomSayTimer = 30000;
                summons.DespawnAll();
            }

            void JustSummoned(Creature* summon) override
            {
                switch (summon->GetEntry())
                {
                    case NPC_HAND_OF_THE_DECEIVER:
                        summon->CastSpell(summon, SPELL_SHADOW_CHANNELING, false);
                        break;
                    case NPC_ANVEENA:
                        summon->SetFlying(true);
                        summon->CastSpell(summon, SPELL_ANVEENA_PRISON, true);
                        summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        break;
                    case NPC_KILJAEDEN:
                        summon->CastSpell(summon, SPELL_REBIRTH, false);
                        summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        summon->m_Events.Schedule(10000, [summon]
                        {
                            summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                            summon->AI()->DoZoneInCombat(summon, 100.0f);
                            if (!summon->IsInCombat())
                                summon->AI()->EnterEvadeMode();
                        });
                        //summon->AddThreat(me->GetVictim(), 1.0f);
                        break;
                }
                summons.Summon(summon);
            }

            void UpdateAI(uint32 diff) override
            {
                me->setActive(instance->GetData(DATA_KILJAEDEN_EVENT) == IN_PROGRESS);

                if (instance->GetData(DATA_KILJAEDEN_EVENT) == DONE)
                    return;

                if (randomSayTimer < diff)
                {
                    if (instance && instance->GetData(DATA_MURU_EVENT) == DONE && instance->GetData(DATA_KILJAEDEN_EVENT) == NOT_STARTED)
                        Talk(SAY_KJ_OFFCOMBAT);
                    randomSayTimer = 30000;
                } else randomSayTimer -= diff;

                if (!summonedDeceivers)
                {
                    for (uint8 i = 0; i < 3; ++i)
                        me->SummonCreature(NPC_HAND_OF_THE_DECEIVER, DeceiverLocations[i], TEMPSUMMON_DEAD_DESPAWN, 0ms);

                    DoSpawnCreature(NPC_ANVEENA,  0, 0, 40, 0, TEMPSUMMON_DEAD_DESPAWN, 0ms);
                    DoCast(me, SPELL_ANVEENA_ENERGY_DRAIN);
                    summonedDeceivers = true;
                }

                if (deceiverDeathCount > 2 && phase == PHASE_DECEIVERS)
                {
                    me->RemoveAurasDueToSpell(SPELL_ANVEENA_ENERGY_DRAIN);
                    phase = PHASE_NORMAL;
                    DoSpawnCreature(NPC_KILJAEDEN, 0, 0, 0, 3.85f, TEMPSUMMON_MANUAL_DESPAWN, 0ms);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_kiljaeden_controllerAI(creature);
        }
};


//AI for Kil'jaeden
class boss_kiljaeden : public CreatureScript
{
    public:
        boss_kiljaeden() : CreatureScript("boss_kiljaeden") { }

        struct boss_kiljaedenAI : public ScriptedAI
        {
            boss_kiljaedenAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                instance = creature->GetInstanceScript();
                SetCombatMovement(false);
            }

            InstanceScript* instance;
            SummonList summons;
            uint32 timer[10];
            uint32 waitTimer;
            uint32 SpeechTimer;
            uint8 phase;
            uint8 ActiveTimers;
            uint8 speechCount;
            uint8 speechPhaseEnd;
            bool isInDarkness;
            bool TimerIsDeactivated[10];
            bool IsWaiting;
            bool OrbActivated;
            bool SpeechBegins;

            void InitializeAI() override
            {
                ScriptedAI::InitializeAI();
            }

            void Reset() override
            {
                TimerIsDeactivated[TIMER_SPEECH] = false;
                timer[TIMER_SPEECH]           = 0;

                // phase 2 timer
                timer[TIMER_SOUL_FLAY]        = 11000;
                timer[TIMER_LEGION_LIGHTNING] = 30000;
                timer[TIMER_FIRE_BLOOM]       = 20000;
                timer[TIMER_SUMMON_SHILEDORB] = 35000;

                // phase 3 timer
                timer[TIMER_SHADOW_SPIKE]     = 4000;
                timer[TIMER_FLAME_DART]       = 3000;
                timer[TIMER_DARKNESS]         = 45000;
                timer[TIMER_ORBS_EMPOWER]     = 35000;

                // phase 4 timer
                timer[TIMER_ARMAGEDDON]       = 2000;

                ActiveTimers = 5;
                waitTimer    = 0;
                speechCount = 0;
                SpeechTimer = 0;

                phase = PHASE_NORMAL;

                isInDarkness  = false;
                IsWaiting     = false;
                OrbActivated  = false;
                SpeechBegins  = true;

                if (instance)
                {
                    if (Creature* pKalec = Unit::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_KJ)))
                        pKalec->RemoveDynObject(SPELL_RING_OF_BLUE_FLAMES);
                }
                ChangeTimers(false, 0);
                summons.DespawnAll();
            }

            void ChangeTimers(bool status, uint32 WTimer)
            {
                for (uint8 i = 1; i < ActiveTimers; ++i)
                    TimerIsDeactivated[i] = status;

                if (WTimer > 0)
                {
                    IsWaiting = true;
                    waitTimer = WTimer;
                }

                if (OrbActivated)
                    TimerIsDeactivated[TIMER_ORBS_EMPOWER] = true;
                if (timer[TIMER_SHADOW_SPIKE] == 0)
                    TimerIsDeactivated[TIMER_SHADOW_SPIKE] = true;
                if (phase == PHASE_SACRIFICE)
                    TimerIsDeactivated[TIMER_SUMMON_SHILEDORB] = true;
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_ARMAGEDDON_TARGET)
                {
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    //summon->SetVisible(false);  //with this we cant see the armageddon visuals
                }
                else
                    summon->SetLevel(me->GetLevel());

                summon->SetFaction(me->GetFaction());
                summons.Summon(summon);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_KJ_DEATH);
                summons.DespawnAll();

                if (instance)
                {
                    instance->SetData(DATA_KILJAEDEN_EVENT, DONE);

                    if (Creature* control = Unit::GetCreature(*me, instance->GetGuidData(DATA_KILJAEDEN_CONTROLLER)))
                        control->m_Events.Schedule(30000, [control]() { control->CastSpell(control, SPELL_SUNWELL_IGNITION, true); });

                    if (Creature* kalecKJ = Unit::GetCreature((*me), instance->GetGuidData(DATA_KALECGOS_KJ)))
                        CAST_AI(boss_kalecgos_kj::boss_kalecgos_kjAI, kalecKJ->AI())->ResetOrbs();
                }
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KJ_SLAY);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                ScriptedAI::EnterEvadeMode();
                summons.DespawnAll();

                // Reset the controller
                if (instance)
                {
                    instance->SetData(DATA_KILJAEDEN_EVENT, NOT_STARTED);
                    if (Creature* control = Unit::GetCreature(*me, instance->GetGuidData(DATA_KILJAEDEN_CONTROLLER)))
                        CAST_AI(npc_kiljaeden_controller::npc_kiljaeden_controllerAI, control->AI())->Reset();
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (instance->GetData(DATA_KALECGOS_EVENT) != DONE || instance->GetData(DATA_BRUTALLUS_EVENT) != DONE || instance->GetData(DATA_FELMYST_EVENT) != DONE)
                {
                    instance->DoCastSpellOnPlayers(SPELL_ELEPORT_TO_APEX_POINT);
                    EnterEvadeMode();
                    return;
                }
                if (instance->GetData(DATA_EREDAR_TWINS_EVENT) != DONE)
                {
                    instance->DoCastSpellOnPlayers(SPELL_TELEPORT_TO_WITCHS_SANCTUM);
                    EnterEvadeMode();
                    return;
                }
                if (instance->GetData(DATA_MURU_EVENT) != DONE)
                {
                    instance->DoCastSpellOnPlayers(SPELL_TELEPORT_TO_SUNWELL_PLATEAU);
                    EnterEvadeMode();
                    return;
                }

                DoZoneInCombat();
            }

            void EnterNextPhase()
            {
                SpeechBegins  = true;
                OrbActivated  = false;
                ChangeTimers(true, 0); // Stop every cast Shadow spike will reactivate em all
                TimerIsDeactivated[TIMER_SHADOW_SPIKE] = false;
                timer[TIMER_SHADOW_SPIKE]     = 100;
                // Empowered orbs before darkness
                timer[TIMER_DARKNESS] = (phase == PHASE_SACRIFICE) ? 15000 : urand(10000,40000);
                timer[TIMER_ORBS_EMPOWER] = (phase == PHASE_SACRIFICE) ? 10000 : 5000; // 45000 35000
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_SINISTER_REFLECTION)
                {
                    target->CastSpell(target, SPELL_SINISTER_REFLECTION_SUMMON, true);
                    target->CastSpell(target, SPELL_SINISTER_REFLECTION_CLONE, true);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || phase < PHASE_NORMAL)
                    return;

                if (IsWaiting)
                {
                    if (waitTimer <= diff)
                    {
                        IsWaiting = false;
                        ChangeTimers(false, 0);
                    } else waitTimer -= diff;
                }

                for (uint8 t = 0; t < ActiveTimers; ++t)
                {
                    if (timer[t] < diff && !TimerIsDeactivated[t])
                    {
                        switch (t)
                        {
                            case TIMER_SPEECH:
                                if (SpeechBegins)
                                {
                                    SpeechBegins=false;
                                    switch (phase)
                                    {
                                        case PHASE_NORMAL:
                                            speechPhaseEnd=1;
                                            break;
                                        case PHASE_DARKNESS:
                                            speechPhaseEnd=4;
                                            break;
                                        case PHASE_ARMAGEDDON:
                                            speechPhaseEnd=7;
                                            break;
                                        case PHASE_SACRIFICE:
                                            speechPhaseEnd=12;
                                            break;
                                    }
                                }
                                if (Speeches[speechCount].timer < SpeechTimer)
                                {
                                    SpeechTimer = 0;
                                    if (instance)
                                        if (Creature* speechCreature = Unit::GetCreature(*me, instance->GetGuidData(Speeches[speechCount].creature)))
                                            Talk(Speeches[speechCount].textid, speechCreature);
                                    if (speechCount == 12)
                                        if (Creature* pAnveena =  Unit::GetCreature(*me, instance->GetGuidData(DATA_ANVEENA)))
                                            pAnveena->CastSpell(me, SPELL_SACRIFICE_OF_ANVEENA, false);
                                            //   ChangeTimers(true, 10000); // Kil should do an emote while screaming without attacking for 10 seconds
                                    if (speechCount == speechPhaseEnd)
                                        TimerIsDeactivated[TIMER_SPEECH]=true;
                                    speechCount++;
                                }
                                SpeechTimer += diff;
                                break;
                            case TIMER_SOUL_FLAY:
                                if (!me->IsNonMeleeSpellCasted(false))
                                {
                                    DoCast(me->GetVictim(), SPELL_SOUL_FLAY_SLOW, false);
                                    DoCast(me->GetVictim(), SPELL_SOUL_FLAY, false);
                                    timer[TIMER_SOUL_FLAY] = 3500;
                                }
                                break;
                            case TIMER_LEGION_LIGHTNING:
                                if (!me->IsNonMeleeSpellCasted(false))
                                {
                                    Unit* randomPlayer;

                                    me->RemoveAurasDueToSpell(SPELL_SOUL_FLAY);
                                    for (uint8 z = 0; z < 6; ++z)
                                    {
                                        randomPlayer = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);
                                        if (!randomPlayer || !randomPlayer->HasAura(SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT,0))
                                            break;
                                    }

                                    if (randomPlayer)
                                        DoCast(randomPlayer, SPELL_LEGION_LIGHTNING, false);
                                    else
                                        TC_LOG_ERROR("scripts", "try to cast SPELL_LEGION_LIGHTNING on invalid target");

                                    timer[TIMER_LEGION_LIGHTNING] = (phase == PHASE_SACRIFICE) ? 18000 : 30000; // 18 seconds in PHASE_SACRIFICE
                                    timer[TIMER_SOUL_FLAY] = 2500;
                                }
                                break;
                            case TIMER_FIRE_BLOOM:
                                if (!me->IsNonMeleeSpellCasted(false))
                                {
                                    me->RemoveAurasDueToSpell(SPELL_SOUL_FLAY);
                                    DoCastAOE(SPELL_FIRE_BLOOM, false);
                                    timer[TIMER_FIRE_BLOOM] = (phase == PHASE_SACRIFICE) ? 25000 : 40000; // 25 seconds in PHASE_SACRIFICE
                                    timer[TIMER_SOUL_FLAY] = 1000;
                                }
                                break;
                            case TIMER_SUMMON_SHILEDORB:
                                for (uint8 i = 1; i < phase; ++i)
                                {
                                    float sx, sy;
                                    sx = ShieldOrbLocations[0][0] + sin(ShieldOrbLocations[i][0]);
                                    sy = ShieldOrbLocations[0][1] + sin(ShieldOrbLocations[i][1]);
                                    me->SummonCreature(NPC_SHIELD_ORB, sx, sy, SHIELD_ORB_Z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 45000ms);
                                }
                                timer[TIMER_SUMMON_SHILEDORB] = urand(30000,60000); // 30-60seconds cooldown
                                timer[TIMER_SOUL_FLAY] = 2000;
                                break;
                            case TIMER_SHADOW_SPIKE: //phase 3
                                if (!me->IsNonMeleeSpellCasted(false))
                                {
                                    Talk(SAY_KJ_REFLECTION);
                                    for (uint8 i = 0; i < 4; ++i)
                                        DoCastAOE(SPELL_SINISTER_REFLECTION, true);
                                    DoCastAOE(SPELL_SHADOW_SPIKE, false);
                                    ChangeTimers(true, 30000);
                                    timer[TIMER_SHADOW_SPIKE] = 0;
                                    TimerIsDeactivated[TIMER_SPEECH] = false;
                                }
                                break;
                            case TIMER_FLAME_DART: //phase 3
                                DoCastAOE(SPELL_FLAME_DART, false);
                                timer[TIMER_FLAME_DART] = 3000; //TODO timer
                                break;
                            case TIMER_DARKNESS: //phase 3
                                if (!me->IsNonMeleeSpellCasted(false))
                                {
                                    // Begins to channel for 8 seconds, then deals 50'000 damage to all raid members.
                                    if (!isInDarkness)
                                    {
                                        Talk(EMOTE_KJ_DARKNESS);
                                        DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS, false);
                                        ChangeTimers(true, 9000);
                                        timer[TIMER_DARKNESS] = 8750;
                                        TimerIsDeactivated[TIMER_DARKNESS] = false;
                                        if (phase == PHASE_SACRIFICE)
                                            TimerIsDeactivated[TIMER_ARMAGEDDON] = false;
                                        isInDarkness = true;
                                    }
                                    else
                                    {
                                        timer[TIMER_DARKNESS] = (phase == PHASE_SACRIFICE) ? 15000 : urand(40000,70000);
                                        isInDarkness = false;
                                        DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                                        Talk(SAY_KJ_DARKNESS);
                                    }
                                    timer[TIMER_SOUL_FLAY] = 9000;
                                }
                                break;
                            case TIMER_ORBS_EMPOWER: //phase 3
                                if (instance)
                                    if (Creature* pKalec = Unit::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_KJ)))
                                     {
                                        switch (phase)
                                        {
                                        case PHASE_SACRIFICE:
                                            CAST_AI(boss_kalecgos_kj::boss_kalecgos_kjAI, pKalec->AI())->EmpowerOrb(true);
                                            break;
                                        default:
                                            CAST_AI(boss_kalecgos_kj::boss_kalecgos_kjAI, pKalec->AI())->EmpowerOrb(false);
                                            break;
                                    }
                                }
                                OrbActivated = true;
                                TimerIsDeactivated[TIMER_ORBS_EMPOWER] = true;
                                break;
                            case TIMER_ARMAGEDDON: //phase 4
                                Unit *target;
                                for (uint8 z = 0; z < 6; ++z)
                                {
                                    target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);
                                    if (!target || !target->HasAura(SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT,0)) break;
                                }
                                if (target)
                                {
                                    float x, y, z;
                                    target->GetPosition(x, y, z);
                                    me->SummonCreature(NPC_ARMAGEDDON_TARGET, x,y,z,0, TEMPSUMMON_TIMED_DESPAWN,15000ms);
                                }
                                timer[TIMER_ARMAGEDDON] = 2000; // No, I'm not kidding
                                break;
                         }
                    }
                }
                DoMeleeAttackIfReady();
                // Time runs over!
                for (uint8 i = 0; i < ActiveTimers; ++i)
                    if (!TimerIsDeactivated[i])
                    {
                        timer[i] -= diff;
                        if (((int32)timer[i]) < 0) timer[i] = 0;
                    }

                // Phase 3
                if (phase <= PHASE_NORMAL && !isInDarkness)
                {
                    if (phase == PHASE_NORMAL && HealthBelowPct(85))
                    {
                        phase = PHASE_DARKNESS;
                        ActiveTimers = 9;
                        EnterNextPhase();
                    }
                    else return;
                }

                // Phase 4
                if (phase <= PHASE_DARKNESS && !isInDarkness)
                {
                    if (phase == PHASE_DARKNESS && HealthBelowPct(55))
                    {
                        phase = PHASE_ARMAGEDDON;
                        ActiveTimers = 10;
                        EnterNextPhase();
                    }
                    else return;
                }

                // Phase 5 specific spells all we can
                if (phase <= PHASE_ARMAGEDDON && !isInDarkness)
                {
                    if (phase == PHASE_ARMAGEDDON && HealthBelowPct(25))
                    {
                        phase = PHASE_SACRIFICE;
                        EnterNextPhase();
                    }
                    else return;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_kiljaedenAI(creature);
        }
};

// AI for Hand of the Deceiver
class npc_hand_of_the_deceiver : public CreatureScript
{
    public:
        npc_hand_of_the_deceiver() : CreatureScript("npc_hand_of_the_deceiver") { }

        struct npc_hand_of_the_deceiverAI : public ScriptedAI
        {
            npc_hand_of_the_deceiverAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            uint32 shadowBoltVolleyTimer;
            uint32 felfirePortalTimer;

            void Reset() override
            {
                // TODO: Timers!
                shadowBoltVolleyTimer = urand(8000,14000); // So they don't all cast it in the same moment.
                felfirePortalTimer = 20000;
                if (instance)
                    instance->SetData(DATA_KILJAEDEN_EVENT, NOT_STARTED);
            }

            void JustSummoned(Creature* summon) override
            {
                summon->SetFaction(me->GetFaction());
                summon->SetLevel(me->GetLevel());
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (instance->GetData(DATA_KALECGOS_EVENT) != DONE || instance->GetData(DATA_BRUTALLUS_EVENT) != DONE || instance->GetData(DATA_FELMYST_EVENT) != DONE)
                {
                    instance->DoCastSpellOnPlayers(SPELL_ELEPORT_TO_APEX_POINT);
                    EnterEvadeMode();
                    return;
                }

                if (instance->GetData(DATA_EREDAR_TWINS_EVENT) != DONE)
                {
                    instance->DoCastSpellOnPlayers(SPELL_TELEPORT_TO_WITCHS_SANCTUM);
                    EnterEvadeMode();
                    return;
                }

                if (instance->GetData(DATA_MURU_EVENT) != DONE)
                {
                    instance->DoCastSpellOnPlayers(SPELL_TELEPORT_TO_SUNWELL_PLATEAU);
                    EnterEvadeMode();
                    return;
                }

                if (instance)
                {
                    instance->SetData(DATA_KILJAEDEN_EVENT, IN_PROGRESS);
                    //if (Creature* сontrol = Unit::GetCreature(*me, instance->GetGuidData(DATA_KILJAEDEN_CONTROLLER)))
                    //    сontrol->AddThreat(who, 1.0f);
                }
                me->InterruptNonMeleeSpells(true);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (!instance)
                    return;

                if (Creature* control = Unit::GetCreature(*me, instance->GetGuidData(DATA_KILJAEDEN_CONTROLLER)))
                    ++(CAST_AI(npc_kiljaeden_controller::npc_kiljaeden_controllerAI, control->AI())->deceiverDeathCount);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->IsInCombat())
                    DoCast(me, SPELL_SHADOW_CHANNELING);

                if (!UpdateVictim())
                    return;

                // Gain Shadow Infusion at 20% health
                if (HealthBelowPct(20) && !me->HasAura(SPELL_SHADOW_INFUSION, 0))
                    DoCast(me, SPELL_SHADOW_INFUSION, true);

                // Shadow Bolt Volley - Shoots Shadow Bolts at all enemies within 30 yards, for ~2k Shadow damage.
                if (shadowBoltVolleyTimer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_SHADOW_BOLT_VOLLEY);
                    shadowBoltVolleyTimer = 12000;
                }
                else
                    shadowBoltVolleyTimer -= diff;

                // Felfire Portal - Creatres a portal, that spawns Volatile Felfire Fiends, which do suicide bombing.
                if (felfirePortalTimer <= diff)
                {
                    if (Creature* portal = DoSpawnCreature(NPC_FELFIRE_PORTAL, 0, 0,0, 0, TEMPSUMMON_TIMED_DESPAWN, 20000ms))
                    {
                        std::list<HostileReference*>::iterator itr;
                        for (auto&& itr : me->GetThreatManager().getThreatList())
                        {
                            Unit* unit = Unit::GetUnit(*me, itr->getUnitGuid());
                            if (unit)
                                portal->AddThreat(unit, 1.0f);
                        }
                    }
                    felfirePortalTimer = 20000;
                } else felfirePortalTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_hand_of_the_deceiverAI(creature);
        }
};

// AI for Felfire Portal
class npc_felfire_portal : public CreatureScript
{
    public:
        npc_felfire_portal() : CreatureScript("npc_felfire_portal") { }

        struct npc_felfire_portalAI : public ScriptedAI
        {
            npc_felfire_portalAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            uint32 spawnFiendTimer;

            void Reset() override
            {
                spawnFiendTimer = 5000;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_NON_ATTACKABLE);
            }

            void JustSummoned(Creature* summon) override
            {
                summon->SetFaction(me->GetFaction());
                summon->SetLevel(me->GetLevel());
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (spawnFiendTimer <= diff)
                {
                    if (Creature* pFiend = DoSpawnCreature(NPC_VOLATILE_FELFIRE_FIEND, 0, 0, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20000ms))
                        pFiend->AddThreat(SelectTarget(SELECT_TARGET_RANDOM,0), 100000.0f);
                    spawnFiendTimer = urand(4000,8000);
                } else spawnFiendTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_felfire_portalAI(creature);
        }
};


// AI for Felfire Fiend
class npc_volatile_felfire_fiend : public CreatureScript
{
    public:
        npc_volatile_felfire_fiend() : CreatureScript("npc_volatile_felfire_fiend") { }

        struct npc_volatile_felfire_fiendAI : public ScriptedAI
        {
            npc_volatile_felfire_fiendAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 explodeTimer;
            bool lockedTarget;

            void Reset() override
            {
                explodeTimer = 2000;
                lockedTarget = false;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (damage > me->GetHealth())
                    DoCast(me, SPELL_FELFIRE_FISSION, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!lockedTarget)
                {
                    me->AddThreat(me->GetVictim(), 10000000.0f);
                    lockedTarget = true;
                }

                if (explodeTimer)
                {
                    if (explodeTimer <= diff)
                        explodeTimer = 0;
                    else explodeTimer -= diff;
                }
                else if (me->IsWithinDistInMap(me->GetVictim(), 3)) // Explode if it's close enough to it's target
                {
                    DoCast(me->GetVictim(), SPELL_FELFIRE_FISSION);
                    me->Kill(me);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_volatile_felfire_fiendAI(creature);
        }
};

// AI for Armageddon target
class npc_armageddon : public CreatureScript
{
    public:
        npc_armageddon() : CreatureScript("npc_armageddon") { }

        struct npc_armageddonAI : public ScriptedAI
        {
            npc_armageddonAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            uint32 timer;
            uint8 spell;

            void Reset() override
            {
                spell = 0;
                timer = 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (timer <= diff)
                {
                    switch (spell)
                    {
                        case 0:
                            DoCast(me, SPELL_ARMAGEDDON_VISUAL, true);
                            ++spell;
                            break;
                        case 1:
                            DoCast(me, SPELL_ARMAGEDDON_VISUAL2, true);
                            timer = 9000;
                            ++spell;
                            break;
                        case 2:
                            DoCast(me, SPELL_ARMAGEDDON_TRIGGER, true);
                            ++spell;
                            timer = 5000;
                            break;
                        case 3:
                            me->Kill(me);
                            me->RemoveCorpse();
                            break;
                    }
                } else timer -=diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_armageddonAI(creature);
        }
};

// AI for Shield Orbs
class npc_shield_orb : public CreatureScript
{
    public:
        npc_shield_orb() : CreatureScript("npc_shield_orb") { }

        struct npc_shield_orbAI : public ScriptedAI
        {
            npc_shield_orbAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            float x, y, r, c, mx, my;
            uint32 checkTimer;
            bool pointReached;
            bool clockwise;

            void Reset() override
            {
                me->SetFlying(true);
                pointReached = true;
                checkTimer = 1000;
                r = 17;
                c = 0;
                mx = ShieldOrbLocations[0][0];
                my = ShieldOrbLocations[0][1];
                clockwise = urand(0,1);

                DoCast(me, SPELL_SHADOW_BOLT, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (pointReached)
                {
                    if (clockwise)
                    {
                        y = my - r * sin(c);
                        x = mx - r * cos(c);
                    }
                    else
                    {
                        y = my + r * sin(c);
                        x = mx + r * cos(c);
                    }
                    pointReached = false;
                    checkTimer = 1000;
                    me->GetMotionMaster()->MovePoint(1, x, y, SHIELD_ORB_Z);
                    c += M_PI / 32;
                    if (c >= 2 * M_PI) c = 0;
                }
                else
                {
                    if (checkTimer <= diff)
                    {
                        DoTeleportTo(x, y , SHIELD_ORB_Z);
                        pointReached = true;
                    }
                    else checkTimer -= diff;
                }
            }

            void MovementInform(uint32 type, uint32 /*id*/) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                pointReached = true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shield_orbAI(creature);
        }
};

// AI for Sinister Reflection
class npc_sinster_reflection : public CreatureScript
{
    public:
        npc_sinster_reflection() : CreatureScript("npc_sinster_reflection") { }

        struct npc_sinster_reflectionAI : public ScriptedAI
        {
            npc_sinster_reflectionAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 timer[3];
            uint8 victimClass;

            void Reset() override
            {
                timer[0] = 0;
                timer[1] = 0;
                timer[2] = 0;
                victimClass = 0;
            }

            void IsSummonedBy(Unit* summoner) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                {
                    if (instance->GetData(DATA_KILJAEDEN_EVENT) != IN_PROGRESS)
                        me->DespawnOrUnsummon(1);
                    else if (Creature* kiljaeden = Unit::GetCreature(*me, instance->GetGuidData(DATA_KILJAEDEN)))
                        kiljaeden->AI()->JustSummoned(me);
                }

                DoCast(me, SPELL_SINISTER_REFLECTION_CLASS, true);
                AttackStart(summoner);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (victimClass == 0 && me->GetVictim())
                {
                    victimClass = me->GetVictim()->GetClass();
                    switch (victimClass)
                    {
                        case CLASS_DRUID:
                            break;
                        case CLASS_HUNTER:
                            break;
                        case CLASS_MAGE:
                            break;
                        case CLASS_WARLOCK:
                            break;
                        case CLASS_WARRIOR:
                            me->SetCanDualWield(true);
                            break;
                        case CLASS_PALADIN:
                            break;
                        case CLASS_PRIEST:
                            break;
                        case CLASS_SHAMAN:
                            me->SetCanDualWield(true);
                            break;
                        case CLASS_ROGUE:
                            me->SetCanDualWield(true);
                            break;
                    }
                }

                switch (victimClass)
                {
                    case CLASS_DRUID:
                        if (timer[1] <= diff)
                        {
                            DoCast(me->GetVictim(), SPELL_SR_MOONFIRE, false);
                            timer[1] = urand(2000,4000);
                        }
                        DoMeleeAttackIfReady();
                        break;
                    case CLASS_HUNTER:
                        if (timer[1] <= diff)
                        {
                            DoCast(me->GetVictim(), SPELL_SR_MULTI_SHOT, false);
                            timer[1] = urand(8000,10000);
                        }
                        if (timer[2] <= diff)
                        {
                            DoCast(me->GetVictim(), SPELL_SR_SHOOT, false);
                            timer[2] = urand(4000,6000);
                        }
                        if (me->IsWithinMeleeRange(me->GetVictim()))
                        {
                            if (timer[0] <= diff)
                            {
                                DoCast(me->GetVictim(), SPELL_SR_WING_CLIP, false);
                                timer[0] = urand(6000,8000);
                            }
                            DoMeleeAttackIfReady();
                        }
                        break;
                    case CLASS_MAGE:
                        if (timer[1] <= diff)
                        {
                            DoCast(me->GetVictim(), SPELL_SR_FIREBALL, false);
                            timer[1] = urand(2000,4000);
                        }
                        DoMeleeAttackIfReady();
                        break;
                    case CLASS_WARLOCK:
                        if (timer[1] <= diff)
                        {
                            DoCast(me->GetVictim(), SPELL_SR_SHADOW_BOLT, false);
                            timer[1] = urand(3000,5000);
                        }
                        if (timer[2] <= diff)
                        {
                            DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true), SPELL_SR_CURSE_OF_AGONY, true);
                            timer[2] = urand(2000,4000);
                        }
                        DoMeleeAttackIfReady();
                        break;
                    case CLASS_WARRIOR:
                        if (timer[1] <= diff)
                        {
                            DoCast(me->GetVictim(), SPELL_SR_WHIRLWIND, false);
                            timer[1] = urand(9000,11000);
                        }
                        DoMeleeAttackIfReady();
                        break;
                    case CLASS_PALADIN:
                        if (timer[1] <= diff)
                        {
                            DoCast(me->GetVictim(), SPELL_SR_HAMMER_OF_JUSTICE, false);
                            timer[1] = urand(6000,8000);
                        }
                        if (timer[2] <= diff)
                        {
                            DoCast(me->GetVictim(), SPELL_SR_HOLY_SHOCK, false);
                            timer[2] = urand(2000,4000);
                        }
                        DoMeleeAttackIfReady();
                        break;
                    case CLASS_PRIEST:
                        if (timer[1] <= diff)
                        {
                            DoCast(me->GetVictim(), SPELL_SR_HOLY_SMITE, false);
                            timer[1] = urand(4000,6000);
                        }
                        if (timer[2] <= diff)
                        {
                            DoCast(me, SPELL_SR_RENEW, false);
                            timer[2] = urand(6000,8000);
                        }
                        DoMeleeAttackIfReady();
                        break;
                    case CLASS_SHAMAN:
                        if (timer[1] <= diff)
                        {
                            DoCast(me->GetVictim(), SPELL_SR_EARTH_SHOCK, false);
                            timer[1] = urand(4000,6000);
                        }
                        DoMeleeAttackIfReady();
                        break;
                    case CLASS_ROGUE:
                        if (timer[1] <= diff)
                        {
                            DoCast(me->GetVictim(), SPELL_SR_HEMORRHAGE, true);
                            timer[1] = urand(4000,6000);
                        }
                        DoMeleeAttackIfReady();
                        break;
                    }
                    TC_LOG_DEBUG("scripts", "Sinister-timer");
                    for (uint8 i = 0; i < 3; ++i)
                        timer[i] -= diff;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sinster_reflectionAI(creature);
        }
};

class npc_power_of_the_blue_flight : public CreatureScript
{
    public:
        npc_power_of_the_blue_flight() : CreatureScript("npc_power_of_the_blue_flight") { }

        struct npc_power_of_the_blue_flightAI : public ScriptedAI
        {
            npc_power_of_the_blue_flightAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL_PCT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL_MAX_HEALTH, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_DISPEL, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_PERIODIC_HEAL, true);
            }

            void OnCharmed(bool apply) override
            {
                if (!apply)
                    me->DespawnOrUnsummon(1);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_power_of_the_blue_flightAI(creature);
        }
};

class spell_sinister_reflection_clone : public SpellScriptLoader
{
    public:
        spell_sinister_reflection_clone() : SpellScriptLoader("spell_sinister_reflection_clone") { }

        class spell_sinister_reflection_clone_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sinister_reflection_clone_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                Unit* caster = GetCaster();
                if (!caster)
                {
                    targets.clear();
                    return;
                }

                targets.remove_if([caster](WorldObject* target)
                {
                    if (Unit* unit = target->ToUnit())
                        if (unit->GetEntry() == NPC_SINISTER_REFLECTION)
                            if (TempSummon* summon = unit->ToTempSummon())
                                if (summon->GetSummonerGUID() == caster->GetGUID())
                                    return false;

                    return true;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sinister_reflection_clone_SpellScript::FilterTargets, EFFECT_ALL, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sinister_reflection_clone_SpellScript();
        }
};

class spell_power_of_the_blue_flight : public SpellScriptLoader
{
    public:
        spell_power_of_the_blue_flight() : SpellScriptLoader("spell_power_of_the_blue_flight") { }

        class spell_power_of_the_blue_flight_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_power_of_the_blue_flight_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Player* target = GetHitPlayer();
                if (!target)
                    return;

                InstanceScript* instance = target->GetInstanceScript();
                if (!instance)
                    return;

                GameObject* orb = nullptr;
                for (uint8 i = 0; i < 4 && !orb; ++i)
                    if (GameObject* temp = ObjectAccessor::GetGameObject(*target, instance->GetGuidData(DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_1 + i)))
                        if (target->GetExactDist(temp) <= 20.0f)
                            orb = temp;

                if (!orb || orb->HasFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE))
                    return;

                orb->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                orb->SetRespawnTime(WEEK);
                orb->UpdateObjectVisibility();

                target->CastSpell(target, SPELL_SUMMON_BLUE_DRAKE, true);
                target->CastSpell(target, SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT, true);

                if (Creature* kalecgos = ObjectAccessor::GetCreature(*target, instance->GetGuidData(DATA_KALECGOS_KJ)))
                    CAST_AI(boss_kalecgos_kj::boss_kalecgos_kjAI, kalecgos->AI())->SetRingOfBlueFlames();
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_power_of_the_blue_flight_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_power_of_the_blue_flight_SpellScript();
        }
};

void AddSC_boss_kiljaeden()
{
    new boss_kalecgos_kj();
    new boss_kiljaeden();
    new npc_kiljaeden_controller();
    new npc_hand_of_the_deceiver();
    new npc_felfire_portal();
    new npc_volatile_felfire_fiend();
    new npc_armageddon();
    new npc_shield_orb();
    new npc_sinster_reflection();
    new npc_power_of_the_blue_flight();
    new spell_sinister_reflection_clone();
    new spell_power_of_the_blue_flight();
}
