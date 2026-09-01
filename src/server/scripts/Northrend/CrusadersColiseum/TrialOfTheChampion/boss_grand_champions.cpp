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

#include "ScriptPCH.h"
#include "trial_of_the_champion.h"

enum Spells
{
    // Vehicle
    SPELL_CHARGE                    = 63010,
    SPELL_CHARGE_TRIGGERED          = 43807, // temp
    SPELL_CHARGE_DMG                = 68498,
    SPELL_THRUST                    = 62544,
    SPELL_SHIELD_BREAKER            = 68504,
    SPELL_SHIELD                    = 66482,
    SPELL_TRAMPLE_AURA              = 67865,
    SPELL_TRAMPLE                   = 67866,
    SPELL_STUNNED                   = 65918,
    SPELL_KNEEL                     = 68442,

    // Defend
    SPELL_DEFEND                    = 66482,
    SPELL_VISUAL_SHIELD_1           = 63130,
    SPELL_VISUAL_SHIELD_2           = 63131,
    SPELL_VISUAL_SHIELD_3           = 63132,

    // Marshal Jacob Alerius && Mokra the Skullcrusher (Warrior)
    SPELL_MORTAL_STRIKE             = 68783,
    SPELL_MORTAL_STRIKE_H           = 68784,
    SPELL_BLADESTORM                = 63784,
    SPELL_INTERCEPT                 = 67540,
    SPELL_ROLLING_THROW             = 67546,

    // Ambrose Boltspark && Eressea Dawnsinger (Mage)
    SPELL_FIREBALL                  = 66042,
    SPELL_FIREBALL_H                = 68310,
    SPELL_BLAST_WAVE                = 66044,
    SPELL_BLAST_WAVE_H              = 68312,
    SPELL_HASTE                     = 66045,
    SPELL_POLYMORPH                 = 66043,
    SPELL_POLYMORPH_H               = 68311,

    // Colosos && Runok Wildmane (Shaman)
    SPELL_CHAIN_LIGHTNING           = 67529,
    SPELL_CHAIN_LIGHTNING_H         = 68319,
    SPELL_EARTH_SHIELD              = 67530,
    SPELL_HEALING_WAVE              = 67528,
    SPELL_HEALING_WAVE_H            = 68318,
    SPELL_HEX_OF_MENDING            = 67534,

    // Jaelyne Evensong && Zul'tore (Hunter)
    //SPELL_DISENGAGE                 = 68340,
    SPELL_DISENGAGE                 = 68339,
    SPELL_LIGHTNING_ARROWS          = 66083,
    SPELL_MULTI_SHOT                = 66081,
    SPELL_SHOOT                     = 65868,
    SPELL_SHOOT_H                   = 67988,

    // Lana Stouthammer Evensong && Deathstalker Visceri (Rouge)
    SPELL_EVISCERATE                = 67709,
    SPELL_EVISCERATE_H              = 68317,
    SPELL_FAN_OF_KNIVES             = 67706,
    SPELL_POISON_BOTTLE             = 67701,
    SPELL_DEADLY_POISON             = 67710,
    SPELL_DEADLY_POISON_H           = 68315
};

enum Points
{
    POINT_RANGE                     = 10,
    POINT_MOUNT                     = 11
};

enum Events
{
    // Mounted
    EVENT_THRUST                    = 1,
    EVENT_SHIELD,
    EVENT_SHIELDBREAKER,
    // Rogue
    EVENT_EVISCERATE,
    EVENT_FANOFKNIVES,
    EVENT_POISONBOTTLE,
    EVENT_DEADLYPOISON,
    // Hunter
    EVENT_DISENGAGE,
    EVENT_SHOOT,
    EVENT_MULTISHOT,
    EVENT_LIGHTNINGARROWS,
    // Shaman
    EVENT_CHAINLIGHTNING,
    EVENT_HEALINGWAVE,
    EVENT_EARTHSHIELD,
    EVENT_HEXOFMENDING,
    // Mage
    EVENT_FIREBALL,
    EVENT_BLASTWAVE,
    EVENT_HASTE,
    EVENT_POLYMORPH,
    // Warrior
    EVENT_ROLLINGTHROW,
    EVENT_BLADESTORM,
    EVENT_MORTALSTRIKE,
    EVENT_INTERCEPT
};

Position const Waypoints[12] =
{
    // Path 1
    { 747.36f, 634.07f, 411.57f, 0.0f },
    { 780.43f, 607.15f, 411.82f, 0.0f },
    { 785.99f, 599.41f, 411.92f, 0.0f },
    { 778.44f, 601.64f, 411.79f, 0.0f },
    // Path 2
    { 747.35f, 634.07f, 411.57f, 0.0f },
    { 768.72f, 581.01f, 411.92f, 0.0f },
    { 763.55f, 590.52f, 411.71f, 0.0f },
    { 763.55f, 590.52f, 411.71f, 0.0f },
    // Path 3
    { 747.35f, 634.07f, 411.57f, 0.0f },
    { 784.02f, 645.33f, 412.39f, 0.0f },
    { 775.67f, 641.91f, 411.91f, 0.0f },
    { 775.67f, 641.91f, 411.91f, 0.0f }
};

Position const Mounts[4] =
{
    { 781.88f, 656.76f, 412.4f, 0.00f },
    { 783.26f, 581.86f, 412.4f, 0.00f },
    { 710.95f, 581.95f, 412.4f, 0.00f },
    { 712.53f, 657.36f, 412.4f, 0.00f }
};

void AggroAllPlayers(Creature* source)
{
    Map::PlayerList const& playerList = source->GetMap()->GetPlayers();

    if (playerList.isEmpty())
            return;

    for (auto&& ref : playerList)
    {
        if (Player* player = ref.GetSource())
        {
            if (player->IsGameMaster())
                continue;

            if (player->GetVehicle())
            {
                if (Creature* vehicle = player->GetVehicleBase()->ToCreature())
                {
                    source->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                    source->SetReactState(REACT_AGGRESSIVE);
                    source->SetInCombatWith(vehicle);
                    player->SetInCombatWith(source);
                    vehicle->SetInCombatWith(source);
                    source->AddThreat(vehicle, 0.0f);
                 }
            }
            else if (player->IsAlive())
            {
                source->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                source->SetReactState(REACT_AGGRESSIVE);
                source->SetInCombatWith(player);
                player->SetInCombatWith(source);
                source->AddThreat(player, 0.0f);
            }
        }
    }
}

Unit* GetRangedTarget(float minRange, float maxRange, WorldObject* source)
{
    Map* map = source->GetMap();
    if (map && map->IsDungeon())
    {
        std::list<Unit*> targets;
        for (auto&& ref : map->GetPlayers())
            if (Player* player = ref.GetSource())
            {
                if (player->IsGameMaster() || !player->IsAlive() || source->GetDistance(player) < minRange || source->GetDistance(player) > maxRange)
                    continue;

                targets.push_back(player);
            }

        if (!targets.empty())
        {
            Unit* target = Trinity::Containers::SelectRandomContainerElement(targets);

            if (Unit* vehicle = target->GetVehicleBase())
                return vehicle;

            return target;
        }
    }

    return NULL;
}

class npc_faction_champion_toc5 : public CreatureScript
{
    public:
        npc_faction_champion_toc5() : CreatureScript("npc_faction_champion_toc5") { }

        struct npc_faction_champion_toc5AI : public ScriptedAI
        {
            npc_faction_champion_toc5AI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                _events.Reset();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == POINT_RANGE)
                    _events.RescheduleEvent(EVENT_CHARGE, 1);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _events.ScheduleEvent(EVENT_THRUST, urand(5000, 10000));
                _events.ScheduleEvent(EVENT_CHARGE, urand(5000, 15000));
                _events.ScheduleEvent(EVENT_SHIELD, 1);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                if (GetRangedTarget(0.0f , 200.0f, me))
                {
                    AggroAllPlayers(me);
                    return;
                }

                ScriptedAI::EnterEvadeMode();
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_CHARGE_TRIGGERED)
                    caster->CastSpell(me, SPELL_CHARGE_DMG, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHARGE:
                            if (Unit* target = GetRangedTarget(8.0f, 25.0f, me))
                            {
                                DoResetThreat();
                                me->AddThreat(target, 100.0f);
                                me->GetMotionMaster()->MoveChase(me->GetVictim());
                                DoCast(target, SPELL_CHARGE);
                            }
                            else if (!GetRangedTarget(8.0f, 200.0f, me)) // all targets below 8 yd distance, try to get range
                            {
                                float x, y, z;
                                me->GetNearPoint(me, x, y, z, 1.0f, 12.0f, float(M_PI * 2 * rand_norm()));
                                me->GetMotionMaster()->MovePoint(POINT_RANGE, x, y, z);
                            }
                            else if (me->GetVictim())
                                me->GetMotionMaster()->MoveChase(me->GetVictim());
                            _events.ScheduleEvent(EVENT_CHARGE, urand(6000, 12000));
                            break;
                        case EVENT_SHIELD:
                            DoCast(me, SPELL_SHIELD);
                            _events.ScheduleEvent(EVENT_SHIELD, urand(10000, 15000));
                            break;
                        case EVENT_THRUST:
                            DoCastVictim(SPELL_THRUST);
                            _events.ScheduleEvent(EVENT_THRUST, urand(2000, 3000));
                            break;
                    }
                }
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_faction_champion_toc5AI(creature);
        }
};

class boss_grand_champion_toc5 : public CreatureScript
{
    public:
        boss_grand_champion_toc5() : CreatureScript("boss_grand_champion_toc5") { }

        struct boss_grand_champion_toc5AI : public ScriptedAI
        {
            boss_grand_champion_toc5AI(Creature* creature) : ScriptedAI(creature)
            {
                _instance = creature->GetInstanceScript();
                _waypointReached = false;
                _phase = 1;
                _point = 1;
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            void Reset() override
            {
                _events.Reset();

                if (_phase == 0 || _phase == 3)
                    me->Dismount();
            }

            void JustReachedHome() override
            {
                Reset();
            }

            void Evade()
            {
                _phase = 0;
                _phaseChangeTimer = 20000;
                EnterEvadeMode();
                me->SetFaction(35);
            }

            void SetData(uint32 type, uint32 /*data*/) override
            {
                if (type && type <= 3)
                {
                    _waypointPath = type - 1;
                    _waypointReached = true;
                }

                switch (type)
                {
                    case 10:
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetWalk(false);
                        Evade();
                        break;
                    case 11:
                        me->DespawnOrUnsummon(10000);
                        Evade();
                        break;
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE || !pointId)
                    return;

                switch (pointId)
                {
                    case POINT_RANGE:
                        _events.RescheduleEvent(EVENT_CHARGE, 1);
                        break;
                    case POINT_MOUNT:
                    {
                        _phase = 1;
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetWalk(false);
                        if (Creature* announcer = ObjectAccessor::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_ANNOUNCER) : ObjectGuid::Empty))
                            announcer->AI()->SetData(DATA_GRAND_CHAMPIONS_DEFEATED, announcer->AI()->GetData(DATA_GRAND_CHAMPIONS_DEFEATED) - 1);
                        CreatureAddon const* cainfo = me->GetCreatureAddon();
                        if (cainfo && cainfo->mount != 0)
                            me->Mount(cainfo->mount);
                        me->SetFullHealth();
                        if (me->GetVictim())
                            me->GetMotionMaster()->MoveChase(me->GetVictim());
                        break;
                    }
                }

                if (pointId != _point)
                    return;

                if (pointId == 4 && _instance)
                    _instance->SetData(DATA_MOVEMENT_DONE, _instance->GetData(DATA_MOVEMENT_DONE) + 1);

                if (pointId < 4)
                {
                    ++_point;
                    _waypointReached = true;
                }
            }

            void InitUnmountedEvents()
            {
                switch (me->GetEntry())
                {
                    // Rogue
                    case NPC_LANA:
                    case NPC_VISCERI:
                        me->GetMap()->SetWorldState(WORLDSTATE_STOUTHAMMER, 1);
                        _events.ScheduleEvent(EVENT_EVISCERATE, urand(5000, 8000));
                        _events.ScheduleEvent(EVENT_FANOFKNIVES, urand(10000, 14000));
                        _events.ScheduleEvent(EVENT_POISONBOTTLE, urand(12000, 19000));
                        _events.ScheduleEvent(EVENT_DEADLYPOISON, urand(5000, 10000));
                        break;
                    // Hunter
                    case NPC_JAELYNE:
                    case NPC_ZULTORE:
                        me->GetMap()->SetWorldState(WORLDSTATE_EVERSONG, 1);
                        _events.ScheduleEvent(EVENT_DISENGAGE, urand(5000, 8000));
                        _events.ScheduleEvent(EVENT_SHOOT, urand(5000, 7000));
                        _events.ScheduleEvent(EVENT_MULTISHOT, urand(12000, 15000));
                        _events.ScheduleEvent(EVENT_LIGHTNINGARROWS, urand(10000, 16000));
                        break;
                    // Shaman
                    case NPC_COLOSOS:
                    case NPC_RUNOK:
                        me->GetMap()->SetWorldState(WORLDSTATE_COLOSOS, 1);
                        _events.ScheduleEvent(EVENT_CHAINLIGHTNING, urand(5000, 10000));
                        _events.ScheduleEvent(EVENT_HEALINGWAVE, urand(8000, 12000));
                        _events.ScheduleEvent(EVENT_EARTHSHIELD, urand(500, 1000));
                        _events.ScheduleEvent(EVENT_HEXOFMENDING, urand(1000, 3000));
                        break;
                    // Mage
                    case NPC_AMBROSE:
                    case NPC_ERESSEA:
                        me->GetMap()->SetWorldState(WORLDSTATE_BOLTSPARK, 1);
                        _events.ScheduleEvent(EVENT_FIREBALL, urand(5000, 6000));
                        _events.ScheduleEvent(EVENT_BLASTWAVE, urand(12000, 15000));
                        _events.ScheduleEvent(EVENT_HASTE, urand(15000, 20000));
                        _events.ScheduleEvent(EVENT_POLYMORPH, urand(10000, 12000));
                        break;
                    // Warrior
                    case NPC_JACOB:
                    case NPC_MOKRA:
                        me->GetMap()->SetWorldState(WORLDSTATE_JACOB_ALERIUS, 1);
                        //_events.ScheduleEvent(EVENT_ROLLINGTHROW, urand(20000, 25000));
                        _events.ScheduleEvent(EVENT_BLADESTORM, urand(15000, 20000));
                        _events.ScheduleEvent(EVENT_MORTALSTRIKE, urand(8000, 12000));
                        _events.ScheduleEvent(EVENT_INTERCEPT, urand(7000, 10000));
                        break;
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoZoneInCombat(me, 150.0f);
                me->CallForHelp(20.0f);

                switch (_phase)
                {
                    case 1:
                        me->CastCustomSpell(SPELL_SHIELD, SPELLVALUE_AURA_STACK, 3, me, true);
                        _events.ScheduleEvent(EVENT_THRUST, urand(5000, 8000));
                        _events.ScheduleEvent(EVENT_CHARGE, urand(5000, 10000));
                        _events.ScheduleEvent(EVENT_SHIELD, urand(7000, 15000));
                        _events.ScheduleEvent(EVENT_SHIELDBREAKER, urand(5000, 12000));
                        break;
                    case 3:
                        InitUnmountedEvents();
                        break;
                }
            }

            void SearchMount()
            {
                float x, y;
                x = me->GetPositionX();
                y = me->GetPositionY();

                if (x >= 746.5f && y >= 618.5f)
                    me->GetMotionMaster()->MovePoint(POINT_MOUNT, Mounts[0]);
                else if (x > 746.5f && y < 618.5f)
                    me->GetMotionMaster()->MovePoint(POINT_MOUNT, Mounts[1]);
                else if (x < 746.5f && y < 618.5f)
                    me->GetMotionMaster()->MovePoint(POINT_MOUNT, Mounts[2]);
                else if (x < 746.5f && y > 618.5f)
                    me->GetMotionMaster()->MovePoint(POINT_MOUNT, Mounts[3]);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (_phase == 2 || _phase == 4)
                {
                    damage = 0;
                    return;
                }

                if (damage >= me->GetHealth())
                {
                    damage = me->GetHealth() - 1;

                    switch (_phase)
                    {
                        case 1:
                            _phase = 2;
                            me->Dismount();
                            me->SetReactState(REACT_PASSIVE);
                            me->SetWalk(true);
                            SearchMount();
                            if (Creature* announcer = ObjectAccessor::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_ANNOUNCER) : ObjectGuid::Empty))
                                announcer->AI()->SetData(DATA_GRAND_CHAMPIONS_DEFEATED, announcer->AI()->GetData(DATA_GRAND_CHAMPIONS_DEFEATED) + 1);
                            break;
                        case 3:
                            _phase = 4;
                            me->Yell("Excellent work!", LANG_UNIVERSAL); // SAY_START_1
                            me->InterruptNonMeleeSpells(true);
                            me->RemoveAurasDueToSpell(SPELL_BLADESTORM);
                            DoCast(me, SPELL_KNEEL, true);
                            if (_instance)
                                _instance->SetData(BOSS_GRAND_CHAMPIONS, DONE);
                            me->SetReactState(REACT_PASSIVE);
                            break;
                    }
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                if (_phase != 0 && GetRangedTarget(0.0f , 200.0f, me))
                {
                    AggroAllPlayers(me);
                    return;
                }

                ScriptedAI::EnterEvadeMode();
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_TRAMPLE:
                        if ((_phase == 2 || _phase == 4) && !me->HasAura(SPELL_STUNNED) && caster->IsCharmed())
                        {
                            // temporary stop motion
                            me->GetMotionMaster()->MovementExpired();
                            me->GetMotionMaster()->MoveIdle();
                            caster->CastSpell(me, SPELL_STUNNED, true);
                        }
                        break;
                    case SPELL_CHARGE_TRIGGERED:
                        caster->CastSpell(me, SPELL_CHARGE_DMG, true);
                        break;
                }
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_FAN_OF_KNIVES)
                    DoCast(target, SPELL_DEADLY_POISON, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (_waypointReached)
                {
                    _waypointReached = false;
                    me->GetMotionMaster()->MovePoint(_point, Waypoints[(_point - 1) + 4 * _waypointPath]);
                }

                switch (_phase)
                {
                    case 0:
                        if (_phaseChangeTimer <= diff)
                        {
                            me->RestoreFaction();
                            _phase = 3;
                            DoZoneInCombat(me, 150.0f);
                        }
                        else
                            _phaseChangeTimer -= diff;
                        break;
                    case 2:
                        if (!me->HasAura(SPELL_KNEEL) && !me->HasAura(SPELL_STUNNED) && !me->HasUnitMovementFlag(MOVEMENTFLAG_FORWARD))
                            SearchMount();
                        return;
                    case 4:
                        return;
                }

                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        // Mounted
                        case EVENT_CHARGE:
                            if (Unit* target = GetRangedTarget(8.0f, 25.0f, me))
                            {
                                DoResetThreat();
                                me->AddThreat(target, 100.0f);
                                me->GetMotionMaster()->MoveChase(me->GetVictim());
                                DoCast(target, SPELL_CHARGE);
                            }
                            else if (!GetRangedTarget(8.0f, 200.0f, me)) // all targets below 8 yd distance, try to get range
                            {
                                float x, y, z;
                                me->GetNearPoint(me, x, y, z, 1.0f, 12.0f, float(M_PI*2*rand_norm()));
                                me->GetMotionMaster()->MovePoint(POINT_RANGE, x, y, z);
                            }
                            else if (me->GetVictim())
                                me->GetMotionMaster()->MoveChase(me->GetVictim());
                            _events.ScheduleEvent(EVENT_CHARGE, urand(5000, 9000));
                            break;
                        case EVENT_SHIELD:
                            DoCast(me, SPELL_SHIELD);
                            _events.ScheduleEvent(EVENT_SHIELD, urand(5000, 7000));
                            break;
                        case EVENT_THRUST:
                            DoCastVictim(SPELL_THRUST);
                            _events.ScheduleEvent(EVENT_THRUST, 2000);
                            break;
                        case EVENT_SHIELDBREAKER:
                            if (Unit* target = GetRangedTarget(10.0f, 30.0f, me))
                            {
                                DoCast(target, SPELL_SHIELD_BREAKER, true);
                                _events.ScheduleEvent(EVENT_SHIELDBREAKER, urand(6000, 11000));
                            }
                            else
                                _events.ScheduleEvent(EVENT_SHIELDBREAKER, 1000);
                            break;
                        // Rogue
                        case EVENT_EVISCERATE:
                            DoCastVictim(SPELL_EVISCERATE);
                            _events.ScheduleEvent(EVENT_EVISCERATE, urand(5000, 8000));
                            break;
                        case EVENT_FANOFKNIVES:
                            DoCastAOE(SPELL_FAN_OF_KNIVES);
                            _events.ScheduleEvent(EVENT_FANOFKNIVES, urand(7000, 10000));
                            break;
                        case EVENT_POISONBOTTLE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_POISON_BOTTLE);
                            _events.ScheduleEvent(EVENT_POISONBOTTLE, urand(12000, 16000));
                            break;
                        case EVENT_DEADLYPOISON:
                            DoCastVictim(SPELL_DEADLY_POISON);
                            _events.ScheduleEvent(EVENT_DEADLYPOISON, urand(5000, 7500));
                            break;
                        // Hunter
                        case EVENT_DISENGAGE:
                            if (me->IsWithinCombatRange(me->GetVictim(), 5.0f))
                                DoCast(SPELL_DISENGAGE);
                            _events.ScheduleEvent(EVENT_DISENGAGE, urand(5000, 7000));
                            break;
                        case EVENT_SHOOT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 45.0f))
                                DoCast(target, SPELL_SHOOT);
                            _events.ScheduleEvent(EVENT_SHOOT, urand(5000, 8000));
                            break;
                        case EVENT_MULTISHOT:
                            if (Unit* target = GetRangedTarget(5.0f, 30.0f, me))
                                DoCast(target, SPELL_MULTI_SHOT);
                            _events.ScheduleEvent(EVENT_MULTISHOT, urand(7000, 10000));
                            break;
                        case EVENT_LIGHTNINGARROWS:
                            DoCast(SPELL_LIGHTNING_ARROWS);
                            _events.ScheduleEvent(EVENT_LIGHTNINGARROWS, urand(30000, 40000));
                            break;
                        // Shaman
                        case EVENT_CHAINLIGHTNING:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_CHAIN_LIGHTNING);
                            _events.ScheduleEvent(EVENT_CHAINLIGHTNING, urand(6000, 12000));
                            break;
                        case EVENT_HEALINGWAVE:
                            if (Unit* friendly = DoSelectLowestHpFriendly(40.0f, 20000))
                                DoCast(friendly, SPELL_HEALING_WAVE);
                            else
                                DoCast(me, SPELL_HEALING_WAVE);
                            _events.ScheduleEvent(EVENT_HEALINGWAVE, urand(7000, 10000));
                            break;
                        case EVENT_EARTHSHIELD:
                            DoCast(me, SPELL_EARTH_SHIELD);
                            _events.ScheduleEvent(EVENT_EARTHSHIELD, urand(25000, 35000));
                            break;
                        case EVENT_HEXOFMENDING:
                            DoCastVictim(SPELL_HEX_OF_MENDING);
                            _events.ScheduleEvent(EVENT_HEXOFMENDING, urand(16000, 22000));
                            break;
                        // Mage
                        case EVENT_FIREBALL:
                            DoCastVictim(SPELL_FIREBALL);
                            _events.ScheduleEvent(EVENT_FIREBALL, urand(4000, 6000));
                            break;
                        case EVENT_BLASTWAVE:
                            DoCastAOE(SPELL_BLAST_WAVE);
                            _events.ScheduleEvent(EVENT_BLASTWAVE, urand(10000, 18000));
                            break;
                        case EVENT_HASTE:
                            DoCast(me, SPELL_HASTE);
                            _events.ScheduleEvent(EVENT_HASTE, urand(20000, 22000));
                            break;
                        case EVENT_POLYMORPH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_POLYMORPH);
                            _events.ScheduleEvent(EVENT_POLYMORPH, urand(8000, 12000));
                            break;
                        // Warrior
                        case EVENT_ROLLINGTHROW:
                            DoCastVictim(SPELL_ROLLING_THROW);
                            _events.ScheduleEvent(EVENT_ROLLINGTHROW, urand(17000, 23000));
                            break;
                        case EVENT_BLADESTORM:
                            DoCast(SPELL_BLADESTORM);
                            _events.ScheduleEvent(EVENT_BLADESTORM, urand(15000, 25000));
                            break;
                        case EVENT_MORTALSTRIKE:
                            DoCastVictim(SPELL_MORTAL_STRIKE);
                            _events.ScheduleEvent(EVENT_MORTALSTRIKE, urand(8000, 12000));
                            break;
                        case EVENT_INTERCEPT:
                            if (Unit* target = GetRangedTarget(8.0f, 25.0f, me))
                                DoCast(target, SPELL_INTERCEPT);
                            _events.ScheduleEvent(EVENT_INTERCEPT, urand(10000, 20000));
                            break;
                    }
                }

                if (_phase == 3)
                    DoMeleeAttackIfReady();
            }

        private:
            InstanceScript* _instance;
            EventMap _events;
            uint8 _phase;
            uint8 _point;
            uint8 _waypointPath;
            uint32 _phaseChangeTimer;
            bool _waypointReached;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_grand_champion_toc5AI(creature);
        }
};

class spell_ride_vehicle_toc5 : public SpellScriptLoader
{
    public:
        spell_ride_vehicle_toc5() : SpellScriptLoader("spell_ride_vehicle_toc5") { }

        class spell_ride_vehicle_toc5_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ride_vehicle_toc5_SpellScript);

            SpellCastResult CheckRequirement()
            {
                if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_DONT_REPORT;

                if (GetExplTargetUnit() && GetExplTargetUnit()->ToCreature())
                {
                    if (GetExplTargetUnit()->GetEntry() == VEHICLE_ARGENT_WARHORSE)
                        if (GetCaster() && GetCaster()->ToPlayer() && GetCaster()->ToPlayer()->GetTeamId() == TEAM_HORDE)
                            return SPELL_FAILED_BAD_TARGETS;

                    if (GetExplTargetUnit()->GetEntry() == VEHICLE_ARGENT_BATTLEWORG)
                        if (GetCaster() && GetCaster()->ToPlayer() && GetCaster()->ToPlayer()->GetTeamId() == TEAM_ALLIANCE)
                            return SPELL_FAILED_BAD_TARGETS;
                }

                if (GetCaster()->ToPlayer()->HasItemOrGemWithIdEquipped(46106, 1) ||
                    GetCaster()->ToPlayer()->HasItemOrGemWithIdEquipped(46069, 1) ||
                    GetCaster()->ToPlayer()->HasItemOrGemWithIdEquipped(46070, 1))
                {
                    if (GetCaster()->IsInDisallowedMountForm())
                        GetCaster()->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
                        return SPELL_CAST_OK;
                }
                else
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_HAVE_LANCE_EQUIPPED);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_ride_vehicle_toc5_SpellScript::CheckRequirement);
            }
        };

        class spell_ride_vehicle_toc5_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ride_vehicle_toc5_AuraScript);

            void HandleOnEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
                    target->RemoveAurasDueToSpell(SPELL_DEFEND);

                if (Unit* caster = GetCaster())
                {
                    caster->RemoveAurasDueToSpell(SPELL_DEFEND);
                    for (uint8 i = 0; i < 3; ++i)
                        caster->RemoveAurasDueToSpell(SPELL_VISUAL_SHIELD_1 + i);
                }
            }

            void HandleOnApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                HandleOnEffect(aurEff, mode);
                if (Unit* target = GetTarget())
                    target->CastSpell(target, SPELL_TRAMPLE_AURA, true);
            }

            void HandleOnRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                HandleOnEffect(aurEff, mode);
                if (Unit* target = GetTarget())
                    target->RemoveAurasDueToSpell(SPELL_TRAMPLE_AURA);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_ride_vehicle_toc5_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_CONTROL_VEHICLE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_ride_vehicle_toc5_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_CONTROL_VEHICLE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ride_vehicle_toc5_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_ride_vehicle_toc5_SpellScript();
        }
};

void AddSC_boss_grand_champions()
{
    new npc_faction_champion_toc5();
    new boss_grand_champion_toc5();
    new spell_ride_vehicle_toc5();
}
