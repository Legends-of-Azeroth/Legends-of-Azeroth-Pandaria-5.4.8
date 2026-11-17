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
#include "utgarde_keep.h"

uint32 entry_search[3] =
{
    186609,
    186610,
    186611
};

class npc_dragonflayer_forge_master : public CreatureScript
{
    public:
        npc_dragonflayer_forge_master() : CreatureScript("npc_dragonflayer_forge_master") { }

        struct npc_dragonflayer_forge_masterAI : public ScriptedAI
        {
            npc_dragonflayer_forge_masterAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                fm_Type = 0;
            }

            InstanceScript* instance;
            uint8 fm_Type;

            void Reset() override
            {
                if (fm_Type == 0)
                    fm_Type = GetForgeMasterType();

                CheckForge();
            }

            void CheckForge()
            {
               if (instance)
                {
                    switch (fm_Type)
                    {
                        case 1:
                            instance->SetData(EVENT_FORGE_1,me->IsAlive() ? NOT_STARTED : DONE);
                            break;
                        case 2:
                            instance->SetData(EVENT_FORGE_2,me->IsAlive() ? NOT_STARTED : DONE);
                            break;
                        case 3:
                            instance->SetData(EVENT_FORGE_3,me->IsAlive() ? NOT_STARTED : DONE);
                            break;
                    }
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (fm_Type == 0)
                    fm_Type = GetForgeMasterType();

                if (instance)
                {
                    switch (fm_Type)
                    {
                        case 1:
                            instance->SetData(EVENT_FORGE_1,DONE);
                            break;
                        case 2:
                            instance->SetData(EVENT_FORGE_2,DONE);
                            break;
                        case 3:
                            instance->SetData(EVENT_FORGE_3,DONE);
                            break;
                    }
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (fm_Type == 0)
                    fm_Type = GetForgeMasterType();

                if (instance)
                {
                    switch (fm_Type)
                    {
                        case 1:
                            instance->SetData(EVENT_FORGE_1,IN_PROGRESS);
                            break;
                        case 2:
                            if (instance->GetData(EVENT_FORGE_1) != DONE)
                            {
                                EnterEvadeMode();
                                return;
                            }
                            instance->SetData(EVENT_FORGE_2,IN_PROGRESS);
                            break;
                        case 3:
                            if (instance->GetData(EVENT_FORGE_2) != DONE)
                            {
                                EnterEvadeMode();
                                return;
                            }
                            instance->SetData(EVENT_FORGE_3,IN_PROGRESS);
                            break;
                    }
                }
                me->HandleEmoteStateCommand(EMOTE_ONESHOT_NONE);
            }

            uint8 GetForgeMasterType()
            {
                float diff = 30.0f;
                int near_f = 0;

                for (uint8 i = 0; i < 3 ; ++i)
                {
                    GameObject* temp;
                    temp = me->FindNearestGameObject(entry_search[i],30);
                    if (temp)
                    {
                        if (me->IsWithinDist(temp,diff,false))
                        {
                            near_f = i + 1;
                            diff = me->GetDistance2d(temp);

                        }
                    }
                }

                switch (near_f)
                {
                    case 1:  return 1;
                    case 2:  return 2;
                    case 3:  return 3;
                    default: return 0;
                }
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (fm_Type == 0)
                    fm_Type = GetForgeMasterType();

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dragonflayer_forge_masterAI(creature);
        }
};

class spell_ticking_time_bomb : public SpellScriptLoader
{
    public:
        spell_ticking_time_bomb() : SpellScriptLoader("spell_ticking_time_bomb") { }

        class spell_ticking_time_bomb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ticking_time_bomb_AuraScript);

            void HandleDummyTick(AuraEffect const* aurEff)
            {
                if (Unit* caster = aurEff->GetCaster())
                    caster->CastSpell(caster, 59687, false);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ticking_time_bomb_AuraScript::HandleDummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ticking_time_bomb_AuraScript();
        }
};

void AddSC_utgarde_keep()
{
    new npc_dragonflayer_forge_master();
    new spell_ticking_time_bomb();
}
