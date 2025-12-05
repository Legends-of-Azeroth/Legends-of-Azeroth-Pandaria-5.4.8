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

/* ScriptData
SDName: Westfall
SD%Complete: 0
SDComment:
SDCategory: Westfall
EndScriptData */

/* ContentData
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "EventProcessor.h"

// Wake Harvest Golem 79436
class spell_westfall_wake_harvest_golem : public SpellScript
{
    PrepareSpellScript(spell_westfall_wake_harvest_golem);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Player* caster = GetCaster()->ToPlayer())
            caster->KilledMonsterCredit(GetSpellInfo()->Effects[EFFECT_1].MiscValue);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_westfall_wake_harvest_golem::HandleHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Quest 26291 - Livin' the Life
// Prevents multiple script executions and ensures NPC appears correctly
enum LivinTheLife
{
    QUEST_LIVIN_THE_LIFE = 26291,
    NPC_TWO_SHOED_LOU = 42317,
    GO_TWO_SHOED_LOUS_OLD_HOUSE = 204351,
    SPELL_SUMMON_TWO_SHOED_LOU = 78882, // Spell that summons the NPC (if exists)
};

class go_two_shoed_lous_old_house : public GameObjectScript
{
public:
    go_two_shoed_lous_old_house() : GameObjectScript("go_two_shoed_lous_old_house") { }

    struct go_two_shoed_lous_old_houseAI : public GameObjectAI
    {
        go_two_shoed_lous_old_houseAI(GameObject* go) : GameObjectAI(go), _scriptInProgress(false), _lastUseTime(0) { }

        bool GossipHello(Player* player) override
        {
            // Prevent multiple clicks within 5 seconds
            uint32 currentTime = getMSTime();
            if (_scriptInProgress || (currentTime - _lastUseTime) < 5000)
                return true; // Block the action

            // Check if player has the quest
            if (player->GetQuestStatus(QUEST_LIVIN_THE_LIFE) != QUEST_STATUS_INCOMPLETE)
                return false;

            // Set flag and timestamp to prevent multiple clicks
            _scriptInProgress = true;
            _lastUseTime = currentTime;

            // Summon Two-Shoed Lou if not already present nearby
            Position pos = go->GetPosition();
            pos.RelocateOffset(2.0f, 0.0f, 0.0f); // Spawn slightly in front of the house
            
            // Check if NPC already exists nearby
            std::list<Creature*> creatures;
            go->GetCreatureListWithEntryInGrid(creatures, NPC_TWO_SHOED_LOU, 10.0f);
            
            if (creatures.empty())
            {
                if (Creature* lou = go->SummonCreature(NPC_TWO_SHOED_LOU, pos, TEMPSUMMON_TIMED_DESPAWN, 60000))
                {
                    lou->SetFacingToObject(go);
                    // Make NPC talk/emote
                    lou->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                }
            }
            else
            {
                // NPC already exists, make it face the house and talk
                for (Creature* lou : creatures)
                {
                    lou->SetFacingToObject(go);
                    lou->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                }
            }

            // Reset flag after delay
            go->m_Events.AddEvent(new ResetScriptEvent(go), go->m_Events.CalculateTime(5000));

            return false; // Allow normal quest processing
        }

    private:
        bool _scriptInProgress;
        uint32 _lastUseTime;

        class ResetScriptEvent : public BasicEvent
        {
        public:
            ResetScriptEvent(GameObject* go) : _go(go) { }

            bool Execute(uint64 /*time*/, uint32 /*diff*/) override
            {
                if (go_two_shoed_lous_old_houseAI* ai = dynamic_cast<go_two_shoed_lous_old_houseAI*>(_go->AI()))
                    ai->_scriptInProgress = false;
                return true;
            }

        private:
            GameObject* _go;
        };
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_two_shoed_lous_old_houseAI(go);
    }
};

void AddSC_westfall()
{
    new spell_script<spell_westfall_wake_harvest_golem>("spell_westfall_wake_harvest_golem");
    new go_two_shoed_lous_old_house();
}
