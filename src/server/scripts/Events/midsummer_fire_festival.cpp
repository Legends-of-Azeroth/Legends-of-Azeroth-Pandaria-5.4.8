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

// 45819 - Throw Torch
class spell_throw_torch : public SpellScript
{
    PrepareSpellScript(spell_throw_torch);

    enum
    {
        SPELL_JUGGLE_TORCH              = 45638,
        SPELL_TORCH_TOSS_SHADOW_SELF    = 46121,
        SPELL_JUGGLE_TORCH_OTHER_SLOW   = 45792,
        SPELL_JUGGLE_TORCH_OTHER_MEDIUM = 45806,
        SPELL_JUGGLE_TORCH_OTHER_FAST   = 45816,
        SPELL_TORCH_TOSS_SHADOW_SLOW    = 46120,
        SPELL_TORCH_TOSS_SHADOW_MEDIUM  = 46118,
        SPELL_TORCH_TOSS_SHADOW_FAST    = 46117,
    };

    void HandleHit(SpellEffIndex index)
    {
        uint32 juggle = 0, shadow = 0;
        if (GetCaster()->IsWithinDist3d(GetHitDest(), GetSpellInfo()->Effects[index].CalcRadius(GetCaster())))
        {
            juggle = SPELL_JUGGLE_TORCH;
            shadow = SPELL_TORCH_TOSS_SHADOW_SELF;
        }
        else
        {
            switch (urand(0, 2))
            {
                case 0: juggle = SPELL_JUGGLE_TORCH_OTHER_SLOW;   shadow = SPELL_TORCH_TOSS_SHADOW_SLOW;   break;
                case 1: juggle = SPELL_JUGGLE_TORCH_OTHER_MEDIUM; shadow = SPELL_TORCH_TOSS_SHADOW_MEDIUM; break;
                case 2: juggle = SPELL_JUGGLE_TORCH_OTHER_FAST;   shadow = SPELL_TORCH_TOSS_SHADOW_FAST;   break;
            }
        }

        GetCaster()->CastSpell(*GetHitDest(), juggle, false);
        GetCaster()->CastSpell(*GetHitDest(), shadow, false);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_throw_torch::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 45644 - Juggle Torch (Catch)
class spell_jungle_torch_catch : public SpellScript
{
    PrepareSpellScript(spell_jungle_torch_catch);

    enum { SPELL_GIVE_TORCH = 45280 };

    void HandleHit()
    {
        if (Player* player = GetHitPlayer())
        {
            if (GetCaster() == player)
                player->m_Events.Schedule(1500, [=] { player->CastSpell(player, SPELL_GIVE_TORCH, true); });
            else
                player->CastSpell(player, SPELL_GIVE_TORCH, true);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_jungle_torch_catch::HandleHit);
    }
};

enum 
{
    SPELL_RIBBON_POLE_PERIODIC_VISUAL = 45406
};

// 29531 - Ribbon Pole Channel
class spell_ribbon_pole_channel : public SpellScript
{
    PrepareSpellScript(spell_ribbon_pole_channel);

    void HandleCast()
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_RIBBON_POLE_PERIODIC_VISUAL, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_ribbon_pole_channel::HandleCast);
    }
};

// 29531 - Ribbon Pole Channel
class spell_ribbon_pole_channel_remove : public AuraScript
{
    PrepareAuraScript(spell_ribbon_pole_channel_remove);

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Unit* caster = GetCaster())
            caster->RemoveAurasDueToSpell(SPELL_RIBBON_POLE_PERIODIC_VISUAL);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_ribbon_pole_channel_remove::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 45406 - Holiday - Midsummer, Ribbon Pole Periodic Visual
class spell_ribbon_pole_periodic_visual : public AuraScript
{
    PrepareAuraScript(spell_ribbon_pole_periodic_visual);

    enum
    {
        SPELL_RIBBON_DANCE           = 29175,
        SPELL_BURNING_HOT_POLE_DANCE = 58934,
    };

    void HandleTick(AuraEffect const* effect)
    {
        Unit* owner = GetUnitOwner();
        if (Aura* aura = owner->GetAura(SPELL_RIBBON_DANCE))
        {
            aura->SetMaxDuration(aura->GetMaxDuration() >= 3600000 ? 3600000 : aura->GetMaxDuration() + 180000);
            aura->RefreshDuration();
        }
        else
            owner->CastSpell(owner, SPELL_RIBBON_DANCE, true);

        if (effect->GetTickNumber() >= 20)
            owner->CastSpell(owner, SPELL_BURNING_HOT_POLE_DANCE, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_ribbon_pole_periodic_visual::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

enum TorchCatchingData
{
    SPELL_FLING_TORCH_MISSILE       = 45669,
    SPELL_TOSS_TORCH_SHADOW         = 46105,
    SPELL_TORCH_TARGET_PICKER       = 45907,
    SPELL_TORCHES_COUGHT            = 45693,
    SPELL_JUGGLE_TORCH_MISSED       = 45676,
    SPELL_TORCH_CATCHING_SUCCESS    = 46081,
    SPELL_TORCH_DAILY_SUCCESS       = 46654,
    NPC_JUGGLE_TARGET               = 25515,
    QUEST_TORCH_CATCHING_A          = 11657,
    QUEST_TORCH_CATCHING_H          = 11923,
    QUEST_MORE_TORCH_CATCHING_A     = 11924,
    QUEST_MORE_TORCH_CATCHING_H     = 11925
};

// 45907 - Torch Target Picker
class spell_torch_target_picker : public SpellScript
{
    PrepareSpellScript(spell_torch_target_picker);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Unit* caster = GetCaster();

        std::list<Creature*> juggleList;
        caster->GetCreatureListWithEntryInGrid(juggleList, NPC_JUGGLE_TARGET, 10.0f);

        for (auto&& itr : juggleList)
            targets.remove(itr);

        if (targets.empty())
            return;

        auto itr = targets.begin();
        std::advance(itr, urand(0, targets.size() - 1));

        WorldObject* target = *itr;
        targets.clear();
        targets.push_back(target);
    }

    void HandleDummy(SpellEffIndex)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_FLING_TORCH_MISSILE, true);
        GetCaster()->CastSpell(GetHitUnit(), SPELL_TOSS_TORCH_SHADOW, true);
    }

    void Register()
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_torch_target_picker::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_torch_target_picker::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 45671 - Juggle Torch (Catch, Quest)
class spell_juggle_torch_catch : public SpellScript
{
    PrepareSpellScript(spell_juggle_torch_catch);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Unit* caster = GetCaster();
        WorldObject* juggleTarget = nullptr;
        bool missed = true;

        if (targets.empty() || !caster->ToPlayer())
            return;

        for (auto&& itr : targets)
        {
            if (itr == caster)
                missed = false;

            if (itr->ToCreature())
                juggleTarget = itr;
        }

        if (missed)
        {
            if (juggleTarget)
                if (Unit* castTarget = juggleTarget->ToUnit())
                    castTarget->CastSpell(castTarget, SPELL_JUGGLE_TORCH_MISSED, true);
            caster->RemoveAurasDueToSpell(SPELL_TORCHES_COUGHT);
        }
        else
        {
            uint8 neededCatches;

            if (caster->ToPlayer()->GetQuestStatus(QUEST_TORCH_CATCHING_A) == QUEST_STATUS_INCOMPLETE
                || caster->ToPlayer()->GetQuestStatus(QUEST_TORCH_CATCHING_H) == QUEST_STATUS_INCOMPLETE)
            {
                neededCatches = 4;
            }
            else if (caster->ToPlayer()->GetQuestStatus(QUEST_MORE_TORCH_CATCHING_A) == QUEST_STATUS_INCOMPLETE
                || caster->ToPlayer()->GetQuestStatus(QUEST_MORE_TORCH_CATCHING_H) == QUEST_STATUS_INCOMPLETE)
            {
                neededCatches = 10;
            }
            else
            {
                caster->RemoveAurasDueToSpell(SPELL_TORCHES_COUGHT);
                return;
            }

            caster->CastSpell(caster, SPELL_TORCH_TARGET_PICKER, true);
            caster->CastSpell(caster, SPELL_TORCHES_COUGHT, true);

            // reward quest
            if (caster->GetAuraCount(SPELL_TORCHES_COUGHT) >= neededCatches)
            {
                caster->CastSpell(caster, SPELL_TORCH_CATCHING_SUCCESS, true);
                caster->CastSpell(caster, SPELL_TORCH_DAILY_SUCCESS, true);
                caster->RemoveAurasDueToSpell(SPELL_TORCHES_COUGHT);
            }
        }
    }

    void Register()
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_juggle_torch_catch::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
    }
};

enum MidsummerFestivalTorchTossing
{
    NPC_TORCH_TOSSING_TARGET_BUNNY              = 25535,
    NPC_TORCH_TOSSING_TARGET_BUNNY_CONTROLLER   = 25535,
    GO_TORCH_TARGET_BRAZIER                     = 187708,
    SPELL_TOSS_TORCH_LAND                       = 46054,
    SPELL_BRAZIER_HIT                           = 45724,
    SPELL_TORCH_TOSSING_SUCCESS_A               = 45719,
    SPELL_TORCH_TOSSING_SUCCESS_H               = 46651,
    SPELL_TORCH_TOSSING_TRAINING                = 45716,
    SPELL_TORCH_TOSSING_PRACTICE                = 46630,
    SPELL_TARGET_INDICATOR                      = 45723,
    QUEST_TORCH_TOSSING_A                       = 11731,
    QUEST_TORCH_TOSSING_H                       = 11922,
    QUEST_MORE_TORCH_TOSSING_A                  = 11921,
    QUEST_MORE_TORCH_TOSSING_H                  = 11926,
    EVENT_RECHECK_TARGETS                       = 1,
    EVENT_SELECT_TARGET                         = 2,
};

// 25535
struct npc_torch_tossing_target_bunny: public ScriptedAI
{
    npc_torch_tossing_target_bunny(Creature* creature) : ScriptedAI(creature) { }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        if (spell->Id != SPELL_TOSS_TORCH_LAND || !me->HasAura(SPELL_TARGET_INDICATOR) || !caster->CanSeeOrDetect(me))
            return;

        Player* player = caster->ToPlayer();
        if (!player)
            return;

        bool training = player->GetQuestStatus(QUEST_TORCH_TOSSING_A) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_TORCH_TOSSING_H) == QUEST_STATUS_INCOMPLETE;
        bool practice = player->GetQuestStatus(QUEST_MORE_TORCH_TOSSING_A) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_MORE_TORCH_TOSSING_H) == QUEST_STATUS_INCOMPLETE;

        if (training || practice)
        {
            me->CastSpell(player, SPELL_BRAZIER_HIT, true, nullptr, nullptr, me->ToTempSummon()->GetSummonerGUID());
            uint32 count = player->GetAuraCount(SPELL_BRAZIER_HIT);
            if (training && count == 8 || practice && count == 20)
            {
                DoCast(player, player->GetTeam() == ALLIANCE ? SPELL_TORCH_TOSSING_SUCCESS_A : SPELL_TORCH_TOSSING_SUCCESS_H, true);
                player->RemoveAurasDueToSpell(SPELL_BRAZIER_HIT);
                player->RemoveAurasDueToSpell(SPELL_TORCH_TOSSING_TRAINING);
                player->RemoveAurasDueToSpell(SPELL_TORCH_TOSSING_PRACTICE);
            }
        }
    }

    void UpdateAI(uint32 diff) override { }
};

// 25536
struct npc_torch_tossing_target_bunny_controller : public ScriptedAI
{
    npc_torch_tossing_target_bunny_controller(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        events.Reset();
        events.ScheduleEvent(EVENT_RECHECK_TARGETS, 1000);
        lastTargetGuid = ObjectGuid::Empty;
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);
        switch (events.ExecuteEvent())
        {
            case EVENT_RECHECK_TARGETS:
            {
                std::set<GameObject*> selectedBraziers;
                std::list<GameObject*> braziers;
                GetGameObjectListWithEntryInGrid(braziers, me, GO_TORCH_TARGET_BRAZIER, 100);

                // Gather GO guids
                for (auto&& go : braziers)
                {
                    // Check for duplicates
                    bool found = false;
                    for (auto&& selected : selectedBraziers)
                    {
                        if (go->GetDistance2d(selected) < 2.5f)
                        {
                            found = true;
                            break;
                        }
                    }

                    if (!found)
                        selectedBraziers.insert(go);
                }

                // If not enough GOs were found - they were probably not loaded yet, repeat the search later
                if (selectedBraziers.size() < 5)
                {
                    events.ScheduleEvent(EVENT_RECHECK_TARGETS, 5000);
                    break;
                }

                // Spawn NPCs at GOs positions
                for (auto&& selected : selectedBraziers)
                    if (Creature* target = me->SummonCreature(NPC_TORCH_TOSSING_TARGET_BUNNY, *selected))
                        targetGuids.insert(target->GetGUID());

                events.ScheduleEvent(EVENT_SELECT_TARGET, 1000);
                break;
            }
            case EVENT_SELECT_TARGET:
            {
                if (!targetGuids.empty())
                {
                    std::list<ObjectGuid> pick(targetGuids.begin(), targetGuids.end());
                    if (lastTargetGuid)
                        pick.remove(lastTargetGuid);
                    Trinity::Containers::RandomResizeList(pick, 1);
                    lastTargetGuid = pick.front();
                    if (Creature* target = ObjectAccessor::GetCreature(*me, lastTargetGuid))
                        target->CastSpell(target, SPELL_TARGET_INDICATOR);
                    events.ScheduleEvent(EVENT_SELECT_TARGET, 3000);
                }
                break;
            }
        }
    }

private:
    EventMap events;
    std::set<ObjectGuid> targetGuids;
    ObjectGuid lastTargetGuid;
};

void AddSC_event_midsummer_fire_festival()
{
    new spell_script<spell_throw_torch>("spell_throw_torch");
    new spell_script<spell_jungle_torch_catch>("spell_jungle_torch_catch");
    new spell_script<spell_ribbon_pole_channel>("spell_ribbon_pole_channel");
    new aura_script<spell_ribbon_pole_channel_remove>("spell_ribbon_pole_channel_remove");
    new aura_script<spell_ribbon_pole_periodic_visual>("spell_ribbon_pole_periodic_visual");
    new spell_script<spell_torch_target_picker>("spell_torch_target_picker");
    new spell_script<spell_juggle_torch_catch>("spell_juggle_torch_catch");
    new creature_script<npc_torch_tossing_target_bunny>("npc_torch_tossing_target_bunny");
    new creature_script<npc_torch_tossing_target_bunny_controller>("npc_torch_tossing_target_bunny_controller");
}
