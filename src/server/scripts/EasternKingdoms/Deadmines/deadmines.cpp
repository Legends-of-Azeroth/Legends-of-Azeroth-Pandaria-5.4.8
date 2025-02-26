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
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "deadmines.h"
#include "Spell.h"

#define GOSSIP_SENDER_DEADMINES_PORT 36

Position deadmines_locs[] =
{
    { -64.1528f, -385.99f, 53.192f, 1.85005f },
    { -305.321f, -491.292f, 49.232f, 0.488691f },
    { -201.096f, -606.05f, 19.3022f, 2.74016f },
    { -129.915f, -788.898f, 17.3409f, 0.366518f },
};

enum Adds
{
    // quest
    NPC_EDWIN_CANCLEEF_1    = 42697, 
    NPC_ALLIANCE_ROGUE      = 42700,
    NPC_VANESSA_VANCLEEF_1  = 42371, // little
};

// Prototype Reaper 49208
struct npc_deadmines_prototype_reaper : public ScriptedAI
{
    npc_deadmines_prototype_reaper(Creature* creature) : ScriptedAI(creature) { }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(90) && me->GetMap()->GetWorldState(WORLDSTATE_PROTOTYPE_PRODIGY))
            me->GetMap()->SetWorldState(WORLDSTATE_PROTOTYPE_PRODIGY, 0);
    }
    void UpdateAI(uint32 /*diff*/) override { }
};

class go_defias_cannon : public GameObjectScript
{
    public:
        go_defias_cannon() : GameObjectScript("go_defias_cannon") { }

        bool OnGossipHello(Player* /*player*/, GameObject* go) override
        {
            InstanceScript* instance = go->GetInstanceScript();
            if (!instance)
                return false;

            //if (instance->GetData(DATA_CANNON_EVENT) != CANNON_NOT_USED)
                //return false ;

            instance->SetData(DATA_CANNON_EVENT, CANNON_BLAST_INITIATED);
            return false;
        }
};

class go_deadmines_teleport : public GameObjectScript
{
    public:
        go_deadmines_teleport() : GameObjectScript("go_deadmines_teleport") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            bool ru = player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU;

            if (InstanceScript* instance = go->GetInstanceScript())
            {
                if (instance->GetBossState(DATA_GLUBTOK) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ru ? "Отправиться ко входу." : "Teleport to entrance.", GOSSIP_SENDER_DEADMINES_PORT, 0);

                if (instance->GetBossState(DATA_ADMIRAL) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ru ? "Отправиться в потайную бухту." : "Teleport to Ironclad Cove.", GOSSIP_SENDER_DEADMINES_PORT, 3);
                else if (instance->GetBossState(DATA_FOEREAPER) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ru ? "Отправиться в гоблинский цех." : "Teleport to The Foundry.", GOSSIP_SENDER_DEADMINES_PORT, 2);
                else if (instance->GetBossState(DATA_HELIX) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ru ? "Отправиться в мачтовую мастерскую." : "Teleport to The Mast Room.", GOSSIP_SENDER_DEADMINES_PORT, 1);
            }

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(go), go->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, GameObject* /*go*/, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();

            if (action >= 4)
                return false;

            Position loc = deadmines_locs[action];
            if (!player->IsInCombat())
                player->NearTeleportTo(loc.GetPositionX(), loc.GetPositionY(), loc.GetPositionZ(), loc.GetOrientation(), false);

            return true;
        }
};

// Off-line 88348
class spell_deadmines_off_line : public SpellScriptLoader
{
    public:
        spell_deadmines_off_line() : SpellScriptLoader("spell_deadmines_off_line") { }

        class spell_deadmines_off_line_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_deadmines_off_line_AuraScript);

            void OnAuraEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Creature* m_caster = GetOwner()->ToCreature())
                    m_caster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
            }

            void OnAuraEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Creature* m_caster = GetOwner()->ToCreature())
                    m_caster->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_deadmines_off_line_AuraScript::OnAuraEffectApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectApplyFn(spell_deadmines_off_line_AuraScript::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_deadmines_off_line_AuraScript();
        }
};

void AddSC_deadmines()
{
    new creature_script<npc_deadmines_prototype_reaper>("npc_deadmines_prototype_reaper");
    new go_defias_cannon();
    new go_deadmines_teleport();
    new spell_deadmines_off_line();
}