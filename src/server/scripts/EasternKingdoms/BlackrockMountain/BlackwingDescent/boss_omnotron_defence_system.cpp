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

#include"ScriptPCH.h"
#include"Spell.h"
#include "blackwing_descent.h"

/* vehicleids:
    arcanotron 863
    toxitron 864
    electron 862
    magmatron 865
*/

enum ScriptTexts
{
    SAY_AGGRO                   = 0,
    SAY_DEATH                   = 1,
    SAY_ELECTRON                = 2,
    SAY_MAGMATRON               = 3,
    SAY_TOXITRON                = 4,
    SAY_ARCANOTRON              = 5,
    SAY_ELECTRON_SHIELD         = 6,
    SAY_MAGMATRON_SHIELD        = 7,
    SAY_TOXITRON_SHIELD         = 8,
    SAY_ARCANOTRON_SHIELD       = 9,
    SAY_MAGMATRON_FLAMETHROWER  = 10,
};

// 82265 88430?
enum Spells
{
    SPELL_ZERO_POWER                                = 78725,
    SPELL_ACTIVE                                    = 78740,
    SPELL_INACTIVE                                  = 78746,
    SPELL_INACTIVE_AURA                             = 78726,
    SPELL_RECHARGE_BLUE                             = 78697, // Electron
    SPELL_RECHARGE_ORANGE                           = 78698, // Magmatron
    SPELL_RECHARGE_PURPLE                           = 78699, // Arcanotron
    SPELL_RECHARGE_GREEN                            = 78700, // Toxitron
    
    // Arcanotron
    SPELL_POWER_CONVERSION                          = 79729,
    SPELL_ARCANE_ANNIHILATOR                        = 79710,
    SPELL_POWER_GENERATOR                           = 79624,
    SPELL_POWER_GENERATOR_SUM                       = 79626,

    // Electron
    SPELL_UNSTABLE_SHIELD                           = 79900,
    SPELL_STATIC_SHOCK                              = 79911,
    SPELL_STATIC_SHOCK_DMG                          = 79912,
    SPELL_ELECTRICAL_DISCHARGE                      = 79879,
    SPELL_LIGHTNING_CONDUCTOR                       = 79888,
    SPELL_LIGHTNING_CONDUCTOR_AOE                   = 79889,

    // Magmatron
    SPELL_BARRIER                                   = 79582,
    SPELL_BACKDRAFT                                 = 79617,
    SPELL_ACQUIRING_TARGET                          = 79501,
    SPELL_FLAMETHROWER                              = 79504,
    SPELL_INSENERATION_SECURITY_MISSURE             = 79023,
    SPELL_INSENERATION_SECURITY_MISSURE_AOE         = 79035, 

    // Toxitron
    SPELL_POISON_SOAKED_SHELL                       = 79835,
    SPELL_CHEMICAL_BOMB                             = 80157,
    SPELL_POISON_PROTOCOL                           = 80053,
};

enum Summons_and_auras
{
    SPELL_POWER_GENERATOR_10                        = 79628,
    SPELL_POWER_GENERATOR_AOE                       = 79629,
    SPELL_OVERCHARGED_POWER_GENERATOR               = 91857,
    SPELL_OVERCHARGED_POWER_GENERATOR_AOE           = 91858,
    SPELL_ARCANE_BLOWBACK                           = 91879,
    SPELL_OVERCHARGE                                = 91881,
    
    SPELL_CHEMICAL_CLOUD_AOE_A                      = 80163,
    SPELL_CHEMICAL_CLOUD_AOE_A_AOE                  = 80164,
    SPELL_CHEMICAL_CLOUD_AOE_B                      = 80162,
    SPELL_CHEMICAL_CLOUD_AOE_B_AOE                  = 80161,
   
    SPELL_FIXATE                                    = 80094,
    SPELL_POISON_BOMB                               = 80092,

    SPELL_POISON_PUDDLE                             = 80095,
    SPELL_POISON_PUDDLE_AOE                         = 80097,
};

enum Adds
{
    NPC_POWER_GENERATOR = 42733,
    NPC_CHEMICAL_CLOUD  = 42934,
    NPC_POISON_PUDDLE   = 42920,
    NPC_POISON_BOMB     = 42897,
};

enum Events
{
    EVENT_POWER_GENERATOR               = 1,
    EVENT_ARCANE_ANNIHILATOR            = 2,
    EVENT_OVERCHARGED                   = 3,
    EVENT_ELECTRICAL_DISCHARGE          = 4,
    EVENT_LIGHTNING_CONDUCTOR           = 5,
    EVENT_ACQUIRING_TARGET              = 6,
    EVENT_INSENERATION_SECURITY_MISSURE = 7,
    EVENT_CHEMICAL_BOMB                 = 8,
    EVENT_POISON_PROTOCOL               = 9,
    EVENT_CONTINUE                      = 10,
};

enum Actions
{
    ACTION_ACTIVE       = 1,
    ACTION_NEXT         = 2,
    ACTION_EVADE_ALL    = 3,
    ACTION_KILL_ALL     = 4,
};

const Position omnotronnefariusspawnPos = {-326.04f, -356.88f, 223.0f, 4.92f};

class boss_omnotron : public CreatureScript
{
    public:
        boss_omnotron() : CreatureScript("boss_omnotron") { }

        struct boss_omnotronAI : public ScriptedAI
        {
            boss_omnotronAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                memset(m_uiGuids, 0, sizeof(m_uiGuids));
                current = 0;
                me->SetReactState(REACT_PASSIVE);
            }
            
            InstanceScript* instance;
            ObjectGuid m_uiGuids[4];
            uint8 current;

            void JustEngagedWith(Unit* who) override
            {
                if (instance->GetBossState(DATA_OMNOTRON) == IN_PROGRESS)
                    return;

                Talk(SAY_AGGRO);
                instance->SetBossState(DATA_OMNOTRON, IN_PROGRESS);

                me->GetMap()->SetWorldState(WORLDSTATE_ACHIEVE_A_TRON_SHOCK, 1);
                me->GetMap()->SetWorldState(WORLDSTATE_ACHIEVE_A_TRON_BOMB, 1);
                me->GetMap()->SetWorldState(WORLDSTATE_ACHIEVE_A_TRON_ANNIHILATOR, 1);
                me->GetMap()->SetWorldState(WORLDSTATE_ACHIEVE_A_TRON_FLAMETHOWER, 1);

                current = 0;

                if (Creature* arcanotron = Unit::GetCreature(*me, instance->GetGuidData(DATA_ARCANOTRON)))
                {
                    m_uiGuids[0] = arcanotron->GetGUID();
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, arcanotron); 
                    DoZoneInCombat(arcanotron);
                }
                if (Creature* electron = Unit::GetCreature(*me, instance->GetGuidData(DATA_ELECTRON)))
                {
                    m_uiGuids[1] = electron->GetGUID();
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, electron); 
                    DoZoneInCombat(electron);
                }
                if (Creature* magmatron = Unit::GetCreature(*me, instance->GetGuidData(DATA_MAGMATRON)))
                {
                    m_uiGuids[2] = magmatron->GetGUID();
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, magmatron); 
                    DoZoneInCombat(magmatron);
                }
                if (Creature* toxitron = Unit::GetCreature(*me, instance->GetGuidData(DATA_TOXITRON)))
                {
                    m_uiGuids[3] = toxitron->GetGUID();
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, toxitron); 
                    DoZoneInCombat(toxitron);
                }
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_NEXT:
                        if (Unit* omnotronnext = GetNextOmnotron())
                        {
                            omnotronnext->GetAI()->DoAction(ACTION_ACTIVE);
                            omnotronnext->RemoveAurasDueToSpell(SPELL_INACTIVE_AURA);
                            omnotronnext->SetPower(POWER_ENERGY, 100);
                            omnotronnext->CastSpell(omnotronnext,SPELL_ACTIVE, true);
                            switch (omnotronnext->GetEntry())
                            {
                                case NPC_ARCANOTRON: Talk(SAY_ARCANOTRON); break;
                                case NPC_ELECTRON: Talk(SAY_ELECTRON); break;
                                case NPC_MAGMATRON: Talk(SAY_MAGMATRON); break;
                                case NPC_TOXITRON: Talk(SAY_TOXITRON); break;
                            }
                        }
                        break;
                    case ACTION_EVADE_ALL:
                        EvadeAll();
                        break;
                }
            }
            private:
                Unit* GetNextOmnotron()
                {
                    current++;
                    if (current > 3) current = 0;

                    if (Unit* nextOmnotron = Unit::GetCreature(*me, m_uiGuids[current]))
                        return nextOmnotron;
                    else
                        return NULL;
                }

                void EvadeAll()
                {
                    if (Creature* arcanotron = Unit::GetCreature(*me, instance->GetGuidData(DATA_ARCANOTRON)))
                        if (arcanotron->IsAIEnabled)
                            arcanotron->AI()->EnterEvadeMode();
                    if (Creature* electron = Unit::GetCreature(*me, instance->GetGuidData(DATA_ELECTRON)))
                        if (electron->IsAIEnabled)
                            electron->AI()->EnterEvadeMode();
                    if (Creature* magmatron = Unit::GetCreature(*me, instance->GetGuidData(DATA_MAGMATRON)))
                        if (magmatron->IsAIEnabled)
                            magmatron->AI()->EnterEvadeMode();
                    if (Creature* toxitron = Unit::GetCreature(*me, instance->GetGuidData(DATA_TOXITRON)))
                        if (toxitron->IsAIEnabled)
                            toxitron->AI()->EnterEvadeMode();
                    EnterEvadeMode();
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_omnotronAI>(creature);
        }
};

struct omnotron_golem_base : public BossAI
{
    omnotron_golem_base(Creature* creature) : BossAI(creature, DATA_OMNOTRON)
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
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (!me || !me->IsAlive())
            return;

        uint32 health = me->GetHealth() > damage ? me->GetHealth() - damage : 0;
        instance->SetData(DATA_HEALTH_OMNOTRON_SHARED, health);
        if (!health)
        {
            if (Player* player = attacker->GetAffectingPlayer())
            {
                for (auto&& itr : { DATA_ELECTRON, DATA_MAGMATRON, DATA_TOXITRON, DATA_ARCANOTRON })
                    if (Creature* golem = Unit::GetCreature(*me, instance->GetGuidData(itr)))
                        if (golem != me)
                            golem->ResetPlayerDamageReq();

                me->SetLootRecipient(player);
                me->LowerPlayerDamageReq(me->GetMaxHealth());
            }
        }
    }
};

class boss_arcanotron : public CreatureScript
{
    public:
        boss_arcanotron() : CreatureScript("boss_arcanotron") { }

        struct boss_arcanotronAI : public omnotron_golem_base
        {
            boss_arcanotronAI(Creature* creature) : omnotron_golem_base(creature) { }

            uint8 stage;

            void Reset() override
            {
                _Reset();
                
                stage = 0;

                me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 5);
                me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 5);

                instance->SetData(DATA_HEALTH_OMNOTRON_SHARED, me->GetMaxHealth());
                me->SetPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 100);
                me->SetHealth(me->GetMaxHealth());
                DoCast(me, SPELL_ZERO_POWER, true); 
            }

            void SpellHit(Unit* caster, const SpellInfo* spell) override
            {
                if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                    if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_ARCANE_ANNIHILATOR)
                        for (uint8 i = 0; i < 3; ++i)
                            if (spell->Effects[i].Effect == SPELL_EFFECT_INTERRUPT_CAST)
                                me->InterruptSpell(CURRENT_GENERIC_SPELL, false);
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                instance->SetBossState(DATA_OMNOTRON, FAIL);
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_ACTIVE:
                        stage = 1;
                        me->RemoveAurasDueToSpell(SPELL_RECHARGE_PURPLE);
                        break;
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (Creature* omnotron = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                    DoZoneInCombat(omnotron);

                events.ScheduleEvent(EVENT_POWER_GENERATOR, 15000);
                events.ScheduleEvent(EVENT_ARCANE_ANNIHILATOR, 8000);
                stage = 1;
                DoCast(me, SPELL_ACTIVE, true);
                instance->SetData(DATA_HEALTH_OMNOTRON_SHARED, me->GetMaxHealth());
                if (IsHeroic())
                    if (Creature* pNefarius = me->SummonCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, omnotronnefariusspawnPos))
                        if (pNefarius->IsAIEnabled)
                            pNefarius->AI()->DoAction(ACTION_OMNOTRON_INTRO);
            }
            
            void JustDied(Unit* killer) override
            {
                _JustDied();
                instance->SetData(DATA_HEALTH_OMNOTRON_SHARED, 0);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                me->SetLootRecipient(NULL);

                if (Creature* electron = Unit::GetCreature(*me, instance->GetGuidData(DATA_ELECTRON)))
                {
                    killer->Kill(electron);
                    electron->SetLootRecipient(NULL);
                }
                if (Creature* magmatron = Unit::GetCreature(*me, instance->GetGuidData(DATA_MAGMATRON)))
                {
                    magmatron->LowerPlayerDamageReq(magmatron->GetMaxHealth());
                    killer->DealDamage(magmatron, magmatron->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false); // additional achievement controller
                    magmatron->SetLootRecipient(NULL);
                }
                if (Creature* toxitron = Unit::GetCreature(*me, instance->GetGuidData(DATA_TOXITRON)))
                {
                    toxitron->LowerPlayerDamageReq(toxitron->GetMaxHealth());
                    killer->DealDamage(toxitron, toxitron->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false); // achievement controller
                }
                if (Creature* omnotroncontroller = Unit::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                {
                    omnotroncontroller->Kill(omnotroncontroller);
                    omnotroncontroller->AI()->Talk(SAY_DEATH);
                }
                if (Creature* pNefarius = me->SummonCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f))
                    if (pNefarius->IsAIEnabled)
                        pNefarius->AI()->DoAction(ACTION_OMNOTRON_DEATH);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (instance->GetData(DATA_HEALTH_OMNOTRON_SHARED) != 0)
                    me->SetHealth(instance->GetData(DATA_HEALTH_OMNOTRON_SHARED));        

                if (stage == 0)
                    return;

                if (me->GetPower(POWER_ENERGY) <= 50 && stage == 1)
                {
                    stage = 2;
                    if (Creature* omnotroncontroller = Unit::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                        if (omnotroncontroller->IsAIEnabled)
                            omnotroncontroller->GetAI()->DoAction(ACTION_NEXT);
                    return;
                }
                else if (me->GetPower(POWER_ENERGY) <= 44 && stage == 2)
                {
                    stage = 3;
                    DoCast(me, SPELL_POWER_CONVERSION, true);
                    if (Creature* omnotroncontroller = Unit::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                        if (omnotroncontroller->IsAIEnabled)
                            omnotroncontroller->AI()->Talk(SAY_ARCANOTRON_SHIELD);
                    return;
                }
                else if (me->GetPower(POWER_ENERGY) < 1 && stage == 3)
                {
                    me->InterruptNonMeleeSpells(false);
                    stage = 0;
                    me->RemoveAurasDueToSpell(SPELL_ACTIVE);
                    DoCast(me, SPELL_INACTIVE);
                    if (Creature* omnotron = Unit::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                    {
                        omnotron->CastSpell(me, SPELL_RECHARGE_PURPLE, true);
                        omnotron->ClearUnitState(UNIT_STATE_CASTING);
                    }
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_POWER_GENERATOR:
                            if (Creature* target = GetFriendlyTarget())
                            {
                                //DoCast(target, SPELL_POWER_GENERATOR);
                                me->SummonCreature(NPC_POWER_GENERATOR, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 40000ms);
                                if (IsHeroic())
                                    if (Creature* pNefarius = me->FindNearestCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, 100.0f))
                                        if (pNefarius->IsAIEnabled)
                                            pNefarius->AI()->DoAction(ACTION_OVERCHARGE);
                            }
                            events.ScheduleEvent(EVENT_POWER_GENERATOR, urand(20000,30000));
                            break;
                        case EVENT_ARCANE_ANNIHILATOR:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_ARCANE_ANNIHILATOR);
                            events.ScheduleEvent(EVENT_ARCANE_ANNIHILATOR, 8000);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                Creature* GetFriendlyTarget()
                {
                    if (Creature* electron = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ELECTRON)))
                        if (!electron->HasAura(SPELL_INACTIVE_AURA))
                            return electron;
                    if (Creature* magmatron = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_MAGMATRON)))
                        if (!magmatron->HasAura(SPELL_INACTIVE_AURA))
                            return magmatron;
                    if (Creature* toxitron = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_TOXITRON)))
                        if (!toxitron->HasAura(SPELL_INACTIVE_AURA))
                            return toxitron;
                    return 0;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_arcanotronAI>(creature);
        }
};

class boss_electron : public CreatureScript
{
    public:
        boss_electron() : CreatureScript("boss_electron") { }

        struct boss_electronAI : public omnotron_golem_base
        {
            boss_electronAI(Creature* creature) : omnotron_golem_base(creature)
            {
                DoCast(me, SPELL_INACTIVE);
            }

            uint8 stage;

            void Reset() override
            {
                _Reset();
                
                stage = 0;

                me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 5);
                me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 5);

                instance->SetData(DATA_HEALTH_OMNOTRON_SHARED, me->GetMaxHealth());
                me->SetPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 100);
                me->SetHealth(me->GetMaxHealth());
                DoCast(me, SPELL_ZERO_POWER, true); 
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                DoCast(me, SPELL_INACTIVE);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                instance->SetBossState(DATA_OMNOTRON, FAIL);
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_ACTIVE:
                        stage = 1;
                        me->RemoveAurasDueToSpell(SPELL_RECHARGE_BLUE);
                        break;
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_LIGHTNING_CONDUCTOR, 10000);
                events.ScheduleEvent(EVENT_ELECTRICAL_DISCHARGE, 6000);
                instance->SetData(DATA_HEALTH_OMNOTRON_SHARED, me->GetMaxHealth());
            }
            
            void JustDied(Unit* killer) override
            {
                _JustDied();
                instance->SetData(DATA_HEALTH_OMNOTRON_SHARED, 0);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                me->SetLootRecipient(NULL);
                if (Creature* arcanotron = Unit::GetCreature(*me, instance->GetGuidData(DATA_ARCANOTRON)))
                {
                    killer->Kill(arcanotron);
                    arcanotron->SetLootRecipient(NULL);
                }
                if (Creature* magmatron = Unit::GetCreature(*me, instance->GetGuidData(DATA_MAGMATRON)))
                {
                    magmatron->LowerPlayerDamageReq(magmatron->GetMaxHealth());
                    killer->DealDamage(magmatron, magmatron->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false); // additional achievement controller
                    magmatron->SetLootRecipient(NULL);
                }
                if (Creature* toxitron = Unit::GetCreature(*me, instance->GetGuidData(DATA_TOXITRON)))
                {
                    toxitron->LowerPlayerDamageReq(toxitron->GetMaxHealth());
                    killer->DealDamage(toxitron, toxitron->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false); // achievement controller
                }
                if (Creature* omnotroncontroller = Unit::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                    omnotroncontroller->Kill(omnotroncontroller);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (instance->GetData(DATA_HEALTH_OMNOTRON_SHARED) != 0)
                    me->SetHealth(instance->GetData(DATA_HEALTH_OMNOTRON_SHARED));        

                if (stage == 0)
                    return;

                events.Update(diff);

                if (me->GetPower(POWER_ENERGY) <= 50 && stage == 1)
                {
                    stage = 2;
                    if (Creature* omnotroncontroller = Unit::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                        if (omnotroncontroller->IsAIEnabled)
                            omnotroncontroller->GetAI()->DoAction(ACTION_NEXT);
                    return;
                }
                else if (me->GetPower(POWER_ENERGY) <= 44 && stage == 2)
                {
                    stage = 3;
                    DoCast(me, SPELL_UNSTABLE_SHIELD);
                    if (Creature* omnotroncontroller = Unit::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                        if (omnotroncontroller->IsAIEnabled)
                            omnotroncontroller->AI()->Talk(SAY_ELECTRON_SHIELD);
                    return;
                }
                else if (me->GetPower(POWER_ENERGY) < 1 && stage == 3)
                {
                    me->InterruptNonMeleeSpells(false);
                    stage = 0;
                    me->RemoveAurasDueToSpell(SPELL_ACTIVE);
                    DoCast(me, SPELL_INACTIVE);
                    if (Creature* omnotron = Unit::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                    {
                        omnotron->CastSpell(me, SPELL_RECHARGE_BLUE, true);
                        omnotron->ClearUnitState(UNIT_STATE_CASTING);
                    }
                    return;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                    case EVENT_ELECTRICAL_DISCHARGE:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                            DoCast(target, SPELL_ELECTRICAL_DISCHARGE);
                        events.ScheduleEvent(EVENT_ELECTRICAL_DISCHARGE, 12000);
                        break;
                    case EVENT_LIGHTNING_CONDUCTOR:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                        {
                            DoCast(target, SPELL_LIGHTNING_CONDUCTOR);
                            if (IsHeroic())
                                if (Creature* pNefarius = me->FindNearestCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, 100.0f))
                                    if (pNefarius->IsAIEnabled)
                                        pNefarius->AI()->DoAction(ACTION_SHADOW_INFUSION);
                        }
                        events.ScheduleEvent(EVENT_LIGHTNING_CONDUCTOR, 27000);
                        break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_electronAI>(creature);
        }
};

class boss_magmatron : public CreatureScript
{
    public:
        boss_magmatron() : CreatureScript("boss_magmatron") { }

        struct boss_magmatronAI : public omnotron_golem_base
        {
            boss_magmatronAI(Creature* creature) : omnotron_golem_base(creature)
            {
                DoCast(me, SPELL_INACTIVE);
            }

            uint8 stage;

            void Reset() override
            {
                _Reset();

                stage = 0;

                me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 5);
                me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 5);

                instance->SetData(DATA_HEALTH_OMNOTRON_SHARED, me->GetMaxHealth());
                me->SetPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 100);
                me->SetHealth(me->GetMaxHealth());
                DoCast(me, SPELL_ZERO_POWER, true); 
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                DoCast(me, SPELL_INACTIVE);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                instance->SetBossState(DATA_OMNOTRON, FAIL);
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_ACTIVE:
                        stage = 1;
                        me->RemoveAurasDueToSpell(SPELL_RECHARGE_ORANGE);
                        break;
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_INSENERATION_SECURITY_MISSURE, 10000);
                //events.ScheduleEvent(EVENT_ACQUIRING_TARGET, 25000);
                instance->SetData(DATA_HEALTH_OMNOTRON_SHARED, me->GetMaxHealth());
            }
            
            void JustDied(Unit* killer) override
            {
                _JustDied();
                instance->SetData(DATA_HEALTH_OMNOTRON_SHARED, 0);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                me->SetLootRecipient(NULL);
                if (Creature* arcanotron = Unit::GetCreature(*me, instance->GetGuidData(DATA_ARCANOTRON)))
                {
                    killer->Kill(arcanotron);
                    arcanotron->SetLootRecipient(NULL);
                }
                if (Creature* electron = Unit::GetCreature(*me, instance->GetGuidData(DATA_ELECTRON)))
                {
                    killer->Kill(electron);
                    electron->SetLootRecipient(NULL);
                }
                if (Creature* toxitron = Unit::GetCreature(*me, instance->GetGuidData(DATA_TOXITRON)))
                {
                    toxitron->LowerPlayerDamageReq(toxitron->GetMaxHealth());
                    killer->DealDamage(toxitron, toxitron->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false); // achievement controller
                }
                if (Creature* omnotroncontroller = Unit::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                    omnotroncontroller->Kill(omnotroncontroller);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (instance->GetData(DATA_HEALTH_OMNOTRON_SHARED) != 0)
                    me->SetHealth(instance->GetData(DATA_HEALTH_OMNOTRON_SHARED));        

                if (stage == 0)
                    return;

                events.Update(diff);

                if (me->GetPower(POWER_ENERGY) <= 50 && stage == 1)
                {
                    stage = 2;
                    if (Creature* omnotroncontroller = Unit::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                        if (omnotroncontroller->IsAIEnabled)
                            omnotroncontroller->GetAI()->DoAction(ACTION_NEXT);
                    return;
                }
                else if (me->GetPower(POWER_ENERGY) <= 44 && stage == 2)
                {
                    stage = 3;
                    DoCast(me, SPELL_BARRIER);
                    if (Creature* omnotroncontroller = Unit::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                        if (omnotroncontroller->IsAIEnabled)
                            omnotroncontroller->AI()->Talk(SAY_MAGMATRON_SHIELD);
                    return;
                }
                else if (me->GetPower(POWER_ENERGY) < 1 && stage == 3)
                {
                    me->InterruptNonMeleeSpells(false);
                    stage = 0;
                    me->RemoveAurasDueToSpell(SPELL_ACTIVE);
                    DoCast(me, SPELL_INACTIVE);
                    if (Creature* omnotron = Unit::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                    {
                        omnotron->CastSpell(me, SPELL_RECHARGE_ORANGE, true);
                        omnotron->ClearUnitState(UNIT_STATE_CASTING);
                    }
                    return;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INSENERATION_SECURITY_MISSURE:
                            DoCast(me, SPELL_INSENERATION_SECURITY_MISSURE);
                            events.ScheduleEvent(EVENT_INSENERATION_SECURITY_MISSURE, 25000);
                            break;
                        case EVENT_ACQUIRING_TARGET:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                            {
                                me->SetReactState(REACT_PASSIVE);
                                me->AttackStop();
                                me->SetControlled(true, UNIT_STATE_STUNNED);
                                events.ScheduleEvent(EVENT_CONTINUE, 8000);
                                DoCast(target, SPELL_ACQUIRING_TARGET);
                                if (IsHeroic())
                                    if (Creature* pNefarius = me->FindNearestCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, 100.0f))
                                        if (pNefarius->IsAIEnabled)
                                            pNefarius->AI()->DoAction(ACTION_ENCASING_SHADOWS);
                                if (Creature* omnotroncontroller = Unit::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                                    if (omnotroncontroller->IsAIEnabled)
                                        omnotroncontroller->AI()->Talk(SAY_MAGMATRON_FLAMETHROWER);
                            }
                            events.ScheduleEvent(EVENT_ACQUIRING_TARGET, 40000);
                            break;
                        case EVENT_CONTINUE:
                            me->SetReactState(REACT_AGGRESSIVE);
                            AttackStart(me->GetVictim());
                            me->SetControlled(false, UNIT_STATE_STUNNED);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_magmatronAI>(creature);
        }
};

class boss_toxitron : public CreatureScript
{
    public:
        boss_toxitron() : CreatureScript("boss_toxitron") { }

        struct boss_toxitronAI : public omnotron_golem_base
        {
            boss_toxitronAI(Creature* creature) : omnotron_golem_base(creature)
            {
                DoCast(me, SPELL_INACTIVE);
            }

            uint8 stage;

            void Reset() override
            {
                _Reset();

                stage = 0;

                me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 5);
                me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 5);

                instance->SetData(DATA_HEALTH_OMNOTRON_SHARED, me->GetMaxHealth());
                me->SetPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 100);
                me->SetHealth(me->GetMaxHealth());
                DoCast(me, SPELL_ZERO_POWER, true); 
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                DoCast(me, SPELL_INACTIVE);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                instance->SetBossState(DATA_OMNOTRON, FAIL);
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_ACTIVE:
                        stage = 1;
                        me->RemoveAurasDueToSpell(SPELL_RECHARGE_GREEN);
                        break;
                }
            }

            void JustSummoned(Creature* summon) override
            {
                BossAI::JustSummoned(summon);
                switch (summon->GetEntry())
                {
                    case NPC_POISON_BOMB:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                        {
                            summon->AI()->AttackStart(target);
                            summon->AddThreat(target, 5000000.0f);
                        }
                    break;
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CHEMICAL_BOMB, 10000);
                events.ScheduleEvent(EVENT_POISON_PROTOCOL, 20000);
                instance->SetData(DATA_HEALTH_OMNOTRON_SHARED, me->GetMaxHealth());
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();
                instance->SetData(DATA_HEALTH_OMNOTRON_SHARED, 0);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                if (Creature* arcanotron = Unit::GetCreature(*me, instance->GetGuidData(DATA_ARCANOTRON)))
                {
                    killer->Kill(arcanotron);
                    arcanotron->SetLootRecipient(NULL);
                }
                if (Creature* electron = Unit::GetCreature(*me, instance->GetGuidData(DATA_ELECTRON)))
                {
                    killer->Kill(electron);
                    electron->SetLootRecipient(NULL);
                }
                if (Creature* magmatron = Unit::GetCreature(*me, instance->GetGuidData(DATA_MAGMATRON)))
                {
                    magmatron->LowerPlayerDamageReq(magmatron->GetMaxHealth());
                    killer->DealDamage(magmatron, magmatron->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false); // additional achievement controller
                    magmatron->SetLootRecipient(NULL);
                }
                if (Creature* omnotroncontroller = Unit::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                    omnotroncontroller->Kill(omnotroncontroller);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (instance->GetData(DATA_HEALTH_OMNOTRON_SHARED) != 0)
                    me->SetHealth(instance->GetData(DATA_HEALTH_OMNOTRON_SHARED));        

                if (stage == 0)
                    return;

                events.Update(diff);

                if (me->GetPower(POWER_ENERGY) <= 50 && stage == 1)
                {
                    stage = 2;
                    if (Creature* omnotroncontroller = Unit::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                        if (omnotroncontroller->IsAIEnabled)
                            omnotroncontroller->GetAI()->DoAction(ACTION_NEXT);
                    return;
                }
                else if (me->GetPower(POWER_ENERGY) <= 44 && stage == 2)
                {
                    stage = 3;
                    DoCast(me, SPELL_POISON_SOAKED_SHELL);
                    if (Creature* omnotroncontroller = Unit::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                        if (omnotroncontroller->IsAIEnabled)
                            omnotroncontroller->AI()->Talk(SAY_TOXITRON_SHIELD);
                    return;
                }
                else if (me->GetPower(POWER_ENERGY) < 1 && stage == 3)
                {
                    me->InterruptNonMeleeSpells(false);
                    stage = 0;
                    me->RemoveAurasDueToSpell(SPELL_ACTIVE);
                    DoCast(me, SPELL_INACTIVE);
                    if (Creature* omnotron = Unit::GetCreature(*me, instance->GetGuidData(DATA_OMNOTRON)))
                    {
                        omnotron->CastSpell(me, SPELL_RECHARGE_GREEN, true);
                        omnotron->ClearUnitState(UNIT_STATE_CASTING);
                    }
                    return;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHEMICAL_BOMB:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                            {
                                DoCast(target, SPELL_CHEMICAL_BOMB);
                                if (IsHeroic())
                                    if (Creature* pNefarius = me->FindNearestCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, 100.0f))
                                        if (pNefarius->IsAIEnabled)
                                            pNefarius->GetAI()->DoAction(ACTION_GRIP_OF_DEATH);
                            }
                            events.ScheduleEvent(EVENT_CHEMICAL_BOMB, 30000);
                            break;
                        case EVENT_POISON_PROTOCOL:
                            DoCast(me, SPELL_POISON_PROTOCOL);
                            events.ScheduleEvent(EVENT_POISON_PROTOCOL, 60000);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_toxitronAI>(creature);
        }
};

class npc_poison_bomb : public CreatureScript
{
    public:
        npc_poison_bomb() : CreatureScript("npc_poison_bomb") { }

        struct npc_poison_bombAI : public ScriptedAI
        {
            npc_poison_bombAI(Creature* creature) : ScriptedAI(creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);    
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR, true);
                me->SetSpeed(MOVE_WALK, 0.6f);
                me->SetSpeed(MOVE_RUN, 0.6f);
            }

            bool bFixate;
            uint32 uiMoveTimer;

            void Reset() override
            {
                bFixate = false;
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (!UpdateVictim())
                    return;

                if (!bFixate)
                {
                    bFixate = true;
                    DoCast(me->GetVictim(), SPELL_FIXATE);
                    me->ClearUnitState(UNIT_STATE_CASTING);
                }

                if (Unit* target = me->GetVictim())
                {
                    if (me->GetDistance(target) <= 3.0f)
                    {
                        DoCast(me, SPELL_POISON_BOMB);
                        me->SummonCreature(NPC_POISON_PUDDLE, me->GetPositionX(),me->GetPositionY(), me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 20000ms);
                        me->Kill(me);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_poison_bombAI>(creature);
        }
};

class npc_poison_puddle : public CreatureScript
{
    public:
        npc_poison_puddle() : CreatureScript("npc_poison_puddle") { }

        struct npc_poison_puddleAI : public ScriptedAI
        {
            npc_poison_puddleAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            void Reset() override
            {
                DoCast(me, SPELL_POISON_PUDDLE);
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (instance->GetBossState(DATA_OMNOTRON) != IN_PROGRESS)
                    me->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_poison_puddleAI>(creature);
        }
};

class npc_power_generator : public CreatureScript
{
    public:
        npc_power_generator() : CreatureScript("npc_power_generator") { }

        struct power_generatorAI : public ScriptedAI
        {
            power_generatorAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            uint8 radius;

            void Reset() override
            {
                radius = 0;
                DoCast(me, SPELL_POWER_GENERATOR_10);
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_OVERCHARGE)
                    events.ScheduleEvent(EVENT_OVERCHARGED, 2000);                
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_OVERCHARGED:
                            if (radius < 8)
                            {
                                radius++;
                                me->ApplyPercentModFloatValue(OBJECT_FIELD_SCALE, (float)(radius*2.5), true);
                                events.ScheduleEvent(EVENT_OVERCHARGED, 1000);
                            }
                            else
                            {
                                DoCast(me, SPELL_ARCANE_BLOWBACK, true);
                                me->DespawnOrUnsummon(500);
                            }
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<power_generatorAI>(creature);
        }
};

class npc_chemical_cloud : public CreatureScript
{
    public:
        npc_chemical_cloud() : CreatureScript("npc_chemical_cloud") { }

        struct npc_chemical_cloudAI : public ScriptedAI
        {
            npc_chemical_cloudAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                DoCast(me, SPELL_CHEMICAL_CLOUD_AOE_A, true);
                DoCast(me, SPELL_CHEMICAL_CLOUD_AOE_B, true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_chemical_cloudAI>(creature);
        }
};

class spell_omnotron_active_trigger : public SpellScriptLoader
{
    public:
        spell_omnotron_active_trigger() : SpellScriptLoader("spell_omnotron_active_trigger") { }

        class spell_omnotron_active_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_omnotron_active_trigger_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!(GetHitUnit() && GetHitUnit()->IsAlive()))
                    return;

                if (Unit* caster = GetCaster())
                {
                    if (caster->GetPower(POWER_ENERGY) >= 1)
                        caster->SetPower(POWER_ENERGY, caster->GetPower(POWER_ENERGY) - 1);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_omnotron_active_trigger_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_omnotron_active_trigger_SpellScript();
        }
};

class spell_magmatron_flamethrower : public SpellScriptLoader
{
    public:
        spell_magmatron_flamethrower() : SpellScriptLoader("spell_magmatron_flamethrower") { }

        class spell_magmatron_flamethrower_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_magmatron_flamethrower_AuraScript);

            void OnPereodic(AuraEffect const* /*aurEff*/)
            {
                //PreventDefaultAction();

                if (Unit* caster = GetCaster())
                    caster->CastSpell(GetCaster(), SPELL_FLAMETHROWER, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_magmatron_flamethrower_AuraScript::OnPereodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL); 
            } 
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_magmatron_flamethrower_AuraScript();
        }
};

class spell_magmatron_barrier : public SpellScriptLoader
{
    public:
        spell_magmatron_barrier() : SpellScriptLoader("spell_magmatron_barrier") { }

        class spell_magmatron_barrier_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_magmatron_barrier_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetTarget())
                    return;

                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_ENEMY_SPELL)
                    GetTarget()->CastSpell(GetTarget(), SPELL_BACKDRAFT, true);
            }
            
            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_magmatron_barrier_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_magmatron_barrier_AuraScript();
        }
};

// Static Shock 79912
class spell_omnotron_static_shock : public SpellScript
{
    PrepareSpellScript(spell_omnotron_static_shock);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Player* target = GetHitPlayer())
            target->GetMap()->SetWorldState(WORLDSTATE_ACHIEVE_A_TRON_SHOCK, 0);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_omnotron_static_shock::HandleScript, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Arcane Annihilator 79710
class spell_omnotron_arcane_annihilator : public SpellScript
{
    PrepareSpellScript(spell_omnotron_arcane_annihilator);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Player* target = GetHitPlayer())
            target->GetMap()->SetWorldState(WORLDSTATE_ACHIEVE_A_TRON_ANNIHILATOR, 0);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_omnotron_arcane_annihilator::HandleScript, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Poison Bomb 80092
class spell_omnotron_poison_bomb : public SpellScript
{
    PrepareSpellScript(spell_omnotron_poison_bomb);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Player* target = GetHitPlayer())
            target->GetMap()->SetWorldState(WORLDSTATE_ACHIEVE_A_TRON_BOMB, 0);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_omnotron_poison_bomb::HandleScript, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Flamethower 79504
class spell_omnotron_flamethower : public SpellScript
{
    PrepareSpellScript(spell_omnotron_flamethower);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (targets.size() > 1)
            GetCaster()->GetMap()->SetWorldState(WORLDSTATE_ACHIEVE_A_TRON_FLAMETHOWER, 0);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_omnotron_flamethower::SelectTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
    }
};

void AddSC_boss_omnotron_defence_system()
{
    new boss_omnotron();
    new boss_arcanotron();
    new boss_electron();
    new boss_magmatron();
    new boss_toxitron();
    new npc_poison_bomb();
    new npc_poison_puddle();
    new npc_power_generator();
    new npc_chemical_cloud();
    new spell_omnotron_active_trigger();
    new spell_magmatron_flamethrower();
    new spell_magmatron_barrier();
    new spell_script<spell_omnotron_static_shock>("spell_omnotron_static_shock");
    new spell_script<spell_omnotron_arcane_annihilator>("spell_omnotron_arcane_annihilator");
    new spell_script<spell_omnotron_poison_bomb>("spell_omnotron_poison_bomb");
    new spell_script<spell_omnotron_flamethower>("spell_omnotron_flamethower");
}