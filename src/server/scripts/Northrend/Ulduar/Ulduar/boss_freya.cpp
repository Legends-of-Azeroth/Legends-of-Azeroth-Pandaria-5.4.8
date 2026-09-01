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

#include "ScriptMgr.h"
#include "ulduar.h"

enum FreyaYells
{
    // Freya
    SAY_AGGRO               = 0,
    SAY_AGGRO_WITH_ELDER    = 1,
    SAY_SLAY                = 2,
    SAY_DEATH               = 3,
    SAY_BERSERK             = 4,
    SAY_SUMMON_CONSERVATOR  = 5,
    SAY_SUMMON_TRIO         = 6,
    SAY_SUMMON_LASHERS      = 7,

    // Elder Brightleaf
    SAY_BRIGHTLEAF_AGGRO    = 0,
    SAY_BRIGHTLEAF_SLAY     = 1,
    SAY_BRIGHTLEAF_DEATH    = 2,

    // Elder Ironbranch
    SAY_IRONBRANCH_AGGRO    = 0,
    SAY_IRONBRANCH_SLAY     = 1,
    SAY_IRONBRANCH_DEATH    = 2,

    // Elder Stonebark
    SAY_STONEBARK_AGGRO     = 0,
    SAY_STONEBARK_SLAY      = 1,
    SAY_STONEBARK_DEATH     = 2
};

enum FreyaSpells
{
    // Freya
    SPELL_ATTUNED_TO_NATURE                      = 62519,
    SPELL_TOUCH_OF_EONAR                         = 62528,
    SPELL_TOUCH_OF_EONAR_25                      = 62892,
    SPELL_SUNBEAM                                = 62623,
    SPELL_ENRAGE                                 = 47008,
    SPELL_FREYA_GROUND_TREMOR                    = 62437,
    SPELL_ROOTS_FREYA_10                         = 62438,
    SPELL_ROOTS_FREYA_25                         = 62861,
    SPELL_STONEBARK_ESSENCE                      = 62483,
    SPELL_STONEBARK_ESSENCE_25                   = 65589,
    SPELL_STONEBARK_ESSENCE_AURA                 = 62386,
    SPELL_IRONBRANCH_ESSENCE                     = 62484,
    SPELL_IRONBRANCH_ESSENCE_25                  = 65588,
    SPELL_IRONBRANCH_ESSENCE_AURA                = 62387,
    SPELL_BRIGHTLEAF_ESSENCE                     = 62485,
    SPELL_BRIGHTLEAF_ESSENCE_25                  = 65587,
    SPELL_BRIGHTLEAF_ESSENCE_AURA                = 62385,
    SPELL_DRAINED_OF_POWER                       = 62467,
    SPELL_SUMMON_EONAR_GIFT                      = 62572,

    // Stonebark
    SPELL_FISTS_OF_STONE                         = 62344,
    SPELL_GROUND_TREMOR                          = 62325,
    SPELL_PETRIFIED_BARK                         = 62337,
    SPELL_PETRIFIED_BARK_DMG                     = 62379,

    // Ironbranch
    SPELL_IMPALE                                 = 62310,
    SPELL_IRON_ROOTS_10                          = 62283,
    SPELL_IRON_ROOTS_25                          = 62930,
    SPELL_THORN_SWARM                            = 62285,

    // Brightleaf
    SPELL_FLUX_AURA                              = 62239,
    SPELL_FLUX                                   = 62262,
    SPELL_FLUX_PLUS                              = 62251,
    SPELL_FLUX_MINUS                             = 62252,
    SPELL_SOLAR_FLARE                            = 62240,
    SPELL_UNSTABLE_SUN_BEAM_SUMMON               = 62207, // Trigger 62221

    // Stack Removing of Attuned to Nature
    SPELL_REMOVE_25STACK                         = 62521,
    SPELL_REMOVE_10STACK                         = 62525,
    SPELL_REMOVE_2STACK                          = 62524,

    // Achievement spells
    SPELL_DEFORESTATION_CREDIT                   = 65015,
    SPELL_KNOCK_ON_WOOD_CREDIT                   = 65074,

    // Wave summoning spells
    SPELL_SUMMON_LASHERS                         = 62687,
    SPELL_SUMMON_TRIO                            = 62686,
    SPELL_SUMMON_ANCIENT_CONSERVATOR             = 62685,

    // Detonating Lasher
    SPELL_DETONATE                               = 62598,
    SPELL_FLAME_LASH                             = 62608,

    // Ancient Water Spirit
    SPELL_TIDAL_WAVE                             = 62653,
    SPELL_TIDAL_WAVE_EFFECT                      = 62654,

    // Storm Lasher
    SPELL_LIGHTNING_LASH                         = 62648,
    SPELL_STORMBOLT                              = 62649,

    // Snaplasher
    SPELL_HARDENED_BARK                          = 62664,
    SPELL_HARDENED_BARK_25                       = 64191,
    SPELL_BARK_AURA                              = 62663,

    // Ancient Conservator
    SPELL_CONSERVATOR_GRIP                       = 62532,
    SPELL_NATURES_FURY_10                        = 62589,
    SPELL_NATURES_FURY_25                        = 63571,
    SPELL_SUMMON_PERIODIC                        = 62566,
    SPELL_SPORE_SUMMON_NW                        = 62582, // Not used, triggered by SPELL_SUMMON_PERIODIC
    SPELL_SPORE_SUMMON_NE                        = 62591,
    SPELL_SPORE_SUMMON_SE                        = 62592,
    SPELL_SPORE_SUMMON_SW                        = 62593,

    // Healthly Spore
    SPELL_HEALTHY_SPORE_VISUAL                   = 62538,
    SPELL_GROW                                   = 62559,
    SPELL_POTENT_PHEROMONES                      = 62541,

    // Eonar's Gift
    SPELL_LIFEBINDERS_GIFT                       = 62584,
    SPELL_PHEROMONES                             = 62619,
    SPELL_EONAR_VISUAL                           = 62579,

    // Nature Bomb
    SPELL_NATURE_BOMB                            = 64587,
    SPELL_OBJECT_BOMB                            = 64600,
    SPELL_SUMMON_NATURE_BOMB                     = 64606,

    // Unstable Sun Beam
    SPELL_UNSTABLE_SUN_BEAM                      = 62211,
    SPELL_UNSTABLE_ENERGY                        = 62217,
    SPELL_PHOTOSYNTHESIS                         = 62209,
    SPELL_UNSTABLE_SUN_BEAM_TRIGGERED            = 62243,
    SPELL_FREYA_UNSTABLE_SUNBEAM                 = 62450, // Or maybe 62866?

    // Sun Beam
    SPELL_FREYA_UNSTABLE_ENERGY                  = 62451,
    SPELL_FREYA_UNSTABLE_ENERGY_VISUAL           = 62216,

    // Attuned To Nature spells
    SPELL_ATTUNED_TO_NATURE_2_DOSE_REDUCTION     = 62524,
    SPELL_ATTUNED_TO_NATURE_10_DOSE_REDUCTION    = 62525,
    SPELL_ATTUNED_TO_NATURE_25_DOSE_REDUCTION    = 62521,
};

#define SPELL_IRON_ROOTS    RAID_MODE(SPELL_IRON_ROOTS_10, SPELL_IRON_ROOTS_25)
#define SPELL_ROOTS_FREYA   RAID_MODE(SPELL_ROOTS_FREYA_10, SPELL_ROOTS_FREYA_25)
#define SPELL_NATURES_FURY  RAID_MODE(SPELL_NATURES_FURY_10, SPELL_NATURES_FURY_25)

enum FreyaNpcs
{
    NPC_SUN_BEAM                                 = 33170,
    NPC_DETONATING_LASHER                        = 32918,
    NPC_ANCIENT_CONSERVATOR                      = 33203,
    NPC_ANCIENT_WATER_SPIRIT                     = 33202,
    NPC_STORM_LASHER                             = 32919,
    NPC_SNAPLASHER                               = 32916,
    NPC_NATURE_BOMB                              = 34129,
    NPC_EONARS_GIFT                              = 33228,
    NPC_HEALTHY_SPORE                            = 33215,
    NPC_UNSTABLE_SUN_BEAM                        = 33050,
    NPC_IRON_ROOTS                               = 33088,
    NPC_STRENGTHENED_IRON_ROOTS                  = 33168,

    OBJECT_NATURE_BOMB                           = 194902,
};

enum FreyaActions
{
    ACTION_ELDER_DEATH          = 1,
    ACTION_ELDER_FREYA_KILLED   = 2,
    ACTION_ELEMENTAL_DEAD       = 3,

    ACTION_BEGIN_ATTACK         = 4
};

enum FreyaEvents
{
    // Freya
    EVENT_WAVE                                   = 1,
    EVENT_EONAR_GIFT                             = 2,
    EVENT_NATURE_BOMB                            = 3,
    EVENT_UNSTABLE_ENERGY                        = 4,
    EVENT_STRENGTHENED_IRON_ROOTS                = 5,
    EVENT_GROUND_TREMOR                          = 6,
    EVENT_SUNBEAM                                = 7,
    EVENT_ENRAGE                                 = 8,

    // Elder Stonebark
    EVENT_TREMOR                                 = 9,
    EVENT_BARK                                   = 10,
    EVENT_FISTS                                  = 11,

    // Elder Ironbranch
    EVENT_IMPALE                                 = 12,
    EVENT_IRON_ROOTS                             = 13,
    EVENT_THORN_SWARM                            = 14,

    // Elder Brightleaf
    EVENT_SOLAR_FLARE                            = 15,
    EVENT_UNSTABLE_SUN_BEAM                      = 16,
    EVENT_FLUX                                   = 17,

    // Detonating Lasher
    EVENT_LASH                                  = 18,
    EVENT_CHANGE_TARGET                         = 19,

    // Storm Lasher
    EVENT_LIGHTNING_LASH                        = 20,
    EVENT_STORMBOLT                             = 21,

    // Ancient Conservator
    EVENT_NATURES_FURY                          = 22,
    EVENT_HEALTHY_SPORE                         = 23
};

enum Data
{
    DATA_GETTING_BACK_TO_NATURE     = 1,
    DATA_KNOCK_ON_WOOD              = 2,
    DATA_ELEMENTAL_DIED             = 3,
    DATA_LASHER_DIED                = 4,
    DATA_TRIO_WAVE_COUNT            = 5
};

#define WAVE_TIME                                60000 // Normal wave is one minute
#define TIME_DIFFERENCE                          10000 // If difference between waveTime and WAVE_TIME is bigger then TIME_DIFFERENCE, schedule EVENT_WAVE in 10 seconds

class boss_freya : public CreatureScript
{
    public:
        boss_freya() : CreatureScript("boss_freya") { }

        struct boss_freyaAI : public BossAI
        {
            boss_freyaAI(Creature* creature) : BossAI(creature, BOSS_FREYA)
            {
               _encounterFinished = false; 
            }

            bool _checkElementalAlive[2];
            void Reset() override
            {
                _encounterFinished = _encounterFinished || (instance && instance->GetBossState(BOSS_FREYA) == DONE);
                if (_encounterFinished) // May be called during fight if Freya gets outfight... hm, should _not_ happen regularly
                {
                    me->SetFaction(35);
                    return;
                }
                _Reset();
                _trioWaveCount = 0;
                _trioWaveController = 0;
                _waveNumber = 0;
                _elderCount = 0;
                me->GetMap()->SetWorldState(WORLDSTATE_KNOCK_ON_WOOD, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_KNOCK_ON_WOOD_x2, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_KNOCK_ON_WOOD_x3, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_GETTING_BACK_TO_NATURE, 0);

                for (uint8 i = 0; i < 3; ++i)
                    for (uint8 n = 0; n < 2; ++n)
                        _elementalGUID[i][n] = ObjectGuid::Empty;

                for (uint8 i = 0; i < 6; ++i)
                    for (uint8 n = 0; n < 2; ++n)
                        _deforestation[i][n] = 0;

                for (uint8 n = 0; n < 2; ++n)
                {
                    _checkElementalAlive[n] = true;
                    _trioDefeated[n] = false;
                }

                for (uint8 n = 0; n < 3; ++n)
                {
                    _random[n] = false;

                    if (Creature* elder = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_BRIGHTLEAF + n)))
                    {
                        if (elder->IsAlive())
                        {
                            elder->ResetLootMode();
                            elder->AI()->EnterEvadeMode();
                        }
                    }
                } 
            }

            void KilledUnit(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_SLAY);
            }

            void DamageTaken(Unit* who, uint32& damage) override
            {
                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    EncounterPostProgress();
                }
            }

            void EncounterPostProgress()
            {
                if (_encounterFinished)
                    return;

                _encounterFinished = true;

                // Freya's chest is dynamically spawned on death by different spells.
                const uint32 summonSpell[2][4] =
                {
                    /* 0Elder, 1Elder, 2Elder, 3Elder killed */
                    /* 10N */ {62950, 62952, 62953, 62954},
                    /* 25N */ {62955, 62956, 62957, 62958}
                };
                    me->CastSpell((Unit*)nullptr, summonSpell[RAID_MODE(0, 1)][_elderCount], true); // RAID_MODE should return 0 or 1 (numeric)

                Talk(SAY_DEATH);
                me->SetReactState(REACT_PASSIVE);
                me->RemoveAllAuras();
                me->AttackStop();
                me->SetFaction(35);
                DoCastAOE(SPELL_KNOCK_ON_WOOD_CREDIT, true); // Requires friendly targets, but must be used before CombatStop in which the boss will reset its encounter state
                me->DeleteThreatList();
                me->CombatStop(true);

                // getting back to nature achievement
                _attunedToNature = me->GetAuraCount(SPELL_ATTUNED_TO_NATURE);

                _JustDied();

                Creature* Elder[3];
                for (uint8 n = 0; n < 3; ++n)
                {
                    Elder[n] = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_BRIGHTLEAF + n));
                    if (Elder[n] && Elder[n]->IsAlive())
                    {
                        Elder[n]->RemoveAllAuras();
                        Elder[n]->AttackStop();
                        Elder[n]->CombatStop(true);
                        Elder[n]->DeleteThreatList();
                        Elder[n]->GetAI()->DoAction(ACTION_ELDER_FREYA_KILLED);
                    }
                }
                me->DespawnOrUnsummon(7.5*IN_MILLISECONDS);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!instance->CheckRequiredBosses(BOSS_FREYA, who->ToPlayer()))
                {
                    //EnterEvadeMode();
                    me->DisappearAndDie();
                    return;
                }

                _JustEngagedWith();
                DoZoneInCombat();

                _elderCount = 0;
                if (Creature* Brightleaf = me->GetCreature(*me, instance->GetGuidData(BOSS_BRIGHTLEAF)))
                    if (Brightleaf->IsAlive())
                    {
                        _elderCount++;
                        Brightleaf->AI()->AttackStart(who);
                        Brightleaf->AddThreat(who, 250.0f);
                        Brightleaf->SetInCombatWith(who);
                        Brightleaf->CastSpell(Brightleaf, SPELL_DRAINED_OF_POWER, true);
                        Brightleaf->CastSpell(me, Is25ManRaid() ? SPELL_BRIGHTLEAF_ESSENCE_25 : SPELL_BRIGHTLEAF_ESSENCE, true);
                        events.ScheduleEvent(EVENT_UNSTABLE_ENERGY, urand(15000, 30000));
                    }

                if (Creature* Ironbranch = me->GetCreature(*me, instance->GetGuidData(BOSS_IRONBRANCH)))
                    if (Ironbranch->IsAlive())
                    {
                        _elderCount++;
                        Ironbranch->AI()->AttackStart(who);
                        Ironbranch->AddThreat(who, 250.0f);
                        Ironbranch->SetInCombatWith(who);
                        Ironbranch->CastSpell(Ironbranch, SPELL_DRAINED_OF_POWER, true);
                        Ironbranch->CastSpell(me, Is25ManRaid() ? SPELL_IRONBRANCH_ESSENCE_25 : SPELL_IRONBRANCH_ESSENCE, true);
                        events.ScheduleEvent(EVENT_STRENGTHENED_IRON_ROOTS, urand(45000, 60000));
                    }

                if (Creature* Stonebark = me->GetCreature(*me, instance->GetGuidData(BOSS_STONEBARK)))
                    if (Stonebark->IsAlive())
                    {
                        _elderCount++;
                        Stonebark->AI()->AttackStart(who);
                        Stonebark->AddThreat(who, 250.0f);
                        Stonebark->SetInCombatWith(who);
                        Stonebark->CastSpell(Stonebark, SPELL_DRAINED_OF_POWER, true);
                        Stonebark->CastSpell(me, Is25ManRaid() ? SPELL_STONEBARK_ESSENCE_25 : SPELL_STONEBARK_ESSENCE, true);
                        events.ScheduleEvent(EVENT_GROUND_TREMOR, urand(35000, 45000));
                    }

                if (_elderCount >= 1)
                    me->GetMap()->SetWorldState(WORLDSTATE_KNOCK_ON_WOOD, 1);

                if (_elderCount >= 2)
                    me->GetMap()->SetWorldState(WORLDSTATE_KNOCK_ON_WOOD_x2, 1);

                if (_elderCount == 3)
                    me->GetMap()->SetWorldState(WORLDSTATE_KNOCK_ON_WOOD_x3, 1);

                if (_elderCount == 0)
                    Talk(SAY_AGGRO);
                else
                    Talk(SAY_AGGRO_WITH_ELDER);

                me->CastCustomSpell(SPELL_ATTUNED_TO_NATURE, SPELLVALUE_AURA_STACK, 150, me, true);

                events.ScheduleEvent(EVENT_WAVE, 10000);
                events.ScheduleEvent(EVENT_EONAR_GIFT, 25000);
                events.ScheduleEvent(EVENT_ENRAGE, 600000);
                events.ScheduleEvent(EVENT_SUNBEAM, urand(5000, 15000));
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_GETTING_BACK_TO_NATURE:
                        return _attunedToNature;
                    case DATA_KNOCK_ON_WOOD:
                        return _elderCount;
                    case DATA_TRIO_WAVE_COUNT:
                        return _trioWaveCount;
                    default:
                        break;
                }

                return 0;
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
                        case EVENT_ENRAGE:
                            Talk(SAY_BERSERK);
                            DoCast(me, SPELL_ENRAGE);
                            break;
                        case EVENT_SUNBEAM:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(target, SPELL_SUNBEAM);
                            events.ScheduleEvent(EVENT_SUNBEAM, urand(10000, 15000));
                            break;
                        case EVENT_NATURE_BOMB:
                        {
                            // On every player
                            std::list<Player*> playerList;
                            Trinity::AnyPlayerInObjectRangeCheck checker(me, 50.0f);
                            Trinity::PlayerListSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, playerList, checker);
                            me->VisitNearbyWorldObject(50.0f, searcher);
                            for (auto&& player : playerList)
                                player->CastSpell(player, SPELL_SUMMON_NATURE_BOMB, true);
                            events.ScheduleEvent(EVENT_NATURE_BOMB, urand(10000, 12000));
                            break;
                        }
                        case EVENT_UNSTABLE_ENERGY:
                            for (int8 n = 0; n < 3; n++)
                            {
                                Position pos = me->GetRandomNearPosition(30);
                                me->SummonCreature(NPC_SUN_BEAM, pos, TEMPSUMMON_TIMED_DESPAWN, 10000ms);
                            }
                            events.ScheduleEvent(EVENT_UNSTABLE_ENERGY, urand(35000, 45000));
                            break;
                        case EVENT_WAVE:
                            if (_waveNumber < 6)
                            {
                                SpawnWave();
                                events.ScheduleEvent(EVENT_WAVE, WAVE_TIME);
                                _waveNumber++;
                            }
                            else
                            {
                                events.ScheduleEvent(EVENT_NATURE_BOMB, urand(10000, 20000));
                                events.CancelEvent(EVENT_WAVE);
                            }
                            break;
                        case EVENT_EONAR_GIFT:
                            DoCast(me, SPELL_SUMMON_EONAR_GIFT);
                            events.ScheduleEvent(EVENT_EONAR_GIFT, urand(40000, 50000));
                            break;
                        case EVENT_STRENGTHENED_IRON_ROOTS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true, -SPELL_ROOTS_FREYA))
                                target->CastSpell(target, SPELL_ROOTS_FREYA, true); // This must be casted by Target self
                            events.ScheduleEvent(EVENT_STRENGTHENED_IRON_ROOTS, urand(12000, 20000));
                            break;
                        case EVENT_GROUND_TREMOR:
                            DoCastAOE(SPELL_FREYA_GROUND_TREMOR);
                            events.ScheduleEvent(EVENT_GROUND_TREMOR, urand(25000, 28000));
                            break;
                    }
                }

                if (!me->HasAura(Is25ManRaid() ? SPELL_TOUCH_OF_EONAR_25 : SPELL_TOUCH_OF_EONAR))
                    me->CastSpell(me, Is25ManRaid() ? SPELL_TOUCH_OF_EONAR_25 : SPELL_TOUCH_OF_EONAR, true);

                // For achievement check
                if (Aura* aura = me->GetAura(SPELL_ATTUNED_TO_NATURE))
                {
                    _attunedToNature = aura->GetStackAmount();

                    if (_attunedToNature >= 25 && me->GetMap()->GetWorldState(WORLDSTATE_GETTING_BACK_TO_NATURE) != 1)
                        me->GetMap()->SetWorldState(WORLDSTATE_GETTING_BACK_TO_NATURE, 1);
                    else if (_attunedToNature < 25 && me->GetMap()->GetWorldState(WORLDSTATE_GETTING_BACK_TO_NATURE) > 0)
                        me->GetMap()->SetWorldState(WORLDSTATE_GETTING_BACK_TO_NATURE, 0);
                }
                else
                {
                    if (me->GetMap()->GetWorldState(WORLDSTATE_GETTING_BACK_TO_NATURE) > 0)
                        me->GetMap()->SetWorldState(WORLDSTATE_GETTING_BACK_TO_NATURE, 0);

                    _attunedToNature = 0;
                }

                _diffTimer += diff;                                               // For getting time difference for Deforestation achievement

                // Elementals must be killed within 12 seconds of each other, or they will all revive and heal
                Creature* Elemental[3][2];
                for (uint8 i = 0; i < 2; ++i)
                {
                    if (_checkElementalAlive[i])
                        _elementalTimer[i] = 0;
                    else
                    {
                        _elementalTimer[i] += diff;
                        for (uint8 k = 0; k < 3; ++k)
                            Elemental[k][i] = ObjectAccessor::GetCreature(*me, _elementalGUID[k][i]);
                        if (_elementalTimer[i] > 12000)
                        {
                            if (!_trioDefeated[i]) // Do *NOT* merge this bool with bool few lines below!
                            {
                                if (Elemental[0][i] && Elemental[1][i] && Elemental[2][i])
                                {
                                    for (uint8 n = 0; n < 3; ++n)
                                    {
                                        if (Elemental[n][i]->IsAlive())
                                            Elemental[n][i]->SetHealth(Elemental[n][i]->GetMaxHealth());
                                        else
                                            Elemental[n][i]->Respawn();
                                    }
                                }
                            }
                            _checkElementalAlive[i] = true;
                        }
                        else
                        {
                            if (!_trioDefeated[i])
                            {
                                if (Elemental[0][i] && Elemental[1][i] && Elemental[2][i])
                                {
                                    if (Elemental[0][i]->isDead() && Elemental[1][i]->isDead() && Elemental[2][i]->isDead())
                                    {
                                        for (uint8 n = 0; n < 3; ++n)
                                        {
                                            summons.Despawn(Elemental[n][i]);
                                            Elemental[n][i]->DespawnOrUnsummon(5000);
                                            _trioDefeated[i] = true;
                                            Elemental[n][i]->CastSpell(me, SPELL_REMOVE_10STACK, true);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                DoMeleeAttackIfReady();
            }

            // Check if all Trio NPCs are dead - achievement check
            void LasherDead(uint32 type)                                         // Type must be in format of a binary mask
            {
                uint8 n = 0;

                // Handling received data
                for (uint8 i = 0; i < 5; ++i)                                    // We have created "instances" for keeping informations about last 6 death lashers - needed because of respawning
                {
                    _deforestation[i][0] = _deforestation[(i + 1)][0];             // Time
                    _deforestation[i][1] = _deforestation[(i + 1)][1];             // Type
                }
                _deforestation[5][0] = _diffTimer;
                _deforestation[5][1] = type;

                // Check for achievement completion
                if (_deforestation[0][1])                                         // Check for proper functionality of binary masks (overflow would not be problem)
                {
                    for (uint8 i = 0; i < 6; ++i)                                // Count binary mask
                    {
                        n += _deforestation[i][1];
                    }
                    if ((_deforestation[5][0] - _deforestation[0][0]) < 10000)     // Time check
                    {
                        if (n == 14 && instance)                                 // Binary mask check - verification of lasher types
                        {
                            instance->DoCastSpellOnPlayers(SPELL_DEFORESTATION_CREDIT);
                        }
                    }
                }
            }

            // Random order of spawning waves
            int GetWaveId()
            {
                if (_random[0] && _random[1] && _random[2])
                    for (uint8 n = 0; n < 3; ++n)
                        _random[n] = false;

                uint8 randomId = urand(0, 2);

                while (_random[randomId])
                    randomId = urand(0, 2);

                _random[randomId] = true;
                return randomId;
            }

            void SpawnWave()
            {
                switch (GetWaveId())
                {
                    case 0:
                        Talk(SAY_SUMMON_LASHERS);
                        for (uint8 n = 0; n < 10; ++n)
                            DoCast(SPELL_SUMMON_LASHERS);
                        break;
                    case 1:
                        Talk(SAY_SUMMON_TRIO);
                        DoCast(SPELL_SUMMON_TRIO);
                        _trioWaveCount++;
                        break;
                    case 2:
                        Talk(SAY_SUMMON_CONSERVATOR);
                        DoCast(SPELL_SUMMON_ANCIENT_CONSERVATOR);
                        break;
                    default:
                        break;
                }
            }

            void JustSummoned(Creature* summoned) override
            {
                switch (summoned->GetEntry())
                {
                    case NPC_SNAPLASHER:
                    case NPC_ANCIENT_WATER_SPIRIT:
                    case NPC_STORM_LASHER:
                        _elementalGUID[_trioWaveController][_trioWaveCount] = summoned->GetGUID();
                        summons.Summon(summoned);
                        ++_trioWaveController;
                        if (_trioWaveController > 2)
                            _trioWaveController = 0;
                        // DO NOT BREAK!!!
                    case NPC_DETONATING_LASHER:
                        summoned->SetReactState(REACT_PASSIVE);
                        summoned->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_SUBMERGED);
                        summoned->m_Events.Schedule(5000, [summoned]() { summoned->AI()->DoAction(ACTION_BEGIN_ATTACK); });
                        return;
                    case NPC_ANCIENT_CONSERVATOR:
                        break;
                    default:
                        summons.Summon(summoned);
                        break;
                }

                // Need to have it there, or summoned units would do nothing until attacked
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 250.0f, true))
                {
                    summoned->AI()->AttackStart(target);
                    summoned->AddThreat(target, 250.0f);
                    DoZoneInCombat(summoned);
                }
            }

            void SummonedCreatureDespawn(Creature* summoned) override
            {
                switch (summoned->GetEntry())
                {
                    case NPC_ANCIENT_CONSERVATOR:
                        summoned->CastSpell(me, SPELL_REMOVE_25STACK, true);
                        summoned->DespawnOrUnsummon(5*IN_MILLISECONDS);
                        summons.Despawn(summoned);
                        break;
                    default:
                        break;
                }
            }

            void SummonedCreatureDies(Creature* summoned, Unit* killer) override
            {
                switch (summoned->GetEntry())
                {
                    case NPC_DETONATING_LASHER:
                        summoned->CastSpell(me, SPELL_REMOVE_2STACK, true);
                        summoned->CastSpell(killer, SPELL_DETONATE, true);
                        summoned->DespawnOrUnsummon(5*IN_MILLISECONDS);
                        summons.Despawn(summoned);
                        break;
                    case NPC_ANCIENT_CONSERVATOR:
                        SummonedCreatureDespawn(summoned); // To avoid duplicated code
                        break;
                    default:
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_ELEMENTAL_DIED:
                        _checkElementalAlive[data] = false;
                        break;
                    case DATA_LASHER_DIED:
                        LasherDead(data);
                        break;
                    default:
                        break;
                }
            }

            void SetGUID(ObjectGuid guid, int32 id) override
            {
                switch (id)
                {
                    case ACTION_ELEMENTAL_DEAD:
                        if (Creature* c = ObjectAccessor::GetCreature(*me, guid))
                        {
                            switch (c->GetEntry())
                            {
                                case NPC_DETONATING_LASHER:
                                    DoCast(me, SPELL_ATTUNED_TO_NATURE_2_DOSE_REDUCTION, true);
                                    break;
                                case NPC_ANCIENT_WATER_SPIRIT:
                                case NPC_STORM_LASHER:
                                case NPC_SNAPLASHER:
                                    DoCast(me, SPELL_ATTUNED_TO_NATURE_10_DOSE_REDUCTION, true);
                                    break;
                                case NPC_ANCIENT_CONSERVATOR:
                                    DoCast(me, SPELL_ATTUNED_TO_NATURE_25_DOSE_REDUCTION, true);
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case ACTION_ELDER_DEATH: // For each dying elder, Freya looses one ability.
                        if (_elderCount > 0)
                            --_elderCount;

                        if (_elderCount < 1)
                            me->GetMap()->SetWorldState(WORLDSTATE_KNOCK_ON_WOOD, 0);

                        if (_elderCount < 2)
                            me->GetMap()->SetWorldState(WORLDSTATE_KNOCK_ON_WOOD_x2, 0);

                        if (_elderCount < 3)
                            me->GetMap()->SetWorldState(WORLDSTATE_KNOCK_ON_WOOD_x3, 0);

                        if (Creature* c = ObjectAccessor::GetCreature(*me, guid))
                        {
                            switch (c->GetEntry())
                            {
                                case NPC_ELDER_BRIGHTLEAF:
                                    events.CancelEvent(EVENT_UNSTABLE_ENERGY);
                                    break;
                                case NPC_ELDER_IRONBRANCH:
                                    events.CancelEvent(EVENT_STRENGTHENED_IRON_ROOTS);
                                    break;
                                case NPC_ELDER_STONEBARK:
                                    events.CancelEvent(EVENT_GROUND_TREMOR);
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    default:
                        break;
                }
            }

            private:
                ObjectGuid _elementalGUID[3][2];

                uint32 _deforestation[6][2];
                uint32 _elementalTimer[2];
                uint32 _diffTimer;
                uint8 _trioWaveCount;
                uint8 _trioWaveController;
                uint8 _elderCount;
                uint8 _attunedToNature;
                uint8 _waveNumber;

                bool _trioDefeated[2];
                bool _random[3];
                bool _encounterFinished;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_freyaAI>(creature);
        }
};

class boss_elder_brightleaf : public CreatureScript
{
    public:
        boss_elder_brightleaf() : CreatureScript("boss_elder_brightleaf") { }

        struct boss_elder_brightleafAI : public BossAI
        {
            boss_elder_brightleafAI(Creature* creature) : BossAI(creature, BOSS_BRIGHTLEAF) { }

            void Reset() override
            {
                _Reset();
                if (me->HasAura(SPELL_DRAINED_OF_POWER))
                    me->RemoveAurasDueToSpell(SPELL_DRAINED_OF_POWER);
                events.ScheduleEvent(EVENT_SOLAR_FLARE, urand(5000, 7000));
                events.ScheduleEvent(EVENT_UNSTABLE_SUN_BEAM, urand(7000, 12000));
                events.ScheduleEvent(EVENT_FLUX, 5000);
                _elderCount = 0;
                _lumberjack = false;
            }

            void KilledUnit(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_BRIGHTLEAF_SLAY);
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();
                Talk(SAY_BRIGHTLEAF_DEATH);

                if (killer && killer->GetTypeId() == TYPEID_PLAYER)
                {
                    if (Creature* Ironbranch = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_IRONBRANCH)))
                        Ironbranch->AI()->DoAction(ACTION_ELDER_DEATH);

                    if (Creature* Stonebark = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_STONEBARK)))
                        Stonebark->AI()->DoAction(ACTION_ELDER_DEATH);

                    if (Creature* Freya = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_FREYA)))
                        Freya->AI()->SetGUID(me->GetGUID(), ACTION_ELDER_DEATH);
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                if (!me->HasAura(SPELL_DRAINED_OF_POWER))
                    Talk(SAY_BRIGHTLEAF_AGGRO);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || me->HasAura(SPELL_DRAINED_OF_POWER))
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_UNSTABLE_SUN_BEAM:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                                if (target->IsAlive())
                                    me->SummonCreature(NPC_UNSTABLE_SUN_BEAM, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 10000ms);
                            events.ScheduleEvent(EVENT_UNSTABLE_SUN_BEAM, 8000);
                            break;
                        case EVENT_SOLAR_FLARE:
                        {
                            uint8 stackAmount = 0;
                            if (me->GetAura(SPELL_FLUX_AURA))
                                stackAmount = me->GetAura(SPELL_FLUX_AURA)->GetStackAmount();
                            me->CastCustomSpell(SPELL_SOLAR_FLARE, SPELLVALUE_MAX_TARGETS, stackAmount, me, false);
                            events.ScheduleEvent(EVENT_SOLAR_FLARE, urand(5000, 10000));
                            break;
                        }
                        case EVENT_FLUX:
                            me->RemoveAurasDueToSpell(SPELL_FLUX_AURA);
                            me->RemoveAurasDueToSpell(SPELL_FLUX_PLUS);
                            me->RemoveAurasDueToSpell(SPELL_FLUX_MINUS);

                            me->AddAura(SPELL_FLUX_AURA, me);
                            if (Aura* Flux = me->GetAura(SPELL_FLUX_AURA))
                                Flux->SetStackAmount(urand(1, 8));
                            events.ScheduleEvent(EVENT_FLUX, 7500);
                            break;
                    }
                }

                if (_lumberjack)
                    _lumberjackTimer += diff;

                DoMeleeAttackIfReady();
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_ELDER_DEATH:
                        ++_elderCount;
                        _lumberjack = true;
                        break;
                    case ACTION_ELDER_FREYA_KILLED:
                        me->DespawnOrUnsummon(1000);
                         _JustDied();
                        break;
                }
            }

        private:
            uint32 _lumberjackTimer;
            uint8 _elderCount;
            bool _lumberjack;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_elder_brightleafAI>(creature);
        }
};

class boss_elder_stonebark : public CreatureScript
{
    public:
        boss_elder_stonebark() : CreatureScript("boss_elder_stonebark") { }

        struct boss_elder_stonebarkAI : public BossAI
        {
            boss_elder_stonebarkAI(Creature* creature) : BossAI(creature, BOSS_STONEBARK) { }

            void Reset() override
            {
                _Reset();
                if (me->HasAura(SPELL_DRAINED_OF_POWER))
                    me->RemoveAurasDueToSpell(SPELL_DRAINED_OF_POWER);
                events.ScheduleEvent(EVENT_TREMOR, urand(10000, 12000));
                events.ScheduleEvent(EVENT_FISTS, urand(25000, 35000));
                events.ScheduleEvent(EVENT_BARK, urand(37500, 40000));
                _elderCount = 0;
                _lumberjack = false;
            }

            void KilledUnit(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_STONEBARK_SLAY);
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();
                Talk(SAY_STONEBARK_DEATH);

                if (killer && killer->GetTypeId() == TYPEID_PLAYER)
                {
                    if (Creature* Ironbranch = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_IRONBRANCH)))
                        Ironbranch->AI()->DoAction(ACTION_ELDER_DEATH);

                    if (Creature* Brightleaf = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_BRIGHTLEAF)))
                        Brightleaf->AI()->DoAction(ACTION_ELDER_DEATH);

                    if (Creature* Freya = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_FREYA)))
                        Freya->AI()->SetGUID(me->GetGUID(), ACTION_ELDER_DEATH);
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                if (!me->HasAura(SPELL_DRAINED_OF_POWER))
                    Talk(SAY_STONEBARK_AGGRO);
            }

            void DamageTaken(Unit* who, uint32& damage) override
            {
                if (who == me)
                    return;

                if (me->HasAura(SPELL_PETRIFIED_BARK))
                {
                    int32 reflect = damage;
                    who->CastCustomSpell(who, SPELL_PETRIFIED_BARK_DMG, &reflect, nullptr, nullptr, true);
                    damage = 0;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || me->HasAura(SPELL_DRAINED_OF_POWER))
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BARK:
                            DoCast(me, SPELL_PETRIFIED_BARK);
                            events.ScheduleEvent(EVENT_BARK, urand(30000, 50000));
                            break;
                        case EVENT_FISTS:
                            DoCastVictim(SPELL_FISTS_OF_STONE);
                            events.ScheduleEvent(EVENT_FISTS, urand(20000, 30000));
                            break;
                        case EVENT_TREMOR:
                            if (!me->HasAura(SPELL_FISTS_OF_STONE))
                                DoCastVictim(SPELL_GROUND_TREMOR);
                            events.ScheduleEvent(EVENT_TREMOR, urand(10000, 20000));
                            break;
                    }
                }

                if (_lumberjack)
                    _lumberjackTimer += diff;

                DoMeleeAttackIfReady();
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_ELDER_DEATH:
                        ++_elderCount;
                        _lumberjack = true;
                        break;
                    case ACTION_ELDER_FREYA_KILLED:
                        me->DespawnOrUnsummon(1000);
                         _JustDied();
                        break;
                }
            }

        private:
            uint32 _lumberjackTimer;
            uint8 _elderCount;
            bool _lumberjack;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_elder_stonebarkAI>(creature);
        }
};

class boss_elder_ironbranch : public CreatureScript
{
    public:
        boss_elder_ironbranch() : CreatureScript("boss_elder_ironbranch") { }

        struct boss_elder_ironbranchAI : public BossAI
        {
            boss_elder_ironbranchAI(Creature* creature) : BossAI(creature, BOSS_IRONBRANCH) { }

            void Reset() override
            {
                _Reset();
                if (me->HasAura(SPELL_DRAINED_OF_POWER))
                    me->RemoveAurasDueToSpell(SPELL_DRAINED_OF_POWER);
                events.ScheduleEvent(EVENT_IMPALE, urand(18000, 22000));
                events.ScheduleEvent(EVENT_IRON_ROOTS, urand(12000, 17000));
                events.ScheduleEvent(EVENT_THORN_SWARM, urand(7500, 12500));
                _elderCount = 0;
                _lumberjack = false;
            }

            void KilledUnit(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_IRONBRANCH_SLAY);
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();
                Talk(SAY_IRONBRANCH_DEATH);

                if (killer && killer->GetTypeId() == TYPEID_PLAYER)
                {
                    if (Creature* Brightleaf = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_BRIGHTLEAF)))
                        Brightleaf->AI()->DoAction(ACTION_ELDER_DEATH);

                    if (Creature* Stonebark = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_STONEBARK)))
                        Stonebark->AI()->DoAction(ACTION_ELDER_DEATH);

                    if (Creature* Freya = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_FREYA)))
                        Freya->AI()->SetGUID(me->GetGUID(), ACTION_ELDER_DEATH);
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                if (!me->HasAura(SPELL_DRAINED_OF_POWER))
                    Talk(SAY_IRONBRANCH_AGGRO);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || me->HasAura(SPELL_DRAINED_OF_POWER))
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_IMPALE:
                            DoCastVictim(SPELL_IMPALE);
                            events.ScheduleEvent(EVENT_IMPALE, urand(15000, 25000));
                            break;
                        case EVENT_IRON_ROOTS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true, -SPELL_IRON_ROOTS))
                                target->CastSpell(target, SPELL_IRON_ROOTS, true);
                            events.ScheduleEvent(EVENT_IRON_ROOTS, urand(10000, 20000));
                            break;
                        case EVENT_THORN_SWARM:
                            DoCastVictim(SPELL_THORN_SWARM);
                            events.ScheduleEvent(EVENT_THORN_SWARM, urand(8000, 13000));
                            break;
                    }
                }

                if (_lumberjack)
                    _lumberjackTimer += diff;

                DoMeleeAttackIfReady();
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_ELDER_DEATH:
                        ++_elderCount;
                        _lumberjack = true;
                        break;
                    case ACTION_ELDER_FREYA_KILLED:
                        me->DespawnOrUnsummon(1000);
                         _JustDied();
                        break;
                }
            }

        private:
            uint32 _lumberjackTimer;
            uint8 _elderCount;
            bool _lumberjack;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_elder_ironbranchAI>(creature);
        }
};

class npc_iron_roots : public CreatureScript
{
    public:
        npc_iron_roots() : CreatureScript("npc_iron_roots") { }

        struct npc_iron_rootsAI : public ScriptedAI
        {
            npc_iron_rootsAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }
            
            void InitializeAI() override
            {
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_ID, 49560, true);  // Death Grip
                me->SetFaction(14);
                me->SetReactState(REACT_PASSIVE);
                Reset();
            }

            void Reset() override
            {
                _summonerGUID = ObjectGuid::Empty;
            }

            void IsSummonedBy(Unit* summoner) override
            {
                if (summoner->GetTypeId() != TYPEID_PLAYER)
                    return;
                // Summoner is a player, who should have root aura on self
                _summonerGUID = summoner->GetGUID();
                me->SetFacingToObject(summoner);
                me->SetInCombatWith(summoner);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Player* target = ObjectAccessor::GetPlayer(*me, _summonerGUID))
                {
                    target->RemoveAurasDueToSpell(SPELL_IRON_ROOTS);
                    target->RemoveAurasDueToSpell(SPELL_ROOTS_FREYA);
                }

                me->RemoveCorpse(false);
            }

            void UpdateAI(uint32 diff) override
            {
                if (_checkTimer <= diff)
                {
                    if (Player* victim = ObjectAccessor::GetPlayer(*me, _summonerGUID))
                        if (!victim->HasAura(SPELL_IRON_ROOTS) && !victim->HasAura(SPELL_ROOTS_FREYA))
                            me->DespawnOrUnsummon(2*IN_MILLISECONDS);
                    _checkTimer = 3*IN_MILLISECONDS;
                }
                else
                    _checkTimer -= diff;
            }

        private:
                uint32 _checkTimer;
                ObjectGuid _summonerGUID;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_iron_rootsAI>(creature);
        }
};

class npc_detonating_lasher : public CreatureScript
{
    public:
        npc_detonating_lasher() : CreatureScript("npc_detonating_lasher") { }

        struct npc_detonating_lasherAI : public ScriptedAI
        {
            npc_detonating_lasherAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                instance = me->GetInstanceScript();
                Reset();
            }

            void Reset() override
            {
                /*if (Unit* target = me->SelectNearbyTarget(0, 100.0f)) // Try to find another target
                    AttackStart(target);*/

                _events.ScheduleEvent(EVENT_LASH, 10 * IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_CHANGE_TARGET, 12.5 * IN_MILLISECONDS);
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override // TODO: Check possible double-call
            {
                if (damage >= me->GetHealth())
                {
                    if (InstanceScript* instance = me->GetInstanceScript())
                        if (Creature* freya = me->GetCreature(*me, instance->GetGuidData(BOSS_FREYA)))
                            freya->AI()->SetGUID(me->GetGUID(), ACTION_ELEMENTAL_DEAD);

                    me->DespawnOrUnsummon(1);
                }
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_BEGIN_ATTACK:
                        me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, 0);
                        me->SetReactState(REACT_AGGRESSIVE);
                        if (Unit* target = me->SelectNearestPlayer(100.0f))
                            me->AI()->AttackStart(target);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance && instance->GetBossState(BOSS_FREYA) != IN_PROGRESS)
                    me->DespawnOrUnsummon();

                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_LASH:
                            DoCast(SPELL_FLAME_LASH);
                            _events.ScheduleEvent(EVENT_LASH, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_CHANGE_TARGET:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                            {
                                // Switching to other target - modify aggro of new target by 20% from current target's aggro
                                me->AddThreat(target, me->GetThreatManager().getThreat(me->GetVictim(), false) * 1.2f);
                                me->AI()->AttackStart(target);
                            }
                            _events.ScheduleEvent(EVENT_CHANGE_TARGET, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                EventMap _events;
                InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_detonating_lasherAI>(creature);
        }
};

class npc_ancient_water_spirit : public CreatureScript
{
    public:
        npc_ancient_water_spirit() : CreatureScript("npc_ancient_water_spirit") { }

        struct npc_ancient_water_spiritAI : public ScriptedAI
        {
            npc_ancient_water_spiritAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                if (instance)
                    if (Creature* Freya = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_FREYA)))
                        _waveCount = Freya->AI()->GetData(DATA_TRIO_WAVE_COUNT);
                Reset();
            }

            void Reset() override
            {
                _tidalWaveTimer = 10*IN_MILLISECONDS;
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* Freya = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_FREYA)))
                {
                    CAST_AI(boss_freya::boss_freyaAI, Freya->AI())->_checkElementalAlive[_waveCount] = false;
                    CAST_AI(boss_freya::boss_freyaAI, Freya->AI())->LasherDead(1);
                }
            }
            
            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_BEGIN_ATTACK:
                        me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, 0);
                        me->SetReactState(REACT_AGGRESSIVE);
                        if (Unit* target = me->SelectNearestPlayer(100.0f))
                            me->AI()->AttackStart(target);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance && instance->GetBossState(BOSS_FREYA) != IN_PROGRESS)
                    me->DespawnOrUnsummon();

                if (!UpdateVictim())
                    return;

                if (_tidalWaveTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                    {
                        DoCast(target, SPELL_TIDAL_WAVE);
                        DoCast(target, SPELL_TIDAL_WAVE_EFFECT, true);
                    }
                    _tidalWaveTimer = urand(12*IN_MILLISECONDS, 25*IN_MILLISECONDS);
                }
                else
                    _tidalWaveTimer -= diff;

                DoMeleeAttackIfReady();
            }

            private:
                uint32 _tidalWaveTimer;
                uint8 _waveCount;
                InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ancient_water_spiritAI>(creature);
        }
};

class npc_storm_lasher : public CreatureScript
{
    public:
        npc_storm_lasher() : CreatureScript("npc_storm_lasher") { }

        struct npc_storm_lasherAI : public ScriptedAI
        {
            npc_storm_lasherAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                if (instance)
                    if (Creature* Freya = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_FREYA)))
                        _waveCount = Freya->AI()->GetData(DATA_TRIO_WAVE_COUNT);
                Reset();
            }

            void Reset() override
            {
                _events.ScheduleEvent(EVENT_LIGHTNING_LASH, 10*IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_STORMBOLT, 5*IN_MILLISECONDS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* Freya = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_FREYA)))
                {
                    CAST_AI(boss_freya::boss_freyaAI, Freya->AI())->_checkElementalAlive[_waveCount] = false;
                    CAST_AI(boss_freya::boss_freyaAI, Freya->AI())->LasherDead(2);
                }
            }
            
            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_BEGIN_ATTACK:
                        me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, 0);
                        me->SetReactState(REACT_AGGRESSIVE);
                        if (Unit* target = me->SelectNearestPlayer(100.0f))
                            me->AI()->AttackStart(target);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance && instance->GetBossState(BOSS_FREYA) != IN_PROGRESS)
                    me->DespawnOrUnsummon();

                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_LIGHTNING_LASH:
                            DoCast(SPELL_LIGHTNING_LASH);
                            _events.ScheduleEvent(EVENT_LIGHTNING_LASH, urand(7*IN_MILLISECONDS, 14*IN_MILLISECONDS));
                            return;
                        case EVENT_STORMBOLT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(target, SPELL_STORMBOLT);
                            _events.ScheduleEvent(EVENT_STORMBOLT, urand(8*IN_MILLISECONDS, 12*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                EventMap _events;
                uint8 _waveCount;
                InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_storm_lasherAI>(creature);
        }
};

class npc_snaplasher : public CreatureScript
{
    public:
        npc_snaplasher() : CreatureScript("npc_snaplasher") { }

        struct npc_snaplasherAI : public ScriptedAI
        {
            npc_snaplasherAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                if (instance)
                    if (Creature* Freya = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_FREYA)))
                        _waveCount = Freya->AI()->GetData(DATA_TRIO_WAVE_COUNT);
                Reset();
            }

            void Reset() override { }
            
            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* Freya = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_FREYA)))
                {
                    CAST_AI(boss_freya::boss_freyaAI, Freya->AI())->_checkElementalAlive[_waveCount] = false;
                    CAST_AI(boss_freya::boss_freyaAI, Freya->AI())->LasherDead(4);
                }
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_BEGIN_ATTACK:
                        me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, 0);
                        me->SetReactState(REACT_AGGRESSIVE);
                        if (Unit* target = me->SelectNearestPlayer(100.0f))
                            me->AI()->AttackStart(target);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (instance && instance->GetBossState(BOSS_FREYA) != IN_PROGRESS)
                    me->DespawnOrUnsummon();

                if (!UpdateVictim())
                    return;

                if (!me->HasAura(Is25ManRaid() ? SPELL_HARDENED_BARK_25 : SPELL_HARDENED_BARK))
                    DoCast(Is25ManRaid() ? SPELL_HARDENED_BARK_25 : SPELL_HARDENED_BARK);

                DoMeleeAttackIfReady();
            }

            private:
                uint8 _waveCount;
                InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_snaplasherAI>(creature);
        }
};

class npc_ancient_conservator : public CreatureScript
{
    public:
        npc_ancient_conservator() : CreatureScript("npc_ancient_conservator") { }

        struct npc_ancient_conservatorAI : public ScriptedAI
        {
            npc_ancient_conservatorAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                Reset();
            }

            void Reset() override
            {
                _events.ScheduleEvent(EVENT_NATURES_FURY, 7.5*IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_HEALTHY_SPORE, 3.5*IN_MILLISECONDS);
                SummonHealthySpores(2);
            }

            void SummonHealthySpores(uint8 sporesCount)
            {
                for (uint8 n = 0; n < sporesCount; ++n)
                {
                    DoCast(SPELL_SUMMON_PERIODIC);
                    DoCast(SPELL_SPORE_SUMMON_NW);
                    DoCast(SPELL_SPORE_SUMMON_NE);
                    DoCast(SPELL_SPORE_SUMMON_SE);
                    DoCast(SPELL_SPORE_SUMMON_SW);
                }
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                if (damage >= me->GetHealth())
                {
                    if (instance)
                        if (Creature* freya = me->GetCreature(*me, instance->GetGuidData(BOSS_FREYA)))
                            freya->AI()->SetGUID(me->GetGUID(), ACTION_ELEMENTAL_DEAD);
                    me->DespawnOrUnsummon(1);
                }
            }

            void JustEngagedWith(Unit* who) override
            {
                DoCast(who, SPELL_CONSERVATOR_GRIP);
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance && instance->GetBossState(BOSS_FREYA) != IN_PROGRESS)
                    me->DespawnOrUnsummon();

                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_HEALTHY_SPORE:
                            SummonHealthySpores(1);
                            _events.ScheduleEvent(EVENT_HEALTHY_SPORE, urand(15*IN_MILLISECONDS, 17.5*IN_MILLISECONDS));
                            return;
                        case EVENT_NATURES_FURY:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true, -SPELL_NATURES_FURY))
                                DoCast(target, SPELL_NATURES_FURY);
                            me->AddAura(SPELL_CONSERVATOR_GRIP, me);
                            _events.ScheduleEvent(EVENT_NATURES_FURY, 5*IN_MILLISECONDS);
                            return;
                        default:
                            return;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                EventMap _events;
                InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ancient_conservatorAI>(creature);
        }
};

class npc_sun_beam : public CreatureScript
{
    public:
        npc_sun_beam() : CreatureScript("npc_sun_beam") { }

        struct npc_sun_beamAI : public ScriptedAI
        {
            npc_sun_beamAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            void InitializeAI() override
            {
                me->SetReactState(REACT_PASSIVE);
                DoCast(SPELL_FREYA_UNSTABLE_ENERGY);
                Reset();
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetDisplayFromModel(1);
                me->DespawnOrUnsummon(12*IN_MILLISECONDS);
                DoCast(me, SPELL_FREYA_UNSTABLE_ENERGY_VISUAL, true); // visual
            }

            void UpdateAI(uint32 diff) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_sun_beamAI>(creature);
        }
};

class npc_healthy_spore : public CreatureScript
{
    public:
        npc_healthy_spore() : CreatureScript("npc_healthy_spore") { }

        struct npc_healthy_sporeAI : public ScriptedAI
        {
            npc_healthy_sporeAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                SetCombatMovement(false);
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                DoCast(me, SPELL_HEALTHY_SPORE_VISUAL);
                DoCast(me, SPELL_POTENT_PHEROMONES);
                DoCast(me, SPELL_GROW);
                _lifeTimer = urand(22*IN_MILLISECONDS, 30*IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (InstanceScript* inst = me->GetInstanceScript())
                {
                    if (inst->GetBossState(BOSS_FREYA) != IN_PROGRESS)
                        me->DisappearAndDie();
                }
                else
                    me->DisappearAndDie();

                if (_lifeTimer <= diff)
                {
                    me->RemoveAurasDueToSpell(SPELL_GROW);
                    me->DespawnOrUnsummon(2.2*IN_MILLISECONDS);
                    _lifeTimer = urand(22*IN_MILLISECONDS, 30*IN_MILLISECONDS);
                }
                else
                    _lifeTimer -= diff;
            }

            private:
                uint32 _lifeTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_healthy_sporeAI>(creature);
        }
};

class npc_eonars_gift : public CreatureScript
{
    public:
        npc_eonars_gift() : CreatureScript("npc_eonars_gift") { }

        struct npc_eonars_giftAI : public ScriptedAI
        {
            npc_eonars_giftAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            void Reset() override
            {
                _lifeBindersGiftTimer = 12*IN_MILLISECONDS;
                _cast = false;
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                DoCast(me, SPELL_GROW);
                DoCast(me, SPELL_PHEROMONES, true);
                DoCast(me, SPELL_EONAR_VISUAL, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (InstanceScript* inst = me->GetInstanceScript())
                {
                    if (inst->GetBossState(BOSS_FREYA) != IN_PROGRESS)
                        me->DisappearAndDie();
                }
                else
                    me->DisappearAndDie();
                
                if (!_cast)
                {
                    if (_lifeBindersGiftTimer <= diff)
                    {
                        me->RemoveAurasDueToSpell(SPELL_GROW);
                        DoCast(SPELL_LIFEBINDERS_GIFT);
                        me->DespawnOrUnsummon(2.5*IN_MILLISECONDS);
                        _cast = true;
                    }
                    else
                        _lifeBindersGiftTimer -= diff;
                }
            }

            private:
                uint32 _lifeBindersGiftTimer;
                bool _cast;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_eonars_giftAI>(creature);
        }
};

class npc_nature_bomb : public CreatureScript
{
    public:
        npc_nature_bomb() : CreatureScript("npc_nature_bomb") { }

        struct npc_nature_bombAI : public ScriptedAI
        {
            npc_nature_bombAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                _bombTimer = urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS);
                DoCast(SPELL_OBJECT_BOMB);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetDisplayFromModel(1);
                SetCombatMovement(false);
            }

            void UpdateAI(uint32 diff) override
            {
                if (GameObject* go = me->FindNearestGameObject(GO_NATURE_BOMB, 5.0f))
                    go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);

                if (_bombTimer <= diff)
                {
                    if (GameObject* go = me->FindNearestGameObject(GO_NATURE_BOMB, 5.0f))
                    {
                        go->SetGoState(GO_STATE_ACTIVE);
                        DoCast(me, SPELL_NATURE_BOMB);
                        // me->RemoveGameObject(go, true);
                        me->RemoveFromWorld();
                    }
                }
                else
                    _bombTimer -= diff;
            }

            private:
                uint32 _bombTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_nature_bombAI>(creature);
        }
};

class npc_unstable_sun_beam : public CreatureScript
{
    public:
        npc_unstable_sun_beam() : CreatureScript("npc_unstable_sun_beam") { }

        struct npc_unstable_sun_beamAI : public ScriptedAI
        {
            npc_unstable_sun_beamAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                _despawnTimer = urand(7000, 12000);
                DoCast(me, SPELL_PHOTOSYNTHESIS);
                DoCast(me, SPELL_UNSTABLE_SUN_BEAM);
                me->SetDisplayFromModel(1);
                SetCombatMovement(false);
            }
            
            void MoveInLineOfSight(Unit* target) override
            {
                if (target && target->GetTypeId() == TYPEID_UNIT)
                {
                    if (target->ToCreature()->GetEntry() == NPC_ELDER_BRIGHTLEAF)
                    {
                        if (me->IsWithinDist2d(target, 4))
                        {
                            if (!target->HasAura(SPELL_PHOTOSYNTHESIS))
                                me->AddAura(SPELL_PHOTOSYNTHESIS, target);
                        }
                        else
                        {
                            if (target->HasAura(SPELL_PHOTOSYNTHESIS))
                                target->RemoveAurasDueToSpell(SPELL_PHOTOSYNTHESIS);
                        }
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (_despawnTimer <= diff)
                {
                    DoCastAOE(SPELL_UNSTABLE_ENERGY, true);
                    me->DisappearAndDie();
                }
                else
                    _despawnTimer -= diff;
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (target && spell->Id == SPELL_UNSTABLE_ENERGY)
                {
                    target->RemoveAurasDueToSpell(SPELL_UNSTABLE_SUN_BEAM);
                    target->RemoveAurasDueToSpell(SPELL_UNSTABLE_SUN_BEAM_TRIGGERED);
                }
            }

        private:
            uint32 _despawnTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_unstable_sun_beamAI>(creature);
        }
};

class IsNoAllyOfNature
{
    public:
        IsNoAllyOfNature(bool freya = false) : _freya(freya) { }

        bool operator() (WorldObject* unit)
        {
            if (unit->ToCreature())
            {
                switch (unit->ToCreature()->GetEntry())
                {
                    case NPC_DETONATING_LASHER:
                    case NPC_ANCIENT_WATER_SPIRIT:
                    case NPC_STORM_LASHER:
                    case NPC_SNAPLASHER:
                    case NPC_ANCIENT_CONSERVATOR:
                        return false;
                    case NPC_FREYA:
                        return !_freya;
                    default:
                        break;
                }
            }
            return true;
        }

    private:
        bool _freya;
};

class spell_essence_targeting_ironbranch : public SpellScriptLoader
{
    public:
        spell_essence_targeting_ironbranch() : SpellScriptLoader("spell_elder_ironbranch_essence_targeting") { }

        class spell_essence_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_essence_targeting_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(IsNoAllyOfNature());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_essence_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_essence_targeting_SpellScript();
        }
};

class spell_essence_targeting_brightleaf : public SpellScriptLoader
{
    public:
        spell_essence_targeting_brightleaf() : SpellScriptLoader("spell_elder_brightleaf_essence_targeting") { }

        class spell_essence_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_essence_targeting_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(IsNoAllyOfNature(true));
            }

            void EmptyTargets(std::list<WorldObject*>& targets)
            {
                // Because we don't know who should be affected by it. +50% damage in 10-man mode, +60% damage in 25-man mode.
                targets.clear();
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_essence_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_essence_targeting_SpellScript::EmptyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_essence_targeting_SpellScript();
        }
};


class spell_aggregation_pheromones_targeting : public SpellScriptLoader
{
    public:
        spell_aggregation_pheromones_targeting() : SpellScriptLoader("spell_aggregation_pheromones_targeting") { }

        class spell_aggregation_pheromones_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aggregation_pheromones_targeting_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                // remove caster if this is the only target
                if (targets.size() < 2)
                    targets.clear();
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_aggregation_pheromones_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_aggregation_pheromones_targeting_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_aggregation_pheromones_targeting_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aggregation_pheromones_targeting_SpellScript();
        }
};

// temporary to trigger spell on proper target
class spell_elder_brightleaf_unstable_sun_beam : public SpellScriptLoader
{
    public:
        spell_elder_brightleaf_unstable_sun_beam() : SpellScriptLoader("spell_elder_brightleaf_unstable_sun_beam") { }

        class spell_elder_brightleaf_unstable_sun_beam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elder_brightleaf_unstable_sun_beam_SpellScript);

            void HandleForceCast(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                uint32 triggered_spell_id = GetSpellInfo()->Effects[effIndex].TriggerSpell;

                if (caster && target && triggered_spell_id)
                    target->CastSpell(target, triggered_spell_id, true, 0, 0, caster->GetGUID());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_elder_brightleaf_unstable_sun_beam_SpellScript::HandleForceCast, EFFECT_1, SPELL_EFFECT_FORCE_CAST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_elder_brightleaf_unstable_sun_beam_SpellScript();
        }
};

class spell_freya_attuned_to_nature_dose_reduction : public SpellScriptLoader
{
    public:
        spell_freya_attuned_to_nature_dose_reduction() : SpellScriptLoader("spell_freya_attuned_to_nature_dose_reduction") { }

        class spell_freya_attuned_to_nature_dose_reduction_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_freya_attuned_to_nature_dose_reduction_SpellScript)

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* target = GetHitUnit();
                SpellInfo const* spellInfo = GetSpellInfo();
                switch (spellInfo->Id)
                {
                    case SPELL_ATTUNED_TO_NATURE_2_DOSE_REDUCTION:
                        if (target->HasAura(GetEffectValue()))
                            for (uint8 n = 0; n < 2; ++n)
                                target->RemoveAuraFromStack(GetEffectValue(), 0, AURA_REMOVE_BY_DEFAULT);
                        break;
                    case SPELL_ATTUNED_TO_NATURE_10_DOSE_REDUCTION:
                        if (target->HasAura(GetEffectValue()))
                            for (uint8 n = 0; n < 10; ++n)
                                target->RemoveAuraFromStack(GetEffectValue(), 0, AURA_REMOVE_BY_DEFAULT);
                        break;
                    case SPELL_ATTUNED_TO_NATURE_25_DOSE_REDUCTION:
                        if (target->HasAura(GetEffectValue()))
                            for (uint8 n = 0; n < 25; ++n)
                                target->RemoveAuraFromStack(GetEffectValue(), 0, AURA_REMOVE_BY_DEFAULT);
                        break;
                    default:
                        break;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_freya_attuned_to_nature_dose_reduction_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_freya_attuned_to_nature_dose_reduction_SpellScript();
        }
};

class spell_freya_iron_roots : public SpellScriptLoader
{
    public:
        spell_freya_iron_roots() : SpellScriptLoader("spell_freya_iron_roots") { }

        class spell_freya_iron_roots_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_freya_iron_roots_SpellScript);

            void HandleSummon(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                uint32 entry = uint32(GetSpellInfo()->Effects[effIndex].MiscValue);

                Position pos = GetCaster()->GetPosition();
                // Not good at all, but this prevents having roots in a different position then player
                if (Creature* Roots = GetCaster()->SummonCreature(entry, pos))
                    GetCaster()->NearTeleportTo(Roots->GetPositionX(), Roots->GetPositionY(), Roots->GetPositionZ(), GetCaster()->GetOrientation());
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_freya_iron_roots_SpellScript::HandleSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_freya_iron_roots_SpellScript();
        }
};

void AddSC_boss_freya()
{
    new boss_freya();
    new boss_elder_brightleaf();
    new boss_elder_ironbranch();
    new boss_elder_stonebark();
    new npc_ancient_conservator();
    new npc_snaplasher();
    new npc_storm_lasher();
    new npc_ancient_water_spirit();
    new npc_detonating_lasher();
    new npc_sun_beam();
    new npc_nature_bomb();
    new npc_eonars_gift();
    new npc_healthy_spore();
    new npc_unstable_sun_beam();
    new npc_iron_roots();
    new spell_essence_targeting_ironbranch();
    new spell_essence_targeting_brightleaf();
    new spell_aggregation_pheromones_targeting();
    new spell_freya_attuned_to_nature_dose_reduction();
    new spell_freya_iron_roots();
    new spell_elder_brightleaf_unstable_sun_beam();
}
