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

#include "BattlePetAbilityEffect.h"
#include "DBCStores.h"
#include "BattlePet.h"
#include "PetBattle.h"

struct BattlePetAbilityEffectHandler
{
    void (BattlePetAbilityEffect::*Handle)();
    PetBattleAbilityTarget Target;
};

static BattlePetAbilityEffectHandler Handlers[PET_BATTLE_TOTAL_ABILITY_EFFECTS] =
{
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 022 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 023 */ { &BattlePetAbilityEffect::HandleHeal,                     PET_BATTLE_ABILITY_TARGET_CASTER },
    /* Effect 024 */ { &BattlePetAbilityEffect::HandleDamage,                   PET_BATTLE_ABILITY_TARGET_TARGET },
    /* Effect 025 */ { &BattlePetAbilityEffect::HandleCatch,                    PET_BATTLE_ABILITY_TARGET_TARGET },
    /* Effect 026 */ { &BattlePetAbilityEffect::HandlePositiveAura,             PET_BATTLE_ABILITY_TARGET_CASTER },
    /* Effect 027 */ { &BattlePetAbilityEffect::HandleRampingDamage,            PET_BATTLE_ABILITY_TARGET_TARGET },
    /* Effect 028 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 029 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 030 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 031 */ { &BattlePetAbilityEffect::HandleSetState,                 PET_BATTLE_ABILITY_TARGET_CASTER },
    /* Effect 032 */ { &BattlePetAbilityEffect::HandleHealPctDealt,             PET_BATTLE_ABILITY_TARGET_CASTER },
    /* Effect 033 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 043 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 044 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 045 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 049 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 050 */ { &BattlePetAbilityEffect::HandleNegativeAura,             PET_BATTLE_ABILITY_TARGET_TARGET },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 052 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 053 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 054 */ { &BattlePetAbilityEffect::HandlePeriodicTrigger,          PET_BATTLE_ABILITY_TARGET_TARGET },
    /* Effect 055 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 056 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 057 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 058 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 059 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 061 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 062 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 063 */ { &BattlePetAbilityEffect::HandlePeriodicPositiveTrigger,  PET_BATTLE_ABILITY_TARGET_CASTER },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 065 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 066 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 067 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 068 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 072 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 073 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 074 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 075 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 076 */ { &BattlePetAbilityEffect::HandleDamageToggleAura,         PET_BATTLE_ABILITY_TARGET_TARGET },
    /* Effect 077 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 078 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 079 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 080 */ { &BattlePetAbilityEffect::HandleWeatherAura,              PET_BATTLE_ABILITY_TARGET_HEAD   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 085 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 086 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 096 */ { &BattlePetAbilityEffect::HandleDamageHitState,           PET_BATTLE_ABILITY_TARGET_TARGET },
    /* Effect 097 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 099 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 100 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 103 */ { &BattlePetAbilityEffect::HandleExtraAttackFirst,         PET_BATTLE_ABILITY_TARGET_TARGET },
    /* Effect 104 */ { &BattlePetAbilityEffect::HandleHealState,                PET_BATTLE_ABILITY_TARGET_TARGET },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 107 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 108 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 111 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 112 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 116 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 117 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 121 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 122 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 123 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 124 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 125 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 128 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 129 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 131 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 132 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 133 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 134 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 135 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 136 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 137 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 138 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 139 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 140 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 141 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 142 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 143 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 144 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 145 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 147 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 149 */ { &BattlePetAbilityEffect::HandleDamageNonLethal,          PET_BATTLE_ABILITY_TARGET_TARGET },
    /* Effect 150 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 156 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 157 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 158 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 159 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 160 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 164 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 165 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 168 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 169 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 170 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 171 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 172 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 177 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 178 */ { &BattlePetAbilityEffect::HandlePowerlessAura,            PET_BATTLE_ABILITY_TARGET_TARGET },
    /* Effect 179 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 194 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /*   UNUSED   */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   },
    /* Effect 197 */ { &BattlePetAbilityEffect::HandleNull,                     PET_BATTLE_ABILITY_TARGET_NONE   }
};

void BattlePetAbilityEffect::SetAbilityInfo(uint32 ability, BattlePetAbilityEffectEntry const* effectEntry, uint8 family)
{
    // ability information
    m_abilityId     = ability;
    m_effectEntry   = effectEntry;
    m_abilityFamily = family;
}

bool BattlePetAbilityEffect::Execute()
{
    if (!Handlers[m_effectEntry->EffectProperty].Handle)
        return false;

    if (m_targets.empty())
    {
        /*// ----------------------------------------------------------------------------------
        std::stringstream ss;
        ss << "DEBUG: Ability " << m_abilityId << " (";
        ss << sBattlePetAbilityStore.LookupEntry(m_abilityId)->Name << ") has unhandled effect ";
        ss << m_effectEntry->EffectProperty << "!";

        ChatHandler(m_petBattle->Teams[PET_BATTLE_CHALLANGER_TEAM]->GetOwner()->GetSession()).PSendSysMessage(ss.str().c_str());
        // ----------------------------------------------------------------------------------*/

        return false;
    }

    // execute handler on targets
    for (auto&& target : m_targets)
    {
        m_target = target;

        if (!m_target->IsTargetable())
            m_flags |= PET_BATTLE_EFFECT_FLAG_MISS;

        (this->*Handlers[m_effectEntry->EffectProperty].Handle)();
    }

    return true;
}

BattlePet* BattlePetAbilityEffect::GetActiveOpponent()
{
    PetBattleTeamIndex index = m_caster->GetTeamIndex() == PET_BATTLE_TEAM_OPPONENT ? PET_BATTLE_TEAM_CHALLANGER : PET_BATTLE_TEAM_OPPONENT;
    return m_petBattle->GetTeam(index)->GetActivePet();
}

void BattlePetAbilityEffect::AddTargets()
{
    auto casterTeam = m_petBattle->GetTeam(m_caster->GetTeamIndex());

    // add required targets for ability
    switch (Handlers[m_effectEntry->EffectProperty].Target)
    {
        case PET_BATTLE_ABILITY_TARGET_CASTER:
        {
            AddTarget(m_caster);
            break;
        }
        case PET_BATTLE_ABILITY_TARGET_TARGET:
        {
            AddTarget(GetActiveOpponent());
            break;
        }
        // add battle pets from both teams
        case PET_BATTLE_ABILITY_TARGET_ALL:
        {
            for (auto&& teamIndex : { PET_BATTLE_TEAM_CHALLANGER, PET_BATTLE_TEAM_OPPONENT })
                for (auto battlePet : m_petBattle->GetTeam(teamIndex)->BattlePets)
                    AddTarget(battlePet);
            break;
        }
        // add unconditionally battle pets from casters team
        case PET_BATTLE_ABILITY_TARGET_CASTER_TEAM_UNCONDITIONAL:
        {
            for (auto battlePet : casterTeam->BattlePets)
                AddTarget(battlePet);
            break;
        }
        // add active battle pet from both teams
        case PET_BATTLE_ABILITY_TARGET_HEAD:
        {
            AddTarget(m_caster);
            AddTarget(GetActiveOpponent());
            break;
        }
        default:
            break;
    }
}

void BattlePetAbilityEffect::AddTarget(BattlePet* battlePet)
{
    if (!battlePet)
        return;

    /*if (!battlePet->IsAlive())
        return;*/

    m_targets.push_back(battlePet);
}

// -------------------------------------------------------------------------------

void BattlePetAbilityEffect::CalculateHit(int32 accuracy)
{
    accuracy += m_caster->States[BATTLE_PET_STATE_STAT_ACCURACY];
    accuracy -= m_target->States[BATTLE_PET_STATE_STAT_DODGE];

    // accuracy is reduced by 2% for every level higher the opponent is
    if (m_target->GetLevel() > m_caster->GetLevel())
        accuracy -= (m_target->GetLevel() - m_caster->GetLevel()) * 2;

    // accuracy is less than 100%, calculate miss chance
    if (accuracy < 100 && !roll_chance_i(accuracy))
        m_flags |= PET_BATTLE_EFFECT_FLAG_MISS;
}

uint32 BattlePetAbilityEffect::CalculateHeal(uint32 heal)
{
    int32 modPercent = CalculatePct(m_caster->States[BATTLE_PET_STATE_STAT_POWER], 5);

    modPercent += m_caster->States[BATTLE_PET_STATE_MOD_HEALING_DEALT_PCT];
    modPercent += m_target->States[BATTLE_PET_STATE_MOD_HEALING_TAKEN_PCT];

    return heal + CalculatePct(heal, modPercent);
}

uint32 BattlePetAbilityEffect::CalculateDamage(uint32 damage)
{
    int32 modPct = CalculatePct(m_caster->States[BATTLE_PET_STATE_STAT_POWER], 5);

    // calculate family damage modifier
    int32 modPetTypePct = -100;
    if (auto damageModEntry = sGtBattlePetTypeDamageModStore.LookupEntry(m_target->GetFamilty() * BATTLE_PET_FAMILY_COUNT + m_abilityFamily))
        modPetTypePct += damageModEntry->Modifier * 100;

    m_flags |= PET_BATTLE_EFFECT_FLAG_HIT;
    if (modPetTypePct > 0)
        m_flags |= PET_BATTLE_EFFECT_FLAG_STRONG;
    if (modPetTypePct < 0)
        m_flags |= PET_BATTLE_EFFECT_FLAG_WEAK;

    // calculate critical chance
    if (roll_chance_i(m_target->States[BATTLE_PET_STATE_STAT_CRITICAL_CHANCE]))
    {
        modPct += 100;
        m_flags |= PET_BATTLE_EFFECT_FLAG_CRITICAL;
    }

    // Beast (237)
    // Beasts deal 25% extra damage below half health
    if (m_caster->States[BATTLE_PET_STATE_PASSIVE_BEAST])
        if ((m_caster->GetCurrentHealth() * 100 / m_caster->GetMaxHealth()) < 50)
            modPetTypePct += 25;

    damage += CalculatePct(damage, modPct);
    damage += CalculatePct(damage, m_caster->States[BATTLE_PET_STATE_MOD_DAMAGE_DEALT_PCT]);
    damage += CalculatePct(damage, m_target->States[BATTLE_PET_STATE_MOD_DAMAGE_TAKEN_PCT]);
    damage += CalculatePct(damage, modPetTypePct);

    // Magic (243)
    // Magic pets cannot be dealt more than 35% of their maximum health in one attack
    if (m_target->States[BATTLE_PET_STATE_PASSIVE_MAGIC])
        if (damage * 100 / m_target->GetMaxHealth() >= 35)
            damage = CalculatePct(m_target->GetMaxHealth(), 35);

    return damage;
}

// -------------------------------------------------------------------------------

void BattlePetAbilityEffect::Damage(BattlePet* target, uint32 damage)
{
    if (m_flags & PET_BATTLE_EFFECT_FAIL_MAKS)
        damage = 0;

    // update target health and notify client
    uint32 health = damage > target->GetCurrentHealth() ? 0 : target->GetCurrentHealth() - damage;
    SetHealth(target, health);

    if (damage)
    {
        // update target states
        m_target->States[BATTLE_PET_STATE_CONDITION_WAS_DAMAGED_THIS_ROUND] = 1;
        m_target->States[BATTLE_PET_STATE_LAST_HIT_TAKEN] = damage;

        // update caster states
        m_caster->States[BATTLE_PET_STATE_CONDITION_DID_DAMAGE_THIS_ROUND] = 1;
        m_caster->States[BATTLE_PET_STATE_LAST_HIT_DEALT] = damage;
    }

    // TODO: PETBATTLE_ABILITY_TURN0_PROC_ON_DAMAGE_DEALT and PETBATTLE_ABILITY_TURN0_PROC_ON_DAMAGE_TAKEN
}

void BattlePetAbilityEffect::Heal(BattlePet* target, uint32 heal)
{
    if (m_flags & PET_BATTLE_EFFECT_FAIL_MAKS)
        heal = 0;

    // make sure heal doesn't exceed targets max health
    uint32 health = target->GetCurrentHealth();
    uint32 maxHealth = target->GetMaxHealth();

    if (target->GetCurrentHealth() + heal > target->GetMaxHealth())
        heal = target->GetMaxHealth() - target->GetCurrentHealth();

    // update target health and notify client
    health += heal;
    SetHealth(target, health);
}

// -------------------------------------------------------------------------------

void BattlePetAbilityEffect::SetHealth(BattlePet* target, uint32 value)
{
    if (value > target->GetMaxHealth())
        value = target->GetMaxHealth();

    if (!(m_flags & PET_BATTLE_EFFECT_FAIL_MAKS) && value == target->GetCurrentHealth())
        m_flags |= PET_BATTLE_EFFECT_FLAG_MISS;

    if (!(m_flags & PET_BATTLE_EFFECT_FAIL_MAKS))
    {
        target->SetCurrentHealth(value);

        // target battle pet has died
        if (!value && target->IsAlive() && !target->States[BATTLE_PET_STATE_UNKILLABLE])
        {
            m_flags |= PET_BATTLE_EFFECT_FLAG_HIT;
            m_petBattle->Kill(m_caster, target, m_effectEntry->Id, m_flags);
        }
    }

    // notify client of health change
    PetBattleEffect effect{ PET_BATTLE_EFFECT_SET_HEALTH, m_caster->GetGlobalIndex(), m_flags, m_effectEntry->Id };
    effect.SetTurn(m_petBattle->GetTeam(m_caster->GetTeamIndex())->GetTurn(), 1);
    effect.UpdateHealth(target->GetGlobalIndex(), value);

    m_petBattle->AddEffect(effect);
}

// -------------------------------------------------------------------------------

// Effect 23: Points, Accuracy
void BattlePetAbilityEffect::HandleHeal()
{
    CalculateHit(m_effectEntry->Properties[1]);
    Heal(m_target, CalculateHeal(m_effectEntry->Properties[0]));
}

// Effect 24: Points, Accuracy, IsPeriodic, OverideIndex
void BattlePetAbilityEffect::HandleDamage()
{
    CalculateHit(m_effectEntry->Properties[1]);

    if (m_effectEntry->Properties[2])
        m_flags |= PET_BATTLE_EFFECT_FLAG_PERIODIC;

    Damage(m_target, CalculateDamage(m_effectEntry->Properties[0]));
}

// Effect 25: BaseChangeToSucceed, IncreasePerToss
void BattlePetAbilityEffect::HandleCatch()
{
    // calculate catch chance
    uint32 chance = m_effectEntry->Properties[0] + (m_target->States[BATTLE_PET_STATE_INTERNAL_CAPTURE_BOOST] * m_effectEntry->Properties[1]);
    bool caught = roll_chance_i(chance);

    PetBattleEffectFlags flags = caught ? PET_BATTLE_EFFECT_FLAG_MISS : PET_BATTLE_EFFECT_FLAG_NONE;

    // notify client of catch
    PetBattleEffect effect{ PET_BATTLE_EFFECT_CATCH, m_caster->GetGlobalIndex(), flags, m_effectEntry->Id };
    effect.SetTurn(m_petBattle->GetTeam(m_caster->GetTeamIndex())->GetTurn(), 1);
    effect.UpdateStat(m_target->GetGlobalIndex(), caught ? 1 : 0);

    m_petBattle->AddEffect(effect);

    if (caught)
        m_petBattle->Catch(m_caster, m_target, m_effectEntry->Id);
    else
        m_target->States[BATTLE_PET_STATE_INTERNAL_CAPTURE_BOOST]++;
}

// Effect 26: Unused, Accuracy, Duration, Unused
void BattlePetAbilityEffect::HandlePositiveAura()
{
    CalculateHit(m_effectEntry->Properties[1]);
    m_petBattle->AddAura(m_caster, m_target, m_effectEntry->TriggerAbility, m_effectEntry->Id, m_effectEntry->Properties[2], m_flags, 1);
}

// Effect 27: Points, Accuracy, PointsIncreasePerUse, PointsMax, StateToTriggerMaxPoints
void BattlePetAbilityEffect::HandleRampingDamage()
{
    CalculateHit(m_effectEntry->Properties[1]);

    // initial setup for ramping ability
    if (m_caster->States[BATTLE_PET_STATE_RAMPING_DAMAGE_ID] != m_abilityId)
    {
        m_petBattle->UpdatePetState(m_caster, m_caster, m_effectEntry->Id, BATTLE_PET_STATE_RAMPING_DAMAGE_ID, m_abilityId);
        m_petBattle->UpdatePetState(m_caster, m_caster, m_effectEntry->Id, BATTLE_PET_STATE_RAMPING_DAMAGE_USES, 0);
    }

    if (m_caster->States[BATTLE_PET_STATE_RAMPING_DAMAGE_USES] < m_effectEntry->Properties[3])
    {
        int32 newValue = m_caster->States[BATTLE_PET_STATE_RAMPING_DAMAGE_USES] + 1;
        m_petBattle->UpdatePetState(m_caster, m_caster, m_effectEntry->Id, BATTLE_PET_STATE_RAMPING_DAMAGE_USES, newValue);
    }

    uint32 damage = CalculateDamage(m_effectEntry->Properties[0] + m_effectEntry->Properties[2] * m_caster->States[BATTLE_PET_STATE_RAMPING_DAMAGE_USES]);
    Damage(m_target, damage);
}

void BattlePetAbilityEffect::HandleSetState()
{
    m_petBattle->UpdatePetState(m_caster, m_target, m_effectEntry->Id, m_effectEntry->Properties[0], m_effectEntry->Properties[1]);
};

void BattlePetAbilityEffect::HandleHealPctDealt()
{
    if (m_effectEntry->Properties[2] && m_chainFailure)
        return;

    CalculateHit(m_effectEntry->Properties[1]);
    Heal(m_target, CalculatePct(m_target->States[BATTLE_PET_STATE_LAST_HIT_DEALT], m_effectEntry->Properties[0]));
};

// Effect 50: ChainFailure, Accuracy, Duration, MaxAllowed, CasterState, TargetState
void BattlePetAbilityEffect::HandleNegativeAura()
{
    // TODO: handle ChainFailure, CasterState, TargetState

    CalculateHit(m_effectEntry->Properties[1]);
    m_petBattle->AddAura(m_caster, m_target, m_effectEntry->TriggerAbility, m_effectEntry->Id, m_effectEntry->Properties[2], m_flags, m_effectEntry->Properties[3]);
}

// Effect 54: ChainFailure, Accuracy, Duration, MaxAllowed
void BattlePetAbilityEffect::HandlePeriodicTrigger()
{
    if (m_effectEntry->Properties[0] && m_chainFailure)
        return;

    CalculateHit(m_effectEntry->Properties[1]);
    m_petBattle->AddAura(m_caster, m_target, m_effectEntry->TriggerAbility, m_effectEntry->Id, m_effectEntry->Properties[2], m_flags, m_effectEntry->Properties[3]);
}

// Effect 63: Unused, Accuracy, Duration
void BattlePetAbilityEffect::HandlePeriodicPositiveTrigger()
{
    CalculateHit(m_effectEntry->Properties[1]);
    m_petBattle->AddAura(m_caster, m_target, m_effectEntry->TriggerAbility, m_effectEntry->Id, m_effectEntry->Properties[2], m_flags);
}

// Effect 76: Points, Accuracy
void BattlePetAbilityEffect::HandleDamageToggleAura()
{
    CalculateHit(m_effectEntry->Properties[1]);
    if (m_flags & PET_BATTLE_EFFECT_FAIL_MAKS)
        return;

    // aura has already been applied, expire and handle damage
    if (auto aura = m_caster->GetAura(m_effectEntry->TriggerAbility))
    {
        aura->Expire();
        Damage(m_target, CalculateDamage(m_effectEntry->Properties[0]));
    }
    // aura has yet to be applied
    else
        m_petBattle->AddAura(m_caster, m_caster, m_effectEntry->TriggerAbility, m_effectEntry->Id, m_effectEntry->Properties[2], m_flags);
}

void BattlePetAbilityEffect::HandleDamageHitState()
{
    if ((!m_effectEntry->Properties[2] || !m_caster->States[m_effectEntry->Properties[2]])
        && (!m_effectEntry->Properties[3] || !m_target->States[m_effectEntry->Properties[3]]))
    {
        CalculateHit(m_effectEntry->Properties[1]);
    }

    if (m_effectEntry->Properties[1])
        m_flags |= PET_BATTLE_EFFECT_FLAG_PERIODIC;

    Damage(m_target, CalculateDamage(m_effectEntry->Properties[0]));
};

// Effect 103: Points, Accuracy
void BattlePetAbilityEffect::HandleExtraAttackFirst()
{
    if (m_petBattle->GetFirstAttackingTeam() != m_caster->GetTeamIndex())
        return;

    CalculateHit(m_effectEntry->Properties[1]);
    Damage(m_target, CalculateDamage(m_effectEntry->Properties[0]));
}

// Effect 104: Points, Accuracy, RequiredCasterState, RequiredTargetState
void BattlePetAbilityEffect::HandleHealState()
{
    // Wtf is this 2 and 3 numbers?
    auto i = m_effectEntry->Properties[2];
    ASSERT(i < BATTLE_PET_MAX_STATES);
    if (!m_caster->States[i] && m_effectEntry->Properties[2])
        return;

    i = m_effectEntry->Properties[3];
    ASSERT(i < BATTLE_PET_MAX_STATES);
    if (!m_target->States[i] && m_effectEntry->Properties[3])
        return;

    CalculateHit(m_effectEntry->Properties[1]);
    Heal(m_caster, CalculateHeal(m_effectEntry->Properties[0]));
}

// Effect 149: Points, Accuracy, IsPeriodic
void BattlePetAbilityEffect::HandleDamageNonLethal()
{
    CalculateHit(m_effectEntry->Properties[1]);

    if (m_effectEntry->Properties[2])
        m_flags |= PET_BATTLE_EFFECT_FLAG_PERIODIC;

    // make sure damage doesn't exceed targets health
    uint32 damage = CalculateDamage(m_effectEntry->Properties[0]);
    if (damage >= m_target->GetCurrentHealth())
        damage = m_target->GetCurrentHealth() - 1;

    Damage(m_target, damage);
}

// Effect 178: StatePoints, Accuracy, Duration, TargetState, ChainFailure, ReportFailAsImmune
void BattlePetAbilityEffect::HandlePowerlessAura()
{
    if (m_effectEntry->Properties[5])
        m_reportFailAsImmune = true;

    // make sure target doesn't already have state value
    if (m_effectEntry->Properties[3] && m_target->States[m_effectEntry->Properties[3]] == m_effectEntry->Properties[0])
    {
        if (m_reportFailAsImmune)
            m_flags |= PET_BATTLE_EFFECT_FLAG_IMMUNE;
        else
            return;
    }

    CalculateHit(m_effectEntry->Properties[1]);
    m_petBattle->AddAura(m_caster, m_target, m_effectEntry->TriggerAbility, m_effectEntry->Id, m_effectEntry->Properties[2], m_flags);
}

// Effect 80: TurnOffset, Accuracy, Duration, MaxAllowed
void BattlePetAbilityEffect::HandleWeatherAura()
{
    CalculateHit(m_effectEntry->Properties[1]);
    m_petBattle->AddAura(m_caster, m_target, m_effectEntry->TriggerAbility, m_effectEntry->Id, m_effectEntry->Properties[2], m_flags, m_effectEntry->Properties[3]);
}
