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

#include "GameObjectAI.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "terrace_of_endless_spring.h"
#include "GridNotifiers.h"

static const Position aLeiWps[] =
{
    { -1018.36f, -2880.54f, 19.64f, 4.71f },
    { -1018.20f, -2851.89f, 38.09f, 1.56f },
    { -1017.8f,  -2771.9f,  38.09f, 4.71f },
};

enum LeiShiSpells
{
    SPELL_AFRAID            = 123181,
    SPELL_SPRAY             = 123121,
    SPELL_HIDE              = 123244,
    SPELL_HIDE_SUMMON       = 123213,
    SPELL_HIDE_STACKS       = 123233,
    SPELL_GET_AWAY          = 123461,
    SPELL_GET_AWAY_TELEPORT = 123441,
    SPELL_PROTECT           = 123250,
    SPELL_PROTECT_RESPAWN   = 123493,
    SPELL_PROTECT_VISUAL    = 123505,
    SPELL_CLOUDED_REFLECTION= 123620,
    SPELL_CLOUDED_IMPACT    = 123625,

    // This is for Heroic Mode
    SPELL_SCARY_FOG_CIRCLE  = 123797,
    SPELL_SCARY_FOG_DOT     = 123705,
    SPELL_SCARY_FOG_STACKS  = 123712,

    SPELL_LEI_SHI_TRANSFORM = 127535,

    SPELL_PARASITIC_CLUTCH    = 125652,
    SPELL_FACE_CLUTCHERS      = 125724,
    SPELL_PLAYER_VEHICLE_AURA = 125697,
    SPELL_LEI_SHI_KILL_CREDIT = 123903,
};

enum LeiShiEvents
{
    // Lei Shi
    EVENT_SPECIAL,
    EVENT_SPRAY,
    EVENT_GET_AWAY,
    EVENT_GET_AWAY_CAST,
    EVENT_PROTECT,
    EVENT_HIDE,

    // Lei Shi (hidden)
    EVENT_HIDDEN_SPRAY,
    EVENT_BERSERK,

    // OOC
    EVENT_WP_1,
    EVENT_WP_2,
    EVENT_WP_3,

    // Misc
    EVENT_CHECK_WIPE    = 12,
};

enum LeiShiActions
{
    ACTION_ANIMATED_PROTECTOR_DIED,
    ACTION_ACTIVATE_ANIMATED_PROTECTORS,
    ACTION_TERMINATE_GET_AWAY_PHASE,
    ACTION_TERMINATE_HIDE_PHASE,
    ACTION_DISPERSE,
};

enum LeiShiSays
{
    TALK_AGGRO = 1,
    TALK_HIDE,
    TALK_GET_AWAY,
    TALK_SLAY,
    TALK_INTRO,
    TALK_DEFEATED,
    TALK_OUTRO_1,
    TALK_OUTRO_2
};

enum ReflectionSays
{
    EMOTE_RIPPLE,
    EMOTE_RIPPLE_2,
    EMOTE_APPEAR
};

Position leiShiPos = { -1017.93f, -2911.3f, 19.902f, 4.74f };

Position hidePositions[4] =
{
    { -990.678f,  -2890.043f, 19.172f, 3.781f },
    { -994.835f,  -2933.835f, 19.172f, 2.377f },
    { -1041.316f, -2932.84f,  19.172f, 0.733f },
    { -1045.101f, -2890.742f, 19.172f, 5.646f }
};

#define HEROIC_DIST_TO_VORTEX 21.0f
#define DIST_TO_SCARY_FOG_DOT 4.5f

enum
{
    NPC_PARASITOID_SHA          = 64346,

    PARASITOID_DATA_LOCATION    = 1,
    PARASITOID_LOCATION_LEFT    = 0,
    PARASITOID_LOCATION_RIGHT   = 1,
    PARASITOID_LOCATION_MAX     = 2,
};

class boss_lei_shi : public CreatureScript
{
    public:
        boss_lei_shi() : CreatureScript("boss_lei_shi") { }

        struct boss_lei_shiAI : public BossAI
        {
            boss_lei_shiAI(Creature* creature) : BossAI(creature, DATA_LEI_SHI)
            {
                leiShiFreed = false;
                me->SetReactState(REACT_PASSIVE);
                SetCombatMovement(false);
            }

            EventMap m_mEvents, wipeEvents;
            uint8 nextAfraidPct;
            uint8 nextProtectPct;
            uint8 endCombatPct;
            std::list<ObjectGuid> animatedProtectors;
            bool hidden;
            bool shielded;
            bool getAwayPhase;
            bool protectScheduled;
            bool leiShiFreed;
            bool nextSpecial;
            float getAwayHealthPct;

            void Reset() override
            {
                _Reset();

                summons.DespawnAll();
                wipeEvents.Reset();

                hidden              = false;
                getAwayPhase        = false;
                shielded            = false;
                protectScheduled    = false;
                nextSpecial         = (bool)urand(0, 1);

                nextAfraidPct       = 90;
                nextProtectPct      = 80;
                endCombatPct        = 2;

                if (instance && instance->GetData(TYPE_LEI_INTRO) == DONE)
                {
                    if (instance->GetData(DATA_LEI_SHI) == NOT_STARTED)
                        instance->SetData(TYPE_LEI_INTRO, DONE);
                    else
                    {
                        me->RestoreDisplayId();
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                }

                if (instance)
                {
                    if (instance->GetData(TYPE_LEI_INTRO) == DONE)
                    {
                        if (instance->GetData(DATA_LEI_SHI) == NOT_STARTED)
                            instance->SetData(TYPE_LEI_INTRO, DONE);
                        else
                        {
                            me->RestoreDisplayId();
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        }
                    }
                    // Will work only if server was restarted and pre-event not completed
                    else if (instance->GetData(TYPE_TSULONG) == DONE)
                    {
                        uint32 delay = 0;
                        me->m_Events.Schedule(delay += 2000, 10, [this]()
                        {
                            instance->SetData(TYPE_LEI_INTRO, SPECIAL);
                        });
                    }
                }

                me->RemoveAura(SPELL_AFRAID);
                me->RemoveAura(SPELL_HIDE);
                me->RemoveAura(SPELL_HIDE_STACKS);

                if (Creature* pReflection = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_REFLECTION_OF_LEI_SHI) : ObjectGuid::Empty))
                    pReflection->RemoveAura(SPELL_SCARY_FOG_CIRCLE);

                me->RemoveAura(SPELL_SCARY_FOG_DOT);
                me->RemoveAura(SPELL_SCARY_FOG_STACKS);
                me->RemoveAllAreasTrigger();

                events.Reset();
                animatedProtectors.clear();
                HandleProtectors(false);

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SCARY_FOG_CIRCLE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SCARY_FOG_DOT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SCARY_FOG_STACKS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SPRAY);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                if (leiShiFreed)
                    me->CastSpell(me, SPELL_LEI_SHI_TRANSFORM, true);

                if (IsHeroic())
                    if (Creature* pReflection = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_REFLECTION_OF_LEI_SHI) : ObjectGuid::Empty))
                        pReflection->RemoveAurasDueToSpell(SPELL_SCARY_FOG_CIRCLE);
            }

            void ScheduleTasks() override
            {
                if (!me->IsSpawnedInLFRDungeon())
                    scheduler.Schedule(Seconds(20), [=](TaskContext ctx)
                    {
                        SummonParasitoid(ctx);
                    });
            }

            void ScheduleSpecialSpell(bool first = true)
            {
                events.ScheduleEvent(nextSpecial ? EVENT_GET_AWAY : EVENT_HIDE, first ? 30 * IN_MILLISECONDS : 50 * IN_MILLISECONDS);
                nextSpecial = !nextSpecial;
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                if (instance)
                    instance->SetBossState(DATA_LEI_SHI, FAIL);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 45)
                {
                    me->SetFacingTo(4.71f);
                    m_mEvents.ScheduleEvent(EVENT_WP_2, 500);
                }

                if (pointId == 47)
                {
                    if (Creature* pSha = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SHA_OF_FEAR) : ObjectGuid::Empty))
                    {
                        if (pSha->AI())
                            pSha->AI()->DoAction(ACTION_SHA_INTRO);
                    }

                    me->SetDisplayId(17612);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                    events.Reset();
                    me->GetMotionMaster()->MoveIdle();
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                if (instance)
                {
                    me->CastSpell(me, SPELL_AFRAID, true);

                    if (auto const afraid = me->GetAuraEffect(SPELL_AFRAID, EFFECT_0))
                        afraid->ChangeAmount(0);

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                    if (IsHeroic())
                        if (Creature* pReflection = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_REFLECTION_OF_LEI_SHI) : ObjectGuid::Empty))
                            pReflection->CastSpell(pReflection, SPELL_SCARY_FOG_CIRCLE, true);

                    Talk(TALK_AGGRO);

                    HandleProtectors();

                    events.ScheduleEvent(EVENT_SPRAY, 400);
                    events.ScheduleEvent(EVENT_BERSERK, 10 * MINUTE * IN_MILLISECONDS);
                    ScheduleSpecialSpell();
                    wipeEvents.ScheduleEvent(EVENT_CHECK_WIPE, 3 * IN_MILLISECONDS);
                }

                DoCast(SPELL_PLAYER_VEHICLE_AURA);
            }

            void HandleProtectors(bool apply = true)
            {
                std::list<Creature*> protectors;
                me->GetCreatureListWithEntryInGrid(protectors, NPC_ANIMATED_PROTECTOR, 100.0f);

                for (auto&& itr : protectors)
                {
                    if (apply)
                        animatedProtectors.push_back(itr->GetGUID());

                    itr->Respawn();
                    itr->AddAura(SPELL_PROTECT_RESPAWN, itr);
                    itr->RemoveAura(SPELL_PROTECT_VISUAL);
                    itr->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(TALK_SLAY);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (!instance)
                    return;

                if (nextAfraidPct)
                {
                    if (me->HealthBelowPctDamaged(nextAfraidPct, damage))
                    {
                        if (auto const afraid = me->GetAuraEffect(SPELL_AFRAID, EFFECT_0))
                            afraid->ChangeAmount(afraid->GetAmount() + 8);

                        nextAfraidPct -= 10;
                    }

                    if (me->HealthBelowPctDamaged(int32(getAwayHealthPct - 4.0f), damage))
                        me->RemoveAura(SPELL_GET_AWAY);

                    if (nextProtectPct >= 20)
                    {
                        if (me->HealthBelowPctDamaged(nextProtectPct, damage) && !protectScheduled)
                        {
                            protectScheduled = true;
                            events.ScheduleEvent(EVENT_PROTECT, 100);
                        }
                    }
                }

                if (me->HealthBelowPctDamaged(endCombatPct, damage))
                {
                    damage = 0;

                    if (leiShiFreed)
                        return;

                    DoCastAOE(SPELL_LEI_SHI_KILL_CREDIT, true);
                    if (Player* player = me->GetMap()->GetFirstPlayerInInstance())
                        player->RewardPersonalLootAndCurrency(me);

                    HandleAchievement();
                    DespawnParasitoids();

                    summons.DespawnAll();
                    instance->SetBossState(DATA_LEI_SHI, DONE);

                    if (IsHeroic())
                        if (Creature* pReflection = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_REFLECTION_OF_LEI_SHI) : ObjectGuid::Empty))
                            pReflection->RemoveAurasDueToSpell(SPELL_SCARY_FOG_CIRCLE);

                    events.Reset();
                    wipeEvents.Reset();

                    EnterEvadeMode();

                    me->CombatStop();
                    me->DeleteThreatList();
                    me->SetFaction(35);
                    me->CastSpell(me, SPELL_LEI_SHI_TRANSFORM, true);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->RestoreDisplayId();
                    me->RemoveAura(SPELL_AFRAID);
                    me->RemoveAura(SPELL_HIDE);
                    me->RemoveAura(SPELL_HIDE_STACKS);

                    if (Creature* pReflection = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_REFLECTION_OF_LEI_SHI) : ObjectGuid::Empty))
                        pReflection->RemoveAura(SPELL_SCARY_FOG_CIRCLE);

                    me->RemoveAura(SPELL_SCARY_FOG_DOT);
                    me->RemoveAura(SPELL_SCARY_FOG_STACKS);
                    me->RemoveAura(SPELL_BERSERK);
                    me->RemoveAllAreasTrigger();
                    me->setRegeneratingHealth(false);

                    if (attacker && attacker->GetTypeId() == TYPEID_PLAYER)
                        me->GetMap()->ToInstanceMap()->PermBindAllPlayers(attacker->ToPlayer());
                    else if (attacker && attacker->GetTypeId() == TYPEID_UNIT && attacker->GetOwner() && attacker->GetOwner()->ToPlayer())
                        me->GetMap()->ToInstanceMap()->PermBindAllPlayers(attacker->GetOwner()->ToPlayer());

                    leiShiFreed = true;
                    Talk(TALK_DEFEATED);

                    m_mEvents.ScheduleEvent(EVENT_WP_1, 15000);

                    if (instance)
                    {
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SCARY_FOG_CIRCLE);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SCARY_FOG_DOT);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SCARY_FOG_STACKS);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SPRAY);
                        instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    }

                    me->RemoveAura(SPELL_PLAYER_VEHICLE_AURA);
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    if (instance->GetBossState(DATA_LEI_SHI) == DONE)
                        return;

                    instance->SetBossState(DATA_LEI_SHI, FAIL);
                }

                _EnterEvadeMode();
                wipeEvents.Reset();
                summons.DespawnAll();
                DespawnParasitoids();

                _DespawnAtEvade();
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (!hidden)
                    return;

                if (!caster)
                    return;

                if (caster->GetTypeId() == TYPEID_UNIT && (caster->GetEntry() == NPC_LEI_SHI || caster->GetEntry() == NPC_LEI_SHI_HIDDEN))
                    return;

                if (!spell)
                    return;

                if (!spell->HasEffect(SPELL_EFFECT_SCHOOL_DAMAGE) &&
                    !spell->HasEffect(SPELL_EFFECT_NORMALIZED_WEAPON_DMG) &&
                    !spell->HasEffect(SPELL_EFFECT_WEAPON_DAMAGE) &&
                    !spell->HasEffect(SPELL_EFFECT_WEAPON_PERCENT_DAMAGE) &&
                    !spell->HasEffect(SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL))
                    return;

                if (auto const hide = me->GetAura(SPELL_HIDE_STACKS))
                    hide->ModCharges(-1);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_ANIMATED_PROTECTOR_DIED:
                    {
                        me->RemoveAura(SPELL_PROTECT);
                        shielded = false;
                        protectScheduled = false;
                        nextProtectPct -= 20;

                        Creature* protector = NULL;

                        for (auto&& itr : animatedProtectors)
                        {
                            if (Creature* elemental = me->GetMap()->GetCreature(itr))
                            {
                                if (!elemental->IsAlive())
                                    protector = elemental;

                                elemental->RemoveAura(SPELL_PROTECT_VISUAL);
                                elemental->AddAura(SPELL_PROTECT_RESPAWN, elemental);
                                elemental->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                            }
                        }

                        if (protector)
                            animatedProtectors.remove(protector->GetGUID());

                        break;
                    }
                    case ACTION_ACTIVATE_ANIMATED_PROTECTORS:
                    {
                        if (animatedProtectors.empty())
                            break;

                        me->CastSpell(me, SPELL_PROTECT, false);
                        shielded = true;
                        uint8 protectorsActivated = 0;
                        for (auto&& itr : animatedProtectors)
                        {
                            // 3 Animated Protectors at 80% and 60%
                            if (((me->GetMap()->IsHeroic() && protectorsActivated >= 4) ||
                                (!me->GetMap()->IsHeroic() && protectorsActivated >= 3)) &&
                                nextProtectPct > 40)
                                break;

                            // 4 Animated Protectors at 40% and 20%
                            if (((me->GetMap()->IsHeroic() && protectorsActivated >= 5) ||
                                (!me->GetMap()->IsHeroic() && protectorsActivated >= 4)) &&
                                nextProtectPct <= 40)
                                break;

                            if (Creature* protector = me->GetMap()->GetCreature(itr))
                            {
                                protector->SetHealth(protector->GetMaxHealth());
                                protector->RemoveAura(SPELL_PROTECT_RESPAWN);
                                protector->CastSpell(protector, SPELL_PROTECT_VISUAL, true);
                                protector->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);

                                if (protector->AI())
                                    protector->AI()->DoZoneInCombat();

                                ++protectorsActivated;
                            }
                        }

                        break;
                    }
                    case ACTION_TERMINATE_GET_AWAY_PHASE:
                    {
                        getAwayPhase = false;
                        me->RemoveAllAreasTrigger();

                        ScheduleSpecialSpell(false);
                        break;
                    }
                    case ACTION_TERMINATE_HIDE_PHASE:
                    {
                        me->RemoveAura(SPELL_HIDE);
                        me->RemoveAura(SPELL_HIDE_STACKS);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        me->RestoreDisplayId();
                        hidden = false;

                        if (IsHeroic())
                            if (Creature* pReflection = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_REFLECTION_OF_LEI_SHI) : ObjectGuid::Empty))
                                pReflection->CastSpell(me, SPELL_SCARY_FOG_CIRCLE, true);

                        // Only have Lei Shi (hidden) in summons
                        summons.DespawnAll();

                        ScheduleSpecialSpell(false);
                        break;
                    }
                    case ACTION_LEISHI_INTRO:
                        me->SetDisplayId(43783);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                        me->SetReactState(REACT_AGGRESSIVE);
                        Talk(TALK_INTRO);
                        
                        if (Creature* pReflection = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_REFLECTION_OF_LEI_SHI) : ObjectGuid::Empty))
                        {
                            pReflection->SetDisplayId(17612);
                            pReflection->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                        }

                        if (GameObject* pVortexWall = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GO_WALL_OF_LEI_SHI) : ObjectGuid::Empty))
                            pVortexWall->SetGoState(GO_STATE_ACTIVE);

                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                m_mEvents.Update(diff);

                switch (m_mEvents.ExecuteEvent())
                {
                    case EVENT_WP_1:
                        Talk(TALK_OUTRO_1);
                        me->SetWalk(true);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(45, aLeiWps[0]);
                        break;
                    case EVENT_WP_2:
                        Talk(TALK_OUTRO_2);
                        me->GetMotionMaster()->MovePoint(46, aLeiWps[1]);
                        m_mEvents.ScheduleEvent(EVENT_WP_3, 8000);
                        break;
                    case EVENT_WP_3:
                        me->SetWalk(false);
                        me->GetMotionMaster()->MovePoint(47, aLeiWps[2]);
                        break;
                }

                wipeEvents.Update(diff);

                while (uint32 eventId = wipeEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_CHECK_WIPE)
                    {
                        if (instance && instance->IsWipe(50.0f, me))
                        {
                            events.Reset();
                            EnterEvadeMode();
                            break;
                        }
                        wipeEvents.ScheduleEvent(EVENT_CHECK_WIPE, 1 * IN_MILLISECONDS);
                    }
                    break;
                }

                if (!UpdateVictim())
                    return;

                if (hidden)
                    return;

                events.Update(diff);
                scheduler.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            break;
                        case EVENT_SPRAY:
                        {
                            if (getAwayPhase || me->HasUnitState(UNIT_STATE_CASTING))
                            {
                                events.ScheduleEvent(EVENT_SPRAY, 400);
                                break;
                            }

                            if (Unit* target = me->GetVictim())
                                me->CastSpell(target, SPELL_SPRAY, false);

                            events.ScheduleEvent(EVENT_SPRAY, 400);
                            break;
                        }
                        case EVENT_GET_AWAY:
                        {
                            if (shielded)
                            {
                                events.ScheduleEvent(EVENT_GET_AWAY, 1 * IN_MILLISECONDS);
                                break;
                            }
                            else
                                me->InterruptNonMeleeSpells(true);

                            // Teleport Lei Shi to the center of the room
                            Talk(TALK_GET_AWAY);
                            me->CastSpell(me, SPELL_GET_AWAY_TELEPORT, true);
                            getAwayPhase = true;
                            getAwayHealthPct = me->GetHealthPct();
                            events.ScheduleEvent(EVENT_GET_AWAY_CAST, 300);
                            break;
                        }
                        case EVENT_GET_AWAY_CAST:
                        {
                            me->CastSpell(me, SPELL_GET_AWAY, false);
                            break;
                        }
                        case EVENT_PROTECT:
                        {
                            if (getAwayPhase)
                            {
                                events.RescheduleEvent(EVENT_PROTECT, 1 * IN_MILLISECONDS);
                                break;
                            }
                            else
                                me->InterruptNonMeleeSpells(true);

                            DoAction(ACTION_ACTIVATE_ANIMATED_PROTECTORS);
                            break;
                        }
                        case EVENT_HIDE:
                        {
                            if (shielded)
                            {
                                events.RescheduleEvent(EVENT_HIDE, 1 * IN_MILLISECONDS);
                                break;
                            }
                            else
                                me->InterruptNonMeleeSpells(true);

                            if (Creature* pReflection = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_REFLECTION_OF_LEI_SHI) : ObjectGuid::Empty))
                                pReflection->RemoveAurasDueToSpell(SPELL_SCARY_FOG_CIRCLE);

                            Talk(TALK_HIDE);
                            me->CastSpell(me, SPELL_HIDE, false);
                            hidden = true;
                            break;
                        }
                    }
                }
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() != NPC_PARASITOID_SHA)
                {
                    summons.Summon(summon);
                    if (me->IsInCombat())
                        DoZoneInCombat(summon);
                }
            }

            void SummonParasitoid(TaskContext ctx)
            {
                if (TryToSummonParasitoid(PARASITOID_LOCATION_MAX))
                    ctx.Repeat(Seconds(15));
                else
                    ctx.Repeat(Seconds(1));
            }

            bool TryToSummonParasitoid(uint32 preferedLocation)
            {
                bool locationBusy[PARASITOID_LOCATION_MAX]{};

                for (auto&& itr : me->GetSummons())
                    if (itr->GetEntry() == NPC_PARASITOID_SHA)
                        if (!itr->GetVehicle() && itr->AI()->GetData(1) < 2)
                            locationBusy[itr->AI()->GetData(PARASITOID_DATA_LOCATION)] = true;

                if (locationBusy[0] && locationBusy[1])
                    return false;

                uint32 location = preferedLocation == PARASITOID_LOCATION_MAX ?
                    urand(PARASITOID_LOCATION_LEFT, PARASITOID_LOCATION_RIGHT) : preferedLocation;

                if (locationBusy[location])
                {
                    for (uint32 i = 0; i < PARASITOID_LOCATION_MAX; ++i)
                        if (!locationBusy[i])
                            location = i;
                }

                SummonParasitoid(location);
                return true;
            }

            void SummonParasitoid(uint32 location)
            {
                Position const pos[PARASITOID_LOCATION_MAX]
                {
                    { -1056.78f, -2950.57f, 2.29f, 5.63f },
                    {  -979.63f, -2949.90f, 2.29f, 3.70f },
                };

                if (Creature* parasitoid = me->SummonCreature(NPC_PARASITOID_SHA, pos[location]))
                {
                    parasitoid->AI()->SetData(PARASITOID_DATA_LOCATION, location);
                }
            }

            void EnforceParasitoidSpawn(uint32 parasitoidSpawnLocation)
            {
                scheduler.Schedule(Seconds(5), [=](TaskContext)
                {
                    TryToSummonParasitoid(parasitoidSpawnLocation);
                });
            }

            void DespawnParasitoids()
            {
                std::list<TempSummon*> summons;
                me->GetSummons(summons, NPC_PARASITOID_SHA);
                for (auto&& itr : summons)
                    itr->UnSummon();
            }

            void HandleAchievement()
            {
                if (!instance)
                    return;

                for (auto&& itr : instance->instance->GetPlayers())
                    if (!itr.GetSource()->HasAura(SPELL_PARASITIC_CLUTCH))
                        return;

                instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, 125724, 0, me);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_lei_shiAI(creature);
        }
};

// Animated Protector - 62995
class npc_animated_protector : public CreatureScript
{
    public:
        npc_animated_protector() : CreatureScript("npc_animated_protector") { }

        struct npc_animated_protectorAI : public ScriptedAI
        {
            npc_animated_protectorAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* leiShi = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LEI_SHI) : ObjectGuid::Empty))
                    leiShi->AI()->DoAction(ACTION_ANIMATED_PROTECTOR_DIED);
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_animated_protectorAI(creature);
        }
};

// Corrupted Protector - 63275
class npc_lei_shi_corrupted_protector : public CreatureScript
{
    public:
        npc_lei_shi_corrupted_protector() : CreatureScript("npc_lei_shi_corrupted_protector") { }

        enum iSpell
        {
            SPELL_DISPERSE = 123610,
        };

        enum iEvents
        {
            EVENT_DISPERSE = 1,
        };

        struct npc_lei_shi_corrupted_protectorAI : public ScriptedAI
        {
            npc_lei_shi_corrupted_protectorAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            bool InFlee;

            void Reset() override
            {
                me->GetMotionMaster()->MoveRandom(1.5f);
                me->RemoveAurasDueToSpell(SPELL_DISPERSE);
                InFlee = false;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_DISPERSE, 1 * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_DISPERSE)
                    events.ScheduleEvent(EVENT_DISPERSE, 2.5 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_DISPERSE)
                    {
                        me->RemoveAurasDueToSpell(SPELL_DISPERSE);
                        DoCast(me, SPELL_DISPERSE);
                        me->ClearUnitState(UNIT_STATE_CASTING);
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lei_shi_corrupted_protectorAI(creature);
        }
};

// Lei Shi (hidden) - 63099
class npc_lei_shi_hidden : public CreatureScript
{
    enum
    {
        SPELL_HIDE_VISUAL       = 132363,
        SPELL_HIDE_PROC         = 123233,
        SPELL_HIDE_TRIGGER      = 132362,
    };
    public:
        npc_lei_shi_hidden() : CreatureScript("npc_lei_shi_hidden") { }

        struct npc_lei_shi_hiddenAI : public ScriptedAI
        {
            npc_lei_shi_hiddenAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            EventMap events;

            void SpellHitTarget(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_HIDE_TRIGGER)
                    caster->CastSpell(caster, SPELL_HIDE_VISUAL, true);
            }

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);

                events.Reset();
                events.ScheduleEvent(EVENT_HIDDEN_SPRAY, 400);

                if (Creature* leiShi = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LEI_SHI) : ObjectGuid::Empty))
                {
                    auto const afraid = leiShi->GetAuraEffect(SPELL_AFRAID, EFFECT_0);
                    if (!afraid)
                        return;

                    if (!me->HasAura(SPELL_AFRAID))
                    {
                        if (auto const newAfraid = me->AddAura(SPELL_AFRAID, me))
                            if (newAfraid->GetEffect(0))
                                newAfraid->GetEffect(0)->ChangeAmount(afraid->GetAmount());
                    }
                    else
                    {
                        if (auto const newAfraid = me->GetAuraEffect(SPELL_AFRAID, EFFECT_0))
                            newAfraid->ChangeAmount(afraid->GetAmount());
                    }
                }

                DoCast(me, SPELL_HIDE_PROC, true);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_HIDDEN_SPRAY:
                    {
                        if (me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            events.ScheduleEvent(EVENT_HIDDEN_SPRAY, 0);
                            break;
                        }

                        ObjectGuid leiShiGuid = ObjectGuid::Empty;

                        if (instance)
                            leiShiGuid = instance->GetGuidData(NPC_LEI_SHI);

                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f))
                            me->CastSpell(target, SPELL_SPRAY, false, NULL, NULL, leiShiGuid);

                        events.ScheduleEvent(EVENT_HIDDEN_SPRAY, 400);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lei_shi_hiddenAI(creature);
        }
};

class npc_lei_shi_reflection : public CreatureScript
{
    enum eEvents : uint32
    {
        EVENT_NONE,
        EVENT_RIPPLE_1,
        EVENT_RIPPLE_2,
        EVENT_APPEAR
    };

    public:
        npc_lei_shi_reflection() : CreatureScript("npc_lei_shi_reflection") { }

        struct npc_lei_shi_reflectionAI : public ScriptedAI
        {
            npc_lei_shi_reflectionAI(Creature* creature) : ScriptedAI(creature) 
            {
                eventDone = false;
            }

            EventMap events;
            bool eventDone;

            void Reset() override
            {
                if (IsHeroic())
                    me->RemoveAurasDueToSpell(SPELL_SCARY_FOG_CIRCLE);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_LEISHI_INTRO && !eventDone)
                {
                    eventDone = true;
                    me->ClearUnitState(UNIT_STATE_STUNNED);
                    events.ScheduleEvent(EVENT_RIPPLE_1, 2000);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RIPPLE_1:
                            DoCast(SPELL_CLOUDED_IMPACT);
                            Talk(EMOTE_RIPPLE);
                            events.ScheduleEvent(EVENT_RIPPLE_2, 14000);
                            break;
                        case EVENT_RIPPLE_2:
                            DoCast(SPELL_CLOUDED_IMPACT);
                            Talk(EMOTE_RIPPLE_2);
                            events.ScheduleEvent(EVENT_APPEAR, 10000);
                            break;
                        case EVENT_APPEAR:
                            if (Creature* pLeiShi = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_LEI_SHI) : ObjectGuid::Empty))
                            {
                                Talk(EMOTE_APPEAR, pLeiShi);

                                if (pLeiShi->AI())
                                    pLeiShi->AI()->DoAction(ACTION_LEISHI_INTRO);
                            }
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lei_shi_reflectionAI(creature);
        }
};

// 64346 - Parasitoid Sha
struct npc_parasitoid_sha : public ScriptedAI
{
    npc_parasitoid_sha(Creature* c) : ScriptedAI(c) { }

    uint32 spawnLocation = PARASITOID_LOCATION_MAX;

    void SetData(uint32 type, uint32 data) override
    {
        if (type == PARASITOID_DATA_LOCATION)
            spawnLocation = data;
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == PARASITOID_DATA_LOCATION)
            return spawnLocation;
        return 0;
    }

    void UpdateAI(uint32 diff) override
    {
        if (me->GetVehicle())
            return;

        if (!UpdateVictim())
            return;

        std::list<Player*> players;
        me->GetPlayerListInGrid(players, 1.0f);

        players.remove_if([](Player* player)
        {
            return player->HasAuraType(SPELL_AURA_CONTROL_VEHICLE);
        });

        if (!players.empty())
        {
            DoCast(*players.begin(), SPELL_PARASITIC_CLUTCH);
            if (me->GetInstanceScript())
                if (Creature* leiShi = ObjectAccessor::GetCreature(*me, me->GetInstanceScript()->GetGuidData(DATA_LEI_SHI)))
                    if (auto ai = dynamic_cast<boss_lei_shi::boss_lei_shiAI*>(leiShi->AI()))
                        ai->EnforceParasitoidSpawn(spawnLocation);
        }
    }
};

// Get Away ! - 123461
class spell_get_away : public SpellScriptLoader
{
    public:
        spell_get_away() : SpellScriptLoader("spell_get_away") { }

        class spell_get_away_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_get_away_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (caster->ToCreature() && caster->ToCreature()->AI())
                        caster->ToCreature()->AI()->DoAction(ACTION_TERMINATE_GET_AWAY_PHASE);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_get_away_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_get_away_AuraScript();
        }
};

// Hide - 123244
class spell_hide : public SpellScriptLoader
{
    public:
        spell_hide() : SpellScriptLoader("spell_hide") { }

        class spell_hide_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hide_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    uint8 pos = urand(0, 3);

                    caster->NearTeleportTo(hidePositions[pos].GetPositionX(),
                                           hidePositions[pos].GetPositionY(),
                                           hidePositions[pos].GetPositionZ(),
                                           hidePositions[pos].GetOrientation());

                    caster->CastSpell(caster, SPELL_HIDE_STACKS, true);
                    caster->CastSpell(caster, SPELL_HIDE_SUMMON, true);

                    caster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    caster->SetDisplayId(11686);
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    caster->RemoveAura(SPELL_HIDE_STACKS);
                    caster->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    caster->RestoreDisplayId();
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hide_AuraScript::OnApply, EFFECT_1, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hide_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hide_AuraScript();
        }
};

// Get Away damage - 123467
class spell_get_away_dmg : public SpellScriptLoader
{
    public:
        spell_get_away_dmg() : SpellScriptLoader("spell_get_away_dmg") { }

        class spell_get_away_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_get_away_dmg_SpellScript);

            void ModifyDamage(SpellEffIndex /*effIndex*/)
            {
                Unit*  caster = GetCaster();
                Player * victim = GetHitPlayer();

                if (!caster || !victim)
                    return;

                if (victim->isMoving() && !victim->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_BACKWARD) && victim->isInFront(caster))
                    SetHitDamage(GetHitDamage() / 2);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_get_away_dmg_SpellScript::ModifyDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_get_away_dmg_SpellScript();
        }
};

// Hide (stacks) - 123233
class spell_hide_stacks : public SpellScriptLoader
{
    public:
        spell_hide_stacks() : SpellScriptLoader("spell_hide_stacks") { }

        class spell_hide_stacks_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hide_stacks_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (caster->ToCreature() && caster->ToCreature()->AI())
                        caster->ToCreature()->AI()->DoAction(ACTION_TERMINATE_HIDE_PHASE);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_hide_stacks_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hide_stacks_AuraScript();
        }
};

// Scary Fog (DoT) - 123705
class spell_scary_fog_dot : public SpellScriptLoader
{
    public:
        spell_scary_fog_dot() : SpellScriptLoader("spell_scary_fog_dot") { }

        class spell_scary_fog_dot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_scary_fog_dot_SpellScript);

            void CorrectRange(std::list<WorldObject*>& targets)
            {
                targets.clear();

                if (Unit* caster = GetCaster())
                {
                    Map::PlayerList const& players = caster->GetMap()->GetPlayers();

                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    {
                        if (Player* player = itr->GetSource())
                        {
                            if (player->GetExactDist2d(GetCaster()->GetPositionX(), GetCaster()->GetPositionY()) >= HEROIC_DIST_TO_VORTEX)
                                targets.push_back(player);
                        }
                    }

                    for (auto&& itr : targets)
                    {
                        if (itr->ToUnit())
                        {
                            if (auto const scaryFog = caster->AddAura(SPELL_SCARY_FOG_STACKS, itr->ToUnit()))
                            {
                                scaryFog->SetDuration(35000);
                                scaryFog->SetMaxDuration(35000);
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_scary_fog_dot_SpellScript::CorrectRange, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_scary_fog_dot_SpellScript();
        }

        class spell_scary_fog_dot_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_scary_fog_dot_AuraScript);

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                if (Unit* target = GetTarget())
                    target->CastSpell(target, SPELL_SCARY_FOG_STACKS, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_scary_fog_dot_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_scary_fog_dot_AuraScript();
        }
};

// Scary Fog (stacks) - 123712
class spell_scary_fog_stacks : public SpellScriptLoader
{
    public:
        spell_scary_fog_stacks() : SpellScriptLoader("spell_scary_fog_stacks") { }

        class spell_scary_fog_stacks_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_scary_fog_stacks_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& targets)
            {
                for (auto&& itr : targets)
                {
                    if (itr->ToUnit() && itr->ToUnit()->GetEntry() != NPC_LEI_SHI_HIDDEN)
                    {
                        if (Unit* caster = GetCaster())
                        {
                            if (auto const scary = caster->GetAura(SPELL_SCARY_FOG_STACKS))
                            {
                                if (auto const scaryTarget = itr->ToUnit()->GetAura(SPELL_SCARY_FOG_STACKS))
                                    scaryTarget->SetStackAmount(scary->GetStackAmount());
                                else if (auto const scaryTarget = caster->AddAura(SPELL_SCARY_FOG_STACKS, itr->ToUnit()))
                                    scaryTarget->SetStackAmount(scary->GetStackAmount());
                            }
                        }
                    }
                }

                targets.clear();
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_scary_fog_stacks_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_scary_fog_stacks_SpellScript();
        }
};

// Disperse - 123610
class spell_lei_shi_disperse : public SpellScriptLoader
{
    public:
        spell_lei_shi_disperse() : SpellScriptLoader("spell_lei_shi_disperse") { }

        class spell_lei_shi_disperse_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_lei_shi_disperse_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* caster = GetCaster())
                    if (caster->ToCreature() && caster->ToCreature()->AI())
                        caster->ToCreature()->AI()->DoAction(ACTION_DISPERSE);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_lei_shi_disperse_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_lei_shi_disperse_SpellScript();
        }
};

// 125697 - Player Vehicle Aura
class spell_player_vehicle_aura : public AuraScript
{
    PrepareAuraScript(spell_player_vehicle_aura);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        // Confirmed by sniffs.
        GetTarget()->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_PLAYER_VEHICLE);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_player_vehicle_aura::HandleApply, EFFECT_0, SPELL_AURA_SET_VEHICLE_ID, AURA_EFFECT_HANDLE_REAL);
    }
};

// 408, 409. Summoned by 123461 - Get Away!
class sat_leishi_get_away : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringUpdate(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                if (target->IsAlive() && !target->HasForcedMovement())
                    target->SendApplyMovementForce(true, *caster, caster->GetMap()->IsHeroic() ? -7.5f : -5.0f);
                else if (!target->IsAlive() && target->HasForcedMovement())
                    target->SendApplyMovementForce(false, *caster);
            }
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                if (target->HasForcedMovement())
                    target->SendApplyMovementForce(false, *caster);
    }
};

void AddSC_boss_lei_shi()
{
    new boss_lei_shi();
    new npc_animated_protector();
    new npc_lei_shi_corrupted_protector();
    new npc_lei_shi_hidden();
    new npc_lei_shi_reflection();
    new creature_script<npc_parasitoid_sha>("npc_parasitoid_sha");
    new spell_get_away();
    new spell_get_away_dmg();
    new spell_hide();
    new spell_hide_stacks();
    new spell_scary_fog_dot();
    new spell_scary_fog_stacks();
    new spell_lei_shi_disperse();
    new aura_script<spell_player_vehicle_aura>("spell_player_vehicle_aura");
    new atrigger_script<sat_leishi_get_away>("sat_leishi_get_away");
}
