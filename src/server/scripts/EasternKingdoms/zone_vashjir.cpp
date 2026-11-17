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

// http://www.wowhead.com/quest=25281/pay-it-forward http://www.wowhead.com/quest=25936/pay-it-forward
enum QuestPayItForward
{
    SPELL_BLOW_BUBBLE_A  = 74151,
    SPELL_BUBBLE_SELF    = 74416,
    SPELL_BLOW_BUBBLE_H  = 77825,

    NPC_DROWNING_SOLDIER = 39663,
    NPC_DROWNING_WARRIOR = 41672
};

struct npc_drowning_soldier_and_warrior : public ScriptedAI
{
    npc_drowning_soldier_and_warrior(Creature* creature) : ScriptedAI(creature)
    {
        JustAppeared();
    }

    void JustAppeared() override
    {
        me->setRegeneratingHealth(false);
        me->SetHealth(6190);
        move_timer = 0;
    }

    void SpellHit(Unit* caster, const SpellInfo* spell) override
    {
        if (Player* player = caster->ToPlayer())
        {
            if (me->HasAura(SPELL_BUBBLE_SELF))
                return;
            if (spell->Id == SPELL_BLOW_BUBBLE_A)
                player->KilledMonsterCredit(NPC_DROWNING_SOLDIER);
            else if (spell->Id == SPELL_BLOW_BUBBLE_H)
                player->KilledMonsterCredit(NPC_DROWNING_WARRIOR);
            me->CastSpell(me, SPELL_BUBBLE_SELF, true);
            move_timer = 3000;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!move_timer)
            return;

        move_timer -= diff;
        if (move_timer <= 0)
        {
            move_timer = 0;

            // Бежать вперёд на 15 ярдов
            const float angle = (me->GetOrientation()) - static_cast<float>(M_PI / 2);
            const float x = me->GetPositionX() - 15 * std::sin(angle);
            const float y = me->GetPositionY() + 15 * std::cos(angle);
            const float z = me->GetMap()->GetHeight(x, y, me->GetPositionZ());
            me->GetMotionMaster()->MovePoint(0, x, y, z);

            me->DespawnOrUnsummon(5000);
        }
    }
private:
    int32 move_timer;
};

void AddSC_vashjir()
{
    new creature_script<npc_drowning_soldier_and_warrior>("npc_drowning_soldier_and_warrior");
}
