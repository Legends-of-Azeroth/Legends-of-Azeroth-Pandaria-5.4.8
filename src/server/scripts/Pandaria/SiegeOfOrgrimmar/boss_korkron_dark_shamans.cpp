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
    /*Haromm*/                 
    SPELL_FROSTSTORM_STRIKE    = 144215,
    SPELL_TOXIC_MIST           = 144089,
    SPELL_TOXICITY             = 144107,
    SPEll_FOUL_STREAM          = 144090,
    SPELL_ASHEN_WALL           = 144070,
    SPELL_ASHEN_WALL_SUMM      = 144071,
                               
    /*Kardris*/                
    SPELL_FROSTSTORM_BOLT      = 144214,
    SPELL_FOUL_GEYSER          = 143990,
    SPELL_FOUL_GEYSER_MISSLE   = 143992,
    SPELL_FOUL_GEYSER_EFF      = 143993,
    SPELL_TOXIC_STORM          = 144005,
    SPELL_TOXIC_STORM_AURA     = 144006,
    SPELL_TOXIC_STORM_EFF      = 144017,
    SPELL_TOXIC_STORM_VORTEX   = 144019,
    SPELL_TOXIC_TORNADO        = 144029,
    SPELL_TOXIC_TORNADO_EFF    = 144030,
    SPELL_FALLING_ASH          = 143973,
    SPELL_FALLING_ASH_COSMETIC = 143986,
    SPELL_FALLING_ASH_EFF      = 143987,
    SPELL_FIRE_BLOSSOM         = 147819, // Wrong animation, but same radius.

    // Misc
    SPELL_SPIRIT_LINK_HAROMM   = 144227,
    SPELL_SPIRIT_LINK_KARDRIS  = 144226,
    SPELL_SHARED_HEALTH        = 144224,
    SPELL_BLOODLUST            = 144302,
    SPEll_BERSERK              = 47008,
    SPELL_SWIPE                = 144303,
    SPELL_REND                 = 144304,
    SPELL_FOULNESS             = 144064,
    SPELL_FOULNESS_EFF         = 144066,

    // Totems
    SPEll_FOULSTREAM_TOTEM     = 144289,
    SPEll_POISONMIST_TOTEM     = 144288,
    SPEll_RUSTED_IRON_TOTEM    = 144291,
    SPEll_ASHFLAR_TOTEM        = 144290,

    // Heroic
    SPELL_IRON_TOMB_SELECTOR   = 144328,
    SPELL_IRON_TOMB_SUMM       = 144329,
    SPELL_IRON_TOMB_EFF        = 144334,
    SPELL_IRON_PRISON          = 144330,
    SPELL_IRON_PRISON_EFF      = 144331,
    SPELL_RUSTED_IRON_TOTEM    = 144291,
};

enum Events
{
    EVENT_FROSTSTORM_BOLT = 1,
    EVENT_FROSTSTORM_STRIKE,
    EVENT_FOUL_GEYSER,
    EVENT_TOXIC_MIST,
    EVENT_TOXIC_STORM,
    EVENT_FALLING_ASH,
    EVENT_FOUL_STREAM,
    EVENT_ASHEN_WALL,
    EVENT_SWIPE,
    EVENT_REND,
    EVENT_BERSERK,
    EVENT_IRON_TOMB,
    EVENT_IRON_PRISON,
};

enum Yells
{
    TALK_AGGRO,
    TALK_POISONMIST_TOTEM_ANN,
    TALK_KARDRIS_POISONMIST_TOTEM,
    TALK_HAROMM_FOULSTREAM_TOTEM_ANN = 2,
    TALK_KARDRIS_FOULSTREAM_TOTEM_ANN,
    TALK_HAROMM_FOULSTREAM_TOTEM = 3,
    TALK_ASHFLARE_TOTEM_ANN,
    TALK_KARDRIS_ASHFLARE_TOTEM,
    TALK_HAROMM_BLOODLUST = 5,
    TALK_KARDRIS_BLOODLUST,
    TALK_HAROMM_BLOODLUST_ANN = 6,
    TALK_KARDRIS_BLOODLUST_ANN,
    TALK_HAROMM_DEAD = 7,
    TALK_KARDRIS_DEAD,
    TALK_HAROM_RUSTED_IRON_TOTEM_ANN = 8,
    TALK_HAROM_RUSTED_IRON_TOTEM,
};

const std::map<uint32, std::array<uint32, 2>> invWolfType =
{
    { NPC_EARTHBREAKER_HAROMM, { NPC_DARKFANG,  0 } },
    { NPC_WAVEBINDER_KARDIS,   { NPC_BLOODCLAW, 1 } },
};

const std::map<uint32, std::array<uint32, 8>> invTotemsType =
{
    { 85, { SPEll_POISONMIST_TOTEM, EVENT_TOXIC_MIST,  EVENT_TOXIC_STORM, 1000, TALK_POISONMIST_TOTEM_ANN,        TALK_POISONMIST_TOTEM_ANN,         0,                            TALK_KARDRIS_POISONMIST_TOTEM } },
    { 65, { SPEll_FOULSTREAM_TOTEM, EVENT_FOUL_STREAM, EVENT_FOUL_GEYSER, 1000, TALK_HAROMM_FOULSTREAM_TOTEM_ANN, TALK_KARDRIS_FOULSTREAM_TOTEM_ANN, TALK_HAROMM_FOULSTREAM_TOTEM, 0                             } },
    { 50, { SPEll_ASHFLAR_TOTEM,    EVENT_ASHEN_WALL,  EVENT_FALLING_ASH, 1000, TALK_ASHFLARE_TOTEM_ANN,          TALK_ASHFLARE_TOTEM_ANN,           0,                            TALK_KARDRIS_ASHFLARE_TOTEM   } },
    { 25, { SPELL_BLOODLUST,        0,                 0,                 0,    TALK_HAROMM_BLOODLUST_ANN,        TALK_HAROMM_BLOODLUST_ANN,         TALK_HAROMM_BLOODLUST,        TALK_KARDRIS_BLOODLUST        } },
};

class boss_earthbreaker_haromm : public CreatureScript
{
    public:
        boss_earthbreaker_haromm() : CreatureScript("boss_earthbreaker_haromm") { }

        struct boss_earthbreaker_harommAI : public BossAI
        {
            boss_earthbreaker_harommAI(Creature* creature) : BossAI(creature, DATA_KORKRON_DARK_SHAMANS)  
            {
                me->setActive(true);
            }

            EventMap berserkerEvents;
            bool atEvade;
            uint32 nextHealthTotem;
            uint32 healthShift;
            bool bloodLust;
            bool ironTotem;
            bool hasDie;

            void Reset() override
            {
                _Reset();

                events.Reset();
                berserkerEvents.Reset();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->SetReactState(REACT_DEFENSIVE);

                atEvade   = false;
                bloodLust = false;
                ironTotem = false;
                hasDie    = false;

                nextHealthTotem = 85;
                healthShift = 1;

                // Heroic
                if (IsHeroic())
                {
                    me->SetTotalAuraEffectValue(SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK, 1.35f);
                    me->UpdateCastingSpeed();
                    me->UpdateHasteRegen();
                }

                scheduler
                    .Schedule(Milliseconds(1500), [this](TaskContext context)
                {
                    if (Creature* wolf = me->SummonCreature(invWolfType.find(me->GetEntry())->second[0], *me, TEMPSUMMON_MANUAL_DESPAWN))
                        me->CastSpell(wolf, VEHICLE_SPELL_RIDE_HARDCODED, true);
                });

                scheduler
                    .Schedule(Seconds(2), [this](TaskContext context)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                if (instance)
                    instance->SetBossState(DATA_KORKRON_DARK_SHAMANS, FAIL);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(TALK_AGGRO);

                scheduler
                    .Schedule(Milliseconds(3000), [this](TaskContext context)
                {
                    if (Creature* kardris = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_WAVEBINDER_KARDIS)))
                        kardris->AI()->Talk(TALK_AGGRO);
                });

                _JustEngagedWith();

                me->SetReactState(REACT_AGGRESSIVE);

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                    if (Creature* kardris = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_WAVEBINDER_KARDIS)))
                        kardris->SetInCombatWithZone();
                }

                me->ExitVehicle();

                events.ScheduleEvent(EVENT_FROSTSTORM_STRIKE, 5.5 * IN_MILLISECONDS);
                berserkerEvents.ScheduleEvent(EVENT_BERSERK, 9 * MINUTE * IN_MILLISECONDS);
            }

            void EnterEvadeMode() override
            {
                if (atEvade)
                    return;

                atEvade = true;
                summons.DespawnAll();
                berserkerEvents.Reset();

                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_KORKRON_DARK_SHAMANS, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FROSTSTORM_STRIKE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOXIC_MIST);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOXICITY);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_IRON_PRISON);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    if (Creature* kardris = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_WAVEBINDER_KARDIS)))
                        kardris->AI()->EnterEvadeMode();
                }

                HandleRemoveIronTombs();
                scheduler.CancelAll();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                _DespawnAtEvade();
                me->GetMap()->CreatureRelocation(me, darkShamansRelocatePos[invWolfType.find(me->GetEntry())->second[1]].GetPositionX(), darkShamansRelocatePos[invWolfType.find(me->GetEntry())->second[1]].GetPositionY(), darkShamansRelocatePos[invWolfType.find(me->GetEntry())->second[1]].GetPositionZ(), darkShamansRelocatePos[invWolfType.find(me->GetEntry())->second[1]].GetOrientation());
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FROSTSTORM_STRIKE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOXIC_MIST);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOXICITY);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                HandleRemoveIronTombs();
                berserkerEvents.Reset();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() != NPC_BLOODCLAW && summon->GetEntry() != NPC_DARKFANG)
                    summon->SetInCombatWithZone();
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (Creature* kardris = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_WAVEBINDER_KARDIS)))
                {
                    if (damage < kardris->GetHealth() && attacker->GetEntry() != NPC_WAVEBINDER_KARDIS)
                    {
                        kardris->ModifyHealth(-int32(damage));
                        kardris->AI()->DamageTaken(me, damage);
                    }
                }

                if (HealthBelowPct(nextHealthTotem) && nextHealthTotem > 40)
                {
                    DoCast(me, invTotemsType.find(nextHealthTotem)->second[0], true);
                    Talk(invTotemsType.find(nextHealthTotem)->second[4]);

                    if (uint32 totemYell = invTotemsType.find(nextHealthTotem)->second[6])
                        Talk(totemYell);

                    if (uint32 eventId = invTotemsType.find(nextHealthTotem)->second[1])
                        events.ScheduleEvent(eventId, invTotemsType.find(nextHealthTotem)->second[3]);

                    healthShift = healthShift > 1 ? 1 : 2;
                    nextHealthTotem -= healthShift % 2 == 0 ? 20 : 15;
                }

                // Heroic
                if (IsHeroic() && HealthBelowPct(95) && !ironTotem)
                {
                    ironTotem = true;
                    DoCast(me, SPELL_RUSTED_IRON_TOTEM, true);
                    Talk(TALK_HAROM_RUSTED_IRON_TOTEM_ANN);
                    Talk(TALK_HAROM_RUSTED_IRON_TOTEM);

                    events.ScheduleEvent(EVENT_IRON_TOMB, 1000);
                }

                if (HealthBelowPct(25) && !bloodLust)
                {
                    bloodLust = true;

                    Talk(TALK_HAROMM_BLOODLUST);
                    Talk(TALK_HAROMM_BLOODLUST_ANN);
                    DoCast(me, SPELL_BLOODLUST, true);

                    if (Creature* kardris = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_WAVEBINDER_KARDIS)))
                    {
                        kardris->AI()->Talk(TALK_KARDRIS_BLOODLUST);
                        kardris->AI()->Talk(TALK_KARDRIS_BLOODLUST_ANN);
                        kardris->CastSpell(kardris, SPELL_BLOODLUST, true);
                    }
                }

                // Finish Encounter
                if (attacker->GetTypeId() == TYPEID_PLAYER && damage >= me->GetHealth() && !hasDie)
                {
                    hasDie = true;

                    if (Creature* kardris = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_WAVEBINDER_KARDIS)))
                    {
                        kardris->LowerPlayerDamageReq(kardris->GetMaxHealth());
                        attacker->DealDamage(kardris, kardris->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    }
                }
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
                    if (eventId == EVENT_BERSERK)
                        DoCast(me, SPEll_BERSERK, true);
                    break;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FROSTSTORM_STRIKE:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_FROSTSTORM_STRIKE);

                            events.ScheduleEvent(EVENT_FROSTSTORM_STRIKE, urand(6 * IN_MILLISECONDS, 7 * IN_MILLISECONDS));
                            break;
                        case EVENT_TOXIC_MIST:
                            DoCast(me, SPELL_TOXIC_MIST);
                            events.ScheduleEvent(EVENT_TOXIC_MIST, 32.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_IRON_TOMB:
                            DoCast(me, SPELL_IRON_TOMB_SELECTOR);
                            events.ScheduleEvent(EVENT_IRON_TOMB, 31.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_FOUL_STREAM:
                            if (Unit* target = ObjectAccessor::GetUnit(*me, GetStreamTargetGUID()))
                                DoCast(target, SPEll_FOUL_STREAM);

                            events.ScheduleEvent(EVENT_FOUL_STREAM, 32.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_ASHEN_WALL:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_ASHEN_WALL);

                            events.ScheduleEvent(EVENT_ASHEN_WALL, 31.5 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff);
            }

            private: // default doesn`t work here - so much los and distance
                uint64 GetDpsGuidIfPossible()
                {
                    std::list<Player*> targets, m_targets;
                    GetPlayerListInGrid(targets, me, 80.0f);
                    m_targets = targets;

                    targets.remove_if(TankSpecTargetSelector());

                    if (!targets.empty())
                        return Trinity::Containers::SelectRandomContainerElement(targets)->GetGUID();

                    targets.clear();

                    for (auto&& itr : m_targets)
                        targets.push_back(itr);

                    if (!targets.empty())
                        return Trinity::Containers::SelectRandomContainerElement(targets)->GetGUID();

                    return 0;
                }

                ObjectGuid GetStreamTargetGUID()
                {
                    std::list<Player*> targets, m_targets;
                    GetPlayerListInGrid(targets, me, 80.0f);
                    m_targets = targets;

                    // Remove only players with toxic (blizzlike)
                    targets.remove_if([=](Player* target) { return target && target->HasAura(SPELL_TOXICITY); });

                    if (!targets.empty())
                        return Trinity::Containers::SelectRandomContainerElement(targets)->GetGUID();

                    return ObjectGuid::Empty;
                }

                void HandleRemoveIronTombs()
                {
                    std::list<GameObject*> tombsList;
                    GetGameObjectListWithEntryInGrid(tombsList, me, GO_IRON_TOMB, 300.0f);

                    for (auto&& itr : tombsList)
                        itr->Delete();
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_earthbreaker_harommAI(creature);
        }
};

class boss_wavebinder_kardris : public CreatureScript
{
    public:
        boss_wavebinder_kardris() : CreatureScript("boss_wavebinder_kardris") { }

        struct boss_wavebinder_kardrisAI : public ScriptedAI
        {
            boss_wavebinder_kardrisAI(Creature* creature) : ScriptedAI(creature), summons(me) 
            {
                me->setActive(true);
                instance = me->GetInstanceScript();
            }

            TaskScheduler scheduler;
            EventMap events;
            SummonList summons;
            InstanceScript* instance;
            ObjectGuid foulGeyserGUID;
            uint32 nextHealthTotem;
            uint32 healthShift;
            uint32 geyserSuccess;
            bool atEvade;
            bool ironTotem;
            bool hasDie;
            bool hasCasting; // handle for her...

            void Reset() override
            {
                events.Reset();
                foulGeyserGUID = ObjectGuid::Empty;
                nextHealthTotem = 85;
                healthShift     = 1;
                geyserSuccess   = 0;
                hasDie          = false;
                hasCasting      = false;
                ironTotem       = false;

                me->SetReactState(REACT_DEFENSIVE);

                // Heroic
                if (IsHeroic())
                {
                    me->SetTotalAuraEffectValue(SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK, 1.35f);
                    me->UpdateCastingSpeed();
                    me->UpdateHasteRegen();
                }

                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                atEvade = false;

                scheduler
                    .Schedule(Milliseconds(1500), [this](TaskContext context)
                {
                    if (Creature* wolf = me->SummonCreature(invWolfType.find(me->GetEntry())->second[0], *me, TEMPSUMMON_MANUAL_DESPAWN))
                        me->CastSpell(wolf, VEHICLE_SPELL_RIDE_HARDCODED, true);
                });

                scheduler
                    .Schedule(Seconds(2), [this](TaskContext context)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() != NPC_BLOODCLAW && summon->GetEntry() != NPC_DARKFANG)
                    summon->SetInCombatWithZone();
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (Creature* haromm = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KORKRON_DARK_SHAMANS)))
                {
                    if (damage < haromm->GetHealth() && attacker->GetEntry() != NPC_EARTHBREAKER_HAROMM)
                    {
                        haromm->ModifyHealth(-int32(damage));
                        haromm->AI()->DamageTaken(me, damage);
                    }
                }

                if (HealthBelowPct(nextHealthTotem) && nextHealthTotem > 40)
                {
                    DoCast(me, invTotemsType.find(nextHealthTotem)->second[0], true);

                    // Announce only if we`r not near
                    if (Creature* haromm = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KORKRON_DARK_SHAMANS)))
                        if (me->GetExactDist2d(haromm) > 40.0f)
                            Talk(invTotemsType.find(nextHealthTotem)->second[5]);

                    if (uint32 totemYell = invTotemsType.find(nextHealthTotem)->second[7])
                        Talk(totemYell);

                    if (uint32 eventId = invTotemsType.find(nextHealthTotem)->second[2])
                        events.ScheduleEvent(eventId, invTotemsType.find(nextHealthTotem)->second[3]);

                    healthShift = healthShift > 1 ? 1 : 2;
                    nextHealthTotem -= healthShift % 2 == 0 ? 20 : 15;
                }

                // Heroic
                if (IsHeroic() && HealthBelowPct(95) && !ironTotem)
                {
                    ironTotem = true;
                    DoCast(me, SPELL_RUSTED_IRON_TOTEM, true);

                    events.ScheduleEvent(EVENT_IRON_PRISON, 1000);
                }

                // Finish Encounter
                if (attacker->GetTypeId() == TYPEID_PLAYER && damage >= me->GetHealth() && !hasDie)
                {
                    hasDie = true;

                    if (Creature* haromm = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KORKRON_DARK_SHAMANS)))
                    {
                        haromm->LowerPlayerDamageReq(haromm->GetMaxHealth());
                        attacker->DealDamage(haromm, haromm->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    }
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                summons.DespawnAll();
            }

            void JustEngagedWith(Unit* who) override
            {
                me->setActive(true, ActiveFlags::InCombat);
                DoZoneInCombat();

                if (Creature* haromm = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KORKRON_DARK_SHAMANS)))
                    DoZoneInCombat(haromm, 200.0f);

                me->SetReactState(REACT_AGGRESSIVE);

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                me->ExitVehicle();

                events.ScheduleEvent(EVENT_FROSTSTORM_BOLT, 5.5 * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_REMOVE_GEYSER)
                {
                    scheduler
                        .Schedule(Milliseconds(500), [this](TaskContext context)
                    {
                        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                        events.RescheduleEvent(EVENT_FROSTSTORM_BOLT, 3 * IN_MILLISECONDS);
                        hasCasting = false;
                    });
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_GEYSER_CAST_SUCCESS)
                    geyserSuccess = data;
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_GEYSER_CAST_SUCCESS)
                    return geyserSuccess;

                return 0;
            }

            void EnterEvadeMode() override
            {
                if (atEvade)
                    return;

                atEvade = true;
                summons.DespawnAll();

                ScriptedAI::EnterEvadeMode();

                me->DeleteThreatList();
                me->ResetPlayerDamageReq();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    if (Creature* haromm = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KORKRON_DARK_SHAMANS)))
                        haromm->AI()->EnterEvadeMode();
                }

                scheduler.CancelAll();

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                uint32 corpseDelay = me->GetCorpseDelay();
                uint32 respawnDelay = me->GetRespawnDelay();

                me->SetCorpseDelay(1);
                me->SetRespawnDelay(29);

                me->DespawnOrUnsummon();

                me->SetCorpseDelay(corpseDelay);
                me->SetRespawnDelay(respawnDelay);
                me->GetMap()->CreatureRelocation(me, darkShamansRelocatePos[invWolfType.find(me->GetEntry())->second[1]].GetPositionX(), darkShamansRelocatePos[invWolfType.find(me->GetEntry())->second[1]].GetPositionY(), darkShamansRelocatePos[invWolfType.find(me->GetEntry())->second[1]].GetPositionZ(), darkShamansRelocatePos[invWolfType.find(me->GetEntry())->second[1]].GetOrientation());
            }

            ObjectGuid GetGUID(int32 type) const override
            {
                return foulGeyserGUID;
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || hasCasting)
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FOUL_GEYSER:
                        {
                            geyserSuccess = 0;

                            if (Unit* target = me->GetVictim())
                            {
                                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                                foulGeyserGUID = target->GetGUID();
                                DoCast(target, SPELL_FOUL_GEYSER);
                            }
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                            {
                                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                                foulGeyserGUID = target->GetGUID();
                                DoCast(target, SPELL_FOUL_GEYSER);
                            }

                            // some issue - doesn`t set state ???
                            hasCasting = true;
                            me->AddUnitState(UNIT_STATE_CASTING);

                            // In case if it was not cast success (NE malf, vanish etc. - abuse)
                            uint32 checkTimer = IsHeroic() ? 2500 : 3500;
                            scheduler
                                .Schedule(Milliseconds(checkTimer), [this](TaskContext context)
                            {
                                if (!GetData(TYPE_GEYSER_CAST_SUCCESS))
                                    DoAction(ACTION_REMOVE_GEYSER);
                            });

                            events.ScheduleEvent(EVENT_FOUL_GEYSER, 32.5 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_TOXIC_STORM:
                            if (Unit* target = ObjectAccessor::GetUnit(*me, GetCasterGuidIfPossible()))
                            {
                                events.DelayEvents(200);
                                ObjectGuid tempStormGUID = target->GetGUID();
                                me->SetTarget(target->GetGUID());

                                // Target Scanning (DBM)
                                scheduler
                                    .Schedule(Milliseconds(100), [this, tempStormGUID](TaskContext context)
                                {
                                    if (Unit* stormTarget = ObjectAccessor::GetUnit(*me, tempStormGUID))
                                        DoCast(stormTarget, SPELL_TOXIC_STORM);

                                    if (Unit* victim = me->GetVictim())
                                        me->SetTarget(victim->GetGUID());
                                });
                            }

                            events.ScheduleEvent(EVENT_TOXIC_STORM, 31 * IN_MILLISECONDS);
                            break;
                        case EVENT_IRON_PRISON:
                            DoCast(me, SPELL_IRON_PRISON);
                            events.ScheduleEvent(EVENT_IRON_PRISON, 31.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_FALLING_ASH:
                            if (Unit* target = ObjectAccessor::GetUnit(*me, GetCasterGuidIfPossible()))
                                DoCast(target, SPELL_FALLING_ASH);

                            events.ScheduleEvent(EVENT_FALLING_ASH, 32.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_FROSTSTORM_BOLT:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_FROSTSTORM_BOLT);

                            events.ScheduleEvent(EVENT_FROSTSTORM_BOLT, urand(7.5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff);
            }

            private: // default doesn`t work here - so much los and distance
                ObjectGuid GetCasterGuidIfPossible()
                {
                    std::list<Player*> targets, m_targets;
                    GetPlayerListInGrid(targets, me, 80.0f);
                    m_targets = targets;

                    targets.remove_if(TankSpecTargetSelector());
                    targets.remove_if(MeeleSpecTargetSelector());

                    if (!targets.empty())
                        return Trinity::Containers::SelectRandomContainerElement(targets)->GetGUID();

                    targets.clear();

                    for (auto&& itr : m_targets)
                        targets.push_back(itr);

                    if (!targets.empty())
                        return Trinity::Containers::SelectRandomContainerElement(targets)->GetGUID();

                    return ObjectGuid::Empty;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_wavebinder_kardrisAI(creature);
        }
};

// Bloodclaw 71923, Darkfang 71921
struct npc_dark_shamans_wolf : public ScriptedAI
{
    npc_dark_shamans_wolf(Creature* creature) : ScriptedAI(creature) { }

    InstanceScript* instance;
    EventMap events;

    void Reset() override
    {
        instance = me->GetInstanceScript();
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        if (Creature* haromm = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KORKRON_DARK_SHAMANS)))
            DoZoneInCombat(haromm, 200.0f);

        if (Creature* kardris = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_WAVEBINDER_KARDIS)))
            DoZoneInCombat(kardris, 200.0f);

        events.ScheduleEvent(EVENT_REND, 5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SWIPE, 6.5 * IN_MILLISECONDS);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();

        if (instance)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

            if (Creature* haromm = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KORKRON_DARK_SHAMANS)))
                haromm->AI()->EnterEvadeMode();

            if (Creature* kardris = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_WAVEBINDER_KARDIS)))
                kardris->AI()->EnterEvadeMode();
        }
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
                case EVENT_REND:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_REND);

                    events.ScheduleEvent(EVENT_REND, 9.5 * IN_MILLISECONDS);
                    break;
                case EVENT_SWIPE:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SWIPE);

                    events.ScheduleEvent(EVENT_SWIPE, 7 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Foul Slime 71825
struct npc_foul_slime : public ScriptedAI
{
    npc_foul_slime(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        DoCast(me, SPELL_FOULNESS);
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Toxic Storm 71801
struct npc_dark_shamans_toxic_storm : public ScriptedAI
{
    npc_dark_shamans_toxic_storm(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        DoCast(me, SPELL_TOXIC_STORM_AURA);

        scheduler
            .Schedule(Seconds(11), [this](TaskContext context)
        {
            DoCast(me, SPELL_TOXIC_STORM_VORTEX);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Toxic Tornado 71817
struct npc_dark_shamans_toxic_tornado : public ScriptedAI
{
    npc_dark_shamans_toxic_tornado(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    Position spawnPos;

    void Reset() override
    {
        me->DespawnOrUnsummon(60 * IN_MILLISECONDS);
        spawnPos = { me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation() };
        DoCast(me, SPELL_TOXIC_TORNADO);

        if (Creature* haromm = ObjectAccessor::GetCreature(*me, me->GetInstanceId() ? me->GetInstanceScript()->GetGuidData(DATA_KORKRON_DARK_SHAMANS) : ObjectGuid::Empty))
            haromm->AI()->JustSummoned(me);

        scheduler
            .Schedule(Seconds(1), [this](TaskContext context)
        {
            GenerateMovement();
            context.Repeat(Milliseconds(me->GetSplineDuration()));
        });
    }

    void GenerateMovement()
    {
        Position pos = me->GetRandomPoint(spawnPos, 20.0f);
        me->GetMotionMaster()->MovePoint(0, pos);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Falling Ash 71789
struct npc_dark_shamans_falling_ash : public ScriptedAI
{
    npc_dark_shamans_falling_ash(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    Position fallingPos;

    void Reset() override
    {
        me->SetCanFly(true);
        me->SetDisableGravity(true);

        me->SetSpeed(MOVE_RUN, 0.75f, true);
        me->SetSpeed(MOVE_FLIGHT, 0.75f, true);

        fallingPos = { me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() - 75.0f, 0.0f };
        DoCast(me, SPELL_FALLING_ASH_COSMETIC);
        me->GetMotionMaster()->MovePoint(0, fallingPos);

        me->SetAnimTier(AnimTier::Hover);

        scheduler
            .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
        {
            DoCast(me, SPELL_FALLING_ASH_EFF);
            me->DespawnOrUnsummon(1000);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Ashen Elemental 71827
struct npc_dark_shamans_ashen_elemental : public ScriptedAI
{
    npc_dark_shamans_ashen_elemental(Creature* creature) : ScriptedAI(creature) 
    { 
        SetCombatMovement(false);
    }

    TaskScheduler scheduler;

    void Reset() override
    {
        me->SetReactState(REACT_PASSIVE);

        scheduler
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            for (auto&& itr : me->GetInstanceScript()->instance->GetPlayers())
            {
                if (itr.GetSource() && itr.GetSource()->GetExactDist2d(me) < 3.5f)
                    me->AI()->AttackStart(itr.GetSource());

            }

            context.Repeat(Milliseconds(1000));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Iron Tomb 71941
struct npc_dark_shamans_iron_tomb : public ScriptedAI
{
    npc_dark_shamans_iron_tomb(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        scheduler
            .Schedule(Milliseconds(500), [this](TaskContext context)
        {
            DoCast(me, SPELL_IRON_TOMB_EFF);
        });
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);
    }
};

// Toxic Mist 144089
class spell_dark_shamans_toxic_mist : public SpellScript
{
    PrepareSpellScript(spell_dark_shamans_toxic_mist);

    std::list<WorldObject*> m_targets, copyTargets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // Try not select targets with tank spec and already affected by mark
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_TOXIC_MIST); });

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
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dark_shamans_toxic_mist::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dark_shamans_toxic_mist::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Toxic Mist 144089
class spell_dark_shamans_toxic_mist_aura : public AuraScript
{
    PrepareAuraScript(spell_dark_shamans_toxic_mist_aura);

    void OnRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->RemoveAurasDueToSpell(SPELL_TOXICITY);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dark_shamans_toxic_mist_aura::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAPPLY);
        OnEffectRemove += AuraEffectRemoveFn(spell_dark_shamans_toxic_mist_aura::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Ashen Wall 144070
class spell_dark_shamans_ashen_wall : public SpellScript
{
    PrepareSpellScript(spell_dark_shamans_ashen_wall);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                // perform wall pos
                caster->CastSpell(target, SPELL_ASHEN_WALL_SUMM, true);

                // left side
                for (uint8 i = 1; i < 5; i++)
                {
                    GetPositionWithDistInOrientation(target, 3.0f * i, Position::NormalizeOrientation(target->GetOrientation() + M_PI / 2), x, y);
                    caster->CastSpell(x, y, target->GetMap()->GetHeight(x, y, target->GetPositionZ(), true, 10.0f), SPELL_ASHEN_WALL_SUMM, true);
                }

                // right side
                for (uint8 i = 1; i < 5; i++)
                {
                    GetPositionWithDistInOrientation(target, 3.0f * i, Position::NormalizeOrientation(target->GetOrientation() - M_PI / 2), x, y);
                    caster->CastSpell(x, y, target->GetMap()->GetHeight(x, y, target->GetPositionZ(), true, 10.0f), SPELL_ASHEN_WALL_SUMM, true);
                }
            }
        }
    }

    private:
        float x, y;

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_dark_shamans_ashen_wall::HandleHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Foul Geyser 143990
class spell_dark_shamans_foul_geyser : public SpellScript
{
    PrepareSpellScript(spell_dark_shamans_foul_geyser);

    void HandleAfterCast()
    {
        // Check Success Cast or not (Melf, vanish abuse)
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->SetData(TYPE_GEYSER_CAST_SUCCESS, 1);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_dark_shamans_foul_geyser::HandleAfterCast);
    }
};

// Foul Geyser 143990
class spell_dark_shamans_foul_geyser_aura : public AuraScript
{
    PrepareAuraScript(spell_dark_shamans_foul_geyser_aura);

    void HandleApply(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        // Handle - Channeled
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AddUnitState(UNIT_STATE_CASTING);
    }

    void HandleRemove(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_REMOVE_GEYSER);
    }

    void OnTick(AuraEffect const* aurEff)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            if (Unit* geyserOwner = ObjectAccessor::GetUnit(*owner, owner->AI()->GetGUID()))
                owner->CastSpell(geyserOwner, SPELL_FOUL_GEYSER_MISSLE, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dark_shamans_foul_geyser_aura::HandleApply, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_dark_shamans_foul_geyser_aura::HandleRemove, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dark_shamans_foul_geyser_aura::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// Bloodlust 144302
class spell_dark_shamans_bloodlust : public SpellScript
{
    PrepareSpellScript(spell_dark_shamans_bloodlust);

    std::list<WorldObject*> copyTargets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_WAVEBINDER_KARDIS && target->GetEntry() != NPC_EARTHBREAKER_HAROMM; });
        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>&targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dark_shamans_bloodlust::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dark_shamans_bloodlust::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dark_shamans_bloodlust::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Falling Ash 143973
class spell_dark_shamans_falling_ash : public SpellScript
{
    PrepareSpellScript(spell_dark_shamans_falling_ash);

    void SelectTargets(SpellDestination& dest)
    {
        Position newPos = { dest._position.GetPositionX(), dest._position.GetPositionY(), dest._position.GetPositionZ() + 75.0f, 0 };
        dest.Relocate(newPos);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_dark_shamans_falling_ash::SelectTargets, EFFECT_0, TARGET_DEST_CASTER_RANDOM);
    }
};

// Foul Geyser Eff 143993
class spell_dark_shamans_foul_geyser_eff : public SpellScript
{
    PrepareSpellScript(spell_dark_shamans_foul_geyser_eff);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetBossState(DATA_KORKRON_DARK_SHAMANS) != IN_PROGRESS)
            PreventHitEffect(eff_idx);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_dark_shamans_foul_geyser_eff::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectHit += SpellEffectFn(spell_dark_shamans_foul_geyser_eff::HandleEffectHitTarget, EFFECT_1, SPELL_EFFECT_SUMMON);
    }
};

// Iron Tomb Selector 144328
class spell_dark_shamans_iron_tomb_selector : public SpellScript
{
    PrepareSpellScript(spell_dark_shamans_iron_tomb_selector);

    std::list<WorldObject*> m_targets;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[effIdx].BasePoints, true);
    }

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // not tanks and Melee select
        targets.remove_if(MeeleSpecTargetSelector());

        uint32 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 3 : 2;

        if (targets.size() < targetsCount)
        {
            targets.clear();

            for (auto&& itr : m_targets)
                targets.push_back(itr);

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount);

            return;
        }

        if (targets.size() > targetsCount)
            Trinity::Containers::RandomResizeList(targets, targetsCount);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dark_shamans_iron_tomb_selector::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dark_shamans_iron_tomb_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Iron Prison 144330
class spell_dark_shamans_iron_prison : public SpellScript
{
    PrepareSpellScript(spell_dark_shamans_iron_prison);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // not tanks select there
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_IRON_PRISON); });

        if (targets.size() < 2)
        {
            targets.clear();

            for (auto&& itr : m_targets)
                targets.push_back(itr); // nvm, maxAffect is 2

            targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_IRON_PRISON); });

            return;
        }

        if (targets.size() > 2)
            Trinity::Containers::RandomResizeList(targets, 2);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dark_shamans_iron_prison::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Iron Prison 144330
class spell_dark_shamans_iron_prison_aura : public AuraScript
{
    PrepareAuraScript(spell_dark_shamans_iron_prison_aura);

    void HandleRemove(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (owner->GetInstanceScript() && owner->GetInstanceScript()->GetBossState(DATA_KORKRON_DARK_SHAMANS) != IN_PROGRESS)
                return;

            int32 mod = (int32)(owner->GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, SPELL_SCHOOL_MASK_NORMAL) * 100);
            owner->CastCustomSpell(owner, SPELL_IRON_PRISON_EFF, &mod, NULL, NULL, true);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_dark_shamans_iron_prison_aura::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_korkron_dark_shamans()
{
    new boss_earthbreaker_haromm();
    new boss_wavebinder_kardris();

    new creature_script<npc_dark_shamans_wolf>("npc_dark_shamans_wolf");
    new creature_script<npc_foul_slime>("npc_foul_slime");
    new creature_script<npc_dark_shamans_toxic_storm>("npc_dark_shamans_toxic_storm");
    new creature_script<npc_dark_shamans_toxic_tornado>("npc_dark_shamans_toxic_tornado");
    new creature_script<npc_dark_shamans_falling_ash>("npc_dark_shamans_falling_ash");
    new creature_script<npc_dark_shamans_ashen_elemental>("npc_dark_shamans_ashen_elemental");
    new creature_script<npc_dark_shamans_iron_tomb>("npc_dark_shamans_iron_tomb");

    new spell_script<spell_dark_shamans_toxic_mist>("spell_dark_shamans_toxic_mist");
    new aura_script<spell_dark_shamans_toxic_mist_aura>("spell_dark_shamans_toxic_mist_aura");
    new spell_script<spell_dark_shamans_ashen_wall>("spell_dark_shamans_ashen_wall");
    new spell_script<spell_dark_shamans_foul_geyser>("spell_dark_shamans_foul_geyser");
    new aura_script<spell_dark_shamans_foul_geyser_aura>("spell_dark_shamans_foul_geyser_aura");
    new spell_script<spell_dark_shamans_bloodlust>("spell_dark_shamans_bloodlust");
    new spell_script<spell_dark_shamans_falling_ash>("spell_dark_shamans_falling_ash");
    new spell_script<spell_dark_shamans_foul_geyser_eff>("spell_dark_shamans_foul_geyser_eff");
    new spell_script<spell_dark_shamans_iron_tomb_selector>("spell_dark_shamans_iron_tomb_selector");
    new spell_script<spell_dark_shamans_iron_prison>("spell_dark_shamans_iron_prison");
    new aura_script<spell_dark_shamans_iron_prison_aura>("spell_dark_shamans_iron_prison_aura");
}
