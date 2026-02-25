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
#include "ScriptedCreature.h"
#include "siege_of_orgrimmar.h"

enum eSpells
{
    /*Cry in the Darkness*/
    SPELL_SHOCK_BLAST              = 143991,
    SPELL_POWER_REGEN              = 143345,
    SPELL_POWER_REGEN_EFF          = 143409,
    SPELL_CLUMP_CHECK              = 143430,
    SPELL_SHOCK_BLAST_FORCE        = 143706, // by controller
    SPELL_SHOCK_BLAST_EFF          = 143707,
    SPELL_ACCELERATION             = 143411,
    SPELL_BLOODIED                 = 143452,
    SPELL_DEAFENING_SCREECH        = 143343,
    SPELL_FEARSOME_ROAR            = 143426,
    SPELL_TAIL_LASH                = 143428,
    SPELL_PANIC                    = 143766,

    /*Frenzy for Blood*/
    SPELL_ACID_BREATH              = 143780,
    SPELL_CORROSIVE_BLOOD          = 143795,
    SPELL_CORROSIVE_BLOOD_EFF      = 143791,
    SPELL_BLOOD_FRENZY             = 143440,
    SPELL_BLOOD_FRENZY_EFF         = 143442,
    SPELL_BLOOD_FRENZY_ROAR        = 144067,
    SPELL_FIXATE                   = 143445,
    SPELL_FIXATE_IMMUNE            = 146540,
    SPELL_ATTACK_ME                = 146539,
    SPELL_FREEZING_BREATH          = 143773,
    SPELL_ICY_BLOOD                = 143801,
    SPELL_ICY_BLOOD_EFF            = 143800,
    SPELL_FROZEN_SOLID             = 143777,
    SPELL_CLEAR_FROZEN_SOLID       = 143779,
    SPELL_SUMMON_ICE_TOMB_STUN     = 136893,
    SPELL_SUMMON_ICE_TOMB          = 136929,
    SPELL_SCORCHING_BREATH         = 143767,
    SPELL_BURNING_BLOOD            = 143781,
    SPELL_BURNING_BLOOD_SELECTOR   = 143782,
    SPELL_BURNING_BLOOD_EFF        = 143783,
    SPELL_BURNING_BLOOD_AT_EFF     = 143784,
    SPELL_ENRAGE                   = 145974,
    SPELL_UNLOCKING                = 143917,
    SPELL_SKELETON_KEY             = 146589,
    SPELL_SKELETON_KEY_UNK         = 148153, // to creature by player on spellclick?
    SPELL_SUICIDE                  = 148350,
    SPELL_SUICIDE_SELF             = 143997, // Montak
    SPELL_SUICIDE_SELF_2           = 143998, // Akolik
    SPELL_FROST_PUSTULES           = 143968, // visual transform
    SPELL_FIRE_PUSTULES            = 143970,
    SPELL_ACID_PUSTULES            = 143971,
    SPELL_AKOLIK_INCANTATION       = 144016,
    SPELL_WATER_OF_PURIFY          = 144013,
    SPELL_FIRE_OF_THE_SPIRITS      = 144014,
    SPELL_CREATURE_FIXATE          = 146581,
    SPELL_CHOMP                    = 147996, // jump into caw!
    SPELL_RESTORATIVE_WATERS       = 144117,
    SPELL_FLAME_COATING            = 144115,
    SPELL_FLAME_COATING_EFF        = 144116,
    SPELL_DUMMY_NUKE               = 146538,

    // Misc
    SPELL_KILL_JAILERS             = 147994,
    SPELL_LIGHTNING_CHAIN_VISUAL   = 147987,
    SPELL_RIOT                     = 147989,
    SPELL_ZERO_POWER               = 72242,
    SPELL_BERSERK                  = 26662,
    SPELL_SUMMON_PLAYER            = 25104,
    ITEM_SKELETON_KEY              = 100948,
    SPELL_MELEE                    = 149356,
    SPELL_PERMANENT_FEIGN_DEATH    = 70628,
    SPELL_MEAT_EXPLOSION           = 130620,
    SPELL_MEAT_EXPLOSION_2         = 130621,
    SPELL_MEAT_EXPLOSION_3         = 130622,
    SPELL_MEGA_SNAIL_BOSS_MORPH    = 148355,
    SPELL_MEGA_SNAIL_ACHIEV_SELECT = 148348,

    // Heroic
    SPELL_VAMPIRIC_FRENZY_PROC     = 147977,
    SPELL_VAMPIRIC_FRENZY_PROC_2   = 147980, // seems like for 10 normal, but maybe not
    SPELL_VAMPIRIC_FRENZY_HEAL_EFF = 147978,
    SPELL_MENDING                  = 149569,
    SPELL_WRECKING_BALL            = 147952,
    SPELL_WRECKING_BALL_BREAK      = 147906,
    SPELL_WRECKING_BALL_AT         = 147609,
    SPELL_WRECKING_BALL_EFF        = 148143,
    SPELL_WRECKING_BALL_VISUAL     = 147946,
};

enum eEvents
{
    // Thok Bloodthirsty
    EVENT_BERSERK = 1,
    EVENT_FEARSOME_ROAR,
    EVENT_DEFEANING_SCREECH,
    EVENT_SHOCK_BLAST,
    EVENT_TAIL_LASH,
    EVENT_FIXATE,
    EVENT_CORROSIVE_BLOOD,
    EVENT_ICY_BLOOD,
    EVENT_BURNING_BLOOD,
    EVENT_CAVE_BATS,
    EVENT_YETI,

    // Misc
    EVENT_ENRAGE,
};

enum Actions
{
    ACTION_DEFEANING_SCREECH = 1,
    ACTION_FRENZY,
    ACTION_FIXATE,
    ACTION_FIXATE_DELAY,
    ACTION_WRECKING_BALL,
};

enum eSays
{
    TALK_INTRO,
    TALK_SPECIAL_1,
    TALK_SPECIAL_2,
};

const std::map<uint32, uint32> invScreechType =
{
    { 0,  4 },
    { 1,  5 },
    { 2,  8 },
    { 3, 13 },
    { 4, 18 },
    { 5, 25 },
    { 6, 25 },
    { 7, 25 },
    { 8, 50 }, // next always 50 per tick
};

const std::map<uint32, std::array<uint32, 5>> invDevourType =
{
    { NPC_AKOLIK,             { SPELL_ACID_BREATH,      EVENT_CORROSIVE_BLOOD, NPC_SKUMBLADE_CAPTIVE, SPELL_AKOLIK_INCANTATION,  SPELL_ACID_PUSTULES  } },
    { NPC_WATERSPEAKER_GORAI, { SPELL_FREEZING_BREATH,  EVENT_ICY_BLOOD,       NPC_INKGILL_CAPTIVE,   SPELL_WATER_OF_PURIFY,     SPELL_FROST_PUSTULES } },
    { NPC_MONTAK,             { SPELL_SCORCHING_BREATH, EVENT_BURNING_BLOOD,   NPC_GAI_CHO_REFUGE,    SPELL_FIRE_OF_THE_SPIRITS, SPELL_FIRE_PUSTULES  } },
};

const std::map<uint32, uint32> invCageType =
{
    { GO_JINYU_PRISON,     NPC_WATERSPEAKER_GORAI },
    { GO_JINYU_PRISON_2,   NPC_WATERSPEAKER_GORAI },
    { GO_SAUROK_PRISON,    NPC_AKOLIK             },
    { GO_SAUROK_PRISON_2,  NPC_AKOLIK             },
    { GO_YAUNGOL_PRISON,   NPC_MONTAK             },
    { GO_YAUNGOL_PRISON_2, NPC_MONTAK             },
};

const std::vector<uint32> meatExplosions =
{
    SPELL_MEAT_EXPLOSION,
    SPELL_MEAT_EXPLOSION_2,
    SPELL_MEAT_EXPLOSION_3,
};

class boss_thok_the_bloodthirsty : public CreatureScript
{
    public:
        boss_thok_the_bloodthirsty() : CreatureScript("boss_thok_the_bloodthirsty") { }

        struct boss_thok_the_bloodthirstyAI : public BossAI
        {
            boss_thok_the_bloodthirstyAI(Creature* creature) : BossAI(creature, DATA_THOK_THE_BLOODTHIRSTY) 
            { 
                me->setActive(true);
                hasInitialize = false;
            }

            EventMap berserkerEvents;
            bool hasInitialize;
            bool stopFixate;
            bool canEat;
            bool isFrenzy;
            uint32 eatHelperEntry;
            uint32 cleaveSpellEntry;
            uint32 nextFrenzyPct;
            uint32 thirdCageCount;
            ObjectGuid targetGUID;
            ObjectGuid fixateTargetGUID;

            void Reset() override
            {
                _Reset();

                me->SetPowerType(POWER_MANA);
                me->SetMaxPower(POWER_MANA, 100);
                me->SetPower(POWER_MANA, 0);
                me->setRegeneratingMana(false);
                me->SetSpeed(MOVE_RUN, 1.8f);
                targetGUID = ObjectGuid::Empty;
                fixateTargetGUID = ObjectGuid::Empty;
                eatHelperEntry   = 0;
                cleaveSpellEntry = 0;
                nextFrenzyPct    = 80;
                thirdCageCount   = 0;
                stopFixate       = false;
                canEat           = false;
                isFrenzy         = false;
                me->SetAutoattackOverrideSpell(0, 0);

                me->SetReactState(REACT_DEFENSIVE);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                me->RemoveAurasDueToSpell(SPELL_FIXATE_IMMUNE);

                events.Reset();
                berserkerEvents.Reset();
                summons.DespawnAll();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                // Not available to use
                if (GameObject* thirdCage = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GO_THRICE_LOCKED_CAGE) : ObjectGuid::Empty))
                    thirdCage->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);

                scheduler
                    .Schedule(Seconds(1), [this](TaskContext context)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });

                if (hasInitialize)
                    return;

                hasInitialize = true;

                scheduler
                    .Schedule(Milliseconds(1500), [this](TaskContext context)
                {
                    if (instance && instance->GetData(DATA_THOK_BLOODTHIRSTY_PRE_EVENT) == DONE)
                    {
                        me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                        instance->HandleGameObject(instance->GetGuidData(GO_REX_DOOR), true, NULL);

                        Movement::MoveSplineInit init(me);

                        for (auto itr : thokBloodthristyRelocate)
                            init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                        init.SetUncompressed();
                        init.SetSmooth();
                        init.Launch();
                    }
                });
            }

            void SetGUID(ObjectGuid guid, int32 /*type*/) override
            {
                fixateTargetGUID = guid;
            }

            ObjectGuid GetGUID(int32 /*type*/) const override
            {
                return fixateTargetGUID;
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                if (instance)
                    instance->SetBossState(DATA_THOK_THE_BLOODTHIRSTY, FAIL);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                // Only FLR Algorythm
                if (instance && instance->GetData(DATA_LFR) && !instance->GetData(DATA_FLEX) && nextFrenzyPct > 10)
                {
                    if (HealthBelowPct(nextFrenzyPct))
                    {
                        nextFrenzyPct -= 25; // break next frenzy if cause this in phase, but not switch to new

                        if (!isFrenzy)
                            DoAction(ACTION_FRENZY);
                    }
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveAurasDueToSpell(SPELL_MELEE);
                HandleResetPrisoners(true); // server crash case

                DoCast(me, SPELL_KILL_JAILERS, true);
                DoCast(me, SPELL_POWER_REGEN, true);

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                    instance->DoCastSpellOnPlayers(SPELL_SHOCK_BLAST);
                }

                // Jail Vehicle
                if (Creature* shockCollar = me->SummonCreature(NPC_SHOCK_COLLAR, *me, TEMPSUMMON_MANUAL_DESPAWN))
                    shockCollar->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);

                // Allow to use it (Achievement)
                if (GameObject* thirdCage = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GO_THRICE_LOCKED_CAGE) : ObjectGuid::Empty))
                    thirdCage->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);

                events.ScheduleEvent(EVENT_FEARSOME_ROAR, 12 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_TAIL_LASH, 10 * IN_MILLISECONDS);
                berserkerEvents.ScheduleEvent(EVENT_SHOCK_BLAST, 2 * IN_MILLISECONDS);
                berserkerEvents.ScheduleEvent(EVENT_BERSERK, 10 * MINUTE * IN_MILLISECONDS);
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_GIANT_SNAIL_LOCKED_COUNT)
                    thirdCageCount += data;
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case TYPE_BLOOD_FRENZY:
                        return canEat ? 1 : 0;
                    case TYPE_GIANT_SNAIL_LOCKED_COUNT:
                        return thirdCageCount;
                }

                return 0;
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (me->IsInCombat())
                    summon->SetInCombatWithZone();
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_INTRO:
                    {
                        if (instance && instance->GetData(DATA_THOK_BLOODTHIRSTY_PRE_EVENT) == DONE)
                            return;

                        if (instance)
                        {
                            instance->HandleGameObject(instance->GetGuidData(GO_REX_DOOR), true, NULL);
                            instance->SetData(DATA_THOK_BLOODTHIRSTY_PRE_EVENT, DONE);

                            // Pull beasts
                            if (Creature* yeti = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_STARVED_YETI_TRASH)))
                                yeti->AI()->DoAction(ACTION_START_INTRO);

                            if (Creature* mushan = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ENRAGED_MUSHAN_BEAST)))
                                mushan->AI()->DoAction(ACTION_START_INTRO);
                        }

                        me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                        me->GetMotionMaster()->MovePoint(0, thokBloodthristyRelocate[0]);

                        // Pull Jailers to prison
                        std::list<Creature*> jailersList;
                        GetCreatureListWithEntryInGrid(jailersList, me, NPC_KORKRON_JAILER_THRASH, 250.0f);
                        jailersList.remove_if([=](Creature* target) { return target && target->GetExactDist2d(me) < 80.0f; });

                        uint8 itr = 0;
                        for (auto&& jailer : jailersList)
                        {
                            if (itr > 4) // in case
                                break;

                            jailer->GetMotionMaster()->MovePoint(0, korkronJailersPos[itr]);
                            itr++;
                        }
                        break;
                    }
                    case ACTION_DEFEANING_SCREECH:
                        me->SetPower(POWER_ENERGY, 100);
                        events.ScheduleEvent(EVENT_DEFEANING_SCREECH, 0.5 * IN_MILLISECONDS);
                        break;
                    case ACTION_FRENZY:
                        isFrenzy = true;
                        Talk(TALK_SPECIAL_1);
                        stopFixate = false;
                        me->RemoveAurasDueToSpell(SPELL_POWER_REGEN);
                        me->SetPower(POWER_MANA, 0);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHOCK_BLAST);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOODIED);

                        if (Unit* target = me->GetVictim())
                            targetGUID = target->GetGUID();

                        berserkerEvents.CancelEvent(EVENT_SHOCK_BLAST);
                        me->RemoveAurasDueToSpell(SPELL_ACCELERATION);
                        me->PrepareChanneledCast(me->GetOrientation());
                        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

                        DoCast(me, SPELL_BLOOD_FRENZY_ROAR, true);

                        // Summon Jailer
                        me->SummonCreature(NPC_KORKRON_JAILER, jailerEntrancePos, TEMPSUMMON_MANUAL_DESPAWN);

                        events.CancelEvent(EVENT_FEARSOME_ROAR);
                        events.CancelEvent(EVENT_DEFEANING_SCREECH);
                        events.CancelEvent(EVENT_TAIL_LASH);
                        berserkerEvents.ScheduleEvent(EVENT_SHOCK_BLAST, 9 * IN_MILLISECONDS);

                        scheduler
                            .Schedule(Milliseconds(3000), [this](TaskContext context)
                        {
                            me->SetAutoattackOverrideSpell(SPELL_DUMMY_NUKE, 0);
                            me->SetSpeed(MOVE_RUN, IsHeroic() ? 0.28f : 0.25f);
                            me->UpdateSpeed(MOVE_RUN, true);
                            canEat = true;
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOODIED); // in case
                            events.ScheduleEvent(EVENT_FIXATE, 1 * IN_MILLISECONDS);
                        });
                        break;
                    case ACTION_FIXATE:
                        // Interrupt fixate if any helper was imprison
                        if (stopFixate)
                            break;

                        me->PrepareChanneledCast(me->GetOrientation());
                        events.ScheduleEvent(EVENT_FIXATE, 1 * IN_MILLISECONDS);
                        break;
                    case NPC_AKOLIK:
                    case NPC_WATERSPEAKER_GORAI:
                    case NPC_MONTAK:
                    {
                        me->SetAutoattackOverrideSpell(0, 0);
                        berserkerEvents.CancelEvent(EVENT_CORROSIVE_BLOOD);
                        berserkerEvents.CancelEvent(EVENT_ICY_BLOOD);
                        berserkerEvents.CancelEvent(EVENT_BURNING_BLOOD);

                        me->InterruptNonMeleeSpells(false, SPELL_FIXATE);

                        stopFixate = true;
                        canEat = false;
                        me->SetSpeed(MOVE_RUN, 1.8f);
                        me->UpdateSpeed(MOVE_RUN, true);
                        events.CancelEvent(EVENT_FIXATE);
                        eatHelperEntry = actionId;

                        me->PrepareChanneledCast(me->GetOrientation());
                        me->StopMoving();

                        me->RemoveAurasDueToSpell(SPELL_BLOOD_FRENZY);
                        me->RemoveAurasDueToSpell(SPELL_BLOOD_FRENZY_EFF);

                        if (Unit* target = ObjectAccessor::GetUnit(*me, fixateTargetGUID))
                            target->RemoveAurasDueToSpell(SPELL_FIXATE);

                        auto key = invDevourType.find(eatHelperEntry);

                        // Init new events depends of devour
                        DoCast(me, key->second[4]);

                        if (Creature* helper = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(actionId) : ObjectGuid::Empty))
                        {
                            DoCast(helper, SPELL_CREATURE_FIXATE);
                            me->GetMotionMaster()->MovePoint(1, *helper);
                        }

                        // Restore Default phase
                        DoCast(me, SPELL_POWER_REGEN, true);

                        events.ScheduleEvent(EVENT_FEARSOME_ROAR, 14 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_TAIL_LASH, 10 * IN_MILLISECONDS);

                        // Schedule new events
                        cleaveSpellEntry = key->second[0];

                        berserkerEvents.ScheduleEvent(key->second[1], 9.5 * IN_MILLISECONDS);

                        if (IsHeroic())
                        {
                            // Akolik generate spawn a yeti, montak none, waterspeaker - cave bats
                            if (actionId == NPC_AKOLIK)
                                berserkerEvents.ScheduleEvent(EVENT_CAVE_BATS, 16 * IN_MILLISECONDS);
                            else if (actionId == NPC_WATERSPEAKER_GORAI)
                                berserkerEvents.ScheduleEvent(EVENT_YETI, 16 * IN_MILLISECONDS);
                        }
                        break;
                    }
                    case ACTION_FIXATE_DELAY:
                    {
                        me->ClearUnitState(UNIT_STATE_CASTING);

                        scheduler
                            .Schedule(Seconds(3), [this](TaskContext context) // blizzlike
                        {
                            if (!me->HasAura(SPELL_BLOOD_FRENZY) && canEat)
                                DoCast(me, SPELL_BLOOD_FRENZY);

                            if (Unit* target = ObjectAccessor::GetUnit(*me, fixateTargetGUID))
                            {
                                if (!target->HasAura(SPELL_FIXATE)) // fixate was interrupted by any reason - look into remove fixate script
                                    return;

                                AttackStart(target);
                                me->SetReactState(REACT_PASSIVE);
                                target->CastSpell(me, SPELL_ATTACK_ME, true);
                            }
                        });
                        break;
                    }
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                {
                    me->ClearUnitState(UNIT_STATE_IGNORE_PATHFINDING);

                    DoCast(me, SPELL_MELEE);

                    // Lightning Channeling
                    std::list<Creature*> teslaList;
                    GetCreatureListWithEntryInGrid(teslaList, me, NPC_TESLA_COIL, 200.0f);

                    for (auto&& itr : teslaList)
                        itr->CastSpell(me, SPELL_LIGHTNING_CHAIN_VISUAL, false);
                }

                // Eat him!
                if (pointId == 1)
                {
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                    me->RemoveAurasDueToSpell(SPELL_FIXATE_IMMUNE);

                    if (Creature* helper = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(eatHelperEntry) : ObjectGuid::Empty))
                    {
                        if (helper->GetEntry() == NPC_AKOLIK)
                            helper->AI()->Talk(TALK_SPECIAL_2);

                        me->Kill(helper, true);
                        helper->CastSpell(helper, SPELL_MEAT_EXPLOSION_2, true);

                        if (IsHeroic())
                            DoCast(me, SPELL_MENDING); // heal self 

                        //helper->CastSpell(helper, SPELL_CHOMP, true);
                    }

                    if (instance)
                        instance->DoCastSpellOnPlayers(SPELL_SHOCK_BLAST);

                    scheduler
                        .Schedule(Milliseconds(1000), [this](TaskContext context)
                    {
                        DoZoneInCombat(me, 200.0f);
                        me->StopMoving();
                        me->GetMotionMaster()->Clear();
                        me->RemoveChanneledCast(targetGUID);
                        isFrenzy = false;
                    });
                }
            }

            void KilledUnit(Unit* /*victim*/) override { }

            void EnterEvadeMode() override
            {
                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_THOK_THE_BLOODTHIRSTY, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHOCK_BLAST);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOODIED);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PANIC);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FIXATE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORROSIVE_BLOOD_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FREEZING_BREATH);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SCORCHING_BREATH);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SKELETON_KEY);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                // Remove Ice tombs if need
                std::list<GameObject*> iceTombs;
                GetGameObjectListWithEntryInGrid(iceTombs, me, GO_THOK_ICE_TOMB, 400.0f);

                for (auto&& itr : iceTombs)
                    itr->Delete();

                HandleResetPrisoners();
                summons.DespawnAll();
                berserkerEvents.Reset();
                me->SetAutoattackOverrideSpell(0, 0);

                if (GameObject* thirdCage = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GO_THRICE_LOCKED_CAGE) : ObjectGuid::Empty))
                {
                    thirdCage->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    thirdCage->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                    thirdCage->SetGoState(GO_STATE_READY);
                    thirdCage->SetLootState(GO_NOT_READY);
                }

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                _DespawnAtEvade();
                me->GetMap()->CreatureRelocation(me, thokBloodthristyRelocate[1].GetPositionX(), thokBloodthristyRelocate[1].GetPositionY(), thokBloodthristyRelocate[1].GetPositionZ(), thokBloodthristyRelocate[1].GetOrientation());
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHOCK_BLAST);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOODIED);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PANIC);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FIXATE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORROSIVE_BLOOD_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FREEZING_BREATH);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SCORCHING_BREATH);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SKELETON_KEY);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    // Achiev Snail!
                    if (GetData(TYPE_GIANT_SNAIL_LOCKED_COUNT) > 2)
                    {
                        if (Creature* gastropod = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_MONSTROUS_GASTROPOD)))
                            gastropod->AI()->DoAction(ACTION_START_INTRO);
                    }
                }

                // Remove Ice tombs if need
                std::list<GameObject*> iceTombs;
                GetGameObjectListWithEntryInGrid(iceTombs, me, GO_THOK_ICE_TOMB, 400.0f);

                for (auto&& itr : iceTombs)
                    itr->Delete();

                if (IsHeroic()) // Remove this from map
                {
                    std::list<Creature*> prisonersList;
                    GetCreatureListWithEntryInGrid(prisonersList, me, NPC_CAPTIVE_CAVE_BAT, 400.0f);
                    GetCreatureListWithEntryInGrid(prisonersList, me, NPC_STARVED_YETI, 400.0f);

                    for (auto&& itr : prisonersList)
                        itr->DespawnOrUnsummon();
                }

                berserkerEvents.Reset();
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);
                berserkerEvents.Update(diff);

                while (uint32 eventId = berserkerEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            break;
                        case EVENT_SHOCK_BLAST:
                            if (canUseShockBlast() && !me->HasUnitState(UNIT_STATE_CASTING))
                                DoCast(me, SPELL_SHOCK_BLAST_FORCE, true);

                            berserkerEvents.ScheduleEvent(EVENT_SHOCK_BLAST, 2.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_CORROSIVE_BLOOD:
                            if (canUseShockBlast())
                                DoCast(me, SPELL_CORROSIVE_BLOOD, true);

                            berserkerEvents.ScheduleEvent(EVENT_CORROSIVE_BLOOD, 9.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_ICY_BLOOD:
                            if (canUseShockBlast())
                                DoCast(me, SPELL_ICY_BLOOD, true);

                            berserkerEvents.ScheduleEvent(EVENT_ICY_BLOOD, 9.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_BURNING_BLOOD:
                            if (canUseShockBlast())
                                DoCast(me, SPELL_BURNING_BLOOD, true);

                            berserkerEvents.ScheduleEvent(EVENT_BURNING_BLOOD, 9.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_CAVE_BATS:
                        {
                            std::list<GameObject*> prisonsList;
                            GetGameObjectListWithEntryInGrid(prisonsList, me, GO_BAT_CAGE_1, 400.0f);
                            GetGameObjectListWithEntryInGrid(prisonsList, me, GO_BAT_CAGE_2, 400.0f);
                            GetGameObjectListWithEntryInGrid(prisonsList, me, GO_BAT_CAGE_3, 400.0f);
                            GetGameObjectListWithEntryInGrid(prisonsList, me, GO_BAT_CAGE_4, 400.0f);

                            for (auto&& itr : prisonsList)
                            {
                                itr->SetGoState(GO_STATE_ACTIVE);
                                itr->SetLootState(GO_ACTIVATED);
                            }

                            // Activate Bats
                            DoCast(me, SPELL_VAMPIRIC_FRENZY_PROC_2, true);
                            break;
                        }
                        case EVENT_YETI:
                            if (GameObject* yetiCage = me->FindNearestGameObject(GO_YETI_CAGE, 400.0f))
                            {
                                yetiCage->SetGoState(GO_STATE_ACTIVE);
                                yetiCage->SetLootState(GO_ACTIVATED);
                            }

                            // Activate Yeti
                            if (Creature* yeti = me->FindNearestCreature(NPC_STARVED_YETI, 400.0f, true))
                                yeti->AI()->DoAction(ACTION_START_INTRO);
                            break;
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DEFEANING_SCREECH:
                            DoCast(me, SPELL_DEAFENING_SCREECH);

                            if (canUseShockBlast())
                                berserkerEvents.RescheduleEvent(EVENT_SHOCK_BLAST, 2.5 * IN_MILLISECONDS);

                            // also delay for fearsome roar to victim
                            if (events.GetTimeUntilEvent(EVENT_FEARSOME_ROAR) < 1.5 * IN_MILLISECONDS && canUseShockBlast()) // delay only if it possible
                                events.RescheduleEvent(EVENT_FEARSOME_ROAR, 1.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_FEARSOME_ROAR:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, cleaveSpellEntry ? cleaveSpellEntry : SPELL_FEARSOME_ROAR);

                            events.ScheduleEvent(EVENT_FEARSOME_ROAR, 11 * IN_MILLISECONDS);
                            break;
                        case EVENT_TAIL_LASH:
                            DoCast(me, SPELL_TAIL_LASH);
                            events.ScheduleEvent(EVENT_TAIL_LASH, 10 * IN_MILLISECONDS);
                            break;
                        case EVENT_FIXATE:
                            if (stopFixate)
                                break;

                            DoCast(me, SPELL_FIXATE);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                void HandleResetPrisoners(bool onlyRespawn = false)
                {
                    std::list<Creature*> prisonersList;
                    GetCreatureListWithEntryInGrid(prisonersList, me, NPC_AKOLIK, 400.0f);
                    GetCreatureListWithEntryInGrid(prisonersList, me, NPC_WATERSPEAKER_GORAI, 400.0f);
                    GetCreatureListWithEntryInGrid(prisonersList, me, NPC_MONTAK, 400.0f);
                    GetCreatureListWithEntryInGrid(prisonersList, me, NPC_INKGILL_CAPTIVE, 400.0f);
                    GetCreatureListWithEntryInGrid(prisonersList, me, NPC_SKUMBLADE_CAPTIVE, 400.0f);
                    GetCreatureListWithEntryInGrid(prisonersList, me, NPC_GAI_CHO_REFUGE, 400.0f);

                    if (IsHeroic())
                    {
                        GetCreatureListWithEntryInGrid(prisonersList, me, NPC_CAPTIVE_CAVE_BAT, 400.0f);
                        GetCreatureListWithEntryInGrid(prisonersList, me, NPC_STARVED_YETI, 400.0f);
                    }

                    for (auto&& itr : prisonersList)
                    {
                        itr->Respawn();
                        itr->RemoveAurasDueToSpell(SPELL_SUICIDE_SELF_2);

                        if (!onlyRespawn)
                        {
                            if (itr->AI())
                                itr->AI()->EnterEvadeMode();

                            me->GetMap()->CreatureRelocation(itr, itr->GetHomePosition().GetPositionX(), itr->GetHomePosition().GetPositionY(), itr->GetHomePosition().GetPositionZ(), itr->GetHomePosition().GetOrientation());
                        }
                    }

                    // Reset Cages
                    for (auto&& itr : invCageType)
                    {
                        if (GameObject* prison = me->FindNearestGameObject(itr.first, 400.0f))
                        {
                            prison->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                            prison->SetGoState(GO_STATE_READY);
                            prison->SetLootState(GO_NOT_READY);
                        }
                    }

                    // Heroic
                    if (IsHeroic())
                    {
                        std::list<GameObject*> prisonsList;
                        GetGameObjectListWithEntryInGrid(prisonsList, me, GO_YETI_CAGE, 400.0f);
                        GetGameObjectListWithEntryInGrid(prisonsList, me, GO_BAT_CAGE_1, 400.0f);
                        GetGameObjectListWithEntryInGrid(prisonsList, me, GO_BAT_CAGE_2, 400.0f);
                        GetGameObjectListWithEntryInGrid(prisonsList, me, GO_BAT_CAGE_3, 400.0f);
                        GetGameObjectListWithEntryInGrid(prisonsList, me, GO_BAT_CAGE_4, 400.0f);

                        for (auto&& itr : prisonsList)
                        {
                            itr->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                            itr->SetGoState(GO_STATE_READY);
                            itr->SetLootState(GO_NOT_READY);
                        }
                    }
                }

                bool canUseShockBlast()
                {
                    uint32 acceleration = me->GetAura(SPELL_ACCELERATION) ? me->GetAura(SPELL_ACCELERATION)->GetStackAmount() : 0;
                    uint32 bloodFrenzyAmount = me->GetAura(SPELL_BLOOD_FRENZY_EFF) ? me->GetAura(SPELL_BLOOD_FRENZY_EFF)->GetStackAmount() : 0;

                    if (acceleration > 7 || bloodFrenzyAmount > 7)
                        return false;

                    return true;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_thok_the_bloodthirstyAI(creature);
        }
};

// Korkron Jailer 71658
struct npc_korkron_jailer : public ScriptedAI
{
    npc_korkron_jailer(Creature* creature) : ScriptedAI(creature) 
    {
        hasTriggered = false;
        hasClicked   = false;
    }

    EventMap events;
    bool hasTriggered;
    bool hasClicked;

    void Reset() override 
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (hasClicked)
            return;

        hasClicked = true;
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

        if (clicker->ToPlayer())
            clicker->ToPlayer()->AddItem(ITEM_SKELETON_KEY, 1);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (hasTriggered)
            damage = 0;

        if (damage >= me->GetHealth() && !hasTriggered)
        {
            damage = 0;
            events.Reset();
            hasTriggered = true;
            me->RemoveAllAuras();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, 64); // ref to spellclick (but doesn`t work...)
            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            me->PrepareChanneledCast(me->GetOrientation());
            me->SetHealth(1);
            me->SetFaction(35); // cuz ref doesn`t work
            me->SetFlag(UNIT_FIELD_FLAGS_2, 69240832);
            DoCast(me, SPELL_PERMANENT_FEIGN_DEATH, true);

            if (me->GetInstanceScript())
                me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTRO);
        events.ScheduleEvent(EVENT_ENRAGE, 7.5 * IN_MILLISECONDS);

        if (me->GetInstanceScript())
            me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_ENRAGE)
            {
                DoCast(me, SPELL_ENRAGE);
                events.ScheduleEvent(EVENT_ENRAGE, 13 * IN_MILLISECONDS);
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

struct helpers_baseAI : public ScriptedAI
{
    helpers_baseAI(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    bool hasTriggered;
    float x, y;

    void Reset() override
    {
        scheduler.CancelAll();
        hasTriggered = false;
        x = 0.0f; y = 0.0f;
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_NPC);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && !hasTriggered)
        {
            hasTriggered = true;

            Talk(TALK_INTRO);

            if (me->GetEntry() == NPC_WATERSPEAKER_GORAI) // heal eff to raid
                DoCast(me, SPELL_RESTORATIVE_WATERS);

            if (me->GetEntry() == NPC_MONTAK) // buff eff to raid
                DoCast(me, SPELL_FLAME_COATING);

            // Set our helpers to combat with us
            uint32 helperEntry = invDevourType.find(me->GetEntry())->second[2];
            
            std::list<Creature*> helpersList;
            GetCreatureListWithEntryInGrid(helpersList, me, helperEntry, 100.0f);

            for (auto&& itr : helpersList)
            {
                GetPositionWithDistInOrientation(itr, 70.0f, itr->GetAngle(thokBloodthristyRelocate[1].GetPositionX(), thokBloodthristyRelocate[1].GetPositionY()), x, y);
                itr->SetWalk(false);
                itr->GetMotionMaster()->MovePoint(0, x, y, -289.94f, 0.0f);
            }

            GetPositionWithDistInOrientation(me, 70.0f, me->GetAngle(thokBloodthristyRelocate[1].GetPositionX(), thokBloodthristyRelocate[1].GetPositionY()), x, y);
            me->SetWalk(false);
            me->GetMotionMaster()->MovePoint(0, x, y, -289.94f, 0.0f);

            // Pull
            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration() + 1000), [this](TaskContext context)
            {
                if (me->GetEntry() == NPC_AKOLIK)
                    Talk(TALK_SPECIAL_1);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_NPC);
                me->SetInCombatWithZone();

                // Set our helpers to death timer
                uint32 helperEntry = invDevourType.find(me->GetEntry())->second[2];

                std::list<Creature*> helpersList;
                GetCreatureListWithEntryInGrid(helpersList, me, helperEntry, 100.0f);

                for (auto&& itr : helpersList)
                    itr->CastSpell(itr, SPELL_SUICIDE_SELF_2, true);

                DoCast(me, invDevourType.find(me->GetEntry())->second[3]);
            });
        }
    }

    void EnterEvadeMode() override
    {
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetBossState(DATA_THOK_THE_BLOODTHIRSTY) == IN_PROGRESS)
            return;

        ScriptedAI::EnterEvadeMode();
        Reset();

        uint32 corpseDelay = me->GetCorpseDelay();
        uint32 respawnDelay = me->GetRespawnDelay();

        me->SetCorpseDelay(1);
        me->SetRespawnDelay(29);

        me->DespawnOrUnsummon();

        me->SetCorpseDelay(corpseDelay);
        me->SetRespawnDelay(respawnDelay);

        me->GetMap()->CreatureRelocation(me, me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ(), me->GetHomePosition().GetOrientation());
    }
};

struct npc_thok_helper_base : public helpers_baseAI
{
    npc_thok_helper_base(Creature* creature) : helpers_baseAI(creature) { }
};

// Akolik 71742
struct npc_akolik : public helpers_baseAI
{
    npc_akolik(Creature* creature) : helpers_baseAI(creature) 
    { 
        SetCombatMovement(false);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Waterspiker Gorai 71749
struct npc_soo_waterspeaker_gorai : public helpers_baseAI
{
    npc_soo_waterspeaker_gorai(Creature* creature) : helpers_baseAI(creature)
    {
        SetCombatMovement(false);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Montak 71763
struct npc_montak : public helpers_baseAI
{
    npc_montak(Creature* creature) : helpers_baseAI(creature) 
    {
        SetCombatMovement(false);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Ice Tomb 69398
struct npc_thok_ice_tomb : public ScriptedAI
{
    npc_thok_ice_tomb(Creature* creature) : ScriptedAI(creature)
    {
        SetCombatMovement(false);
    }

    ObjectGuid ownerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        if (Creature* thok = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_THOK_THE_BLOODTHIRSTY) : ObjectGuid::Empty))
            thok->AI()->JustSummoned(me);

        me->SetDisplayFromModel(1);
        ownerGUID = summoner->GetGUID();
        summoner->CastSpell(summoner, SPELL_SUMMON_ICE_TOMB_STUN, true);

        if (GameObject* go = me->SummonGameObject(GO_THOK_ICE_TOMB, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, { }, 0))
            me->AddGameObject(go);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Player* owner = ObjectAccessor::GetPlayer(*me, ownerGUID))
        {
            owner->RemoveAurasDueToSpell(SPELL_SUMMON_ICE_TOMB_STUN);
            owner->RemoveAurasDueToSpell(SPELL_FROZEN_SOLID);

            me->DespawnOrUnsummon(0.5 * IN_MILLISECONDS);
        }

        me->RemoveAllGameObjects();
    }

    void Unsummoned() override
    {
        if (Player* owner = ObjectAccessor::GetPlayer(*me, ownerGUID))
        {
            owner->RemoveAurasDueToSpell(SPELL_SUMMON_ICE_TOMB_STUN);
            owner->RemoveAurasDueToSpell(SPELL_FROZEN_SOLID);
        }

        me->RemoveAllGameObjects();
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Shock Collar 71645
struct npc_thok_shock_collar : public ScriptedAI
{
    npc_thok_shock_collar(Creature* creature) : ScriptedAI(creature) { }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        damage = 0;
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Captive Cave Bat 73522
struct npc_thok_captive_cave_bat : public ScriptedAI
{
    npc_thok_captive_cave_bat(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    float x, y;
    uint32 frenzyPct;

    void Reset() override
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
        scheduler.CancelAll();
        frenzyPct = urand(0, 30);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            me->SetWalk(false);
            GetPositionWithDistInOrientation(me, 45.0f, me->GetOrientation(), x, y);
            me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ(), me->GetOrientation());

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration() + urand(500, 1000)), [this](TaskContext context)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                me->SetInCombatWithZone();
            });
        }
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_VAMPIRIC_FRENZY)
            return frenzyPct;

        return 0;
    }

    void JustEngagedWith(Unit* /*who*/) override { }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
        Reset();

        uint32 corpseDelay = me->GetCorpseDelay();
        uint32 respawnDelay = me->GetRespawnDelay();

        me->SetCorpseDelay(1);
        me->SetRespawnDelay(29);

        me->DespawnOrUnsummon();

        me->SetCorpseDelay(corpseDelay);
        me->SetRespawnDelay(respawnDelay);

        me->GetMap()->CreatureRelocation(me, me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ(), me->GetHomePosition().GetOrientation());
    }

    void UpdateAI(uint32 diff) override 
    { 
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Starved Yeti 73526
struct npc_thok_starved_yeti : public ScriptedAI
{
    npc_thok_starved_yeti(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    float x, y;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
        scheduler.CancelAll();
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_START_INTRO:
                me->SetWalk(false);
                GetPositionWithDistInOrientation(me, 28.0f, me->GetOrientation(), x, y);
                me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ(), me->GetOrientation());

                scheduler
                    .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                {
                    me->GetMotionMaster()->MoveJump(thokBloodthristyRelocate[1], 25.0f, 25.0f, EVENT_JUMP);
                });
                break;
            case ACTION_WRECKING_BALL:
                break;
        }
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
        Reset();

        uint32 corpseDelay = me->GetCorpseDelay();
        uint32 respawnDelay = me->GetRespawnDelay();

        me->SetCorpseDelay(1);
        me->SetRespawnDelay(29);

        me->DespawnOrUnsummon();

        me->SetCorpseDelay(corpseDelay);
        me->SetRespawnDelay(respawnDelay);

        me->GetMap()->CreatureRelocation(me, me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ(), me->GetHomePosition().GetOrientation());
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetInCombatWithZone();

            scheduler
                .Schedule(Seconds(1), [this](TaskContext context)
            {
                DoCast(me, SPELL_WRECKING_BALL);
            });
        }

        if (pointId == EVENT_CHARGE)
        {
            me->RemoveAurasDueToSpell(SPELL_WRECKING_BALL_AT);

            me->SetFacingTo(Position::NormalizeOrientation(me->GetAngle(&thokBloodthristyRelocate[1]) - M_PI));
            DoCast(me, SPELL_WRECKING_BALL_BREAK);

            scheduler
                .Schedule(Milliseconds(16000), [this](TaskContext context)
            {
                DoCast(me, SPELL_WRECKING_BALL);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Monstrous Gastropod 73164
struct npc_thok_monstrous_gastropod : public ScriptedAI
{
    npc_thok_monstrous_gastropod(Creature* creature) : ScriptedAI(creature) 
    {
        hasTriggered = false;
    }

    TaskScheduler scheduler;
    float x, y;
    bool hasTriggered;

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && !hasTriggered)
        {
            hasTriggered = true;

            Movement::MoveSplineInit init(me);
            for (auto&& itr : gastropodPath)
                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

            if (Creature* thok = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_THOK_THE_BLOODTHIRSTY) : ObjectGuid::Empty))
                init.Path().push_back(G3D::Vector3(thok->GetPositionX(), thok->GetPositionY(), thok->GetPositionZ()));

            init.SetUncompressed();
            init.Launch();

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
            {
                DoCast(me, SPELL_MEGA_SNAIL_BOSS_MORPH);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// jinyu, Yaungol, Saurok Prisons 222011, 222047, 223005
class go_thok_helper_prisons : public GameObjectScript
{
    public:
        go_thok_helper_prisons() : GameObjectScript("go_thok_helper_prisons") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (player->HasAura(SPELL_SKELETON_KEY) && go->GetInstanceScript() && go->GetInstanceScript()->GetBossState(DATA_THOK_THE_BLOODTHIRSTY) == IN_PROGRESS)
                player->CastSpell(go, SPELL_UNLOCKING, true);

            return true; // not open by default
        }
};

// Third-Locked Cage 222030
class go_thok_third_locked_cage : public GameObjectScript
{
    public:
        go_thok_third_locked_cage() : GameObjectScript("go_thok_third_locked_cage") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (player->HasAura(SPELL_SKELETON_KEY) && go->GetInstanceScript() && go->GetInstanceScript()->GetBossState(DATA_THOK_THE_BLOODTHIRSTY) == IN_PROGRESS)
            {
                if (Creature* thok = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(DATA_THOK_THE_BLOODTHIRSTY) : ObjectGuid::Empty))
                {
                    thok->AI()->SetData(TYPE_GIANT_SNAIL_LOCKED_COUNT, 1);

                    player->RemoveAurasDueToSpell(SPELL_SKELETON_KEY);

                    // Summon Jailer
                    thok->SummonCreature(NPC_KORKRON_JAILER, jailerEntrancePos, TEMPSUMMON_MANUAL_DESPAWN);

                    if (thok->AI()->GetData(TYPE_GIANT_SNAIL_LOCKED_COUNT) > 2)
                    {
                        go->SetGoState(GO_STATE_ACTIVE);
                        go->SetLootState(GO_ACTIVATED);
                    }
                }
            }

            return true; // not open by default
        }
};

// Kill Jailers 147994
class spell_thok_kill_jailers : public SpellScript
{
    PrepareSpellScript(spell_thok_kill_jailers);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_KORKRON_JAILER_THRASH; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_kill_jailers::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Shock Blast 143706
class spell_thok_shock_blast : public SpellScript
{
    PrepareSpellScript(spell_thok_shock_blast);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_SHOCK_COLLAR; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_shock_blast::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Shock Blast Eff 143707
class spell_thok_shock_blast_eff : public SpellScript
{
    PrepareSpellScript(spell_thok_shock_blast_eff);

    std::list<WorldObject*> m_targets, copyTargets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // Try not select targets with tank specs

        uint8 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 10 : 4;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 4; // min 4, max 10
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 12)
                targetsCount++;

            if (totalPlayersCount > 15)
                targetsCount++;

            if (totalPlayersCount > 17)
                targetsCount++;

            if (totalPlayersCount > 19)
                targetsCount++;

            if (totalPlayersCount > 21)
                targetsCount++;

            if (totalPlayersCount > 23)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() >= targetsCount) // doent`t matter - MaxAffect is 4(10)
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 4; // min 4, max 10
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 12)
                targetsCount++;

            if (totalPlayersCount > 15)
                targetsCount++;

            if (totalPlayersCount > 17)
                targetsCount++;

            if (totalPlayersCount > 19)
                targetsCount++;

            if (totalPlayersCount > 21)
                targetsCount++;

            if (totalPlayersCount > 23)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_shock_blast_eff::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Power Regen 143409
class spell_thok_power_regen_eff : public SpellScript
{
    PrepareSpellScript(spell_thok_power_regen_eff);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* target = GetHitCreature())
        {
            uint32 acceleationValue = target->GetAura(SPELL_ACCELERATION) ? target->GetAura(SPELL_ACCELERATION)->GetStackAmount() : 0;

            if (acceleationValue > 8)
                acceleationValue = 8;

            additionalEnergy = invScreechType.find(acceleationValue)->second - GetSpellInfo()->Effects[EFFECT_0].BasePoints;

            if (additionalEnergy || acceleationValue == 0 && !additionalEnergy)
            {
                if (target->GetPower(POWER_MANA) + (int32)additionalEnergy > 99)
                    target->AI()->DoAction(ACTION_DEFEANING_SCREECH);
                else
                    target->SetPower(POWER_MANA, target->GetPower(POWER_MANA) + additionalEnergy);
            }
        }
    }

    private:
        uint32 additionalEnergy;

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_thok_power_regen_eff::HandleHit, EFFECT_0, SPELL_EFFECT_ENERGIZE);
    }
};

// Acceleration 143411
class spell_thok_acceleration : public AuraScript
{
    PrepareAuraScript(spell_thok_acceleration);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->SetPower(POWER_ENERGY, 0);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_thok_acceleration::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
    }
};

// Clump Check 143430
class spell_thok_clump_check : public SpellScript
{
    PrepareSpellScript(spell_thok_clump_check);

    std::list<WorldObject*> m_targets;

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* target = GetHitUnit())
        {
            // Remove bloodied if health upper 50%
            if (target->HealthAbovePct(50))
                target->RemoveAurasDueToSpell(SPELL_BLOODIED);
            else
                target->CastSpell(target, SPELL_BLOODIED, true);
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        // set phase 2 if 5(15) player with bloodied
        uint8 targetCount = GetCaster()->GetMap()->Is25ManRaid() ? 15 : 5;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetCount = 5; // min 5, max 15
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 14)
                targetCount += 5;

            if (totalPlayersCount > 20)
                targetCount += 5;
        }

        m_targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && !target->ToUnit()->HasAura(SPELL_BLOODIED); });

        // Prevent clump checking progress if in LFR mode
        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_LFR) && !GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
            return;

        // second phase
        if (m_targets.size() >= targetCount)
            if (Creature* thok = ObjectAccessor::GetCreature(*GetCaster(), GetCaster()->GetInstanceScript() ? GetCaster()->GetInstanceScript()->GetGuidData(DATA_THOK_THE_BLOODTHIRSTY) : ObjectGuid::Empty))
                thok->AI()->DoAction(ACTION_FRENZY);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_thok_clump_check::HandleHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_clump_check::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Fixate 143445
class spell_thok_fixate : public SpellScript
{
    PrepareSpellScript(spell_thok_fixate);

    std::list<WorldObject*> m_targets, copyTargets;

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->AI()->SetGUID(target->GetGUID());
                caster->AI()->DoAction(ACTION_FIXATE_DELAY);
            }
        }
    }

    void HandleFixate(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, GetSpellInfo()->Effects[effIndex].BasePoints);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // Try not select targets with tank specs

        // Try not select same target twice if possible
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && GetCaster() && GetCaster()->ToCreature() && GetCaster()->ToCreature()->AI()->GetGUID() == target->ToUnit()->GetGUID(); });

        if (targets.size() > 1)
        {
            Trinity::Containers::RandomResizeList(targets, 1);
            copyTargets = targets;
            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);

        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>&targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_thok_fixate::HandleHit, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
        OnEffectHitTarget += SpellEffectFn(spell_thok_fixate::HandleFixate, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_fixate::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_fixate::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Fixate 143445
class spell_thok_fixate_aura : public AuraScript
{
    PrepareAuraScript(spell_thok_fixate_aura);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetCaster() && GetCaster()->ToCreature())
            GetCaster()->ToCreature()->AI()->DoAction(ACTION_FIXATE);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_thok_fixate_aura::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Corrosive Blood 143795
class spell_thok_corrosive_blood : public SpellScript
{
    PrepareSpellScript(spell_thok_corrosive_blood);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_SHOCK_COLLAR; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_corrosive_blood::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Corrosive Blood Eff 143791
class spell_thok_corrosive_blood_eff : public SpellScript
{
    PrepareSpellScript(spell_thok_corrosive_blood_eff);

    std::list<WorldObject*> m_targets, copyTargets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // Try not select targets with tank specs

        uint8 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 7 : 2;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 2; // min 2, max 7
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 12)
                targetsCount++;

            if (totalPlayersCount > 15)
                targetsCount++;

            if (totalPlayersCount > 17)
                targetsCount++;

            if (totalPlayersCount > 19)
                targetsCount++;

            if (totalPlayersCount > 21)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() >= targetsCount) // doent`t matter - MaxAffect is 2(7)
        {
            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount);

            copyTargets = targets;
            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 2; // min 2, max 7
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 12)
                targetsCount++;

            if (totalPlayersCount > 15)
                targetsCount++;

            if (totalPlayersCount > 17)
                targetsCount++;

            if (totalPlayersCount > 19)
                targetsCount++;

            if (totalPlayersCount > 21)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() > targetsCount)
            Trinity::Containers::RandomResizeList(targets, targetsCount);

        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>&targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_corrosive_blood_eff::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_corrosive_blood_eff::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Icy Blood 143801
class spell_thok_icy_blood : public SpellScript
{
    PrepareSpellScript(spell_thok_icy_blood);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_SHOCK_COLLAR; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_icy_blood::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Icy Blood Eff 143800
class spell_thok_icy_blood_eff : public SpellScript
{
    PrepareSpellScript(spell_thok_icy_blood_eff);

    std::list<WorldObject*> m_targets, copyTargets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // Try not select targets with tank specs

        uint8 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 8 : 4;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 4; // min 4, max 8
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 12)
                targetsCount++;

            if (totalPlayersCount > 16)
                targetsCount++;

            if (totalPlayersCount > 19)
                targetsCount++;

            if (totalPlayersCount > 23)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() >= targetsCount) // doent`t matter - MaxAffect is 4(8)
        {
            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount);

            copyTargets = targets;
            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 4; // min 4, max 8
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 12)
                targetsCount++;

            if (totalPlayersCount > 16)
                targetsCount++;

            if (totalPlayersCount > 19)
                targetsCount++;

            if (totalPlayersCount > 23)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() > targetsCount)
            Trinity::Containers::RandomResizeList(targets, targetsCount);

        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>&targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_icy_blood_eff::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_icy_blood_eff::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Icy Blood 143800
class spell_thok_icy_blood_eff_aura : public AuraScript
{
    PrepareAuraScript(spell_thok_icy_blood_eff_aura);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetStackAmount() > 4 && GetOwner() && GetOwner()->ToUnit())
        {
            GetOwner()->ToUnit()->RemoveAurasDueToSpell(GetSpellInfo()->Id);
            GetOwner()->ToUnit()->CastSpell(GetOwner()->ToUnit(), SPELL_SUMMON_ICE_TOMB, true);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_thok_icy_blood_eff_aura::HandleAuraEffectApply, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAPPLY);
    }
};

// Summon Ice Tomb Stun 136893
class spell_thok_summon_ice_tomb_stun : public AuraScript
{
    PrepareAuraScript(spell_thok_summon_ice_tomb_stun);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (aurEff->GetBase()->GetDuration() > 0)
            return;

        if (Player* owner = GetOwner()->ToPlayer())
        {
            std::list<TempSummon*> iceTombs;
            owner->GetSummons(iceTombs, NPC_ICE_TOMB);

            if (!iceTombs.empty())
                iceTombs.front()->CastSpell(owner, SPELL_FROZEN_SOLID, true); // to summoner
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_thok_summon_ice_tomb_stun::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// Burning Blood 143781
class spell_thok_burning_blood : public SpellScript
{
    PrepareSpellScript(spell_thok_burning_blood);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_SHOCK_COLLAR; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_burning_blood::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Burning Blood Selector 143782
class spell_thok_burning_blood_selector : public SpellScript
{
    PrepareSpellScript(spell_thok_burning_blood_selector);

    std::list<WorldObject*> m_targets;

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[effIndex].BasePoints, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // Try not select targets with tank specs

        uint8 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 5 : 2;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 2; // min 2, max 5
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 13)
                targetsCount++;

            if (totalPlayersCount > 17)
                targetsCount++;

            if (totalPlayersCount > 21)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() >= targetsCount) // doent`t matter - MaxAffect is 2(5)
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 2; // min 2, max 5
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 13)
                targetsCount++;

            if (totalPlayersCount > 17)
                targetsCount++;

            if (totalPlayersCount > 21)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_thok_burning_blood_selector::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_burning_blood_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Akolik Incantation 144016, Water of Purify 144013, Fire of the Spirits 144014
class spell_thok_akolik_incantation : public SpellScript
{
    PrepareSpellScript(spell_thok_akolik_incantation);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Creature* target = GetHitCreature())
            {
                target->AI()->Talk(TALK_INTRO, caster);
                target->AI()->DoAction(caster->GetEntry());
            }
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_THOK_THE_BLOODTHIRSTY; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_thok_akolik_incantation::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_akolik_incantation::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Bloodied 143452
class spell_thok_bloodied : public AuraScript
{
    PrepareAuraScript(spell_thok_bloodied);

    void OnUpdate(uint32 /*diff*/, AuraEffect* aurEff)
    {
        if (GetOwner() && GetOwner()->ToUnit() && GetOwner()->ToUnit()->GetInstanceScript() && GetOwner()->ToUnit()->GetInstanceScript()->GetBossState(DATA_THOK_THE_BLOODTHIRSTY) != IN_PROGRESS)
            GetOwner()->ToUnit()->RemoveAurasDueToSpell(GetSpellInfo()->Id);
    }

    void Register() override
    {
        OnEffectUpdate += AuraEffectUpdateFn(spell_thok_bloodied::OnUpdate, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// Skeleton Key 146589
class spell_thok_skeleton_key : public AuraScript
{
    PrepareAuraScript(spell_thok_skeleton_key);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Player* owner = GetOwner()->ToPlayer())
        {
            if (owner->HasItemCount(ITEM_SKELETON_KEY, 1))
                owner->DestroyItemCount(ITEM_SKELETON_KEY, 1, true);

            // Call new prisoner in case if key aren`t used
            if (aurEff->GetBase()->GetDuration() > 0 || owner->GetInstanceScript() && owner->GetInstanceScript()->GetBossState(DATA_THOK_THE_BLOODTHIRSTY) != IN_PROGRESS)
                return;

            // Summon Jailer
            if (Creature* thok = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_THOK_THE_BLOODTHIRSTY) : ObjectGuid::Empty))
                thok->SummonCreature(NPC_KORKRON_JAILER, jailerEntrancePos, TEMPSUMMON_MANUAL_DESPAWN);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_thok_skeleton_key::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Unlocking 143917
class spell_thok_unlocking : public SpellScript
{
    PrepareSpellScript(spell_thok_unlocking);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            if (caster->GetInstanceScript() && caster->GetInstanceScript()->GetBossState(DATA_THOK_THE_BLOODTHIRSTY) != IN_PROGRESS)
            {
                PreventHitEffect(effIndex);
                return;
            }

            if (!caster->HasAura(SPELL_SKELETON_KEY) || !caster->HasItemCount(ITEM_SKELETON_KEY, 1))
            {
                PreventHitEffect(effIndex);
                return;
            }

            // Nothing for this cage
            if (GetHitGObj() && GetHitGObj()->GetEntry() == GO_THRICE_LOCKED_CAGE)
            {
                PreventHitEffect(effIndex);
                return;
            }

            if (GameObject* go = GetHitGObj())
            {
                if (Creature* prisoner = ObjectAccessor::GetCreature(*go, go->GetInstanceScript() ? go->GetInstanceScript()->GetGuidData(invCageType.find(go->GetEntry())->second) : ObjectGuid::Empty))
                {
                    if (!prisoner->IsAlive()) // cage has a twice sectors
                    {
                        PreventHitEffect(effIndex);
                        return;
                    }

                    caster->RemoveAurasDueToSpell(SPELL_SKELETON_KEY);

                    go->SetGoState(GO_STATE_ACTIVE);
                    go->SetLootState(GO_ACTIVATED);

                    prisoner->AI()->DoAction(ACTION_START_INTRO);
                }
            }
        }
    }

    void HandleUnlock(SpellEffIndex effIndex)
    {
        if (GetCaster() && GetCaster()->ToPlayer() && GetCaster()->HasAura(SPELL_SKELETON_KEY) && GetCaster()->ToPlayer()->HasItemCount(ITEM_SKELETON_KEY, 1) && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetBossState(DATA_THOK_THE_BLOODTHIRSTY) == IN_PROGRESS && GetHitGObj() && GetHitGObj()->GetEntry() != GO_THRICE_LOCKED_CAGE)
        {
            if (Creature* prisoner = ObjectAccessor::GetCreature(*GetCaster(), GetCaster()->GetInstanceScript() ? GetCaster()->GetInstanceScript()->GetGuidData(invCageType.find(GetHitGObj()->GetEntry())->second) : ObjectGuid::Empty))
            {
                if (!prisoner->IsAlive()) // cage has a twice sectors
                {
                    PreventHitEffect(effIndex);
                    return;
                }
            }
        }
        else
            PreventHitEffect(effIndex);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_thok_unlocking::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnEffectHitTarget += SpellEffectFn(spell_thok_unlocking::HandleHit, EFFECT_1, SPELL_EFFECT_OPEN_LOCK);
    }
};

// Freezing Breath 143773
class spell_thok_freezing_breath : public AuraScript
{
    PrepareAuraScript(spell_thok_freezing_breath);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetStackAmount() > 4 && GetOwner() && GetOwner()->ToUnit())
        {
            GetOwner()->ToUnit()->RemoveAurasDueToSpell(GetSpellInfo()->Id);
            GetOwner()->ToUnit()->CastSpell(GetOwner()->ToUnit(), SPELL_SUMMON_ICE_TOMB, true);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_thok_freezing_breath::HandleAuraEffectApply, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAPPLY);
    }
};

// Shock Blast Bloodied 143991
class spell_thok_shock_blast_bloodied : public AuraScript
{
    PrepareAuraScript(spell_thok_shock_blast_bloodied);

    void OnUpdate(uint32 /*diff*/, AuraEffect* aurEff)
    {
        if (GetOwner() && GetOwner()->ToUnit() && GetOwner()->ToUnit()->GetInstanceScript() && GetOwner()->ToUnit()->GetInstanceScript()->GetBossState(DATA_THOK_THE_BLOODTHIRSTY) != IN_PROGRESS)
            GetOwner()->ToUnit()->RemoveAurasDueToSpell(GetSpellInfo()->Id);
    }

    void Register() override
    {
        OnEffectUpdate += AuraEffectUpdateFn(spell_thok_shock_blast_bloodied::OnUpdate, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// Flame Coating 144115
class spell_thok_flame_coating : public AuraScript
{
    PrepareAuraScript(spell_thok_flame_coating);

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (Unit* target = eventInfo.GetProcTarget())
                owner->CastSpell(target, SPELL_FLAME_COATING_EFF, true);
    }

    void Register()
    {
        OnProc += AuraProcFn(spell_thok_flame_coating::HandleOnProc);
    }
};

// Deafening Screech 143343
class spell_thok_deafening_screech : public SpellScript
{
    PrepareSpellScript(spell_thok_deafening_screech);

    void HandleHit(SpellEffIndex effIndex)
    {
        int32 damage = GetHitDamage();

        // Blizzlike nerf 10% for 25 ppl (was on ptr, for 10 ppls after 1-2 month same)
        if (GetCaster() && GetCaster()->GetMap()->IsHeroic())
            damage = (int32)((damage * 90) / 100);

        if (GetCaster() && GetCaster()->GetMap()->IsHeroic() && GetCaster()->GetAura(SPELL_ACCELERATION) && GetCaster()->GetAura(SPELL_ACCELERATION)->GetStackAmount() > 30)
            SetHitDamage(damage * 2);
        else
            SetHitDamage(damage);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->GetTypeId() != TYPEID_PLAYER; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_thok_deafening_screech::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_deafening_screech::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_deafening_screech::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Wrecking Ball 147952
class spell_thok_wrecking_ball : public SpellScript
{
    PrepareSpellScript(spell_thok_wrecking_ball);

    void HandleBeforeCast(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            float selectedOri = Position::NormalizeOrientation((caster->GetOrientation() - M_PI) + frand(-M_PI / 4, M_PI / 4));

            x = thokBloodthristyRelocate[1].GetPositionX() + 78.0f * cos(selectedOri);
            y = thokBloodthristyRelocate[1].GetPositionY() + 78.0f * sin(selectedOri);
            z = caster->GetPositionZ();
            caster->UpdateAllowedPositionZ(x, y, z);

            if (Creature* bunny = GetCaster()->SummonCreature(NPC_GENERAL_FLY_BUNNY, x, y, z, 0.0f, TEMPSUMMON_MANUAL_DESPAWN))
            {
                if (Creature* thok = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(DATA_THOK_THE_BLOODTHIRSTY) : ObjectGuid::Empty))
                    thok->AI()->JustSummoned(bunny);

                bunny->CastSpell(bunny, SPELL_WRECKING_BALL_VISUAL, true);
            }
        }
    }

    void HandleAfterCast()
    {
        if (Unit* caster = GetCaster())
        {
            caster->CastSpell(caster, SPELL_WRECKING_BALL_AT, true);
            caster->GetMotionMaster()->MoveCharge(x, y, z, 30.0f, EVENT_CHARGE);
        }
    }

    private:
        float x, y, z;

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_thok_wrecking_ball::HandleBeforeCast, EFFECT_0, SPELL_EFFECT_DUMMY);
        AfterCast += SpellCastFn(spell_thok_wrecking_ball::HandleAfterCast);
    }
};

// Wrecking Ball Visual 147946
class spell_thok_wrecking_ball_visual : public AuraScript
{
    PrepareAuraScript(spell_thok_wrecking_ball_visual);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->DespawnOrUnsummon();
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_thok_wrecking_ball_visual::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Vampiric Frenzy 147980
class spell_thok_vampiric_frenzy : public SpellScript
{
    PrepareSpellScript(spell_thok_vampiric_frenzy);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* target = GetHitCreature())
            target->AI()->DoAction(ACTION_START_INTRO);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_CAPTIVE_CAVE_BAT; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_thok_vampiric_frenzy::HandleHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_vampiric_frenzy::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_vampiric_frenzy::SelectTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Vampiric Frenzy 147980
class spell_thok_vampiric_frenzy_aura : public AuraScript
{
    PrepareAuraScript(spell_thok_vampiric_frenzy_aura);

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (GetOwner() && GetOwner()->ToCreature() && GetOwner()->ToCreature()->HealthAbovePct(GetOwner()->ToCreature()->AI()->GetData(TYPE_VAMPIRIC_FRENZY)))
            PreventDefaultAction();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_thok_vampiric_frenzy_aura::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// Fearsome Roar 143426
class spell_thok_fearsome_roar : public SpellScript
{
    PrepareSpellScript(spell_thok_fearsome_roar);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->GetTypeId() != TYPEID_PLAYER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thok_fearsome_roar::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_24);
    }
};

// Summon Ice Tomb 136929
class spell_thok_summon_ice_tomb : public SpellScript
{
    PrepareSpellScript(spell_thok_summon_ice_tomb);

    void HandleHit(SpellEffIndex effIndex)
    {
        // We haven`t default tools for bind it by caster
        PreventHitEffect(effIndex);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_thok_summon_ice_tomb::HandleHit, EFFECT_1, SPELL_EFFECT_SUMMON_OBJECT_WILD);
    }
};

// Dummy Nuke 146538
class spell_thok_dummy_nuke : public SpellScript
{
    PrepareSpellScript(spell_thok_dummy_nuke);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* target = GetHitUnit())
        {
            int32 damage = (int32)target->GetMaxHealth();
            SetHitDamage(damage);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_thok_dummy_nuke::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Mega Snail vs Giant Dinosaur Morph 148355
class spell_thok_mega_snail_vs_giant_dinosaur_morph : public AuraScript
{
    PrepareAuraScript(spell_thok_mega_snail_vs_giant_dinosaur_morph);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->CastSpell(owner, SPELL_MEGA_SNAIL_ACHIEV_SELECT, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_thok_mega_snail_vs_giant_dinosaur_morph::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1025. Summoned by 143440 - Blood Frenzy
class sat_thok_blood_frenzy : public IAreaTriggerAura
{
    void OnInit() override
    {
        GetAreaTrigger()->AttachToTarget();
    }

    bool CheckTriggering(WorldObject* triggering) override
    {
        // Not Devour our victim - instakill on melee attack
        return triggering && triggering->ToPlayer() && GetCaster() && GetCaster()->ToCreature() && GetCaster()->HasInArc(M_PI / 4, triggering->ToPlayer()) && GetCaster()->ToCreature()->AI()->GetData(TYPE_BLOOD_FRENZY);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_DUMMY_NUKE, true);

            itr->CastSpell(itr, Trinity::Containers::SelectRandomContainerElement(meatExplosions), true);
        }
    }
};

// 1045. Summoned by 143783 - Burning Blood
class sat_thok_burning_blood : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && GetAreaTrigger() && GetAreaTrigger()->GetDuration() < 238000; // delay
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_BURNING_BLOOD_AT_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_BURNING_BLOOD_AT_EFF);
    }

};

// 1296. Summoned by 147609 - Wrecking Ball
class sat_thok_wrecking_ball : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_WRECKING_BALL_EFF, true);
    }
};

void AddSC_thok_the_bloodthirsty()
{
    new boss_thok_the_bloodthirsty();
    new creature_script<npc_korkron_jailer>("npc_korkron_jailer");
    new creature_script<npc_akolik>("npc_akolik");
    new creature_script<npc_soo_waterspeaker_gorai>("npc_soo_waterspeaker_gorai");
    new creature_script<npc_montak>("npc_montak");
    new creature_script<npc_thok_helper_base>("npc_thok_helper_base");
    new creature_script<npc_thok_ice_tomb>("npc_thok_ice_tomb");
    new creature_script<npc_thok_shock_collar>("npc_thok_shock_collar");
    new creature_script<npc_thok_captive_cave_bat>("npc_thok_captive_cave_bat");
    new creature_script<npc_thok_starved_yeti>("npc_thok_starved_yeti");
    new creature_script<npc_thok_monstrous_gastropod>("npc_thok_monstrous_gastropod");
    new go_thok_helper_prisons();
    new go_thok_third_locked_cage();

    new spell_script<spell_thok_kill_jailers>("spell_thok_kill_jailers");
    new spell_script<spell_thok_shock_blast>("spell_thok_shock_blast");
    new spell_script<spell_thok_shock_blast_eff>("spell_thok_shock_blast_eff");
    new spell_script<spell_thok_power_regen_eff>("spell_thok_power_regen_eff");
    new aura_script<spell_thok_acceleration>("spell_thok_acceleration");
    new spell_script<spell_thok_clump_check>("spell_thok_clump_check");
    new spell_script<spell_thok_fixate>("spell_thok_fixate");
    new aura_script<spell_thok_fixate_aura>("spell_thok_fixate_aura");
    new spell_script<spell_thok_corrosive_blood>("spell_thok_corrosive_blood");
    new spell_script<spell_thok_corrosive_blood_eff>("spell_thok_corrosive_blood_eff");
    new spell_script<spell_thok_icy_blood>("spell_thok_icy_blood");
    new spell_script<spell_thok_icy_blood_eff>("spell_thok_icy_blood_eff");
    new aura_script<spell_thok_icy_blood_eff_aura>("spell_thok_icy_blood_eff_aura");
    new aura_script<spell_thok_summon_ice_tomb_stun>("spell_thok_summon_ice_tomb_stun");
    new spell_script<spell_thok_burning_blood>("spell_thok_burning_blood");
    new spell_script<spell_thok_burning_blood_selector>("spell_thok_burning_blood_selector");
    new spell_script<spell_thok_akolik_incantation>("spell_thok_akolik_incantation");
    new aura_script<spell_thok_bloodied>("spell_thok_bloodied");
    new aura_script<spell_thok_skeleton_key>("spell_thok_skeleton_key");
    new spell_script<spell_thok_unlocking>("spell_thok_unlocking");
    new aura_script<spell_thok_freezing_breath>("spell_thok_freezing_breath");
    new aura_script<spell_thok_shock_blast_bloodied>("spell_thok_shock_blast_bloodied");
    new aura_script<spell_thok_flame_coating>("spell_thok_flame_coating");
    new spell_script<spell_thok_deafening_screech>("spell_thok_deafening_screech");
    new spell_script<spell_thok_wrecking_ball>("spell_thok_wrecking_ball");
    new spell_script<spell_thok_vampiric_frenzy>("spell_thok_vampiric_frenzy");
    new aura_script<spell_thok_vampiric_frenzy_aura>("spell_thok_vampiric_frenzy_aura");
    new aura_script<spell_thok_wrecking_ball_visual>("spell_thok_wrecking_ball_visual");
    new spell_script<spell_thok_fearsome_roar>("spell_thok_fearsome_roar");
    new spell_script<spell_thok_summon_ice_tomb>("spell_thok_summon_ice_tomb");
    new spell_script<spell_thok_dummy_nuke>("spell_thok_dummy_nuke");
    new aura_script<spell_thok_mega_snail_vs_giant_dinosaur_morph>("spell_thok_mega_snail_vs_giant_dinosaur_morph");
    new atrigger_script<sat_thok_blood_frenzy>("sat_thok_blood_frenzy");
    new atrigger_script<sat_thok_burning_blood>("sat_thok_burning_blood");
    new atrigger_script<sat_thok_wrecking_ball>("sat_thok_wrecking_ball");
};
