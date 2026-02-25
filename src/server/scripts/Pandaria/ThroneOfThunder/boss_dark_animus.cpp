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

#include "throne_of_thunder.h"

enum Spells
{
    /*Dark Animus*/
    SPELL_ACTIVATION_SEQUENCE           = 139537, // LFR only
    SPELL_SIPHON_ANIMA_EFF              = 138118, // misc?
    SPELL_SIPHON_ANIMA                  = 138644,
    SPELL_ANIMA_FONT_EFF                = 138707,
    SPELL_ANIMA_FONT_MISSLE             = 138697,
    SPELL_ANIMA_FONT_SELECTOR           = 138694,
    SPELL_ANIMA_FONT                    = 138691,
    SPELL_TOUCH_OF_THE_ANIMUS           = 138659,
    SPELL_INTERRUPTING_JOLT             = 138763,
    SPELL_FULL_POWER_EFF                = 138738,
    SPELL_FULL_POWER_MISSLE             = 138749,
    SPELL_FULL_POWER_SELECTOR           = 138734,
    SPELL_FULL_POWER                    = 138729,
    SPELL_ANIMA_RING_EFF                = 136962,
    SPELL_ANIMA_RING                    = 136954, // Heloo, it`s me! (c) Moving AT
    SPELL_ANIMA_RING_ROOT               = 138978, // on hit 136954
    SPELL_ANIMA_RING_2                  = 136955,
    SPELL_ANIMA_RING_3                  = 136956,
    SPELL_ANIMA_RING_5                  = 136957,
    SPELL_ANIMA_RING_6                  = 136958,
    SPELL_ANIMA_RING_9                  = 136959,
    SPELL_ANIMA_RING_11                 = 136960,
    SPELL_ANIMA_RING_8                  = 138671,
    SPELL_ANIMA_RING_12                 = 138672,
    SPELL_ANIMA_RING_4                  = 138673,
    SPELL_ANIMA_RING_1                  = 138674,
    SPELL_ANIMA_RING_7                  = 138675,
    SPELL_ANIMA_RING_10                 = 138676,

    /*Anima Golem*/
    SPELL_EVASIVE                       = 140759,
    SPELL_ACCELERATION_LINK             = 138451,
    SPELL_ACCELERATION_LINK_SELECTOR    = 138452,
    SPELL_ACCELERATION_LINK_EFF         = 138453,

    /*Large Anima Golem*/
    SPELL_CRIMSON_WAKE_EFF              = 138485,
    SPELL_CRIMSON_WAKE_SUMM             = 138480,
    SPELL_CRIMSON_WAKE_SLOW             = 138482,
    SPELL_CRIMSON_WAKE_MED              = 138484,
    SPELL_CRIMSON_WAKE_FAST             = 138544,
    SPELL_CRIMSON_WAKE_SELECTOR         = 138541,
    SPELL_CRIMSON_WAKE_FIXATE           = 138486,

    /*Massive Anima Golem*/
    SPELL_EXPLOSIVE_SLAM                = 138569,
    SPELL_MATTER_SWAP                   = 138609,
    SPELL_MATTER_SWAP_ALLY_SELECTOR     = 138615,
    SPELL_MATTER_SWAP_EFF               = 138618,
    SPELL_MATTER_SWAP_TELEPORT          = 138613,
    SPELL_MATTER_SWAP_ANNOUNCE_SELECTOR = 139919,

    /*Misc | Shared*/
    SPELL_ANIMA_ORB_VISUAL              = 138322,
    SPELL_TRANSFUSION_MISSILE           = 138908,
    SPELL_TRANSFUSION_TRANSFORM         = 138399,
    SPELL_TRANSFUSION_ORB               = 138378,
    SPELL_TRANSFUSION_ENERGIZE_4        = 138399,
    SPELL_TRANSFUSION_BY_VALUE          = 138909,
    SPELL_DISABLE_ENERGY_REGEN          = 72242,

    SPELL_POWERED_DOWN                  = 138373,
    SPeLL_CRITICALLY_DAMAGED_ACTIVATED  = 138409,
    SPELL_CIRITCALY_DAMAGED             = 138400,

    SPELL_ANIMA_CAPACITY_4              = 138384,
    SPELL_ANIMA_CAPACITY_8              = 138385,
    SPELL_ANIMA_CAPACITY_36             = 138386,
    SPELL_ANIMA_CAPACITY_100            = 138387,
    SPELL_THREAT_PROC_EFF               = 61733,

    /*Heroic*/
    SPELL_EMPOWER_GOLEM                 = 138780,
};

enum Events
{
    EVENT_SIPHON_ANIMA = 1,
    EVENT_ANIMA_RING,
    EVENT_INTERRUPTING_JOLT,
    EVENT_ANIMA_FONT,
    EVENT_FULL_POWER,
    EVENT_TOUCH_OF_ANIMA,
    EVENT_CHECK_WIPE_OR_RESET,
    EVENT_CRIMSON_WAKE,
    EVENT_EXPLOSIVE_SLAM,
    EVENT_MATTER_SWAP,
    EVENT_EMPOWER_GOLEM,
};

enum Actions
{
    ACTION_ANIMA_ORB_ACTIVATE,
    ACTION_RESET_GOLEM,
    ACTION_CRIMSON_WAKE,
};

enum Yells
{
    TALK_ANN_INTRO = 0,
    TALK_ANN_JOLT = 0,
    TALK_ANN_SIPHON,
};

// Dark Animus 69427
class boss_dark_animus : public CreatureScript
{
    public:
        boss_dark_animus() : CreatureScript("boss_dark_animus") { }

        struct boss_dark_animusAI : public BossAI
        {
            boss_dark_animusAI(Creature* creature) : BossAI(creature, DATA_DARK_ANIMUS) 
            {
                me->setActive(true);
            }

            uint32 gainPower;
            ObjectGuid animaRingGUID;
            bool activated;
            bool ringOfAnima;
            bool touchOfAnima;
            bool animaFont;
            bool interruptingJolt;
            bool FullPower;
            bool atEvade;

            void Reset() override
            {
                _Reset();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                me->SetReactState(REACT_PASSIVE);
                gainPower = 0;
                activated = false;
                activated = false;
                ringOfAnima = false;
                touchOfAnima = false;
                animaFont = false;
                interruptingJolt = false;
                FullPower = false;
                atEvade = false;
                animaRingGUID = ObjectGuid::Empty;
                me->SetPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 0);
                DoCast(me, SPELL_DISABLE_ENERGY_REGEN, true);
                DoCast(me, SPELL_ANIMA_CAPACITY_100, true);
                DoCast(me, SPELL_POWERED_DOWN, true);

                events.Reset();
            }

            void SetGUID(ObjectGuid guid, int32 type) override
            {
                animaRingGUID = guid;
            }

            ObjectGuid GetGUID(int32 /*type*/) const override
            {
                return animaRingGUID;
            }

            void JustEngagedWith(Unit* who) override
            {
                _JustEngagedWith();

                if (GameObject* go = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GO_ANIMUS_ENTRANCE) : ObjectGuid::Empty))
                    go->SetGoState(GO_STATE_READY);

                if (IsHeroic())
                    events.ScheduleEvent(EVENT_EMPOWER_GOLEM, 20 * IN_MILLISECONDS);

                if (instance && instance->GetData(LFR_DATA))
                    DoCast(me, SPELL_ACTIVATION_SEQUENCE, true);
            }

            void DoAction(int32 actionId) override
            {
                if (Creature* orb = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ANIMA_ORB) : ObjectGuid::Empty))
                    if (!orb->AI()->GetData(TYPE_DARK_ANIMUS)) // prevent energize if not launched
                        return;

                gainPower += actionId;

                // Init
                if (!activated)
                {
                    activated = true;

                    if (instance)
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                    me->RemoveAurasDueToSpell(SPELL_POWERED_DOWN);

                    scheduler
                        .SetValidator([this] { return instance && instance->GetBossState(DATA_DARK_ANIMUS) == IN_PROGRESS; })
                        .Schedule(Seconds(2), [this](TaskContext context)
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                        me->SetReactState(REACT_AGGRESSIVE);
                    });

                    events.ScheduleEvent(EVENT_SIPHON_ANIMA, IsHeroic() ? 120 * IN_MILLISECONDS : 20 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_TOUCH_OF_ANIMA, 12.5 * IN_MILLISECONDS);
                }

                // On Gain 25 Anima Power
                if (gainPower >= 25 && !ringOfAnima)
                {
                    ringOfAnima = true;
                    events.ScheduleEvent(EVENT_ANIMA_RING, 1 * IN_MILLISECONDS);
                }

                // On Gain 50 Anima Power
                if (gainPower >= 50 && !animaFont)
                {
                    animaFont = true;
                    events.ScheduleEvent(EVENT_ANIMA_FONT, 1 * IN_MILLISECONDS);
                }

                // On Gain 75 Anima Power
                if (gainPower >= 75 && !interruptingJolt)
                {
                    interruptingJolt = true;
                    events.ScheduleEvent(EVENT_INTERRUPTING_JOLT, 1 * IN_MILLISECONDS);
                }

                // On Gain 100 Anima Power
                if (gainPower > 99 && !FullPower)
                {
                    if (instance && !instance->GetData(LFR_DATA))
                        events.ScheduleEvent(EVENT_FULL_POWER, 1 * IN_MILLISECONDS);

                    FullPower = true;
                }
            }

            void EnterEvadeMode() override
            {
                if (atEvade)
                    return;

                atEvade = true;

                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_DARK_ANIMUS, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOUCH_OF_THE_ANIMUS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANIMA_FONT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANIMA_RING_EFF);

                    if (Creature* animaOrb = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ANIMA_ORB)))
                        animaOrb->AI()->EnterEvadeMode();

                    if (GameObject* go = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_ANIMUS_ENTRANCE)))
                        go->SetGoState(GO_STATE_ACTIVE);
                }

                _DespawnAtEvade();
                me->GetMotionMaster()->MoveTargetedHome();
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                me->RemoveAllAreasTrigger();

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOUCH_OF_THE_ANIMUS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANIMA_FONT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANIMA_RING_EFF);

                    if (Creature* animaOrb = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ANIMA_ORB)))
                        animaOrb->AI()->JustDied(me);

                    if (GameObject* go = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_ANIMUS_ENTRANCE)))
                        go->SetGoState(GO_STATE_ACTIVE);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                scheduler.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SIPHON_ANIMA:
                            Talk(TALK_ANN_SIPHON);
                            DoCast(me, SPELL_SIPHON_ANIMA);
                            events.ScheduleEvent(EVENT_SIPHON_ANIMA, 20 * IN_MILLISECONDS);
                            break;
                        case EVENT_TOUCH_OF_ANIMA:
                            DoCast(me, SPELL_TOUCH_OF_THE_ANIMUS);
                            events.ScheduleEvent(EVENT_TOUCH_OF_ANIMA, urand(15.5 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
                            break;
                        case EVENT_ANIMA_RING:
                            if (Unit* target = me->GetVictim())
                            {
                                animaRingGUID = target->GetGUID();
                                DoCast(target, SPELL_ANIMA_RING_ROOT, true);
                                DoCast(target, SPELL_ANIMA_RING);
                            }
                            events.ScheduleEvent(EVENT_ANIMA_RING, 24.2 * IN_MILLISECONDS);
                            break;
                        case EVENT_ANIMA_FONT:
                            DoCast(me, SPELL_ANIMA_FONT);
                            events.ScheduleEvent(EVENT_ANIMA_FONT, urand(24 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                            break;
                        case EVENT_INTERRUPTING_JOLT:
                            Talk(TALK_ANN_JOLT);
                            DoCast(me, SPELL_INTERRUPTING_JOLT);
                            events.ScheduleEvent(EVENT_INTERRUPTING_JOLT, IsHeroic() ? 21.5 * IN_MILLISECONDS : urand(23 * IN_MILLISECONDS, 24 * IN_MILLISECONDS));
                            break;
                        case EVENT_FULL_POWER:
                            me->PrepareChanneledCast(me->GetOrientation(), SPELL_FULL_POWER);
                            events.ScheduleEvent(EVENT_FULL_POWER, 5.5 * IN_MILLISECONDS); // in case
                            break;
                        case EVENT_EMPOWER_GOLEM:
                            DoCast(me, SPELL_EMPOWER_GOLEM);
                            events.ScheduleEvent(EVENT_EMPOWER_GOLEM, urand(15 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 135.0f);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_dark_animusAI(creature);
        }
};

// Anima Orb 69756
class npc_anima_orb : public CreatureScript
{
    public:
        npc_anima_orb() : CreatureScript("npc_anima_orb") { }

        struct npc_anima_orbAI : public ScriptedAI
        {
            npc_anima_orbAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            TaskScheduler scheduler;
            uint32 combatValue;
            bool atEvade;

            void InitializeAI() override
            {
                me->setActive(true);
                me->SetDisplayFromModel(1);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                me->SetReactState(REACT_PASSIVE);
                instance = me->GetInstanceScript();
                Reset();

                // Init golem setup cuz sometimes we have issue with visibility initialize in instance script (12 broken instead 13)
                // delay for init 
                me->m_Events.Schedule(1500, [this]()
                {
                    if (instance)
                        instance->SetData(GOLEM_DATA, DONE);
                });
            }
            
            void Reset() override
            {
                if (instance && instance->GetBossState(DATA_DARK_ANIMUS) == DONE)
                    return;

                me->SetDisplayFromModel(1);
                DoCast(me, SPELL_ANIMA_ORB_VISUAL, true);
                me->SetFaction(14);
                me->SetMaxHealth(1);
                me->SetVisible(true);
                me->SetReactState(REACT_PASSIVE);
                atEvade = false;
                combatValue = 0;
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_DARK_ANIMUS)
                    return combatValue;

                return 0;
            }

            void JustDied(Unit* /*killer*/) override
            {
                scheduler.CancelAll();

                std::list<Creature*> golemsList;
                GetCreatureListWithEntryInGrid(golemsList, me, NPC_ANIMUS_GOLEM, 200.0f);
                GetCreatureListWithEntryInGrid(golemsList, me, NPC_LARGE_ANIMA_GOLEM, 200.0f);
                GetCreatureListWithEntryInGrid(golemsList, me, NPC_MASSIVE_ANIMA_GOLEM, 200.0f);

                for (auto&& itr : golemsList)
                    itr->DisappearAndDie();

                me->DisappearAndDie();
            }

            void EnterEvadeMode() override
            {
                if (atEvade)
                    return;

                atEvade = true;

                me->DeleteThreatList();
                me->CombatStop(true);

                summons.DespawnAll();

                scheduler.CancelAll();

                if (Creature* animus = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_DARK_ANIMUS) : ObjectGuid::Empty))
                    animus->AI()->EnterEvadeMode();

                std::list<Creature*> golemsList;
                GetCreatureListWithEntryInGrid(golemsList, me, NPC_ANIMUS_GOLEM, 200.0f);
                GetCreatureListWithEntryInGrid(golemsList, me, NPC_LARGE_ANIMA_GOLEM, 200.0f);
                GetCreatureListWithEntryInGrid(golemsList, me, NPC_MASSIVE_ANIMA_GOLEM, 200.0f);

                for (auto&& itr : golemsList)
                    itr->AI()->EnterEvadeMode();

                uint32 corpseDelay = me->GetCorpseDelay();
                uint32 respawnDelay = me->GetRespawnDelay();

                me->SetCorpseDelay(1);
                me->SetRespawnDelay(29);

                me->DespawnOrUnsummon();

                me->SetCorpseDelay(corpseDelay);
                me->SetRespawnDelay(respawnDelay);
                me->GetMap()->CreatureRelocation(me, me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ(), me->GetHomePosition().GetOrientation());
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                // Prevent multi anima gain
                if (combatValue)
                {
                    damage = 0;
                    return;
                }

                if (int32(me->GetHealth() - damage) < 1)
                {
                    damage = 0;
                    me->RemoveAurasDueToSpell(SPELL_ANIMA_ORB_VISUAL);
                    DoCast(me, SPELL_TRANSFUSION_ORB);

                    if (instance && !instance->GetData(LFR_DATA))
                        DoCast(me, SPELL_ACCELERATION_LINK, true);

                    if (Creature* animus = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_DARK_ANIMUS) : ObjectGuid::Empty))
                        DoZoneInCombat(animus, 200.0f);

                    // Init combat at pull (break evade).
                    std::list<Creature*> animaList;
                    GetCreatureListWithEntryInGrid(animaList, me, NPC_ANIMUS_GOLEM, 200.0f);
                    GetCreatureListWithEntryInGrid(animaList, me, NPC_LARGE_ANIMA_GOLEM, 200.0f);
                    GetCreatureListWithEntryInGrid(animaList, me, NPC_MASSIVE_ANIMA_GOLEM, 200.0f);

                    for (auto&& itr : animaList)
                        itr->SetInCombatWithZone();

                    me->SetVisible(false);
                    combatValue = 1;

                    scheduler
                        .Schedule(Seconds(1), [this](TaskContext context)
                    {
                        if (instance && instance->IsWipe(75.0f, me))
                        {
                            EnterEvadeMode();
                            return;
                        }

                        context.Repeat(Seconds(1));
                    });

                    Talk(TALK_ANN_INTRO);
                }
            }

            void UpdateAI(uint32 diff) override 
            {
                scheduler.Update(diff);
            }

            private:
                SummonList summons;
                InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_anima_orbAI(creature);
        }
};

struct golemsBaseAI : public ScriptedAI
{
    golemsBaseAI(Creature* creature) : ScriptedAI(creature), summons(me), hasBroken(false) { }

    InstanceScript* instance;
    EventMap events;
    TaskScheduler scheduler;
    SummonList summons;
    uint32 gainPower;
    bool activated;
    bool hasBroken;
    bool gainAbility;
    bool atEvade;

    void InitializeAI() override
    {
        me->setActive(true);
        instance = me->GetInstanceScript();

        if (instance && instance->GetBossState(DATA_DARK_ANIMUS) != DONE)
            me->Respawn();

        Reset();
    }

    void Reset() override
    {
        events.Reset();
        scheduler.CancelAll();

        activated   = false;
        gainAbility = false;
        atEvade     = false;
        gainPower   = 0;
        me->SetPowerType(POWER_ENERGY);
        ScriptedAI::Reset();
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
        me->SetReactState(REACT_DEFENSIVE);

        DoCast(me, SPELL_DISABLE_ENERGY_REGEN, true);
        DoCast(me, SPELL_POWERED_DOWN, true);

        switch (me->GetEntry())
        {
            case NPC_ANIMUS_GOLEM:
                if (instance && !instance->GetData(LFR_DATA))
                    DoCast(me, SPELL_EVASIVE, true);

                me->SetMaxPower(POWER_ENERGY, 4);
                DoCast(me, SPELL_ANIMA_CAPACITY_4, true);
                break;
            case NPC_LARGE_ANIMA_GOLEM:
                me->SetMaxPower(POWER_ENERGY, 8);
                DoCast(me, SPELL_ANIMA_CAPACITY_8, true);
                break;
            case NPC_MASSIVE_ANIMA_GOLEM:
                me->SetMaxPower(POWER_ENERGY, 36);
                DoCast(me, SPELL_ANIMA_CAPACITY_36, true);
                break;
        }

        me->SetPower(POWER_ENERGY, 0);

        // Reset broken golem if it possible
        if (hasBroken)
            DoAction(ACTION_BROKEN_GOLEM);
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_ANIMA_ENERGY)
            return gainPower;

        return 0;
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_BROKEN_GOLEM)
        {
            DoCast(me, SPELL_CRITICALLY_DAMAGED, true);
            me->setRegeneratingHealth(false);
            me->SetHealth(CalculatePct(me->GetMaxHealth(), 5.0f));
            hasBroken = true;
            return;
        }

        if (Creature* orb = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ANIMA_ORB) : ObjectGuid::Empty))
            if (!orb->AI()->GetData(TYPE_DARK_ANIMUS)) // prevent energize if not launched
                return;

        actionId > 50 ? gainPower -= actionId - 50 : gainPower += actionId;

        if (gainPower < 1 && !me->HasAura(SPELL_POWERED_DOWN))
        {
            DoCast(me, SPELL_POWERED_DOWN);
            me->SetReactState(REACT_DEFENSIVE);
            activated = false;
            gainAbility = false;
            events.Reset();
            return;
        }

        if (gainPower < 1)
            return;

        if (me->HasAura(SPELL_CRITICALLY_DAMAGED) && !activated)
        {
            activated = true;
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
            me->RemoveAurasDueToSpell(SPELL_POWERED_DOWN);
            return;
        }

        if (!activated)
        {
            activated = true;
            me->RemoveAurasDueToSpell(SPELL_POWERED_DOWN);

            scheduler
                .SetValidator([this] { return instance && instance->GetBossState(DATA_DARK_ANIMUS) == IN_PROGRESS; })
                .Schedule(Seconds(2), [this](TaskContext context)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                me->SetReactState(REACT_AGGRESSIVE);
                DoZoneInCombat(me, 200.0f);
            });
        }
        
        if (gainAbility)
            return;

        switch (me->GetEntry())
        {
            case NPC_LARGE_ANIMA_GOLEM:
                gainAbility = true;
                events.ScheduleEvent(EVENT_CRIMSON_WAKE, urand(7.5 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
                break;
            case NPC_MASSIVE_ANIMA_GOLEM:
                gainAbility = true;
                events.ScheduleEvent(EVENT_EXPLOSIVE_SLAM, urand(12.5 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_MATTER_SWAP, urand(10 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                break;
        }
    }

    void EnterEvadeMode() override
    {
        if (atEvade)
            return;

        atEvade = true;

        if (Creature* animaOrb = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ANIMA_ORB)))
            animaOrb->AI()->EnterEvadeMode();

        if (!me->IsAlive())
            me->Respawn();

        me->DeleteThreatList();
        me->CombatStop(true);

        uint32 corpseDelay = me->GetCorpseDelay();
        uint32 respawnDelay = me->GetRespawnDelay();

        me->SetCorpseDelay(1);
        me->SetRespawnDelay(29);

        me->DespawnOrUnsummon();

        me->SetCorpseDelay(corpseDelay);
        me->SetRespawnDelay(respawnDelay);
        me->GetMotionMaster()->MoveTargetedHome();
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (gainPower > 0)
            DoCast(me, SPELL_TRANSFUSION_MISSILE, true);
    }
};

// Animus Golem 69701
class npc_animus_golem : public CreatureScript
{
    public:
        npc_animus_golem() : CreatureScript("npc_animus_golem") { }

        struct npc_animus_golemAI : public golemsBaseAI
        {
            npc_animus_golemAI(Creature* creature) : golemsBaseAI(creature) { }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (instance && instance->GetBossState(DATA_DARK_ANIMUS) != IN_PROGRESS)
                    instance->SetBossState(DATA_DARK_ANIMUS, IN_PROGRESS);
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 135.0f);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_animus_golemAI(creature);
        }
};

// Large Animus Golem 69700
class npc_large_animus_golem : public CreatureScript
{
    public:
        npc_large_animus_golem() : CreatureScript("npc_large_animus_golem") { }

        struct npc_large_animus_golemAI : public golemsBaseAI
        {
            npc_large_animus_golemAI(Creature* creature) : golemsBaseAI(creature) { }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_CRIMSON_WAKE)
                    {
                        DoCast(me, SPELL_CRIMSON_WAKE_SUMM);
                        events.ScheduleEvent(EVENT_CRIMSON_WAKE, urand(15 * IN_MILLISECONDS, 35 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 135.0f);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_large_animus_golemAI(creature);
        }
};

// Crimson Wake 69951
struct npc_crimson_wake : public ScriptedAI
{
    npc_crimson_wake(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    InstanceScript* instance;
    ObjectGuid targetGUID;

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        targetGUID = guid;
    }

    void IsSummonedBy(Unit* summoner) override
    {
        instance = me->GetInstanceScript();

        if (Creature* orb = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ANIMA_ORB) : ObjectGuid::Empty))
            orb->AI()->JustSummoned(me);

        me->SetSpeed(MOVE_RUN, 0.1f, true);
        DoCast(me, SPELL_CRIMSON_WAKE_SLOW, true);
        DoCast(me, SPELL_CRIMSON_WAKE_SELECTOR, true);

        scheduler
            .SetValidator([this] { return instance && instance->GetBossState(DATA_DARK_ANIMUS) == IN_PROGRESS; })
            .Schedule(Seconds(1), [this](TaskContext context)
        {
            if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
            {
                if (!target->IsAlive())
                    DoCast(me, SPELL_CRIMSON_WAKE_SELECTOR, true);
            }
            else
                DoCast(me, SPELL_CRIMSON_WAKE_SELECTOR, true);

            context.Repeat(Seconds(1));
        });

        scheduler
            .SetValidator([this] { return instance && instance->GetBossState(DATA_DARK_ANIMUS) == IN_PROGRESS; })
            .Schedule(Seconds(9), [this](TaskContext context)
        {
            DoCast(me, SPELL_CRIMSON_WAKE_SLOW);
            context.Repeat(Seconds(8));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Massive Animus Golem 69699
class npc_massive_animus_golem : public CreatureScript
{
    public:
        npc_massive_animus_golem() : CreatureScript("npc_massive_animus_golem") { }

        struct npc_massive_animus_golemAI : public golemsBaseAI
        {
            npc_massive_animus_golemAI(Creature* creature) :golemsBaseAI(creature) { }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MATTER_SWAP:
                            DoCast(me, SPELL_MATTER_SWAP);
                            events.ScheduleEvent(EVENT_MATTER_SWAP, urand(35 * IN_MILLISECONDS, 40 * IN_MILLISECONDS));
                            break;
                        case EVENT_EXPLOSIVE_SLAM:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_EXPLOSIVE_SLAM);

                            events.ScheduleEvent(EVENT_EXPLOSIVE_SLAM, urand(12.5 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 135.0f);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_massive_animus_golemAI(creature);
        }
};

// Transfusion Missle 138908
class spell_transfusion_missile : public SpellScript
{
    PrepareSpellScript(spell_transfusion_missile);

    void HandleMissile(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Creature* target = GetHitCreature())
            {
                if (Creature* orb = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(NPC_ANIMA_ORB) : ObjectGuid::Empty))
                    if (!orb->AI()->GetData(TYPE_DARK_ANIMUS)) // prevent energize if missle hit at evade
                        return;

                // At death energy dissapear - need save value
                int32 bp = caster->AI()->GetData(TYPE_ANIMA_ENERGY);
                target->CastCustomSpell(target, SPELL_TRANSFUSION_BY_VALUE, &bp, 0, 0, true);
                target->AI()->DoAction(bp);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_transfusion_missile::HandleMissile, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Transfusion Orb 138378
class spell_transfusion_orb : public SpellScript
{
    PrepareSpellScript(spell_transfusion_orb);

    void HandleEnergize(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            if (target->GetEntry() != NPC_DARK_ANIMUS)
                target->CastSpell(target, SPELL_TRANSFUSION_ENERGIZE_4, true);
            else
            {
                int32 bp = 52; // for dark animus
                target->CastCustomSpell(target, SPELL_TRANSFUSION_ENERGIZE_4, &bp, NULL, NULL, true);
            }

            if (Creature* orb = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(NPC_ANIMA_ORB) : ObjectGuid::Empty))
                if (orb->AI()->GetData(TYPE_DARK_ANIMUS)) // prevent energize if missle hit at evade
                    target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
        }
    }

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
        {
            if (caster->GetMap()->IsHeroic())
                targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_ANIMUS_GOLEM && target->GetEntry() != NPC_DARK_ANIMUS; });
            else
                targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_ANIMUS_GOLEM; });
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_transfusion_orb::HandleEnergize, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_transfusion_orb::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Transfusion Intro Energize 138399
class spell_transfusion_intro_energize : public SpellScript
{
    PrepareSpellScript(spell_transfusion_intro_energize);

    void HandleEnergize(SpellEffIndex effIndex)
    {
        if (Creature* target = GetHitCreature())
        {
            if (Creature* orb = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(NPC_ANIMA_ORB) : ObjectGuid::Empty))
            {
                if (orb->AI()->GetData(TYPE_DARK_ANIMUS)) // prevent energize if missle hit at evade
                    target->AI()->DoAction(GetSpellValue()->EffectBasePoints[effIndex]);
                else
                    PreventHitEffect(effIndex);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_transfusion_intro_energize::HandleEnergize, EFFECT_0, SPELL_EFFECT_ENERGIZE);
    }
};

// Acceleration Link 138451
class spell_acceleration_link : public SpellScript
{
    PrepareSpellScript(spell_acceleration_link);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_ANIMUS_GOLEM; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_acceleration_link::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Acceleration Link Selector 138452
class spell_acceleration_link_selector : public SpellScript
{
    PrepareSpellScript(spell_acceleration_link_selector);

    void HandleEnergize(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_ACCELERATION_LINK_EFF, true);
    }

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_ANIMUS_GOLEM; });

        // If target`s count were change - remove diff between stucks
        if (Unit* caster = GetCaster())
        {
            if (Aura* accelerationAura = caster->GetAura(SPELL_ACCELERATION_LINK_EFF))
            {
                if (targets.size() < accelerationAura->GetStackAmount())
                {
                    if (targets.size() > 0)
                        accelerationAura->SetStackAmount(targets.size());
                    else
                        caster->RemoveAurasDueToSpell(SPELL_ACCELERATION_LINK_EFF);
                }
                else if (targets.size() == accelerationAura->GetStackAmount())
                    targets.clear();
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_acceleration_link_selector::HandleEnergize, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_acceleration_link_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Touch of The Animus 138659
class spell_touch_of_the_animus : public SpellScript
{
    PrepareSpellScript(spell_touch_of_the_animus);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove whole exclude dps
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if([=](Player* target) { return target && target->HasAura(SPELL_TOUCH_OF_THE_ANIMUS); });

            uint32 targetCount = caster->GetMap()->Is25ManRaid() ? 2 : 1;

            // if we have enough players without tanks, meelee
            if (pList.size() >= targetCount)
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (targets.size() > targetCount)
                    Trinity::Containers::RandomResizeList(targets, targetCount);

                m_targets = targets;

                return;
            }

            // if we haven`t enough players
            targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_TOUCH_OF_THE_ANIMUS); });

            if (targets.size() > targetCount)
                Trinity::Containers::RandomResizeList(targets, targetCount);

            m_targets = targets;
        }
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_touch_of_the_animus::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_touch_of_the_animus::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Anima Font 138691
class spell_anima_font : public SpellScript
{
    PrepareSpellScript(spell_anima_font);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && (!target->ToUnit()->HasAura(SPELL_TOUCH_OF_THE_ANIMUS) || target->ToUnit()->HasAura(SPELL_ANIMA_FONT)); });

        if (Unit* caster = GetCaster())
        {
            uint32 targetCount = caster->GetMap()->Is25ManRaid() ? 2 : 1;

            if (targets.size() > targetCount)
                Trinity::Containers::RandomResizeList(targets, 1);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_anima_font::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Anima Font Selector 138694
class spell_anima_font_selector : public SpellScript
{
    PrepareSpellScript(spell_anima_font_selector);

    std::list<WorldObject*> m_targets;

    void HandleMissle(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_ANIMA_FONT_MISSLE, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        // Remove whole exclude dps
        m_targets.remove_if(TankSpecTargetSelector());

        // if we have enough players without tanks, meelee
        if (m_targets.size() >= 1)
        {
            targets.clear();

            for (auto&& itr : m_targets)
                targets.push_back(itr);

            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);

            return;
        }

        // if we haven`t enough players
        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_anima_font_selector::HandleMissle, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_anima_font_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Full Power Selector 138734
class spell_full_power_selector : public SpellScript
{
    PrepareSpellScript(spell_full_power_selector);

    void HandleMissle(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_FULL_POWER_MISSLE, true);
    }

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
        {
            uint32 targetCount = caster->GetMap()->Is25ManRaid() ? 7 : 3;

            if (targets.size() > targetCount)
                Trinity::Containers::RandomResizeList(targets, targetCount);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_full_power_selector::HandleMissle, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_full_power_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Siphone Anima 138644
class spell_siphon_anima : public SpellScript
{
    PrepareSpellScript(spell_siphon_anima);

    void HandleEnergize(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            caster->AI()->DoAction(GetSpellInfo()->Effects[effIndex].BasePoints);
            caster->SetPower(POWER_ENERGY, caster->GetPower(POWER_ENERGY) + GetSpellInfo()->Effects[effIndex].BasePoints);

            // Update Creature Diff Power
            if (Creature* target = GetHitCreature())
                target->AI()->DoAction(GetSpellInfo()->Effects[effIndex].BasePoints + 50);
        }
    }

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_ANIMUS_GOLEM && target->GetEntry() != NPC_LARGE_ANIMA_GOLEM && target->GetEntry() != NPC_MASSIVE_ANIMA_GOLEM; });
        targets.remove_if([=](WorldObject* target) {return target && target->ToCreature() && target->ToCreature()->AI()->GetData(TYPE_ANIMA_ENERGY) < 1; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siphon_anima::HandleEnergize, EFFECT_0, SPELL_EFFECT_POWER_DRAIN);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siphon_anima::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Anima Ring 136955, 136956, 136957, 136958, 136959, 136960, 138671, 138672, 138673, 138674, 138675, 138676
class spell_anima_ring : public SpellScript
{
    PrepareSpellScript(spell_anima_ring);

    void SelectTargets(SpellDestination& dest)
    {
        if (Unit* target = GetSpell()->m_targets.GetUnitTarget())
        {
            uint32 mod = GetSpellInfo()->Id <= SPELL_ANIMA_RING_11 ? GetSpellInfo()->Id - SPELL_ANIMA_RING_2 : GetSpellInfo()->Id - SPELL_ANIMA_RING_8 + 6;

            GetPositionWithDistInOrientation(target, 5.0f, M_PI / 6 + (M_PI / 6) * mod, x, y);

            Position newPos = { x, y, target->GetPositionZ() + 0.7f, 0 };
            dest.Relocate(newPos);
        }
    }

    private:
        float x, y;

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_anima_ring::SelectTargets, EFFECT_0, TARGET_DEST_TARGET_ANY);
    }
};

// Matter Swap 138609
class spell_matter_swap : public SpellScript
{
    PrepareSpellScript(spell_matter_swap);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove whole exclude casters
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if(MeeleSpecTargetSelector());
            pList.remove_if([=](Player* target) { return target && target->HasAura(SPELL_MATTER_SWAP); });

            // if we have enough players without tanks, meelee
            if (!pList.empty())
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (targets.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);

                m_targets = targets;

                return;
            }

            // if we haven`t enough players
            pList.remove_if([=](Player* target) { return target && target->HasAura(SPELL_MATTER_SWAP); });

            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);

            m_targets = targets;
        }
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_matter_swap::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_matter_swap::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Matter Swap Aura 138609
class spell_matter_swap_aura : public AuraScript
{
    PrepareAuraScript(spell_matter_swap_aura);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            // Save Last Duration on Remove
            int32 bp = aurEff->GetBase()->GetDuration();
            owner->CastCustomSpell(owner, SPELL_MATTER_SWAP_ALLY_SELECTOR, &bp, 0, 0, true);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_matter_swap_aura::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Matter Swap Ally Selector 138615
class spell_matter_swap_ally_selector : public SpellScript
{
    PrepareSpellScript(spell_matter_swap_ally_selector);

    void HandleDummy(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                int32 damageToCaster = (caster->GetMaxHealth() * GetSpellValue()->EffectBasePoints[effIndex]) / 12000;
                int32 damageToTarget = caster->GetMaxHealth() - damageToCaster;

                // Swap Players and Deal Damage
                caster->CastSpell(target, SPELL_MATTER_SWAP_TELEPORT, true);
                target->CastSpell(caster, SPELL_MATTER_SWAP_TELEPORT, true);
                caster->CastCustomSpell(target, SPELL_MATTER_SWAP_EFF, &damageToTarget, 0, 0, true);
                target->CastCustomSpell(caster, SPELL_MATTER_SWAP_EFF, &damageToCaster, 0, 0, true);

            }
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_MATTER_SWAP); });

        if (Unit* caster = GetCaster())
            targets.sort(Trinity::ObjectDistanceOrderPred(caster, false));

        if (targets.size() > 1)
            targets.resize(1);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_matter_swap_ally_selector::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_matter_swap_ally_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Matter Swap Announce 139919
class spell_matter_swap_announce : public SpellScript
{
    PrepareSpellScript(spell_matter_swap_announce);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_MATTER_SWAP); });

        if (Unit* caster = GetCaster())
            targets.sort(Trinity::ObjectDistanceOrderPred(caster, false));

        if (targets.size() > 1)
            targets.resize(1);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_matter_swap_announce::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Crimson Wake Speed 138482, 138484, 138544
class spell_crimson_wake_speed : public AuraScript
{
    PrepareAuraScript(spell_crimson_wake_speed);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
            /*switch (GetSpellInfo()->Id)
            {
                case SPELL_CRIMSON_WAKE_SLOW:
                    owner->CastSpell(owner, SPELL_CRIMSON_WAKE_MED, true);
                    break;
                case SPELL_CRIMSON_WAKE_MED:
                    owner->CastSpell(owner, SPELL_CRIMSON_WAKE_FAST, true);
                    break;
                case SPELL_CRIMSON_WAKE_FAST:
                    owner->DespawnOrUnsummon();
                    break;
            }*/
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_crimson_wake_speed::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Crimson Wake Selector 138541
class spell_crimson_wake_selector : public SpellScript
{
    PrepareSpellScript(spell_crimson_wake_selector);

    void HandleDummy(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->FixateOnTarget(target->GetGUID());
                caster->CastSpell(target, SPELL_CRIMSON_WAKE_FIXATE, true);
                caster->ClearUnitState(UNIT_STATE_CASTING);
            }
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove whole exclude casters
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if(MeeleSpecTargetSelector());
            pList.remove_if([=](Player* target) { return target && target->HasAura(SPELL_CRIMSON_WAKE_FIXATE); });

            // if we have enough players without tanks, meelee
            if (!pList.empty())
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (targets.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);

                return;
            }

            // if we haven`t enough players
            pList.remove_if([=](Player* target) { return target && target->HasAura(SPELL_CRIMSON_WAKE_FIXATE); });

            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_crimson_wake_selector::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_crimson_wake_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// 719. Summoned by 136955, 136956, 136957, 136958, 136959, 136960, 138671, 138672, 138673, 138674, 138675, 138676 - Anima Ring
class sat_anima_ring : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer();
    }

    void OnInit() override
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = ObjectAccessor::GetUnit(*caster, caster->AI()->GetGUID()))
            {
                AreaTrigger* at = GetAreaTrigger();
                float dist = at->GetExactDist2d(target);
                float x, y;

                std::vector<Position> path;

                for (uint32 i = 0; i < 16; ++i)
                {
                    x = at->GetPositionX() + ((dist / 16.0f) * i * cos(at->GetAngle(target)));
                    y = at->GetPositionY() + ((dist / 16.0f) * i * sin(at->GetAngle(target)));
                    path.push_back({ x, y, at->GetPositionZ() });
                }

                GetAreaTrigger()->InitSpline(path, at->GetDuration());
            }
        }
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
            itr->CastSpell(itr, SPELL_ANIMA_RING_EFF, true);
    }
};

// Threat Proc Eff 61733
class spell_threat_proc_eff : public SpellScript
{
    PrepareSpellScript(spell_threat_proc_eff);

    void HandleDummy(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                target->AddThreat(caster, 1000000.0f);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_threat_proc_eff::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Threat Proc 61732
class spell_threat_proc : public AuraScript
{
    PrepareAuraScript(spell_threat_proc)

    void HandleOnProc(ProcEventInfo& rProcInfo)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (Unit* victim = rProcInfo.GetProcTarget())
                owner->CastSpell(victim, SPELL_THREAT_PROC_EFF, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_threat_proc::HandleOnProc);
    }
};

// Empower Golem 138780
class spell_empower_golem : public SpellScript
{
    PrepareSpellScript(spell_empower_golem);

    std::list<WorldObject*> tempList;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_ANIMUS_GOLEM && target->GetEntry() != NPC_LARGE_ANIMA_GOLEM && target->GetEntry() != NPC_MASSIVE_ANIMA_GOLEM; });
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && !target->ToUnit()->GetVictim(); });

        // In case
        if (targets.empty())
        {
            tempList = targets;
            return;
        }

        // Select golem with lowest HP
        auto const mostInjuredItr = std::min_element(targets.cbegin(), targets.cend(),
            [](WorldObject const* a, WorldObject const* b)
        {
            // This spell can not target anything except units, so no check
            return a->ToUnit()->GetHealthPct() < b->ToUnit()->GetHealthPct();
        });

        auto const mostInjured = (*mostInjuredItr)->ToUnit();
        targets.clear();

        if (mostInjured)
            targets.push_back(mostInjured);

        tempList = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = tempList;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_empower_golem::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_empower_golem::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_empower_golem::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Activation Sequence 139537
class spell_activation_sequence : public AuraScript
{
    PrepareAuraScript(spell_activation_sequence);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_START_INTRO);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_activation_sequence::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class cond_anima_transfusion : public ConditionScript
{
    public:
        cond_anima_transfusion() : ConditionScript("cond_anima_transfusion") { }
    
        bool OnConditionCheck(const Condition* cond, ConditionSourceInfo& source) override
        {
            if (source.mConditionTargets[0] && source.mConditionTargets[1])
                if (Creature* caster = source.mConditionTargets[1]->ToCreature())
                    if (Creature* target = source.mConditionTargets[0]->ToCreature())
                        if (target->AI()->GetData(TYPE_ANIMA_ENERGY) + caster->AI()->GetData(TYPE_ANIMA_ENERGY) <= (uint32)target->GetMaxPower(POWER_ENERGY))
                            return true;
    
            return false;
        }
};

void AddSC_boss_dark_animus()
{
    new boss_dark_animus();
    new npc_anima_orb();
    new npc_animus_golem();
    new npc_large_animus_golem();
    new creature_script<npc_crimson_wake>("npc_crimson_wake");
    new npc_massive_animus_golem();
    new spell_script<spell_transfusion_missile>("spell_transfusion_missile");
    new spell_script<spell_transfusion_orb>("spell_transfusion_orb");
    new spell_script<spell_transfusion_intro_energize>("spell_transfusion_intro_energize");
    new spell_script<spell_acceleration_link>("spell_acceleration_link");
    new spell_script<spell_acceleration_link_selector>("spell_acceleration_link_selector");
    new spell_script<spell_touch_of_the_animus>("spell_touch_of_the_animus");
    new spell_script<spell_anima_font>("spell_anima_font");
    new spell_script<spell_anima_font_selector>("spell_anima_font_selector");
    new spell_script<spell_full_power_selector>("spell_full_power_selector");
    new spell_script<spell_siphon_anima>("spell_siphon_anima");
    new spell_script<spell_anima_ring>("spell_anima_ring");
    new spell_script<spell_matter_swap>("spell_matter_swap");
    new aura_script<spell_matter_swap_aura>("spell_matter_swap_aura");
    new spell_script<spell_matter_swap_ally_selector>("spell_matter_swap_ally_selector");
    new spell_script<spell_matter_swap_announce>("spell_matter_swap_announce");
    new aura_script<spell_crimson_wake_speed>("spell_crimson_wake_speed");
    new spell_script<spell_crimson_wake_selector>("spell_crimson_wake_selector");
    new spell_script<spell_threat_proc_eff>("spell_threat_proc_eff");
    new aura_script<spell_threat_proc>("spell_threat_proc");
    new spell_script<spell_empower_golem>("spell_empower_golem");
    new aura_script<spell_activation_sequence>("spell_activation_sequence");
    new atrigger_script<sat_anima_ring>("sat_anima_ring");
    new cond_anima_transfusion();
}
