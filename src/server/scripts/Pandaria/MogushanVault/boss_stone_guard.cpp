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

#include "mogu_shan_vault.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "TemporarySummon.h"

enum eSpells
{
    SPELL_STONE_GUARD_CREDIT            = 128288,

    // Jasper
    SPELL_JASPER_OVERLOAD               = 115843,
    SPELL_JASPER_PETRIFICATION          = 116036,
    SPELL_JASPER_PETRIFICATION_BAR      = 131270,
    SEPLL_JASPER_TRUE_FORM              = 115828,
    SPELL_JASPER_CHAINS                 = 130395,
    SPELL_JASPER_CHAINS_VISUAL          = 130403,
    SPELL_JASPER_CHAINS_DAMAGE          = 130404,

    // Jade
    SPELL_JADE_OVERLOAD                 = 115842,
    SPELL_JADE_PETRIFICATION            = 116006,
    SPELL_JADE_PETRIFICATION_BAR        = 131269,
    SEPLL_JADE_TRUE_FORM                = 115827,
    SPELL_JADE_SHARDS                   = 116223,

    // Amethyst
    SPELL_AMETHYST_OVERLOAD             = 115844,
    SPELL_AMETHYST_PETRIFICATION        = 116057,
    SPELL_AMETHYST_PETRIFICATION_BAR    = 131255,
    SPELL_AMETHYST_TRUE_FORM            = 115829,
    SPELL_AMETHYST_POOL                 = 116235,
    SPELL_AMETHYST_POOL_AUR_EFFECT      = 130774,

    // Cobalt
    SPELL_COBALT_OVERLOAD               = 115840,
    SPELL_COBALT_PETRIFICATION          = 115852,
    SPELL_COBALT_PETRIFICATION_BAR      = 131268,
    SEPLL_COBALT_TRUE_FORM              = 115771,
    SPELL_COBALT_MINE                   = 129460,
    SPELL_COBALT_MINE_SELECTOR          = 129426,
    SPELL_COBALT_MINE_SELECTOR_EFF      = 129428,

    // Shared
    SPELL_SOLID_STONE                   = 115745,
    SPELL_REND_FLESH                    = 125206,
    SPELL_ANIM_SIT                      = 128886,
    SPELL_ZERO_ENERGY                   = 72242,
    SPELL_TOTALY_PETRIFIED              = 115877,

    //  Energized Tiles
    SPELL_TILES_AURA                    = 116579, // for check activated trigger
    SPELL_TILES_AURA_EFFECT             = 116541, // stuck on all players in area
    SPELL_TILES_DISPLAYED               = 116542,
    SPELL_LIVING_AMETHYST               = 116322,
    SPELL_LIVING_COBALT                 = 116199,
    SPELL_LIVING_JADE                   = 116301,
    SPELL_LIVING_JASPER                 = 116304,

    // Achievements
    SPELL_ACHIEVEMENT_MUST_LOVE_DOGS    = 125625,
};

enum eEvents
{
    // Controller
    EVENT_PETRIFICATION                 = 1,
    EVENT_CRYSTALS                      = 2,
    EVENT_PETRIFIED                     = 3,

    // Guardians
    EVENT_CHECK_NEAR_GUARDIANS          = 4,
    EVENT_CHECK_ENERGY                  = 5,
    EVENT_REND_FLESH                    = 6,
    EVENT_MAIN_ATTACK                   = 7,
    EVENT_ENRAGE                        = 8,

    // Tiles
    EVENT_TILING                        = 9,
};

enum Talk
{
    TALK_PETRIFY = 0,
};

enum Misc
{
    STONE_GUARD_LOOT_ID = 59915,
};

uint32 guardiansEntry[4] =
{
    NPC_JASPER,
    NPC_JADE,
    NPC_AMETHYST,
    NPC_COBALT
};

const std::vector<uint32> petrificationList =
{
    SPELL_JASPER_PETRIFICATION,
    SPELL_JADE_PETRIFICATION,
    SPELL_COBALT_PETRIFICATION,
    SPELL_AMETHYST_PETRIFICATION,
};

uint32 const LivingCrystals[CRYSTALL_AURA] = { SPELL_LIVING_AMETHYST, SPELL_LIVING_COBALT, SPELL_LIVING_JADE, SPELL_LIVING_JASPER };

// For living crystals - Stone guard
Position stoneGuardsPos[CRYSTALL_AURA] =
{
    {3919.89f, 1258.59f, 466.363f, 4.66991f},
    {3878.93f, 1258.49f, 466.363f, 4.69497f},
    {3928.00f, 1246.34f, 466.363f, 4.71147f},
    {3870.75f, 1246.28f, 466.363f, 4.54348f}
};

std::vector<uint32> CrystalAuraType =
{
    SPELL_LIVING_AMETHYST,
    SPELL_LIVING_COBALT,
    SPELL_LIVING_JADE,
    SPELL_LIVING_JASPER,
};

std::vector<uint32> CrystalType =
{
    NPC_LIVING_AMETHYST_CRYSTAL,
    NPC_LIVING_COBALT_CRYSTAL,
    NPC_LIVING_JADE_CRYSTAL,
    NPC_LIVING_JASPER_CRYSTAL,
    NPC_TILING_CREATURE,
};

// Specific orientation for tiles (should not be turned)
#define TILE_ORIENTATION 4.714031f

// Invisible modelID for NPC Tiling Creature 62026
#define INVISIBLE_DISPLAYID 11686

// 60089 - Stone Guard Controller
class boss_stone_guard_controller : public CreatureScript
{
    public:
        boss_stone_guard_controller() : CreatureScript("boss_stone_guard_controller") { }

        struct boss_stone_guard_controllerAI : public ScriptedAI
        {
            boss_stone_guard_controllerAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                hasLooted = false;
                looterId  = 0;
            }

            InstanceScript* instance;
            EventMap events;

            uint32 prevPetrifiedGuardian;
            float Tiles;
            bool petrified;
            bool petrifiedActivated;
            bool hasLooted;
            uint32 looterId;

            uint8 totalGuardian;

            // Heroic
            uint8 powDownCount;

            bool fightInProgress;

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetVisible(false);

                fightInProgress       = false;
                petrifiedActivated    = false;
                petrified             = false;
                prevPetrifiedGuardian = 0;

                if (instance->GetBossState(DATA_STONE_GUARD) != DONE)
                    instance->SetBossState(DATA_STONE_GUARD, NOT_STARTED);

                if (IsHeroic())
                    powDownCount = 2;

                me->RemoveAllDynObjects();
                HandleRemoveCrystals();

                // Removing buff
                if (instance)
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TILES_AURA_EFFECT);

                events.Reset();
            }

            void HandleRemoveCrystals()
            {
                std::list<Creature*> LivingCrystals;

                for (auto&& itr : CrystalType)
                    GetCreatureListWithEntryInGrid(LivingCrystals, me, itr, VISIBLE_RANGE);

                for (auto&& m_crystal : LivingCrystals)
                    m_crystal->DespawnOrUnsummon();
            }

            void HandleResetTiles()
            {
                std::list<Creature*> Tiles;
                GetCreatureListWithEntryInGrid(Tiles, me, NPC_TILING_CREATURE, VISIBLE_RANGE);

                for (auto&& itr : Tiles)
                    itr->AddAura(SPELL_TILES_AURA, itr);

                for (auto m_auraType : CrystalAuraType)
                    if (instance)
                        instance->DoRemoveAurasDueToSpellOnPlayers(m_auraType);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                damage = 0;
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_ENTER_COMBAT:
                    {
                        for (uint32 entry : guardiansEntry)
                        {
                            if (Creature* guardian = me->GetMap()->GetCreature(instance->GetGuidData(entry)))
                            {
                                if (guardian->IsAlive())
                                {
                                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, guardian);

                                    if (!hasLooted)
                                    {
                                        hasLooted = true;
                                        looterId = guardian->GetEntry();
                                    }
                                }
                            }
                        }

                        events.ScheduleEvent(EVENT_PETRIFICATION, 8000);
                        events.ScheduleEvent(EVENT_PETRIFIED, 2500);

                        if (IsHeroic())
                            events.ScheduleEvent(EVENT_CRYSTALS, 7000);

                        fightInProgress = true;
                        Tiles = 0.0f;
                        DoZoneInCombat(me, 200.0f);
                        break;
                    }
                    case ACTION_GUARDIAN_DIED:
                    {
                        if (instance->GetBossState(DATA_STONE_GUARD) == DONE)
                            break;

                        instance->SetBossState(DATA_STONE_GUARD, DONE);
                        RewardPlayers();

                        for (uint32 entry: guardiansEntry)
                            if (Creature* guardian = me->GetMap()->GetCreature(instance->GetGuidData(entry)))
                                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, guardian);

                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOTALY_PETRIFIED);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_JASPER_CHAINS);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_JASPER_PETRIFICATION_BAR);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_JADE_PETRIFICATION_BAR);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_AMETHYST_PETRIFICATION_BAR);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_COBALT_PETRIFICATION_BAR);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TILES_AURA_EFFECT);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_AMETHYST_POOL_AUR_EFFECT);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_REND_FLESH);
                        instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                        if (Creature* Cho = me->SummonCreature(NPC_LOREWALKER_CHO, ChoPastStoneGuardsSpawn, TEMPSUMMON_MANUAL_DESPAWN))
                            Cho->AI()->DoAction(ACTION_CHO_DOGS_DIE);

                        fightInProgress = false;

                        // Removing Crystals
                        HandleRemoveCrystals();

                        // Removing energized tiles
                        me->RemoveAllDynObjects();
                        me->CombatStop(true);
                        me->Kill(me);
                        break;
                    }
                    case ACTION_POWER_DOWN:
                    {
                        for (uint32 entry : guardiansEntry)
                        {
                            if (Creature* guardian = me->GetMap()->GetCreature(instance->GetGuidData(entry)))
                            {
                                if (powDownCount)
                                {
                                    if (powDownCount == 2 || guardian->GetHealthPct() < 40.0f)
                                    {
                                        // Removing Tiles
                                        me->RemoveAllDynObjects();

                                        // Removing buff
                                        if (instance)
                                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TILES_AURA_EFFECT);

                                        HandleResetTiles();

                                        // Decreasing powDowCount
                                        --powDownCount;
                                    }
                                }
                                break;
                            }
                        }
                        break;
                    }
                    case ACTION_PETRIFICATION:
                    {
                        if (!petrifiedActivated)
                        {
                            petrifiedActivated = true;
                            events.ScheduleEvent(EVENT_PETRIFICATION, urand(6.5 * IN_MILLISECONDS, 7 * IN_MILLISECONDS));
                        }
                        break;
                    }
                    case ACTION_EVADE:
                    {
                        for (uint32 entry : guardiansEntry)
                            if (Creature* guardian = me->GetMap()->GetCreature(instance->GetGuidData(entry)))
                                if (guardian->AI())
                                    guardian->AI()->EnterEvadeMode();

                        Reset();
                        break;
                    }
                }
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_GUARDIAN_LOOT)
                    return looterId;
                return 0;
            }

            void RewardPlayers()
            {
                if (PlayersHavePets())
                    DoCast(me, SPELL_ACHIEVEMENT_MUST_LOVE_DOGS);

                DoCastAOE(SPELL_STONE_GUARD_CREDIT);

                if (Player* player = me->GetMap()->GetFirstPlayerInInstance())
                    player->RewardPersonalLootAndCurrency(me, STONE_GUARD_LOOT_ID);
            }

            bool PlayersHavePets()
            {
                Map::PlayerList const &playerList = instance->instance->GetPlayers();

                for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                {
                    Player* player = itr->GetSource();

                    if (!player)
                        continue;

                    std::list<TempSummon*> DogsList;

                    for (auto&& Dogs : vDogs)
                        player->GetSummons(DogsList, Dogs);

                    if (DogsList.empty())
                        return false;
                }

                return true;
            }

            // using only in 10/10 hc mode
            uint32 GetDownGuardian()
            {
                for (uint32 entry : guardiansEntry)
                {
                    if (Creature* guardian = ObjectAccessor::GetCreature(*me, instance->GetGuidData(entry)))
                        if (guardian->IsInCombat())
                            continue;

                    return entry;
                }

                return 0;
            }

            bool AnyGuardHasPetrification()
            {
                for (uint32 entry : guardiansEntry)
                    if (Creature* guardian = ObjectAccessor::GetCreature(*me, instance->GetGuidData(entry)))
                        for (auto&& pId : petrificationList)
                            if (guardian->HasAura(pId))
                                return true;

                return false;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!fightInProgress)
                    return;

                if (!instance)
                    return;

                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_PETRIFICATION:
                    {
                        if (fightInProgress)
                        {
                            petrifiedActivated = false;

                            if (AnyGuardHasPetrification())
                                break;

                            std::vector<uint32> StoneGuardsStorage = { NPC_COBALT, NPC_JADE, NPC_JASPER, NPC_AMETHYST };

                            // Only for 10/10 hc mode
                            if (!Is25ManRaid())
                            {
                                if (uint32 dGuardian = GetDownGuardian())
                                    StoneGuardsStorage.erase(std::find(StoneGuardsStorage.begin(), StoneGuardsStorage.end(), dGuardian));
                            }

                            // Remove From Temp container value, that was used before (real random)
                            if (prevPetrifiedGuardian)
                                StoneGuardsStorage.erase(std::find(StoneGuardsStorage.begin(), StoneGuardsStorage.end(), prevPetrifiedGuardian));

                            // Select new guardian from updated container
                            prevPetrifiedGuardian = Trinity::Containers::SelectRandomContainerElement(StoneGuardsStorage);

                            if (Creature* sGuardian = ObjectAccessor::GetCreature(*me, instance->GetGuidData(prevPetrifiedGuardian)))
                                if (sGuardian->IsAlive() && sGuardian->IsInCombat())
                                    sGuardian->AI()->DoAction(ACTION_PETRIFICATION);
 
                        }
                        break;
                    }
                    case EVENT_PETRIFIED:
                    {
                        if (HasAnyPlayerIsPetrified() && !petrified)
                        {
                            DoAction(ACTION_EVADE);
                            petrified = true;
                        }
                        else 
                            if (HasAnyPlayerIsPetrified() && petrified)
                                if (instance)
                                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOTALY_PETRIFIED);

                        events.ScheduleEvent(EVENT_PETRIFIED, 2500);
                        break;
                    }
                    case EVENT_CRYSTALS:
                    {
                        ActivateTilesTriggers();
                        for (uint8 i = 0; i < 4; ++i)
                            if (ObjectGuid stoneGuardGuid = instance->GetGuidData(guardiansEntry[i]))
                                if (Creature* stoneGuard = instance->instance->GetCreature(stoneGuardGuid))
                                    if (stoneGuard->IsAlive())
                                    {
                                        switch (stoneGuard->GetEntry())
                                        {
                                            case NPC_JASPER:
                                            {
                                                me->SummonCreature(NPC_LIVING_JASPER_CRYSTAL, stoneGuardsPos[i]);
                                                break;
                                            }
                                            case NPC_JADE:
                                            {
                                                me->SummonCreature(NPC_LIVING_JADE_CRYSTAL, stoneGuardsPos[i]);
                                                break;
                                            }
                                            case NPC_AMETHYST:
                                            {
                                                me->SummonCreature(NPC_LIVING_AMETHYST_CRYSTAL, stoneGuardsPos[i]);
                                                break;
                                            }
                                            case NPC_COBALT:
                                            {
                                                me->SummonCreature(NPC_LIVING_COBALT_CRYSTAL, stoneGuardsPos[i]);
                                                break;
                                            }
                                        }
                                    }
                                    break;
                    }
                }
            }
            private:
                void ActivateTilesTriggers()
                {
                    for (uint32 i = 0; i < MAX_TILES; i++)
                    {
                        for (uint32 j = 0; j < MAX_TILES_ON_LINE; j++)
                        {
                            // exclude crystal platforms at cross
                            if (i == 1 || i == 2 || i == 7 || i == 8)
                                if (j == 2 || j == 3)
                                {
                                    Tiles -= 4.1f;
                                    continue;
                                }

                            if (i == 3 || i == 4 || i == 9 || i == 10)
                                if (j == 5 || j == 6)
                                {
                                    Tiles -= 4.1f;
                                    continue;
                                }

                            me->SummonCreature(NPC_TILING_CREATURE, TilesStartPos[i].GetPositionX(), TilesStartPos[i].GetPositionY() + Tiles, TilesStartPos[i].GetPositionZ(), TilesStartPos[i].GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
                            Tiles -= 4.1f;
                        }
                        Tiles = 0.0f;
                    }
                }

                bool HasAnyPlayerIsPetrified()
                {
                    std::list<Player*> PetrifiedPlayers;
                    GetPlayerListInGrid(PetrifiedPlayers, me, 200.0f);

                    for (auto&& itr : PetrifiedPlayers)
                        if (itr->IsAlive() && itr->HasAura(SPELL_TOTALY_PETRIFIED)) // need check once, cuz another had same buff
                            return true;

                    return false;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_stone_guard_controllerAI(creature);
        }
};

// 59915 - Jasper Guardian
// 60043 - Jade Guardian
// 60047 - Amethyst Guardian
// 60051 - Cobalt Guardian
class boss_generic_guardian : public CreatureScript
{
    public:
        boss_generic_guardian() : CreatureScript("boss_generic_guardian") { }

        struct boss_generic_guardianAI : public BossAI
        {
            boss_generic_guardianAI(Creature* creature) : BossAI(creature, DATA_STONE_GUARD), summons(creature) 
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
            }

            EventMap events;
            SummonList summons;

            uint32 spellOverloadId;
            uint32 spellPetrificationId;
            uint32 spellPetrificationBarId;
            uint32 spellTrueFormId;
            uint32 spellMainAttack;
            uint32 delay;

            bool warnedForOverload;
            bool isInTrueForm;
            bool CombatDelayDone;

            Creature* GetController()
            {
                if (instance)
                    return me->GetMap()->GetCreature(instance->GetGuidData(NPC_STONE_GUARD_CONTROLLER));
                else
                    return NULL;
            }

            void InitializeAI() override
            {
                me->setActive(true);
                Reset();
            }

            void Reset() override
            {
                _Reset();
                isInTrueForm      = false;
                warnedForOverload = false;
                CombatDelayDone   = false;
                delay             = 0;
                me->SetReactState(REACT_DEFENSIVE);
                me->SetPowerType(POWER_ENERGY);
                me->SetPower(POWER_ENERGY, 0);
                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);

                me->CastSpell(me, SPELL_SOLID_STONE, true);
                me->CastSpell(me, SPELL_ANIM_SIT,    true);
                me->CastSpell(me, SPELL_ZERO_ENERGY, true);

                if (Creature* stoneGuardController = me->GetMap()->GetCreature(instance->GetGuidData(NPC_STONE_GUARD_CONTROLLER)))
                    stoneGuardController->AI()->Reset();

                instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                summons.DespawnAll();
                me->RemoveAllAreasTrigger();

                switch (me->GetEntry())
                {
                    case NPC_JASPER:
                        spellOverloadId             = SPELL_JASPER_OVERLOAD;
                        spellPetrificationId        = SPELL_JASPER_PETRIFICATION;
                        spellPetrificationBarId     = SPELL_JASPER_PETRIFICATION_BAR;
                        spellTrueFormId             = SEPLL_JASPER_TRUE_FORM;
                        spellMainAttack             = SPELL_JASPER_CHAINS;
                        break;
                    case NPC_JADE:
                        spellOverloadId             = SPELL_JADE_OVERLOAD;
                        spellPetrificationId        = SPELL_JADE_PETRIFICATION;
                        spellPetrificationBarId     = SPELL_JADE_PETRIFICATION_BAR;
                        spellTrueFormId             = SEPLL_JADE_TRUE_FORM;
                        spellMainAttack             = SPELL_JADE_SHARDS;
                        break;
                    case NPC_AMETHYST:
                        spellOverloadId             = SPELL_AMETHYST_OVERLOAD;
                        spellPetrificationId        = SPELL_AMETHYST_PETRIFICATION;
                        spellPetrificationBarId     = SPELL_AMETHYST_PETRIFICATION_BAR;
                        spellTrueFormId             = SPELL_AMETHYST_TRUE_FORM;
                        spellMainAttack             = SPELL_AMETHYST_POOL;
                        break;
                    case NPC_COBALT:
                        spellOverloadId             = SPELL_COBALT_OVERLOAD;
                        spellPetrificationId        = SPELL_COBALT_PETRIFICATION;
                        spellPetrificationBarId     = SPELL_COBALT_PETRIFICATION_BAR;
                        spellTrueFormId             = SEPLL_COBALT_TRUE_FORM;
                        spellMainAttack             = SPELL_COBALT_MINE;
                        break;
                    default:
                        spellOverloadId             = 0;
                        spellPetrificationId        = 0;
                        spellPetrificationBarId     = 0;
                        spellTrueFormId             = 0;
                        spellMainAttack             = 0;
                        break;
                }

                if (instance)
                    instance->DoRemoveAurasDueToSpellOnPlayers(spellPetrificationBarId);

                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (instance)
                    instance->SetBossState(DATA_STONE_GUARD, IN_PROGRESS);

                me->RemoveAurasDueToSpell(SPELL_SOLID_STONE);
                me->RemoveAurasDueToSpell(SPELL_ANIM_SIT);

                events.ScheduleEvent(EVENT_CHECK_NEAR_GUARDIANS, 2000);
                events.ScheduleEvent(EVENT_CHECK_ENERGY, 1000);
                events.ScheduleEvent(EVENT_REND_FLESH, 5000);
                events.ScheduleEvent(EVENT_MAIN_ATTACK, 10000);
                events.ScheduleEvent(EVENT_ENRAGE, 8 * MINUTE * IN_MILLISECONDS);

                delay = 0;
                me->m_Events.Schedule(delay += 3000, 20, [this]()
                {
                    CombatDelayDone = true;
                });
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                me->RemoveAurasDueToSpell(SPELL_AMETHYST_PETRIFICATION);
                me->RemoveAurasDueToSpell(SPELL_JADE_PETRIFICATION);
                me->RemoveAurasDueToSpell(SPELL_COBALT_PETRIFICATION);
                me->RemoveAurasDueToSpell(SPELL_JASPER_PETRIFICATION);

                for (uint32 entry : guardiansEntry)
                {
                    if (instance)
                    {
                        if (Creature* gardian = me->GetMap()->GetCreature(instance->GetGuidData(entry)))
                        {
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, gardian);
                            gardian->RemoveAurasDueToSpell(SPELL_AMETHYST_PETRIFICATION);
                            gardian->RemoveAurasDueToSpell(SPELL_JADE_PETRIFICATION);
                            gardian->RemoveAurasDueToSpell(SPELL_COBALT_PETRIFICATION);
                            gardian->RemoveAurasDueToSpell(SPELL_JASPER_PETRIFICATION);
                        }
                    }
                }

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_JASPER_CHAINS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOTALY_PETRIFIED);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_COBALT_PETRIFICATION_BAR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_JASPER_PETRIFICATION_BAR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_JADE_PETRIFICATION_BAR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_AMETHYST_PETRIFICATION_BAR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_AMETHYST_POOL_AUR_EFFECT);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                if (Creature* controller = GetController())
                    controller->AI()->Reset();

                BossAI::EnterEvadeMode();
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_STONE_GUARD, FAIL);
                }
                _DespawnAtEvade();
                me->GetMotionMaster()->MoveTargetedHome();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (Creature* controller = GetController())
                {
                    if (damage >= me->GetHealth() && me->IsAlive())
                    {
                        if (controller->AI()->GetData(TYPE_GUARDIAN_LOOT) == me->GetEntry())
                            me->LowerPlayerDamageReq(me->GetMaxHealth()); // Allow player loots even if only the controller has damaged the guardian

                        for (uint32 entry : guardiansEntry)
                        {
                            if (Creature* gardian = controller->GetMap()->GetCreature(instance->GetGuidData(entry)))
                            {
                                if (gardian != me)
                                {
                                    if (controller->AI()->GetData(TYPE_GUARDIAN_LOOT) == gardian->GetEntry())
                                        attacker->DealDamage(gardian, gardian->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                                    else
                                        attacker->Kill(gardian, false);
                                }
                            }
                        }

                        controller->AI()->DoAction(ACTION_GUARDIAN_DIED);

                                return;
                    }

                    for (uint32 entry: guardiansEntry)
                        if (Creature* gardian = controller->GetMap()->GetCreature(instance->GetGuidData(entry)))
                            if (gardian->GetGUID() != me->GetGUID() && damage < gardian->GetHealth())
                                gardian->ModifyHealth(-int32(damage));

                            // Heroic : turns off white tiles when life < 75% and then < 40%
                    if (IsHeroic())
                        if (me->HealthBelowPctDamaged(75, damage))
                            controller->AI()->DoAction(ACTION_POWER_DOWN);
                }
            }

            void RegeneratePower(Powers /*power*/, int32& value)
            {
                if (!me->IsInCombat())
                {
                    value = 0;
                    return;
                }

                if (isInTrueForm)
                    value = 4; // Creature regen every 2 seconds, and guardians must regen at 2/sec
                    else
                        value = 0;
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->RemoveAllAreasTrigger();

                if (Creature* controller = GetController())
                {
                    controller->AI()->DoAction(ACTION_GUARDIAN_DIED);

                    if (controller->AI()->GetData(TYPE_GUARDIAN_LOOT) != me->GetEntry())
                        me->SetLootRecipient(NULL);
                }

            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_ENTER_COMBAT:
                        if (!me->IsInCombat())
                            if (Player* victim = me->SelectNearestPlayerNotGM(100.0f))
                                AttackStart(victim);
                            break;
                    case ACTION_PETRIFICATION:
                    {
                        Talk(TALK_PETRIFY);
                        me->CastSpell(me, spellPetrificationId, true);
                        instance->DoCastSpellOnPlayers(spellPetrificationBarId);
                        break;
                    }
                }
            }

            bool CheckNearGuardians()
            {
                for (uint32 entry : guardiansEntry)
                    if (entry != me->GetEntry())
                        if (Creature* sGuardian = GetClosestCreatureWithEntry(me, entry, 12.0f, true))
                            return true;

                        return false;
            }

            bool GuardiansInCombat()
            {
                for (uint32 entry : guardiansEntry)
                {
                    if (me->GetEntry() == entry)
                        continue;

                    if (Creature* guardian = me->GetMap()->GetCreature(instance->GetGuidData(entry)))
                        if (!guardian->IsInCombat() && guardian->IsAlive())
                            return false;
                }

                return true;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                {
                    if (!me->isMoving() && !me->HasAura(SPELL_ANIM_SIT))
                        me->CastSpell(me, SPELL_ANIM_SIT, true);

                    return;
                }

                events.Update(diff);

                // Misdirection exploit
                if (CombatDelayDone && !GuardiansInCombat())
                    if (Creature* controller = GetController())
                        controller->AI()->DoAction(ACTION_EVADE);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_NEAR_GUARDIANS:
                        {
                            bool hasNearGardian = CheckNearGuardians();

                            if (!isInTrueForm && hasNearGardian)
                            {
                                me->RemoveAurasDueToSpell(SPELL_SOLID_STONE);
                                me->CastSpell(me, spellTrueFormId, true);
                                isInTrueForm = true;
                            }
                            else if (isInTrueForm && !hasNearGardian)
                            {
                                me->CastSpell(me, SPELL_SOLID_STONE, true);
                                me->RemoveAurasDueToSpell(spellTrueFormId);
                                isInTrueForm = false;
                            }

                            events.ScheduleEvent(EVENT_CHECK_NEAR_GUARDIANS, 2000);
                            break;
                        }
                        case EVENT_CHECK_ENERGY:
                        {
                            if (me->GetPower(POWER_ENERGY) >= me->GetMaxPower(POWER_ENERGY))
                            {
                                me->CastSpell(me, spellOverloadId, false);
                                me->RemoveAurasDueToSpell(spellPetrificationId);
                                if (instance)
                                    instance->DoRemoveAurasDueToSpellOnPlayers(spellPetrificationBarId);
                            }
                            else if (me->GetPower(POWER_ENERGY) >= 85 && !warnedForOverload)
                                warnedForOverload = true;

                            events.ScheduleEvent(EVENT_CHECK_ENERGY, 1000);
                            break;
                        }
                        case EVENT_REND_FLESH:
                        {
                            if (Unit* victim = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                if (Creature* controller = GetController())
                                    me->CastSpell(victim, SPELL_REND_FLESH, true, NULL, NULL, controller->GetGUID());

                            events.ScheduleEvent(EVENT_REND_FLESH, urand(20000, 25000));
                            break;
                        }
                        case EVENT_MAIN_ATTACK:
                        {
                            switch (me->GetEntry())
                            {
                                case NPC_JADE:
                                {
                                    me->CastSpell(me, SPELL_JADE_SHARDS, false);
                                    events.ScheduleEvent(EVENT_MAIN_ATTACK, 8000);
                                    break;
                                }
                                case NPC_COBALT:
                                case NPC_AMETHYST:
                                {
                                    if (Unit* victim = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                        me->CastSpell(victim, spellMainAttack, false);

                                    events.ScheduleEvent(EVENT_MAIN_ATTACK, 10000);
                                    break;
                                }
                                case NPC_JASPER:
                                {
                                    std::list<Player*> playerList;
                                    std::list<Player*> tempPlayerList;
                                    GetPlayerListInGrid(playerList, me, 100.0f);

                                    // Chain shouldn`t try to take a player with tank spec as second target
                                    playerList.remove_if(TankSpecTargetSelector());
                                    // Remove GMs
                                    playerList.remove_if([](WorldObject* target) {
                                        return target->ToPlayer()->IsGameMaster();
                                    });

                                    for (auto&& player : playerList)
                                        if (player->IsAlive() && !player->HasAura(SPELL_JASPER_CHAINS) && !player->HasAura(SPELL_TOTALY_PETRIFIED))
                                            tempPlayerList.push_back(player);

                                    if (tempPlayerList.size() < 2)
                                        break;

                                    Trinity::Containers::RandomResizeList(tempPlayerList, 2);

                                    Player* firstPlayer = *tempPlayerList.begin();
                                    Player* SecondPlayer = *(++(tempPlayerList.begin()));

                                    if (!firstPlayer || !SecondPlayer)
                                        break;

                                    if (auto const aura = me->AddAura(SPELL_JASPER_CHAINS, firstPlayer))
                                        aura->SetScriptGuid(0, SecondPlayer->GetGUID());

                                    if (auto const aura = me->AddAura(SPELL_JASPER_CHAINS, SecondPlayer))
                                        aura->SetScriptGuid(0, firstPlayer->GetGUID());

                                    events.ScheduleEvent(EVENT_MAIN_ATTACK, 20000);
                                    break;
                                }
                            }

                            break;
                        }
                        case EVENT_ENRAGE:
                            me->CastSpell(me, SPELL_BERSERK, true);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_generic_guardianAI(creature);
        }
};

enum eMineSpell
{
    SPELL_COBALT_MINE_VISUAL    = 129455,
    SPELL_COBALT_MINE_EXPLOSION = 116281
};

// 65803 - Cobalt mine
class npc_cobalt_mine : public CreatureScript
{
    public:
        npc_cobalt_mine() : CreatureScript("npc_cobalt_mine") { }

        struct npc_cobalt_mineAI : public ScriptedAI
        {
            npc_cobalt_mineAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            void Reset() override
            {
                me->AddAura(SPELL_COBALT_MINE_VISUAL, me);

                uint32 delay = 0;
                me->m_Events.Schedule(delay += 3000, 1, [this]()
                {
                    DoCast(me, SPELL_COBALT_MINE_SELECTOR, true);
                });
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance && instance->GetBossState(DATA_STONE_GUARD) != IN_PROGRESS)
                    me->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cobalt_mineAI(creature);
        }
};

// Generic Living Crystals : 60304 (Cobalt) 60306 (Jade) 60307 (Jasper) 60308 (Amethyst)
class npc_living_crystal : public CreatureScript
{
    public:
        npc_living_crystal() : CreatureScript("npc_living_crystal") { }

        struct npc_living_crystalAI : public ScriptedAI
        {
            npc_living_crystalAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            void OnSpellClick(Unit* clicker, bool& /*result*/) override
            {
                if (Player* itr = clicker->ToPlayer())
                {
                    switch (me->GetEntry())
                    {
                        case NPC_LIVING_COBALT_CRYSTAL:
                            RemoveAnotherCrystalAura(itr);
                            itr->CastSpell(itr, SPELL_LIVING_COBALT, false);
                            if (auto cobalt = itr->GetAura(SPELL_LIVING_COBALT))
                                cobalt->SetStackAmount(10);
                            break;
                        case NPC_LIVING_AMETHYST_CRYSTAL:
                            RemoveAnotherCrystalAura(itr);
                            itr->CastSpell(itr, SPELL_LIVING_AMETHYST, false);
                            if (auto amethyst = itr->GetAura(SPELL_LIVING_AMETHYST))
                                amethyst->SetStackAmount(10);
                            break;
                        case NPC_LIVING_JADE_CRYSTAL:
                            RemoveAnotherCrystalAura(itr);
                            itr->CastSpell(itr, SPELL_LIVING_JADE, false);
                            if (auto jade = itr->GetAura(SPELL_LIVING_JADE))
                                jade->SetStackAmount(10);
                            break;
                        case NPC_LIVING_JASPER_CRYSTAL:
                            RemoveAnotherCrystalAura(itr);
                            itr->CastSpell(itr, SPELL_LIVING_JASPER, false);
                            if (auto jasper = itr->GetAura(SPELL_LIVING_JASPER))
                                jasper->SetStackAmount(10);
                            break;
                    }
                }
            }

        private:
            void RemoveAnotherCrystalAura(Unit* owner)
            {
                for (uint32 i = 0; i < CRYSTALL_AURA; i++)
                    if (owner->HasAura(LivingCrystals[i]))
                        owner->RemoveAura(LivingCrystals[i]);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_living_crystalAI(creature);
        }
};

// Petrification - 115852 / 116006 / 116036 / 116057
class spell_petrification : public SpellScriptLoader
{
    public:
        spell_petrification() : SpellScriptLoader("spell_petrification") { }

        class spell_petrification_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_petrification_AuraScript);

            void HandleTriggerSpell(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();

                Unit* caster = GetCaster();

                if (!caster)
                    return;

                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, caster, 200.0f);

                for (auto&& target: playerList)
                {
                    if (target->HasAura(SPELL_TOTALY_PETRIFIED))
                        continue;

                    uint32 triggeredSpell = GetSpellInfo()->Effects[0].TriggerSpell;

                    if (!target->HasAura(triggeredSpell))
                        caster->AddAura(triggeredSpell, target);

                    if (auto const triggeredAura = target->GetAura(triggeredSpell))
                    {
                        uint8 stackCount = triggeredAura->GetStackAmount();

                        uint8 newStackCount = (stackCount + 5 > 100) ? 100 : (stackCount + 5);
                        triggeredAura->SetStackAmount(newStackCount);
                        triggeredAura->RefreshDuration();
                        triggeredAura->RecalculateAmountOfEffects();

                        target->SetPower(POWER_ALTERNATE_POWER, newStackCount);

                        if (newStackCount >= 100)
                            caster->AddAura(SPELL_TOTALY_PETRIFIED, target);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_petrification_AuraScript::HandleTriggerSpell, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_petrification_AuraScript();
        }
};

// Jasper Chains - 130395
class spell_jasper_chains : public SpellScriptLoader
{
    public:
        spell_jasper_chains() : SpellScriptLoader("spell_jasper_chains") { }

        class spell_jasper_chains_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_jasper_chains_AuraScript);

            ObjectGuid playerLinkedGuid;

            bool Load() override
            {
                playerLinkedGuid = ObjectGuid::Empty;
                return true;
            }

            void SetGuid(uint32 /*type*/, ObjectGuid guid) override
            {
                playerLinkedGuid = guid;
            }

            void HandlePeriodic(AuraEffect const* /*aurEff*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetTarget();
                const SpellInfo* spell = GetSpellInfo();
                Player* linkedPlayer = ObjectAccessor::GetPlayer(*target, playerLinkedGuid);

                if (!caster || !target || !spell || !linkedPlayer || !linkedPlayer->IsAlive() || !linkedPlayer->HasAura(spell->Id))
                {
                    Remove();
                    return;
                }

                if (target->GetDistance(linkedPlayer) > spell->Effects[EFFECT_0].BasePoints)
                {
                    if (auto const aura = target->GetAura(spell->Id))
                    {
                        if (aura->GetStackAmount() >= 15)
                        {
                            aura->Remove();
                            return;
                        }
                    }

                    caster->AddAura(spell->Id, target);
                    target->CastSpell(linkedPlayer, SPELL_JASPER_CHAINS_DAMAGE, true);
                }
                else
                    target->CastSpell(linkedPlayer, SPELL_JASPER_CHAINS_VISUAL, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_jasper_chains_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_jasper_chains_AuraScript();
        }
};

// Jasper Chains (damage) - 130404
class spell_jasper_chains_damage : public SpellScriptLoader
{
    public:
        spell_jasper_chains_damage() : SpellScriptLoader("spell_jasper_chains_damage") { }

        class spell_jasper_chains_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_jasper_chains_damage_SpellScript);

            void DealDamage()
            {
                if (Unit* target = GetHitUnit())
                {
                    if (auto const aura = target->GetAura(SPELL_JASPER_CHAINS))
                    {
                        uint8 stacks = aura->GetStackAmount();
                        int32 damage = GetHitDamage();
                        uint32 damageAffix = target->GetInstanceScript()->instance->IsHeroic() ? 25 : 15;
                        uint32 reduceAffix = target->HasAura(SPELL_JASPER_PETRIFICATION) ? 0.1 : 1;

                        // Damage increased on 25/15% for each stack in Heroic/Normal mode. Reduce fire damage on 90% if jasper guardian is active
                        if (damage && stacks)
                            SetHitDamage((damage + (damage * stacks * damageAffix) / 100) * reduceAffix);
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_jasper_chains_damage_SpellScript::DealDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_jasper_chains_damage_SpellScript();
        }
};

// 116561 - Energized Tiles Script Effect
class spell_energized_tiles : public SpellScriptLoader
{
    public:
        spell_energized_tiles() : SpellScriptLoader("spell_energized_tiles") { }

        class spell_energized_tiles_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_energized_tiles_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                if (!caster || !target || !caster->HasAura(SPELL_TILES_AURA))
                    return;

                for (auto m_type : CrystalAuraType)
                {
                    if (!target->HasAura(m_type) || !caster->HasAura(SPELL_TILES_AURA))
                        continue;

                    if (Aura* m_aura = target->GetAura(m_type))
                    {
                        m_aura->SetStackAmount(m_aura->GetStackAmount() - 1);
                        caster->RemoveAura(SPELL_TILES_AURA);

                        if (!m_aura->GetStackAmount())
                            m_aura->Remove();

                        if (InstanceScript* instance = caster->GetInstanceScript())
                        {
                            if (Creature* controller = ObjectAccessor::GetCreature(*caster, instance->GetGuidData(NPC_STONE_GUARD_CONTROLLER)))
                                controller->CastSpell(caster, SPELL_TILES_DISPLAYED, false);

                            instance->DoCastSpellOnPlayers(SPELL_TILES_AURA_EFFECT, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_energized_tiles_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_energized_tiles_SpellScript();
        }
};

// Petrification Overload 115843, 115842, 115844, 115840
class spell_petrification_overload : public SpellScriptLoader
{
    public:
        spell_petrification_overload() : SpellScriptLoader("spell_petrification_overload") { }

        class spell_petrification_overload_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_petrification_overload_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* caster = GetCaster())
                    if (InstanceScript* instance = caster->GetInstanceScript())
                        if (Creature* controller = ObjectAccessor::GetCreature(*caster, instance->GetGuidData(NPC_STONE_GUARD_CONTROLLER)))
                            controller->AI()->DoAction(ACTION_PETRIFICATION);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_petrification_overload_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_petrification_overload_SpellScript();
        }
};

// Cobalt Mine Selector 129428
class spell_cobalt_mine_selector : public SpellScript
{
    PrepareSpellScript(spell_cobalt_mine_selector);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (!caster->HasAura(SPELL_COBALT_MINE_SELECTOR))
                return;

            if (Unit* target = GetHitUnit())
            {
                caster->RemoveAurasDueToSpell(SPELL_COBALT_MINE_SELECTOR);
                caster->CastSpell(caster, SPELL_COBALT_MINE_EXPLOSION, true);
                caster->DespawnOrUnsummon(700);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_cobalt_mine_selector::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 465. Summoned by 116235 - Amethyst Pool
class sat_amethyst_pool : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering->ToPlayer();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* pItr = triggering->ToPlayer())
            m_caster->CastSpell(pItr, SPELL_AMETHYST_POOL_AUR_EFFECT, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* pItr = triggering->ToPlayer())
            pItr->RemoveAurasDueToSpell(SPELL_AMETHYST_POOL_AUR_EFFECT);
    }
};

void AddSC_boss_stone_guard()
{
    new boss_stone_guard_controller();
    new boss_generic_guardian();
    new npc_cobalt_mine();
    new npc_living_crystal();
    new spell_petrification();
    new spell_jasper_chains();
    new spell_jasper_chains_damage();
    new spell_energized_tiles();
    new spell_petrification_overload();
    new spell_script<spell_cobalt_mine_selector>("spell_cobalt_mine_selector");
    new atrigger_script<sat_amethyst_pool>("sat_amethyst_pool");
}
