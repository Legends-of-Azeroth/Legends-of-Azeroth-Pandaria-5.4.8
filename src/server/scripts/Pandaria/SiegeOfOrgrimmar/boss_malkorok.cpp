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
#include "siege_of_orgrimmar.h"

enum Spells
{
    // phase 1
    SPELL_ANCIENT_MIASMA                      = 142861,
    SPELL_ANCIENT_MIASMA_EFF                  = 142906,
    SPELL_ANCIENT_MIASMA_DUMMY                = 143018,
    SPELL_ANCIENT_MIASMA_LINK                 = 143042,
    SPELL_ANCIENT_MIASMA_LINK_2               = 143047,
    SPELL_ANCIENT_MIASMA_LEAVE_BODY           = 149443,
    SPELL_ANCIENT_BARRIER                     = 142862,
    SPELL_WEAK_ANCIENT_BARRIER                = 142863,
    SPELL_MIDDLE_ANCIENT_BARRIER              = 142864,
    SPELL_STRONG_ANCIENT_BARRIER              = 142865,
    SPELL_SIESMIC_SLAM                        = 142849, // cast on random player, will knockback in air
    SPELL_SIESMIC_SLAM_SELECTOR               = 142851,
    SPELL_ARCING_SMASH                        = 142815,
    SPELL_ARCING_SMASH_PREPARE                = 142898,
    SPELL_ARCING_SMASH_VISUAL_CAST            = 143805,
    SPELL_ARCING_SMASH_CONTROLL               = 142826,
    SPELL_IMPLODING_ENERGY_COSMETIC_VOID_ZONE = 144069,
    SPELL_IMPLOSION_PLAYER                    = 142986, // cast aoe in small area if player was at void zone
    SPELL_IMPLOSION_RAID                      = 142987, // cast aoe on raid if players didn`t were at void zone
    SPELL_IMPLOSION_COSMETIC                  = 142980, // waves on npc_implosion
    SPELL_IMPLODING_ENERGY_SELECTOR           = 142988,
    SPELL_BREATH_OF_YSHAARJ                   = 142816, // will cast from npc after 3 arcing smash
    SPELL_BREATH_OF_YSHAARJ_COSMETIC          = 142842,
    SPELL_FATAL_STRIKE                        = 142990, // debuff on tanks
    SPELL_FATAL_STRIKES                       = 146254,

    // phase 2
    SPELL_BLOOD_RAGE                          = 142879, // aoe melee attacks
    SPELL_BLOOD_RAGE_TRIGGERED_AOE            = 142890, // on each attack
    SPELL_DISPLACED_ENERGY                    = 142913, // debuff on player
    SPELL_DISPLACED_ENERGY_EFF                = 142928,
    SPELL_RELENTLESS_ASSAULT                  = 143261, // will stuck every time after 2th phase
    SPELL_EXPEL_MIASMA                        = 143199, // cast at begun 2th phase

    // Misc
    SPELL_ERADICATE                           = 143916,
    SPELL_ZERO_POWER                          = 72242,
    NPC_ANCIENT_MIASMA_VISUAL                 = 143018,
    SPELL_ZEN                                 = 131221, // wrong id, no data in sniffs about prevent falling damage
    SPELL_YSHAARJ_CORRUPTION                  = 142848, // achiev

    // Heroic
    SPELL_ESSENCE_OF_YSHAARJ                  = 143846,
    SPELL_ESSENCE_OF_YSHAARJ_TRIGGER_EFF      = 143848,
    SPELL_ESSENCE_OF_YSHAARJ_AT               = 143850,
    SPELL_ESSENCE_OF_YSHAARJ_EFF              = 143857,
    SPELL_LANGUISH                            = 143919,
    SPELL_LANGUISH_AT                         = 142989,
};

enum Events
{
    EVENT_SIESMIC_SLAM = 1,
    EVENT_ARCING_SMASH,
    EVENT_IMPLOSION,
    EVENT_BREATH_OF_YSSHAARJ,
    EVENT_FATAL_STRIKE,
    EVENT_EXPEL_MIASMA,
    EVENT_BLOOD_RAGE,
    EVENT_RELENTLESS_ASSAULT,
    EVENT_DISPLACED_ENERGY,
    EVENT_ARCING_SMASH_PREPARE,
    EVENT_DETONATE,
    EVENT_END_OF_ARCING_SMASH,
    EVENT_CLEAVE,
    EVENT_ERADICATE,
    EVENT_MODIFY_POWER,
    EVENT_SECOND_PHASE,
    EVENT_MIASMA_EFF,
};

enum Actions
{
    ACTION_DETONATE,
    ACTION_ACTIVATE_MIASMA,
    ACTION_EXPEL_MIASMA,
    ACTION_ARCING_SMASH,
    ACTION_INIT_ARCING_SEQ,
    ACTION_ARCING_SMASH_CANCEL,
    ACTION_YSHAARJ_CLEAR,
};

enum Yells
{
    TALK_INTRO,
    TALK_AGGRO,
    TALK_ARCING_SMASH,
    TALK_BREATH_OF_YSHAARJ,
    TALK_BLOOD_RAGE,
    TALK_BERSERK,
    TALK_WIPE,
    TALK_DEATH,
    TALK_YSHAARJ_BREATH_ANN,
    TALK_BLOOD_RAGE_ANN,
    TALK_BLOOD_RAGE_FADED_ANN,
    EMOTE_MALOKROK_ENRAGED,
};

class boss_malkorok : public CreatureScript
{
    public:
        boss_malkorok() : CreatureScript("boss_malkorok") { }

        struct boss_malkorokAI : public BossAI
        {
            boss_malkorokAI(Creature* creature) : BossAI(creature, DATA_MALKOROK) 
            {
                me->setActive(true);
                hasIntroDone = false;
            }

            EventMap berserkerEvents;
            uint32 ArcingSmashCount;
            uint32 phase;
            ObjectGuid targetGUID;
            uint32 energyGainTick;
            uint32 breathCount;
            bool hasIntroDone;
            float x, y;

            void Reset() override
            {
                _Reset();
                events.Reset();
                berserkerEvents.Reset();
                me->SetReactState(REACT_DEFENSIVE);
                ArcingSmashCount = 0;
                x = 0.0f; y = 0.0f;
                me->SetPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 0);
                DoCast(me, SPELL_ZERO_POWER);
                phase = 1;
                targetGUID = ObjectGuid::Empty;
                energyGainTick = 0;
                breathCount = 0;
                me->SetAutoattackOverrideSpell(0, 0);

                if (Creature* AncientMiasma = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ANCIENT_MIASMA) : ObjectGuid::Empty))
                    AncientMiasma->AI()->DoAction(ACTION_ACTIVATE_MIASMA);

                summons.DespawnAll();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                scheduler
                    .Schedule(Seconds(1), [this](TaskContext /*context*/)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                if (instance)
                    instance->SetBossState(DATA_MALKOROK, FAIL);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                me->SetReactState(REACT_AGGRESSIVE);
                Talk(TALK_AGGRO);

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                    instance->SetBossState(DATA_MALKOROK, IN_PROGRESS);
                }

                Miasma(false);
                FirstPhaseEvents();

                berserkerEvents.ScheduleEvent(EVENT_ERADICATE, 6 * MINUTE * IN_MILLISECONDS);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == EVENT_JUMP)
                    DoCast(me, SPELL_ARCING_SMASH_CONTROLL, true);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_INTRO:
                        if (hasIntroDone || !me->IsAlive())
                            break;

                        hasIntroDone = true;

                        Talk(TALK_INTRO);

                        // Put our army to form groups
                        if (instance)
                            instance->SetData(DATA_KORKRON_BARRACKS_EVENT, DONE);
                        break;
                    case ACTION_ARCING_SMASH:
                        if (Unit* target = me->GetVictim())
                            targetGUID = target->GetGUID();

                        me->PrepareChanneledCast(me->GetOrientation());
                        me->GetMotionMaster()->MoveJump(me->GetHomePosition(), 20.0f, 20.0f, EVENT_JUMP);
                        break;
                    case ACTION_ARCING_SMASH_CANCEL:
                        scheduler
                            .Schedule(Milliseconds(2000), [this](TaskContext /*context*/)
                        {
                            // Imploding Energy
                            HandleInitializeImplodingEnergy();
                            me->RemoveChanneledCast(targetGUID);
                        });
                        break;
                    case ACTION_YSHAARJ_CLEAR:
                        scheduler
                            .Schedule(Milliseconds(500), [this](TaskContext /*context*/)
                        {
                            HandleClearArcingTriggers();
                        });
                        break;
                    case ACTION_EXPEL_MIASMA:
                        me->SetAutoattackOverrideSpell(0, 0);
                        DoCast(me, SPELL_RELENTLESS_ASSAULT, true);
                        events.Reset();
                        
                        DoCast(me, SPELL_FATAL_STRIKES);
                        events.ScheduleEvent(EVENT_SIESMIC_SLAM, 3.5 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_ARCING_SMASH_PREPARE, 10 * IN_MILLISECONDS);
                        berserkerEvents.ScheduleEvent(EVENT_MODIFY_POWER, 1000);

                        Miasma(false);

                        if (Creature* AncientMiasma = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ANCIENT_MIASMA) : ObjectGuid::Empty))
                            AncientMiasma->AI()->DoAction(ACTION_ACTIVATE_MIASMA);
                        break;
                }
            }

            void KilledUnit(Unit* /*victim*/) override { }

            void EnterEvadeMode() override
            {
                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_MALKOROK, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FATAL_STRIKE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANCIENT_MIASMA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANCIENT_MIASMA_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DISPLACED_ENERGY);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANCIENT_BARRIER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WEAK_ANCIENT_BARRIER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MIDDLE_ANCIENT_BARRIER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STRONG_ANCIENT_BARRIER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ZEN);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    if (Creature* AncientMiasma = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ANCIENT_MIASMA)))
                    {
                        AncientMiasma->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_YSHAARJ);
                        AncientMiasma->RemoveAllAreasTrigger();
                    }
                }

                HandleClearArcingTriggers();
                summons.DespawnAll();
                berserkerEvents.Reset();

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                _DespawnAtEvade();
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(TALK_DEATH);

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FATAL_STRIKE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANCIENT_MIASMA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANCIENT_MIASMA_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DISPLACED_ENERGY);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANCIENT_BARRIER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WEAK_ANCIENT_BARRIER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MIDDLE_ANCIENT_BARRIER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STRONG_ANCIENT_BARRIER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ZEN);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    if (Creature* AncientMiasma = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ANCIENT_MIASMA)))
                    {
                        AncientMiasma->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_YSHAARJ);
                        AncientMiasma->RemoveAllAreasTrigger();
                    }
                }

                berserkerEvents.Reset();
                HandleClearArcingTriggers();
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);
                berserkerEvents.Update(diff);

                while (uint32 eventId = berserkerEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MIASMA_EFF:
                            // Should apply upon exp 5 sec of pull for not break invisibility or smth else
                            DoCast(me, SPELL_ANCIENT_MIASMA_EFF, true);

                            for (auto&& itr : instance->instance->GetPlayers())
                            {
                                if (Player* target = itr.GetSource())
                                {
                                    if (target->HasAura(SPELL_ZEN))
                                        continue;

                                    target->CastSpell(target, SPELL_ZEN, true);
                                }
                            }
                            break;
                        case EVENT_ERADICATE:
                            me->InterruptNonMeleeSpells(false);
                            DoCast(me, SPELL_ERADICATE);
                            break;
                        case EVENT_MODIFY_POWER:
                        {
                            if (energyGainTick > 2)
                                energyGainTick = 0;

                            uint32 delay = ++energyGainTick > 2 ? 2 * IN_MILLISECONDS : 1 * IN_MILLISECONDS; // each third tick it has delay 2s instead 1

                            // Approximate to sequence
                            if (me->GetPower(POWER_ENERGY) > 92)
                                delay = 2 * IN_MILLISECONDS;

                            if (me->GetPower(POWER_ENERGY) + 1 > 99)
                            {
                                me->SetPower(POWER_ENERGY, 100);
                                events.ScheduleEvent(EVENT_SECOND_PHASE, 0.5 * IN_MILLISECONDS);
                                break;
                            }
                            else
                                me->SetPower(POWER_ENERGY, me->GetPower(POWER_ENERGY) + 1);

                            berserkerEvents.ScheduleEvent(EVENT_MODIFY_POWER, delay);
                            break;
                        }
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SIESMIC_SLAM:
                            // Exclude 4td cast
                            if (ArcingSmashCount > 2)
                                break;

                            DoCast(me, SPELL_SIESMIC_SLAM_SELECTOR);           
                            events.ScheduleEvent(EVENT_SIESMIC_SLAM, 20000);
                            break;
                        case EVENT_BREATH_OF_YSSHAARJ:
                            Talk(TALK_BREATH_OF_YSHAARJ);
                            Talk(TALK_YSHAARJ_BREATH_ANN);
                            DoCast(me, SPELL_BREATH_OF_YSHAARJ_COSMETIC);
                            breathCount++;
                            events.ScheduleEvent(EVENT_ARCING_SMASH_PREPARE, 14 * IN_MILLISECONDS);
                            events.RescheduleEvent(EVENT_SIESMIC_SLAM, 7.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_DISPLACED_ENERGY:
                            DoCast(me, SPELL_DISPLACED_ENERGY);
                            events.ScheduleEvent(EVENT_DISPLACED_ENERGY, 8.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_ARCING_SMASH_PREPARE:
                            if (++ArcingSmashCount > 3)
                            {
                                ArcingSmashCount = 0;
                                events.ScheduleEvent(EVENT_BREATH_OF_YSSHAARJ, 0.5 * IN_MILLISECONDS);
                                break;
                            }
                        
                            DoCast(me, SPELL_ARCING_SMASH_PREPARE);
                            events.ScheduleEvent(EVENT_ARCING_SMASH_PREPARE, 19 * IN_MILLISECONDS);
                            break;
                        case EVENT_SECOND_PHASE:
                            if (breathCount < 2) // in case (happened 1 time)
                            {
                                events.RescheduleEvent(EVENT_SECOND_PHASE, 1 * IN_MILLISECONDS);
                                break;
                            }
                            SecondPhaseEvents();
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 43.5f);
            }

            private:
                void HandleClearArcingTriggers()
                {
                    std::list<Creature*> arcingTriggersList;
                    GetCreatureListWithEntryInGrid(arcingTriggersList, me, NPC_ARCING_SMASH, 100.0f);

                    for (auto&& itr : arcingTriggersList)
                        itr->AI()->SetData(TYPE_ARCING_SMASH, 0);
                }

                void FirstPhaseEvents()
                {
                    DoCast(me, SPELL_FATAL_STRIKES);
                    events.ScheduleEvent(EVENT_SIESMIC_SLAM, 5 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_ARCING_SMASH_PREPARE, 10.7 * IN_MILLISECONDS);
                    berserkerEvents.ScheduleEvent(EVENT_MODIFY_POWER, 1000);
                }

                void SecondPhaseEvents()
                {
                    events.Reset();
                    me->RemoveAurasDueToSpell(SPELL_FATAL_STRIKES);

                    Miasma(true);

                    if (Creature* AncientMiasma = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ANCIENT_MIASMA) : ObjectGuid::Empty))
                        AncientMiasma->AI()->DoAction(ACTION_EXPEL_MIASMA);

                    Talk(TALK_BLOOD_RAGE_ANN);
                    DoCast(me, SPELL_ANCIENT_MIASMA_LEAVE_BODY, true);
                    Talk(TALK_BLOOD_RAGE);
                    DoCast(me, SPELL_BLOOD_RAGE);
                    breathCount = 0;
                    me->SetAutoattackOverrideSpell(SPELL_BLOOD_RAGE_TRIGGERED_AOE, 0);

                    events.ScheduleEvent(EVENT_DISPLACED_ENERGY, 8000);
                }

                void Miasma(bool remove)
                {
                    if (remove)
                    {
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANCIENT_MIASMA);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANCIENT_MIASMA_EFF);

                        if (Creature* AncientMiasma = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ANCIENT_MIASMA) : ObjectGuid::Empty))
                            AncientMiasma->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_YSHAARJ);
                    }
                    else
                    {
                        DoCast(me, SPELL_ANCIENT_MIASMA, true);
                        berserkerEvents.ScheduleEvent(EVENT_MIASMA_EFF, 5 * IN_MILLISECONDS);

                        if (IsHeroic())
                        {
                            if (Creature* AncientMiasma = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ANCIENT_MIASMA) : ObjectGuid::Empty))
                                AncientMiasma->CastSpell(AncientMiasma, SPELL_ESSENCE_OF_YSHAARJ, true);
                        }
                    }
                }

                void HandleInitializeImplodingEnergy()
                {
                    float oriStep = Is25ManRaid() ? frand(M_PI / 6, (3 * M_PI) / 8) : frand(M_PI / 8, (5 * M_PI) / 6);
                    float defaultOri = frand(0.0f, 2 * M_PI);

                    uint8 count = Is25ManRaid() ? 7 : 3;

                    if (instance && instance->GetData(DATA_FLEX))
                    {
                        // Calculate Energy Count
                        count = 3; // min 3, max 7
                        uint32 totalPlayersCount = instance->instance->GetPlayersCountExceptGMs();

                        if (totalPlayersCount > 13)
                            count++;

                        if (totalPlayersCount > 16)
                            count++;

                        if (totalPlayersCount > 19)
                            count++;

                        if (totalPlayersCount > 22)
                            count++;

                        if (totalPlayersCount > 4)
                            oriStep = frand(M_PI / 6, (3 * M_PI) / 8);
                    }

                    for (uint8 i = 0; i < count; i++)
                    {
                        GetPositionWithDistInOrientation(me, frand(10.0f, 40.0f), defaultOri + i * oriStep, x, y);
                        me->SummonCreature(NPC_IMPLOSION, x, y, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
                    }

                    DoCast(me, SPELL_IMPLOSION_COSMETIC);
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_malkorokAI(creature);
        }
};

// Implosion 71470 
struct npc_malkorok_implosion : public ScriptedAI
{
    npc_malkorok_implosion(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->SetReactState(REACT_PASSIVE);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        damage = 0;
    }

    void UpdateAI(uint32 diff) override { }
};

// Arcing Smash 71455
struct npc_malkorok_arcing_smash : public ScriptedAI
{
    npc_malkorok_arcing_smash(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    uint32 timer;
    uint32 affectData;

    void Reset() override
    {
        affectData = 0;
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_ARCING_SMASH)
            affectData = data;
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_ARCING_SMASH)
            return affectData;

        return 0;
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        damage = 0;
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Ancient Miasma 71513
struct npc_malkorok_ancient_miasma : public ScriptedAI
{
    npc_malkorok_ancient_miasma(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

    EventMap events;
    uint32 timer;

    void Reset() override
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_ACTIVATE_MIASMA:
                DoCast(me, NPC_ANCIENT_MIASMA_VISUAL, true);
                break;
            case ACTION_EXPEL_MIASMA:
                me->RemoveAurasDueToSpell(NPC_ANCIENT_MIASMA_VISUAL);
                break;
        }
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        damage = 0;
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);
    }

    private:
        InstanceScript* _instance;
};

// Malkorok Controller 71459
struct npc_malkorok_controller : public ScriptedAI
{
    npc_malkorok_controller(Creature* creature) : ScriptedAI(creature) { }

    InstanceScript* instance;
    std::list<Player*> arcPlayers;

    void Reset() override
    {
        instance = me->GetInstanceScript();
        arcPlayers.clear();
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_INIT_ARCING_SEQ)
        {
            // Select any not affected trigger
            std::list<Creature*> arcingSmashTriggersList, tempArcingSmashTriggersList;
            GetCreatureListWithEntryInGrid(arcingSmashTriggersList, me, NPC_ARCING_SMASH, 50.0f);

            arcPlayers.clear();
            GetPlayerListInGrid(arcPlayers, me, 150.0f);

            arcingSmashTriggersList.remove_if([=](Creature* target) { return target && target->AI()->GetData(TYPE_ARCING_SMASH); });

            if (arcingSmashTriggersList.empty())
                return;

            tempArcingSmashTriggersList = arcingSmashTriggersList;

            // we should select cone with any players in arc. in another case - random
            arcingSmashTriggersList.remove_if([=](Creature* target) { return target && !hasAnyInArc(target); });

            if (arcingSmashTriggersList.empty())
            {
                arcingSmashTriggersList.clear();

                for (auto&& itr : tempArcingSmashTriggersList)
                    arcingSmashTriggersList.push_back(itr);
            }

            if (Creature* smashTrigger = Trinity::Containers::SelectRandomContainerElement(arcingSmashTriggersList))
            {
                smashTrigger->CastSpell(smashTrigger, SPELL_ARCING_SMASH, false);
                
                if (Creature* malkorok = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_MALKOROK) : ObjectGuid::Empty))
                {
                    malkorok->PlayOneShotAnimKitId(4308);
                    malkorok->PrepareChanneledCast(smashTrigger->GetOrientation(), SPELL_ARCING_SMASH_VISUAL_CAST);
                }

                smashTrigger->AI()->SetData(TYPE_ARCING_SMASH, 1);
            }
        }
    }

    private:
        bool hasAnyInArc(Creature* target)
        {
            std::list<Player*> tempArcPlayers = arcPlayers;
            tempArcPlayers.remove_if([=](Player* pTarget) { return pTarget && !target->HasInArc(M_PI / 3, pTarget); });

            return !tempArcPlayers.empty();
        }
};

// Living Corruption 71644
struct npc_malkorok_living_corruption : public customCreatureAI
{
    npc_malkorok_living_corruption(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->SetInCombatWithZone();
        DoCast(me, SPELL_LANGUISH_AT, true);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// implosion cosmetic 142980
class spell_malkorok_implosion_cosmetic : public SpellScript
{
    PrepareSpellScript(spell_malkorok_implosion_cosmetic);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_IMPLOSION; });
    }

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* target = GetHitUnit())
        {
            target->CastSpell(target, SPELL_IMPLODING_ENERGY_SELECTOR, false);
            target->CastSpell(target, SPELL_IMPLODING_ENERGY_COSMETIC_VOID_ZONE, true);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_malkorok_implosion_cosmetic::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_malkorok_implosion_cosmetic::HandleHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Siesmic Slam Selector 142851
class spell_malkorok_siesmic_slam_selector : public SpellScript
{
    PrepareSpellScript(spell_malkorok_siesmic_slam_selector);

    std::list<WorldObject*> m_targets;

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->CastSpell(target, SPELL_SIESMIC_SLAM, true);

                if (caster->GetMap()->IsHeroic())
                    caster->SummonCreature(NPC_LIVING_CORRUPTION, *target, TEMPSUMMON_MANUAL_DESPAWN);
            }
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // Try not select targets with tank/melee specs
        targets.remove_if(MeeleSpecTargetSelector());

        uint8 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 3 : 1;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 1; // min 1, max 3
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 15)
                targetsCount++;

            if (totalPlayersCount > 20)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() >= targetsCount) // doesn`t matter count - MaxAffect is 1(3)
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 1; // min 1, max 3
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 15)
                targetsCount++;

            if (totalPlayersCount > 20)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_malkorok_siesmic_slam_selector::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_malkorok_siesmic_slam_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Arcing Smash Preparation 142898
class spell_malkorok_arcing_smash_preparation : public SpellScript
{
    PrepareSpellScript(spell_malkorok_arcing_smash_preparation);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->DoAction(ACTION_ARCING_SMASH);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_malkorok_arcing_smash_preparation::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Arcing Smash Controll 142826
class spell_malkorok_arcing_smash_controll : public SpellScript
{
    PrepareSpellScript(spell_malkorok_arcing_smash_controll);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (Creature* controller = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(NPC_MALKOROK_CONTROLLER) : ObjectGuid::Empty))
                controller->AI()->DoAction(ACTION_INIT_ARCING_SEQ);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_malkorok_arcing_smash_controll::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Arcing Smash Visual Cast 143805
class spell_malkorok_arcing_smash_visual_cast : public AuraScript
{
    PrepareAuraScript(spell_malkorok_arcing_smash_visual_cast);

    void HandleOnRemove(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_ARCING_SMASH_CANCEL);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_malkorok_arcing_smash_visual_cast::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Breath of Yshaarj 142842
class spell_malkorok_breath_of_yshaarj : public SpellScript
{
    PrepareSpellScript(spell_malkorok_breath_of_yshaarj);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* target = GetHitCreature())
        {
            target->CastSpell(target, SPELL_BREATH_OF_YSHAARJ, false);
            target->CastSpell(target, SPELL_YSHAARJ_CORRUPTION, true); // required for achiev
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_ARCING_SMASH; });
        targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && !target->ToCreature()->AI()->GetData(TYPE_ARCING_SMASH); });
    }

    void HandleAfterCast()
    {
        // Set Clear our Triggers
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->DoAction(ACTION_YSHAARJ_CLEAR);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_malkorok_breath_of_yshaarj::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_malkorok_breath_of_yshaarj::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        AfterCast += SpellCastFn(spell_malkorok_breath_of_yshaarj::HandleAfterCast);
    }
};

// Eradicate 143916
class spell_malkorok_eradicate : public SpellScript
{
    PrepareSpellScript(spell_malkorok_eradicate);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->GetTypeId() != TYPEID_PLAYER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_malkorok_eradicate::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Blood Rage 142890
class spell_malkorok_blood_rage : public SpellScript
{
    PrepareSpellScript(spell_malkorok_blood_rage);

    uint8 targetsCount;

    bool Load() override
    {
        targetsCount = 1;
        return true;
    }

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        targetsCount = targets.size();
    }

    void RecalculateDamage(SpellEffIndex /*effIndex*/)
    {
        if (targetsCount > 0)
            SetHitDamage(GetHitDamage() / targetsCount);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_malkorok_blood_rage::CheckTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_WITH_OFFSET);
        OnEffectHitTarget += SpellEffectFn(spell_malkorok_blood_rage::RecalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Ancient Miasma 142861
class spell_malkorok_ancient_miasma_heal_absorb : public AuraScript
{
    PrepareAuraScript(spell_malkorok_ancient_miasma_heal_absorb);

    void OnAbsorb(AuraEffect* aurEff, uint32& heal, uint32& absorb)
    {
        if (Unit* owner = GetUnitOwner())
        {
            // Prevent if encounter not in progress
            if (owner->GetInstanceScript() && owner->GetInstanceScript()->GetBossState(DATA_MALKOROK) != IN_PROGRESS)
                return;

            if (Aura* ancientBarrier = owner->GetAura(SPELL_ANCIENT_BARRIER))
            {
                int32 maxAbsorb = owner->GetMaxHealth();
                int32 amountDiff = ancientBarrier->GetEffect(EFFECT_0)->GetAmount() + absorb;

                // Max Absorb by heal is 100% of max HP
                if (amountDiff >= maxAbsorb)
                    amountDiff = maxAbsorb;

                ancientBarrier->GetEffect(EFFECT_0)->ChangeAmount(amountDiff);
                ancientBarrier->SetNeedClientUpdateForTargets();
            }
            else // set new absorb
            {
                int32 bp = absorb > owner->GetMaxHealth() ? owner->GetMaxHealth() : (int32)absorb;

                if (Creature* controller = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(NPC_MALKOROK_CONTROLLER) : ObjectGuid::Empty))
                    controller->CastCustomSpell(owner, SPELL_ANCIENT_BARRIER, &bp, NULL, NULL, true);
            }
        }
    }

    void Register()
    {
        OnEffectAbsorbHeal += AuraEffectAbsorbHealFn(spell_malkorok_ancient_miasma_heal_absorb::OnAbsorb, EFFECT_0);
    }
};

// Ancient Barrier 142862
class spell_malkorok_ancient_barrier : public AuraScript
{
    PrepareAuraScript(spell_malkorok_ancient_barrier);

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        if (!GetUnitOwner())
            return;

        uint32 maxBarrierAmount = GetUnitOwner()->GetMaxHealth();

        if (maxBarrierAmount == 0)
            return;

        uint32 newBarrierAmount = GetAura()->GetEffect(EFFECT_0)->GetAmount();

        float barrierPct = (100.0f * newBarrierAmount) / maxBarrierAmount;
        uint32 newVisualSpellId = GetVisualSpellForAmount(barrierPct);

        if (m_CurrentVisualSpell != newVisualSpellId)
        {
            if (m_CurrentVisualSpell)
                GetUnitOwner()->RemoveAura(m_CurrentVisualSpell);

            GetUnitOwner()->CastSpell(GetUnitOwner(), newVisualSpellId, true);

            m_CurrentVisualSpell = newVisualSpellId;
        }

        if (m_CurrentBarrierAmount != newBarrierAmount)
        {
            m_CurrentBarrierAmount = newBarrierAmount;

            if (AuraEffect* aurEff = GetUnitOwner()->GetAuraEffect(m_CurrentVisualSpell, EFFECT_0))
                aurEff->SetAmount(m_CurrentBarrierAmount);
        }
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (!GetUnitOwner())
            return;

        GetUnitOwner()->RemoveAura(m_CurrentVisualSpell);
    }

    void Register()
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_malkorok_ancient_barrier::HandlePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
        AfterEffectRemove += AuraEffectRemoveFn(spell_malkorok_ancient_barrier::HandleRemove, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }

    private:
        uint32 m_CurrentVisualSpell;
        uint32 m_CurrentBarrierAmount;

        uint32 GetVisualSpellForAmount(const float barrierPct) const
        {
            uint32 spellId = 0;

            if (barrierPct >= 90.f)
                spellId = SPELL_STRONG_ANCIENT_BARRIER;
            else if (barrierPct >= 20.0f)
                spellId = SPELL_MIDDLE_ANCIENT_BARRIER;
            else
                spellId = SPELL_WEAK_ANCIENT_BARRIER;

            return spellId;
        }

};

// Imploding Energy Selector 142988
class spell_malkorok_imploding_energy_selector : public SpellScript
{
    PrepareSpellScript(spell_malkorok_imploding_energy_selector);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            caster->CastSpell(caster, targets.empty() ? SPELL_IMPLOSION_RAID : SPELL_IMPLOSION_PLAYER, true);
            caster->DespawnOrUnsummon(0.5 * IN_MILLISECONDS);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_malkorok_imploding_energy_selector::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// Ancient Miasma Leave Buddy 149443
class spell_malkorok_ancient_miasma_leave_body : public SpellScript
{
    PrepareSpellScript(spell_malkorok_ancient_miasma_leave_body);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_SLG_GENERIC_MOP; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_malkorok_ancient_miasma_leave_body::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Blood Rage 142879
class spell_malkorok_blood_rage_aura : public AuraScript
{
    PrepareAuraScript(spell_malkorok_blood_rage_aura);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (GetOwner() && GetOwner()->ToCreature() && GetOwner()->ToCreature()->GetPower(POWER_ENERGY) == 0)
        {
            GetOwner()->ToCreature()->AI()->Talk(TALK_BLOOD_RAGE_FADED_ANN);
            GetOwner()->ToCreature()->RemoveAurasDueToSpell(SPELL_BLOOD_RAGE);
            GetOwner()->ToCreature()->CastSpell(GetOwner()->ToCreature(), SPELL_EXPEL_MIASMA, false);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_malkorok_blood_rage_aura::OnTrigger, EFFECT_0, SPELL_AURA_POWER_BURN);
    }
};

// Expel Miasma 143199
class spell_malkorok_expel_miasma : public SpellScript
{
    PrepareSpellScript(spell_malkorok_expel_miasma);

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->DoAction(ACTION_EXPEL_MIASMA);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_malkorok_expel_miasma::HandleAfterCast);
    }
};

// Displaced Energy 142913
class spell_malkorok_displaced_energy : public SpellScript
{
    PrepareSpellScript(spell_malkorok_displaced_energy);

    std::list<WorldObject*> m_targets, copyTargets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // Try not select targets with tank/melee specs
        targets.remove_if(MeeleSpecTargetSelector());

        uint8 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 4 : 2;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 2; // min 2, max 4
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 15)
                targetsCount++;

            if (totalPlayersCount > 20)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() >= targetsCount) // MaxAffect is 2(4)
        {
            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount);

            copyTargets = targets;
            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 2; // min 2, max 4
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 15)
                targetsCount++;

            if (totalPlayersCount > 20)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() > targetsCount)
            Trinity::Containers::RandomResizeList(targets, targetsCount);

        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>&targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_malkorok_displaced_energy::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_malkorok_displaced_energy::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_malkorok_displaced_energy::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Displaced Energy 142913
class spell_malkorok_displaced_energy_aura : public AuraScript
{
    PrepareAuraScript(spell_malkorok_displaced_energy_aura);

    void HandleOnRemove(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_DISPLACED_ENERGY_EFF, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_malkorok_displaced_energy_aura::HandleOnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Displaced Energy 142928
class spell_malkorok_displaced_energy_eff : public SpellScript
{
    PrepareSpellScript(spell_malkorok_displaced_energy_eff);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_DISPLACED_ENERGY); });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_malkorok_displaced_energy_eff::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Essence of Y`shaarj Trigger Eff 143848
class spell_malkorok_essence_of_yshaarj_trigger_eff : public SpellScript
{
    PrepareSpellScript(spell_malkorok_essence_of_yshaarj_trigger_eff);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            if (caster->GetInstanceScript() && caster->GetInstanceScript()->GetBossState(DATA_MALKOROK) != IN_PROGRESS)
                return;

            caster->CastSpell(GetHitDest()->GetPositionX(), GetHitDest()->GetPositionY(), GetHitDest()->GetPositionZ(), SPELL_ESSENCE_OF_YSHAARJ_AT, true);
        }
    }

    void SelectTargets(SpellDestination& dest)
    {
        float dist = frand(20.0f, 40.0f);
        float angle = frand(0.0f, 2 * M_PI);
        float x = GetCaster()->GetPositionX() + dist * cos(angle);
        float y = GetCaster()->GetPositionY() + dist * sin(angle);

        Position newPos = { x, y, GetCaster()->GetPositionZ(), 0 };
        dest.Relocate(newPos);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_malkorok_essence_of_yshaarj_trigger_eff::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_malkorok_essence_of_yshaarj_trigger_eff::SelectTargets, EFFECT_0, TARGET_DEST_DEST_RANDOM);
    }
};

// Essence of Y`shaarj Launcher 143846
class spell_malkorok_essence_of_yshaarj_launcher : public AuraScript
{
    PrepareAuraScript(spell_malkorok_essence_of_yshaarj_launcher)

    void OnPeriodic(AuraEffect const* aurEff)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (owner->GetMap()->Is25ManRaid())
            {
                for (uint32 i = 0; i < 4; i++)
                    owner->CastSpell(owner, SPELL_ESSENCE_OF_YSHAARJ_TRIGGER_EFF, true);
            }
            else
                owner->CastSpell(owner, SPELL_ESSENCE_OF_YSHAARJ_TRIGGER_EFF, true);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_malkorok_essence_of_yshaarj_launcher::OnPeriodic, EFFECT_2, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// Y`shaarj Corruption 142848
class spell_malkorok_yshaarj_corruption : public SpellScript
{
    PrepareSpellScript(spell_malkorok_yshaarj_corruption);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_CORRUPTED_SKULLSPLITTER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_malkorok_yshaarj_corruption::SelectTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_110);
    }
};

// Y`shaarj Corruption 142848
class spell_malkorok_yshaarj_corruption_aura : public AuraScript
{
    PrepareAuraScript(spell_malkorok_yshaarj_corruption_aura);

    void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            owner->GetMap()->SetWorldState(WORLDSTATE_UNLIMITED_POTENTIAL, 1);
            owner->GetMap()->SetWorldState(WORLDSTATE_UNLIMITED_POTENTIAL_2, 1);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_malkorok_yshaarj_corruption_aura::HandleOnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1048. Summoned by 143850 - Essence of Y`shaarj
class sat_malkorok_essence_of_yshaarj : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && object->ToPlayer()->IsAlive();
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                // Remove barrier before damage effect
                itr->RemoveAurasDueToSpell(SPELL_ANCIENT_BARRIER);
                caster->CastSpell(itr, SPELL_ESSENCE_OF_YSHAARJ_EFF, true);
            }
        }
    }
};

// 1051. Summoned by 142989 - Languish
class sat_malkorok_languish : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_LANGUISH, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_LANGUISH);
    }
};

void AddSC_malkorok()
{
    new boss_malkorok();
    new creature_script<npc_malkorok_implosion>("npc_malkorok_implosion");
    new creature_script<npc_malkorok_arcing_smash>("npc_malkorok_arcing_smash");
    new creature_script<npc_malkorok_ancient_miasma>("npc_malkorok_ancient_miasma");
    new creature_script<npc_malkorok_controller>("npc_malkorok_controller");
    new creature_script<npc_malkorok_living_corruption>("npc_malkorok_living_corruption");

    new spell_script<spell_malkorok_implosion_cosmetic>("spell_malkorok_implosion_cosmetic");
    new spell_script<spell_malkorok_siesmic_slam_selector>("spell_malkorok_siesmic_slam_selector");
    new spell_script<spell_malkorok_arcing_smash_preparation>("spell_malkorok_arcing_smash_preparation");
    new spell_script<spell_malkorok_arcing_smash_controll>("spell_malkorok_arcing_smash_controll");
    new aura_script<spell_malkorok_arcing_smash_visual_cast>("spell_malkorok_arcing_smash_visual_cast");
    new spell_script<spell_malkorok_breath_of_yshaarj>("spell_malkorok_breath_of_yshaarj");
    new spell_script<spell_malkorok_eradicate>("spell_malkorok_eradicate");
    new spell_script<spell_malkorok_blood_rage>("spell_malkorok_blood_rage");
    new aura_script<spell_malkorok_ancient_miasma_heal_absorb>("spell_malkorok_ancient_miasma_heal_absorb");
    new aura_script<spell_malkorok_ancient_barrier>("spell_malkorok_ancient_barrier");
    new spell_script<spell_malkorok_imploding_energy_selector>("spell_malkorok_imploding_energy_selector");
    new spell_script<spell_malkorok_ancient_miasma_leave_body>("spell_malkorok_ancient_miasma_leave_body");
    new aura_script<spell_malkorok_blood_rage_aura>("spell_malkorok_blood_rage_aura");
    new spell_script<spell_malkorok_expel_miasma>("spell_malkorok_expel_miasma");
    new spell_script<spell_malkorok_displaced_energy>("spell_malkorok_displaced_energy");
    new aura_script<spell_malkorok_displaced_energy_aura>("spell_malkorok_displaced_energy_aura");
    new spell_script<spell_malkorok_displaced_energy_eff>("spell_malkorok_displaced_energy_eff");
    new spell_script<spell_malkorok_essence_of_yshaarj_trigger_eff>("spell_malkorok_essence_of_yshaarj_trigger_eff");
    new aura_script<spell_malkorok_essence_of_yshaarj_launcher>("spell_malkorok_essence_of_yshaarj_launcher");
    new spell_script<spell_malkorok_yshaarj_corruption>("spell_malkorok_yshaarj_corruption");
    new aura_script<spell_malkorok_yshaarj_corruption_aura>("spell_malkorok_yshaarj_corruption_aura");
    new atrigger_script<sat_malkorok_essence_of_yshaarj>("sat_malkorok_essence_of_yshaarj");
    new atrigger_script<sat_malkorok_languish>("sat_malkorok_languish");
};
