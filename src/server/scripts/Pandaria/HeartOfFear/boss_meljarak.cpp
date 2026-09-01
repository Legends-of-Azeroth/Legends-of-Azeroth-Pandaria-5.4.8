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
#include "heart_of_fear.h"

enum Yells
{
    SAY_INTRO            = 0, // Your defiance of the empress ends here!

    SAY_AGGRO            = 1, // All of Pandaria will fall beneath the Wings of the Empress!
    SAY_SLAY             = 2, // 0 - The Empress commands it!; 1 - Pitiful.
    SAY_DEATH            = 3, // Empress... I have... failed you...

    SAY_WHIRLING_BLADES  = 4, // My blade never misses its mark!
    SAY_RAIN_OF_BLADES   = 5, // The skies belong to the Empress!

    SAY_WATCHFUL_EYE     = 6, // 0 - You fight like cowards!; 1 - Aid me brethren!

    SAY_ADD_GROUP_DIES   = 7, // 0 - To die for the empress is an honor!; 1 - The wings of the empress cannot fail!; 2 - You will pay for your transgressions!
    SAY_SUMMON_REINFORCE = 8,  // 0 - The Kor'thik have no equal in combat!; 1 - The mighty Zar'thik bend the wind to their will!; 2 - The Sra'thik command the elements of this land, leaving their foes imprisoned for all time!

    ANN_WHIRLING_BLADES  = 9, // Wind Lord Mel'jarak begins to cast [Whirling Blades]!
    ANN_RAIN_OF_BLADES   = 10,// Wind Lord Mel'jarak begins to cast [Rain of Blades]!
    ANN_REINFORCEMENTS   = 11 // Wind Lord Mel'jarak calls for reinforcements!
};

enum Spells
{
    SPELL_WHIRLING_BLADE             = 121896, // Triggers 121898 damage every 0.25s and 121897, 122083 dummy location spells (visuals?).
    SPELL_WHIRLING_BLADE_DUMMY       = 121897,
    SPELL_WHIRLING_BLADE_DUMMY_AURA  = 122083,
    SPELL_RAIN_OF_BLADES             = 122406, // Triggers 122407 damage spell every 0.5s.
    SPELL_WIND_BOMB                  = 131813, // Triggers 131814 bomb summon npc 67053.
    SPELL_WIND_BOMB_SUMM             = 131814,

    SPELL_WATCHFUL_EYE_1             = 125933, // Starting Normal and always Heroic aura - 9 adds alive, can control 4 / 3 on Heroic.
    SPELL_WATCHFUL_EYE_2             = 125935, // 6 adds alive, can control 2.
    SPELL_WATCHFUL_EYE_3             = 125936, // 3 adds alive, can control 0.

    SPELL_RECKLESNESS_N              = 122354, // Normal stackable version.
    SPELL_RECKLESNESS_H              = 125873, // Heroic 30 sec version.

    SPELL_BERSERK_MELJARAK           = 120207, // 480 seconds or 8 mins Enrage.

    SPELL_IMP_SPEAR_ABIL             = 122220, // Aura player receives when clicking on the racks. Gives Action Bar with button 1 to throw spear.
    SPELL_IMP_SPEAR                  = 122224, // Incapacitates an add for 50 seconds, damage breaks it.

    // Wind Bomb
    SPELL_WIND_BOMB_THR_DMG          = 131830, // Damage in 5 yards at throwing.
    SPELL_WIND_BOMB_ARM              = 131835, // Visual bomb arm.
    SPELL_WIND_BOMB_EXPLODE          = 131842, // If a player goes in 6 yards (on off checked with 131836).

    // The Swarm
    SPELL_FATE_OF_THE_KORT           = 121774, // Share Damage spell for Kor'thik Elite Blademasters.
    SPELL_FATE_OF_THE_SRAT           = 121802, // Share Damage spell for Sra'thik Amber-Trappers.
    SPELL_FATE_OF_THE_ZART           = 121807, // Share Damage spell for Zar'thik Battle-Menders.

    // - Kor'thik Elite Blademaster
    SPELL_KORTHIK_STRIKE             = 123962, // All 3 use this at once on the same player.
    SPELL_KORTHIK_STRIKE_AURA        = 123963, // Lable
    SPELL_KORTHIK_STRIKE_CHARGE      = 122409, // Req lable

    // - Sra'thik Amber-Trapper
    SPELL_AMBER_PRISON               = 121876, // Initial cast, triggers 121881 after 3 seconds.
    SPELL_AMBER_PRISON_TRIG          = 121881, // Triggers 121874 cast.
    SPELL_AMBER_PRISON_FC            = 121874, // Triggers 121885 aura.
    SPELL_AMBER_PRISON_AURA          = 121885, // Stun, root, visual etc. !ADD THIS: /* insert into spell_linked_spell values (121885, 129078, 1, 'Summon Amber Prison when hit by aura'); */
    SPELL_AMBER_PRISON_SUMM          = 129078, // Summons 62531 Amber Prison npc.

    SPELL_RESIDUE                    = 122055, // Residue afflicts players who destroy Amber Prisons.

    SPELL_CORROSIVE_RESIN            = 122064,

    // !-  Moving while afflicted by Corrosive Resin removes a stack of Corrosive Resin and creates a Corrosive Resin Pool at the player's location. -!
    SPELL_CORR_RESIN_POOL_S          = 122123, // Summons Corrosive Resin Pool npc 67046.
    SPELL_CORR_RESIN_POOL_A          = 129005, // Aura for Corrosive Resin Pool, triggers 122125 dmg each sec.

    // - Zar'thik Battle-Mender
    SPELL_MENDING                    = 122193, // Triggers a 200 yard dummy + heal spell 122147.
    SPELL_QUICKENING                 = 122149,  // All Swarm gets boost 25% dmg + as.
    SPELL_MENDING_EX_AURA            = 122185,
};

enum Events
{
    EVENT_WHIRLING_BLADE        = 1, // 36 secs after pull
    EVENT_RAIN_OF_BLADES        = 2,         // 60 secs after pull
    EVENT_WIND_BOMB             = 3,

    EVENT_CHECK_ADD_CC_DEATH    = 4,     // Checks how many adds are dead / CC'ed.
    EVENT_SUMMON_REINFORCEMENTS = 5,

    EVENT_BERSERK_MELJARAK      = 6,

    // Wind Bomb
    EVENT_ARM                   = 7,                    // 3 secs after spawn
    EVENT_CHECK_PLAYER          = 8,           // Check if needs to explode.

    // The Swarm

    // - Kor'thik Elite Blademaster
    EVENT_KORTHIK_STRIKE        = 9,         // 19s after pull.

    // - Sra'thik Amber-Trapper
    EVENT_AMBER_PRISON          = 10,
    EVENT_CORROSIVE_RESIN       = 11,

    // - Zar'thik Battle-Mender
    EVENT_MENDING               = 12,
    EVENT_QUICKENING            = 13,

    // Heroic
    EVENT_LANDING               = 14,
    EVENT_KORTHIK_STRIKE_CHARGE = 15,
};

enum Actions
{
    ACTION_WHIRLING_BLADE  = 0,
    ACTION_SWARM_IN_COMBAT = 1,
    ACTION_SET_SPAWN_SWARM = 2,
    ACTION_RESET_SPAWN     = 3,
    ACTION_SWARM_TYPE_DIED = 4,
};

enum Misc
{
    GROUP_ALIVE  = 0,
    GROUP_KILLED = 1,

    WORLD_STATE_ACHIEVEMENT_LESS_THAN_THREE = 12003,
};

void SetEncounterUnitForEachGroup(Creature* me, ObjectGuid ownerGUID, InstanceScript* _instance, uint32 type, uint32 diffType = 0)
{
    Unit* owner = ObjectAccessor::GetUnit(*me, ownerGUID);

    if (!owner)
        return;

    std::list<Creature*> Swarm;

    switch (type)
    {
        case ENCOUNTER_FRAME_ENGAGE:
            // When we should send frame engage only for new group
            if (diffType)
            {
                if (Creature* SwarmType = GetClosestCreatureWithEntry(owner, diffType, 100.0f, true))
                    _instance->SendEncounterUnit(type, SwarmType);

                break;
            }
            if (Creature* Korthik = GetClosestCreatureWithEntry(owner, NPC_KORTHIK_ELITE_BLADEMASTER, 100.0f, true))
                _instance->SendEncounterUnit(type, Korthik);

            if (Creature* Zarthik = GetClosestCreatureWithEntry(owner, NPC_ZARTHIK_BATTLE_MENDER, 100.0f, true))
                _instance->SendEncounterUnit(type, Zarthik);

            if (Creature* Srathik = GetClosestCreatureWithEntry(owner, NPC_SRATHIK_AMBER_TRAPPER, 100.0f, true))
                _instance->SendEncounterUnit(type, Srathik);
            break;
        case ENCOUNTER_FRAME_DISENGAGE:
            GetCreatureListWithEntryInGrid(Swarm, owner, NPC_KORTHIK_ELITE_BLADEMASTER, 100.0f);
            GetCreatureListWithEntryInGrid(Swarm, owner, NPC_ZARTHIK_BATTLE_MENDER, 100.0f);
            GetCreatureListWithEntryInGrid(Swarm, owner, NPC_SRATHIK_AMBER_TRAPPER, 100.0f);
            for (auto&& itr : Swarm)
                _instance->SendEncounterUnit(type, itr);
            break;
    }
}

class boss_wind_lord_meljarak : public CreatureScript
{
    public:
        boss_wind_lord_meljarak() : CreatureScript("boss_wind_lord_meljarak") { }

        struct boss_wind_lord_meljarakAI : public BossAI
        {
            boss_wind_lord_meljarakAI(Creature* creature) : BossAI(creature, DATA_MELJARAK) { }

            bool WindBomb, HasReck;
            uint32 recklessnessCounter, delay;
            uint32 _checkControlledTimer = 1000;
            ObjectGuid targetGUID, strikeTargetGUID;
            std::map <uint32, uint32> groupDict;

            void Reset() override
            {
                _Reset();
                events.Reset();
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveAllAuras();
                me->AddAura(SPELL_WATCHFUL_EYE_1, me);
                summons.DespawnAll();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    SetEncounterUnitForEachGroup(me, me->GetGUID(), instance, ENCOUNTER_FRAME_DISENGAGE);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }
                WindBomb            = false;
                HasReck             = false;
                strikeTargetGUID = ObjectGuid::Empty;
                recklessnessCounter = 0;
                targetGUID = ObjectGuid::Empty;
                delay               = 0;
                ResetSwarm();
                HandleResetSpears();

                if (IsHeroic())
                    SendResetSwarmSpawners();

                if (instance->GetBossState(DATA_MELJARAK) != DONE)
                    SpawnSwarm();

                groupDict.clear();
                groupDict.insert(std::pair<uint32, uint32>(NPC_ZARTHIK_BATTLE_MENDER,     GROUP_ALIVE));
                groupDict.insert(std::pair<uint32, uint32>(NPC_SRATHIK_AMBER_TRAPPER,     GROUP_ALIVE));
                groupDict.insert(std::pair<uint32, uint32>(NPC_KORTHIK_ELITE_BLADEMASTER, GROUP_ALIVE));

                me->GetMap()->SetWorldState(WORLD_STATE_ACHIEVEMENT_LESS_THAN_THREE, 1);
            }

            void InitializeAI() override
            {
                SetEquipmentSlots(false, MELJARAK_2H_WEAPON, 0, EQUIP_NO_CHANGE);
                Reset();
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                SetEncounterUnitForEachGroup(me, me->GetGUID(), instance, ENCOUNTER_FRAME_DISENGAGE);
                HandleResetSpears();
                RemoveTraps();

                if (IsHeroic())
                    SendResetSwarmSpawners();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_IMP_SPEAR_ABIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORROSIVE_RESIN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_AMBER_PRISON_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RESIDUE);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_MELJARAK, FAIL);
                    summons.DespawnAll();
                }

                _DespawnAtEvade();
                me->GetMotionMaster()->MoveTargetedHome();
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                if (instance)
                    instance->SetBossState(DATA_MELJARAK, FAIL);
            }

            void JustEngagedWith(Unit* who) override
            {
                // @TODO: Set in combat for other protectors
                if (!instance->CheckRequiredBosses(DATA_MELJARAK, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    who->NearTeleportTo(EncountersEntrance[0].GetPositionX(), EncountersEntrance[0].GetPositionY(), EncountersEntrance[0].GetPositionZ(), EncountersEntrance[0].GetOrientation());
                    return;
                }

                _JustEngagedWith();

                std::list<Creature*> swarm = GetSwarmList();
                for (auto&& itr : swarm)
                    itr->AI()->DoZoneInCombat();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                    instance->SetBossState(DATA_MELJARAK, IN_PROGRESS);
                    SetEncounterUnitForEachGroup(me, me->GetGUID(), instance, ENCOUNTER_FRAME_ENGAGE);
                }
                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_WHIRLING_BLADE, 18000);
                events.ScheduleEvent(EVENT_RAIN_OF_BLADES, 30000);
                events.ScheduleEvent(EVENT_BERSERK_MELJARAK, 8 * MINUTE * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_KORTHIK_STRIKE, 19000);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->setActive(true);

                if (me->IsInCombat())
                    summon->AI()->DoZoneInCombat();
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;
            }

            void DoAction(int32 actionId) override 
            {
                switch (actionId)
                {
                    case ACTION_WHIRLING_BLADE:
                        me->RemoveChanneledCast(targetGUID);
                        me->RemoveAurasDueToSpell(SPELL_WHIRLING_BLADE);
                        break;
                    case NPC_KORTHIK_ELITE_BLADEMASTER:
                    case NPC_ZARTHIK_BATTLEMENDER:
                    case NPC_SRATHIK_AMBER_TRAPPER:
                        if (groupDict.empty())
                            break;

                        if (groupDict.find(actionId)->second == GROUP_ALIVE)
                        {
                            groupDict.find(actionId)->second = GROUP_KILLED;

                            DoCast(me, IsHeroic() ? SPELL_RECKLESNESS_H : SPELL_RECKLESNESS_N, true);

                            // Prevent Spam voice in Heroic if aura yet available
                            if (IsHeroic() && me->HasAura(SPELL_RECKLESNESS_H))
                                break;

                            Talk(SAY_ADD_GROUP_DIES);

                            // In heroic available only SPELL_WATCHFUL_EYE_1 not depend of group death
                            if (IsHeroic())
                                break;

                            if (me->HasAura(SPELL_WATCHFUL_EYE_1))
                            {
                                me->RemoveAurasDueToSpell(SPELL_WATCHFUL_EYE_1);
                                me->AddAura(SPELL_WATCHFUL_EYE_2, me);
                                break;
                            }

                            if (me->HasAura(SPELL_WATCHFUL_EYE_2))
                            {
                                me->RemoveAurasDueToSpell(SPELL_WATCHFUL_EYE_2);
                                me->AddAura(SPELL_WATCHFUL_EYE_3, me);
                            }
                        }
                        break;
                        // In Heroic Remove Death state if swarm were reset
                    case NPC_KORTHIK_ELITE_BLADEMASTER + 5:
                    case NPC_ZARTHIK_BATTLEMENDER + 5:
                    case NPC_SRATHIK_AMBER_TRAPPER + 5:
                        groupDict.find(actionId - 5)->second = GROUP_ALIVE;
                        break;
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (me->HealthBelowPct(75) && !WindBomb)
                {
                    WindBomb = true;
                    events.ScheduleEvent(EVENT_WIND_BOMB, 5000);
                }
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_SLAY);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    SetEncounterUnitForEachGroup(me, me->GetGUID(), instance, ENCOUNTER_FRAME_DISENGAGE);
                    instance->SetBossState(DATA_MELJARAK, DONE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_IMP_SPEAR_ABIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORROSIVE_RESIN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_AMBER_PRISON_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RESIDUE);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }
                ResetSwarm();
                HandleResetSpears(true);
                RemoveTraps();

                if (IsHeroic())
                    SendResetSwarmSpawners();
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_MELJARAK_RECKLESSNES)
                    return recklessnessCounter;

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_MELJARAK_RECKLESSNES)
                    recklessnessCounter = data;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (_checkControlledTimer <= diff)
                {
                    ControllAreaType();
                    _checkControlledTimer = 1000;
                }
                else _checkControlledTimer -= diff;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_WHIRLING_BLADE:
                            Talk(SAY_WHIRLING_BLADES);

                            if (Unit* victim = me->GetVictim())
                                targetGUID = victim->GetGUID();

                            me->CastSpell(me, SPELL_WHIRLING_BLADE, false);
                            events.ScheduleEvent(EVENT_WHIRLING_BLADE, urand(48000, 64000));
                            break;
                        case EVENT_RAIN_OF_BLADES:
                            Talk(SAY_RAIN_OF_BLADES);
                            Talk(ANN_RAIN_OF_BLADES);

                            if (Unit* target = me->GetVictim())
                            {
                                targetGUID = target->GetGUID();
                                me->PrepareChanneledCast(me->GetOrientation(), SPELL_RAIN_OF_BLADES);

                                delay = 0;
                                me->m_Events.Schedule(delay += 6500, 20, [this]()
                                {
                                    me->RemoveChanneledCast(targetGUID);
                                });
                            }
                            events.ScheduleEvent(EVENT_RAIN_OF_BLADES, urand(48000, 64000));
                            break;
                        case EVENT_WIND_BOMB:
                            DoCast(me, SPELL_WIND_BOMB);
                            events.ScheduleEvent(EVENT_WIND_BOMB, urand(18000, 24000));
                            break;
                        case EVENT_BERSERK_MELJARAK:
                            DoCast(me, SPELL_BERSERK_MELJARAK);
                            break;
                        case EVENT_KORTHIK_STRIKE:
                            // Available only of this group is alive. but continue schedule event because heroic
                            if (Creature* korthik = GetClosestCreatureWithEntry(me, NPC_KORTHIK_ELITE_BLADEMASTER, 200.0f, true))
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                {
                                    strikeTargetGUID = target->GetGUID();
                                    me->AddAura(SPELL_KORTHIK_STRIKE_AURA, target);
                                }
                                else if (Unit* target = me->GetVictim())
                                {
                                    strikeTargetGUID = target->GetGUID();
                                    me->AddAura(SPELL_KORTHIK_STRIKE_AURA, target);
                                }

                                events.ScheduleEvent(EVENT_KORTHIK_STRIKE_CHARGE, urand(2 * IN_MILLISECONDS, 9 * IN_MILLISECONDS));
                            }

                            events.ScheduleEvent(EVENT_KORTHIK_STRIKE, urand(19000, 29000));
                            break;
                        case EVENT_KORTHIK_STRIKE_CHARGE:
                            if (strikeTargetGUID)
                                EliteBattleMastersCastStrike(strikeTargetGUID);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff);

            }

            private:
                std::list <Creature*> GetSwarmList()
                {
                    std::list<Creature*> Swarm;

                    GetCreatureListWithEntryInGrid(Swarm, me, NPC_KORTHIK_ELITE_BLADEMASTER, 100.0f);
                    GetCreatureListWithEntryInGrid(Swarm, me, NPC_ZARTHIK_BATTLE_MENDER, 100.0f);
                    GetCreatureListWithEntryInGrid(Swarm, me, NPC_SRATHIK_AMBER_TRAPPER, 100.0f);
                    return Swarm;
                }

                void ResetSwarm()
                {
                    std::list <Creature*> Swarm = GetSwarmList();
                    
                    for (auto&& itr : Swarm)
                        itr->DespawnOrUnsummon();
                    
                }

                void SpawnSwarm()
                {
                    for (uint32 i = 0; i < 3; i++)
                    {
                        if (TempSummon* korthik = me->SummonCreature(NPC_KORTHIK_ELITE_BLADEMASTER, KorthikSpawn[i], TEMPSUMMON_MANUAL_DESPAWN))
                            korthik->SetCanFly(false);

                        if (TempSummon* zarthik = me->SummonCreature(NPC_ZARTHIK_BATTLE_MENDER, ZarthikSpawn[i], TEMPSUMMON_MANUAL_DESPAWN))
                            zarthik->SetCanFly(false);

                        if (TempSummon* srathik = me->SummonCreature(NPC_SRATHIK_AMBER_TRAPPER, SrathikSpawn[i], TEMPSUMMON_MANUAL_DESPAWN))
                            srathik->SetCanFly(false);
                    }
                }

                void EliteBattleMastersCastStrike(ObjectGuid targetGUID)
                {
                    Unit* m_target = ObjectAccessor::GetUnit(*me, targetGUID);

                    if (!m_target)
                        return;

                    std::list<Creature*> battleMasters;
                    GetCreatureListWithEntryInGrid(battleMasters, me, NPC_KORTHIK_ELITE_BLADEMASTER, 150.0f);

                    // Cast it.
                    if (!battleMasters.empty())
                        for (std::list<Creature*>::iterator iter = battleMasters.begin(); iter != battleMasters.end(); ++iter)
                            if ((*iter)->IsAlive())
                                (*iter)->CastSpell(m_target, SPELL_KORTHIK_STRIKE_CHARGE, false);
                }

                // Only Heroic
                void SendResetSwarmSpawners()
                {
                    std::list<Creature*> SwarmSpawner;
                    GetCreatureListWithEntryInGrid(SwarmSpawner, me, NPC_KORTHIK_SPAWN_HEROIC, 200.0f);
                    GetCreatureListWithEntryInGrid(SwarmSpawner, me, NPC_ZARTHIK_SPAWN_HEROIC, 200.0f);
                    GetCreatureListWithEntryInGrid(SwarmSpawner, me, NPC_SRATHIK_SPAWN_HEROIC, 200.0f);

                    for (auto&& itr : SwarmSpawner)
                        itr->AI()->DoAction(ACTION_RESET_SPAWN);
                }

                uint32 canControlCount()
                {
                    if (me->HasAura(SPELL_WATCHFUL_EYE_1))
                        return IsHeroic() ? 3 : 4;

                    if (me->HasAura(SPELL_WATCHFUL_EYE_2))
                        return 2;

                    return 0;
                }

                void ControllAreaType()
                {
                    std::list<Creature*> swarmList;
                    GetCreatureListWithEntryInGrid(swarmList, me, NPC_ZARTHIK_BATTLEMENDER, 200.0f);
                    GetCreatureListWithEntryInGrid(swarmList, me, NPC_KORTHIK_ELITE_BLADEMASTER, 200.0f);
                    GetCreatureListWithEntryInGrid(swarmList, me, NPC_SRATHIK_AMBER_TRAPPER, 200.0f);

                    uint32 controledCount = 0;

                    for (std::list<Creature*> ::iterator itr = swarmList.begin(); itr != swarmList.end(); itr++)
                        if ((*itr)->HasCrowdControlAura())
                            controledCount++;

                    // If controlled swarmers more than it possible - remove it whole
                    if (controledCount > canControlCount())
                        for (std::list<Creature*> ::iterator itr = swarmList.begin(); itr != swarmList.end(); itr++)
                            (*itr)->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TAKE_DAMAGE);
                }

                void HandleResetSpears(bool onDeath = false)
                {
                    std::list<GameObject*> mantidSpears;
                    GetGameObjectListWithEntryInGrid(mantidSpears, me, GO_MANTID_WEAPON_RACK, 200.0f);

                    for (auto&& sItr : mantidSpears)
                        onDeath ? sItr->Delete() : sItr->Respawn();
                }

                void RemoveTraps()
                {
                    std::list<Creature*> trapList;
                    GetCreatureListWithEntryInGrid(trapList, me, NPC_AMBER_PRISON, 200.0f);

                    for (auto&& itr : trapList)
                        itr->DespawnOrUnsummon();
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_wind_lord_meljarakAI(creature);
        }
};

// Meljarak swarm 65500, 65498, 65499
class npc_meljarak_swarm : public CreatureScript
{
    public:
        npc_meljarak_swarm() : CreatureScript("npc_meljarak_swarm") { }

        struct npc_meljarak_swarmAI : public ScriptedAI
        {
            npc_meljarak_swarmAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

            EventMap events, nonCombatEvents;
            uint32 timer, wp;
            bool triggeredDeath;

            void Reset() override 
            {
                events.Reset();
                triggeredDeath = false;
            }

            void IsSummonedBy(Unit* summoner) override
            {
                // Temp Hackfix
                if (_instance && _instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
                {
                    // should be 54.81m in 10 normal
                    me->SetCreateHealth(54810000);
                    me->SetMaxHealth(54810000);
                    me->SetHealth(54810000);
                    me->ResetPlayerDamageReq();
                }

                triggeredDeath = false;

                switch (me->GetEntry())
                {
                    case NPC_KORTHIK_ELITE_BLADEMASTER:
                        SetEquipmentSlots(false, KORTHIK_2H_WEAPON, 0, EQUIP_NO_CHANGE);
                        me->AddAura(SPELL_FATE_OF_THE_KORT, me);
                        break;
                    case NPC_ZARTHIK_BATTLE_MENDER:
                        SetEquipmentSlots(false, ZARTHIK_2H_WEAPON, 0, EQUIP_NO_CHANGE);
                        me->AddAura(SPELL_FATE_OF_THE_ZART, me);
                        break;
                    case NPC_SRATHIK_AMBER_TRAPPER:
                        SetEquipmentSlots(false, SRATHIK_1H_WEAPON, SRATHIK_1H_WEAPON, EQUIP_NO_CHANGE);
                        me->AddAura(SPELL_FATE_OF_THE_SRAT, me);
                        break;
                }

                if (summoner->GetEntry() == NPC_WIND_LORD_MELJARAK)
                {
                    me->SetAnimTier(AnimTier::Ground);
                    me->OverrideInhabitType(INHABIT_GROUND);
                    me->UpdateMovementFlags();
                    return;
                }

                // Send any type of swarm flying and land to platform
                Movement::MoveSplineInit init(me);
                for (auto itr : SwarmLanding)
                {
                    float x, y, z;
                    me->GetRandomPoint(itr, 3.0f, x, y, z);
                    init.Path().push_back(G3D::Vector3(x, y, z));
                }

                init.SetSmooth();
                init.SetUncompressed();
                init.Launch();

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                me->SetReactState(REACT_PASSIVE);
                nonCombatEvents.ScheduleEvent(EVENT_LANDING, me->GetSplineDuration());
            }

            void JustEngagedWith(Unit* who) override
            {
                if (Creature* meljarak = GetClosestCreatureWithEntry(me, NPC_WIND_LORD_MELJARAK, 100.0f, true))
                    if (!meljarak->GetVictim())             // Just optimization. JustEngagedWith won't be called anyway.
                        meljarak->AI()->AttackStart(who);   // Only Meljarak have to sets all the swarm in combat.

                switch (me->GetEntry())
                {
                    case NPC_SRATHIK_AMBER_TRAPPER:
                        events.ScheduleEvent(EVENT_AMBER_PRISON, urand(13000, 24000));
                        events.ScheduleEvent(EVENT_CORROSIVE_RESIN, urand(8000, 20000));
                        break;
                    case NPC_ZARTHIK_BATTLE_MENDER:
                        events.ScheduleEvent(EVENT_MENDING, urand(30000, 49000));
                        events.ScheduleEvent(EVENT_QUICKENING, urand(12000, 28000));
                        break;
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (_instance)
                    _instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                events.Reset();
                me->CombatStop();
                me->DespawnOrUnsummon(5 * IN_MILLISECONDS);

                if (Creature* meljarak = ObjectAccessor::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_MELJARAK) : ObjectGuid::Empty))
                    meljarak->AI()->DoAction(me->GetEntry());

                // Only Heroic
                if (!IsHeroic())
                    return;

                uint32 Swarmer = 0;

                switch (me->GetEntry())
                {
                    case NPC_KORTHIK_ELITE_BLADEMASTER:
                        Swarmer = NPC_KORTHIK_SPAWN_HEROIC;
                        break;
                    case NPC_ZARTHIK_BATTLE_MENDER:
                        Swarmer = NPC_ZARTHIK_SPAWN_HEROIC;
                        break;
                    case NPC_SRATHIK_AMBER_TRAPPER:
                        Swarmer = NPC_SRATHIK_SPAWN_HEROIC;
                        break;
                }

                // lauch timer on 45s for each type of group
                if (Creature* Swarm = GetClosestCreatureWithEntry(me, Swarmer, 200.0f, true))
                    Swarm->AI()->DoAction(ACTION_SET_SPAWN_SWARM);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override 
            {
                if (damage >= me->GetHealth() && !triggeredDeath)
                {
                    triggeredDeath = true;
                    me->LowerPlayerDamageReq(me->GetMaxHealth()); // Allow player loots even if only the controller has damaged the guardian

                    std::list<Creature*> selectedGroup;
                    GetCreatureListWithEntryInGrid(selectedGroup, me, me->GetEntry(), 200.0f);

                    for (auto&& itr : selectedGroup)
                        if (itr->GetGUID() != me->GetGUID())
                            me->Kill(itr, false);
                    return;
                }

                RecalculateHealth(damage);

            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_LANDING)
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetCanFly(false);
                        me->SetAnimTier(AnimTier::Ground);
                        me->OverrideInhabitType(INHABIT_GROUND);
                        me->UpdateMovementFlags();

                        me->SetInCombatWithZone();
                    }
                    break;
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_AMBER_PRISON:
                            DoCast(me, SPELL_AMBER_PRISON);
                            events.ScheduleEvent(EVENT_AMBER_PRISON, urand(35000, 70000));
                            break;

                        case EVENT_CORROSIVE_RESIN:
                            DoCast(me, SPELL_CORROSIVE_RESIN);
                            events.ScheduleEvent(EVENT_CORROSIVE_RESIN, urand(25000, 32000));
                            break;

                        case EVENT_MENDING:
                        {
                            DoCast(me, SPELL_MENDING);
                            events.ScheduleEvent(EVENT_MENDING, urand(37000, 62000));
                            break;
                        }
                        case EVENT_QUICKENING:
                        {
                            DoCast(me, SPELL_QUICKENING);
                            events.ScheduleEvent(EVENT_QUICKENING, urand(33000, 54000));
                            break;
                        }
                    }
                }

                DoMeleeAttackIfReady();
            }

            void SpellHit(Unit*, SpellInfo const* spell) override
            {
                if (!_instance)
                    return;

                if (spell->HasAttribute(SPELL_ATTR0_CU_AURA_CC) || spell->HasAura(SPELL_AURA_MOD_ROOT))
                {
                    std::list<Creature*> swarm;
                    GetCreatureListWithEntryInGrid(swarm, me, NPC_KORTHIK_ELITE_BLADEMASTER, 100.0f);
                    GetCreatureListWithEntryInGrid(swarm, me, NPC_ZARTHIK_BATTLE_MENDER, 100.0f);
                    GetCreatureListWithEntryInGrid(swarm, me, NPC_SRATHIK_AMBER_TRAPPER, 100.0f);

                    uint32 controlled = 0;
                    for (auto&& itr : swarm)
                        if (itr->HasCrowdControlAura()) // May be need to check non breakable too, but it is only short stuns I think
                            ++controlled;

                    if (controlled >= 2)
                        me->GetMap()->SetWorldState(WORLD_STATE_ACHIEVEMENT_LESS_THAN_THREE, 0);
                }
            }

        private:
            InstanceScript* _instance;

            void RecalculateHealth(uint32 damage)
            {
                std::list<Creature*> Swarm;
                GetCreatureListWithEntryInGrid(Swarm, me, me->GetEntry(), 100.0f);

                for (auto&& itr : Swarm)
                    if (itr->GetGUID() != me->GetGUID() && damage < itr->GetHealth())
                        itr->ModifyHealth(-int32(damage));
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_meljarak_swarmAI(creature);
        }
};

// Whirling blades stalker 63930
class npc_whirling_blade_stalker : public CreatureScript
{
    public:
        npc_whirling_blade_stalker() : CreatureScript("npc_whirling_blade_stalker") { }

        enum eEvents
        {
            EVENT_MOVE_BACK = 1,
        };

        struct npc_whirling_blade_stalkerAI : public ScriptedAI
        {
            npc_whirling_blade_stalkerAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }
            EventMap events;
            uint32 timer, Pointer;
            Position pos;
            std::list <uint32> whirlingBladeAffected;
            float x, y, dist;

            void IsSummonedBy(Unit* summoner) override 
            {
                me->SetSpeed(MOVE_RUN, 2.65f);
                me->SetSpeed(MOVE_WALK, 2.65f);
                me->SetSpeed(MOVE_SWIM, 2.65f);
                me->AddAura(SPELL_WHIRLING_BLADE, me);
                Pointer = 0;
                dist = frand(35.0f, 42.0f);
                me->SetObjectScale(summoner->GetObjectScale());

                GetPositionWithDistInOrientation(me, dist, me->GetOrientation(), x, y);
                pos = { x, y, me->GetPositionZ(), me->GetOrientation() };

                me->GetMotionMaster()->MovePoint(Pointer, pos);
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                whirlingBladeAffected.clear();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case 0:
                        Pointer = 1;
                        whirlingBladeAffected.clear();
                        events.ScheduleEvent(EVENT_MOVE_BACK, 100);
                        break;
                    case 1:
                        if (Unit* Meljarak = ObjectAccessor::GetUnit(*me, _instance->GetGuidData(DATA_MELJARAK)))
                            Meljarak->GetAI()->DoAction(ACTION_WHIRLING_BLADE);

                        me->DespawnOrUnsummon();
                        break;
                }
            }

            // We should only once hit player on way, and once at way to back.
            uint32 GetData(uint32 type) const override
            {
                for (auto&& itr : whirlingBladeAffected)
                    if (type == itr)
                        return 1;

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_WHIRLING_BLADE)
                    whirlingBladeAffected.push_back(data);
            }

            void InitializeAI() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override { damage = 0; }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_MOVE_BACK)
                    {
                        float orientation = me->GetOrientation() + M_PI;

                        GetPositionWithDistInOrientation(me, dist, Position::NormalizeOrientation(orientation), x, y);
                        pos = { x, y, me->GetPositionZ(), Position::NormalizeOrientation(orientation) };

                        if (Unit* Meljarak = ObjectAccessor::GetUnit(*me, _instance->GetGuidData(DATA_MELJARAK)))
                        {
                            me->CastSpell(Meljarak, SPELL_WHIRLING_BLADE_DUMMY, false);
                            me->CastSpell(Meljarak, SPELL_WHIRLING_BLADE_DUMMY_AURA, false);
                        }

                        me->GetMotionMaster()->MovePoint(Pointer, pos);
                    }
                    break;
                }
            }

        private:
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_whirling_blade_stalkerAI(creature);
        }
};

// Wind Bomb: 67053.
class npc_wind_bomb_meljarak : public CreatureScript
{
    public:
        npc_wind_bomb_meljarak() : CreatureScript("npc_wind_bomb_meljarak") { }

        struct npc_wind_bomb_meljarakAI : public ScriptedAI
        {
            npc_wind_bomb_meljarakAI(Creature* creature) : ScriptedAI(creature) { }
            EventMap events;

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetDisplayId(45684);
                me->SetInCombatWithZone();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);

                DoCast(me, SPELL_WIND_BOMB_THR_DMG);

                events.ScheduleEvent(EVENT_ARM, 3000);
                events.ScheduleEvent(EVENT_CHECK_PLAYER, 4000);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARM:
                            DoCast(me, SPELL_WIND_BOMB_ARM);
                            break;
                        case EVENT_CHECK_PLAYER:
                            if (Player* player = me->SelectNearestPlayer(6.0f))
                            {
                                if (player->IsWithinDistInMap(me, 5.15f))
                                {
                                    DoCast(me, SPELL_WIND_BOMB_EXPLODE);
                                    me->DespawnOrUnsummon();
                                }
                                else
                                    events.ScheduleEvent(EVENT_CHECK_PLAYER, 500);
                            }
                            else
                                events.ScheduleEvent(EVENT_CHECK_PLAYER, 500);
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_wind_bomb_meljarakAI(creature);
        }
};

// Corrosive resin pool stalker: 67046.
class npc_corrosive_resin_pool : public CreatureScript
{
    public:
        npc_corrosive_resin_pool() : CreatureScript("npc_corrosive_resin_pool") { }

        struct npc_corrosive_resin_poolAI : public ScriptedAI
        {
            npc_corrosive_resin_poolAI(Creature* creature) : ScriptedAI(creature) { }
            EventMap events;

            void IsSummonedBy(Unit* summoner) override
            {
                me->AddAura(SPELL_CORR_RESIN_POOL_A, me);
                me->DespawnOrUnsummon(60000);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_corrosive_resin_poolAI(creature);
        }
};

// Amber Prison Stalker: 62531
class npc_amber_prison : public CreatureScript
{
    public:
        npc_amber_prison() : CreatureScript("npc_amber_prison") { }

        struct npc_amber_prisonAI : public ScriptedAI
        {
            npc_amber_prisonAI(Creature* creature) : ScriptedAI(creature) { }
            EventMap events;
            ObjectGuid ownerGUID;

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NON_ATTACKABLE);
                summoner->CastSpell(summoner, SPELL_AMBER_PRISON_AURA, true);
                ownerGUID = summoner->GetGUID();
            }

            void OnSpellClick(Unit* clicker, bool& /*result*/) override
            {
                if (clicker->HasAura(SPELL_RESIDUE) || clicker->HasAura(SPELL_AMBER_PRISON_AURA) || clicker->GetGUID() == ownerGUID)
                    return;

                if (Unit* owner = ObjectAccessor::GetUnit(*me, ownerGUID))
                {
                    owner->RemoveAurasDueToSpell(SPELL_AMBER_PRISON_AURA);
                    clicker->CastSpell(clicker, SPELL_RESIDUE, false);
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_amber_prisonAI(creature);
        }
};

// Swarm Spawner: 62447,62452,62451
class npc_swarm_spawner_heroic : public CreatureScript
{
    public:
        npc_swarm_spawner_heroic() : CreatureScript("npc_swarm_spawner_heroic") { }

        enum sEvents
        {
            EVENT_SPAWN_KORTHIK = 1,
            EVENT_SPAWN_ZARTHIK = 2,
            EVENT_SPAWN_SRATHIK = 3,
        };

        struct npc_swarm_spawner_heroicAI : public ScriptedAI
        {
            npc_swarm_spawner_heroicAI(Creature* creature) : ScriptedAI(creature) { }
            EventMap events;
            uint32 SwarmType, EventType;
            bool Launched;
            InstanceScript* instance;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();

                switch (me->GetEntry())
                {
                    case NPC_KORTHIK_SPAWN_HEROIC:
                        EventType = EVENT_SPAWN_KORTHIK;
                        SwarmType = NPC_KORTHIK_ELITE_BLADEMASTER;
                        break;
                    case NPC_ZARTHIK_SPAWN_HEROIC:
                        EventType = EVENT_SPAWN_ZARTHIK;
                        SwarmType = NPC_ZARTHIK_BATTLE_MENDER;
                        break;
                    case NPC_SRATHIK_SPAWN_HEROIC:
                        EventType = EVENT_SPAWN_SRATHIK;
                        SwarmType = NPC_SRATHIK_AMBER_TRAPPER;
                        break;
                }

                Reset();
            }

            void Reset() override
            {
                events.Reset();
                Launched = false;
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_SET_SPAWN_SWARM:
                        if (!Launched)
                        {
                            Launched = true;
                            events.ScheduleEvent(EventType, 45 * IN_MILLISECONDS);
                        }
                        break;
                    case ACTION_RESET_SPAWN:
                        Reset();
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SPAWN_KORTHIK:
                        case EVENT_SPAWN_SRATHIK:
                        case EVENT_SPAWN_ZARTHIK:
                            for (uint8 i = 0; i < 3; i++)
                                me->SummonCreature(SwarmType, me->GetPositionX() + frand(-2.0f, 2.0f), me->GetPositionY() + frand(-8.0f, 8.0f), me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN);
                            
                            Launched = false;

                            // Send new frame for group
                            if (Creature* Meljarak = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_MELJARAK)))
                            {
                                SetEncounterUnitForEachGroup(me, Meljarak->GetGUID(), instance, ENCOUNTER_FRAME_ENGAGE, SwarmType);
                                Meljarak->AI()->DoAction(SwarmType + 5);
                            }
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_swarm_spawner_heroicAI(creature);
        }
};

// GameObject - 212349 - Mantid weapon rack
class go_mantid_weapon_rack : public GameObjectScript
{
    public:
        go_mantid_weapon_rack() : GameObjectScript("go_mantid_weapon_rack") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            InstanceScript* instance = go->GetInstanceScript();

            if (instance && player)
            {
                if (player->HasAura(SPELL_IMP_SPEAR_ABIL))
                    return false;

                player->AddAura(SPELL_IMP_SPEAR_ABIL, player);
                return true;
            }

            return false;
        }
};

// Whirling blade 121896
class spell_whirling_blade : public SpellScript
{
    PrepareSpellScript(spell_whirling_blade);

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Player* itr = GetFarthestPlayerInArea(GetCaster(), 150.0f))
            {
                if (Creature* Whirlingblade = caster->SummonCreature(NPC_WHIRLING_BLADE, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ(), caster->GetAngle(itr), TEMPSUMMON_MANUAL_DESPAWN))
                {
                    caster->PrepareChanneledCast(caster->GetAngle(itr));
                    caster->CastSpell(itr->GetPositionX(), itr->GetPositionY(), itr->GetPositionZ() + 2.0f, SPELL_WHIRLING_BLADE_DUMMY, false);
                    caster->CastSpell(itr->GetPositionX(), itr->GetPositionY(), itr->GetPositionZ() + 2.0f, SPELL_WHIRLING_BLADE_DUMMY_AURA, false);
                }
            }

            caster->RemoveAurasDueToSpell(SPELL_WHIRLING_BLADE);
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_whirling_blade::HandleAfterCast);
    }
};

// Corrosive Resin: 122064.
class spell_meljarak_corrosive_resin_aura : public AuraScript
{
    PrepareAuraScript(spell_meljarak_corrosive_resin_aura);

    void PeriodicTick(AuraEffect const* /*aurEff*/)
    {
        Unit* owner = GetOwner()->ToUnit();
        if (!owner)
            return;

        // Check target moving.
        if (owner->isMoving())
        {
            // Check the aura.
            if (Aura* resinAura = owner->GetAura(SPELL_CORROSIVE_RESIN))
            {
                // Remove the stacks.
                if (resinAura->GetStackAmount() > 1)
                    resinAura->SetStackAmount(resinAura->GetStackAmount() - 1);
                else
                    owner->RemoveAurasDueToSpell(SPELL_CORROSIVE_RESIN);

                // Summon the pool.
                owner->SummonCreature(NPC_CORROSIVE_RESIN_POOL, owner->GetPositionX(), owner->GetPositionY(), owner->GetPositionZ(), owner->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
            }
        }
    }

    void HandleApplyAura(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        Unit* owner = GetOwner()->ToUnit();

        if (!owner)
            return;

        if (Aura* resinAura = owner->GetAura(SPELL_CORROSIVE_RESIN))
            resinAura->SetStackAmount(5);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_meljarak_corrosive_resin_aura::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        OnEffectApply += AuraEffectApplyFn(spell_meljarak_corrosive_resin_aura::HandleApplyAura, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Corrosive Resin: 122064.
class spell_meljarak_corrosive_resin : public SpellScript
{
    PrepareSpellScript(spell_meljarak_corrosive_resin);

    std::list<WorldObject*> m_targets;

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove whole exclude casters
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if([=](Player* target) { return target->HasAura(SPELL_CORROSIVE_RESIN); });

            // If we not found any dps then try select anyone without same aura
            if (pList.empty())
            {
                targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && target->ToPlayer()->HasAura(SPELL_CORROSIVE_RESIN); });
                m_targets = targets;
                return;
            }

            targets.clear();

            // Doesn`t matter push only 1 target or more cuz maxAffectTargets is 1
            for (auto&& itr : pList)
                targets.push_back(itr);

            m_targets = targets;
        }
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_meljarak_corrosive_resin::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_meljarak_corrosive_resin::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Amber Prison Target Selector 121876
class spell_amber_prison_selector : public SpellScript
{
    PrepareSpellScript(spell_amber_prison_selector);

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove whole exclude casters
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if([=](Player* target) { return target->HasAura(SPELL_AMBER_PRISON_AURA); });

            // If we not found any dps then try select anyone without same aura
            if (pList.empty())
            {
                targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && target->ToPlayer()->HasAura(SPELL_AMBER_PRISON_AURA); });
                return;
            }

            targets.clear();

            // Doesn`t matter push only 1 target or more cuz maxAffectTargets is 1
            for (auto&& itr : pList)
                targets.push_back(itr);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_amber_prison_selector::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Amber Prison 121874
class spell_meljarak_amber_prison : public SpellScript
{
    PrepareSpellScript(spell_meljarak_amber_prison);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_AMBER_PRISON_SUMM, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_meljarak_amber_prison::HandleEffectHit, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
    }
};

class MendingPredicate 
{
    public:
        MendingPredicate(const uint32 entry) : _entry(entry) { }
    
        bool operator()(WorldObject* object)
        {
            return object && object->GetEntry() != _entry;
        }
    
    private:
        const uint32 _entry;
};

// mending - 122193
class spell_mending : public SpellScript
{
    PrepareSpellScript(spell_mending);

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        targets.clear();

        if (Unit* caster = GetCaster())
        {
            std::list<Creature*> tmpTargets;
            GetCreatureListWithEntryInGrid(tmpTargets, caster, NPC_ZARTHIK_BATTLEMENDER, 200.0f);
            GetCreatureListWithEntryInGrid(tmpTargets, caster, NPC_KORTHIK_ELITE_BLADEMASTER, 200.0f);
            GetCreatureListWithEntryInGrid(tmpTargets, caster, NPC_SRATHIK_AMBER_TRAPPER, 200.0f);

            if (tmpTargets.empty())
                return;

            tmpTargets.sort(Trinity::HealthPctOrderPred());
            uint32 lowestEntry = tmpTargets.front()->GetEntry();
            tmpTargets.remove_if(MendingPredicate(lowestEntry));

            for (auto&& itr : tmpTargets)
            {
                itr->CastSpell(itr, SPELL_MENDING_EX_AURA, true);
                targets.push_back(itr);
            }
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mending::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Impaling Spear 122224
class spell_meljarak_impaling_spear : public SpellScript
{
    PrepareSpellScript(spell_meljarak_impaling_spear);

    SpellCastResult CheckCast()
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = ObjectAccessor::GetUnit(*caster, caster->GetTarget()))
                if (target->GetEntry() == NPC_KORTHIK_ELITE_BLADEMASTER || target->GetEntry() == NPC_ZARTHIK_BATTLEMENDER || target->GetEntry() == NPC_SRATHIK_AMBER_TRAPPER)
                    return SPELL_CAST_OK;

        return SPELL_FAILED_SPELL_UNAVAILABLE;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_meljarak_impaling_spear::CheckCast);
    }
};

// Korthik Strike 122409
class spell_meljarak_korthik_strike : public SpellScript
{
    PrepareSpellScript(spell_meljarak_korthik_strike);

    void HandleEffectCharge(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_KORTHIK_STRIKE, false);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_meljarak_korthik_strike::HandleEffectCharge, EFFECT_0, SPELL_EFFECT_CHARGE);
    }
};

class BerserkPredicate
{
    public:
        BerserkPredicate() {}

        bool operator() (WorldObject* object)
        {
            for (auto&& itr : meljarakSwarmType)
                if (object && object->GetEntry() == itr)
                    return false;

            return true;
        }
};

// Meljarak Berserk 120207
class spell_meljarak_berserk : public SpellScript
{
    PrepareSpellScript(spell_meljarak_berserk);

    std::list<WorldObject*> m_targets;

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(BerserkPredicate());
        m_targets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_meljarak_berserk::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_meljarak_berserk::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Wind Bomb 131813
class spell_meljarak_wind_bomb : public SpellScript
{
    PrepareSpellScript(spell_meljarak_wind_bomb);

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        // same effect like in spell definition, but don`t know why target cast it to encounter
        PreventDefaultEffect(EFFECT_0);

        if (Unit* target = GetHitUnit())
            if (InstanceScript* instance = target->GetInstanceScript())
                if (Creature* Meljarak = ObjectAccessor::GetCreature(*target, instance->GetGuidData(DATA_MELJARAK)))
                    Meljarak->CastSpell(target, SPELL_WIND_BOMB_SUMM, false);
    }

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove whole exclude casters
            pList.remove_if(MeeleSpecTargetSelector());
            pList.remove_if(TankSpecTargetSelector());

            // If we not found any casters (rdps/healer) then prevent it
            if (pList.empty())
                return;

            targets.clear();

            // Doesn`t matter push only 1 target or more cuz maxAffectTargets is 1
            for (auto&& itr : pList)
                targets.push_back(itr);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_meljarak_wind_bomb::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_meljarak_wind_bomb::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

class WhirlingBladePredicate 
{
    public:
        WhirlingBladePredicate(Creature* const m_caster) : _caster(m_caster) { }

        bool operator()(WorldObject* object)
        {
            if (object && object->ToPlayer())
            {
                if (_caster->AI()->GetData(object->ToPlayer()->GetGUID().GetCounter()))
                    return true;

                _caster->AI()->SetData(TYPE_WHIRLING_BLADE, object->ToPlayer()->GetGUID().GetCounter());
                return false;
            }

            return false;
        }

    private:
        Creature const* _caster;
};

// Whirling Blade Effect (damage) 121898
class spell_whirling_blade_eff : public SpellScript
{
    PrepareSpellScript(spell_whirling_blade_eff);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            targets.remove_if(WhirlingBladePredicate(caster));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_whirling_blade_eff::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

void AddSC_boss_meljarak()
{
    new boss_wind_lord_meljarak();
    new npc_meljarak_swarm();
    new npc_whirling_blade_stalker();
    new npc_wind_bomb_meljarak();
    new npc_corrosive_resin_pool();
    new npc_amber_prison();
    new npc_swarm_spawner_heroic();
    new go_mantid_weapon_rack();
    new spell_script<spell_whirling_blade>("spell_whirling_blade");
    new aura_script<spell_meljarak_corrosive_resin_aura>("spell_meljarak_corrosive_resin_aura");
    new spell_script<spell_meljarak_corrosive_resin>("spell_meljarak_corrosive_resin");
    new spell_script<spell_amber_prison_selector>("spell_amber_prison_selector");
    new spell_script<spell_meljarak_amber_prison>("spell_meljarak_amber_prison");
    new spell_script<spell_mending>("spell_mending");
    new spell_script<spell_meljarak_impaling_spear>("spell_meljarak_impaling_spear");
    new spell_script<spell_meljarak_korthik_strike>("spell_meljarak_korthik_strike");
    new spell_script<spell_meljarak_berserk>("spell_meljarak_berserk");
    new spell_script<spell_meljarak_wind_bomb>("spell_meljarak_wind_bomb");
    new spell_script<spell_whirling_blade_eff>("spell_whirling_blade_eff");
}
