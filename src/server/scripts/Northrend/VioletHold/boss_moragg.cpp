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
#include "violet_hold.h"

enum Spells
{
    SPELL_CORROSIVE_SALIVA                     = 54527,
    SPELL_OPTIC_LINK                           = 54396,
    SPELL_RAY_OF_SUFFERING                     = 54442,
    SPELL_RAY_OF_SUFFERING_H                   = 59524,
    SPELL_RAY_OF_SUFFERING_TIGGER              = 54417,
    SPELL_RAY_OF_PAIN                          = 54438,
    SPELL_RAY_OF_PAIN_H                        = 59523,
    SPELL_RAY_OF_PAIN_TIGGER                   = 54416,
};

class boss_moragg : public CreatureScript
{
    public:
        boss_moragg() : CreatureScript("boss_moragg") { }

        struct boss_moraggAI : public ScriptedAI
        {
            boss_moraggAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint32 uiOpticLinkTimer;
            uint32 uiCorrosiveSalivaTimer;

            // Workaraound
            uint32 uiRaySuffer;
            uint32 uiRayPain;

            InstanceScript* instance;

            void Reset() override
            {
                uiOpticLinkTimer = 10000;
                uiCorrosiveSalivaTimer = 5000;

                uiRaySuffer = DUNGEON_MODE(5000, 3000);
                uiRayPain = DUNGEON_MODE(6500, 4500);

                if (instance)
                {
                    if (instance->GetData(DATA_WAVE_COUNT) == 6)
                        instance->SetData(DATA_1ST_BOSS_EVENT, NOT_STARTED);
                    else if (instance->GetData(DATA_WAVE_COUNT) == 12)
                        instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (instance)
                {
                    if (GameObject* pDoor = instance->instance->GetGameObject(instance->GetGuidData(DATA_MORAGG_CELL)))
                        if (pDoor->GetGoState() == GO_STATE_READY)
                        {
                            EnterEvadeMode();
                            return;
                        }
                    if (instance->GetData(DATA_WAVE_COUNT) == 6)
                        instance->SetData(DATA_1ST_BOSS_EVENT, IN_PROGRESS);
                    else if (instance->GetData(DATA_WAVE_COUNT) == 12)
                        instance->SetData(DATA_2ND_BOSS_EVENT, IN_PROGRESS);
                }

                //DoCast(me, DUNGEON_MODE(SPELL_RAY_OF_SUFFERING,SPELL_RAY_OF_SUFFERING_H), true);
                //DoCast(me, DUNGEON_MODE(SPELL_RAY_OF_PAIN,SPELL_RAY_OF_PAIN_H), true);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                ScriptedAI::EnterEvadeMode();
                if (instance)
                    instance->SetData(DATA_WIPE, 1);
            }

            void AttackStart(Unit* who) override
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC) || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (me->Attack(who, true))
                {
                    me->AddThreat(who, 0.0f);
                    me->SetInCombatWith(who);
                    who->SetInCombatWith(me);
                    DoStartMovement(who);
                }
            }

            void MoveInLineOfSight(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (uiRaySuffer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        DoCast(target, SPELL_RAY_OF_SUFFERING_TIGGER, true);
                    uiRaySuffer = DUNGEON_MODE(5000, 3000);
                } else uiRaySuffer -= diff;

                if (uiRayPain <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        DoCast(target, SPELL_RAY_OF_PAIN_TIGGER, true);
                    uiRayPain = DUNGEON_MODE(6500, 4500);
                } else uiRayPain -= diff;

                if (uiOpticLinkTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        DoCast(target, SPELL_OPTIC_LINK);
                    uiOpticLinkTimer = 15000;
                } else uiOpticLinkTimer -= diff;

                if (uiCorrosiveSalivaTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(me->GetVictim(), SPELL_CORROSIVE_SALIVA);
                        uiCorrosiveSalivaTimer = 10000;
                    }
                } else uiCorrosiveSalivaTimer -= diff;

                DoMeleeAttackIfReady();
            }
            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                {
                    if (instance->GetData(DATA_WAVE_COUNT) == 6)
                    {
                        if (IsHeroic() && instance->GetData(DATA_1ST_BOSS_EVENT) == DONE)
                            me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                        instance->SetData(DATA_1ST_BOSS_EVENT, DONE);
                        instance->SetData(DATA_WAVE_COUNT, 7);
                    }
                    else if (instance->GetData(DATA_WAVE_COUNT) == 12)
                    {
                        if (IsHeroic() && instance->GetData(DATA_2ND_BOSS_EVENT) == DONE)
                            me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                        instance->SetData(DATA_2ND_BOSS_EVENT, DONE);
                        instance->SetData(DATA_WAVE_COUNT,13);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_moraggAI(creature);
        }
};

void AddSC_boss_moragg()
{
    new boss_moragg();
}
