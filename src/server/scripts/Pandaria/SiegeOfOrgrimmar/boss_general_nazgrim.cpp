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
#include "siege_of_orgrimmar.h"

enum Spells
{
    // Stance
    SPELL_BATTLE_STANCE           = 143589,
    SPELL_BERSERKER_STANCE        = 143594,
    SPELL_DEFENSIVE_STANCE        = 143593,
                                  
    SPELL_BONECRACKER             = 143638,
    SPELL_COOLING_OFF             = 143484,
    SPELL_HEROIC_SHOCKWAVE        = 143716,
    SPELL_HEROIC_SHOCKWAVE_JUMP   = 143500,
    SPELL_RAVAGER                 = 143872,
    SPELL_RAVAGER_AURA            = 143874,
    SPELL_RAVAGER_EFF             = 143873,
    SPELL_RAVAGER_ENERGIZE        = 143877,
    SPELL_SUNDERING_BLOW          = 143494,
    SPELL_WAR_SONG                = 143503,
    SPELL_AFRTERSHOCK             = 143712,
    SPELL_KORKRON_BANNER          = 143591,
    SPELL_KORKRON_BANNER_EFF      = 143501,
    SPELL_KORKRON_BANNER_VISUAL   = 143536,
    SPELL_KORKRON_BANNER_ALLY_EFF = 143535,
    SPELL_BERSERK                 = 47008,
    SPELL_ZERO_MANA_REGEN         = 96301,
    SPELL_GENERATE_RAGE           = 144278, // by defensive stance

    // Kork`ron Warrions
    SPELL_MAGISTRIKE              = 143431,
    SPELL_ARCANE_SHOCK            = 143432,
    SPELL_UNSTABLE_BLINK          = 143433,

    SPELL_IRON_STORM              = 143420,
    SPELL_LAST_STAND              = 143427,

    SPELL_ASSASSINS_MARK          = 143480,
    SPELL_STEALTH                 = 118969,
    SPELL_BACKSTAB                = 143481,

    SPELL_EARTH_SHIELD_EFF        = 143479,
    SPELL_EARTH_SHIELD            = 143475,
    SPELL_EMPOWERED_CHAIN_HEAL    = 143473,
    SPELL_HEALING_TIDE_EFF        = 143478,
    SPELL_HEALING_TIDE            = 143477,
    SPELL_HEALING_TIDE_TOTEM      = 143474,

    // Heroic
    SPELL_EXECUTE                 = 143502,
    SPELL_HUNTERS_MARK            = 143882,
    SPELL_SHOOT                   = 143884,
    SPELL_MULTI_SHOT              = 143887,
    SPELL_VOLLEY_VISUAL           = 118100, // not found anything else

    // Misc
    SPELL_FIRE_CHANNELING         = 120264,
};

enum Events
{
    EVENT_CHANGE_STANCE = 1,
    EVENT_BERSERK,
    EVENT_SUNDERING_BLOW,
    EVENT_BONECRACKER,
    EVENT_RAGE_ABILITY,
    EVENT_CALL_KORKRON,
    EVENT_EXECUTE,
    EVENT_SPELLS_BASE,

    // Kork`ron
    EVENT_IRON_STORM,
    EVENT_MAGISTRIKE,
    EVENT_ARCANE_SHOCK,
    EVENT_EARTH_SHIELD,
    EVENT_HEALING_TIDE,
    EVENT_CHAIN_HEAL,
    EVENT_BLINK,
    EVENT_LAST_STAND,
    EVENT_SHOT,
    EVENT_MULTY_SHOT,
};

enum Yells
{
    TALK_INTRO_ALLIANCE,
    TALK_AGGRO_ALLIANCE,
    TALK_BATTLE_STANCE_ANN,
    TALK_LAST_STAND, // at ten pct he`ll call a additional wave
    TALK_BERSERKER_STANCE_ANN,
    TALK_DEATH_ALLIANCE,
    TALK_INTRO_HORDE,
    TALK_AGGRO_HORDE,
    TALK_DEATH_HORDE,
    TALK_KORKRON_FORCE,
    TALK_DEFENSIVE_STANCE_ANN,
    TALK_DEATH_HORDE_ADDITIONAL,
};

enum Stances
{
    NONE_STANCE,
    BATTLE_STANCE,
    BERSERKER_STANCE,
    DEFENSIVE_STANCE,
};

enum TypeGUID
{
    GUID_TYPE_RAVAGER,
    GUID_TYPE_SHOCKWAVE,
};

enum procTypes
{
    PROC_DEFENSIVE_STANCE,
    PROC_EARTH_SHIELD,
};

std::map<uint32, std::array<uint32, 2>> invStanceType =
{
    { NONE_STANCE,      { 0,                      0                         } },
    { BATTLE_STANCE,    { SPELL_BATTLE_STANCE,    TALK_BATTLE_STANCE_ANN    } },
    { BERSERKER_STANCE, { SPELL_BERSERKER_STANCE, TALK_BERSERKER_STANCE_ANN } },
    { DEFENSIVE_STANCE, { SPELL_DEFENSIVE_STANCE, TALK_DEFENSIVE_STANCE_ANN } },
};

std::map<uint32, std::array<uint32, 4>> invKorkronForcesType =
{
    { 1, { NPC_KORKRON_IRONBLADE, NPC_KORKRON_ARCWEAVER, 0,                     0 } },
    { 2, { NPC_KORKRON_ASSASSIN, NPC_KORKRON_WARSHAMAN,  0,                     0 } },
    { 3, { NPC_KORKRON_ASSASSIN, NPC_KORKRON_IRONBLADE,  0,                     0 } },
    { 4, { NPC_KORKRON_WARSHAMAN, NPC_KORKRON_ARCWEAVER, 0,                     0 } },
    { 5, { NPC_KORKRON_WARSHAMAN, NPC_KORKRON_IRONBLADE, 0,                     0 } },
    { 6, { NPC_KORKRON_ASSASSIN, NPC_KORKRON_ARCWEAVER,  0,                     0 } },
    { 7, { NPC_KORKRON_ASSASSIN, NPC_KORKRON_WARSHAMAN,  NPC_KORKRON_IRONBLADE, 0 } },
    { 8, { NPC_KORKRON_WARSHAMAN, NPC_KORKRON_ARCWEAVER, NPC_KORKRON_IRONBLADE, 0 } },
    { 9, { NPC_KORKRON_ASSASSIN, NPC_KORKRON_ARCWEAVER,  NPC_KORKRON_WARSHAMAN, 0 } },
    { 10, { NPC_KORKRON_ASSASSIN, NPC_KORKRON_ARCWEAVER, NPC_KORKRON_IRONBLADE, 0 } },

    { 11, { NPC_KORKRON_ASSASSIN, NPC_KORKRON_ARCWEAVER, NPC_KORKRON_IRONBLADE, NPC_KORKRON_WARSHAMAN } }, // additional forces at low hp
};

std::map<uint32, std::array<uint32, 5>> invKorkronForcesHeroicType =
{
    { 1, { NPC_KORKRON_IRONBLADE, NPC_KORKRON_ARCWEAVER, NPC_KORKRON_ASSASSIN,  0, 0 } },
    { 2, { NPC_KORKRON_ASSASSIN,  NPC_KORKRON_WARSHAMAN, NPC_KORKRON_SNIPER,    0, 0 } },
    { 3, { NPC_KORKRON_WARSHAMAN, NPC_KORKRON_ARCWEAVER, NPC_KORKRON_IRONBLADE, 0, 0 } },
    { 4, { NPC_KORKRON_ASSASSIN,  NPC_KORKRON_ARCWEAVER, NPC_KORKRON_SNIPER,    0, 0 } },
    { 5, { NPC_KORKRON_WARSHAMAN, NPC_KORKRON_IRONBLADE, NPC_KORKRON_ASSASSIN,  0, 0 } },
    { 6, { NPC_KORKRON_SNIPER,    NPC_KORKRON_ARCWEAVER, NPC_KORKRON_WARSHAMAN, 0, 0 } },
    { 7, { NPC_KORKRON_SNIPER,    NPC_KORKRON_ASSASSIN,  NPC_KORKRON_IRONBLADE, 0, 0 } },
    { 8, { NPC_KORKRON_ASSASSIN,  NPC_KORKRON_ARCWEAVER, NPC_KORKRON_WARSHAMAN, 0, 0 } },
    { 9, { NPC_KORKRON_IRONBLADE, NPC_KORKRON_SNIPER,    NPC_KORKRON_WARSHAMAN, 0, 0 } },
    { 10, { NPC_KORKRON_SNIPER,   NPC_KORKRON_ARCWEAVER, NPC_KORKRON_IRONBLADE, 0, 0 } },

    { 11, { NPC_KORKRON_ASSASSIN, NPC_KORKRON_ARCWEAVER, NPC_KORKRON_IRONBLADE, NPC_KORKRON_WARSHAMAN, NPC_KORKRON_SNIPER } }, // additional forces at low hp
};

class boss_general_nazgrim : public CreatureScript
{
    public:
        boss_general_nazgrim() : CreatureScript("boss_general_nazgrim") { }

        struct boss_general_nazgrimAI : public BossAI
        {
            boss_general_nazgrimAI(Creature* creature) : BossAI(creature, DATA_GENERAL_NAZGRIM) 
            {
                me->setActive(true);
                hasIntroDone = false;
            }

            EventMap berserkEvents;
            uint8 phaseId;
            uint32 forcesId;
            ObjectGuid ravagerTargetGUID; // ravager has launch to target at begun.
            ObjectGuid shockwaveTargetGUID;
            ObjectGuid targetGUID;
            bool hasIntroDone;
            bool hasAdditionalForces;

            void Reset() override
            {
                _Reset();

                events.Reset();
                berserkEvents.Reset();
                phaseId = NONE_STANCE;
                ravagerTargetGUID = ObjectGuid::Empty;
                shockwaveTargetGUID = ObjectGuid::Empty;
                forcesId            = 1;
                targetGUID = ObjectGuid::Empty;

                me->SetPowerType(POWER_MANA);
                me->SetMaxPower(POWER_MANA, 100);
                me->SetPower(POWER_MANA, 0);
                DoCast(me, SPELL_ZERO_MANA_REGEN, true);
                SetEquipmentSlots(false, 7612, 0, 0); // default weapon

                hasAdditionalForces = false;

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                scheduler
                    .Schedule(Seconds(1), [this](TaskContext context)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            ObjectGuid GetGUID(int32 type) const override
            {
                switch (type)
                {
                    case GUID_TYPE_RAVAGER:
                        return ravagerTargetGUID;
                    case GUID_TYPE_SHOCKWAVE:
                        return shockwaveTargetGUID;
                }

                return ObjectGuid::Empty;
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                if (instance)
                    instance->SetBossState(DATA_GENERAL_NAZGRIM, FAIL);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                if (instance)
                {
                    Talk(instance->GetData(DATA_GROUP_FACTION) ? TALK_AGGRO_HORDE : TALK_AGGRO_ALLIANCE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }

                events.ScheduleEvent(EVENT_SUNDERING_BLOW, 8.5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BONECRACKER, 18.3 * IN_MILLISECONDS);
                berserkEvents.ScheduleEvent(EVENT_CHANGE_STANCE, 1 * IN_MILLISECONDS);
                berserkEvents.ScheduleEvent(EVENT_CALL_KORKRON, 45 * IN_MILLISECONDS);

                if (IsHeroic())
                    events.ScheduleEvent(EVENT_EXECUTE, 18 * IN_MILLISECONDS);

                // at begun it has timer 22s (but really it waiting till boss got 30 rage power or higher)
                events.ScheduleEvent(EVENT_RAGE_ABILITY, 22 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SPELLS_BASE, 10 * IN_MILLISECONDS); // Should check select spell every 10 seconds or so.
                berserkEvents.ScheduleEvent(EVENT_BERSERK, 10 * MINUTE * IN_MILLISECONDS);
            }

            void SetPhase(uint8 newPhase)
            {
                uint8 oldPhase = phaseId;
                me->RemoveAurasDueToSpell(phaseId);
                phaseId = newPhase;

                switch (phaseId)
                {
                    case BATTLE_STANCE:
                    case BERSERKER_STANCE:
                        SetEquipmentSlots(false, 7612, 0, EQUIP_NO_CHANGE); // Default
                        break;
                    case DEFENSIVE_STANCE:
                          // Todo Find equipments for defesesive stance.
                        break;
                }

                auto key = invStanceType.find(phaseId);

                Talk(key->second[1]);
                DoCast(me, key->second[0]);
                berserkEvents.ScheduleEvent(EVENT_CHANGE_STANCE, 60 * IN_MILLISECONDS);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->SetInCombatWithZone();
            }

            void EnterEvadeMode() override
            {
                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_GENERAL_NAZGRIM, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BONECRACKER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGISTRIKE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ASSASSINS_MARK);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUNDERING_BLOW);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    me->GetMap()->SetWorldState(WORLDSTATE_GAMON_WILL_SAVE_US, 0);
                    me->GetMap()->SetWorldState(WORLDSTATE_GAMON_WILL_SAVE_US_2, 0);

                    if (Creature* gamon = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_GAMON)))
                        gamon->DespawnOrUnsummon();
                }

                // Should remove them at first evade (blizzlike)
                std::list<Creature*> faithfuls;
                GetCreatureListWithEntryInGrid(faithfuls, me, NPC_ORGRIMMAR_FAITHFUL, 200.0f);

                for (auto&& itr : faithfuls)
                    itr->DespawnOrUnsummon();

                berserkEvents.Reset();
                summons.DespawnAll();

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                _DespawnAtEvade();
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_INTRO:
                        if (hasIntroDone || !me->IsAlive())
                            break;

                        hasIntroDone = true;

                        if (instance)
                            Talk(instance->GetData(DATA_GROUP_FACTION) ? TALK_INTRO_HORDE : TALK_INTRO_ALLIANCE);
                        break;
                    case ACTION_COOLING_OFF:
                        events.ScheduleEvent(EVENT_RAGE_ABILITY, 0.5 * IN_MILLISECONDS);
                        break;
                }
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (HealthBelowPct(10) && !hasAdditionalForces)
                {
                    hasAdditionalForces = true;
                    Talk(TALK_LAST_STAND);

                    // Summon Additional Forces
                    if (IsHeroic())
                    {
                        auto key = invKorkronForcesHeroicType.find(11);

                        for (uint8 i = 0; i < 5; i++)
                            me->SummonCreature(key->second[i], nazgrimKorkronSpawnPos[i], TEMPSUMMON_MANUAL_DESPAWN);
                    }
                    else
                    {
                        auto key = invKorkronForcesType.find(11);

                        for (uint8 i = 0; i < 4; i++)
                            me->SummonCreature(key->second[i], nazgrimKorkronSpawnPos[i], TEMPSUMMON_MANUAL_DESPAWN);
                    }
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == EVENT_JUMP)
                {
                    DoCast(me, SPELL_HEROIC_SHOCKWAVE);
                    me->RemoveChanneledCast(targetGUID);
                }
            }

            void KilledUnit(Unit* /*victim*/) override { }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                {
                    uint32 hordeYell = urand(0, 1) ? TALK_DEATH_HORDE_ADDITIONAL : TALK_DEATH_HORDE;
                    Talk(instance->GetData(DATA_GROUP_FACTION) ? hordeYell : TALK_DEATH_ALLIANCE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BONECRACKER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGISTRIKE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ASSASSINS_MARK);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUNDERING_BLOW);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    if (Creature* gamon = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_GAMON)))
                        gamon->AI()->DoAction(ACTION_GAMON_OUTRO);
                }

                berserkEvents.Reset();
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);
                berserkEvents.Update(diff);

                while (uint32 eventId = berserkEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHANGE_STANCE:
                            if (IsHeroic() && me->HasUnitState(UNIT_STATE_CASTING)) // Execute sometimes in this
                            {
                                berserkEvents.RescheduleEvent(EVENT_CHANGE_STANCE, 0.5 * IN_MILLISECONDS);
                                break;
                            }
                            SetPhase(phaseId > BERSERKER_STANCE ? BATTLE_STANCE : phaseId + 1);
                            break;
                        case EVENT_CALL_KORKRON:
                            Talk(TALK_KORKRON_FORCE);
                            HandleCallHelp();
                            berserkEvents.ScheduleEvent(EVENT_CALL_KORKRON, 45 * IN_MILLISECONDS);
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            break;
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BONECRACKER:
                            DoCast(me, SPELL_BONECRACKER);
                            events.ScheduleEvent(EVENT_BONECRACKER, 30 * IN_MILLISECONDS);
                            break;
                        case EVENT_SUNDERING_BLOW:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_SUNDERING_BLOW);

                            events.ScheduleEvent(EVENT_SUNDERING_BLOW, urand(7.6 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                            break;
                        case EVENT_EXECUTE:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_EXECUTE);

                            events.ScheduleEvent(EVENT_EXECUTE, 18 * IN_MILLISECONDS);
                            break;
                        case EVENT_RAGE_ABILITY:
                            if (!HandleSelectSpellByRagePower()) // Reschedule if not enough power for ability
                                events.ScheduleEvent(EVENT_RAGE_ABILITY, 0.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_SPELLS_BASE:
                            HandleSelectSpellByRagePower();
                               events.ScheduleEvent(EVENT_SPELLS_BASE, 10 * IN_MILLISECONDS);
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 75.0f);
            }

            private:
                uint32 HandleSelectSpellByRagePower()
                {
                    uint32 power = me->GetPower(POWER_MANA);

                    // Heroic Shockwave
                    if (power >= 30 && power < 50)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                        {
                            if (Unit* victim = me->GetVictim())
                                targetGUID = victim->GetGUID();

                            me->PrepareChanneledCast(me->GetOrientation());
                            shockwaveTargetGUID = target->GetGUID();
                            DoCast(target, SPELL_HEROIC_SHOCKWAVE_JUMP);

                          // In case: 1 time it was happened...
                            scheduler
                                .Schedule(Milliseconds(4000), [this](TaskContext context)
                            {
                                if (me->HasReactState(REACT_PASSIVE))
                                    me->RemoveChanneledCast(targetGUID);
                            });
                        }
                        else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        {
                            if (Unit* victim = me->GetVictim())
                                targetGUID = victim->GetGUID();

                            me->PrepareChanneledCast(me->GetOrientation());
                            shockwaveTargetGUID = target->GetGUID();
                            DoCast(target, SPELL_HEROIC_SHOCKWAVE_JUMP);

                            // In case: 1 time it was happened...
                            scheduler
                                .Schedule(Milliseconds(4000), [this](TaskContext context)
                            {
                                if (me->HasReactState(REACT_PASSIVE))
                                    me->RemoveChanneledCast(targetGUID);

                            });
                        }

                        // 15.5s CD on rage Ability
                        DoCast(me, SPELL_COOLING_OFF, true);

                        return 1;
                    }

                    // Kork`ron Banner
                    if (power >= 50 && power < 70)
                    {
                        // 15.5s CD on rage Ability
                        DoCast(me, SPELL_COOLING_OFF, true);

                        DoCast(me, SPELL_KORKRON_BANNER);
                        return 1;
                    }

                    // Warsong
                    if (power >= 70 && power < 100)
                    {
                        // 15.5s CD on rage Ability
                        DoCast(me, SPELL_COOLING_OFF, true);

                        DoCast(me, SPELL_WAR_SONG);
                        return 1;
                    }

                    // Ravager
                    if (power >= 100)
                    {
                        // 15.5s CD on rage Ability
                        DoCast(me, SPELL_COOLING_OFF, true);

                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                        {
                            ravagerTargetGUID = target->GetGUID();
                            DoCast(target, SPELL_RAVAGER);
                        }
                        else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        {
                            ravagerTargetGUID = target->GetGUID();
                            DoCast(target, SPELL_RAVAGER);
                        }

                        return 1;
                    }

                    return 0;
                }

                void HandleCallHelp()
                {
                    // Only ten waves + additional at 10% percent of health
                    if (forcesId > 10)
                        return;

                    // Select some 2 spawn positions
                    std::vector<uint32> korkronPosList = { 0, 1, 2, 3, 4 };
                    std::random_device rd;
                    std::mt19937 g(rd());
                    std::shuffle(korkronPosList.begin(), korkronPosList.end(), g);                    
                    korkronPosList.resize(4);

                    if (IsHeroic())
                    {
                        auto forcesKey = invKorkronForcesHeroicType.find(forcesId);
                        for (uint8 i = 0; i < korkronPosList.size(); i++)
                            me->SummonCreature(forcesKey->second[i], nazgrimKorkronSpawnPos[korkronPosList[i]], TEMPSUMMON_MANUAL_DESPAWN);
                    }
                    else
                    {
                        auto forcesKey = invKorkronForcesType.find(forcesId);
                        for (uint8 i = 0; i < korkronPosList.size(); i++)
                            me->SummonCreature(forcesKey->second[i], nazgrimKorkronSpawnPos[korkronPosList[i]], TEMPSUMMON_MANUAL_DESPAWN);
                    }

                    forcesId++;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_general_nazgrimAI(creature);
        }
};

// Orgrimmar Faithful 71715
struct npc_orgrimmar_faithful : public ScriptedAI
{
    npc_orgrimmar_faithful(Creature* creature) : ScriptedAI(creature)
    {
        instance = creature->GetInstanceScript();
    }

    InstanceScript* instance;
    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->CallForHelp(15.0f);
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Kork`ron Ironblade 71770
struct npc_nazgrim_korkron_ironblade : public ScriptedAI
{
    npc_nazgrim_korkron_ironblade(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap events;
    bool hasTriggered;

    void Reset() override
    {
        events.Reset();
        scheduler.CancelAll();
        hasTriggered = false;

        if (!me->GetDBTableGUIDLow())
            return;

        scheduler
            .Schedule(Milliseconds(1), [this](TaskContext context)
        {
            std::vector<uint32> emoteType = { EMOTE_ONESHOT_LAUGH, EMOTE_ONESHOT_TALK, EMOTE_ONESHOT_ROAR, EMOTE_ONESHOT_CHEER };
            me->HandleEmoteCommand(Trinity::Containers::SelectRandomContainerElement(emoteType));

            context.Repeat(Seconds(4));
        });
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        scheduler.CancelAll();
        uint32 timer = me->GetDBTableGUIDLow() ? 1 * IN_MILLISECONDS : urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS);
        scheduler
            .Schedule(Milliseconds(timer), [this](TaskContext context)
        {
            if (!me->HasAura(SPELL_IRON_STORM))
                DoCast(me, SPELL_IRON_STORM);

            context.Repeat(Seconds(8));
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (!me->GetDBTableGUIDLow())
            me->SetLootRecipient(NULL);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(51) && !hasTriggered)
        {
            hasTriggered = true;
            me->InterruptNonMeleeSpells(true, SPELL_IRON_STORM);
            events.ScheduleEvent(EVENT_LAST_STAND, 0.5 * IN_MILLISECONDS);
        }
    }

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
            if (eventId == EVENT_LAST_STAND)
                DoCast(me, SPELL_LAST_STAND);
            break;
        }

        if (!me->HasAura(SPELL_IRON_STORM))
            DoMeleeAttackIfReady();
    }
};

// Kork`ron Arcweaver 71771
struct npc_nazgrim_korkron_arcweaver : public ScriptedAI
{
    npc_nazgrim_korkron_arcweaver(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void AttackStart(Unit* target) override
    {
        if (!target)
            return;

        if (me->Attack(target, false))
            DoStartNoMovement(target);
    }

    void CastInterrupted(SpellInfo const* spell) override
    {
        events.RescheduleEvent(EVENT_ARCANE_SHOCK, 6 * IN_MILLISECONDS);
        events.RescheduleEvent(EVENT_MAGISTRIKE, 7 * IN_MILLISECONDS);

        // Blink on Interrupt
        if (urand(0, 1))
            events.ScheduleEvent(EVENT_BLINK, 1.5 * IN_MILLISECONDS);

        if (Unit* vict = me->GetVictim())
            me->GetMotionMaster()->MoveChase(vict);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (!me->GetDBTableGUIDLow())
            me->SetLootRecipient(NULL);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_ARCANE_SHOCK, 2 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_MAGISTRIKE, 8.5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_ARCANE_SHOCK:
                    me->StopMoving();
                    me->GetMotionMaster()->MovementExpired();

                    if (Unit* vict = me->GetVictim())
                    {
                        if (me->GetExactDist2d(vict) > 48.0f)
                            DoStartMovement(vict);
                        else
                            DoStartNoMovement(vict);
                    }

                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                        DoCast(target, SPELL_ARCANE_SHOCK);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        DoCast(target, SPELL_ARCANE_SHOCK);

                    events.ScheduleEvent(EVENT_ARCANE_SHOCK, urand(2 * IN_MILLISECONDS, 2.1 * IN_MILLISECONDS));
                    break;
                case EVENT_MAGISTRIKE:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                        DoCast(target, SPELL_MAGISTRIKE);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        DoCast(target, SPELL_MAGISTRIKE);

                    events.ScheduleEvent(EVENT_MAGISTRIKE, urand(9 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                    break;
                case EVENT_BLINK:
                    me->SetFacingTo(me->GetAngle(nazgrimCenterPos.GetPositionX(), nazgrimCenterPos.GetPositionY()));
                    DoCast(me, SPELL_UNSTABLE_BLINK, true);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Kork`ron Assassin 71772
struct npc_nazgrim_korkron_assassin : public ScriptedAI
{
    npc_nazgrim_korkron_assassin(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid markGUID;

    void Reset() override
    {
        markGUID = ObjectGuid::Empty;
        DoCast(me, me->GetDBTableGUIDLow() ? SPELL_STEALTH : SPELL_ASSASSINS_MARK, true);
        me->ClearUnitState(UNIT_STATE_CASTING);
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        markGUID = guid;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, me->GetDBTableGUIDLow() ? SPELL_ASSASSINS_MARK : SPELL_STEALTH, true);
        me->ClearUnitState(UNIT_STATE_CASTING);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Unit* target = ObjectAccessor::GetUnit(*me, markGUID))
            target->RemoveAurasDueToSpell(SPELL_ASSASSINS_MARK);

        if (!me->GetDBTableGUIDLow())
            me->SetLootRecipient(NULL);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && me->IsAlive() && HealthAbovePct(5))
        {
            DoCast(me, SPELL_ASSASSINS_MARK);
            me->ClearUnitState(UNIT_STATE_CASTING);
        }
    }

    void DamageDealt(Unit* victim, uint32& /*damage*/, DamageEffectType damageType) override
    {
        if (victim && victim->isInBack(me) && damageType == DIRECT_DAMAGE)
            DoCast(victim, SPELL_BACKSTAB, true);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictimWithGaze())
            return;

        DoMeleeAttackIfReady();
    }
};

// Kork`ron Warshaman 71773
struct npc_nazgrim_korkron_warshaman : public ScriptedAI
{
    npc_nazgrim_korkron_warshaman(Creature* creature) : ScriptedAI(creature)
    {
        instance = creature->GetInstanceScript();
    }

    TaskScheduler scheduler;
    InstanceScript* instance;
    EventMap events;

    void Reset() override
    {
        events.Reset();

        if (me->GetDBTableGUIDLow()) // on bridge, visual
            DoCast(me, SPELL_FIRE_CHANNELING);
    }

    ObjectGuid GetLowestFriendGUID()
    {
        std::list<Creature*> tmpTargets;

        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_GENERAL_NAZGRIM, 40.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_KORKRON_IRONBLADE, 40.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_KORKRON_ARCWEAVER, 40.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_KORKRON_ASSASSIN, 40.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_KORKRON_WARSHAMAN, 40.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_KORKRON_SNIPER, 40.0f);
        tmpTargets.remove_if([=](Creature* target) { return target && !target->IsAlive(); });

        if (tmpTargets.empty())
            return ObjectGuid::Empty;

        tmpTargets.sort(Trinity::HealthPctOrderPred());

        if (Creature* lowestTarget = tmpTargets.front())
            return lowestTarget->GetGUID();

        return ObjectGuid::Empty;
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (!me->GetDBTableGUIDLow())
            me->SetLootRecipient(NULL);
    }

    void CastInterrupted(SpellInfo const* spell) override
    {
        events.RescheduleEvent(EVENT_CHAIN_HEAL, 6 * IN_MILLISECONDS);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->InterruptNonMeleeSpells(false, SPELL_FIRE_CHANNELING);
        events.ScheduleEvent(EVENT_CHAIN_HEAL, 5.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_EARTH_SHIELD, 8.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_HEALING_TIDE, 14.5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CHAIN_HEAL:
                    if (Unit* target = ObjectAccessor::GetUnit(*me, GetLowestFriendGUID()))
                        DoCast(target, SPELL_EMPOWERED_CHAIN_HEAL);

                    events.ScheduleEvent(EVENT_CHAIN_HEAL, 2 * IN_MILLISECONDS);
                    break;
                case EVENT_EARTH_SHIELD:
                    if (Unit* target = ObjectAccessor::GetUnit(*me, GetLowestFriendGUID()))
                        DoCast(target, SPELL_EARTH_SHIELD);

                    events.ScheduleEvent(EVENT_EARTH_SHIELD, 12.5 * IN_MILLISECONDS);
                    break;
                case EVENT_HEALING_TIDE:
                    DoCast(me, SPELL_HEALING_TIDE_TOTEM);
                    events.ScheduleEvent(EVENT_HEALING_TIDE, 18.5 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Kork`ron Sniper 71656
struct npc_nazgrim_korkron_sniper : public ScriptedAI
{
    npc_nazgrim_korkron_sniper(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap events;
    ObjectGuid markGUID;

    void Reset() override
    {
        markGUID = ObjectGuid::Empty;
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        markGUID = guid;
    }

    ObjectGuid GetGUID(int32 type) const override
    {
        return markGUID;
    }

    void AttackStart(Unit* target) override
    {
        if (!target)
            return;

        if (me->Attack(target, false))
            DoStartNoMovement(target);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, SPELL_HUNTERS_MARK, true);
        me->ClearUnitState(UNIT_STATE_CASTING);

        events.ScheduleEvent(EVENT_SHOT, 1.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_MULTY_SHOT, 11 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Unit* target = ObjectAccessor::GetUnit(*me, markGUID))
            target->RemoveAurasDueToSpell(SPELL_HUNTERS_MARK);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && me->IsAlive() && HealthAbovePct(5))
        {
            DoCast(me, SPELL_HUNTERS_MARK);
            me->ClearUnitState(UNIT_STATE_CASTING);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictimWithGaze())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_MULTY_SHOT:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_MULTI_SHOT);

                    events.ScheduleEvent(EVENT_MULTY_SHOT, 11 * IN_MILLISECONDS);
                    break;
                case EVENT_SHOT:
                    me->StopMoving();
                    me->GetMotionMaster()->MovementExpired();

                    if (Unit* vict = me->GetVictim())
                    {
                        if (me->GetExactDist2d(vict) > 20.0f)
                            DoStartMovement(vict);
                        else
                            DoStartNoMovement(vict);
                    }

                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SHOOT); // 30y range

                    events.ScheduleEvent(EVENT_SHOT, 2.1 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Kork`ron Banner 71626
struct npc_nazgrim_korkron_banner : public ScriptedAI
{
    npc_nazgrim_korkron_banner(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->SetReactState(REACT_PASSIVE);
        DoCast(me, SPELL_KORKRON_BANNER_VISUAL, true);
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->DisappearAndDie();
    }
};

// Healing Tide Totem 71610
struct npc_nazgrim_korkron_healing_tide_totem : public ScriptedAI
{
    npc_nazgrim_korkron_healing_tide_totem(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);

        if (Creature* nazgrim = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GENERAL_NAZGRIM) : ObjectGuid::Empty))
            nazgrim->AI()->JustSummoned(me);

        me->SetReactState(REACT_PASSIVE);
        DoCast(me, SPELL_HEALING_TIDE, true);
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->DisappearAndDie();
    }
};

// Heroic Shockwave 71697
struct npc_nazgrim_heroic_shockwave : public ScriptedAI
{
    npc_nazgrim_heroic_shockwave(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        me->DespawnOrUnsummon(4.5 * IN_MILLISECONDS);

        scheduler
            .Schedule(Milliseconds(500), [this](TaskContext context)
        {
            if (Creature* nazgrim = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GENERAL_NAZGRIM) : ObjectGuid::Empty))
            {
                nazgrim->AI()->JustSummoned(me);

                if (Unit* target = ObjectAccessor::GetUnit(*me, nazgrim->AI()->GetGUID(GUID_TYPE_SHOCKWAVE)))
                    me->PrepareChanneledCast(me->GetAngle(target), SPELL_AFRTERSHOCK);
            }
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Ravager 71762
struct npc_nazgrim_ravager : public ScriptedAI
{
    npc_nazgrim_ravager(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    bool hasLaunch;

    void Reset() override
    {
        hasLaunch = false;
        DoCast(me, SPELL_RAVAGER_AURA, true);

        scheduler
            .Schedule(Milliseconds(500), [this](TaskContext context)
        {
            if (!hasLaunch)
            {
                hasLaunch = true;

                // If found launcher target
                if (Creature* nazgrim = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GENERAL_NAZGRIM) : ObjectGuid::Empty))
                {
                    if (Unit* target = ObjectAccessor::GetUnit(*me, nazgrim->AI()->GetGUID(GUID_TYPE_RAVAGER)))
                    {
                        me->GetMotionMaster()->MovePoint(0, *target);
                    }
                    else
                        GenerateMovement();

                }
            }
            else
                GenerateMovement();

            context.Repeat(Milliseconds(me->GetSplineDuration()));
        });
    }

    void GenerateMovement()
    {
        Position pos = me->GetRandomPoint(nazgrimCenterPos, 40.0f);
        me->GetMotionMaster()->MovePoint(0, pos);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Ravager Eff 143873
class spell_ravager_eff : public SpellScript
{
    PrepareSpellScript(spell_ravager_eff);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets; // save players who got damage from it
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ravager_eff::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ravager_eff::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Sundering Blow 143494
class spell_nazgrim_sundering_blow : public SpellScript
{
    PrepareSpellScript(spell_nazgrim_sundering_blow);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* target = GetHitUnit())
        {
            uint8 mod = GetCaster()->HasAura(SPELL_BERSERKER_STANCE) ? 2 : 1;

            int32 additionalPower = mod > 1 ? 5 : 0;
            if (Aura* sunBlow = target->GetAura(GetSpellInfo()->Id))
                GetCaster()->SetPower(POWER_MANA, GetCaster()->GetPower(POWER_MANA) + mod * 5 * (sunBlow->GetStackAmount() - 1) + additionalPower);
        }
    }

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_nazgrim_sundering_blow::HandleHit, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
    }
};

// Bonecraker 143638
class spell_nazgrim_bonecracker : public SpellScript
{
    PrepareSpellScript(spell_nazgrim_bonecracker);

    std::list<WorldObject*> m_targets, copyTargets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // Try not select targets affected by bonecracker if it possible
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_BONECRACKER); });

        uint8 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 5 : 2;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 2; // min 2, max 5
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 13)
                targetsCount++;

            if (totalPlayersCount > 17)
                targetsCount++;

            if (totalPlayersCount > 21)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() >= targetsCount) // doesn`t matter count - MaxAffect is 2(5)
        {
            copyTargets = targets;
            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 2; // min 2, max 5
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 13)
                targetsCount++;

            if (totalPlayersCount > 17)
                targetsCount++;

            if (totalPlayersCount > 21)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nazgrim_bonecracker::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nazgrim_bonecracker::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

class AftershockDistanceOrderPred
{
    public:
        AftershockDistanceOrderPred(const WorldObject* object, bool ascending = true) : m_object(object), m_ascending(ascending) {}
        bool operator() (const WorldObject* a, const WorldObject* b) const
        {
            return m_ascending ? a->GetDistance(m_object) < b->GetDistance(m_object) :
                a->GetDistance(m_object) > b->GetDistance(m_object);
        }
    private:
        const WorldObject* m_object;
        const bool m_ascending;
};

// Heroic Shockwave Aftershock 143713
class spell_nazgrim_heroic_shockwave_aftershock : public SpellScript
{
    PrepareSpellScript(spell_nazgrim_heroic_shockwave_aftershock);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        // exclude owner of shockwave cuz all triggers will facing to him (across)
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && GetCaster() && GetCaster()->ToCreature() && target->ToPlayer()->GetGUID() == GetCaster()->ToCreature()->AI()->GetGUID(GUID_TYPE_SHOCKWAVE); });
        targets.sort(AftershockDistanceOrderPred(GetCaster(), true));

        uint8 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 5 : 2;

        if (targets.size() > targetsCount) // doesn`t matter count - MaxAffect is 2(5)
        {
            targets.resize(targetsCount);
            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nazgrim_heroic_shockwave_aftershock::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Nazgrim Energize 143877, 143876, 143590
class spell_nazgrim_energize : public SpellScript
{
    PrepareSpellScript(spell_nazgrim_energize);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (GetCaster() && GetCaster()->HasAura(SPELL_BERSERKER_STANCE))
            GetCaster()->SetPower(POWER_MANA, GetCaster()->GetPower(POWER_MANA) + sSpellMgr->GetSpellInfo(GetSpellInfo()->Id, GetCaster()->GetMap()->GetDifficulty())->Effects[0].BasePoints);
    }

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_nazgrim_energize::HandleHit, EFFECT_0, SPELL_EFFECT_ENERGIZE);
    }
};

// Ironstorm 143420
class spell_nazgrim_ironstorm : public AuraScript
{
    PrepareAuraScript(spell_nazgrim_ironstorm);

    void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->ClearUnitState(UNIT_STATE_CASTING);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_nazgrim_ironstorm::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Assassins Mark 143480
class spell_nazgrim_assassing_mark : public SpellScript
{
    PrepareSpellScript(spell_nazgrim_assassing_mark);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // Try not select targets with tank or melee spec and already affected by mark
        targets.remove_if(MeeleSpecTargetSelector());
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_ASSASSINS_MARK); });

        if (targets.size() >= 1) // doesn`t matter count - MaxAffect is 1
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nazgrim_assassing_mark::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Assassins Mark 143480
class spell_nazgrim_assassing_mark_aura : public AuraScript
{
    PrepareAuraScript(spell_nazgrim_assassing_mark_aura);

    void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetCaster() && GetCaster()->ToCreature())
        {
            if (Unit* owner = GetOwner()->ToUnit())
            {
                GetCaster()->ToCreature()->AI()->SetGUID(owner->GetGUID());
                GetCaster()->ToCreature()->AI()->AttackStart(owner);
                GetCaster()->ToCreature()->SetReactState(REACT_PASSIVE);
            }
        }
    }

    void HandleOnRemove(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetCaster() && GetCaster()->ToCreature())
            GetCaster()->ToCreature()->AI()->DoAction(ACTION_START_INTRO);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_nazgrim_assassing_mark_aura::HandleOnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_nazgrim_assassing_mark_aura::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Cooling Off 143484
class spell_nazgrim_cooling_off : public AuraScript
{
    PrepareAuraScript(spell_nazgrim_cooling_off);

    void HandleOnRemove(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_COOLING_OFF);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_nazgrim_cooling_off::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Defensive Stance 143593
class spell_nazgrim_defensive_stance : public AuraScript
{
    PrepareAuraScript(spell_nazgrim_defensive_stance);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetActor() && !eventInfo.GetActor()->HasAura(SPELL_SUNDERING_BLOW);
    }

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        if (Unit* attacker = eventInfo.GetActor())
        {
            attacker->CastSpell(attacker, SPELL_GENERATE_RAGE, true);
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_nazgrim_defensive_stance::CheckProc);
        OnProc += AuraProcFn(spell_nazgrim_defensive_stance::HandleOnProc);
    }
};

// Kork`ron Banner 143535
class spell_nazgrim_korkron_banner : public SpellScript
{
    PrepareSpellScript(spell_nazgrim_korkron_banner);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_KORKRON_IRONBLADE && target->GetEntry() != NPC_KORKRON_ARCWEAVER && target->GetEntry() != NPC_KORKRON_ASSASSIN && target->GetEntry() != NPC_KORKRON_WARSHAMAN && target->GetEntry() != NPC_KORKRON_SNIPER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nazgrim_korkron_banner::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Healing Tide eff 143478
class spell_nazgrim_healing_tide_eff : public SpellScript
{
    PrepareSpellScript(spell_nazgrim_healing_tide_eff);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_KORKRON_IRONBLADE && target->GetEntry() != NPC_KORKRON_ARCWEAVER && target->GetEntry() != NPC_KORKRON_ASSASSIN && target->GetEntry() != NPC_KORKRON_WARSHAMAN && target->GetEntry() != NPC_GENERAL_NAZGRIM && target->GetEntry() != NPC_KORKRON_SNIPER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nazgrim_healing_tide_eff::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Heroic Shockwave 143500
class spell_nazgrim_heroic_shockwave : public SpellScript
{
    PrepareSpellScript(spell_nazgrim_heroic_shockwave);

    void HandleHit(SpellEffIndex effIndex)
    {
        /*if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(caster, SPELL_HEROIC_SHOCKWAVE, true);*/
    }

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_nazgrim_heroic_shockwave::HandleHit, EFFECT_0, SPELL_EFFECT_JUMP);
    }
};

// TARGET_UNIT_CONE_UNK_130 working wrong
class AftershockPredicate 
{
    public:
        AftershockPredicate(Unit* const m_caster) : _caster(m_caster) { }

        bool operator()(WorldObject* object)
        {
            Player* target = object->ToPlayer();

            if (!target)
                return true;

            x = _caster->GetPositionX() + (56.0f * cos(_caster->GetOrientation()));
            y = _caster->GetPositionY() + (56.0f * sin(_caster->GetOrientation()));

            if (!_caster->HasInArc(M_PI, target))
                return true;

            width = target->GetObjectSize() + 1.2f;
            float angle = _caster->GetRelativeAngle(target);

            return fabs(sin(angle)) * _caster->GetExactDist2d(target->GetPositionX(), target->GetPositionY()) >= width;
        }
    
    private:
        Unit const* _caster;
        float x, y;
        float width;
};

// Afterhock 143712
class spell_nazgrim_aftershock : public SpellScript
{
    PrepareSpellScript(spell_nazgrim_aftershock);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if(AftershockPredicate(GetCaster()));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nazgrim_aftershock::SelectTargets, EFFECT_0, TARGET_UNIT_CONE_UNK_130);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nazgrim_aftershock::SelectTargets, EFFECT_1, TARGET_UNIT_CONE_UNK_130);
    }
};

// Hunter`s Mark 143882
class spell_nazgrim_hunters_mark : public SpellScript
{
    PrepareSpellScript(spell_nazgrim_hunters_mark);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // Try not select targets only with healer spec
        targets.remove_if(DpsSpecTargetSelector());
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_HUNTERS_MARK); });

        if (targets.size() >= 1) // doesn`t matter count - MaxAffect is 1
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_nazgrim_hunters_mark::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Hunter`s Mark 143882
class spell_nazgrim_hunters_mark_aura : public AuraScript
{
    PrepareAuraScript(spell_nazgrim_hunters_mark_aura);

    void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetCaster() && GetCaster()->ToCreature())
        {
            if (Unit* owner = GetOwner()->ToUnit())
            {
                GetCaster()->ToCreature()->AI()->SetGUID(owner->GetGUID());
                GetCaster()->ToCreature()->AI()->AttackStart(owner);
                GetCaster()->ToCreature()->SetReactState(REACT_PASSIVE);
            }
        }
    }

    void HandleOnRemove(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetCaster() && GetCaster()->ToCreature())
            GetCaster()->ToCreature()->AI()->DoAction(ACTION_START_INTRO);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_nazgrim_hunters_mark_aura::HandleOnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_nazgrim_hunters_mark_aura::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Multi Shot 143887
class spell_nazgrim_multi_shot : public SpellScript
{
    PrepareSpellScript(spell_nazgrim_multi_shot);

    void HandleAfterCast()
    {
        // Visual
        if (Creature* caster = GetCaster()->ToCreature())
            if (Unit* mainTarget = caster->GetVictim())
                caster->CastSpell(mainTarget, SPELL_VOLLEY_VISUAL, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_nazgrim_multi_shot::HandleAfterCast);
    }
};

// Areatrigger 9440
class AreaTrigger_at_soo_leftside_nazgrim : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_leftside_nazgrim() : AreaTriggerScript("AreaTrigger_at_soo_leftside_nazgrim") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* nazgrim = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(DATA_GENERAL_NAZGRIM) : ObjectGuid::Empty))
                nazgrim->AI()->DoAction(ACTION_START_INTRO);

            return true;
        }
};

void AddSC_general_nazgrim()
{
    new boss_general_nazgrim();

    new creature_script<npc_orgrimmar_faithful>("npc_orgrimmar_faithful");
    new creature_script<npc_nazgrim_korkron_ironblade>("npc_nazgrim_korkron_ironblade");
    new creature_script<npc_nazgrim_korkron_arcweaver>("npc_nazgrim_korkron_arcweaver");
    new creature_script<npc_nazgrim_korkron_assassin>("npc_nazgrim_korkron_assassin");
    new creature_script<npc_nazgrim_korkron_warshaman>("npc_nazgrim_korkron_warshaman");
    new creature_script<npc_nazgrim_korkron_sniper>("npc_nazgrim_korkron_sniper");
    new creature_script<npc_nazgrim_korkron_banner>("npc_nazgrim_korkron_banner");
    new creature_script<npc_nazgrim_korkron_healing_tide_totem>("npc_nazgrim_korkron_healing_tide_totem");
    new creature_script<npc_nazgrim_heroic_shockwave>("npc_nazgrim_heroic_shockwave");
    new creature_script<npc_nazgrim_ravager>("npc_nazgrim_ravager");

    new spell_script<spell_ravager_eff>("spell_ravager_eff");
    new spell_script<spell_nazgrim_sundering_blow>("spell_nazgrim_sundering_blow");
    new spell_script<spell_nazgrim_bonecracker>("spell_nazgrim_bonecracker");
    new spell_script<spell_nazgrim_heroic_shockwave_aftershock>("spell_nazgrim_heroic_shockwave_aftershock");
    new spell_script<spell_nazgrim_energize>("spell_nazgrim_energize");
    new aura_script<spell_nazgrim_ironstorm>("spell_nazgrim_ironstorm");
    new spell_script<spell_nazgrim_assassing_mark>("spell_nazgrim_assassing_mark");
    new aura_script<spell_nazgrim_assassing_mark_aura>("spell_nazgrim_assassing_mark_aura");
    new aura_script<spell_nazgrim_cooling_off>("spell_nazgrim_cooling_off");
    new aura_script<spell_nazgrim_defensive_stance>("spell_nazgrim_defensive_stance");
    new spell_script<spell_nazgrim_korkron_banner>("spell_nazgrim_korkron_banner");
    new spell_script<spell_nazgrim_healing_tide_eff>("spell_nazgrim_healing_tide_eff");
    new spell_script<spell_nazgrim_heroic_shockwave>("spell_nazgrim_heroic_shockwave");
    new spell_script<spell_nazgrim_aftershock>("spell_nazgrim_aftershock");
    new spell_script<spell_nazgrim_hunters_mark>("spell_nazgrim_hunters_mark");
    new aura_script<spell_nazgrim_hunters_mark_aura>("spell_nazgrim_hunters_mark_aura");
    new spell_script<spell_nazgrim_multi_shot>("spell_nazgrim_multi_shot");
    new AreaTrigger_at_soo_leftside_nazgrim();
};
