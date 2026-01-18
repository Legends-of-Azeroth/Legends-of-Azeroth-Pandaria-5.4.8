/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "MapManager.h"
#include "Spell.h"
#include "Vehicle.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CreatureTextMgr.h"
#include "Unit.h"
#include "Player.h"
#include "Creature.h"
#include "InstanceScript.h"
#include "Map.h"
#include "VehicleDefines.h"
#include "SpellInfo.h"
#include <utility>
#include "throne_of_thunder.h"

enum Yells
{
    // Boss
    ANN_RAMPAGE = 0,      // Megaera begins to [Rampage].
    ANN_SUBSIDE,          // Megaera's rage subsides.

    // Trigger Announce
    ANN_INTRO_1 = 0,
    ANN_INTRO_2,
    ANN_INTRO_3,
    ANN_BURNING_CINDERS,
    ANN_TORRENT_OF_FROST,
    ANN_RAMPAGE_ADDIT,
    ANN_RAMPAGE_DONE,
};

enum Spells
{
    // Hydra Frenzy.
    SPELL_HYDRA_FRENZY          = 139942, // Attack speed increase (others when a head dies, this + heal to full).

    // Flaming Head
    SPELL_CINDERS               = 139822, // When in Concealing Fog. Melts Icy Ground. Pool NPC summoned when removed / every 3 seconds on Heroic.
    SPELL_CINDERS_SUMMON        = 139834, // Summons NPC_CINDERS.
    SPELL_CINDERS_AURA          = 139835, // Pool npc aura (per. dmg. trigger for 139836).

    SPELL_IGNITE_FLESH          = 137729, // Triggers 137330 dmg each sec and SPELL_IGNITE_FLESH_AURA on targets.
    SPELL_IGNITE_FLESH_AURA     = 137731, // Periodic damage aura on players hit by main spell.

    // Frozen Head
    // Torrent of Ice : NPC_TORRENT_OF_ICE summoned at player location, follows him having SPELL_TORRENT_OF_ICE_NPC_A. SPELL_TORRENT_OF_ICE cast on it.
    SPELL_TORRENT_OF_ICE        = 139866, // When in Concealing Fog. Each sec triggers SPELL_TORRENT_OF_ICE_SUMMON.
    SPELL_TORRENT_OF_ICE_SUMMON = 139870, // Summons NPC_ICY_GROUND.
    SPELL_TORRENT_OF_ICE_TARGET = 139857, // Target aura ("Torrent of Ice is targeting you!").
    SPELL_TORRENT_OF_ICE_NPC_A  = 139890, // NPC aura (60k periodic damage).

    SPELL_ICY_GROUND_AURA       = 139909, // Periodic damage and slow aura on NPC_ICY_GROUND.
    SPELL_ICY_GROUND_GROW       = 140213, // 1% scale increase (Heroic only!). 1 min -> double size. Not good as-is!
    SPELL_ICY_GROUND_VISUAL     = 139875, // Triggers SPELL_ICY_GROUND_DUMMY.
    SPELL_ICY_GROUND_DUMMY      = 139877, // Check for target and add stacks (SPELL_ICY_GROUND_AURA).

    SPELL_ARCTIC_FREEZE         = 139841, // Triggers damage and SPELL_ARCTIC_FREEZE_DUMMY.
    SPELL_ARCTIC_FREEZE_DUMMY   = 139843, // Stack checker and adder - 5 stacks = SPELL_ARCTIC_FREEZE_STUN on player.
    SPELL_ARCTIC_FREEZE_STUN    = 139844, // 20 sec stun.

    // Venomous Head
    SPELL_ACID_RAIN_SUMMON      = 139845, // Summons dummy npc (remove trigger spell)
    SPELL_ACID_RAIN_VISUAL      = 139847, // Visual for dummy npc
    SPELL_ACID_RAIN_MISSILE     = 139848, // Should target dummy npc only (needs trigger spell added (DAMAGE below))
    SPELL_ACID_RAIN_DAMAGE      = 139850, // Damage, needs spellscript for range.

    SPELL_ROT_ARMOR             = 139838, // Triggers 139839 per. dmg. + 139840 damage taken debuff. 
    SPELL_ROT_ARMOR_EFF         = 139840,

    // Arcane Head - Heroic only!
    SPELL_NETHER_TEAR           = 140138,
    SPELL_NETHER_PORTAL         = 140258,

    SPELL_DIFFUSION             = 139991, // Triggers per. dmg. and SPELL_DIFFUSION_PER_DUMMY.
    SPELL_DIFFUSION_PER_DUMMY   = 139993, // Periodic dummy for 10% health redirect.
    SPELL_DIFFUSION_REDIRECT    = 139994,
    SPELL_NETHER_WYRM_SPAWN     = 140151,

    // Nether Wyrm
    SPELL_NETHER_SPIKE          = 140178,
    SPELL_SUPPRESSION           = 140179,

    // Shared | Boss
    SPELL_MEGAERAS_RAGE_FIRE    = 139758, // Flaming Head.
    SPELL_MEGAERAS_RAGE_FROST   = 139816, // Frozen Head.
    SPELL_MEGAERAS_RAGE_POISON  = 139818, // Venomous Head.
    SPELL_MEGAERAS_RAGE_ARCANE  = 139820, // Arcane Head - Heroic only!.
    SPELL_ELEMENTAL_BONDS_FIRE  = 139586, // 15% dmg increase aura for each additional Flaming Head, on each.
    SPELL_ELEMENTAL_BONDS_FROST = 139587, // 15% dmg increase aura for each additional Frozen Head, on each.
    SPELL_ELEMENTAL_BONDS_VENOM = 139588, // 15% dmg increase aura for each additional Venomous Head, on each.
    SPELL_ELEMENTAL_BONDS_ARCAN = 139589, // 15% dmg increase aura for each additional Arcane Head, on each.
    SPELL_SUBMERGE              = 103963, // Submerge visual (when casted makes head submerge).
    SPELL_EMERGE                = 139832, // Submerged visual (when removed makes head emerge).
    SPELL_CONCEALING_FOG        = 137973, // Dummy on eff 0 for attackable state (body + back head).
    SPELL_RAMPAGE_FIRE_CAST     = 139433,
    SPELL_RAMPAGE_FROST_CAST    = 139440,
    SPELL_RAMPAGE_POISON_CAST   = 139504,
    SPELL_RAMPAGE_ARCANE_CAST   = 139513,
                                
    SPELL_RAMPAGE_FIRE          = 139548, // Flaming Head.
    SPELL_RAMPAGE_FROST         = 139549, // Frozen Head.
    SPELL_RAMPAGE_POISON        = 139551, // Venomous Head.
    SPELL_RAMPAGE_ARCANE        = 139552, // Arcane Head - Heroic only!.

    SPELL_MEGAERA_KILL_CREDIT   = 140353,

    // Rampage - When a head dies.
    SPELL_RAMPAGE = 139458, // Periodic dummy on eff 0 for damage increase on the specific spells (+25% for each extra available head of type).
};

enum Actions
{
    ACTION_BERSERK,
    ACTION_HEAD_DEFEATED,
    ACTION_MEGERA_RAGE,
    ACTION_REMOVE_FRAMES,
    ACTION_ACTIVITY_SUBMERGE,
    ACTION_SET_IN_COMBAT,
    ACTION_DELAY_EVENTS,
    ACTION_RESPAWN,
    ACTION_EVADE,
    ACTION_DELAY,
};

enum Events
{
    // Boss
    EVENT_RAMPAGE               = 1,
    EVENT_MEGAERAS_RAGE,

    EVENT_SUBMERGE,
    EVENT_EMERGE,
    EVENT_BERSERK,

    EVENT_RELOCATE_HEAD,
    EVENT_REEMERGE_HEAD,
    EVENT_SUBMERGE_HEAD,
    EVENT_START_HEADS,

    // Heads - General
    EVENT_CHECK_MEGAERAS_RAGE,

    // Flaming Head
    EVENT_CINDERS,
    EVENT_IGNITE_FLESH,

    // Frozen Head
    EVENT_TORRENT_OF_ICE,
    EVENT_ARCTIC_FREEZE,

    // Venomous Head
    EVENT_ACID_RAIN,
    EVENT_ROT_ARMOR,

    // Arcane Head - Heroic only!
    EVENT_NETHER_TEAR,
    EVENT_DIFFUSION,

    // Icy Ground
    EVENT_CHECK_ICY_GROUND_AND_CINDERS,
    EVENT_ICY_GROUND_GROW,
    EVENT_ICY_GROUND_INIT,

    // Nether Wyrm - From Arcane Head - Heroic only! 
    EVENT_NETHER_SPIKE,
    EVENT_SUPPRESSION
};

enum Timers
{
    // Boss
    TIMER_BERSERK_H             = 600000, // 10 minutes (Heroic).
    TIMER_BERSERK               = 780000  // 13 minutes.
};

enum HeadPositions
{
    VENOMOUS_HEAD,
    FROZEN_HEAD,
    FLAMING_HEAD,
    ARCANE_HEAD
};

const std::map<uint32, Position> invHeadCombatType =
{
    { NPC_FLAMING_HEAD,  { 6437.47f, 4535.20f, -210.62f, 2.62f } },
    { NPC_FROZEN_HEAD,   { 6417.99f, 4507.41f, -210.58f, 2.18f } },
    { NPC_ARCANE_HEAD,   { 6437.30f, 4565.74f, -210.57f, 3.34f } },
    { NPC_VENOMOUS_HEAD, { 6394.10f, 4497.44f, -210.24f, 1.56f } },
};

const std::map<uint32, uint32> invRampageType =
{
    { NPC_FLAMING_HEAD,  SPELL_RAMPAGE_FIRE_CAST   },
    { NPC_FROZEN_HEAD,   SPELL_RAMPAGE_FROST_CAST  },
    { NPC_ARCANE_HEAD,   SPELL_RAMPAGE_ARCANE_CAST },
    { NPC_VENOMOUS_HEAD, SPELL_RAMPAGE_POISON_CAST },
};

const std::map<uint32, uint32> invElementalType =
{
    { NPC_FLAMING_HEAD,  SPELL_ELEMENTAL_BONDS_FIRE },
    { NPC_FROZEN_HEAD,   SPELL_ELEMENTAL_BONDS_FROST },
    { NPC_ARCANE_HEAD,   SPELL_ELEMENTAL_BONDS_ARCAN },
    { NPC_VENOMOUS_HEAD, SPELL_ELEMENTAL_BONDS_VENOM },
};

const std::map<uint32, uint32> invEmpowerElementalType =
{
    { SPELL_RAMPAGE_FIRE,   NPC_FLAMING_HEAD  },
    { SPELL_RAMPAGE_FROST,  NPC_FROZEN_HEAD   },
    { SPELL_RAMPAGE_POISON, NPC_VENOMOUS_HEAD },
    { SPELL_RAMPAGE_ARCANE, NPC_ARCANE_HEAD   },
};

// Megaera 68065.
class boss_megaera : public CreatureScript
{
    public:
        boss_megaera() : CreatureScript("boss_megaera") { }

        struct boss_megaeraAI : public BossAI
        {
            boss_megaeraAI(Creature* creature) : BossAI(creature, DATA_MEGAERA) 
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetDisplayFromModel(1);

                // delay for init 
                me->m_Events.Schedule(1500, [this]()
                {
                    if (instance && instance->GetData(DATA_MEGERA_EVENT) == DONE)
                        DoAction(ACTION_MEGAERA_EMERGE);
                });
            }

            EventMap RPevents;
            uint8 headKills;
            uint8 rampageCount;
            bool isRaging;
            bool isRampaging;
            bool atEvade;
            std::list<uint64> headGuids;
            uint32 remainingHead;
            uint32 killedHeads[4];
            ObjectGuid movingHeadGuid;
            ObjectGuid activeHeadGuid;
            uint32 fireBonds;
            uint32 frostBonds;
            uint32 poisonBonds;
            uint32 arcaneBonds;
            std::list<uint32> activityHead, inActivityHead;

            void Reset() override
            {
                _Reset();
                scheduler.CancelAll();
                events.Reset();
                summons.DespawnAll();
                activityHead.clear();
                inActivityHead.clear();

                activityHead.push_back(NPC_FROZEN_HEAD);
                activityHead.push_back(NPC_VENOMOUS_HEAD);
                inActivityHead.push_back(NPC_FLAMING_HEAD);

                if (IsHeroic())
                    inActivityHead.push_back(NPC_ARCANE_HEAD);

                // Set Head kills count (at 7 encounter ends), rampage damage count to 0 (20 max) and Megaera's Rage scheduled to false (check used in head scripts for melee range).
                headKills    = 0;
                rampageCount = 0;
                fireBonds    = 0;
                frostBonds   = 0;
                poisonBonds  = 0;
                arcaneBonds  = 0;
                isRaging     = false;
                isRampaging  = false;
                atEvade      = false;

                // Main boss body doesn't move.
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

                me->AddAura(SPELL_CONCEALING_FOG, me);
                me->SetReactState(REACT_DEFENSIVE);

                if (instance)
                    instance->SetBossState(DATA_MEGAERA, NOT_STARTED);

                me->GetMap()->SetWorldState(WORLDSTATE_HEAD_CASE_DEFAULT, 1); // by default
                
                for (auto&& itr : invCaseHeadType)
                    me->GetMap()->SetWorldState(itr.second, 1);

                _Reset();

                RPevents.ScheduleEvent(EVENT_START_HEADS, 5000);
            }

            // Used to despawn all summons having a specific entry.
            void DespawnSummon(uint32 entry)
            {
                std::list<Creature*> summonsList;
                GetCreatureListWithEntryInGrid(summonsList, me, entry, 200.0f);
                if (!summonsList.empty())
                    for (std::list<Creature*>::iterator summs = summonsList.begin(); summs != summonsList.end(); summs++)
                        if ((*summs)->IsAlive())
                            (*summs)->DespawnOrUnsummon();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case NPC_FLAMING_HEAD:
                        return fireBonds;
                    case NPC_FROZEN_HEAD:
                        return frostBonds;
                    case NPC_VENOMOUS_HEAD:
                        return poisonBonds;
                    case NPC_ARCANE_HEAD:
                        return arcaneBonds;
                }

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case NPC_FLAMING_HEAD:
                        fireBonds++;
                        break;
                    case NPC_FROZEN_HEAD:
                        frostBonds++;
                        break;
                    case NPC_VENOMOUS_HEAD:
                        poisonBonds++;
                        break;
                    case NPC_ARCANE_HEAD:
                        arcaneBonds++;
                        break;
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                // Just Berserk scheduled here, the other events are handled by the specific heads / through happenings (ex. a head dies -> Rampage, etc).
                events.ScheduleEvent(EVENT_BERSERK, (IsHeroic() ? TIMER_BERSERK_H : TIMER_BERSERK));

                // Put all heads in combat and send the frames for the front ones.
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                    if (Creature* firstHead = me->FindNearestCreature(NPC_FROZEN_HEAD, 100.0f, true))
                    {
                        firstHead->SetReactState(REACT_AGGRESSIVE);
                        firstHead->AI()->DoZoneInCombat(firstHead, 150.0f);
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, firstHead);
                    }
                    if (Creature* secondHead = me->FindNearestCreature(NPC_VENOMOUS_HEAD, 100.0f, true))
                    {
                        secondHead->SetReactState(REACT_AGGRESSIVE);
                        secondHead->AI()->DoZoneInCombat(secondHead, 150.0f);
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, secondHead);
                    }
                    if (Creature* concealingHead = me->FindNearestCreature(IsHeroic() ? NPC_ARCANE_HEAD : NPC_FLAMING_HEAD, 100.0f, true))
                    {
                        concealingHead->SetReactState(REACT_AGGRESSIVE);
                        concealingHead->AI()->DoZoneInCombat(concealingHead, 150.0f);
                    }

                    instance->SetBossState(DATA_MEGAERA, IN_PROGRESS);
                }

                _JustEngagedWith();

                scheduler
                    .Schedule(Milliseconds(IsHeroic() ? TIMER_BERSERK_H : TIMER_BERSERK), [this](TaskContext context)
                {
                    DoCast(me, SPELL_BERSERK);
                    SendHeadsAction(ACTION_BERSERK);
                });

                scheduler
                    .Schedule(Milliseconds(1000), [this](TaskContext context)
                {
                    if (instance && instance->IsWipe(170.0f, me))
                        EnterEvadeMode();

                    context.Repeat(Milliseconds(1000));
                });
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_SET_IN_COMBAT:
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoZoneInCombat(me, 150.0f);
                        break;
                    case ACTION_MEGAERA_EMERGE:
                        if (Creature* firstHead = me->FindNearestCreature(NPC_FROZEN_HEAD, 100.0f, true))
                        {
                            firstHead->RemoveAurasDueToSpell(SPELL_EMERGE);
                            firstHead->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
                        }

                        if (Creature* secondHead = me->FindNearestCreature(NPC_VENOMOUS_HEAD, 100.0f, true))
                        {
                            secondHead->RemoveAurasDueToSpell(SPELL_EMERGE);
                            secondHead->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
                        }

                        if (Creature* thirdHead = me->FindNearestCreature(NPC_FLAMING_HEAD, 100.0f, true))
                        {
                            thirdHead->RemoveAurasDueToSpell(SPELL_EMERGE);
                            thirdHead->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
                        }

                        if (IsHeroic())
                        {
                            if (Creature* fourHead = me->FindNearestCreature(NPC_ARCANE_HEAD, 100.0f, true))
                            {
                                fourHead->RemoveAurasDueToSpell(SPELL_EMERGE);
                                fourHead->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
                            }
                        }
                        break;
                    case NPC_FLAMING_HEAD:
                    case NPC_FROZEN_HEAD:
                    case NPC_VENOMOUS_HEAD:
                    case NPC_ARCANE_HEAD:
                        // Increase killed heads count.
                        if (++headKills >= 7)
                        {
                            me->Kill(me);
                            break;
                        }

                        // Set defeated head into inactivity list
                        activityHead.erase(std::find(activityHead.begin(), activityHead.end(), actionId));
                        inActivityHead.push_back(actionId);

                        // Take first inactivity head from list and pull in combat
                        if (Creature* selectedHead = me->FindNearestCreature(inActivityHead.front(), 200.0f, true))
                        {
                            activityHead.push_back(selectedHead->GetEntry());
                            selectedHead->AI()->DoAction(ACTION_HEAD_DEFEATED);
                            inActivityHead.pop_front();
                        }

                        // Activity head too should`ve submerge after another death
                        SendHeadsAction(ACTION_ACTIVITY_SUBMERGE);

                        // Cast Elemental Bonds on self too for future heads
                        SetData(actionId, 0);

                        // Cast Elemental Bonds on same type of head
                        std::list<Creature*> headsList;
                        GetCreatureListWithEntryInGrid(headsList, me, actionId, 200.0f);

                        for (auto&& itr : headsList)
                            itr->CastSpell(itr, invElementalType.find(actionId)->second, true);

                        // Spawn Twice defeated Head
                        SendSpawnDefeatedHeads(actionId);

                        // Remove 15% health each time a head is killed.
                        me->SetHealth(me->GetMaxHealth() - ((0.15 * me->GetMaxHealth()) * headKills));

                        // Megera Rampage 
                        scheduler
                            .Schedule(Milliseconds(4500), [this](TaskContext context)
                        {
                            if (Creature* announcer = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SLG_GENERIC_MOP) : ObjectGuid::Empty))
                                announcer->AI()->Talk(ANN_RAMPAGE_ADDIT);

                            DoCast(me, SPELL_RAMPAGE, true);

                            SendHeadsAction(ACTION_MEGERA_RAGE);
                        });

                        // Rampage Done
                        scheduler
                            .Schedule(Milliseconds(24500), [this](TaskContext context)
                        {
                            me->RemoveAurasDueToSpell(SPELL_RAMPAGE);

                            if (Creature* announcer = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SLG_GENERIC_MOP) : ObjectGuid::Empty))
                                announcer->AI()->Talk(ANN_RAMPAGE_DONE);
                        });
                        break;
                }
            };

            void EnterEvadeMode() override
            {
                if (atEvade)
                    return;

                atEvade = true;

                if (instance)
                {
                    instance->SetBossState(DATA_MEGAERA, FAIL);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TORRENT_OF_ICE_TARGET);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CINDERS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARCTIC_FREEZE_STUN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ROT_ARMOR_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARCTIC_FREEZE_DUMMY);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_IGNITE_FLESH_AURA);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
                }

                BossAI::EnterEvadeMode();
                _EnterEvadeMode();
                HandleResetPlace();

                DespawnSummon(NPC_CINDERS);
                DespawnSummon(NPC_TORRENT_OF_ICE);
                DespawnSummon(NPC_ICY_GROUND);
                DespawnSummon(NPC_NETHER_WYRM);

                SendHeadsAction(ACTION_REMOVE_FRAMES);
                SendHeadsAction(ACTION_EVADE);

                _DespawnAtEvade();
                Reset();
            }

            void JustReachedHome() override
            {
                me->ClearUnitState(UNIT_STATE_EVADE);

                // This should take care of everything needed.
                Reset();

                _JustReachedHome();
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->setActive(true);

                // The heads cannot move.
                if (summon->GetEntry() == NPC_FLAMING_HEAD || summon->GetEntry() == NPC_FROZEN_HEAD || summon->GetEntry() == NPC_VENOMOUS_HEAD || summon->GetEntry() == NPC_ARCANE_HEAD)
                {
                    headGuids.push_back(summon->GetGUID());
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

                    // If controller has aura by our type then set this our on self with stack count
                    if (Aura* aura = summon->AddAura(invElementalType.find(summon->GetEntry())->second, summon))
                        if (GetData(summon->GetEntry()) > 1)
                            aura->SetStackAmount(GetData(summon->GetEntry()));
                }

                // Take care of putting the newly - spawned heads to fight while in combat.
                if (me->IsInCombat())
                    summon->SetInCombatWithZone();
            }

            void JustDied(Unit* killer) override
            {
                DespawnSummon(NPC_CINDERS);
                DespawnSummon(NPC_TORRENT_OF_ICE);
                DespawnSummon(NPC_ICY_GROUND);
                DespawnSummon(NPC_NETHER_WYRM);

                SendHeadsAction(ACTION_REMOVE_FRAMES);
                HandleResetPlace();

                _JustDied();

                DoCastAOE(SPELL_MEGAERA_KILL_CREDIT, true);

                if (Player* player = me->GetMap()->GetFirstPlayerInInstance())
                    player->RewardPersonalLootAndCurrency(me);

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TORRENT_OF_ICE_TARGET);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CINDERS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARCTIC_FREEZE_STUN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ROT_ARMOR_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARCTIC_FREEZE_DUMMY);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_IGNITE_FLESH_AURA);

                    // We Should Check instance mod
                    instance->UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, NPC_MEGAERA, me);

                    if (GameObject* go = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(IsHeroic() ? GO_MEGAERA_CHEST_HEROIC : GO_MEGAERA_CHEST_NORMAL)))
                        instance->DoRespawnGameObject(go->GetGUID(), 7 * DAY);
                }

                for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                {
                    if (Unit* pHead = ObjectAccessor::GetCreature(*me, *itr))
                        me->Kill(pHead);
                }

                // kill not summoned heads too
                if (Creature* firstHead = me->FindNearestCreature(NPC_FROZEN_HEAD, 100.0f, true))
                    me->Kill(firstHead);

                if (Creature* secondHead = me->FindNearestCreature(NPC_VENOMOUS_HEAD, 100.0f, true))
                    me->Kill(secondHead);

                if (Creature* thirdHead = me->FindNearestCreature(NPC_FLAMING_HEAD, 100.0f, true))
                    me->Kill(thirdHead);

                if (IsHeroic())
                {
                    if (Creature* fourHead = me->FindNearestCreature(NPC_ARCANE_HEAD, 100.0f, true))
                        me->Kill(fourHead);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);
            }

            private:
                void SendHeadsAction(int32 actionId)
                {
                    std::list<Creature*> megaeraHeads;
                    GetCreatureListWithEntryInGrid(megaeraHeads, me, NPC_FLAMING_HEAD, 200.0f);
                    GetCreatureListWithEntryInGrid(megaeraHeads, me, NPC_FROZEN_HEAD, 200.0f);
                    GetCreatureListWithEntryInGrid(megaeraHeads, me, NPC_VENOMOUS_HEAD, 200.0f);
                    GetCreatureListWithEntryInGrid(megaeraHeads, me, NPC_ARCANE_HEAD, 200.0f);

                    for (auto&& itr : megaeraHeads)
                        itr->AI()->DoAction(actionId);
                }

                void SendSpawnDefeatedHeads(uint32 defeatEntry)
                {
                    std::list<Creature*> spawnerList;
                    GetCreatureListWithEntryInGrid(spawnerList, me, NPC_BACK_HEAD_SPAWNER, 200.0f);
                    spawnerList.remove_if([=](Creature* target) { return target->AI() && target->AI()->GetData(TYPE_HEAD_SPAWNED); });

                    if (spawnerList.size() > 2)
                        Trinity::Containers::RandomResizeList(spawnerList, 2);

                    for (auto&& itr : spawnerList)
                    {
                        itr->AI()->SetData(TYPE_HEAD_SPAWNED, 1);
                        itr->SummonCreature(defeatEntry, *itr);
                    }
                }

                void HandleResetPlace()
                {
                    std::list<Creature*> spawnerList;
                    GetCreatureListWithEntryInGrid(spawnerList, me, NPC_BACK_HEAD_SPAWNER, 200.0f);

                    for (auto&& itr : spawnerList)
                        itr->AI()->Reset();
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_megaeraAI(creature);
        }
};

struct megaeraHeadsBaseAI : public ScriptedAI
{
    megaeraHeadsBaseAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

    EventMap events;
    TaskScheduler scheduler;
    InstanceScript* instance;
    ObjectGuid summonerGUID;
    bool inRampage;
    bool atEvade;

    void InitializeAI() override
    {
        me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);

        if (!me->GetDBTableGUIDLow())
            return;

        if (!me->IsAlive() && instance && instance->GetBossState(DATA_MEGAERA) != DONE)
            me->Respawn();

        inRampage = false;
        atEvade   = false;
        events.Reset();
        me->NearTeleportTo(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ(), me->GetHomePosition().GetOrientation());

        DoCast(me, SPELL_EMERGE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
    }

    void Reset() override
    {
        inRampage = false;
        atEvade   = false;
        events.Reset();
        me->RemoveAurasDueToSpell(SPELL_EMERGE);
        me->NearTeleportTo(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ(), me->GetHomePosition().GetOrientation());

        scheduler
            .Schedule(Seconds(2), [this](TaskContext context)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
        });
    }

    void IsSummonedBy(Unit* summoner) override
    {
        me->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);

        summonerGUID = summoner->GetGUID();

        // Set Facing to this trigger
        if (Creature* genericBunny = me->FindNearestCreature(NPC_SLG_GENERIC_MOP, 200.0f, true))
            me->SetFacingToObject(genericBunny);

        DoCast(me, SPELL_CONCEALING_FOG, true);

        if (Creature* megaeraController = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_MEGAERA) : ObjectGuid::Empty))
            megaeraController->AI()->JustSummoned(me);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        // While Rampage is Active - Head couldn`t die
        if (damage >= me->GetHealth() && inRampage)
            damage = 0;
    }

    void EnterEvadeMode() override
    {
        if (!me->GetDBTableGUIDLow() || atEvade)
            return;

        atEvade = true;

        if (!me->IsAlive())
            me->Respawn();

        me->SetVisible(true);
        me->RemoveAllAuras();
        me->DeleteThreatList();
        me->CombatStop(true);

        if (Creature* megaera = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_MEGAERA) : ObjectGuid::Empty))
            megaera->AI()->EnterEvadeMode();

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);

        uint32 corpseDelay = me->GetCorpseDelay();
        uint32 respawnDelay = me->GetRespawnDelay();

        me->SetCorpseDelay(1);
        me->SetRespawnDelay(29);

        me->DespawnOrUnsummon();

        me->SetCorpseDelay(corpseDelay);
        me->SetRespawnDelay(respawnDelay);

        Reset();
        me->GetMap()->CreatureRelocation(me, me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ(), me->GetHomePosition().GetOrientation());
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_HEAD_DEFEATED:
                me->HandleEmoteCommand(EMOTE_ONESHOT_SUBMERGE);

                // Emplace our seed
                if (Creature* spawner = ObjectAccessor::GetCreature(*me, summonerGUID))
                    spawner->AI()->SetData(TYPE_HEAD_SPAWNED, 0);

                scheduler
                    .Schedule(Milliseconds(1200), [this](TaskContext context)
                {
                    DoCast(me, SPELL_EMERGE);
                    me->NearTeleportTo(invHeadCombatType.find(me->GetEntry())->second.GetPositionX(), invHeadCombatType.find(me->GetEntry())->second.GetPositionY(), invHeadCombatType.find(me->GetEntry())->second.GetPositionZ(), invHeadCombatType.find(me->GetEntry())->second.GetOrientation());
                });
            
                scheduler
                    .Schedule(Milliseconds(2700), [this](TaskContext context)
                {
                    me->RemoveAurasDueToSpell(SPELL_EMERGE);
                    me->RemoveAurasDueToSpell(SPELL_CONCEALING_FOG);

                    if (instance)
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me); // Remove
                });
                break;
            case ACTION_BERSERK:
                DoCast(me, SPELL_BERSERK);
                break;
            case ACTION_REMOVE_FRAMES:
                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
                break;
            case ACTION_MEGERA_RAGE:
                events.ScheduleEvent(EVENT_MEGAERAS_RAGE, urand(0.5 * IN_MILLISECONDS, 1.5 * IN_MILLISECONDS));
                scheduler
                    .Schedule(Milliseconds(20000), [this](TaskContext context)
                {
                    inRampage = false;
                });
                break;
            case ACTION_ACTIVITY_SUBMERGE:
                inRampage = true;
                events.DelayEvents(30.5 * IN_MILLISECONDS);

                if (me->HasAura(SPELL_CONCEALING_FOG))
                    break;

                DoCast(me, SPELL_HYDRA_FRENZY, true);

                me->HandleEmoteCommand(EMOTE_ONESHOT_SUBMERGE);

                scheduler
                    .Schedule(Milliseconds(1200), [this](TaskContext context)
                {
                    DoCast(me, SPELL_EMERGE);
                });

                scheduler
                    .Schedule(Milliseconds(2700), [this](TaskContext context)
                {
                    me->RemoveAurasDueToSpell(SPELL_EMERGE);
                });

                break;
            case ACTION_EVADE:
                EnterEvadeMode();
                break;
            case ACTION_DELAY:
                if (me->GetEntry() == NPC_FROZEN_HEAD)
                    events.DelayEvents(11 * IN_MILLISECONDS);
                else
                    events.DelayEvents(urand(3 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                break;

        }
    }

    void HandleSendDelay()
    {
        std::list<Creature*> headTypeList;
        GetCreatureListWithEntryInGrid(headTypeList, me, me->GetEntry(), 200.0f);
        headTypeList.remove_if([=](Creature* target) { return target && !target->HasAura(SPELL_CONCEALING_FOG); });

        for (auto&& itr : headTypeList)
            if (itr->GetGUID() != me->GetGUID())
                itr->AI()->DoAction(ACTION_DELAY);
    }

    void JustDied(Unit* killer) override
    {
        if (Creature* megaeraController = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_MEGAERA) : ObjectGuid::Empty))
            megaeraController->AI()->DoAction(me->GetEntry());

        if (instance)
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove

        // doesn`t know what is happened, but hide them
        me->SetVisible(false);

        if (me->GetEntry() == NPC_ARCANE_HEAD)
            return;

        me->GetMap()->SetWorldState(invCaseHeadType.find(me->GetEntry())->second, 0);
    }
};

// Flaming Head 70212.
class npc_flaming_head_megaera : public CreatureScript
{
    public:
        npc_flaming_head_megaera() : CreatureScript("npc_flaming_head_megaera") { }

        struct npc_flaming_head_megaeraAI : public megaeraHeadsBaseAI
        {
            npc_flaming_head_megaeraAI(Creature* creature) : megaeraHeadsBaseAI(creature) { }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (me->GetDBTableGUIDLow())
                    DoCast(me, SPELL_CONCEALING_FOG, true);

                events.ScheduleEvent(EVENT_CHECK_MEGAERAS_RAGE, 18000);
                events.ScheduleEvent(EVENT_CINDERS, me->GetDBTableGUIDLow() ? urand(10000, 15000) : urand(32000, 43000));
                events.ScheduleEvent(EVENT_IGNITE_FLESH, urand(12000, 17000));

                if (Creature* megaeraController = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_MEGAERA) : ObjectGuid::Empty))
                    if (!megaeraController->IsInCombat())
                        megaeraController->AI()->DoAction(ACTION_SET_IN_COMBAT);
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(SPELL_EMERGE))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_MEGAERAS_RAGE:
                            // Must be one of the heads in front.
                            if (!me->HasAura(SPELL_CONCEALING_FOG) && !inRampage)
                            {
                                // Find boss and check for melee distance to victim + unscheduled boss action.
                                if (Unit* vict = me->GetVictim())
                                    if (!me->IsWithinMeleeRange(vict))
                                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                            DoCast(target, SPELL_MEGAERAS_RAGE_FIRE);
                            }
                            events.ScheduleEvent(EVENT_CHECK_MEGAERAS_RAGE, 4000);
                            break;

                        case EVENT_CINDERS:
                            if (me->HasAura(SPELL_CONCEALING_FOG) && !me->HasAura(SPELL_EMERGE) && !inRampage)
                                DoCast(me, SPELL_CINDERS);

                            HandleSendDelay();

                            events.ScheduleEvent(EVENT_CINDERS, (IsHeroic() ? urand(54000, 60000) : urand(23000, 29000)));
                            break;

                        case EVENT_IGNITE_FLESH:
                            if (!me->HasAura(SPELL_CONCEALING_FOG) && !me->HasAura(SPELL_EMERGE) && !inRampage)
                            {
                                if (Unit* target = me->GetVictim())
                                {
                                    me->SetFacingToObject(me->GetVictim());
                                    DoCast(target, SPELL_IGNITE_FLESH);
                                }
                            }

                            events.ScheduleEvent(EVENT_IGNITE_FLESH, urand(16000, 18000));
                            break;
                        case EVENT_MEGAERAS_RAGE:
                            if (!inRampage)
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                DoCast(target, invRampageType.find(me->GetEntry())->second, true);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                DoCast(target, invRampageType.find(me->GetEntry())->second, true);

                            events.ScheduleEvent(EVENT_MEGAERAS_RAGE, urand(2 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                            break;
                    }
                }

                // Can't melee from the fog.
                if (!me->HasAura(SPELL_CONCEALING_FOG) && !me->HasAura(SPELL_EMERGE) && !inRampage)
                    DoMeleeAttackIfReady(true);

            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_flaming_head_megaeraAI(creature);
        }
};

// Frozen Head 70235.
class npc_frozen_head_megaera : public CreatureScript
{
    public:
        npc_frozen_head_megaera() : CreatureScript("npc_frozen_head_megaera") { }

        struct npc_frozen_head_megaeraAI : public megaeraHeadsBaseAI
        {
            npc_frozen_head_megaeraAI(Creature* creature) : megaeraHeadsBaseAI(creature) { }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CHECK_MEGAERAS_RAGE, 18900);
                events.ScheduleEvent(EVENT_TORRENT_OF_ICE, me->GetDBTableGUIDLow() ? urand(14000, 23000) : urand(32000, 43000));
                events.ScheduleEvent(EVENT_ARCTIC_FREEZE, urand(7000, 12000));

                if (Creature* megaeraController = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_MEGAERA) : ObjectGuid::Empty))
                    if (!megaeraController->IsInCombat())
                        megaeraController->AI()->DoAction(ACTION_SET_IN_COMBAT);
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(SPELL_EMERGE))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_MEGAERAS_RAGE:
                            // Must be one of the heads in front.
                            if (!me->HasAura(SPELL_CONCEALING_FOG) && !inRampage)
                            {
                                // Find boss and check for melee distance to victim + unscheduled boss action.
                                if (Unit* vict = me->GetVictim())
                                    if (!me->IsWithinMeleeRange(vict))
                                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                            DoCast(target, SPELL_MEGAERAS_RAGE_FROST);
                            }
                            events.ScheduleEvent(EVENT_CHECK_MEGAERAS_RAGE, 4000);
                            break;
                        case EVENT_TORRENT_OF_ICE:
                            if (me->HasAura(SPELL_CONCEALING_FOG) && !me->HasAura(SPELL_EMERGE) && !inRampage)
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector(SPELL_TORRENT_OF_ICE_TARGET)))
                                {
                                    me->AddAura(SPELL_TORRENT_OF_ICE_TARGET, target);

                                    if (Creature* announcer = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SLG_GENERIC_MOP) : ObjectGuid::Empty))
                                        announcer->AI()->Talk(ANN_TORRENT_OF_FROST, target);

                                    if (Creature* torrent = me->SummonCreature(NPC_TORRENT_OF_ICE, *target, TEMPSUMMON_TIMED_DESPAWN, 180000))
                                        torrent->AI()->SetGUID(target->GetGUID());
                                }
                                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector(SPELL_TORRENT_OF_ICE_TARGET)))
                                {
                                    me->AddAura(SPELL_TORRENT_OF_ICE_TARGET, target);

                                    if (Creature* announcer = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SLG_GENERIC_MOP) : ObjectGuid::Empty))
                                        announcer->AI()->Talk(ANN_TORRENT_OF_FROST, target);

                                    if (Creature* torrent = me->SummonCreature(NPC_TORRENT_OF_ICE, *target, TEMPSUMMON_TIMED_DESPAWN, 180000))
                                        torrent->AI()->SetGUID(target->GetGUID());
                                }

                                HandleSendDelay();
                            }

                            events.ScheduleEvent(EVENT_TORRENT_OF_ICE, urand(26000, 45000));
                            break;
                        case EVENT_ARCTIC_FREEZE:
                            if (!me->HasAura(SPELL_CONCEALING_FOG) && !me->HasAura(SPELL_EMERGE) && !inRampage)
                            {
                                me->SetFacingToObject(me->GetVictim());

                                if (Unit* target = me->GetVictim())
                                    DoCast(target, SPELL_ARCTIC_FREEZE);
                            }
                            events.ScheduleEvent(EVENT_ARCTIC_FREEZE, urand(16000, 18000));
                            break;
                        case EVENT_MEGAERAS_RAGE:
                            if (!inRampage)
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                DoCast(target, invRampageType.find(me->GetEntry())->second, true);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                DoCast(target, invRampageType.find(me->GetEntry())->second, true);

                            events.ScheduleEvent(EVENT_MEGAERAS_RAGE, urand(2 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                            break;
                    }
                }

                // Can't melee from the fog.
                if (!me->HasAura(SPELL_CONCEALING_FOG) && !me->HasAura(SPELL_EMERGE) && !inRampage)
                    DoMeleeAttackIfReady(true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_frozen_head_megaeraAI(creature);
        }
};

// Venomous Head 70247.
class npc_venomous_head_megaera : public CreatureScript
{
    public:
        npc_venomous_head_megaera() : CreatureScript("npc_venomous_head_megaera") { }

        struct npc_venomous_head_megaeraAI : public megaeraHeadsBaseAI
        {
            npc_venomous_head_megaeraAI(Creature* creature) : megaeraHeadsBaseAI(creature) { }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CHECK_MEGAERAS_RAGE, 19800);
                events.ScheduleEvent(EVENT_ACID_RAIN, me->GetDBTableGUIDLow() ? urand(12000, 17000) : urand(32000, 43000));
                events.ScheduleEvent(EVENT_ROT_ARMOR, urand(10000, 15000));

                if (Creature* megaeraController = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_MEGAERA) : ObjectGuid::Empty))
                    if (!megaeraController->IsInCombat())
                        megaeraController->AI()->DoAction(ACTION_SET_IN_COMBAT);
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(SPELL_EMERGE))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_MEGAERAS_RAGE:
                            // Must be one of the heads in front.
                            if (!me->HasAura(SPELL_CONCEALING_FOG) && !inRampage)
                            {
                                // Find boss and check for melee distance to victim + unscheduled boss action.
                                if (Unit* vict = me->GetVictim())
                                    if (!me->IsWithinMeleeRange(vict))
                                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                            DoCast(target, SPELL_MEGAERAS_RAGE_POISON);
                            }
                            events.ScheduleEvent(EVENT_CHECK_MEGAERAS_RAGE, 4000);
                            break;
                        case EVENT_ACID_RAIN:
                            if (me->HasAura(SPELL_CONCEALING_FOG) && !me->HasAura(SPELL_EMERGE) && !inRampage)
                                DoCast(me, SPELL_ACID_RAIN_SUMMON);

                            HandleSendDelay();

                            events.ScheduleEvent(EVENT_ACID_RAIN, (IsHeroic() ? urand(54000, 60000) : urand(23000, 29000)));
                            break;
                        case EVENT_ROT_ARMOR:
                            if (!me->HasAura(SPELL_CONCEALING_FOG) && !me->HasAura(SPELL_EMERGE) && !inRampage)
                            {
                                me->SetFacingToObject(me->GetVictim());

                                if (Unit* target = me->GetVictim())
                                    DoCast(target, SPELL_ROT_ARMOR);
                            }

                            events.ScheduleEvent(EVENT_ROT_ARMOR, urand(16000, 18000));
                            break;
                        case EVENT_MEGAERAS_RAGE:
                            if (!inRampage)
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                DoCast(target, invRampageType.find(me->GetEntry())->second, true);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                DoCast(target, invRampageType.find(me->GetEntry())->second, true);

                            events.ScheduleEvent(EVENT_MEGAERAS_RAGE, urand(2 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                            break;
                    }
                }

                // Can't melee from the fog.
                if (!me->HasAura(SPELL_CONCEALING_FOG) && !me->HasAura(SPELL_EMERGE) && !inRampage)
                    DoMeleeAttackIfReady(true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_venomous_head_megaeraAI(creature);
        }
};

// Arcane Head 70252 - Heroic only!
class npc_arcane_head_megaera : public CreatureScript
{
    public:
        npc_arcane_head_megaera() : CreatureScript("npc_arcane_head_megaera") { }

        struct npc_arcane_head_megaeraAI : public megaeraHeadsBaseAI
        {
            npc_arcane_head_megaeraAI(Creature* creature) : megaeraHeadsBaseAI(creature) { }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (me->GetDBTableGUIDLow())
                    DoCast(me, SPELL_CONCEALING_FOG, true);

                events.ScheduleEvent(EVENT_CHECK_MEGAERAS_RAGE, 20600);
                events.ScheduleEvent(EVENT_NETHER_TEAR, me->GetDBTableGUIDLow() ? urand(12000, 17000) : urand(32000, 43000));
                events.ScheduleEvent(EVENT_DIFFUSION, urand(7000, 12000));

                if (Creature* megaeraController = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_MEGAERA) : ObjectGuid::Empty))
                    if (!megaeraController->IsInCombat())
                        megaeraController->AI()->DoAction(ACTION_SET_IN_COMBAT);
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(SPELL_EMERGE))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_MEGAERAS_RAGE:
                            // Must be one of the heads in front.
                            if (!me->HasAura(SPELL_CONCEALING_FOG) && !inRampage)
                            {
                                // Find boss and check for melee distance to victim + unscheduled boss action.
                                if (Unit* vict = me->GetVictim())
                                    if (!me->IsWithinMeleeRange(vict))
                                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                            DoCast(target, SPELL_MEGAERAS_RAGE_ARCANE);
                            }
                            events.ScheduleEvent(EVENT_CHECK_MEGAERAS_RAGE, 4000);
                            break;
                        case EVENT_NETHER_TEAR:
                            if (me->HasAura(SPELL_CONCEALING_FOG) && !me->HasAura(SPELL_EMERGE) && !inRampage)
                                DoCast(me, SPELL_NETHER_TEAR);

                            HandleSendDelay();

                            events.ScheduleEvent(EVENT_NETHER_TEAR, urand(22000, 27000));
                            break;
                        case EVENT_DIFFUSION:
                            if (!me->HasAura(SPELL_CONCEALING_FOG) && !me->HasAura(SPELL_EMERGE) && !inRampage)
                            {
                                me->SetFacingToObject(me->GetVictim());

                                if (Unit* target = me->GetVictim())
                                    DoCast(target, SPELL_DIFFUSION);
                            }

                            events.ScheduleEvent(EVENT_DIFFUSION, urand(16000, 18000));
                            break;
                        case EVENT_MEGAERAS_RAGE:
                            if (!inRampage)
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                DoCast(target, invRampageType.find(me->GetEntry())->second, true);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                DoCast(target, invRampageType.find(me->GetEntry())->second, true);

                            events.ScheduleEvent(EVENT_MEGAERAS_RAGE, urand(2 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                            break;
                    }
                }

                // Can't melee from the fog.
                if (!me->HasAura(SPELL_CONCEALING_FOG) && !me->HasAura(SPELL_EMERGE) && !inRampage)
                    DoMeleeAttackIfReady(true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_arcane_head_megaeraAI(creature);
        }
};

// Cinders 70432.
struct npc_cinders_megaera : public ScriptedAI
{
    npc_cinders_megaera(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* /*summoner*/) override
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DISABLE_MOVE);
        me->SetReactState(REACT_PASSIVE);
        DoCast(me, SPELL_CINDERS_AURA, true);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        damage = 0;
    }
};

// Nether Tear Spawner 70506
struct npc_nether_tear_spawner : public ScriptedAI
{
    npc_nether_tear_spawner(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetReactState(REACT_PASSIVE);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        damage = 0;
    }
};

// Icy Ground 70446.
struct npc_icy_ground_megaera : public ScriptedAI
{
    npc_icy_ground_megaera(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* /*summoner*/) override
    {
        DoCast(me, SPELL_ICY_GROUND_VISUAL, true);
        me->SetReactState(REACT_PASSIVE);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        damage = 0;
    }

    void UpdateAI(uint32 /*diff*/) override { }
};


// Icy Ground 70446.
struct npc_torrent_of_ice : public ScriptedAI
{
    npc_torrent_of_ice(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    uint32 torrentActive;
    ObjectGuid targetGUID;
    ObjectGuid summonerGUID;

    void SetGUID(ObjectGuid guid, int32 /*value*/) override
    {
        targetGUID = guid;
    }

    void IsSummonedBy(Unit* summoner) override
    {
        torrentActive = 0;
        summonerGUID = summoner->GetGUID();
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
        me->PrepareChanneledCast(me->GetOrientation());

        me->SetSpeed(MOVE_WALK, 0.64f);
        me->SetSpeed(MOVE_RUN, 0.64f);

        if (Unit* owner = ObjectAccessor::GetUnit(*me, summonerGUID))
            owner->CastSpell(me, SPELL_TORRENT_OF_ICE);

        events.ScheduleEvent(EVENT_ICY_GROUND_INIT, 3 * IN_MILLISECONDS);
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_TORRENT_OF_ICE)
            return torrentActive;

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_TORRENT_OF_ICE)
            torrentActive = data;
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        damage = 0;
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_ICY_GROUND_INIT)
            {
                torrentActive = 1;

                if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
                {
                    if (target->IsAlive())
                        me->RemoveChanneledCast(target->GetGUID());
                    else if (Creature* owner = ObjectAccessor::GetCreature(*me, summonerGUID))
                    {
                        // Select new target by owner
                        if (Unit* newTarget = owner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector(SPELL_TORRENT_OF_ICE_TARGET)))
                            me->RemoveChanneledCast(newTarget->GetGUID());
                    }
                }

                if (Aura* aura = me->AddAura(SPELL_TORRENT_OF_ICE_NPC_A, me))
                    aura->SetDuration(8000);

                me->InterruptNonMeleeSpells(true);
                me->DespawnOrUnsummon(8.5 * IN_MILLISECONDS);
            }
            break;
        }
    }
};

// Nether Wyrm 70507 - From Arcane Head - Heroic only!
struct npc_nether_wyrm_megaera : public ScriptedAI
{
    npc_nether_wyrm_megaera(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void IsSummonedBy(Unit* /*summoner*/) override
    {
        if (Creature* megaera = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_MEGAERA) : ObjectGuid::Empty))
            megaera->AI()->JustSummoned(me);

        DoZoneInCombat(me, 150.0f);

        events.ScheduleEvent(EVENT_NETHER_SPIKE, urand(3000, 7000));
        events.ScheduleEvent(EVENT_SUPPRESSION, urand(9000, 14000));
    }

    void JustDied(Unit* killer) override
    {
        me->DisappearAndDie();
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
            switch (eventId)
            {
                case EVENT_NETHER_SPIKE:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                        DoCast(target, SPELL_NETHER_SPIKE);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        DoCast(target, SPELL_NETHER_SPIKE);
                
                    events.ScheduleEvent(EVENT_NETHER_SPIKE, urand(7000, 11000));
                    break;
                
                case EVENT_SUPPRESSION:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                        DoCast(target, SPELL_SUPPRESSION);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        DoCast(target, SPELL_NETHER_SPIKE);
                
                    events.ScheduleEvent(EVENT_SUPPRESSION, urand(18000, 23000));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Back Head Spawner 68136
struct npc_megaera_back_head_spawner : public ScriptedAI
{
    npc_megaera_back_head_spawner(Creature* creature) : ScriptedAI(creature) { }

    uint32 emplaceSeed;

    void Reset() override
    {
        emplaceSeed = 0;
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_HEAD_SPAWNED)
            return emplaceSeed;

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_HEAD_SPAWNED)
            emplaceSeed = data;
    }
};

// Concealing Fog - 137973.
class spell_concealing_fog_megaera : public AuraScript
{
    PrepareAuraScript(spell_concealing_fog_megaera);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetOwner()->ToUnit())
            target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetOwner()->ToUnit())
            target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_concealing_fog_megaera::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_concealing_fog_megaera::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};
    

// Cinders - 139822.
class spell_cinders_megaera : public SpellScript
{
    PrepareSpellScript(spell_cinders_megaera);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove whole exclude casters
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if([=](Player* target) { return target && target->HasAura(SPELL_TORRENT_OF_ICE_TARGET); });

            // if we have enough players without tanks, meelee
            if (!pList.empty())
            {
                targets.clear();

                // doesn`t matter - push all cuz max affected target is 1
                for (auto&& itr : pList)
                    targets.push_back(itr);

                m_targets = targets;

                return;
            }

            // if we haven`t enough players
            pList.remove_if([=](Player* target) { return target && target->HasAura(SPELL_TORRENT_OF_ICE_TARGET); });
            m_targets = targets;
        }
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            if (Creature* announcer = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(NPC_SLG_GENERIC_MOP) : ObjectGuid::Empty))
                announcer->AI()->Talk(ANN_BURNING_CINDERS, target);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cinders_megaera::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cinders_megaera::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_cinders_megaera::HandleDummy, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
    }
};

// Cinders - 139822.
class spell_cinders_megaera_aura : public AuraScript
{
    PrepareAuraScript(spell_cinders_megaera_aura);

    void OnTick(AuraEffect const* aurEff)
    {
        // NPC is spawned every 3 seconds (so 3 ticks, 1 ticks / sec) on Heroic diff.
        if (Unit* caster = GetCaster())
            if (Unit* target = GetOwner()->ToUnit())
                if (caster->GetMap()->IsHeroic() && aurEff->GetTickNumber() % 3 == 0)
                    caster->CastSpell(*target, SPELL_CINDERS_SUMMON, true);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        // NPC is spawned on aura removal on Normal diff.
        if (Unit* caster = GetCaster())
            if (Unit* target = GetOwner()->ToUnit())
                caster->CastSpell(*target, SPELL_CINDERS_SUMMON, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_cinders_megaera_aura::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
        OnEffectRemove += AuraEffectRemoveFn(spell_cinders_megaera_aura::OnRemove, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Arctic Freeze (stack checker) - 139843.
class spell_arctic_freeze_megaera : public AuraScript
{
    PrepareAuraScript(spell_arctic_freeze_megaera)

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        // Check aura stacks for stun apply.
        if (Unit* target = GetOwner()->ToUnit())
        {
            if (Aura* arcticFreeze = target->GetAura(SPELL_ARCTIC_FREEZE_DUMMY))
            {
                int32 stacks = arcticFreeze->GetStackAmount();

                // Stack handling.
                if (stacks >= 5 && !target->HasAura(SPELL_ARCTIC_FREEZE_STUN))
                {
                    target->RemoveAurasDueToSpell(SPELL_ARCTIC_FREEZE_DUMMY);
                    target->CastSpell(target, SPELL_ARCTIC_FREEZE_STUN, true);
                }
            }
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_arctic_freeze_megaera::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Acid Glob (Acid Rain dummy) - 134343.
class spell_acid_glob_megaera : public SpellScriptLoader
{
    public:
        spell_acid_glob_megaera() : SpellScriptLoader("spell_acid_glob_megaera") { }

        class spell_acid_glob_megaera_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_acid_glob_megaera_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();

                if (!caster)
                    return;

                if (!caster->ToCreature())
                    return;
                /*
                // The aura is 3 seconds long, and this is triggered each sec. So if not heroic, we should remove the main aura after this first cast.
                if (Unit* target = caster->ToCreature()->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                    caster->CastSpell(target, SPELL_ACID_GLOB_MISSILE, true);
                if (!caster->GetMap()->IsHeroic())
                    caster->RemoveAurasDueToSpell(SPELL_ACID_RAIN);*/
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_acid_glob_megaera_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_acid_glob_megaera_SpellScript();
        }
};

// Icy Ground Creature Hit Selector 139877
class spell_icy_ground_dummy : public SpellScript
{
    PrepareSpellScript(spell_icy_ground_dummy);

    void HandleEffectHitTarget(SpellEffIndex /*eff_idx*/)
    {
        if (Creature* creature = GetHitCreature())
            creature->DespawnOrUnsummon(500);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_icy_ground_dummy::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Submerged 139832
class spell_megaera_submerged : public AuraScript
{
    PrepareAuraScript(spell_megaera_submerged);

    void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetOwner()->ToUnit())
            target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void HandleOnRemove(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetOwner()->ToUnit())
            if (!target->HasAura(SPELL_CONCEALING_FOG))
                target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_megaera_submerged::HandleOnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_megaera_submerged::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class correctSummonerPredicate
{
    private:
        ObjectGuid uiGuid;
    public:
        correctSummonerPredicate(uint64 _guid) : uiGuid(_guid) { }

        bool operator()(WorldObject* target) const
        {
            if (target->ToCreature())
            {
                return target->ToCreature()->GetOwnerGUID() != uiGuid;
            }
            
            return false;
        }
};

// Acid Rain Force Cast 139845
class spell_acid_rain_summon : public SpellScript
{
    PrepareSpellScript(spell_acid_rain_summon);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if(TankSpecTargetSelector());

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void HandleEffectHitTarget(SpellEffIndex /*eff_idx*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (!caster || !target)
            return;

        if (Creature* pRain = caster->SummonCreature(NPC_ACID_RAIN, *target, TEMPSUMMON_TIMED_DESPAWN, 10 * IN_MILLISECONDS))
        {
            pRain->CastSpell(pRain, SPELL_ACID_RAIN_VISUAL, true);
            caster->CastSpell(pRain, SPELL_ACID_RAIN_MISSILE, true);
        }

        // In Heroic 3 Globules
        if (caster->GetMap()->IsHeroic())
        {
            if (Creature* announcer = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(NPC_SLG_GENERIC_MOP) : ObjectGuid::Empty))
            {
                for (uint8 i = 0; i < 2; i++)
                {
                    if (Creature* pRain = caster->SummonCreature(NPC_ACID_RAIN, announcer->GetPositionX() + frand(-30.0f, 30.0f), announcer->GetPositionY() + frand(-28.0f, 28.0f), announcer->GetPositionZ(), announcer->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 10 * IN_MILLISECONDS))
                    {
                        pRain->CastSpell(pRain, SPELL_ACID_RAIN_VISUAL, true);
                        caster->CastSpell(pRain, SPELL_ACID_RAIN_MISSILE, true);
                    }
                }
            }
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_acid_rain_summon::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_acid_rain_summon::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
    }
};

// Acid Rain Missle 139848
class spell_acid_rain_missile : public SpellScript
{
    PrepareSpellScript(spell_acid_rain_missile);

    void HandleEffectHit(SpellEffIndex /*eff_idx*/)
    {
        if (Unit* target = GetHitUnit())
        {
            target->CastSpell(target, SPELL_ACID_RAIN_DAMAGE, true);
            target->RemoveAurasDueToSpell(SPELL_ACID_RAIN_VISUAL);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_acid_rain_missile::HandleEffectHit, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
    }
};

// Acid Rain Damage Eff 139850
class spell_acid_rain_damage : public SpellScript
{
    PrepareSpellScript(spell_acid_rain_damage);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && (!target->ToPlayer() || target->ToPlayer() && target->ToPlayer()->IsGameMaster()); });
    }

    void HandleEffectHitTarget(SpellEffIndex /*eff_idx*/)
    {
        if (Unit* unit = GetHitUnit())
        {
            // 75% damage reduction after 30 yards distance
            float dist = unit->GetExactDist2d(GetCaster());

            int32 reduction = dist >= 30 ? GetHitDamage() * 0.25 : GetHitDamage() * (1 - ((dist * 2.5) / 100));
            SetHitDamage(reduction);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_acid_rain_damage::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_acid_rain_damage::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Hydra Frenzy 139942
class spell_megaera_hydra_frenzy : public AuraScript
{
    PrepareAuraScript(spell_megaera_hydra_frenzy);

    void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->SetFullHealth();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_megaera_hydra_frenzy::HandleOnApply, EFFECT_0, SPELL_AURA_MOD_ATTACKSPEED, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
    }
};

// Rampage 139458
class spell_megaera_rampage : public AuraScript
{
    PrepareAuraScript(spell_megaera_rampage)

    bool Load() override
    {
        prevSpellId = 0;
        return true;
    }

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            std::vector<uint32> rampageType = { SPELL_RAMPAGE_FIRE, SPELL_RAMPAGE_FROST, SPELL_RAMPAGE_POISON };

            if (owner->GetMap()->IsHeroic())
                rampageType.push_back(SPELL_RAMPAGE_ARCANE);
            
            if (prevSpellId)
                rampageType.erase(std::find(rampageType.begin(), rampageType.end(), prevSpellId));

            // Select new spell from updated container
            prevSpellId = Trinity::Containers::SelectRandomContainerElement(rampageType);
            uint32 auraMod = owner->AI()->GetData(invEmpowerElementalType.find(prevSpellId)->second);
            float mod = (auraMod * sSpellMgr->GetSpellInfo(SPELL_RAMPAGE, owner->GetMap()->GetDifficulty())->Effects[0].BasePoints) / 100.0f;
            int32 bp = sSpellMgr->GetSpellInfo(prevSpellId, owner->GetMap()->GetDifficulty())->Effects[0].BasePoints + (int32)(sSpellMgr->GetSpellInfo(prevSpellId)->Effects[EFFECT_0].BasePoints * mod);
            owner->CastCustomSpell(owner, prevSpellId, &bp, 0, 0, true);
        }
    }

    private:
        uint32 prevSpellId;

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_megaera_rampage::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Nether Tear 140138
class spell_nether_tear : public AuraScript
{
    PrepareAuraScript(spell_nether_tear);

    void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->CastSpell(owner, SPELL_NETHER_PORTAL, true);
    }

    void HandleOnRemove(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->RemoveAurasDueToSpell(SPELL_NETHER_PORTAL);
    }

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            if (owner->GetInstanceScript() && owner->GetInstanceScript()->GetBossState(DATA_MEGAERA) == IN_PROGRESS)
                owner->SummonCreature(NPC_NETHER_WYRM, *owner, TEMPSUMMON_MANUAL_DESPAWN);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_nether_tear::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_nether_tear::HandleOnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_nether_tear::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// Diffusion Proc 139993
class spell_diffusion_proc : public AuraScript
{
    PrepareAuraScript(spell_diffusion_proc);

    void HandleOnProc(ProcEventInfo& rProcInfo)
    {
        int32 healAmount = rProcInfo.GetHealInfo()->GetHeal() ? (int32)(rProcInfo.GetHealInfo()->GetHeal() * 0.1f) : 0;

        if (!healAmount)
            return;

        if (Unit* owner = rProcInfo.GetActor())
            owner->CastCustomSpell(owner, SPELL_DIFFUSION_REDIRECT, &healAmount, 0, 0, true);
    }

    void Register()
    {
        OnProc += AuraProcFn(spell_diffusion_proc::HandleOnProc);
    }
};

// Torrent of Ice Summon 139870
class spell_torrent_of_ice_summon : public SpellScript
{
    PrepareSpellScript(spell_torrent_of_ice_summon);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Creature* target = GetHitCreature())
            if (!target->AI()->GetData(TYPE_TORRENT_OF_ICE))
                PreventHitEffect(eff_idx);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_torrent_of_ice_summon::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SUMMON);
    }
};

// 853, 876. Summoned By 139875 - Icy Ground
class sat_icy_ground : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    /*uint32 timer;
    float radius;

    void OnInit() override
    {
        if (GetCaster() && GetCaster()->GetMap()->IsHeroic())
        {
            timer = 1000;
            radius = 8.2f;
            HandleGrow();
        }
    }

    void HandleGrow()
    {
        if (AreaTrigger* areaTrigger = GetAreaTrigger())
        {
            areaTrigger->SetFloatValue(AREATRIGGER_FIELD_EXPLICIT_SCALE, radius / 8.2f);
            areaTrigger->SetVisualRadius(radius + 0.8f);
            areaTrigger->UpdateObjectVisibility();
        }
    }

    void OnUpdate(uint32 diff) override
    {
        if (timer <= diff)
        {
            radius += 0.017f;
            HandleGrow();

            timer = 1000;
        }
        else timer -= diff;
    }*/

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* target = triggering->ToUnit())
        {
            if (target->HasAura(SPELL_CINDERS))
            {
                if (Creature* pOwner = m_caster->ToCreature())
                {
                    pOwner->DespawnOrUnsummon(500);
                    return;
                }
            }

            if (Aura* aura = target->GetAura(SPELL_ICY_GROUND_AURA))
                aura->RefreshDuration();
            else
                m_caster->CastSpell(target, SPELL_ICY_GROUND_AURA, true);
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Unit* target = triggering->ToUnit())
            if (Unit* caster = GetCaster())
                target->RemoveAura(SPELL_ICY_GROUND_AURA, caster->GetGUID(), 0, AURA_REMOVE_BY_EXPIRE);
    }

    void OnTriggeringUpdate(WorldObject* triggering) override
    {
        if (Unit* target = triggering->ToUnit())
            if (Aura* aura = target->GetAura(SPELL_ICY_GROUND_AURA))
                aura->RefreshDuration();
    }
};

// Areatrigger 8953, 8943, 8944, 8954
class AreaTrigger_into_megaera_water : public AreaTriggerScript
{
    public:
        AreaTrigger_into_megaera_water() : AreaTriggerScript("AreaTrigger_into_megaera_water") { }
    
        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            player->JumpTo(trigger->ID == 8954 ? 9.0f : 10.0f, trigger->ID == 8954 ? 40.0f : 10.0f, false);
    
            return true;
        }
};

class cond_nether_tear_selector : public ConditionScript
{
    public:
        cond_nether_tear_selector() : ConditionScript("cond_nether_tear_selector") { }
    
        bool OnConditionCheck(const Condition* cond, ConditionSourceInfo& source) override
        {
            if (source.mConditionTargets[0])
                if (Creature* target = source.mConditionTargets[0]->ToCreature())
                    if (!target->HasAura(SPELL_NETHER_PORTAL))
                        return true;
    
            return false;
        }
};

void AddSC_boss_megaera()
{
    new boss_megaera();
    new npc_flaming_head_megaera();
    new npc_frozen_head_megaera();
    new npc_venomous_head_megaera();
    new npc_arcane_head_megaera();
    new creature_script<npc_cinders_megaera>("npc_cinders_megaera");
    new creature_script<npc_nether_tear_spawner>("npc_nether_tear_spawner");
    new creature_script<npc_icy_ground_megaera>("npc_icy_ground_megaera");
    new creature_script<npc_torrent_of_ice>("npc_torrent_of_ice");
    new creature_script<npc_nether_wyrm_megaera>("npc_nether_wyrm_megaera");
    new creature_script<npc_megaera_back_head_spawner>("npc_megaera_back_head_spawner");

    new aura_script<spell_concealing_fog_megaera>("spell_concealing_fog_megaera");
    new spell_script<spell_cinders_megaera>("spell_cinders_megaera");
    new aura_script<spell_cinders_megaera_aura>("spell_cinders_megaera_aura");
    new aura_script<spell_arctic_freeze_megaera>("spell_arctic_freeze_megaera");
    new spell_acid_glob_megaera();
    new spell_script<spell_icy_ground_dummy>("spell_icy_ground_dummy");
    new aura_script<spell_megaera_submerged>("spell_megaera_submerged");
    new spell_script<spell_acid_rain_summon>("spell_acid_rain_summon");
    new spell_script<spell_acid_rain_missile>("spell_acid_rain_missile");
    new spell_script<spell_acid_rain_damage>("spell_acid_rain_damage");
    new aura_script<spell_megaera_hydra_frenzy>("spell_megaera_hydra_frenzy");
    new aura_script<spell_megaera_rampage>("spell_megaera_rampage");
    new aura_script<spell_nether_tear>("spell_nether_tear");
    new aura_script<spell_diffusion_proc>("spell_diffusion_proc");
    new spell_script<spell_torrent_of_ice_summon>("spell_torrent_of_ice_summon");
    new atrigger_script<sat_icy_ground>("sat_icy_ground");
    new AreaTrigger_into_megaera_water();
    new cond_nether_tear_selector();
}
