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

#include "ScriptedCreature.h"
#include "Vehicle.h"

enum
{
    SPELL_DEFEND_AURA                   = 62719,
    SPELL_DEFEND_AURA_1                 = 64100,
    SPELL_SHIELD_BREAKER                = 62575,
    SPELL_SHIELD_BREAKER_2              = 66480,
    SPELL_SHIELD_BREAKER_3              = 65147,
    SPELL_CHARGE                        = 62960,
    SPELL_CHARGE_2                      = 63010,
    SPELL_THRUST                        = 62544,
    SPELL_THRUST_2                      = 68505,
    SPELL_DEFEND                        = 62552,
    SPELL_VISUAL_SHIELD_1               = 63130,
    SPELL_VISUAL_SHIELD_2               = 63131,
    SPELL_VISUAL_SHIELD_3               = 63132,
    SPELL_DARK_SHIELD                   = 64505,
    SPELL_RIDE_VEHICLE                  = 63151,
    SPELL_PLAYER_ON_TOURNAMENT_MOUNT    = 63034,
    SPELL_LANCE_EQUIPPED                = 62853,
    SPELL_YELLOW_ROSE_FIREWORK          = 11544,
    SPELL_ROPE_BEAM                     = 63413,
    SPELL_DARNASSUS_PENNANT             = 63443,
    SPELL_EXODAR_PENNANT                = 63439,
    SPELL_GNOMEREGAN_PENNANT            = 63442,
    SPELL_IRONFORGE_PENNANT             = 63440,
    SPELL_ORGRIMMAR_PENNANT             = 63444,
    SPELL_SENJIN_PENNANT                = 63446,
    SPELL_SILVERMOON_PENNANT            = 63438,
    SPELL_STORMWIND_PENNANT             = 62727,
    SPELL_THUNDERBLUFF_PENNANT          = 63445,
    SPELL_UNDERCITY_PENNANT             = 63441,
    SPELL_CHECK_MOUNT                   = 67039,
    SPELL_CHECK_TIRED                   = 67334,
    SPELL_SQUIRE_BANK_ERRAND            = 67368,
    SPELL_SQUIRE_STABLING               = 67374,
    SPELL_SQUIRE_POSTMAN                = 67376,
    SPELL_SQUIRE_SHOP                   = 67377,
    SPELL_SQUIRE_TIRED                  = 67401,

    SPELL_CHARGE_CREDIT                 = 62658,
    SPELL_MELEE_CREDIT                  = 62672,
    SPELL_RANGED_CREDIT                 = 62673,

    NPC_ARGENT_CHAMPION_CREDIT          = 33708,

    SAY_FACTION_VALIANT_START           = 0,
    SAY_FACTION_VALIANT_WIN             = 1,
    SAY_FACTION_VALIANT_LOSE            = 2,

    SAY_FACTION_CHAMPION_START          = 0,
    SAY_FACTION_CHAMPION_WIN            = 1,
    SAY_FACTION_CHAMPION_LOSE           = 2,

    TIMER_SPELL_MIN                     = 1000,
    TIMER_SPELL_MAX                     = 2500,
    TIMER_MOVEMENT_MIN                  = 1000,
    TIMER_MOVEMENT_MAX                  = 2500,
    TIMER_SPELL_MIN_2                   = 1000,
    TIMER_SPELL_MAX_2                   = 2000,
    TIMER_MOVEMENT_MIN_2                = 1000,
    TIMER_MOVEMENT_MAX_2                = 2000,

    POINT_INTRO                         = 1001,

    GOSSIP_VALIANT_ITEM_MENU_ID         = 0,
    GOSSIP_VALIANT_GOSSIP_TEXT_1        = 14384,
    GOSSIP_VALIANT_GOSSIP_TEXT_2        = 14489,

    GOSSIP_CHAMP_ITEM_MENU_ID           = 0,
    GOSSIP_CHAMP_GOSSIP_TEXT_1          = 14421,
    GOSSIP_CHAMP_GOSSIP_TEXT_2          = 14489,
};

bool CanEngageInMountedCombat(Unit const* who)
{
    return who->HasAura(SPELL_RIDE_VEHICLE) && who->HasAura(SPELL_PLAYER_ON_TOURNAMENT_MOUNT) && who->HasAura(SPELL_LANCE_EQUIPPED) && !who->IsInCombat();
}

uint32 GetBestedIdFor(bool champion, Creature* me)
{
    static const struct
    {
        uint32 ValiantID;
        uint32 ChampionID;
        uint32 BestedSpellID;
    } RingBestedDebuffs[] =
    {
        { 33559, 33738, 64805 }, // Darnassus Valiant       / Darnassus Champion        / Bested Darnassus
        { 33562, 33739, 64808 }, // Exodar Valiant          / Exodar Champion           / Bested the Exodar
        { 33558, 33740, 64809 }, // Gnomeregan Valiant      / Gnomeregan Champion       / Bested Gnomeregan
        { 33564, 33743, 64810 }, // Ironforge Valiant       / Ironforge Champion        / Bested Ironforge
        { 33306, 33744, 64811 }, // Orgrimmar Valiant       / Orgrimmar Champion        / Bested Orgrimmar
        { 33285, 33745, 64812 }, // Sen'jin Valiant         / Sen'jin Champion          / Bested Sen'jin
        { 33382, 33746, 64813 }, // Silvermoon Valiant      / Silvermoon Champion       / Bested Silvermoon City
        { 33561, 33747, 64814 }, // Stormwind Valiant       / Stormwind Champion        / Bested Stormwind
        { 33383, 33748, 64815 }, // Thunder Bluff Valiant   / Thunder Bluff Champion    / Bested Thunder Bluff
        { 33384, 33749, 64816 }, // Undercity Valiant       / Undercity Champion        / Bested the Undercity
    };

    for (auto const& debuff : RingBestedDebuffs)
        if ((champion ? debuff.ChampionID : debuff.ValiantID) == me->GetEntry())
            return debuff.BestedSpellID;

    return 0;
}

// I gave up on refactoring this shit...

struct npc_ring_rider_base : ScriptedAI
{
    npc_ring_rider_base(Creature* c) : ScriptedAI(c) {}

    uint32 spellTimer, movementTimer;
    uint32 spellTimerMin, spellTimerMax;
    uint32 movementTimerMin, movementTimerMax;
    Position center;

    void Reset() override
    {
        me->SetFaction(35);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
    }

    bool CanAIAttack(Unit const* target) const override
    {
        if (target->GetTypeId() == TYPEID_UNIT)
            if (Vehicle* vehicle = target->GetVehicleKit())
                if (Unit* passenger = vehicle->GetPassenger(0))
                    return passenger->GetTypeId() == TYPEID_PLAYER;

        return false;
    }

    void AttackStart(Unit* victim) override
    {
        if (Unit* vehicle = victim->GetVehicleBase())
            victim = vehicle;
        ScriptedAI::AttackStart(victim);
    }

    void DoEvade()
    {
        me->SetFaction(35);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
        EnterEvadeMode();
    }

    Position FindDestinationInCircle(Position const* from, float maxDist, float radius)
    {
        Position dest = { from->GetPositionX() + maxDist * ((float)rand_norm() * 2.0f - 1.0f), from->GetPositionY() + maxDist * ((float)rand_norm() * 2.0f - 1.0f), from->GetPositionZ() };
        if (center.GetExactDist(&dest) > radius)
        {
            float angle = dest.GetAngle(&center);
            Position offset = { maxDist * cos(angle), maxDist * sin(angle) };
            dest.RelocateOffset(offset);
        }
        return dest;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!UpdateVictim())
            return;

        Unit* target = me->GetVictim();
        if (!target)
            return;

        if (target->GetTypeId() == TYPEID_PLAYER)
            target = target->GetVehicleBase();

        if (!target)
            return;

        if (spellTimer <= uiDiff)
        {
            if (me->IsInRange(target, 5, 40, false))
            {
                switch (urand(0, 2))
                {
                    case 0:
                        DoCast(target, SPELL_SHIELD_BREAKER_2, true);
                        break;
                    case 1:
                        DoCast(target, SPELL_CHARGE, true);
                        break;
                    case 2:
                        me->CastSpell(me, SPELL_DEFEND, true);
                        break;
                }
            }
            else
            {
                switch (urand(0, 3))
                {
                    case 0:
                        me->CastSpell(me, SPELL_DEFEND, true);
                        break;
                    case 1: case 2: case 3:
                        DoCast(target, SPELL_THRUST, true);
                        break;
                }
            }
            spellTimer = urand(spellTimerMin, spellTimerMax);
        }
        else
            spellTimer -= uiDiff;

        if (movementTimer <= uiDiff)
        {
            me->GetMotionMaster()->MovePoint(0, FindDestinationInCircle(me, 10.0f, 30.0f));
            movementTimer = urand(movementTimerMin, movementTimerMax);
        }
        else
            movementTimer -= uiDiff;
    }
};

class npc_ring_valiants : public CreatureScript
{
public:
    npc_ring_valiants() : CreatureScript("npc_ring_valiants") { }

    bool OnGossipHello(Player* pPlayer, Creature* creature) override
    {
        if (((pPlayer->GetQuestStatus(13665) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(13745) == QUEST_STATUS_INCOMPLETE ||
            pPlayer->GetQuestStatus(13761) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(13767) == QUEST_STATUS_INCOMPLETE ||
            pPlayer->GetQuestStatus(13772) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(13777) == QUEST_STATUS_INCOMPLETE ||
            pPlayer->GetQuestStatus(13782) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(13787) == QUEST_STATUS_INCOMPLETE ||
            pPlayer->GetQuestStatus(13750) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(13756) == QUEST_STATUS_INCOMPLETE)
            || (((pPlayer->GetQuestStatus(13814) == QUEST_STATUS_INCOMPLETE) || (pPlayer->GetQuestStatus(13811) == QUEST_STATUS_INCOMPLETE) || (pPlayer->GetQuestStatus(13793) == QUEST_STATUS_INCOMPLETE) || (pPlayer->GetQuestStatus(13790) == QUEST_STATUS_INCOMPLETE)) && !(pPlayer->HasAchieved(2836)))) &&
            !(pPlayer->HasAura(62665) || pPlayer->HasAura(GetBestedIdFor(false, creature))) && CanEngageInMountedCombat(pPlayer))
        {
            pPlayer->ADD_GOSSIP_ITEM_DB(pPlayer->GetDefaultGossipMenuForSource(creature), GOSSIP_VALIANT_ITEM_MENU_ID, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        }

        if (pPlayer->HasAura(GetBestedIdFor(false, creature)))
        {
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_VALIANT_GOSSIP_TEXT_2, creature->GetGUID());
            return true;
        }

        pPlayer->SEND_GOSSIP_MENU(GOSSIP_VALIANT_GOSSIP_TEXT_1, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* pPlayer, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            pPlayer->CLOSE_GOSSIP_MENU();
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
            creature->SetWalk(false);
            if (creature->GetHomePosition().GetPositionX() > 8500.0f)
                creature->GetMotionMaster()->MovePoint(POINT_INTRO, 8658.1f, 719.514282f, 547.787842f);
            else
                creature->GetMotionMaster()->MovePoint(POINT_INTRO, 8338.1f, 719.514282f, 553.787842f);
        }
        return true;
    }

    struct npc_ring_valiantsAI : public npc_ring_rider_base
    {
        npc_ring_valiantsAI(Creature* pCreature) : npc_ring_rider_base(pCreature)
        {
            spellTimerMin = TIMER_SPELL_MIN;
            spellTimerMax = TIMER_SPELL_MAX;
            movementTimerMin = TIMER_MOVEMENT_MIN;
            movementTimerMax = TIMER_MOVEMENT_MAX;
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE || id != POINT_INTRO)
                return;
            me->SetFaction(14);
            center.Relocate(*me);
        }

        void DamageTaken(Unit* attacker, uint32& damage) override
        {
            if (attacker && damage > me->GetHealth())
            {
                if (Player* player = attacker->GetAffectingPlayer())
                {
                    damage = 0;

                    player->CastSpell(player, 62724, true); // Reward mark

                    if (uint32 debuff = GetBestedIdFor(false, me))
                        player->CastSpell(player, debuff, true);

                    Talk(SAY_FACTION_VALIANT_LOSE, player);
                    DoEvade();
                }
            }
        }

        void KilledUnit(Unit* victim) override
        {
            if (Player* player = victim->GetCharmerOrOwnerPlayerOrPlayerItself())
                Talk(SAY_FACTION_VALIANT_WIN, player);

            me->CombatStop(true);
            me->CastSpell(me, SPELL_YELLOW_ROSE_FIREWORK, true);
            DoEvade();
        }

        void JustEngagedWith(Unit* who) override
        {
            if (Player* player = who->GetCharmerOrOwnerPlayerOrPlayerItself())
                Talk(SAY_FACTION_VALIANT_START, player);
            spellTimer = urand(TIMER_SPELL_MIN, TIMER_SPELL_MAX);
            movementTimer = urand(TIMER_MOVEMENT_MIN, TIMER_MOVEMENT_MAX);
            me->CastSpell(me, SPELL_DEFEND, true);
            me->CastSpell(me, SPELL_DEFEND, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ring_valiantsAI(creature);
    }
};

class npc_ring_champions : public CreatureScript
{
public:
    npc_ring_champions() : CreatureScript("npc_ring_champions") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if ((player->GetQuestStatus(13790) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(13793) == QUEST_STATUS_INCOMPLETE ||
            player->GetQuestStatus(13811) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(13814) == QUEST_STATUS_INCOMPLETE) &&
            !(player->HasAura(62665) || player->HasAura(GetBestedIdFor(true, creature))) && CanEngageInMountedCombat(player))
        {
            player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), GOSSIP_CHAMP_ITEM_MENU_ID, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        }
        if (player->HasAura(GetBestedIdFor(true, creature)))
        {
            player->SEND_GOSSIP_MENU(GOSSIP_CHAMP_GOSSIP_TEXT_2, creature->GetGUID());
            return true;
        }

        player->SEND_GOSSIP_MENU(GOSSIP_CHAMP_GOSSIP_TEXT_1, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
            creature->SetWalk(false);
            if (creature->GetHomePosition().GetPositionY() < 950.35f)
                creature->GetMotionMaster()->MovePoint(POINT_INTRO, 8432.1f, 921.514282f, 544.787842f);
            else
                creature->GetMotionMaster()->MovePoint(POINT_INTRO, 8432.1f, 975.514282f, 544.787842f);
        }
        return true;
    }

    struct npc_ring_championsAI : public npc_ring_rider_base
    {
        npc_ring_championsAI(Creature* creature) : npc_ring_rider_base(creature)
        {
            spellTimerMin = TIMER_SPELL_MIN_2;
            spellTimerMax = TIMER_SPELL_MAX_2;
            movementTimerMin = TIMER_MOVEMENT_MIN_2;
            movementTimerMax = TIMER_MOVEMENT_MAX_2;
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE || id != POINT_INTRO)
                return;
            me->SetFaction(14);
            center.Relocate(*me);
            me->CastSpell(me, SPELL_DEFEND, true);
            me->CastSpell(me, SPELL_DEFEND, true);
            me->CastSpell(me, SPELL_DEFEND, true);
        }

        void DamageTaken(Unit* attacker, uint32& damage) override
        {
            if (attacker && damage > me->GetHealth())
            {
                if (Player* player = attacker->GetAffectingPlayer())
                {
                    damage = 0;

                    player->CastSpell(player, 63596, true); // Reward mark

                    if (uint32 debuff = GetBestedIdFor(true, me))
                        player->CastSpell(player, debuff, true);

                    Talk(SAY_FACTION_CHAMPION_LOSE, player);
                    DoEvade();
                }
            }
        }

        void KilledUnit(Unit* victim) override
        {
            if (Player* player = victim->GetCharmerOrOwnerPlayerOrPlayerItself())
                Talk(SAY_FACTION_CHAMPION_WIN, player);

            me->CastSpell(me, SPELL_YELLOW_ROSE_FIREWORK, true);
            me->CombatStop(true);
            DoEvade();
        }

        void JustEngagedWith(Unit* who) override
        {
            if (Player* player = who->GetCharmerOrOwnerPlayerOrPlayerItself())
                Talk(SAY_FACTION_CHAMPION_START, player);
            spellTimer = urand(TIMER_SPELL_MIN_2, TIMER_SPELL_MAX_2);
            movementTimer = urand(TIMER_MOVEMENT_MIN_2, TIMER_MOVEMENT_MAX_2);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ring_championsAI(creature);
    }
};

// 62552, 62719, 66482 - Defend
class spell_argent_tournament_defend : public AuraScript
{
    PrepareAuraScript(spell_argent_tournament_defend);

    void RefreshVisualShields(AuraEffect const* effect, AuraEffectHandleModes)
    {
        if (GetCaster())
        {
            Unit* target = GetTarget();

            for (uint8 i = 0; i < GetSpellInfo()->StackAmount; ++i)
                target->RemoveAurasDueToSpell(SPELL_VISUAL_SHIELD_1 + i);

            target->CastSpell(target, SPELL_VISUAL_SHIELD_1 + GetAura()->GetStackAmount() - 1, true, nullptr, effect);
        }
        else
            GetTarget()->RemoveAurasDueToSpell(GetId());
    }

    void RemoveVisualShields(AuraEffect const*, AuraEffectHandleModes)
    {
        for (uint8 i = 0; i < GetSpellInfo()->StackAmount; ++i)
            GetTarget()->RemoveAurasDueToSpell(SPELL_VISUAL_SHIELD_1 + i);
    }

    void RemoveDummyFromDriver(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* caster = GetCaster())
            if (TempSummon* vehicle = caster->ToTempSummon())
                if (Unit* rider = vehicle->GetSummoner())
                    rider->RemoveAurasDueToSpell(GetId());
    }

    void Register() override
    {
        SpellInfo const* spell = sSpellMgr->GetSpellInfo(m_scriptSpellId);

        // Defend spells casted by NPCs (add visuals)
        if (spell->Effects[EFFECT_0].ApplyAuraName == SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN)
        {
            AfterEffectApply += AuraEffectApplyFn(spell_argent_tournament_defend::RefreshVisualShields, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            OnEffectRemove += AuraEffectRemoveFn(spell_argent_tournament_defend::RemoveVisualShields, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
        }

        // Remove Defend spell from player when he dismounts
        if (spell->Effects[EFFECT_2].ApplyAuraName == SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN)
            OnEffectRemove += AuraEffectRemoveFn(spell_argent_tournament_defend::RemoveDummyFromDriver, EFFECT_2, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);

        // Defend spells casted by players (add/remove visuals)
        if (spell->Effects[EFFECT_1].ApplyAuraName == SPELL_AURA_DUMMY)
        {
            AfterEffectApply += AuraEffectApplyFn(spell_argent_tournament_defend::RefreshVisualShields, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            OnEffectRemove += AuraEffectRemoveFn(spell_argent_tournament_defend::RemoveVisualShields, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
        }
    }
};

void AddSC_argent_tournament()
{
    new npc_ring_valiants();
    new npc_ring_champions();
    new aura_script<spell_argent_tournament_defend>("spell_argent_tournament_defend");
}
