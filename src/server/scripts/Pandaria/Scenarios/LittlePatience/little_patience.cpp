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

#include "little_patience.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "CreatureAI.h"
#include "SpellScript.h"
#include "Vehicle.h"
#include "ScenarioMgr.h"

enum Spells
{
    SPELL_GORGE                           = 134554,
    SPELL_GORGE_EFF                       = 134555,
    SPELL_BLOOD_RAGE                      = 134959,
    SPELL_FRENZIED_ASSAULT                = 134983,
    SPELL_BLOOD_RAGE_EFF                  = 134963,
    SPELL_BLOOD_RAGE_FIXATE               = 134974,
    SPELL_CRUSHING_LEAP                   = 135005,
    SPELL_CRUSHING_LEAP_EFF               = 135006,
    SPELL_POUNCE                          = 119579,
    SPELL_POUNCE_EFF                      = 119578,
    SPELL_RAVAGE                          = 119580,
    SPELL_VICIOUS_REND                    = 135498,
    SPELL_SWAMP_GAS                       = 135494,
    SPELL_SWAMP_GAS_TRIGGERING            = 135495,
    SPELL_SWAMP_GAS_EFF                   = 135496,
    SPELL_SPIRIT_BOMB                     = 134609,
    SPELL_SPIRIT_BOMB_SUMM                = 134610,
    SPELL_SPIRIT_BOMB_TRIGGERING          = 134592,
    SPELL_SPIRIT_BOMB_EFF                 = 134600,
    SPELL_SPIRIT_BARRAGE                  = 134674,

    // Constructions
    SPELL_JINYU_CONSTRUCTION_PROGRESS     = 133311,
    SPELL_GNOME_CONSTRUCTION_PROGRESS     = 133849,
    SPELL_DWARF_CONSTRUCTION_PROGRESS     = 134086,
    SPELL_PANDAREN_CONSTRUCTION_PROGRESS  = 134567,
    SPELL_NIGHT_ELF_CONSTRUCTION_PROGRESS = 135260,
    SPELL_CONSTRUCTION_BAR_MOVEMENT       = 133852,
    SPELL_TERRIFIED                       = 133494,
};

enum Events
{
    EVENT_GORGE = 1,
    EVENT_BLOOD_RAGE,
    EVENT_CRUSHING_LEAP,
    EVENT_POUNCE,
    EVENT_RAVAGE,
    EVENT_VICIOUS_REND,
    EVENT_SWAMP_GAS,
    EVENT_SPIRIT_BOMB,
    EVENT_SPIRIT_BARRAGE,
};

const std::map<uint32, uint32> leadersBarProgress =
{
    { NPC_ELDER_ADLER,       SPELL_JINYU_CONSTRUCTION_PROGRESS     },
    { NPC_ROSEY_AXLEROD,     SPELL_GNOME_CONSTRUCTION_PROGRESS     },
    { NPC_FERA_PEARL,        SPELL_NIGHT_ELF_CONSTRUCTION_PROGRESS },
    { NPC_DUFF_MCSTRUM,      SPELL_DWARF_CONSTRUCTION_PROGRESS     },
    { NPC_MASTER_BROWNSTONE, SPELL_PANDAREN_CONSTRUCTION_PROGRESS  },
};

const std::vector<uint32> progressInspired =
{
    SPELL_INSPIRED_NIGHT_ELF,
    SPELL_INSPIRED_GNOME_EFF,
    SPELL_INSPIRED_JYNUI,
    SPELL_INSPIRED_DWARF
};

// Muckmaw Crocolisk 67922
struct npc_little_patience_muckmaw_crocolisk : public customCreatureAI
{
    npc_little_patience_muckmaw_crocolisk(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, SPELL_GORGE);
        events.ScheduleEvent(EVENT_GORGE, 12 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_GORGE, 12 * IN_MILLISECONDS, EVENT_GORGE, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Commander Scargash 68474
struct npc_little_patience_commander_scargash : public customCreatureAI
{
    npc_little_patience_commander_scargash(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        Talk(TALK_INTRO);

        if (who)
            DoCast(who, SPELL_CRUSHING_LEAP);

        events.ScheduleEvent(EVENT_BLOOD_RAGE, 5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_CRUSHING_LEAP, 10 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetInstanceScript())
            me->GetInstanceScript()->SetData(DATA_SCARGASH_DEFEAT, DONE);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_BLOOD_RAGE)
            {
                Talk(TALK_SPECIAL_5); // Announce
                Talk(urand(0, 1) ? TALK_SPECIAL_4 : TALK_SPECIAL_6);
            }

            ExecuteTargetEvent(SPELL_BLOOD_RAGE, 15 * IN_MILLISECONDS, EVENT_BLOOD_RAGE, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_CRUSHING_LEAP, 17 * IN_MILLISECONDS, EVENT_CRUSHING_LEAP, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Krasari Stalker 68348
struct npc_little_patience_krasari_stalker : public customCreatureAI
{
    npc_little_patience_krasari_stalker(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        if (who)
            DoCast(who, SPELL_POUNCE);

        events.ScheduleEvent(EVENT_POUNCE, 12 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_RAVAGE, 4.5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_RAVAGE, 8 * IN_MILLISECONDS, EVENT_RAVAGE, eventId);
            ExecuteTargetEvent(SPELL_POUNCE, 15.5 * IN_MILLISECONDS, EVENT_POUNCE, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// King Varian Wrynn 69026
struct npc_little_patience_king_varian_wrynn : public ScriptedAI
{
    npc_little_patience_king_varian_wrynn(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    uint32 delay;
    uint32 cosmeticDelay;

    void Reset() override
    {
        scheduler.CancelAll();
        delay = 0;
        cosmeticDelay = 0;
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_START_INTRO:
                me->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 0);
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(0, varianTablePos);

                delay = me->GetSplineDuration();
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext /*context*/)
                {
                    Talk(TALK_INTRO);

                    if (Player* target = me->FindNearestPlayer(100.0f))
                        me->SetFacingToObject(target);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 5600), [this](TaskContext /*context*/)
                {
                    if (Creature* chiJi = ObjectAccessor::GetCreature(*me, me->GetInstanceId() ? me->GetInstanceScript()->GetGuidData(NPC_CHI_JI) : ObjectGuid::Empty))
                        chiJi->AI()->Talk(TALK_INTRO);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 15000), [this](TaskContext /*context*/)
                {
                    // Spawn faction leaders
                    for (auto&& itr : invFactionLeadersType)
                        me->SummonCreature(itr.first, itr.second, TEMPSUMMON_MANUAL_DESPAWN);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 5000), [this](TaskContext context)
                {
                    // Any Assault
                    uint32 idx = urand(TYPE_GNOME, TYPE_PANDAREN);
                    if (Creature* assaulter = me->SummonCreature(Trinity::Containers::SelectRandomContainerElement(assaulters), invAssaultType.find(idx)->second, TEMPSUMMON_MANUAL_DESPAWN))
                        assaulter->AI()->SetData(TYPE_ASSAULT_IDX, idx);

                    context.Repeat(Seconds(60));
                });

                // Dialogue with Tyrande
                scheduler
                    .Schedule(Milliseconds(delay += 1 * MINUTE * IN_MILLISECONDS), [this](TaskContext /*context*/)
                {
                    if (Creature* tyrande = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TYRANDE_WHISPERWIND) : ObjectGuid::Empty))
                        tyrande->AI()->Talk(TALK_INTRO);

                    scheduler
                        .Schedule(Milliseconds(9500), [this](TaskContext /*context*/)
                    {
                        Talk(TALK_SPECIAL_2);
                    });
                });

                scheduler
                    .Schedule(Milliseconds(delay += 1 * MINUTE * IN_MILLISECONDS), [this](TaskContext /*context*/)
                {
                    if (Creature* tyrande = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TYRANDE_WHISPERWIND) : ObjectGuid::Empty))
                        tyrande->AI()->Talk(TALK_SPECIAL_1);

                    scheduler
                        .Schedule(Milliseconds(9500), [this](TaskContext /*context*/)
                    {
                        Talk(TALK_SPECIAL_6);
                    });
                });

                scheduler
                    .Schedule(Milliseconds(delay += 1 * MINUTE * IN_MILLISECONDS), [this](TaskContext /*context*/)
                {
                    if (Creature* tyrande = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TYRANDE_WHISPERWIND) : ObjectGuid::Empty))
                        tyrande->AI()->Talk(TALK_SPECIAL_2);

                    scheduler
                        .Schedule(Milliseconds(9500), [this](TaskContext /*context*/)
                    {
                        Talk(TALK_SPECIAL_8);
                    });
                });

                // Just Periodic Announce
                scheduler
                    .Schedule(Milliseconds(45 * IN_MILLISECONDS), [this](TaskContext context)
                {
                    Talk(Trinity::Containers::SelectRandomContainerElement(customAnnouce));
                    context.Repeat(Seconds(20));
                });
                break;
            case ACTION_FORCE_ASSAULT_TEMPLE:
                scheduler.CancelAll();
                Talk(TALK_SPECIAL_9);

                delay = 6 * IN_MILLISECONDS;
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext /*context*/)
                {
                    if (Creature* scargash = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_COMMANDER_SCARGASH) : ObjectGuid::Empty))
                        scargash->AI()->Talk(TALK_INTRO);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 6500), [this](TaskContext /*context*/)
                {
                    Talk(TALK_SPECIAL_10);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 3500), [this](TaskContext /*context*/)
                {
                    Talk(TALK_SPECIAL_11);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 10000), [this](TaskContext /*context*/)
                {
                    if (Creature* scargash = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_COMMANDER_SCARGASH) : ObjectGuid::Empty))
                        scargash->AI()->Talk(TALK_SPECIAL_1);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 6000), [this](TaskContext /*context*/)
                {
                    // Attack!
                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SetData(DATA_MASSIVE_ATTACK, 1);
                });
                break;
            case ACTION_SCARGASH_SLAIN:
                me->SetVisible(true);
                me->GetMotionMaster()->MovePoint(0, varianOuterPos);
                
                delay = me->GetSplineDuration();
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext /*context*/)
                {
                    Talk(TALK_SPECIAL_12);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 7000), [this](TaskContext /*context*/)
                {
                    if (Creature* tyrande = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TYRANDE_WHISPERWIND + 1) : ObjectGuid::Empty))
                        tyrande->AI()->Talk(TALK_SPECIAL_3);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 6100), [this](TaskContext /*context*/)
                {
                    Talk(TALK_SPECIAL_13);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 8200), [this](TaskContext /*context*/)
                {
                    Talk(TALK_SPECIAL_14);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 5000), [this](TaskContext /*context*/)
                {
                    Talk(TALK_SPECIAL_15);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 7000), [this](TaskContext /*context*/)
                {
                    Talk(TALK_SPECIAL_16);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 14000), [this](TaskContext /*context*/)
                {
                    if (Creature* tyrande = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TYRANDE_WHISPERWIND + 1) : ObjectGuid::Empty))
                        tyrande->AI()->Talk(TALK_SPECIAL_4);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 8500), [this](TaskContext /*context*/)
                {
                    if (Creature* chiJi = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_CHI_JI + 1) : ObjectGuid::Empty))
                    {
                        chiJi->SetFacingToObject(me);
                        chiJi->AI()->Talk(TALK_SPECIAL_1);
                    }
                });

                scheduler
                    .Schedule(Milliseconds(delay += 7300), [this](TaskContext /*context*/)
                {
                    if (Creature* chiJi = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_CHI_JI + 1) : ObjectGuid::Empty))
                        chiJi->AI()->Talk(TALK_SPECIAL_2);

                    // Set quest flag for me
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                });

                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Faction Leader 67569, 67883, 68599, 68057, 68240
struct npc_little_patience_faction_leader : public customCreatureAI
{
    npc_little_patience_faction_leader(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    uint32 constructionProgress;

    void IsSummonedBy(Unit* summoner) override
    {
        constructionProgress = 0;

        scheduler
            .Schedule(Seconds(1), [this](TaskContext context)
        {
            // 20 per second & 45 if this camp has buff
            if (!me->HasAura(SPELL_TERRIFIED))
                constructionProgress += hasInspiredBuff() ? 12 : 5;

            if (constructionProgress > 999) // this camp done with their construction
            {
                if (me->GetInstanceScript())
                    me->GetInstanceScript()->SetData(DATA_CONSTRUCTION_COUNT, 1);
            }
            else
                context.Repeat(Seconds(2));
        });
    }
    void AttackStart(Unit* target) override { }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_PROGRESS_BAR_VALUE)
            return constructionProgress;

        return 0;
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }

    private:
        bool hasInspiredBuff()
        {
            for (auto&& itr : progressInspired)
                if (me->HasAura(itr))
                    return true;

            return false;
        }
};

struct little_patience_assaulterAI : public customCreatureAI
{
    little_patience_assaulterAI(Creature* creature) : customCreatureAI(creature)
    {
        assaultIndex = 0;
    }

    TaskScheduler scheduler;
    uint32 assaultIndex;

    void Reset() override
    {
        events.Reset();

        auto itr = invAssaultBindType.find(assaultIndex);

        std::list<Creature*> targets;
        GetCreatureListWithEntryInGrid(targets, me, itr->second[0], 300.0f);
        GetCreatureListWithEntryInGrid(targets, me, itr->second[1], 300.0f);

        for (auto&& creature : targets)
            creature->CastSpell(creature, SPELL_TERRIFIED, true);

        // Announce
        if (Creature* factionLeader = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(itr->second[0]) : ObjectGuid::Empty))
            factionLeader->AI()->Talk(TALK_INTRO);
    }

    void AttackStart(Unit* target) override
    {
        if (target && target->GetTypeId() != TYPEID_PLAYER)
            return;

        if (me->Attack(target, true))
            me->GetMotionMaster()->MoveChase(target);
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_ASSAULT_IDX)
            assaultIndex = data;
    }

    void JustDied(Unit* /*killer*/) override
    {
        auto itr = invAssaultBindType.find(assaultIndex);

        std::list<Creature*> targets;
        GetCreatureListWithEntryInGrid(targets, me, itr->second[0], 300.0f);
        GetCreatureListWithEntryInGrid(targets, me, itr->second[1], 300.0f);

        for (auto&& creature : targets)
            creature->RemoveAurasDueToSpell(SPELL_TERRIFIED);

        if (uint32 inspiredSpell = itr->second[2]) // not exist for pandarens
            DoCast(me, inspiredSpell, true);
    }
};

// Riverblade Marauder 68775
struct npc_little_patience_riverblade_marauder : public little_patience_assaulterAI
{
    npc_little_patience_riverblade_marauder(Creature* creature) : little_patience_assaulterAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_VICIOUS_REND, 2 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_VICIOUS_REND, 7.5 * IN_MILLISECONDS, EVENT_VICIOUS_REND, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Bogrot 67974
struct npc_little_patience_bogrot : public little_patience_assaulterAI
{
    npc_little_patience_bogrot(Creature* creature) : little_patience_assaulterAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_SWAMP_GAS, 5.5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_SWAMP_GAS, 12 * IN_MILLISECONDS, EVENT_SWAMP_GAS, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Swamp Gas 68816
struct npc_little_patience_swamp_gas : public customCreatureAI
{
    npc_little_patience_swamp_gas(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        DoCast(me, SPELL_SWAMP_GAS_TRIGGERING);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Wakened Mogu 68102
struct npc_little_patience_wakened_mogu : public little_patience_assaulterAI
{
    npc_little_patience_wakened_mogu(Creature* creature) : little_patience_assaulterAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_SPIRIT_BOMB, 1.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SPIRIT_BARRAGE, 8.5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_SPIRIT_BARRAGE, 10.5 * IN_MILLISECONDS, EVENT_SPIRIT_BARRAGE, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_SPIRIT_BOMB, 3 * IN_MILLISECONDS, EVENT_SPIRIT_BOMB, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Spirit Bomb 68296
struct npc_little_patience_spirit_bomb : public customCreatureAI
{
    npc_little_patience_spirit_bomb(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        DoCast(me, SPELL_SPIRIT_BOMB_TRIGGERING);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Blood Rage Fixate 134974
class spell_little_patience_blood_rage_fixate : public SpellScript
{
    PrepareSpellScript(spell_little_patience_blood_rage_fixate);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->AI()->AttackStart(target);
                caster->SetReactState(REACT_PASSIVE);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_little_patience_blood_rage_fixate::HandleEffectHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Blood Rage 134959
class spell_little_patience_blood_rage : public AuraScript
{
    PrepareAuraScript(spell_little_patience_blood_rage);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        SetStackAmount(15);
    }

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (GetStackAmount() > 1)
                SetStackAmount(GetStackAmount() - 1);
            else
                owner->RemoveAurasDueToSpell(GetSpellInfo()->Id);
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_little_patience_blood_rage::OnAuraEffectApply, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        OnProc += AuraProcFn(spell_little_patience_blood_rage::HandleOnProc);
    }
};

// Inspired Night Elf 135414
class spell_little_patience_inspired_night_elf : public SpellScript
{
    PrepareSpellScript(spell_little_patience_inspired_night_elf);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_NIGHT_ELF_LABORER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_little_patience_inspired_night_elf::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Inspired Jinyu 133461
class spell_little_patience_inspired_jinyu : public SpellScript
{
    PrepareSpellScript(spell_little_patience_inspired_jinyu);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_FALLSONG_LABORER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_little_patience_inspired_jinyu::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Inspired Gnome 133928
class spell_little_patience_inspired_gnome : public SpellScript
{
    PrepareSpellScript(spell_little_patience_inspired_gnome);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_GNOME_LABORER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_little_patience_inspired_gnome::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Inspired Dwarf 134116
class spell_little_patience_inspired_dwarf : public SpellScript
{
    PrepareSpellScript(spell_little_patience_inspired_dwarf);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_DWARF_LABORER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_little_patience_inspired_dwarf::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Construction Bar Movement 133852
class spell_little_patience_construction_bar_movement : public SpellScript
{
    PrepareSpellScript(spell_little_patience_construction_bar_movement);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_PREPARE_CONSTRUCTS) == DONE) // prevent if stage done
            return;

        std::list<Creature*> leaders;

        for (auto&& itr : leadersBarProgress)
            GetCreatureListWithEntryInGrid(leaders, GetCaster(), itr.first, 25.0f);

        leaders.remove_if([=](Creature* target) { return target && target->GetDBTableGUIDLow(); });

        if (leaders.empty())
        {
            // remove progress bars
            for (auto&& itr : leadersBarProgress)
                GetCaster()->RemoveAurasDueToSpell(itr.second);

            return;
        }

        if (Creature* factionLeader = *leaders.begin())
            GetCaster()->CastSpell(GetCaster(), leadersBarProgress.find(factionLeader->GetEntry())->second, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_little_patience_construction_bar_movement::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Faction Construction Progress 133311, 133849, 134086, 134567, 135260
class spell_little_patience_faction_construction_progress : public AuraScript
{
    PrepareAuraScript(spell_little_patience_faction_construction_progress);

    void OnUpdate(uint32 /*diff*/, AuraEffect* aurEff)
    {
        // Set current alt power
        uint32 leaderEntry = getBindLeader();

        if (!leaderEntry)
            return;

        if (Player* owner = GetOwner()->ToPlayer())
            if (Creature* leader = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(leaderEntry) : ObjectGuid::Empty))
                owner->SetPower(POWER_ALTERNATE_POWER, leader->AI()->GetData(TYPE_PROGRESS_BAR_VALUE));
    }

    private:
        uint32 getBindLeader()
        {
            for (auto&& itr : leadersBarProgress)
                if (GetSpellInfo()->Id == itr.second)
                    return itr.first;

            return 0;
        }

    void Register() override
    {
        OnEffectUpdate += AuraEffectUpdateFn(spell_little_patience_faction_construction_progress::OnUpdate, EFFECT_0, SPELL_AURA_ENABLE_ALT_POWER);
    }
};

// Terrified 133494
class spell_little_patience_terrified : public AuraScript
{
    PrepareAuraScript(spell_little_patience_terrified);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AddUnitState(UNIT_STATE_STUNNED);
    }

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->ClearUnitState(UNIT_STATE_STUNNED);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_little_patience_terrified::OnAuraEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_little_patience_terrified::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_little_patience()
{
    new creature_script<npc_little_patience_muckmaw_crocolisk>("npc_little_patience_muckmaw_crocolisk");
    new creature_script<npc_little_patience_commander_scargash>("npc_little_patience_commander_scargash");
    new creature_script<npc_little_patience_krasari_stalker>("npc_little_patience_krasari_stalker");
    new creature_script<npc_little_patience_king_varian_wrynn>("npc_little_patience_king_varian_wrynn");
    new creature_script<npc_little_patience_faction_leader>("npc_little_patience_faction_leader");
    new creature_script<npc_little_patience_riverblade_marauder>("npc_little_patience_riverblade_marauder");
    new creature_script<npc_little_patience_bogrot>("npc_little_patience_bogrot");
    new creature_script<npc_little_patience_swamp_gas>("npc_little_patience_swamp_gas");
    new creature_script<npc_little_patience_wakened_mogu>("npc_little_patience_wakened_mogu");
    new creature_script<npc_little_patience_spirit_bomb>("npc_little_patience_spirit_bomb");

    new spell_script<spell_little_patience_blood_rage_fixate>("spell_little_patience_blood_rage_fixate");
    new aura_script<spell_little_patience_blood_rage>("spell_little_patience_blood_rage");
    new spell_script<spell_little_patience_inspired_night_elf>("spell_little_patience_inspired_night_elf");
    new spell_script<spell_little_patience_inspired_jinyu>("spell_little_patience_inspired_jinyu");
    new spell_script<spell_little_patience_inspired_gnome>("spell_little_patience_inspired_gnome");
    new spell_script<spell_little_patience_inspired_dwarf>("spell_little_patience_inspired_dwarf");
    new spell_script<spell_little_patience_construction_bar_movement>("spell_little_patience_construction_bar_movement");
    new aura_script<spell_little_patience_faction_construction_progress>("spell_little_patience_faction_construction_progress");
    new aura_script<spell_little_patience_terrified>("spell_little_patience_terrified");
}