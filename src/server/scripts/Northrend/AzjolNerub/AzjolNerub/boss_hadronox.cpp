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

/*
* Comment: No Waves atm and the doors spells are crazy...
*
* When your group enters the main room (the one after the bridge), you will notice a group of 3 Nerubians.
* When you engage them, 2 more groups like this one spawn behind the first one - it is important to pull the first group back,
* so you don't aggro all 3. Hadronox will be under you, fighting Nerubians.
*
* This is the timed gauntlet - waves of non-elite spiders
* will spawn from the 3 doors located a little above the main room, and will then head down to fight Hadronox. After clearing the
* main room, it is recommended to just stay in it, kill the occasional non-elites that will attack you instead of the boss, and wait for
* Hadronox to make his way to you. When Hadronox enters the main room, she will web the doors, and no more non-elites will spawn.
*/

#include "ScriptPCH.h"
#include "azjol_nerub.h"

enum Spells
{
    // Hadronox
    SPELL_ACID_CLOUD        = 53400,
    SPELL_LEECH_POISON      = 53030,
    SPELL_LEECH_POISON_H    = 59417,
    SPELL_LEECH_POISON_HEAL = 53800,
    SPELL_PIERCE_ARMOR      = 53418,
    SPELL_WEB_GRAB          = 57731,

    // Anub'ar Crusher
    SPELL_SMASH             = 53318,
    SPELL_FRENZY            = 53801,

    // Anub'ar Champion
    SPELL_REND              = 53317,
    SPELL_PUMMEL            = 53394,

    // Anub'ar Crypt Fiend
    SPELL_INFECTED_WOUNDS   = 53330,
    SPELL_CRUSHING_WEBS     = 53322,
};

enum Events
{
    // Hadronox
    EVENT_ACID_CLOUD = 1,
    EVENT_LEECH_POISON,
    EVENT_PIERCE_ARMOR,
    EVENT_WEB_GRAB,

    // Anub'ar Crusher
    EVENT_SMASH,

    // Anub'ar Champion
    EVENT_REND,
    EVENT_PUMMEL_READY,

    // Anub'ar Crypt Fiend
    EVENT_INFECTED_WOUNDS,
    EVENT_CRUSHING_WEBS,
};

enum Points
{
    POINT_TOP = 1,
    POINT_DESCEND,
    POINT_PATH,
};

Position const southFrontDoorTopPos     = { 485.973083f, 611.002319f, 771.445984f };
Position const northFrontDoorTopPos     = { 574.743713f, 610.699829f, 771.422424f };
Position const southFrontDoorDescendPos = { 513.587341f, 587.266968f, 736.253479f };
Position const northFrontDoorDescendPos = { 543.906372f, 584.968567f, 731.606934f };

Position const sideDoorTopPos           = { 589.655762f, 596.762268f, 739.208191f };
Position const sideDoorDescendPos       = { 597.948853f, 587.418884f, 727.007202f };

Position const southCrusherPos          = { 519.881348f, 567.957336f, 734.215332f };
Position const northCrusherPos          = { 540.876770f, 568.185913f, 731.904846f };

#define PATH_LENGTH 13
Position const path[PATH_LENGTH] =
{
    { 531.225159f, 558.201538f, 733.072205f },
    { 542.184753f, 567.969421f, 731.599243f },
    { 568.166260f, 573.014832f, 727.083557f },
    { 593.469360f, 573.510254f, 724.240173f },
    { 608.736389f, 563.629272f, 719.219788f }, // side door first pos
    { 621.438782f, 541.842468f, 707.864502f },
    { 615.918030f, 521.365845f, 695.226685f },
    { 605.628784f, 513.657349f, 695.622986f },
    { 587.067322f, 512.367676f, 695.425659f },
    { 567.433411f, 513.558777f, 698.751709f },
    { 553.511292f, 519.990784f, 690.144775f },
    { 539.613770f, 529.312622f, 686.441956f },
    { 525.438171f, 540.663208f, 676.177917f },
};

class boss_hadronox : public CreatureScript
{
    public:
        boss_hadronox() : CreatureScript("boss_hadronox") { }

        struct boss_hadronoxAI : public ScriptedAI
        {
            boss_hadronoxAI(Creature* creature) : ScriptedAI(creature)
            {
                pathEnded = false;
                currentWaypointIndex = PATH_LENGTH - 1;
                me->setActive(true);
            }

            void Reset() override
            {
                events.Reset();

                me->GetMotionMaster()->MovePoint(POINT_PATH, path[currentWaypointIndex]);
                me->GetMap()->SetWorldState(WORLD_STATE_HADRONOX_DENIED, 1);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                    instance->SetData(DATA_HADRONOX_EVENT, IN_PROGRESS);

                events.ScheduleEvent(EVENT_ACID_CLOUD, urand(10000, 14000));
                events.ScheduleEvent(EVENT_LEECH_POISON, urand(3000, 9000));
                events.ScheduleEvent(EVENT_PIERCE_ARMOR, urand(1000, 3000));
                events.ScheduleEvent(EVENT_WEB_GRAB, urand(15000, 19000));
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                ScriptedAI::EnterEvadeMode();
                //if (InstanceScript* instance = me->GetInstanceScript())
                //    instance->SetData(DATA_HADRONOX_EVENT, FAIL);
            }

            // when Hadronox kills any enemy (that includes a party member) she will regain 10% of her HP if the target had Leech Poison on
            void KilledUnit(Unit* victim) override
            {
                // not sure if this aura check is correct, I think it is though
                if (!victim || !victim->HasAura(DUNGEON_MODE(SPELL_LEECH_POISON, SPELL_LEECH_POISON_H)) || !me->IsAlive())
                    return;

                DoCast(me, SPELL_LEECH_POISON_HEAL, true);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                    instance->SetData(DATA_HADRONOX_EVENT, DONE);
            }

            void DamageDealt(Unit* victim, uint32& damage, DamageEffectType /*type*/) override
            {
                // May or may not be required, depends mainly on luck and speed of the dungeon runners
                if (victim && (victim->GetEntry() == NPC_ANUBAR_CHAMPION_TRASH || victim->GetEntry() == NPC_ANUBAR_CRYPT_FIEND_TRASH))
                    damage += damage / 4;
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE || pointId != POINT_PATH)
                    return;

                switch (currentWaypointIndex)
                {
                    case 4:
                        DoCastAOE(SPELL_WEB_SIDE_DOORS, true);
                        break;
                    case 1:
                        DoCastAOE(SPELL_WEB_FRONT_DOORS, true);
                        me->SetHomePosition(path[0]);
                        break;
                    case 0:
                        pathEnded = true;
                        me->setActive(false);
                        return;
                    default:
                        break;
                }

                me->GetMotionMaster()->MovePoint(POINT_PATH, path[--currentWaypointIndex]);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (currentWaypointIndex == 0 && who->GetTypeId() == TYPEID_PLAYER && me->GetExactDist2d(who) <= 40.0f && who->GetPositionZ() < 750.0f && who->IsAlive() && me->IsValidAttackTarget(who))
                {
                    if (InstanceScript* instance = me->GetInstanceScript())
                    {
                        if (!instance->GetData(DATA_TRASH_LEFT))
                        {
                            AttackStart(who);
                            return;
                        }
                    }
                }
                if (who->GetTypeId() == TYPEID_UNIT && !who->IsPet())
                    ScriptedAI::MoveInLineOfSight(who);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ACID_CLOUD:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, false))
                                DoCast(target, SPELL_ACID_CLOUD);
                            events.ScheduleEvent(EVENT_ACID_CLOUD, urand(20000, 30000));
                            break;
                        case EVENT_LEECH_POISON:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, false))
                                DoCast(target, SPELL_LEECH_POISON);
                            events.ScheduleEvent(EVENT_LEECH_POISON, urand(11000, 14000));
                            break;
                        case EVENT_PIERCE_ARMOR:
                            DoCastVictim(SPELL_PIERCE_ARMOR);
                            events.ScheduleEvent(EVENT_PIERCE_ARMOR, 8000);
                            break;
                        case EVENT_WEB_GRAB:
                            DoCastAOE(SPELL_WEB_GRAB);
                            events.ScheduleEvent(EVENT_WEB_GRAB, urand(15000, 30000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            uint32 currentWaypointIndex;
            bool pathEnded;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_hadronoxAI(creature);
        }
};

class npc_anubar_crusher : public CreatureScript
{
    public:
        npc_anubar_crusher() : CreatureScript("npc_anubar_crusher") { }

        struct npc_anubar_crusherAI : public ScriptedAI
        {
            npc_anubar_crusherAI(Creature* creature) : ScriptedAI(creature)
            {
                summoned = false;
                if (InstanceScript* instance = me->GetInstanceScript())
                    instance->SetData(DATA_TRASH_LEFT, IN_PROGRESS);
            }

            void Reset() override
            {
                events.Reset();
                frenzied = false;
            }

            void JustEngagedWith(Unit* who) override
            {
                me->GetMotionMaster()->MoveChase(who);

                if (!summoned)
                    Talk(0);

                // Disabled until Map::GetHeight maxSearchDist is changed. Otherwise they will follow the player in the air.
                events.ScheduleEvent(EVENT_SMASH, DUNGEON_MODE(urand(4000, 9000), urand(4000, 7000)));

                if (InstanceScript* instance = me->GetInstanceScript())
                    instance->SetData(DATA_HADRONOX_EVENT, SPECIAL);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                ScriptedAI::EnterEvadeMode();
                if (InstanceScript* instance = me->GetInstanceScript())
                    instance->SetData(DATA_HADRONOX_EVENT, FAIL);
            }

            void IsSummonedBy(Unit* summoner) override
            {
                if (summoner && summoner->GetEntry() == NPC_WORLD_TRIGGER_LARGE_AOI)
                {
                    summoned = true;
                    Position dest = me->GetExactDistSq(&northFrontDoorTopPos) < me->GetExactDistSq(&southFrontDoorTopPos) ? northFrontDoorTopPos : southFrontDoorTopPos;
                    me->GetMotionMaster()->MovePoint(POINT_TOP, dest);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case POINT_TOP:
                    {
                        Position dest = me->GetExactDistSq(&northFrontDoorDescendPos) < me->GetExactDistSq(&southFrontDoorDescendPos) ? northFrontDoorDescendPos : southFrontDoorDescendPos;
                        me->GetMotionMaster()->MovePoint(POINT_DESCEND, dest);
                        break;
                    }
                    case POINT_DESCEND:
                    {
                        Position dest = me->GetExactDistSq(&northCrusherPos) < me->GetExactDistSq(&southCrusherPos) ? northCrusherPos : southCrusherPos;
                        me->GetMotionMaster()->MovePoint(POINT_PATH, dest);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_PLAYER && me->GetExactDist2d(who) <= 40.0f && who->GetPositionZ() < 750.0f && who->IsAlive() && me->IsValidAttackTarget(who))
                    AttackStart(who);
                else
                    ScriptedAI::MoveInLineOfSight(who);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (!frenzied && me->HealthBelowPctDamaged(30, damage))
                {
                    frenzied = true;
                    DoCast(me, SPELL_FRENZY);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                    instance->SetData(DATA_TRASH_LEFT, DONE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SMASH:
                            DoCastVictim(SPELL_SMASH);
                            events.ScheduleEvent(EVENT_SMASH, DUNGEON_MODE(urand(12000, 18000), urand(9000, 12000)));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            bool frenzied;
            bool summoned;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_anubar_crusherAI(creature);
        }
};

class npc_anubar_champion : public CreatureScript
{
    public:
        npc_anubar_champion() : CreatureScript("npc_anubar_champion") { }

        struct npc_anubar_championAI : public ScriptedAI
        {
            npc_anubar_championAI(Creature* creature) : ScriptedAI(creature)
            {
                isTrash = creature->GetEntry() == NPC_ANUBAR_CHAMPION_TRASH;
                nextWaypointIndex = 1;
                if (!isTrash)
                    if (InstanceScript* instance = me->GetInstanceScript())
                        instance->SetData(DATA_TRASH_LEFT, IN_PROGRESS);
            }

            void Reset() override
            {
                events.Reset();
                pummelReady = true;
            }

            void JustEngagedWith(Unit* who) override
            {
                me->GetMotionMaster()->MoveChase(who);
                events.ScheduleEvent(EVENT_REND, DUNGEON_MODE(urand(6000, 9000), urand(4000, 7000)));
            }

            void IsSummonedBy(Unit* summoner) override
            {
                if (summoner && summoner->GetEntry() == NPC_WORLD_TRIGGER_LARGE_AOI)
                {
                    Position dest = me->GetExactDistSq(&northFrontDoorTopPos) < me->GetExactDistSq(&southFrontDoorTopPos) ? northFrontDoorTopPos : southFrontDoorTopPos;
                    me->GetMotionMaster()->MovePoint(POINT_TOP, dest);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case POINT_TOP:
                    {
                        Position dest = me->GetExactDistSq(&northFrontDoorDescendPos) < me->GetExactDistSq(&southFrontDoorDescendPos) ? northFrontDoorDescendPos : southFrontDoorDescendPos;
                        me->GetMotionMaster()->MovePoint(POINT_DESCEND, dest);
                        break;
                    }
                    case POINT_DESCEND:
                    {
                        if (isTrash)
                        {
                            me->GetMotionMaster()->MovePoint(POINT_PATH, path[nextWaypointIndex++]);
                        }
                        else
                        {
                            Position dest = me->GetExactDistSq(&northCrusherPos) < me->GetExactDistSq(&southCrusherPos) ? northCrusherPos : southCrusherPos;
                            Position offset = { 6, 0, 0 };
                            dest.RelocateOffset(offset);
                            me->GetMotionMaster()->MovePoint(POINT_PATH, dest);
                        }
                        break;
                    }
                    case POINT_PATH:
                        if (isTrash)
                        {
                            if (nextWaypointIndex < PATH_LENGTH)
                                me->GetMotionMaster()->MovePoint(POINT_PATH, path[nextWaypointIndex++]);
                            else
                                me->DespawnOrUnsummon();
                        }
                        break;
                    default:
                        break;
                }
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!isTrash && who->GetTypeId() == TYPEID_PLAYER && me->GetExactDist2d(who) <= 40.0f && who->GetPositionZ() < 750.0f && who->IsAlive() && me->IsValidAttackTarget(who))
                    AttackStart(who);
                else
                    ScriptedAI::MoveInLineOfSight(who);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (!isTrash)
                    if (InstanceScript* instance = me->GetInstanceScript())
                        instance->SetData(DATA_TRASH_LEFT, DONE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_REND:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_REND);
                            events.ScheduleEvent(EVENT_REND, DUNGEON_MODE(urand(17000, 32000), urand(15000, 18000)));
                            break;
                        case EVENT_PUMMEL_READY:
                            pummelReady = true;
                            break;
                        default:
                            break;
                    }
                }

                if (pummelReady && me->GetVictim() && me->GetVictim()->HasUnitState(UNIT_STATE_CASTING))
                {
                    pummelReady = false;
                    DoCastVictim(SPELL_PUMMEL);
                    events.ScheduleEvent(EVENT_REND, DUNGEON_MODE(urand(14000, 17000), urand(9000, 12000)));
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            uint32 nextWaypointIndex;
            bool isTrash;
            bool pummelReady;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_anubar_championAI(creature);
        }
};

class npc_anubar_crypt_fiend : public CreatureScript
{
    public:
        npc_anubar_crypt_fiend() : CreatureScript("npc_anubar_crypt_fiend") { }

        struct npc_anubar_crypt_fiendAI : public ScriptedAI
        {
            npc_anubar_crypt_fiendAI(Creature* creature) : ScriptedAI(creature)
            {
                isTrash = creature->GetEntry() == NPC_ANUBAR_CRYPT_FIEND_TRASH;
                nextWaypointIndex = 4;
                if (!isTrash)
                    if (InstanceScript* instance = me->GetInstanceScript())
                        instance->SetData(DATA_TRASH_LEFT, IN_PROGRESS);
            }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_INFECTED_WOUNDS, urand(4000, 7000));
                events.ScheduleEvent(EVENT_CRUSHING_WEBS, urand(9000, 12000));
            }

            void IsSummonedBy(Unit* summoner) override
            {
                if (summoner && summoner->GetEntry() == 23472)
                {
                    if (isTrash)
                    {
                        me->GetMotionMaster()->MovePoint(POINT_TOP, sideDoorTopPos);
                    }
                    else
                    {
                        Position dest = me->GetExactDistSq(&northFrontDoorTopPos) < me->GetExactDistSq(&southFrontDoorTopPos) ? northFrontDoorTopPos : southFrontDoorTopPos;
                        me->GetMotionMaster()->MovePoint(POINT_TOP, dest);
                    }
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case POINT_TOP:
                    {
                        if (isTrash)
                            me->GetMotionMaster()->MovePoint(POINT_DESCEND, sideDoorDescendPos);
                        else
                        {
                            Position dest = me->GetExactDistSq(&northFrontDoorDescendPos) < me->GetExactDistSq(&southFrontDoorDescendPos) ? northFrontDoorDescendPos : southFrontDoorDescendPos;
                            me->GetMotionMaster()->MovePoint(POINT_DESCEND, dest);
                        }
                        break;
                    }
                    case POINT_DESCEND:
                    {
                        if (isTrash)
                            me->GetMotionMaster()->MovePoint(POINT_PATH, path[nextWaypointIndex++]);
                        else
                        {
                            Position dest = me->GetExactDistSq(&northCrusherPos) < me->GetExactDistSq(&southCrusherPos) ? northCrusherPos : southCrusherPos;
                            Position offset = { -6, 0, 0 };
                            dest.RelocateOffset(offset);
                            me->GetMotionMaster()->MovePoint(POINT_PATH, dest);
                        }
                        break;
                    }
                    case POINT_PATH:
                        if (isTrash)
                        {
                            if (nextWaypointIndex < PATH_LENGTH)
                                me->GetMotionMaster()->MovePoint(POINT_PATH, path[nextWaypointIndex++]);
                            else
                                me->DespawnOrUnsummon();
                        }
                        break;
                    default:
                        break;
                }
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!isTrash && who->GetTypeId() == TYPEID_PLAYER && me->GetExactDist2d(who) <= 40.0f && who->GetPositionZ() < 750.0f && who->IsAlive() && me->IsValidAttackTarget(who))
                    AttackStart(who);
                else
                    ScriptedAI::MoveInLineOfSight(who);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (!isTrash)
                    if (InstanceScript* instance = me->GetInstanceScript())
                        instance->SetData(DATA_TRASH_LEFT, DONE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INFECTED_WOUNDS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_INFECTED_WOUNDS);
                            events.ScheduleEvent(EVENT_INFECTED_WOUNDS, urand(9000, 12000));
                            break;
                        case EVENT_CRUSHING_WEBS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_CRUSHING_WEBS);
                            events.ScheduleEvent(EVENT_CRUSHING_WEBS, DUNGEON_MODE(urand(13000, 17000), urand(10000, 13000)));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            uint32 nextWaypointIndex;
            bool isTrash;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_anubar_crypt_fiendAI(creature);
        }
};

class spell_web_doors : public SpellScriptLoader
{
    public:
        spell_web_doors() : SpellScriptLoader("spell_web_doors") { }

        class spell_web_doors_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_web_doors_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                targets.clear();
                if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                {
                    if (GetSpellInfo()->Id == SPELL_WEB_FRONT_DOORS)
                    {
                        if (Unit* target = instance->instance->GetCreature(instance->GetGuidData(DATA_HADRONOX_FRONT_DOOR_1)))
                            targets.push_back(target);
                        if (Unit* target = instance->instance->GetCreature(instance->GetGuidData(DATA_HADRONOX_FRONT_DOOR_2)))
                            targets.push_back(target);
                    }
                    else if (Unit* target = instance->instance->GetCreature(instance->GetGuidData(DATA_HADRONOX_SIDE_DOOR)))
                        targets.push_back(target);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_web_doors_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_web_doors_SpellScript();
        }
};

void AddSC_boss_hadronox()
{
    new boss_hadronox();
    new npc_anubar_crusher();
    new npc_anubar_champion();
    new npc_anubar_crypt_fiend();
    new spell_web_doors();
}
