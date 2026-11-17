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
#include "CreatureTextMgr.h"

enum
{
    // "Now This is Ram Racing... Almost" and "Bark for ..." quests
    SPELL_RENTAL_RACING_RAM                 = 43883,
    SPELL_RAM_LEVEL_NEUTRAL                 = 43310,
    SPELL_RAM_TROT                          = 42992,
    SPELL_RAM_CANTER                        = 42993,
    SPELL_RAM_GALLOP                        = 42994,
    SPELL_RAM_FATIGUE                       = 43052,
    SPELL_EXHAUSTED_RAM                     = 43332,
    SPELL_BREWFEST_QUEST_SPEED_BUNNY_GREEN  = 43345,
    SPELL_BREWFEST_QUEST_SPEED_BUNNY_YELLOW = 43346,
    SPELL_BREWFEST_QUEST_SPEED_BUNNY_RED    = 43347,

    // There and Back Again quest
    SPELL_RAMMSTEIN_SWIFT_WORK_RAM          = 43880,
    SPELL_CREATE_TICKETS                    = 44501,

    // Drunken Stupor achievement
    SPELL_SYNTHEBREW_GOGGLES                = 65607,

    // "Bark for..." quests
    NPC_DND_BREWFEST_BARKER_BUNNY_1         = 24202,
    NPC_DND_BREWFEST_BARKER_BUNNY_2         = 24203,
    NPC_DND_BREWFEST_BARKER_BUNNY_3         = 24204,
    NPC_DND_BREWFEST_BARKER_BUNNY_4         = 24205,
    QUEST_BARK_FOR_THE_BARLEYBREWS          = 11293,
    QUEST_BARK_FOR_THE_THUNDERBREWS         = 11294,
    QUEST_BARK_FOR_DROHNS_DISTILLERY        = 11407,
    QUEST_BARK_FOR_TCHALIS_VOODOO_BREWERY   = 11408,
    SAY_BARKER_BUNNY_BARLEYBREWS            = 0,
    SAY_BARKER_BUNNY_THUNDERBREWS           = 1,
    SAY_BARKER_BUNNY_DROHNS_DISTILLERY      = 2,
    SAY_BARKER_BUNNY_TCHALIS_VOODOO_BREWERY = 3,
    SPLEL_QUEST_COMPLETE_THEBARLEYBREWS     = 47134,

};

// 42924 - Giddyup!
class spell_brewfest_giddyup : public AuraScript
{
    PrepareAuraScript(spell_brewfest_giddyup);

    void OnChange(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        if (!target)
            return;

        if (!target->HasAura(SPELL_RENTAL_RACING_RAM) && !target->HasAura(SPELL_RAMMSTEIN_SWIFT_WORK_RAM))
        {
            target->RemoveAura(GetId());
            return;
        }

        if (target->HasAura(SPELL_EXHAUSTED_RAM))
            return;

        switch (GetStackAmount())
        {
            case 1:
                target->RemoveAura(SPELL_RAM_LEVEL_NEUTRAL);
                target->RemoveAura(SPELL_RAM_CANTER);
                target->CastSpell(target, SPELL_RAM_TROT, true);
                break;
            case 6:
                target->RemoveAura(SPELL_RAM_TROT);
                target->RemoveAura(SPELL_RAM_GALLOP);
                target->CastSpell(target, SPELL_RAM_CANTER, true);
                break;
            case 11:
                target->RemoveAura(SPELL_RAM_CANTER);
                target->CastSpell(target, SPELL_RAM_GALLOP, true);
                break;
            default:
                break;
        }

        if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEFAULT)
        {
            target->RemoveAura(SPELL_RAM_TROT);
            target->CastSpell(target, SPELL_RAM_LEVEL_NEUTRAL, true);
        }
    }

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        Unit* target = GetTarget();
        if (!target)
            return;

        target->RemoveAuraFromStack(GetId());
    }

    void Register()
    {
        AfterEffectApply += AuraEffectApplyFn(spell_brewfest_giddyup::OnChange, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
        OnEffectRemove += AuraEffectRemoveFn(spell_brewfest_giddyup::OnChange, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_brewfest_giddyup::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 42992 - Ram - Trot
// 42993 - Ram - Canter
// 42994 - Ram - Gallop
// 43310 - Ram Level - Neutral
class spell_brewfest_ram : public AuraScript
{
    PrepareAuraScript(spell_brewfest_ram);

    void OnPeriodic(AuraEffect const* aurEff)
    {
        Unit* target = GetTarget();
        if (!target)
            return;

        if (target->HasAura(SPELL_EXHAUSTED_RAM))
            return;

        switch (GetId())
        {
            case SPELL_RAM_LEVEL_NEUTRAL:
                if (Aura* aura = target->GetAura(SPELL_RAM_FATIGUE))
                    aura->ModStackAmount(-4);

                break;
            case SPELL_RAM_TROT:
                if (Aura* aura = target->GetAura(SPELL_RAM_FATIGUE))
                    aura->ModStackAmount(-2);

                if (aurEff->GetTickNumber() == 4)
                    target->CastSpell(target, SPELL_BREWFEST_QUEST_SPEED_BUNNY_GREEN, true);

                break;
            case SPELL_RAM_CANTER:
                target->CastCustomSpell(SPELL_RAM_FATIGUE, SPELLVALUE_AURA_STACK, 1, target, TRIGGERED_FULL_MASK);
                if (aurEff->GetTickNumber() == 8)
                    target->CastSpell(target, SPELL_BREWFEST_QUEST_SPEED_BUNNY_YELLOW, true);

                break;
            case SPELL_RAM_GALLOP:
                target->CastCustomSpell(SPELL_RAM_FATIGUE, SPELLVALUE_AURA_STACK, target->HasAura(SPELL_RAM_FATIGUE) ? 4 : 5, target, TRIGGERED_FULL_MASK);
                if (aurEff->GetTickNumber() == 8)
                    target->CastSpell(target, SPELL_BREWFEST_QUEST_SPEED_BUNNY_RED, true);

                break;
            default:
                break;
        }
    }

    void Register()
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_brewfest_ram::OnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 43052 - Ram Fatigue
class spell_brewfest_ram_fatigue : public AuraScript
{
    PrepareAuraScript(spell_brewfest_ram_fatigue);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        if (!target)
            return;

        if (GetStackAmount() >= 100)
        {
            target->RemoveAura(SPELL_RAM_LEVEL_NEUTRAL);
            target->RemoveAura(SPELL_RAM_TROT);
            target->RemoveAura(SPELL_RAM_CANTER);
            target->RemoveAura(SPELL_RAM_GALLOP);

            target->CastSpell(target, SPELL_EXHAUSTED_RAM, true);
        }
    }

    void Register()
    {
        AfterEffectApply += AuraEffectApplyFn(spell_brewfest_ram_fatigue::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// 43450 - Brewfest - apple trap - friendly DND
class spell_brewfest_apple_trap : public AuraScript
{
    PrepareAuraScript(spell_brewfest_apple_trap);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAura(SPELL_RAM_FATIGUE);
    }

    void Register()
    {
        OnEffectApply += AuraEffectApplyFn(spell_brewfest_apple_trap::OnApply, EFFECT_0, SPELL_AURA_FORCE_REACTION, AURA_EFFECT_HANDLE_REAL);
    }
};

// 43662 - Brewfest - Throw Keg - Player - DND
class spell_brewfest_throw_keg_player : public SpellScript
{
    PrepareSpellScript(spell_brewfest_throw_keg_player);

    void HandleHit(SpellEffIndex)
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return;

        Aura* rammsteinAura = player->GetAura(SPELL_RAMMSTEIN_SWIFT_WORK_RAM);
        // Not blizzlike but I don't want to change all the conditions, may be at next year...if this shit will show signs of life
        if (!rammsteinAura)
            rammsteinAura = player->GetAura(SPELL_RENTAL_RACING_RAM);

        if (!rammsteinAura)
            return;

        rammsteinAura->SetDuration(rammsteinAura->GetDuration() + 30 * IN_MILLISECONDS);

        player->CastSpell(player, SPELL_CREATE_TICKETS, true);
    }

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_brewfest_throw_keg_player::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

enum CreateEmptyBrewBottle
{
    SPELL_BOTM_CREATE_EMPTY_BREW_BOTTLE    = 51655
};

// 42254, 42255, 42256, 42257, 42258, 42259, 42260, 42261, 42263, 42264, 43959, 43961 - Weak Alcohol
class spell_brewfest_botm_weak_alcohol : public SpellScript
{
    PrepareSpellScript(spell_brewfest_botm_weak_alcohol);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_BOTM_CREATE_EMPTY_BREW_BOTTLE))
            return false;
        return true;
    }

    void HandleAfterCast()
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_BOTM_CREATE_EMPTY_BREW_BOTTLE, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_brewfest_botm_weak_alcohol::HandleAfterCast);
    }
};

enum EmptyBottleThrow
{
    SPELL_BOTM_EMPTY_BOTTLE_THROW_IMPACT_CREATURE    = 51695,   // Just unit, not creature
    SPELL_BOTM_EMPTY_BOTTLE_THROW_IMPACT_GROUND      = 51697
};

// 51694 - BOTM - Empty Bottle Throw - Resolve
class spell_brewfest_botm_empty_bottle_throw_resolve : public SpellScript
{
    PrepareSpellScript(spell_brewfest_botm_empty_bottle_throw_resolve);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_BOTM_EMPTY_BOTTLE_THROW_IMPACT_CREATURE) ||
            !sSpellMgr->GetSpellInfo(SPELL_BOTM_EMPTY_BOTTLE_THROW_IMPACT_GROUND))
            return false;
        return true;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (Unit* target = GetHitUnit())
            caster->CastSpell(target, SPELL_BOTM_EMPTY_BOTTLE_THROW_IMPACT_CREATURE, true);
        else
            caster->CastSpell(GetHitDest()->GetPosition(), SPELL_BOTM_EMPTY_BOTTLE_THROW_IMPACT_GROUND, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_brewfest_botm_empty_bottle_throw_resolve::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

enum MetoksBubbleBock
{
    SPELL_BOTM_BUBBLE_BREW_TRIGGER_MISSILE    = 50072
};

// 49822 - Bloated
class spell_brewfest_botm_bloated : public AuraScript
{
    PrepareAuraScript(spell_brewfest_botm_bloated);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_BOTM_BUBBLE_BREW_TRIGGER_MISSILE))
            return false;
        return true;
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_BOTM_BUBBLE_BREW_TRIGGER_MISSILE, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_brewfest_botm_bloated::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

enum BlackrockLager
{
    SPELL_BOTM_BELCH_FIRE_VISUAL    = 49737
};

// 49738 - Internal Combustion
class spell_brewfest_botm_internal_combustion : public AuraScript
{
    PrepareAuraScript(spell_brewfest_botm_internal_combustion);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_BOTM_BELCH_FIRE_VISUAL))
            return false;
        return true;
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_BOTM_BELCH_FIRE_VISUAL, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_brewfest_botm_internal_combustion::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

enum BinaryBrew
{
    SPELL_LEARN_GNOMISH_BINARY      = 50242,
    SPELL_LEARN_GOBLIN_BINARY       = 50246
};

// 50243 - Teach Language
class spell_brewfest_botm_teach_language : public SpellScript
{
    PrepareSpellScript(spell_brewfest_botm_teach_language);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_LEARN_GNOMISH_BINARY) ||
            !sSpellMgr->GetSpellInfo(SPELL_LEARN_GOBLIN_BINARY))
            return false;
        return true;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Player* caster = GetCaster()->ToPlayer())
            caster->CastSpell(caster, caster->GetTeam() == ALLIANCE ? SPELL_LEARN_GNOMISH_BINARY : SPELL_LEARN_GOBLIN_BINARY, true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_brewfest_botm_teach_language::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

enum WildWinterPilsner
{
    SPELL_BOTM_UNLEASH_THE_BEAST    = 50099
};

// 50098 - The Beast Within
class spell_brewfest_botm_the_beast_within : public AuraScript
{
    PrepareAuraScript(spell_brewfest_botm_the_beast_within);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_BOTM_UNLEASH_THE_BEAST))
            return false;
        return true;
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_BOTM_UNLEASH_THE_BEAST);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_brewfest_botm_the_beast_within::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

enum IzzardsEverFlavor
{
    SPELL_BOTM_BELCH_BREW_BELCH_VISUAL    = 49860
};

// 49864 - Gassy
class spell_brewfest_botm_gassy : public AuraScript
{
    PrepareAuraScript(spell_brewfest_botm_gassy);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_BOTM_BELCH_BREW_BELCH_VISUAL))
            return false;
        return true;
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_BOTM_BELCH_BREW_BELCH_VISUAL, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_brewfest_botm_gassy::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

enum StranglethornBrew
{
    SPELL_BOTM_JUNGLE_BREW_VISION_EFFECT    = 50010
};

// 49962 - Jungle Madness!
class spell_brewfest_botm_jungle_madness : public SpellScript
{
    PrepareSpellScript(spell_brewfest_botm_jungle_madness);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_BOTM_JUNGLE_BREW_VISION_EFFECT))
            return false;
        return true;
    }

    void HandleAfterCast()
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_BOTM_JUNGLE_BREW_VISION_EFFECT, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_brewfest_botm_jungle_madness::HandleAfterCast);
    }
};

class at_brewfest_bark : public AreaTriggerScript
{
public:
    at_brewfest_bark() : AreaTriggerScript("at_brewfest_bark") { }

    bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
    {
        if (!player->HasAura(SPELL_RENTAL_RACING_RAM))
            return false;

        uint32 text;
        if (player->GetQuestStatus(QUEST_BARK_FOR_THE_BARLEYBREWS) == QUEST_STATUS_INCOMPLETE)
            text = SAY_BARKER_BUNNY_BARLEYBREWS;
        else if (player->GetQuestStatus(QUEST_BARK_FOR_THE_THUNDERBREWS) == QUEST_STATUS_INCOMPLETE)
            text = SAY_BARKER_BUNNY_THUNDERBREWS;
        else if (player->GetQuestStatus(QUEST_BARK_FOR_DROHNS_DISTILLERY) == QUEST_STATUS_INCOMPLETE)
            text = SAY_BARKER_BUNNY_DROHNS_DISTILLERY;
        else if (player->GetQuestStatus(QUEST_BARK_FOR_TCHALIS_VOODOO_BREWERY) == QUEST_STATUS_INCOMPLETE)
            text = SAY_BARKER_BUNNY_TCHALIS_VOODOO_BREWERY;
        else
            return false;

        Creature* bunny = nullptr;
        for (auto id : { NPC_DND_BREWFEST_BARKER_BUNNY_1, NPC_DND_BREWFEST_BARKER_BUNNY_2, NPC_DND_BREWFEST_BARKER_BUNNY_3, NPC_DND_BREWFEST_BARKER_BUNNY_4 })
            if (bunny = player->FindNearestCreature(id, 50.0f))
                break;

        if (!bunny)
            return false;

        player->KilledMonsterCredit(bunny->GetEntry(), bunny->GetGUID());
        sCreatureTextMgr->SendChat(bunny, text, 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_NORMAL, 0, TEAM_OTHER, false, player);

        // FIXME
        if (text == SAY_BARKER_BUNNY_BARLEYBREWS)
        {
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_BARK_FOR_THE_BARLEYBREWS))
            {
                bool done = true;
                for (auto const& objective : quest->Objectives)
                    if (!player->IsQuestObjectiveComplete(quest, objective))
                        done = false;

                if (done)
                    player->CastSpell(player, SPLEL_QUEST_COMPLETE_THEBARLEYBREWS, true);
            }
        }

        return false;
    }
};

enum
{
    // Creature
    NPC_BREW_OF_THE_MONTH_CLUB              = 27487,

    // Quest
    QUEST_BREW_OF_THE_MONTH_CLUB_A          = 12278,
    QUEST_BREW_OF_THE_MONTH_CLUB_H          = 12306,
    QUEST_BREW_OF_THE_MONTH_CLUB_A_2        = 12420,
    QUEST_BREW_OF_THE_MONTH_CLUB_H_2        = 12421,
    QUEST_BREWFEST_BOTM_JANUARY_FIRST       = 14974,
    QUEST_BREWFEST_BOTM_FEBRUARY_FIRST      = 14975,
    QUEST_BREWFEST_BOTM_MARCH_FIRST         = 14976,
    QUEST_BREWFEST_BOTM_APRIL_FIRST         = 14977,
    QUEST_BREWFEST_BOTM_MAY_FIRST           = 14978,
    QUEST_BREWFEST_BOTM_JUNE_FIRST          = 14979,
    QUEST_BREWFEST_BOTM_JULY_FIRST          = 14980,
    QUEST_BREWFEST_BOTM_AUGUST_FIRST        = 14981,
    QUEST_BREWFEST_BOTM_SEPTEMBER_FIRST     = 14982,
    QUEST_BREWFEST_BOTM_OCTOBER_FIRST       = 14983,
    QUEST_BREWFEST_BOTM_NOVEMBER_FIRST      = 14984,
    QUEST_BREWFEST_BOTM_DECEMBER_FIRST      = 14985,
    QUEST_BREWFEST_BOTM_JANUARY_SEASONAL    = 14986,
    QUEST_BREWFEST_BOTM_FEBRUARY_SEASONAL   = 14987,
    QUEST_BREWFEST_BOTM_MARCH_SEASONAL      = 14988,
    QUEST_BREWFEST_BOTM_APRIL_SEASONAL      = 14989,
    QUEST_BREWFEST_BOTM_MAY_SEASONAL        = 14990,
    QUEST_BREWFEST_BOTM_JUNE_SEASONAL       = 14991,
    QUEST_BREWFEST_BOTM_JULY_SEASONAL       = 14992,
    QUEST_BREWFEST_BOTM_AUGUST_SEASONAL     = 14993,
    QUEST_BREWFEST_BOTM_SEPTEMBER_SEASONAL  = 14994,
    QUEST_BREWFEST_BOTM_OCTOBER_SEASONAL    = 14995,
    QUEST_BREWFEST_BOTM_NOVEMBER_SEASONAL   = 14996,
    QUEST_BREWFEST_BOTM_DECEMBER_SEASONAL   = 14997,

    // MailTemplate
    MAIL_TEMPLATE_BOTM_JANUARY_1            = 197,
    MAIL_TEMPLATE_BOTM_FEBRUARY_1           = 198,
    MAIL_TEMPLATE_BOTM_MARCH_1              = 199,
    MAIL_TEMPLATE_BOTM_APRIL_1              = 200,
    MAIL_TEMPLATE_BOTM_MAY_1                = 201,
    MAIL_TEMPLATE_BOTM_JUNE_1               = 202,
    MAIL_TEMPLATE_BOTM_JULY_1               = 203,
    MAIL_TEMPLATE_BOTM_AUGUST_1             = 204,
    MAIL_TEMPLATE_BOTM_SEPTEMBER_1          = 205,
    MAIL_TEMPLATE_BOTM_OCTOBER_1            = 206,
    MAIL_TEMPLATE_BOTM_NOVEMBER_1           = 207,
    MAIL_TEMPLATE_BOTM_DECEMBER_1           = 208,
    MAIL_TEMPLATE_BOTM_JANUARY_2            = 212,
    MAIL_TEMPLATE_BOTM_FEBRUARY_2           = 213,
    MAIL_TEMPLATE_BOTM_MARCH_2              = 214,
    MAIL_TEMPLATE_BOTM_APRIL_2              = 215,
    MAIL_TEMPLATE_BOTM_MAY_2                = 216,
    MAIL_TEMPLATE_BOTM_JUNE_2               = 217,
    MAIL_TEMPLATE_BOTM_JULY_2               = 218,
    MAIL_TEMPLATE_BOTM_AUGUST_2             = 219,
    MAIL_TEMPLATE_BOTM_SEPTEMBER_2          = 220,
    MAIL_TEMPLATE_BOTM_OCTOBER_2            = 221,
    MAIL_TEMPLATE_BOTM_NOVEMBER_2           = 222,
    MAIL_TEMPLATE_BOTM_DECEMBER_2           = 223,
};

// 50 51 52 53 54 55 56 57 58 59 60 61
class event_brewfest_month : GameEventScript
{
    public:
        event_brewfest_month(char const* name, std::pair<uint32, uint32> mailTemplates, std::pair<uint32, uint32> quests)
            : GameEventScript(name)
            , mailTemplateFirstID(mailTemplates.first)
            , mailTemplateSecondID(mailTemplates.second)
            , questFirstID(quests.first)
            , questSeasonalID(quests.second)
        { }

        void OnStarted() override
        {
            // Send mail to all online players
            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            for (auto&& session : sWorld->GetAllSessions())
                if (Player* player = session.second->GetPlayer())
                    SendMail(player, trans);
            CharacterDatabase.CommitTransaction(trans);
        }

        void FillWorldStates(Player* player, WorldStateBuilder& /*builder*/) override
        {
            if (IsEligible(player))
            {
                CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
                SendMail(player, trans);
                CharacterDatabase.CommitTransaction(trans);
            }
        }

    private:
        uint32 mailTemplateFirstID;
        uint32 mailTemplateSecondID;
        uint32 questFirstID;
        uint32 questSeasonalID;

        void SendMail(Player* player, CharacterDatabaseTransaction trans)
        {
            if (IsEligible(player))
            {
                // Send a different mail for the first year
                uint32 mailTemplate = mailTemplateSecondID;
                if (!player->IsQuestRewarded(questFirstID))
                {
                    player->CompleteQuest(questFirstID);
                    mailTemplate = mailTemplateFirstID;
                }
                player->CompleteQuest(questSeasonalID);
                MailDraft(mailTemplate).SendMailTo(trans, player, MailSender(MAIL_CREATURE, NPC_BREW_OF_THE_MONTH_CLUB), MAIL_CHECK_MASK_HAS_BODY, 0);
            }
        }

        bool IsEligible(Player* player)
        {
            // Not member of the club
            if (!player->IsQuestRewarded(QUEST_BREW_OF_THE_MONTH_CLUB_A) &&
                !player->IsQuestRewarded(QUEST_BREW_OF_THE_MONTH_CLUB_H) &&
                !player->IsQuestRewarded(QUEST_BREW_OF_THE_MONTH_CLUB_A_2) &&
                !player->IsQuestRewarded(QUEST_BREW_OF_THE_MONTH_CLUB_H_2))
                return false;

            // Has already received a brew this month
            if (player->IsQuestRewarded(questSeasonalID))
                return false;

            return true;
        }
};

void AddSC_event_brewfest()
{
    new aura_script<spell_brewfest_giddyup>("spell_brewfest_giddyup");
    new aura_script<spell_brewfest_ram>("spell_brewfest_ram");
    new aura_script<spell_brewfest_ram_fatigue>("spell_brewfest_ram_fatigue");
    new aura_script<spell_brewfest_apple_trap>("spell_brewfest_apple_trap");
    new spell_script<spell_brewfest_throw_keg_player>("spell_brewfest_throw_keg_player");
    new spell_script<spell_brewfest_botm_weak_alcohol>("spell_brewfest_botm_weak_alcohol");
    new spell_script<spell_brewfest_botm_empty_bottle_throw_resolve>("spell_brewfest_botm_empty_bottle_throw_resolve");
    new aura_script<spell_brewfest_botm_bloated>("spell_brewfest_botm_bloated");
    new aura_script<spell_brewfest_botm_internal_combustion>("spell_brewfest_botm_internal_combustion");
    new spell_script<spell_brewfest_botm_teach_language>("spell_brewfest_botm_teach_language");
    new aura_script<spell_brewfest_botm_the_beast_within>("spell_brewfest_botm_the_beast_within");
    new aura_script<spell_brewfest_botm_gassy>("spell_brewfest_botm_gassy");
    new spell_script<spell_brewfest_botm_jungle_madness>("spell_brewfest_botm_jungle_madness");

    new at_brewfest_bark();

    new event_brewfest_month("event_brewfest_botm_january",   { MAIL_TEMPLATE_BOTM_JANUARY_1,   MAIL_TEMPLATE_BOTM_JANUARY_2   }, { QUEST_BREWFEST_BOTM_JANUARY_FIRST,   QUEST_BREWFEST_BOTM_JANUARY_SEASONAL   });
    new event_brewfest_month("event_brewfest_botm_february",  { MAIL_TEMPLATE_BOTM_FEBRUARY_1,  MAIL_TEMPLATE_BOTM_FEBRUARY_2  }, { QUEST_BREWFEST_BOTM_FEBRUARY_FIRST,  QUEST_BREWFEST_BOTM_FEBRUARY_SEASONAL  });
    new event_brewfest_month("event_brewfest_botm_march",     { MAIL_TEMPLATE_BOTM_MARCH_1,     MAIL_TEMPLATE_BOTM_MARCH_2     }, { QUEST_BREWFEST_BOTM_MARCH_FIRST,     QUEST_BREWFEST_BOTM_MARCH_SEASONAL     });
    new event_brewfest_month("event_brewfest_botm_april",     { MAIL_TEMPLATE_BOTM_APRIL_1,     MAIL_TEMPLATE_BOTM_APRIL_2     }, { QUEST_BREWFEST_BOTM_APRIL_FIRST,     QUEST_BREWFEST_BOTM_APRIL_SEASONAL     });
    new event_brewfest_month("event_brewfest_botm_may",       { MAIL_TEMPLATE_BOTM_MAY_1,       MAIL_TEMPLATE_BOTM_MAY_2       }, { QUEST_BREWFEST_BOTM_MAY_FIRST,       QUEST_BREWFEST_BOTM_MAY_SEASONAL       });
    new event_brewfest_month("event_brewfest_botm_june",      { MAIL_TEMPLATE_BOTM_JUNE_1,      MAIL_TEMPLATE_BOTM_JUNE_2      }, { QUEST_BREWFEST_BOTM_JUNE_FIRST,      QUEST_BREWFEST_BOTM_JUNE_SEASONAL      });
    new event_brewfest_month("event_brewfest_botm_july",      { MAIL_TEMPLATE_BOTM_JULY_1,      MAIL_TEMPLATE_BOTM_JULY_2      }, { QUEST_BREWFEST_BOTM_JULY_FIRST,      QUEST_BREWFEST_BOTM_JULY_SEASONAL      });
    new event_brewfest_month("event_brewfest_botm_august",    { MAIL_TEMPLATE_BOTM_AUGUST_1,    MAIL_TEMPLATE_BOTM_AUGUST_2    }, { QUEST_BREWFEST_BOTM_AUGUST_FIRST,    QUEST_BREWFEST_BOTM_AUGUST_SEASONAL    });
    new event_brewfest_month("event_brewfest_botm_september", { MAIL_TEMPLATE_BOTM_SEPTEMBER_1, MAIL_TEMPLATE_BOTM_SEPTEMBER_2 }, { QUEST_BREWFEST_BOTM_SEPTEMBER_FIRST, QUEST_BREWFEST_BOTM_SEPTEMBER_SEASONAL });
    new event_brewfest_month("event_brewfest_botm_october",   { MAIL_TEMPLATE_BOTM_OCTOBER_1,   MAIL_TEMPLATE_BOTM_OCTOBER_2   }, { QUEST_BREWFEST_BOTM_OCTOBER_FIRST,   QUEST_BREWFEST_BOTM_OCTOBER_SEASONAL   });
    new event_brewfest_month("event_brewfest_botm_november",  { MAIL_TEMPLATE_BOTM_NOVEMBER_1,  MAIL_TEMPLATE_BOTM_NOVEMBER_2  }, { QUEST_BREWFEST_BOTM_NOVEMBER_FIRST,  QUEST_BREWFEST_BOTM_NOVEMBER_SEASONAL  });
    new event_brewfest_month("event_brewfest_botm_december",  { MAIL_TEMPLATE_BOTM_DECEMBER_1,  MAIL_TEMPLATE_BOTM_DECEMBER_2  }, { QUEST_BREWFEST_BOTM_DECEMBER_FIRST,  QUEST_BREWFEST_BOTM_DECEMBER_SEASONAL  });
}
