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
#include"bastion_of_twilight.h"

// todo: ����������� � entry ����� � orders
// todo: ����������� ���������� absorb fire, absorb shadow
// todo: ������� ������ ������
// todo: ����������� � ��������� fester blood

enum ScriptTexts
{
    SAY_KILL        = 1,
    SAY_CONVERSION  = 2,
    SAY_AGGRO       = 3,
    SAY_ADDS        = 4,
    SAY_CONSUME     = 6,
    SAY_CREATIONS   = 7,
    SAY_DEATH_1     = 10,
    SAY_DEATH_2     = 11,
    SAY_WIPE        = 12,
};

enum Spells
{
    SPELL_BERSERK                                  = 64238,
    SPELL_BOSS_HITTING_YA                          = 73878,
    SPELL_CONVERSION                               = 91303,
    SPELL_WORSHIPPING                              = 91317,
    SPELL_TWISTED_DEVOTION                         = 91331,
    SPELL_FURY_OF_CHOGALL                          = 82524,
    SPELL_FLAME_ORDERS                             = 81171,
    SPELL_FLAME_ORDERS_SUM_1                       = 81188,
    SPELL_FLAME_ORDERS_SUM_2                       = 81186,
    SPELL_FLAME_ORDERS_PERIODIC_TRIGGER            = 87581,
    SPELL_FLAME_ORDERS_1                           = 87579,
    SPELL_FLAME_ORDERS_SUM_3                       = 87578,
    SPELL_FLAME_ORDERS_SUM_4                       = 87582,
    SPELL_SHADOW_ORDERS                            = 81556,
    SPELL_SHADOW_ORDERS_SUM_1                      = 81557,
    SPELL_SHADOW_ORDERS_SUM_2                      = 81558,
    SPELL_SHADOW_ORDERS_PERIODIC_TRIGGER           = 87576,
    SPELL_SHADOW_ORDERS_1                          = 87575,
    SPELL_SHADOW_ORDERS_SUM_3                      = 87574,
    SPELL_SHADOW_ORDERS_SUM_4                      = 87583,
    SPELL_SUMMON_CORRUPTING_ADHERENT               = 81628,
    SPELL_SUMMON_CORRUPTING_ADHERENT_1             = 81611,
    SPELL_SUMMON_CORRUPTING_ADHERENT_2             = 81618,
    SPELL_ABSORB_FIRE                              = 81196,
    SPELL_ABSORB_SHADOW                            = 81566,
    SPELL_SHADOW_SHELL                             = 93311,
    SPELL_FIRE_SHELL                               = 93276,
    SPELL_FIRE_DESTRUCTION                         = 81194,
    SPELL_EMPOWERED_SHADOWS                        = 81572,
    SPELL_UNLEASHED_SHADOWS_DMG                    = 81571,
    SPELL_CORRUPTING_CRASH                         = 81685,
    SPELL_DEPRAVITY                                = 81713,
    SPELL_FESTER_BLOOD                             = 82299,
    SPELL_FESTERING_BLOOD                          = 82914,
    SPELL_SPRAYED_CORRUPTION                       = 82919,
    SPELL_FESTER_BLOOD_SCRIPT                      = 82337,
    SPELL_SPILLED_BLOOD_OF_THE_OLD_GOD             = 81757,
    SPELL_SPILLED_BLOOD_OF_THE_OLD_GOD_DMG         = 81761,
    SPELL_SPILLED_BLOOD_OF_THE_OLD_GOD_DUMMY       = 81771,
    SPELL_FESTERED_BLOOD_SUM                       = 82333,
    SPELL_BLAZE_PERIODIC_TRIGGER                   = 81536,
    SPELL_BLAZE_DMG                                = 81538,
    SPELL_CORRUPTION_OF_THE_OLD_GOD                = 82361,
    SPELL_CORRUPTION_OF_THE_OLD_GOD_DMG            = 82363,
    SPELL_CORRUPTION_OF_THE_OLD_GOD_VISUAL         = 82356,
    SPELL_CONSUME_BLOOD_OF_THE_OLD_GOD             = 82630,
    SPELL_DARKENED_CREATIONS                       = 82414,
    SPELL_DARKENED_CREATIONS_SUM                   = 82433,
    SPELL_DEBILITATING_BEAM                        = 82411,

    // Corruption spells
    SPELL_CORRUPTED_BLOOD                          = 93104,
    SPELL_CORRUPTED_BLOOD_AURA                     = 93103,
    SPELL_CORRUPTION_ACCELERATED                   = 81836,
    SPELL_CORRUPTION_ACCELERATED_DMG               = 81943,
    SPELL_CORRUPTION_SICKNESS                      = 81829,
    SPELL_CORRUPTION_SICKNESS_1                    = 82235,
    SPELL_CORRUPTION_SICKNESS_DMG                  = 81831,
    SPELL_CORRUPTION_MALFORMATION_VEHICLE          = 82125,
    SPELL_CORRUPTION_MALFORMATION_DUMMY            = 82167,
    SPELL_SHADOW_BOLT                              = 82151,
    SPELL_CORRUPTION_ABSOLUTE                      = 82170,
    SPELL_CORRUPTION_ABSOLUTE_VISUAL               = 82193,
};

enum Adds
{
    NPC_FIRE_PORTAL_1           = 43393,
    NPC_FIRE_ELEMENTAL_1        = 43406,
    NPC_FIRE_PORTAL_2           = 47020,
    NPC_FIRE_ELEMENTAL_2        = 47017,
    NPC_SHADOW_PORTAL_1         = 43603,
    NPC_SHADOW_LORD_1           = 43592, 
    NPC_SHADOW_PORTAL_2         = 47019,
    NPC_SHADOW_LORD_2           = 47016,
    NPC_CORRUPTING_ADHERENT     = 43622,
    NPC_DARKENED_CREATION       = 44045,
    NPC_BLAZE                   = 43585,
    NPC_BLOOD_OF_THE_OLD_GOD    = 43707,
    NPC_SPRAY_BLOOD             = 45848,
    NPC_CONGEALED_BLOOD         = 45858,
    NPC_OLD_GOD_PORTAL          = 45685,
    NPC_FACELESS_GUARDIAN       = 45676,
    NPC_MALFORMATION            = 43888,
    NPC_CORRUPTION              = 43999,
};

enum Events
{
    EVENT_CONVERSION            = 1,
    EVENT_FURY_OF_CHOGALL       = 2,
    EVENT_ORDERS                = 3,
    EVENT_CORRUPTING_ADHERENT   = 5,
    EVENT_ORDER_MOVE            = 6,
    EVENT_ORDER_DESPAWN         = 7,
    EVENT_DEPRAVITY             = 8,
    EVENT_CORRUPTING_CRASH      = 9,
    EVENT_FESTER_BLOOD          = 10,
    EVENT_FESTER_BLOOD_1        = 11,
    EVENT_CONSUME_BLOOD         = 12,
    EVENT_CONSUME_BLOOD_1       = 13,
    EVENT_DARKENED_CREATIONS    = 15,
    EVENT_DEBILITATING_BEAM     = 16,
    EVENT_BERSERK               = 17,
    EVENT_SHADOW_BOLT           = 18,
    EVENT_ORDER_SUM             = 19,
    EVENT_SPILLED_BLOOD         = 20,
};

enum Equipment
{
    EQUIPMENT_ID_WEAPON    = 63680,
};

enum DoCorruptionTypes
{
    CORRUPTION_INIT        = 1,
    CORRUPTION_CLEAR       = 2,
};

const Position corruptionPos = {-1162.15f, -799.06f, 836.0f, 0.0f}; 

template <class C> typename C::value_type const& SelectRandomContainerElement(C const& container)
{
    typename C::const_iterator it = container.begin();
    std::advance(it, urand(0, container.size() - 1));
    return *it;
}

void AddCorruption(Unit* player, uint32 value)
{
    uint32 oldvalue = player->GetPower(POWER_ALTERNATE_POWER);
    uint32 newvalue = oldvalue + value;
    if (newvalue > 100)
        newvalue = 100;

    player->SetPower(POWER_ALTERNATE_POWER, newvalue);

    if (newvalue > 30)
        player->GetMap()->SetWorldState(WORLDSTATE_ABYSS_WILL_GAZE_BACK_INTO_YOU, 0);
    
    if (oldvalue < 100 && newvalue > 99)
    {
        if (!player->HasAura(SPELL_CORRUPTION_ABSOLUTE) &&
            !player->HasAura(SPELL_CORRUPTION_ABSOLUTE_VISUAL))
        {
            player->CastSpell(player, SPELL_CORRUPTION_ABSOLUTE, true);
            player->CastSpell(player, SPELL_CORRUPTION_ABSOLUTE_VISUAL, true);
        }
    }
    else if (oldvalue < 75 && newvalue >= 75)
    {
        if (!player->HasAura(SPELL_CORRUPTION_MALFORMATION_DUMMY) &&
            !player->HasAura(SPELL_CORRUPTION_MALFORMATION_VEHICLE))
        {
            player->CastSpell(player, SPELL_CORRUPTION_MALFORMATION_DUMMY, true);
            player->CastSpell(player, SPELL_CORRUPTION_MALFORMATION_VEHICLE, true);
            if (Creature* pMalformation = player->SummonCreature(NPC_MALFORMATION, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0.0f, TEMPSUMMON_CORPSE_DESPAWN))
                pMalformation->EnterVehicle(player);
        }
    }
    else if (oldvalue < 50 && newvalue >=50)
    {
        if (!player->HasAura(SPELL_CORRUPTION_SICKNESS))
            player->CastSpell(player, SPELL_CORRUPTION_SICKNESS, true);
    }
    else if (oldvalue < 25 && newvalue >=25)
    {
        if (!player->HasAura(SPELL_CORRUPTION_ACCELERATED))
            player->CastSpell(player, SPELL_CORRUPTION_ACCELERATED, true);
    }
}

class boss_chogall : public CreatureScript
{
    public:
        boss_chogall() : CreatureScript("boss_chogall") { }

        struct boss_chogallAI : public BossAI
        {
            boss_chogallAI(Creature* creature) : BossAI(creature, DATA_CHOGALL), summons(me)
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

            EventMap events;
            SummonList summons;
            bool bPhase2;
            uint8 uiOrder; // 0 | 1
            
            void Reset() override 
            {
                _Reset();

                events.Reset();
                summons.DespawnAll();
                SetEquipmentSlots(false, EQUIPMENT_ID_WEAPON, 0, 0);
                me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 10);
                me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 10);
                DoCorruption(CORRUPTION_CLEAR);
                bPhase2 = false;
                uiOrder = 0;

                me->GetMap()->SetWorldState(WORLDSTATE_ABYSS_WILL_GAZE_BACK_INTO_YOU, 1);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                if (me->IsInCombat())
                    DoZoneInCombat(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoCorruption(CORRUPTION_CLEAR);
                DoCorruption(CORRUPTION_INIT);
                DoCast(me, SPELL_CORRUPTED_BLOOD, true);
                DoCast(me, SPELL_BOSS_HITTING_YA, true);
                events.ScheduleEvent(EVENT_ORDERS, 5000);
                events.ScheduleEvent(EVENT_BERSERK, 10 * MINUTE * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CORRUPTING_ADHERENT, 57000);
                events.ScheduleEvent(EVENT_CONVERSION, urand(12000, 15000));
                events.ScheduleEvent(EVENT_FURY_OF_CHOGALL, 40000);
                Talk(SAY_AGGRO);
                instance->SetBossState(DATA_CHOGALL, IN_PROGRESS);
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                Talk(SAY_WIPE);
                instance->SetBossState(DATA_CHOGALL, FAIL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                summons.DespawnAll();
                DoCorruption(CORRUPTION_CLEAR);
                Talk(IsHeroic()? SAY_DEATH_2: SAY_DEATH_1);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;
                
                if (me->HealthBelowPct(25) && !bPhase2)
                {
                    bPhase2 = true;
                    events.CancelEvent(EVENT_ORDERS);
                    events.CancelEvent(EVENT_CONVERSION);
                    events.CancelEvent(EVENT_CORRUPTING_ADHERENT);
                    events.CancelEvent(EVENT_FESTER_BLOOD);
                    events.CancelEvent(EVENT_FESTER_BLOOD_1);
                    events.CancelEvent(EVENT_FURY_OF_CHOGALL);
                    events.DelayEvents(5100);
                    DoCast(me, SPELL_CONSUME_BLOOD_OF_THE_OLD_GOD);
                    events.ScheduleEvent(EVENT_CONSUME_BLOOD_1, 5100);
                    events.ScheduleEvent(EVENT_DARKENED_CREATIONS, urand(10000, 11000));
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            break;
                        case EVENT_FURY_OF_CHOGALL:
                            DoCast(me->GetVictim(), SPELL_FURY_OF_CHOGALL);
                            events.ScheduleEvent(EVENT_FURY_OF_CHOGALL, urand(47000, 48000));
                            break;
                        case EVENT_CONVERSION:
                            Talk(SAY_CONVERSION);
                            me->CastCustomSpell(SPELL_CONVERSION, SPELLVALUE_MAX_TARGETS, RAID_MODE(2, 3, 4, 6), 0, false);
                            events.ScheduleEvent(EVENT_CONVERSION, urand(35000, 37000)); 
                            break;
                        case EVENT_ORDERS:
                            switch (uiOrder)
                            {
                                case 0:
                                    uiOrder = 1;
                                    DoCast(me, SPELL_SHADOW_ORDERS_1);
                                    break;
                                case 1:
                                    uiOrder = 0;
                                    DoCast(me, SPELL_FLAME_ORDERS_1);
                                    break;
                            }
                            events.ScheduleEvent(EVENT_ORDERS, urand(25000, 26000));
                            break;
                        case EVENT_CORRUPTING_ADHERENT:
                            Talk(SAY_ADDS);
                            DoCast(me, SPELL_SUMMON_CORRUPTING_ADHERENT);
                            events.ScheduleEvent(EVENT_FESTER_BLOOD, 40000);
                            events.ScheduleEvent(EVENT_CORRUPTING_ADHERENT, urand(90000, 93000));
                            break;
                        case EVENT_FESTER_BLOOD:
                            events.DelayEvents(2100);
                            DoCast(me, SPELL_FESTER_BLOOD);
                            events.ScheduleEvent(EVENT_FESTER_BLOOD_1, 2100);
                            break;
                        case EVENT_FESTER_BLOOD_1:
                            if (!summons.empty())
                            {
                                for (std::list<ObjectGuid>::const_iterator itr = summons.begin(); itr != summons.end(); ++itr)
                                {
                                    if (Creature* pSummon = Unit::GetCreature(*me, *itr))
                                    {
                                        if (pSummon->GetEntry() != NPC_CORRUPTING_ADHERENT)
                                            continue;
                                        if (pSummon->GetStandState() == UNIT_STAND_STATE_DEAD)
                                            pSummon->CastSpell(pSummon, SPELL_FESTER_BLOOD_SCRIPT, true);
                                        else
                                            pSummon->CastSpell(pSummon, SPELL_FESTERING_BLOOD, true);
                                    }
                                }
                            }
                            break;
                        case EVENT_CONSUME_BLOOD:
                            Talk(SAY_CONSUME);
                            events.CancelEvent(EVENT_ORDERS);
                            events.CancelEvent(EVENT_CONVERSION);
                            events.CancelEvent(EVENT_CORRUPTING_ADHERENT);
                            events.CancelEvent(EVENT_FESTER_BLOOD);
                            events.CancelEvent(EVENT_FESTER_BLOOD_1);
                            events.DelayEvents(5100);
                            DoCast(me, SPELL_CONSUME_BLOOD_OF_THE_OLD_GOD);
                            events.ScheduleEvent(EVENT_CONSUME_BLOOD_1, 5100);
                            events.ScheduleEvent(EVENT_DARKENED_CREATIONS, urand(10000, 11000));
                            break;
                        case EVENT_CONSUME_BLOOD_1:
                            summons.DespawnEntry(NPC_CORRUPTING_ADHERENT);
                            me->SummonCreature(NPC_CORRUPTION, corruptionPos);
                            DoCast(me, SPELL_CORRUPTION_OF_THE_OLD_GOD);
                            break;
                        case EVENT_DARKENED_CREATIONS:
                            Talk(SAY_CREATIONS);
                            me->CastCustomSpell(SPELL_DARKENED_CREATIONS, SPELLVALUE_MAX_TARGETS, RAID_MODE(4, 4, 10, 10), 0, false);
                            events.ScheduleEvent(EVENT_DARKENED_CREATIONS, urand(29000, 31000));
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                void DoCorruption(DoCorruptionTypes type)
                {
                    Map::PlayerList const &pList = me->GetMap()->GetPlayers();
                    if (pList.isEmpty())
                        return;

                    switch (type)
                    {
                        case CORRUPTION_INIT:
                            for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
                            {
                                Player* player = itr->GetSource();
                                player->SetMaxPower(POWER_ALTERNATE_POWER, 100);
                            }
                            break;
                        case CORRUPTION_CLEAR:
                            for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
                            {
                                Player* player = itr->GetSource();
                                player->SetPower(POWER_ALTERNATE_POWER, 0);
                                player->RemoveAurasDueToSpell(SPELL_CORRUPTION_ACCELERATED);
                                player->RemoveAurasDueToSpell(SPELL_CORRUPTION_SICKNESS);
                                player->RemoveAurasDueToSpell(SPELL_CORRUPTION_MALFORMATION_VEHICLE);
                                player->RemoveAurasDueToSpell(SPELL_CORRUPTION_MALFORMATION_DUMMY);
                                player->RemoveAurasDueToSpell(SPELL_CORRUPTION_ABSOLUTE);
                                player->RemoveAurasDueToSpell(SPELL_CORRUPTION_ABSOLUTE_VISUAL);
                                player->RemoveAurasDueToSpell(SPELL_CORRUPTED_BLOOD_AURA);
                            }
                            break;
                    }
                }

                void CheckWorchiper()
                {
                    Map::PlayerList const &pList = me->GetMap()->GetPlayers();
                    if (pList.isEmpty())
                        return;

                    for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
                    {
                        Player* player = itr->GetSource();
                    }
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_chogallAI>(creature);
        }
};

class npc_chogall_fire_portal : public CreatureScript
{
    public:
        npc_chogall_fire_portal() : CreatureScript("npc_chogall_fire_portal") { }

        struct npc_chogall_fire_portalAI : public ScriptedAI
        {
            npc_chogall_fire_portalAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
                SetCombatMovement(false);
            }

            EventMap events;

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                events.ScheduleEvent(EVENT_ORDER_SUM, 3000);
            }

            void Reset() override { }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ORDER_SUM:
                            DoCast(me, SPELL_FLAME_ORDERS_SUM_4);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_chogall_fire_portalAI>(creature);
        }
};

class npc_chogall_shadow_portal : public CreatureScript
{
    public:
        npc_chogall_shadow_portal() : CreatureScript("npc_chogall_shadow_portal") { }

        struct npc_chogall_shadow_portalAI : public ScriptedAI
        {
            npc_chogall_shadow_portalAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
                SetCombatMovement(false);
            }

            EventMap events;

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                events.ScheduleEvent(EVENT_ORDER_SUM, 3000);
            }

            void Reset() override  { }

            void JustDied(Unit* /*killer*/) override { }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ORDER_SUM:
                            DoCast(me, SPELL_SHADOW_ORDERS_SUM_4);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_chogall_shadow_portalAI>(creature);
        }
};

class npc_chogall_fire_elemental : public CreatureScript
{
    public:
        npc_chogall_fire_elemental() : CreatureScript("npc_chogall_fire_elemental") { }

        struct npc_chogall_fire_elementalAI : public ScriptedAI
        {
            npc_chogall_fire_elementalAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
                instance = creature->GetInstanceScript();
            }

            EventMap events;
            InstanceScript* instance;
            Creature* pChogall;
            bool bNear;

            void IsSummonedBy(Unit* summoner) override
            {
                bNear = false;
                pChogall = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_CHOGALL));
                if (!pChogall)
                    me->DespawnOrUnsummon();
                DoCast(me, SPELL_FIRE_SHELL);
                events.ScheduleEvent(EVENT_ORDER_MOVE, 1000);
            }

            void Reset() override  { }

            void UpdateAI(uint32 diff) override
            {
                if (instance->GetBossState(DATA_CHOGALL) != IN_PROGRESS)
                    me->DespawnOrUnsummon();

                events.Update(diff);

                if (me->GetDistance(pChogall) < 1.0f && !bNear)
                {
                    bNear = true;
                    me->GetMotionMaster()->Clear();
                    pChogall->CastSpell(pChogall, SPELL_FIRE_DESTRUCTION, true);
                    me->DespawnOrUnsummon();
                    return;
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ORDER_MOVE:
                            if (pChogall)
                                me->GetMotionMaster()->MoveFollow(pChogall, 1.0f, 0.0f);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_chogall_fire_elementalAI>(creature);
        }
};

class npc_chogall_shadow_lord : public CreatureScript
{
    public:
        npc_chogall_shadow_lord() : CreatureScript("npc_chogall_shadow_lord") { }

        struct npc_chogall_shadow_lordAI : public ScriptedAI
        {
            npc_chogall_shadow_lordAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            EventMap events;
            Creature* pChogall;
            bool bNear;

            void IsSummonedBy(Unit* summoner) override
            {
                bNear = false;
                pChogall = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_CHOGALL));
                if (!pChogall)
                    me->DespawnOrUnsummon();
                DoCast(me, SPELL_SHADOW_SHELL);
                events.ScheduleEvent(EVENT_ORDER_MOVE, 1000);
            }

            void Reset() override  { }

            void UpdateAI(uint32 diff) override
            {
                if (instance->GetBossState(DATA_CHOGALL) != IN_PROGRESS)
                    me->DespawnOrUnsummon();

                events.Update(diff);

                if (me->GetDistance(pChogall) < 1.0f && !bNear)
                {
                    bNear = true;
                    me->GetMotionMaster()->Clear();
                    pChogall->CastSpell(pChogall, SPELL_EMPOWERED_SHADOWS, true);
                    me->DespawnOrUnsummon();
                    return;
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ORDER_MOVE:
                            if (pChogall)
                                me->GetMotionMaster()->MoveFollow(pChogall, 1.0f, 0.0f);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_chogall_shadow_lordAI>(creature);
        }
};

class npc_chogall_blaze : public CreatureScript
{
    public:
        npc_chogall_blaze() : CreatureScript("npc_chogall_blaze") { }

        struct npc_chogall_blazeAI : public ScriptedAI
        {
            npc_chogall_blazeAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
                SetCombatMovement(false);
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                DoCast(me, SPELL_BLAZE_PERIODIC_TRIGGER);
            }

            void Reset() override { }

            void JustDied(Unit* /*killer*/) override { }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_chogall_blazeAI>(creature);
        }
};

class npc_chogall_corruption : public CreatureScript
{
    public:
        npc_chogall_corruption() : CreatureScript("npc_chogall_corruption") { }

        struct npc_chogall_corruptionAI : public ScriptedAI
        {
            npc_chogall_corruptionAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
                SetCombatMovement(false);
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                DoCast(me, SPELL_CORRUPTION_OF_THE_OLD_GOD_VISUAL);
            }

            void Reset() override { }

            void JustDied(Unit* /*killer*/) override { }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_chogall_corruptionAI>(creature);
        }
};

class npc_chogall_darkened_creation : public CreatureScript
{
    public:
        npc_chogall_darkened_creation() : CreatureScript("npc_chogall_darkened_creation") { }

        struct npc_chogall_darkened_creationAI : public ScriptedAI
        {
            npc_chogall_darkened_creationAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                SetCombatMovement(false);
            }

            InstanceScript* instance;
            EventMap events;

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                DoZoneInCombat(me);
                events.ScheduleEvent(EVENT_DEBILITATING_BEAM, 2000);
            }

            void Reset() override { }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                    if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_DEBILITATING_BEAM)
                        for (uint8 i = 0; i < 3; ++i)
                            if (spell->Effects[i].Effect == SPELL_EFFECT_INTERRUPT_CAST)
                                me->InterruptSpell(CURRENT_GENERIC_SPELL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance->GetBossState(DATA_CHOGALL) != IN_PROGRESS)
                    me->DespawnOrUnsummon();

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DEBILITATING_BEAM:
                            DoCast(me->GetVictim(), SPELL_DEBILITATING_BEAM);
                            events.ScheduleEvent(EVENT_DEBILITATING_BEAM, 11600);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_chogall_darkened_creationAI>(creature);
        }
};

class npc_chogall_malformation : public CreatureScript
{
    public:
        npc_chogall_malformation() : CreatureScript("npc_chogall_malformation") { }

        struct npc_chogall_malformationAI : public ScriptedAI
        {
            npc_chogall_malformationAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                instance = creature->GetInstanceScript();
                SetCombatMovement(false);
            }

            InstanceScript* instance;
            EventMap events;

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                DoZoneInCombat(me);
                events.ScheduleEvent(EVENT_SHADOW_BOLT, 2000);
            }

            void Reset() override  { }

            void JustDied(Unit* /*killer*/) override
            {
                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance->GetBossState(DATA_CHOGALL) != IN_PROGRESS)
                {
                    if (Unit* player = me->GetVehicleBase())
                    {
                        player->RemoveAurasDueToSpell(SPELL_CORRUPTION_ACCELERATED);
                        player->RemoveAurasDueToSpell(SPELL_CORRUPTION_SICKNESS);
                        player->RemoveAurasDueToSpell(SPELL_CORRUPTION_MALFORMATION_VEHICLE);
                        player->RemoveAurasDueToSpell(SPELL_CORRUPTION_MALFORMATION_DUMMY);
                        player->RemoveAurasDueToSpell(SPELL_CORRUPTION_ABSOLUTE);
                        player->RemoveAurasDueToSpell(SPELL_CORRUPTION_ABSOLUTE_VISUAL);
                    }
                    me->DespawnOrUnsummon();
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
                        case EVENT_SHADOW_BOLT:
                            me->CastCustomSpell(SPELL_SHADOW_BOLT, SPELLVALUE_MAX_TARGETS, 1, 0, false);
                            events.ScheduleEvent(EVENT_SHADOW_BOLT, 10000);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_chogall_malformationAI>(creature);
        }
};

class npc_chogall_corrupting_adherent : public CreatureScript
{
    public:
        npc_chogall_corrupting_adherent() : CreatureScript("npc_chogall_corrupting_adherent") { }

        struct npc_chogall_corrupting_adherentAI : public ScriptedAI
        {
            npc_chogall_corrupting_adherentAI(Creature* creature) : ScriptedAI(creature)
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
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            EventMap events;

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                events.ScheduleEvent(EVENT_DEPRAVITY, urand(19000, 21000));
                events.ScheduleEvent(EVENT_CORRUPTING_CRASH, urand(5000, 8000));
            }

            void Reset() override 
            {
                events.Reset();
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_DEPRAVITY)
                    for (uint8 i = 0; i < 3; ++i)
                        if (spell->Effects[i].Effect == SPELL_EFFECT_INTERRUPT_CAST)
                            me->InterruptSpell(CURRENT_GENERIC_SPELL);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    events.Reset();
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DISABLE_MOVE);
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                    me->SetHealth(me->GetMaxHealth());
                    me->RemoveAllAuras();
                    events.ScheduleEvent(EVENT_SPILLED_BLOOD, 2000);
                }
            }

            void JustSummoned(Creature* summon) override
            {
                DoZoneInCombat(summon);
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
                        case EVENT_SPILLED_BLOOD:
                            DoCast(me, SPELL_SPILLED_BLOOD_OF_THE_OLD_GOD);
                            break;
                        case EVENT_DEPRAVITY:
                            DoCast(me, SPELL_DEPRAVITY);
                            events.ScheduleEvent(EVENT_DEPRAVITY, urand(12000, 13000));
                            break;
                        case EVENT_CORRUPTING_CRASH:
                            Unit* target = CheckPlayersInRange(RAID_MODE<uint8>(4, 8), 20.0f, 50.0f);
                            if (!target)
                                target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true);
                            DoCast(target, SPELL_CORRUPTING_CRASH);
                            events.ScheduleEvent(EVENT_CORRUPTING_CRASH, urand(12000, 15000));
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            Unit* CheckPlayersInRange(uint8 playersMin, float rangeMin, float rangeMax)
            {
                Map* map = me->GetMap();
                if (map && map->IsDungeon())
                {
                    std::list<Player*> PlayerList;
                    Map::PlayerList const& Players = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
                    {
                        if (Player* player = itr->GetSource())
                        {
                            float distance = player->GetDistance(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                            if (rangeMin > distance || distance > rangeMax)
                                continue;
                            PlayerList.push_back(player);
                        }
                    }

                    if (PlayerList.empty())
                        return NULL;

                    size_t size = PlayerList.size();
                    if (size < playersMin)
                        return NULL;

                    return SelectRandomContainerElement(PlayerList);
                }
                return NULL;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_chogall_corrupting_adherentAI>(creature);
        }
};

class npc_chogall_blood_of_the_old_god : public CreatureScript
{
    public:
        npc_chogall_blood_of_the_old_god() : CreatureScript("npc_chogall_blood_of_the_old_god") { }

        struct npc_chogall_blood_of_the_old_godAI : public ScriptedAI
        {
            npc_chogall_blood_of_the_old_godAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetSpeed(MOVE_RUN, 0.5f);
                me->SetSpeed(MOVE_WALK, 0.5f);
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            void IsSummonedBy(Unit* /*summoner*/) override { }

            void Reset() override  { }

            void DamageDealt(Unit* victim, uint32& /*damage*/, DamageEffectType type) override
            {
                if (type == DIRECT_DAMAGE)
                    AddCorruption(victim, 2);
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (instance->GetBossState(DATA_CHOGALL) != IN_PROGRESS)
                    me->DespawnOrUnsummon();

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_chogall_blood_of_the_old_godAI>(creature);
        }
};

class spell_chogall_conversion : public SpellScriptLoader
{
    public:
        spell_chogall_conversion() : SpellScriptLoader("spell_chogall_conversion") { }

        class spell_chogall_conversion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_chogall_conversion_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                if (GetCaster()->GetVictim() != GetHitUnit())
                    GetHitUnit()->CastSpell(GetHitUnit(), SPELL_WORSHIPPING, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_chogall_conversion_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_chogall_conversion_SpellScript();
        }
};

class spell_chogall_summon_corrupting_adherent : public SpellScriptLoader
{
    public:
        spell_chogall_summon_corrupting_adherent() : SpellScriptLoader("spell_chogall_summon_corrupting_adherent") { }

        class spell_chogall_summon_corrupting_adherent_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_chogall_summon_corrupting_adherent_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster())
                    return;

                GetCaster()->CastSpell(GetCaster(), urand(0, 1) ? SPELL_SUMMON_CORRUPTING_ADHERENT_1 : SPELL_SUMMON_CORRUPTING_ADHERENT_2, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_chogall_summon_corrupting_adherent_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_chogall_summon_corrupting_adherent_SpellScript();
        }
};

class spell_chogall_fester_blood_script : public SpellScriptLoader
{
    public:
        spell_chogall_fester_blood_script() : SpellScriptLoader("spell_chogall_fester_blood_script") { }

        class spell_chogall_fester_blood_script_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_chogall_fester_blood_script_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster())
                    return;

                for (uint8 i = 0; i < 5; i++)
                    GetCaster()->CastSpell(GetCaster(), SPELL_FESTERED_BLOOD_SUM, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_chogall_fester_blood_script_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_chogall_fester_blood_script_SpellScript();
        }
};

class spell_chogall_corruption_accelerated_corruption : public SpellScriptLoader
{
    public:
        spell_chogall_corruption_accelerated_corruption() : SpellScriptLoader("spell_chogall_corruption_accelerated_corruption") { }

        class spell_chogall_corruption_accelerated_corruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_chogall_corruption_accelerated_corruption_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster())
                    return;

                AddCorruption(GetCaster(), 2);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_chogall_corruption_accelerated_corruption_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_chogall_corruption_accelerated_corruption_SpellScript();
        }
};

class spell_chogall_corruption_sickness_corruption : public SpellScriptLoader
{
    public:
        spell_chogall_corruption_sickness_corruption() : SpellScriptLoader("spell_chogall_corruption_sickness_corruption") { }

        class spell_chogall_corruption_sickness_corruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_chogall_corruption_sickness_corruption_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                if (GetCaster() != GetHitUnit())
                    AddCorruption(GetHitUnit(), 5);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_chogall_corruption_sickness_corruption_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_chogall_corruption_sickness_corruption_SpellScript();
        }
};

class spell_chogall_corrupting_crash_corruption : public SpellScriptLoader
{
    public:
        spell_chogall_corrupting_crash_corruption() : SpellScriptLoader("spell_chogall_corrupting_crash_corruption") { }

        class spell_chogall_corrupting_crash_corruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_chogall_corrupting_crash_corruption_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetHitUnit())
                    return;

                AddCorruption(GetHitUnit(), 10);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_chogall_corrupting_crash_corruption_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_chogall_corrupting_crash_corruption_SpellScript();
        }
};

class spell_chogall_depravity_corruption : public SpellScriptLoader
{
    public:
        spell_chogall_depravity_corruption() : SpellScriptLoader("spell_chogall_depravity_corruption") { }

        class spell_chogall_depravity_corruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_chogall_depravity_corruption_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetHitUnit())
                    return;

                AddCorruption(GetHitUnit(), 10);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_chogall_depravity_corruption_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_chogall_depravity_corruption_SpellScript();
        }
};

class spell_chogall_sprayed_corruption_corruption : public SpellScriptLoader
{
    public:
        spell_chogall_sprayed_corruption_corruption() : SpellScriptLoader("spell_chogall_sprayed_corruption_corruption") { }

        class spell_chogall_sprayed_corruption_corruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_chogall_sprayed_corruption_corruption_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetHitUnit())
                    return;

                AddCorruption(GetHitUnit(), 10);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_chogall_sprayed_corruption_corruption_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_chogall_sprayed_corruption_corruption_SpellScript();
        }
};

class spell_chogall_spilled_blood_of_the_old_god_corruption : public SpellScriptLoader
{
    public:
        spell_chogall_spilled_blood_of_the_old_god_corruption() : SpellScriptLoader("spell_chogall_spilled_blood_of_the_old_god_corruption") { }

        class spell_chogall_spilled_blood_of_the_old_god_corruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_chogall_spilled_blood_of_the_old_god_corruption_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetHitUnit())
                    return;

                AddCorruption(GetHitUnit(), 5);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_chogall_spilled_blood_of_the_old_god_corruption_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_chogall_spilled_blood_of_the_old_god_corruption_SpellScript();
        }
};

class spell_chogall_corruption_of_the_old_god_corruption : public SpellScriptLoader
{
    public:
        spell_chogall_corruption_of_the_old_god_corruption() : SpellScriptLoader("spell_chogall_corruption_of_the_old_god_corruption") { }

        class spell_chogall_corruption_of_the_old_god_corruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_chogall_corruption_of_the_old_god_corruption_SpellScript);

            void OnCast()
            {
                if (!GetHitUnit())
                    return;

                uint32 corruption = GetHitUnit()->GetPower(POWER_ALTERNATE_POWER);
                if (corruption < 1)
                    corruption = 1;
                uint32 damage = GetHitDamage() * (1 + 0.02 * corruption);
                PreventHitDamage();
                SetHitDamage(damage);
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetHitUnit())
                    return;

                AddCorruption(GetHitUnit(), 1);
            }

            void Register() override
            {
                BeforeHit += SpellHitFn(spell_chogall_corruption_of_the_old_god_corruption_SpellScript::OnCast);
                OnEffectHitTarget += SpellEffectFn(spell_chogall_corruption_of_the_old_god_corruption_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_chogall_corruption_of_the_old_god_corruption_SpellScript();
        }
};

class spell_chogall_worshipping : public SpellScriptLoader
{
    public:
        spell_chogall_worshipping() : SpellScriptLoader("spell_chogall_worshipping") { }

        class spell_chogall_worshipping_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_chogall_worshipping_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetTarget())
                    return;

                if (GetTarget()->GetTypeId() == TYPEID_PLAYER)
                    GetTarget()->ToPlayer()->SetClientControl(GetTarget(), 0);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetTarget())
                    return;

                if (GetTarget()->GetTypeId() == TYPEID_PLAYER)
                    GetTarget()->ToPlayer()->SetClientControl(GetTarget(), 1);
            }
            
            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_chogall_worshipping_AuraScript::OnApply, EFFECT_1, SPELL_AURA_MOD_FACTION, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_chogall_worshipping_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_MOD_FACTION, AURA_EFFECT_HANDLE_REAL);
            
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_chogall_worshipping_AuraScript();
        }
};

void AddSC_boss_chogall()
{
    new boss_chogall();
    new npc_chogall_fire_portal();
    new npc_chogall_shadow_portal();
    new npc_chogall_fire_elemental();
    new npc_chogall_shadow_lord();
    new npc_chogall_blaze();
    new npc_chogall_corrupting_adherent();
    new npc_chogall_blood_of_the_old_god();
    new npc_chogall_darkened_creation();
    new npc_chogall_corruption();
    new npc_chogall_malformation();
    new spell_chogall_conversion();
    new spell_chogall_summon_corrupting_adherent();
    new spell_chogall_fester_blood_script();
    new spell_chogall_corruption_accelerated_corruption();
    new spell_chogall_corruption_sickness_corruption();
    new spell_chogall_corrupting_crash_corruption();
    new spell_chogall_depravity_corruption();
    new spell_chogall_sprayed_corruption_corruption();
    new spell_chogall_spilled_blood_of_the_old_god_corruption();
    new spell_chogall_corruption_of_the_old_god_corruption();
    new spell_chogall_worshipping();
}