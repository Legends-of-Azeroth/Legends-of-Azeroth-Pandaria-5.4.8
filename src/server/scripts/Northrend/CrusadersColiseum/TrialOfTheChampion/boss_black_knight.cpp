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

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"
#include "trial_of_the_champion.h"

enum Yells
{
    SAY_INTRO_1             = 0,
    SAY_INTRO_2             = 1,
    SAY_INTRO_3             = 2,
    SAY_AGGRO               = 3,
    SAY_PHASE_2             = 4,
    SAY_PHASE_3             = 5,
    SAY_KILL                = 6,
    SAY_DEATH               = 7,
};

enum Spells
{
    // Phase 1
    SPELL_FEIGN_DEATH       = 66804, // 1
    SPELL_RAISE_BRIGHTSTAR  = 67705, // 46
    SPELL_RAISE_SUNSWORN    = 67715, // 46
    SPELL_PLAGUE_STRIKE     = 67724,
    SPELL_ICY_TOUCH         = 67718,
    SPELL_DEATH_RESPITE     = 67745,
    SPELL_DEATH_RESPITE_3   = 66798,
    SPELL_OBLITERATE_H      = 67883,
    SPELL_OBLITERATE        = 67725,

    // Phase 2
    SPELL_ARMY_DEAD         = 67761,
    SPELL_DESECRATION       = 67778,
    SPELL_GHOUL_EXPLODE     = 67751,

    // Phase 3
    SPELL_DEATH_BITE        = 67808,
    SPELL_MARKED_DEATH      = 67823,

    SPELL_BLACK_KNIGHT_RES  = 67693,

    SPELL_LEAP              = 67749,
    SPELL_CLAW              = 67774,
    SPELL_EXPLODE           = 67729,
};

enum Models
{
    MODEL_SKELETON          = 29846,
    MODEL_GHOST             = 21300
};

enum Eqip
{
    EQUIP_SWORD             = 40343
};

enum Phases
{
    PHASE_UNDEAD            = 1,
    PHASE_SKELETON          = 2,
    PHASE_GHOST             = 3
};

enum Events
{
    EVENT_CLAW              = 1,
    EVENT_LEAP              = 2
};

enum BlackKnightCreatures
{
    NPC_RISEN_CHAMPION      = 35590,
    NPC_RISEN_BRIGHTSTAR    = 35564,
    NPC_RISEN_SUNSWORN      = 35545,
};

class boss_black_knight : public CreatureScript
{
    public:
        boss_black_knight() : CreatureScript("boss_black_knight") { }

        struct boss_black_knightAI : public ScriptedAI
        {
            boss_black_knightAI(Creature* creature) : ScriptedAI(creature), _summons(me)
            {
                _instance = creature->GetInstanceScript();
                isAttacked = false;
            }

            InstanceScript* _instance;
            SummonList _summons;

            bool isResurrecting;
            bool isSummoningArmy;
            bool isDeathArmySummoned;
            bool isAttacked;

            uint8 phase;
            uint8 uiIntroPhase;
            uint32 uiIntroTimer;

            uint32 plagueStrikeTimer;
            uint32 icyTouchTimer;
            uint32 deathRespiteTimer;
            uint32 obliterateTimer;
            uint32 desecrationTimer;
            uint32 resurrectTimer;
            uint32 deathArmyCheckTimer;
            uint32 ghoulExplodeTimer;
            uint32 deathBiteTimer;
            uint32 markedDeathTimer;

            void Reset() override
            {
                _summons.DespawnAll();

                me->SetDisplayId(me->GetNativeDisplayId());
                me->ClearUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED);
                me->GetMap()->SetWorldState(WORLDSTATE_I_VE_HAD_WORSE, 1);

                isResurrecting = false;
                isSummoningArmy = false;
                isDeathArmySummoned = false;

                if (GameObject* go = GameObject::GetGameObject(*me, _instance->GetGuidData(DATA_MAIN_GATE)))
                    _instance->HandleGameObject(go->GetGUID(), true);
                if (GameObject* go = GameObject::GetGameObject(*me, _instance->GetGuidData(DATA_PORTCULLIS)))
                    _instance->HandleGameObject(go->GetGUID(), false);

                if (isAttacked)
                {
                    me->GetMotionMaster()->MovePoint(1, 743.396f, 635.411f, 411.575f);
                    me->SetFaction(14);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
                }

                phase = PHASE_UNDEAD;

                icyTouchTimer = urand(3000, 5000);
                plagueStrikeTimer = urand(4000, 7000);
                deathRespiteTimer = urand(5000, 8000);
                obliterateTimer = urand(10000, 12000);
                desecrationTimer = urand(12000, 15000);
                deathArmyCheckTimer = 1000;
                resurrectTimer = 4000;
                ghoulExplodeTimer = 6000;
                deathBiteTimer = urand(2000, 4000);
                markedDeathTimer = urand(5000, 7000);
                uiIntroTimer = 5000;
            }

            void JustSummoned(Creature* summon) override
            {
                _summons.Summon(summon);
                DoZoneInCombat(summon, 200.0f);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                me->ClearUnitState(UNIT_STATE_STUNNED | UNIT_STATE_ROOT);
                ScriptedAI::EnterEvadeMode();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                isAttacked = true;
                Talk(SAY_AGGRO);
                DoZoneInCombat(me, 150.0f);
                SetEquipmentSlots(false, EQUIP_SWORD, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);

                if (GameObject* go = GameObject::GetGameObject(*me, _instance->GetGuidData(DATA_MAIN_GATE)))
                    _instance->HandleGameObject(go->GetGUID(), false);
                if (GameObject* go = GameObject::GetGameObject(*me, _instance->GetGuidData(DATA_PORTCULLIS)))
                    _instance->HandleGameObject(go->GetGUID(), false);

                if (_instance)
                    _instance->SetData(BOSS_BLACK_KNIGHT, IN_PROGRESS);

                // TODO: add Intro
                if (Creature* brightstar = me->FindNearestCreature(NPC_ARELAS, 250.0f))
                {
                   me->Kill(brightstar);
                    brightstar->DespawnOrUnsummon(1000);
                    me->SummonCreature(NPC_RISEN_BRIGHTSTAR, *brightstar);
                }
                else
                   me->SummonCreature(NPC_RISEN_BRIGHTSTAR, *me);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (isResurrecting)
                {
                    damage = 0;
                    return;
                }

                if (damage >= me->GetHealth() && phase <= PHASE_SKELETON)
                {
                    damage = 0;
                    me->SetHealth(0);
                    me->AddUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED);
                    me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);

                    switch (phase)
                    {
                        case PHASE_UNDEAD:
                            me->SetDisplayId(MODEL_SKELETON);
                            break;
                        case PHASE_SKELETON:
                            me->SetDisplayId(MODEL_GHOST);
                            break;
                    }
                    isResurrecting = true;
                }
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                DoCast(me, SPELL_BLACK_KNIGHT_CREDIT, true);
                Talk(SAY_DEATH);

                if (GameObject* go = GameObject::GetGameObject(*me, _instance ? _instance->GetGuidData(DATA_PORTCULLIS) : ObjectGuid::Empty))
                    _instance->HandleGameObject(go->GetGUID(), true);

                if (_instance)
                    _instance->SetData(BOSS_BLACK_KNIGHT, DONE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (isResurrecting)
                {
                    if (resurrectTimer <= diff)
                    {
                        me->SetFullHealth();
                        switch (phase)
                        {
                            case PHASE_UNDEAD:
                                Talk(SAY_PHASE_2);
                                break;
                            case PHASE_SKELETON:
                                Talk(SAY_PHASE_3);
                                break;
                        }
                        DoCast(me, SPELL_BLACK_KNIGHT_RES, true);
                        me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
                        ++phase;
                        resurrectTimer = 4000;
                        isResurrecting = false;
                        me->ClearUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED);
                    }
                    else
                        resurrectTimer -= diff;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING) || isResurrecting)
                    return;

                switch (phase)
                {
                    case PHASE_UNDEAD:
                    case PHASE_SKELETON:
                    {
                        if (icyTouchTimer <= diff)
                        {
                            DoCastVictim(SPELL_ICY_TOUCH);
                            icyTouchTimer = urand(5000, 7000);
                        }
                        else
                            icyTouchTimer -= diff;

                        if (plagueStrikeTimer <= diff)
                        {
                            DoCastVictim(SPELL_PLAGUE_STRIKE);
                            plagueStrikeTimer = urand(12000, 15000);
                        }
                        else
                            plagueStrikeTimer -= diff;

                        if (obliterateTimer <= diff)
                        {
                            DoCastVictim(SPELL_OBLITERATE);
                            obliterateTimer = urand(12000, 17000);
                        }
                        else
                            obliterateTimer -= diff;

                        switch (phase)
                        {
                            case PHASE_UNDEAD:
                            {
                                if (deathRespiteTimer <= diff)
                                {
                                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                        DoCast(target, SPELL_DEATH_RESPITE);
                                     deathRespiteTimer = urand(10000, 12000);
                                }
                                else
                                    deathRespiteTimer -= diff;
                                break;
                            }
                            case PHASE_SKELETON:
                            {
                                if (!isSummoningArmy)
                                {
                                    isSummoningArmy = true;
                                    me->AddUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED);
                                    me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
                                    DoCast(me, SPELL_ARMY_DEAD);
                                //me->SetReactState(REACT_PASSIVE);
                                //deathArmyCheckTimer = 7000;
                                }

                                if (!isDeathArmySummoned)
                                {
                                    if (deathArmyCheckTimer <= diff)
                                    {
                                        me->ClearUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED);
                                        deathArmyCheckTimer = 0;
                                        ghoulExplodeTimer = urand(3000, 5000);
                                        isDeathArmySummoned = true;
                                        //me->SetReactState(REACT_AGGRESSIVE);
                                    }
                                    else
                                        deathArmyCheckTimer -= diff;
                                }

                                if (desecrationTimer <= diff)
                                {
                                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                                        DoCast(target, SPELL_DESECRATION);
                                    desecrationTimer = urand(15000, 16000);
                                }
                                else
                                    desecrationTimer -= diff;

                                if (ghoulExplodeTimer <= diff)
                                {
                                    DoCast(me, SPELL_GHOUL_EXPLODE);
                                    ghoulExplodeTimer = 5000;
                                }
                                else
                                    ghoulExplodeTimer -= diff;
                                break;
                            }
                            break;
                        }
                        break;
                    }
                    case PHASE_GHOST:
                    {
                        if (deathBiteTimer <= diff)
                        {
                            DoCast(SPELL_DEATH_BITE);
                            deathBiteTimer = 3000;
                        }
                        else
                            deathBiteTimer -= diff;

                        if (markedDeathTimer <= diff)
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                            {
                                if (target && target->IsAlive())
                                DoCast(target,SPELL_MARKED_DEATH);
                            }
                            markedDeathTimer = 10000;
                        }
                        else
                            markedDeathTimer -= diff;
                        break;
                    }
                }

                if (!me->HasUnitState(UNIT_STATE_STUNNED))
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_black_knightAI(creature);
        }
};

class npc_risen_ghoul : public CreatureScript
{
    public:
        npc_risen_ghoul() : CreatureScript("npc_risen_ghoul") { }

        struct npc_risen_ghoulAI : public ScriptedAI
        {
            npc_risen_ghoulAI(Creature* creature) : ScriptedAI(creature)
            {
                _instance = me->GetInstanceScript();
            }

            void Reset() override
            {
                _events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _events.ScheduleEvent(EVENT_CLAW, urand(5000, 10000));

                if (me->GetEntry() != NPC_RISEN_CHAMPION)
                    _events.ScheduleEvent(EVENT_LEAP, urand(10000, 15000));
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_GHOUL_EXPLODE)
                {
                    _events.Reset();
                    DoCast(SPELL_EXPLODE);
                }
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_EXPLODE && target->GetTypeId() == TYPEID_PLAYER)
                    me->GetMap()->SetWorldState(WORLDSTATE_I_VE_HAD_WORSE, 1);
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
                        case EVENT_CLAW:
                            if (me->IsWithinCombatRange(me->GetVictim(), 5.0f))
                            {
                                DoCastVictim(SPELL_CLAW);
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 40.0f, true))
                                {
                                    DoResetThreat();
                                    me->AddThreat(target, 1000.0f);
                                }
                                _events.ScheduleEvent(EVENT_CLAW, urand(4000, 8000));
                            }
                            else
                                _events.ScheduleEvent(EVENT_CLAW, 1000);
                            break;
                        case EVENT_LEAP:
                            if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 1, 30.0f, true))
                            {
                                DoResetThreat();
                                me->AddThreat(target, 1000.0f);
                                DoCast(target, SPELL_LEAP);
                            }
                            _events.ScheduleEvent(EVENT_LEAP, urand(10000, 12000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            InstanceScript* _instance;
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_risen_ghoulAI(creature);
        }
};

class npc_black_knight_skeletal_gryphon : public CreatureScript
{
    public:
        npc_black_knight_skeletal_gryphon() : CreatureScript("npc_black_knight_skeletal_gryphon") { }

        struct npc_black_knight_skeletal_gryphonAI : public npc_escortAI
        {
            npc_black_knight_skeletal_gryphonAI(Creature* creature) : npc_escortAI(creature)
            {
                Start(false, true, ObjectGuid::Empty, NULL);
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            void Reset() override
            {
                me->UpdateObjectVisibility();
            }

            void WaypointReached(uint32 uiPointId) override
            {
                switch (uiPointId)
                {
                        case 1:
                            me->SetSpeed(MOVE_RUN , 2.0f);
                            break;
                        case 2:
                            me->SetSpeed(MOVE_RUN , 2.0f);
                            if (instance)
                                instance->SetData(DATA_BLACK_KNIGHT, NOT_STARTED);
                            break;
                        case 3:
                            me->SetSpeed(MOVE_RUN , 2.0f);
                            break;
                        case 4:
                            me->SetSpeed(MOVE_RUN , 2.0f);
                            break;
                        case 5:
                            me->SetSpeed(MOVE_RUN , 2.0f);
                            break;
                        case 6:
                            me->SetSpeed(MOVE_RUN , 2.0f);
                            break;
                        case 7:
                            me->SetSpeed(MOVE_RUN , 2.0f);
                            break;
                        case 8:
                            me->SetSpeed(MOVE_RUN , 2.0f);
                            break;
                        case 9:
                            me->SetSpeed(MOVE_RUN , 2.0f);
                            break;
                        case 10:
                            me->SetSpeed(MOVE_RUN , 2.0f);
                            break;
                        case 11:
                            me->SetSpeed(MOVE_RUN , 2.0f);
                            break;
                        case 12:
                            me->SetWalk(true);
                            me->SetSpeed(MOVE_RUN, 2.0f);
                            break;
                        case 13:
                            me->SetWalk(true);
                            me->SummonCreature(VEHICLE_GR,744.841f,634.505f,411.575f, 2.79f);
                            break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                npc_escortAI::UpdateAI(diff);

                if (!UpdateVictim())
                    return;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_black_knight_skeletal_gryphonAI(creature);
        }

};

class npc_gr : public CreatureScript
{
    public:
        npc_gr() : CreatureScript("npc_gr") { }

        struct npc_grAI : public npc_escortAI
        {

            npc_grAI(Creature* creature) : npc_escortAI(creature)
            {
                Start(false, true, ObjectGuid::Empty, NULL);
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            void WaypointReached(uint32 uiPointId) override
            {
                switch (uiPointId)
                {
                        case 1:
                            if (instance)
                                instance->SetData(DATA_BLACK_KNIGHT, IN_PROGRESS);
                            break;
                        case 2:
                            if (instance)
                                instance->SetData(DATA_BLACK_KNIGHT, DONE);
                            break;
                        case 3:
                            break;
                        case 4:
                            //if (instance)
                            //{
                            //    instance->SetData(DATA_KNIGHT, NOT_STARTED);
                            //}
                            break;
                        case 5:
                            //if (instance)
                            //{
                            //    instance->SetData(DATA_KNIGHT, IN_PROGRESS);
                            //}
                            break;
                        case 6:
                            break;
                        case 7:
                            break;
                        case 8:
                        case 9:
                            //if (instance)
                            //{
                            //    instance->SetData(DATA_KNIGHT, DONE);
                            //}
                            break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                npc_escortAI::UpdateAI(diff);

                if (!UpdateVictim())
                    return;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_grAI(creature);
        }
};

/*class npc_black_knight_skeletal_gryphon : public CreatureScript
{
    public:
        npc_black_knight_skeletal_gryphon() : CreatureScript("npc_black_knight_skeletal_gryphon") { }

        struct npc_black_knight_skeletal_gryphonAI : public npc_escortAI
        {
            npc_black_knight_skeletal_gryphonAI(Creature* creature) : npc_escortAI(creature)
            {
                Start(false, true, 0, NULL);
                me->SetFlying(true);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
            }

            void WaypointReached(uint32 waypointId) override { }

            void AttackStart(Unit* who) override { }

            void UpdateAI(uint32 diff) override
            {
                npc_escortAI::UpdateAI(diff);

                if (!UpdateVictim())
                    return;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_black_knight_skeletal_gryphonAI(creature);
        }
};*/

void AddSC_boss_black_knight()
{
    new boss_black_knight();
    new npc_risen_ghoul();
    new npc_black_knight_skeletal_gryphon();
    new npc_gr();
}
