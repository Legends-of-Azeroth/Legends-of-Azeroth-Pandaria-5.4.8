/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
*
* Portions derived from Millenium-Studio (Copyright 2015 Millenium-studio SARL).
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
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"
#include "ScriptedCreature.h"

/// Mini Mana Bomb - 65903
struct npc_toy_mini_maba_bomb : public ScriptedAI
{
    npc_toy_mini_maba_bomb(Creature* creature) : ScriptedAI(creature), _explodeTimer(4 * TimeConstants::IN_MILLISECONDS + 900) { }

    enum eSpell
    {
        ArcaneExplosionVisualLarge = 98122
    };

    void Reset() override
    {
        _explodeTimer = 4 * TimeConstants::IN_MILLISECONDS + 900;
    }

    void UpdateAI(uint32 const p_Diff) override
    {
        if (_explodeTimer <= p_Diff)
        {
            me->CastSpell(me, eSpell::ArcaneExplosionVisualLarge, true);
            me->DespawnOrUnsummon();
            return;
        }
        _explodeTimer -= p_Diff;
    }

    uint32 _explodeTimer;
};

void AddSC_npc_toy()
{
    RegisterCreatureAI(npc_toy_mini_maba_bomb);
}
