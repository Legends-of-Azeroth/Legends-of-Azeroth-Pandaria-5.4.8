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

#include "Unit.h"
#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "Battleground.h"
#include "CellImpl.h"
#include "ConditionMgr.h"
#include "CreatureAI.h"
#include "CreatureAIImpl.h"
#include "CreatureGroups.h"
#include "Creature.h"
#include "CreatureTextMgr.h"
#include "Formulas.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "InstanceSaveMgr.h"
#include "InstanceScript.h"
#include "Log.h"
#include "MapManager.h"
#include "MoveSpline.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "OutdoorPvP.h"
#include "PassiveAI.h"
#include "PetAI.h"
#include "Pet.h"
#include "Player.h"
#include "PlayerAI.h"
#include "QuestDef.h"
#include "Random.h"
#include "ReputationMgr.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "Spell.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "TemporarySummon.h"
#include "Totem.h"
#include "Transport.h"
#include "UpdateFieldFlags.h"
#include "Util.h"
#include "Vehicle.h"
#include "World.h"
#include "WorldPacket.h"
#include "MovementStructures.h"
#include "MovementPacketBuilder.h"
#include "WorldSession.h"
#include "BattlePetMgr.h"
#include "SpellHistory.h"
#include "AreaTrigger.h"
#include <math.h>
#include <numeric>
#include "SpellScript.h"
#include "PetBattle.h"
#include "GameEventMgr.h"

float baseMoveSpeed [MAX_MOVE_TYPE] =
{
    2.5f,                  // MOVE_WALK
    7.0f,                  // MOVE_RUN
    4.5f,                  // MOVE_RUN_BACK
    4.722222f,             // MOVE_SWIM
    2.5f,                  // MOVE_SWIM_BACK
    3.141594f,             // MOVE_TURN_RATE
    7.0f,                  // MOVE_FLIGHT
    4.5f,                  // MOVE_FLIGHT_BACK
    3.14f                  // MOVE_PITCH_RATE
};

float playerBaseMoveSpeed [MAX_MOVE_TYPE] =
{
    2.5f,                  // MOVE_WALK
    7.0f,                  // MOVE_RUN
    4.5f,                  // MOVE_RUN_BACK
    4.722222f,             // MOVE_SWIM
    2.5f,                  // MOVE_SWIM_BACK
    3.141594f,             // MOVE_TURN_RATE
    7.0f,                  // MOVE_FLIGHT
    4.5f,                  // MOVE_FLIGHT_BACK
    3.14f                  // MOVE_PITCH_RATE
};

// Used for prepare can/can`t triggr aura
static bool InitTriggerAuraData();
// Define can trigger auras
static bool isTriggerAura [TOTAL_AURAS];
// Define can't trigger auras (need for disable second trigger)
static bool isNonTriggerAura [TOTAL_AURAS];
// Triggered always, even from triggered spells
static bool isAlwaysTriggeredAura [TOTAL_AURAS];
// Prepare lists
static bool procPrepared = InitTriggerAuraData();

static int32 GetLevelDifferenceForPenalty(Unit const* attacker, Unit const* victim)
{
    int32 attackerLevel = attacker->GetLevel();
    int32 victimLevel = victim->GetLevelForTarget(attacker);
    int32 diff = victimLevel - attackerLevel;
    RoundToInterval(diff, -2, 3);
    return diff;
}

DamageInfo::DamageInfo(Unit* _attacker, Unit* _victim, uint32 _damage, SpellInfo const* _spellInfo, SpellSchoolMask _schoolMask, DamageEffectType _damageType)
    : m_attacker(_attacker), m_victim(_victim), m_damage(_damage), m_spellInfo(_spellInfo), m_schoolMask(_schoolMask),
    m_damageType(_damageType), m_attackType(BASE_ATTACK)
{
    m_absorb = 0;
    m_resist = 0;
    m_block = 0;
}
DamageInfo::DamageInfo(CalcDamageInfo& dmgInfo)
    : m_attacker(dmgInfo.attacker), m_victim(dmgInfo.target), m_damage(dmgInfo.damage), m_spellInfo(NULL), m_schoolMask(SpellSchoolMask(dmgInfo.damageSchoolMask)),
    m_damageType(DIRECT_DAMAGE), m_attackType(dmgInfo.attackType)
{
    m_absorb = 0;
    m_resist = 0;
    m_block = 0;
}

void DamageInfo::ModifyDamage(int32 amount)
{
    amount = std::min(amount, int32(GetDamage()));
    m_damage += amount;
}

void DamageInfo::AbsorbDamage(uint32 amount)
{
    amount = std::min(amount, GetDamage());
    m_absorb += amount;
    m_damage -= amount;
}

void DamageInfo::ResistDamage(uint32 amount)
{
    amount = std::min(amount, GetDamage());
    m_resist += amount;
    m_damage -= amount;
}

void DamageInfo::BlockDamage(uint32 amount)
{
    amount = std::min(amount, GetDamage());
    m_block += amount;
    m_damage -= amount;
}

ProcEventInfo::ProcEventInfo(Unit* actor, Unit* actionTarget, Unit* procTarget, uint32 typeMask, uint32 spellTypeMask,
    uint32 spellPhaseMask, uint32 hitMask, DamageInfo* damageInfo, HealInfo* healInfo, ProcTriggerContext const* ctx)
    : _actor(actor), _actionTarget(actionTarget), _procTarget(procTarget), _typeMask(typeMask), _spellTypeMask(spellTypeMask),
    _spellPhaseMask(spellPhaseMask), _hitMask(hitMask), _spell(ctx->TriggeredBySpell), _periodicEffect(ctx->TriggeredByAuraEffect),  _damageInfo(damageInfo), _healInfo(healInfo), _ctx(ctx)
{ }

uint32 ProcEventInfo::GetDamageWithoutResilience() const
{
    uint32 damage = _damageInfo->GetDamage();
    if (_actor && _actionTarget)
    {
        if (Player* target = _actionTarget->GetAffectingPlayer())
            if (Player* caster = _actor->GetAffectingPlayer())
                damage /= (1.0f + target->GetFloatValue(PLAYER_FIELD_MOD_RESILIENCE_PERCENT) / 100.0f);
    }
    return damage;
}

// we can disable this warning for this since it only
// causes undefined behavior when passed to the base class constructor
#ifdef _MSC_VER
#pragma warning(disable:4355)
#endif
Unit::Unit(bool isWorldObject) :
WorldObject(isWorldObject), m_movedPlayer(NULL), m_lastSanctuaryTime(0),
IsAIEnabled(false), NeedChangeAI(false), LastCharmerGUID(),
m_ControlledByPlayer(false), movespline(new Movement::MoveSpline()),
i_AI(NULL), i_disabledAI(NULL), m_procDeep(0),
m_removedAurasCount(0), i_motionMaster(new MotionMaster(this)), m_ThreatManager(this),
m_vehicle(NULL), m_vehicleKit(NULL), m_unitTypeMask(UNIT_MASK_NONE),
m_HostileRefManager(this),
_aiAnimKitId(0), _movementAnimKitId(0), _meleeAnimKitId(0)
{
#ifdef _MSC_VER
#pragma warning(default:4355)
#endif
    m_objectType |= TYPEMASK_UNIT;
    m_objectTypeId = TYPEID_UNIT;

    m_updateFlag = UPDATEFLAG_LIVING;

    for (auto i = 0; i < MAX_ATTACK; ++i)
    {
        m_attackTimer[i] = 0;
        m_attackTimerReminder[i] = 0;
        m_modAttackSpeedPct[i] = 1.0f;
    }

    m_extraAttacks = 0;
    m_canDualWield = false;
    m_VisibilityUpdScheduled = false;

    m_movementCounter = 0;

    m_state = 0;
    m_deathState = ALIVE;

    for (uint8 i = 0; i < CURRENT_MAX_SPELL; ++i)
        m_currentSpells [i] = NULL;

    for (uint8 i = 0; i < SUMMON_SLOT_MAX; ++i)
        m_SummonSlot [i].Clear();

    for (uint8 i = 0; i < MAX_GAMEOBJECT_SLOT; ++i)
        m_ObjectSlot [i].Clear();

    m_auraUpdateIterator = m_ownedAuras.end();

    m_transform = 0;
    m_canModifyStats = false;

    for (uint8 i = 0; i < MAX_SPELL_IMMUNITY; ++i)
        m_spellImmune [i].clear();

    for (uint8 i = 0; i < UNIT_MOD_END; ++i)
    {
        m_auraModifiersGroup [i] [BASE_VALUE] = 0.0f;
        m_auraModifiersGroup [i] [BASE_PCT] = 1.0f;
        m_auraModifiersGroup [i] [TOTAL_VALUE] = 0.0f;
        m_auraModifiersGroup [i] [TOTAL_PCT] = 1.0f;
    }
    // implement 50% base damage from offhand
    m_auraModifiersGroup [UNIT_MOD_DAMAGE_OFFHAND] [TOTAL_PCT] = 0.5f;

    for (uint8 i = 0; i < MAX_ATTACK; ++i)
    {
        m_weaponDamage [i] [MINDAMAGE] = BASE_MINDAMAGE;
        m_weaponDamage [i] [MAXDAMAGE] = BASE_MAXDAMAGE;
    }

    for (uint8 i = 0; i < MAX_STATS; ++i)
        m_createStats [i] = 0.0f;

    m_attacking = NULL;
    m_modMeleeHitChance = 0.0f;
    m_modRangedHitChance = 0.0f;
    m_modSpellHitChance = 0.0f;
    m_baseSpellCritChance = 5;

    for (uint8 i = 0; i < MAX_SPELL_SCHOOL; ++i)
        m_threatModifier [i] = 1.0f;

    m_isSorted = true;

    for (uint8 i = 0; i < MAX_MOVE_TYPE; ++i)
        m_speed_rate [i] = 1.0f;

    m_charmInfo = NULL;

    _redirectThreatInfo = RedirectThreatInfo();

    // remove aurastates allowing special moves
    for (uint8 i = 0; i < MAX_REACTIVE; ++i)
        m_reactiveTimer [i] = 0;

    m_cleanupDone = false;
    m_duringRemoveFromWorld = false;

    m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_ALIVE);

    _lastLiquid = NULL;
    _isWalkingBeforeCharm = false;

    SetTotalAuraEffectValue(SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK, 1.0f);
    SetTotalAuraEffectValue(SPELL_AURA_SPELL_SLOW, 1.0f);
    SetTotalAuraEffectValue(SPELL_AURA_MOD_HASTE, 1.0f);
    SetTotalAuraEffectValue(SPELL_AURA_MOD_SPEED_SLOW_ALL, 1.0f);
    SetTotalAuraEffectValue(SPELL_AURA_MOD_MELEE_RANGED_HASTE, 1.0f);
    SetTotalAuraEffectValue(SPELL_AURA_MOD_MELEE_RANGED_HASTE_2, 1.0f);
    SetTotalAuraEffectValue(SPELL_AURA_MOD_RANGED_HASTE, 1.0f);
    SetTotalAuraEffectValue(SPELL_AURA_HASTE_RANGED, 1.0f);
    SetTotalAuraEffectValue(SPELL_AURA_MOD_RANGED_HASTE_2, 1.0f);
    SetTotalAuraEffectValue(SPELL_AURA_MOD_ATTACKSPEED, 1.0f);
    SetTotalAuraEffectValue(SPELL_AURA_MOD_MELEE_HASTE, 1.0f);
    SetTotalAuraEffectValue(SPELL_AURA_MOD_MELEE_HASTE_2, 1.0f);
    SetTotalAuraEffectValue(SPELL_AURA_MOD_MELEE_HASTE_3, 1.0f);
    SetTotalAuraEffectValue(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, 1.0f); // Used for auto attack only

    m_spellHistory.reset(new SpellHistory(this));
}

////////////////////////////////////////////////////////////
// Methods of class GlobalCooldownMgr
bool GlobalCooldownMgr::HasGlobalCooldown(SpellInfo const* spellInfo) const
{
    GlobalCooldownList::const_iterator itr = m_GlobalCooldowns.find(spellInfo->StartRecoveryCategory);
    return itr != m_GlobalCooldowns.end() && itr->second.duration && getMSTimeDiff(itr->second.cast_time, getMSTime()) < itr->second.duration;
}

void GlobalCooldownMgr::AddGlobalCooldown(SpellInfo const* spellInfo, uint32 gcd)
{
    m_GlobalCooldowns [spellInfo->StartRecoveryCategory] = GlobalCooldown(gcd, getMSTime());
}

void GlobalCooldownMgr::CancelGlobalCooldown(SpellInfo const* spellInfo)
{
    m_GlobalCooldowns [spellInfo->StartRecoveryCategory].duration = 0;
}

////////////////////////////////////////////////////////////
// Methods of class Unit
Unit::~Unit()
{
    // set current spells as deletable
    for (uint8 i = 0; i < CURRENT_MAX_SPELL; ++i)
        if (m_currentSpells [i])
        {
            m_currentSpells [i]->SetReferencedFromCurrent(false);
            m_currentSpells [i] = NULL;
        }

    _DeleteRemovedAuras();

    delete i_motionMaster;
    delete m_charmInfo;
    delete movespline;

    ASSERT(!m_duringRemoveFromWorld);
    ASSERT(!m_attacking);
    ASSERT(m_attackers.empty());
    ASSERT(m_sharedVision.empty());
    ASSERT(m_Controlled.empty());
    ASSERT(m_appliedAuras.empty());
    ASSERT(m_ownedAuras.empty());
    ASSERT(m_removedAuras.empty());
    ASSERT(m_gameObj.empty());
    ASSERT(m_dynObj.empty());
}

void Unit::Update(uint32 p_time)
{
    // WARNING! Order of execution here is important, do not change.
    // Spells must be processed with event system BEFORE they go to _UpdateSpells.
    // Or else we may have some SPELL_STATE_FINISHED spells stalled in pointers, that is bad.
    m_Events.Update(p_time);

    UpdateStealthVisibility(p_time);

    if (!IsInWorld())
        return;

    _damageTrackingTimer -= p_time;
    while (_damageTrackingTimer <= 0)
    {
        std::copy(_npcDamageTaken.begin(), _npcDamageTaken.end() - 1, _npcDamageTaken.begin() + 1);
        std::copy(_playerDamageTaken.begin(), _playerDamageTaken.end() - 1, _playerDamageTaken.begin() + 1);

        _npcDamageTaken[0] = 0;
        _playerDamageTaken[0] = 0;

        _damageTrackingTimer += DAMAGE_TRACKING_UPDATE_INTERVAL;
    }

    if (m_comboPointResetTimer)
    {
        if (m_comboPointResetTimer > p_time)
            m_comboPointResetTimer -= p_time;
        else
        {
            m_comboPointResetTimer = 0;
            ClearComboPointHolders();
        }
    }

    _UpdateSpells(p_time);

    // If this is set during update SetCantProc(false) call is missing somewhere in the code
    // Having this would prevent spells from being proced, so let's crash
    ASSERT(!m_procDeep);

    if (CanHaveThreatList() && getThreatManager().isNeedUpdateToClient(p_time))
        SendThreatListUpdate();

    // update combat timer only for players and pets (only pets with PetAI)
    if (IsInCombat())
    {
        if (m_combatTimerPvP <= p_time)
            m_combatTimerPvP = 0;
        else
            m_combatTimerPvP -= p_time;

        if (m_combatTimerPvE <= p_time)
            m_combatTimerPvE = 0;
        else
            m_combatTimerPvE -= p_time;

        if (GetTypeId() == TYPEID_PLAYER || (ToCreature()->IsPet() && IsControlledByPlayer()))
        {
            bool canResetCombat = false;
            // Check UNIT_STATE_MELEE_ATTACKING or UNIT_STATE_CHASE (without UNIT_STATE_FOLLOW in this case) so pets can reach far away
            // targets without stopping half way there and running off.
            // These flags are reset after target dies or another command is given.
            if (!GetInstanceScript() || // Remove combat if not an instance
                !GetInstanceScript()->IsEncounterInProgress() || // Otherwise remove combat if boss encounter is not in progress
                !HasAuraType(SPELL_AURA_FEIGN_DEATH) && !HasAuraType(SPELL_AURA_MOD_STEALTH) && !HasAuraType(SPELL_AURA_MOD_INVISIBILITY)) // Otherwise remove combat if no Feign Death, Stealth or Invisibility auras are applied
            {
                canResetCombat = true;
            }

            if (canResetCombat && !m_combatTimerPvP)    // We are not fighting boss and PvP timer is expired or never was
            {
                if (m_HostileRefManager.isEmpty())      // No enemies, exit combat now
                    ClearInCombat();
                else if (!m_combatTimerPvE)             // Okay, we have some enemies and PvE combat timer is expired, check if they can reach us
                {
                    auto ref = m_HostileRefManager.getFirst();
                    do
                    {
                        Unit* target = ref->GetSource()->GetOwner();
                        ref = ref->next();
                        if (Creature* creature = target->ToCreature())
                            if (!creature->CanCreatureAttack(this, false))
                                m_HostileRefManager.deleteReference(target);
                    } while (ref);
                    if (m_HostileRefManager.isEmpty())
                        ClearInCombat();
                    else
                        m_combatTimerPvE = 5000;
                }
            }
        }
    }

    UpdateAttackTimer(BASE_ATTACK,   p_time);
    UpdateAttackTimer(OFF_ATTACK,    p_time);
    UpdateAttackTimer(RANGED_ATTACK, p_time);

    // update abilities available only for fraction of time
    UpdateReactives(p_time);

    if (IsAlive())
    {
        ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, HealthBelowPct(20));
        ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, HealthBelowPct(35));
        ModifyAuraState(AURA_STATE_HEALTH_ABOVE_75_PERCENT, HealthAbovePct(75));
    }

    UpdateSplineMovement(p_time);
    GetMotionMaster()->UpdateMotion(p_time);
}

void Unit::UpdateAttackTimer(WeaponAttackType type, uint32 diff)
{
    if (!m_attackTimer[type])
    {
        // Attack ready but was not performed in previous update, reset remainder
        m_attackTimerReminder[type] = 0;
        return;
    }

    if (m_attackTimer[type] >= diff)
    {
        m_attackTimer[type] -= diff;
        m_attackTimerReminder[type] = 0;
    }
    else
    {
        m_attackTimerReminder[type] = (diff - m_attackTimer[type]);
        m_attackTimer[type] = 0;
    }
}

bool Unit::HasOffhandWeapon() const
{
    if (GetTypeId() == TYPEID_PLAYER)
        return ToPlayer()->GetWeaponForAttack(OFF_ATTACK, true);
    else
        return m_canDualWield;
}

void Unit::MonsterMoveWithSpeed(float x, float y, float z, float speed, bool generatePath, bool forceDestination)
{
    Movement::MoveSplineInit init(this);
    init.MoveTo(x, y, z, generatePath, forceDestination);
    init.SetVelocity(speed);
    init.Launch();
}

void Unit::UpdateSplineMovement(uint32 t_diff)
{
    if (movespline->Finalized())
        return;

    movespline->updateState(t_diff);
    bool arrived = movespline->Finalized();

    if (movespline->isCyclic())
    {
        m_splineSyncTimer.Update(t_diff);
        if (m_splineSyncTimer.Passed())
        {
            m_splineSyncTimer.Reset(5000); // Retail value, do not change

            WorldPacket data(SMSG_FLIGHT_SPLINE_SYNC);
            Movement::PacketBuilder::WriteSplineSync(*movespline, data, this);
            SendMessageToSet(&data, true);
        }
    }

    if (arrived)
    {
        if (movespline->HasAnimation())
            SetAnimTier(movespline->GetAnimation());

        DisableSpline();
    }

    UpdateSplinePosition();
}

void Unit::UpdateSplinePosition()
{
    Movement::Location loc = movespline->ComputePosition();
    if (GetTransGUID())
    {
        Position& pos = m_movementInfo.transport.pos;
        pos.m_positionX = loc.x;
        pos.m_positionY = loc.y;
        pos.m_positionZ = loc.z;
        pos.SetOrientation(loc.orientation);

        if (TransportBase* transport = GetDirectTransport())
            transport->CalculatePassengerPosition(loc.x, loc.y, loc.z, &loc.orientation);
    }

    if (HasUnitState(UNIT_STATE_CANNOT_TURN))
        loc.orientation = GetOrientation();

    UpdatePosition(loc.x, loc.y, loc.z, loc.orientation);
}

void Unit::DisableSpline()
{
    m_movementInfo.RemoveMovementFlag(MOVEMENTFLAG_FORWARD);
    movespline->_Interrupt();
}

void Unit::resetAttackTimer(WeaponAttackType type)
{
    m_attackTimer[type] = uint32(GetAttackTime(type) * m_modAttackSpeedPct [type]);
    if (m_attackTimer[type] > m_attackTimerReminder[type])
        m_attackTimer[type] -= m_attackTimerReminder[type];
}

float Unit::GetMeleeReach() const
{
    float reach = m_floatValues [UNIT_FIELD_COMBAT_REACH];
    return reach > MIN_MELEE_REACH ? reach : MIN_MELEE_REACH;
}

bool Unit::IsWithinCombatRange(const Unit* obj, float dist2compare) const
{
    if (!obj || !IsInMap(obj) || !InSamePhase(obj))
        return false;

    float dx = GetPositionX() - obj->GetPositionX();
    float dy = GetPositionY() - obj->GetPositionY();
    float dz = GetPositionZ() - obj->GetPositionZ();
    float distsq = dx * dx + dy * dy + dz * dz;

    float sizefactor = GetCombatReach() + obj->GetCombatReach();
    float maxdist = dist2compare + sizefactor;

    return distsq < maxdist * maxdist;
}

bool Unit::IsWithinMeleeRange(const Unit* obj, float dist) const
{
    if (!obj || !IsInMap(obj) || !InSamePhase(obj))
        return false;

    static float const combatReachTolerance = 4.0f / 3.0f;

    float range = std::max(dist, GetCombatReach() + obj->GetCombatReach() + combatReachTolerance);

    // Clientside lag compensation
    if ((HasUnitMovementFlag(MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_STRAFE_RIGHT | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_FALLING)) &&
        (obj->HasUnitMovementFlag(MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_STRAFE_RIGHT | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_FALLING)) &&
        !IsWalking() && !obj->IsWalking())
        range += combatReachTolerance * 2;

    return GetExactDistSq(obj) < range * range;
}

bool Unit::IsWithinRangedMeleeRange(Unit const* obj, float dist) const
{
    if (!IsInMap(obj))
        return false;

    static float const combatReachTolerance = 4.0f / 3.0f;

    float range = std::max(NOMINAL_MELEE_RANGE, GetCombatReach() + obj->GetCombatReach() + combatReachTolerance) + dist;

    return GetExactDistSq(obj) < range * range;
}

void Unit::GetRandomContactPoint(const Unit* obj, float &x, float &y, float &z, float distance2dMin, float distance2dMax) const
{
    float combat_reach = GetCombatReach();
    if (combat_reach < 0.1f) // sometimes bugged for players
        combat_reach = DEFAULT_PLAYER_COMBAT_REACH;

    uint32 attacker_number = getAttackers().size();
    if (attacker_number > 0)
        --attacker_number;
    GetNearPoint(obj, x, y, z, obj->GetCombatReach(), distance2dMin + (distance2dMax - distance2dMin) * (float) rand_norm(),
                 GetAngle(obj) + (attacker_number ? (static_cast<float>(M_PI / 2) - static_cast<float>(M_PI) * (float) rand_norm()) * float(attacker_number) / combat_reach * 0.3f : 0));
}

AuraApplication * Unit::GetVisibleAura(uint8 slot) const
{
    VisibleAuraMap::const_iterator itr = m_visibleAuras.find(slot);
    if (itr != m_visibleAuras.end())
        return itr->second;
    return 0;
}

void Unit::SetVisibleAura(uint8 slot, AuraApplication * aur)
{
    m_visibleAuras [slot] = aur;
    UpdateAuraForGroup(slot);
}

void Unit::RemoveVisibleAura(uint8 slot)
{
    m_visibleAuras.erase(slot);
    UpdateAuraForGroup(slot);
}

void Unit::UpdateInterruptMask()
{
    m_interruptMask = { 0, 0 };
    for (AuraApplicationList::const_iterator i = m_interruptableAuras.begin(); i != m_interruptableAuras.end(); ++i)
        m_interruptMask |= (*i)->GetBase()->GetSpellInfo()->AuraInterruptFlags;

    if (Spell* spell = m_currentSpells [CURRENT_CHANNELED_SPELL])
        if (spell->getState() == SPELL_STATE_CASTING)
            m_interruptMask |= spell->m_spellInfo->ChannelInterruptFlags;
}

bool Unit::HasVisionObscured(Unit const* target) const
{
    if (!target)
        return false;

    Unit::AuraEffectList const& casterStateAuras = GetAuraEffectsByType(SPELL_AURA_INTERFERE_TARGETTING);
    Unit::AuraEffectList targetStateAuras = target->GetAuraEffectsByType(SPELL_AURA_INTERFERE_TARGETTING);

    for (auto&& casterAura : casterStateAuras)
    {
        Unit* caster = casterAura->GetCaster();
        // caster is friendly to Aura caster => No restrictions
        if (caster && IsFriendlyTo(caster))
            continue;

        bool failCast = true;
        for (auto&& targetAura : targetStateAuras)
        {
            if (casterAura->GetId() == targetAura->GetId() && casterAura->GetCasterGUID() == targetAura->GetCasterGUID())
            {
                failCast = false;
                targetStateAuras.remove(targetAura);
                break;
            }
        }

        if (failCast) // caster has some aura that target not
            return true;
    }

    // target has some aura that caster not
    for (auto&& taretAura : targetStateAuras)
        if (Unit* caster = taretAura->GetCaster())
            if (!IsFriendlyTo(caster))
                return true;

    return false;
}

bool Unit::HasAuraTypeWithFamilyFlags(AuraType auraType, uint32 familyName, uint32 familyFlags) const
{
    if (!HasAuraType(auraType))
        return false;
    AuraEffectList const& auras = GetAuraEffectsByType(auraType);
    for (AuraEffectList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
        if (SpellInfo const* iterSpellProto = (*itr)->GetSpellInfo())
            if (iterSpellProto->SpellFamilyName == familyName && iterSpellProto->SpellFamilyFlags [0] & familyFlags)
                return true;
    return false;
}

bool Unit::HasCrowdControlAuraType(AuraType type, uint32 excludeAura, uint32 dispelType) const
{
    AuraEffectList const& auras = GetAuraEffectsByType(type);
    for (AuraEffectList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
        if ((!excludeAura || excludeAura != (*itr)->GetSpellInfo()->Id) && //Avoid self interrupt of channeled Crowd Control spells like Seduction
            ((*itr)->GetSpellInfo()->Attributes & SPELL_ATTR0_BREAKABLE_BY_DAMAGE || (*itr)->GetSpellInfo()->HasAuraInterruptFlag(AURA_INTERRUPT_FLAG_TAKE_DAMAGE))
            && (!dispelType || dispelType == (*itr)->GetSpellInfo()->Dispel))
            return true;
    return false;
}

bool Unit::HasCrowdControlAura(Unit* excludeCasterChannel, uint32 dispelType) const
{
    uint32 excludeAura = 0;
    if (Spell* currentChanneledSpell = excludeCasterChannel ? excludeCasterChannel->GetCurrentSpell(CURRENT_CHANNELED_SPELL) : NULL)
        excludeAura = currentChanneledSpell->GetSpellInfo()->Id; //Avoid self interrupt of channeled Crowd Control spells like Seduction

    return (HasCrowdControlAuraType(SPELL_AURA_MOD_CONFUSE, excludeAura, dispelType)
        || HasCrowdControlAuraType(SPELL_AURA_MOD_FEAR, excludeAura, dispelType)
        || HasCrowdControlAuraType(SPELL_AURA_MOD_FEAR_2, excludeAura, dispelType)
        || HasCrowdControlAuraType(SPELL_AURA_MOD_STUN, excludeAura, dispelType)
        || HasCrowdControlAuraType(SPELL_AURA_MOD_ROOT, excludeAura, dispelType)
        || HasCrowdControlAuraType(SPELL_AURA_TRANSFORM, excludeAura, dispelType));
}


bool Unit::HasBreakableByDamageAuraType(AuraType type, uint32 excludeAura) const
{
    AuraEffectList const& auras = GetAuraEffectsByType(type);
    for (auto&& itr : auras)
        if ((!excludeAura || excludeAura != itr->GetSpellInfo()->Id) && // Avoid self interrupt of channeled Crowd Control spells like Seduction
            (itr->GetSpellInfo()->HasAttribute(SPELL_ATTR0_BREAKABLE_BY_DAMAGE) || itr->GetSpellInfo()->HasAuraInterruptFlag(AURA_INTERRUPT_FLAG_TAKE_DAMAGE) ||
            (itr->GetSpellInfo()->ProcFlags & TAKEN_HIT_PROC_FLAG_MASK && itr->GetSpellInfo()->ProcCharges == 1))) // Currently used only for crowd control auras, so just that
            return true;
    return false;
}

bool Unit::HasBreakableByDamageCrowdControlAura(Unit* excludeCasterChannel) const
{
    uint32 excludeAura = 0;
    if (Spell* currentChanneledSpell = excludeCasterChannel ? excludeCasterChannel->GetCurrentSpell(CURRENT_CHANNELED_SPELL) : NULL)
        excludeAura = currentChanneledSpell->GetSpellInfo()->Id; //Avoid self interrupt of channeled Crowd Control spells like Seduction

    // Scatter Shot. Has neither attribute nor interrupt flag
    if (HasAura(19503))
        return true;

    return (HasBreakableByDamageAuraType(SPELL_AURA_MOD_CONFUSE, excludeAura)
            || HasBreakableByDamageAuraType(SPELL_AURA_MOD_FEAR, excludeAura)
            || HasBreakableByDamageAuraType(SPELL_AURA_MOD_FEAR_2, excludeAura)
            || HasBreakableByDamageAuraType(SPELL_AURA_MOD_STUN, excludeAura)
            || HasBreakableByDamageAuraType(SPELL_AURA_TRANSFORM, excludeAura));
}

void Unit::DealDamageMods(Unit* victim, uint32 &damage, uint32* absorb)
{
    if (!victim || !victim->IsAlive() || victim->HasUnitState(UNIT_STATE_IN_FLIGHT) || (victim->GetTypeId() == TYPEID_UNIT && victim->ToCreature()->IsInEvadeMode()))
    {
        if (absorb)
            *absorb += damage;
        damage = 0;
    }
}

uint32 Unit::DealDamage(Unit* victim, uint32 damage, CleanDamage const* cleanDamage, DamageEffectType damagetype, SpellSchoolMask damageSchoolMask, SpellInfo const* spellProto, bool durabilityLoss)
{
    if (Creature* target = victim->ToCreature())
    {
        if (GetTypeId() == TYPEID_UNIT && !IsCharmedOwnedByPlayerOrPlayer())
        {
            float sparringLimitPct = target->GetSparringHealthLimit();

            if (sparringLimitPct != 0.0f)
            {
                if (damage >= target->GetHealth()) // First check: if we have a sparring limit we will never allow creatures to kill the sparring victim
                    damage = target->GetHealth() - 1;
                else if (target->GetHealthPct() <= sparringLimitPct) // Second check: stop incomming damage when we have surpassed the health limit
                    damage = 0;
            }
        }
    }

    if (GetTypeId() == TYPEID_PLAYER && ToPlayer()->GetSession()->GetSecurity() < SEC_GAMEMASTER && damage >= sWorld->getIntConfig(CONFIG_DAMAGE_LOG_MIN_DAMAGE))
    {
        float takenMod = victim->GetTotalAuraModifier(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
        float doneMod = GetTotalAuraModifier(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
        TC_LOG_INFO("damage", "Damage: %u, Attacker: %s (%u), Target (%s): %s (%u), Spell: %u, Difficulty: %s, Done: %g, Taken: %g",
            damage, GetName().c_str(), GetGUID().GetCounter(), victim->GetTypeId() == TYPEID_PLAYER ? "player" : "creature",
            victim->GetName().c_str(), victim->GetTypeId() == TYPEID_PLAYER ? victim->GetGUID().GetCounter() : victim->GetEntry(), spellProto ? spellProto->Id : 0, Group::Format(GetMap()->GetDifficulty()), doneMod, takenMod);
    }

    if (victim->IsAIEnabled)
        victim->GetAI()->DamageTaken(this, damage);

    if (IsAIEnabled)
        GetAI()->DamageDealt(victim, damage, damagetype);

    // Hook for OnDamage Event
    sScriptMgr->OnDamage(this, victim, damage);

    // optimization done
    if ((damagetype == SPELL_DIRECT_DAMAGE || damagetype == DOT) && spellProto)
    {
        damage = RecaculateDamage(damage);
    }



    if (victim->GetTypeId() == TYPEID_PLAYER && this != victim)
    {
        // Signal to pets that their owner was attacked
        Pet* pet = victim->ToPlayer()->GetPet();

        if (pet && pet->IsAlive())
            pet->AI()->OwnerAttackedBy(this);

        for (auto&& itr : victim->m_Controlled)
            if (itr->GetTypeId() == TYPEID_UNIT && itr->IsAIEnabled)
                itr->ToCreature()->AI()->OwnerAttackedBy(this);
    }

    if (victim->GetTypeId() == TYPEID_PLAYER && victim->ToPlayer()->GetCommandStatus(CHEAT_GOD))
        return 0;

    // Signal the pet it was attacked so the AI can respond if needed
    if (victim->GetTypeId() == TYPEID_UNIT && this != victim && victim->IsAIEnabled && victim->IsAlive())
        victim->ToCreature()->AI()->AttackedBy(this);

    if (damagetype != NODAMAGE)
    {
        // interrupting auras with AURA_INTERRUPT_FLAG_DAMAGE before checking !damage (absorbed damage breaks that type of auras)
        if (spellProto)
        {
            if (!(spellProto->AttributesEx4 & SPELL_ATTR4_DAMAGE_DOESNT_BREAK_AURAS))
                victim->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TAKE_DAMAGE, spellProto->Id);
        }
        else
            victim->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TAKE_DAMAGE, 0);

        // interrupt spells with SPELL_INTERRUPT_FLAG_ABORT_ON_DMG on absorbed damage (no dots) (interrupt absorbed pick flag on BG)
        if (!damage && damagetype != DOT && cleanDamage && cleanDamage->absorbed_damage)
            if (victim != this && victim->GetTypeId() == TYPEID_PLAYER)
                if (Spell* spell = victim->m_currentSpells[CURRENT_GENERIC_SPELL])
                    if (spell->getState() == SPELL_STATE_PREPARING)
                        if (spell->m_spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_ABORT_ON_DMG)
                            victim->InterruptNonMeleeSpells(false);

        // We're going to call functions which can modify content of the list during iteration over it's elements
        // Let's copy the list so we can prevent iterator invalidation
        AuraEffectList vCopyDamageCopy(victim->GetAuraEffectsByType(SPELL_AURA_SHARE_DAMAGE_PCT));
        // copy damage to casters of this aura
        for (AuraEffectList::iterator i = vCopyDamageCopy.begin(); i != vCopyDamageCopy.end(); ++i)
        {
            // Check if aura was removed during iteration - we don't need to work on such auras
            if (!((*i)->GetBase()->IsAppliedOnTarget(victim->GetGUID())))
                continue;
            // check damage school mask
            if (((*i)->GetMiscValue() & damageSchoolMask) == 0)
                continue;

            Unit* shareDamageTarget = (*i)->GetCaster();
            if (!shareDamageTarget)
                continue;
            SpellInfo const* spell = (*i)->GetSpellInfo();

            uint32 share = CalculatePct(damage, (*i)->GetAmount());

            // Voodoo Doll
            if (spell->Id == 116000 && victim->GetTypeId() == TYPEID_PLAYER)
            {
                Player* _plr = victim->ToPlayer();
                std::list<Unit*> groupList;

                _plr->GetPartyMembers(groupList);

                for (auto itr : groupList)
                {
                    // Voodoo visual
                    if (!itr->HasAura(122151))
                        continue;

                    if (itr->GetGUID() == _plr->GetGUID())
                        continue;

                    shareDamageTarget->DealDamageMods(itr, share, nullptr);
                    shareDamageTarget->DealDamage(itr, share, nullptr, DIRECT_DAMAGE, spell->GetSchoolMask(), spell, false);
                }
            }
            else
            {
                /// @todo check packets if damage is done by victim, or by attacker of victim
                DealDamageMods(shareDamageTarget, share, NULL);
                DealDamage(shareDamageTarget, share, NULL, NODAMAGE, spell->GetSchoolMask(), spell, false);
            }
        }
    }

    bool damageSplitted = spellProto && spellProto->HasAura(SPELL_AURA_SPLIT_DAMAGE_PCT);

    // Rage from Damage made (only from direct weapon damage)
    if (cleanDamage && cleanDamage->hitOutCome != MELEE_HIT_MISS && damagetype == DIRECT_DAMAGE && this != victim && GetPowerType() == POWER_RAGE && !damageSplitted)
    {
        float rage = GetAttackTime(cleanDamage->attackType) / 1000.0f * 1.75f;
        switch (cleanDamage->attackType)
        {
            case OFF_ATTACK:
                rage /= 2;
            case BASE_ATTACK:
                RewardRage(rage, true);
                break;
            default:
                break;
        }
    }

    if (!damage)
    {
        // Rage from absorbed damage
        if (cleanDamage && cleanDamage->absorbed_damage && victim->GetPowerType() == POWER_RAGE)
            victim->RewardRage(cleanDamage->absorbed_damage, false);

        return 0;
    }

    TC_LOG_DEBUG("entities.unit", "DealDamageStart");

    uint32 health = victim->GetHealth();
    TC_LOG_DEBUG("entities.unit", "Unit " UI64FMTD " dealt %u damage to unit " UI64FMTD, GetGUID().GetRawValue(), damage, victim->GetGUID().GetRawValue());

    // duel ends when player has 1 or less hp
    bool duel_hasEnded = false;
    bool duel_wasMounted = false;
    if (victim->GetTypeId() == TYPEID_PLAYER && victim->ToPlayer()->duel && damage >= (health - 1))
    {
        // prevent kill only if killed in duel and killed by opponent or opponent controlled creature
        if (victim->ToPlayer()->duel->opponent == this || victim->ToPlayer()->duel->opponent->GetGUID() == GetOwnerGUID())
            damage = health - 1;

        duel_hasEnded = true;
    }
    else if (victim->IsVehicle() && damage >= (health - 1) && victim->GetCharmer() && victim->GetCharmer()->GetTypeId() == TYPEID_PLAYER)
    {
        Player* victimRider = victim->GetCharmer()->ToPlayer();

        if (victimRider && victimRider->duel && victimRider->duel->isMounted)
        {
            // prevent kill only if killed in duel and killed by opponent or opponent controlled creature
            if (victimRider->duel->opponent == this || victimRider->duel->opponent->GetGUID() == GetCharmerGUID())
                damage = health - 1;

            duel_wasMounted = true;
            duel_hasEnded = true;
        }
    }

    if (this != victim)
    {
        if (Player* killer = GetAffectingPlayer())
        {
            // in bg, count dmg if victim is also a player
            if (victim->GetTypeId() == TYPEID_PLAYER)
            {
                if (Battleground* bg = killer->GetBattleground())
                {
                    bg->UpdatePlayerScore(killer, SCORE_DAMAGE_DONE, damage);
                    bg->UpdatePlayerScore(((Player*)victim), SCORE_DAMAGE_TAKEN, damage);
                }
            }

            // Not sure about this
            if (GetTypeId() == TYPEID_PLAYER)
            {
                killer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_DAMAGE_DONE, damage, 0, 0, victim);
                killer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HIT_DEALT, damage);
            }
        }
    }

    if (victim->GetTypeId() == TYPEID_PLAYER)
        victim->ToPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HIT_RECEIVED, damage);
    else if (!victim->IsControlledByPlayer() || (victim->IsVehicle() && !victim->IsPet()))
    {
        if (!victim->ToCreature()->HasLootRecipient())
            victim->ToCreature()->SetLootRecipient(this);

        if (IsControlledByPlayer())
            victim->ToCreature()->LowerPlayerDamageReq(health < damage ? health : damage);
    }

    if (GetTypeId() != TYPEID_PLAYER)
        victim->_npcDamageTaken[0] += damage;
    else
        victim->_playerDamageTaken[0] += damage;

    if (health <= damage)
    {
        TC_LOG_DEBUG("entities.unit", "DealDamage: victim just died");

        if (victim->GetTypeId() == TYPEID_PLAYER && victim != this)
            victim->ToPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_TOTAL_DAMAGE_RECEIVED, health);

        Kill(victim, durabilityLoss, spellProto);
    }
    else
    {
        TC_LOG_DEBUG("entities.unit", "DealDamageAlive");

        if (victim->GetTypeId() == TYPEID_PLAYER)
            victim->ToPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_TOTAL_DAMAGE_RECEIVED, damage);

        victim->ModifyHealth(-(int32) damage);

        if (damagetype == DIRECT_DAMAGE || damagetype == SPELL_DIRECT_DAMAGE)
            victim->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_DIRECT_DAMAGE, spellProto ? spellProto->Id : 0);

        if (victim->GetTypeId() != TYPEID_PLAYER)
            victim->AddThreat(this, float(damage), damageSchoolMask, spellProto);
        else                                                // victim is a player
        {
            // random durability for items (HIT TAKEN)
            if (roll_chance_f(sWorld->getRate(RATE_DURABILITY_LOSS_DAMAGE)))
            {
                EquipmentSlots slot = EquipmentSlots(urand(0, EQUIPMENT_SLOT_END - 1));
                victim->ToPlayer()->DurabilityPointLossForEquipSlot(slot);
            }
        }

        // Rage from damage received
        if (this != victim && victim->GetPowerType() == POWER_RAGE)
        {
            uint32 rage_damage = damage + (cleanDamage ? cleanDamage->absorbed_damage : 0);
            victim->RewardRage(rage_damage, false);
        }

        if (GetTypeId() == TYPEID_PLAYER)
        {
            // random durability for items (HIT DONE)
            if (roll_chance_f(sWorld->getRate(RATE_DURABILITY_LOSS_DAMAGE)))
            {
                EquipmentSlots slot = EquipmentSlots(urand(0, EQUIPMENT_SLOT_END - 1));
                ToPlayer()->DurabilityPointLossForEquipSlot(slot);
            }
        }

        {
            if (victim != this && victim->GetTypeId() == TYPEID_PLAYER && // does not support creature push_back
                (!spellProto || !spellProto->HasAttribute(SPELL_ATTR3_NO_PUSHBACK_ON_DAMAGE)))
            {
                // IDK how this supposed to work but there are plenty amount of AoE spells that are proofed
                bool dontDelay = GetTypeId() == TYPEID_UNIT && spellProto && spellProto->IsAoESpell();

                if (damagetype != DOT)
                    if (Spell* spell = victim->m_currentSpells [CURRENT_GENERIC_SPELL])
                        if (spell->getState() == SPELL_STATE_PREPARING)
                        {
                            uint32 interruptFlags = spell->m_spellInfo->InterruptFlags;
                            if (interruptFlags & SPELL_INTERRUPT_FLAG_ABORT_ON_DMG)
                                victim->InterruptNonMeleeSpells(false);
                            else if (interruptFlags & SPELL_INTERRUPT_FLAG_PUSH_BACK)
                            {
                                if (!dontDelay)
                                    spell->Delayed();
                            }
                        }

                if (Spell* spell = victim->m_currentSpells [CURRENT_CHANNELED_SPELL])
                    if (!dontDelay && spell->getState() == SPELL_STATE_CASTING)
                    {
                        uint32 channelInterruptFlags = spell->m_spellInfo->ChannelInterruptFlags;
                        if (((channelInterruptFlags & CHANNEL_FLAG_DELAY) != 0) && (damagetype != DOT))
                            spell->DelayedChannel();
                    }
            }
        }

        // last damage from duel opponent
        if (duel_hasEnded)
        {
            Player* he = duel_wasMounted ? victim->GetCharmer()->ToPlayer() : victim->ToPlayer();

            ASSERT(he && he->duel);

            if (duel_wasMounted) // In this case victim==mount
                victim->SetHealth(1);
            else
                he->SetHealth(1);

            he->duel->opponent->CombatStopWithPets(true);
            he->CombatStopWithPets(true);

            he->CastSpell(he, 7267, true);                  // beg
            he->DuelComplete(DUEL_WON);
        }
    }

    TC_LOG_DEBUG("entities.unit", "DealDamageEnd returned %d damage", damage);

    return damage;
}

void Unit::CastStop(uint32 except_spellid)
{
    for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; i++)
        if (m_currentSpells [i] && m_currentSpells [i]->m_spellInfo->Id != except_spellid)
            InterruptSpell(CurrentSpellTypes(i), false);
}

void Unit::CastSpell(SpellCastTargets const& targets, SpellInfo const* spellInfo, CustomSpellValues const* value, TriggerCastFlags triggerFlags, Item* castItem, AuraEffect const* triggeredByAura, ObjectGuid originalCaster)
{
    if (!spellInfo)
    {
        TC_LOG_ERROR("entities.unit", "CastSpell: unknown spell by caster: %s %u)", (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUID().GetCounter() : GetEntry()));
        return;
    }

    /// @todo this is a workaround - not needed anymore, but required for some scripts :(
    if (!originalCaster && triggeredByAura)
        originalCaster = triggeredByAura->GetCasterGUID();

    Spell* spell = new Spell(this, spellInfo, triggerFlags, originalCaster);
    if (value)
        spell->SetContext(*value);

    spell->m_CastItem = castItem;
    spell->prepare(&targets, triggeredByAura);
}

void Unit::CastSpell(Unit* victim, uint32 spellId, bool triggered, Item* castItem, AuraEffect const* triggeredByAura, ObjectGuid originalCaster)
{
    CastSpell(victim, spellId, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE, castItem, triggeredByAura, originalCaster);
}

void Unit::CastSpell(Unit* victim, uint32 spellId, TriggerCastFlags triggerFlags /*= TRIGGER_NONE*/, Item* castItem /*= NULL*/, AuraEffect const* triggeredByAura /*= NULL*/, ObjectGuid originalCaster /*= 0*/)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
    {
        //TC_LOG_ERROR("entities.unit", "CastSpell: unknown spell id %u by caster: %s %u)", spellId, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUIDLow() : GetEntry()));
        return;
    }

    CastSpell(victim, spellInfo, triggerFlags, castItem, triggeredByAura, originalCaster);
}

void Unit::CastSpell(Unit* victim, SpellInfo const* spellInfo, bool triggered, Item* castItem/*= NULL*/, AuraEffect const* triggeredByAura /*= NULL*/, ObjectGuid originalCaster /*= 0*/)
{
    CastSpell(victim, spellInfo, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE, castItem, triggeredByAura, originalCaster);
}

void Unit::CastSpell(Unit* victim, SpellInfo const* spellInfo, TriggerCastFlags triggerFlags, Item* castItem, AuraEffect const* triggeredByAura, ObjectGuid originalCaster)
{
    SpellCastTargets targets;
    targets.SetUnitTarget(victim);
    CastSpell(targets, spellInfo, NULL, triggerFlags, castItem, triggeredByAura, originalCaster);
}

void Unit::CastCustomSpell(Unit* target, uint32 spellId, int32 const* bp0, int32 const* bp1, int32 const* bp2, bool triggered, Item* castItem, AuraEffect const* triggeredByAura, ObjectGuid originalCaster)
{
    CustomSpellValues values;
    if (bp0)
        values.AddSpellMod(SPELLVALUE_BASE_POINT0, *bp0);
    if (bp1)
        values.AddSpellMod(SPELLVALUE_BASE_POINT1, *bp1);
    if (bp2)
        values.AddSpellMod(SPELLVALUE_BASE_POINT2, *bp2);
    CastCustomSpell(spellId, values, target, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE, castItem, triggeredByAura, originalCaster);
}

void Unit::CastCustomSpell(float x, float y, float z, uint32 spellId, int32 const* bp0, int32 const* bp1, int32 const* bp2, bool triggered, Item* castItem, AuraEffect const *triggeredByAura, ObjectGuid originalCaster)
{
    CustomSpellValues values;
    if (bp0)
        values.AddSpellMod(SPELLVALUE_BASE_POINT0, *bp0);
    if (bp1)
        values.AddSpellMod(SPELLVALUE_BASE_POINT1, *bp1);
    if (bp2)
        values.AddSpellMod(SPELLVALUE_BASE_POINT2, *bp2);
    CastCustomSpell(x, y, z, spellId, values, triggered, castItem, triggeredByAura, originalCaster);
}

void Unit::CastCustomSpell(uint32 spellId, SpellValueMod mod, int32 value, Unit* target, bool triggered, Item* castItem, AuraEffect const* triggeredByAura, ObjectGuid originalCaster)
{
    CustomSpellValues values;
    values.AddSpellMod(mod, value);
    CastCustomSpell(spellId, values, target, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE, castItem, triggeredByAura, originalCaster);
}

void Unit::CastCustomSpell(uint32 spellId, SpellValueMod mod, int32 value, Unit* target, TriggerCastFlags triggerFlags, Item* castItem, AuraEffect const* triggeredByAura, ObjectGuid originalCaster)
{
    CustomSpellValues values;
    values.AddSpellMod(mod, value);
    CastCustomSpell(spellId, values, target, triggerFlags, castItem, triggeredByAura, originalCaster);
}

void Unit::CastCustomSpell(uint32 spellId, CustomSpellValues const& value, Unit* victim, TriggerCastFlags triggerFlags, Item* castItem, AuraEffect const* triggeredByAura, ObjectGuid originalCaster)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
    {
        TC_LOG_ERROR("entities.unit", "CastSpell: unknown spell id %u by caster: %s %u)", spellId, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUID().GetCounter() : GetEntry()));
        return;
    }
    SpellCastTargets targets;
    targets.SetUnitTarget(victim);

    CastSpell(targets, spellInfo, &value, triggerFlags, castItem, triggeredByAura, originalCaster);
}

void Unit::CastCustomSpell(float x, float y, float z, uint32 spellId, CustomSpellValues const& value, bool triggered, Item* castItem, AuraEffect const *triggeredByAura, ObjectGuid originalCaster)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
        return;

    SpellCastTargets targets;
    targets.SetDst(x, y, z, GetOrientation());

    CastSpell(targets, spellInfo, &value, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE, castItem, triggeredByAura, originalCaster);
}

void Unit::CastSpell(float x, float y, float z, uint32 spellId, bool triggered, Item* castItem, AuraEffect const* triggeredByAura, ObjectGuid originalCaster)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
    {
        TC_LOG_ERROR("entities.unit", "CastSpell: unknown spell id %u by caster: %s %u)", spellId, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUID().GetCounter() : GetEntry()));
        return;
    }
    SpellCastTargets targets;
    targets.SetDst(x, y, z, GetOrientation());

    CastSpell(targets, spellInfo, NULL, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE, castItem, triggeredByAura, originalCaster);
}

void Unit::CastSpell(GameObject* go, uint32 spellId, bool triggered, Item* castItem, AuraEffect* triggeredByAura, ObjectGuid originalCaster)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
    {
        TC_LOG_ERROR("entities.unit", "CastSpell: unknown spell id %u by caster: %s %u)", spellId, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUID().GetCounter() : GetEntry()));
        return;
    }
    SpellCastTargets targets;
    targets.SetGOTarget(go);

    CastSpell(targets, spellInfo, NULL, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE, castItem, triggeredByAura, originalCaster);
}

void Unit::CastSpell(Position const& pos, uint32 spellId, TriggerCastFlags triggerFlags, Item* castItem, AuraEffect const* triggeredByAura, ObjectGuid originalCaster)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
    {
        TC_LOG_ERROR("entities.unit", "CastSpell: unknown spell id %u by caster: %s %u)", spellId, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUID().GetCounter() : GetEntry()));
        return;
    }
    SpellCastTargets targets;
    targets.SetDst(pos);

    CastSpell(targets, spellInfo, nullptr, triggerFlags, castItem, triggeredByAura, originalCaster);
}

// Obsolete func need remove, here only for comotability vs another patches
uint32 Unit::SpellNonMeleeDamageLog(Unit* victim, uint32 spellID, uint32 damage)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellID);
    SpellNonMeleeDamage damageInfo(this, victim, spellInfo->Id, spellInfo->SchoolMask);
    damage = SpellDamageBonusDone(victim, spellInfo, EFFECT_0, damage, SPELL_DIRECT_DAMAGE);
    damage = victim->SpellDamageBonusTaken(this, spellInfo, EFFECT_0, damage, SPELL_DIRECT_DAMAGE);

    CalculateSpellDamageTaken(&damageInfo, damage, spellInfo);
    DealDamageMods(damageInfo.target, damageInfo.damage, &damageInfo.absorb);
    SendSpellNonMeleeDamageLog(&damageInfo);
    DealSpellDamage(&damageInfo, true);
    return damageInfo.damage;
}

void Unit::CalculateSpellDamageTaken(SpellNonMeleeDamage* damageInfo, int32 damage, SpellInfo const* spellInfo, WeaponAttackType attackType, bool crit, uint32 procVictim, uint32 procEx)
{
    if (damage < 0)
        return;

    Unit* victim = damageInfo->target;
    if (!victim || !victim->IsAlive())
        return;

    SpellSchoolMask damageSchoolMask = SpellSchoolMask(damageInfo->schoolMask);

    if (IsDamageReducedByArmor(damageSchoolMask, spellInfo))
        damage = CalcArmorReducedDamage(victim, damage, spellInfo, attackType);

    bool blocked = false;
    // Per-school calc
    switch (spellInfo->DmgClass)
    {
        // Melee and Ranged Spells
        case SPELL_DAMAGE_CLASS_RANGED:
        case SPELL_DAMAGE_CLASS_MELEE:
        {
            // Physical Damage
            if (damageSchoolMask & SPELL_SCHOOL_MASK_NORMAL)
            {
                // Get blocked status
                blocked = isSpellBlocked(victim, spellInfo, attackType);
            }

            if (crit)
            {
                damageInfo->HitInfo |= SPELL_HIT_TYPE_CRIT;

                // Calculate crit bonus
                uint32 crit_bonus = damage;
                // Apply crit_damage bonus for melee spells
                if (Player* modOwner = GetSpellModOwner())
                    modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_CRIT_DAMAGE_BONUS, crit_bonus);
                damage += crit_bonus;

                // Apply SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE or SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE
                float critPctDamageMod = 0.0f;
                if (attackType == RANGED_ATTACK)
                    critPctDamageMod += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE);
                else
                    critPctDamageMod += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE);

                // Increase crit damage from SPELL_AURA_MOD_CRIT_DAMAGE_BONUS
                critPctDamageMod += (GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_CRIT_DAMAGE_BONUS, spellInfo->GetSchoolMask()) - 1.0f) * 100;

                if (critPctDamageMod != 0)
                    AddPct(damage, critPctDamageMod);
            }

            // Spell weapon based damage CAN BE crit & blocked at same time
            if (blocked)
            {
                // double blocked amount if block is critical
                uint32 value = victim->GetBlockPercent();
                if (victim->isBlockCritical())
                    value *= 2; // double blocked percent
                damageInfo->blocked = CalculatePct(damage, value);
                damage -= damageInfo->blocked;
            }

            if (!(spellInfo->AttributesCu & SPELL_ATTR0_CU_IGNORE_RESILIENCE))
                ApplyResilience(victim, &damage);
            break;
        }
        // Magical Attacks
        case SPELL_DAMAGE_CLASS_NONE:
        case SPELL_DAMAGE_CLASS_MAGIC:
        {
            // If crit add critical bonus
            if (crit)
            {
                damageInfo->HitInfo |= SPELL_HIT_TYPE_CRIT;
                damage = SpellCriticalDamageBonus(spellInfo, damage, victim);
            }

            if (!(spellInfo->AttributesCu & SPELL_ATTR0_CU_IGNORE_RESILIENCE))
                ApplyResilience(victim, &damage);
            break;
        }
        default:
            break;
    }

    // Script Hook For CalculateSpellDamageTaken -- Allow scripts to change the Damage post class mitigation calculations
    sScriptMgr->ModifySpellDamageTaken(damageInfo->target, damageInfo->attacker, damage);

    // Calculate absorb resist
    if (damage > 0)
    {
        CalcAbsorbResist(victim, damageSchoolMask, SPELL_DIRECT_DAMAGE, damage, &damageInfo->absorb, &damageInfo->resist, spellInfo, procVictim, procEx);
        damage -= damageInfo->absorb + damageInfo->resist;
    }
    else
        damage = 0;

    damageInfo->damage = damage;
}

void Unit::DealSpellDamage(SpellNonMeleeDamage* damageInfo, bool durabilityLoss)
{
    if (damageInfo == 0)
        return;

    Unit* victim = damageInfo->target;

    if (!victim)
        return;

    if (!victim->IsAlive() || victim->HasUnitState(UNIT_STATE_IN_FLIGHT) || (victim->GetTypeId() == TYPEID_UNIT && victim->ToCreature()->IsInEvadeMode()))
        return;

    SpellInfo const* spellProto = sSpellMgr->GetSpellInfo(damageInfo->SpellID);
    if (spellProto == NULL)
    {
        TC_LOG_DEBUG("entities.unit", "Unit::DealSpellDamage has wrong damageInfo->SpellID: %u", damageInfo->SpellID);
        return;
    }

    // Call default DealDamage
    CleanDamage cleanDamage(damageInfo->cleanDamage, damageInfo->absorb, BASE_ATTACK, MELEE_HIT_NORMAL);
    DealDamage(victim, damageInfo->damage, &cleanDamage, SPELL_DIRECT_DAMAGE, SpellSchoolMask(damageInfo->schoolMask), spellProto, durabilityLoss);
}

/// @todo for melee need create structure as in
void Unit::CalculateMeleeDamage(Unit* victim, uint32 damage, CalcDamageInfo* damageInfo, WeaponAttackType attackType)
{
    damageInfo->attacker = this;
    damageInfo->target = victim;
    damageInfo->damageSchoolMask = GetMeleeDamageSchoolMask();
    damageInfo->attackType = attackType;
    damageInfo->damage = 0;
    damageInfo->cleanDamage = 0;
    damageInfo->absorb = 0;
    damageInfo->resist = 0;
    damageInfo->blocked_amount = 0;

    damageInfo->TargetState = 0;
    damageInfo->HitInfo = 0;
    damageInfo->procAttacker = PROC_FLAG_NONE;
    damageInfo->procVictim = PROC_FLAG_NONE;
    damageInfo->procEx = PROC_EX_INTERNAL_ACTIVE;
    damageInfo->hitOutCome = MELEE_HIT_EVADE;

    if (!victim)
        return;

    if (!IsAlive() || !victim->IsAlive())
        return;

    // Select HitInfo/procAttacker/procVictim flag based on attack type
    switch (attackType)
    {
        case BASE_ATTACK:
            damageInfo->procAttacker = PROC_FLAG_DONE_MELEE_AUTO_ATTACK | PROC_FLAG_DONE_MAINHAND_ATTACK;
            damageInfo->procVictim = PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK;
            break;
        case OFF_ATTACK:
            damageInfo->procAttacker = PROC_FLAG_DONE_MELEE_AUTO_ATTACK | PROC_FLAG_DONE_OFFHAND_ATTACK;
            damageInfo->procVictim = PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK;
            damageInfo->HitInfo = HITINFO_OFFHAND;
            break;
        default:
            return;
    }

    // Physical Immune check
    if (damageInfo->target->IsImmunedToDamage(SpellSchoolMask(damageInfo->damageSchoolMask)))
    {
        damageInfo->HitInfo |= HITINFO_NORMALSWING;
        damageInfo->TargetState = VICTIMSTATE_IS_IMMUNE;

        damageInfo->procEx |= PROC_EX_IMMUNE;
        damageInfo->damage = 0;
        damageInfo->cleanDamage = 0;
        return;
    }

    damage += CalculateDamage(damageInfo->attackType, false, true);
    // Add melee damage bonus
    damage = MeleeDamageBonusDone(damageInfo->target, damage, damageInfo->attackType);
    damageInfo->unmitigatedDamage = damage;
    damage = damageInfo->target->MeleeDamageBonusTaken(this, damage, damageInfo->attackType);

    // Script Hook For CalculateMeleeDamage -- Allow scripts to change the Damage pre class mitigation calculations
    sScriptMgr->ModifyMeleeDamage(damageInfo->target, damageInfo->attacker, damage);

    // Calculate armor reduction
    if (IsDamageReducedByArmor((SpellSchoolMask) (damageInfo->damageSchoolMask)))
    {
        damageInfo->damage = CalcArmorReducedDamage(damageInfo->target, damage, NULL, damageInfo->attackType);
        damageInfo->cleanDamage += damage - damageInfo->damage;
    }
    else
        damageInfo->damage = damage;

    damageInfo->hitOutCome = RollMeleeOutcomeAgainst(damageInfo->target, damageInfo->attackType);

    switch (damageInfo->hitOutCome)
    {
        case MELEE_HIT_EVADE:
            damageInfo->HitInfo |= HITINFO_MISS | HITINFO_SWINGNOHITSOUND;
            damageInfo->TargetState = VICTIMSTATE_EVADES;
            damageInfo->procEx |= PROC_EX_EVADE;
            damageInfo->damage = 0;
            damageInfo->cleanDamage = 0;
            return;
        case MELEE_HIT_MISS:
            damageInfo->HitInfo |= HITINFO_MISS;
            damageInfo->TargetState = VICTIMSTATE_INTACT;
            damageInfo->procEx |= PROC_EX_MISS;
            damageInfo->damage = 0;
            damageInfo->cleanDamage = 0;
            break;
        case MELEE_HIT_NORMAL:
            damageInfo->TargetState = VICTIMSTATE_HIT;
            damageInfo->procEx |= PROC_EX_NORMAL_HIT;
            break;
        case MELEE_HIT_CRIT:
        {
            damageInfo->HitInfo |= HITINFO_CRITICALHIT;
            damageInfo->TargetState = VICTIMSTATE_HIT;

            damageInfo->procEx |= PROC_EX_CRITICAL_HIT;
            // Crit bonus calc
            damageInfo->damage += damageInfo->damage;
            float mod = 0.0f;
            // Apply SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE or SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE
            if (damageInfo->attackType == RANGED_ATTACK)
                mod += damageInfo->target->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE);
            else
                mod += damageInfo->target->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE);

            // Increase crit damage from SPELL_AURA_MOD_CRIT_DAMAGE_BONUS
            mod += (GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_CRIT_DAMAGE_BONUS, damageInfo->damageSchoolMask) - 1.0f) * 100;

            if (mod != 0)
                AddPct(damageInfo->damage, mod);
            break;
        }
        case MELEE_HIT_PARRY:
            damageInfo->TargetState = VICTIMSTATE_PARRY;
            damageInfo->procEx |= PROC_EX_PARRY;
            damageInfo->cleanDamage += damageInfo->damage;
            damageInfo->damage = 0;
            break;
        case MELEE_HIT_DODGE:
            damageInfo->TargetState = VICTIMSTATE_DODGE;
            damageInfo->procEx |= PROC_EX_DODGE;
            damageInfo->cleanDamage += damageInfo->damage;
            damageInfo->damage = 0;
            break;
        case MELEE_HIT_GLANCING:
        {
            damageInfo->HitInfo |= HITINFO_GLANCING;
            damageInfo->TargetState = VICTIMSTATE_HIT;
            damageInfo->procEx |= PROC_EX_NORMAL_HIT;
            int32 leveldif = int32(victim->GetLevelForTarget(this)) - int32(GetLevel());
            if (leveldif > 3)
                leveldif = 3;
            float reducePercent = 1 - leveldif * 0.1f;
            damageInfo->cleanDamage += damageInfo->damage - uint32(reducePercent * damageInfo->damage);
            damageInfo->damage = uint32(reducePercent * damageInfo->damage);
            break;
        }
        case MELEE_HIT_CRUSHING:
            damageInfo->HitInfo |= HITINFO_CRUSHING;
            damageInfo->TargetState = VICTIMSTATE_HIT;
            damageInfo->procEx |= PROC_EX_NORMAL_HIT;
            // 150% normal damage
            damageInfo->damage += (damageInfo->damage / 2);
            break;
        case MELEE_HIT_MISSFIRE:
            damageInfo->HitInfo |= HITINFO_MISS | HITINFO_MISSFIRE;
            damageInfo->TargetState = VICTIMSTATE_INTACT;
            damageInfo->procEx |= PROC_EX_MISS;
            damageInfo->damage = 0;
            damageInfo->cleanDamage = 0;
            if (!GetAuraEffectsByType(SPELL_AURA_MISSFIRE).empty()) // I hope there is one effect.
                victim->CastSpell(this, GetAuraEffectsByType(SPELL_AURA_MISSFIRE).front()->GetSpellEffectInfo().TriggerSpell, true);
            break;
        default:
            break;
    }

    if (damageInfo->hitOutCome == MELEE_HIT_NORMAL || damageInfo->hitOutCome == MELEE_HIT_CRIT)
    {
        if (victim->HasInArc(M_PI, this) || victim->HasAuraType(SPELL_AURA_IGNORE_HIT_DIRECTION))
        {
            if (victim->GetTypeId() == TYPEID_PLAYER || !(victim->ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_BLOCK))
            {
                float block = victim->GetBlockChangeAgainst(this, damageInfo->attackType);
                if (block && roll_chance_f(block))
                {
                    damageInfo->HitInfo |= HITINFO_BLOCK;
                    damageInfo->procEx |= PROC_EX_BLOCK;

                    // 30% damage blocked, double blocked amount if block is critical
                    damageInfo->blocked_amount = CalculatePct(damageInfo->damage, damageInfo->target->isBlockCritical() ? damageInfo->target->GetBlockPercent() * 2 : damageInfo->target->GetBlockPercent());
                    damageInfo->damage -= damageInfo->blocked_amount;
                    damageInfo->cleanDamage += damageInfo->blocked_amount;
                }
            }
        }
    }

    // Always apply HITINFO_AFFECTS_VICTIM in case its not a miss
    if (!(damageInfo->HitInfo & HITINFO_MISS))
        damageInfo->HitInfo |= HITINFO_AFFECTS_VICTIM;

    int32 resilienceReduction = damageInfo->damage;
    ApplyResilience(victim, &resilienceReduction);
    resilienceReduction = damageInfo->damage - resilienceReduction;
    damageInfo->damage -= resilienceReduction;
    damageInfo->cleanDamage += resilienceReduction;

    // Calculate absorb resist
    if (int32(damageInfo->damage) > 0)
    {
        damageInfo->procVictim |= PROC_FLAG_TAKEN_DAMAGE;
        // Calculate absorb & resists
        CalcAbsorbResist(damageInfo->target, SpellSchoolMask(damageInfo->damageSchoolMask), DIRECT_DAMAGE, damageInfo->damage, &damageInfo->absorb, &damageInfo->resist, nullptr, damageInfo->procVictim, damageInfo->procEx);

        if (damageInfo->absorb)
        {
            damageInfo->HitInfo |= (damageInfo->damage - damageInfo->absorb == 0 ? HITINFO_FULL_ABSORB : HITINFO_PARTIAL_ABSORB);
            damageInfo->procEx |= PROC_EX_ABSORB;
        }

        if (damageInfo->resist)
            damageInfo->HitInfo |= (damageInfo->damage - damageInfo->resist == 0 ? HITINFO_FULL_RESIST : HITINFO_PARTIAL_RESIST);

        damageInfo->damage -= damageInfo->absorb + damageInfo->resist;
    }
    else // Impossible get negative result but....
        damageInfo->damage = 0;
}

void Unit::DealMeleeDamage(CalcDamageInfo* damageInfo, bool durabilityLoss)
{
    Unit* victim = damageInfo->target;

    if (!victim->IsAlive() || victim->HasUnitState(UNIT_STATE_IN_FLIGHT) || (victim->GetTypeId() == TYPEID_UNIT && victim->ToCreature()->IsInEvadeMode()))
        return;

    // Hmmmm dont like this emotes client must by self do all animations
    if (damageInfo->HitInfo & HITINFO_CRITICALHIT)
        victim->HandleEmoteCommand(EMOTE_ONESHOT_WOUND_CRITICAL);
    if (damageInfo->blocked_amount && damageInfo->TargetState != VICTIMSTATE_BLOCKS)
        victim->HandleEmoteCommand(EMOTE_ONESHOT_PARRY_SHIELD);

    if (damageInfo->TargetState == VICTIMSTATE_PARRY)
    {
        // Get attack timers
        float offtime = float(victim->getAttackTimer(OFF_ATTACK));
        float basetime = float(victim->getAttackTimer(BASE_ATTACK));
        // Reduce attack time
        if (victim->HasOffhandWeapon() && offtime < basetime)
        {
            float percent20 = victim->GetAttackTime(OFF_ATTACK) * 0.20f;
            float percent60 = 3.0f * percent20;
            if (offtime > percent20 && offtime <= percent60)
                victim->setAttackTimer(OFF_ATTACK, uint32(percent20));
            else if (offtime > percent60)
            {
                offtime -= 2.0f * percent20;
                victim->setAttackTimer(OFF_ATTACK, uint32(offtime));
            }
        }
        else
        {
            float percent20 = victim->GetAttackTime(BASE_ATTACK) * 0.20f;
            float percent60 = 3.0f * percent20;
            if (basetime > percent20 && basetime <= percent60)
                victim->setAttackTimer(BASE_ATTACK, uint32(percent20));
            else if (basetime > percent60)
            {
                basetime -= 2.0f * percent20;
                victim->setAttackTimer(BASE_ATTACK, uint32(basetime));
            }
        }
    }

    // Call default DealDamage
    CleanDamage cleanDamage(damageInfo->cleanDamage, damageInfo->absorb, damageInfo->attackType, damageInfo->hitOutCome);
    DealDamage(victim, damageInfo->damage, &cleanDamage, DIRECT_DAMAGE, SpellSchoolMask(damageInfo->damageSchoolMask), NULL, durabilityLoss);

    // If this is a creature and it attacks from behind it has a probability to daze it's victim
    if ((damageInfo->hitOutCome == MELEE_HIT_CRIT || damageInfo->hitOutCome == MELEE_HIT_CRUSHING || damageInfo->hitOutCome == MELEE_HIT_NORMAL || damageInfo->hitOutCome == MELEE_HIT_GLANCING) &&
        GetTypeId() != TYPEID_PLAYER && !ToCreature()->IsControlledByPlayer() && !victim->HasInArc(M_PI, this)
        && (victim->GetTypeId() == TYPEID_PLAYER || !victim->ToCreature()->isWorldBoss()) && !victim->IsVehicle())
    {
        // -probability is between 0% and 40%
        // 20% base chance
        float Probability = 20.0f;

        // there is a newbie protection, at level 10 just 7% base chance; assuming linear function
        if (victim->GetLevel() < 30)
            Probability = 0.65f * victim->GetLevel() + 0.5f;

        uint32 VictimDefense = victim->GetMaxSkillValueForLevel(this);
        uint32 AttackerMeleeSkill = GetMaxSkillValueForLevel();

        Probability *= AttackerMeleeSkill / (float) VictimDefense*0.16;

        if (Probability < 0)
            Probability = 0;

        if (Probability > 40.0f)
            Probability = 40.0f;

        if (roll_chance_f(Probability))
            CastSpell(victim, 1604, true);
    }

    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->CastItemCombatSpell(victim, damageInfo->attackType, damageInfo->procAttacker, damageInfo->procVictim, damageInfo->procEx, nullptr);

    // Do effect if any damage done to target
    if (damageInfo->damage)
    {
        // We're going to call functions which can modify content of the list during iteration over it's elements
        // Let's copy the list so we can prevent iterator invalidation
        AuraEffectList vDamageShieldsCopy(victim->GetAuraEffectsByType(SPELL_AURA_DAMAGE_SHIELD));
        for (AuraEffectList::const_iterator dmgShieldItr = vDamageShieldsCopy.begin(); dmgShieldItr != vDamageShieldsCopy.end(); ++dmgShieldItr)
        {
            SpellInfo const* i_spellProto = (*dmgShieldItr)->GetSpellInfo();
            // Damage shield can be resisted...
            if (SpellMissInfo missInfo = victim->SpellHitResult(this, i_spellProto, MAX_EFFECT_MASK))
            {
                victim->SendSpellMiss(this, i_spellProto->Id, missInfo);
                continue;
            }

            // ...or immuned
            if (IsImmunedToDamage(i_spellProto))
            {
                victim->SendSpellDamageImmune(this, i_spellProto->Id);
                continue;
            }

            uint32 damage = (*dmgShieldItr)->GetAmount();

            if (Unit* caster = (*dmgShieldItr)->GetCaster())
            {
                uint32 effIndex = (*dmgShieldItr)->GetEffIndex();
                damage = caster->SpellDamageBonusDone(this, i_spellProto, effIndex, damage, SPELL_DIRECT_DAMAGE);
                damage = this->SpellDamageBonusTaken(caster, i_spellProto, effIndex, damage, SPELL_DIRECT_DAMAGE);
            }

            // No Unit::CalcAbsorbResist here - opcode doesn't send that data - this damage is probably not affected by that
            victim->DealDamageMods(this, damage, NULL);

            /// @todo Move this to a packet handler
            ObjectGuid guid = GetGUID();
            ObjectGuid victimGuid = victim->GetGUID();
            WorldPacket data(SMSG_SPELL_DAMAGE_SHIELD);

            data.WriteBit(guid[1]);
            data.WriteBit(victimGuid[2]);
            data.WriteBit(false);
            data.WriteBit(victimGuid[6]);
            data.WriteBit(guid[3]);
            data.WriteBit(victimGuid[4]);
            data.WriteBit(guid[2]);
            data.WriteBit(guid[5]);
            data.WriteBit(guid[6]);
            data.WriteBit(victimGuid[3]);
            data.WriteBit(guid[0]);
            data.WriteBit(victimGuid[5]);
            data.WriteBit(victimGuid[1]);
            data.WriteBit(victimGuid[0]);
            data.WriteBit(guid[7]);
            data.WriteBit(guid[4]);
            data.WriteBit(victimGuid[7]);

            data.FlushBits();

            data.WriteByteSeq(guid[2]);
            data.WriteByteSeq(victimGuid[6]);
            data.WriteByteSeq(guid[6]);
            data.WriteByteSeq(guid[4]);
            data.WriteByteSeq(victimGuid[3]);
            data.WriteByteSeq(guid[7]);
            data << int32(0);                            // FIX ME: Send resisted damage, both fully resisted and partly resisted
            data.WriteByteSeq(victimGuid[4]);
            data.WriteByteSeq(guid[1]);
            data << int32(damage);                       // Damage
            data.WriteByteSeq(victimGuid[7]);
            data << uint32(i_spellProto->Id);
            int32 overkill = int32(damage) - int32(GetHealth());
            data << int32(overkill > 0 ? overkill : -1); // Overkill
            data.WriteByteSeq(guid[5]);
            data.WriteByteSeq(victimGuid[5]);
            data.WriteByteSeq(guid[0]);
            data.WriteByteSeq(victimGuid[1]);
            data.WriteByteSeq(victimGuid[0]);
            data.WriteByteSeq(victimGuid[2]);
            data << uint32(i_spellProto->SchoolMask);
            data.WriteByteSeq(guid[3]);
            
            victim->SendMessageToSet(&data, true);

            victim->DealDamage(this, damage, 0, SPELL_DIRECT_DAMAGE, i_spellProto->GetSchoolMask(), i_spellProto, true);
        }
    }
}

void Unit::HandleEmoteCommand(uint32 anim_id)
{
    // only for oneshot emotes
    if (auto emote = sEmotesStore.LookupEntry(anim_id))
    {
        if (emote->Id == 0 || emote->EmoteType != 0)
        {
            //TC_LOG_ERROR("scripts", "HandleEmoteCommand cant`t be used for state emotes. Emote %u, %s %u)", emote->Id, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUIDLow() : GetEntry()));
            return;
        }

        WorldPacket data(SMSG_EMOTE, 4 + 8);
        data << uint32(emote->Id);
        data << uint64(GetGUID());
        SendMessageToSet(&data, true);
    }
}

void Unit::HandleEmoteStateCommand(uint32 anim_id)
{
    // only for state emotes
    if (auto emote = sEmotesStore.LookupEntry(anim_id))
    {
        if (emote->Id != 0 && emote->EmoteType == 0)
        {
            TC_LOG_ERROR("scripts", "HandleEmoteStateCommand cant`t be used for oneshot emotes. Emote %u, %s %u)", emote->Id, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUID().GetCounter() : GetEntry()));
            return;
        }

        SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, emote->Id);
    }
}

bool Unit::HandleSwitchEmoteCommand(uint32 anim_id)
{
    // select between state and oneshot
    if (auto emote = sEmotesStore.LookupEntry(anim_id))
    {
        if (emote->Id != 0 && emote->EmoteType == 0)
        {
            WorldPacket data(SMSG_EMOTE, 4 + 8);
            data << uint32(emote->Id);
            data << uint64(GetGUID());
            SendMessageToSet(&data, true);
        }
        else
            SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, emote->Id);

        return true;
    }

    return false;
}

bool Unit::IsDamageReducedByArmor(SpellSchoolMask schoolMask, SpellInfo const* spellInfo, uint8 effIndex)
{
    // only physical spells damage gets reduced by armor
    if ((schoolMask & SPELL_SCHOOL_MASK_NORMAL) == 0 || schoolMask == SPELL_SCHOOL_MASK_ALL) // "Chaos" damage
        return false;
    if (spellInfo)
    {
        // there are spells with no specific attribute but they have "ignores armor" in tooltip
        if (spellInfo->AttributesCu & SPELL_ATTR0_CU_IGNORE_ARMOR)
            return false;

        // bleeding effects are not reduced by armor
        if (effIndex != MAX_SPELL_EFFECTS)
        {
            if (spellInfo->Effects[effIndex].ApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE)
                return false;
            if (spellInfo->Effects [effIndex].Effect == SPELL_EFFECT_SCHOOL_DAMAGE)
                if (spellInfo->GetEffectMechanicMask(effIndex) & (1 << MECHANIC_BLEED))
                    return false;
        }
    }
    return true;
}

uint32 Unit::CalcArmorReducedDamage(Unit* victim, const uint32 damage, SpellInfo const* spellInfo, WeaponAttackType /*attackType*/)
{
    float armor = float(victim->GetArmor());

    // bypass enemy armor by SPELL_AURA_BYPASS_ARMOR_FOR_CASTER
    int32 armorBypassPct = 0;
    AuraEffectList const & reductionAuras = victim->GetAuraEffectsByType(SPELL_AURA_BYPASS_ARMOR_FOR_CASTER);
    for (AuraEffectList::const_iterator i = reductionAuras.begin(); i != reductionAuras.end(); ++i)
        if ((*i)->GetCasterGUID() == GetGUID())
            armorBypassPct += (*i)->GetAmount();
    armor = CalculatePct(armor, 100 - std::min(armorBypassPct, 100));

    // Ignore enemy armor by SPELL_AURA_MOD_TARGET_RESISTANCE aura
    armor += GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_TARGET_RESISTANCE, SPELL_SCHOOL_MASK_NORMAL);

    if (spellInfo)
        if (Player* modOwner = GetSpellModOwner())
            modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_IGNORE_ARMOR, armor);

    AuraEffectList const& resIgnoreAuras = GetAuraEffectsByType(SPELL_AURA_MOD_IGNORE_TARGET_RESIST);
    for (AuraEffectList::const_iterator j = resIgnoreAuras.begin(); j != resIgnoreAuras.end(); ++j)
    {
        if ((*j)->GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL)
            armor = floor(AddPct(armor, -(*j)->GetAmount()));
    }

    float bonusPct = 0;
    AuraEffectList const& armorPenAuras = GetAuraEffectsByType(SPELL_AURA_MOD_ARMOR_PENETRATION_PCT);
    for (auto&& itr : armorPenAuras)
        if (!spellInfo || itr->GetMiscValue() & spellInfo->GetSchoolMask()) // I don't sure what this is needed
            bonusPct += itr->GetAmount();

    armor -= CalculatePct(armor, bonusPct);

    if (armor < 0.0f)
        armor = 0.0f;

    float levelModifier = GetLevel();

    if (levelModifier > 85)
        levelModifier = levelModifier + (4.5 * (levelModifier - 59)) + (20 * (levelModifier - 80)) + (22 * (levelModifier - 85));
    else if (levelModifier > 80)
        levelModifier = levelModifier + (4.5 * (levelModifier - 59)) + (20 * (levelModifier - 80));
    else if (levelModifier > 59)
        levelModifier = levelModifier + (4.5 * (levelModifier - 59));

    float tmpvalue = 0.1f * armor / (8.5f * levelModifier + 40);
    tmpvalue = tmpvalue / (1.0f + tmpvalue);

    if (tmpvalue < 0.0f)
        tmpvalue = 0.0f;
    if (tmpvalue > 0.75f)
        tmpvalue = 0.75f;

    return std::max<uint32>(damage * (1.0f - tmpvalue), 1);
}

uint32 Unit::CalcSpellResistance(Unit* victim, SpellSchoolMask schoolMask, SpellInfo const* spellInfo) const
{
    // Magic damage, check for resists
    if (!(schoolMask & SPELL_SCHOOL_MASK_SPELL))
        return 0;

    // Ignore spells that can't be resisted
    if (spellInfo && spellInfo->AttributesEx4 & SPELL_ATTR4_IGNORE_RESISTANCES)
        return 0;

    uint8 const bossLevel = 83;
    uint32 const bossResistanceConstant = 510;
    uint32 resistanceConstant = 0;
    uint8 level = victim->GetLevel();

    if (level == bossLevel)
        resistanceConstant = bossResistanceConstant;
    else
        resistanceConstant = level * 5;

    int32 baseVictimResistance = victim->GetResistance(schoolMask);
    baseVictimResistance += GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_TARGET_RESISTANCE, schoolMask);

    if (Player const* player = ToPlayer())
        baseVictimResistance -= player->GetSpellPenetrationItemMod();

    // Resistance can't be lower then 0
    int32 victimResistance = std::max<int32>(baseVictimResistance, 0);

    if (victimResistance > 0)
    {
        int32 ignoredResistance = 0;

        AuraEffectList const& ResIgnoreAuras = GetAuraEffectsByType(SPELL_AURA_MOD_IGNORE_TARGET_RESIST);
        for (AuraEffectList::const_iterator itr = ResIgnoreAuras.begin(); itr != ResIgnoreAuras.end(); ++itr)
            if ((*itr)->GetMiscValue() & schoolMask)
                ignoredResistance += (*itr)->GetAmount();

        ignoredResistance = std::min<int32>(ignoredResistance, 100);
        ApplyPct(victimResistance, 100 - ignoredResistance);
    }

    if (victimResistance <= 0)
        return 0;

    float averageResist = float(victimResistance) / float(victimResistance + resistanceConstant);

    float discreteResistProbability [11];
    for (uint32 i = 0; i < 11; ++i)
    {
        discreteResistProbability [i] = 0.5f - 2.5f * fabs(0.1f * i - averageResist);
        if (discreteResistProbability [i] < 0.0f)
            discreteResistProbability [i] = 0.0f;
    }

    if (averageResist <= 0.1f)
    {
        discreteResistProbability [0] = 1.0f - 7.5f * averageResist;
        discreteResistProbability [1] = 5.0f * averageResist;
        discreteResistProbability [2] = 2.5f * averageResist;
    }

    uint32 resistance = 0;
    float r = float(rand_norm());
    float probabilitySum = discreteResistProbability [0];

    while (r >= probabilitySum && resistance < 10)
        probabilitySum += discreteResistProbability [++resistance];

    return resistance * 10;
}

void Unit::CalcAbsorbResist(Unit* victim, SpellSchoolMask schoolMask, DamageEffectType damagetype, uint32 const damage, uint32* absorb, uint32* resist, SpellInfo const* spellInfo, uint32 procVictim, uint32 procEx)
{
    if (!victim || !victim->IsAlive() || !damage)
        return;

    DamageInfo dmgInfo = DamageInfo(this, victim, damage, spellInfo, schoolMask, damagetype);

    uint32 spellResistance = CalcSpellResistance(victim, schoolMask, spellInfo);
    dmgInfo.ResistDamage(CalculatePct(damage, spellResistance));

    // Ignore Absorption Auras
    float auraAbsorbMod = 0;
    AuraEffectList const& AbsIgnoreAurasA = GetAuraEffectsByType(SPELL_AURA_MOD_TARGET_ABSORB_SCHOOL);
    for (AuraEffectList::const_iterator itr = AbsIgnoreAurasA.begin(); itr != AbsIgnoreAurasA.end(); ++itr)
    {
        if (!((*itr)->GetMiscValue() & schoolMask))
            continue;

        if ((*itr)->GetAmount() > auraAbsorbMod)
            auraAbsorbMod = float((*itr)->GetAmount());
    }

    AuraEffectList const& AbsIgnoreAurasB = GetAuraEffectsByType(SPELL_AURA_MOD_TARGET_ABILITY_ABSORB_SCHOOL);
    for (AuraEffectList::const_iterator itr = AbsIgnoreAurasB.begin(); itr != AbsIgnoreAurasB.end(); ++itr)
    {
        if (!((*itr)->GetMiscValue() & schoolMask))
            continue;

        if (((*itr)->GetAmount() > auraAbsorbMod) && (*itr)->IsAffectingSpell(spellInfo))
            auraAbsorbMod = float((*itr)->GetAmount());
    }

    RoundToInterval(auraAbsorbMod, 0.0f, 100.0f);

    // We're going to call functions which can modify content of the list during iteration over it's elements
    // Let's copy the list so we can prevent iterator invalidation
    AuraEffectList vSchoolAbsorbCopy(victim->GetAuraEffectsByType(SPELL_AURA_SCHOOL_ABSORB));
    vSchoolAbsorbCopy.sort(Trinity::AbsorbAuraOrderPred());

    // absorb without mana cost
    for (AuraEffectList::iterator itr = vSchoolAbsorbCopy.begin(); (itr != vSchoolAbsorbCopy.end()) && (dmgInfo.GetDamage() > 0); ++itr)
    {
        AuraEffect* absorbAurEff = *itr;
        // Check if aura was removed during iteration - we don't need to work on such auras
        AuraApplication const* aurApp = absorbAurEff->GetBase()->GetApplicationOfTarget(victim->GetGUID());
        if (!aurApp)
            continue;
        if (!(absorbAurEff->GetMiscValue() & schoolMask))
            continue;

        if (spellInfo && spellInfo->Id == 115080) // Touch of Death
        {
            bool ignore = true;
            switch (absorbAurEff->GetId())
            {
                case 20711:  // Spirit of Redemption
                case 31230:  // Cheat Death
                case 31850:  // Ardent Defender
                case 47788:  // Guardian Spirit
                case 86949:  // Cauterize
                case 114556: // Purgatory
                    ignore = false;
                    break;
            }
            if (ignore)
                continue;
        }

        // get amount which can be still absorbed by the aura
        int32 currentAbsorb = absorbAurEff->GetAmount();
        // aura with infinite absorb amount - let the scripts handle absorbtion amount, set here to 0 for safety
        if (currentAbsorb < 0)
            currentAbsorb = 0;

        uint32 tempAbsorb = uint32(currentAbsorb);

        bool defaultPrevented = false;

        absorbAurEff->GetBase()->CallScriptEffectAbsorbHandlers(absorbAurEff, aurApp, dmgInfo, tempAbsorb, defaultPrevented);
        currentAbsorb = tempAbsorb;

        if (defaultPrevented)
            continue;

        // Apply absorb mod auras
        AddPct(currentAbsorb, -auraAbsorbMod);

        // absorb must be smaller than the damage itself
        currentAbsorb = RoundToInterval(currentAbsorb, 0, int32(dmgInfo.GetDamage()));

        dmgInfo.AbsorbDamage(currentAbsorb);

        if (Unit* caster = aurApp->GetBase()->GetCaster())
        {
            if (Player* player = caster->GetCharmerOrOwnerPlayerOrPlayerItself())
                if (Battleground* bg = player->GetBattleground())
                    bg->UpdatePlayerScore(player, SCORE_DAMAGE_ABSORBED, currentAbsorb);
        }

        tempAbsorb = currentAbsorb;
        absorbAurEff->GetBase()->CallScriptEffectAfterAbsorbHandlers(absorbAurEff, aurApp, dmgInfo, tempAbsorb);

        // Check if our aura is using amount to count damage
        if (absorbAurEff->GetAmount() >= 0)
        {
            // Reduce shield amount
            absorbAurEff->SetAmount(absorbAurEff->GetAmount() - currentAbsorb);
            // Aura cannot absorb anything more - remove it
            if (absorbAurEff->GetAmount() <= 0)
                absorbAurEff->GetBase()->Remove(AURA_REMOVE_BY_ENEMY_SPELL);
        }
    }

    if (spellInfo && spellInfo->Id == 115080) // Touch of Death
    {
        *resist = dmgInfo.GetResist();
        *absorb = dmgInfo.GetAbsorb();
        return;
    }

    // absorb by mana cost
    AuraEffectList vManaShieldCopy(victim->GetAuraEffectsByType(SPELL_AURA_MANA_SHIELD));
    for (AuraEffectList::const_iterator itr = vManaShieldCopy.begin(); (itr != vManaShieldCopy.end()) && (dmgInfo.GetDamage() > 0); ++itr)
    {
        AuraEffect* absorbAurEff = *itr;
        // Check if aura was removed during iteration - we don't need to work on such auras
        AuraApplication const* aurApp = absorbAurEff->GetBase()->GetApplicationOfTarget(victim->GetGUID());
        if (!aurApp)
            continue;
        // check damage school mask
        if (!(absorbAurEff->GetMiscValue() & schoolMask))
            continue;

        // get amount which can be still absorbed by the aura
        int32 currentAbsorb = absorbAurEff->GetAmount();
        // aura with infinite absorb amount - let the scripts handle absorbtion amount, set here to 0 for safety
        if (currentAbsorb < 0)
            currentAbsorb = 0;

        uint32 tempAbsorb = currentAbsorb;

        bool defaultPrevented = false;

        absorbAurEff->GetBase()->CallScriptEffectManaShieldHandlers(absorbAurEff, aurApp, dmgInfo, tempAbsorb, defaultPrevented);
        currentAbsorb = tempAbsorb;

        if (defaultPrevented)
            continue;

        AddPct(currentAbsorb, -auraAbsorbMod);

        // absorb must be smaller than the damage itself
        currentAbsorb = RoundToInterval(currentAbsorb, 0, int32(dmgInfo.GetDamage()));

        int32 manaReduction = currentAbsorb;

        // lower absorb amount by talents
        if (float manaMultiplier = absorbAurEff->GetSpellInfo()->Effects [absorbAurEff->GetEffIndex()].CalcValueMultiplier(absorbAurEff->GetCaster()))
            manaReduction = int32(float(manaReduction) * manaMultiplier);

        int32 manaTaken = -victim->ModifyPower(POWER_MANA, -manaReduction);

        // take case when mana has ended up into account
        currentAbsorb = currentAbsorb ? int32(float(currentAbsorb) * (float(manaTaken) / float(manaReduction))) : 0;

        dmgInfo.AbsorbDamage(currentAbsorb);

        if (Unit* caster = aurApp->GetBase()->GetCaster())
        {
            if (Player* player = caster->GetCharmerOrOwnerPlayerOrPlayerItself())
                if (Battleground* bg = player->GetBattleground())
                    bg->UpdatePlayerScore(player, SCORE_DAMAGE_ABSORBED, currentAbsorb);
        }

        tempAbsorb = currentAbsorb;
        absorbAurEff->GetBase()->CallScriptEffectAfterManaShieldHandlers(absorbAurEff, aurApp, dmgInfo, tempAbsorb);

        // Check if our aura is using amount to count damage
        if (absorbAurEff->GetAmount() >= 0)
        {
            absorbAurEff->SetAmount(absorbAurEff->GetAmount() - currentAbsorb);
            if ((absorbAurEff->GetAmount() <= 0))
                absorbAurEff->GetBase()->Remove(AURA_REMOVE_BY_ENEMY_SPELL);
        }
    }

    // split damage auras - only when not damaging self
    if (victim != this)
    {
        // We're going to call functions which can modify content of the list during iteration over it's elements
        // Let's copy the list so we can prevent iterator invalidation
        AuraEffectList vSplitDamagePctCopy(victim->GetAuraEffectsByType(SPELL_AURA_SPLIT_DAMAGE_PCT));
        for (AuraEffectList::iterator itr = vSplitDamagePctCopy.begin(); itr != vSplitDamagePctCopy.end() && dmgInfo.GetDamage() > 0; ++itr)
        {
            // Check if aura was removed during iteration - we don't need to work on such auras
            AuraApplication const* aurApp = (*itr)->GetBase()->GetApplicationOfTarget(victim->GetGUID());
            if (!aurApp)
                continue;

            // check damage school mask
            if (!((*itr)->GetMiscValue() & schoolMask))
                continue;

            // Damage can be splitted only if aura has an alive caster
            Unit* caster = (*itr)->GetCaster();
            if (!caster || (caster == victim) || !caster->IsInWorld() || !caster->IsAlive())
                continue;

            uint32 splitDamage = CalculatePct(dmgInfo.GetDamage(), (*itr)->GetAmount());

            (*itr)->GetBase()->CallScriptEffectSplitHandlers((*itr), aurApp, dmgInfo, splitDamage);

            // absorb must be smaller than the damage itself
            splitDamage = RoundToInterval(splitDamage, uint32(0), uint32(dmgInfo.GetDamage()));

            if (caster->IsImmunedToDamage((*itr)->GetSpellInfo()))
                SendSpellDamageImmune(caster, (*itr)->GetSpellInfo()->Id);
            else
            {
                dmgInfo.AbsorbDamage(splitDamage);
                uint32 splitted = splitDamage - dmgInfo.GetAbsorbedSplit();
                uint32 splitAbsorb = dmgInfo.GetAbsorbedSplit();
                DealDamageMods(caster, splitted, &splitAbsorb);

                if (Creature* target = dmgInfo.GetVictim()->ToCreature())
                {
                    if (GetTypeId() == TypeID::TYPEID_UNIT && !IsCharmedOwnedByPlayerOrPlayer())
                    {
                        float sparringLimitPct = target->GetSparringHealthLimit();

                        if (sparringLimitPct != 0.0f)
                        {
                            if (target->GetHealthPct() <= sparringLimitPct)
                            {
                                dmgInfo.ModifyDamage(dmgInfo.GetDamage() * -1);
                            }
                        }
                    }
                }

                SendSpellNonMeleeDamageLog(caster, (*itr)->GetSpellInfo()->Id, splitted, schoolMask, splitAbsorb, 0, false, 0, false);

                CleanDamage cleanDamage = CleanDamage(splitted, 0, BASE_ATTACK, MELEE_HIT_NORMAL);
                DealDamage(caster, splitted, &cleanDamage, DIRECT_DAMAGE, schoolMask, (*itr)->GetSpellInfo(), false);
                if (procVictim && procEx)
                    victim->ProcDamageAndSpell(caster, 0, procVictim | PROC_FLAG_TAKEN_DAMAGE, procEx, cleanDamage.mitigated_damage, cleanDamage.attackType, spellInfo);
            }
        }
    }

    *resist = dmgInfo.GetResist();
    *absorb = dmgInfo.GetAbsorb();
}

void Unit::CalcHealAbsorb(Unit* victim, SpellInfo const* healSpell, uint32& healAmount, uint32& absorb)
{
    if (!healAmount)
        return;

    int32 remainingHeal = healAmount;

    // Need remove expired auras after
    bool existExpired = false;

    // absorb without mana cost
    AuraEffectList const& vHealAbsorb = victim->GetAuraEffectsByType(SPELL_AURA_SCHOOL_HEAL_ABSORB);
    for (AuraEffectList::const_iterator i = vHealAbsorb.begin(); i != vHealAbsorb.end() && remainingHeal > 0; ++i)
    {
        AuraEffect* effect = *i;
        if (!(effect->GetMiscValue() & healSpell->SchoolMask))
            continue;

        AuraApplication const* aurApp = effect->GetBase()->GetApplicationOfTarget(victim->GetGUID());
        if (!aurApp)
            continue;

        // Max Amount can be absorbed by this aura
        int32 currentAbsorb = effect->GetAmount();

        // Found empty aura (impossible but..)
        if (currentAbsorb <= 0)
        {
            existExpired = true;
            continue;
        }

        // currentAbsorb - damage can be absorbed by shield
        // If need absorb less damage
        if (remainingHeal < currentAbsorb)
            currentAbsorb = remainingHeal;

        uint32 tmpheal = uint32(remainingHeal);
        uint32 tmpabsorb = uint32(currentAbsorb);

        bool defaultPrevented = false;
        effect->GetBase()->CallScriptEffectAbsorbHealHandler(effect, aurApp, tmpheal, tmpabsorb, defaultPrevented);

        remainingHeal = int32(tmpheal);
        currentAbsorb = int32(tmpabsorb);

        if (defaultPrevented)
            continue;

        remainingHeal -= currentAbsorb;

        // Consuming Shroud, Warmaster Blackhorn, Dragon Soul
        if ((*i)->GetSpellInfo()->Id == 110214 || (*i)->GetSpellInfo()->Id == 110598)
            if (victim)
            {
                int32 bp0 = (remainingHeal > 0 ? (healAmount - remainingHeal) : healAmount);
                victim->CastCustomSpell((Unit*)NULL, 110215, &bp0, NULL, NULL, true);
            }

        // Reduce shield amount
        (*i)->SetAmount((*i)->GetAmount() - currentAbsorb);
        // Need remove it later
        if ((*i)->GetAmount() <= 0)
            existExpired = true;
    }

    // Remove all expired absorb auras
    if (existExpired)
    {
        for (AuraEffectList::const_iterator i = vHealAbsorb.begin(); i != vHealAbsorb.end();)
        {
            AuraEffect* auraEff = *i;
            ++i;
            if (auraEff->GetAmount() <= 0)
            {
                uint32 removedAuras = victim->m_removedAurasCount;
                auraEff->GetBase()->Remove(AURA_REMOVE_BY_ENEMY_SPELL);
                if (removedAuras + 1 < victim->m_removedAurasCount)
                    i = vHealAbsorb.begin();
            }
        }
    }

    absorb = remainingHeal > 0 ? (healAmount - remainingHeal) : healAmount;
    healAmount = remainingHeal;
}

void Unit::AttackerStateUpdate(Unit* victim, bool ignoreLos, WeaponAttackType attType, bool extra)
{
    if (HasUnitState(UNIT_STATE_CANNOT_AUTOATTACK) || HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED))
        return;

    if (!victim->IsAlive())
        return;

    if ((attType == BASE_ATTACK || attType == OFF_ATTACK) && (!ignoreLos && !IsWithinLOSInMap(victim)))
        return;

    CombatStart(victim);
    RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_MELEE_ATTACK);

    if (attType != BASE_ATTACK && attType != OFF_ATTACK)
        return;                                             // ignore ranged case


    // melee attack spell casted at main hand attack only - no normal melee dmg dealt
    if (attType == BASE_ATTACK && m_currentSpells [CURRENT_MELEE_SPELL] && !extra)
        m_currentSpells [CURRENT_MELEE_SPELL]->cast();
    else if (m_autoattackOverrideSpell)
    {
        if (attType == OFF_ATTACK)
            CastSpell(victim, m_autoattackOverrideOffhandSpell, true);
        else
            CastSpell(victim, m_autoattackOverrideSpell, true);
    }
    else
    {
        // attack can be redirected to another target
        victim = GetMeleeHitRedirectTarget(victim);

        CalcDamageInfo damageInfo;
        CalculateMeleeDamage(victim, 0, &damageInfo, attType);
        // Send log damage message to client
        DealDamageMods(victim, damageInfo.damage, &damageInfo.absorb);

        if (Creature* target = victim->ToCreature())
        {
            if (GetTypeId() == TypeID::TYPEID_UNIT && !IsCharmedOwnedByPlayerOrPlayer())
            {
                float sparringLimitPct = target->GetSparringHealthLimit();

                if (sparringLimitPct != 0.0f)
                {
                    if (target->GetHealthPct() <= sparringLimitPct)
                    {
                        damageInfo.HitInfo |= HITINFO_FAKE_DAMAGE;
                    }
                }
            }
        }

        SendAttackStateUpdate(&damageInfo);
        DealMeleeDamage(&damageInfo, true);

        ProcTriggerContext ctx{ &damageInfo };
        ctx.UnmitigatedDamage = damageInfo.unmitigatedDamage;
        ProcDamageAndSpell(damageInfo.target, damageInfo.procAttacker, damageInfo.procVictim, damageInfo.procEx, damageInfo.damage, damageInfo.attackType, nullptr, nullptr, ctx);

        if (GetTypeId() == TYPEID_PLAYER)
            TC_LOG_DEBUG("entities.unit", "AttackerStateUpdate: (Player) %u attacked %u (TypeId: %u) for %u dmg, absorbed %u, blocked %u, resisted %u.",
            GetGUID().GetCounter(), victim->GetGUID().GetCounter(), victim->GetTypeId(), damageInfo.damage, damageInfo.absorb, damageInfo.blocked_amount, damageInfo.resist);
        else
            TC_LOG_DEBUG("entities.unit", "AttackerStateUpdate: (NPC)    %u attacked %u (TypeId: %u) for %u dmg, absorbed %u, blocked %u, resisted %u.",
            GetGUID().GetCounter(), victim->GetGUID().GetCounter(), victim->GetTypeId(), damageInfo.damage, damageInfo.absorb, damageInfo.blocked_amount, damageInfo.resist);

        if (Player* player = ToPlayer())
            player->AddToAsisstList(victim->GetGUID());
    }
}

void Unit::HandleProcExtraAttackFor(Unit* victim)
{
    while (m_extraAttacks)
    {
        AttackerStateUpdate(victim, BASE_ATTACK, true);
        --m_extraAttacks;
    }
}

MeleeHitOutcome Unit::RollMeleeOutcomeAgainst(const Unit* victim, WeaponAttackType attType) const
{
    // Miss chance based on melee
    int32 missChance = MeleeSpellMissChance(victim, attType, 0) * 100;
    int32 dodgeChance = victim->GetDodgeChanceAgainst(this, attType) * 100;
    int32 parryChance = victim->GetParryChanceAgainst(this, attType) * 100;

    // Critical hit chance
    int32 critChance = GetUnitCriticalChance(attType, victim) * 100;

    if (victim->GetTypeId() == TYPEID_UNIT && victim->ToCreature()->IsInEvadeMode())
        return MELEE_HIT_EVADE;

    int32 attackerMaxSkillValueForLevel = GetMaxSkillValueForLevel(victim);
    int32 victimMaxSkillValueForLevel = victim->GetMaxSkillValueForLevel(this);

    // bonus from skills is 0.04%
    int32    sum = 0, tmp = 0;
    int32    roll = urand(0, 10000);

    TC_LOG_DEBUG("entities.unit", "RollMeleeOutcomeAgainst: rolled %d, miss %d, dodge %d, parry %d, crit %d",
                 roll, missChance, dodgeChance, parryChance, critChance);

    for (auto&& it : GetAuraEffectsByType(SPELL_AURA_MISSFIRE))
        if (roll < it->GetAmount() * 100)
            return MELEE_HIT_MISSFIRE;

    tmp = missChance;

    if (tmp > 0 && roll < (sum += tmp))
    {
        TC_LOG_DEBUG("entities.unit", "RollMeleeOutcomeAgainst: MISS");
        return MELEE_HIT_MISS;
    }

    // always crit against a sitting target (except 0 crit chance)
    if (victim->GetTypeId() == TYPEID_PLAYER && critChance > 0 && !victim->IsStandState())
    {
        TC_LOG_DEBUG("entities.unit", "RollMeleeOutcomeAgainst: CRIT (sitting victim)");
        return MELEE_HIT_CRIT;
    }

    // Dodge chance

    // only players can't dodge if attacker is behind
    if (victim->GetTypeId() == TYPEID_PLAYER && !victim->HasInArc(M_PI, this) && !victim->HasAuraType(SPELL_AURA_IGNORE_HIT_DIRECTION))
    {
        TC_LOG_DEBUG("entities.unit", "RollMeleeOutcomeAgainst: attack came from behind and victim was a player.");
    }
    else
    {
        tmp = dodgeChance;
        if ((tmp > 0)                                        // check if unit _can_ dodge
            && roll < (sum += tmp))
        {
            TC_LOG_DEBUG("entities.unit", "RollMeleeOutcomeAgainst: DODGE <%d, %d)", sum - tmp, sum);
            return MELEE_HIT_DODGE;
        }
    }

    // parry & block chances

    // check if attack comes from behind, nobody can parry or block if attacker is behind
    if (!victim->HasInArc(M_PI, this) && !victim->HasAuraType(SPELL_AURA_IGNORE_HIT_DIRECTION))
        TC_LOG_DEBUG("entities.unit", "RollMeleeOutcomeAgainst: attack came from behind.");
    else
    {
        if (victim->GetTypeId() == TYPEID_PLAYER || !(victim->ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_PARRY))
        {
            tmp = parryChance;
            if (tmp > 0                                         // check if unit _can_ parry
                && roll < (sum += tmp))
            {
                TC_LOG_DEBUG("entities.unit", "RollMeleeOutcomeAgainst: PARRY <%d, %d)", sum - tmp, sum);
                return MELEE_HIT_PARRY;
            }
        }
    }

    // Max 40% chance to score a glancing blow against mobs that are higher level (can do only players and pets and not with ranged weapon)
    if (attType != RANGED_ATTACK &&
        GetAffectingPlayer() &&
        !victim->GetAffectingPlayer() &&
        GetLevel() < victim->GetLevelForTarget(this))
    {
        // cap possible value (with bonuses > max skill)
        int32 skill = attackerMaxSkillValueForLevel;

        // Current formula is unknown. Since patch 3.0 players consistently report of 24% chance. So just decrease constant to 9
        tmp = (9 + (victimMaxSkillValueForLevel - skill)) * 100;
        tmp = tmp > 4000 ? 4000 : tmp;
        if (roll < (sum += tmp))
        {
            TC_LOG_DEBUG("entities.unit", "RollMeleeOutcomeAgainst: GLANCING <%d, %d)", sum - 4000, sum);
            return MELEE_HIT_GLANCING;
        }
    }

    // Critical chance
    tmp = critChance;

    if (tmp > 0 && roll < (sum += tmp))
    {
        TC_LOG_DEBUG("entities.unit", "RollMeleeOutcomeAgainst: CRIT <%d, %d)", sum - tmp, sum);
        if (GetTypeId() == TYPEID_UNIT && (ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_CRIT))
            TC_LOG_DEBUG("entities.unit", "RollMeleeOutcomeAgainst: CRIT DISABLED)");
        else
            return MELEE_HIT_CRIT;
    }

    // mobs can score crushing blows if they're 4 or more levels above victim
    if (GetLevelForTarget(victim) >= victim->GetLevelForTarget(this) + 4 &&
        // can be from by creature (if can) or from controlled player that considered as creature
        !IsControlledByPlayer() &&
        !(GetTypeId() == TYPEID_UNIT && ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_CRUSH))
    {
        // when their weapon skill is 15 or more above victim's defense skill
        tmp = victimMaxSkillValueForLevel;
        // tmp = mob's level * 5 - player's current defense skill
        tmp = attackerMaxSkillValueForLevel - tmp;
        if (tmp >= 15)
        {
            // add 2% chance per lacking skill point, min. is 15%
            tmp = tmp * 200 - 1500;
            if (roll < (sum += tmp))
            {
                TC_LOG_DEBUG("entities.unit", "RollMeleeOutcomeAgainst: CRUSHING <%d, %d)", sum - tmp, sum);
                return MELEE_HIT_CRUSHING;
            }
        }
    }

    TC_LOG_DEBUG("entities.unit", "RollMeleeOutcomeAgainst: NORMAL");
    return MELEE_HIT_NORMAL;
}

uint32 Unit::CalculateDamage(WeaponAttackType attType, bool normalized, bool addTotalPct)
{
    float min_damage, max_damage;

    if (GetTypeId() == TYPEID_PLAYER && (normalized || !addTotalPct))
        ToPlayer()->CalculateMinMaxDamage(attType, normalized, addTotalPct, min_damage, max_damage);
    else
    {
        switch (attType)
        {
            case RANGED_ATTACK:
                min_damage = GetFloatValue(UNIT_FIELD_MIN_RANGED_DAMAGE);
                max_damage = GetFloatValue(UNIT_FIELD_MAX_RANGED_DAMAGE);
                break;
            case BASE_ATTACK:
                min_damage = GetFloatValue(UNIT_FIELD_MIN_DAMAGE);
                max_damage = GetFloatValue(UNIT_FIELD_MAX_DAMAGE);
                break;
            case OFF_ATTACK:
                min_damage = GetFloatValue(UNIT_FIELD_MIN_OFF_HAND_DAMAGE);
                max_damage = GetFloatValue(UNIT_FIELD_MAX_OFF_HAND_DAMAGE);
                break;
                // Just for good manner
            default:
                min_damage = 0.0f;
                max_damage = 0.0f;
                break;
        }
    }

    if (min_damage <= 0.0f)
        min_damage = 1.0f;

    if (max_damage <= 0.0f)
        max_damage = 1.0f;

    if (min_damage > max_damage)
        std::swap(min_damage, max_damage);

    if (max_damage == 0.0f)
        max_damage = 5.0f;

    return urand((uint32) min_damage, (uint32) max_damage);
}

float Unit::CalculateLevelPenalty(SpellInfo const* spellProto) const
{
    if (spellProto->SpellLevel <= 0 || spellProto->SpellLevel >= spellProto->MaxLevel)
        return 1.0f;

    float LvlPenalty = 0.0f;

    if (spellProto->SpellLevel < 20)
        LvlPenalty = 20.0f - spellProto->SpellLevel * 3.75f;
    float LvlFactor = (float(spellProto->SpellLevel) + 6.0f) / float(GetLevel());
    if (LvlFactor > 1.0f)
        LvlFactor = 1.0f;

    return AddPct(LvlFactor, -LvlPenalty);
}

void Unit::SendMeleeAttackStart(Unit* victim)
{
    WorldPacket data(SMSG_ATTACKSTART, 8 + 8);

    ObjectGuid attackerGuid = victim->GetGUID();
    ObjectGuid victimGuid = GetGUID();

    data.WriteBit(victimGuid [7]);
    data.WriteBit(attackerGuid [7]);
    data.WriteBit(attackerGuid [3]);
    data.WriteBit(victimGuid [3]);
    data.WriteBit(victimGuid [5]);
    data.WriteBit(attackerGuid [4]);
    data.WriteBit(attackerGuid [1]);
    data.WriteBit(victimGuid [4]);
    data.WriteBit(attackerGuid [0]);
    data.WriteBit(victimGuid [6]);
    data.WriteBit(attackerGuid [5]);
    data.WriteBit(victimGuid [2]);
    data.WriteBit(attackerGuid [6]);
    data.WriteBit(victimGuid [1]);
    data.WriteBit(attackerGuid [2]);
    data.WriteBit(victimGuid [0]);

    data.WriteByteSeq(attackerGuid [5]);
    data.WriteByteSeq(attackerGuid [0]);
    data.WriteByteSeq(victimGuid [5]);
    data.WriteByteSeq(attackerGuid [4]);
    data.WriteByteSeq(attackerGuid [6]);
    data.WriteByteSeq(victimGuid [6]);
    data.WriteByteSeq(victimGuid [1]);
    data.WriteByteSeq(victimGuid [0]);
    data.WriteByteSeq(attackerGuid [7]);
    data.WriteByteSeq(victimGuid [4]);
    data.WriteByteSeq(attackerGuid [2]);
    data.WriteByteSeq(victimGuid [3]);
    data.WriteByteSeq(victimGuid [7]);
    data.WriteByteSeq(victimGuid [2]);
    data.WriteByteSeq(attackerGuid [3]);
    data.WriteByteSeq(attackerGuid [1]);

    SendMessageToSet(&data, true);
}

void Unit::SendMeleeAttackStop(Unit* victim)
{
    WorldPacket data(SMSG_ATTACKSTOP, 8 + 8);

    ObjectGuid attackerGuid = GetGUID();
    ObjectGuid victimGuid = victim ? victim->GetGUID() : ObjectGuid::Empty;

    data.WriteBit(victimGuid[5]);
    data.WriteBit(victimGuid[6]);
    data.WriteBit(attackerGuid[3]);
    data.WriteBit(attackerGuid[6]);
    data.WriteBit(attackerGuid[7]);
    data.WriteBit(attackerGuid[2]);
    data.WriteBit(attackerGuid[5]);
    data.WriteBit(victimGuid[4]);
    data.WriteBit(victim ? victim->isDead() : 0);
    data.WriteBit(victimGuid[3]);
    data.WriteBit(victimGuid[0]);
    data.WriteBit(victimGuid[2]);
    data.WriteBit(victimGuid[7]);
    data.WriteBit(attackerGuid[4]);
    data.WriteBit(attackerGuid[1]);
    data.WriteBit(attackerGuid[0]);
    data.WriteBit(victimGuid[1]);

    data.FlushBits();

    data.WriteByteSeq(victimGuid[0]);
    data.WriteByteSeq(victimGuid[3]);
    data.WriteByteSeq(victimGuid[5]);
    data.WriteByteSeq(victimGuid[2]);
    data.WriteByteSeq(attackerGuid[0]);
    data.WriteByteSeq(attackerGuid[6]);
    data.WriteByteSeq(attackerGuid[3]);
    data.WriteByteSeq(victimGuid[4]);
    data.WriteByteSeq(attackerGuid[1]);
    data.WriteByteSeq(attackerGuid[4]);
    data.WriteByteSeq(victimGuid[6]);
    data.WriteByteSeq(attackerGuid[5]);
    data.WriteByteSeq(attackerGuid[7]);
    data.WriteByteSeq(attackerGuid[2]);
    data.WriteByteSeq(victimGuid[1]);
    data.WriteByteSeq(victimGuid[7]);

    SendMessageToSet(&data, true);
}

bool Unit::isSpellBlocked(Unit* victim, SpellInfo const* spellProto, WeaponAttackType /*attackType*/)
{
    // These spells can't be blocked
    if (spellProto && spellProto->Attributes & SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK)
        return false;

    if (victim->HasAuraType(SPELL_AURA_IGNORE_HIT_DIRECTION) || victim->HasInArc(M_PI, this))
    {
        // Check creatures flags_extra for disable block
        if (victim->GetTypeId() == TYPEID_UNIT &&
            victim->ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_BLOCK)
            return false;

        if (roll_chance_f(victim->GetUnitBlockChance()))
            return true;
    }
    return false;
}

bool Unit::isBlockCritical()
{
    if (roll_chance_i(GetTotalAuraModifier(SPELL_AURA_MOD_BLOCK_CRIT_CHANCE)))
    {
        // Very hacky.
        if (HasAura(76857))
            CastSpell(this, 12880, true);
        return true;
    }
    return false;
}

int32 Unit::GetMechanicResistChance(SpellInfo const* spellInfo) const
{
    if (!spellInfo)
        return 0;

    int32 resistMech = 0;
    for (uint8 eff = 0; eff < MAX_SPELL_EFFECTS; ++eff)
    {
        if (!spellInfo->Effects [eff].IsEffect())
            break;

        int32 effectMech = spellInfo->GetEffectMechanic(eff);
        if (effectMech)
        {
            int32 temp = GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_MECHANIC_RESISTANCE, effectMech);
            if (resistMech < temp)
                resistMech = temp;
        }
    }
    return resistMech;
}

bool Unit::CanUseAttackType(uint8 attacktype) const
{
    switch (attacktype)
    {
        case BASE_ATTACK:
            return !HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED);
        case OFF_ATTACK:
            return !HasFlag(UNIT_FIELD_FLAGS2, UNIT_FLAG2_DISARM_OFFHAND);
        case RANGED_ATTACK:
            return !HasFlag(UNIT_FIELD_FLAGS2, UNIT_FLAG2_DISARM_RANGED);
        default:
            return true;
    }
}

uint32 Unit::GetBlockPercent() const
{
    float val = 30.0f;
    val += GetTotalAuraModifier(SPELL_AURA_MOD_SHIELD_BLOCKVALUE_PCT);
    RoundToInterval(val, 0.0f, 100.0f);
    return val;
}

// Melee based spells hit result calculations
SpellMissInfo Unit::MeleeSpellHitResult(Unit* victim, SpellInfo const* spellInfo)
{
    WeaponAttackType attType = BASE_ATTACK;

    // Check damage class instead of attack type to correctly handle judgements
    // - they are meele, but can't be dodged/parried/deflected because of ranged dmg class
    if (spellInfo->DmgClass == SPELL_DAMAGE_CLASS_RANGED)
        attType = RANGED_ATTACK;

    if (GetAffectingPlayer())
    {
        int32 missChanceMod = 0;
        if (attType == RANGED_ATTACK)
            missChanceMod = victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_RANGED_HIT_CHANCE);
        else
            missChanceMod = victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE);
        if (missChanceMod <= -100)
            return SPELL_MISS_MISS;

        if (spellInfo->HasAttribute(SPELL_ATTR3_IGNORE_HIT_RESULT))
            return SPELL_MISS_NONE;
    }

    uint32 roll = urand(0, 10000);

    uint32 missChance = uint32(MeleeSpellMissChance(victim, attType, spellInfo->Id) * 100.0f);

    // Roll miss
    uint32 tmp = missChance;
    if (roll < tmp)
        return SPELL_MISS_MISS;

    // Chance resist mechanic (select max value from every mechanic spell effect)
    int32 resist_mech = 0;
    // Get effects mechanic and chance
    for (uint8 eff = 0; eff < MAX_SPELL_EFFECTS; ++eff)
    {
        int32 effect_mech = spellInfo->GetEffectMechanic(eff);
        if (effect_mech)
        {
            int32 temp = victim->GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_MECHANIC_RESISTANCE, effect_mech);
            if (resist_mech < temp * 100)
                resist_mech = temp * 100;
        }
    }
    // Roll chance
    tmp += resist_mech;
    if (roll < tmp)
        return SPELL_MISS_RESIST;

    bool canDodge = true;
    bool canParry = true;
    bool canBlock = spellInfo->AttributesEx3 & SPELL_ATTR3_BLOCKABLE_SPELL;

    // Same spells cannot be parry/dodge
    if (spellInfo->Attributes & SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK)
        return SPELL_MISS_NONE;

    if (spellInfo->SpellFamilyName == SPELLFAMILY_DEATHKNIGHT && spellInfo->SpellFamilyFlags[0] & 0x10) // Death Strike
        canParry = false;

    // Chance resist mechanic
    int32 resist_chance = victim->GetMechanicResistChance(spellInfo) * 100;
    tmp += resist_chance;
    if (roll < tmp)
        return SPELL_MISS_RESIST;

    // Ranged attacks can only miss, resist and deflect
    if (attType == RANGED_ATTACK)
    {
        canParry = false;

        // only if in front
        if (victim->HasInArc(M_PI, this) || victim->HasAuraType(SPELL_AURA_IGNORE_HIT_DIRECTION))
        {
            int32 deflect_chance = victim->GetTotalAuraModifier(SPELL_AURA_DEFLECT_SPELLS) * 100;
            tmp += deflect_chance;
            if (roll < tmp)
                return SPELL_MISS_DEFLECT;
        }
    }
    // Check for attack from behind
    if (!victim->HasInArc(M_PI, this))
    {
        if (!victim->HasAuraType(SPELL_AURA_IGNORE_HIT_DIRECTION))
        {
            // Can`t dodge from behind in PvP (but its possible in PvE)
            if (victim->GetTypeId() == TYPEID_PLAYER)
                canDodge = false;
            // Can`t parry or block
            canParry = false;
            canBlock = false;
        }
        else // Only deterrence as of 3.3.5
        {
            if (spellInfo->AttributesCu & SPELL_ATTR0_CU_REQ_CASTER_BEHIND_TARGET)
                canParry = false;
        }
    }
    // Check creatures flags_extra for disable parry
    if (victim->GetTypeId() == TYPEID_UNIT)
    {
        uint32 flagEx = victim->ToCreature()->GetCreatureTemplate()->flags_extra;
        if (flagEx & CREATURE_FLAG_EXTRA_NO_PARRY)
            canParry = false;
        // Check creatures flags_extra for disable block
        if (flagEx & CREATURE_FLAG_EXTRA_NO_BLOCK)
            canBlock = false;
    }
    // Ignore combat result aura
    AuraEffectList const& ignore = GetAuraEffectsByType(SPELL_AURA_IGNORE_COMBAT_RESULT);
    for (AuraEffectList::const_iterator i = ignore.begin(); i != ignore.end(); ++i)
    {
        if (!(*i)->IsAffectingSpell(spellInfo))
            continue;
        switch ((*i)->GetMiscValue())
        {
            case MELEE_HIT_DODGE:
                canDodge = false;
                break;
            case MELEE_HIT_BLOCK:
                canBlock = false;
                break;
            case MELEE_HIT_PARRY:
                canParry = false;
                break;
            default:
                TC_LOG_DEBUG("entities.unit", "Spell %u SPELL_AURA_IGNORE_COMBAT_RESULT has unhandled state %d", (*i)->GetId(), (*i)->GetMiscValue());
                break;
        }
    }

    if (attType == RANGED_ATTACK)
    {
        canBlock = false;
        canParry = false;
    }

    if (canDodge)
    {
        // Roll dodge
        int32 dodgeChance = victim->GetDodgeChanceAgainst(this, attType) * 100.0f;
        tmp += dodgeChance;
        if (roll < tmp)
            return SPELL_MISS_DODGE;
    }

    if (canParry)
    {
        // Roll parry
        int32 parryChance = victim->GetParryChanceAgainst(this, attType) * 100.0f;
        tmp += parryChance;
        if (roll < tmp)
            return SPELL_MISS_PARRY;
    }

    if (canBlock)
    {
        int32 blockChance = victim->GetBlockChangeAgainst(this, attType) * 100.0f;
        tmp += blockChance;
        if (roll < tmp)
            return SPELL_MISS_BLOCK;
    }

    return SPELL_MISS_NONE;
}

/// @todo need use unit spell resistances in calculations
SpellMissInfo Unit::MagicSpellHitResult(Unit* victim, SpellInfo const* spellInfo)
{
    // Can`t miss on dead target (on skinning for example)
    if ((!victim->IsAlive() && victim->GetTypeId() != TYPEID_PLAYER))
        return SPELL_MISS_NONE;

    if (GetAffectingPlayer())
    {
        if (victim->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE, spellInfo->GetSchoolMask()) <= -100)
            return SPELL_MISS_MISS;

        if (spellInfo->HasAttribute(SPELL_ATTR3_IGNORE_HIT_RESULT))
        {
            if (victim->GetTotalAuraModifier(SPELL_AURA_DEFLECT_SPELLS) >= 100)
                return SPELL_MISS_DEFLECT;
            return SPELL_MISS_NONE;
        }
    }

    SpellSchoolMask schoolMask = spellInfo->GetSchoolMask();
    float missChance = 6.0f + 3.0f * GetLevelDifferenceForPenalty(this, victim);
    float hitChance = 100.0f - missChance;

    // Spellmod from SPELLMOD_RESIST_MISS_CHANCE
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_RESIST_MISS_CHANCE, hitChance);

    // Chance hit from victim SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE auras
    hitChance += victim->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE, schoolMask);

    Player* statOwner = nullptr;
    if (HasUnitTypeMask(UNIT_MASK_MINION))
    {
        Minion const* minion = static_cast<Minion const*>(this);
        if (minion->GetOwner())
            statOwner = minion->GetOwner()->ToPlayer();
    }
    float hitChanceMod = statOwner ? statOwner->m_modSpellHitChance : m_modSpellHitChance;

    hitChance *= 100.f;
    // Increase hit chance from attacker SPELL_AURA_MOD_SPELL_HIT_CHANCE and attacker ratings
    hitChance += hitChanceMod * 100.0f;

    RoundToInterval(hitChance, 0.0f, 10000.0f);

    int32 tmp = 10000 - hitChance;

    int32 rand = irand(0, 10000);

    if (rand < tmp)
        return SPELL_MISS_MISS;

    // Chance resist mechanic (select max value from every mechanic spell effect)
    int32 resist_chance = victim->GetMechanicResistChance(spellInfo) * 100;
    tmp += resist_chance;

    // Roll chance
    if (rand < tmp)
        return SPELL_MISS_RESIST;

    bool front = victim->HasInArc(M_PI, this);

    // cast by caster in front of victim
    if (front || victim->HasAuraType(SPELL_AURA_IGNORE_HIT_DIRECTION))
    {
        int32 deflect_chance = victim->GetTotalAuraModifier(SPELL_AURA_DEFLECT_SPELLS) * 100;
        tmp += deflect_chance;
        if (front)
            tmp += victim->GetTotalAuraModifier(SPELL_AURA_DEFLECT_SPELLS_FRONT) * 100;
        if (rand < tmp)
            return SPELL_MISS_DEFLECT;
    }

    return SPELL_MISS_NONE;
}

// Calculate spell hit result can be:
// Every spell can: Evade/Immune/Reflect/Sucesful hit
// For melee based spells:
//   Miss
//   Dodge
//   Parry
// For spells
//   Resist
SpellMissInfo Unit::SpellHitResult(Unit* victim, SpellInfo const* spellInfo, uint32 effectMask, bool CanReflect, Spell const* spellInstance)
{
    if (Creature* creatureVictim = victim->ToCreature())
        if (CreatureAI* ai = creatureVictim->AI())
            if (SpellMissInfo info = ai->SpellHitResult(this, spellInfo, spellInstance))
                return info;

    bool positive = !(effectMask & spellInfo->NegativeEffectMask);

    // Check for immune
    if (victim->IsImmunedToSpell(spellInfo, effectMask) && !(spellInfo->Id == 32592 && HasAura(55691))) // Glyph of Mass Dispel
        return SPELL_MISS_IMMUNE;

    // All positive spells can`t miss
    /// @todo client not show miss log for this spells - so need find info for this in dbc and use it!
    if (positive && !IsHostileTo(victim)) // prevent from affecting enemy by "positive" spell
        return SPELL_MISS_NONE;

    // Check for immune
    if (victim->IsImmunedToDamage(spellInfo) && !(spellInfo->Id == 32592 && HasAura(55691))) // Glyph of Mass Dispel
        return SPELL_MISS_IMMUNE;

    if (this == victim)
        return SPELL_MISS_NONE;

    // Return evade for units in evade mode
    if (victim->GetTypeId() == TYPEID_UNIT && victim->ToCreature()->IsInEvadeMode())
        return SPELL_MISS_EVADE;

    // Try victim reflect spell
    if (CanReflect)
    {
        int32 reflectchance = victim->GetTotalAuraModifier(SPELL_AURA_REFLECT_SPELLS);
        Unit::AuraEffectList const& mReflectSpellsSchool = victim->GetAuraEffectsByType(SPELL_AURA_REFLECT_SPELLS_SCHOOL);
        for (Unit::AuraEffectList::const_iterator i = mReflectSpellsSchool.begin(); i != mReflectSpellsSchool.end(); ++i)
            if ((*i)->GetMiscValue() & spellInfo->GetSchoolMask())
                reflectchance += (*i)->GetAmount();
        if (reflectchance > 0 && roll_chance_i(reflectchance))
            return SPELL_MISS_REFLECT;
    }

    switch (spellInfo->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_RANGED:
        case SPELL_DAMAGE_CLASS_MELEE:
            return MeleeSpellHitResult(victim, spellInfo);
        case SPELL_DAMAGE_CLASS_NONE:
            return SPELL_MISS_NONE;
        case SPELL_DAMAGE_CLASS_MAGIC:
            return MagicSpellHitResult(victim, spellInfo);
    }
    return SPELL_MISS_NONE;
}

uint32 Unit::GetUnitMeleeSkill(Unit const* target) const
{
    return (target ? GetLevelForTarget(target) : GetLevel()) * 5;
}

float Unit::GetUnitDodgeChance() const
{
    // Probably SPELL_ATTR10_UNK17 but I'm not sure at all
    if (IsNonMeleeSpellCasted(false))
        if (!FindCurrentSpellBySpellId(101546) && !FindCurrentSpellBySpellId(123986)) // Spinning Crane Kick, Chi Burst
            return 0.0f;

    if (HasUnitState(UNIT_STATE_CONTROLLED))
        return 0.0f;

    if (GetTypeId() == TYPEID_PLAYER)
        return GetFloatValue(PLAYER_FIELD_DODGE_PERCENTAGE);
    else
    {
        if (ToCreature()->IsTotem())
            return 0.0f;
        else
        {
            float dodge = 3.0f;
            dodge += GetTotalAuraModifier(SPELL_AURA_MOD_DODGE_PERCENT);
            return dodge > 0.0f ? dodge : 0.0f;
        }
    }
}

float Unit::GetUnitParryChance() const
{
    if (IsNonMeleeSpellCasted(false))
        if (!FindCurrentSpellBySpellId(101546) && !FindCurrentSpellBySpellId(123986)) // Spinning Crane Kick, Chi Burst
            return 0.0f;

    if (HasUnitState(UNIT_STATE_CONTROLLED))
        return 0.0f;

    float chance = 0.0f;

    if (Player const* player = ToPlayer())
    {
        if (player->CanParry())
        {
            Item* tmpitem = player->GetWeaponForAttack(BASE_ATTACK, true);
            if (!tmpitem)
                tmpitem = player->GetWeaponForAttack(OFF_ATTACK, true);

            if (tmpitem)
                chance = GetFloatValue(PLAYER_FIELD_PARRY_PERCENTAGE);
        }
    }
    else if (GetTypeId() == TYPEID_UNIT)
    {
        if (GetCreatureType() == CREATURE_TYPE_HUMANOID || ToCreature()->isWorldBoss())
        {
            chance = 3.0f;
            chance += GetTotalAuraModifier(SPELL_AURA_MOD_PARRY_PERCENT);
        }
    }

    return chance > 0.0f ? chance : 0.0f;
}

float Unit::GetUnitMissChance(WeaponAttackType attType) const
{
    float miss_chance = 3.00f;

    if (attType == RANGED_ATTACK)
        miss_chance -= GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_RANGED_HIT_CHANCE);
    else
        miss_chance -= GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE);

    return miss_chance;
}

float Unit::GetUnitBlockChance() const
{
    if (HasAuraType(SPELL_AURA_FORCED_DEFLECT) && !HasUnitState(UNIT_STATE_CONTROLLED))
        if (AuraEffect const* eff = GetAuraEffectsByType(SPELL_AURA_FORCED_DEFLECT).front())
            if (eff->GetBase()->HasEffectType(SPELL_AURA_MOD_BLOCK_PERCENT))
                return GetTotalAuraModifier(SPELL_AURA_MOD_BLOCK_PERCENT);

    if (IsNonMeleeSpellCasted(false) || HasUnitState(UNIT_STATE_CONTROLLED))
        return 0.0f;

    if (Player const* player = ToPlayer())
    {
        if (player->CanBlock())
        {
            Item* tmpitem = player->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (tmpitem && !tmpitem->IsBroken())
                return GetFloatValue(PLAYER_FIELD_BLOCK_PERCENTAGE);
        }
        // is player but has no block ability or no not broken shield equipped
        return 0.0f;
    }
    else
    {
        if (ToCreature()->IsTotem())
            return 0.0f;
        else
        {
            float block = 3.0f;
            block += GetTotalAuraModifier(SPELL_AURA_MOD_BLOCK_PERCENT);
            return block > 0.0f ? block : 0.0f;
        }
    }
}

static float GetExpertiseOrHitChanceFormOwner(Player* owner)
{
    float chance = 0.0f;
    switch (owner->GetClass())
    {
        case CLASS_MAGE:
        case CLASS_WARLOCK:
            chance += owner->GetExpertiseDodgeOrParryReduction(BASE_ATTACK) / 2.f;
            chance += owner->m_modSpellHitChance / 2.f;
            break;
        case CLASS_HUNTER:
            chance += owner->GetExpertiseDodgeOrParryReduction(RANGED_ATTACK) / 2.f;
            chance += owner->m_modRangedHitChance / 2.f;
            break;
        default:
            chance += owner->GetExpertiseDodgeOrParryReduction(BASE_ATTACK) / 2.f;
            chance += owner->m_modMeleeHitChance / 2.f;
            break;
    }
    return chance;
}

float Unit::GetDodgeChanceAgainst(Unit const* attacker, WeaponAttackType attType) const
{
    float dodgeChance = GetUnitDodgeChance();
    if (!dodgeChance)
        return 0.0f;

    dodgeChance += 1.5 * GetLevelDifferenceForPenalty(attacker, this);

    // Modify dodge chance by attacker SPELL_AURA_MOD_COMBAT_RESULT_CHANCE
    dodgeChance += attacker->GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_COMBAT_RESULT_CHANCE, VICTIMSTATE_DODGE);
    dodgeChance *= attacker->GetTotalAuraMultiplier(SPELL_AURA_MOD_ENEMY_DODGE);

    if (attacker->GetTypeId() == TYPEID_PLAYER)
        dodgeChance -= attacker->ToPlayer()->GetExpertiseDodgeOrParryReduction(attType);
    else
    {
        Player* statOwner = nullptr;
        if (attacker->HasUnitTypeMask(UNIT_MASK_MINION))
        {
            Minion const* minion = static_cast<Minion const*>(attacker);
            if (minion->GetOwner())
                statOwner = minion->GetOwner()->ToPlayer();
        }
        if (statOwner)
            dodgeChance -= GetExpertiseOrHitChanceFormOwner(statOwner);
        else
            dodgeChance -= attacker->GetTotalAuraModifier(SPELL_AURA_MOD_EXPERTISE) * 0.25f;
    }

    if (dodgeChance < 0.0f)
        dodgeChance = 0.0f;

    return dodgeChance;
}

float Unit::GetParryChanceAgainst(Unit const* attacker, WeaponAttackType attType) const
{
    float parryChance = GetUnitParryChance();
    if (!parryChance)
        return 0.0f;

    parryChance += 1.5 * GetLevelDifferenceForPenalty(attacker, this);
    // Reduce parry chance by attacker expertise rating
    if (attacker->GetTypeId() == TYPEID_PLAYER)
        parryChance -= attacker->ToPlayer()->GetExpertiseDodgeOrParryReduction(attType);
    else
    {
        Player* statOwner = nullptr;
        if (attacker->HasUnitTypeMask(UNIT_MASK_MINION))
        {
            Minion const* minion = static_cast<Minion const*>(attacker);
            if (minion->GetOwner())
                statOwner = minion->GetOwner()->ToPlayer();
        }
        if (statOwner)
            parryChance -= GetExpertiseOrHitChanceFormOwner(statOwner);
        else
            parryChance -= attacker->GetTotalAuraModifier(SPELL_AURA_MOD_EXPERTISE) * 0.25f;
    }

    if (parryChance < 0)
        parryChance = 0;

    return parryChance;
}

float Unit::GetBlockChangeAgainst(Unit const * attacker, WeaponAttackType) const
{
    float blockChance = GetUnitBlockChance();
    if (!blockChance)
        return 0.0f;
    blockChance += 1.5 * GetLevelDifferenceForPenalty(attacker, this);
    return blockChance;
}

float Unit::GetUnitCriticalChance(WeaponAttackType attackType, const Unit* victim) const
{
    float crit;

    if (GetTypeId() == TYPEID_PLAYER)
    {
        switch (attackType)
        {
            case BASE_ATTACK:
                crit = GetFloatValue(PLAYER_FIELD_CRIT_PERCENTAGE);
                break;
            case OFF_ATTACK:
                crit = GetFloatValue(PLAYER_FIELD_OFFHAND_CRIT_PERCENTAGE);
                break;
            case RANGED_ATTACK:
                crit = GetFloatValue(PLAYER_FIELD_RANGED_CRIT_PERCENTAGE);
                break;
                // Just for good manner
            default:
                crit = 0.0f;
                break;
        }
    }
    else
    {
        Player* critOwner = nullptr;
        if (IsPetGuardianStuff())
            if (Unit* owner = GetOwner())
                critOwner = owner->ToPlayer();
        if (critOwner)
        {
            crit = critOwner->GetFloatValue(PLAYER_FIELD_CRIT_PERCENTAGE); // I dunno how it must work, so be it
            crit += GetTotalAuraModifier(SPELL_AURA_MOD_CRIT_PCT);
        }
        else
        {
            crit = 5.0f;
            crit += GetTotalAuraModifier(SPELL_AURA_MOD_WEAPON_CRIT_PERCENT);
            crit += GetTotalAuraModifier(SPELL_AURA_MOD_CRIT_PCT);
        }
    }

    // flat aura mods
    if (attackType == RANGED_ATTACK)
        crit += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_CHANCE);
    else
        crit += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_CHANCE);

    crit += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCE);

    if (crit < 0.0f)
        crit = 0.0f;
    return crit;
}

void Unit::_DeleteRemovedAuras()
{
    while (!m_removedAuras.empty())
    {
        delete m_removedAuras.front();
        m_removedAuras.pop_front();
    }
}

void Unit::_UpdateSpells(uint32 time)
{
    if (m_currentSpells [CURRENT_AUTOREPEAT_SPELL])
        _UpdateAutoRepeatSpell();

    // remove finished spells from current pointers
    for (uint32 i = 0; i < CURRENT_MAX_SPELL; ++i)
    {
        if (m_currentSpells [i] && m_currentSpells [i]->getState() == SPELL_STATE_FINISHED)
        {
            m_currentSpells [i]->SetReferencedFromCurrent(false);
            m_currentSpells [i] = NULL;                      // remove pointer
        }
    }

    // m_auraUpdateIterator can be updated in indirect called code at aura remove to skip next planned to update but removed auras
    for (m_auraUpdateIterator = m_ownedAuras.begin(); m_auraUpdateIterator != m_ownedAuras.end();)
    {
        Aura* i_aura = m_auraUpdateIterator->second;
        ++m_auraUpdateIterator;                            // need shift to next for allow update if need into aura update
        i_aura->UpdateOwner(time, this);
    }

    // remove expired auras - do that after updates(used in scripts?)
    for (AuraMap::iterator i = m_ownedAuras.begin(); i != m_ownedAuras.end();)
    {
        if (i->second->IsExpired())
            RemoveOwnedAura(i, AURA_REMOVE_BY_EXPIRE);
        else
            ++i;
    }

    for (VisibleAuraMap::iterator itr = m_visibleAuras.begin(); itr != m_visibleAuras.end(); ++itr)
        if (itr->second->IsNeedClientUpdate())
            itr->second->ClientUpdate();

    _DeleteRemovedAuras();

    if (!m_gameObj.empty())
    {
        GameObjectList::iterator itr;
        for (itr = m_gameObj.begin(); itr != m_gameObj.end();)
        {
            if (!(*itr)->isSpawned())
            {
                (*itr)->SetOwnerGUID(ObjectGuid::Empty);
                (*itr)->SetRespawnTime(0);
                (*itr)->Delete();
                m_gameObj.erase(itr++);
            }
            else
                ++itr;
        }
    }
}

void Unit::_UpdateAutoRepeatSpell()
{
    SpellInfo const* spellProto = nullptr;
    if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL])
    {
        spellProto = m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo;
    }

    if (!spellProto)
    {
        return;
    }

    static uint32 const HUNTER_AUTOSHOOT = 75;

    // check "realtime" interrupts
    // don't cancel spells which are affected by a SPELL_AURA_CAST_WHILE_WALKING effect
    if (((IsPlayer() && ToPlayer()->isMoving() && spellProto->Id != HUNTER_AUTOSHOOT) || IsNonMeleeSpellCasted(false, false, true, spellProto->Id == HUNTER_AUTOSHOOT)) &&
        !HasAuraTypeWithAffectMask(SPELL_AURA_CAST_WHILE_WALKING, spellProto))
    {
        // cancel wand shoot
        if (spellProto->Id != 75)
            InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
        return;
    }

    // castroutine
    if (isAttackReady(RANGED_ATTACK) && m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->getState() != SPELL_STATE_PREPARING)
    {
        // Check if able to cast
        SpellCastResult result = m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->CheckCast(true);
        if (result != SPELL_CAST_OK)
        {
            if (spellProto->Id != HUNTER_AUTOSHOOT)
                InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
            else if (GetTypeId() == TYPEID_PLAYER)
                Spell::SendCastResult(ToPlayer(), spellProto, m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_cast_count, result);

            return;
        }

        // we want to shoot
        Spell* spell = new Spell(this, spellProto, TRIGGERED_IGNORE_GCD);
        spell->prepare(&(m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_targets));
    }
}

void Unit::SetCurrentCastedSpell(Spell* pSpell)
{
    ASSERT(pSpell);                                         // NULL may be never passed here, use InterruptSpell or InterruptNonMeleeSpells

    CurrentSpellTypes CSpellType = pSpell->GetCurrentContainer();

    if (pSpell == m_currentSpells [CSpellType])             // avoid breaking self
        return;

    // special breakage effects:
    switch (CSpellType)
    {
        case CURRENT_GENERIC_SPELL:
        {
            // generic spells always break channeled not delayed spells
            InterruptSpell(CURRENT_CHANNELED_SPELL, false);

            // autorepeat breaking
            if (m_currentSpells [CURRENT_AUTOREPEAT_SPELL])
            {
                // break autorepeat if not Auto Shot
                if (m_currentSpells [CURRENT_AUTOREPEAT_SPELL]->GetSpellInfo()->Id != 75)
                    InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
            }
            if (pSpell->GetCastTime() > 0)
                AddUnitState(UNIT_STATE_CASTING);

            break;
        }
        case CURRENT_CHANNELED_SPELL:
        {
            // channel spells always break generic non-delayed and any channeled spells
            InterruptSpell(CURRENT_GENERIC_SPELL, false);
            InterruptSpell(CURRENT_CHANNELED_SPELL);

            // it also does break autorepeat if not Auto Shot
            if (m_currentSpells [CURRENT_AUTOREPEAT_SPELL] &&
                m_currentSpells [CURRENT_AUTOREPEAT_SPELL]->GetSpellInfo()->Id != 75)
                InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
            AddUnitState(UNIT_STATE_CASTING);

            break;
        }
        case CURRENT_AUTOREPEAT_SPELL:
        {
            if (m_currentSpells[CSpellType] && m_currentSpells[CSpellType]->getState() == SPELL_STATE_IDLE)
                m_currentSpells[CSpellType]->setState(SPELL_STATE_FINISHED);

            // only Auto Shoot does not break anything
            if (pSpell->GetSpellInfo()->Id != 75)
            {
                // generic autorepeats break generic non-delayed and channeled non-delayed spells
                InterruptSpell(CURRENT_GENERIC_SPELL, false);
                InterruptSpell(CURRENT_CHANNELED_SPELL, false);
            }

            break;
        }
        default:
            break; // other spell types don't break anything now
    }

    // current spell (if it is still here) may be safely deleted now
    if (m_currentSpells [CSpellType])
        m_currentSpells [CSpellType]->SetReferencedFromCurrent(false);

    // set new current spell
    m_currentSpells [CSpellType] = pSpell;
    pSpell->SetReferencedFromCurrent(true);

    pSpell->m_selfContainer = &(m_currentSpells [pSpell->GetCurrentContainer()]);
}

void Unit::InterruptSpell(CurrentSpellTypes spellType, bool withDelayed, bool withInstant)
{
    //TC_LOG_DEBUG("entities.unit", "Interrupt spell for unit %u.", GetEntry());
    Spell* spell = m_currentSpells [spellType];
    if (spell
        && (withDelayed || spell->getState() != SPELL_STATE_DELAYED)
        && (withInstant || spell->GetCastTime() > 0 || spell->getState() == SPELL_STATE_CASTING))
    {
        // for example, do not let self-stun aura interrupt itself
        if (!spell->IsInterruptable())
            return;

        // send autorepeat cancel message for autorepeat spells
        if (spellType == CURRENT_AUTOREPEAT_SPELL)
            if (GetTypeId() == TYPEID_PLAYER)
                ToPlayer()->SendAutoRepeatCancel(this);

        if (GetTypeId() == TYPEID_UNIT && IsAIEnabled)
            ToCreature()->AI()->CastInterrupted(spell->GetSpellInfo());

        if (spell->getState() != SPELL_STATE_FINISHED)
            spell->cancel();
        else
        {
            m_currentSpells[spellType] = nullptr;
            spell->SetReferencedFromCurrent(false);
        }
    }
}

void Unit::FinishSpell(CurrentSpellTypes spellType, bool ok /*= true*/)
{
    Spell* spell = m_currentSpells [spellType];
    if (!spell)
        return;

    if (spellType == CURRENT_CHANNELED_SPELL)
        spell->SendChannelUpdate(0);

    spell->finish(ok);
}

bool Unit::IsNonMeleeSpellCasted(bool withDelayed, bool skipChanneled, bool skipAutorepeat, bool isAutoshoot, bool skipInstant) const
{
    // We don't do loop here to explicitly show that melee spell is excluded.
    // Maybe later some special spells will be excluded too.

    // if skipInstant then instant spells shouldn't count as being casted
    if (skipInstant && m_currentSpells[CURRENT_GENERIC_SPELL] && m_currentSpells[CURRENT_GENERIC_SPELL]->getState() != SPELL_STATE_FINISHED && !m_currentSpells[CURRENT_GENERIC_SPELL]->GetCastTime())
        return false;

    // generic spells are casted when they are not finished and not delayed
    if (m_currentSpells [CURRENT_GENERIC_SPELL] &&
        (m_currentSpells [CURRENT_GENERIC_SPELL]->getState() != SPELL_STATE_FINISHED) &&
        (withDelayed || m_currentSpells [CURRENT_GENERIC_SPELL]->getState() != SPELL_STATE_DELAYED))
    {
        if (!isAutoshoot || !(m_currentSpells [CURRENT_GENERIC_SPELL]->m_spellInfo->AttributesEx2 & SPELL_ATTR2_NOT_RESET_AUTO_ACTIONS))
            return true;
    }
    // channeled spells may be delayed, but they are still considered casted
    else if (!skipChanneled && m_currentSpells [CURRENT_CHANNELED_SPELL] &&
             (m_currentSpells [CURRENT_CHANNELED_SPELL]->getState() != SPELL_STATE_FINISHED))
    {
        if (!isAutoshoot || !(m_currentSpells [CURRENT_CHANNELED_SPELL]->m_spellInfo->AttributesEx2 & SPELL_ATTR2_NOT_RESET_AUTO_ACTIONS))
            return true;
    }
    // autorepeat spells may be finished or delayed, but they are still considered casted
    else if (!skipAutorepeat && m_currentSpells [CURRENT_AUTOREPEAT_SPELL])
        return true;

    return false;
}

void Unit::InterruptNonMeleeSpells(bool withDelayed, uint32 spell_id, bool withInstant)
{
    // generic spells are interrupted if they are not finished or delayed
    if (m_currentSpells [CURRENT_GENERIC_SPELL] && (!spell_id || m_currentSpells [CURRENT_GENERIC_SPELL]->m_spellInfo->Id == spell_id))
        InterruptSpell(CURRENT_GENERIC_SPELL, withDelayed, withInstant);

    // autorepeat spells are interrupted if they are not finished or delayed
    if (m_currentSpells [CURRENT_AUTOREPEAT_SPELL] && (!spell_id || m_currentSpells [CURRENT_AUTOREPEAT_SPELL]->m_spellInfo->Id == spell_id))
        InterruptSpell(CURRENT_AUTOREPEAT_SPELL, withDelayed, withInstant);

    // channeled spells are interrupted if they are not finished, even if they are delayed
    if (m_currentSpells [CURRENT_CHANNELED_SPELL] && (!spell_id || m_currentSpells [CURRENT_CHANNELED_SPELL]->m_spellInfo->Id == spell_id))
        InterruptSpell(CURRENT_CHANNELED_SPELL, true, true);
}

Spell* Unit::FindCurrentSpellBySpellId(uint32 spell_id) const
{
    for (uint32 i = 0; i < CURRENT_MAX_SPELL; i++)
        if (m_currentSpells [i] && m_currentSpells [i]->m_spellInfo->Id == spell_id)
            return m_currentSpells [i];
    return NULL;
}

int32 Unit::GetCurrentSpellCastTime(uint32 spell_id) const
{
    if (Spell const* spell = FindCurrentSpellBySpellId(spell_id))
        return spell->GetCastTime();
    return 0;
}

bool Unit::isInFrontInMap(Unit const* target, float distance, float arc) const
{
    return IsWithinDistInMap(target, distance) && HasInArc(arc, target);
}

bool Unit::isInBackInMap(Unit const* target, float distance, float arc) const
{
    return IsWithinDistInMap(target, distance) && !HasInArc(2 * M_PI - arc, target);
}

bool Unit::isInAccessiblePlaceFor(Creature const* c) const
{
    if (IsInWater())
        return c->CanSwim();
    else
        return c->CanWalk() || c->CanFly();
}

bool Unit::IsInWater() const
{
    return GetLiquidStatus() & (LIQUID_MAP_IN_WATER | LIQUID_MAP_UNDER_WATER);
}

bool Unit::IsUnderWater() const
{
    return GetLiquidStatus() & LIQUID_MAP_UNDER_WATER;
}

void Unit::ProcessPositionDataChanged(PositionFullTerrainStatus const& data)
{
    ZLiquidStatus oldLiquidStatus = GetLiquidStatus();
    WorldObject::ProcessPositionDataChanged(data);
    ProcessTerrainStatusUpdate(oldLiquidStatus, data.liquidInfo);
}

void Unit::ProcessTerrainStatusUpdate(ZLiquidStatus /*oldLiquidStatus*/, Optional<LiquidData> const& newLiquidData)
{
    if (!IsControlledByPlayer())
        return;

    // remove appropriate auras if we are swimming/not swimming respectively
    if (IsInWater())
        RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_NOT_ABOVEWATER);
    else
        RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_NOT_UNDERWATER);

    // liquid aura handling
    LiquidTypeEntry const* curLiquid = nullptr;
    if (IsInWater() && newLiquidData)
        curLiquid = sLiquidTypeStore.LookupEntry(newLiquidData->entry);
    if (curLiquid != _lastLiquid)
    {
        if (_lastLiquid && _lastLiquid->SpellID)
            RemoveAurasDueToSpell(_lastLiquid->SpellID);
        Player* player = GetCharmerOrOwnerPlayerOrPlayerItself();

        // Set _lastLiquid before casting liquid spell to avoid infinite loops
        _lastLiquid = curLiquid;

        if (curLiquid && curLiquid->SpellID && (!player || !player->IsGameMaster()))
            CastSpell(this, curLiquid->SpellID, true);
    }
}

void Unit::DeMorph()
{
    SetDisplayId(GetNativeDisplayId());
}

Aura* Unit::_TryStackingOrRefreshingExistingAura(SpellInfo const* newAura, uint32 effMask, Unit* caster, int32* baseAmount /*= NULL*/, Item* castItem /*= NULL*/, ObjectGuid casterGUID /*= 0*/)
{
    ASSERT(casterGUID || caster);

    // Check if these can stack anyway
    if (!casterGUID && !newAura->IsStackableOnOneSlotWithDifferentCasters())
        casterGUID = caster->GetGUID();

    // passive and Incanter's Absorption and auras with different type can stack with themselves any number of times
    if (!newAura->IsMultiSlotAura())
    {
        // check if cast item changed
        ObjectGuid castItemGUID = ObjectGuid::Empty;
        if (castItem)
            castItemGUID = castItem->GetGUID();

        // find current aura from spell and change it's stackamount, or refresh it's duration
        if (Aura* foundAura = GetOwnedAura(newAura->Id, casterGUID, (newAura->AttributesCu & SPELL_ATTR0_CU_ENCHANT_PROC) ? castItemGUID : ObjectGuid::Empty, 0))
        {
            // effect masks do not match
            // extremely rare case
            // let's just recreate aura
            if (effMask != foundAura->GetEffectMask())
                return NULL;


            float cumulativeAmount = 0.0f;
            int32 effectIndex = -1;

            // update basepoints with new values - effect amount will be recalculated in ModStackAmount
            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            {
                if (!foundAura->HasEffect(i))
                    continue;
                if (effectIndex == -1 && foundAura->GetSpellInfo()->HasAttribute(SPELL_ATTR10_CUMULATIVE_PERIODIC) && foundAura->GetEffect(i)->m_isPeriodic && foundAura->GetEffect(i)->GetAmplitude())
                {
                    AuraEffect* eff = foundAura->GetEffect(i);
                    int32 ticks = foundAura->GetDuration() / eff->GetAmplitude();
                    if (foundAura->GetDuration() % eff->GetAmplitude() != 0)
                    {
                        ticks += 1;
                    }
                    cumulativeAmount = eff->GetFloatAmount() * ticks;
                    effectIndex = i;
                }

                int bp;
                if (baseAmount)
                    bp = *(baseAmount + i);
                else
                    bp = foundAura->GetSpellInfo()->Effects [i].BasePoints;

                int32* oldBP = const_cast<int32*>(&(foundAura->GetEffect(i)->m_baseAmount));
                *oldBP = bp;
            }

            // correct cast item guid if needed
            if (castItemGUID != foundAura->GetCastItemGUID())
            {
                ObjectGuid oldGUID = castItemGUID;
            }

            if (foundAura->GetId() == 980)
                foundAura->RefreshTimers(true);
            else
                foundAura->ModStackAmount(1);

            if (effectIndex != -1)
            {
                AuraEffect* eff = foundAura->GetEffect(effectIndex);
                int32 ticks = foundAura->GetCalculatedMaxDuration() / eff->GetAmplitude();
                cumulativeAmount += eff->GetFloatAmount() * ticks;
                ticks = eff->GetBase()->GetDuration() / eff->GetAmplitude();
                if (eff->GetBase()->GetDuration() % eff->GetAmplitude() != 0)
                    ticks += 1;
                eff->SetFloatAmount(std::ceil(cumulativeAmount / ticks)); // New amount was divided as int, try to compensate it
            }

            return foundAura;
        }
    }

    return NULL;
}

void Unit::_AddAura(UnitAura* aura, Unit* caster)
{
    ASSERT(!m_cleanupDone);

    m_ownedAuras.insert(AuraMap::value_type(aura->GetId(), aura));

    _RemoveNoStackAurasDueToAura(aura);
}

// creates aura application instance and registers it in lists
// aura application effects are handled separately to prevent aura list corruption
AuraApplication * Unit::_CreateAuraApplication(Aura* aura, uint32 effMask)
{
    // can't apply aura on unit which is going to be deleted - to not create a memory leak
    ASSERT(!m_cleanupDone);
    // aura musn't be removed
    ASSERT(!aura->IsRemoved());

    // aura mustn't be already applied on target
    ASSERT(!aura->IsAppliedOnTarget(GetGUID()) && "Unit::_CreateAuraApplication: aura musn't be applied on target");

    SpellInfo const* aurSpellInfo = aura->GetSpellInfo();
    uint32 aurId = aurSpellInfo->Id;

    // ghost spell check, allow apply any auras at player loading in ghost mode (will be cleanup after load)
    // SPELL_ATTR0_CASTABLE_WHILE_DEAD because with death persistent aura won't be removed at death
    if (!IsAlive() && !aurSpellInfo->HasAttribute(SPELL_ATTR0_PASSIVE) && !aurSpellInfo->IsDeathPersistent() && !aurSpellInfo->HasAttribute(SPELL_ATTR0_CASTABLE_WHILE_DEAD) &&
        (GetTypeId() != TYPEID_PLAYER || !ToPlayer()->GetSession()->PlayerLoading()))
        return NULL;

    Unit* caster = aura->GetCaster();

    AuraApplication * aurApp = new AuraApplication(this, caster, aura, effMask);
    m_appliedAuras.insert(AuraApplicationMap::value_type(aurId, aurApp));

    auto procEntry = sSpellMgr->GetSpellProcEvent(aurId);
    if (procEntry)
    {
        if (procEntry->phaseMask & ProcPhaseMask::Hit)
            m_procAuras[uint32(ProcPhase::Hit)].insert(AuraApplicationMap::value_type(aurId, aurApp));
        if (procEntry->phaseMask & ProcPhaseMask::Cast)
            m_procAuras[uint32(ProcPhase::Cast)].insert(AuraApplicationMap::value_type(aurId, aurApp));
        if (procEntry->phaseMask & ProcPhaseMask::AfterCast)
            m_procAuras[uint32(ProcPhase::AfterCast)].insert(AuraApplicationMap::value_type(aurId, aurApp));
    }
    else
        m_procAuras[uint32(ProcPhase::Hit)].insert(AuraApplicationMap::value_type(aurId, aurApp));

    if (aurSpellInfo->AuraInterruptFlags)
    {
        m_interruptableAuras.push_back(aurApp);
        AddInterruptMask(aurSpellInfo->AuraInterruptFlags);
    }

    if (AuraStateType aState = aura->GetSpellInfo()->GetAuraState())
        m_auraStateAuras.insert(AuraStateAurasMap::value_type(aState, aurApp));

    aura->_ApplyForTarget(this, caster, aurApp);
    return aurApp;
}

void Unit::_ApplyAuraEffect(Aura* aura, uint8 effIndex)
{
    ASSERT(aura);
    ASSERT(aura->HasEffect(effIndex));
    AuraApplication * aurApp = aura->GetApplicationOfTarget(GetGUID());
    ASSERT(aurApp);
    if (!aurApp->GetEffectMask())
        _ApplyAura(aurApp, 1 << effIndex);
    else
        aurApp->_HandleEffect(effIndex, true);
}

// handles effects of aura application
// should be done after registering aura in lists
void Unit::_ApplyAura(AuraApplication * aurApp, uint32 effMask)
{
    Aura* aura = aurApp->GetBase();

    _RemoveNoStackAurasDueToAura(aura);

    if (aurApp->GetRemoveMode())
        return;

    // Update target aura state flag
    if (AuraStateType aState = aura->GetSpellInfo()->GetAuraState())
        ModifyAuraState(aState, true);

    if (aurApp->GetRemoveMode())
        return;

    // Sitdown on apply aura req seated
    if (aura->GetSpellInfo()->HasAuraInterruptFlag(AURA_INTERRUPT_FLAG_NOT_SEATED) && !IsSitState())
        SetStandState(UNIT_STAND_STATE_SIT);

    Unit* caster = aura->GetCaster();

    if (aurApp->GetRemoveMode())
        return;

    aura->HandleAuraSpecificMods(aurApp, caster, true, false);

    // apply effects of the aura
    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (effMask & 1 << i && (!aurApp->GetRemoveMode()))
            aurApp->_HandleEffect(i, true);
    }
}

// removes aura application from lists and unapplies effects
void Unit::_UnapplyAura(AuraApplicationMap::iterator &i, AuraRemoveMode removeMode)
{
    AuraApplication * aurApp = i->second;
    ASSERT(aurApp);
    ASSERT(!aurApp->GetRemoveMode());
    ASSERT(aurApp->GetTarget() == this);

    aurApp->SetRemoveMode(removeMode);
    Aura* aura = aurApp->GetBase();
    TC_LOG_DEBUG("spells", "Aura %u now is remove mode %d", aura->GetId(), removeMode);

    // dead loop is killing the server probably
    ASSERT(m_removedAurasCount < 0xFFFFFFFF);

    ++m_removedAurasCount;

    Unit* caster = aura->GetCaster();

    // Remove all pointers from lists here to prevent possible pointer invalidation on spellcast/auraapply/auraremove
    m_appliedAuras.erase(i);

    for (auto&& auras : m_procAuras)
    {
        auto bounds = auras.equal_range(aura->GetSpellInfo()->Id);
        for (auto itr = bounds.first; itr != bounds.second;)
        {
            if (itr->second == aurApp)
                auras.erase(itr++);
            else
                ++itr;
        }
    }

    if (aura->GetSpellInfo()->AuraInterruptFlags)
    {
        m_interruptableAuras.remove(aurApp);
        UpdateInterruptMask();
    }

    bool auraStateFound = false;
    AuraStateType auraState = aura->GetSpellInfo()->GetAuraState();
    if (auraState)
    {
        bool canBreak = false;
        // Get mask of all aurastates from remaining auras
        for (AuraStateAurasMap::iterator itr = m_auraStateAuras.lower_bound(auraState); itr != m_auraStateAuras.upper_bound(auraState) && !(auraStateFound && canBreak);)
        {
            if (itr->second == aurApp)
            {
                m_auraStateAuras.erase(itr);
                itr = m_auraStateAuras.lower_bound(auraState);
                canBreak = true;
                continue;
            }
            auraStateFound = true;
            ++itr;
        }
    }

    aurApp->_Remove();
    aura->_UnapplyForTarget(this, caster, aurApp);

    // remove effects of the spell - needs to be done after removing aura from lists
    for (uint32 itr = 0; itr < MAX_SPELL_EFFECTS; ++itr)
    {
        if (aurApp->HasEffect(itr))
            aurApp->_HandleEffect(itr, false);
    }

    // all effect mustn't be applied
    ASSERT(!aurApp->GetEffectMask());

    // Remove totem at next update if totem loses its aura
    if (aurApp->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE && GetTypeId() == TYPEID_UNIT && ToCreature()->IsTotem() && ToTotem()->GetSummonerGUID() == aura->GetCasterGUID())
    {
        if (ToTotem()->GetSpell() == aura->GetId() && ToTotem()->GetTotemType() == TOTEM_PASSIVE)
            ToTotem()->setDeathState(JUST_DIED);
    }

    // Remove aurastates only if were not found
    if (!auraStateFound)
        ModifyAuraState(auraState, false);

    aura->HandleAuraSpecificMods(aurApp, caster, false, false);

    // only way correctly remove all auras from list
    //if (removedAuras != m_removedAurasCount) new aura may be added
    i = m_appliedAuras.begin();
}

void Unit::_UnapplyAura(AuraApplication * aurApp, AuraRemoveMode removeMode)
{
    // aura can be removed from unit only if it's applied on it, shouldn't happen
    ASSERT(aurApp->GetBase()->GetApplicationOfTarget(GetGUID()) == aurApp);

    uint32 spellId = aurApp->GetBase()->GetId();
    AuraApplicationMapBoundsNonConst range = m_appliedAuras.equal_range(spellId);

    for (AuraApplicationMap::iterator iter = range.first; iter != range.second;)
    {
        if (iter->second == aurApp)
        {
            _UnapplyAura(iter, removeMode);
            return;
        }
        else
            ++iter;
    }
    ASSERT(false);
}

void Unit::_RemoveNoStackAurasDueToAura(Aura* aura)
{
    SpellInfo const* spellProto = aura->GetSpellInfo();

    // passive spell special case (only non stackable with ranks)
    if (spellProto->IsPassiveStackableWithRanks())
        return;

    bool remove = false;
    for (AuraApplicationMap::iterator i = m_appliedAuras.begin(); i != m_appliedAuras.end(); ++i)
    {
        if (remove)
        {
            remove = false;
            i = m_appliedAuras.begin();
        }

        if (aura->CanStackWith(i->second->GetBase()))
            continue;

        RemoveAura(i, AURA_REMOVE_BY_DEFAULT);
        if (i == m_appliedAuras.end())
            break;
        remove = true;
    }
}

void Unit::_RegisterAuraEffect(AuraEffect* aurEff, bool apply)
{
    if (apply)
        m_modAuras [aurEff->GetAuraType()].push_back(aurEff);
    else
        m_modAuras [aurEff->GetAuraType()].remove(aurEff);
}

// All aura base removes should go threw this function!
void Unit::RemoveOwnedAura(AuraMap::iterator &i, AuraRemoveMode removeMode)
{
    Aura* aura = i->second;
    ASSERT(!aura->IsRemoved());

    // if unit currently update aura list then make safe update iterator shift to next
    if (m_auraUpdateIterator == i)
        ++m_auraUpdateIterator;

    m_ownedAuras.erase(i);
    m_removedAuras.push_back(aura);

    if (aura->IsBoundToCaster())
    {
        Unit* caster = aura->GetCaster();
        if (!caster)
        {
            TC_LOG_ERROR("shitlog", "Unit::RemoveOwnedAura !caster aura: %u, owner: " UI64FMTD " (entry: %u)\n", aura->GetId(), GetGUID().GetRawValue(), GetEntry());
            caster = ObjectAccessor::FindPlayer(aura->GetCasterGUID());
            if (!caster)
                TC_LOG_ERROR("shitlog", "Unit::RemoveOwnedAura !caster and !caster aura: %u, owner: " UI64FMTD " (entry: %u)\n", aura->GetId(), GetGUID().GetRawValue(), GetEntry());
        }
        // ASSERT(caster);
        if (caster)
        {
            // FIXME
            ASSERT(IsInMap(caster) || (caster->GetMap() == GetMap()));    // During remove from map
            caster->UnbindAura(aura);
        }
    }

    aura->_Remove(removeMode);

    i = m_ownedAuras.begin();
}

void Unit::RemoveOwnedAura(uint32 spellId, ObjectGuid casterGUID, uint32 reqEffMask, AuraRemoveMode removeMode)
{
    for (AuraMap::iterator itr = m_ownedAuras.lower_bound(spellId); itr != m_ownedAuras.upper_bound(spellId);)
        if (((itr->second->GetEffectMask() & reqEffMask) == reqEffMask) && (!casterGUID || itr->second->GetCasterGUID() == casterGUID))
        {
            RemoveOwnedAura(itr, removeMode);
            itr = m_ownedAuras.lower_bound(spellId);
        }
        else
            ++itr;
}

void Unit::RemoveOwnedAura(Aura* aura, AuraRemoveMode removeMode)
{
    if (aura->IsRemoved())
        return;

    ASSERT(aura->GetOwner() == this);

    uint32 spellId = aura->GetId();
    AuraMapBoundsNonConst range = m_ownedAuras.equal_range(spellId);

    for (AuraMap::iterator itr = range.first; itr != range.second; ++itr)
    {
        if (itr->second == aura)
        {
            RemoveOwnedAura(itr, removeMode);
            return;
        }
    }

    ASSERT(false);
}

Aura* Unit::GetOwnedAura(uint32 spellId, ObjectGuid casterGUID, ObjectGuid itemCasterGUID, uint32 reqEffMask, Aura* except) const
{
    AuraMapBounds range = m_ownedAuras.equal_range(spellId);
    for (AuraMap::const_iterator itr = range.first; itr != range.second; ++itr)
    {
        if (((itr->second->GetEffectMask() & reqEffMask) == reqEffMask)
            && (!casterGUID || itr->second->GetSpellInfo()->HasCustomAttribute(SPELL_ATTR0_CU_CAN_STACK_FROM_DIFF_CASTERS) || itr->second->GetCasterGUID() == casterGUID)
            && (!itemCasterGUID || itr->second->GetCastItemGUID() == itemCasterGUID)
            && (!except || except != itr->second))
        {
            return itr->second;
        }
    }
    return NULL;
}

void Unit::RemoveAura(AuraApplicationMap::iterator &i, AuraRemoveMode mode)
{
    AuraApplication * aurApp = i->second;
    // Do not remove aura which is already being removed
    if (aurApp->GetRemoveMode())
        return;
    Aura* aura = aurApp->GetBase();
    _UnapplyAura(i, mode);
    // Remove aura - for Area and Target auras
    if (aura->GetOwner() == this)
        aura->Remove(mode);
}

void Unit::RemoveAura(uint32 spellId, uint64 caster, uint32 reqEffMask, AuraRemoveMode removeMode)
{
    AuraApplicationMapBoundsNonConst range = m_appliedAuras.equal_range(spellId);
    for (AuraApplicationMap::iterator iter = range.first; iter != range.second;)
    {
        Aura const* aura = iter->second->GetBase();
        if (((aura->GetEffectMask() & reqEffMask) == reqEffMask)
            && (!caster || aura->GetCasterGUID() == caster))
        {
            RemoveAura(iter, removeMode);
            return;
        }
        else
            ++iter;
    }
}

void Unit::RemoveAura(AuraApplication * aurApp, AuraRemoveMode mode)
{
    // we've special situation here, RemoveAura called while during aura removal
    // this kind of call is needed only when aura effect removal handler
    // or event triggered by it expects to remove
    // not yet removed effects of an aura
    if (aurApp->GetRemoveMode())
    {
        // remove remaining effects of an aura
        for (uint32 itr = 0; itr < MAX_SPELL_EFFECTS; ++itr)
        {
            if (aurApp->HasEffect(itr))
                aurApp->_HandleEffect(itr, false);
        }
        return;
    }
    // no need to remove
    if (aurApp->GetBase()->GetApplicationOfTarget(GetGUID()) != aurApp || aurApp->GetBase()->IsRemoved())
        return;

    uint32 spellId = aurApp->GetBase()->GetId();
    AuraApplicationMapBoundsNonConst range = m_appliedAuras.equal_range(spellId);

    for (AuraApplicationMap::iterator iter = range.first; iter != range.second;)
    {
        if (aurApp == iter->second)
        {
            RemoveAura(iter, mode);
            return;
        }
        else
            ++iter;
    }
}

void Unit::RemoveAura(Aura* aura, AuraRemoveMode mode)
{
    if (aura->IsRemoved())
        return;
    if (AuraApplication * aurApp = aura->GetApplicationOfTarget(GetGUID()))
        RemoveAura(aurApp, mode);
}

void Unit::RemoveAurasDueToSpell(uint32 spellId, uint64 casterGUID, uint32 reqEffMask, AuraRemoveMode removeMode)
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.lower_bound(spellId); iter != m_appliedAuras.upper_bound(spellId);)
    {
        Aura const* aura = iter->second->GetBase();
        if (((aura->GetEffectMask() & reqEffMask) == reqEffMask)
            && (!casterGUID || aura->GetCasterGUID() == casterGUID))
        {
            RemoveAura(iter, removeMode);
            iter = m_appliedAuras.lower_bound(spellId);
        }
        else
            ++iter;
    }
}

void Unit::RemoveAuraFromStack(uint32 spellId, uint64 casterGUID, AuraRemoveMode removeMode, int32 num)
{
    AuraMapBoundsNonConst range = m_ownedAuras.equal_range(spellId);
    for (AuraMap::iterator iter = range.first; iter != range.second;)
    {
        Aura* aura = iter->second;
        if ((aura->GetType() == UNIT_AURA_TYPE)
            && (!casterGUID || aura->GetCasterGUID() == casterGUID))
        {
            aura->ModStackAmount(-num, removeMode);
            return;
        }
        else
            ++iter;
    }
}

void Unit::RemoveAurasDueToSpellByDispel(uint32 spellId, uint32 dispellerSpellId, uint64 casterGUID, Unit* dispeller, uint8 chargesRemoved/*= 1*/)
{
    AuraMapBoundsNonConst range = m_ownedAuras.equal_range(spellId);
    for (AuraMap::iterator iter = range.first; iter != range.second;)
    {
        Aura* aura = iter->second;
        if (aura->GetCasterGUID() == casterGUID)
        {
            DispelInfo dispelInfo(dispeller, dispellerSpellId, chargesRemoved);

            // Call OnDispel hook on AuraScript
            aura->CallScriptDispel(&dispelInfo);

            if (aura->GetSpellInfo()->AttributesEx7 & SPELL_ATTR7_DISPEL_CHARGES)
                aura->ModCharges(-dispelInfo.GetRemovedCharges(), AURA_REMOVE_BY_ENEMY_SPELL);
            else
                aura->ModStackAmount(-dispelInfo.GetRemovedCharges(), AURA_REMOVE_BY_ENEMY_SPELL);

            // Call AfterDispel hook on AuraScript
            aura->CallScriptAfterDispel(&dispelInfo);

            return;
        }
        else
            ++iter;
    }
}

void Unit::RemoveAurasDueToSpellBySteal(uint32 spellId, uint64 casterGUID, Unit* stealer)
{
    AuraMapBoundsNonConst range = m_ownedAuras.equal_range(spellId);
    for (AuraMap::iterator iter = range.first; iter != range.second;)
    {
        Aura* aura = iter->second;
        if (aura->GetCasterGUID() == casterGUID)
        {
            int32 damage [MAX_SPELL_EFFECTS];
            int32 baseDamage [MAX_SPELL_EFFECTS];
            uint32 effMask = 0;
            uint32 recalculateMask = 0;
            Unit* caster = aura->GetCaster();
            for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            {
                if (aura->GetEffect(i))
                {
                    baseDamage [i] = aura->GetEffect(i)->GetBaseAmount();
                    damage [i] = aura->GetEffect(i)->GetAmount();
                    effMask |= (1 << i);
                    if (aura->GetEffect(i)->CanBeRecalculated())
                        recalculateMask |= (1 << i);
                }
                else
                {
                    baseDamage [i] = 0;
                    damage [i] = 0;
                }
            }

            bool stealCharge = aura->GetSpellInfo()->AttributesEx7 & SPELL_ATTR7_DISPEL_CHARGES;
            // Cast duration to unsigned to prevent permanent aura's such as Righteous Fury being permanently added to caster
            int32 dur = std::min(2 * MINUTE * IN_MILLISECONDS, aura->GetDuration());

            // Find the same or similar aura on caster that shouldn't be overridden
            Aura* oldAura = stealer->GetAura(aura->GetId(), aura->GetCasterGUID());
            if (!oldAura)
            {
                for (auto&& existing : stealer->GetAppliedAuras())
                {
                    Aura* existingAura = existing.second->GetBase();
                    if (existingAura->GetDuration() >= dur && !aura->CanStackWith(existingAura))
                    {
                        oldAura = existingAura;
                        break;
                    }
                }
            }

            // If caster has the same aura as the one being stolen - add charge/stack and update duration
            if (oldAura && oldAura->GetId() == aura->GetId())
            {
                int32 oldDur = oldAura->GetDuration();

                if (stealCharge)
                    oldAura->ModCharges(1);
                else
                    oldAura->ModStackAmount(1);

                oldAura->SetDuration(std::max(oldDur, dur));
            }
            // If caster either doesn't have the same or similar aura, or they have lower duration than the one we can steal - apply new/override old
            else if (!oldAura)
            {
                // single target state must be removed before aura creation to preserve existing single target aura
                if (aura->IsBoundToCaster())
                    caster->UnbindAura(aura);   // Caster must exists

                if (Aura* newAura = Aura::TryRefreshStackOrCreate(aura->GetSpellInfo(), effMask, stealer, NULL, &baseDamage [0], NULL, aura->GetCasterGUID()))
                {
                    // FIXME: using aura->GetMaxDuration() maybe not blizzlike but it fixes stealing of spells like Innervate
                    newAura->SetLoadedState(aura->GetMaxDuration(), dur, stealCharge ? 1 : aura->GetCharges(), 1, recalculateMask, &damage [0]);
                    newAura->ApplyForTargets();
                    if (Unit* newAuraCaster = newAura->GetCaster())
                        if (newAura->IsBoundToCaster())
                            newAuraCaster->UnbindAura(newAura);
                }
            }

            if (stealCharge)
                aura->ModCharges(-1, AURA_REMOVE_BY_ENEMY_SPELL);
            else
                aura->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);

            return;
        }
        else
            ++iter;
    }
}

void Unit::RemoveAurasDueToItemSpell(uint32 spellId, uint64 castItemGuid)
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.lower_bound(spellId); iter != m_appliedAuras.upper_bound(spellId);)
    {
        if (iter->second->GetBase()->GetCastItemGUID() == castItemGuid)
        {
            RemoveAura(iter);
            iter = m_appliedAuras.lower_bound(spellId);
        }
        else
            ++iter;
    }
}

void Unit::RemoveAurasByType(AuraType auraType, uint64 casterGUID, Aura* except, bool negative, bool positive)
{
    for (AuraEffectList::iterator iter = m_modAuras [auraType].begin(); iter != m_modAuras [auraType].end();)
    {
        Aura* aura = (*iter)->GetBase();
        AuraApplication * aurApp = aura->GetApplicationOfTarget(GetGUID());

        ++iter;
        if (aura != except && (!casterGUID || aura->GetCasterGUID() == casterGUID)
            && ((negative && !aurApp->IsPositive()) || (positive && aurApp->IsPositive())))
        {
            uint32 removedAuras = m_removedAurasCount;
            RemoveAura(aurApp);
            if (m_removedAurasCount > removedAuras + 1)
                iter = m_modAuras [auraType].begin();
        }
    }
}

void Unit::RemoveAurasByTypeOnImmunity(AuraType auraType)
{
    for (auto itr = m_modAuras[auraType].begin(); itr != m_modAuras[auraType].end();)
    {
        Aura* aura = (*itr)->GetBase();
        AuraApplication* aurApp = aura->GetApplicationOfTarget(GetGUID());
        ++itr;
        // Apparently, we shouldn't remove root auras by type
        if (aura->GetSpellInfo()->Id == 142913) // Displaced Energy
        {
            aura->RemoveEffect((*itr)->GetEffIndex());
            continue;
        }
        if (!aura->GetSpellInfo()->HasAttribute(SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY))
        {
            uint32 removedAuras = m_removedAurasCount;
            RemoveAura(aurApp);
            if (m_removedAurasCount > removedAuras + 1)
                itr = m_modAuras[auraType].begin();
        }
    }
}

void Unit::RemoveAurasWithAttribute(uint32 flags)
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        SpellInfo const* spell = iter->second->GetBase()->GetSpellInfo();
        if (spell->Attributes & flags)
            RemoveAura(iter);
        else
            ++iter;
    }
}

void Unit::RemoveBoundAuras(uint32 newPhase, bool phaseid)
{
    // Auras from other casters.
    for (auto it = m_appliedAuras.begin(); it != m_appliedAuras.end();)
    {
        Aura* aura = it->second->GetBase();
        if (aura->GetCasterGUID() != GetGUID() && aura->IsBoundToCaster())
        {
            Unit* caster = aura->GetCaster();
            ASSERT(caster);
            if (!newPhase && !phaseid || newPhase && !caster->InSamePhase(newPhase) || (!newPhase && !caster->IsPhased(this)))
            {
                caster->UnbindAura(aura);
                if (aura->GetSpellInfo()->IsSingleTarget())
                {
                    RemoveAura(it);
                    continue;
                }
            }
        }
        ++it;
    }

    // Auras we are caster of which
    for (auto it = m_boundAuras.begin(); it != m_boundAuras.end();)
    {
        auto& auras = it->second;
        for (auto it2 = auras.begin(); it2 != auras.end();)
        {
            Aura* aura = *it2;
            if ((!newPhase || aura->GetOwner()->InSamePhase(newPhase)) && aura->GetOwner() != this)
            {
                auras.erase(it2++);
                aura->UnbindFromCaster();
                if (aura->GetSpellInfo()->IsSingleTarget())
                {
                    aura->Remove();
                    it2 = auras.begin();     // Basically there are no need, but theoretically possible remove aura from another target.
                }
            }
            else
                ++it2;
        }
        if (it->second.empty())
            m_boundAuras.erase(it++);
        else
            ++it;
    }
}

void Unit::RemoveAurasWithInterruptFlags(flag64 flag, uint32 except, SpellInfo const* bySpell)
{
    if (!(m_interruptMask & flag))
        return;

    // interrupt auras
    for (AuraApplicationList::iterator iter = m_interruptableAuras.begin(); iter != m_interruptableAuras.end();)
    {
        Aura* aura = (*iter)->GetBase();
        ++iter;
        if ((aura->GetSpellInfo()->AuraInterruptFlags & flag) && (!except || aura->GetId() != except)
            && CanAuraBeInterrupted(aura, flag, bySpell))
        {
            uint32 removedAuras = m_removedAurasCount;
            RemoveAura(aura, AURA_REMOVE_BY_INTERRUPT);
            if (m_removedAurasCount > removedAuras + 1)
                iter = m_interruptableAuras.begin();
        }
    }

    // interrupt channeled spell
    if (CanChannelBeInterrupted(flag, except, bySpell))
        InterruptNonMeleeSpells(false);

    UpdateInterruptMask();
}

bool Unit::CanAuraBeInterrupted(Aura* aura, flag64 interruptFlag, SpellInfo const* bySpell)
{
    if ((interruptFlag[0] & AURA_INTERRUPT_FLAG_MOVE) && HasAuraTypeWithAffectMask(SPELL_AURA_CAST_WHILE_WALKING, aura->GetSpellInfo()))
        return false;

    if (interruptFlag[0] & (AURA_INTERRUPT_FLAG_CAST | AURA_INTERRUPT_FLAG_MELEE_ATTACK | AURA_INTERRUPT_FLAG_SPELL_ATTACK))
    {
        if (aura->GetId() == 115191) // Subterfuge. Handled in script.
            return false;
        if (aura->GetId() == 115193)  // Vanish with Subterfuge. Handled in script.
            return false;

        if (bySpell) // Camouflage
        {
            for (auto&& itr : GetAuraEffectsByType(SPELL_AURA_MOD_CAMOUFLAGE))
            {
                if (itr->GetBase() == aura)
                {
                    if (bySpell->Id == 109259) // Powershot must remove camouflage instantly
                        return true; 
                    if (bySpell->HasAttribute(SPELL_ATTR7_CONSOLIDATED_RAID_BUFF))       // Pet buffs
                        return false;
                    if (!itr->IsAffectingSpell(bySpell) && bySpell->GetCategory() != 47) // Combat States
                        return true;
                    return false;
                }
            }
        }

        if (bySpell && bySpell->Id == 51690 && aura->HasEffectType(SPELL_AURA_MOD_STEALTH)) // Killing Spree
            return false;

        if (aura->GetId() == 24450) // Prowl
            return false;

        if (aura->GetId() == 125883 && bySpell) // Zen Flight
        {
            if (bySpell->HasAttribute(SPELL_ATTR0_TRADESPELL) ||    // Any crafting
                bySpell->HasAttribute(SPELL_ATTR1_IS_FISHING) ||    // Fishing
                bySpell->HasEffect(SPELL_EFFECT_OPEN_LOCK)    ||    // Herb gathering, mining etc
                bySpell->Id == 818)                                 // Cooking
                return false;
        }
    }

    return true;
}

bool Unit::CanChannelBeInterrupted(flag64 flag, uint32 except, SpellInfo const* bySpell)
{
    Spell* spell = m_currentSpells[CURRENT_CHANNELED_SPELL];
    if (!spell || spell->getState() != SPELL_STATE_CASTING)
        return false;

    if (spell->m_spellInfo->Id == except)
        return false;

    if (spell->m_spellInfo->ChannelInterruptFlags & flag)
    {
        // Aura allows cast while movement
        if (flag[0] & AURA_INTERRUPT_FLAG_MOVE && HasAuraTypeWithAffectMask(SPELL_AURA_CAST_WHILE_WALKING, spell->GetSpellInfo()))
            return false;

        // Don't break channel if player moved due to the movement force
        if (flag[0] & AURA_INTERRUPT_FLAG_MOVE && GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasForcedMovement() && !m_movementInfo.flags)
            return false;

        // Auto shot start shouldn't break barrage
        if (flag[0] & (AURA_INTERRUPT_FLAG_CAST | AURA_INTERRUPT_FLAG_SPELL_ATTACK) && bySpell && bySpell->Id == 75 && spell->GetSpellInfo()->Id == 120360)
            return false;

        return true;
    }
    return false;
}

void Unit::RemoveAurasWithFamily(SpellFamilyNames family, uint32 familyFlag1, uint32 familyFlag2, uint32 familyFlag3, uint64 casterGUID)
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        Aura const* aura = iter->second->GetBase();
        if (!casterGUID || aura->GetCasterGUID() == casterGUID)
        {
            SpellInfo const* spell = aura->GetSpellInfo();
            if (spell->SpellFamilyName == uint32(family) && spell->SpellFamilyFlags.HasFlag(familyFlag1, familyFlag2, familyFlag3))
            {
                RemoveAura(iter);
                continue;
            }
        }
        ++iter;
    }
}

void Unit::RemoveMovementImpairingAuras(bool partial)
{
    RemoveAurasWithMechanic((1 << MECHANIC_SNARE) | (1 << MECHANIC_ROOT), AURA_REMOVE_BY_DEFAULT, 0, partial);
}

void Unit::RemoveAurasWithMechanic(uint32 mechanic_mask, AuraRemoveMode removemode, uint32 except, bool partial, bool onImmunity)
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        Aura* aura = iter->second->GetBase();

        if (onImmunity && aura->GetSpellInfo()->Id == 143385) // Just hack, I'm not shure about SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY (this spell hasn't) or anything else
        {
            ++iter;
            continue;
        }

        bool oldPartial = partial;
        if (aura->GetSpellInfo()->Id == 142913) // Displaced Energy
            partial = true;

        if (!except || aura->GetId() != except)
        {
            if (partial && aura->GetType() == UNIT_AURA_TYPE)
            {
                // For unit auras only remove the whole aura if the spell had specified mechanics. If separate effects have these mechanics - remove the associated AuraEffect
                if ((1 << aura->GetSpellInfo()->Mechanic) & mechanic_mask)
                {
                    RemoveAura(iter, removemode);
                    continue;
                }

                // Remove separate AuraEffects which have the matching mechanics
                bool removed = false;
                for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                {
                    if (aura->HasEffect(i) && iter->second->HasEffect(i) && aura->GetSpellInfo()->GetEffectMechanicMask(i) & mechanic_mask)
                    {
                        if (iter->second->GetEffectMask() == (1 << i)) // If this is the only effect of the aura - simply remove it entirely
                        {
                            RemoveAura(iter, removemode);
                            removed = true;
                            break;
                        }
                        else if (aura->GetOwner() == this) // Only remove effects if we are the owner of the aura, otherwise we'll end up removing effects for all targets
                            aura->RemoveEffect(i);
                    }
                }

                if (removed)
                    continue;

                // If all AuraEffects were removed - remove the aura itself
                if (!aura->GetEffectMask())
                {
                    RemoveAura(iter, removemode);
                    continue;
                }
            }
            else
            {
                // For dynobj auras remove the whole aura if either spell or any of its effects had specified mechanics. If we remove AuraEffects, the dynobj will linger as a visual doing nothing.
                if (aura->GetSpellInfo()->GetAllEffectsMechanicMask() & mechanic_mask)
                {
                    RemoveAura(iter, removemode);
                    continue;
                }
            }
        }
        ++iter;

        partial = oldPartial;
    }
}

void Unit::RemoveAurasWithMechanicOnImmunity(uint32 mechanicMask, uint32 except, bool partial)
{
    RemoveAurasWithMechanic(mechanicMask, AURA_REMOVE_BY_DEFAULT, except, partial, true);
}

void Unit::RemoveAreaAurasDueToLeaveWorld()
{
    // make sure that all area auras not applied on self are removed - prevent access to deleted pointer later
    for (AuraMap::iterator iter = m_ownedAuras.begin(); iter != m_ownedAuras.end();)
    {
        Aura* aura = iter->second;
        ++iter;
        Aura::ApplicationMap const& appMap = aura->GetApplicationMap();
        for (Aura::ApplicationMap::const_iterator itr = appMap.begin(); itr != appMap.end();)
        {
            AuraApplication * aurApp = itr->second;
            ++itr;
            Unit* target = aurApp->GetTarget();
            if (target == this)
                continue;
            target->RemoveAura(aurApp);
            // things linked on aura remove may apply new area aura - so start from the beginning
            iter = m_ownedAuras.begin();
        }
    }

    // remove area auras owned by others
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        if (iter->second->GetBase()->GetOwner() != this)
        {
            RemoveAura(iter);
        }
        else
            ++iter;
    }
}

void Unit::RemoveAllAuras()
{
    // this may be a dead loop if some events on aura remove will continiously apply aura on remove
    // we want to have all auras removed, so use your brain when linking events
    while (!m_appliedAuras.empty() || !m_ownedAuras.empty())
    {
        AuraApplicationMap::iterator aurAppIter;
        for (aurAppIter = m_appliedAuras.begin(); aurAppIter != m_appliedAuras.end();)
            _UnapplyAura(aurAppIter, AURA_REMOVE_BY_DEFAULT);

        AuraMap::iterator aurIter;
        for (aurIter = m_ownedAuras.begin(); aurIter != m_ownedAuras.end();)
            RemoveOwnedAura(aurIter);
    }

    if (IsInWorld() && IsAlive())
        if (Creature* creature = ToCreature())
            creature->ApplyInstanceAuraIfNeeded();
}

void Unit::RemoveArenaAuras()
{
    // in join, remove positive buffs, on end, remove negative
    // used to remove positive visible auras in arenas
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        AuraApplication const* aurApp = iter->second;
        Aura const* aura = aurApp->GetBase();
        if (!(aura->GetSpellInfo()->AttributesEx4 & SPELL_ATTR4_UNK21) // don't remove stances, shadowform, pally/hunter auras
            && !aura->IsPassive()                               // don't remove passive auras
            && !aura->GetSpellInfo()->HasAttribute(SPELL_ATTR0_CU_PERSISTENT)
            && (aurApp->IsPositive() || !(aura->GetSpellInfo()->AttributesEx3 & SPELL_ATTR3_DEATH_PERSISTENT))) // not negative death persistent auras
            RemoveAura(iter);
        else
            ++iter;
    }

    // Demonic Gateway cooldown
    RemoveAurasDueToSpell(113942);
}

void Unit::RemoveBattlegroundStartingAuras()
{
    // remove auras with duration lower than 30s
    AuraApplicationMap& auraMap = GetAppliedAuras();
    for (auto iter = auraMap.begin(); iter != auraMap.end();)
    {
        AuraApplication* aurApp = iter->second;
        Aura* aura = aurApp->GetBase();
        if (!aura->IsPermanent() && aurApp->IsPositive() &&
            (aura->GetDuration() <= 30 * IN_MILLISECONDS || !aura->GetCaster()) &&
            !aura->GetSpellInfo()->HasAttribute(SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY) &&
            !aura->HasEffectType(SPELL_AURA_MOD_INVISIBILITY) &&
            aura->GetSpellInfo()->Id != 114018) // Shroud of Concealment, triggers SPELL_AURA_MOD_INVISIBILITY
        {
            RemoveAura(iter);
        }
        else
        {
            if (aura->GetSpellInfo()->Id == 125195 || // Tigereye Brew
                aura->GetSpellInfo()->Id == 115867 || // Mana Tea
                aura->GetSpellInfo()->Id == 128939)   // Elusive Brew
                RemoveAura(iter);
            else
                ++iter;
        }
    }
}

void Unit::RemoveAllAurasOnDeath()
{
    // used just after dieing to remove all visible auras
    // and disable the mods for the passive ones
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        Aura const* aura = iter->second->GetBase();
        if (!aura->IsPassive() && !aura->IsDeathPersistent())
            _UnapplyAura(iter, AURA_REMOVE_BY_DEATH);
        else
            ++iter;
    }

    for (AuraMap::iterator iter = m_ownedAuras.begin(); iter != m_ownedAuras.end();)
    {
        Aura* aura = iter->second;
        if (!aura->IsPassive() && !aura->IsDeathPersistent())
            RemoveOwnedAura(iter, AURA_REMOVE_BY_DEATH);
        else
            ++iter;
    }
}

void Unit::RemoveAllAurasRequiringDeadTarget()
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        Aura const* aura = iter->second->GetBase();
        if (!aura->IsPassive() && aura->GetSpellInfo()->IsRequiringDeadTarget())
            _UnapplyAura(iter, AURA_REMOVE_BY_DEFAULT);
        else
            ++iter;
    }

    for (AuraMap::iterator iter = m_ownedAuras.begin(); iter != m_ownedAuras.end();)
    {
        Aura* aura = iter->second;
        if (!aura->IsPassive() && aura->GetSpellInfo()->IsRequiringDeadTarget())
            RemoveOwnedAura(iter, AURA_REMOVE_BY_DEFAULT);
        else
            ++iter;
    }
}

void Unit::RemoveAllAurasExceptType(AuraType type)
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        Aura const* aura = iter->second->GetBase();
        if (aura->GetSpellInfo()->HasAura(type))
            ++iter;
        else
            _UnapplyAura(iter, AURA_REMOVE_BY_DEFAULT);
    }

    for (AuraMap::iterator iter = m_ownedAuras.begin(); iter != m_ownedAuras.end();)
    {
        Aura* aura = iter->second;
        if (aura->GetSpellInfo()->HasAura(type))
            ++iter;
        else
            RemoveOwnedAura(iter, AURA_REMOVE_BY_DEFAULT);
    }
}

void Unit::RemoveAllAurasExceptType(AuraType type1, AuraType type2)
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        Aura const* aura = iter->second->GetBase();
        if (aura->GetSpellInfo()->HasAura(type1) || aura->GetSpellInfo()->HasAura(type2))
            ++iter;
        else
            _UnapplyAura(iter, AURA_REMOVE_BY_DEFAULT);
    }

    for (AuraMap::iterator iter = m_ownedAuras.begin(); iter != m_ownedAuras.end();)
    {
        Aura* aura = iter->second;
        if (aura->GetSpellInfo()->HasAura(type1) || aura->GetSpellInfo()->HasAura(type2))
            ++iter;
        else
            RemoveOwnedAura(iter, AURA_REMOVE_BY_DEFAULT);
    }
}

void Unit::DelayOwnedAuras(uint32 spellId, uint64 caster, int32 delaytime)
{
    AuraMapBoundsNonConst range = m_ownedAuras.equal_range(spellId);
    for (; range.first != range.second; ++range.first)
    {
        Aura* aura = range.first->second;
        if (!caster || aura->GetCasterGUID() == caster)
        {
            if (delaytime < 0)
            {
                if (aura->GetDuration() + delaytime <= 0)
                    aura->SetDuration(0);
                else
                    aura->SetDuration(aura->GetDuration() + delaytime);
            }
            else
            {
                aura->SetMaxDuration(aura->GetMaxDuration() + delaytime);
                aura->SetDuration(aura->GetDuration() + delaytime);

                for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                    if (AuraEffect* effect = aura->GetEffect(i))
                        if (effect->IsPeriodic())
                            effect->SetPeriodicTimer(effect->GetPeriodicTimer() + delaytime);
            }

            // update for out of range group members (on 1 slot use)
            aura->SetNeedClientUpdateForTargets();
            TC_LOG_DEBUG("spells", "Aura %u partially interrupted on unit %u, new duration: %u ms", aura->GetId(), GetGUID().GetCounter(), aura->GetDuration());
        }
    }
}

void Unit::_RemoveAllAuraStatMods()
{
    for (AuraApplicationMap::iterator i = m_appliedAuras.begin(); i != m_appliedAuras.end(); ++i)
        (*i).second->GetBase()->HandleAllEffects(i->second, AURA_EFFECT_HANDLE_STAT, false);
}

void Unit::_ApplyAllAuraStatMods()
{
    for (AuraApplicationMap::iterator i = m_appliedAuras.begin(); i != m_appliedAuras.end(); ++i)
        (*i).second->GetBase()->HandleAllEffects(i->second, AURA_EFFECT_HANDLE_STAT, true);
}

void Unit::BindAura(Aura* aura)
{
    if (aura->IsRemoved() || aura->IsBoundToCaster())
        return;
    m_boundAuras[aura->GetId()].push_back(aura);
    aura->BindToCaster();
}

void Unit::UnbindAura(Aura* aura)
{
    auto listit = m_boundAuras.find(aura->GetId());
    if (listit == m_boundAuras.end())
    {
        TC_LOG_ERROR("shitlog", "Unit::UnbindAura listit == m_boundAuras.end() aura: %u, caster: " UI64FMTD " (entry: %u)\n" , aura->GetId(), GetGUID().GetRawValue(), GetEntry());
        return;
    }
    auto& list = m_boundAuras[aura->GetId()];
    auto it = std::find(list.begin(), list.end(), aura);
    if (it == list.end())
    {
        TC_LOG_ERROR("shitlog", "Unit::UnbindAura it == list.end() aura: %u, caster: " UI64FMTD " (entry: %u)\n", aura->GetId(), GetGUID().GetRawValue(), GetEntry());
        return;
    }
    list.erase(it);
    aura->UnbindFromCaster();
}

AuraEffect* Unit::GetAuraEffect(uint32 spellId, uint8 effIndex, uint64 caster) const
{
    AuraApplicationMapBounds range = m_appliedAuras.equal_range(spellId);
    for (AuraApplicationMap::const_iterator itr = range.first; itr != range.second; ++itr)
    {
        if (itr->second->HasEffect(effIndex)
            && (!caster || itr->second->GetBase()->GetCasterGUID() == caster))
        {
            return itr->second->GetBase()->GetEffect(effIndex);
        }
    }
    return NULL;
}

AuraEffect* Unit::GetAuraEffectOfRankedSpell(uint32 spellId, uint8 effIndex, uint64 caster) const
{
    uint32 rankSpell = sSpellMgr->GetFirstSpellInChain(spellId);
    while (rankSpell)
    {
        if (AuraEffect* aurEff = GetAuraEffect(rankSpell, effIndex, caster))
            return aurEff;
        rankSpell = sSpellMgr->GetNextSpellInChain(rankSpell);
    }
    return NULL;
}

AuraEffect* Unit::GetAuraEffect(AuraType type, SpellFamilyNames name, uint32 iconId, uint8 effIndex) const
{
    AuraEffectList const& auras = GetAuraEffectsByType(type);
    for (Unit::AuraEffectList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
    {
        if (effIndex != (*itr)->GetEffIndex())
            continue;
        SpellInfo const* spell = (*itr)->GetSpellInfo();
        if (spell->SpellIconID == iconId && spell->SpellFamilyName == uint32(name) && !spell->SpellFamilyFlags)
            return *itr;
    }
    return NULL;
}

AuraEffect* Unit::GetAuraEffect(AuraType type, SpellFamilyNames family, flag128 const& flags, uint64 casterGUID) const
{
    AuraEffectList const& auras = GetAuraEffectsByType(type);
    for (AuraEffectList::const_iterator i = auras.begin(); i != auras.end(); ++i)
    {
        SpellInfo const* spell = (*i)->GetSpellInfo();
        if (spell->SpellFamilyName == uint32(family) && spell->SpellFamilyFlags & flags)
        {
            if (casterGUID && (*i)->GetCasterGUID() != casterGUID)
                continue;
            return (*i);
        }
    }
    return NULL;
}

AuraEffect* Unit::GetDummyAuraEffect(SpellFamilyNames name, uint32 iconId, uint8 effIndex) const
{
    return GetAuraEffect(SPELL_AURA_DUMMY, name, iconId, effIndex);
}

AuraApplication * Unit::GetAuraApplication(uint32 spellId, uint64 casterGUID, uint64 itemCasterGUID, uint32 reqEffMask, AuraApplication * except) const
{
    AuraApplicationMapBounds range = m_appliedAuras.equal_range(spellId);
    for (; range.first != range.second; ++range.first)
    {
        AuraApplication* app = range.first->second;
        Aura const* aura = app->GetBase();

        if (((aura->GetEffectMask() & reqEffMask) == reqEffMask)
            && (!casterGUID || aura->GetCasterGUID() == casterGUID)
            && (!itemCasterGUID || aura->GetCastItemGUID() == itemCasterGUID)
            && (!except || except != app))
        {
            return app;
        }
    }
    return NULL;
}

Aura* Unit::GetAura(uint32 spellId, uint64 casterGUID, uint64 itemCasterGUID, uint32 reqEffMask) const
{
    AuraApplication * aurApp = GetAuraApplication(spellId, casterGUID, itemCasterGUID, reqEffMask);
    return aurApp ? aurApp->GetBase() : NULL;
}

AuraApplication * Unit::GetAuraApplicationOfRankedSpell(uint32 spellId, uint64 casterGUID, uint64 itemCasterGUID, uint32 reqEffMask, AuraApplication* except) const
{
    uint32 rankSpell = sSpellMgr->GetFirstSpellInChain(spellId);
    while (rankSpell)
    {
        if (AuraApplication * aurApp = GetAuraApplication(rankSpell, casterGUID, itemCasterGUID, reqEffMask, except))
            return aurApp;
        rankSpell = sSpellMgr->GetNextSpellInChain(rankSpell);
    }
    return NULL;
}

Aura* Unit::GetAuraOfRankedSpell(uint32 spellId, uint64 casterGUID, uint64 itemCasterGUID, uint32 reqEffMask) const
{
    AuraApplication * aurApp = GetAuraApplicationOfRankedSpell(spellId, casterGUID, itemCasterGUID, reqEffMask);
    return aurApp ? aurApp->GetBase() : NULL;
}

void Unit::GetDispellableAuraList(Unit* caster, uint32 dispelMask, DispelChargesList& dispelList)
{
    // we should not be able to dispel diseases if the target is affected by unholy blight
    if (dispelMask & (1 << DISPEL_DISEASE) && HasAura(50536))
        dispelMask &= ~(1 << DISPEL_DISEASE);

    AuraMap const& auras = GetOwnedAuras();
    for (AuraMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
    {
        Aura* aura = itr->second;
        AuraApplication * aurApp = aura->GetApplicationOfTarget(GetGUID());
        if (!aurApp)
            continue;

        // don't try to remove passive auras
        if (aura->IsPassive())
            continue;

        if (aura->GetSpellInfo()->GetDispelMask() & dispelMask)
        {
            if (aura->GetSpellInfo()->Dispel == DISPEL_MAGIC)
            {
                // do not remove positive auras if friendly target
                //               negative auras if non-friendly target
                if (aurApp->IsPositive() == IsFriendlyTo(caster))
                    continue;
            }

            // The charges / stack amounts don't count towards the total number of auras that can be dispelled.
            // Ie: A dispel on a target with 5 stacks of Winters Chill and a Polymorph has 1 / (1 + 1) -> 50% chance to dispell
            // Polymorph instead of 1 / (5 + 1) -> 16%.
            bool dispel_charges = aura->GetSpellInfo()->AttributesEx7 & SPELL_ATTR7_DISPEL_CHARGES;
            uint8 charges = dispel_charges ? aura->GetCharges() : aura->GetStackAmount();
            if (charges > 0)
                dispelList.push_back(std::make_pair(aura, charges));
        }
    }
}

bool Unit::HasAuraEffect(uint32 spellId, uint8 effIndex, uint64 caster) const
{
    AuraApplicationMapBounds range = m_appliedAuras.equal_range(spellId);
    for (AuraApplicationMap::const_iterator itr = range.first; itr != range.second; ++itr)
    {
        if (itr->second->HasEffect(effIndex)
            && (!caster || itr->second->GetBase()->GetCasterGUID() == caster))
        {
            return true;
        }
    }
    return false;
}

uint32 Unit::GetAuraCount(uint32 spellId) const
{
    uint32 count = 0;
    AuraApplicationMapBounds range = m_appliedAuras.equal_range(spellId);

    for (AuraApplicationMap::const_iterator itr = range.first; itr != range.second; ++itr)
    {
        if (itr->second->GetBase()->GetStackAmount() == 0)
            ++count;
        else
            count += (uint32) itr->second->GetBase()->GetStackAmount();
    }

    return count;
}

bool Unit::HasAura(uint32 spellId, uint64 casterGUID, uint64 itemCasterGUID, uint32 reqEffMask) const
{
    if (GetAuraApplication(spellId, casterGUID, itemCasterGUID, reqEffMask))
        return true;
    return false;
}

bool Unit::HasAuraType(AuraType auraType) const
{
    return (!m_modAuras [auraType].empty());
}

bool Unit::HasAuraTypeWithCaster(AuraType auratype, uint64 caster) const
{
    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        if (caster == (*i)->GetCasterGUID())
            return true;
    return false;
}

bool Unit::HasAuraTypeWithMiscvalue(AuraType auratype, int32 miscvalue) const
{
    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        if (miscvalue == (*i)->GetMiscValue())
            return true;
    return false;
}

bool Unit::HasAuraTypeWithAffectMask(AuraType auratype, SpellInfo const* affectedSpell) const
{
    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        if ((*i)->IsAffectingSpell(affectedSpell))
            return true;
    return false;
}

bool Unit::HasAuraTypeWithValue(AuraType auratype, int32 value) const
{
    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        if (value == (*i)->GetAmount())
            return true;
    return false;
}

bool Unit::HasNegativeAuraWithInterruptFlag(SpellAuraInterruptFlags flag, uint64 guid) const
{
    if (!(m_interruptMask[0] & flag))
        return false;
    for (AuraApplicationList::const_iterator iter = m_interruptableAuras.begin(); iter != m_interruptableAuras.end(); ++iter)
    {
        if (!(*iter)->IsPositive() && (*iter)->GetBase()->GetSpellInfo()->HasAuraInterruptFlag(flag) && (!guid || (*iter)->GetBase()->GetCasterGUID() == guid))
            return true;
    }
    return false;
}

bool Unit::HasNegativeAuraWithAttribute(uint32 flag, uint64 guid) const
{
    for (AuraApplicationMap::const_iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end(); ++iter)
    {
        Aura const* aura = iter->second->GetBase();
        if (!iter->second->IsPositive() && aura->GetSpellInfo()->Attributes & flag && (!guid || aura->GetCasterGUID() == guid))
            return true;
    }
    return false;
}

bool Unit::HasAuraWithMechanic(uint32 mechanicMask) const
{
    for (AuraApplicationMap::const_iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end(); ++iter)
    {
        SpellInfo const* spellInfo = iter->second->GetBase()->GetSpellInfo();
        if (spellInfo->Mechanic && (mechanicMask & (1 << spellInfo->Mechanic)))
            return true;

        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            if (iter->second->HasEffect(i) && spellInfo->Effects [i].Effect && spellInfo->Effects [i].Mechanic)
                if (mechanicMask & (1 << spellInfo->Effects [i].Mechanic))
                    return true;
    }

    return false;
}

AuraEffect* Unit::IsScriptOverriden(SpellInfo const* spell, int32 script) const
{
    AuraEffectList const& auras = GetAuraEffectsByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    for (AuraEffectList::const_iterator i = auras.begin(); i != auras.end(); ++i)
    {
        if ((*i)->GetMiscValue() == script)
            if ((*i)->IsAffectingSpell(spell))
                return (*i);
    }
    return NULL;
}

uint32 Unit::GetDiseasesByCaster(uint64 casterGUID, bool remove)
{
    static const AuraType diseaseAuraTypes [] =
    {
        SPELL_AURA_PERIODIC_DAMAGE, // Frost Fever and Blood Plague
        SPELL_AURA_LINKED,          // Crypt Fever and Ebon Plague
        SPELL_AURA_NONE
    };

    uint32 diseases = 0;
    for (AuraType const* itr = diseaseAuraTypes; *itr != SPELL_AURA_NONE; ++itr)
    {
        for (AuraEffectList::iterator i = m_modAuras [*itr].begin(); i != m_modAuras [*itr].end();)
        {
            // Get auras with disease dispel type by caster
            if ((*i)->GetSpellInfo()->Dispel == DISPEL_DISEASE
                && (*i)->GetCasterGUID() == casterGUID)
            {
                ++diseases;

                if (remove)
                {
                    RemoveAura((*i)->GetId(), (*i)->GetCasterGUID());
                    i = m_modAuras [*itr].begin();
                    continue;
                }
            }
            ++i;
        }
    }
    return diseases;
}

uint32 Unit::GetDoTsByCaster(uint64 casterGUID) const
{
    static const AuraType diseaseAuraTypes [] =
    {
        SPELL_AURA_PERIODIC_DAMAGE,
        SPELL_AURA_PERIODIC_DAMAGE_PERCENT,
        SPELL_AURA_NONE
    };

    uint32 dots = 0;
    for (AuraType const* itr = &diseaseAuraTypes [0]; itr && itr [0] != SPELL_AURA_NONE; ++itr)
    {
        Unit::AuraEffectList const& auras = GetAuraEffectsByType(*itr);
        for (AuraEffectList::const_iterator i = auras.begin(); i != auras.end(); ++i)
        {
            // Get auras by caster
            if ((*i)->GetCasterGUID() == casterGUID)
                ++dots;
        }
    }
    return dots;
}

int32 Unit::GetTotalAuraModifier(AuraType auratype) const
{
    std::map<SpellGroup, int32> sameEffectSpellGroup;
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (auto&& it : GetAuraEffectsByType(auratype))
        if (!sSpellMgr->AddSameEffectStackRuleSpellGroups(it->GetSpellInfo(), it->GetAmount(), sameEffectSpellGroup))
            modifier += it->GetAmount();

    for (auto&& it : sameEffectSpellGroup)
        modifier += it.second;

    return modifier;
}

float Unit::GetTotalAuraMultiplier(AuraType auratype) const
{
    std::map<SpellGroup, int32> sameEffectSpellGroup;
    float multiplier = 1.0f;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (auto&& it : GetAuraEffectsByType(auratype))
        if (!sSpellMgr->AddSameEffectStackRuleSpellGroups(it->GetSpellInfo(), it->GetAmount(), sameEffectSpellGroup))
            AddPct(multiplier, it->GetAmount());

    for (auto&& it : sameEffectSpellGroup)
        AddPct(multiplier, it.second);

    return multiplier;
}

int32 Unit::GetMaxPositiveAuraModifier(AuraType auratype) const
{
    int32 modifier = 0;

    Battleground* bg = GetBattlegorund();

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (auto&& itr : mTotalAuraList)
    {
        if (bg && itr->GetSpellInfo()->CheckArenaAndBattlegroundCastRules(bg) != SPELL_CAST_OK)
            continue;

        if (itr->GetAmount() > modifier)
            modifier = itr->GetAmount();
    }

    return modifier;
}

int32 Unit::GetMaxNegativeAuraModifier(AuraType auratype) const
{
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        if ((*i)->GetAmount() < modifier)
            modifier = (*i)->GetAmount();

    return modifier;
}

int32 Unit::GetTotalAuraModifierByMiscMask(AuraType auratype, uint32 miscMask) const
{
    std::map<SpellGroup, int32> SameEffectSpellGroup;
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);

    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        if ((*i)->GetMiscValue() & miscMask)
            if (!sSpellMgr->AddSameEffectStackRuleSpellGroups((*i)->GetSpellInfo(), (*i)->GetAmount(), SameEffectSpellGroup))
                modifier += (*i)->GetAmount();

    for (std::map<SpellGroup, int32>::const_iterator itr = SameEffectSpellGroup.begin(); itr != SameEffectSpellGroup.end(); ++itr)
        modifier += itr->second;

    return modifier;
}

float Unit::GetTotalAuraMultiplierByMiscMask(AuraType auratype, uint32 miscMask) const
{
    std::map<SpellGroup, int32> SameEffectSpellGroup;
    float multiplier = 1.0f;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if (((*i)->GetMiscValue() & miscMask))
        {
            // Check if the Aura Effect has a the Same Effect Stack Rule and if so, use the highest amount of that SpellGroup
            // If the Aura Effect does not have this Stack Rule, it returns false so we can add to the multiplier as usual
            if (!sSpellMgr->AddSameEffectStackRuleSpellGroups((*i)->GetSpellInfo(), (*i)->GetAmount(), SameEffectSpellGroup))
                AddPct(multiplier, (*i)->GetAmount());
        }
    }
    // Add the highest of the Same Effect Stack Rule SpellGroups to the multiplier
    for (std::map<SpellGroup, int32>::const_iterator itr = SameEffectSpellGroup.begin(); itr != SameEffectSpellGroup.end(); ++itr)
        AddPct(multiplier, itr->second);

    return multiplier;
}

int32 Unit::GetMaxPositiveAuraModifierByMiscMask(AuraType auratype, uint32 miscMask, const AuraEffect* except) const
{
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if (except != (*i) && (*i)->GetMiscValue()& miscMask && (*i)->GetAmount() > modifier)
            modifier = (*i)->GetAmount();
    }

    return modifier;
}

int32 Unit::GetMaxNegativeAuraModifierByMiscMask(AuraType auratype, uint32 miscMask) const
{
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->GetMiscValue()& miscMask && (*i)->GetAmount() < modifier)
            modifier = (*i)->GetAmount();
    }

    return modifier;
}

int32 Unit::GetTotalAuraModifierByMiscValue(AuraType auratype, int32 miscValue) const
{
    std::map<SpellGroup, int32> SameEffectSpellGroup;
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->GetMiscValue() == miscValue)
            if (!sSpellMgr->AddSameEffectStackRuleSpellGroups((*i)->GetSpellInfo(), (*i)->GetAmount(), SameEffectSpellGroup))
                modifier += (*i)->GetAmount();
    }

    for (std::map<SpellGroup, int32>::const_iterator itr = SameEffectSpellGroup.begin(); itr != SameEffectSpellGroup.end(); ++itr)
        modifier += itr->second;

    return modifier;
}

float Unit::GetTotalAuraMultiplierByMiscValue(AuraType auratype, int32 miscValue) const
{
    std::map<SpellGroup, int32> SameEffectSpellGroup;
    float multiplier = 1.0f;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->GetMiscValue() == miscValue)
            if (!sSpellMgr->AddSameEffectStackRuleSpellGroups((*i)->GetSpellInfo(), (*i)->GetAmount(), SameEffectSpellGroup))
                AddPct(multiplier, (*i)->GetAmount());
    }

    for (std::map<SpellGroup, int32>::const_iterator itr = SameEffectSpellGroup.begin(); itr != SameEffectSpellGroup.end(); ++itr)
        AddPct(multiplier, itr->second);

    return multiplier;
}

int32 Unit::GetMaxPositiveAuraModifierByMiscValue(AuraType auratype, int32 miscValue) const
{
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->GetMiscValue() == miscValue && (*i)->GetAmount() > modifier)
            modifier = (*i)->GetAmount();
    }

    return modifier;
}

int32 Unit::GetMaxNegativeAuraModifierByMiscValue(AuraType auratype, int32 miscValue) const
{
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->GetMiscValue() == miscValue && (*i)->GetAmount() < modifier)
            modifier = (*i)->GetAmount();
    }

    return modifier;
}

int32 Unit::GetTotalAuraModifierByAffectMask(AuraType auratype, SpellInfo const* affectedSpell) const
{
    std::map<SpellGroup, int32> SameEffectSpellGroup;
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->IsAffectingSpell(affectedSpell))
            if (!sSpellMgr->AddSameEffectStackRuleSpellGroups((*i)->GetSpellInfo(), (*i)->GetAmount(), SameEffectSpellGroup))
                modifier += (*i)->GetAmount();
    }

    for (std::map<SpellGroup, int32>::const_iterator itr = SameEffectSpellGroup.begin(); itr != SameEffectSpellGroup.end(); ++itr)
        modifier += itr->second;

    return modifier;
}

float Unit::GetTotalAuraMultiplierByAffectMask(AuraType auratype, SpellInfo const* affectedSpell) const
{
    std::map<SpellGroup, int32> SameEffectSpellGroup;
    float multiplier = 1.0f;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->IsAffectingSpell(affectedSpell))
            if (!sSpellMgr->AddSameEffectStackRuleSpellGroups((*i)->GetSpellInfo(), (*i)->GetAmount(), SameEffectSpellGroup))
                AddPct(multiplier, (*i)->GetAmount());
    }

    for (std::map<SpellGroup, int32>::const_iterator itr = SameEffectSpellGroup.begin(); itr != SameEffectSpellGroup.end(); ++itr)
        AddPct(multiplier, itr->second);

    return multiplier;
}

int32 Unit::GetMaxPositiveAuraModifierByAffectMask(AuraType auratype, SpellInfo const* affectedSpell) const
{
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->IsAffectingSpell(affectedSpell) && (*i)->GetAmount() > modifier)
            modifier = (*i)->GetAmount();
    }

    return modifier;
}

int32 Unit::GetMaxNegativeAuraModifierByAffectMask(AuraType auratype, SpellInfo const* affectedSpell) const
{
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->IsAffectingSpell(affectedSpell) && (*i)->GetAmount() < modifier)
            modifier = (*i)->GetAmount();
    }

    return modifier;
}

float Unit::GetTotalHaseMultiplier(AuraType auraType) const
{
    std::map<SpellGroup, int32> sameEffectSpellGroup;
    float positive = 1.0f;

    for (auto&& it : GetAuraEffectsByType(auraType))
        if (it->GetAmount() > 0)
            if (!sSpellMgr->AddSameEffectStackRuleSpellGroups(it->GetSpellInfo(), it->GetAmount(), sameEffectSpellGroup))
                AddPct(positive, it->GetAmount());

    for (auto&& it : sameEffectSpellGroup)
        AddPct(positive, it.second);

    sameEffectSpellGroup.clear();

    float negative = 1.0f;
    for (auto&& it : GetAuraEffectsByType(auraType))
        if (it->GetAmount() < 0)
            if (!sSpellMgr->AddSameEffectStackRuleSpellGroups(it->GetSpellInfo(), std::abs(it->GetAmount()), sameEffectSpellGroup))
                AddPct(negative, std::abs(it->GetAmount()));

    for (auto&& it : sameEffectSpellGroup)
        AddPct(negative, it.second);

    return positive / negative;
}

float Unit::GetResistanceBuffMods(SpellSchools school, bool positive) const
{
    return GetFloatValue(positive ? UNIT_FIELD_RESISTANCE_BUFF_MODS_POSITIVE + school : UNIT_FIELD_RESISTANCE_BUFF_MODS_NEGATIVE + school);
}

void Unit::SetResistanceBuffMods(SpellSchools school, bool positive, float val)
{
    SetFloatValue(positive ? UNIT_FIELD_RESISTANCE_BUFF_MODS_POSITIVE + school : UNIT_FIELD_RESISTANCE_BUFF_MODS_NEGATIVE + school, val);
}

void Unit::ApplyResistanceBuffModsMod(SpellSchools school, bool positive, float val, bool apply)
{
    ApplyModSignedFloatValue(positive ? UNIT_FIELD_RESISTANCE_BUFF_MODS_POSITIVE + school : UNIT_FIELD_RESISTANCE_BUFF_MODS_NEGATIVE + school, val, apply);
}

void Unit::ApplyResistanceBuffModsPercentMod(SpellSchools school, bool positive, float val, bool apply)
{
    ApplyPercentModFloatValue(positive ? UNIT_FIELD_RESISTANCE_BUFF_MODS_POSITIVE + school : UNIT_FIELD_RESISTANCE_BUFF_MODS_NEGATIVE + school, val, apply);
}

void Unit::InitStatBuffMods()
{
    for (uint8 i = STAT_STRENGTH; i < MAX_STATS; ++i)
        SetFloatValue(UNIT_FIELD_STAT_POS_BUFF + i, 0);
    for (uint8 i = STAT_STRENGTH; i < MAX_STATS; ++i)
        SetFloatValue(UNIT_FIELD_STAT_NEG_BUFF + i, 0);
}

void Unit::ApplyStatBuffMod(Stats stat, float val, bool apply)
{
    ApplyModSignedFloatValue((val > 0 ? UNIT_FIELD_STAT_POS_BUFF + stat : UNIT_FIELD_STAT_NEG_BUFF + stat), val, apply);
}

void Unit::ApplyStatPercentBuffMod(Stats stat, float val, bool apply)
{
    ApplyPercentModFloatValue(UNIT_FIELD_STAT_POS_BUFF + stat, val, apply);
    ApplyPercentModFloatValue(UNIT_FIELD_STAT_NEG_BUFF + stat, val, apply);
}

void Unit::GetSummons(std::list<TempSummon*>& list, uint32 entry)
{
    for (auto&& summon : m_summons)
        if (summon->GetEntry() == entry)
            list.push_back(summon);
}

void Unit::_RegisterDynObject(DynamicObject* dynObj)
{
    m_dynObj.push_back(dynObj);
}

void Unit::RegisterAreaTrigger(AreaTrigger* areaTrigger)
{
    m_areaTrigger.push_back(areaTrigger);
}

void Unit::_UnregisterDynObject(DynamicObject* dynObj)
{
    m_dynObj.remove(dynObj);
}

void Unit::UnregisterAreaTrigger(AreaTrigger* areaTrigger)
{
    m_areaTrigger.remove(areaTrigger);
}

DynamicObject* Unit::GetDynObject(uint32 spellId)
{
    if (m_dynObj.empty())
        return NULL;
    for (DynObjectList::const_iterator i = m_dynObj.begin(); i != m_dynObj.end(); ++i)
    {
        DynamicObject* dynObj = *i;
        if (dynObj->GetSpellId() == spellId)
            return dynObj;
    }
    return NULL;
}

AreaTrigger* Unit::GetAreaTrigger(uint32 spellId)
{
    if (m_areaTrigger.empty())
        return NULL;
    for (AreaTriggerList::const_iterator i = m_areaTrigger.begin(); i != m_areaTrigger.end(); ++i)
    {
        AreaTrigger* areaTrigger = *i;
        if (areaTrigger->GetSpellId() == spellId)
            return areaTrigger;
    }
    return NULL;
}

int32 Unit::CountDynObject(uint32 spellId)
{
    int32 count = 0;

    if (m_dynObj.empty())
        return 0;
    for (DynObjectList::const_iterator i = m_dynObj.begin(); i != m_dynObj.end(); ++i)
    {
        DynamicObject* dynObj = *i;
        if (dynObj->GetSpellId() == spellId)
            count++;
    }
    return count;
}

int32 Unit::CountAreaTrigger(uint32 spellId)
{
    int32 count = 0;

    if (m_areaTrigger.empty())
        return 0;
    for (AreaTriggerList::const_iterator i = m_areaTrigger.begin(); i != m_areaTrigger.end(); ++i)
    {
        AreaTrigger* areaTrigger = *i;
        if (areaTrigger->GetSpellId() == spellId)
            count++;
    }
    return count;
}

void Unit::GetDynObjectList(std::list<DynamicObject*> &list, uint32 spellId)
{
    if (m_dynObj.empty())
        return;
    for (DynObjectList::const_iterator i = m_dynObj.begin(); i != m_dynObj.end(); ++i)
    {
        DynamicObject* dynObj = *i;
        if (dynObj->GetSpellId() == spellId)
            list.push_back(dynObj);
    }
}

void Unit::GetAreaTriggerList(std::list<AreaTrigger*> &list, uint32 spellId)
{
    if (m_areaTrigger.empty())
        return;
    for (AreaTriggerList::const_iterator i = m_areaTrigger.begin(); i != m_areaTrigger.end(); ++i)
    {
        AreaTrigger* areaTrigger = *i;
        if (areaTrigger->GetSpellId() == spellId)
            list.push_back(areaTrigger);
    }
}

void Unit::RemoveDynObject(uint32 spellId)
{
    if (m_dynObj.empty())
        return;
    for (DynObjectList::iterator i = m_dynObj.begin(); i != m_dynObj.end();)
    {
        DynamicObject* dynObj = *i;
        if (dynObj->GetSpellId() == spellId)
        {
            dynObj->Remove();
            i = m_dynObj.begin();
        }
        else
            ++i;
    }
}

void Unit::RemoveAreaTrigger(uint32 spellId)
{
    if (m_areaTrigger.empty())
        return;
    for (AreaTriggerList::iterator i = m_areaTrigger.begin(); i != m_areaTrigger.end();)
    {
        AreaTrigger* areaTrigger = *i;
        if (areaTrigger->GetSpellId() == spellId)
        {
            areaTrigger->Remove();
            i = m_areaTrigger.begin();
        }
        else
            ++i;
    }
}

void Unit::RemoveAreaTrigger(AuraEffect const* eff)
{
    for (auto&& areaTrigger : m_areaTrigger)
    {
        if (areaTrigger->GetSpellId() == eff->GetSpellInfo()->Id && areaTrigger->GetEffectIndex() == eff->GetEffIndex())
        {
            areaTrigger->Remove();
            return;
        }
    }
}

void Unit::RemoveAllDynObjects()
{
    while (!m_dynObj.empty())
        m_dynObj.front()->Remove();
}

void Unit::RemoveAllAreasTrigger()
{
    while (!m_areaTrigger.empty())
        m_areaTrigger.front()->Remove();
}

GameObject* Unit::GetGameObject(uint32 spellId) const
{
    for (GameObjectList::const_iterator i = m_gameObj.begin(); i != m_gameObj.end(); ++i)
        if ((*i)->GetSpellId() == spellId)
            return *i;

    return NULL;
}

void Unit::AddGameObject(GameObject* gameObj)
{
    if (!gameObj || !(gameObj->GetOwnerGUID() == 0))
        return;

    m_gameObj.push_back(gameObj);
    gameObj->SetOwnerGUID(GetGUID());

    if (GetTypeId() == TYPEID_PLAYER && gameObj->GetSpellId())
    {
        SpellInfo const* createBySpell = sSpellMgr->GetSpellInfo(gameObj->GetSpellId());
        // Need disable spell use for owner
        if (createBySpell && createBySpell->IsCooldownStartedOnEvent())
            // note: item based cooldowns and cooldown spell mods with charges ignored (unknown existing cases)
            ToPlayer()->GetSpellHistory()->SetCooldownOnHold(createBySpell, 0);
    }
}

void Unit::RemoveGameObject(GameObject* gameObj, bool del)
{
    if (!gameObj || gameObj->GetOwnerGUID() != GetGUID())
        return;

    gameObj->SetOwnerGUID(ObjectGuid::Empty);

    for (uint8 i = 0; i < MAX_GAMEOBJECT_SLOT; ++i)
    {
        if (m_ObjectSlot [i] == gameObj->GetGUID())
        {
            m_ObjectSlot [i].Clear();
            break;
        }
    }

    // GO created by some spell
    if (uint32 spellid = gameObj->GetSpellId())
    {
        RemoveAurasDueToSpell(spellid);

        if (GetTypeId() == TYPEID_PLAYER)
        {
            SpellInfo const* createBySpell = sSpellMgr->GetSpellInfo(spellid);
            // Need activate spell use for owner
            if (createBySpell && createBySpell->IsCooldownStartedOnEvent())
                // note: item based cooldowns and cooldown spell mods with charges ignored (unknown existing cases)
                ToPlayer()->SendCooldownEvent(createBySpell);
        }
    }

    m_gameObj.remove(gameObj);

    if (del)
    {
        gameObj->SetRespawnTime(0);
        gameObj->Delete();
    }
}

void Unit::RemoveGameObject(uint32 spellid, bool del)
{
    if (m_gameObj.empty())
        return;
    GameObjectList::iterator i, next;
    for (i = m_gameObj.begin(); i != m_gameObj.end(); i = next)
    {
        next = i;
        if (spellid == 0 || (*i)->GetSpellId() == spellid)
        {
            (*i)->SetOwnerGUID(ObjectGuid::Empty);
            if (del)
            {
                (*i)->SetRespawnTime(0);
                (*i)->Delete();
            }

            next = m_gameObj.erase(i);
        }
        else
            ++next;
    }
}

void Unit::RemoveAllGameObjects()
{
    // remove references to unit
    while (!m_gameObj.empty())
    {
        GameObjectList::iterator i = m_gameObj.begin();
        (*i)->SetOwnerGUID(ObjectGuid::Empty);
        (*i)->SetRespawnTime(0);
        (*i)->Delete();
        m_gameObj.erase(i);
    }
}

void Unit::SendSpellNonMeleeDamageLog(SpellNonMeleeDamage* log)
{
    log->damage = RecaculateDamage(log->damage); // Does not send dots updated values? needs investigation done
    ObjectGuid attackerGuid = log->attacker->GetGUID();
    ObjectGuid targetGuid = log->target->GetGUID();
    int32 overkill = log->damage - log->target->GetHealth();

    WorldPacket data(SMSG_SPELLNONMELEEDAMAGELOG, (16 + 4 + 4 + 4 + 1 + 4 + 4 + 1 + 1 + 4 + 4 + 1)); // we guess size
    data.WriteBit(targetGuid [2]);
    data.WriteBit(attackerGuid [7]);
    data.WriteBit(attackerGuid [6]);
    data.WriteBit(attackerGuid [1]);
    data.WriteBit(attackerGuid [5]);
    data.WriteBit(0); // Unk
    data.WriteBit(attackerGuid [0]);
    data.WriteBit(targetGuid [0]);
    data.WriteBit(targetGuid [7]);
    data.WriteBit(attackerGuid [3]);
    data.WriteBit(targetGuid [6]);
    data.WriteBit(0); // Unk
    data.WriteBit(0); // HasPowerData
    data.WriteBit(targetGuid [1]);
    data.WriteBit(0); // No floats
    data.WriteBit(targetGuid [5]);
    data.WriteBit(attackerGuid [2]);
    data.WriteBit(attackerGuid [4]);
    data.WriteBit(targetGuid [3]);
    data.WriteBit(targetGuid [4]);

    data.FlushBits();

    data << uint32(log->blocked);
    data.WriteByteSeq(attackerGuid [1]);
    data << uint32(overkill > 0 ? overkill : 0);
    data.WriteByteSeq(targetGuid [3]);
    data.WriteByteSeq(attackerGuid [0]);
    data.WriteByteSeq(targetGuid [6]);
    data.WriteByteSeq(targetGuid [4]);
    data.WriteByteSeq(attackerGuid [7]);
    data << uint32(log->resist);
    data << uint32(log->absorb);
    data.WriteByteSeq(attackerGuid [5]);
    data.WriteByteSeq(targetGuid [5]);
    data.WriteByteSeq(attackerGuid [3]);
    data.WriteByteSeq(attackerGuid [2]);
    data.WriteByteSeq(targetGuid [2]);
    data.WriteByteSeq(attackerGuid [6]);
    data.WriteByteSeq(targetGuid [0]);
    data.WriteByteSeq(attackerGuid [4]);
    data << uint32(log->damage);
    data << uint8(log->schoolMask);
    data.WriteByteSeq(targetGuid [7]);
    data << uint32(log->HitInfo);
    data.WriteByteSeq(targetGuid [1]);
    data << uint32(log->SpellID);

    SendMessageToSet(&data, true);
}

void Unit::SendSpellNonMeleeDamageLog(Unit* target, uint32 SpellID, uint32 Damage, SpellSchoolMask damageSchoolMask, uint32 AbsorbedDamage, uint32 Resist, bool PhysicalDamage, uint32 Blocked, bool CriticalHit)
{
    SpellNonMeleeDamage log(this, target, SpellID, damageSchoolMask);
    log.damage = Damage - AbsorbedDamage - Resist - Blocked;
    log.absorb = AbsorbedDamage;
    log.resist = Resist;
    log.physicalLog = PhysicalDamage;
    log.blocked = Blocked;
    log.HitInfo = SPELL_HIT_TYPE_UNK1 | SPELL_HIT_TYPE_UNK3 | SPELL_HIT_TYPE_UNK6;
    if (CriticalHit)
        log.HitInfo |= SPELL_HIT_TYPE_CRIT;
    SendSpellNonMeleeDamageLog(&log);
}

void Unit::ProcDamageAndSpell(Unit* victim, uint32 procAttacker, uint32 procVictim, uint32 procExtra, uint32 amount, WeaponAttackType attType, SpellInfo const* procSpell, SpellInfo const* procAura, ProcTriggerContext const& context)
{
    // Not much to do if no flags are set.
    if (procAttacker)
        ProcDamageAndSpellFor(false, victim, procAttacker, procExtra, attType, procSpell, amount, procAura, context);
    // Now go on with a victim's events'n'auras
    // Not much to do if no flags are set or there is no victim
    if (victim && victim->IsAlive() && procVictim)
        victim->ProcDamageAndSpellFor(true, this, procVictim, procExtra, attType, procSpell, amount, procAura, context);
}

void Unit::SendPeriodicAuraLog(SpellPeriodicAuraLogInfo* pInfo)
{
    AuraEffect const* aura = pInfo->auraEff;
    ObjectGuid casterGuid = aura->GetCasterGUID();
    ObjectGuid targetGuid = GetGUID();

    WorldPacket data(SMSG_SPELL_PERIODIC_AURA_LOG, 30);

    data.WriteBit(targetGuid [7]);
    data.WriteBit(casterGuid [0]);
    data.WriteBit(casterGuid [7]);
    data.WriteBit(targetGuid [1]);
    data.WriteBits(1, 21); // Count

    data.WriteBit(targetGuid[0]);

    bool overheal = pInfo->overDamage && (aura->GetAuraType() == SPELL_AURA_PERIODIC_HEAL || aura->GetAuraType() == SPELL_AURA_OBS_MOD_HEALTH);
    bool overdamage = pInfo->overDamage && (aura->GetAuraType() == SPELL_AURA_PERIODIC_DAMAGE || aura->GetAuraType() == SPELL_AURA_PERIODIC_DAMAGE_PERCENT);

    uint32 MaskOrPowerID = 0;
    if (aura->GetAuraType() == SPELL_AURA_PERIODIC_DAMAGE || aura->GetAuraType() == SPELL_AURA_PERIODIC_DAMAGE_PERCENT)
        MaskOrPowerID = aura->GetSpellInfo()->GetSchoolMask();
    if (aura->GetAuraType() == SPELL_AURA_PERIODIC_HEAL || aura->GetAuraType() == SPELL_AURA_OBS_MOD_HEALTH || aura->GetAuraType() == SPELL_AURA_PERIODIC_ENERGIZE)
        MaskOrPowerID = aura->GetMiscValue();

    data.WriteBit(!overheal);
    data.WriteBit(!(pInfo->absorb > 0));
    data.WriteBit(pInfo->critical);
    data.WriteBit(!overdamage);
    data.WriteBit(!MaskOrPowerID);

    data.WriteBit(targetGuid[5]);
    data.WriteBit(targetGuid[3]);
    data.WriteBit(casterGuid[1]);
    data.WriteBit(targetGuid[2]);
    data.WriteBit(casterGuid[6]);
    data.WriteBit(casterGuid[3]);
    data.WriteBit(casterGuid[4]);
    data.WriteBit(0); // Power data
    data.WriteBit(casterGuid[2]);
    data.WriteBit(targetGuid[6]);
    data.WriteBit(casterGuid[5]);
    data.WriteBit(targetGuid[4]);

    if (overheal)
        data << uint32(pInfo->overDamage);

    data << uint32(pInfo->damage);                  // damage
    data << uint32(aura->GetAuraType());

    if (overdamage)
        data << uint32(pInfo->overDamage);
    if (pInfo->absorb)
        data << uint32(pInfo->absorb);
    if (MaskOrPowerID)
        data << uint32(MaskOrPowerID);

    data.WriteByteSeq(casterGuid[5]);
    data.WriteByteSeq(casterGuid[3]);
    data.WriteByteSeq(targetGuid[4]);
    data << uint32(aura->GetId());                          // spellId
    data.WriteByteSeq(targetGuid[6]);
    data.WriteByteSeq(casterGuid[7]);
    data.WriteByteSeq(casterGuid[1]);
    data.WriteByteSeq(targetGuid[5]);
    data.WriteByteSeq(casterGuid[0]);
    data.WriteByteSeq(targetGuid[1]);
    data.WriteByteSeq(targetGuid[7]);
    data.WriteByteSeq(casterGuid[4]);
    data.WriteByteSeq(targetGuid[3]);
    data.WriteByteSeq(casterGuid[2]);
    data.WriteByteSeq(targetGuid[0]);
    data.WriteByteSeq(targetGuid[2]);
    data.WriteByteSeq(casterGuid[6]);

    SendMessageToSet(&data, true);
}

void Unit::SendSpellMiss(Unit* target, uint32 spellID, SpellMissInfo missInfo)
{
    ObjectGuid casterGuid = GetGUID();
    ObjectGuid targetGuid = target->GetGUID();

    WorldPacket data(SMSG_SPELLLOGMISS);

    data.WriteBit(casterGuid[5]);
    data.WriteBit(casterGuid[1]);
    data.WriteBit(casterGuid[4]);
    data.WriteBit(casterGuid[0]);
    data.WriteBit(casterGuid[7]);
    data.WriteBit(casterGuid[3]);
    data.WriteBit(casterGuid[2]);
    data.WriteBit(casterGuid[6]);
    data.WriteBits(1, 23);                                      // target count
    // for (i = 0; i < target count; ++i)
    data.WriteBit(targetGuid[0]);
    data.WriteBit(targetGuid[1]);
    data.WriteBit(targetGuid[6]);
    data.WriteBit(targetGuid[2]);
    data.WriteBit(targetGuid[5]);
    data.WriteBit(targetGuid[3]);
    data.WriteBit(targetGuid[4]);
    data.WriteBit(targetGuid[7]);
    data.WriteBit(false); // unk
    // end loop

    // for (i = 0; i < target count; ++i)
    data << uint8(missInfo);
    data.WriteByteSeq(targetGuid[7]);
    data.WriteByteSeq(targetGuid[5]);
    data.WriteByteSeq(targetGuid[0]);
    data.WriteByteSeq(targetGuid[6]);
    data.WriteByteSeq(targetGuid[3]);
    data.WriteByteSeq(targetGuid[2]);
    //if (unk)
    //{
    //    data << float(0);
    //    data << float(0);
    //}
    data.WriteByteSeq(targetGuid[1]);
    data.WriteByteSeq(targetGuid[4]);
    // end loop

    data.WriteByteSeq(casterGuid[6]);
    data.WriteByteSeq(casterGuid[4]);
    data.WriteByteSeq(casterGuid[2]);
    data.WriteByteSeq(casterGuid[0]);
    data.WriteByteSeq(casterGuid[1]);
    data << uint32(spellID);
    data.WriteByteSeq(casterGuid[3]);
    data.WriteByteSeq(casterGuid[7]);
    data.WriteByteSeq(casterGuid[5]);

    SendMessageToSet(&data, true);
}

void Unit::SendSpellDamageResist(Unit* target, uint32 spellId)
{
    ObjectGuid casterGUID = GetGUID();
    ObjectGuid targetGUID = target->GetGUID();
    WorldPacket data(SMSG_PROC_RESIST, 8 + 8 + 1 + 1 + 4 + 4 + 4);

    data.WriteBit(targetGUID[4]);
    data.WriteBit(casterGUID[7]);
    data.WriteBit(targetGUID[5]);
    data.WriteBit(targetGUID[6]);
    data.WriteBit(casterGUID[0]);
    data.WriteBit(0); // unk48
    data.WriteBit(casterGUID[6]);
    data.WriteBit(casterGUID[4]);
    data.WriteBit(casterGUID[5]);
    data.WriteBit(casterGUID[1]);
    data.WriteBit(targetGUID[7]);
    data.WriteBit(targetGUID[1]);
    data.WriteBit(casterGUID[2]);
    data.WriteBit(targetGUID[2]);
    data.WriteBit(0); // unk20
    data.WriteBit(casterGUID[3]);
    data.WriteBit(targetGUID[0]);
    data.WriteBit(targetGUID[3]);

    data.WriteByteSeq(casterGUID[1]);
    data.WriteByteSeq(casterGUID[6]);
    data.WriteByteSeq(casterGUID[5]);
    data.WriteByteSeq(casterGUID[7]);
    //if (unk20)
    //    data << float(0);
    //if (unk48)
    //    data << float(0);
    data.WriteByteSeq(targetGUID[5]);
    data.WriteByteSeq(targetGUID[7]);
    data.WriteByteSeq(casterGUID[4]);
    data.WriteByteSeq(casterGUID[0]);
    data << uint32(spellId);
    data.WriteByteSeq(casterGUID[2]);
    data.WriteByteSeq(targetGUID[0]);
    data.WriteByteSeq(casterGUID[3]);
    data.WriteByteSeq(targetGUID[3]);
    data.WriteByteSeq(targetGUID[1]);
    data.WriteByteSeq(targetGUID[4]);
    data.WriteByteSeq(targetGUID[6]);
    data.WriteByteSeq(targetGUID[2]);

    SendMessageToSet(&data, true);
}

void Unit::SendSpellDamageImmune(Unit* target, uint32 spellId)
{
    ObjectGuid casterGuid = GetGUID();
    ObjectGuid targetGuid = target->GetGUID();
    WorldPacket data(SMSG_SPELL_OR_DAMAGE_IMMUNE, 8 + 8 + 4 + 1);
    data.WriteBit(casterGuid[5]);
    data.WriteBit(casterGuid[0]);
    data.WriteBit(targetGuid[6]);
    data.WriteBit(0); // bool - log format: 0-default, 1-debug
    data.WriteBit(casterGuid[7]);
    data.WriteBit(targetGuid[5]);
    data.WriteBit(targetGuid[2]);
    data.WriteBit(casterGuid[2]);
    data.WriteBit(casterGuid[1]);
    data.WriteBit(casterGuid[3]);
    data.WriteBit(targetGuid[4]);
    data.WriteBit(casterGuid[4]);
    data.WriteBit(targetGuid[7]);
    data.WriteBit(casterGuid[6]);
    data.WriteBit(targetGuid[3]);
    data.WriteBit(targetGuid[1]);
    data.WriteBit(targetGuid[0]);

    data.FlushBits();

    data.WriteByteSeq(targetGuid[4]);
    data.WriteByteSeq(targetGuid[3]);
    data.WriteByteSeq(casterGuid[2]);
    data.WriteByteSeq(targetGuid[6]);
    data.WriteByteSeq(targetGuid[5]);
    data.WriteByteSeq(casterGuid[3]);
    data.WriteByteSeq(targetGuid[7]);
    data.WriteByteSeq(targetGuid[2]);
    data.WriteByteSeq(targetGuid[0]);
    data.WriteByteSeq(casterGuid[7]);
    data.WriteByteSeq(casterGuid[5]);
    data.WriteByteSeq(casterGuid[0]);
    data.WriteByteSeq(casterGuid[1]);
    data.WriteByteSeq(casterGuid[4]);
    data << uint32(spellId);
    data.WriteByteSeq(targetGuid[1]);
    data.WriteByteSeq(casterGuid[6]);
    SendMessageToSet(&data, true);
}

void Unit::SendAttackStateUpdate(CalcDamageInfo* damageInfo)
{
    TC_LOG_DEBUG("entities.unit", "WORLD: Sending SMSG_ATTACKERSTATEUPDATE");

    ObjectGuid guid = GetGUID();
    uint32 count = 1;
    size_t maxsize = 4 + 5 + 5 + 4 + 4 + 1 + 4 + 4 + 4 + 4 + 4 + 1 + 4 + 4 + 4 + 4 + 4 * 12;
    WorldPacket data(SMSG_ATTACKERSTATEUPDATE, maxsize);    // we guess size

    bool hasUnkFlags = damageInfo->HitInfo & HITINFO_UNK26;
    uint32 unkCounter = 0;

    data.WriteBit(hasUnkFlags);

    if (hasUnkFlags)
    {
        data.WriteBits(unkCounter, 21);

        data << uint32(0);

        for (uint32 i = 0; i < unkCounter; ++i)
        {
            data << uint32(0);
            data << uint32(0);
        }

        data << uint32(0);
        data << uint32(0);
    }

    // Needs to be flushed because data.wpos() wouldnt return the correct placeholder
    data.FlushBits();

    size_t size = data.wpos();
    data << uint32(0); // Placeholder
    size_t datapos = data.wpos();

    data << uint32(damageInfo->HitInfo);
    data << damageInfo->attacker->GetPackGUID();
    data << damageInfo->target->GetPackGUID();
    data << uint32(damageInfo->damage);                     // Full damage
    int32 overkill = damageInfo->damage - damageInfo->target->GetHealth();
    data << uint32(overkill < 0 ? 0 : overkill);            // Overkill
    data << uint8(count);                                   // Sub damage count

    for (uint32 i = 0; i < count; ++i)
    {
        data << uint32(damageInfo->damageSchoolMask);       // School of sub damage
        data << float(damageInfo->damage);                  // sub damage
        data << uint32(damageInfo->damage);                 // Sub Damage
    }

    if (damageInfo->HitInfo & (HITINFO_FULL_ABSORB | HITINFO_PARTIAL_ABSORB))
    {
        for (uint32 i = 0; i < count; ++i)
            data << uint32(damageInfo->absorb);             // Absorb
    }

    if (damageInfo->HitInfo & (HITINFO_FULL_RESIST | HITINFO_PARTIAL_RESIST))
    {
        for (uint32 i = 0; i < count; ++i)
            data << uint32(damageInfo->resist);             // Resist
    }

    data << uint8(damageInfo->TargetState);
    data << uint32(0);  // Unknown attackerstate
    data << uint32(0);  // Melee spellid

    if (damageInfo->HitInfo & HITINFO_BLOCK)
        data << uint32(damageInfo->blocked_amount);

    if (damageInfo->HitInfo & HITINFO_RAGE_GAIN)
        data << uint32(0);

    //! Probably used for debugging purposes, as it is not known to appear on retail servers
    if (damageInfo->HitInfo & HITINFO_UNK1)
    {
        data << uint32(0);
        data << float(0);
        data << float(0);
        data << float(0);
        data << float(0);
        data << float(0);
        data << float(0);
        data << float(0);
        data << float(0);

        for (uint8 i = 0; i < 2; ++i)
        {
            data << float(0);
            data << float(0);
        }
        data << uint32(0);
    }

    if (damageInfo->HitInfo & (HITINFO_BLOCK | HITINFO_UNK12))
        data << float(0);

    data.put(size, uint32(data.wpos() - datapos)); // Blizz - Weird and Lazy people....
    SendMessageToSet(&data, true);
}

void Unit::SendAttackStateUpdate(uint32 HitInfo, Unit* target, uint8 /*SwingType*/, SpellSchoolMask damageSchoolMask, uint32 Damage, uint32 AbsorbDamage, uint32 Resist, VictimState TargetState, uint32 BlockedAmount)
{
    CalcDamageInfo dmgInfo;
    dmgInfo.HitInfo = HitInfo;
    dmgInfo.attacker = this;
    dmgInfo.target = target;
    dmgInfo.damage = Damage - AbsorbDamage - Resist - BlockedAmount;
    dmgInfo.damageSchoolMask = damageSchoolMask;
    dmgInfo.absorb = AbsorbDamage;
    dmgInfo.resist = Resist;
    dmgInfo.TargetState = TargetState;
    dmgInfo.blocked_amount = BlockedAmount;
    SendAttackStateUpdate(&dmgInfo);
}

bool Unit::HandleAuraProcOnPowerAmount(Unit* victim, uint32 /*damage*/, AuraEffect* triggeredByAura, SpellInfo const* procSpell, uint32 procFlag, uint32 /*procEx*/, uint32 cooldown, ProcTriggerContext const& context)
{
    // Get triggered aura spell info
    SpellInfo const* auraSpellInfo = triggeredByAura->GetSpellInfo();

    // Get effect index used for the proc
    uint32 effIndex = triggeredByAura->GetEffIndex();

    // Power amount required to proc the spell
    int32 powerAmountRequired = triggeredByAura->GetAmount();
    // Power type required to proc
    Powers powerRequired = Powers(auraSpellInfo->Effects [triggeredByAura->GetEffIndex()].MiscValue);

    // Set trigger spell id, target, custom basepoints
    uint32 trigger_spell_id = auraSpellInfo->Effects [triggeredByAura->GetEffIndex()].TriggerSpell;

    Unit*  target = NULL;
    int32  basepoints0 = 0;

    Item* castItem = triggeredByAura->GetBase()->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
        ? ToPlayer()->GetItemByGuid(triggeredByAura->GetBase()->GetCastItemGUID()) : NULL;

    /* Try handle unknown trigger spells or with invalid power amount or misc value
    if (sSpellMgr->GetSpellInfo(trigger_spell_id) == NULL || powerAmountRequired == NULL || powerRequired >= MAX_POWER)
    {
    switch (auraSpellInfo->SpellFamilyName)
    {
    case SPELLFAMILY_GENERIC:
    {
    break;
    }
    }
    }*/

    // All ok. Check current trigger spell
    SpellInfo const* triggerEntry = sSpellMgr->GetSpellInfo(trigger_spell_id);
    if (triggerEntry == NULL)
    {
        // Not cast unknown spell
        // TC_LOG_ERROR("Unit::HandleAuraProcOnPowerAmount: Spell %u have 0 in EffectTriggered[%d], not handled custom case?", auraSpellInfo->Id, triggeredByAura->GetEffIndex());
        return false;
    }

    // not allow proc extra attack spell at extra attack
    if (m_extraAttacks && triggerEntry->HasEffect(SPELL_EFFECT_ADD_EXTRA_ATTACKS))
        return false;

    if (!powerRequired || !powerAmountRequired)
    {
        TC_LOG_ERROR("spells", "Unit::HandleAuraProcOnPowerAmount: Spell %u have 0 powerAmountRequired in EffectAmount[%d] or 0 powerRequired in EffectMiscValue, not handled custom case?", auraSpellInfo->Id, triggeredByAura->GetEffIndex());
        return false;
    }

    if (GetPower(powerRequired) != powerAmountRequired)
        return false;

    // Custom requirements (not listed in procEx) Warning! damage dealing after this
    // Custom triggered spells
    switch (auraSpellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_DRUID:
        {
            // Eclipse Mastery Driver Passive
            if (auraSpellInfo->Id == 79577)
            {
                uint32 solarEclipseMarker = 67483;
                uint32 lunarEclipseMarker = 67484;

                switch (effIndex)
                {
                    case 0:
                    {
                        if (HasAura(trigger_spell_id))
                            return false;

                        // Do not proc if proc spell isnt starfire and starsurge
                        if (procSpell->Id != 2912 && procSpell->Id != 78674)
                            return false;

                        if (HasAura(solarEclipseMarker))
                        {
                            RemoveAurasDueToSpell(solarEclipseMarker);
                            CastSpell(this, lunarEclipseMarker, true);
                        }
                        break;
                    }
                    case 1:
                    {
                        if (HasAura(trigger_spell_id))
                            return false;

                        // Do not proc if proc spell isnt wrath and starsurge
                        if (procSpell->Id != 5176 && procSpell->Id != 78674)
                            return false;

                        if (HasAura(lunarEclipseMarker))
                        {
                            RemoveAurasDueToSpell(lunarEclipseMarker);
                            CastSpell(this, solarEclipseMarker, true);
                        }

                        break;
                    }
                }
            }
            break;
        }
    }

    if (cooldown && GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasSpellCooldown(trigger_spell_id))
        return false;

    // try detect target manually if not set
    if (target == NULL)
        target = !(procFlag & (PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS)) && triggerEntry && triggerEntry->IsPositive() ? this : victim;

    if (basepoints0)
        CastCustomSpell(target, trigger_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);
    else
        CastSpell(target, trigger_spell_id, true, castItem, triggeredByAura);

    if (cooldown && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->AddSpellCooldown(trigger_spell_id, Milliseconds(cooldown));

    return true;
}

//victim may be NULL
bool Unit::HandleDummyAuraProc(Unit* victim, uint32 damage, AuraEffect* triggeredByAura, SpellInfo const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown, ProcTriggerContext const& context)
{
    SpellInfo const* dummySpell = triggeredByAura->GetSpellInfo();
    uint32 effIndex = triggeredByAura->GetEffIndex();
    int32  triggerAmount = triggeredByAura->GetAmount();

    Item* castItem = triggeredByAura->GetBase()->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
        ? ToPlayer()->GetItemByGuid(triggeredByAura->GetBase()->GetCastItemGUID()) : NULL;

    uint32 triggered_spell_id = 0;

    // otherwise, it's the triggered_spell_id by default
    Unit* target = victim;
    int32 basepoints0 = 0;
    ObjectGuid originalCaster = ObjectGuid::Empty;

    switch (dummySpell->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (dummySpell->Id)
            {
                // Essence of Dreams, Ultraxion, Dragon Soul
                case 105900:
                    basepoints0 = damage;
                    triggered_spell_id = 105996;
                    break;
                // Fel Decay, Peroth'arn, Well of Eternity
                case 108124:
                    if (!victim)
                        return false;
                    if (!damage)
                        return false;
                    triggered_spell_id = 108128;
                    target = victim;
                    basepoints0 = damage;
                    break;
                // Concentration, Majordomo Staghelm
                case 98229:
                    SetPower(POWER_ALTERNATE_POWER, 0);
                    break;
                // Vital Spark, Baleroc
                case 99262:
                {
                    if (!victim || !victim->HasAura(99252))
                        return false;

                    if (Aura* aur = GetAura(99262))
                    {
                        int32 bp = aur->GetStackAmount() * 5;
                        CastCustomSpell(this, 99263, &bp, 0, 0, true);
                        aur->Remove();
                    }
                    break;
                }
                // Torment, Baleroc (normal)
                case 99256:
                case 100230:
                {
                    if (!victim || HasAura(99252) || victim->HasAura(99263))
                        return false;

                    if (Aura* aur = GetAura(dummySpell->Id))
                    {
                        uint8 stacks = aur->GetStackAmount();
                        if (stacks < 3)
                            return false;

                        int32 bp = int32(stacks / 3);
                        victim->CastCustomSpell(99262, SPELLVALUE_AURA_STACK, bp, victim, true);
                    }
                    break;
                }
                // Torment, Baleroc (heroic)
                case 100231:
                case 100232:
                {
                    if (!victim || HasAura(99252) || victim->HasAura(99263))
                        return false;

                    if (Aura* aur = GetAura(dummySpell->Id))
                    {
                        uint8 stacks = aur->GetStackAmount();
                        if (stacks < 5)
                            return false;

                        int32 bp = int32(stacks / 5);
                        victim->CastCustomSpell(99262, SPELLVALUE_AURA_STACK, bp, victim, true);
                    }
                    break;
                }
                // Unstable Power
                case 24658:
                {
                    if (!procSpell || procSpell->Id == 24659)
                        return false;
                    // Need remove one 24659 aura
                    RemoveAuraFromStack(24659);
                    return true;
                }
                // Restless Strength
                case 24661:
                {
                    // Need remove one 24662 aura
                    RemoveAuraFromStack(24662);
                    return true;
                }
                // Mark of Malice
                case 33493:
                {
                    // Cast finish spell at last charge
                    if (triggeredByAura->GetBase()->GetCharges() > 1)
                        return false;

                    target = this;
                    triggered_spell_id = 33494;
                    break;
                }
                // Twisted Reflection (boss spell)
                case 21063:
                    triggered_spell_id = 21064;
                    break;
                    // Vampiric Aura (boss spell)
                case 38196:
                {
                    basepoints0 = 3 * damage;               // 300%
                    if (basepoints0 < 0)
                        return false;

                    triggered_spell_id = 31285;
                    target = this;
                    break;
                }
                // Aura of Madness (Darkmoon Card: Madness trinket)
                //=====================================================
                // 39511 Sociopath: +35 strength (Paladin, Rogue, Druid, Warrior)
                // 40997 Delusional: +70 attack power (Rogue, Hunter, Paladin, Warrior, Druid)
                // 40998 Kleptomania: +35 agility (Warrior, Rogue, Paladin, Hunter, Druid)
                // 40999 Megalomania: +41 damage/healing (Druid, Shaman, Priest, Warlock, Mage, Paladin)
                // 41002 Paranoia: +35 spell/melee/ranged crit strike rating (All classes)
                // 41005 Manic: +35 haste (spell, melee and ranged) (All classes)
                // 41009 Narcissism: +35 intellect (Druid, Shaman, Priest, Warlock, Mage, Paladin, Hunter)
                // 41011 Martyr Complex: +35 stamina (All classes)
                // 41406 Dementia: Every 5 seconds either gives you +5% damage/healing. (Druid, Shaman, Priest, Warlock, Mage, Paladin)
                // 41409 Dementia: Every 5 seconds either gives you -5% damage/healing. (Druid, Shaman, Priest, Warlock, Mage, Paladin)
                case 39446:
                {
                    if (GetTypeId() != TYPEID_PLAYER || !IsAlive())
                        return false;

                    // Select class defined buff
                    switch (GetClass())
                    {
                        case CLASS_PALADIN:                 // 39511, 40997, 40998, 40999, 41002, 41005, 41009, 41011, 41409
                        case CLASS_DRUID:                   // 39511, 40997, 40998, 40999, 41002, 41005, 41009, 41011, 41409
                            triggered_spell_id = RAND(39511, 40997, 40998, 40999, 41002, 41005, 41009, 41011, 41409);
                            break;
                        case CLASS_ROGUE:                   // 39511, 40997, 40998, 41002, 41005, 41011
                        case CLASS_WARRIOR:                 // 39511, 40997, 40998, 41002, 41005, 41011
                        case CLASS_DEATH_KNIGHT:
                            triggered_spell_id = RAND(39511, 40997, 40998, 41002, 41005, 41011);
                            break;
                        case CLASS_PRIEST:                  // 40999, 41002, 41005, 41009, 41011, 41406, 41409
                        case CLASS_SHAMAN:                  // 40999, 41002, 41005, 41009, 41011, 41406, 41409
                        case CLASS_MAGE:                    // 40999, 41002, 41005, 41009, 41011, 41406, 41409
                        case CLASS_WARLOCK:                 // 40999, 41002, 41005, 41009, 41011, 41406, 41409
                            triggered_spell_id = RAND(40999, 41002, 41005, 41009, 41011, 41406, 41409);
                            break;
                        case CLASS_HUNTER:                  // 40997, 40999, 41002, 41005, 41009, 41011, 41406, 41409
                            triggered_spell_id = RAND(40997, 40999, 41002, 41005, 41009, 41011, 41406, 41409);
                            break;
                        default:
                            return false;
                    }

                    target = this;
                    if (roll_chance_i(10))
                        ToPlayer()->Say("This is Madness!", LANG_UNIVERSAL); /// @todo It should be moved to database, shouldn't it?
                    break;
                }
                // Sunwell Exalted Caster Neck (??? neck)
                // cast ??? Light's Wrath if Exalted by Aldor
                // cast ??? Arcane Bolt if Exalted by Scryers
                case 46569:
                    return false;                           // old unused version
                    // Sunwell Exalted Caster Neck (Shattered Sun Pendant of Acumen neck)
                    // cast 45479 Light's Wrath if Exalted by Aldor
                    // cast 45429 Arcane Bolt if Exalted by Scryers
                case 45481:
                {
                    Player* player = ToPlayer();
                    if (!player)
                        return false;

                    // Get Aldor reputation rank
                    if (player->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45479;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (player->GetReputationRank(934) == REP_EXALTED)
                    {
                        // triggered at positive/self casts also, current attack target used then
                        if (target && IsFriendlyTo(target))
                        {
                            target = GetVictim();
                            if (!target)
                            {
                                target = player->GetSelectedUnit();
                                if (!target)
                                    return false;
                            }
                            if (IsFriendlyTo(target))
                                return false;
                        }

                        triggered_spell_id = 45429;
                        break;
                    }
                    return false;
                }
                // Sunwell Exalted Melee Neck (Shattered Sun Pendant of Might neck)
                // cast 45480 Light's Strength if Exalted by Aldor
                // cast 45428 Arcane Strike if Exalted by Scryers
                case 45482:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    // Get Aldor reputation rank
                    if (ToPlayer()->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45480;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (ToPlayer()->GetReputationRank(934) == REP_EXALTED)
                    {
                        triggered_spell_id = 45428;
                        break;
                    }
                    return false;
                }
                // Sunwell Exalted Tank Neck (Shattered Sun Pendant of Resolve neck)
                // cast 45431 Arcane Insight if Exalted by Aldor
                // cast 45432 Light's Ward if Exalted by Scryers
                case 45483:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    // Get Aldor reputation rank
                    if (ToPlayer()->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45432;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (ToPlayer()->GetReputationRank(934) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45431;
                        break;
                    }
                    return false;
                }
                // Sunwell Exalted Healer Neck (Shattered Sun Pendant of Restoration neck)
                // cast 45478 Light's Salvation if Exalted by Aldor
                // cast 45430 Arcane Surge if Exalted by Scryers
                case 45484:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    // Get Aldor reputation rank
                    if (ToPlayer()->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45478;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (ToPlayer()->GetReputationRank(934) == REP_EXALTED)
                    {
                        triggered_spell_id = 45430;
                        break;
                    }
                    return false;
                }
                // Kill command
                case 58914:
                {
                    // Remove aura stack from pet
                    RemoveAuraFromStack(58914);
                    Unit* owner = GetOwner();
                    if (!owner)
                        return true;
                    // reduce the owner's aura stack
                    owner->RemoveAuraFromStack(34027);
                    return true;
                }
                // Vampiric Touch (generic, used by some boss)
                case 52723:
                case 60501:
                {
                    triggered_spell_id = 52724;
                    basepoints0 = damage / 2;
                    target = this;
                    break;
                }
                // Shadowfiend Death (Gain mana if pet dies with Glyph of Shadowfiend)
                case 57989:
                {
                    Unit* owner = GetOwner();
                    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
                        return false;
                    // Glyph of Shadowfiend (need cast as self cast for owner, no hidden cooldown)
                    owner->CastSpell(owner, 58227, true, castItem, triggeredByAura);
                    return true;
                }
                // Divine purpose
                case 31871:
                case 31872:
                {
                    // Roll chane
                    if (!victim || !victim->IsAlive() || !roll_chance_i(triggerAmount))
                        return false;

                    // Remove any stun effect on target
                    victim->RemoveAurasWithMechanic(1 << MECHANIC_STUN, AURA_REMOVE_BY_ENEMY_SPELL);
                    return true;
                }
                // Purified Shard of the Scale - Onyxia 10 Caster Trinket
                case 69755:
                {
                    triggered_spell_id = (procFlag & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS) ? 69733 : 69729;
                    break;
                }
                // Shiny Shard of the Scale - Onyxia 25 Caster Trinket
                case 69739:
                {
                    triggered_spell_id = (procFlag & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS) ? 69734 : 69730;
                    break;
                }
                case 71519: // Deathbringer's Will Normal
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    std::vector<uint32> RandomSpells;
                    switch (GetClass())
                    {
                        case CLASS_WARRIOR:
                        case CLASS_PALADIN:
                        case CLASS_DEATH_KNIGHT:
                            RandomSpells.push_back(71484);
                            RandomSpells.push_back(71491);
                            RandomSpells.push_back(71492);
                            break;
                        case CLASS_SHAMAN:
                        case CLASS_ROGUE:
                            RandomSpells.push_back(71486);
                            RandomSpells.push_back(71485);
                            RandomSpells.push_back(71492);
                            break;
                        case CLASS_DRUID:
                            RandomSpells.push_back(71484);
                            RandomSpells.push_back(71485);
                            RandomSpells.push_back(71492);
                            break;
                        case CLASS_HUNTER:
                            RandomSpells.push_back(71486);
                            RandomSpells.push_back(71491);
                            RandomSpells.push_back(71485);
                            break;
                        default:
                            return false;
                    }
                    if (RandomSpells.empty()) // shouldn't happen
                        return false;

                    uint8 rand_spell = irand(0, (RandomSpells.size() - 1));
                    CastSpell(target, RandomSpells [rand_spell], true, castItem, triggeredByAura, originalCaster);
                    for (std::vector<uint32>::iterator itr = RandomSpells.begin(); itr != RandomSpells.end(); ++itr)
                    {
                        if (!ToPlayer()->HasSpellCooldown(*itr))
                            ToPlayer()->AddSpellCooldown(*itr, Seconds(cooldown));
                    }
                    break;
                }
                case 71562: // Deathbringer's Will Heroic
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    std::vector<uint32> RandomSpells;
                    switch (GetClass())
                    {
                        case CLASS_WARRIOR:
                        case CLASS_PALADIN:
                        case CLASS_DEATH_KNIGHT:
                            RandomSpells.push_back(71561);
                            RandomSpells.push_back(71559);
                            RandomSpells.push_back(71560);
                            break;
                        case CLASS_SHAMAN:
                        case CLASS_ROGUE:
                            RandomSpells.push_back(71558);
                            RandomSpells.push_back(71556);
                            RandomSpells.push_back(71560);
                            break;
                        case CLASS_DRUID:
                            RandomSpells.push_back(71561);
                            RandomSpells.push_back(71556);
                            RandomSpells.push_back(71560);
                            break;
                        case CLASS_HUNTER:
                            RandomSpells.push_back(71558);
                            RandomSpells.push_back(71559);
                            RandomSpells.push_back(71556);
                            break;
                        default:
                            return false;
                    }
                    if (RandomSpells.empty()) // shouldn't happen
                        return false;

                    uint8 rand_spell = irand(0, (RandomSpells.size() - 1));
                    CastSpell(target, RandomSpells [rand_spell], true, castItem, triggeredByAura, originalCaster);
                    for (std::vector<uint32>::iterator itr = RandomSpells.begin(); itr != RandomSpells.end(); ++itr)
                    {
                        if (!ToPlayer()->HasSpellCooldown(*itr))
                            ToPlayer()->AddSpellCooldown(*itr, Seconds(cooldown));
                    }
                    break;
                }
                case 65032: // Boom aura (321 Boombot)
                {
                    if (victim->GetEntry() != 33343)   // Scrapbot
                        return false;

                    InstanceScript* instance = GetInstanceScript();
                    if (!instance)
                        return false;

                    instance->DoCastSpellOnPlayers(65037);  // Achievement criteria marker
                    break;
                }
                case 47020: // Enter vehicle XT-002 (Scrapbot)
                {
                    if (GetTypeId() != TYPEID_UNIT)
                        return false;

                    Unit* vehicleBase = GetVehicleBase();
                    if (!vehicleBase)
                        return false;

                    // Todo: Check if this amount is blizzlike
                    vehicleBase->ModifyHealth(int32(vehicleBase->CountPctFromMaxHealth(1)));
                    break;
                }
                // Vampiric Might (Cult Fanatic, Icecrown Citadel, Lady Deathwhisper encounter)
                case 70674:
                {
                    if (damage <= 0)
                        return false;

                    basepoints0 = 3 * damage;  // heal 300%
                    triggered_spell_id = 70677;
                    target = this;
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            // Incanter's Regalia set (add trigger chance to Mana Shield)
            if (dummySpell->SpellFamilyFlags [0] & 0x8000)
            {
                if (GetTypeId() != TYPEID_PLAYER)
                    return false;

                target = this;
                triggered_spell_id = 37436;
                break;
            }
        }
        case SPELLFAMILY_WARLOCK:
        {
            // Seed of Corruption (Mobs cast) - no die req
            if (dummySpell->SpellFamilyFlags.IsEqual(0, 0, 0) && dummySpell->SpellIconID == 1932)
            {
                // if damage is more than need deal finish spell
                if (triggeredByAura->GetAmount() <= int32(damage))
                {
                    // remember caster before aura delete
                    Unit* caster = triggeredByAura->GetCaster();

                    // Remove aura (before cast for prevent infinite loop handlers)
                    RemoveAurasDueToSpell(triggeredByAura->GetId());

                    // Cast finish spell (triggeredByAura already not exist!)
                    if (caster)
                        caster->CastSpell(this, 32865, true, castItem);
                    return true;                            // no hidden cooldown
                }
                // Damage counting
                triggeredByAura->SetAmount(triggeredByAura->GetAmount() - damage);
                return true;
            }
            switch (dummySpell->Id)
            {
                // Shadowflame (Voidheart Raiment set bonus)
                case 37377:
                {
                    triggered_spell_id = 37379;
                    break;
                }
                // Pet Healing (Corruptor Raiment or Rift Stalker Armor)
                case 37381:
                {
                    target = GetGuardianPet();
                    if (!target)
                        return false;

                    // heal amount
                    basepoints0 = CalculatePct(int32(damage), triggerAmount);
                    triggered_spell_id = 37382;
                    break;
                }
                // Shadowflame Hellfire (Voidheart Raiment set bonus)
                case 39437:
                {
                    triggered_spell_id = 37378;
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            switch (dummySpell->Id)
            {
                // Priest Tier 6 Trinket (Ashtongue Talisman of Acumen)
                case 40438:
                {
                    // Shadow Word: Pain
                    if (procSpell->SpellFamilyFlags [0] & 0x8000)
                        triggered_spell_id = 40441;
                    // Renew
                    else if (procSpell->SpellFamilyFlags [0] & 0x40)
                        triggered_spell_id = 40440;
                    else
                        return false;

                    target = this;
                    break;
                }
                // Oracle Healing Bonus ("Garments of the Oracle" set)
                case 26169:
                {
                    // heal amount
                    basepoints0 = int32(CalculatePct(damage, 10));
                    target = this;
                    triggered_spell_id = 26170;
                    break;
                }
                // Frozen Shadoweave (Shadow's Embrace set) warning! its not only priest set
                case 39372:
                {
                    if (!procSpell || (procSpell->GetSchoolMask() & (SPELL_SCHOOL_MASK_FROST | SPELL_SCHOOL_MASK_SHADOW)) == 0)
                        return false;

                    // heal amount
                    basepoints0 = CalculatePct(int32(damage), triggerAmount);
                    target = this;
                    triggered_spell_id = 39373;
                    break;
                }
                // Greater Heal (Vestments of Faith (Priest Tier 3) - 4 pieces bonus)
                case 28809:
                {
                    triggered_spell_id = 28810;
                    break;
                }
                // Priest T10 Healer 2P Bonus
                case 70770:
                    // Flash Heal
                    if (procSpell->SpellFamilyFlags [0] & 0x800)
                    {
                        triggered_spell_id = 70772;
                        SpellInfo const* blessHealing = sSpellMgr->GetSpellInfo(triggered_spell_id);
                        if (!blessHealing)
                            return false;
                        basepoints0 = int32(CalculatePct(damage, triggerAmount) / (blessHealing->GetMaxDuration() / blessHealing->Effects [0].ApplyAuraTickCount));
                    }
                    break;
            }
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            switch (dummySpell->Id)
            {
                // Glyph of Bloodletting
                case 54815:
                {
                    if (!target)
                        return false;

                    // try to find spell Rip on the target
                    if (AuraEffect const* AurEff = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_DRUID, flag128{ 0x00800000 }, GetGUID()))
                    {
                        // Rip's max duration, note: spells which modifies Rip's duration also counted
                        uint32 CountMin = AurEff->GetBase()->GetMaxDuration();

                        // just Rip's max duration without other spells
                        uint32 CountMax = AurEff->GetSpellInfo()->GetMaxDuration();

                        // add possible auras' and Glyph of Shred's max duration
                        CountMax += 3 * triggerAmount * IN_MILLISECONDS;      // Glyph of Bloodletting        -> +6 seconds
                        CountMax += HasAura(60141) ? 4 * IN_MILLISECONDS : 0; // Rip Duration/Lacerate Damage -> +4 seconds

                        // if min < max -> that means caster didn't cast 3 shred yet
                        // so set Rip's duration and max duration
                        if (CountMin < CountMax)
                        {
                            AurEff->GetBase()->SetDuration(AurEff->GetBase()->GetDuration() + triggerAmount * IN_MILLISECONDS);
                            AurEff->GetBase()->SetMaxDuration(CountMin + triggerAmount * IN_MILLISECONDS);
                            return true;
                        }
                    }
                    // if not found Rip
                    return false;
                }
                // Leader of the Pack
                case 24932:
                {
                    if (triggerAmount <= 0)
                        return false;
                    basepoints0 = int32(CountPctFromMaxHealth(triggerAmount));
                    target = this;
                    triggered_spell_id = 34299;
                    // Regenerate 4% mana
                    int32 mana = CalculatePct(GetCreateMana(), triggerAmount);
                    CastCustomSpell(this, 68285, &mana, NULL, NULL, true, castItem, triggeredByAura);
                    break;
                }
                // Healing Touch (Dreamwalker Raiment set)
                case 28719:
                {
                    uint32 cost = context.TriggeredBySpell ? context.TriggeredBySpell->GetPowerCost() : 0;
                    // mana back
                    basepoints0 = int32(CalculatePct(cost, 30));
                    target = this;
                    triggered_spell_id = 28742;
                    break;
                }
                // Healing Touch Refund (Idol of Longevity trinket)
                case 28847:
                {
                    target = this;
                    triggered_spell_id = 28848;
                    break;
                }
                // Mana Restore (Malorne Raiment set / Malorne Regalia set)
                case 37288:
                case 37295:
                {
                    target = this;
                    triggered_spell_id = 37238;
                    break;
                }
                // Druid Tier 6 Trinket
                case 40442:
                {
                    float  chance;

                    // Starfire
                    if (procSpell->SpellFamilyFlags [0] & 0x4)
                    {
                        triggered_spell_id = 40445;
                        chance = 25.0f;
                    }
                    // Rejuvenation
                    else if (procSpell->SpellFamilyFlags [0] & 0x10)
                    {
                        triggered_spell_id = 40446;
                        chance = 25.0f;
                    }
                    // Mangle (Bear) and Mangle (Cat)
                    else if (procSpell->SpellFamilyFlags [1] & 0x00000440)
                    {
                        triggered_spell_id = 40452;
                        chance = 40.0f;
                    }
                    else
                        return false;

                    if (!roll_chance_f(chance))
                        return false;

                    target = this;
                    break;
                }
                // Item - Druid T10 Balance 4P Bonus
                case 70723:
                {
                    // Wrath & Starfire
                    if ((procSpell->SpellFamilyFlags [0] & 0x5) && (procEx & PROC_EX_CRITICAL_HIT))
                    {
                        triggered_spell_id = 71023;
                        SpellInfo const* triggeredSpell = sSpellMgr->GetSpellInfo(triggered_spell_id);
                        if (!triggeredSpell)
                            return false;
                        basepoints0 = CalculatePct(int32(damage), triggerAmount) / (triggeredSpell->GetMaxDuration() / triggeredSpell->Effects [0].ApplyAuraTickCount);
                    }
                    break;
                }
                // Item - Druid T10 Restoration 4P Bonus (Rejuvenation)
                case 70664:
                {
                    // Proc only from normal Rejuvenation
                    if (procSpell->SpellVisual [0] != 32)
                        return false;

                    Player* caster = ToPlayer();
                    if (!caster)
                        return false;
                    if (!caster->GetGroup() && victim == this)
                        return false;

                    CastCustomSpell(70691, SPELLVALUE_BASE_POINT0, damage, victim, true);
                    return true;
                }
            }
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            // Judgements of the Wise
            if (dummySpell->SpellIconID == 3017)
            {
                target = this;
                triggered_spell_id = 31930;
                break;
            }
            switch (dummySpell->Id)
            {
                // Sacred Shield
                case 53601:
                {
                    if (procFlag & PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS)
                        return false;

                    if (damage > 0)
                        triggered_spell_id = 58597;

                    // Item - Paladin T8 Holy 4P Bonus
                    if (Unit* caster = triggeredByAura->GetCaster())
                        if (AuraEffect const* aurEff = caster->GetAuraEffect(64895, 0))
                            cooldown = aurEff->GetAmount();

                    target = this;
                    break;
                }
                // Holy Power (Redemption Armor set)
                case 28789:
                {
                    if (!victim)
                        return false;

                    // Set class defined buff
                    switch (victim->GetClass())
                    {
                        case CLASS_PALADIN:
                        case CLASS_PRIEST:
                        case CLASS_SHAMAN:
                        case CLASS_DRUID:
                            triggered_spell_id = 28795;     // Increases the friendly target's mana regeneration by $s1 per 5 sec. for $d.
                            break;
                        case CLASS_MAGE:
                        case CLASS_WARLOCK:
                            triggered_spell_id = 28793;     // Increases the friendly target's spell damage and healing by up to $s1 for $d.
                            break;
                        case CLASS_HUNTER:
                        case CLASS_ROGUE:
                            triggered_spell_id = 28791;     // Increases the friendly target's attack power by $s1 for $d.
                            break;
                        case CLASS_WARRIOR:
                            triggered_spell_id = 28790;     // Increases the friendly target's armor
                            break;
                        default:
                            return false;
                    }
                    break;
                }
                // Paladin Tier 6 Trinket (Ashtongue Talisman of Zeal)
                case 40470:
                {
                    if (!procSpell)
                        return false;

                    float chance;

                    // Flash of light/Holy light
                    if (procSpell->SpellFamilyFlags [0] & 0xC0000000)
                    {
                        triggered_spell_id = 40471;
                        chance = 15.0f;
                    }
                    // Judgement (any)
                    else if (procSpell->GetSpellSpecific() == SPELL_SPECIFIC_JUDGEMENT)
                    {
                        triggered_spell_id = 40472;
                        chance = 50.0f;
                    }
                    else
                        return false;

                    if (!roll_chance_f(chance))
                        return false;

                    break;
                }
                // Item - Paladin T8 Holy 2P Bonus
                case 64890:
                {
                    triggered_spell_id = 64891;
                    basepoints0 = triggerAmount * damage / 300;
                    break;
                }
                case 71406: // Tiny Abomination in a Jar
                case 71545: // Tiny Abomination in a Jar (Heroic)
                {
                    if (!victim || !victim->IsAlive())
                        return false;

                    CastSpell(this, 71432, true, NULL, triggeredByAura);

                    Aura const* dummy = GetAura(71432);
                    if (!dummy || dummy->GetStackAmount() < (dummySpell->Id == 71406 ? 8 : 7))
                        return false;

                    RemoveAurasDueToSpell(71432);
                    triggered_spell_id = 71433;  // default main hand attack
                    // roll if offhand
                    if (Player const* player = ToPlayer())
                        if (player->GetWeaponForAttack(OFF_ATTACK, true) && urand(0, 1))
                            triggered_spell_id = 71434;
                    target = victim;
                    break;
                }
                // Item - Icecrown 25 Normal Dagger Proc
                case 71880:
                {
                    switch (GetPowerType())
                    {
                        case POWER_MANA:
                            triggered_spell_id = 71881;
                            break;
                        case POWER_RAGE:
                            triggered_spell_id = 71883;
                            break;
                        case POWER_ENERGY:
                            triggered_spell_id = 71882;
                            break;
                        case POWER_RUNIC_POWER:
                            triggered_spell_id = 71884;
                            break;
                        default:
                            return false;
                    }
                    break;
                }
                // Item - Icecrown 25 Heroic Dagger Proc
                case 71892:
                {
                    switch (GetPowerType())
                    {
                        case POWER_MANA:
                            triggered_spell_id = 71888;
                            break;
                        case POWER_RAGE:
                            triggered_spell_id = 71886;
                            break;
                        case POWER_ENERGY:
                            triggered_spell_id = 71887;
                            break;
                        case POWER_RUNIC_POWER:
                            triggered_spell_id = 71885;
                            break;
                        default:
                            return false;
                    }
                    break;
                }
                case 20154:     // Seal of Righteousness
                    if (triggeredByAura->GetEffIndex() == EFFECT_0)
                        triggered_spell_id = 101423;
                    break;
                case 31801:  // Seal of Truth
                {
                    if (effIndex != 0)                      
                        return false;

                    
                    if (procFlag & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG && procSpell->Id != 20271)
                        return false;

                    triggered_spell_id = 31803;

                    int32 l_Bp = 17;

                    if (HasAura(56416))
                        CastCustomSpell(victim, 42463, &l_Bp, NULL, NULL, true);
                    else
                        CastSpell(victim, 42463, true);

                    break;
                }
                case 105361:    // Seal of Command
                    if (triggeredByAura->GetEffIndex() == EFFECT_0)
                        triggered_spell_id = 118215;
                    break;
            }
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            switch (dummySpell->Id)
            {
                // Totemic Power (The Earthshatterer set)
                case 28823:
                {
                    if (!victim)
                        return false;

                    // Set class defined buff
                    switch (victim->GetClass())
                    {
                        case CLASS_PALADIN:
                        case CLASS_PRIEST:
                        case CLASS_SHAMAN:
                        case CLASS_DRUID:
                            triggered_spell_id = 28824;     // Increases the friendly target's mana regeneration by $s1 per 5 sec. for $d.
                            break;
                        case CLASS_MAGE:
                        case CLASS_WARLOCK:
                            triggered_spell_id = 28825;     // Increases the friendly target's spell damage and healing by up to $s1 for $d.
                            break;
                        case CLASS_HUNTER:
                        case CLASS_ROGUE:
                            triggered_spell_id = 28826;     // Increases the friendly target's attack power by $s1 for $d.
                            break;
                        case CLASS_WARRIOR:
                            triggered_spell_id = 28827;     // Increases the friendly target's armor
                            break;
                        default:
                            return false;
                    }
                    break;
                }
                // Lesser Healing Wave (Totem of Flowing Water Relic)
                case 28849:
                {
                    target = this;
                    triggered_spell_id = 28850;
                    break;
                }
                // Shaman Tier 6 Trinket
                case 40463:
                {
                    if (!procSpell)
                        return false;

                    float chance;
                    if (procSpell->SpellFamilyFlags [0] & 0x1)
                    {
                        triggered_spell_id = 40465;         // Lightning Bolt
                        chance = 15.0f;
                    }
                    else if (procSpell->SpellFamilyFlags [0] & 0x80)
                    {
                        triggered_spell_id = 40465;         // Lesser Healing Wave
                        chance = 10.0f;
                    }
                    else if (procSpell->SpellFamilyFlags [1] & 0x00000010)
                    {
                        triggered_spell_id = 40466;         // Stormstrike
                        chance = 50.0f;
                    }
                    else
                        return false;

                    if (!roll_chance_f(chance))
                        return false;

                    target = this;
                    break;
                }
                // Shaman T8 Elemental 4P Bonus
                case 64928:
                {
                    basepoints0 = CalculatePct(int32(damage), triggerAmount);
                    triggered_spell_id = 64930;            // Electrified
                    break;
                }
                // Shaman T9 Elemental 4P Bonus
                case 67228:
                {
                    // Lava Burst
                    if (procSpell->SpellFamilyFlags [1] & 0x1000)
                    {
                        triggered_spell_id = 71824;
                        SpellInfo const* triggeredSpell = sSpellMgr->GetSpellInfo(triggered_spell_id);
                        if (!triggeredSpell)
                            return false;
                        basepoints0 = CalculatePct(int32(damage), triggerAmount) / (triggeredSpell->GetMaxDuration() / triggeredSpell->Effects [0].ApplyAuraTickCount);
                    }
                    break;
                }
                // Item - Shaman T10 Restoration 4P Bonus
                case 70808:
                {
                    // Chain Heal
                    if ((procSpell->SpellFamilyFlags [0] & 0x100) && (procEx & PROC_EX_CRITICAL_HIT))
                    {
                        triggered_spell_id = 70809;
                        SpellInfo const* triggeredSpell = sSpellMgr->GetSpellInfo(triggered_spell_id);
                        if (!triggeredSpell)
                            return false;
                        basepoints0 = CalculatePct(int32(damage), triggerAmount) / (triggeredSpell->GetMaxDuration() / triggeredSpell->Effects [0].ApplyAuraTickCount);
                    }
                    break;
                }
                // Item - Shaman T10 Elemental 2P Bonus
                case 70811:
                {
                    // Lightning Bolt & Chain Lightning
                    if (procSpell->SpellFamilyFlags [0] & 0x3)
                    {
                        if (ToPlayer()->HasSpellCooldown(16166))
                        {
                            ToPlayer()->ModifySpellCooldown(16166, 1000);
                            return true;
                        }
                    }
                    return false;
                }
                // Item - Shaman T10 Elemental 4P Bonus
                case 70817:
                {
                    if (!target)
                        return false;
                    // try to find spell Flame Shock on the target
                    if (AuraEffect const* aurEff = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_SHAMAN, flag128{ 0x10000000 }, GetGUID()))
                    {
                        Aura* flameShock = aurEff->GetBase();
                        int32 maxDuration = flameShock->GetMaxDuration();
                        int32 newDuration = flameShock->GetDuration() + 2 * aurEff->GetAmplitude();

                        flameShock->SetDuration(newDuration);
                        // is it blizzlike to change max duration for FS?
                        if (newDuration > maxDuration)
                            flameShock->SetMaxDuration(newDuration);

                        return true;
                    }
                    // if not found Flame Shock
                    return false;
                }
                break;
            }
            // Frozen Power
            if (dummySpell->SpellIconID == 3780)
            {
                if (!target)
                    return false;
                if (GetDistance(target) < 15.0f)
                    return false;
                float chance = (float) triggerAmount;
                if (!roll_chance_f(chance))
                    return false;

                triggered_spell_id = 63685;
                break;
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // Blood-Caked Blade
            if (dummySpell->SpellIconID == 138)
            {
                if (!target || !target->IsAlive())
                    return false;

                triggered_spell_id = dummySpell->Effects [effIndex].TriggerSpell;
                break;
            }
            // Runic Power Back on Snare/Root
            if (dummySpell->Id == 61257)
            {
                // only for spells and hit/crit (trigger start always) and not start from self casted spells
                if (procSpell == 0 || !(procEx & (PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT)) || this == victim)
                    return false;
                // Need snare or root mechanic
                if (!(procSpell->GetAllEffectsMechanicMask() & ((1 << MECHANIC_ROOT) | (1 << MECHANIC_SNARE))))
                    return false;
                triggered_spell_id = 61258;
                target = this;
                break;
            }
            break;
        }
        case SPELLFAMILY_PET:
        {
            switch (dummySpell->SpellIconID)
            {
                // Guard Dog
                case 201:
                {
                    if (!victim)
                        return false;

                    triggered_spell_id = 54445;
                    target = this;
                    float addThreat = float(CalculatePct(procSpell->Effects [0].CalcValue(this), triggerAmount));
                    victim->AddThreat(this, addThreat);
                    break;
                }
                // Silverback
                case 1582:
                    triggered_spell_id = dummySpell->Id == 62765 ? 62801 : 62800;
                    target = this;
                    break;
            }
            break;
        }
        default:
            break;
    }

    // if not handled by custom case, get triggered spell from dummySpell proto
    if (!triggered_spell_id)
        triggered_spell_id = dummySpell->Effects [triggeredByAura->GetEffIndex()].TriggerSpell;

    // processed charge only counting case
    if (!triggered_spell_id)
        return true;

    SpellInfo const* triggerEntry = sSpellMgr->GetSpellInfo(triggered_spell_id);
    if (!triggerEntry)
    {
        TC_LOG_ERROR("entities.unit", "Unit::HandleDummyAuraProc: Spell %u has non-existing triggered spell %u", dummySpell->Id, triggered_spell_id);
        return false;
    }

    if (basepoints0)
        CastCustomSpell(target, triggered_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura, originalCaster);
    else
        CastSpell(target, triggered_spell_id, true, castItem, triggeredByAura, originalCaster);

    return true;
}

/*
*/


// Used in case when access to whole aura is needed
// All procs should be handled like this...
bool Unit::HandleAuraProc(Unit* victim, uint32 /*damage*/, Aura* triggeredByAura, SpellInfo const* procSpell, uint32 /*procFlag*/, uint32 procEx, uint32 cooldown, bool* handled, ProcTriggerContext const& context)
{
    SpellInfo const* dummySpell = triggeredByAura->GetSpellInfo();

    switch (dummySpell->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
            switch (dummySpell->Id)
            {
                // Nevermelting Ice Crystal
                case 71564:
                    RemoveAuraFromStack(71564);
                    *handled = true;
                    break;
                    // Gaseous Bloat
                case 70672:
                {
                    *handled = true;
                    uint32 stack = triggeredByAura->GetStackAmount();
                    int32 const mod = GetMap()->Is25ManRaid() ? 1500 : 1250;
                    int32 dmg = 0;
                    for (uint8 i = 1; i < stack; ++i)
                        dmg += mod * stack;
                    if (Unit* caster = triggeredByAura->GetCaster())
                        caster->CastCustomSpell(70701, SPELLVALUE_BASE_POINT0, dmg);
                    break;
                }
                // Ball of Flames Proc
                case 71756:
                    RemoveAuraFromStack(dummySpell->Id);
                    *handled = true;
                    break;
                    // Discerning Eye of the Beast
                case 59915:
                {
                    CastSpell(this, 59914, true);   // 59914 already has correct basepoints in DBC, no need for custom bp
                    *handled = true;
                    break;
                }
                // Swift Hand of Justice
                case 59906:
                {
                    int32 bp0 = CalculatePct(GetMaxHealth(), dummySpell->Effects [EFFECT_0].CalcValue());
                    CastCustomSpell(this, 59913, &bp0, NULL, NULL, true);
                    *handled = true;
                    break;
                }
            }

            break;
        case SPELLFAMILY_PALADIN:
        {
            // Judgements of the Just
            if (dummySpell->SpellIconID == 3015)
            {
                *handled = true;
                CastSpell(victim, 68055, true);
                return true;
            }
            // Glyph of Divinity
            else if (dummySpell->Id == 54939)
            {
                if (!procSpell)
                    return false;
                *handled = true;
                // Check if we are the target and prevent mana gain
                if (victim && triggeredByAura->GetCasterGUID() == victim->GetGUID())
                    return false;
                // Lookup base amount mana restore
                for (uint8 i = 0; i < MAX_SPELL_EFFECTS; i++)
                {
                    if (procSpell->Effects [i].Effect == SPELL_EFFECT_ENERGIZE)
                    {
                        // value multiplied by 2 because you should get twice amount
                        int32 mana = procSpell->Effects [i].CalcValue() * 2;
                        CastCustomSpell(this, 54986, 0, &mana, NULL, true);
                    }
                }
                return true;
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            switch (dummySpell->Id)
            {
                // Empowered Fire
                case 31656:
                case 31657:
                case 31658:
                {
                    *handled = true;

                    SpellInfo const* spInfo = sSpellMgr->GetSpellInfo(67545);
                    if (!spInfo)
                        return false;

                    int32 bp0 = int32(CalculatePct(GetCreateMana(), spInfo->Effects [0].CalcValue()));
                    CastCustomSpell(this, 67545, &bp0, NULL, NULL, true, NULL, triggeredByAura->GetEffect(EFFECT_0), GetGUID());
                    return true;
                }
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            switch (dummySpell->Id)
            {
                // Hungering Cold aura drop
                case 51209:
                    *handled = true;
                    // Drop only in not disease case
                    if (procSpell && procSpell->Dispel == DISPEL_DISEASE)
                        return false;
                    break;
            }
            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            switch (dummySpell->Id)
            {
                // Item - Warrior T10 Protection 4P Bonus
                case 70844:
                {
                    int32 basepoints0 = CalculatePct(GetMaxHealth(), dummySpell->Effects [EFFECT_1].CalcValue());
                    CastCustomSpell(this, 70845, &basepoints0, NULL, NULL, true);
                    break;
                }
                // Recklessness
                case 1719:
                {
                    //! Possible hack alert
                    //! Don't drop charges on proc, they will be dropped on SpellMod removal
                    //! Before this change, it was dropping two charges per attack, one in ProcDamageAndSpellFor, and one in RemoveSpellMods.
                    //! The reason of this behaviour is Recklessness having three auras, 2 of them can not proc (isTriggeredAura array) but the other one can, making the whole spell proc.
                    *handled = true;
                    break;
                }
                default:
                    break;
            }
            break;
        }
    }
    return false;
}

bool Unit::HandleProcTriggerSpell(Unit* victim, uint32 damage, AuraEffect* triggeredByAura, SpellInfo const* procSpell, uint32 procFlags, uint32 procEx, uint32 cooldown, ProcTriggerContext const& context)
{
    // Get triggered aura spell info
    SpellInfo const* auraSpellInfo = triggeredByAura->GetSpellInfo();

    // Basepoints of trigger aura
    int32 triggerAmount = triggeredByAura->GetAmount();

    // Set trigger spell id, target, custom basepoints
    uint32 trigger_spell_id = auraSpellInfo->Effects [triggeredByAura->GetEffIndex()].TriggerSpell;

    Unit*  target = NULL;
    int32  basepoints0 = 0;

    if (triggeredByAura->GetAuraType() == SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE)
        basepoints0 = triggerAmount;

    Item* castItem = triggeredByAura->GetBase()->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
        ? ToPlayer()->GetItemByGuid(triggeredByAura->GetBase()->GetCastItemGUID()) : NULL;

    // Try handle unknown trigger spells
    if (sSpellMgr->GetSpellInfo(trigger_spell_id) == NULL)
    {
        switch (auraSpellInfo->SpellFamilyName)
        {
            case SPELLFAMILY_GENERIC:
                switch (auraSpellInfo->Id)
                {
                    case 102726: // Mark of Silence, Asira Dawnslayer, Hour of Twilight
                        if (Unit* pAsira = triggeredByAura->GetCaster())
                            pAsira->CastSpell(this, 103597, true);
                        return true;
                    case 99399: // Burning Wound, Ragnaros, Firelands
                    case 101238:
                    case 101239:
                    case 101240:
                    {
                        uint8 stacks = triggeredByAura->GetBase()->GetStackAmount();
                        int32 bp = urand(1950, 2050);
                        bp *= stacks;
                        CastCustomSpell(victim, 99400, &bp, NULL, NULL, true, NULL, triggeredByAura);
                        return true;
                    }
                    case 99296: // Combustible, Ragnaros (Firelands)
                    case 100282:
                    case 100283:
                    case 100284:
                        GetMotionMaster()->MovementExpired(false);
                        StopMoving();
                        if (victim)
                            CastSpell(victim, 99303, true);
                        if (Creature* pMeteor = ToCreature())
                            pMeteor->AI()->DoAction(4); // ACTION_COMBUSTION
                        break;
                    case 79900: // Unstable Shield, Electron
                    case 91447:
                    case 91448:
                    case 91449:
                        if (victim)
                            CastSpell(victim, 79911, true);
                        break;
                    case 99984: // Slightly Warm Pincers
                        trigger_spell_id = 99987;
                        target = victim;
                        break;
                    case 43820:             // Charm of the Witch Doctor (Amani Charm of the Witch Doctor trinket)
                        // Pct value stored in dummy
                        basepoints0 = victim->GetCreateHealth() * auraSpellInfo->Effects [1].CalcValue() / 100;
                        target = victim;
                        break;
                    case 57345:             // Darkmoon Card: Greatness
                    {
                        float stat = 0.0f;
                        // strength
                        if (GetStat(STAT_STRENGTH) > stat)
                        {
                            trigger_spell_id = 60229; stat = GetStat(STAT_STRENGTH);
                        }
                        // agility
                        if (GetStat(STAT_AGILITY) > stat)
                        {
                            trigger_spell_id = 60233; stat = GetStat(STAT_AGILITY);
                        }
                        // intellect
                        if (GetStat(STAT_INTELLECT) > stat)
                        {
                            trigger_spell_id = 60234; stat = GetStat(STAT_INTELLECT);
                        }
                        // spirit
                        if (GetStat(STAT_SPIRIT) > stat)
                        {
                            trigger_spell_id = 60235;
                        }
                        break;
                    }
                    case 64568:             // Blood Reserve
                    {
                        if (HealthBelowPctDamaged(35, damage))
                        {
                            CastCustomSpell(this, 64569, &triggerAmount, NULL, NULL, true);
                            RemoveAura(64568);
                        }
                        return false;
                    }
                    case 67702:             // Death's Choice, Item - Coliseum 25 Normal Melee Trinket
                    {
                        float stat = 0.0f;
                        // strength
                        if (GetStat(STAT_STRENGTH) > stat)
                        {
                            trigger_spell_id = 67708; stat = GetStat(STAT_STRENGTH);
                        }
                        // agility
                        if (GetStat(STAT_AGILITY) > stat)
                        {
                            trigger_spell_id = 67703;
                        }
                        break;
                    }
                    case 67771:             // Death's Choice (heroic), Item - Coliseum 25 Heroic Melee Trinket
                    {
                        float stat = 0.0f;
                        // strength
                        if (GetStat(STAT_STRENGTH) > stat)
                        {
                            trigger_spell_id = 67773; stat = GetStat(STAT_STRENGTH);
                        }
                        // agility
                        if (GetStat(STAT_AGILITY) > stat)
                        {
                            trigger_spell_id = 67772;
                        }
                        break;
                    }
                    // Mana Drain Trigger
                    case 27522:
                    case 40336:
                    {
                        // On successful melee or ranged attack gain $29471s1 mana and if possible drain $27526s1 mana from the target.
                        if (IsAlive())
                            CastSpell(this, 29471, true, castItem, triggeredByAura);
                        if (victim && victim->IsAlive())
                            CastSpell(victim, 27526, true, castItem, triggeredByAura);
                        return true;
                    }
                    // Evasive Maneuvers
                    case 50240:
                    {
                        // Remove a Evasive Charge
                        Aura* charge = GetAura(50241);
                        if (charge && charge->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL))
                            RemoveAurasDueToSpell(50240);
                        break;
                    }
                    // Ethereal Pet Aura
                    case 50051:
                        if (victim)
                            if (ObjectGuid critterGuid = GetCritterGUID())
                                if (Creature* critter = ObjectAccessor::GetCreature(*this, critterGuid))
                                    if (critter->GetEntry() == 27914) // Ethereal Soul-Trader
                                        victim->CastSpell(critter, 50050, true); // Ethereal Pet Aura OnKill
                        return true;
                }
                break;
            case SPELLFAMILY_DRUID:
            {
                switch (auraSpellInfo->Id)
                {
                    // Druid Forms Trinket
                    case 37336:
                    {
                        switch (GetShapeshiftForm())
                        {
                            case FORM_NONE:     trigger_spell_id = 37344; break;
                            case FORM_CAT:      trigger_spell_id = 37341; break;
                            case FORM_BEAR:     trigger_spell_id = 37340; break;
                            case FORM_TREE:     trigger_spell_id = 37342; break;
                            case FORM_MOONKIN:  trigger_spell_id = 37343; break;
                            default:
                                return false;
                        }
                        break;
                    }
                    // Druid T9 Feral Relic (Lacerate, Swipe, Mangle, and Shred)
                    case 67353:
                    {
                        switch (GetShapeshiftForm())
                        {
                            case FORM_CAT:      trigger_spell_id = 67355; break;
                            case FORM_BEAR:     trigger_spell_id = 67354; break;
                            default:
                                return false;
                        }
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case SPELLFAMILY_HUNTER:
            {
                // Item - Hunter T9 4P Bonus
                if (auraSpellInfo->Id == 67151)
                {
                    trigger_spell_id = 68130;
                    target = this;
                    break;
                }
                break;
            }
            case SPELLFAMILY_PALADIN:
            {
                switch (auraSpellInfo->Id)
                {
                    // Soul Preserver
                    case 60510:
                    {
                        switch (GetClass())
                        {
                            case CLASS_DRUID:
                                trigger_spell_id = 60512;
                                break;
                            case CLASS_PALADIN:
                                trigger_spell_id = 60513;
                                break;
                            case CLASS_PRIEST:
                                trigger_spell_id = 60514;
                                break;
                            case CLASS_SHAMAN:
                                trigger_spell_id = 60515;
                                break;
                        }

                        target = this;
                        break;
                    }
                    case 37657: // Lightning Capacitor
                    case 54841: // Thunder Capacitor
                    case 67712: // Item - Coliseum 25 Normal Caster Trinket
                    case 67758: // Item - Coliseum 25 Heroic Caster Trinket
                    {
                        if (!victim || !victim->IsAlive() || GetTypeId() != TYPEID_PLAYER)
                            return false;

                        uint32 stack_spell_id = 0;
                        switch (auraSpellInfo->Id)
                        {
                            case 37657:
                                stack_spell_id = 37658;
                                trigger_spell_id = 37661;
                                break;
                            case 54841:
                                stack_spell_id = 54842;
                                trigger_spell_id = 54843;
                                break;
                            case 67712:
                                stack_spell_id = 67713;
                                trigger_spell_id = 67714;
                                break;
                            case 67758:
                                stack_spell_id = 67759;
                                trigger_spell_id = 67760;
                                break;
                        }

                        CastSpell(this, stack_spell_id, true, NULL, triggeredByAura);

                        Aura* dummy = GetAura(stack_spell_id);
                        if (!dummy || dummy->GetStackAmount() < triggerAmount)
                            return false;

                        RemoveAurasDueToSpell(stack_spell_id);
                        target = victim;
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case SPELLFAMILY_DEATHKNIGHT:
            {
                // Item - Death Knight T10 Melee 4P Bonus
                if (auraSpellInfo->Id == 70656)
                {
                    if (GetTypeId() != TYPEID_PLAYER || GetClass() != CLASS_DEATH_KNIGHT)
                        return false;

                    for (uint8 i = 0; i < MAX_RUNES; ++i)
                        if (!ToPlayer()->HasRuneCooldown(i))
                            return false;
                }
                break;
            }
            case SPELLFAMILY_ROGUE:
            {
                switch (auraSpellInfo->Id)
                {
                    // Rogue T10 2P bonus, should only proc on caster
                    case 70805:
                    {
                        if (victim != this)
                            return false;
                        break;
                    }
                }
                break;
            }
            default:
                break;
        }
    }

    // All ok. Check current trigger spell
    SpellInfo const* triggerEntry = sSpellMgr->GetSpellInfo(trigger_spell_id);
    if (triggerEntry == NULL)
    {
        // Don't cast unknown spell
        // TC_LOG_ERROR("entities.unit", "Unit::HandleProcTriggerSpell: Spell %u has 0 in EffectTriggered[%d]. Unhandled custom case?", auraSpellInfo->Id, triggeredByAura->GetEffIndex());
        return false;
    }

    // not allow proc extra attack spell at extra attack
    if (m_extraAttacks && triggerEntry->HasEffect(SPELL_EFFECT_ADD_EXTRA_ATTACKS))
        return false;

    // Custom requirements (not listed in procEx) Warning! damage dealing after this
    // Custom triggered spells
    switch (auraSpellInfo->Id)
    {
        // Fusing Vapors, Yor'sahj, Dragon Soul
        case 103968:
            if (GetHealthPct() > 50.0f)
                return false;
            break;
        // Embedded Blade, Mannoroth, Well of Eternity
        case 109542:
            if (!victim)
                return false;
            target = victim;
            break;
        // Burnout, Alysrazor
        case 99432:
            if (!victim)
                return false;
            target = victim;
            break;
        // Molten Axe, Echo of Baine
        case 101836:
            if (!victim)
                return false;
            target = victim;
            break;
        // Molten Fists, Echo of Baine
        case 101866:
            if (!victim)
                return false;
            target = victim;
            break;
            // Frostburn Formula
        case 96325:
            if (!victim || victim->GetLevel() > 85)
                return true;
            break;
        // Deep Wounds
        case 12834:
        case 12849:
        case 12867:
        {
            if (GetTypeId() != TYPEID_PLAYER)
                return false;

            // now compute approximate weapon damage by formula from wowwiki.com
            Item* item = NULL;
            if (procFlags & PROC_FLAG_DONE_OFFHAND_ATTACK)
                item = ToPlayer()->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            else
                item = ToPlayer()->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);

            // dunno if it's really needed but will prevent any possible crashes
            if (!item)
                return false;

            ItemTemplate const* weapon = item->GetTemplate();

            float weaponDPS = weapon->DPS;
            float attackPower = GetTotalAttackPowerValue(BASE_ATTACK) / 14.0f;
            float weaponSpeed = float(weapon->Delay) / 1000.0f;
            basepoints0 = int32((weaponDPS + attackPower) * weaponSpeed);
            break;
        }
        // Persistent Shield (Scarab Brooch trinket)
        // This spell originally trigger 13567 - Dummy Trigger (vs dummy efect)
        case 26467:
        {
            basepoints0 = int32(CalculatePct(damage, 15));
            target = victim;
            trigger_spell_id = 26470;
            break;
        }
        // Unyielding Knights (item exploit 29108\29109)
        case 38164:
        {
            if (!victim || victim->GetEntry() != 19457)  // Proc only if your target is Grillok
                return false;
            break;
        }
        // Deflection
        case 52420:
        {
            if (!HealthBelowPctDamaged(35, damage))
                return false;
            break;
        }

        // Cheat Death
        case 28845:
        {
            // When your health drops below 20%
            if (HealthBelowPctDamaged(20, damage) || HealthBelowPct(20))
                return false;
            break;
        }
        // Greater Heal Refund (Avatar Raiment set)
        case 37594:
        {
            if (!victim || !victim->IsAlive())
                return false;

            // Doesn't proc if target already has full health
            if (victim->IsFullHealth())
                return false;
            // If your Greater Heal brings the target to full health, you gain $37595s1 mana.
            if (victim->GetHealth() + damage < victim->GetMaxHealth())
                return false;
            break;
        }
        // Bonus Healing (Crystal Spire of Karabor mace)
        case 40971:
        {
            // If your target is below $s1% health
            if (!victim || !victim->IsAlive() || victim->HealthAbovePct(triggerAmount))
                return false;
            break;
        }
        // Decimation
        case 63156:
        case 63158:
            // Can proc only if target has hp below 25%
            if (!victim || !victim->HealthBelowPct(auraSpellInfo->Effects [EFFECT_1].CalcValue()))
                return false;
            break;
            // Deathbringer Saurfang - Blood Beast's Blood Link
        case 72176:
            basepoints0 = 3;
            break;
            // Professor Putricide - Ooze Spell Tank Protection
        case 71770:
            if (victim)
                victim->CastSpell(victim, trigger_spell_id, true);    // EffectImplicitTarget is self
            return true;
        case 45057: // Evasive Maneuvers (Commendation of Kael`thas trinket)
        case 71634: // Item - Icecrown 25 Normal Tank Trinket 1
        case 71640: // Item - Icecrown 25 Heroic Tank Trinket 1
        case 75475: // Item - Chamber of Aspects 25 Normal Tank Trinket
        case 75481: // Item - Chamber of Aspects 25 Heroic Tank Trinket
        {
            // Procs only if damage takes health below $s1%
            if (!HealthBelowPctDamaged(triggerAmount, damage))
                return false;
            break;
        }
        default:
            break;
    }

    // Custom basepoints/target for exist spell
    // dummy basepoints or other customs
    switch (trigger_spell_id)
    {
        // Auras which should proc on area aura source (caster in this case):
        // Cast positive spell on enemy target
        case 7099:  // Curse of Mending
        case 39703: // Curse of Mending
        case 29494: // Temptation
        case 20233: // Improved Lay on Hands (cast on target)
        {
            target = victim;
            break;
        }
        // Finish movies that add combo
        case 14189: // Seal Fate (Netherblade set)
        case 14157: // Ruthlessness
        {
            if (!victim || victim == this)
                return false;
            // Need add combopoint AFTER finish movie (or they dropped in finish phase)
            break;
        }
        // Item - Druid T10 Balance 2P Bonus
        case 16870:
        {
            if (HasAura(70718))
                CastSpell(this, 70721, true);
            break;
        }
        // Enlightenment (trigger only from mana cost spells)
        case 35095:
        {
            if (!context.TriggeredBySpell || context.TriggeredBySpell->GetPowerType() != POWER_MANA || !context.TriggeredBySpell->GetPowerCost())
                return false;
            break;
        }
        case 46916:  // Slam! (Bloodsurge proc)
        case 52437:  // Sudden Death
        {
            // Item - Warrior T10 Melee 4P Bonus
            if (AuraEffect const* aurEff = GetAuraEffect(70847, 0))
            {
                if (!roll_chance_i(aurEff->GetAmount()))
                    break;
                CastSpell(this, 70849, true, castItem, triggeredByAura); // Extra Charge!
                CastSpell(this, 71072, true, castItem, triggeredByAura); // Slam GCD Reduced
                CastSpell(this, 71069, true, castItem, triggeredByAura); // Execute GCD Reduced
            }
            break;
        }
        // Maelstrom Weapon
        case 53817:
        {
            // Item - Shaman T10 Enhancement 4P Bonus
            if (AuraEffect const* aurEff = GetAuraEffect(70832, 0))
                if (Aura const* maelstrom = GetAura(53817))
                    if ((maelstrom->GetStackAmount() == maelstrom->GetSpellInfo()->StackAmount - 1) && roll_chance_i(aurEff->GetAmount()))
                        CastSpell(this, 70831, true, castItem, triggeredByAura);
            break;
        }
        // Savage Defense
        case 62606:
        {
            basepoints0 = CalculatePct(triggerAmount, GetTotalAttackPowerValue(BASE_ATTACK));
            break;
        }
        // Body and Soul
        case 64128:
        case 65081:
        {
            // Proc only from PW:S cast
            if (!(procSpell->SpellFamilyFlags [0] & 0x00000001))
                return false;
            break;
        }
        // Culling the Herd
        case 70893:
        {
            // check if we're doing a critical hit
            if (!(procSpell->SpellFamilyFlags [1] & 0x10000000) && (procEx != PROC_EX_CRITICAL_HIT))
                return false;
            // check if we're procced by Claw, Bite or Smack (need to use the spell icon ID to detect it)
            if (!(procSpell->SpellIconID == 262 || procSpell->SpellIconID == 1680 || procSpell->SpellIconID == 473))
                return false;
            break;
        }
    }

    // try detect target manually if not set
    if (target == NULL)
        target = !(procFlags & (PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS)) && triggerEntry && triggerEntry->IsPositive() ? this : victim;

    if (basepoints0)
        CastCustomSpell(target, trigger_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);
    else
        CastSpell(target, trigger_spell_id, true, castItem, triggeredByAura);

    return true;
}

bool Unit::HandleOverrideClassScriptAuraProc(Unit* victim, uint32 /*damage*/, AuraEffect* triggeredByAura, SpellInfo const* /*procSpell*/, uint32 cooldown, ProcTriggerContext const& context)
{
    int32 scriptId = triggeredByAura->GetMiscValue();

    if (!victim || !victim->IsAlive())
        return false;

    Item* castItem = triggeredByAura->GetBase()->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
        ? ToPlayer()->GetItemByGuid(triggeredByAura->GetBase()->GetCastItemGUID()) : NULL;

    uint32 triggered_spell_id = 0;

    switch (scriptId)
    {
        case 4533:                                          // Dreamwalker Raiment 2 pieces bonus
        {
            // Chance 50%
            if (!roll_chance_i(50))
                return false;

            switch (victim->GetPowerType())
            {
                case POWER_MANA:   triggered_spell_id = 28722; break;
                case POWER_RAGE:   triggered_spell_id = 28723; break;
                case POWER_ENERGY: triggered_spell_id = 28724; break;
                default:
                    return false;
            }
            break;
        }
        case 4537:                                          // Dreamwalker Raiment 6 pieces bonus
            triggered_spell_id = 28750;                     // Blessing of the Claw
            break;
        default:
            break;
    }

    // not processed
    if (!triggered_spell_id)
        return false;

    // standard non-dummy case
    SpellInfo const* triggerEntry = sSpellMgr->GetSpellInfo(triggered_spell_id);

    if (!triggerEntry)
    {
        TC_LOG_ERROR("entities.unit", "Unit::HandleOverrideClassScriptAuraProc: Spell %u triggering for class script id %u", triggered_spell_id, scriptId);
        return false;
    }

    CastSpell(victim, triggered_spell_id, true, castItem, triggeredByAura);

    return true;
}

void Unit::SetPowerType(Powers new_powertype)
{
    SetFieldPowerType(new_powertype);

    if (GetTypeId() == TYPEID_PLAYER)
    {
        if (ToPlayer()->GetGroup())
            ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_POWER_TYPE);
    }
    else if (Pet* pet = ToCreature()->ToPet())
    {
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_POWER_TYPE);
        }
    }

    switch (new_powertype)
    {
        default:
        case POWER_MANA:
            break;
        case POWER_RAGE:
            SetMaxPower(POWER_RAGE, GetCreatePowers(POWER_RAGE));
            SetPower(POWER_RAGE, 0);
            break;
        case POWER_FOCUS:
            SetMaxPower(POWER_FOCUS, GetCreatePowers(POWER_FOCUS));
            SetPower(POWER_FOCUS, GetCreatePowers(POWER_FOCUS));
            break;
        case POWER_ENERGY:
            SetMaxPower(POWER_ENERGY, GetCreatePowers(POWER_ENERGY));
            break;
    }
}

FactionTemplateEntry const* Unit::GetFactionTemplateEntry() const
{
    FactionTemplateEntry const* entry = sFactionTemplateStore.LookupEntry(GetFaction());
    if (!entry)
    {
        static uint64 guid = 0;                             // prevent repeating spam same faction problem

        if (GetGUID() != guid)
        {
            if (Player const* player = ToPlayer())
                TC_LOG_ERROR("entities.unit", "Player %s has invalid faction (faction template id) #%u", player->GetName().c_str(), GetFaction());
            else if (Creature const* creature = ToCreature())
                TC_LOG_ERROR("entities.unit", "Creature (template id: %u) has invalid faction (faction template id) #%u", creature->GetCreatureTemplate()->Entry, GetFaction());
            else
                TC_LOG_ERROR("entities.unit", "Unit (name=%s, type=%u) has invalid faction (faction template id) #%u", GetName().c_str(), uint32(GetTypeId()), GetFaction());

            guid = GetGUID();
        }
    }
    return entry;
}

// function based on function Unit::UnitReaction from 13850 client
ReputationRank Unit::GetReactionTo(Unit const* target) const
{
    // always friendly to self
    if (this == target)
        return REP_FRIENDLY;

    if (!target)
        return REP_FRIENDLY;

    // always friendly to charmer or owner
    if (GetCharmerOrOwnerOrSelf() == target->GetCharmerOrOwnerOrSelf())
        return REP_FRIENDLY;

    // Possible we must handle UNIT_BYTE2_FLAG_UNK1 because it causes client to make target attackable
    {
        const uint32 HATES_EVERYTHING = 1145;
        if (FactionTemplateEntry const* factionTemplate = GetFactionTemplateEntry())
            if (factionTemplate->faction == HATES_EVERYTHING)
                return REP_HOSTILE;

        if (FactionTemplateEntry const* factionTemplate = target->GetFactionTemplateEntry())
            if (factionTemplate->faction == HATES_EVERYTHING)
                return REP_HOSTILE;
    }

    Player const* selfPlayerOwner = GetAffectingPlayer();
    Player const* targetPlayerOwner = target->GetAffectingPlayer();

    // check forced reputation to support SPELL_AURA_FORCE_REACTION
    if (selfPlayerOwner)
    {
        if (FactionTemplateEntry const* targetFactionTemplateEntry = target->GetFactionTemplateEntry())
            if (ReputationRank const* repRank = selfPlayerOwner->GetReputationMgr().GetForcedRankIfAny(targetFactionTemplateEntry))
                return *repRank;
    }
    else if (targetPlayerOwner)
    {
        if (FactionTemplateEntry const* selfFactionTemplateEntry = GetFactionTemplateEntry())
            if (ReputationRank const* repRank = targetPlayerOwner->GetReputationMgr().GetForcedRankIfAny(selfFactionTemplateEntry))
                return *repRank;
    }

    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
    {
        if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
        {
            if (selfPlayerOwner && targetPlayerOwner)
            {
                // always friendly to other unit controlled by player, or to the player himself
                if (selfPlayerOwner == targetPlayerOwner)
                    return REP_FRIENDLY;

                // duel - always hostile to opponent
                if (selfPlayerOwner->duel && selfPlayerOwner->duel->opponent == targetPlayerOwner && selfPlayerOwner->duel->startTime != 0)
                    return REP_HOSTILE;

                if (GetMapId() == 37 && sWorld->getBoolConfig(CONFIG_ICORE_ROYALE_EVENT_ENABLED))
                    return REP_HOSTILE;

                // same group - checks dependant only on our faction - skip FFA_PVP for example
                if (selfPlayerOwner->IsInRaidWith(targetPlayerOwner))
                    return REP_FRIENDLY; // return true to allow config option AllowTwoSide.Interaction.Group to work
                // however client seems to allow mixed group parties, because in 13850 client it works like:
                // return GetFactionReactionTo(GetFactionTemplateEntry(), target);
            }

            // check FFA_PVP
            if (GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1) & UNIT_BYTE2_FLAG_FFA_PVP
                && target->GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1) & UNIT_BYTE2_FLAG_FFA_PVP)
                return REP_HOSTILE;

            if (selfPlayerOwner)
            {
                if (FactionTemplateEntry const* targetFactionTemplateEntry = target->GetFactionTemplateEntry())
                {
                    if (!selfPlayerOwner->HasFlag(UNIT_FIELD_FLAGS2, UNIT_FLAG2_IGNORE_REPUTATION))
                    {
                        if (FactionEntry const* targetFactionEntry = sFactionStore.LookupEntry(targetFactionTemplateEntry->faction))
                        {
                            if (targetFactionEntry->CanHaveReputation())
                            {
                                // check contested flags
                                if (targetFactionTemplateEntry->factionFlags & FACTION_TEMPLATE_FLAG_CONTESTED_GUARD
                                    && selfPlayerOwner->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP))
                                    return REP_HOSTILE;

                                // if faction has reputation, hostile state depends only from AtWar state
                                if (selfPlayerOwner->GetReputationMgr().IsAtWar(targetFactionEntry))
                                    return REP_HOSTILE;
                                return REP_FRIENDLY;
                            }
                        }
                    }
                }
            }
        }
    }
    // do checks dependant only on our faction
    return GetFactionReactionTo(GetFactionTemplateEntry(), target);
}

ReputationRank Unit::GetFactionReactionTo(FactionTemplateEntry const* factionTemplateEntry, Unit const* target)
{
    // always neutral when no template entry found
    if (!factionTemplateEntry)
        return REP_NEUTRAL;

    FactionTemplateEntry const* targetFactionTemplateEntry = target->GetFactionTemplateEntry();
    if (!targetFactionTemplateEntry)
        return REP_NEUTRAL;

    if (Player const* targetPlayerOwner = target->GetAffectingPlayer())
    {
        // check contested flags
        if (factionTemplateEntry->factionFlags & FACTION_TEMPLATE_FLAG_CONTESTED_GUARD
            && targetPlayerOwner->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP))
            return REP_HOSTILE;
        if (ReputationRank const* repRank = targetPlayerOwner->GetReputationMgr().GetForcedRankIfAny(factionTemplateEntry))
            return *repRank;
        if (!target->HasFlag(UNIT_FIELD_FLAGS2, UNIT_FLAG2_IGNORE_REPUTATION))
        {
            if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionTemplateEntry->faction))
            {
                if (factionEntry->CanHaveReputation())
                {
                    // CvP case - check reputation, don't allow state higher than neutral when at war
                    ReputationRank repRank = targetPlayerOwner->GetReputationMgr().GetRank(factionEntry);
                    if (targetPlayerOwner->GetReputationMgr().IsAtWar(factionEntry))
                        repRank = std::min(REP_NEUTRAL, repRank);
                    return repRank;
                }
            }
        }
    }

    // common faction based check
    if (factionTemplateEntry->IsHostileTo(*targetFactionTemplateEntry))
        return REP_HOSTILE;
    if (factionTemplateEntry->IsFriendlyTo(*targetFactionTemplateEntry))
        return REP_FRIENDLY;
    if (targetFactionTemplateEntry->IsFriendlyTo(*factionTemplateEntry))
        return REP_FRIENDLY;
    if (factionTemplateEntry->factionFlags & FACTION_TEMPLATE_FLAG_HOSTILE_BY_DEFAULT)
        return REP_HOSTILE;
    // neutral by default
    return REP_NEUTRAL;
}

bool Unit::IsHostileTo(Unit const* unit) const
{
    return GetReactionTo(unit) <= REP_HOSTILE;
}

bool Unit::IsFriendlyTo(Unit const* unit) const
{
    return GetReactionTo(unit) >= REP_FRIENDLY;
}

bool Unit::IsHostileToPlayers() const
{
    FactionTemplateEntry const* my_faction = GetFactionTemplateEntry();
    if (!my_faction || !my_faction->faction)
        return false;

    FactionEntry const* raw_faction = sFactionStore.LookupEntry(my_faction->faction);
    if (raw_faction && raw_faction->reputationListID >= 0)
        return false;

    return my_faction->IsHostileToPlayers();
}

bool Unit::IsNeutralToAll() const
{
    FactionTemplateEntry const* my_faction = GetFactionTemplateEntry();
    if (!my_faction || !my_faction->faction)
        return true;

    FactionEntry const* raw_faction = sFactionStore.LookupEntry(my_faction->faction);
    if (raw_faction && raw_faction->reputationListID >= 0)
        return false;

    return my_faction->IsNeutralToAll();
}

void Unit::_addAttacker(Unit* pAttacker)
{
    m_attackers.insert(pAttacker);
}

void Unit::_removeAttacker(Unit* pAttacker)
{
    m_attackers.erase(pAttacker);
}

Unit* Unit::getAttackerForHelper() const                 // If someone wants to help, who to give them
{
    if (GetVictim() != NULL)
        return GetVictim();

    if (!m_attackers.empty())
        return *(m_attackers.begin());

    return NULL;
}

bool Unit::Attack(Unit* victim, bool meleeAttack)
{
    if (!victim || victim == this
        // dead units can neither attack nor be attacked
        || !IsAlive() || !victim->IsInWorld() || !victim->IsAlive()
        // player cannot attack in mount state
        || GetTypeId() == TYPEID_PLAYER && IsMounted()
        // nobody can attack GM in GM-mode or evading creature
        || (victim->GetTypeId() == TYPEID_PLAYER ? victim->ToPlayer()->IsGameMaster() : victim->ToCreature()->IsInEvadeMode()))
    {
        // Send update to the player, otherwise he might be stuck with Auto Attack on but not actually hitting anybody
        if (GetTypeId() == TYPEID_PLAYER)
            SendMeleeAttackStop(victim);

        return false;
    }

    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED))
        return false;

    if (GetTypeId() == TYPEID_UNIT && !IsPet() && ToCreature()->IsInEvadeMode())
        return false;

    // remove SPELL_AURA_MOD_UNATTACKABLE at attack (in case non-interruptible spells stun aura applied also that not let attack)
    if (HasAuraType(SPELL_AURA_MOD_UNATTACKABLE))
        RemoveAurasByType(SPELL_AURA_MOD_UNATTACKABLE);

    if (m_attacking)
    {
        if (m_attacking == victim)
        {
            // switch to melee attack from ranged/magic
            if (meleeAttack)
            {
                if (!HasUnitState(UNIT_STATE_MELEE_ATTACKING))
                {
                    AddUnitState(UNIT_STATE_MELEE_ATTACKING);
                    SendMeleeAttackStart(victim);
                    return true;
                }
            }
            else if (HasUnitState(UNIT_STATE_MELEE_ATTACKING))
            {
                ClearUnitState(UNIT_STATE_MELEE_ATTACKING);
                SendMeleeAttackStop(victim);
                return true;
            }
            return false;
        }

        // switch target
        InterruptSpell(CURRENT_MELEE_SPELL);
        if (!meleeAttack)
            ClearUnitState(UNIT_STATE_MELEE_ATTACKING);
    }

    if (m_attacking)
        m_attacking->_removeAttacker(this);

    m_attacking = victim;
    m_attacking->_addAttacker(this);

    // Set our target
    SetTarget(victim->GetGUID());

    if (meleeAttack)
        AddUnitState(UNIT_STATE_MELEE_ATTACKING);

    // set position before any AI calls/assistance
    //if (GetTypeId() == TYPEID_UNIT)
    //    ToCreature()->SetCombatStartPosition(GetPositionX(), GetPositionY(), GetPositionZ());

    if (GetTypeId() == TYPEID_UNIT && !ToCreature()->IsPet())
    {
        // should not let player enter combat by right clicking target - doesn't helps
        SetInCombatWith(victim);
        if (victim->GetTypeId() == TYPEID_PLAYER)
            victim->SetInCombatWith(this);
        AddThreat(victim, 0.0f);

        ToCreature()->SendAIReaction(AI_REACTION_HOSTILE);
        ToCreature()->CallAssistance();
    }

    // delay offhand weapon attack to next attack time
    if (HasOffhandWeapon())
        resetAttackTimer(OFF_ATTACK);

    if (meleeAttack)
        SendMeleeAttackStart(victim);

    // Let the pet know we've started attacking someting. Handles melee attacks only
    // Spells such as auto-shot and others handled in WorldSession::HandleCastSpellOpcode
    if (this->GetTypeId() == TYPEID_PLAYER)
    {
        Pet* playerPet = this->ToPlayer()->GetPet();

        if (playerPet && playerPet->IsAlive())
            playerPet->AI()->OwnerAttacked(victim);
    }

    return true;
}

bool Unit::AttackStop()
{
    if (!m_attacking)
        return false;

    Unit* victim = m_attacking;

    m_attacking->_removeAttacker(this);
    m_attacking = NULL;

    // Clear our target
    SetTarget(ObjectGuid::Empty);

    ClearUnitState(UNIT_STATE_MELEE_ATTACKING);

    InterruptSpell(CURRENT_MELEE_SPELL);

    // reset only at real combat stop
    if (Creature* creature = ToCreature())
    {
        creature->SetNoCallAssistance(false);

        if (creature->HasSearchedAssistance())
        {
            creature->SetNoSearchAssistance(false);
            UpdateSpeed(MOVE_RUN, false);
        }
    }

    SendMeleeAttackStop(victim);

    return true;
}

void Unit::CombatStop(bool includingCast)
{
    if (includingCast && IsNonMeleeSpellCasted(false))
        InterruptNonMeleeSpells(false);

    AttackStop();
    RemoveAllAttackers();
    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->SendAttackSwingCancelAttack();     // melee and ranged forced attack cancel
    ClearInCombat();
}

void Unit::CombatStopWithPets(bool includingCast)
{
    CombatStop(includingCast);

    for (ControlList::const_iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
        (*itr)->CombatStop(includingCast);
}

bool Unit::isAttackingPlayer() const
{
    if (HasUnitState(UNIT_STATE_ATTACK_PLAYER))
        return true;

    for (ControlList::const_iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
        if ((*itr)->isAttackingPlayer())
            return true;

    for (uint8 i = 0; i < SUMMON_SLOT_MAX; ++i)
        if (m_SummonSlot [i])
            if (Creature* summon = GetMap()->GetCreature(m_SummonSlot [i]))
                if (summon->isAttackingPlayer())
                    return true;

    return false;
}

void Unit::RemoveAllAttackers()
{
    while (!m_attackers.empty())
    {
        AttackerSet::iterator iter = m_attackers.begin();
        if (!(*iter)->AttackStop())
        {
            TC_LOG_ERROR("entities.unit", "WORLD: Unit has an attacker that isn't attacking it!");
            m_attackers.erase(iter);
        }
    }
}

void Unit::ModifyAuraState(AuraStateType flag, bool apply)
{
    if (apply)
    {
        if (!HasFlag(UNIT_FIELD_AURA_STATE, 1 << (flag - 1)))
        {
            SetFlag(UNIT_FIELD_AURA_STATE, 1 << (flag - 1));
            if (GetTypeId() == TYPEID_PLAYER)
            {
                PlayerSpellMap const& sp_list = ToPlayer()->GetSpellMap();
                for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
                {
                    if (itr->second->state == PLAYERSPELL_REMOVED || itr->second->disabled)
                        continue;
                    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(itr->first);
                    if (!spellInfo || !spellInfo->IsPassive())
                        continue;
                    if (spellInfo->CasterAuraState == uint32(flag))
                        CastSpell(this, itr->first, true, NULL);
                }
            }
            else if (Pet* pet = ToCreature()->ToPet())
            {
                for (auto&& itr : pet->m_spells)
                {
                    if (itr.second.state == PETSPELL_REMOVED)
                        continue;
                    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(itr.first);
                    if (!spellInfo || !spellInfo->IsPassive())
                        continue;
                    if (spellInfo->CasterAuraState == uint32(flag))
                        CastSpell(this, itr.first, true, NULL);
                }
            }
        }
    }
    else
    {
        if (HasFlag(UNIT_FIELD_AURA_STATE, 1 << (flag - 1)))
        {
            RemoveFlag(UNIT_FIELD_AURA_STATE, 1 << (flag - 1));

            Unit::AuraApplicationMap& tAuras = GetAppliedAuras();
            for (Unit::AuraApplicationMap::iterator itr = tAuras.begin(); itr != tAuras.end();)
            {
                SpellInfo const* spellProto = (*itr).second->GetBase()->GetSpellInfo();
                if (spellProto->CasterAuraState == uint32(flag))
                    RemoveAura(itr);
                else
                    ++itr;
            }
        }
    }
}

uint32 Unit::BuildAuraStateUpdateForTarget(Unit* target) const
{
    uint32 auraStates = GetUInt32Value(UNIT_FIELD_AURA_STATE) &~(PER_CASTER_AURA_STATE_MASK);
    for (AuraStateAurasMap::const_iterator itr = m_auraStateAuras.begin(); itr != m_auraStateAuras.end(); ++itr)
        if ((1 << (itr->first - 1)) & PER_CASTER_AURA_STATE_MASK)
            if (itr->second->GetBase()->GetCasterGUID() == target->GetGUID())
                auraStates |= (1 << (itr->first - 1));

    return auraStates;
}

bool Unit::HasAuraState(AuraStateType flag, SpellInfo const* spellProto, Unit const* caster) const
{
    if (caster)
    {
        if (spellProto)
        {
            AuraEffectList const& stateAuras = caster->GetAuraEffectsByType(SPELL_AURA_ABILITY_IGNORE_AURASTATE);
            for (AuraEffectList::const_iterator j = stateAuras.begin(); j != stateAuras.end(); ++j)
                if ((*j)->IsAffectingSpell(spellProto))
                    return true;
        }
        // Check per caster aura state
        // If aura with aurastate by caster not found return false
        if ((1 << (flag - 1)) & PER_CASTER_AURA_STATE_MASK)
        {
            AuraStateAurasMapBounds range = m_auraStateAuras.equal_range(flag);
            for (AuraStateAurasMap::const_iterator itr = range.first; itr != range.second; ++itr)
                if (itr->second->GetBase()->GetCasterGUID() == caster->GetGUID())
                    return true;
            return false;
        }
        if (spellProto && caster->IcyVeinsHack && (spellProto->Id == 131080 || spellProto->Id == 131081))
            return true;
    }

    return HasFlag(UNIT_FIELD_AURA_STATE, 1 << (flag - 1));
}

void Unit::SetOwnerGUID(ObjectGuid owner)
{
    if (GetOwnerGUID() == owner)
        return;

    SetGuidValue(UNIT_FIELD_SUMMONED_BY, owner);
    if (!owner)
        return;

    // Update owner dependent fields
    Player* player = ObjectAccessor::GetPlayer(*this, owner);
    if (!player || !player->HaveAtClient(this)) // if player cannot see this unit yet, he will receive needed data with create object
        return;

    SetFieldNotifyFlag(UF_FLAG_OWNER);

    UpdateData udata(GetMapId());
    WorldPacket packet;
    BuildValuesUpdateBlockForPlayer(&udata, player);
    udata.BuildPacket(&packet);
    player->SendDirectMessage(&packet);

    RemoveFieldNotifyFlag(UF_FLAG_OWNER);
}

Unit* Unit::GetOwner() const
{
    if (ObjectGuid ownerid = GetOwnerGUID())
        return ObjectAccessor::GetUnit(*this, ownerid);

    return nullptr;
}

Unit* Unit::GetCharmer() const
{
    if (ObjectGuid charmerid = GetCharmerGUID())
        return ObjectAccessor::GetUnit(*this, charmerid);
    return NULL;
}

Player* Unit::GetCharmerOrOwnerPlayerOrPlayerItself() const
{
    ObjectGuid guid = GetCharmerOrOwnerGUID();
    if (guid.IsPlayer())
        return ObjectAccessor::GetPlayer(*this, guid);

    return const_cast<Unit*>(this)->ToPlayer();
}

Player* Unit::GetAffectingPlayer() const
{
    if (!GetCharmerOrOwnerGUID())
        return const_cast<Unit*>(this)->ToPlayer();

    if (Unit* owner = GetCharmerOrOwner())
        return owner->GetCharmerOrOwnerPlayerOrPlayerItself();

    return NULL;
}

Minion* Unit::GetFirstMinion() const
{
    if (ObjectGuid pet_guid = GetMinionGUID())
    {
        if (Creature* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*this, pet_guid))
            if (pet->HasUnitTypeMask(UNIT_MASK_MINION))
                return (Minion*) pet;

        TC_LOG_ERROR("entities.unit", "Unit::GetFirstMinion: Minion %u not exist.", pet_guid.GetCounter());
        const_cast<Unit*>(this)->SetMinionGUID(ObjectGuid::Empty);
    }

    return NULL;
}

Guardian* Unit::GetGuardianPet() const
{
    if (ObjectGuid pet_guid = GetPetGUID())
    {
        if (Creature* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*this, pet_guid))
            if (pet->HasUnitTypeMask(UNIT_MASK_GUARDIAN))
                return (Guardian*) pet;

        TC_LOG_FATAL("entities.unit", "Unit::GetGuardianPet: Guardian " UI64FMTD " not exist.", pet_guid.GetRawValue());
        const_cast<Unit*>(this)->SetPetGUID(ObjectGuid::Empty);
    }

    return NULL;
}

Unit* Unit::GetCharm() const
{
    if (ObjectGuid charm_guid = GetCharmGUID())
    {
        if (Unit* pet = ObjectAccessor::GetUnit(*this, charm_guid))
            return pet;

        TC_LOG_ERROR("entities.unit", "Unit::GetCharm: Charmed creature %u not exist.", charm_guid.GetCounter());
        const_cast<Unit*>(this)->SetGuidValue(UNIT_FIELD_CHARM, ObjectGuid::Empty);
    }

    return NULL;
}

Unit* Unit::GetCharmerOrOwner() const
{
    return GetCharmerGUID() ? GetCharmer() : GetOwner();
}

Unit* Unit::GetCharmerOrOwnerOrSelf() const
{
    if (Unit* u = GetCharmerOrOwner())
        return u;

    return (Unit*)this;
}

void Unit::SetMinion(Minion *minion, bool apply)
{
    TC_LOG_DEBUG("entities.unit", "SetMinion %u for %u, apply %u", minion->GetEntry(), GetEntry(), apply);

    if (apply)
    {
        if (minion->GetOwnerGUID())
        {
            TC_LOG_FATAL("entities.unit", "SetMinion: Minion %u is not the minion of owner %u", minion->GetEntry(), GetEntry());
            return;
        }

        minion->SetOwnerGUID(GetGUID());

        m_Controlled.insert(minion);

        if (GetTypeId() == TYPEID_PLAYER)
        {
            minion->m_ControlledByPlayer = true;
            minion->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
        }

        // Can only have one pet. If a new one is summoned, dismiss the old one.
        if (minion->IsGuardianPet())
        {
            if (Guardian* oldPet = GetGuardianPet())
            {
                if (oldPet != minion && (oldPet->IsPet() || minion->IsPet() || oldPet->GetEntry() != minion->GetEntry()))
                {
                    // remove existing minion pet
                    if (oldPet->IsPet())
                        ((Pet*) oldPet)->Remove(PET_REMOVE_DISMISS, PET_REMOVE_FLAG_RESET_CURRENT);
                    else
                        oldPet->UnSummon();
                    SetPetGUID(minion->GetGUID());
                    SetMinionGUID(ObjectGuid::Empty);
                }
            }
            else
            {
                SetPetGUID(minion->GetGUID());
                SetMinionGUID(ObjectGuid::Empty);
            }
        }

        if (minion->HasUnitTypeMask(UNIT_MASK_CONTROLABLE_GUARDIAN))
        {
            if (AddGuidValue(UNIT_FIELD_SUMMON, minion->GetGUID()))
            {
            }
        }

        if (minion->m_Properties && minion->m_Properties->Type == SUMMON_TYPE_MINIPET)
        {
            SetCritterGUID(minion->GetGUID());
        }

        // PvP, FFAPvP
        minion->SetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1, GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1));

        // Ghoul pets have energy instead of mana (is anywhere better place for this code?)
        if (minion->IsPetGhoul())
            minion->SetPowerType(POWER_ENERGY);

        if (GetTypeId() == TYPEID_PLAYER)
        {
            // Send infinity cooldown - client does that automatically but after relog cooldown needs to be set again
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(minion->GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL));

            if (spellInfo && (spellInfo->IsCooldownStartedOnEvent()))
                ToPlayer()->GetSpellHistory()->SetCooldownOnHold(spellInfo, 0);
        }
    }
    else
    {
        if (minion->GetOwnerGUID() != GetGUID())
        {
            TC_LOG_FATAL("entities.unit", "SetMinion: Minion %u is not the minion of owner %u", minion->GetEntry(), GetEntry());
            return;
        }

        m_Controlled.erase(minion);

        if (minion->m_Properties && minion->m_Properties->Type == SUMMON_TYPE_MINIPET)
        {
            if (GetCritterGUID() == minion->GetGUID())
                SetCritterGUID(ObjectGuid::Empty);
        }

        if (minion->IsGuardianPet())
        {
            if (GetPetGUID() == minion->GetGUID())
                SetPetGUID(ObjectGuid::Empty);
        }
        else if (minion->IsTotem())
        {
            // All summoned by totem minions must disappear when it is removed.
            uint32 totemSpell = minion->GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL);
            if (totemSpell == 2894 || totemSpell == 2062)
            {
                uint32 normal = totemSpell == 2894 ? 117663 : 117753;
                uint32 primal = totemSpell == 2894 ? 118291 : 118323;
                for (auto&& spell : { normal, primal })
                    if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell))
                        RemoveAllMinionsByEntry(spellInfo->Effects[EFFECT_0].MiscValue);
            }
        }

        if (GetTypeId() == TYPEID_PLAYER)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(minion->GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL));
            // Remove infinity cooldown
            if (spellInfo && (spellInfo->IsCooldownStartedOnEvent()))
                ToPlayer()->SendCooldownEvent(spellInfo);
        }

        //if (minion->HasUnitTypeMask(UNIT_MASK_GUARDIAN))
        {
            if (RemoveGuidValue(UNIT_FIELD_SUMMON, minion->GetGUID()))
            {
                // Check if there is another minion
                for (ControlList::iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
                {
                    // do not use this check, creature do not have charm guid
                    //if (GetCharmGUID() == (*itr)->GetGUID())
                    if (GetGUID() == (*itr)->GetCharmerGUID())
                        continue;

                    //ASSERT((*itr)->GetOwnerGUID() == GetGUID());
                    if ((*itr)->GetOwnerGUID() != GetGUID())
                    {
                        OutDebugInfo();
                        (*itr)->OutDebugInfo();
                        ASSERT(false);
                    }
                    ASSERT((*itr)->GetTypeId() == TYPEID_UNIT);

                    if (!(*itr)->HasUnitTypeMask(UNIT_MASK_CONTROLABLE_GUARDIAN))
                        continue;

                    if (AddGuidValue(UNIT_FIELD_SUMMON, (*itr)->GetGUID()))
                    {
                        // show another pet bar if there is no charm bar
                        if (GetTypeId() == TYPEID_PLAYER && !GetCharmGUID())
                        {
                            if ((*itr)->IsPet())
                                ToPlayer()->PetSpellInitialize();
                            else
                                ToPlayer()->CharmSpellInitialize();
                        }
                    }
                    break;
                }
            }
        }
    }
}

void Unit::GetAllMinionsByEntry(std::list<Creature*>& Minions, uint32 entry)
{
    for (Unit::ControlList::iterator itr = m_Controlled.begin(); itr != m_Controlled.end();)
    {
        Unit* unit = *itr;
        ++itr;
        if (unit->GetEntry() == entry && unit->GetTypeId() == TYPEID_UNIT
            && unit->ToCreature()->IsSummon()) // minion, actually
            Minions.push_back(unit->ToCreature());
    }
}

Unit* Unit::GetFirstMinionByEntry(uint32 entry)
{
    for (Unit::ControlList::iterator itr = m_Controlled.begin(); itr != m_Controlled.end();)
    {
        Unit* unit = *itr;
        ++itr;
        if (unit->GetEntry() == entry && unit->GetTypeId() == TYPEID_UNIT
            && unit->ToCreature()->IsSummon())
            return unit;
    }

    return NULL;
}

void Unit::RemoveAllMinionsByEntry(uint32 entry)
{
    for (Unit::ControlList::iterator itr = m_Controlled.begin(); itr != m_Controlled.end();)
    {
        Unit* unit = *itr;
        ++itr;
        if (unit->GetEntry() == entry && unit->GetTypeId() == TYPEID_UNIT
            && unit->ToCreature()->IsSummon()) // minion, actually
            unit->ToTempSummon()->UnSummon();
        // i think this is safe because i have never heard that a despawned minion will trigger a same minion
    }
}

void Unit::SetCharm(Unit* charm, bool apply)
{
    if (apply)
    {
        if (GetTypeId() == TYPEID_PLAYER)
        {
            if (!AddGuidValue(UNIT_FIELD_CHARM, charm->GetGUID()))
                TC_LOG_FATAL("entities.unit", "Player %s is trying to charm unit %u, but it already has a charmed unit " UI64FMTD "", GetName().c_str(), charm->GetEntry(), GetCharmGUID().GetRawValue());

            charm->m_ControlledByPlayer = true;
            /// @todo maybe we can use this flag to check if controlled by player
            charm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
        }
        else
            charm->m_ControlledByPlayer = false;

        // PvP, FFAPvP
        charm->SetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1, GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1));

        if (!charm->AddGuidValue(UNIT_FIELD_CHARMED_BY, GetGUID()))
            TC_LOG_FATAL("entities.unit", "Unit %u is being charmed, but it already has a charmer " UI64FMTD "", charm->GetEntry(), charm->GetCharmerGUID().GetRawValue());

        _isWalkingBeforeCharm = charm->IsWalking();
        if (_isWalkingBeforeCharm)
            charm->SetWalk(false);

        m_Controlled.insert(charm);
    }
    else
    {
        if (GetTypeId() == TYPEID_PLAYER)
        {
            if (!RemoveGuidValue(UNIT_FIELD_CHARM, charm->GetGUID()))
                TC_LOG_FATAL("entities.unit", "Player %s is trying to uncharm unit %u, but it has another charmed unit " UI64FMTD "", GetName().c_str(), charm->GetEntry(), GetCharmGUID().GetRawValue());
        }

        if (!charm->RemoveGuidValue(UNIT_FIELD_CHARMED_BY, GetGUID()))
            TC_LOG_FATAL("entities.unit", "Unit %u is being uncharmed, but it has another charmer " UI64FMTD "", charm->GetEntry(), charm->GetCharmerGUID().GetRawValue());

        if (charm->GetTypeId() == TYPEID_PLAYER)
        {
            charm->m_ControlledByPlayer = true;
            charm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
            charm->ToPlayer()->UpdatePvPState();
        }
        else if (Player* player = charm->GetCharmerOrOwnerPlayerOrPlayerItself())
        {
            charm->m_ControlledByPlayer = true;
            charm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
            charm->SetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1, player->GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1));
        }
        else
        {
            charm->m_ControlledByPlayer = false;
            charm->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
            charm->SetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1, 0);
        }

        if (charm->IsWalking() != _isWalkingBeforeCharm)
            charm->SetWalk(_isWalkingBeforeCharm);

        if (charm->GetTypeId() == TYPEID_PLAYER
            || !charm->ToCreature()->HasUnitTypeMask(UNIT_MASK_MINION)
            || charm->GetOwnerGUID() != GetGUID())
        {
            m_Controlled.erase(charm);
        }
    }
}

int32 Unit::DealHeal(Unit* victim, uint32 addhealth)
{
    int32 gain = 0;
    if (victim->isDead())
        return gain;

    if (victim->IsAIEnabled)
        victim->GetAI()->HealReceived(this, addhealth);

    if (IsAIEnabled)
        GetAI()->HealDone(victim, addhealth);

    if (addhealth)
        gain = victim->ModifyHealth(int32(addhealth));

    // Hook for OnHeal Event
    sScriptMgr->OnHeal(this, victim, (uint32&) gain);

    Unit* unit = this;

    if (GetTypeId() == TYPEID_UNIT && ToCreature()->IsTotem())
        unit = GetOwner();

    if (Player* player = unit->ToPlayer())
    {
        if (Battleground* bg = player->GetBattleground())
            bg->UpdatePlayerScore(player, SCORE_HEALING_DONE, gain);

        // use the actual gain, as the overheal shall not be counted, skip gain 0 (it ignored anyway in to criteria)
        if (gain)
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HEALING_DONE, gain, 0, 0, victim);

        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HEAL_CASTED, addhealth);
    }

    if (Player* player = victim->ToPlayer())
    {
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_TOTAL_HEALING_RECEIVED, gain);
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HEALING_RECEIVED, addhealth);

        if (Battleground* bg = player->GetBattleground())
            bg->UpdatePlayerScore(player, SCORE_HEALING_TAKEN, gain);
    }

    return gain;
}

Unit* Unit::GetMagicHitRedirectTarget(Unit* victim, SpellInfo const* spellInfo)
{
    // Patch 1.2 notes: Spell Reflection no longer reflects abilities
    if (spellInfo->Attributes & SPELL_ATTR0_ABILITY || spellInfo->AttributesEx & SPELL_ATTR1_CANT_BE_REDIRECTED || spellInfo->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)
        return victim;

    Unit::AuraEffectList const& magnetAuras = victim->GetAuraEffectsByType(SPELL_AURA_SPELL_MAGNET);
    for (Unit::AuraEffectList::const_iterator itr = magnetAuras.begin(); itr != magnetAuras.end(); ++itr)
    {
        if (Unit* magnet = (*itr)->GetBase()->GetCaster())
            if (spellInfo->CheckExplicitTarget(this, magnet) == SPELL_CAST_OK
                && (IsWithinLOSInMap(magnet) || magnet->IsTotem()))
            {
                return magnet;
            }
    }
    return victim;
}

Unit* Unit::GetMeleeHitRedirectTarget(Unit* victim, SpellInfo const* spellInfo)
{
    AuraEffectList const& hitTriggerAuras = victim->GetAuraEffectsByType(SPELL_AURA_ADD_CASTER_HIT_TRIGGER);
    for (AuraEffectList::const_iterator i = hitTriggerAuras.begin(); i != hitTriggerAuras.end(); ++i)
    {
        if (Unit* magnet = (*i)->GetBase()->GetCaster())
            if (IsValidAttackTarget(magnet, spellInfo) && magnet->IsWithinLOSInMap(this)
                && (!spellInfo || (spellInfo->CheckExplicitTarget(this, magnet) == SPELL_CAST_OK
                && spellInfo->CheckTarget(this, magnet, false) == SPELL_CAST_OK)))
                {
                    (*i)->GetBase()->DropCharge(AURA_REMOVE_BY_EXPIRE);
                    return magnet;
                }
    }
    return victim;
}

Unit* Unit::GetFirstControlled() const
{
    // Sequence: charmed, pet, other guardians
    Unit* unit = GetCharm();
    if (!unit)
        if (ObjectGuid guid = GetMinionGUID())
            unit = ObjectAccessor::GetUnit(*this, guid);

    return unit;
}

void Unit::RemoveAllControlled()
{
    // possessed pet and vehicle
    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->StopCastingCharm();

    while (!m_Controlled.empty())
    {
        Unit* target = *m_Controlled.begin();
        m_Controlled.erase(m_Controlled.begin());
        if (target->GetCharmerGUID() == GetGUID())
            target->RemoveCharmAuras();
        else if (target->GetOwnerGUID() == GetGUID() && target->IsSummon())
            target->ToTempSummon()->UnSummon();
        else
            TC_LOG_ERROR("entities.unit", "Unit %u is trying to release unit %u which is neither charmed nor owned by it", GetEntry(), target->GetEntry());
    }
    if (GetPetGUID())
        TC_LOG_FATAL("entities.unit", "Unit %u is not able to release its pet " UI64FMTD, GetEntry(), GetPetGUID().GetRawValue());
    if (GetMinionGUID())
        TC_LOG_FATAL("entities.unit", "Unit %u is not able to release its minion " UI64FMTD, GetEntry(), GetMinionGUID().GetRawValue());
    if (GetCharmGUID())
        TC_LOG_FATAL("entities.unit", "Unit %u is not able to release its charm " UI64FMTD, GetEntry(), GetCharmGUID().GetRawValue());
}

bool Unit::isPossessedByPlayer() const
{
    return HasUnitState(UNIT_STATE_POSSESSED) && GetCharmerGUID().IsPlayer();
}

bool Unit::isPossessing(Unit* u) const
{
    return u->isPossessed() && GetCharmGUID() == u->GetGUID();
}

bool Unit::isPossessing() const
{
    if (Unit* u = GetCharm())
        return u->isPossessed();
    else
        return false;
}

Unit* Unit::GetNextRandomRaidMemberOrPet(float radius)
{
    Player* player = NULL;
    if (GetTypeId() == TYPEID_PLAYER)
        player = ToPlayer();
    // Should we enable this also for charmed units?
    else if (GetTypeId() == TYPEID_UNIT && ToCreature()->IsPet())
        player = GetOwner()->ToPlayer();

    if (!player)
        return NULL;
    Group* group = player->GetGroup();
    // When there is no group check pet presence
    if (!group)
    {
        // We are pet now, return owner
        if (player != this)
            return IsWithinDistInMap(player, radius) ? player : NULL;
        Unit* pet = GetGuardianPet();
        // No pet, no group, nothing to return
        if (!pet)
            return NULL;
        // We are owner now, return pet
        return IsWithinDistInMap(pet, radius) ? pet : NULL;
    }

    std::vector<Unit*> nearMembers;
    std::vector<Unit*> nearPets;
    // reserve place for players and pets because resizing vector every unit push is unefficient (vector is reallocated then)
    nearMembers.reserve(group->GetMembersCount() * 2);

    for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
        if (Player* Target = itr->GetSource())
        {
            // IsHostileTo check duel and controlled by enemy
            if (Target != this && Target->IsAlive() && IsWithinDistInMap(Target, radius) && !IsHostileTo(Target))
                nearMembers.push_back(Target);

            // Push player's pet to vector
            if (Unit* pet = Target->GetGuardianPet())
                if (pet != this && pet->IsAlive() && IsWithinDistInMap(pet, radius) && !IsHostileTo(pet))
                    nearPets.push_back(pet);
        }

    if (nearMembers.empty() && nearPets.empty())
        return nullptr;

    if (nearMembers.empty())
    {
        // May be not good idea...
        uint32 randTarget = urand(0, nearPets.size() - 1);
        return nearPets[randTarget];
    }
    uint32 randTarget = urand(0, nearMembers.size() - 1);
    return nearMembers[randTarget];

}

// only called in Player::SetSeer
// so move it to Player?
void Unit::AddPlayerToVision(Player* player)
{
    if (m_sharedVision.empty())
    {
        setActive(true, ActiveFlags::SharedVision);
        SetWorldObject(true);
    }
    m_sharedVision.push_back(player);
}

// only called in Player::SetSeer
void Unit::RemovePlayerFromVision(Player* player)
{
    m_sharedVision.remove(player);
    if (m_sharedVision.empty())
    {
        setActive(false, ActiveFlags::SharedVision);
        SetWorldObject(false);
    }
}

void Unit::RemoveBindSightAuras()
{
    RemoveAurasByType(SPELL_AURA_BIND_SIGHT);
}

void Unit::RemoveCharmAuras()
{
    RemoveAurasByType(SPELL_AURA_MOD_CHARM);
    RemoveAurasByType(SPELL_AURA_MOD_POSSESS);
    RemoveAurasByType(SPELL_AURA_AOE_CHARM);
    RemoveAurasDueToSpell(12480); // Hex of Jammal'an hasn't charm effect ¯\_(ツ)_/¯
}

void Unit::UnsummonAllTotems()
{
    for (uint8 i = 0; i < SUMMON_SLOT_MAX; ++i)
    {
        if (!m_SummonSlot [i])
            continue;

        if (Creature* OldTotem = GetMap()->GetCreature(m_SummonSlot [i]))
            if (OldTotem->IsSummon())
                OldTotem->ToTempSummon()->UnSummon();
    }
}

void Unit::SendHealSpellLog(Unit* victim, uint32 SpellID, uint32 Damage, uint32 OverHeal, uint32 Absorb, bool critical)
{
    ObjectGuid victimGuid = victim->GetGUID();
    ObjectGuid casterGuid = GetGUID();

    // we guess size
    WorldPacket data(SMSG_SPELLHEALLOG, 8 + 8 + 4 + 4 + 4 + 4 + 1 + 1);

    data << uint32(SpellID);
    data << uint32(Absorb);
    data << uint32(Damage);
    data << uint32(OverHeal);

    data.WriteBit(victimGuid [0]);
    data.WriteBit(casterGuid [2]);
    data.WriteBit(casterGuid [6]);
    data.WriteBit(victimGuid [2]);
    data.WriteBit(critical);
    data.WriteBit(casterGuid [3]);
    data.WriteBit(casterGuid [0]);
    data.WriteBit(casterGuid [5]);
    data.WriteBit(victimGuid [3]);
    data.WriteBit(0); // PowerData
    data.WriteBit(victimGuid [7]);
    data.WriteBit(victimGuid [5]);
    data.WriteBit(casterGuid [7]);
    data.WriteBit(victimGuid [4]);
    data.WriteBit(0);
    data.WriteBit(0);
    data.WriteBit(casterGuid [4]);
    data.WriteBit(casterGuid [1]);
    data.WriteBit(victimGuid [1]);
    data.WriteBit(victimGuid [6]);

    data.WriteByteSeq(casterGuid [2]);
    data.WriteByteSeq(victimGuid [6]);
    data.WriteByteSeq(casterGuid [5]);
    data.WriteByteSeq(casterGuid [3]);
    data.WriteByteSeq(victimGuid [7]);
    data.WriteByteSeq(casterGuid [7]);
    data.WriteByteSeq(casterGuid [6]);
    data.WriteByteSeq(casterGuid [1]);
    data.WriteByteSeq(victimGuid [2]);
    data.WriteByteSeq(victimGuid [4]);
    data.WriteByteSeq(victimGuid [3]);
    data.WriteByteSeq(victimGuid [0]);
    data.WriteByteSeq(victimGuid [5]);
    data.WriteByteSeq(casterGuid [0]);
    data.WriteByteSeq(victimGuid [1]);
    data.WriteByteSeq(casterGuid [4]);

    SendMessageToSet(&data, true);
}

int32 Unit::HealBySpell(Unit* victim, SpellInfo const* spellInfo, uint32 addHealth, bool critical)
{
    uint32 absorb = 0;
    // calculate heal absorb and reduce healing
    CalcHealAbsorb(victim, spellInfo, addHealth, absorb);

    int32 gain = DealHeal(victim, addHealth);
    SendHealSpellLog(victim, spellInfo->Id, addHealth, uint32(addHealth - gain), absorb, critical);
    return gain;
}

void Unit::SendEnergizeSpellLog(Unit* victim, uint32 spellId, int32 damage, Powers powerType)
{
    //bool hasPower = false;
    ObjectGuid victimGuid = victim->GetGUID();
    ObjectGuid casterGuid = GetGUID();

    WorldPacket data(SMSG_SPELLENERGIZELOG, (8 + 8 + 4 + 4 + 4 + 1));

    data.WriteBit(victimGuid [7]);
    data.WriteBit(victimGuid [3]);
    data.WriteBit(casterGuid [1]);
    data.WriteBit(victimGuid [4]);
    data.WriteBit(victimGuid [2]);
    data.WriteBit(casterGuid [3]);
    data.WriteBit(victimGuid [5]);

    data.WriteBit(0); // hasPower

    data.WriteBit(casterGuid [7]);
    data.WriteBit(casterGuid [0]);
    data.WriteBit(casterGuid [2]);

    //if (hasPower)
    //    data.WriteBits(count, 21);

    data.WriteBit(casterGuid [4]);
    data.WriteBit(casterGuid [6]);
    data.WriteBit(victimGuid [6]);
    data.WriteBit(victimGuid [1]);
    data.WriteBit(victimGuid [0]);
    data.WriteBit(casterGuid [5]);


    data.WriteByteSeq(victimGuid [0]);
    data.WriteByteSeq(casterGuid [5]);
    data.WriteByteSeq(victimGuid [6]);

    /*if (hasPower)
    {
    data << UInt32();

    for (var i = 0; i < count; ++i)
    {
    data << Int32();
    data << Int32();
    }
    data << Int32();
    data << Int32();
    }*/
    data.WriteByteSeq(casterGuid [6]);
    data.WriteByteSeq(victimGuid [2]);
    data.WriteByteSeq(casterGuid [0]);
    data.WriteByteSeq(victimGuid [1]);
    data << int32(damage);
    data.WriteByteSeq(victimGuid [4]);
    data.WriteByteSeq(casterGuid [1]);
    data.WriteByteSeq(casterGuid [7]);
    data.WriteByteSeq(victimGuid [5]);
    data.WriteByteSeq(casterGuid [2]);
    data.WriteByteSeq(casterGuid [3]);
    data.WriteByteSeq(victimGuid [7]);
    data.WriteByteSeq(casterGuid [4]);
    data.WriteByteSeq(victimGuid [3]);
    data << uint32(spellId);

    data << uint32(powerType);

    SendMessageToSet(&data, true);
}

void Unit::EnergizeBySpell(Unit* victim, uint32 spellId, int32 damage, Powers powerType)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);

    if (spellInfo->SpellFamilyName == SPELLFAMILY_MAGE && HasAuraType(SPELL_AURA_MOD_MANA_REGEN_BY_HASTE))
        damage /= GetFloatValue(UNIT_FIELD_MOD_HASTE_REGEN);

    SendEnergizeSpellLog(victim, spellId, damage, powerType);
    // needs to be called after sending spell log
    victim->ModifyPower(powerType, damage);
    victim->getHostileRefManager().threatAssist(this, float(damage) * 0.5f, spellInfo);
}

int32 Unit::SpellDamageBonusDone(Unit* victim, SpellInfo const* spellProto, uint32 effIndex, int32 damage, DamageEffectType damagetype, uint32 stack) const
{
    if (!spellProto || !victim || damagetype == DIRECT_DAMAGE)
        return damage;

    // Some spells don't benefit from done mods
    if (spellProto->AttributesEx3 & SPELL_ATTR3_NO_DONE_BONUS || spellProto->AttributesEx4 & SPELL_ATTR4_FIXED_DAMAGE)
        return damage;

    // For totems get damage bonus from owner
    if (GetTypeId() == TYPEID_UNIT && ToCreature()->IsTotem())
        if (Unit* owner = GetOwner())
            return owner->SpellDamageBonusDone(victim, spellProto, effIndex, damage, damagetype);

    // Done total percent damage auras
    float doneTotalMod = 1.0f;
    int32 doneTotal = 0;

    // Pet damage?
    if (GetTypeId() == TYPEID_UNIT && !ToCreature()->IsPet())
        doneTotalMod *= ToCreature()->GetSpellDamageMod(ToCreature()->GetCreatureTemplate()->rank);

    // Some spells don't benefit from pct done mods
    if (!(spellProto->AttributesEx6 & SPELL_ATTR6_NO_DONE_PCT_DAMAGE_MODS))
    {
        AuraEffectList const& mModDamagePercentDone = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
        for (AuraEffectList::const_iterator i = mModDamagePercentDone.begin(); i != mModDamagePercentDone.end(); ++i)
        {
            if ((*i)->GetMiscValue() & spellProto->GetSchoolMask())
            {
                if ((*i)->GetSpellInfo()->EquippedItemClass == -1)
                    AddPct(doneTotalMod, (*i)->GetFloatAmount());
                else if (!((*i)->GetSpellInfo()->AttributesEx5 & SPELL_ATTR5_SPECIAL_ITEM_CLASS_CHECK) && ((*i)->GetSpellInfo()->EquippedItemSubClassMask == 0))
                    AddPct(doneTotalMod, (*i)->GetFloatAmount());
                else if (ToPlayer() && ToPlayer()->HasItemFitToSpellRequirements((*i)->GetSpellInfo()))
                    AddPct(doneTotalMod, (*i)->GetFloatAmount());
            }
        }

        for (auto&& it : GetAuraEffectsByType(SPELL_AURA_MOD_SPELL_DAMAGE_DONE_FROM_MANA_PERCENT))
            if (it->GetMiscValue() & spellProto->GetSchoolMask())
                AddPct(doneTotalMod, it->GetFloatAmount() * GetPower(POWER_MANA) / GetMaxPower(POWER_MANA));
    }

    uint32 creatureTypeMask = victim->GetCreatureTypeMask();
    // Add flat bonus from spell damage versus
    doneTotal += GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_FLAT_SPELL_DAMAGE_VERSUS, creatureTypeMask);
    AuraEffectList const& mDamageDoneVersus = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_DONE_VERSUS);
    for (AuraEffectList::const_iterator i = mDamageDoneVersus.begin(); i != mDamageDoneVersus.end(); ++i)
        if (creatureTypeMask & uint32((*i)->GetMiscValue()))
            AddPct(doneTotalMod, (*i)->GetAmount());

    // bonus against aurastate
    AuraEffectList const& mDamageDoneVersusAurastate = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_DONE_VERSUS_AURASTATE);
    for (AuraEffectList::const_iterator i = mDamageDoneVersusAurastate.begin(); i != mDamageDoneVersusAurastate.end(); ++i)
        if (victim->HasAuraState(AuraStateType((*i)->GetMiscValue())))
            AddPct(doneTotalMod, (*i)->GetAmount());

    // Add SPELL_AURA_MOD_DAMAGE_DONE_FOR_MECHANIC percent bonus
    AddPct(doneTotalMod, GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_DAMAGE_DONE_FOR_MECHANIC, spellProto->Mechanic));

    // Probable this must applies as SPELL_ATTR6_NO_DONE_PCT_DAMAGE_MODS but I'm not sure
    if (!spellProto->HasAttribute(SPELL_ATTR0_CU_IGNORE_PVP_POWER))
        if (Player* player = GetCharmerOrOwnerPlayerOrPlayerItself())
            if (victim->GetCharmerOrOwnerPlayerOrPlayerItself())
                AddPct(doneTotalMod, player->GetFloatValue(PLAYER_FIELD_PVP_POWER_DAMAGE));

    // done scripted mod (take it from owner)
    Unit const* owner = GetOwner() ? GetOwner() : this;
    AuraEffectList const& mOverrideClassScript = owner->GetAuraEffectsByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    for (AuraEffectList::const_iterator i = mOverrideClassScript.begin(); i != mOverrideClassScript.end(); ++i)
    {
        if (!(*i)->IsAffectingSpell(spellProto))
            continue;

        switch ((*i)->GetMiscValue())
        {
            case 4920: // Molten Fury
            case 4919:
            {
                if (victim->HasAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, spellProto, this))
                    AddPct(doneTotalMod, (*i)->GetAmount());
                break;
            }
            case 6917: // Death's Embrace damage effect
            case 6926:
            case 6928:
            {
                // Health at 25% or less (25% stored at effect 2 of the spell)
                if (victim->HealthBelowPct(CalculateSpellDamage(this, (*i)->GetSpellInfo(), EFFECT_2)))
                    AddPct(doneTotalMod, (*i)->GetAmount());
            }
            case 6916: // Death's Embrace heal effect
            case 6925:
            case 6927:
                if (HealthBelowPct(CalculateSpellDamage(this, (*i)->GetSpellInfo(), EFFECT_2)))
                    AddPct(doneTotalMod, (*i)->GetAmount());
                break;
                // Soul Siphon
            case 4992:
            case 4993:
            {
                // effect 1 m_amount
                int32 maxPercent = (*i)->GetAmount();
                // effect 0 m_amount
                int32 stepPercent = CalculateSpellDamage(this, (*i)->GetSpellInfo(), 0);
                // count affliction effects and calc additional damage in percentage
                int32 modPercent = 0;
                AuraApplicationMap const& victimAuras = victim->GetAppliedAuras();
                for (AuraApplicationMap::const_iterator itr = victimAuras.begin(); itr != victimAuras.end(); ++itr)
                {
                    Aura const* aura = itr->second->GetBase();
                    SpellInfo const* spell = aura->GetSpellInfo();
                    if (spell->SpellFamilyName != SPELLFAMILY_WARLOCK || !(spell->SpellFamilyFlags [1] & 0x0004071B || spell->SpellFamilyFlags [0] & 0x8044C402))
                        continue;
                    modPercent += stepPercent * aura->GetStackAmount();
                    if (modPercent >= maxPercent)
                    {
                        modPercent = maxPercent;
                        break;
                    }
                }
                AddPct(doneTotalMod, modPercent);
                break;
            }
            case 5481: // Starfire Bonus
            {
                if (victim->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_DRUID, flag128{ 0x200002 }))
                    AddPct(doneTotalMod, (*i)->GetAmount());
                break;
            }
            case 4418: // Increased Shock Damage
            case 4554: // Increased Lightning Damage
            case 4555: // Improved Moonfire
            case 5142: // Increased Lightning Damage
            case 5147: // Improved Consecration / Libram of Resurgence
            case 5148: // Idol of the Shooting Star
            case 6008: // Increased Lightning Damage
            case 8627: // Totem of Hex
            {
                doneTotal += (*i)->GetAmount();
                break;
            }
        }
    }

    // Custom scripted damage
    switch (spellProto->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (spellProto->Id)
            {
                case 98535: // Leaping Flames, Majordomo Staghelm
                case 98474: // Flame Scythe, Majordomo Staghelm
                case 100212:
                case 100213:
                case 100214:
                    // Fury, SPELL_AURA_276
                    if (Aura const* aur = GetAura(97235))
                        AddPct(doneTotalMod, aur->GetStackAmount() * 8);
                    break;
            }
            break;
        }
        case SPELLFAMILY_MAGE:
            // Torment the weak
            if (spellProto->GetSchoolMask() & SPELL_SCHOOL_MASK_ARCANE)
            {
                if (victim->HasAuraWithMechanic((1 << MECHANIC_SNARE) | (1 << MECHANIC_SLOW_ATTACK)))
                {
                    AuraEffectList const& mDumyAuras = GetAuraEffectsByType(SPELL_AURA_DUMMY);
                    for (AuraEffectList::const_iterator i = mDumyAuras.begin(); i != mDumyAuras.end(); ++i)
                    {
                        if ((*i)->GetSpellInfo()->SpellIconID == 2215)
                        {
                            AddPct(doneTotalMod, (*i)->GetAmount());
                            break;
                        }
                    }
                }
            }
            break;
        case SPELLFAMILY_WARLOCK:
            // Shadow Bite (30% increase from each dot)
            if (spellProto->SpellFamilyFlags [1] & 0x00400000 && IsPet())
                if (uint8 count = victim->GetDoTsByCaster(GetOwnerGUID()))
                    AddPct(doneTotalMod, 30 * count);
            break;
        case SPELLFAMILY_DEATHKNIGHT:
            // Sigil of the Vengeful Heart
            if (spellProto->SpellFamilyFlags [0] & 0x2000)
                if (AuraEffect* aurEff = GetAuraEffect(64962, EFFECT_1))
                    doneTotal += aurEff->GetAmount();
            break;
    }

    // Done fixed damage bonus auras
    int32 DoneAdvertisedBenefit = SpellBaseDamageBonusDone(spellProto->GetSchoolMask());

    // Check for table values
    float coeff = 0;
    SpellBonusEntry const* bonus = sSpellMgr->GetSpellBonusData(spellProto->Id);
    if (bonus)
    {
        if (damagetype == DOT)
        {
            coeff = bonus->dot_damage;
            if (bonus->ap_dot_bonus > 0)
            {
                WeaponAttackType attType = spellProto->IsRangedWeaponSpell() ? RANGED_ATTACK : BASE_ATTACK;
                float apbonus = float(victim->GetTotalAuraModifier(attType == BASE_ATTACK ? SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS : SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS));
                apbonus += GetTotalAttackPowerValue(attType);
                if (spellProto->Effects[effIndex].IsScaledByComboPoints() && GetTypeId() == TYPEID_PLAYER)
                    if (uint8 cp = ToPlayer()->GetComboPoints())
                        apbonus *= cp;
                doneTotal += int32(bonus->ap_dot_bonus * stack * apbonus);
            }
        }
        else
        {
            coeff = bonus->direct_damage;
            if (bonus->ap_bonus > 0)
            {
                WeaponAttackType attType = spellProto->IsRangedWeaponSpell() ? RANGED_ATTACK : BASE_ATTACK;
                float apbonus = float(victim->GetTotalAuraModifier(attType == BASE_ATTACK ? SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS : SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS));
                apbonus += GetTotalAttackPowerValue(attType);
                if (spellProto->Effects[effIndex].IsScaledByComboPoints() && GetTypeId() == TYPEID_PLAYER)
                    if (uint8 cp = ToPlayer()->GetComboPoints())
                        apbonus *= cp;
                doneTotal += int32(bonus->ap_bonus * stack * apbonus);
            }
        }
    }
    // Default calculation
    if (DoneAdvertisedBenefit && coeff >= 0)
    {
        if (coeff == 0)
            coeff = spellProto->Effects[effIndex].BonusMultiplier;

        if (Player* modOwner = GetSpellModOwner())
        {
            coeff *= 100.0f;
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_BONUS_MULTIPLIER, coeff);
            coeff /= 100.0f;
        }
        doneTotal += int32(DoneAdvertisedBenefit * coeff * stack);
    }

    float tmpDamage = (damage + doneTotal) * doneTotalMod;
    // apply spellmod to Done damage (flat and pct)
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, damagetype == DOT ? SPELLMOD_DOT : SPELLMOD_DAMAGE, tmpDamage);

    return int32(std::max(tmpDamage, 0.0f));
}

int32 Unit::SpellDamageBonusTaken(Unit* caster, SpellInfo const* spellProto, uint32 effIndex, int32 damage, DamageEffectType damagetype, uint32 stack) const
{
    if (!spellProto || damagetype == DIRECT_DAMAGE)
        return damage;

    if (spellProto->HasAttribute(SPELL_ATTR4_FIXED_DAMAGE))
        return damage;

    int32 takenTotal = 0;
    float takenTotalMod = 1.0f;
    float takenTotalCasterMod = 0.0f;

    if (GetCharmerOrOwnerPlayerOrPlayerItself())
    {
        switch (spellProto->Id)
        {
            case 44457:  takenTotalMod = 0.85f; break; // Living Bomb
            case 44461:  takenTotalMod = 0.85f; break; // Living Bomb
            case 113092: takenTotalMod = 0.70f; break; // Frost Bomb
            case 114954: takenTotalMod = 0.85f; break; // Nether Tempest
            case 116858: takenTotalMod = 0.75f; break; // Chaos Bolt
            default:
                break;
        }
    }

    // get all auras from caster that allow the spell to ignore resistance (sanctified wrath)
    AuraEffectList const& IgnoreResistAuras = caster->GetAuraEffectsByType(SPELL_AURA_MOD_IGNORE_TARGET_RESIST);
    for (AuraEffectList::const_iterator i = IgnoreResistAuras.begin(); i != IgnoreResistAuras.end(); ++i)
    {
        if ((*i)->GetMiscValue() & spellProto->GetSchoolMask())
            takenTotalCasterMod += (float((*i)->GetAmount()));
    }

    // from positive and negative SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN
    // multiplicative bonus, for example Dispersion + Shadowform (0.10*0.85=0.085)
    takenTotalMod *= GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, spellProto->GetSchoolMask());

    //.. taken pct: dummy auras
    AuraEffectList const& mDummyAuras = GetAuraEffectsByType(SPELL_AURA_DUMMY);
    for (AuraEffectList::const_iterator i = mDummyAuras.begin(); i != mDummyAuras.end(); ++i)
    {
        switch ((*i)->GetSpellInfo()->SpellIconID)
        {
            // Cheat Death
            case 2109:
                if ((*i)->GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL)
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        continue;
                    AddPct(takenTotalMod, (*i)->GetAmount());
                }
                break;
        }
    }

    // From caster spells
    uint64 guid = caster->GetGUID();
    if (Player* player = caster->GetAffectingPlayer())
        guid = player->GetGUID();

    for (auto&& itr : GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_FROM_CASTER))
        if (itr->GetCasterGUID() == guid && itr->IsAffectingSpell(spellProto))
            AddPct(takenTotalMod, itr->GetAmount());

    // Mod damage from spell mechanic
    if (uint32 mechanicMask = spellProto->GetAllEffectsMechanicMask())
    {
        AuraEffectList const& mDamageDoneMechanic = GetAuraEffectsByType(SPELL_AURA_MOD_MECHANIC_DAMAGE_TAKEN_PERCENT);
        for (AuraEffectList::const_iterator i = mDamageDoneMechanic.begin(); i != mDamageDoneMechanic.end(); ++i)
            if (mechanicMask & uint32(1 << ((*i)->GetMiscValue())))
                AddPct(takenTotalMod, (*i)->GetAmount());
    }

    if (spellProto->DmgClass == SPELL_DAMAGE_CLASS_RANGED)
        for (auto&& it : GetAuraEffectsByType(SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN_PCT))
            AddPct(takenTotalMod, it->GetAmount());
    if (spellProto->DmgClass == SPELL_DAMAGE_CLASS_MELEE)
        for (auto&& it : GetAuraEffectsByType(SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN_PCT))
            AddPct(takenTotalMod, it->GetAmount());

    int32 TakenAdvertisedBenefit = SpellBaseDamageBonusTaken(spellProto->GetSchoolMask());

    // Check for table values
    float coeff = 0;
    SpellBonusEntry const* bonus = sSpellMgr->GetSpellBonusData(spellProto->Id);
    if (bonus)
        coeff = (damagetype == DOT) ? bonus->dot_damage : bonus->direct_damage;

    // Check for table values
    if (TakenAdvertisedBenefit && coeff >= 0)
    {
        if (coeff == 0 && spellProto->Effects[effIndex].BonusMultiplier != 1)
            coeff = spellProto->Effects[effIndex].BonusMultiplier;

        if (Player* modOwner = GetSpellModOwner())
        {
            coeff *= 100.0f;
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_BONUS_MULTIPLIER, coeff);
            coeff /= 100.0f;
        }

        takenTotal += int32(TakenAdvertisedBenefit * coeff);
    }

    float tmpDamage = 0.0f;

    if (takenTotalCasterMod)
    {
        if (takenTotal < 0)
        {
            if (takenTotalMod < 1)
                tmpDamage = ((float(CalculatePct(damage, takenTotalCasterMod) + takenTotal) * takenTotalMod) + CalculatePct(damage, takenTotalCasterMod));
            else
                tmpDamage = ((float(CalculatePct(damage, takenTotalCasterMod) + takenTotal) + CalculatePct(damage, takenTotalCasterMod)) * takenTotalMod);
        }
        else if (takenTotalMod < 1)
            tmpDamage = ((CalculatePct(damage + takenTotal, takenTotalCasterMod) * takenTotalMod) + CalculatePct(damage + takenTotal, takenTotalCasterMod));
    }
    if (!tmpDamage)
        tmpDamage = (damage + takenTotal) * takenTotalMod;

    return int32(std::max(tmpDamage, 0.0f));
}

int32 Unit::SpellBaseDamageBonusDone(SpellSchoolMask schoolMask) const
{
    int32 doneAdvertisedBenefit = 0;

    auto& overridden = GetAuraEffectsByType(SPELL_AURA_OVERRIDE_SPELL_POWER_BY_AP_PCT);
    if (!overridden.empty())
    {
        // No other bonuses. At least tooltip says so.
        // TODO: Only one aura is possible?
        for (auto&& it : overridden)
            if (it->GetMiscValue() & schoolMask)
                doneAdvertisedBenefit = it->GetAmount() * GetTotalAttackPowerValue(BASE_ATTACK) / 100;
        return doneAdvertisedBenefit;
    }

    for (auto&& it : GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_DONE))
        if (it->GetMiscValue() & schoolMask)
            doneAdvertisedBenefit += it->GetAmount();

    if (GetTypeId() == TYPEID_PLAYER)
    {
        // Base value
        doneAdvertisedBenefit += ToPlayer()->GetBaseSpellPowerBonus();

        // Check if we are ever using mana - PaperDollFrame.lua
        if (GetPowerIndex(POWER_MANA) != MAX_POWERS)
            doneAdvertisedBenefit += std::max(0, int32(GetStat(STAT_INTELLECT)) - 10);  // spellpower from intellect

        // Spell power from SPELL_AURA_MOD_SPELL_POWER_PCT
        doneAdvertisedBenefit *= GetTotalAuraMultiplier(SPELL_AURA_MOD_SPELL_POWER_PCT);

        // Damage bonus from stats
        for (auto&& it : GetAuraEffectsByType(SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT))
        {
            if (it->GetMiscValue() & schoolMask)
            {
                // stat used stored in miscValueB for this aura
                Stats usedStat = Stats(it->GetMiscValueB());
                doneAdvertisedBenefit += int32(CalculatePct(GetStat(usedStat), it->GetAmount()));
            }
        }
        // ... and attack power
        // TODO: I see no one aura of that type.
        for (auto&& it : GetAuraEffectsByType(SPELL_AURA_MOD_SPELL_DAMAGE_OF_ATTACK_POWER))
            if (it->GetMiscValue() & schoolMask)
                doneAdvertisedBenefit += int32(CalculatePct(GetTotalAttackPowerValue(BASE_ATTACK), it->GetAmount()));
    }

    if (IsGuardian())
        doneAdvertisedBenefit += ((Guardian const*)this)->GetBonusDamage();

    return doneAdvertisedBenefit;
}

int32 Unit::SpellBaseDamageBonusTaken(SpellSchoolMask schoolMask) const
{
    int32 TakenAdvertisedBenefit = 0;

    AuraEffectList const& mDamageTaken = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_TAKEN);
    for (AuraEffectList::const_iterator i = mDamageTaken.begin(); i != mDamageTaken.end(); ++i)
        if (((*i)->GetMiscValue() & schoolMask) != 0)
            TakenAdvertisedBenefit += (*i)->GetAmount();

    return TakenAdvertisedBenefit;
}

bool Unit::isSpellCrit(Unit* victim, SpellInfo const* spellProto, SpellSchoolMask schoolMask, WeaponAttackType attackType) const
{
    // not critting spell
    if ((spellProto->AttributesEx2 & SPELL_ATTR2_CANT_CRIT))
        return false;

    switch (spellProto->Id)
    {
        case 53353: // Chimera Shot - Healing can crit, other spells - not
        case 34428: // Victory Rush
        case 53365: // Unholy Strength
            break;
        default:
            if (spellProto->HasEffect(SPELL_EFFECT_HEAL_PCT))
                return false;
            break;
    }

    float crit_chance = GetSpellCrit(victim, spellProto, schoolMask, attackType);

    if (crit_chance == 0.0f)
        return false;
    else if (crit_chance >= 100.0f)
        return true;

    if (roll_chance_f(crit_chance))
        return true;

    return false;
}

float Unit::GetSpellCrit(Unit* victim, SpellInfo const* spellProto, SpellSchoolMask schoolMask, WeaponAttackType attackType, bool periodic) const
{
    //! Mobs can't crit with spells. Player summons can.
    if (GetTypeId() == TYPEID_UNIT && !GetOwnerGUID().IsPlayer() && !(IsVehicle() && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED)))
        return 0.0f;

    if (spellProto->HasAttribute(SPELL_ATTR2_CANT_CRIT))
        return 0.0f;

    float crit_chance = 0.0f;

    // Base crit
    switch (spellProto->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_NONE:
        case SPELL_DAMAGE_CLASS_MAGIC:
        {
            if (spellProto->DmgClass == SPELL_DAMAGE_CLASS_MAGIC && schoolMask & SPELL_SCHOOL_MASK_NORMAL && spellProto->Id != 77478) // Earthquake
                break;

            Player* critOwner = nullptr;
            if (IsPetGuardianStuff())
                if (Unit* owner = GetOwner())
                    critOwner = owner->ToPlayer();

            if (critOwner)
            {
                if (spellProto->Id == 123996 || // Crackling Tiger Lightning, confirmed what scales from max crit (also it seems to melee of Xuen too scales from max crit of monk)
                    spellProto->Id == 132467 || spellProto->Id == 148135 || // Storm, Earth and Fire, Chi Wave damage and Chi Burst damage, confirmed
                    GetEntry() == 61029 || GetEntry() == 15438)             // Shaman fire elementals
                    crit_chance = critOwner->GetFloatValue(PLAYER_FIELD_CRIT_PERCENTAGE);
                crit_chance = std::max(crit_chance, critOwner->GetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE + GetFirstSchoolInMask(schoolMask)));
                crit_chance += GetTotalAuraModifier(SPELL_AURA_MOD_CRIT_PCT);
            }
            else if (GetTypeId() == TYPEID_PLAYER)
            {
                if (spellProto->DmgClass == SPELL_DAMAGE_CLASS_MAGIC && !(schoolMask & SPELL_SCHOOL_MASK_NORMAL))
                    crit_chance = GetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE + GetFirstSchoolInMask(schoolMask));
                else // Unholy Strength, I dunno
                    crit_chance = GetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE + 1);
            }
            else
                crit_chance += float(m_baseSpellCritChance);

            if (!spellProto->IsPositive())
            {
                // Modify critical chance by victim SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_CHANCE
                crit_chance += victim->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_CHANCE, schoolMask);
                // Modify critical chance by victim SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCE
                crit_chance += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCE);
            }
            break;
        }
        case SPELL_DAMAGE_CLASS_MELEE:
        case SPELL_DAMAGE_CLASS_RANGED:
            crit_chance = GetUnitCriticalChance(attackType, victim);
            break;
    }

    bool unableToCrit = victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCE) <= -1000;
    for (auto&& itr : GetAuraEffectsByType(SPELL_AURA_MOD_CRIT_PCT))
        if (itr->GetAmount() <= -100)
            unableToCrit = true;

    // Custom shit
    switch (spellProto->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_NONE:
            // We need more spells to find a general way (if there is any)
            switch (spellProto->Id)
            {
                case 379:   // Earth Shield
                case 7001:  // Lightwell Renew
                case 126154:// Lightwell Renew
                case 53365: // Unholy Strengt
                case 33778: // Lifebloom Final Bloom
                case 64844: // Divine Hymn
                case 71607: // Item - Bauble of True Blood 10m
                case 71646: // Item - Bauble of True Blood 25m
                case 73685: // Unleash Life
                case 85222: // Light of Dawn
                case 102352:// Cenarion Ward
                    break;
                default:
                    return 0.0f;
            }
            // FIXME
            //if (!spellProto->CanCritDamageClassNone())
            //    return 0.0f;
        case SPELL_DAMAGE_CLASS_MAGIC:
        {
            if (victim)
            {
                // scripted (increase crit chance ... against ... target by x%
                AuraEffectList const& mOverrideClassScript = GetAuraEffectsByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
                for (AuraEffectList::const_iterator i = mOverrideClassScript.begin(); i != mOverrideClassScript.end(); ++i)
                {
                    if (!((*i)->IsAffectingSpell(spellProto)))
                        continue;
                    switch ((*i)->GetMiscValue())
                    {
                        // Shatter
                        case  911:
                            if (!victim->HasAuraState(AURA_STATE_FROZEN, spellProto, this))
                                break;
                            crit_chance *= 2; // double the critical chance against frozen targets
                            crit_chance += 50.0f; // plus an additional 50%
                            break;
                        case 7917: // Glyph of Shadowburn
                            if (victim->HasAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, spellProto, this))
                                crit_chance += (*i)->GetAmount();
                            break;
                        case 7997: // Renewed Hope
                        case 7998:
                            if (victim->HasAura(6788))
                                crit_chance += (*i)->GetAmount();
                            break;
                        default:
                            break;
                    }
                }
                // Custom crit by class
                switch (spellProto->SpellFamilyName)
                {
                    case SPELLFAMILY_MAGE:
                        // Glyph of Fire Blast
                        if (spellProto->SpellFamilyFlags[0] == 0x2 && spellProto->SpellIconID == 12)
                            if (victim->HasAuraWithMechanic((1 << MECHANIC_STUN) | (1 << MECHANIC_KNOCKOUT)))
                                if (AuraEffect const *aurEff = GetAuraEffect(56369, EFFECT_0))
                                    crit_chance += aurEff->GetAmount();
                        // Inferno Blast
                        if (spellProto->Id == 108853)
                            return 100.0f;
                        break;
                    case SPELLFAMILY_PALADIN:
                    {
                        switch (spellProto->Id)
                        {
                            case 25912: // Holy Shock (damage)
                            case 25914: // Holy Shock (heal)
                                crit_chance += 25.0f;
                                break;
                            default:
                                break;
                        }
                        break;
                    }
                    case SPELLFAMILY_DRUID:
                    {
                        switch (spellProto->Id)
                        {
                            case 8936:  // Regrowth ...
                                if (periodic)
                                    break;
                                        //  ... has a 60% increased chance for a critical effect.
                                crit_chance += 60.0f;

                                // Glyph of Regrowth
                                // Increases the critical strike chance of your Regrowth by 40%, but removes the periodic component of the spell.
                                if (HasAura(116218))
                                    return 100.0f;
                                break;
                            default:
                                break;
                        }

                        break;
                    }
                    case SPELLFAMILY_SHAMAN:
                    {
                        switch (spellProto->Id)
                        {
                            case 8004:  // Healing Surge
                                if (HasAura(53390))
                                    crit_chance += 30.0f;
                                break;
                            case 51505: // Lava Burst
                            case 77451: // Lava Burst (Mastery: Elemental Overload)
                                if (unableToCrit)
                                    break;
                                return 100.0f;
                            default:
                                break;
                        }
                        break;
                    }
                    case SPELLFAMILY_WARLOCK:
                    {
                        if (spellProto->SpellFamilyFlags[0] & 0x00000004 || spellProto->SpellFamilyFlags[1] & 0x00000040)
                            if (AuraEffect const* bonus = GetAuraEffect(145075, EFFECT_0)) // Destructive Influence, T16 2P bonus
                                crit_chance += bonus->GetAmount();

                        switch (spellProto->Id)
                        {
                            case 6353:  // Soul Fire
                            case 104027:// Soul Fire (Metamorphosis)
                            case 116858:// Chaos Bolt ...
                                return 100.0f;
                            case 31117: // Unstable Affliction dispell
                                if (unableToCrit)
                                    break;
                                return 100.0f;
                                // Hack fix for these spells - They deal Chaos damage, SPELL_SCHOOL_MASK_ALL
                            case 103964:// Touch of Chaos
                            case 124915:// Chaos Wave
                                crit_chance = GetFloatValue(PLAYER_FIELD_SPELL_CRIT_PERCENTAGE + SPELL_SCHOOL_MASK_NORMAL);
                                break;
                            default:
                                break;
                        }
                        break;
                    }
                    case SPELLFAMILY_PRIEST:
                    {
                        switch (spellProto->Id)
                        {
                            case 94472:     // Atonement crit
                            case 140816:    // Power Word: Solace crit
                                return 100.0f;
                        }
                        break;
                    }
                }
            }
            break;
        }
        case SPELL_DAMAGE_CLASS_MELEE:
            if (victim)
            {
                // Custom crit by class
                switch (spellProto->SpellFamilyName)
                {
                    case SPELLFAMILY_DRUID:
                    {
                        switch (spellProto->Id)
                        {
                            case 6785:  // Ravage
                            case 102545:// Ravage!
                                        // Ravage has a 50% increased chance to critically strike targets with over 80% health.
                                if (victim->GetHealthPct() > 80.0f)
                                    crit_chance += 50.0f;
                                break;
                            case 22568: // Ferocious Bite
                                        // +25% crit chance for Ferocious Bite on bleeding targets
                                if (victim->HasAuraState(AURA_STATE_BLEEDING))
                                    crit_chance += 25.0f;
                                break;
                            default:
                                break;
                        }
                        break;
                    }
                    case SPELLFAMILY_WARRIOR:
                    {
                        switch (spellProto->Id)
                        {
                            case 7384:  // Overpower ...
                                        // ... has a 60% increased chance to be a critical strike.
                                crit_chance += 60.0f;
                                break;
                            case 118000:// Dragon Roar ...
                                if (unableToCrit)
                                    break;
                                return 100.0f;
                            default:
                                break;
                        }
                        break;
                    }
                }
            }
            // Both ranged and melee spells
        case SPELL_DAMAGE_CLASS_RANGED:
        {
            if (victim)
            {
                // Ranged Spell (hunters)
                switch (spellProto->Id)
                {
                    case 19434: // Aimed Shot
                    case 82928: // Aimed Shot (Master Marksman)
                    case 56641: // Steady Shot
                        if (HasAura(34483)) // Careful Aim
                            if (victim->GetHealthPct() > 80.0f)
                                crit_chance += 75.0f;
                        break;
                    default:
                        break;
                }
            }
            break;
        }
        default:
            return 0.0f;
    }

    if (unableToCrit)
        return 0.0f;

    if (victim)
    {
        AuraEffectList const& critAuras = victim->GetAuraEffectsByType(SPELL_AURA_MOD_CRIT_CHANCE_FOR_CASTER);
        for (AuraEffectList::const_iterator i = critAuras.begin(); i != critAuras.end(); ++i)
            if ((*i)->GetCasterGUID() == GetGUID() && (*i)->IsAffectingSpell(spellProto))
                crit_chance += (*i)->GetAmount();

        // Mind Blast & Mind Spike debuff
        if (spellProto->Id == 8092)
            victim->RemoveAurasDueToSpell(87178, GetGUID());
        // Bloodthirst
        else if (spellProto->Id == 23881)
            crit_chance *= 2;
    }

    // percent done
    // only players use intelligence for critical chance computations
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_CRITICAL_CHANCE, crit_chance);

    crit_chance = crit_chance > 0.0f ? crit_chance : 0.0f;
    return crit_chance;
}

int32 Unit::SpellCriticalDamageBonus(SpellInfo const* spellProto, int32 damage, Unit* /*victim*/)
{
    // Starting from the Mist of Pandaria, by default all spells and abilities will crit for double damage. https://worldofwarcraft.com/en-us/news/4544194
    int32 critBonus = damage;

    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_CRIT_DAMAGE_BONUS, critBonus);

    critBonus += damage;

    float critMod = (GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_CRIT_DAMAGE_BONUS, spellProto->GetSchoolMask()) - 1.0f) * 100;

    if (critBonus != 0)
        AddPct(critBonus, critMod);

    return std::max(critBonus, 0);
}

uint32 Unit::SpellCriticalHealingBonus(SpellInfo const* /*spellProto*/, uint32 damage, Unit* /*victim*/)
{
    // Calculate critical bonus
    int32 crit_bonus = damage;

    damage += crit_bonus;

    damage = int32(float(damage) * GetTotalAuraMultiplier(SPELL_AURA_MOD_CRITICAL_HEALING_AMOUNT));

    return damage;
}

uint32 Unit::SpellHealingBonusDone(Unit* victim, SpellInfo const* spellProto, uint32 effIndex, uint32 healamount, DamageEffectType damagetype, uint32 stack) const
{
    if (spellProto->HasAttribute(SPELL_ATTR3_NO_DONE_BONUS))
        return healamount;

    if (spellProto->HasAttribute(SPELL_ATTR6_NO_DONE_HEALING_BONUS))
        return healamount;

    if (spellProto->Id == 45470) // Death Strike
        return healamount;

    // For totems get healing bonus from owner (statue isn't totem in fact)
    if (GetTypeId() == TYPEID_UNIT && IsTotem())
        if (Unit* owner = GetOwner())
            return owner->SpellHealingBonusDone(victim, spellProto, effIndex, healamount, damagetype, stack);

    // No bonus healing for potion spells
    if (spellProto->SpellFamilyName == SPELLFAMILY_POTION)
        return healamount;

    float doneTotalMod = 1.0f;
    int32 doneTotal = 0;

    // Healing done percent
    for (auto&& it : GetAuraEffectsByType(SPELL_AURA_MOD_HEALING_DONE_PERCENT))
        AddPct(doneTotalMod, it->GetFloatAmount());

    // done scripted mod (take it from owner)
    Unit const* owner = GetOwner() ? GetOwner() : this;
    AuraEffectList const& mOverrideClassScript = owner->GetAuraEffectsByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    for (auto&& it : owner->GetAuraEffectsByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS))
    {
        if (!it->IsAffectingSpell(spellProto))
            continue;
        switch (it->GetMiscValue())
        {
            case 4415: // Increased Rejuvenation Healing
            case 4953:
            case 3736: // Hateful Totem of the Third Wind / Increased Lesser Healing Wave / LK Arena (4/5/6) Totem of the Third Wind / Savage Totem of the Third Wind
                doneTotal += it->GetAmount();
                break;
            case 21:   // Test of Faith
            case 6935:
            case 6918:
                if (victim->HealthBelowPct(50))
                    AddPct(doneTotalMod, it->GetAmount());
                break;
            case 8477: // Nourish Heal Boost
            {
                int32 stepPercent = it->GetAmount();
                int32 modPercent = 0;
                AuraApplicationMap const& victimAuras = victim->GetAppliedAuras();
                for (AuraApplicationMap::const_iterator itr = victimAuras.begin(); itr != victimAuras.end(); ++itr)
                {
                    Aura const* aura = itr->second->GetBase();
                    if (aura->GetCasterGUID() != GetGUID())
                        continue;
                    SpellInfo const* m_spell = aura->GetSpellInfo();
                    if (m_spell->SpellFamilyName != SPELLFAMILY_DRUID ||
                        !(m_spell->SpellFamilyFlags [1] & 0x00000010 || m_spell->SpellFamilyFlags [0] & 0x50))
                        continue;
                    modPercent += stepPercent * aura->GetStackAmount();
                }
                AddPct(doneTotalMod, modPercent);
                break;
            }
            default:
                break;
        }
    }

    // Done fixed damage bonus auras
    int32 doneAdvertisedBenefit = SpellBaseHealingBonusDone(spellProto->GetSchoolMask());

    // Check for table values
    SpellBonusEntry const* bonus = sSpellMgr->GetSpellBonusData(spellProto->Id);
    float coeff = 0;
    if (bonus)
    {
        if (damagetype == DOT)
        {
            coeff = bonus->dot_damage;
            if (bonus->ap_dot_bonus > 0)
                doneTotal += int32(bonus->ap_dot_bonus * stack * GetTotalAttackPowerValue(
                (spellProto->IsRangedWeaponSpell() && spellProto->DmgClass != SPELL_DAMAGE_CLASS_MELEE) ? RANGED_ATTACK : BASE_ATTACK));
        }
        else
        {
            coeff = bonus->direct_damage;
            if (bonus->ap_bonus > 0)
                doneTotal += int32(bonus->ap_bonus * stack * GetTotalAttackPowerValue(
                (spellProto->IsRangedWeaponSpell() && spellProto->DmgClass != SPELL_DAMAGE_CLASS_MELEE) ? RANGED_ATTACK : BASE_ATTACK));
        }
    }
    else
    {
        // No bonus healing for SPELL_DAMAGE_CLASS_NONE class spells by default
        if (spellProto->DmgClass == SPELL_DAMAGE_CLASS_NONE
            && spellProto->Effects[effIndex].BonusMultiplier == 0.0f)
        {
            return uint32(std::max(healamount * doneTotalMod, 0.0f));
        }
    }

    // Default calculation
    if (doneAdvertisedBenefit && coeff >= 0)
    {
        if (coeff == 0)
            coeff = spellProto->Effects[effIndex].BonusMultiplier;

        if (Player* modOwner = GetSpellModOwner())
        {
            coeff *= 100.0f;
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_BONUS_MULTIPLIER, coeff);
            coeff /= 100.0f;
        }

        doneTotal += int32(doneAdvertisedBenefit * coeff * stack);
    }

    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        switch (spellProto->Effects [i].ApplyAuraName)
        {
            // Bonus healing does not apply to these spells
            case SPELL_AURA_PERIODIC_LEECH:
            case SPELL_AURA_PERIODIC_HEALTH_FUNNEL:
                doneTotal = 0;
                break;
        }
        if (spellProto->Effects [i].Effect == SPELL_EFFECT_HEALTH_LEECH)
            doneTotal = 0;
    }

    if (GetTypeId() == TYPEID_PLAYER) // Now we calculate healing CUSTOM BALACING SYSTEM 
    {
        healamount = GetSpellModOwner()->ToPlayer()->RecaculateHealing(healamount);
    }

    // use float as more appropriate for negative values and percent applying
    float heal = float(int32(healamount) + doneTotal) * doneTotalMod;
    // apply spellmod to Done amount
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, damagetype == DOT ? SPELLMOD_DOT : SPELLMOD_DAMAGE, heal);

    return uint32(std::max(heal, 0.0f));
}

uint32 Unit::SpellHealingBonusTaken(Unit* caster, SpellInfo const* spellProto, uint32 effIndex, uint32 healamount, DamageEffectType damagetype, uint32 stack) const
{
    if (spellProto->HasAttribute(SPELL_ATTR3_NO_DONE_BONUS)
        && spellProto->Id != 115714                             // Glyph of Spellsteal, idk about done bonus, but taken is confirmed
        && spellProto->Id != 115450)                            // Detox (with glyph) same thing
        return healamount;

    float takenTotalMod = 1.0f;

    // Healing taken percent
    takenTotalMod *= GetTotalAuraMultiplier(SPELL_AURA_MOD_HEALING_PCT);

    if (GetMap()->IsBattlegroundOrArena() && !HasAura(134735))
        AddPct(takenTotalMod, -sWorld->getFloatConfig(CONFIG_BATTLE_FATIGUE));

    // Tenacity increase healing % taken
    if (AuraEffect const* Tenacity = GetAuraEffect(58549, 0))
        AddPct(takenTotalMod, Tenacity->GetAmount());

    if (damagetype == DOT)
        AddPct(takenTotalMod, GetTotalAuraModifier(SPELL_AURA_MOD_HOT_PCT));

    if (int32 mod = caster->GetCharmerOrOwnerOrSelf()->GetMaxPositiveAuraModifier(SPELL_AURA_MOD_HEALING_PCT_BY_TARGET_HEALTH_PCT))
    {
        mod *= (100.0f - std::floor(GetHealthPct())) / 100.0f;
        AddPct(takenTotalMod, mod);
    }

    // Healing Done
    int32 takenTotal = 0;

    // Nourish cast
    if (spellProto->SpellFamilyName == SPELLFAMILY_DRUID && spellProto->SpellFamilyFlags [1] & 0x2000000)
    {
        // Rejuvenation, Regrowth, Lifebloom, or Wild Growth
        if (GetAuraEffect(SPELL_AURA_PERIODIC_HEAL, SPELLFAMILY_DRUID, flag128{ 0x50, 0x4000010 }))
            // increase healing by 20%
            takenTotalMod *= 1.2f;
    }

    // Check for table values
    SpellBonusEntry const* bonus = sSpellMgr->GetSpellBonusData(spellProto->Id);
    float coeff = 0;
    float factorMod = 1.0f;
    if (bonus)
        coeff = (damagetype == DOT) ? bonus->dot_damage : bonus->direct_damage;
    else
    {
        // No bonus healing for SPELL_DAMAGE_CLASS_NONE class spells by default
        if (spellProto->DmgClass == SPELL_DAMAGE_CLASS_NONE
            && spellProto->Effects[effIndex].BonusMultiplier == 0.0f)
        {
            return uint32(std::max(healamount * takenTotalMod, 0.0f));
        }
    }

    // Taken fixed damage bonus auras
    int32 TakenAdvertisedBenefit = SpellBaseHealingBonusTaken(spellProto->GetSchoolMask());
    // Default calculation
    if (TakenAdvertisedBenefit && coeff >= 0)
    {
        if (coeff == 0 && spellProto->Effects[effIndex].BonusMultiplier != 1)
            coeff = spellProto->Effects[effIndex].BonusMultiplier;

        if (Player* modOwner = GetSpellModOwner())
        {
            coeff *= 100.0f;
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_BONUS_MULTIPLIER, coeff);
            coeff /= 100.0f;
        }

        takenTotal += int32(TakenAdvertisedBenefit * coeff);
    }

    AuraEffectList const& mHealingGet = GetAuraEffectsByType(SPELL_AURA_MOD_HEALING_RECEIVED);
    for (AuraEffectList::const_iterator i = mHealingGet.begin(); i != mHealingGet.end(); ++i)
        if (caster->GetGUID() == (*i)->GetCasterGUID() && (*i)->IsAffectingSpell(spellProto))
            AddPct(takenTotalMod, (*i)->GetAmount());

    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        switch (spellProto->Effects [i].ApplyAuraName)
        {
            // Bonus healing does not apply to these spells
            case SPELL_AURA_PERIODIC_LEECH:
            case SPELL_AURA_PERIODIC_HEALTH_FUNNEL:
                takenTotal = 0;
                break;
        }
        if (spellProto->Effects [i].Effect == SPELL_EFFECT_HEALTH_LEECH)
            takenTotal = 0;
    }

    float heal = float(int32(healamount) + takenTotal) * takenTotalMod;

    return uint32(std::max(heal, 0.0f));
}

uint32 Unit::SpellAbsorbBonusDone(Unit* victim, SpellInfo const* spellProto, uint32 effIndex, uint32 absorbAmount, uint32 stack) const
{
    // For totems get healing bonus from owner (statue isn't totem in fact)
    if (GetTypeId() == TYPEID_UNIT && IsTotem())
        if (Unit* owner = GetOwner())
            return owner->SpellAbsorbBonusDone(victim, spellProto, effIndex, absorbAmount, stack);

    switch (spellProto->Id)
    {
        case 47753:     // Divine Aegis
        case 86273:     // Illuminated Healing 
        case 114214:    // Angelic Bulwark
        case 77535:     // Blood Shield            
        case 114908:    // Spirit Shell
        case 122470:    // Touch of Karma
            return absorbAmount;
        default:
            break;
    }

    float doneTotal = 0.0f;
    float doneTotalMod = 1.0f;

    // Mastery: Shield Discipline
    if (AuraEffect* eff = GetAuraEffect(77484, EFFECT_0))
        AddPct(doneTotalMod, eff->GetFloatAmount());

    // Apply absorb-reduction auras
    doneTotalMod *= victim->GetTotalAuraMultiplier(SPELL_AURA_MOD_ABSORPTION_PCT);

    if (GetMap()->IsBattlegroundOrArena() && !HasAura(134735))
        AddPct(doneTotalMod, -sWorld->getFloatConfig(CONFIG_BATTLE_FATIGUE));

    // Done fixed damage bonus auras
    int32 doneAdvertisedBenefit = SpellBaseHealingBonusDone(spellProto->GetSchoolMask());

    // Check for table values
    SpellBonusEntry const* bonus = sSpellMgr->GetSpellBonusData(spellProto->Id);
    float coeff = 0.0f;
    if (bonus)
    {
        coeff = bonus->direct_damage;
        if (bonus->ap_bonus > 0)
            doneTotal += bonus->ap_bonus * stack * GetTotalAttackPowerValue(
                (spellProto->IsRangedWeaponSpell() && spellProto->DmgClass != SPELL_DAMAGE_CLASS_MELEE) ? RANGED_ATTACK : BASE_ATTACK);
    }

    // Default calculation
    if (doneAdvertisedBenefit && coeff >= 0)
    {
        if (coeff == 0)
            coeff = spellProto->Effects[effIndex].BonusMultiplier;

        if (Player* modOwner = GetSpellModOwner())
        {
            coeff *= 100.0f;
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_BONUS_MULTIPLIER, coeff);
            coeff /= 100.0f;
        }

        doneTotal += doneAdvertisedBenefit * coeff;
    }

    float absorb = (absorbAmount + doneTotal) * doneTotalMod;
    // apply spellmod to Done amount
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_DAMAGE, absorb);

    return std::max(0.0f, absorb);
}

int32 Unit::SpellBaseHealingBonusDone(SpellSchoolMask schoolMask) const
{
    int32 advertisedBenefit = 0;

    auto& overridden = GetAuraEffectsByType(SPELL_AURA_OVERRIDE_SPELL_POWER_BY_AP_PCT);
    if (!overridden.empty())
    {
        // No other bonuses. At least tooltip says so.
        for (auto&& it : overridden)
            if (it->GetMiscValue() & schoolMask)
                advertisedBenefit = it->GetAmount() * GetTotalAttackPowerValue(BASE_ATTACK) / 100;
        return advertisedBenefit;
    }

    for (auto&& it : GetAuraEffectsByType(SPELL_AURA_MOD_HEALING_DONE))
        if (!it->GetMiscValue() || (it->GetMiscValue() & schoolMask) != 0)
            advertisedBenefit += it->GetAmount();

    // Healing bonus of spirit, intellect and strength
    if (GetTypeId() == TYPEID_PLAYER)
    {
        // Base value
        advertisedBenefit += ToPlayer()->GetBaseSpellPowerBonus();

        // Check if we are ever using mana - PaperDollFrame.lua
        if (GetPowerIndex(POWER_MANA) != MAX_POWERS)
            advertisedBenefit += std::max(0, int32(GetStat(STAT_INTELLECT)) - 10);  // spellpower from intellect

        // Spell power from SPELL_AURA_MOD_SPELL_POWER_PCT
        advertisedBenefit *= GetTotalAuraMultiplier(SPELL_AURA_MOD_SPELL_POWER_PCT);

        // Healing bonus from stats
        for (auto&& it : GetAuraEffectsByType(SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT))
        {
            // stat used dependent from misc value (stat index)
            Stats usedStat = Stats(it->GetSpellInfo()->Effects[it->GetEffIndex()].MiscValue);
            advertisedBenefit += int32(CalculatePct(GetStat(usedStat), it->GetAmount()));
        }

        // ... and attack power
        AuraEffectList const& mHealingDonebyAP = GetAuraEffectsByType(SPELL_AURA_MOD_SPELL_HEALING_OF_ATTACK_POWER);
        for (AuraEffectList::const_iterator i = mHealingDonebyAP.begin(); i != mHealingDonebyAP.end(); ++i)
            if ((*i)->GetMiscValue() & schoolMask)
                advertisedBenefit += int32(CalculatePct(GetTotalAttackPowerValue(BASE_ATTACK), (*i)->GetAmount()));
    }

    if (IsGuardian())
        advertisedBenefit = ((Guardian const*)this)->GetBonusDamage();

    return advertisedBenefit;
}

int32 Unit::SpellBaseHealingBonusTaken(SpellSchoolMask schoolMask) const
{
    int32 advertisedBenefit = 0;

    AuraEffectList const& mDamageTaken = GetAuraEffectsByType(SPELL_AURA_MOD_HEALING);
    for (AuraEffectList::const_iterator i = mDamageTaken.begin(); i != mDamageTaken.end(); ++i)
        if (((*i)->GetMiscValue() & schoolMask) != 0)
            advertisedBenefit += (*i)->GetAmount();

    return advertisedBenefit;
}

bool Unit::IsImmunedToDamage(SpellSchoolMask shoolMask) const
{
    // If m_immuneToSchool type contain this school type, IMMUNE damage.
    SpellImmuneList const& schoolList = m_spellImmune [IMMUNITY_SCHOOL];
    for (SpellImmuneList::const_iterator itr = schoolList.begin(); itr != schoolList.end(); ++itr)
        if (itr->type & shoolMask)
            return true;

    // If m_immuneToDamage type contain magic, IMMUNE damage.
    SpellImmuneList const& damageList = m_spellImmune [IMMUNITY_DAMAGE];
    for (SpellImmuneList::const_iterator itr = damageList.begin(); itr != damageList.end(); ++itr)
        if (itr->type & shoolMask)
            return true;

    return false;
}

bool Unit::IsImmunedToDamage(SpellInfo const* spellInfo) const
{
    if (spellInfo->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)
        return false;

    uint32 shoolMask = spellInfo->GetSchoolMask();
    if (spellInfo->Id != 42292 && spellInfo->Id != 59752)
    {
        // If m_immuneToSchool type contain this school type, IMMUNE damage.
        SpellImmuneList const& schoolList = m_spellImmune [IMMUNITY_SCHOOL];
        for (SpellImmuneList::const_iterator itr = schoolList.begin(); itr != schoolList.end(); ++itr)
            if (itr->type & shoolMask && !spellInfo->CanPierceImmuneAura(sSpellMgr->GetSpellInfo(itr->spellId)))
                return true;
    }

    // If m_immuneToDamage type contain magic, IMMUNE damage.
    SpellImmuneList const& damageList = m_spellImmune [IMMUNITY_DAMAGE];
    for (SpellImmuneList::const_iterator itr = damageList.begin(); itr != damageList.end(); ++itr)
        if (itr->type & shoolMask)
            return true;

    return false;
}

bool Unit::IsImmunedToSpell(SpellInfo const* spellInfo, uint32 effectMask) const
{
    if (!spellInfo)
        return false;

    // Single spell immunity.
    SpellImmuneList const& idList = m_spellImmune [IMMUNITY_ID];
    for (SpellImmuneList::const_iterator itr = idList.begin(); itr != idList.end(); ++itr)
        if (itr->type == spellInfo->Id)
            return true;

    if (spellInfo->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)
        return false;

    if (spellInfo->Dispel)
    {
        SpellImmuneList const& dispelList = m_spellImmune [IMMUNITY_DISPEL];
        for (SpellImmuneList::const_iterator itr = dispelList.begin(); itr != dispelList.end(); ++itr)
            if (itr->type == spellInfo->Dispel)
                return true;
    }

    // Spells that don't have effectMechanics.
    if (spellInfo->Mechanic)
    {
        SpellImmuneList const& mechanicList = m_spellImmune [IMMUNITY_MECHANIC];
        for (SpellImmuneList::const_iterator itr = mechanicList.begin(); itr != mechanicList.end(); ++itr)
            if (itr->type == spellInfo->Mechanic)
                return true;
    }

    bool immuneToAllEffects = true;
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        // State/effect immunities applied by aura expect full spell immunity
        // Ignore effects with mechanic, they are supposed to be checked separately
        if (!spellInfo->Effects [i].IsEffect())
            continue;
        if (!IsImmunedToSpellEffect(spellInfo, i))
        {
            immuneToAllEffects = false;
            break;
        }
    }

    if (immuneToAllEffects) //Return immune only if the target is immune to all spell effects.
        return true;

    if (spellInfo->Id != 42292 && spellInfo->Id != 59752)
    {
        bool positive = !(effectMask & spellInfo->NegativeEffectMask);
        SpellImmuneList const& schoolList = m_spellImmune [IMMUNITY_SCHOOL];
        for (SpellImmuneList::const_iterator itr = schoolList.begin(); itr != schoolList.end(); ++itr)
        {
            SpellInfo const* immuneSpellInfo = sSpellMgr->GetSpellInfo(itr->spellId);
            if ((itr->type & spellInfo->GetSchoolMask())
                && !(immuneSpellInfo && immuneSpellInfo->IsPositive() && positive)
                && !spellInfo->CanPierceImmuneAura(immuneSpellInfo))
                return true;
        }
    }

    return false;
}

bool Unit::IsImmunedToSpellEffect(SpellInfo const* spellInfo, uint32 index) const
{
    if (!spellInfo || !spellInfo->Effects [index].IsEffect())
        return false;

    // If m_immuneToEffect type contain this effect type, IMMUNE effect.
    uint32 effect = spellInfo->Effects [index].Effect;
    SpellImmuneList const& effectList = m_spellImmune [IMMUNITY_EFFECT];
    for (SpellImmuneList::const_iterator itr = effectList.begin(); itr != effectList.end(); ++itr)
        if (itr->type == effect)
            return true;

    if (uint32 mechanic = spellInfo->Effects [index].Mechanic)
    {
        SpellImmuneList const& mechanicList = m_spellImmune [IMMUNITY_MECHANIC];
        for (SpellImmuneList::const_iterator itr = mechanicList.begin(); itr != mechanicList.end(); ++itr)
            if (itr->type == mechanic)
                return true;
    }

    if (uint32 aura = spellInfo->Effects [index].ApplyAuraName)
    {
        SpellImmuneList const& list = m_spellImmune [IMMUNITY_STATE];
        for (SpellImmuneList::const_iterator itr = list.begin(); itr != list.end(); ++itr)
            if (itr->type == aura)
                // if (!(spellInfo->AttributesEx3 & SPELL_ATTR3_IGNORE_HIT_RESULT))
                    return true;

        // Check for immune to application of harmful magical effects
        AuraEffectList const& immuneAuraApply = GetAuraEffectsByType(SPELL_AURA_MOD_IMMUNE_AURA_APPLY_SCHOOL);
        for (AuraEffectList::const_iterator iter = immuneAuraApply.begin(); iter != immuneAuraApply.end(); ++iter)
            if (spellInfo->Dispel == DISPEL_MAGIC &&                                      // Magic debuff
                ((*iter)->GetMiscValue() & spellInfo->GetSchoolMask()) &&  // Check school
                !spellInfo->IsPositiveEffect(index))                                  // Harmful
                return true;
    }

    switch (spellInfo->Effects[index].Effect)
    {
        case SPELL_EFFECT_SCRIPT_EFFECT:
            if (spellInfo->Effects[index].Mechanic != MECHANIC_GRIP) // Gorefiend's Grasp
                break;
        case SPELL_EFFECT_KNOCK_BACK:
        case SPELL_EFFECT_KNOCK_BACK_DEST:
        case SPELL_EFFECT_JUMP_DEST:
            if (spellInfo->HasPvpKnockbackDiminishing())
                if (Player const* player = ToPlayer())
                    if (player->IsImmueToPvpKnockback())
                        return true;
            break;
        default:
            break;
    }

    // Anti-magic Shell; immune to magical aura effects
    if (HasAuraType(SPELL_AURA_MOD_IMMUNE_AURA_APPLY_SCHOOL) && HasAuraEffect(48707, EFFECT_1))
        if (spellInfo->CanBeBlockedByAntiMagicShell(index))
            return true;

    return false;
}

uint32 Unit::GetSchoolImmunityMask() const
{
    uint32 mask = 0;
    SpellImmuneList const& schoolList = m_spellImmune [IMMUNITY_SCHOOL];
    for (SpellImmuneList::const_iterator itr = schoolList.begin(); itr != schoolList.end(); ++itr)
        mask |= itr->type;

    return mask;
}

uint32 Unit::GetMechanicImmunityMask() const
{
    uint32 mask = 0;
    SpellImmuneList const& mechanicList = m_spellImmune [IMMUNITY_MECHANIC];
    for (SpellImmuneList::const_iterator itr = mechanicList.begin(); itr != mechanicList.end(); ++itr)
        mask |= (1 << itr->type);

    return mask;
}

uint32 Unit::MeleeDamageBonusDone(Unit* victim, uint32 pdamage, WeaponAttackType attType, SpellInfo const* spellProto)
{
    if (!victim || pdamage == 0)
        return 0;

    uint32 creatureTypeMask = victim->GetCreatureTypeMask();

    // Done fixed damage bonus auras
    int32 doneFlatBenefit = 0;

    // ..done
    AuraEffectList const& mDamageDoneCreature = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_DONE_CREATURE);
    for (AuraEffectList::const_iterator i = mDamageDoneCreature.begin(); i != mDamageDoneCreature.end(); ++i)
        if (creatureTypeMask & uint32((*i)->GetMiscValue()))
            doneFlatBenefit += (*i)->GetAmount();

    // ..done
    // SPELL_AURA_MOD_DAMAGE_DONE included in weapon damage

    // ..done (base at attack power for marked target and base at attack power for creature type)
    int32 apbonus = 0;

    if (attType == RANGED_ATTACK)
    {
        apbonus += victim->GetTotalAuraModifier(SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS);

        // ..done (base at attack power and creature type)
        AuraEffectList const& mCreatureAttackPower = GetAuraEffectsByType(SPELL_AURA_MOD_RANGED_ATTACK_POWER_VERSUS);
        for (AuraEffectList::const_iterator i = mCreatureAttackPower.begin(); i != mCreatureAttackPower.end(); ++i)
            if (creatureTypeMask & uint32((*i)->GetMiscValue()))
                apbonus += (*i)->GetAmount();
    }
    else
    {
        apbonus += victim->GetTotalAuraModifier(SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS);

        // ..done (base at attack power and creature type)
        AuraEffectList const& mCreatureAttackPower = GetAuraEffectsByType(SPELL_AURA_MOD_MELEE_ATTACK_POWER_VERSUS);
        for (AuraEffectList::const_iterator i = mCreatureAttackPower.begin(); i != mCreatureAttackPower.end(); ++i)
            if (creatureTypeMask & uint32((*i)->GetMiscValue()))
                apbonus += (*i)->GetAmount();
    }

    if (apbonus != 0)                                       // Can be negative
    {
        bool normalized = false;
        if (spellProto)
            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                if (spellProto->Effects [i].Effect == SPELL_EFFECT_NORMALIZED_WEAPON_DMG)
                {
                    normalized = true;
                    break;
                }
        doneFlatBenefit += int32(apbonus / 14.0f * GetAPMultiplier(attType, normalized));
    }

    // Done total percent damage auras
    float doneTotalMod = 1.0f;

    // Some spells don't benefit from pct done mods
    if (spellProto)
        if (!(spellProto->AttributesEx6 & SPELL_ATTR6_NO_DONE_PCT_DAMAGE_MODS))
        {
            AuraEffectList const& modDamagePercentDone = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            for (auto&& it : modDamagePercentDone)
            {
                if (it->GetMiscValue() & spellProto->GetSchoolMask() && !(spellProto->GetSchoolMask() & SPELL_SCHOOL_MASK_NORMAL))
                {
                    // The spell is not physical and the aura affects school of the spell and normal school.
                    // It means that the bonus is already applied to weapon damage.
                    if (it->GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL)
                        continue;

                    if (it->GetSpellInfo()->EquippedItemClass == -1)
                        AddPct(doneTotalMod, it->GetFloatAmount());
                    else if (!(it->GetSpellInfo()->AttributesEx5 & SPELL_ATTR5_SPECIAL_ITEM_CLASS_CHECK) && (it->GetSpellInfo()->EquippedItemSubClassMask == 0))
                        AddPct(doneTotalMod, it->GetFloatAmount());
                    else if (ToPlayer() && ToPlayer()->HasItemFitToSpellRequirements(it->GetSpellInfo()))
                        AddPct(doneTotalMod, it->GetFloatAmount());
                }
            }
        }

    AuraEffectList const& mDamageDoneVersus = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_DONE_VERSUS);
    for (AuraEffectList::const_iterator i = mDamageDoneVersus.begin(); i != mDamageDoneVersus.end(); ++i)
        if (creatureTypeMask & uint32((*i)->GetMiscValue()))
            AddPct(doneTotalMod, (*i)->GetAmount());

    // bonus against aurastate
    AuraEffectList const& mDamageDoneVersusAurastate = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_DONE_VERSUS_AURASTATE);
    for (AuraEffectList::const_iterator i = mDamageDoneVersusAurastate.begin(); i != mDamageDoneVersusAurastate.end(); ++i)
        if (victim->HasAuraState(AuraStateType((*i)->GetMiscValue())))
            AddPct(doneTotalMod, (*i)->GetAmount());

    // Add SPELL_AURA_MOD_DAMAGE_DONE_FOR_MECHANIC percent bonus
    if (spellProto)
        AddPct(doneTotalMod, GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_DAMAGE_DONE_FOR_MECHANIC, spellProto->Mechanic));

    // Probable this must applies as SPELL_ATTR6_NO_DONE_PCT_DAMAGE_MODS but I'm not sure
    if (!spellProto || !spellProto->HasAttribute(SPELL_ATTR0_CU_IGNORE_PVP_POWER))
        if (Player* player = GetCharmerOrOwnerPlayerOrPlayerItself())
            if (victim->GetCharmerOrOwnerPlayerOrPlayerItself())
                AddPct(doneTotalMod, player->GetFloatValue(PLAYER_FIELD_PVP_POWER_DAMAGE));

    // done scripted mod (take it from owner)
    // Unit* owner = GetOwner() ? GetOwner() : this;
    // AuraEffectList const& mOverrideClassScript = owner->GetAuraEffectsByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);

    float tmpDamage = float(int32(pdamage) + doneFlatBenefit) * doneTotalMod;

    // apply spellmod to Done damage
    if (spellProto)
        if (Player* modOwner = GetSpellModOwner())
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_DAMAGE, tmpDamage);

    // bonus result can be negative
    return uint32(std::max(tmpDamage, 0.0f));
}

uint32 Unit::MeleeDamageBonusTaken(Unit* attacker, uint32 pdamage, WeaponAttackType attType, SpellInfo const* spellProto)
{
    if (pdamage == 0)
        return 0;

    int32 takenFlatBenefit = 0;
    float takenTotalCasterMod = 0.0f;

    // get all auras from caster that allow the spell to ignore resistance (sanctified wrath)
    SpellSchoolMask attackSchoolMask = spellProto ? spellProto->GetSchoolMask() : attacker->GetMeleeDamageSchoolMask();
    for (auto&& it : attacker->GetAuraEffectsByType(SPELL_AURA_MOD_IGNORE_TARGET_RESIST))
    {
        if (it->GetMiscValue() & attackSchoolMask)
            takenTotalCasterMod += (float(it->GetAmount()));
    }

    // ..taken
    for (auto&& it : GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_TAKEN))
        if (it->GetMiscValue() & attackSchoolMask)
            takenFlatBenefit += it->GetAmount();

    if (attType != RANGED_ATTACK)
        takenFlatBenefit += GetTotalAuraModifier(SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN);
    else
        takenFlatBenefit += GetTotalAuraModifier(SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN);

    // Taken total percent damage auras
    float takenTotalMod = 1.0f;

    // ..taken
    takenTotalMod *= GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, attackSchoolMask);

    // .. taken pct (special attacks)
    if (spellProto)
    {
        // From caster spells
        uint64 guid = attacker->GetGUID();
        if (Player* player = attacker->GetAffectingPlayer())
            guid = player->GetGUID();

        for (auto&& itr : GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_FROM_CASTER))
            if (itr->GetCasterGUID() == guid && itr->IsAffectingSpell(spellProto))
                AddPct(takenTotalMod, itr->GetAmount());

        // Mod damage from spell mechanic
        uint32 mechanicMask = spellProto->GetAllEffectsMechanicMask();

        // Shred, Maul - "Effects which increase Bleed damage also increase Shred damage"
        if (spellProto->SpellFamilyName == SPELLFAMILY_DRUID && spellProto->SpellFamilyFlags [0] & 0x00008800)
            mechanicMask |= (1 << MECHANIC_BLEED);

        if (mechanicMask)
        {
            for (auto&& it : GetAuraEffectsByType(SPELL_AURA_MOD_MECHANIC_DAMAGE_TAKEN_PERCENT))
                if (mechanicMask & uint32(1 << (it->GetMiscValue())))
                    AddPct(takenTotalMod, it->GetAmount());
        }
    }
    else
    {
        // Auto attack.
        for (auto&& it : GetAuraEffectsByType(SPELL_AURA_MOD_AUTO_ATTACK_DAMAGE_FROM_CASTER))
            if (it->GetCasterGUID() == attacker->GetGUID())
                AddPct(takenTotalMod, it->GetAmount());
    }

    // .. taken pct: class scripts
    //*AuraEffectList const& mclassScritAuras = GetAuraEffectsByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    //for (AuraEffectList::const_iterator i = mclassScritAuras.begin(); i != mclassScritAuras.end(); ++i)
    //{
    //    switch ((*i)->GetMiscValue())
    //    {
    //    }
    //}*/

    for (auto&& it : GetAuraEffectsByType(attType == RANGED_ATTACK ? SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN_PCT : SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN_PCT))
        AddPct(takenTotalMod, it->GetAmount());

    float tmpDamage = 0.0f;

    if (takenTotalCasterMod)
    {
        if (takenFlatBenefit < 0)
        {
            if (takenTotalMod < 1)
                tmpDamage = ((float(CalculatePct(pdamage, takenTotalCasterMod) + takenFlatBenefit) * takenTotalMod) + CalculatePct(pdamage, takenTotalCasterMod));
            else
                tmpDamage = ((float(CalculatePct(pdamage, takenTotalCasterMod) + takenFlatBenefit) + CalculatePct(pdamage, takenTotalCasterMod)) * takenTotalMod);
        }
        else if (takenTotalMod < 1)
            tmpDamage = ((CalculatePct(float(pdamage) + takenFlatBenefit, takenTotalCasterMod) * takenTotalMod) + CalculatePct(float(pdamage) + takenFlatBenefit, takenTotalCasterMod));
    }
    if (!tmpDamage)
        tmpDamage = (float(pdamage) + takenFlatBenefit) * takenTotalMod;

    // bonus result can be negative
    return uint32(std::max(tmpDamage, 0.0f));
}

void Unit::ApplyUberImmune(uint32 spellid, bool apply)
{
    if (apply)
        RemoveAurasWithMechanic(IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK, AURA_REMOVE_BY_DEFAULT, spellid);
    for (uint32 mech = MECHANIC_CHARM; mech != MECHANIC_WOUNDED; ++mech)
    {
        if (mech == MECHANIC_DISARM)
            continue;
        if (1 << mech & IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK)
            ApplySpellImmune(spellid, IMMUNITY_MECHANIC, mech, apply);
    }
}

void Unit::ApplySpellImmune(uint32 spellId, uint32 op, uint32 type, bool apply)
{
    if (apply)
    {
        SpellImmune Immune;
        Immune.spellId = spellId;
        Immune.type = type;
        m_spellImmune[op].push_back(Immune);
    }
    else
    {
        for (auto itr = m_spellImmune[op].begin(); itr != m_spellImmune[op].end();)
        {
            if (itr->spellId == spellId && itr->type == type)
                m_spellImmune[op].erase(itr++);
            else
                ++itr;
        }
    }
}

void Unit::ApplySpellDispelImmunity(const SpellInfo* spellProto, DispelType type, bool apply)
{
    ApplySpellImmune(spellProto->Id, IMMUNITY_DISPEL, type, apply);

    if (apply && spellProto->AttributesEx & SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY)
    {
        // Create dispel mask by dispel type
        uint32 dispelMask = SpellInfo::GetDispelMask(type);
        // Dispel all existing auras vs current dispel type
        AuraApplicationMap& auras = GetAppliedAuras();
        for (AuraApplicationMap::iterator itr = auras.begin(); itr != auras.end();)
        {
            SpellInfo const* spell = itr->second->GetBase()->GetSpellInfo();
            if (spell->GetDispelMask() & dispelMask)
            {
                // Dispel aura
                RemoveAura(itr);
            }
            else
                ++itr;
        }
    }
}

float Unit::GetWeaponProcChance() const
{
    // normalized proc chance for weapon attack speed
    // (odd formula...)
    if (isAttackReady(BASE_ATTACK))
        return (GetAttackTime(BASE_ATTACK) * 1.8f / 1000.0f);
    else if (HasOffhandWeapon() && isAttackReady(OFF_ATTACK))
        return (GetAttackTime(OFF_ATTACK) * 1.6f / 1000.0f);
    return 0;
}

float Unit::GetRealPPMProcChance(SpellInfo const* spellProto, Item const* item, WeaponAttackType attackType) const
{
    if (!spellProto->ProcsPerMinute)
        return 0.0f;

    float procsPerMinute = spellProto->ProcsPerMinute->ProcsPerMinute;
    if (procsPerMinute <= 0.0f)
        return 0.0f;

    float chance = procsPerMinute * 100.0f / 60.0f;
    if (Player const* player = ToPlayer())
    {
        procsPerMinute = spellProto->CalcRppmRate(player, item);
        if (procsPerMinute <= 0.0f)
            return 0.0f;

        float interval = 60.0f / procsPerMinute;
        float sinceLastProc = 1000.0f;
        float sinceLastAttempt = 10.0f;
        auto* procEntry = player->GetSpellHistory()->GetRppmProcEntry(spellProto->Id, attackType);
        if (procEntry)
        {
            auto now = TimeValue::Now();
            sinceLastAttempt = std::min(10.0f, (now - procEntry->LastAttempt).ToMilliseconds() / 1000.0f);
            sinceLastProc = std::min(1000.0f, (now - procEntry->LastSuccess).ToMilliseconds() / 1000.0f);
        }

        chance = procsPerMinute * 100.0f * sinceLastAttempt / 60.0f;
        chance *= std::max(1.0f, 1.0f + ((sinceLastProc / interval) - 1.5f) * 3.0f);
    }

    return chance;
}

float Unit::GetPPMProcChance(uint32 WeaponSpeed, float PPM, SpellInfo const* spellProto) const
{
    // proc per minute chance calculation
    if (PPM <= 0)
        return 0.0f;

    // Apply chance modifer aura
    if (spellProto)
        if (Player* modOwner = GetSpellModOwner())
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_PROC_PER_MINUTE, PPM);

    return (WeaponSpeed * PPM) / 600.0f;   // result is chance in percents (probability = Speed_in_sec * (PPM / 60))
}

void Unit::Mount(uint32 mount, uint32 VehicleId, uint32 creatureEntry)
{
    if (mount)
        SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, mount);

    SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNT);

    if (Player* player = ToPlayer())
    {
        // mount as a vehicle
        if (VehicleId)
        {
            if (CreateVehicleKit(VehicleId, creatureEntry))
            {
                player->SendOnCancelExpectedVehicleRideAura();

                // mounts can also have accessories
                GetVehicleKit()->InstallAllAccessories(false);
            }
        }

        // unsummon pet
        Pet* pet = player->GetPet();
        if (pet)
        {
            Battleground* bg = player->GetBattleground();

            bool unsummon = bg != nullptr;
            if (player->HasAuraType(SPELL_AURA_MOUNTED))
                if (MountCapabilityEntry const* mountCapability = sMountCapabilityStore.LookupEntry(player->GetAuraEffectsByType(SPELL_AURA_MOUNTED).front()->GetAmount()))
                    if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(mountCapability->SpeedModSpell))
                        unsummon = spellInfo->HasAura(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED);

            if (unsummon)
                player->UnsummonPetTemporaryIfAny();
            else
            {
                pet->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
                pet->AI()->EnterEvadeMode();
            }
        }

        player->SendMovementSetCollisionHeight(player->GetCollisionHeight());

    }

    RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_MOUNT);
}

void Unit::Dismount()
{
    if (!IsMounted())
        return;

    SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 0);
    RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNT);

    if (Player* thisPlayer = ToPlayer())
        thisPlayer->SendMovementSetCollisionHeight(thisPlayer->GetCollisionHeight());

    ObjectGuid guid = GetGUID();
    WorldPacket data(SMSG_DISMOUNT, 8);

    data.WriteBit(guid [6]);
    data.WriteBit(guid [3]);
    data.WriteBit(guid [0]);
    data.WriteBit(guid [7]);
    data.WriteBit(guid [1]);
    data.WriteBit(guid [2]);
    data.WriteBit(guid [5]);
    data.WriteBit(guid [4]);

    data.WriteByteSeq(guid [3]);
    data.WriteByteSeq(guid [6]);
    data.WriteByteSeq(guid [7]);
    data.WriteByteSeq(guid [5]);
    data.WriteByteSeq(guid [1]);
    data.WriteByteSeq(guid [4]);
    data.WriteByteSeq(guid [2]);
    data.WriteByteSeq(guid [0]);

    SendMessageToSet(&data, true);

    // dismount as a vehicle
    if (GetTypeId() == TYPEID_PLAYER && GetVehicleKit())
    {
        // Remove vehicle from player
        RemoveVehicleKit();
    }

    RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_NOT_MOUNTED);

    // only resummon old pet if the player is already added to a map
    // this prevents adding a pet to a not created map which would otherwise cause a crash
    // (it could probably happen when logging in after a previous crash)
    if (Player* player = ToPlayer())
    {
        if (Pet* pPet = player->GetPet())
        {
            if (pPet->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED) && !pPet->HasUnitState(UNIT_STATE_STUNNED))
                pPet->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
        }
        else
            player->ResummonPetTemporaryUnSummonedIfAny();
    }
}

MountCapabilityEntry const* Unit::GetMountCapability(uint32 mountType) const
{
    if (!mountType)
        return NULL;

    MountTypeEntry const* mountTypeEntry = sMountTypeStore.LookupEntry(mountType);
    if (!mountTypeEntry)
        return NULL;

    uint32 zoneId, areaId, topZoneId;
    GetZoneAndAreaId(zoneId, areaId);
    topZoneId = zoneId;
    while (true)
    {
        AreaTableEntry const* zone = sAreaTableStore.LookupEntry(topZoneId);
        if (!zone)
            break;
        if (zone->ParentAreaID == 0)
            break;
        topZoneId = zone->ParentAreaID;
    }

    uint32 ridingSkill = 5000;
    if (GetTypeId() == TYPEID_PLAYER)
        ridingSkill = ToPlayer()->GetSkillValue(SKILL_RIDING);

    for (uint32 i = MAX_MOUNT_CAPABILITIES; i > 0; --i)
    {
        MountCapabilityEntry const* mountCapability = sMountCapabilityStore.LookupEntry(mountTypeEntry->MountCapability [i - 1]);
        if (!mountCapability)
            continue;

        if (ridingSkill < mountCapability->RequiredRidingSkill)
            continue;

        // TODO: More research here
        if (!(mountCapability->Flags & 0x01))
        {
            bool ok = false;
            if (mountCapability->Flags & MOUNT_FLAG_CAN_SWIM)
            {
                float x, y, z;
                GetPosition(x, y, z);
                ok = GetMap()->GetLiquidStatus(GetPhaseMask(), x, y, z, MAP_ALL_LIQUIDS) & (LIQUID_MAP_WATER_WALK | LIQUID_MAP_IN_WATER | LIQUID_MAP_UNDER_WATER);
            }
            if (!ok)
                continue;
        }

        if (mountCapability->RequiredMap != -1 && int32(GetMapId()) != mountCapability->RequiredMap)
            continue;

        // TODO: Client reversing is needed
        if ((mountCapability->RequiredMap == 530 || mountCapability->RequiredMap == 870) && IsNoFlyZone(zoneId))
            continue;

        if (mountCapability->RequiredArea && (mountCapability->RequiredArea != zoneId && mountCapability->RequiredArea != areaId && mountCapability->RequiredArea != topZoneId))
            continue;

        // mountCapability->RequiredAura always is 0, this is not the thing.
        // Check permission if we are already mounted.
        if (IsMounted() && mountCapability->SpeedModSpell && !HasAura(mountCapability->SpeedModSpell))
            continue;

        if (mountCapability->RequiredSpell && (GetTypeId() != TYPEID_PLAYER || !ToPlayer()->HasSpell(mountCapability->RequiredSpell)))
            continue;

        return mountCapability;
    }

    return NULL;
}

bool Unit::IsServiceProvider() const
{
    return HasFlag(UNIT_FIELD_NPC_FLAGS,
                   UNIT_NPC_FLAG_VENDOR | UNIT_NPC_FLAG_TRAINER | UNIT_NPC_FLAG_FLIGHTMASTER |
                   UNIT_NPC_FLAG_PETITIONER | UNIT_NPC_FLAG_BATTLEMASTER | UNIT_NPC_FLAG_BANKER |
                   UNIT_NPC_FLAG_INNKEEPER | UNIT_NPC_FLAG_SPIRITHEALER |
                   UNIT_NPC_FLAG_SPIRITGUIDE | UNIT_NPC_FLAG_TABARDDESIGNER | UNIT_NPC_FLAG_AUCTIONEER);
}

void Unit::SetInCombatWith(Unit* enemy)
{
    Unit* eOwner = enemy->GetCharmerOrOwnerOrSelf();
    if (eOwner->IsPvP())
    {
        SetInCombatState(true, enemy);
        return;
    }

    // check for duel
    if (eOwner->GetTypeId() == TYPEID_PLAYER && eOwner->ToPlayer()->duel)
    {
        Unit const* myOwner = GetCharmerOrOwnerOrSelf();
        if (((Player const*) eOwner)->duel->opponent == myOwner)
        {
            SetInCombatState(true, enemy);
            return;
        }
    }
    SetInCombatState(false, enemy);
}

void Unit::CombatStart(Unit* target, bool initialAggro)
{
    if (initialAggro)
    {
        if (!target->IsStandState())
            target->SetStandState(UNIT_STAND_STATE_STAND);

        if (target->GetTypeId() == TYPEID_UNIT && target->IsAIEnabled)
            target->ToCreature()->AI()->AttackedBy(this);   // Call it anyway, only PetAI of all core AI has this function, but we can use it for custom AI (especially derived from PetAI)

        if (!target->IsInCombat() && target->GetTypeId() != TYPEID_PLAYER
            && !target->ToCreature()->HasReactState(REACT_PASSIVE) && target->ToCreature()->IsAIEnabled)
        {
            if (!target->IsPet())                           // PetAI has special handler before AttackStart()
                target->ToCreature()->AI()->AttackStart(this);
        }

        SetInCombatWith(target);
        target->SetInCombatWith(this);
    }
    Unit* who = target->GetCharmerOrOwnerOrSelf();
    if (who->GetTypeId() == TYPEID_PLAYER)
        SetContestedPvP(who->ToPlayer());

    Player* me = GetCharmerOrOwnerPlayerOrPlayerItself();
    if (me && who->IsPvP()
        && (who->GetTypeId() != TYPEID_PLAYER
        || !me->duel || me->duel->opponent != who))
    {
        me->UpdatePvP(true);
        me->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
    }
}

void Unit::SetInCombatState(bool PvP, Unit* enemy)
{
    // only alive units can be in combat
    if (!IsAlive())
        return;

    if (PvP)
        m_combatTimerPvP = 5000;
    // Set it anyway
    m_combatTimerPvE = 15000;

    if (IsInCombat() || HasUnitState(UNIT_STATE_EVADE))
        return;

    SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

    if (Creature* creature = ToCreature())
    {
        // Set home position at place of engaging combat for escorted creatures
        if ((IsAIEnabled && creature->AI()->IsEscorted()) ||
            GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE ||
            GetMotionMaster()->GetCurrentMovementGeneratorType() == POINT_MOTION_TYPE)
            creature->SetHomePosition(GetPositionX(), GetPositionY(), GetPositionZ(), GetOrientation());

        if (enemy)
        {
            if (IsAIEnabled)
            {
                creature->AI()->JustEngagedWith(enemy);

                if (creature->GetEntry() != 69427 && creature->GetEntry() != 69701 && creature->GetEntry() != 69700 && creature->GetEntry() != 69699 && creature->GetEntry() != 72249 && creature->GetEntry() != 71742 && creature->GetEntry() != 71749 && creature->GetEntry() != 71763)
                    RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC); // unit has engaged in combat, remove immunity so players can fight back
            }
            if (creature->GetFormation())
                creature->GetFormation()->MemberEngagingTarget(creature, enemy);
        }

        if (!(creature->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_MOUNTED_COMBAT))
            Dismount();
    }

    for (auto&& it : m_Controlled)
    {
        if (GetCritterGUID() == it->GetGUID())
            continue;
        it->SetInCombatState(PvP, enemy);
        it->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PET_IN_COMBAT);
    }

    if (Player* player = ToPlayer())
    {
        if (auto petBattle = sPetBattleSystem->GetPlayerPetBattle(player->GetGUID()))
            petBattle->EndBattle(nullptr);

        if (player->HasAuraType(SPELL_AURA_ENABLE_WORGER_ALTERED_FORM) && !player->HasAuraType(SPELL_AURA_WORGEN_ALTERED_FORM))
            player->CastSpell(player, 97709, true);

        // Glyph of Disguise. AURA_INTERRUPT_FLAG_UNK28 doesn't seem to be AURA_INTERRUPT_FLAG_COMBAT =/
        RemoveAurasDueToSpell(121308);
    }

    if (GetCurrentSpell(CURRENT_GENERIC_SPELL) && !GetCurrentSpell(CURRENT_GENERIC_SPELL)->GetSpellInfo()->CanBeUsedInCombat())
        InterruptSpell(CURRENT_GENERIC_SPELL, false, false);
}

void Unit::ClearInCombat()
{
    m_combatTimerPvE = 0;
    m_combatTimerPvP = 0;
    RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

    // Player's state will be cleared in Player::UpdateContestedPvP
    if (Creature* creature = ToCreature())
    {
        if (creature->GetCreatureTemplate() && creature->GetCreatureTemplate()->unit_flags & UNIT_FLAG_IMMUNE_TO_PC)
            SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC); // set immunity state to the one from db on evade

        ClearUnitState(UNIT_STATE_ATTACK_PLAYER);
        if (HasFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_TAPPED))
            SetUInt32Value(OBJECT_FIELD_DYNAMIC_FLAGS, creature->GetCreatureTemplate()->dynamicflags);

        if (!IsCharmed())
            return;
    }

    RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PET_IN_COMBAT);
}

bool Unit::isTargetableForAttack(bool checkFakeDeath) const
{
    if (!IsAlive())
        return false;

    if (HasFlag(UNIT_FIELD_FLAGS,
        UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC))
        return false;

    if (GetTypeId() == TYPEID_PLAYER && ToPlayer()->IsGameMaster())
        return false;

    if (HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
        return false;

    return !HasUnitState(UNIT_STATE_UNATTACKABLE) && (!checkFakeDeath || !HasUnitState(UNIT_STATE_DIED));
}

bool Unit::IsValidAttackTarget(Unit const* target) const
{
    return IsValidAttackTarget(target, nullptr, nullptr, false, nullptr);
}

bool Unit::IsValidAttackTarget(Unit const* target, SpellInfo const* bySpell, WorldObject const* obj, bool ignoreVisibility) const
{
    return IsValidAttackTarget(target, bySpell, obj, ignoreVisibility, nullptr);
}

// function based on function Unit::CanAttack from 13850 client
bool Unit::IsValidAttackTarget(Unit const* target, SpellInfo const* bySpell, WorldObject const* obj, bool ignoreVisibility, AttackSwingResult* error) const
{
    ASSERT(target);

    // can't attack self
    if (this == target)
        return false;

    // can't attack unattackable units or GMs
    if (target->HasUnitState(UNIT_STATE_UNATTACKABLE)
        || (target->GetTypeId() == TYPEID_PLAYER && target->ToPlayer()->IsGameMaster()))
    {
        if (ToPlayer() && !bySpell && error)
            *error = ATTACKSWING_CANNOT_ATTACK;
        return false;
    }

    // can't attack own vehicle or passenger
    if (m_vehicle)
    {
        if ((IsOnVehicle(target) || m_vehicle->GetBase()->IsOnVehicle(target)) && target->GetEntry() != 56895 && target->GetEntry() != 73531) // FIXME: Raigonn's weak spot
        {
            if (ToPlayer() && !bySpell && error)
                *error = ATTACKSWING_CANNOT_ATTACK;
            return false;
        }
    }

    // can't attack invisible (ignore stealth for aoe spells) also if the area being looked at is from a spell use the dynamic object created instead of the casting unit.
    if (!ignoreVisibility && (!bySpell || !(bySpell->AttributesEx6 & SPELL_ATTR6_CAN_TARGET_INVISIBLE)) && (obj ? !obj->CanSeeOrDetect(target, bySpell && bySpell->IsAffectingArea()) : !CanSeeOrDetect(target, bySpell && bySpell->IsAffectingArea())))
        return false;

    // can't attack dead
    if ((!bySpell || !bySpell->IsAllowingDeadTarget()) && !target->IsAlive())
    {
        if (ToPlayer() && !bySpell && error)
            *error = ATTACKSWING_DEAD_TARGET;
        return false;
    }
    // can't attack untargetable
    if ((!bySpell || !(bySpell->AttributesEx6 & SPELL_ATTR6_CAN_TARGET_UNTARGETABLE))
        && target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
        return false;

    if (Player const* playerAttacker = ToPlayer())
    {
        if (playerAttacker->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_UBER))
            return false;
    }

    // check flags
    if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_TAXI_FLIGHT | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_UNK_16)
        || (!HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC))
        /* Temporary
        || (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC))
        */
        // check if this is a world trigger cast - GOs are using world triggers to cast their spells, so we need to ignore their immunity flag here, this is a temp workaround, needs removal when go cast is implemented properly
        || (GetEntry() != WORLD_TRIGGER && !target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC))
        /*
        || (GetEntry() != WORLD_TRIGGER &&  target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC))
        */)
        return false;

    if ((!bySpell || !(bySpell->AttributesEx8 & SPELL_ATTR8_ATTACK_IGNORE_IMMUNE_TO_PC_FLAG))
        && (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC))
        // check if this is a world trigger cast - GOs are using world triggers to cast their spells, so we need to ignore their immunity flag here, this is a temp workaround, needs removal when go cast is implemented properly
        && GetEntry() != WORLD_TRIGGER)
        return false;

    // Must be redundant, but I'm not sure
    if (target->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
        return false;
        
    // CvC case - can attack each other only when one of them is hostile
    if (!HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && !target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
        return GetReactionTo(target) <= REP_HOSTILE || target->GetReactionTo(this) <= REP_HOSTILE;

    // PvP, PvC, CvP case
    // can't attack friendly targets
    if (GetReactionTo(target) > REP_NEUTRAL || target->GetReactionTo(this) > REP_NEUTRAL)
        return false;

    // Not all neutral creatures can be attacked
    if (GetReactionTo(target) == REP_NEUTRAL &&
        target->GetReactionTo(this) == REP_NEUTRAL)
    {
        if (!(target->GetTypeId() == TYPEID_PLAYER && GetTypeId() == TYPEID_PLAYER) &&
            !(target->GetTypeId() == TYPEID_UNIT && GetTypeId() == TYPEID_UNIT))
        {
            Player const* player = target->GetTypeId() == TYPEID_PLAYER ? target->ToPlayer() : ToPlayer();
            Unit const* creature = target->GetTypeId() == TYPEID_UNIT ? target : this;

            if (FactionTemplateEntry const* factionTemplate = creature->GetFactionTemplateEntry())
            {
                if (!(player->GetReputationMgr().GetForcedRankIfAny(factionTemplate)))
                    if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionTemplate->faction))
                        if (FactionState const* repState = player->GetReputationMgr().GetState(factionEntry))
                            if (!(repState->Flags & FACTION_FLAG_AT_WAR))
                                return false;

            }
        }
    }

    Creature const* creatureAttacker = ToCreature();
    if (creatureAttacker && creatureAttacker->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_PARTY_MEMBER)
        return false;

    Player const* playerAffectingAttacker = HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) ? GetAffectingPlayer() : NULL;
    Player const* playerAffectingTarget = target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) ? target->GetAffectingPlayer() : NULL;

    // check duel - before sanctuary checks
    if (playerAffectingAttacker && playerAffectingTarget)
        if (playerAffectingAttacker->duel && playerAffectingAttacker->duel->opponent == playerAffectingTarget && playerAffectingAttacker->duel->startTime != 0)
            return true;

    // PvP case - can't attack when attacker or target are in sanctuary
    // however, 13850 client doesn't allow to attack when one of the unit's has sanctuary flag and is pvp
    if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE)
        && ((target->GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1) & UNIT_BYTE2_FLAG_SANCTUARY) || (GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1) & UNIT_BYTE2_FLAG_SANCTUARY)))
    {
        if (ToPlayer() && !bySpell && error)
            *error = ATTACKSWING_CANNOT_ATTACK;
        return false;
    }

    // additional checks - only PvP case
    if (playerAffectingAttacker && playerAffectingTarget)
    {
        if (target->GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1) & UNIT_BYTE2_FLAG_PVP)
            return true;

        if (GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1) & UNIT_BYTE2_FLAG_FFA_PVP
            && target->GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1) & UNIT_BYTE2_FLAG_FFA_PVP)
            return true;

        return (GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1) & UNIT_BYTE2_FLAG_UNK1)
            || (target->GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1) & UNIT_BYTE2_FLAG_UNK1);
    }
    return true;
}

bool Unit::IsValidAssistTarget(Unit const* target) const
{
    return _IsValidAssistTarget(target, NULL);
}

// function based on function Unit::CanAssist from 13850 client
bool Unit::_IsValidAssistTarget(Unit const* target, SpellInfo const* bySpell) const
{
    ASSERT(target);

    // can assist to self
    if (this == target)
        return true;

    // can't assist unattackable units or GMs
    if (target->HasUnitState(UNIT_STATE_UNATTACKABLE)
        || (target->GetTypeId() == TYPEID_PLAYER && target->ToPlayer()->IsGameMaster()))
        return false;

    // can't assist own vehicle or passenger
    if (m_vehicle)
        if (IsOnVehicle(target) || m_vehicle->GetBase()->IsOnVehicle(target))
            return false;

    // can't assist invisible
    if ((!bySpell || !(bySpell->AttributesEx6 & SPELL_ATTR6_CAN_TARGET_INVISIBLE)) && !CanSeeOrDetect(target, bySpell && bySpell->IsAffectingArea()))
        return false;

    // can't assist dead
    if ((!bySpell || !bySpell->IsAllowingDeadTarget()) && !target->IsAlive())
        return false;

    // can't assist untargetable
    if ((!bySpell || !(bySpell->AttributesEx6 & SPELL_ATTR6_CAN_TARGET_UNTARGETABLE))
        && target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
        return false;

    if (!bySpell || !(bySpell->AttributesEx6 & SPELL_ATTR6_ASSIST_IGNORE_IMMUNE_FLAG))
    {
        if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
        {
            if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC))
                return false;
        }
        else
        {
            if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC))
                return false;
        }
    }

    // can't assist non-friendly targets
    if (GetReactionTo(target) <= REP_NEUTRAL
        && target->GetReactionTo(this) <= REP_NEUTRAL
        && (!ToCreature() || !(ToCreature()->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_PARTY_MEMBER)))
        return false;

    // Controlled player case, we can assist creatures (reaction already checked above, our faction == charmer faction)
    if (GetTypeId() == TYPEID_PLAYER && IsCharmed() && GetCharmerGUID().IsCreatureOrVehicle())
        return true;
    // PvP case
    else if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
    {
        Player const* targetPlayerOwner = target->GetAffectingPlayer();
        if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
        {
            Player const* selfPlayerOwner = GetAffectingPlayer();
            if (selfPlayerOwner && targetPlayerOwner)
            {
                // can't assist player which is dueling someone
                if (selfPlayerOwner != targetPlayerOwner
                    && targetPlayerOwner->duel)
                    return false;
            }
            // can't assist player in ffa_pvp zone from outside
            if ((target->GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1) & UNIT_BYTE2_FLAG_FFA_PVP)
                && !(GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1) & UNIT_BYTE2_FLAG_FFA_PVP))
                return false;
            // can't assist player out of sanctuary from sanctuary if has pvp enabled
            if (target->GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1) & UNIT_BYTE2_FLAG_PVP)
                if ((GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1) & UNIT_BYTE2_FLAG_SANCTUARY) && !(target->GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1) & UNIT_BYTE2_FLAG_SANCTUARY))
                    return false;
        }
    }
    // PvC case - player can assist creature only if has specific type flags
    // !target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) &&
    else if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE)
             && (!bySpell || !(bySpell->AttributesEx6 & SPELL_ATTR6_ASSIST_IGNORE_IMMUNE_FLAG))
             && !((target->GetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 1) & UNIT_BYTE2_FLAG_PVP)))
    {
        if (Creature const* creatureTarget = target->ToCreature())
            return creatureTarget->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_PARTY_MEMBER || creatureTarget->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_AID_PLAYERS;
    }
    return true;
}

int32 Unit::ModifyHealth(int32 dVal)
{
    int32 gain = 0;

    if (dVal == 0)
        return 0;

    int32 curHealth = (int32) GetHealth();

    int32 val = dVal + curHealth;
    if (val <= 0)
    {
        SetHealth(0);
        return -curHealth;
    }

    int32 maxHealth = (int32) GetMaxHealth();

    if (val < maxHealth)
    {
        SetHealth(val);
        gain = val - curHealth;
    }
    else if (curHealth != maxHealth)
    {
        SetHealth(maxHealth);
        gain = maxHealth - curHealth;
    }

    return gain;
}

int32 Unit::GetHealthGain(int32 dVal)
{
    int32 gain = 0;

    if (dVal == 0)
        return 0;

    int32 curHealth = (int32) GetHealth();

    int32 val = dVal + curHealth;
    if (val <= 0)
    {
        return -curHealth;
    }

    int32 maxHealth = (int32) GetMaxHealth();

    if (val < maxHealth)
        gain = dVal;
    else if (curHealth != maxHealth)
        gain = maxHealth - curHealth;

    return gain;
}

// returns negative amount on power reduction
int32 Unit::ModifyPower(Powers power, int32 dVal)
{
    int32 gain = 0;

    if (dVal == 0)
        return 0;

    int32 curPower = GetPower(power);

    int32 val = dVal + curPower;
    if (val <= GetMinPower(power))
    {
        SetPower(power, GetMinPower(power));
        return -curPower;
    }

    int32 maxPower = GetMaxPower(power);

    if (val < maxPower)
    {
        SetPower(power, val);
        gain = val - curPower;
    }
    else if (curPower != maxPower)
    {
        SetPower(power, maxPower);
        gain = maxPower - curPower;
    }

    return gain;
}

// returns negative amount on power reduction
int32 Unit::ModifyPowerPct(Powers power, float pct, bool apply)
{
    float amount = (float) GetMaxPower(power);
    ApplyPercentModFloatVar(amount, pct, apply);

    return ModifyPower(power, (int32) amount - GetMaxPower(power));
}

float Unit::GetBasePowerRegen(Powers power) const
{
    switch (power)
    {
        case POWER_FOCUS:
            return 5.0f;
        case POWER_ENERGY:
            return 10.0f;
        case POWER_BURNING_EMBERS:
            return 0.5f;
    }
    return 0.0f;
}

float Unit::GetPowerRegen(Powers power) const
{
    uint32 index = GetPowerIndex(power);
    if (index == MAX_POWERS)
        return 0.0f;

    float regen = GetBasePowerRegen(power);
    switch (power)
    {
        case POWER_RAGE:
            if (IsInCombat())
                regen = GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_POWER_REGEN, power) / 5.0f; // I guess must be in field
            else if (!HasAuraType(SPELL_AURA_INTERRUPT_REGEN))
                regen = -12.5f;
            break;
        case POWER_FOCUS:
        case POWER_ENERGY:
            regen += GetFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER + index);
            regen /= GetFloatValue(UNIT_FIELD_MOD_HASTE_REGEN);
            break;
    }
    return regen;
}

uint32 Unit::GetAttackTime(WeaponAttackType att) const
{
    float f_BaseAttackTime = GetFloatValue(UNIT_FIELD_ATTACK_ROUND_BASE_TIME + att) / m_modAttackSpeedPct [att];
    return (uint32) f_BaseAttackTime;
}

bool Unit::IsAlwaysVisibleFor(WorldObject const* seer) const
{
    if (WorldObject::IsAlwaysVisibleFor(seer))
        return true;

    // Always seen by owner
    if (uint64 guid = GetCharmerOrOwnerGUID())
        if (seer->GetGUID() == guid)
            return true;

    if (Player const* seerPlayer = seer->ToPlayer())
        if (Unit* owner = GetOwner())
            if (Player* ownerPlayer = owner->ToPlayer())
                if (ownerPlayer->IsGroupVisibleFor(seerPlayer))
                    return true;

    return false;
}

bool Unit::IsAlwaysDetectableFor(WorldObject const* seer) const
{
    if (WorldObject::IsAlwaysDetectableFor(seer))
        return true;

    if (HasAuraTypeWithCaster(SPELL_AURA_MOD_STALKED, seer->GetGUID()))
        return true;

    return false;
}

bool Unit::IsVisible() const
{
    return (m_serverSideVisibility.GetValue(SERVERSIDE_VISIBILITY_GM) > SEC_PLAYER) ? false : true;
}

void Unit::SetVisible(bool x)
{
    if (!x)
        m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GM, SEC_GAMEMASTER);
    else
        m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GM, SEC_PLAYER);

    UpdateObjectVisibility();
}

void Unit::UpdateSpeed(UnitMoveType mtype, bool forced)
{
    int32 main_speed_mod = 0;
    float stack_bonus = 1.0f;
    float non_stack_bonus = 1.0f;

    switch (mtype)
    {
        // Only apply debuffs
        case MOVE_FLIGHT_BACK:
        case MOVE_RUN_BACK:
        case MOVE_SWIM_BACK:
            break;
        case MOVE_WALK:
            return;
        case MOVE_RUN:
        {
            if (IsMounted()) // Use on mount auras
            {
                main_speed_mod = GetMaxPositiveAuraModifier(SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED);
                stack_bonus = GetTotalAuraMultiplier(SPELL_AURA_MOD_MOUNTED_SPEED_ALWAYS);
                non_stack_bonus += GetMaxPositiveAuraModifier(SPELL_AURA_MOD_MOUNTED_SPEED_NOT_STACK) / 100.0f;
            }
            else
            {
                GetMovementSpeedModifiers(main_speed_mod, stack_bonus, non_stack_bonus);
            }
            break;
        }
        case MOVE_SWIM:
        {
            main_speed_mod = GetMaxPositiveAuraModifier(SPELL_AURA_MOD_INCREASE_SWIM_SPEED);
            break;
        }
        case MOVE_FLIGHT:
        {
            if (GetTypeId() == TYPEID_UNIT && IsControlledByPlayer()) // not sure if good for pet
            {
                main_speed_mod = GetMaxPositiveAuraModifier(SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED);
                stack_bonus = GetTotalAuraMultiplier(SPELL_AURA_MOD_VEHICLE_SPEED_ALWAYS);

                // for some spells this mod is applied on vehicle owner
                int32 owner_speed_mod = 0;

                if (Unit* owner = GetCharmer())
                    owner_speed_mod = owner->GetMaxPositiveAuraModifier(SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED);

                main_speed_mod = std::max(main_speed_mod, owner_speed_mod);
            }
            else if (IsMounted())
            {
                main_speed_mod = GetMaxPositiveAuraModifier(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED);
                stack_bonus = GetTotalAuraMultiplier(SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS);
            }
            else             // Use not mount (shapeshift for example) auras (should stack)
                main_speed_mod = GetTotalAuraModifier(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED) + GetTotalAuraModifier(SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED);

            non_stack_bonus += GetMaxPositiveAuraModifier(SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK) / 100.0f;

            // Update speed for vehicle if available
            if (GetTypeId() == TYPEID_PLAYER && GetVehicle())
                GetVehicleBase()->UpdateSpeed(MOVE_FLIGHT, true);
            break;
        }
        default:
            TC_LOG_ERROR("entities.unit", "Unit::UpdateSpeed: Unsupported move type (%d)", mtype);
            return;
    }

    // now we ready for speed calculation
    float speed = std::max(non_stack_bonus, stack_bonus);
    if (main_speed_mod)
        AddPct(speed, main_speed_mod);

    switch (mtype)
    {
        case MOVE_RUN:
        case MOVE_SWIM:
        case MOVE_FLIGHT:
        {
            // Set creature speed rate
            if (GetTypeId() == TYPEID_UNIT)
            {
                if (mtype == MOVE_WALK)
                    speed *= IsPet() ? 1.0f : ToCreature()->GetCreatureTemplate()->speed_walk;
                else
                    speed *= IsPet() ? Pet::BaseRunSpeedRate : ToCreature()->GetCreatureTemplate()->speed_run;    // at this point, MOVE_WALK is never reached
            }

            // Normalize speed by 191 aura SPELL_AURA_USE_NORMAL_MOVEMENT_SPEED if need
            /// @todo possible affect only on MOVE_RUN
            if (int32 normalization = GetMaxPositiveAuraModifier(SPELL_AURA_USE_NORMAL_MOVEMENT_SPEED))
            {
                // Use speed from aura
                float max_speed = normalization / (IsControlledByPlayer() ? playerBaseMoveSpeed [mtype] : baseMoveSpeed [mtype]);
                if (speed > max_speed)
                    speed = max_speed;
            }
            break;
        }
        default:
            break;
    }

    // for creature case, we check explicit if mob searched for assistance
    if (GetTypeId() == TYPEID_UNIT)
    {
        if (ToCreature()->HasSearchedAssistance())
            speed *= 0.66f;                                 // best guessed value, so this will be 33% reduction. Based off initial speed, mob can then "run", "walk fast" or "walk".
    }

    // Apply strongest slow aura mod to speed
    int32 slow = GetTotalAuraEffectValue(SPELL_AURA_MOD_DECREASE_SPEED);
    if (slow)
        AddPct(speed, slow);

    if (float minSpeedMod = (float) GetMaxPositiveAuraModifier(SPELL_AURA_MOD_MINIMUM_SPEED))
    {
        float min_speed = minSpeedMod / 100.0f;
        if (speed < min_speed)
            speed = min_speed;
    }

    // I dunno how it must work
    if (HasAuraType(SPELL_AURA_MOD_SPEED_NO_CONTROL) && mtype == MOVE_RUN_BACK)
        speed = m_speed_rate[MOVE_RUN] * baseMoveSpeed[MOVE_RUN] / baseMoveSpeed[MOVE_RUN_BACK];

    SetSpeed(mtype, speed, forced);
}

float Unit::GetSpeed(UnitMoveType mtype) const
{
    return m_speed_rate [mtype] * (IsControlledByPlayer() ? playerBaseMoveSpeed [mtype] : baseMoveSpeed [mtype]);
}

void Unit::SetSpeed(UnitMoveType mtype, float rate, bool forced)
{
    if (rate <= 0.00000023841858) // From client
        rate = 0.0000005f;

    // Update speed only on change
    if (m_speed_rate [mtype] == rate)
        return;

    m_speed_rate [mtype] = rate;

    propagateSpeedChange();

    // Don't build packets because we've got noone to send
    // them to except self, and self is not created at client.
    if (!IsInWorld())
        return;

    static OpcodeServer const moveTypeToOpcode [MAX_MOVE_TYPE] [3] =
    {
        { SMSG_SPLINE_MOVE_SET_WALK_SPEED, SMSG_MOVE_SET_WALK_SPEED, SMSG_MOVE_UPDATE_WALK_SPEED },
        { SMSG_SPLINE_MOVE_SET_RUN_SPEED, SMSG_MOVE_SET_RUN_SPEED, SMSG_MOVE_UPDATE_RUN_SPEED },
        { SMSG_SPLINE_MOVE_SET_RUN_BACK_SPEED, SMSG_MOVE_SET_RUN_BACK_SPEED, SMSG_MOVE_UPDATE_RUN_BACK_SPEED },
        { SMSG_SPLINE_MOVE_SET_SWIM_SPEED, SMSG_MOVE_SET_SWIM_SPEED, SMSG_MOVE_UPDATE_SWIM_SPEED },
        { SMSG_SPLINE_MOVE_SET_SWIM_BACK_SPEED, SMSG_MOVE_SET_SWIM_BACK_SPEED, SMSG_MOVE_UPDATE_SWIM_BACK_SPEED },
        { SMSG_SPLINE_MOVE_SET_TURN_RATE, SMSG_MOVE_SET_TURN_RATE, SMSG_MOVE_UPDATE_TURN_RATE },
        { SMSG_SPLINE_MOVE_SET_FLIGHT_SPEED, SMSG_MOVE_SET_FLIGHT_SPEED, SMSG_MOVE_UPDATE_FLIGHT_SPEED },
        { SMSG_SPLINE_MOVE_SET_FLIGHT_BACK_SPEED, SMSG_MOVE_SET_FLIGHT_BACK_SPEED, SMSG_MOVE_UPDATE_FLIGHT_BACK_SPEED },
        { SMSG_SPLINE_MOVE_SET_PITCH_RATE, SMSG_MOVE_SET_PITCH_RATE, SMSG_MOVE_UPDATE_PITCH_RATE },
    };

    if (GetTypeId() == TYPEID_PLAYER)
    {
        // register forced speed changes for WorldSession::HandleForceSpeedChangeAck
        // and do it only for real sent packets and use run for run/mounted as client expected
        ++ToPlayer()->m_forced_speed_changes [mtype];

        if (Pet* pet = ToPlayer()->GetPet())
            if (pet->GetMotionMaster()->GetCurrentMovementGeneratorType() == FOLLOW_MOTION_TYPE)
                pet->GetMotionMaster()->propagateSpeedChange();
    }

    static MovementStatusElements const speedVal = MSEExtraFloat;
    Movement::ExtraMovementStatusElement extra(&speedVal);
    extra.Data.floatData = GetSpeed(mtype);

    Movement::PacketSender(this, moveTypeToOpcode [mtype] [0], moveTypeToOpcode [mtype] [1], moveTypeToOpcode [mtype] [2], &extra).Send();
}

void Unit::setDeathState(DeathState s)
{
    if (s != ALIVE && s != JUST_RESPAWNED)
    {
        CombatStop();
        DeleteThreatList();
        getHostileRefManager().deleteReferences();

        if (IsNonMeleeSpellCasted(false))
            InterruptNonMeleeSpells(false);

        ExitVehicle();                                      // Exit vehicle before calling RemoveAllControlled
        // vehicles use special type of charm that is not removed by the next function
        // triggering an assert
        UnsummonAllTotems();
        RemoveAllControlled();
        RemoveAllAurasOnDeath();
    }

    if (s == JUST_DIED)
    {
        ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        // remove aurastates allowing special moves
        ClearAllReactives();
        ClearDiminishings();
        if (IsInWorld())
        {
            // Only clear MotionMaster for entities that exists in world
            // Avoids crashes in the following conditions :
            //  * Using 'call pet' on dead pets
            //  * Using 'call stabled pet'
            //  * Logging in with dead pets
            GetMotionMaster()->Clear(false);
            GetMotionMaster()->MoveIdle();
        }
        StopMoving();
        DisableSpline();
        // without this when removing IncreaseMaxHealth aura player may stuck with 1 hp
        // do not why since in IncreaseMaxHealth currenthealth is checked
        SetHealth(0);
        if (GetPowerType() != POWER_MANA)
            SetPower(GetPowerType(), 0);

        // players in instance don't have ZoneScript, but they have InstanceScript
        if (ZoneScript* zoneScript = GetZoneScript() ? GetZoneScript() : GetInstanceScript())
            zoneScript->OnUnitDeath(this);

        m_comboPointResetTimer = 15 * IN_MILLISECONDS;
    }
    else if (s == JUST_RESPAWNED)
        RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE); // clear skinnable for creature and player (at battleground)
    else if (s == ALIVE)
        ClearComboPointHolders();

    m_deathState = s;
}

/*########################################
########                          ########
########       AGGRO SYSTEM       ########
########                          ########
########################################*/
bool Unit::CanHaveThreatList() const
{
    // only creatures can have threat list
    if (GetTypeId() != TYPEID_UNIT)
        return false;

    // only alive units can have threat list
    if (!IsAlive() || isDying())
        return false;

    // totems can not have threat list
    if (ToCreature()->IsTotem())
        return false;

    // vehicles can not have threat list
    //if (ToCreature()->IsVehicle())
    //    return false;

    // summons can not have a threat list, unless they are controlled by a creature
    if (HasUnitTypeMask(UNIT_MASK_MINION | UNIT_MASK_GUARDIAN | UNIT_MASK_CONTROLABLE_GUARDIAN) && ((Pet*)this)->GetOwnerGUID().IsPlayer())
        return false;

    return true;
}

//======================================================================

float Unit::ApplyTotalThreatModifier(float fThreat, SpellSchoolMask schoolMask)
{
    if (!HasAuraType(SPELL_AURA_MOD_THREAT) || fThreat < 0)
        return fThreat;

    SpellSchools school = GetFirstSchoolInMask(schoolMask);

    return fThreat * m_threatModifier [school];
}

//======================================================================

void Unit::AddThreat(Unit* victim, float fThreat, SpellSchoolMask schoolMask, SpellInfo const* threatSpell)
{
    // Only mobs can manage threat lists
    if (CanHaveThreatList())
        m_ThreatManager.addThreat(victim, fThreat, schoolMask, threatSpell);
}

//======================================================================

void Unit::DeleteThreatList()
{
    if (CanHaveThreatList() && !m_ThreatManager.isThreatListEmpty())
        SendClearThreatListOpcode();
    m_ThreatManager.clearReferences();
}

//======================================================================

void Unit::TauntApply(Unit* taunter)
{
    ASSERT(GetTypeId() == TYPEID_UNIT);

    if (!taunter || (taunter->GetTypeId() == TYPEID_PLAYER && taunter->ToPlayer()->IsGameMaster()))
        return;

    if (!CanHaveThreatList())
        return;

    Creature* creature = ToCreature();

    if (creature->HasReactState(REACT_PASSIVE))
        return;

    Unit* target = GetVictim();
    if (target && target == taunter)
        return;

    SetInFront(taunter);
    if (creature->IsAIEnabled)
        creature->AI()->AttackStart(taunter);

    //m_ThreatManager.tauntApply(taunter);
}

//======================================================================

void Unit::TauntFadeOut(Unit* taunter)
{
    ASSERT(GetTypeId() == TYPEID_UNIT);

    if (!taunter || (taunter->GetTypeId() == TYPEID_PLAYER && taunter->ToPlayer()->IsGameMaster()))
        return;

    if (!CanHaveThreatList())
        return;

    Creature* creature = ToCreature();

    if (creature->HasReactState(REACT_PASSIVE))
        return;

    Unit* target = GetVictim();
    if (!target || target != taunter)
        return;

    if (m_ThreatManager.isThreatListEmpty())
    {
        if (creature->IsAIEnabled)
            creature->AI()->EnterEvadeMode();
        return;
    }

    target = creature->SelectVictim();  // might have more taunt auras remaining

    if (target && target != taunter)
    {
        SetInFront(target);
        if (creature->IsAIEnabled)
            creature->AI()->AttackStart(target);
    }
}

//======================================================================

Unit* Creature::SelectVictim()
{
    // function provides main threat functionality
    // next-victim-selection algorithm and evade mode are called
    // threat list sorting etc.

    Unit* target = nullptr;

    for (auto&& itr : m_boundAuras)
    {
        for (auto&& aura : itr.second)
        {
            if (aura->HasEffectType(SPELL_AURA_FIXATE_TARGET))
            {
                target = aura->GetUnitOwner();
                if (target && CanCreatureAttack(target))
                {
                    SetInFront(target);
                    return target;
                }
            }
        }
    }

    // First checking if we have some taunt on us
    AuraEffectList const& tauntAuras = GetAuraEffectsByType(SPELL_AURA_MOD_TAUNT);
    if (!tauntAuras.empty())
    {
        Unit* caster = tauntAuras.back()->GetCaster();

        // The last taunt aura caster is alive an we are happy to attack him
        if (caster && caster->IsAlive())
            return GetVictim();
        else if (tauntAuras.size() > 1)
        {
            // We do not have last taunt aura caster but we have more taunt auras,
            // so find first available target

            // Auras are pushed_back, last caster will be on the end
            AuraEffectList::const_iterator aura = --tauntAuras.end();
            do
            {
                --aura;
                caster = (*aura)->GetCaster();
                if (caster && CanSeeOrDetect(caster, true) && IsValidAttackTarget(caster) && caster->isInAccessiblePlaceFor(ToCreature()))
                {
                    target = caster;
                    break;
                }
            }
            while (aura != tauntAuras.begin());
        }
        else
            target = GetVictim();
    }

    if (CanHaveThreatList())
    {
        if (!target && !m_ThreatManager.isThreatListEmpty())
            // No taunt aura or taunt aura caster is dead standard target selection
            target = m_ThreatManager.getHostilTarget();
    }
    else if (!HasReactState(REACT_PASSIVE))
    {
        // We have player pet probably
        target = getAttackerForHelper();
        if (!target && IsSummon())
        {
            if (Unit* owner = ToTempSummon()->GetOwner())
            {
                if (owner->IsInCombat())
                    target = owner->getAttackerForHelper();
                if (!target)
                {
                    for (ControlList::const_iterator itr = owner->m_Controlled.begin(); itr != owner->m_Controlled.end(); ++itr)
                    {
                        if ((*itr)->IsInCombat())
                        {
                            target = (*itr)->getAttackerForHelper();
                            if (target)
                                break;
                        }
                    }
                }
            }
        }
    }
    else
        return NULL;

    if (target && _IsTargetAcceptable(target) && CanCreatureAttack(target))
    {
        SetInFront(target);
        return target;
    }

    // last case when creature must not go to evade mode:
    // it in combat but attacker not make any damage and not enter to aggro radius to have record in threat list
    // for example at owner command to pet attack some far away creature
    // Note: creature does not have targeted movement generator but has attacker in this case
    for (AttackerSet::const_iterator itr = m_attackers.begin(); itr != m_attackers.end(); ++itr)
    {
        if ((*itr) && !CanCreatureAttack(*itr) && (*itr)->GetTypeId() != TYPEID_PLAYER
            && !(*itr)->ToCreature()->HasUnitTypeMask(UNIT_MASK_CONTROLABLE_GUARDIAN))
            return NULL;
    }

    /// @todo a vehicle may eat some mob, so mob should not evade
    if (GetVehicle())
        return NULL;

    // search nearby enemy before enter evade mode
    if (HasReactState(REACT_AGGRESSIVE))
    {
        target = SelectNearestTargetInAttackDistance(m_CombatDistance ? m_CombatDistance : ATTACK_DISTANCE);

        if (target && _IsTargetAcceptable(target) && CanCreatureAttack(target))
            return target;
    }

    Unit::AuraEffectList const& iAuras = GetAuraEffectsByType(SPELL_AURA_MOD_INVISIBILITY);
    if (!iAuras.empty())
    {
        for (Unit::AuraEffectList::const_iterator itr = iAuras.begin(); itr != iAuras.end(); ++itr)
        {
            if ((*itr)->GetBase()->IsPermanent())
            {
                AI()->EnterEvadeMode();
                break;
            }
        }
        return NULL;
    }

    // enter in evade mode in other case
    AI()->EnterEvadeMode();

    return NULL;
}

//======================================================================
//======================================================================
//======================================================================

float Unit::ApplyEffectModifiers(SpellInfo const* spellProto, uint8 effect_index, float value) const
{
    if (Player* modOwner = GetSpellModOwner())
    {
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_ALL_EFFECTS, value);
        switch (effect_index)
        {
            case 0:
                modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_EFFECT1, value);
                break;
            case 1:
                modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_EFFECT2, value);
                break;
            case 2:
                modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_EFFECT3, value);
                break;
            case 3:
                modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_EFFECT4, value);
                break;
            case 4:
                modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_EFFECT5, value);
                break;
        }
    }
    return value;
}

// function uses real base points (typically value - 1)
int32 Unit::CalculateSpellDamage(Unit const* target, SpellInfo const* spellProto, uint8 effect_index, int32 const* basePoints, Item const* castItem) const
{
    return spellProto->Effects [effect_index].CalcValue(this, basePoints, target, castItem);
}

int32 Unit::CalcSpellDuration(SpellInfo const* spellProto)
{
    uint8 comboPoints = m_movedPlayer ? m_movedPlayer->GetComboPoints() : 0;

    int32 minduration = spellProto->GetDuration();
    int32 maxduration = spellProto->GetMaxDuration();

    int32 duration;

    if (comboPoints && minduration != -1 && minduration != maxduration)
        duration = minduration + int32((maxduration - minduration) * comboPoints / 5);
    else
        duration = minduration;

    return duration;
}

int32 Unit::ModSpellDuration(SpellInfo const* spellProto, Unit const* target, int32 duration, bool positive, uint32 effectMask)
{
    // don't mod permanent auras duration
    if (duration < 0)
        return duration;

    // cut duration only of negative effects
    if (!positive)
    {
        int32 mechanic = spellProto->GetSpellMechanicMaskByEffectMask(effectMask);

        int32 durationMod;
        int32 durationMod_always = 0;
        int32 durationMod_not_stack = 0;

        for (uint8 i = 1; i <= MECHANIC_ENRAGED; ++i)
        {
            if (!(mechanic & 1 << i))
                continue;
            // Find total mod value (negative bonus)
            int32 new_durationMod_always = target->GetTotalAuraModifierByMiscValue(SPELL_AURA_MECHANIC_DURATION_MOD, i);
            // Find max mod (negative bonus)
            int32 new_durationMod_not_stack = target->GetMaxNegativeAuraModifierByMiscValue(SPELL_AURA_MECHANIC_DURATION_MOD_NOT_STACK, i);
            // Check if mods applied before were weaker
            if (new_durationMod_always < durationMod_always)
                durationMod_always = new_durationMod_always;
            if (new_durationMod_not_stack < durationMod_not_stack)
                durationMod_not_stack = new_durationMod_not_stack;
        }

        // Select strongest negative mod
        if (durationMod_always > durationMod_not_stack)
            durationMod = durationMod_not_stack;
        else
            durationMod = durationMod_always;

        if (durationMod != 0)
            AddPct(duration, durationMod);

        // there are only negative mods currently
        durationMod_always = target->GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_AURA_DURATION_BY_DISPEL, spellProto->Dispel);
        durationMod_not_stack = target->GetMaxNegativeAuraModifierByMiscValue(SPELL_AURA_MOD_AURA_DURATION_BY_DISPEL_NOT_STACK, spellProto->Dispel);

        durationMod = 0;
        if (durationMod_always > durationMod_not_stack)
            durationMod += durationMod_not_stack;
        else
            durationMod += durationMod_always;

        if (durationMod != 0)
            AddPct(duration, durationMod);
    }
    else
    {
        // else positive mods here, there are no currently
        // when there will be, change GetTotalAuraModifierByMiscValue to GetTotalPositiveAuraModifierByMiscValue

        // Mixology - duration boost
        if (target->GetTypeId() == TYPEID_PLAYER)
        {
            if ((spellProto->SpellFamilyName == SPELLFAMILY_POTION || spellProto->SpellFamilyName == SPELLFAMILY_GENERIC) && (
                sSpellMgr->IsSpellMemberOfSpellGroup(spellProto->Id, SPELL_GROUP_ELIXIR_BATTLE) ||
                sSpellMgr->IsSpellMemberOfSpellGroup(spellProto->Id, SPELL_GROUP_ELIXIR_GUARDIAN)))
            {
                if (target->ToPlayer()->HasSkill(SKILL_ALCHEMY) && target->HasSpell(spellProto->Effects [0].TriggerSpell))
                    duration *= 2;
            }
        }

        if (target->GetTypeId() == TYPEID_PLAYER)
        {
            auto auras = target->GetAuraEffectsByType(SPELL_AURA_MOD_STAT);
            for (auto&& aurEff : auras)
            {
                if (aurEff->GetId() == spellProto->Id && aurEff->GetBase()->GetDuration() < 900000)
                {
                    if ((aurEff->GetSpellInfo()->SpellFamilyName == SPELLFAMILY_POTION || aurEff->GetSpellInfo()->SpellFamilyName == SPELLFAMILY_GENERIC) && aurEff->GetSpellInfo()->DurationEntry->ID == 42 && (
                        sSpellMgr->IsSpellMemberOfSpellGroup(aurEff->GetId(), SPELL_GROUP_ELIXIR_BATTLE) ||
                        sSpellMgr->IsSpellMemberOfSpellGroup(aurEff->GetId(), SPELL_GROUP_ELIXIR_GUARDIAN)))
                    {
                        duration += aurEff->GetBase()->GetDuration();
                    }
                }
            }
        }
    }

    // Glyphs which increase duration of selfcasted buffs
    if (target == this)
    {
        switch (spellProto->SpellFamilyName)
        {
            case SPELLFAMILY_DRUID:
                if (spellProto->SpellFamilyFlags [0] & 0x100)
                {
                    // Glyph of Thorns
                    if (AuraEffect* aurEff = GetAuraEffect(57862, 0))
                        duration += aurEff->GetAmount() * MINUTE * IN_MILLISECONDS;
                }
                break;
        }
    }
    return std::max(duration, 0);
}

void Unit::ModSpellCastTime(SpellInfo const* spellProto, int32 & castTime, Spell* spell)
{
    if (!spellProto || castTime < 0)
        return;
    // called from caster
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_CASTING_TIME, castTime, spell);

    if (!(spellProto->Attributes & (SPELL_ATTR0_ABILITY | SPELL_ATTR0_TRADESPELL)) && !spellProto->HasAttribute(SPELL_ATTR3_NO_DONE_BONUS))
        castTime = int32(float(castTime) * GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED));
    else if (spellProto->Attributes & SPELL_ATTR0_REQ_AMMO && !(spellProto->AttributesEx2 & SPELL_ATTR2_AUTOREPEAT_FLAG))
        castTime = int32(float(castTime) * m_modAttackSpeedPct [RANGED_ATTACK]);
    else if ((spellProto->SpellVisual[0] == 91 && HasAura(20552)) || // Herbalism with Cultivation
             ((spellProto->SpellVisual[0] == 3881 || spellProto->SpellVisual[0] == 10531) && HasAura(67556)) || // Cooking with Chef Hat.
             (spellProto->SpellVisual[0] == 1008 && HasAura(68978)) || // Skinning with Flayer
             ((spellProto->SpellVisual[0] == 91 || spellProto->SpellVisual[0] == 93 || spellProto->SpellVisual[0] == 1008 || spellProto->SpellVisual[0] == 19438) && HasAura(126342))) // Gathering with Master Forager
        castTime = 500;

    //TODO: Move this shit to scripts
    if (spellProto->Id == 50769 && HasAura(114108)) // Revive + Soul of the Forest
        castTime *= 2;
    if (spellProto->Id == 8690 && HasAura(115933) && HasAura(642))  // Glyph of Righteous Retreat
        castTime /= 2;
    if (spellProto->Id == 118905)       // Static Charge
        if (Unit* shaman = GetOwner())
            if (shaman->HasAura(55442)) // Glyph of Capacitor Totem
                castTime = 3000;
    if (spellProto->Id == 3110 || spellProto->Id == 115746) // Firebolt or Felbolt + Glyph of Demon Training
        if (Unit* warlock = GetOwner())
            if (warlock->HasAura(56249))
                castTime /= 2;
}

DiminishingLevels Unit::GetDiminishing(DiminishingGroup group)
{
    for (Diminishing::iterator i = m_Diminishing.begin(); i != m_Diminishing.end(); ++i)
    {
        if (i->DRGroup != group)
            continue;

        if (!i->hitCount)
            return DIMINISHING_LEVEL_1;

        if (!i->hitTime)
            return DIMINISHING_LEVEL_1;

        // If last spell was casted more than 15 seconds ago - reset the count.
        if (i->stack == 0 && getMSTimeDiff(i->hitTime, getMSTime()) > 15000)
        {
            i->hitCount = DIMINISHING_LEVEL_1;
            return DIMINISHING_LEVEL_1;
        }
        // or else increase the count.
        else
            return DiminishingLevels(i->hitCount);
    }
    return DIMINISHING_LEVEL_1;
}

void Unit::IncrDiminishing(DiminishingGroup group)
{
    // Checking for existing in the table
    for (Diminishing::iterator i = m_Diminishing.begin(); i != m_Diminishing.end(); ++i)
    {
        if (i->DRGroup != group)
            continue;
        if (int32(i->hitCount) < GetDiminishingReturnsMaxLevel(group))
            i->hitCount += 1;
        return;
    }
    m_Diminishing.push_back(DiminishingReturn(group, getMSTime(), DIMINISHING_LEVEL_2));
}

float Unit::ApplyDiminishingToDuration(DiminishingGroup group, int32& duration, Unit* caster, DiminishingLevels Level)
{
    if (duration == -1 || group == DIMINISHING_NONE)
        return 1.0f;

    // test pet/charm masters instead pets/charmeds
    Unit const* tarGetOwner = GetCharmerOrOwner();
    Unit const* casterOwner = caster->GetCharmerOrOwner();

    float mod = 1.0f;

    if (group == DIMINISHING_TAUNT)
    {
        if (GetTypeId() == TYPEID_UNIT && (ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_TAUNT_DIMINISH))
        {
            DiminishingLevels diminish = Level;
            switch (diminish)
            {
                case DIMINISHING_LEVEL_1: break;
                case DIMINISHING_LEVEL_2: mod = 0.65f; break;
                case DIMINISHING_LEVEL_3: mod = 0.4225f; break;
                case DIMINISHING_LEVEL_4: mod = 0.274625f; break;
                case DIMINISHING_LEVEL_TAUNT_IMMUNE: mod = 0.0f; break;
                default: break;
            }
        }
    }
    // Some diminishings applies to mobs too (for example, Stun)
    else if ((GetDiminishingReturnsGroupType(group) == DRTYPE_PLAYER
        && ((tarGetOwner ? (tarGetOwner->GetTypeId() == TYPEID_PLAYER) : (GetTypeId() == TYPEID_PLAYER))
        || (GetTypeId() == TYPEID_UNIT && ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_ALL_DIMINISH)))
        || GetDiminishingReturnsGroupType(group) == DRTYPE_ALL)
    {
        DiminishingLevels diminish = Level;
        switch (diminish)
        {
            case DIMINISHING_LEVEL_1: break;
            case DIMINISHING_LEVEL_2: mod = 0.5f; break;
            case DIMINISHING_LEVEL_3: mod = 0.25f; break;
            case DIMINISHING_LEVEL_IMMUNE: mod = 0.0f; break;
            default: break;
        }
    }

    duration = int32(duration * mod);
    return mod;
}

void Unit::ApplyDiminishingAura(DiminishingGroup group, bool apply)
{
    // Checking for existing in the table
    for (Diminishing::iterator i = m_Diminishing.begin(); i != m_Diminishing.end(); ++i)
    {
        if (i->DRGroup != group)
            continue;

        if (apply)
            i->stack += 1;
        else if (i->stack)
        {
            i->stack -= 1;
            // Remember time after last aura from group removed
            if (i->stack == 0)
                i->hitTime = getMSTime();
        }
        break;
    }
}

float Unit::GetSpellMaxRangeForTarget(Unit const* target, SpellInfo const* spellInfo) const
{
    if (!spellInfo->RangeEntry)
        return 0;
    if (spellInfo->RangeEntry->maxRangeFriend == spellInfo->RangeEntry->maxRangeHostile)
        return spellInfo->GetMaxRange();
    if (target == NULL)
        return spellInfo->GetMaxRange(true);
    return spellInfo->GetMaxRange(!IsHostileTo(target));
}

float Unit::GetSpellMinRangeForTarget(Unit const* target, SpellInfo const* spellInfo) const
{
    if (!spellInfo->RangeEntry)
        return 0;
    if (spellInfo->RangeEntry->minRangeFriend == spellInfo->RangeEntry->minRangeHostile)
        return spellInfo->GetMinRange();
    return spellInfo->GetMinRange(!IsHostileTo(target));
}

Unit* Unit::GetUnit(WorldObject& object, ObjectGuid guid)
{
    return ObjectAccessor::GetUnit(object, guid);
}

Player* Unit::GetPlayer(WorldObject& object, ObjectGuid guid)
{
    return ObjectAccessor::GetPlayer(object, guid);
}

Creature* Unit::GetCreature(WorldObject& object, ObjectGuid guid)
{
    return object.GetMap()->GetCreature(guid);
}

uint32 Unit::GetCreatureType() const
{
    if (GetTypeId() == TYPEID_PLAYER)
    {
        ShapeshiftForm form = GetShapeshiftForm();
        SpellShapeshiftFormEntry const* ssEntry = sSpellShapeshiftFormStore.LookupEntry(form);
        if (ssEntry && ssEntry->creatureType > 0)
            return ssEntry->creatureType;
        else
            return CREATURE_TYPE_HUMANOID;
    }
    else
        return ToCreature()->GetCreatureTemplate()->type;
}

uint32 Unit::GetCreatureTypeMask() const
{
    uint32 creatureType = GetCreatureType();
    return (creatureType >= 1) ? (1 << (creatureType - 1)) : 0;
}

void Unit::SetShapeshiftForm(ShapeshiftForm form)
{
    SetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, 3, form);
}

bool Unit::IsInFeralForm() const
{
    ShapeshiftForm form = GetShapeshiftForm();
    return form == FORM_CAT || form == FORM_BEAR;
}

bool Unit::IsInDisallowedMountForm() const
{
    ShapeshiftForm form = GetShapeshiftForm();
    return form != FORM_NONE && form != FORM_BATTLESTANCE && form != FORM_BERSERKERSTANCE && form != FORM_DEFENSIVESTANCE &&
        form != FORM_SHADOW && form != FORM_STEALTH && form != FORM_UNDEAD && form != FORM_WISE_SERPENT && form != FORM_STURDY_OX && form != FORM_FIERCE_TIGER && form != FORM_MOONKIN;
}

/*#######################################
########                         ########
########       STAT SYSTEM       ########
########                         ########
#######################################*/

bool Unit::HandleStatModifier(UnitMods unitMod, UnitModifierType modifierType, float amount, bool apply)
{
    if (unitMod >= UNIT_MOD_END || modifierType >= MODIFIER_TYPE_END)
    {
        TC_LOG_ERROR("entities.unit", "ERROR in HandleStatModifier(): non-existing UnitMods or wrong UnitModifierType!");
        return false;
    }

    switch (modifierType)
    {
        case BASE_VALUE:
        case TOTAL_VALUE:
            m_auraModifiersGroup [unitMod] [modifierType] += apply ? amount : -amount;
            break;
        case BASE_PCT:
        case TOTAL_PCT:
            ApplyPercentModFloatVar(m_auraModifiersGroup [unitMod] [modifierType], amount, apply);
            break;
        default:
            break;
    }

    if (!CanModifyStats())
        return false;

    switch (unitMod)
    {
        case UNIT_MOD_STAT_STRENGTH:
        case UNIT_MOD_STAT_AGILITY:
        case UNIT_MOD_STAT_STAMINA:
        case UNIT_MOD_STAT_INTELLECT:
        case UNIT_MOD_STAT_SPIRIT:         UpdateStats(GetStatByAuraGroup(unitMod));  break;

        case UNIT_MOD_ARMOR:               UpdateArmor();           break;
        case UNIT_MOD_HEALTH:              UpdateMaxHealth();       break;

        case UNIT_MOD_MANA:
        case UNIT_MOD_RAGE:
        case UNIT_MOD_FOCUS:
        case UNIT_MOD_ENERGY:
        case UNIT_MOD_RUNE:
        case UNIT_MOD_CHI:
        case UNIT_MOD_RUNIC_POWER:          UpdateMaxPower(GetPowerTypeByAuraGroup(unitMod));          break;

        case UNIT_MOD_RESISTANCE_HOLY:
        case UNIT_MOD_RESISTANCE_FIRE:
        case UNIT_MOD_RESISTANCE_NATURE:
        case UNIT_MOD_RESISTANCE_FROST:
        case UNIT_MOD_RESISTANCE_SHADOW:
        case UNIT_MOD_RESISTANCE_ARCANE:   UpdateResistances(GetSpellSchoolByAuraGroup(unitMod));      break;

        case UNIT_MOD_ATTACK_POWER:        UpdateAttackPowerAndDamage();         break;
        case UNIT_MOD_ATTACK_POWER_RANGED: UpdateAttackPowerAndDamage(true);     break;

        case UNIT_MOD_DAMAGE_MAINHAND:     UpdateDamagePhysical(BASE_ATTACK);    break;
        case UNIT_MOD_DAMAGE_OFFHAND:      UpdateDamagePhysical(OFF_ATTACK);     break;
        case UNIT_MOD_DAMAGE_RANGED:       UpdateDamagePhysical(RANGED_ATTACK);  break;

        default:
            break;
    }

    return true;
}

float Unit::GetModifierValue(UnitMods unitMod, UnitModifierType modifierType) const
{
    if (unitMod >= UNIT_MOD_END || modifierType >= MODIFIER_TYPE_END)
    {
        TC_LOG_ERROR("entities.unit", "attempt to access non-existing modifier value from UnitMods!");
        return 0.0f;
    }

    if (modifierType == TOTAL_PCT && m_auraModifiersGroup [unitMod] [modifierType] <= 0.0f)
        return 0.0f;

    return m_auraModifiersGroup [unitMod] [modifierType];
}

float Unit::GetTotalStatValue(Stats stat) const
{
    UnitMods unitMod = UnitMods(UNIT_MOD_STAT_START + stat);

    if (m_auraModifiersGroup [unitMod] [TOTAL_PCT] <= 0.0f)
        return 0.0f;

    // value = ((base_value * base_pct) + total_value) * total_pct
    float value = m_auraModifiersGroup [unitMod] [BASE_VALUE] + GetCreateStat(stat);
    value *= m_auraModifiersGroup [unitMod] [BASE_PCT];
    value += m_auraModifiersGroup [unitMod] [TOTAL_VALUE];
    value *= m_auraModifiersGroup [unitMod] [TOTAL_PCT];

    return value;
}

float Unit::GetTotalAuraModValue(UnitMods unitMod) const
{
    if (unitMod >= UNIT_MOD_END)
    {
        TC_LOG_ERROR("entities.unit", "attempt to access non-existing UnitMods in GetTotalAuraModValue()!");
        return 0.0f;
    }

    if (m_auraModifiersGroup [unitMod] [TOTAL_PCT] <= 0.0f)
        return 0.0f;

    float value = m_auraModifiersGroup [unitMod] [BASE_VALUE];
    value *= m_auraModifiersGroup [unitMod] [BASE_PCT];
    value += m_auraModifiersGroup [unitMod] [TOTAL_VALUE];
    value *= m_auraModifiersGroup [unitMod] [TOTAL_PCT];

    return value;
}

SpellSchools Unit::GetSpellSchoolByAuraGroup(UnitMods unitMod) const
{
    SpellSchools school = SPELL_SCHOOL_NORMAL;

    switch (unitMod)
    {
        case UNIT_MOD_RESISTANCE_HOLY:     school = SPELL_SCHOOL_HOLY;          break;
        case UNIT_MOD_RESISTANCE_FIRE:     school = SPELL_SCHOOL_FIRE;          break;
        case UNIT_MOD_RESISTANCE_NATURE:   school = SPELL_SCHOOL_NATURE;        break;
        case UNIT_MOD_RESISTANCE_FROST:    school = SPELL_SCHOOL_FROST;         break;
        case UNIT_MOD_RESISTANCE_SHADOW:   school = SPELL_SCHOOL_SHADOW;        break;
        case UNIT_MOD_RESISTANCE_ARCANE:   school = SPELL_SCHOOL_ARCANE;        break;

        default:
            break;
    }

    return school;
}

Stats Unit::GetStatByAuraGroup(UnitMods unitMod) const
{
    Stats stat = STAT_STRENGTH;

    switch (unitMod)
    {
        case UNIT_MOD_STAT_STRENGTH:    stat = STAT_STRENGTH;      break;
        case UNIT_MOD_STAT_AGILITY:     stat = STAT_AGILITY;       break;
        case UNIT_MOD_STAT_STAMINA:     stat = STAT_STAMINA;       break;
        case UNIT_MOD_STAT_INTELLECT:   stat = STAT_INTELLECT;     break;
        case UNIT_MOD_STAT_SPIRIT:      stat = STAT_SPIRIT;        break;

        default:
            break;
    }

    return stat;
}

Powers Unit::GetPowerTypeByAuraGroup(UnitMods unitMod) const
{
    switch (unitMod)
    {
        case UNIT_MOD_RAGE:        return POWER_RAGE;
        case UNIT_MOD_FOCUS:       return POWER_FOCUS;
        case UNIT_MOD_ENERGY:      return POWER_ENERGY;
        case UNIT_MOD_RUNE:        return POWER_RUNES;
        case UNIT_MOD_RUNIC_POWER: return POWER_RUNIC_POWER;
        case UNIT_MOD_CHI:         return POWER_CHI;
        default:
        case UNIT_MOD_MANA:        return POWER_MANA;
    }
}

float Unit::GetHasteMultiplier() const
{
    float mult = GetTotalAuraMultiplier(SPELL_AURA_MOD_HASTE, [](AuraEffect const* eff) { return eff->GetAmount() > 0; });

    if (GetClass() != CLASS_WARLOCK && GetClass() != CLASS_MAGE)
        mult *= GetTotalAuraMultiplier(SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK, [](AuraEffect const* eff)
    {
        // Only heroism/blood lust/etc
        return eff->GetAmount() > 0 && eff->GetBase()->HasEffectType(SPELL_AURA_MOD_MELEE_RANGED_HASTE);
    });
    else
        mult *= GetTotalAuraMultiplier(SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK, [](AuraEffect const* eff) { return eff->GetAmount() > 0; });

    if (GetClass() == CLASS_HUNTER)
        mult *= GetTotalAuraMultiplier(SPELL_AURA_MOD_RANGED_HASTE, [](AuraEffect const* eff) { return eff->GetAmount() > 0; });

    if (GetTypeId() == TYPEID_PLAYER)
        AddPct(mult, ToPlayer()->GetRatingBonusValue(CR_HASTE_SPELL));
    return mult;
}

float Unit::GetTotalAttackPowerValue(WeaponAttackType attType) const
{
    if (attType == RANGED_ATTACK)
    {
        int32 ap = GetInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER);
        if (ap < 0)
            return 0.0f;
        return ap * (1.0f + GetFloatValue(UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER));
    }
    else
    {
        int32 ap = GetInt32Value(UNIT_FIELD_ATTACK_POWER);
        if (ap < 0)
            return 0.0f;
        return ap * (1.0f + GetFloatValue(UNIT_FIELD_ATTACK_POWER_MULTIPLIER));
    }
}

float Unit::GetWeaponDamageRange(WeaponAttackType attType, WeaponDamageRange type) const
{
    if (attType == OFF_ATTACK && !HasOffhandWeapon())
        return 0.0f;

    return m_weaponDamage [attType] [type];
}

bool Unit::CanFreeMove() const
{
    return !HasUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_FLEEING | UNIT_STATE_IN_FLIGHT |
                         UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DISTRACTED) && GetOwnerGUID() == 0;
}

void Unit::SetLevel(uint8 lvl, bool sendUpdate)
{
    SetUInt32Value(UNIT_FIELD_LEVEL, lvl);

    // group update
    if (GetTypeId() == TYPEID_PLAYER && ToPlayer()->GetGroup())
        ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_LEVEL);

    if (GetTypeId() == TYPEID_PLAYER)
        sWorld->UpdateCharacterNameDataLevel(ToPlayer()->GetGUID(), lvl);
}

void Unit::SetHealth(uint32 val)
{
    if (getDeathState() == JUST_DIED)
        val = 0;
    else if (GetTypeId() == TYPEID_PLAYER && getDeathState() == DEAD)
        val = 1;
    else
    {
        uint32 maxHealth = GetMaxHealth();
        if (maxHealth < val)
            val = maxHealth;
    }

    SetUInt32Value(UNIT_FIELD_HEALTH, val);

    // group update
    if (Player* player = ToPlayer())
    {
        if (player->GetGroup())
            player->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_CUR_HP);
    }
    else if (Pet* pet = ToCreature()->ToPet())
    {
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_CUR_HP);
        }
    }
}

void Unit::SetMaxHealth(uint32 val)
{
    if (!val)
        val = 1;

    uint32 max = uint32(std::numeric_limits<int32>::max());
    val = std::min(max, val);   // We operate in int32, client aslo want int32

    uint32 health = GetHealth();
    SetUInt32Value(UNIT_FIELD_MAX_HEALTH, val);

    // group update
    if (GetTypeId() == TYPEID_PLAYER)
    {
        if (ToPlayer()->GetGroup())
            ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_MAX_HP);
    }
    else if (Pet* pet = ToCreature()->ToPet())
    {
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_MAX_HP);
        }
    }

    if (val < health)
        SetHealth(val);
}

void Unit::RegenerateHealth()
{
    uint32 curValue = GetHealth();
    uint32 maxValue = GetMaxHealth();

    if (curValue >= maxValue)
        return;

    float addvalue = 0.0f;

    // polymorphed case
    if (IsPolymorphed())
        addvalue = GetMaxHealth() / 3.0f;
    // normal regen case (maybe partly in combat case)
    else if (!IsInCombat() || HasAuraType(SPELL_AURA_MOD_REGEN_DURING_COMBAT))
    {
        addvalue = CountPctFromMaxHealth(1); // 1% per 2 second, confirmed by sniffs
        if (!IsInCombat())
        {
            if (GetTypeId() == TYPEID_UNIT && !GetCharmerOrOwnerGUID().IsPlayer())
                addvalue = GetMaxHealth() / 3.0f;
            else
            {
                for (auto&& itr : GetAuraEffectsByType(SPELL_AURA_MOD_HEALTH_REGEN_PERCENT))
                {
                    // Soul Harvest (101976)
                    if (itr->GetSpellInfo()->HasAttribute(SPELL_ATTR4_NOT_USABLE_IN_ARENA_OR_RATED_BG))
                        if (Battleground const* bg = GetBattlegorund())
                            if (bg->IsArena() || bg->IsRatedBG())
                                continue;
                    AddPct(addvalue, itr->GetAmount());
                }
                addvalue += GetTotalAuraModifier(SPELL_AURA_MOD_REGEN) * 2 * IN_MILLISECONDS / (5 * IN_MILLISECONDS);
            }
        }
        else  // If don't have any aura == 0
            ApplyPct(addvalue, GetTotalAuraModifier(SPELL_AURA_MOD_REGEN_DURING_COMBAT));
    }

    // always regeneration bonus (including combat)
    addvalue += GetTotalAuraModifier(SPELL_AURA_MOD_HEALTH_REGEN_IN_COMBAT);
    // Is it used at all?
    if (Player const* player = ToPlayer())
        addvalue += player->GetHealthRegenFromItems() / 2.5f;

    if (addvalue < 0)
        addvalue = 0;

    ModifyHealth(int32(addvalue));
}

int32 Unit::GetPower(Powers power) const
{
    uint32 powerIndex = GetPowerIndex(power);
    if (powerIndex == MAX_POWERS)
        return 0;

    return GetUInt32Value(UNIT_FIELD_POWER + powerIndex);
}

int32 Unit::GetMaxPower(Powers power) const
{
    uint32 powerIndex = GetPowerIndex(power);
    if (powerIndex == MAX_POWERS)
        return 0;

    return GetInt32Value(UNIT_FIELD_MAX_POWER + powerIndex);
}

void Unit::SetPower(Powers power, int32 val)
{
    uint32 powerIndex = GetPowerIndex(power);
    if (powerIndex == MAX_POWERS)
        return;

    int32 maxPower = int32(GetMaxPower(power));
    if (maxPower < val)
        val = maxPower;

    int32 oldValue = GetPower(power);

    SetInt32Value(UNIT_FIELD_POWER + powerIndex, val);

    if (IsInWorld())
    {
        ObjectGuid guid = GetGUID();

        WorldPacket data(SMSG_POWER_UPDATE, 8 + 4 + 1 + 4);
        data.WriteBit(guid [4]);
        data.WriteBit(guid [6]);
        data.WriteBit(guid [7]);
        data.WriteBit(guid [5]);
        data.WriteBit(guid [2]);
        data.WriteBit(guid [3]);
        data.WriteBit(guid [0]);
        data.WriteBit(guid [1]);
        data.WriteBits(1, 21); // 1 update

        data.WriteByteSeq(guid [7]);
        data.WriteByteSeq(guid [0]);
        data.WriteByteSeq(guid [5]);
        data.WriteByteSeq(guid [3]);
        data.WriteByteSeq(guid [1]);
        data.WriteByteSeq(guid [2]);
        data.WriteByteSeq(guid [4]);

        data << uint8(power);
        data << int32(val);

        data.WriteByteSeq(guid [6]);

        SendMessageToSet(&data, GetTypeId() == TYPEID_PLAYER);
    }

    // group update
    if (Player* player = ToPlayer())
    {
        if (player->GetGroup())
            player->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_CUR_POWER);
    }
    else if (Pet* pet = ToCreature()->ToPet())
    {
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_CUR_POWER);
        }
    }

    // Why there are two auras? They seem to be identical, but SPELL_AURA_PROC_ON_POWER_AMOUNT is only for eclipse
    for (auto&& auraType : { SPELL_AURA_PROC_ON_POWER_AMOUNT, SPELL_AURA_TRIGGER_ON_POWER_LEVEL })
    {
        for (auto&& effect : GetAuraEffectsByType(auraType))
        {
            if (power == effect->GetMiscValue())
            {
                int32 newValue = val;
                int32 threshold = effect->GetAmount();

                // 0 - change up, 1 - change down
                if (effect->GetMiscValueB())
                {
                    if (newValue < oldValue && threshold < oldValue &&  threshold >= newValue)
                        CastSpell(this, effect->GetSpellEffectInfo().TriggerSpell, true, nullptr, effect);
                }
                else
                {
                    if (newValue > oldValue && threshold > oldValue && threshold <= newValue)
                        CastSpell(this, effect->GetSpellEffectInfo().TriggerSpell, true, nullptr, effect);
                }
            }
        }
    }

    if (power == POWER_SHADOW_ORBS)
    {
        if (val > 0)
        {
            if (!HasAura(77487))
            {
                if (HasAura(57985))     // Glyph of Shadow Ravens
                    CastSpell(this, 127850, true);
                else                    // Shadow Orb visual
                    CastSpell(this, 77487, true);
            }
        }
        else
        {
            RemoveAurasDueToSpell(77487);
            RemoveAurasDueToSpell(127850);
        }
        if (val == GetMaxPower(power))  // Devouring Plague announcement
            CastSpell(this, 124495, true);
        else
            RemoveAurasDueToSpell(124495);
    }
}

void Unit::SetMaxPower(Powers power, int32 val)
{
    uint32 powerIndex = GetPowerIndex(power);
    if (powerIndex == MAX_POWERS)
        return;

    int32 cur_power = GetPower(power);
    SetInt32Value(UNIT_FIELD_MAX_POWER + powerIndex, val);

    // group update
    if (GetTypeId() == TYPEID_PLAYER)
    {
        if (ToPlayer()->GetGroup())
            ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_MAX_POWER);
    }
    else if (Pet* pet = ToCreature()->ToPet())
    {
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_MAX_POWER);
        }
    }

    if (val < cur_power)
        SetPower(power, val);
}

uint32 Unit::GetPowerIndex(uint32 powerType) const
{
    /// This is here because hunter pets are of the warrior class.
    /// With the current implementation, the core only gives them
    /// POWER_RAGE, so we enforce the class to hunter so that they
    /// effectively get focus power.
    if (powerType == POWER_ENERGY)
    {
        if (auto creature = ToCreature())
            if (creature->GetCreatureTemplate()->unit_class == CLASS_ROGUE && creature->GetCreatureTemplate()->VehicleId > 0)
                return 0;

        switch (GetEntry())
        {
            case 24207: // Army of the Dead Ghoul
            case 30161: // Wyrmrest Skytalon
            case 62837: // Shekzeer
            case 67977: // Tortos
            case 69699: // Massive Anima Golem
            case 69700: // Large Anima Golem
            case 69701: // Anima Golem
            case 69427: // Dark Animus
            case 69880: // Fallen Zandalari
            case 71543: // Immersius
            case 71734: // Sha of Pride
            case 71466: // Iron Juggernaut
            case 71454: // Malkorok
            case 71420: // Hungry Kunchong
            case 71578: // Amber Parasite
            case 71865: // Garrosh HellScream
            case 72272: // Minion of Y`shaarj
                return 0;
            default:
                break;
        }
        if (HasAura(123746)) // Fel Energy
            return 0;
    }

    if (powerType == POWER_RAGE && GetEntry() == 60708) // Meng the Demented. TODO: Seems to be override display power id
        return 0;

    uint32 classId = GetClass();
    if (Pet const* pet = ToPet())
    {
        if (pet->getPetType() == HUNTER_PET)
            classId = CLASS_HUNTER;
        else if (pet->getPetType() == SUMMON_PET)
        {
            switch (pet->GetOwner()->GetClass())
            {
                case CLASS_DEATH_KNIGHT:
                    return powerType == POWER_ENERGY ? 0 : MAX_POWERS;
                case CLASS_MAGE:
                    return powerType == POWER_MANA ? 0 : MAX_POWERS;
                case CLASS_WARLOCK:
                    return powerType == POWER_ENERGY ? 0 : MAX_POWERS;
            }
        }
    }
    return GetPowerIndexByClass(powerType, classId);
}

int32 Unit::GetCreatePowers(Powers power) const
{
    switch (power)
    {
        case POWER_MANA:
            return GetCreateMana();
        case POWER_RAGE:
            return 1000;
        case POWER_FOCUS:
            if (GetTypeId() == TYPEID_PLAYER)
                return GetClass() == CLASS_HUNTER ? 100 : 0;
            return IsPet() && ToPet()->getPetType() == HUNTER_PET ? 100 : 0;
        case POWER_ENERGY:
        {
            if (IsPet())
                return 200;
            if (IsGuardian())
                if (Unit* owner = GetOwner())
                    if (owner->GetTypeId() == TYPEID_PLAYER && owner->GetClass() == CLASS_WARLOCK)
                        return 200.0f;
            return 100;
        }
        case POWER_RUNIC_POWER:
            return 1000;
        case POWER_RUNES:
            return 0;
        case POWER_ECLIPSE:
            return 100;
        case POWER_HOLY_POWER:
            return 3;
        case POWER_BURNING_EMBERS:
            if (GetTypeId() == TYPEID_PLAYER && GetClass() == CLASS_WARLOCK)
                return ToPlayer()->GetTalentSpecialization() == SPEC_WARLOCK_DESTRUCTION ? 40 : 0;
            break;
        case POWER_DEMONIC_FURY:
            if (GetTypeId() == TYPEID_PLAYER && GetClass() == CLASS_WARLOCK)
                return ToPlayer()->GetTalentSpecialization() == SPEC_WARLOCK_DEMONOLOGY ? 1000 : 0;
            break;
        case POWER_SOUL_SHARDS:
            if (GetTypeId() == TYPEID_PLAYER && GetClass() == CLASS_WARLOCK)
                return ToPlayer()->GetTalentSpecialization() == SPEC_WARLOCK_AFFLICTION ? 400 : 0;
            break;
        case POWER_SHADOW_ORBS:
            if (GetTypeId() == TYPEID_PLAYER && GetClass() == CLASS_PRIEST)
                return ToPlayer()->GetTalentSpecialization() == SPEC_PRIEST_SHADOW ? 3 : 0;
            break;
        case POWER_HEALTH:
            return 0;
        case POWER_CHI:
            return 4;
        default:
            break;
    }

    return 0;
}

void Unit::AddToWorld()
{
    if (!IsInWorld())
    {
        WorldObject::AddToWorld();
    }
    RebuildTerrainSwaps();
}

void Unit::RemoveFromWorld()
{
    // cleanup
    ASSERT(GetGUID());

    if (IsInWorld())
    {
        GetMotionMaster()->Clear(false);                    // Do it here, because MovementInform may provoke casts.

        m_duringRemoveFromWorld = true;
        if (UnitAI* ai = GetAI())
            ai->OnDespawn();
                    
        if (IsVehicle())
            RemoveVehicleKit(true);

        RemoveCharmAuras();
        RemoveBindSightAuras();
        RemoveBoundAuras();

        RemoveAllGameObjects();
        RemoveAllDynObjects();
        RemoveAllAreasTrigger();

        ExitVehicle();  // Remove applied auras with SPELL_AURA_CONTROL_VEHICLE
        UnsummonAllTotems();
        RemoveAllControlled();

        RemoveAreaAurasDueToLeaveWorld();

        if (!GetCharmerGUID().IsEmpty())
        {
            TC_LOG_FATAL("entities.unit", "Unit %u has charmer guid when removed from world", GetEntry());
            ASSERT(false);
        }

        if (Unit* owner = GetOwner())
        {
            if (owner->m_Controlled.find(this) != owner->m_Controlled.end())
            {
                TC_LOG_FATAL("entities.unit", "Unit %u is in controlled list of %u when removed from world", GetEntry(), owner->GetEntry());
                ASSERT(false);
            }
        }

        if (GetTypeId() == TYPEID_PLAYER)
        {
            while (!m_summons.empty())
            {
                auto summon = m_summons.front();
                m_summons.pop_front();
                summon->UnSummon();
            }
        }

        getHostileRefManager().clearReferences();

        WorldObject::RemoveFromWorld();
        m_duringRemoveFromWorld = false;
    }
}

void Unit::CleanupBeforeRemoveFromMap(bool finalCleanup)
{
    // This needs to be before RemoveFromWorld to make GetCaster() return a valid pointer on aura removal
    InterruptNonMeleeSpells(true);

    if (IsInWorld())
        RemoveFromWorld();

    ASSERT(GetGUID());

    // A unit may be in removelist and not in world, but it is still in grid
    // and may have some references during delete
    RemoveAllAuras();
    RemoveAllGameObjects();

    if (finalCleanup)
        m_cleanupDone = true;

    m_Events.KillAllEvents(false);                      // non-delatable (currently casted spells) will not deleted now but it will deleted at call in Map::RemoveAllObjectsInRemoveList
    CombatStop(true);                                   // Because scripts in RemoveAllAuras can trigger fucking spells, need to call InterruptNonMeleeSpells another time
    ClearComboPointHolders();
    DeleteThreatList();
    getHostileRefManager().setOnlineOfflineState(false);
    GetMotionMaster()->Clear(false);                    // remove different non-standard movement generators.
}

void Unit::CleanupsBeforeDelete(bool finalCleanup)
{
    CleanupBeforeRemoveFromMap(finalCleanup);

    WorldObject::CleanupsBeforeDelete(finalCleanup);
}

void Unit::UpdateCharmAI()
{
    switch (GetTypeId())
    {
        case TYPEID_UNIT:
            if (i_disabledAI) // disabled AI must be primary AI
            {
                if (!IsCharmed())
                {
                    delete i_AI;
                    i_AI = i_disabledAI;
                    i_disabledAI = nullptr;
                    TC_LOG_DEBUG("crash", "Unit::UpdateCharmAI, GUID: " UI64FMTD ", entry: %u, charmer: " UI64FMTD ", origianl AI", GetGUID().GetRawValue(), GetEntry(), GetCharmerGUID().GetRawValue());
                }
            }
            else
            {
                if (IsCharmed())
                {
                    i_disabledAI = i_AI;
                    if (isPossessed() || IsVehicle())
                    {
                        TC_LOG_DEBUG("crash", "Unit::UpdateCharmAI, GUID: " UI64FMTD ", entry: %u, charmer: " UI64FMTD ", PossessedAI", GetGUID().GetRawValue(), GetEntry(), GetCharmerGUID().GetRawValue());
                        i_AI = new PossessedAI(ToCreature());
                    }
                    else
                    {
                        TC_LOG_DEBUG("crash", "Unit::UpdateCharmAI, GUID: " UI64FMTD ", entry: %u, charmer: " UI64FMTD ", PetAI", GetGUID().GetRawValue(), GetEntry(), GetCharmerGUID().GetRawValue());
                        i_AI = new PetAI(ToCreature());
                    }
                }
            }
            break;
        case TYPEID_PLAYER:
        {
            if (IsCharmed()) // if we are currently being charmed, then we should apply charm AI
            {
                i_disabledAI = i_AI;

                UnitAI* newAI = nullptr;
                // first, we check if the creature's own AI specifies an override playerai for its owned players
                if (Unit* charmer = GetCharmer())
                {
                    if (Creature* creatureCharmer = charmer->ToCreature())
                    {
                        if (PlayerAI* charmAI = creatureCharmer->IsAIEnabled ? creatureCharmer->AI()->GetAIForCharmedPlayer(ToPlayer()) : nullptr)
                            newAI = charmAI;
                    }
                    else
                    {
                        // This is totally ok and intended in our core.
                        //sLog->outError("Attempt to assign charm AI to player %u who is charmed by non-creature " UI64FMTD ".", GetGUIDLow(), GetCharmerGUID());
                    }
                }
                if (!newAI) // otherwise, we default to the generic one
                    newAI = new SimpleCharmedPlayerAI(ToPlayer(), isPossessed());
                i_AI = newAI;
                newAI->OnCharmed(true);
            }
            else
            {
                if (i_AI)
                {
                    // we allow the charmed PlayerAI to clean up
                    i_AI->OnCharmed(false);
                    // then delete it
                    delete i_AI;
                }
                else
                {
                    TC_LOG_ERROR("misc", "Attempt to remove charm AI from player %u who doesn't currently have charm AI.", GetGUID().GetCounter());
                }
                // and restore our previous PlayerAI (if we had one)
                i_AI = i_disabledAI;
                i_disabledAI = nullptr;
                // IsAIEnabled gets handled in the caller
            }
            break;
        }
        default:
            TC_LOG_ERROR("misc", "Attempt to update charm AI for unit " UI64FMTD ", which is neither player nor creature.", GetGUID().GetRawValue());
    }
}

CharmInfo* Unit::InitCharmInfo()
{
    if (!m_charmInfo)
        m_charmInfo = new CharmInfo(this);

    return m_charmInfo;
}

void Unit::DeleteCharmInfo()
{
    if (!m_charmInfo)
        return;

    m_charmInfo->RestoreState();
    delete m_charmInfo;
    m_charmInfo = NULL;
}

CharmInfo::CharmInfo(Unit* unit)
    : _unit(unit), _CommandState(COMMAND_FOLLOW), _petnumber(0), _barInit(false),
    _isCommandAttack(false), _isAtStay(false), _isFollowing(false), _isReturning(false),
    _stayX(0.0f), _stayY(0.0f), _stayZ(0.0f)
{
    for (uint8 i = 0; i < MAX_SPELL_CHARM; ++i)
        _charmspells [i].SetActionAndType(0, ACT_DISABLED);

    if (_unit->GetTypeId() == TYPEID_UNIT)
    {
        _oldReactState = _unit->ToCreature()->GetReactState();
        _unit->ToCreature()->SetReactState(REACT_PASSIVE);
    }
}

CharmInfo::~CharmInfo()
{ }

void CharmInfo::RestoreState()
{
    if (_unit->GetTypeId() == TYPEID_UNIT)
        if (Creature* creature = _unit->ToCreature())
            creature->SetReactState(_oldReactState);
}

void CharmInfo::InitPetActionBar()
{
    // the first 3 SpellOrActions are attack, follow and stay
    for (uint32 i = 0; i < ACTION_BAR_INDEX_PET_SPELL_START - ACTION_BAR_INDEX_START; ++i)
        SetActionBar(ACTION_BAR_INDEX_START + i, COMMAND_ATTACK - i, ACT_COMMAND);

    // middle 4 SpellOrActions are spells/special attacks/abilities
    for (uint32 i = 0; i < ACTION_BAR_INDEX_PET_SPELL_END - ACTION_BAR_INDEX_PET_SPELL_START; ++i)
        SetActionBar(ACTION_BAR_INDEX_PET_SPELL_START + i, 0, ACT_PASSIVE);

    // last 3 SpellOrActions are reactions
    SetActionBar(ACTION_BAR_INDEX_PET_SPELL_END, REACT_ASSIST, ACT_REACTION);
    SetActionBar(ACTION_BAR_INDEX_PET_SPELL_END + 1, REACT_DEFENSIVE, ACT_REACTION);
    SetActionBar(ACTION_BAR_INDEX_PET_SPELL_END + 2, REACT_PASSIVE, ACT_REACTION);
}

void CharmInfo::InitEmptyActionBar(bool withAttack)
{
    if (withAttack)
        SetActionBar(ACTION_BAR_INDEX_START, COMMAND_ATTACK, ACT_COMMAND);
    else
        SetActionBar(ACTION_BAR_INDEX_START, 0, ACT_PASSIVE);
    for (uint32 x = ACTION_BAR_INDEX_START + 1; x < ACTION_BAR_INDEX_END; ++x)
        SetActionBar(x, 0, ACT_PASSIVE);
}

void CharmInfo::InitPossessCreateSpells()
{
    if (_unit->GetTypeId() == TYPEID_UNIT)
    {
        
        // Adding switch until better way is found. Malcrom
        // Adding entrys to this switch will prevent COMMAND_ATTACK being added to pet bar.
        switch (_unit->GetEntry())
        {
            case 23575: // Mindless Abomination
            case 24783: // Trained Rock Falcon
            case 27664: // Crashin' Thrashin' Racer
            case 40281: // Crashin' Thrashin' Racer
            case 28511: // Eye of Acherus
                break;
            default:
                InitEmptyActionBar();
                break;
        }

        for (uint32 i = 0; i < CREATURE_MAX_SPELLS; ++i)
        {
            uint32 spellId = _unit->ToCreature()->m_spells [i];
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
            if (spellInfo && !(spellInfo->Attributes & SPELL_ATTR0_CASTABLE_WHILE_DEAD))
            {
                if (spellInfo->IsPassive())
                    _unit->CastSpell(_unit, spellInfo, true);
                else
                    AddSpellToActionBar(spellInfo, ACT_PASSIVE, i % MAX_UNIT_ACTION_BAR_INDEX);
            }
        }
    }
    else
        InitEmptyActionBar();    
}

void CharmInfo::InitCharmCreateSpells()
{
    if (_unit->GetTypeId() == TYPEID_PLAYER)                // charmed players don't have spells
    {
        InitEmptyActionBar();
        return;
    }

    switch (_unit->GetEntry())
    {
        case 17469: // Orc Grunt
        case 21748: // Orc Wolf
        case 21750: // Orc Warlock
        case 21747: // Orc Necrolyte
        case 21726: // Summoned Daemon
        case 21752: // Warchief Blackhand
        case 17211: // Human Footman
        case 21664: // Human Charger
        case 21683: // Human Conjurer
        case 21682: // Human Cleric
        case 21160: // Conjured Water Elemental
        case 21684: // King Llane
            break;
        default:
            InitPetActionBar();
            break;
    }

    for (uint32 x = 0; x < MAX_SPELL_CHARM; ++x)
    {
        uint32 spellId = _unit->ToCreature()->m_spells [x];
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);

        if (!spellInfo || spellInfo->Attributes & SPELL_ATTR0_CASTABLE_WHILE_DEAD)
        {
            _charmspells [x].SetActionAndType(spellId, ACT_DISABLED);
            continue;
        }

        if (spellInfo->IsPassive())
        {
            _unit->CastSpell(_unit, spellInfo, true);
            _charmspells [x].SetActionAndType(spellId, ACT_PASSIVE);
        }
        else
        {
            _charmspells [x].SetActionAndType(spellId, ACT_DISABLED);

            ActiveStates newstate = ACT_PASSIVE;

            if (!spellInfo->IsAutocastable())
                newstate = ACT_PASSIVE;
            else
            {
                if (spellInfo->NeedsExplicitUnitTarget())
                {
                    newstate = ACT_ENABLED;
                    ToggleCreatureAutocast(spellInfo, true);
                }
                else
                    newstate = ACT_DISABLED;
            }

            AddSpellToActionBar(spellInfo, newstate);
        }
    }
}

bool CharmInfo::AddSpellToActionBar(SpellInfo const* spellInfo, ActiveStates newstate, uint8 preferredSlot)
{
    uint32 spell_id = spellInfo->Id;
    uint32 first_id = spellInfo->GetFirstRankSpell()->Id;

    ASSERT(preferredSlot < MAX_UNIT_ACTION_BAR_INDEX);
    // new spell rank can be already listed
    for (uint8 i = 0; i < MAX_UNIT_ACTION_BAR_INDEX; ++i)
    {
        if (uint32 action = PetActionBar [i].GetAction())
        {
            if (PetActionBar [i].IsActionBarForSpell() && sSpellMgr->GetFirstSpellInChain(action) == first_id)
            {
                PetActionBar [i].SetAction(spell_id);
                return true;
            }
        }
    }

    // or use empty slot in other case
    for (uint8 i = 0; i < MAX_UNIT_ACTION_BAR_INDEX; ++i)
    {
        uint8 j = (preferredSlot + i) % MAX_UNIT_ACTION_BAR_INDEX;
        if (!PetActionBar[j].GetAction() && PetActionBar[j].IsActionBarForSpell())
        {
            SetActionBar(j, spell_id, newstate == ACT_DECIDE ? spellInfo->IsAutocastable() ? ACT_DISABLED : ACT_PASSIVE : newstate);
            return true;
        }
    }
    return false;
}

bool CharmInfo::RemoveSpellFromActionBar(uint32 spell_id)
{
    uint32 first_id = sSpellMgr->GetFirstSpellInChain(spell_id);

    for (uint8 i = 0; i < MAX_UNIT_ACTION_BAR_INDEX; ++i)
    {
        if (uint32 action = PetActionBar [i].GetAction())
        {
            if (PetActionBar [i].IsActionBarForSpell() && sSpellMgr->GetFirstSpellInChain(action) == first_id)
            {
                SetActionBar(i, 0, ACT_PASSIVE);
                return true;
            }
        }
    }

    return false;
}

void CharmInfo::ToggleCreatureAutocast(SpellInfo const* spellInfo, bool apply)
{
    if (spellInfo->IsPassive())
        return;

    for (uint32 x = 0; x < MAX_SPELL_CHARM; ++x)
        if (spellInfo->Id == _charmspells [x].GetAction())
            _charmspells [x].SetType(apply ? ACT_ENABLED : ACT_DISABLED);
}

void CharmInfo::SetPetNumber(uint32 petnumber, bool statwindow)
{
    _petnumber = petnumber;
    if (statwindow)
        _unit->SetUInt32Value(UNIT_FIELD_PET_NUMBER, _petnumber);
    else
        _unit->SetUInt32Value(UNIT_FIELD_PET_NUMBER, 0);
}

void CharmInfo::LoadPetActionBar(const std::string& data)
{
    InitPetActionBar();

    Tokenizer tokens(data, ' ');

    if (tokens.size() != (ACTION_BAR_INDEX_END - ACTION_BAR_INDEX_START) * 2)
        return;                                             // non critical, will reset to default

    uint8 index = ACTION_BAR_INDEX_START;
    Tokenizer::const_iterator iter = tokens.begin();
    for (; index < ACTION_BAR_INDEX_END; ++iter, ++index)
    {
        // use unsigned cast to avoid sign negative format use at long-> ActiveStates (int) conversion
        ActiveStates type = ActiveStates(atol(*iter));
        ++iter;
        uint32 action = uint32(atol(*iter));

        PetActionBar [index].SetActionAndType(action, type);

        // check correctness
        if (PetActionBar [index].IsActionBarForSpell())
        {
            SpellInfo const* spelInfo = sSpellMgr->GetSpellInfo(PetActionBar [index].GetAction());
            if (!spelInfo)
                SetActionBar(index, 0, ACT_PASSIVE);
            else if (!spelInfo->IsAutocastable())
                SetActionBar(index, PetActionBar [index].GetAction(), ACT_PASSIVE);
        }
    }
}

void CharmInfo::BuildActionBar(WorldPacket* data)
{
    for (uint32 i = 0; i < MAX_UNIT_ACTION_BAR_INDEX; ++i)
        *data << uint32(PetActionBar [i].packedData);
}

void CharmInfo::SetSpellAutocast(SpellInfo const* spellInfo, bool AutocastEnabled)
{
    for (uint8 i = 0; i < MAX_UNIT_ACTION_BAR_INDEX; ++i)
    {
        if (spellInfo->Id == PetActionBar [i].GetAction() && PetActionBar [i].IsActionBarForSpell())
        {
            PetActionBar [i].SetType(AutocastEnabled ? ACT_ENABLED : ACT_DISABLED);
            break;
        }
    }
}

bool Unit::isFrozen() const
{
    return HasAuraState(AURA_STATE_FROZEN);
}

bool Unit::IsSef() const
{
    if (GetEntry() != 69680 && GetEntry() != 69792 && GetEntry() != 69791)
        return false;
    return true;
}

struct ProcTriggeredData
{
    ProcTriggeredData(AuraApplication* auraApp, ProcEventInfo& eventInfo)
        : auraApp(auraApp), aura(auraApp->GetBase()), eventInfo(eventInfo) { }

    Aura* aura;
    AuraApplication* auraApp;
    SpellProcEventEntry const* spellProcEvent = nullptr;
    uint32 effMask = 0;
    ProcEventInfo& eventInfo;
};

typedef std::list< ProcTriggeredData > ProcTriggeredList;

// List of auras that CAN be trigger but may not exist in spell_proc_event
// in most case need for drop charges
// in some types of aura need do additional check
// for example SPELL_AURA_MECHANIC_IMMUNITY - need check for mechanic
bool InitTriggerAuraData()
{
    for (uint16 i = 0; i < TOTAL_AURAS; ++i)
    {
        isTriggerAura [i] = false;
        isNonTriggerAura [i] = false;
        isAlwaysTriggeredAura [i] = false;
    }
    isTriggerAura [SPELL_AURA_PROC_ON_POWER_AMOUNT] = true;
    isTriggerAura [SPELL_AURA_DUMMY] = true;
    isTriggerAura [SPELL_AURA_MOD_CONFUSE] = true;
    isTriggerAura [SPELL_AURA_MOD_THREAT] = true;
    isTriggerAura [SPELL_AURA_MOD_STUN] = true; // Aura does not have charges but needs to be removed on trigger
    isTriggerAura [SPELL_AURA_MOD_DAMAGE_DONE] = true;
    isTriggerAura [SPELL_AURA_MOD_DAMAGE_TAKEN] = true;
    isTriggerAura [SPELL_AURA_MOD_RESISTANCE] = true;
    isTriggerAura [SPELL_AURA_MOD_STEALTH] = true;
    isTriggerAura [SPELL_AURA_MOD_FEAR] = true; // Aura does not have charges but needs to be removed on trigger
    isTriggerAura [SPELL_AURA_MOD_FEAR_2] = true;
    isTriggerAura [SPELL_AURA_MOD_ROOT] = true;
    isTriggerAura [SPELL_AURA_TRANSFORM] = true;
    isTriggerAura [SPELL_AURA_REFLECT_SPELLS] = true;
    isTriggerAura [SPELL_AURA_DAMAGE_IMMUNITY] = true;
    isTriggerAura [SPELL_AURA_PROC_TRIGGER_SPELL] = true;
    isTriggerAura [SPELL_AURA_PROC_TRIGGER_DAMAGE] = true;
    isTriggerAura [SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK] = true;
    isTriggerAura [SPELL_AURA_SCHOOL_ABSORB] = true; // Savage Defense untested
    isTriggerAura [SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT] = true;
    isTriggerAura [SPELL_AURA_MOD_POWER_COST_SCHOOL] = true;
    isTriggerAura [SPELL_AURA_REFLECT_SPELLS_SCHOOL] = true;
    isTriggerAura [SPELL_AURA_MECHANIC_IMMUNITY] = true;
    isTriggerAura [SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN] = true;
    isTriggerAura [SPELL_AURA_SPELL_MAGNET] = true;
    isTriggerAura [SPELL_AURA_MOD_ATTACK_POWER] = true;
    isTriggerAura [SPELL_AURA_MOD_POWER_REGEN_PERCENT] = true;
    isTriggerAura [SPELL_AURA_ADD_CASTER_HIT_TRIGGER] = true;
    isTriggerAura [SPELL_AURA_OVERRIDE_CLASS_SCRIPTS] = true;
    isTriggerAura [SPELL_AURA_MOD_MECHANIC_RESISTANCE] = true;
    isTriggerAura [SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS] = true;
    isTriggerAura [SPELL_AURA_MOD_MELEE_HASTE] = true;
    isTriggerAura [SPELL_AURA_MOD_MELEE_HASTE_3] = true;
    isTriggerAura [SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE] = true;
    isTriggerAura [SPELL_AURA_RAID_PROC_FROM_CHARGE] = true;
    isTriggerAura [SPELL_AURA_RAID_PROC_FROM_CHARGE_WITH_VALUE] = true;
    isTriggerAura [SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE] = true;
    isTriggerAura [SPELL_AURA_MOD_DAMAGE_FROM_CASTER] = true;
    isTriggerAura [SPELL_AURA_MOD_SPELL_CRIT_CHANCE] = true;
    isTriggerAura [SPELL_AURA_ABILITY_IGNORE_AURASTATE] = true;
    isTriggerAura [SPELL_AURA_PROC_TRIGGER_SPELL_2] = true;
    isTriggerAura [SPELL_AURA_PROC_TRIGGER_SPELL_COPY] = true;

    isNonTriggerAura [SPELL_AURA_MOD_POWER_REGEN] = true;
    isNonTriggerAura [SPELL_AURA_REDUCE_PUSHBACK] = true;

    isAlwaysTriggeredAura [SPELL_AURA_OVERRIDE_CLASS_SCRIPTS] = true;
    isAlwaysTriggeredAura [SPELL_AURA_MOD_FEAR] = true;
    isAlwaysTriggeredAura [SPELL_AURA_MOD_FEAR_2] = true;
    isAlwaysTriggeredAura [SPELL_AURA_MOD_ROOT] = true;
    isAlwaysTriggeredAura [SPELL_AURA_MOD_STUN] = true;
    isAlwaysTriggeredAura [SPELL_AURA_TRANSFORM] = true;
    isAlwaysTriggeredAura [SPELL_AURA_SPELL_MAGNET] = true;
    isAlwaysTriggeredAura [SPELL_AURA_SCHOOL_ABSORB] = true;
    isAlwaysTriggeredAura [SPELL_AURA_MOD_STEALTH] = true;

    return true;
}

uint32 createProcExtendMask(SpellNonMeleeDamage* damageInfo, SpellMissInfo missCondition)
{
    uint32 procEx = PROC_EX_NONE;
    // Check victim state
    if (missCondition != SPELL_MISS_NONE)
        switch (missCondition)
    {
            case SPELL_MISS_MISS:    procEx |= PROC_EX_MISS;   break;
            case SPELL_MISS_RESIST:  procEx |= PROC_EX_RESIST; break;
            case SPELL_MISS_DODGE:   procEx |= PROC_EX_DODGE;  break;
            case SPELL_MISS_PARRY:   procEx |= PROC_EX_PARRY;  break;
            case SPELL_MISS_BLOCK:   procEx |= PROC_EX_BLOCK;  break;
            case SPELL_MISS_EVADE:   procEx |= PROC_EX_EVADE;  break;
            case SPELL_MISS_IMMUNE:  procEx |= PROC_EX_IMMUNE; break;
            case SPELL_MISS_IMMUNE2: procEx |= PROC_EX_IMMUNE; break;
            case SPELL_MISS_DEFLECT: procEx |= PROC_EX_DEFLECT; break;
            case SPELL_MISS_ABSORB:  procEx |= PROC_EX_ABSORB; break;
            case SPELL_MISS_REFLECT: procEx |= PROC_EX_REFLECT; break;
            default:
                break;
    }
    else
    {
        // On block
        if (damageInfo->blocked)
            procEx |= PROC_EX_BLOCK;
        // On absorb
        if (damageInfo->absorb)
            procEx |= PROC_EX_ABSORB;
        // On crit
        if (damageInfo->HitInfo & SPELL_HIT_TYPE_CRIT)
            procEx |= PROC_EX_CRITICAL_HIT;
        else
            procEx |= PROC_EX_NORMAL_HIT;
    }
    return procEx;
}

void Unit::ProcDamageAndSpellFor(bool isVictim, Unit* target, uint32 procFlag, uint32 procExtra, WeaponAttackType attType, SpellInfo const* procSpell, uint32 damage, SpellInfo const* procAura, ProcTriggerContext const& context)
{
    // Player is loaded now - do not allow passive spell casts to proc
    if (GetTypeId() == TYPEID_PLAYER && ToPlayer()->GetSession()->PlayerLoading())
        return;
    // For melee/ranged based attack need update skills and set some Aura states if victim present
    if (procFlag & MELEE_BASED_TRIGGER_MASK && target)
    {
        // If exist crit/parry/dodge/block need update aura state (for victim and attacker)
        if (procExtra & (PROC_EX_CRITICAL_HIT | PROC_EX_PARRY | PROC_EX_DODGE | PROC_EX_BLOCK))
        {
            // for victim
            if (isVictim)
            {
                // if victim and dodge attack
                if (procExtra & PROC_EX_DODGE)
                {
                    // Update AURA_STATE on dodge
                    if (GetClass() != CLASS_ROGUE) // skip Rogue Riposte
                    {
                        ModifyAuraState(AURA_STATE_DEFENSE, true);
                        StartReactiveTimer(REACTIVE_DEFENSE);
                    }
                }
                // if victim and parry attack
                if (procExtra & PROC_EX_PARRY)
                {
                    // For Hunters only Counterattack (skip Mongoose bite)
                    if (GetClass() == CLASS_HUNTER)
                    {
                        ModifyAuraState(AURA_STATE_HUNTER_PARRY, true);
                        StartReactiveTimer(REACTIVE_HUNTER_PARRY);
                    }
                    else
                    {
                        ModifyAuraState(AURA_STATE_DEFENSE, true);
                        StartReactiveTimer(REACTIVE_DEFENSE);
                    }
                }
                // if and victim block attack
                if (procExtra & PROC_EX_BLOCK)
                {
                    ModifyAuraState(AURA_STATE_DEFENSE, true);
                    StartReactiveTimer(REACTIVE_DEFENSE);
                }
            }
            else // For attacker
            {
                // Overpower on victim dodge
                if (procExtra & PROC_EX_DODGE && GetTypeId() == TYPEID_PLAYER && GetClass() == CLASS_WARRIOR)
                {
                    ToPlayer()->AddComboPoints(target, 1);
                    StartReactiveTimer(REACTIVE_OVERPOWER);
                }
            }
        }
    }

    Unit* actor = isVictim ? target : this;
    Unit* actionTarget = !isVictim ? target : this;

    DamageInfo damageInfo = DamageInfo(actor, actionTarget, damage, procSpell, procSpell ? SpellSchoolMask(procSpell->SchoolMask) : SPELL_SCHOOL_MASK_NORMAL, SPELL_DIRECT_DAMAGE);
    HealInfo healInfo = HealInfo(damage, context.EffectiveHeal, context.Overheal);
    ProcEventInfo eventInfo = ProcEventInfo(actor, actionTarget, target, procFlag, 0, 0, procExtra, &damageInfo, &healInfo, &context);

    ProcTriggeredList procTriggered;

    AuraApplicationMap& procAuras = m_procAuras[uint32(context.Phase)];

    // Fill procTriggered list
    for (auto&& itr : procAuras)
    {
        // Do not allow auras to proc from effect triggered by itself
        if (procAura && procAura->Id == itr.first)
            continue;
        ProcTriggeredData triggerData{ itr.second, eventInfo };
        // Defensive procs are active on absorbs (so absorption effects are not a hindrance)
        bool active = damage || (procExtra & PROC_EX_BLOCK && isVictim) || context.Phase == ProcPhase::Cast;
        if (isVictim)
            procExtra &= ~PROC_EX_INTERNAL_REQ_FAMILY;

        SpellInfo const* spellProto = itr.second->GetBase()->GetSpellInfo();

        // only auras that has triggered spell should proc from fully absorbed damage
        if (procExtra & PROC_EX_ABSORB && isVictim)
            if (damage || spellProto->Effects [EFFECT_0].TriggerSpell || spellProto->Effects [EFFECT_1].TriggerSpell || spellProto->Effects [EFFECT_2].TriggerSpell)
                active = true;

        if (procExtra & PROC_EX_REFLECT)
            if (spellProto->AttributesCu & SPELL_ATTR0_CU_SPELL_REFLECT)
                active = true;

        if (!active && procExtra & (PROC_EX_INTERNAL_ACTIVE | PROC_EX_ABSORB))
            active = true;

        if (!IsTriggeredAtSpellProcEvent(target, triggerData, procSpell, procFlag, procExtra, attType, isVictim, active))
            continue;

        // do checks using conditions table
        if (!sConditionMgr->IsObjectMeetingNotGroupedConditions(CONDITION_SOURCE_TYPE_SPELL_PROC, spellProto->Id, eventInfo.GetActor(), eventInfo.GetActionTarget()))
            continue;

        // Triggered spells not triggering additional spells
        bool triggered = !(spellProto->AttributesEx3 & SPELL_ATTR3_CAN_PROC_WITH_TRIGGERED) ?
            (procExtra & PROC_EX_INTERNAL_TRIGGERED && !(procFlag & PROC_FLAG_DONE_TRAP_ACTIVATION)) : false;

        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (itr.second->HasEffect(i))
            {
                AuraEffect* aurEff = itr.second->GetBase()->GetEffect(i);
                // Skip this auras
                if (isNonTriggerAura [aurEff->GetAuraType()])
                    continue;
                // If not trigger by default and spellProcEvent == NULL - skip
                if (!isTriggerAura [aurEff->GetAuraType()] && triggerData.spellProcEvent == NULL)
                    continue;
                // Some spells must always trigger
                if (!triggered || isAlwaysTriggeredAura [aurEff->GetAuraType()])
                    triggerData.effMask |= 1 << i;
            }
        }
        if (triggerData.effMask)
            procTriggered.push_front(triggerData);
    }

    // Nothing found
    if (procTriggered.empty())
        return;

    // Note: must SetCantProc(false) before return
    if (procExtra & (PROC_EX_INTERNAL_TRIGGERED | PROC_EX_INTERNAL_CANT_PROC))
        SetCantProc(true);

    // Handle effects proceed this time
    for (ProcTriggeredList::const_iterator i = procTriggered.begin(); i != procTriggered.end(); ++i)
    {
        // look for aura in auras list, it may be removed while proc event processing
        if (i->aura->IsRemoved())
            continue;

        bool useCharges = i->aura->IsUsingCharges();
        // no more charges to use, prevent proc
        if (useCharges && !i->aura->GetCharges())
            continue;

        bool takeCharges = false;
        SpellInfo const* spellInfo = i->aura->GetSpellInfo();
        uint32 Id = i->aura->GetId();

        AuraApplication* aurApp = i->aura->GetApplicationOfTarget(GetGUID());

        bool prepare = i->aura->CallScriptPrepareProcHandlers(aurApp, eventInfo);

        // For players set spell cooldown if need
        uint32 cooldown = 0;
        if (prepare)
        {
            if (i->spellProcEvent && i->spellProcEvent->cooldown)
                cooldown = i->spellProcEvent->cooldown;
            else
                cooldown = spellInfo->ProcCooldown;
        }

        if (spellInfo->ProcsPerMinute)
        {
            bool rppmAllAttacks = false;
            Item* item = nullptr;
            Player const* player = isVictim && target ? target->ToPlayer() : ToPlayer();
            if (i->aura->GetCastItemGUID() && player)
            {
                item = player->GetItemByGuid(i->aura->GetCastItemGUID());
                if (item && (item == player->GetWeaponForAttack(BASE_ATTACK) || item == player->GetWeaponForAttack(OFF_ATTACK)))
                {
                }
                else
                    rppmAllAttacks = true;
            }
            if (rppmAllAttacks)
            {
                GetSpellHistory()->RppmPorcSuccess(Id, BASE_ATTACK);
                GetSpellHistory()->RppmPorcSuccess(Id, OFF_ATTACK);
                GetSpellHistory()->RppmPorcSuccess(Id, RANGED_ATTACK);
            }
            else
                GetSpellHistory()->RppmPorcSuccess(Id, attType);
        }
        if (cooldown)
            GetSpellHistory()->StartProcCooldown(Id, Milliseconds(cooldown));

        // Note: must SetCantProc(false) before return
        if (spellInfo->AttributesEx3 & SPELL_ATTR3_DISABLE_PROC)
            SetCantProc(true);

        bool handled = i->aura->CallScriptProcHandlers(aurApp, eventInfo);

        // "handled" is needed as long as proc can be handled in multiple places
        if (!handled && HandleAuraProc(target, damage, i->aura, procSpell, procFlag, procExtra, cooldown, &handled, context))
        {
            TC_LOG_DEBUG("spells", "ProcDamageAndSpell: casting spell %u (triggered with value by %s aura of spell %u)", spellInfo->Id, (isVictim ? "a victim's" : "an attacker's"), Id);
            takeCharges = true;
        }

        if (!handled)
        {
            for (uint8 effIndex = 0; effIndex < MAX_SPELL_EFFECTS; ++effIndex)
            {
                if (!(i->effMask & (1 << effIndex)))
                    continue;

                AuraEffect* triggeredByAura = i->aura->GetEffect(effIndex);
                ASSERT(triggeredByAura);

                bool prevented = i->aura->CallScriptEffectProcHandlers(triggeredByAura, aurApp, eventInfo);
                if (prevented)
                {
                    takeCharges = true;
                    continue;
                }

                switch (triggeredByAura->GetAuraType())
                {
                    case SPELL_AURA_PROC_TRIGGER_SPELL:
                    case SPELL_AURA_PROC_TRIGGER_SPELL_2:
                    {
                        TC_LOG_DEBUG("spells", "ProcDamageAndSpell: casting spell %u (triggered by %s aura of spell %u)", spellInfo->Id, (isVictim ? "a victim's" : "an attacker's"), triggeredByAura->GetId());
                        // Don`t drop charge or add cooldown for not started trigger
                        if (HandleProcTriggerSpell(target, damage, triggeredByAura, procSpell, procFlag, procExtra, cooldown, context))
                            takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_PROC_TRIGGER_DAMAGE:
                    {
                        // target has to be valid
                        if (!eventInfo.GetProcTarget())
                            break;

                        triggeredByAura->HandleProcTriggerDamageAuraProc(aurApp, eventInfo); // this function is part of the new proc system
                        takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_MANA_SHIELD:
                    case SPELL_AURA_DUMMY:
                    {
                        TC_LOG_DEBUG("spells", "ProcDamageAndSpell: casting spell id %u (triggered by %s dummy aura of spell %u)", spellInfo->Id, (isVictim ? "a victim's" : "an attacker's"), triggeredByAura->GetId());
                        if (HandleDummyAuraProc(target, damage, triggeredByAura, procSpell, procFlag, procExtra, cooldown, context))
                            takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_PROC_ON_POWER_AMOUNT:
                    {
                        TC_LOG_DEBUG("spells", "ProcDamageAndSpell: casting spell id %u (triggered by %s aura of spell %u)", spellInfo->Id, (isVictim ? "a victim's" : "an attacker's"), triggeredByAura->GetId());
                        if (HandleAuraProcOnPowerAmount(target, damage, triggeredByAura, procSpell, procFlag, procExtra, cooldown, context))
                            takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_OBS_MOD_POWER:
                    case SPELL_AURA_MOD_SPELL_CRIT_CHANCE:
                    case SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN:
                    case SPELL_AURA_MOD_POWER_REGEN_PERCENT:
                    case SPELL_AURA_MOD_MELEE_HASTE:
                    case SPELL_AURA_MOD_MELEE_HASTE_3:
                        TC_LOG_DEBUG("spells", "ProcDamageAndSpell: casting spell id %u (triggered by %s aura of spell %u)", spellInfo->Id, isVictim ? "a victim's" : "an attacker's", triggeredByAura->GetId());
                        takeCharges = true;
                        break;
                    case SPELL_AURA_OVERRIDE_CLASS_SCRIPTS:
                    {
                        TC_LOG_DEBUG("spells", "ProcDamageAndSpell: casting spell id %u (triggered by %s aura of spell %u)", spellInfo->Id, (isVictim ? "a victim's" : "an attacker's"), triggeredByAura->GetId());
                        if (HandleOverrideClassScriptAuraProc(target, damage, triggeredByAura, procSpell, cooldown, context))
                            takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_RAID_PROC_FROM_CHARGE_WITH_VALUE:
                    {
                        TC_LOG_DEBUG("spells", "ProcDamageAndSpell: casting mending (triggered by %s dummy aura of spell %u)",
                                     (isVictim ? "a victim's" : "an attacker's"), triggeredByAura->GetId());

                        HandleAuraRaidProcFromChargeWithValue(triggeredByAura);
                        takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_RAID_PROC_FROM_CHARGE:
                    {
                        TC_LOG_DEBUG("spells", "ProcDamageAndSpell: casting mending (triggered by %s dummy aura of spell %u)",
                                     (isVictim ? "a victim's" : "an attacker's"), triggeredByAura->GetId());

                        HandleAuraRaidProcFromCharge(triggeredByAura);
                        takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE:
                    {
                        TC_LOG_DEBUG("spells", "ProcDamageAndSpell: casting spell %u (triggered with value by %s aura of spell %u)", spellInfo->Id, (isVictim ? "a victim's" : "an attacker's"), triggeredByAura->GetId());

                        if (HandleProcTriggerSpell(target, damage, triggeredByAura, procSpell, procFlag, procExtra, cooldown, context))
                            takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK:
                        // Skip melee hits or instant cast spells
                        if (procSpell && procSpell->CalcCastTime(GetLevel()) > 0)
                            takeCharges = true;
                        break;
                    case SPELL_AURA_REFLECT_SPELLS:
                        if (procExtra & PROC_EX_REFLECT)
                            takeCharges = true;
                        break;
                    case SPELL_AURA_REFLECT_SPELLS_SCHOOL:
                        // Skip Melee hits and spells ws wrong school
                        if (procExtra & PROC_EX_REFLECT && procSpell && (triggeredByAura->GetMiscValue() & procSpell->SchoolMask))         // School check
                            takeCharges = true;
                        break;
                    case SPELL_AURA_SPELL_MAGNET:
                        // Skip Melee hits and targets with magnet aura
                        if (procSpell && (triggeredByAura->GetBase()->GetUnitOwner()->ToUnit() == ToUnit()))         // Magnet
                            takeCharges = true;
                        break;
                    case SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT:
                    case SPELL_AURA_MOD_POWER_COST_SCHOOL:
                        // Skip melee hits and spells ws wrong school or zero cost
                        if (context.TriggeredBySpell &&
                            !context.TriggeredBySpell->GetPowerCost() &&                        // Cost check
                            (triggeredByAura->GetMiscValue() & procSpell->SchoolMask))          // School check
                            takeCharges = true;
                        break;
                    case SPELL_AURA_MECHANIC_IMMUNITY:
                        // Compare mechanic
                        if (procSpell && procSpell->Mechanic == uint32(triggeredByAura->GetMiscValue()))
                            takeCharges = true;
                        break;
                    case SPELL_AURA_MOD_MECHANIC_RESISTANCE:
                        // Compare mechanic
                        if (procSpell && procSpell->Mechanic == uint32(triggeredByAura->GetMiscValue()))
                            takeCharges = true;
                        break;
                    case SPELL_AURA_MOD_DAMAGE_FROM_CASTER:
                        // Compare casters
                        if (triggeredByAura->GetCasterGUID() == target->GetGUID())
                            takeCharges = true;
                        break;
                        // CC Auras which use their amount amount to drop
                        // Are there any more auras which need this?
                    case SPELL_AURA_MOD_CONFUSE:
                    case SPELL_AURA_MOD_FEAR:
                    case SPELL_AURA_MOD_FEAR_2:
                    case SPELL_AURA_MOD_STUN:
                    case SPELL_AURA_MOD_ROOT:
                    case SPELL_AURA_TRANSFORM:
                    {
                        // Ascendance (114052) is SPELL_AURA_TRANSFORM and has DONE proc flags
                        if (!(procFlag & TAKEN_HIT_PROC_FLAG_MASK))
                            break;

                        // Okay, exclude healing (I don't remember concrete auras)
                        if (procFlag & (PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_POS | PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS))
                            break;

                        // For the fuck sake, WHY periodic effects call ProcDamageAndSpell only with *_PRERIODIC flag...
                        if (procExtra & PROC_EX_INTERNAL_HOT)
                            break;

                        // chargeable mods are breaking on hit
                        if (useCharges)
                            takeCharges = true;
                        else
                        {
                            // Spell own direct damage at apply wont break the CC
                            if (procSpell && (procSpell->Id == triggeredByAura->GetId()))
                            {
                                Aura* aura = triggeredByAura->GetBase();
                                // called from spellcast, should not have ticked yet
                                if (aura->GetDuration() == aura->GetMaxDuration())
                                    break;
                            }
                            int32 damageLeft = triggeredByAura->GetAmount();
                            if (damageLeft < 0)
                                break;
                            // No damage left
                            int32 dmg = damage;
                            if (context.SpellDamage)
                                dmg += context.SpellDamage->absorb;
                            else if (context.MeleeDamage)
                                dmg += context.MeleeDamage->absorb;
                            if (damageLeft < dmg)
                                i->aura->Remove();
                            else
                                triggeredByAura->SetAmount(damageLeft - damage);
                        }
                        break;
                    }
                    //case SPELL_AURA_ADD_FLAT_MODIFIER:
                    //case SPELL_AURA_ADD_PCT_MODIFIER:
                    // HandleSpellModAuraProc
                    //break;
                    default:
                        // nothing do, just charges counter
                        takeCharges = true;
                        break;
                } // switch (triggeredByAura->GetAuraType())
                i->aura->CallScriptAfterEffectProcHandlers(triggeredByAura, aurApp, eventInfo);
            } // for (uint8 effIndex = 0; effIndex < MAX_SPELL_EFFECTS; ++effIndex)
        } // if (!handled)

        // Remove charge (aura can be removed by triggers)
        if (prepare && useCharges && takeCharges)
            i->aura->DropCharge();

        i->aura->CallScriptAfterProcHandlers(aurApp, eventInfo);

        if (spellInfo->AttributesEx3 & SPELL_ATTR3_DISABLE_PROC)
            SetCantProc(false);
    }

    // Cleanup proc requirements
    if (procExtra & (PROC_EX_INTERNAL_TRIGGERED | PROC_EX_INTERNAL_CANT_PROC))
        SetCantProc(false);
}

SpellSchoolMask Unit::GetMeleeDamageSchoolMask() const
{
    return SPELL_SCHOOL_MASK_NORMAL;
}

ObjectGuid Unit::GetCharmerOrOwnerGUID() const
{
    return GetCharmerGUID() ? GetCharmerGUID() : GetOwnerGUID();
}

ObjectGuid Unit::GetCharmerOrOwnerOrOwnGUID() const
{
    if (ObjectGuid guid = GetCharmerOrOwnerGUID())
        return guid;
    return GetGUID();
}

Player* Unit::GetSpellModOwner() const
{
    if (Player* player = const_cast<Unit*>(this)->ToPlayer())
        return player;

    if (ToCreature()->IsGuardian() || ToCreature()->IsTotem())
    {
        if (Unit* owner = GetOwner())
            if (Player* player = owner->ToPlayer())
                return player;
    }
    return NULL;
}

///----------Pet responses methods-----------------
void Unit::SendPetActionFeedback(uint8 msg)
{
    Unit* owner = GetOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    bool hasSpell = false;

    WorldPacket data(SMSG_PET_ACTION_FEEDBACK, 1);
    data.WriteBit(!hasSpell);
    data << uint8(msg);
    if (hasSpell)
        data << uint32(0); // spellId

    owner->ToPlayer()->GetSession()->SendPacket(&data);
}

void Unit::SendPetTalk(uint32 pettalk)
{
    Unit* owner = GetOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    ObjectGuid guid = owner->GetGUID();
    WorldPacket data(SMSG_PET_ACTION_SOUND, 8 + 4);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[4]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[1]);
    data << uint32(pettalk);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[0]);
    owner->ToPlayer()->GetSession()->SendPacket(&data);
}

void Unit::SendPetAIReaction(ObjectGuid UnitGUID)
{
    Unit* owner = GetOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    WorldPacket data(SMSG_AI_REACTION, 8 + 4);

    data.WriteBit(UnitGUID [5]);
    data.WriteBit(UnitGUID [7]);
    data.WriteBit(UnitGUID [0]);
    data.WriteBit(UnitGUID [4]);
    data.WriteBit(UnitGUID [6]);
    data.WriteBit(UnitGUID [2]);
    data.WriteBit(UnitGUID [3]);
    data.WriteBit(UnitGUID [1]);

    data.WriteByteSeq(UnitGUID [4]);
    data.WriteByteSeq(UnitGUID [6]);
    data.WriteByteSeq(UnitGUID [5]);
    data << uint32(AI_REACTION_HOSTILE);
    data.WriteByteSeq(UnitGUID [7]);
    data.WriteByteSeq(UnitGUID [1]);
    data.WriteByteSeq(UnitGUID [2]);
    data.WriteByteSeq(UnitGUID [0]);
    data.WriteByteSeq(UnitGUID [3]);

    owner->ToPlayer()->GetSession()->SendPacket(&data);
}

///----------End of Pet responses methods----------

void Unit::StopMoving()
{
    ClearUnitState(UNIT_STATE_MOVING);

    // not need send any packets if not in world or not moving
    if (!IsInWorld() || movespline->Finalized())
        return;

    // Update position now since Stop does not start a new movement that can be updated later
    UpdateSplinePosition();
    Movement::MoveSplineInit init(this);
    init.Stop();
    // Update spline now, otherwise we'd be stuck with this spline active until next update, which might interfere with spells, facing etc
    UpdateSplineMovement(1);
}

void Unit::PauseMovement(uint32 timer/* = 0*/, uint8 slot/* = 0*/, bool forced/* = true*/)
{
    if (IsInvalidMovementSlot(slot))
        return;

    // hack
    // if (MovementGenerator* movementGenerator = GetMotionMaster()->GetCurrentMovementGenerator(MovementSlot(slot)))
    //     movementGenerator->Pause(timer);

    // if (forced && GetMotionMaster()->GetCurrentSlot() == MovementSlot(slot))
    //     StopMoving();
}

void Unit::ResumeMovement(uint32 timer/* = 0*/, uint8 slot/* = 0*/)
{
    if (IsInvalidMovementSlot(slot))
        return;

    // hack
    // if (MovementGenerator* movementGenerator = GetMotionMaster()->GetCurrentMovementGenerator(MovementSlot(slot)))
    //     movementGenerator->Resume(timer);
}

void Unit::SendMovementFlagUpdate(bool self /* = false */)
{
    WorldPacket data(SMSG_PLAYER_MOVE, 40);
    WriteMovementInfo(data);
    SendMessageToSet(&data, self);
}

bool Unit::IsSitState() const
{
    uint8 s = GetStandState();
    return s == UNIT_STAND_STATE_SIT_CHAIR || s == UNIT_STAND_STATE_SIT_LOW_CHAIR || s == UNIT_STAND_STATE_SIT_MEDIUM_CHAIR ||
        s == UNIT_STAND_STATE_SIT_HIGH_CHAIR || s == UNIT_STAND_STATE_SIT;
}

bool Unit::IsStandState() const
{
    uint8 s = GetStandState();
    return !IsSitState() && s != UNIT_STAND_STATE_SLEEP && s != UNIT_STAND_STATE_KNEEL;
}

void Unit::SetStandState(UnitStandStateType state)
{
    SetByteValue(UNIT_FIELD_ANIM_TIER, UNIT_BYTES_1_OFFSET_STAND_STATE, state);

    if (state == UNIT_STAND_STATE_STAND)
       RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_NOT_SEATED);

    if (GetTypeId() == TYPEID_PLAYER)
    {
        WorldPacket data(SMSG_STANDSTATE_UPDATE, 1);
        data << (uint8)state;
        ToPlayer()->GetSession()->SendPacket(&data);
    }
}

bool Unit::IsPolymorphed() const
{
    uint32 transformId = getTransForm();
    if (!transformId)
        return false;

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(transformId);
    if (!spellInfo)
        return false;

    return spellInfo->GetSpellSpecific() == SPELL_SPECIFIC_MAGE_POLYMORPH;
}

void Unit::SetAnimTier(AnimTier tier)
{
    if (!IsCreature())
        return;

    SetByteValue(UNIT_FIELD_ANIM_TIER, UNIT_BYTES_1_OFFSET_ANIM_TIER, static_cast<uint8>(tier));
}

void Unit::SetDisplayId(uint32 modelId)
{
    SetUInt32Value(UNIT_FIELD_DISPLAY_ID, modelId);
}

void Unit::RestoreDisplayId()
{
    for (auto&& itr : GetAuraEffectsByType(SPELL_AURA_CLONE_CASTER))
    {
        if (Unit* caster = itr->GetCaster())
        {
            SetDisplayId(caster->GetDisplayId());
            return;
        }
    }

    AuraEffect* handledAura = NULL;
    // try to receive model from transform auras
    Unit::AuraEffectList const& transforms = GetAuraEffectsByType(SPELL_AURA_TRANSFORM);
    if (!transforms.empty())
    {
        // iterate over already applied transform auras - from newest to oldest
        for (Unit::AuraEffectList::const_reverse_iterator i = transforms.rbegin(); i != transforms.rend(); ++i)
        {
            if (AuraApplication const* aurApp = (*i)->GetBase()->GetApplicationOfTarget(GetGUID()))
            {
                if (!handledAura)
                    handledAura = (*i);
                // prefer negative auras
                if (!aurApp->IsPositive())
                {
                    handledAura = (*i);
                    break;
                }
            }
        }
    }
    // transform aura was found
    if (handledAura)
        handledAura->HandleEffect(this, AURA_EFFECT_HANDLE_SEND_FOR_CLIENT, true);
    // we've found shapeshift
    else if (uint32 modelId = GetModelForForm(GetShapeshiftForm()))
        SetDisplayId(modelId);
    // no auras found - set modelid to default
    else
        SetDisplayId(GetNativeDisplayId());
}

void Unit::ClearComboPointHolders()
{
    while (!m_comboPointHolders.empty())
    {
        ObjectGuid guid = *m_comboPointHolders.begin();

        Player* player = ObjectAccessor::FindPlayer(guid);
        if (player && player->GetComboTarget() == GetGUID())         // recheck for safe
            player->ClearComboPoints();                        // remove also guid from m_ComboPointHolders;
        else
            m_comboPointHolders.erase(guid);             // or remove manually
    }
    m_comboPointResetTimer = 0;
}

void Unit::ClearAllReactives()
{
    for (uint8 i = 0; i < MAX_REACTIVE; ++i)
        m_reactiveTimer [i] = 0;

    if (HasAuraState(AURA_STATE_DEFENSE))
        ModifyAuraState(AURA_STATE_DEFENSE, false);
    if (GetClass() == CLASS_HUNTER && HasAuraState(AURA_STATE_HUNTER_PARRY))
        ModifyAuraState(AURA_STATE_HUNTER_PARRY, false);
    if (GetClass() == CLASS_WARRIOR && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->ClearComboPoints();
}

void Unit::UpdateReactives(uint32 p_time)
{
    for (uint8 i = 0; i < MAX_REACTIVE; ++i)
    {
        ReactiveType reactive = ReactiveType(i);

        if (!m_reactiveTimer [reactive])
            continue;

        if (m_reactiveTimer [reactive] <= p_time)
        {
            m_reactiveTimer [reactive] = 0;

            switch (reactive)
            {
                case REACTIVE_DEFENSE:
                    if (HasAuraState(AURA_STATE_DEFENSE))
                        ModifyAuraState(AURA_STATE_DEFENSE, false);
                    break;
                case REACTIVE_HUNTER_PARRY:
                    if (GetClass() == CLASS_HUNTER && HasAuraState(AURA_STATE_HUNTER_PARRY))
                        ModifyAuraState(AURA_STATE_HUNTER_PARRY, false);
                    break;
                case REACTIVE_OVERPOWER:
                    if (GetClass() == CLASS_WARRIOR && GetTypeId() == TYPEID_PLAYER)
                        ToPlayer()->ClearComboPoints();
                    break;
            }
        }
        else
        {
            m_reactiveTimer [reactive] -= p_time;
        }
    }
}

Unit* Unit::SelectNearbyTarget(Unit* exclude, float dist) const
{
    std::list<Unit*> targets;
    Trinity::AnyUnfriendlyUnitInObjectRangeCheck u_check(this, this, dist);
    Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(this, targets, u_check);
    VisitNearbyObject(dist, searcher);

    // remove current target
    if (GetVictim())
        targets.remove(GetVictim());

    if (exclude)
        targets.remove(exclude);

    // remove not LoS targets
    for (std::list<Unit*>::iterator tIter = targets.begin(); tIter != targets.end();)
    {
        if (!IsWithinLOSInMap(*tIter) || (*tIter)->IsTotem() || (*tIter)->IsSpiritService() || (*tIter)->GetCreatureType() == CREATURE_TYPE_CRITTER)
            targets.erase(tIter++);
        else
            ++tIter;
    }

    // no appropriate targets
    if (targets.empty())
        return NULL;

    // select random
    return Trinity::Containers::SelectRandomContainerElement(targets);
}

Unit* Unit::SelectNearbyAlly(Unit* exclude, float dist) const
{
    std::list<Unit*> targets;
    Trinity::AnyFriendlyUnitInObjectRangeCheck u_check(this, this, dist);
    Trinity::UnitListSearcher<Trinity::AnyFriendlyUnitInObjectRangeCheck> searcher(this, targets, u_check);
    VisitNearbyObject(dist, searcher);

    if (exclude)
        targets.remove(exclude);

    // remove not LoS targets
    for (std::list<Unit*>::iterator tIter = targets.begin(); tIter != targets.end();)
    {
        if (!IsWithinLOSInMap(*tIter) || (*tIter)->IsTotem() || (*tIter)->IsSpiritService() || (*tIter)->GetCreatureType() == CREATURE_TYPE_CRITTER)
            targets.erase(tIter++);
        else
            ++tIter;
    }

    // no appropriate targets
    if (targets.empty())
        return NULL;

    // select random
    return Trinity::Containers::SelectRandomContainerElement(targets);
}

// select nearest hostile unit within the given distance (regardless of threat list).
Unit* Unit::SelectNearestTarget(float dist) const
{
    CellCoord p(Trinity::ComputeCellCoord(GetPositionX(), GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Unit* target = NULL;

    {
        if (dist == 0.0f)
            dist = MAX_VISIBILITY_DISTANCE;

        Trinity::NearestHostileUnitCheck u_check(this, dist);
        Trinity::UnitLastSearcher<Trinity::NearestHostileUnitCheck> searcher(this, target, u_check);
        VisitNearbyObject(dist, searcher);
    }

    return target;
}

uint32 Unit::GetCastingTimeForBonus(SpellInfo const* spellProto, DamageEffectType damagetype, uint32 CastingTime) const
{
    // Not apply this to creature casted spells with casttime == 0
    if (CastingTime == 0 && GetTypeId() == TYPEID_UNIT && !ToCreature()->IsPet())
        return 3500;

    if (CastingTime > 7000) CastingTime = 7000;
    if (CastingTime < 1500) CastingTime = 1500;

    if (damagetype == DOT && !spellProto->IsChanneled())
        CastingTime = 3500;

    int32 overTime = 0;
    uint8 effects = 0;
    bool DirectDamage = false;
    bool AreaEffect = false;

    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; i++)
    {
        switch (spellProto->Effects [i].Effect)
        {
            case SPELL_EFFECT_SCHOOL_DAMAGE:
            case SPELL_EFFECT_POWER_DRAIN:
            case SPELL_EFFECT_HEALTH_LEECH:
            case SPELL_EFFECT_ENVIRONMENTAL_DAMAGE:
            case SPELL_EFFECT_POWER_BURN:
            case SPELL_EFFECT_HEAL:
                DirectDamage = true;
                break;
            case SPELL_EFFECT_APPLY_AURA:
                switch (spellProto->Effects [i].ApplyAuraName)
                {
                    case SPELL_AURA_PERIODIC_DAMAGE:
                    case SPELL_AURA_PERIODIC_HEAL:
                    case SPELL_AURA_PERIODIC_LEECH:
                        if (spellProto->GetDuration())
                            overTime = spellProto->GetDuration();
                        break;
                    default:
                        // -5% per additional effect
                        ++effects;
                        break;
                }
            default:
                break;
        }

        if (spellProto->Effects [i].IsTargetingArea())
            AreaEffect = true;
    }

    // Combined Spells with Both Over Time and Direct Damage
    if (overTime > 0 && CastingTime > 0 && DirectDamage)
    {
        // mainly for DoTs which are 3500 here otherwise
        uint32 OriginalCastTime = spellProto->CalcCastTime(GetLevel());
        if (OriginalCastTime > 7000) OriginalCastTime = 7000;
        if (OriginalCastTime < 1500) OriginalCastTime = 1500;
        // Portion to Over Time
        float PtOT = (overTime / 15000.0f) / ((overTime / 15000.0f) + (OriginalCastTime / 3500.0f));

        if (damagetype == DOT)
            CastingTime = uint32(CastingTime * PtOT);
        else if (PtOT < 1.0f)
            CastingTime = uint32(CastingTime * (1 - PtOT));
        else
            CastingTime = 0;
    }

    // Area Effect Spells receive only half of bonus
    if (AreaEffect)
        CastingTime /= 2;

    // 50% for damage and healing spells for leech spells from damage bonus and 0% from healing
    for (uint8 j = 0; j < MAX_SPELL_EFFECTS; ++j)
    {
        if (spellProto->Effects [j].Effect == SPELL_EFFECT_HEALTH_LEECH ||
            (spellProto->Effects [j].Effect == SPELL_EFFECT_APPLY_AURA && spellProto->Effects [j].ApplyAuraName == SPELL_AURA_PERIODIC_LEECH))
        {
            CastingTime /= 2;
            break;
        }
    }

    // -5% of total per any additional effect
    for (uint8 i = 0; i < effects; ++i)
        CastingTime *= 0.95f;

    return CastingTime;
}

void Unit::UpdateAuraForGroup(uint8 slot)
{
    if (slot >= MAX_AURAS)                        // slot not found, return
        return;
    if (Player* player = ToPlayer())
    {
        if (player->GetGroup())
        {
            player->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_AURAS);
            player->SetAuraUpdateMaskForRaid(slot);
        }
    }
    else if (GetTypeId() == TYPEID_UNIT && ToCreature()->IsPet())
    {
        Pet* pet = ((Pet*)this);
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
            {
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_AURAS);
                pet->SetAuraUpdateMaskForRaid(slot);
            }
        }
    }
}

void Unit::SetCantProc(bool apply)
{
    if (apply)
        ++m_procDeep;
    else
    {
        ASSERT(m_procDeep);
        --m_procDeep;
    }
}

float Unit::GetAPMultiplier(WeaponAttackType attType, bool normalized)
{
    if (!normalized || GetTypeId() != TYPEID_PLAYER)
        return float(GetAttackTime(attType)) / 1000.0f;

    Item* Weapon = ToPlayer()->GetWeaponForAttack(attType, true);
    if (!Weapon)
        return 2.4f;                                         // fist attack

    switch (Weapon->GetTemplate()->InventoryType)
    {
        case INVTYPE_2HWEAPON:
            return 3.3f;
        case INVTYPE_RANGED:
        case INVTYPE_RANGEDRIGHT:
        case INVTYPE_THROWN:
            return 2.8f;
        case INVTYPE_WEAPON:
        case INVTYPE_WEAPONMAINHAND:
        case INVTYPE_WEAPONOFFHAND:
        default:
            return Weapon->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER ? 1.7f : 2.4f;
    }
}

void Unit::SetContestedPvP(Player* attackedPlayer)
{
    Player* player = GetCharmerOrOwnerPlayerOrPlayerItself();

    if (!player || (attackedPlayer && (attackedPlayer == player || (player->duel && player->duel->opponent == attackedPlayer))))
        return;

    player->SetContestedPvPTimer(30000);
    if (!player->HasUnitState(UNIT_STATE_ATTACK_PLAYER))
    {
        player->AddUnitState(UNIT_STATE_ATTACK_PLAYER);
        player->SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP);
        // call MoveInLineOfSight for nearby contested guards
        UpdateObjectVisibility();
    }
    if (!HasUnitState(UNIT_STATE_ATTACK_PLAYER))
    {
        AddUnitState(UNIT_STATE_ATTACK_PLAYER);
        // call MoveInLineOfSight for nearby contested guards
        UpdateObjectVisibility();
    }
}

void Unit::AddPetAura(PetAura const* petSpell)
{
    if (GetTypeId() != TYPEID_PLAYER)
        return;

    m_petAuras.insert(petSpell);
    if (Pet* pet = ToPlayer()->GetPet())
        pet->CastPetAura(petSpell);
}

void Unit::RemovePetAura(PetAura const* petSpell)
{
    if (GetTypeId() != TYPEID_PLAYER)
        return;

    m_petAuras.erase(petSpell);
    if (Pet* pet = ToPlayer()->GetPet())
    {
        pet->RemoveOwnedAura(petSpell->GetAura(pet->GetEntry()));
        ASSERT(!pet->HasAura(petSpell->GetAura(pet->GetEntry())));   // Ensure that pet was an owner of aura if any.
    }
}

Pet* Unit::CreateTamedPetFrom(Creature* creatureTarget, uint32 spell_id)
{
    if (GetTypeId() != TYPEID_PLAYER)
        return NULL;

    Pet* pet = new Pet(ToPlayer(), HUNTER_PET);

    if (!pet->CreateBaseAtCreature(creatureTarget))
    {
        delete pet;
        return NULL;
    }

    InitTamedPet(pet, GetLevel(), spell_id);

    return pet;
}

Pet* Unit::CreateTamedPetFrom(uint32 creatureEntry, uint32 spell_id)
{
    if (GetTypeId() != TYPEID_PLAYER)
        return NULL;

    CreatureTemplate const* creatureInfo = sObjectMgr->GetCreatureTemplate(creatureEntry);
    if (!creatureInfo)
        return NULL;

    Pet* pet = new Pet(ToPlayer(), HUNTER_PET);

    if (!pet->CreateBaseAtCreatureInfo(creatureInfo, this) || !InitTamedPet(pet, GetLevel(), spell_id))
    {
        delete pet;
        return NULL;
    }

    return pet;
}

bool Unit::InitTamedPet(Pet* pet, uint8 level, uint32 spell_id)
{
    pet->SetCreatorGUID(GetGUID());
    pet->SetFaction(GetFaction());
    pet->SetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL, spell_id);

    if (GetTypeId() == TYPEID_PLAYER)
        pet->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);

    if (!pet->InitStatsForLevel(level))
    {
        TC_LOG_ERROR("entities.unit", "Pet::InitStatsForLevel() failed for creature (Entry: %u)!", pet->GetEntry());
        return false;
    }

    for (auto phase : GetPhases())
        pet->SetPhased(phase, false, true);

    pet->GetCharmInfo()->SetPetNumber(sObjectMgr->GeneratePetNumber(), true);
    // this enables pet details window (Shift+P)
    pet->InitPetCreateSpells();
    //pet->InitLevelupSpellsForLevel();
    pet->SetFullHealth();
    return true;
}

bool Unit::IsTriggeredAtSpellProcEvent(Unit* victim, ProcTriggeredData& triggerData, SpellInfo const* procSpell, uint32 procFlag, uint32 procExtra, WeaponAttackType attType, bool isVictim, bool active)
{
    Aura* aura = triggerData.aura;
    SpellInfo const* spellProto = aura->GetSpellInfo();

    // let the aura be handled by new proc system if it has new entry
    if (sSpellMgr->GetSpellProcEntry(spellProto->Id))
        return false;

    // Get proc Event Entry
    SpellProcEventEntry const* spellProcEvent = sSpellMgr->GetSpellProcEvent(spellProto->Id);
    triggerData.spellProcEvent = spellProcEvent;

    // Get EventProcFlag
    uint32 EventProcFlag;
    if (spellProcEvent && spellProcEvent->procFlags) // if exist get custom spellProcEvent->procFlags
        EventProcFlag = spellProcEvent->procFlags;
    else
        EventProcFlag = spellProto->ProcFlags;       // else get from spell proto
    // Continue if no trigger exist
    if (!EventProcFlag)
        return false;

    // Additional checks for triggered spells (ignore trap casts)
    if (procExtra & PROC_EX_INTERNAL_TRIGGERED && !(procFlag & PROC_FLAG_DONE_TRAP_ACTIVATION))
    {
        if (!(spellProto->AttributesEx3 & SPELL_ATTR3_CAN_PROC_WITH_TRIGGERED))
            return false;
    }

    // For damage-breakable auras and spells with this attribute i.e. 5019 - Shoot
    // But 49016 - Unholy Frenzy must not break CC auras.
    // Stealth must be breakable anyway...
    if (procSpell && !procSpell->CanTriggerProc())
        if ((!spellProto->HasAttribute(SPELL_ATTR0_CU_AURA_CC) || procSpell->IsPositive()) && !spellProto->HasAura(SPELL_AURA_MOD_STEALTH))
            return false;

    // Check spellProcEvent data requirements
    if (!sSpellMgr->IsSpellProcEventCanTriggeredBy(spellProcEvent, EventProcFlag, procSpell, procFlag, procExtra, active))
        return false;

    if (Battleground const* bg = GetBattlegorund())
        if (spellProto->CheckArenaAndBattlegroundCastRules(bg) != SPELL_CAST_OK)
            return false;

    if (spellProto->HasAttribute(SPELL_ATTR11_NOT_USABLE_IN_CHALLENGES) && GetMap()->IsChallengeDungeon())
        return false;

    // In most cases req get honor or XP from kill
    if (EventProcFlag & PROC_FLAG_KILL && GetTypeId() == TYPEID_PLAYER)
    {
        bool allow = false;

        if (victim)
            allow = ToPlayer()->IsHonorOrXPTarget(victim);

        // Shadow Word: Death - can trigger from every kill
        if (aura->GetId() == 32409)
            allow = true;
        if (!allow)
            return false;
    }
    // Aura added by spell can`t trigger from self (prevent drop charges/do triggers)
    // But except periodic and kill triggers (can triggered from self)
    if (procSpell && procSpell->Id == spellProto->Id
        && !(spellProto->ProcFlags&(PROC_FLAG_TAKEN_PERIODIC | PROC_FLAG_KILL)))
        return false;

    // Check if current equipment allows aura to proc
    if (!isVictim && GetTypeId() == TYPEID_PLAYER)
    {
        Player* player = ToPlayer();
        if (spellProto->EquippedItemClass == ITEM_CLASS_WEAPON)
        {
            Item* item = NULL;
            if (attType == BASE_ATTACK || attType == RANGED_ATTACK)
                item = player->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            else if (attType == OFF_ATTACK)
                item = player->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);

            if (player->IsInFeralForm())
                return false;

            if (!item || item->IsBroken() || item->GetTemplate()->Class != ITEM_CLASS_WEAPON || !((1 << item->GetTemplate()->SubClass) & spellProto->EquippedItemSubClassMask))
                return false;
        }
        else if (spellProto->EquippedItemClass == ITEM_CLASS_ARMOR)
        {
            // Check if player is wearing shield
            Item* item = player->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || item->IsBroken() || item->GetTemplate()->Class != ITEM_CLASS_ARMOR || !((1 << item->GetTemplate()->SubClass) & spellProto->EquippedItemSubClassMask))
                return false;
        }
    }
    // Get chance from spell
    float chance = float(spellProto->ProcChance);
    // If in spellProcEvent exist custom chance, chance = spellProcEvent->customChance;
    if (spellProcEvent && spellProcEvent->customChance)
        chance = spellProcEvent->customChance;

    // Check cooldown
    if (!GetSpellHistory()->IsProcReady(spellProto->Id))
        return false;

    if (!aura->CallScriptCheckProcHandlers(triggerData.auraApp, triggerData.eventInfo))
        return false;

    bool rppmAllAttacks = false;

    if (spellProto->ProcsPerMinute)
    {
        Item* item = nullptr;
        Player const* player = isVictim && victim ? victim->ToPlayer() : ToPlayer();
        if (aura->GetCastItemGUID() && player)
        {
            // I really don't know how this shit supposed to work
            item = player->GetItemByGuid(aura->GetCastItemGUID());
            if (item && (item == player->GetWeaponForAttack(BASE_ATTACK) || item == player->GetWeaponForAttack(OFF_ATTACK)))
            {
                if (item != player->GetWeaponForAttack(attType))
                    return false;
            }
            else if (procSpell)
                rppmAllAttacks = true;
        }
        if (!isVictim)
            chance = GetRealPPMProcChance(spellProto, item, attType);
        else if (victim)
            chance = victim->GetRealPPMProcChance(spellProto, item, attType);
    }
    // If PPM exist calculate chance from PPM
    else if ((spellProcEvent && spellProcEvent->ppmRate != 0.0f))
    {
        if (!isVictim)
        {
            uint32 weaponSpeed = GetAttackTime(attType);
            chance = GetPPMProcChance(weaponSpeed, spellProcEvent->ppmRate, spellProto);
        }
        else if (victim)
        {
            uint32 weaponSpeed = victim->GetAttackTime(attType);
            chance = victim->GetPPMProcChance(weaponSpeed, spellProcEvent->ppmRate, spellProto);
        }
    }

    Player* player = GetSpellModOwner();
    if (spellProto->ProcsPerMinute)
    {
        if (rppmAllAttacks)
        {
            GetSpellHistory()->RppmProcAttempt(spellProto->Id, BASE_ATTACK);
            GetSpellHistory()->RppmProcAttempt(spellProto->Id, OFF_ATTACK);
            GetSpellHistory()->RppmProcAttempt(spellProto->Id, RANGED_ATTACK);
        }
        else
            GetSpellHistory()->RppmProcAttempt(spellProto->Id, attType);
    }

    if (spellProto->ProcsPerMinute && procSpell && procSpell->IsAoESpell())
    {
        uint32 procId = (procSpell->Id << 2) + spellProto->Id;
        if (!GetSpellHistory()->IsProcReady(procId))
            return false;
        GetSpellHistory()->StartProcCooldown(procId, Milliseconds(200));
    }

    // Apply chance modifer aura
    if (player)
        player->ApplySpellMod(spellProto->Id, SPELLMOD_CHANCE_OF_SUCCESS, chance);

    return roll_chance_f(chance);
}

bool Unit::HandleAuraRaidProcFromChargeWithValue(AuraEffect* triggeredByAura)
{
    // aura can be deleted at casts
    SpellInfo const* spellProto = triggeredByAura->GetSpellInfo();
    int32 heal = triggeredByAura->GetAmount();
    ObjectGuid casterGuid = triggeredByAura->GetCasterGUID();
    Unit* caster = triggeredByAura->GetCaster();

    // Currently only Prayer of Mending
    if (!(spellProto->SpellFamilyName == SPELLFAMILY_PRIEST && spellProto->SpellFamilyFlags [1] & 0x20))
    {
        TC_LOG_DEBUG("spells", "Unit::HandleAuraRaidProcFromChargeWithValue, received not handled spell: %u", spellProto->Id);
        return false;
    }

    // heal
    CastCustomSpell(33110, SPELLVALUE_BASE_POINT0, heal, this, true, nullptr, nullptr, casterGuid);

    // jumps
    int32 jumps = triggeredByAura->GetBase()->GetCharges() - 1;

    // current aura expire
    triggeredByAura->GetBase()->SetCharges(1);             // will removed at next charges decrease

    // next target selection
    if (jumps > 0)
    {
        float radius = triggeredByAura->GetSpellEffectInfo().CalcRadius(caster);

        std::list<WorldObject*> targets;
        Player* player = GetAffectingPlayer();

        if (player && player->GetGroup())
        {
            for (auto&& itr : *player->GetGroup())
            {
                if (!itr->IsWithinDist(this, radius))
                    continue;

                targets.push_back(itr);
                if (Unit* pet = itr->GetGuardianPet())
                    if (!pet->IsWithinDist(this, radius))
                        targets.push_back(pet);
            }
        }
        else
        {
            if (Unit* owner = GetOwner())
                if (!owner->IsWithinDist(this, radius))
                    targets.push_back(owner);
            if (Unit* pet = GetGuardianPet())
                if (!pet->IsWithinDist(this, radius))
                    targets.push_back(pet);
        }
        targets.remove(this);

        Spell::ApplySmartHeal(this, targets, 1);

        if (!targets.empty())
        {
            Unit* triggerTarget = targets.front()->ToUnit();
            CastCustomSpell(spellProto->Id, SPELLVALUE_BASE_POINT0, heal, triggerTarget, true, nullptr, triggeredByAura, casterGuid);
            if (Aura* aura = triggerTarget->GetAura(spellProto->Id, casterGuid))
                aura->SetCharges(jumps);
        }
    }
    return true;

}
bool Unit::HandleAuraRaidProcFromCharge(AuraEffect* triggeredByAura)
{
    // aura can be deleted at casts
    SpellInfo const* spellProto = triggeredByAura->GetSpellInfo();

    uint32 damageSpellId;
    switch (spellProto->Id)
    {
        case 57949:            // shiver
            damageSpellId = 57952;
            //animationSpellId = 57951; dummy effects for jump spell have unknown use (see also 41637)
            break;
        case 59978:            // shiver
            damageSpellId = 59979;
            break;
        case 43593:            // Cold Stare
            damageSpellId = 43594;
            break;
        default:
            TC_LOG_ERROR("entities.unit", "Unit::HandleAuraRaidProcFromCharge, received unhandled spell: %u", spellProto->Id);
            return false;
    }

    ObjectGuid caster_guid = triggeredByAura->GetCasterGUID();

    // jumps
    int32 jumps = triggeredByAura->GetBase()->GetCharges() - 1;

    // current aura expire
    triggeredByAura->GetBase()->SetCharges(1);             // will removed at next charges decrease

    // next target selection
    if (jumps > 0)
    {
        if (Unit* caster = triggeredByAura->GetCaster())
        {
            float radius = triggeredByAura->GetSpellInfo()->Effects [triggeredByAura->GetEffIndex()].CalcRadius(caster);
            if (Unit* target = GetNextRandomRaidMemberOrPet(radius))
            {
                CastSpell(target, spellProto, true, NULL, triggeredByAura, caster_guid);
                if (Aura* aura = target->GetAura(spellProto->Id, caster->GetGUID()))
                    aura->SetCharges(jumps);
            }
        }
    }

    CastSpell(this, damageSpellId, true, NULL, triggeredByAura, caster_guid);

    return true;
}

void Unit::SendDurabilityLoss(Player* receiver, uint32 percent)
{
    WorldPacket data(SMSG_DURABILITY_DAMAGE_DEATH, 4);
    data << uint32(percent);
    receiver->GetSession()->SendPacket(&data);
}

void Unit::PlayOneShotAnimKitId(uint16 animKitId)
{
    if (animKitId && !sAnimKitStore.LookupEntry(animKitId))
        return;

    ObjectGuid guid = GetGUID();
    WorldPacket data(SMSG_PLAY_ONE_SHOT_ANIM_KIT, 7 + 2);

    data.WriteBit(guid[3]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[2]);

    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[4]);
    data << uint16(animKitId);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[0]);

    SendMessageToSet(&data, true);
}

void Unit::SetAIAnimKitId(uint16 animKitId)
{
    if (_aiAnimKitId == animKitId)
        return;

    if (animKitId && !sAnimKitStore.LookupEntry(animKitId))
        return;

    _aiAnimKitId = animKitId;

    ObjectGuid guid = GetGUID();
    WorldPacket data(SMSG_SET_AI_ANIM_KIT);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[7]);

    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[5]);
    data << uint16(animKitId);
    data.WriteByteSeq(guid[6]);
    SendMessageToSet(&data, true);
}

void Unit::SetMovementAnimKitId(uint16 animKitId)
{
    if (_movementAnimKitId == animKitId)
        return;

    if (animKitId && !sAnimKitStore.LookupEntry(animKitId))
        return;

    _movementAnimKitId = animKitId;

    ObjectGuid guid = GetGUID();
    WorldPacket data(SMSG_SET_MOVEMENT_ANIM_KIT);
    data << uint16(animKitId);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[3]);

    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[1]);
    SendMessageToSet(&data, true);
}

void Unit::SetMeleeAnimKitId(uint16 animKitId)
{
    if (_meleeAnimKitId == animKitId)
        return;

    if (animKitId && !sAnimKitStore.LookupEntry(animKitId))
        return;

    _meleeAnimKitId = animKitId;

    ObjectGuid guid = GetGUID();
    WorldPacket data(SMSG_SET_MELEE_ANIM_KIT);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[5]);

    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[7]);
    data << uint16(animKitId);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[2]);
    SendMessageToSet(&data, true);
}

void Unit::Kill(Unit* victim, bool durabilityLoss, SpellInfo const* spellInfo)
{
    // Prevent killing unit twice (and giving reward from kill twice)
    if (!victim->GetHealth())
        return;

    // I hope this doesn't break something...
    // But we have bug with killing unit than it is dead
    if (!victim->IsAlive())
    {
        TC_LOG_ERROR("shitlog", "Unit::Kill, victim isn't alive, killer: " UI64FMTD " (%u), victim: " UI64FMTD " (%u)\n", GetGUID().GetRawValue(), GetEntry(), victim->GetGUID().GetRawValue(), victim->GetEntry());
        return;
    }

    // Prevents the "double kill" from OnEffectRemove handlers
    victim->SetHealth(0);

    // find player: owner of controlled `this` or `this` itself maybe
    Player* player = GetCharmerOrOwnerPlayerOrPlayerItself();
    Creature* creature = victim->ToCreature();

    bool isRewardAllowed = true;
    if (creature)
    {
        isRewardAllowed = creature->IsDamageEnoughForLootingAndReward();
        if (!isRewardAllowed)
            creature->SetLootRecipient(NULL);
    }

    if (isRewardAllowed && creature)
        if (Player* looter = creature->GetLootRecipient())
            if (looter->IsInMap(creature))
                player = looter;

    // Reward player, his pets, and group/raid members
    // call kill spell proc event (before real die and combat stop to triggering auras removed at death/combat stop)
    if (isRewardAllowed && player && player != victim)
    {
        ObjectGuid killerGuid = player->GetGUID();
        ObjectGuid victimGuid = victim->GetGUID();
        WorldPacket data(SMSG_PARTYKILLLOG); // send event PARTY_KILL

        data.WriteBit(killerGuid[7]);
        data.WriteBit(killerGuid[2]);
        data.WriteBit(victimGuid[1]);
        data.WriteBit(killerGuid[4]);
        data.WriteBit(victimGuid[2]);
        data.WriteBit(victimGuid[5]);
        data.WriteBit(killerGuid[3]);
        data.WriteBit(killerGuid[1]);
        data.WriteBit(killerGuid[0]);
        data.WriteBit(victimGuid[3]);
        data.WriteBit(victimGuid[0]);
        data.WriteBit(victimGuid[4]);
        data.WriteBit(killerGuid[6]);
        data.WriteBit(victimGuid[7]);
        data.WriteBit(killerGuid[5]);
        data.WriteBit(victimGuid[6]);

        data.WriteByteSeq(killerGuid[0]);
        data.WriteByteSeq(killerGuid[5]);
        data.WriteByteSeq(victimGuid[0]);
        data.WriteByteSeq(victimGuid[2]);
        data.WriteByteSeq(killerGuid[7]);
        data.WriteByteSeq(killerGuid[6]);
        data.WriteByteSeq(killerGuid[1]);
        data.WriteByteSeq(killerGuid[4]);
        data.WriteByteSeq(victimGuid[4]);
        data.WriteByteSeq(victimGuid[1]);
        data.WriteByteSeq(killerGuid[2]);
        data.WriteByteSeq(victimGuid[6]);
        data.WriteByteSeq(victimGuid[3]);
        data.WriteByteSeq(victimGuid[5]);
        data.WriteByteSeq(victimGuid[7]);
        data.WriteByteSeq(killerGuid[3]);

        Player* looter = player;

        if (Group* group = player->GetGroup())
        {
            group->BroadcastPacket(&data, group->GetMemberGroup(player->GetGUID()));

            if (creature && group->GetLootMethod() != FREE_FOR_ALL && !creature->IsPet())
            {
                group->UpdateLooterGuid(creature, true);
                if (group->GetLooterGuid())
                {
                    looter = ObjectAccessor::FindPlayer(group->GetLooterGuid());
                    if (looter)
                    {
                        creature->SetLootRecipient(looter);   // update creature loot recipient to the allowed looter.
                        group->SendLooter(creature, looter);
                    }
                    else
                        group->SendLooter(creature, NULL);
                }
                else
                    group->SendLooter(creature, NULL);

                group->UpdateLooterGuid(creature);
            }
        }
        else
        {
            player->SendDirectMessage(&data);

            if (creature)
            {
                ObjectGuid creatureGuid = creature->GetGUID();
                WorldPacket data2(SMSG_LOOT_LIST);

                data2.WriteBit(creatureGuid [5]);
                data2.WriteBit(0);
                data2.WriteBit(creatureGuid [1]);
                data2.WriteBit(0);
                data2.WriteBit(creatureGuid [4]);
                data2.WriteBit(creatureGuid [3]);
                data2.WriteBit(creatureGuid [2]);
                data2.WriteBit(creatureGuid [7]);
                data2.WriteBit(creatureGuid [0]);
                data2.WriteBit(creatureGuid [6]);

                data2.WriteByteSeq(creatureGuid [5]);
                data2.WriteByteSeq(creatureGuid [1]);
                data2.WriteByteSeq(creatureGuid [6]);
                data2.WriteByteSeq(creatureGuid [2]);
                data2.WriteByteSeq(creatureGuid [3]);
                data2.WriteByteSeq(creatureGuid [0]);
                data2.WriteByteSeq(creatureGuid [7]);
                data2.WriteByteSeq(creatureGuid [4]);

                player->SendMessageToSet(&data2, true);
            }
        }

        if (creature && !creature->GetMap()->IsChallengeDungeon())
        {
            Loot* loot = &creature->loot;
            if (creature->lootForPickPocketed)
            {
                creature->ResetPickPocketRefillTimer();
                creature->lootForPickPocketed = false;
            }

            loot->clear();
            if (uint32 lootid = creature->GetCreatureTemplate()->lootid)
                loot->FillLoot(creature, lootid, LootTemplates_Creature, looter, false, false, creature->GetMap()->GetDifficulty());

            loot->generateMoneyLoot(creature->GetCreatureTemplate()->mingold, creature->GetCreatureTemplate()->maxgold);
        }

        player->RewardPlayerAndGroupAtKill(victim, false);
    }

    // Do KILL and KILLED procs. KILL proc is called only for the unit who landed the killing blow (and its owner - for pets and totems) regardless of who tapped the victim
    if (IsPet() || IsTotem())
        if (Unit* owner = GetOwner())
            owner->ProcDamageAndSpell(victim, PROC_FLAG_KILL, PROC_FLAG_NONE, PROC_EX_NONE, 0, BASE_ATTACK, spellInfo);

    if (victim->GetCreatureType() != CREATURE_TYPE_CRITTER)
        ProcDamageAndSpell(victim, PROC_FLAG_KILL, PROC_FLAG_KILLED, PROC_EX_NONE, 0, BASE_ATTACK, spellInfo);

    // Proc auras on death - must be before aura/combat remove
    victim->ProcDamageAndSpell(NULL, PROC_FLAG_DEATH, PROC_FLAG_NONE, PROC_EX_NONE, 0, BASE_ATTACK, 0);

    // update get killing blow achievements, must be done before setDeathState to be able to require auras on target
    // and before Spirit of Redemption as it also removes auras
    if (player)
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_GET_KILLING_BLOWS, 1, 0, 0, victim);

    // if talent known but not triggered (check priest class for speedup check)
    bool spiritOfRedemption = false;
    if (victim->GetTypeId() == TYPEID_PLAYER && victim->GetClass() == CLASS_PRIEST)
    {
        AuraEffectList const& dummyAuras = victim->GetAuraEffectsByType(SPELL_AURA_DUMMY);
        for (AuraEffectList::const_iterator itr = dummyAuras.begin(); itr != dummyAuras.end(); ++itr)
        {
            if ((*itr)->GetSpellInfo()->SpellIconID == 1654)
            {
                AuraEffect const* aurEff = *itr;
                // save value before aura remove
                uint32 ressSpellId = victim->GetUInt32Value(PLAYER_FIELD_SELF_RES_SPELL);
                if (!ressSpellId)
                    ressSpellId = victim->ToPlayer()->GetResurrectionSpellId();
                // Remove all expected to remove at death auras (most important negative case like DoT or periodic triggers)
                victim->RemoveAllAurasOnDeath();
                // restore for use at real death
                victim->SetUInt32Value(PLAYER_FIELD_SELF_RES_SPELL, ressSpellId);

                // FORM_SPIRITOFREDEMPTION and related auras
                victim->CastSpell(victim, 27827, true, NULL, aurEff);
                spiritOfRedemption = true;
                break;
            }
        }
    }

    if (!spiritOfRedemption)
    {
        TC_LOG_DEBUG("entities.unit", "SET JUST_DIED");
        victim->setDeathState(JUST_DIED);
        /* Turns out this was a bad idea, because units can kill themselves during their AI update, causing "this" pointer to become deleted once we go back up the call stack to UpdateAI.
           The issue of wrong AI hooks being called was solved in another way in the code below, by manually calling hooks for i_disabledAI if it was present and waiting to be reenabled.
        // Switch back to regular AI if the victim was charmed. Charm auras should have been removed in Unit::setDeathState()
        if (victim->NeedChangeAI)
        {
            victim->UpdateCharmAI();
            victim->NeedChangeAI = false;
            victim->IsAIEnabled = (victim->GetAI() != nullptr);
            victim->LastCharmerGUID = 0;
        }
        */
    }

    // Inform pets (if any) when player kills target)
    // MUST come after victim->setDeathState(JUST_DIED); or pet next target
    // selection will get stuck on same target and break pet react state
    if (player)
    {
        Pet* pet = player->GetPet();
        if (pet && pet->IsAlive() && pet->isControlled())
            pet->AI()->KilledUnit(victim);
    }

    // 10% durability loss on death
    // clean InHateListOf
    if (Player* plrVictim = victim->ToPlayer())
    {
        // remember victim PvP death for corpse type and corpse reclaim delay
        // at original death (not at SpiritOfRedemtionTalent timeout)
        plrVictim->SetPvPDeath(player != NULL);

        // only if not player and not controlled by player pet. And not at BG
        if ((durabilityLoss && !player && !victim->ToPlayer()->InBattleground()) || (player && sWorld->getBoolConfig(CONFIG_DURABILITY_LOSS_IN_PVP)))
        {
            double baseLoss = sWorld->getRate(RATE_DURABILITY_LOSS_ON_DEATH);
            uint32 loss = uint32(baseLoss - (baseLoss * plrVictim->GetTotalAuraMultiplier(SPELL_AURA_MOD_DURABILITY_LOSS)));
            TC_LOG_DEBUG("entities.unit", "We are dead, losing %u percent durability", loss);
            // Durability loss is calculated more accurately again for each item in Player::DurabilityLoss
            plrVictim->DurabilityLossAll(baseLoss, false);
            // durability lost message
            SendDurabilityLoss(plrVictim, loss);
        }
        // Call KilledUnit for creatures
        if (GetTypeId() == TYPEID_UNIT && IsAIEnabled)
            ToCreature()->AI()->KilledUnit(victim);

        // last damage from non duel opponent or opponent controlled creature
        if (plrVictim->duel)
        {
            plrVictim->duel->opponent->CombatStopWithPets(true);
            plrVictim->CombatStopWithPets(true);
            plrVictim->DuelComplete(DUEL_INTERRUPTED);
        }
    }
    else                                                // creature died
    {
        TC_LOG_DEBUG("entities.unit", "DealDamageNotPlayer");

        if (!creature->IsPet())
        {
            creature->DeleteThreatList();
            CreatureTemplate const* cInfo = creature->GetCreatureTemplate();
            if (cInfo && (cInfo->lootid || cInfo->maxgold > 0))
                creature->SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        }

        // Call KilledUnit for creatures, this needs to be called after the lootable flag is set
        if (GetTypeId() == TYPEID_UNIT && IsAIEnabled)
            ToCreature()->AI()->KilledUnit(victim);

        // Get original creature AI in case it was charmed, as the AI hasn't switched back from PossessedAI/PetAI yet
        CreatureAI* victimAI = creature->IsAIEnabled ? creature->AI() : nullptr;
        if (creature->NeedChangeAI && creature->i_disabledAI)
            if (CreatureAI* ai = dynamic_cast<CreatureAI*>(creature->i_disabledAI))
                victimAI = ai;

        // Call creature just died function
        if (victimAI)
            victimAI->JustDied(this);

        if (TempSummon* summon = creature->ToTempSummon())
            if (Unit* summoner = summon->GetSummoner())
                if (summoner->ToCreature() && summoner->IsAIEnabled)
                    summoner->ToCreature()->AI()->SummonedCreatureDies(creature, this);

        // Dungeon specific stuff, only applies to players killing creatures
        if (creature->GetInstanceId())
        {
            Map* instanceMap = creature->GetMap();
            Player* creditedPlayer = GetCharmerOrOwnerPlayerOrPlayerItself();
            /// @todo do instance binding anyway if the charmer/owner is offline

            if (instanceMap->IsDungeon() && creditedPlayer)
            {
                if (instanceMap->IsRaidOrHeroicDungeon())
                {
                    if (creature->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_INSTANCE_BIND)
                    {
                        ((InstanceMap*)instanceMap)->PermBindAllPlayers(creditedPlayer);
                        if (instanceMap->IsRaid() && ((InstanceMap*)instanceMap)->IsLFGMap())
                        {
                            Group* group = creditedPlayer->GetGroup();
                            if (group)
                                group->AddLootLockout(creature);
                            else
                                creditedPlayer->AddLootLockout(LootLockoutType::PersonalLoot, creature->GetEntry(), instanceMap->GetDifficulty());
                        }

                    }
                }
                else
                {
                    // the reset time is set but not added to the scheduler
                    // until the players leave the instance
                    time_t resettime = creature->GetRespawnTimeEx() + 2 * HOUR;
                    if (InstanceSave* save = sInstanceSaveMgr->GetInstanceSave(creature->GetInstanceId()))
                        if (save->GetResetTime() < resettime) save->SetResetTime(resettime);
                }
            }
        }
    }

    // outdoor pvp things, do these after setting the death state, else the player activity notify won't work... doh...
    // handle player kill only if not suicide (spirit of redemption for example)
    if (player && this != victim)
    {
        if (OutdoorPvP* pvp = player->GetOutdoorPvP())
            pvp->HandleKill(player, victim);

        if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(player->GetZoneId()))
            bf->HandleKill(player, victim);
    }

    //if (victim->GetTypeId() == TYPEID_PLAYER)
    //    if (OutdoorPvP* pvp = victim->ToPlayer()->GetOutdoorPvP())
    //        pvp->HandlePlayerActivityChangedpVictim->ToPlayer();

    // battleground things (do this at the end, so the death state flag will be properly set to handle in the bg->handlekill)
    if (player && player->InBattleground())
    {
        if (Battleground* bg = player->GetBattleground())
        {
            if (Player* playerVictim = victim->ToPlayer())
                bg->HandleKillPlayer(playerVictim, player);
            else
                bg->HandleKillUnit(victim->ToCreature(), player);
        }
    }

    // achievement stuff
    if (victim->GetTypeId() == TYPEID_PLAYER)
    {
        if (GetTypeId() == TYPEID_UNIT)
            victim->ToPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_CREATURE, GetEntry());
        else if (GetTypeId() == TYPEID_PLAYER && victim != this)
            victim->ToPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_PLAYER, 1, ToPlayer()->GetTeam());
    }

    // Hook for OnPVPKill Event
    if (Player* killerPlr = ToPlayer())
    {
        if (Player* killedPlr = victim->ToPlayer())
            sScriptMgr->OnPVPKill(killerPlr, killedPlr);
        else if (Creature* killedCre = victim->ToCreature())
            sScriptMgr->OnCreatureKill(killerPlr, killedCre);
    }
    else if (Creature* killerCre = ToCreature())
    {
        if (Player* killed = victim->ToPlayer())
            sScriptMgr->OnPlayerKilledByCreature(killerCre, killed);
    }
}

float Unit::GetPositionZMinusOffset() const
{
    float offset = 0.0f;
    if (HasUnitMovementFlag(MOVEMENTFLAG_HOVER))
        offset = GetFloatValue(UNIT_FIELD_HOVER_HEIGHT);

    return GetPositionZ() - offset;
}

void Unit::SetControlled(bool apply, UnitState state)
{
    if (apply)
    {
        if (HasUnitState(state))
            return;

        AddUnitState(state);
        switch (state)
        {
            case UNIT_STATE_STUNNED:
                SetStunned(true);
                CastStop();
                break;
            case UNIT_STATE_ROOT:
                if (!HasUnitState(UNIT_STATE_STUNNED))
                    SetRooted(true);
                break;
            case UNIT_STATE_CONFUSED:
                if (!HasUnitState(UNIT_STATE_STUNNED))
                {
                    ClearUnitState(UNIT_STATE_MELEE_ATTACKING);
                    SendMeleeAttackStop();
                    // SendAutoRepeatCancel ?
                    SetConfused(true);
                    CastStop();
                }
                break;
            case UNIT_STATE_FLEEING:
                if (!HasUnitState(UNIT_STATE_STUNNED | UNIT_STATE_CONFUSED))
                {
                    ClearUnitState(UNIT_STATE_MELEE_ATTACKING);
                    SendMeleeAttackStop();
                    // SendAutoRepeatCancel ?
                    SetFeared(true);
                    CastStop();
                }
                break;
            default:
                break;
        }
    }
    else
    {
        switch (state)
        {
            case UNIT_STATE_STUNNED:
                if (HasAuraType(SPELL_AURA_MOD_STUN))
                    return;

                SetStunned(false);
                break;
            case UNIT_STATE_ROOT:
                if (HasAuraType(SPELL_AURA_MOD_ROOT) || GetVehicle())
                    return;

                if (!HasUnitState(UNIT_STATE_STUNNED))
                    SetRooted(false);
                break;
            case UNIT_STATE_CONFUSED:
                if (HasAuraType(SPELL_AURA_MOD_CONFUSE))
                    return;

                SetConfused(false);
                break;
            case UNIT_STATE_FLEEING:
                if (HasAuraType(SPELL_AURA_MOD_FEAR) || HasAuraType(SPELL_AURA_MOD_FEAR_2))
                    return;

                SetFeared(false);
                break;
            default:
                return;
        }

        ClearUnitState(state);

        if (HasUnitState(UNIT_STATE_STUNNED))
            SetStunned(true);
        else
        {
            if (HasUnitState(UNIT_STATE_ROOT))
                SetRooted(true);

            if (HasUnitState(UNIT_STATE_CONFUSED))
                SetConfused(true);
            else if (HasUnitState(UNIT_STATE_FLEEING))
                SetFeared(true);
        }
    }
}

void Unit::SetStunned(bool apply)
{
    if (apply)
    {
        SetTarget(ObjectGuid::Empty);
        SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);

        if (GetTypeId() == TYPEID_PLAYER)
            SetStandState(UNIT_STAND_STATE_STAND);

        SetRooted(true);

        CastStop();
    }
    else
    {
        if (IsAlive() && GetVictim())
            SetTarget(GetVictim()->GetGUID());

        // don't remove UNIT_FLAG_STUNNED for pet when owner is mounted (disabled pet's interface)
        Unit* owner = GetOwner();
        if (!owner || (owner->GetTypeId() == TYPEID_PLAYER && !owner->ToPlayer()->IsMounted()))
            RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);

        if (!HasUnitState(UNIT_STATE_ROOT))         // prevent moving if it also has root effect
            SetRooted(false);
    }
}

void Unit::SetRooted(bool apply, bool packetOnly /*= false*/)
{
    if (!packetOnly)
    {
        if (apply)
        {
            bool stoppable = !GetVehicle() && !GetTransport() && !GetTransGUID();

            bool isEffectMovement = GetMotionMaster()->GetCurrentMovementGeneratorType() == EFFECT_MOTION_TYPE;

            // MOVEMENTFLAG_ROOT cannot be used in conjunction with MOVEMENTFLAG_MASK_MOVING (tested 3.3.5a)
            // this will freeze clients. That's why we remove MOVEMENTFLAG_MASK_MOVING before
            // setting MOVEMENTFLAG_ROOT
            RemoveUnitMovementFlag(MOVEMENTFLAG_MASK_MOVING);
            AddUnitMovementFlag(MOVEMENTFLAG_ROOT);

            if (stoppable && !isEffectMovement)
                StopMoving();
        }
        else
            RemoveUnitMovementFlag(MOVEMENTFLAG_ROOT);
    }

    if (apply)
        Movement::PacketSender(this, SMSG_SPLINE_MOVE_ROOT, SMSG_MOVE_ROOT, SMSG_MOVE_ROOT).Send();
    else
        Movement::PacketSender(this, SMSG_SPLINE_MOVE_UNROOT, SMSG_MOVE_UNROOT, SMSG_MOVE_UNROOT).Send();
}

void Unit::SetFeared(bool apply)
{
    if (apply)
    {
        SetTarget(ObjectGuid::Empty);

        Unit* caster = nullptr;
        bool hasAura = false;
        for (auto&& type : { SPELL_AURA_MOD_FEAR, SPELL_AURA_MOD_FEAR_2 })
        {
            auto& fearAuras = GetAuraEffectsByType(type);
            if (!GetAuraEffectsByType(type).empty())
            {
                caster = ObjectAccessor::GetUnit(*this, fearAuras.front()->GetCasterGUID());
                if (caster)
                {
                    hasAura = true;
                    break;
                }
            }
        }
        if (!caster)
            caster = getAttackerForHelper();
        GetMotionMaster()->MoveFleeing(caster, !hasAura ? sWorld->getIntConfig(CONFIG_CREATURE_FAMILY_FLEE_DELAY) : 0);             // caster == NULL processed in MoveFleeing
    }
    else
    {
        // In case if generator was expired somehow
        RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING);
        ClearUnitState(UNIT_STATE_FLEEING | UNIT_STATE_FLEEING_MOVE);

        if (IsAlive())
        {
            if (GetMotionMaster()->GetCurrentMovementGeneratorType() == FLEEING_MOTION_TYPE)
                GetMotionMaster()->MovementExpired();
            if (GetVictim())
                SetTarget(GetVictim()->GetGUID());
        }
    }

    if (Player* player = isPossessedByPlayer() ? GetCharmerOrOwnerPlayerOrPlayerItself() : ToPlayer())
        player->SetClientControl(this, !apply);
}

void Unit::SetConfused(bool apply)
{
    if (apply)
    {
        SetTarget(ObjectGuid::Empty);
        GetMotionMaster()->MoveConfused();
    }
    else
    {
        // In case if generator was expired somehow
        RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
        ClearUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_CONFUSED_MOVE);

        if (IsAlive())
        {
            if (GetMotionMaster()->GetCurrentMovementGeneratorType() == CONFUSED_MOTION_TYPE)
                GetMotionMaster()->MovementExpired();
            if (GetVictim())
                SetTarget(GetVictim()->GetGUID());
        }
    }

    if (Player* player = isPossessedByPlayer() ? GetCharmerOrOwnerPlayerOrPlayerItself() : ToPlayer())
        player->SetClientControl(this, !apply);
}

bool Unit::SetCharmedBy(Unit* charmer, CharmType type, AuraApplication const* aurApp)
{
    if (!charmer)
        return false;

    if (GetTypeId() == TYPEID_UNIT)
        TC_LOG_DEBUG("crash", "Unit::SetCharmedBy1, GUID: " UI64FMTD ", entry: %u, charmer: " UI64FMTD ", type: %u, aura: %u", GetGUID().GetRawValue(), GetEntry(), GetCharmerGUID().GetRawValue(), type, aurApp ? aurApp->GetBase()->GetId() : 0);

    // dismount players when charmed
    if (GetTypeId() == TYPEID_PLAYER)
        RemoveAurasByType(SPELL_AURA_MOUNTED);

    if (charmer->GetTypeId() == TYPEID_PLAYER)
        charmer->RemoveAurasByType(SPELL_AURA_MOUNTED);

    ASSERT(type != CHARM_TYPE_POSSESS || charmer->GetTypeId() == TYPEID_PLAYER);
    ASSERT((type == CHARM_TYPE_VEHICLE) == IsVehicle() || (GetTypeId() == TYPEID_PLAYER && HasAuraType(SPELL_AURA_SET_VEHICLE_ID)));

    TC_LOG_DEBUG("entities.unit", "SetCharmedBy: charmer %u (GUID %u), charmed %u (GUID %u), type %u.", charmer->GetEntry(), charmer->GetGUID().GetCounter(), GetEntry(), GetGUID().GetCounter(), uint32(type));

    if (this == charmer)
    {
        TC_LOG_FATAL("entities.unit", "Unit::SetCharmedBy: Unit %u (GUID %u) is trying to charm itself!", GetEntry(), GetGUID().GetCounter());
        return false;
    }

    //if (HasUnitState(UNIT_STATE_UNATTACKABLE))
    //    return false;

    if (GetTypeId() == TYPEID_PLAYER && ToPlayer()->GetTransport())
    {
        TC_LOG_FATAL("entities.unit", "Unit::SetCharmedBy: Player on transport is trying to charm %u (GUID %u)", GetEntry(), GetGUID().GetCounter());
        return false;
    }

    // Already charmed
    if (GetCharmerGUID())
    {
        TC_LOG_FATAL("entities.unit", "Unit::SetCharmedBy: %u (GUID %u) has already been charmed but %u (GUID %u) is trying to charm it!", GetEntry(), GetGUID().GetCounter(), charmer->GetEntry(), charmer->GetGUID().GetCounter());
        return false;
    }

    CastStop();

    // Garrosh Touch of Y`shaarj shouldn`t remove combat - it cause multi-pot using (possible for whole, but nvm)
    if (!aurApp || (aurApp->GetBase()->GetId() != 145065 && aurApp->GetBase()->GetId() != 145171))
    {
        CombatStop(); /// @todo CombatStop(true) may cause crash (interrupt spells)
        DeleteThreatList();
    }

    // Charmer stop charming
    if (charmer->GetTypeId() == TYPEID_PLAYER)
    {
        charmer->ToPlayer()->StopCastingCharm();
        if (!aurApp || !aurApp->GetBase()->HasEffectType(SPELL_AURA_BIND_SIGHT))
            charmer->ToPlayer()->StopCastingBindSight();
    }

    // Charmed stop charming
    if (GetTypeId() == TYPEID_PLAYER)
    {
        ToPlayer()->StopCastingCharm();
        ToPlayer()->StopCastingBindSight();
    }

    // StopCastingCharm may remove a possessed pet?
    if (!IsInWorld())
    {
        TC_LOG_FATAL("entities.unit", "Unit::SetCharmedBy: %u (GUID %u) is not in world but %u (GUID %u) is trying to charm it!", GetEntry(), GetGUID().GetCounter(), charmer->GetEntry(), charmer->GetGUID().GetCounter());
        return false;
    }

    // charm is set by aura, and aura effect remove handler was called during apply handler execution
    // prevent undefined behaviour
    if (aurApp && aurApp->GetRemoveMode())
        return false;

    // Set charmed
    Map* map = GetMap();
    if (!IsVehicle() || (IsVehicle() && map && !map->IsBattleground()))
        SetFaction(charmer->GetFaction());

    charmer->SetCharm(this, true);

    if (GetTypeId() == TYPEID_UNIT && this->GetEntry() != 69458 && this->GetEntry() != 67275 && this->GetEntry() != 68849 && this->GetEntry() != 72952)
    {
        ToCreature()->AI()->OnCharmed(true);
        GetMotionMaster()->MoveIdle(); // Stop random movement
        // If a controlled motion is active - just delete the active one
        if (GetMotionMaster()->top() == GetMotionMaster()->GetMotionSlot(MOTION_SLOT_CONTROLLED))
            GetMotionMaster()->DirectDelete(MOTION_SLOT_ACTIVE);
        else
        {
            StopMoving();
            GetMotionMaster()->MovementExpired();
        }
    }
    else if (Player* player = ToPlayer())
    {
        if (player->isAFK())
            player->ToggleAFK();

        //if (charmer->GetTypeId() == TYPEID_UNIT) // we are charmed by a creature
        {
            // change AI to charmed AI on next Update tick
            NeedChangeAI = true;
            if (IsAIEnabled)
            {
                IsAIEnabled = false;
                player->AI()->OnCharmed(true);
            }
        }
        player->SetClientControl(this, false);
    }

    // charm is set by aura, and aura effect remove handler was called during apply handler execution
    // prevent undefined behaviour
    if (aurApp && aurApp->GetRemoveMode())
        return false;

    // Pets already have a properly initialized CharmInfo, don't overwrite it.
    if (type != CHARM_TYPE_VEHICLE && !GetCharmInfo())
    {
        InitCharmInfo();
        if (type == CHARM_TYPE_POSSESS)
            GetCharmInfo()->InitPossessCreateSpells();
        else
            GetCharmInfo()->InitCharmCreateSpells();
    }

    if (charmer->GetTypeId() == TYPEID_PLAYER)
    {
        switch (type)
        {
            case CHARM_TYPE_VEHICLE:
                SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
                charmer->ToPlayer()->SetClientControl(this, 1);

                if (this->GetEntry() != 69458 && this->GetEntry() != 67275 && this->GetEntry() != 68849 && this->GetEntry() != 72952)
                    charmer->ToPlayer()->SetMover(this);

                charmer->ToPlayer()->SetViewpoint(this, true);
                charmer->ToPlayer()->VehicleSpellInitialize();
                break;
            case CHARM_TYPE_POSSESS:
                AddUnitState(UNIT_STATE_POSSESSED);
                SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
                charmer->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                charmer->ToPlayer()->SetMover(this);
                charmer->ToPlayer()->SetClientControl(this, !HasUnitState(UNIT_STATE_FLEEING | UNIT_STATE_CONFUSED));
                charmer->ToPlayer()->SetViewpoint(this, true);
                charmer->ToPlayer()->PossessSpellInitialize();
                break;
            case CHARM_TYPE_CHARM:
                if (GetTypeId() == TYPEID_UNIT && charmer->GetClass() == CLASS_WARLOCK)
                {
                    CreatureTemplate const* cinfo = ToCreature()->GetCreatureTemplate();
                    if (cinfo && cinfo->type == CREATURE_TYPE_DEMON)
                    {
                        // to prevent client crash
                        SetClass(CLASS_MAGE);

                        // just to enable stat window
                        if (GetCharmInfo())
                            GetCharmInfo()->SetPetNumber(sObjectMgr->GeneratePetNumber(), true);

                        // if charmed two demons the same session, the 2nd gets the 1st one's name
                        SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, uint32(time(NULL))); // cast can't be helped
                    }
                }
                charmer->ToPlayer()->CharmSpellInitialize();
                break;
            default:
            case CHARM_TYPE_CONVERT:
                break;
        }
    }
    if (GetTypeId() == TYPEID_UNIT)
        TC_LOG_DEBUG("crash", "Unit::SetCharmedBy2, GUID: " UI64FMTD ", entry: %u, charmer: " UI64FMTD ", type: %u, aura: %u", GetGUID().GetRawValue(), GetEntry(), GetCharmerGUID().GetRawValue(), type, aurApp ? aurApp->GetBase()->GetId() : 0);
    return true;
}

void Unit::RemoveCharmedBy(Unit* charmer)
{
    if (!IsCharmed())
        return;

    if (!charmer)
        charmer = GetCharmer();
    if (charmer != GetCharmer()) // one aura overrides another?
    {
        TC_LOG_FATAL("entities.unit", "Unit::RemoveCharmedBy: this: " UI64FMTD " true charmer: " UI64FMTD " false charmer: " UI64FMTD,
            GetGUID().GetRawValue(), GetCharmerGUID().GetRawValue(), charmer->GetGUID().GetRawValue());
        //        ASSERT(false);
        return;
    }

    CharmType type;
    if (HasUnitState(UNIT_STATE_POSSESSED))
        type = CHARM_TYPE_POSSESS;
    else if (charmer && charmer->IsOnVehicle(this))
        type = CHARM_TYPE_VEHICLE;
    else
        type = CHARM_TYPE_CHARM;

    if (GetTypeId() == TYPEID_UNIT)
        TC_LOG_DEBUG("crash", "Unit::RemoveCharmedBy1, GUID: " UI64FMTD ", entry: %u, charmer: " UI64FMTD ", type: %u", GetGUID().GetRawValue(), GetEntry(), charmer->GetGUID().GetRawValue(), type);

    CastStop();

    // Garrosh Touch of Y`shaarj shouldn`t remove combat - it cause multi-pot using (possible for whole, but nvm)
    if (this->GetMapId() != 1136)
        CombatStop(); /// @todo CombatStop(true) may cause crash (interrupt spells)

    if (this->GetMapId() != 1136) // look up
    {
        getHostileRefManager().deleteReferences();
        DeleteThreatList();
    }

    Map* map = GetMap();
    if (!IsVehicle() || (IsVehicle() && map && !map->IsBattleground()))
        RestoreFaction();

    MovementGeneratorType mgType = GetMotionMaster()->GetMotionSlotType(MOTION_SLOT_ACTIVE);
    if (mgType == FOLLOW_MOTION_TYPE || mgType == CHASE_MOTION_TYPE)
    {
        // If a controlled motion is active - just delete the active targeted one
        if (GetMotionMaster()->top() == GetMotionMaster()->GetMotionSlot(MOTION_SLOT_CONTROLLED))
            GetMotionMaster()->DirectDelete(MOTION_SLOT_ACTIVE);
        else
        {
            StopMoving();
            GetMotionMaster()->MovementExpired();
        }
    }

    GetMotionMaster()->InitDefault();

    if (type == CHARM_TYPE_POSSESS)
    {
        ClearUnitState(UNIT_STATE_POSSESSED);
        RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
    }

    if (Creature* creature = ToCreature())
    {
        // Creature will restore its old AI on next update
        if (creature->AI())
            creature->AI()->OnCharmed(false);

        // Vehicle should not attack its passenger after he exists the seat
        if (type != CHARM_TYPE_VEHICLE)
            LastCharmerGUID = charmer->GetGUID();
    }
    else if (Player* player = ToPlayer())
    {
        //if (charmer->GetTypeId() == TYPEID_UNIT) // charmed by a creature, this means we had PlayerAI
        {
            NeedChangeAI = true;
            IsAIEnabled = false;
        }
        ToPlayer()->SetClientControl(this, true);
    }

    // If charmer still exists
    if (!charmer)
        return;

    ASSERT(type != CHARM_TYPE_POSSESS || charmer->GetTypeId() == TYPEID_PLAYER);
    ASSERT(type != CHARM_TYPE_VEHICLE || (GetTypeId() == TYPEID_UNIT && IsVehicle()));

    charmer->SetCharm(this, false);

    if (charmer->GetTypeId() == TYPEID_PLAYER)
    {
        switch (type)
        {
            case CHARM_TYPE_VEHICLE:
                charmer->ToPlayer()->SetClientControl(charmer, 1);
                charmer->ToPlayer()->SetViewpoint(this, false);
                charmer->ToPlayer()->SetClientControl(this, 0);
                if (GetTypeId() == TYPEID_PLAYER)
                    ToPlayer()->SetMover(this);
                break;
            case CHARM_TYPE_POSSESS:
                if (!charmer->ToPlayer()->HasUnitState(UNIT_STATE_FLEEING | UNIT_STATE_CONFUSED))
                    charmer->ToPlayer()->SetClientControl(charmer, 1);
                charmer->ToPlayer()->SetViewpoint(this, false);
                charmer->ToPlayer()->SetClientControl(this, 0);
                charmer->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                if (GetTypeId() == TYPEID_PLAYER)
                    ToPlayer()->SetMover(this);
                break;
            case CHARM_TYPE_CHARM:
                if (GetTypeId() == TYPEID_UNIT && charmer->GetClass() == CLASS_WARLOCK)
                {
                    CreatureTemplate const* cinfo = ToCreature()->GetCreatureTemplate();
                    if (cinfo && cinfo->type == CREATURE_TYPE_DEMON)
                    {
                        SetClass(uint8(cinfo->unit_class));
                        if (GetCharmInfo())
                            GetCharmInfo()->SetPetNumber(0, true);
                        else
                            TC_LOG_ERROR("entities.unit", "Aura::HandleModCharm: target=" UI64FMTD " with typeid=%d has a charm aura but no charm info!", GetGUID().GetRawValue(), GetTypeId());
                    }
                }
                break;
            default:
            case CHARM_TYPE_CONVERT:
                break;
        }
    }

    // a guardian should always have charminfo
    if (charmer->GetTypeId() == TYPEID_PLAYER && this != charmer->GetFirstControlled())
        charmer->ToPlayer()->SendRemoveControlBar();
    else if (GetTypeId() == TYPEID_PLAYER || (GetTypeId() == TYPEID_UNIT && !ToCreature()->IsGuardian()))
        DeleteCharmInfo();

    if (GetTypeId() == TYPEID_UNIT)
        TC_LOG_DEBUG("crash", "Unit::RemoveCharmedBy2, GUID: " UI64FMTD ", entry: %u, charmer: " UI64FMTD ", type: %u", GetGUID().GetRawValue(), GetEntry(), charmer->GetGUID().GetRawValue(), type);
}

void Unit::RestoreFaction()
{
    // SPELL_AURA_MOD_FACTION + charm/possess
    for (auto&& effect : GetAuraEffectsByType(SPELL_AURA_MOD_FACTION))
    {
        SetFaction(effect->GetMiscValue());
        return;
    }

    if (GetTypeId() == TYPEID_PLAYER)
    {
        Player* player = ToPlayer();
        if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_INTERFACTIONAL_TEAMS))
        {
            Battleground* bg = player->GetBattleground();
            if (bg && bg->IsSoloQueueMatch())
                player->SetFaction(player->GetBGTeam() == ALLIANCE ? 1 : 2);
            else
                player->setFactionForRace(GetRace());
        }
        else
            player->setFactionForRace(GetRace());
    }
    else
    {
        if (HasUnitTypeMask(UNIT_MASK_MINION))
        {
            if (Unit* owner = GetOwner())
            {
                SetFaction(owner->GetFaction());
                return;
            }
        }

        if (CreatureTemplate const* cinfo = ToCreature()->GetCreatureTemplate())  // normal creature
        {
            FactionTemplateEntry const* faction = GetFactionTemplateEntry();
            SetFaction((faction && faction->friendlyMask & 0x004) ? cinfo->faction : cinfo->faction);
        }
    }
}

Unit* Unit::GetRedirectThreatTarget()
{
    return _redirectThreatInfo.TargetGUID ? ObjectAccessor::GetUnit(*this, _redirectThreatInfo.TargetGUID) : nullptr;
}

bool Unit::CreateVehicleKit(uint32 id, uint32 creatureEntry, bool loading /*= false*/)
{
    VehicleEntry const* vehInfo = sVehicleStore.LookupEntry(id);
    if (!vehInfo)
        return false;

    if (m_vehicleKit && HasAuraType(SPELL_AURA_SET_VEHICLE_ID))
        RemoveAurasByType(SPELL_AURA_SET_VEHICLE_ID);

    // Vehicle kit could be reinstalled from CreatureTemplate in SPELL_AURA_SET_VEHICLE_ID handler
    RemoveVehicleKit();

    m_vehicleKit.reset(new Vehicle{ this, vehInfo, creatureEntry });
    m_updateFlag |= UPDATEFLAG_VEHICLE;
    m_unitTypeMask |= UNIT_MASK_VEHICLE;

    if (!loading)
        SendSetVehicleRecId(id);

    return true;
}

class VehicleDeletionEvent : public BasicEvent
{
public:
    VehicleDeletionEvent(const std::shared_ptr<Vehicle>& vehicleKit) : m_vehicleKit(vehicleKit) {}

    bool Execute(uint64, uint32) override
    {
        // Absolutely uselss code. Only purpose is not to confuse someone (or even me) with an empty lambda.
        // shared_ptr will be destroyed in destructor anyway (or in destructor of anonymous class in case of the lambda).
        // Also probable log here
        m_vehicleKit.reset();
        return true;
    }
private:
    std::shared_ptr<Vehicle> m_vehicleKit;
};

void Unit::RemoveVehicleKit(bool remove /*= false*/)
{
    if (!m_vehicleKit)
        return;

    if (!remove)
        SendSetVehicleRecId(0);

    m_vehicleKit->Uninstall();

    // We need this becasue we must keep Vehicle object during Vehicle::RemovePassanger.
    // And yes, RemoveVehicleKit may be called during Vehicle::RemovePassanger due to the AI scripts or OnRemove of the aura script.
    // And it's basically okay, it's not usable after m_vehicleKit->Uninstall() (or should be)
    m_Events.Schedule(1, new VehicleDeletionEvent{ m_vehicleKit });

    m_vehicleKit.reset();

    m_updateFlag &= ~UPDATEFLAG_VEHICLE;
    m_unitTypeMask &= ~UNIT_MASK_VEHICLE;
    RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_PLAYER_VEHICLE);
}

bool Unit::IsOnVehicle(const Unit* vehicle) const
{
    return m_vehicle && m_vehicle == vehicle->GetVehicleKit();
}

Unit* Unit::GetVehicleBase() const
{
    return m_vehicle ? m_vehicle->GetBase() : NULL;
}

Creature* Unit::GetVehicleCreatureBase() const
{
    if (Unit* veh = GetVehicleBase())
        if (Creature* c = veh->ToCreature())
            return c;

    return NULL;
}

ObjectGuid Unit::GetTransGUID() const
{
    if (GetVehicle())
        return GetVehicleBase()->GetGUID();
    if (GetTransport())
        return GetTransport()->GetGUID();

    return ObjectGuid::Empty;
}

TransportBase* Unit::GetDirectTransport() const
{
    if (Vehicle* veh = GetVehicle())
        return veh;
    return GetTransport();
}

bool Unit::IsInPartyWith(Unit const* unit) const
{
    if (this == unit)
        return true;

    const Unit* u1 = GetCharmerOrOwnerOrSelf();
    const Unit* u2 = unit->GetCharmerOrOwnerOrSelf();
    if (u1 == u2)
        return true;

    if (u1->GetTypeId() == TYPEID_PLAYER && u2->GetTypeId() == TYPEID_PLAYER)
        return u1->ToPlayer()->IsInSameGroupWith(u2->ToPlayer());
    else if ((u2->GetTypeId() == TYPEID_PLAYER && u1->GetTypeId() == TYPEID_UNIT && u1->ToCreature()->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_PARTY_MEMBER) ||
             (u1->GetTypeId() == TYPEID_PLAYER && u2->GetTypeId() == TYPEID_UNIT && u2->ToCreature()->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_PARTY_MEMBER))
             return true;
    else
        return false;
}

bool Unit::IsInRaidWith(Unit const* unit, bool ignoreCharmer) const
{
    if (this == unit)
        return true;

    const Unit* u1 = GetCharmerOrOwnerOrSelf();
    const Unit* u2 = unit->GetCharmerOrOwnerOrSelf();

    if (ignoreCharmer && GetTypeId() == TYPEID_PLAYER)
        u1 = this;
    if (ignoreCharmer && unit->GetTypeId() == TYPEID_PLAYER)
        u2 = unit;

    if (u1 == u2)
        return true;

    if (u1->GetTypeId() == TYPEID_PLAYER && u2->GetTypeId() == TYPEID_PLAYER)
        return u1->ToPlayer()->IsInSameRaidWith(u2->ToPlayer());
    else if ((u2->GetTypeId() == TYPEID_PLAYER && u1->GetTypeId() == TYPEID_UNIT && u1->ToCreature()->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_PARTY_MEMBER) ||
             (u1->GetTypeId() == TYPEID_PLAYER && u2->GetTypeId() == TYPEID_UNIT && u2->ToCreature()->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_PARTY_MEMBER))
             return true;
    else
        return false;
}

void Unit::GetPartyMembers(std::list<Unit*> &TagUnitMap)
{
    Unit* owner = GetCharmerOrOwnerOrSelf();
    Group* group = NULL;
    if (owner->GetTypeId() == TYPEID_PLAYER)
        group = owner->ToPlayer()->GetGroup();

    if (group)
    {
        uint8 subgroup = owner->ToPlayer()->GetSubGroup();

        for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* Target = itr->GetSource();

            // IsHostileTo check duel and controlled by enemy
            if (Target && Target->GetSubGroup() == subgroup && !IsHostileTo(Target))
            {
                if (Target->IsAlive() && IsInMap(Target))
                    TagUnitMap.push_back(Target);

                if (Guardian* pet = Target->GetGuardianPet())
                    if (pet->IsAlive() && IsInMap(Target))
                        TagUnitMap.push_back(pet);
            }
        }
    }
    else
    {
        if (owner->IsAlive() && (owner == this || IsInMap(owner)))
            TagUnitMap.push_back(owner);
        if (Guardian* pet = owner->GetGuardianPet())
            if (pet->IsAlive() && (pet == this || IsInMap(pet)))
                TagUnitMap.push_back(pet);
    }
}

bool Unit::IsContestedGuard() const
{
    if (FactionTemplateEntry const* entry = GetFactionTemplateEntry())
        return entry->IsContestedGuardFaction();

    return false;
}

void Unit::SetPvP(bool state)
{
    if (state)
        SetByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, 1, UNIT_BYTE2_FLAG_PVP);
    else
        RemoveByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, 1, UNIT_BYTE2_FLAG_PVP);
}

Aura* Unit::AddAura(uint32 spellId, Unit* target)
{
    if (!target)
        return nullptr;

    Difficulty difficulty = target->IsInWorld() ? target->GetMap()->GetDifficulty() : REGULAR_DIFFICULTY;
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId, difficulty);
    if (!spellInfo)
        return nullptr;

    if (!target->IsAlive() && target->getDeathState() != JUST_RESPAWNED && !(spellInfo->Attributes & SPELL_ATTR0_PASSIVE) && !(spellInfo->AttributesEx2 & SPELL_ATTR2_CAN_TARGET_DEAD))
        return nullptr;

    return AddAura(spellInfo, MAX_EFFECT_MASK, target);
}

Aura* Unit::AddAura(SpellInfo const* spellInfo, uint32 effMask, Unit* target)
{
    if (!spellInfo)
        return NULL;

    if (target->IsImmunedToSpell(spellInfo, effMask))
        return NULL;

    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (!(effMask & (1 << i)))
            continue;

        if (target->IsImmunedToSpellEffect(spellInfo, i))
            effMask &= ~(1 << i);
    }

    if (Aura* aura = Aura::TryRefreshStackOrCreate(spellInfo, effMask, target, this))
    {
        aura->ApplyForTargets();
        return aura;
    }
    return NULL;
}

Aura* Unit::AddAuraForTarget(Aura* aura, Unit* target)
{
    if (!target)
        return nullptr;

    if (Aura* newAura = AddAura(aura->GetSpellInfo(), aura->GetEffectMask(), target))
    {
        newAura->SetMaxDuration(aura->GetDuration());
        newAura->SetDuration(aura->GetDuration());
        for (int i = 0; i < MAX_SPELL_EFFECTS; ++i)
            if (aura->GetEffectMask() & (1 << i) && newAura->GetEffectMask() & (1 << i))
                newAura->GetEffect(i)->SetAmount(aura->GetEffect(i)->GetAmount());

        return newAura;
    }
    return nullptr;
}

void Unit::SetAuraStack(uint32 spellId, Unit* target, uint32 stack)
{
    Aura* aura = target->GetAura(spellId, GetGUID());
    if (!aura)
        aura = AddAura(spellId, target);
    if (aura && stack)
        aura->SetStackAmount(stack);
}

void Unit::SendPlaySpellVisual(uint32 spellVisualId, ObjectGuid target, float x, float y, float z, float speed, bool hasDest, uint16 missReason, uint16 reflectStatus)
{
    ObjectGuid targetGuid = target;
    ObjectGuid sourceGuid = GetGUID();

    WorldPacket data(SMSG_PLAY_SPELL_VISUAL, 4 + 4 + 4 + 8);

    data.WriteBit(targetGuid[4]);
    data.WriteBit(sourceGuid[6]);
    data.WriteBit(sourceGuid[4]);
    data.WriteBit(sourceGuid[7]);
    data.WriteBit(targetGuid[6]);
    data.WriteBit(sourceGuid[2]);
    data.WriteBit(sourceGuid[0]);
    data.WriteBit(targetGuid[2]);
    data.WriteBit(hasDest);
    data.WriteBit(targetGuid[7]);
    data.WriteBit(sourceGuid[3]);
    data.WriteBit(sourceGuid[1]);
    data.WriteBit(targetGuid[0]);
    data.WriteBit(targetGuid[1]);
    data.WriteBit(sourceGuid[5]);
    data.WriteBit(targetGuid[5]);
    data.WriteBit(targetGuid[3]);

    data << float(z);
    data.WriteByteSeq(targetGuid[2]);
    data.WriteByteSeq(targetGuid[6]);
    data.WriteByteSeq(targetGuid[5]);
    data.WriteByteSeq(sourceGuid[2]);
    data.WriteByteSeq(targetGuid[1]);
    data << float(x);
    data.WriteByteSeq(targetGuid[3]);
    data << uint16(reflectStatus);
    data.WriteByteSeq(sourceGuid[4]);
    data.WriteByteSeq(sourceGuid[7]);
    data << float(speed);
    data << float(y);
    data.WriteByteSeq(targetGuid[4]);
    data.WriteByteSeq(sourceGuid[5]);
    data << uint32(spellVisualId);
    data.WriteByteSeq(sourceGuid[1]);
    data.WriteByteSeq(targetGuid[7]);
    data << uint16(missReason);
    data.WriteByteSeq(sourceGuid[0]);
    data.WriteByteSeq(sourceGuid[6]);
    data.WriteByteSeq(targetGuid[0]);
    data.WriteByteSeq(sourceGuid[3]);

    SendMessageToSet(&data, true);
}

void Unit::SendPlaySpellVisualKit(uint32 SpellVisualId, uint32 Duration, int32 Type)
{
    ObjectGuid UnitGuid = GetGUID();

    WorldPacket data(SMSG_PLAY_SPELL_VISUAL_KIT, 4 + 4 + 4 + 8);

    data.WriteBit(UnitGuid [4]);
    data.WriteBit(UnitGuid [2]);
    data.WriteBit(UnitGuid [6]);
    data.WriteBit(UnitGuid [5]);
    data.WriteBit(UnitGuid [1]);
    data.WriteBit(UnitGuid [3]);
    data.WriteBit(UnitGuid [0]);
    data.WriteBit(UnitGuid [7]);

    data.WriteByteSeq(UnitGuid [5]);
    data.WriteByteSeq(UnitGuid [7]);
    data << uint32(Type);
    data.WriteByteSeq(UnitGuid [1]);
    data.WriteByteSeq(UnitGuid [0]);
    data.WriteByteSeq(UnitGuid [6]);
    data << uint32(Duration);
    data.WriteByteSeq(UnitGuid [4]);
    data.WriteByteSeq(UnitGuid [2]);
    data.WriteByteSeq(UnitGuid [3]);
    data << uint32(SpellVisualId);

    SendMessageToSet(&data, true);
}

float Unit::GetResilienceReduction() const
{
    float percent = GetCombatRatingReduction(CR_RESILIENCE_PLAYER_DAMAGE_TAKEN) * 0.19375; // FIXME: diminishing

    percent += -GetFloatValue(PLAYER_FIELD_MOD_RESILIENCE_PERCENT);
    RoundToInterval(percent, 0.0f, 100.0f);

    return (100.0f - percent) / 100.0f;
}

void Unit::ApplyResilience(Unit const* victim, int32* damage) const
{
    // player mounted on multi-passenger mount is also classified as vehicle
    if ((IsVehicle() && GetTypeId() != TYPEID_PLAYER && !IsPet()) ||
        (victim->IsVehicle() && victim->GetTypeId() != TYPEID_PLAYER && !victim->IsPet()))
        return;

    if (GetMap()->IsDungeon())
        return;

    // Don't consider resilience if not in PvP - player or pet
    if (!GetCharmerOrOwnerPlayerOrPlayerItself())
        return;

    Unit const* target = NULL;
    if (victim->GetTypeId() == TYPEID_PLAYER)
        target = victim;
    else if (victim->GetTypeId() == TYPEID_UNIT && victim->GetOwner() && victim->GetOwner()->GetTypeId() == TYPEID_PLAYER)
        target = victim->GetOwner();

    if (!target)
        return;

    *damage *= target->GetResilienceReduction();
}

void Unit::ApplyPvPHealMods(Unit const* target, uint32& damage, SpellInfo const* spellInfo)
{
    if (spellInfo->HasAttribute(SPELL_ATTR0_CU_IGNORE_PVP_POWER))
        return;

    if (GetMap()->IsDungeon())
        return;

    Player* thisPlayer = GetAffectingPlayer();
    if (!thisPlayer)
        return;

    Player* targetPlayer = target->GetAffectingPlayer();
    if (!targetPlayer)
        return;

    float mod = 1.0f;

    AddPct(mod, thisPlayer->GetFloatValue(PLAYER_FIELD_PVP_POWER_HEALING));

    // Dampening
    if (AuraEffect* aurEff = target->GetAuraEffect(SPELL_DAMPENING, EFFECT_0))
        AddPct(mod, -aurEff->GetAmount());

    damage *= mod;
}

// Melee based spells can be miss, parry or dodge on this step
// Crit or block - determined on damage calculation phase! (and can be both in some time)
float Unit::MeleeSpellMissChance(const Unit* victim, WeaponAttackType attType, uint32 spellId) const
{
    //calculate miss chance
    float missChance = victim->GetUnitMissChance(attType) + 1.5 * GetLevelDifferenceForPenalty(this, victim);

    if (!spellId && HasOffhandWeapon() && GetTypeId() == TYPEID_PLAYER)
        missChance += 19;

    // Calculate hit chance
    float hitChance = 100.0f;

    // Spellmod from SPELLMOD_RESIST_MISS_CHANCE
    if (spellId)
    {
        if (Player* modOwner = GetSpellModOwner())
            modOwner->ApplySpellMod(spellId, SPELLMOD_RESIST_MISS_CHANCE, hitChance);
    }

    missChance += 100.0f - hitChance;

    Player* statOwner = nullptr;
    if (HasUnitTypeMask(UNIT_MASK_MINION))
    {
        Minion const* minion = static_cast<Minion const*>(this);
        if (minion->GetOwner())
            statOwner = minion->GetOwner()->ToPlayer();
    }

    if (statOwner)
        missChance -= GetExpertiseOrHitChanceFormOwner(statOwner);
    else if (attType == RANGED_ATTACK)
        missChance -= m_modRangedHitChance;
    else
        missChance -= m_modMeleeHitChance;

    if (missChance < 0.0f)
        return 0.0f;
    return missChance;
}

void Unit::SetPhaseMask(uint32 newPhaseMask, bool update)
{
    if (newPhaseMask == GetPhaseMask())
        return;

    if (IsInWorld())
    {
        RemoveBoundAuras(newPhaseMask, false);             // we can lost access to caster or target

        // modify hostile references for new phasemask, some special cases deal with hostile references themselves
        if (GetTypeId() == TYPEID_UNIT || (!ToPlayer()->IsGameMaster() && !ToPlayer()->GetSession()->PlayerLogout()))
        {
            HostileRefManager& refManager = getHostileRefManager();
            HostileReference* ref = refManager.getFirst();

            while (ref)
            {
                if (Unit* unit = ref->GetSource()->GetOwner())
                    if (Creature* creature = unit->ToCreature())
                        refManager.setOnlineOfflineState(creature, creature->InSamePhase(newPhaseMask));

                ref = ref->next();
            }

            // modify threat lists for new phasemask
            if (GetTypeId() != TYPEID_PLAYER)
            {
                std::list<HostileReference*> threatList = getThreatManager().getThreatList();
                std::list<HostileReference*> offlineThreatList = getThreatManager().getOfflineThreatList();

                // merge expects sorted lists
                threatList.sort();
                offlineThreatList.sort();
                threatList.merge(offlineThreatList);

                for (std::list<HostileReference*>::const_iterator itr = threatList.begin(); itr != threatList.end(); ++itr)
                    if (Unit* unit = (*itr)->getTarget())
                        unit->getHostileRefManager().setOnlineOfflineState(ToCreature(), unit->InSamePhase(newPhaseMask));
            }
        }
    }

    WorldObject::SetPhaseMask(newPhaseMask, update);

    // Phase pets and summons
    if (IsInWorld())
    {
         for (ControlList::const_iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
            if ((*itr)->GetTypeId() == TYPEID_UNIT)
                (*itr)->SetPhaseMask(newPhaseMask, true);

        for (uint8 i = 0; i < SUMMON_SLOT_MAX; ++i)
            if (m_SummonSlot [i])
                if (Creature* summon = GetMap()->GetCreature(m_SummonSlot [i]))
                    summon->SetPhaseMask(newPhaseMask, true);       
    }

    // Update visibility after phasing pets and summons so they wont despawn
    if (update)
        UpdateObjectVisibility();            
}

void Unit::ClearPhases(bool update)
{
    WorldObject::ClearPhases(update);
}

bool Unit::SetPhased(uint32 id, bool update, bool apply)
{
    bool res = WorldObject::SetPhased(id, update, apply);

    if (!IsInWorld())
        return res;

    if (GetTypeId() == TYPEID_UNIT || (!ToPlayer()->IsGameMaster() && !ToPlayer()->GetSession()->PlayerLogout()))
    {
        HostileRefManager& refManager = getHostileRefManager();
        HostileReference* ref = refManager.getFirst();

        while (ref)
        {
            if (Unit* unit = ref->GetSource()->GetOwner())
                if (Creature* creature = unit->ToCreature())
                    refManager.setOnlineOfflineState(creature, creature->IsPhased(this));

            ref = ref->next();
        }

        // modify threat lists for new phasemask
        if (GetTypeId() != TYPEID_PLAYER)
        {
            std::list<HostileReference*> threatList = getThreatManager().getThreatList();
            std::list<HostileReference*> offlineThreatList = getThreatManager().getOfflineThreatList();

            // merge expects sorted lists
            threatList.sort();
            offlineThreatList.sort();
            threatList.merge(offlineThreatList);

            for (std::list<HostileReference*>::const_iterator itr = threatList.begin(); itr != threatList.end(); ++itr)
                if (Unit* unit = (*itr)->getTarget())
                    unit->getHostileRefManager().setOnlineOfflineState(ToCreature(), unit->IsPhased(this));
        }
    }

    for (ControlList::const_iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
        if ((*itr)->GetTypeId() == TYPEID_UNIT)
            (*itr)->SetPhased(id, true, apply);

    for (uint8 i = 0; i < SUMMON_SLOT_MAX; ++i)
        if (m_SummonSlot[i])
            if (Creature* summon = GetMap()->GetCreature(m_SummonSlot[i]))
                summon->SetPhased(id, true, apply);

    RemoveBoundAuras(0, true);
    return res;
}

class Unit::AINotifyTask : public BasicEvent
{
    Unit& m_owner;
public:
    explicit AINotifyTask(Unit * me) : m_owner(*me) {
        m_owner.m_VisibilityUpdScheduled = true;
    }

    ~AINotifyTask() {
        m_owner.m_VisibilityUpdScheduled = false;
    }

    virtual bool Execute(uint64 , uint32) 
    {
        Trinity::AIRelocationNotifier notifier(m_owner);
        m_owner.VisitNearbyObject(m_owner.GetVisibilityRange(), notifier);
        return true;
    }

    static void ScheduleAINotify(Unit* me)
    {
        if (!me->m_VisibilityUpdScheduled)
            me->m_Events.AddEvent(new AINotifyTask(me), me->m_Events.CalculateTime(World::Visibility_AINotifyDelay));
    }
};

class Unit::VisibilityUpdateTask : public BasicEvent
{
    Unit& m_owner;
public:
    explicit VisibilityUpdateTask(Unit * me) : m_owner(*me) {}

    virtual bool Execute(uint64 , uint32) 
    {
        UpdateVisibility(&m_owner);
        return true;
    }

    static void UpdateVisibility(Unit* me)
    {
        if (!me->m_sharedVision.empty())
            for (SharedVisionList::const_iterator it = me->m_sharedVision.begin();it!= me->m_sharedVision.end();)
            {
                Player * tmp = *it;
                ++it;
                tmp->UpdateVisibilityForPlayer();
            }
        if (me->isType(TYPEMASK_PLAYER))
            ((Player*)me)->UpdateVisibilityForPlayer();
        me->WorldObject::UpdateObjectVisibility(true);
    }
};

void Unit::OnRelocated()
{
    if (!m_lastVisibilityUpdPos.IsInDist(this, World::Visibility_RelocationLowerLimit)) {
        m_lastVisibilityUpdPos = *this;
        m_Events.AddEvent(new VisibilityUpdateTask(this), m_Events.CalculateTime(1));
    }
    AINotifyTask::ScheduleAINotify(this);
}

void Unit::UpdateObjectVisibility(bool forced)
{
    if (forced)
        VisibilityUpdateTask::UpdateVisibility(this);
    else
        m_Events.AddEvent(new VisibilityUpdateTask(this), m_Events.CalculateTime(1));
    AINotifyTask::ScheduleAINotify(this);
}

void Unit::SendMoveKnockBack(Player* player, float speedXY, float speedZ, float vcos, float vsin)
{
    ObjectGuid guid = GetGUID();
    WorldPacket data(SMSG_MOVE_KNOCK_BACK, (1 + 8 + 4 + 4 + 4 + 4 + 4));

    data << float(speedXY);
    data << float(vsin);
    data << float(speedZ);
    data << uint32(0);
    data << float(vcos);


    data.WriteBit(guid [2]);
    data.WriteBit(guid [0]);
    data.WriteBit(guid [7]);
    data.WriteBit(guid [1]);
    data.WriteBit(guid [4]);
    data.WriteBit(guid [6]);
    data.WriteBit(guid [5]);
    data.WriteBit(guid [3]);

    data.WriteByteSeq(guid [6]);
    data.WriteByteSeq(guid [0]);
    data.WriteByteSeq(guid [7]);
    data.WriteByteSeq(guid [5]);
    data.WriteByteSeq(guid [4]);
    data.WriteByteSeq(guid [3]);
    data.WriteByteSeq(guid [1]);
    data.WriteByteSeq(guid [2]);

    player->GetSession()->SendPacket(&data);

    if (player->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) || player->HasAuraType(SPELL_AURA_FLY))
        Movement::PacketSender(this, SMSG_SPLINE_MOVE_SET_FLYING, SMSG_MOVE_SET_CAN_FLY).Send();
}

void Unit::KnockbackFrom(float x, float y, float speedXY, float speedZ)
{
    Player* player = ToPlayer();
    if (!player)
    {
        if (Unit* charmer = GetCharmer())
        {
            player = charmer->ToPlayer();
            if (player && player->m_mover != this)
                player = NULL;
        }
    }

    if (!player)
        GetMotionMaster()->MoveKnockbackFrom(x, y, speedXY, speedZ);
    else
    {
        if (!movespline->Finalized())
            StopMoving();

        float vcos, vsin;
        GetSinCos(x, y, vsin, vcos);
        SendMoveKnockBack(player, speedXY, -speedZ, vcos, vsin);
    }
}

float Unit::GetCombatRatingReduction(CombatRating cr) const
{
    if (Player const* player = ToPlayer())
        return player->GetRatingBonusValue(cr);
    // Player's pet get resilience from owner
    else if (IsPet() && GetOwner())
        if (Player* owner = GetOwner()->ToPlayer())
            return owner->GetRatingBonusValue(cr);

    return 0.0f;
}

uint32 Unit::GetModelForForm(ShapeshiftForm form) const
{
    if (GetTypeId() == TYPEID_PLAYER)
    {
        switch (form)
        {
            case FORM_CAT:
            {
                // Hack for Druid of the Flame, Fandral's Flamescythe
                if (HasAura(99245) || HasAura(138927))
                    return 38150;
                bool kingOfTheJungle = HasAura(102543);
                // Based on Hair color
                if (GetRace() == RACE_NIGHTELF)
                {
                    uint8 hairColor = GetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, 3);
                    switch (hairColor)
                    {
                        case 7: // Violet
                        case 8:
                            return kingOfTheJungle ? 43764 : 29405;
                        case 3: // Light Blue
                            return kingOfTheJungle ? 43763 : 29406;
                        case 0: // Green
                        case 1: // Light Green
                        case 2: // Dark Green
                            return kingOfTheJungle ? 43762 : 29407;
                        case 4: // White
                            return kingOfTheJungle ? 43765 : 29408;
                        default: // original - Dark Blue
                            return kingOfTheJungle ? 43761 : 892;
                    }
                }
                else if (GetRace() == RACE_TROLL)
                {
                    uint8 hairColor = GetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, 3);
                    switch (hairColor)
                    {
                        case 0: // Red
                        case 1:
                            return kingOfTheJungle ? 43776 : 33668;
                        case 2: // Yellow
                        case 3:
                            return kingOfTheJungle ? 43778 : 33667;
                        case 4: // Blue
                        case 5:
                        case 6:
                            return kingOfTheJungle ? 43773 : 33666;
                        case 7: // Purple
                        case 10:
                            return kingOfTheJungle ? 43775 : 33665;
                        default: // original - white
                            return kingOfTheJungle ? 43777 : 33669;
                    }
                }
                else if (GetRace() == RACE_WORGEN)
                {
                    // Based on Skin color
                    uint8 skinColor = GetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, 0);
                    // Male
                    if (GetGender() == GENDER_MALE)
                    {
                        switch (skinColor)
                        {
                            case 1: // Brown
                                return kingOfTheJungle ? 43781 : 33662;
                            case 2: // Black
                            case 7:
                                return kingOfTheJungle ? 43780 : 33661;
                            case 4: // yellow
                                return kingOfTheJungle ? 43784 : 33664;
                            case 3: // White
                            case 5:
                                return kingOfTheJungle ? 43785 : 33663;
                            default: // original - Gray
                                return kingOfTheJungle ? 43782 : 33660;
                        }
                    }
                    // Female
                    else
                    {
                        switch (skinColor)
                        {
                            case 5: // Brown
                            case 6:
                                return kingOfTheJungle ? 43781 : 33662;
                            case 7: // Black
                            case 8:
                                return kingOfTheJungle ? 43780 : 33661;
                            case 3: // yellow
                            case 4:
                                return kingOfTheJungle ? 43784 : 33664;
                            case 2: // White
                                return kingOfTheJungle ? 43785 : 33663;
                            default: // original - Gray
                                return kingOfTheJungle ? 43782 : 33660;
                        }
                    }
                }
                // Based on Skin color
                else if (GetRace() == RACE_TAUREN)
                {
                    uint8 skinColor = GetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, 0);
                    // Male
                    if (GetGender() == GENDER_MALE)
                    {
                        switch (skinColor)
                        {
                            case 12: // White
                            case 13:
                            case 14:
                            case 18: // Completly White
                                return kingOfTheJungle ? 43769 : 29409;
                            case 9: // Light Brown
                            case 10:
                            case 11:
                                return kingOfTheJungle ? 43770 : 29410;
                            case 6: // Brown
                            case 7:
                            case 8:
                                return kingOfTheJungle ? 43768 : 29411;
                            case 0: // Dark
                            case 1:
                            case 2:
                            case 3: // Dark Grey
                            case 4:
                            case 5:
                                return kingOfTheJungle ? 43766 : 29412;
                            default: // original - Grey
                                return kingOfTheJungle ? 43767 : 8571;
                        }
                    }
                    // Female
                    else
                    {
                        switch (skinColor)
                        {
                            case 10: // White
                                return kingOfTheJungle ? 43769 : 29409;
                            case 6: // Light Brown
                            case 7:
                                return kingOfTheJungle ? 43770 : 29410;
                            case 4: // Brown
                            case 5:
                                return kingOfTheJungle ? 43768 : 29411;
                            case 0: // Dark
                            case 1:
                            case 2:
                            case 3:
                                return kingOfTheJungle ? 43766 : 29412;
                            default: // original - Grey
                                return kingOfTheJungle ? 43767 : 8571;
                        }
                    }
                }
                else if (Player::TeamForRace(GetRace()) == ALLIANCE)
                    return 892;
                else
                    return 8571;
            }
            case FORM_BEAR:
            {
                // Based on Hair color
                bool ursocsSon = HasAura(102558);
                if (GetRace() == RACE_NIGHTELF)
                {
                    uint8 hairColor = GetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, 3);
                    switch (hairColor)
                    {
                        case 0: // Green
                        case 1: // Light Green
                        case 2: // Dark Green
                            return ursocsSon ? 43759 : 29413;
                        case 6: // Black
                            return ursocsSon ? 43756 : 29414;
                        case 4: // White
                            return ursocsSon ? 43760 : 29416;
                        case 3: // Light Blue
                            return ursocsSon ? 43757 : 29415;
                        default: // original - Violet
                            return ursocsSon ? 43758 : 2281;
                    }
                }
                else if (GetRace() == RACE_TROLL)
                {
                    uint8 hairColor = GetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, 3);
                    switch (hairColor)
                    {
                        case 0: // Red
                        case 1:
                            return ursocsSon ? 43748 : 33657;
                        case 2: // Yellow
                        case 3:
                            return ursocsSon ? 43750 : 33659;
                        case 7: // Purple
                        case 10:
                            return ursocsSon ? 43747 : 33656;
                        case 8: // White
                        case 9:
                        case 11:
                        case 12:
                            return ursocsSon ? 43749 : 33658;
                        default: // original - Blue
                            return ursocsSon ? 43746 : 33655;
                    }
                }
                else if (GetRace() == RACE_WORGEN)
                {
                    // Based on Skin color
                    uint8 skinColor = GetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, 0);
                    // Male
                    if (GetGender() == GENDER_MALE)
                    {
                        switch (skinColor)
                        {
                            case 1: // Brown
                                return ursocsSon ? 43752 : 33652;
                            case 2: // Black
                            case 7:
                                return ursocsSon ? 43751 : 33651;
                            case 4: // Yellow
                                return ursocsSon ? 43755 : 33654;
                            case 3: // White
                            case 5:
                                return ursocsSon ? 43754 : 33653;
                            default: // original - Gray
                                return ursocsSon ? 43753 : 33650;
                        }
                    }
                    // Female
                    else
                    {
                        switch (skinColor)
                        {
                            case 5: // Brown
                            case 6:
                                return ursocsSon ? 43752 : 33652;
                            case 7: // Black
                            case 8:
                                return ursocsSon ? 43751 : 33651;
                            case 3: // yellow
                            case 4:
                                return ursocsSon ? 43755 : 33654;
                            case 2: // White
                                return ursocsSon ? 43754 : 33653;
                            default: // original - Gray
                                return ursocsSon ? 43753 : 33650;
                        }
                    }
                }
                // Based on Skin color
                else if (GetRace() == RACE_TAUREN)
                {
                    uint8 skinColor = GetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, 0);
                    // Male
                    if (GetGender() == GENDER_MALE)
                    {
                        switch (skinColor)
                        {
                            case 0: // Dark (Black)
                            case 1:
                            case 2:
                                return ursocsSon ? 43741 : 29418;
                            case 3: // White
                            case 4:
                            case 5:
                            case 12:
                            case 13:
                            case 14:
                                return ursocsSon ? 43743 : 29419;
                            case 9: // Light Brown/Grey
                            case 10:
                            case 11:
                            case 15:
                            case 16:
                            case 17:
                                return ursocsSon ? 43745 : 29420;
                            case 18: // Completly White
                                return ursocsSon ? 43744 : 29421;
                            default: // original - Brown
                                return ursocsSon ? 43742 : 2289;
                        }
                    }
                    // Female
                    else
                    {
                        switch (skinColor)
                        {
                            case 0: // Dark (Black)
                            case 1:
                                return ursocsSon ? 43741 : 29418;
                            case 2: // White
                            case 3:
                                return ursocsSon ? 43743 : 29419;
                            case 6: // Light Brown/Grey
                            case 7:
                            case 8:
                            case 9:
                                return ursocsSon ? 43745 : 29420;
                            case 10: // Completly White
                                return ursocsSon ? 43744 : 29421;
                            default: // original - Brown
                                return ursocsSon ? 43742 : 2289;
                        }
                    }
                }
                else if (Player::TeamForRace(GetRace()) == ALLIANCE)
                    return 2281;
                else
                    return 2289;
            }
            case FORM_FLIGHT:
                if (Player::TeamForRace(GetRace()) == ALLIANCE)
                    return 20857;
                else
                {
                    if (GetRace() == RACE_TROLL)
                        return 37728;
                    else
                        return 20872;
                }
            case FORM_FLIGHT_EPIC:
                if (Player::TeamForRace(GetRace()) == HORDE)
                {
                    if (GetRace() == RACE_TROLL)
                        return 37730;
                    else if (GetRace() == RACE_TAUREN)
                        return 21244;
                }
                else if (Player::TeamForRace(GetRace()) == ALLIANCE)
                {
                    if (GetRace() == RACE_NIGHTELF)
                        return 21243;
                    else if (GetRace() == RACE_WORGEN)
                        return 37729;
                }
                return 21244;
            case FORM_TRAVEL:
            {
                if (HasAura(131113)) // Druid Form Gepard - Glyph
                    return 1043;

                switch (GetRace())
                {
                    case RACE_NIGHTELF:
                    case RACE_WORGEN:
                        return 40816;

                    case RACE_TROLL:
                    case RACE_TAUREN:
                        return 45339;
                }
                break;
            }
            case FORM_MOONKIN:
            {
                bool chosenOfElune = HasAura(102560);
                bool glyphOfStars = GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasGlyph(114301);
                if (Player::TeamForRace(GetRace()) == HORDE)
                {
                    if (GetRace() == RACE_TROLL)
                    {
                        if (chosenOfElune)
                            return 43789;
                        else if (!glyphOfStars)
                            return 37174;
                    }
                    else if (GetRace() == RACE_TAUREN)
                    {
                        if (chosenOfElune)
                            return 43786;
                        else if (!glyphOfStars)
                            return 15375;
                    }
                }
                else if (Player::TeamForRace(GetRace()) == ALLIANCE)
                {
                    if (GetRace() == RACE_NIGHTELF)
                    {
                        if (chosenOfElune)
                            return 43790;
                        else if (!glyphOfStars)
                            return 15374;
                    }
                    else if (GetRace() == RACE_WORGEN)
                    {
                        if (chosenOfElune)
                            return 43787;
                        else if (!glyphOfStars)
                            return 37173;
                    }
                }
                break;
            }
            case FORM_AQUA:
                if (HasAura(114333))    // Glyph of the Orca
                    return 4591;
                break;
            case FORM_GHOSTWOLF:
                if (HasAura(58135))     // Glyph of Spectral Wolf
                    return 30162;
                break;
            case FORM_METAMORPHOSIS:
                if (HasAura(137206))    // The Codex of Xerrath
                    return 48088;
                break;
            default:
                break;
        }
    }

    uint32 modelid = 0;
    SpellShapeshiftFormEntry const* formEntry = sSpellShapeshiftFormStore.LookupEntry(form);
    if (formEntry && formEntry->modelID_A)
    {
        // Take the alliance modelid as default
        if (GetTypeId() != TYPEID_PLAYER)
            return formEntry->modelID_A;
        else
        {
            if (Player::TeamForRace(GetRace()) == ALLIANCE)
                modelid = formEntry->modelID_A;
            else
                modelid = formEntry->modelID_H;

            // If the player is horde but there are no values for the horde modelid - take the alliance modelid
            if (!modelid && Player::TeamForRace(GetRace()) == HORDE)
                modelid = formEntry->modelID_A;
        }
    }

    return modelid;
}

uint32 Unit::GetModelForTotem(uint32 totemType) const
{
    switch (totemType)
    {
        case 3211:
        case 3403:
            totemType = SUMMON_TYPE_TOTEM_FIRE;
            break;
        case 3402:
            totemType = SUMMON_TYPE_TOTEM_WATER;
            break;
        case 3401:
        case 3406:
        case 3407:
        case 3405:
        case 3399:
            totemType = SUMMON_TYPE_TOTEM_AIR;
            break;
        case 3400:
        case 3404:
            totemType = SUMMON_TYPE_TOTEM_EARTH;
            break;
    }

    switch (GetRace())
    {
        case RACE_ORC:
        {
            switch (totemType)
            {
                case SUMMON_TYPE_TOTEM_FIRE:    // fire
                    return 30758;
                case SUMMON_TYPE_TOTEM_EARTH:   // earth
                    return 30757;
                case SUMMON_TYPE_TOTEM_WATER:   // water
                    return 30759;
                case SUMMON_TYPE_TOTEM_AIR:     // air
                    return 30756;
            }
            break;
        }
        case RACE_DWARF:
        {
            switch (totemType)
            {
                case SUMMON_TYPE_TOTEM_FIRE:    // fire
                    return 30754;
                case SUMMON_TYPE_TOTEM_EARTH:   // earth
                    return 30753;
                case SUMMON_TYPE_TOTEM_WATER:   // water
                    return 30755;
                case SUMMON_TYPE_TOTEM_AIR:     // air
                    return 30736;
            }
            break;
        }
        case RACE_TROLL:
        {
            switch (totemType)
            {
                case SUMMON_TYPE_TOTEM_FIRE:    // fire
                    return 30762;
                case SUMMON_TYPE_TOTEM_EARTH:   // earth
                    return 30761;
                case SUMMON_TYPE_TOTEM_WATER:   // water
                    return 30763;
                case SUMMON_TYPE_TOTEM_AIR:     // air
                    return 30760;
            }
            break;
        }
        case RACE_TAUREN:
        {
            switch (totemType)
            {
                case SUMMON_TYPE_TOTEM_FIRE:    // fire
                    return 4589;
                case SUMMON_TYPE_TOTEM_EARTH:   // earth
                    return 4588;
                case SUMMON_TYPE_TOTEM_WATER:   // water
                    return 4587;
                case SUMMON_TYPE_TOTEM_AIR:     // air
                    return 4590;
            }
            break;
        }
        case RACE_DRAENEI:
        {
            switch (totemType)
            {
                case SUMMON_TYPE_TOTEM_FIRE:    // fire
                    return 19074;
                case SUMMON_TYPE_TOTEM_EARTH:   // earth
                    return 19073;
                case SUMMON_TYPE_TOTEM_WATER:   // water
                    return 19075;
                case SUMMON_TYPE_TOTEM_AIR:     // air
                    return 19071;
            }
            break;
        }
        case RACE_GOBLIN:
        {
            switch (totemType)
            {
                case SUMMON_TYPE_TOTEM_FIRE:    // fire
                    return 30783;
                case SUMMON_TYPE_TOTEM_EARTH:   // earth
                    return 30782;
                case SUMMON_TYPE_TOTEM_WATER:   // water
                    return 30784;
                case SUMMON_TYPE_TOTEM_AIR:     // air
                    return 30781;
            }
            break;
        }
        case RACE_PANDAREN_NEUTRAL:
        case RACE_PANDAREN_ALLIANCE:
        case RACE_PANDAREN_HORDE:
        {
            switch (totemType)
            {
                case SUMMON_TYPE_TOTEM_FIRE:    // fire
                    return 41670;
                case SUMMON_TYPE_TOTEM_EARTH:   // earth
                    return 41669;
                case SUMMON_TYPE_TOTEM_WATER:   // water
                    return 41671;
                case SUMMON_TYPE_TOTEM_AIR:     // air
                    return 41668;
            }
            break;
        }
    }
    return 0;
}

void Unit::JumpTo(float speedXY, float speedZ, bool forward, Position* dest)
{
    float angle = forward ? 0 : M_PI;
    if (dest)
        angle += GetRelativeAngle(dest);

    if (GetTypeId() == TYPEID_UNIT)
        GetMotionMaster()->MoveJumpTo(angle, speedXY, speedZ);
    else
    {
        if (!movespline->Finalized())
            StopMoving();

        Player* player = ToPlayer();
        player->SetFallInformation(0, ToPlayer()->GetPositionZ());

        float vcos = std::cos(angle + GetOrientation());
        float vsin = std::sin(angle + GetOrientation());
        SendMoveKnockBack(ToPlayer(), speedXY, -speedZ, vcos, vsin);
    }
}

void Unit::JumpTo(WorldObject* obj, float speedZ)
{
    float x, y, z;
    obj->GetContactPoint(this, x, y, z);
    float speedXY = GetExactDist2d(x, y) * 10.0f / speedZ;
    GetMotionMaster()->MoveJump(x, y, z, speedXY, speedZ);
}

bool Unit::HandleSpellClick(Unit* clicker, int8 seatId)
{
    bool result = false;
    uint32 spellClickEntry = GetVehicleKit() ? GetVehicleKit()->GetCreatureEntry() : GetEntry();
    SpellClickInfoMapBounds clickPair = sObjectMgr->GetSpellClickInfoMapBounds(spellClickEntry);
    for (SpellClickInfoContainer::const_iterator itr = clickPair.first; itr != clickPair.second; ++itr)
    {
        //! First check simple relations from clicker to clickee
        if (!itr->second.IsFitToRequirements(clicker, this))
            continue;

        //! Check database conditions
        if (!sConditionMgr->IsObjectMeetingSpellClickConditions(spellClickEntry, itr->second.spellId, clicker, this))        
            continue;

        Unit* caster = (itr->second.castFlags & NPC_CLICK_CAST_CASTER_CLICKER) ? clicker : this;
        Unit* target = (itr->second.castFlags & NPC_CLICK_CAST_TARGET_CLICKER) ? clicker : this;
        ObjectGuid origCasterGUID = (itr->second.castFlags & NPC_CLICK_CAST_ORIG_CASTER_OWNER) ? GetOwnerGUID() : clicker->GetGUID();

        SpellInfo const* spellEntry = sSpellMgr->GetSpellInfo(itr->second.spellId);
        // if (!spellEntry) should be checked at npc_spellclick load

        if (seatId > -1)
        {
            uint8 i = 0;
            bool valid = false;
            while (i < MAX_SPELL_EFFECTS && !valid)
            {
                if (spellEntry->Effects [i].ApplyAuraName == SPELL_AURA_CONTROL_VEHICLE)
                {
                    valid = true;
                    break;
                }
                ++i;
            }

            if (!valid)
            {
                TC_LOG_ERROR("sql.sql", "Spell %u specified in npc_spellclick_spells is not a valid vehicle enter aura!", itr->second.spellId);
                continue;
            }

            if (IsInMap(caster))
                caster->CastCustomSpell(itr->second.spellId, SpellValueMod(SPELLVALUE_BASE_POINT0 + i), seatId + 1, target, GetVehicleKit() ? TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE : TRIGGERED_NONE, NULL, NULL, origCasterGUID);
            else    // This can happen during Player::_LoadAuras
            {
                int32 bp0 [MAX_SPELL_EFFECTS];
                for (uint32 j = 0; j < MAX_SPELL_EFFECTS; ++j)
                    bp0 [j] = spellEntry->Effects [j].BasePoints;

                bp0 [i] = seatId + 1;
                Aura::TryRefreshStackOrCreate(spellEntry, MAX_EFFECT_MASK, this, clicker, bp0, NULL, origCasterGUID);
            }
        }
        else
        {
            if (IsInMap(caster))
                caster->CastSpell(target, spellEntry, GetVehicleKit() ? TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE : TRIGGERED_NONE, NULL, NULL, origCasterGUID);
            else
                Aura::TryRefreshStackOrCreate(spellEntry, MAX_EFFECT_MASK, this, clicker, NULL, NULL, origCasterGUID);
        }

        result = true;
    }

    Creature* creature = ToCreature();
    if (creature && creature->IsAIEnabled)
        creature->AI()->OnSpellClick(clicker, result);

    if (result)
        clicker->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_USE);

    return result;
}

void Unit::EnterVehicle(Unit* base, int8 seatId, bool fullTriggered)
{
    CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, seatId + 1, base, fullTriggered ? TRIGGERED_FULL_MASK : TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE);
}

void Unit::_EnterVehicle(Vehicle* vehicle, int8 seatId, AuraApplication const* aurApp)
{
    // Must be called only from aura handler
    ASSERT(aurApp);

    if (!IsAlive() || GetVehicleKit() == vehicle || vehicle->GetBase()->IsOnVehicle(this))
        return;

    if (m_vehicle)
    {
        if (m_vehicle == vehicle)
        {
            if (seatId >= 0 && seatId != GetTransSeat())
                ChangeSeat(seatId);
            return;
        }
        else
            ExitVehicle();
    }

    if (aurApp && aurApp->GetRemoveMode())
        return;

    if (Player* player = ToPlayer())
    {
        if (vehicle->GetBase()->GetTypeId() == TYPEID_PLAYER && player->IsInCombat())
            return;

        // drop flag
        if (Battleground* bg = player->GetBattleground())
            bg->EventPlayerDroppedFlag(player);

        InterruptNonMeleeSpells(false);
        player->StopCastingCharm();
        player->StopCastingBindSight();
        player->SendOnCancelExpectedVehicleRideAura();
        Dismount();
        RemoveAurasByType(SPELL_AURA_MOUNTED);

        switch (vehicle->GetVehicleInfo()->m_ID)
        {
            case 533: // Bone Spike
            case 647: // Bone Spike
            case 648: // Bone Spike
                break;
            default:
                player->UnsummonPetTemporaryIfAny();
                break;
        }
    }

    ASSERT(!m_vehicle);
    vehicle->AddPassenger(this, seatId);
}

void Unit::ChangeSeat(int8 seatId, bool next)
{
    if (!m_vehicle)
        return;

    if (seatId < 0)
    {
        seatId = m_vehicle->GetNextEmptySeat(GetTransSeat(), next);
        if (seatId < 0)
            return;
    }
    else if (seatId == GetTransSeat() || !m_vehicle->HasEmptySeat(seatId))
        return;

    auto vehicle = m_vehicle;
    vehicle->RemovePassenger(this); // SetVehicle(nulltpr)
    if (!vehicle->AddPassenger(this, seatId))
        ASSERT(false);
}

void Unit::ExitVehicle(Position const* /*exitPosition*/)
{
    //! This function can be called at upper level code to initialize an exit from the passenger's side.
    if (!m_vehicle)
        return;

    GetVehicleBase()->RemoveAurasByType(SPELL_AURA_CONTROL_VEHICLE, GetGUID());
    //! The following call would not even be executed successfully as the
    //! SPELL_AURA_CONTROL_VEHICLE unapply handler already calls _ExitVehicle without
    //! specifying an exitposition. The subsequent call below would return on if (!m_vehicle).
    /*_ExitVehicle(exitPosition);*/
    //! To do:
    //! We need to allow SPELL_AURA_CONTROL_VEHICLE unapply handlers in spellscripts
    //! to specify exit coordinates and either store those per passenger, or we need to
    //! init spline movement based on those coordinates in unapply handlers, and
    //! relocate exiting passengers based on Unit::moveSpline data. Either way,
    //! Coming Soon(TM)
}

void Unit::_ExitVehicle(Position const* exitPosition)
{
    /// It's possible m_vehicle is NULL, when this function is called indirectly from @VehicleJoinEvent::Abort.
    /// In that case it was not possible to add the passenger to the vehicle. The vehicle aura has already been removed
    /// from the target in the aforementioned function and we don't need to do anything else at this point.
    if (!m_vehicle)
        return;

    Position pos;
    if (!exitPosition)                           // Exit position not specified
        pos = m_vehicle->GetBase()->GetPosition(); // This should use passenger's current position, leaving it as it is now
    else
        pos = *exitPosition;

    // During removing passanger original vehicle can be deinstalled, make 
    std::shared_ptr<Vehicle> oldVehicle = m_vehicle->GetBase()->m_vehicleKit;

    Vehicle* vehicle = m_vehicle->RemovePassenger(this, &pos);

    Player* player = ToPlayer();

    // If the player is on mounted duel and exits the mount, he should immediatly lose the duel
    if (player && player->duel && player->duel->isMounted)
        player->DuelComplete(DUEL_FLED);

    SetControlled(false, UNIT_STATE_ROOT);      // SMSG_MOVE_FORCE_UNROOT, ~MOVEMENTFLAG_ROOT

    AddUnitState(UNIT_STATE_MOVE);

    if (player)
        player->SetFallInformation(0, GetPositionZ());

    float height = pos.GetPositionZ();

    Movement::MoveSplineInit init(this);

    // Creatures without inhabit type air should begin falling after exiting the vehicle
    if (GetTypeId() == TYPEID_UNIT && !CanFly() && height > GetMap()->GetWaterOrGroundLevel(GetPhaseMask(), pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), &height) + 0.1f)
        init.SetFall();

    init.MoveTo(pos.GetPositionX(), pos.GetPositionY(), height, false);
    init.SetFacing(GetOrientation());
    init.SetTransportExit();
    init.Launch();

    if (player)
        player->ResummonPetTemporaryUnSummonedIfAny();

    if (vehicle->GetBase()->HasUnitTypeMask(UNIT_MASK_MINION) && vehicle->GetBase()->GetTypeId() == TYPEID_UNIT)
        if (((Minion*) vehicle->GetBase())->GetOwner() == this)
            vehicle->GetBase()->ToCreature()->DespawnOrUnsummon(1);

    if (HasUnitTypeMask(UNIT_MASK_ACCESSORY))
    {
        // Vehicle just died, we die too
        if (vehicle->GetBase()->getDeathState() == JUST_DIED)
            setDeathState(JUST_DIED);
        // If for other reason we as minion are exiting the vehicle (ejected, master dismounted) - unsummon
        else
            ToTempSummon()->UnSummon(2000); // Approximation
    }
}

bool Unit::IsFalling() const
{
    return m_movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLING_FAR) || movespline->isFalling();
}

bool Unit::CanSwim() const
{
    // Mirror client behavior, if this method returns false then client will not use swimming animation and for players will apply gravity as if there was no water
    if (HasUnitFlag(UNIT_FLAG_CANNOT_SWIM))
        return false;
    if (HasUnitFlag(UNIT_FLAG_PLAYER_CONTROLLED)) // is player
        return true;
    if (HasUnitFlag2(UNIT_FLAG2_UNUSED_6))
        return false;
    if (HasUnitFlag(UNIT_FLAG_PET_IN_COMBAT))
        return true;
    return HasUnitFlag(UNIT_FLAG_RENAME | UNIT_FLAG_CAN_SWIM);
}

void Unit::NearTeleportTo(float x, float y, float z, float orientation, bool casting /*= false*/)
{
    DisableSpline();
    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->TeleportTo(GetMapId(), x, y, z, orientation, TELE_TO_NOT_LEAVE_TRANSPORT | TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET | (casting ? TELE_TO_SPELL : 0));
    else
    {
        Position pos = { x, y, z, orientation };
        SendTeleportPacket(pos);
        UpdatePosition(x, y, z, orientation, true);
        UpdateObjectVisibility();

        GetMotionMaster()->propagateSpeedChange(); // Fake speed change so that the motion gets recalculated to account for our teleportation
    }
}

void Unit::WriteMovementInfo(WorldPacket& data, Movement::ExtraMovementStatusElement* extras /*= NULL*/)
{
    MovementInfo const& mi = m_movementInfo;

    bool hasMountDisplayId = GetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID) != 0;
    bool hasMovementFlags = GetUnitMovementFlags() != 0;
    bool hasMovementFlags2 = GetExtraUnitMovementFlags() != 0;
    bool hasTimestamp = true;
    bool hasOrientation = !G3D::fuzzyEq(GetOrientation(), 0.0f);
    bool hasTransportData = GetTransGUID() != 0;
    bool hasSpline = IsSplineEnabled();

    bool hasTransportTime2 = hasTransportData && m_movementInfo.transport.time2 != 0;
    bool hasTransportTime3 = false;
    bool hasTransportVehicleId = hasTransportData && m_movementInfo.transport.time3 != 0;
    bool hasPitch = HasUnitMovementFlag(MovementFlags(MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) || HasExtraUnitMovementFlag(MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING);
    bool hasFallDirection = HasUnitMovementFlag(MOVEMENTFLAG_FALLING);
    bool hasFallData = hasFallDirection || m_movementInfo.jump.fallTime != 0;
    bool hasSplineElevation = HasUnitMovementFlag(MOVEMENTFLAG_SPLINE_ELEVATION);

    MovementStatusElements const* sequence = GetMovementStatusElementsSequence(data.GetOpcode());
    if (!sequence)
    {
        TC_LOG_ERROR("network", "Unit::WriteMovementInfo: No movement sequence found for opcode %s", GetOpcodeNameForLogging(static_cast<OpcodeClient>(data.GetOpcode())).c_str());
        return;
    }

    ObjectGuid guid = GetGUID();
    ObjectGuid tguid = hasTransportData ? GetTransGUID() : ObjectGuid::Empty;

    for (; *sequence != MSEEnd; ++sequence)
    {
        MovementStatusElements const& element = *sequence;

        switch (element)
        {
            case MSEHasGuidByte0:
            case MSEHasGuidByte1:
            case MSEHasGuidByte2:
            case MSEHasGuidByte3:
            case MSEHasGuidByte4:
            case MSEHasGuidByte5:
            case MSEHasGuidByte6:
            case MSEHasGuidByte7:
                data.WriteBit(guid [element - MSEHasGuidByte0]);
                break;
            case MSEHasTransportGuidByte0:
            case MSEHasTransportGuidByte1:
            case MSEHasTransportGuidByte2:
            case MSEHasTransportGuidByte3:
            case MSEHasTransportGuidByte4:
            case MSEHasTransportGuidByte5:
            case MSEHasTransportGuidByte6:
            case MSEHasTransportGuidByte7:
                if (hasTransportData)
                    data.WriteBit(tguid [element - MSEHasTransportGuidByte0]);
                break;
            case MSEGuidByte0:
            case MSEGuidByte1:
            case MSEGuidByte2:
            case MSEGuidByte3:
            case MSEGuidByte4:
            case MSEGuidByte5:
            case MSEGuidByte6:
            case MSEGuidByte7:
                data.WriteByteSeq(guid [element - MSEGuidByte0]);
                break;
            case MSETransportGuidByte0:
            case MSETransportGuidByte1:
            case MSETransportGuidByte2:
            case MSETransportGuidByte3:
            case MSETransportGuidByte4:
            case MSETransportGuidByte5:
            case MSETransportGuidByte6:
            case MSETransportGuidByte7:
                if (hasTransportData)
                    data.WriteByteSeq(tguid [element - MSETransportGuidByte0]);
                break;
            case MSEHasCounter:
                data.WriteBit(!m_movementCounter);
                break;
            case MSEHasMovementFlags:
                data.WriteBit(!hasMovementFlags);
                break;
            case MSEHasMovementFlags2:
                data.WriteBit(!hasMovementFlags2);
                break;
            case MSEHasMountDisplayId:
                data.WriteBit(!hasMountDisplayId);
                break;
            case MSEHasTimestamp:
                data.WriteBit(!hasTimestamp);
                break;
            case MSEHasOrientation:
                data.WriteBit(!hasOrientation);
                break;
            case MSEHasTransportData:
                data.WriteBit(hasTransportData);
                break;
            case MSEHasTransportTime2:
                if (hasTransportData)
                    data.WriteBit(hasTransportTime2);
                break;
            case MSEHasTransportTime3:
                if (hasTransportData)
                    data.WriteBit(hasTransportTime3); // this should be renamed
                break;
            case MSEHasPitch:
                data.WriteBit(!hasPitch);
                break;
            case MSEHasFallData:
                data.WriteBit(hasFallData);
                break;
            case MSEHasFallDirection:
                if (hasFallData)
                    data.WriteBit(hasFallDirection);
                break;
            case MSEHasSplineElevation:
                data.WriteBit(!hasSplineElevation);
                break;
            case MSEHasSpline:
                data.WriteBit(hasSpline);
                break;
            case MSEMountDisplayIdWithCheck: // Fallback here
                if (!hasMountDisplayId)
                    break;
            case MSEMountDisplayIdWithoutCheck:
                data << GetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID);
                break;
            case MSEMovementFlags:
                if (hasMovementFlags)
                    data.WriteBits(GetUnitMovementFlags(), 30);
                break;
            case MSEMovementFlags2:
                if (hasMovementFlags2)
                    data.WriteBits(GetExtraUnitMovementFlags(), 13);
                break;
            case MSETimestamp:
                if (hasTimestamp)
                    data << getMSTime();
                break;
            case MSEPositionX:
                if (data.GetOpcode() == SMSG_MOVE_TELEPORT && hasTransportData)
                    data << GetTransOffsetX();
                else
                    data << GetPositionX();
                break;
            case MSEPositionY:
                if (data.GetOpcode() == SMSG_MOVE_TELEPORT && hasTransportData)
                    data << GetTransOffsetY();
                else
                    data << GetPositionY();
                break;
            case MSEPositionZ:
                if (data.GetOpcode() == SMSG_MOVE_TELEPORT && hasTransportData)
                    data << GetTransOffsetZ();
                else
                    data << GetPositionZ();
                break;
            case MSEOrientation:
                if (!hasOrientation)
                    break;
            case MSEOrientationWithoutCheck:
                if (data.GetOpcode() == SMSG_MOVE_TELEPORT && hasTransportData)
                    data << GetTransOffsetO();
                else
                    data << GetOrientation();
                break;
            case MSETransportPositionX:
                if (hasTransportData)
                    data << GetTransOffsetX();
                break;
            case MSETransportPositionY:
                if (hasTransportData)
                    data << GetTransOffsetY();
                break;
            case MSETransportPositionZ:
                if (hasTransportData)
                    data << GetTransOffsetZ();
                break;
            case MSETransportOrientation:
                if (hasTransportData)
                    data << GetTransOffsetO();
                break;
            case MSETransportSeat:
                if (hasTransportData)
                    data << GetTransSeat();
                break;
            case MSETransportTime:
                if (hasTransportData)
                    data << GetTransTime();
                break;
            case MSETransportTime2:
                if (hasTransportData && hasTransportTime2)
                    data << mi.transport.time2;
                break;
            case MSETransportTime3:
                if (hasTransportData && hasTransportTime3)
                    data << mi.transport.time3; // this should be renamed
                break;
            case MSEPitch:
                if (hasPitch)
                    data << mi.pitch;
                break;
            case MSEFallTime:
                if (hasFallData)
                    data << mi.jump.fallTime;
                break;
            case MSEFallVerticalSpeed:
                if (hasFallData)
                    data << mi.jump.zspeed;
                break;
            case MSEFallCosAngle:
                if (hasFallData && hasFallDirection)
                    data << mi.jump.cosAngle;
                break;
            case MSEFallSinAngle:
                if (hasFallData && hasFallDirection)
                    data << mi.jump.sinAngle;
                break;
            case MSEFallHorizontalSpeed:
                if (hasFallData && hasFallDirection)
                    data << mi.jump.xyspeed;
                break;
            case MSESplineElevation:
                if (hasSplineElevation)
                    data << mi.splineElevation;
                break;
            case MSEForcesCount:
                data.WriteBits(0, 22);
                break;
            case MSECounter:
                if (m_movementCounter)
                    data << m_movementCounter;
                m_movementCounter++;
                break;
            case MSECount:
                data << m_movementCounter++;
                break;
            case MSEZeroBit:
                data.WriteBit(0);
                break;
            case MSEOneBit:
                data.WriteBit(1);
                break;
            case MSEExtraElement:
                extras->WriteNextElement(data);
                break;
            case MSEUintCount:
                data << uint32(0);
                break;
            case MSEForces:
                /*
                for (uint8 i = 0; i < forcesCount; ++i)
                data << uint32(0);
                */
                break;
            default:
                ASSERT(Movement::PrintInvalidSequenceElement(element, __FUNCTION__));
                break;
        }
    }
}

void Unit::SendTeleportPacket(Position& pos)
{
    // SMSG_MOVE_UPDATE_TELEPORT is sent to nearby players to signal the teleport
    // SMSG_MOVE_TELEPORT is sent to self in order to trigger CMSG_MOVE_TELEPORT_ACK and update the position server side

    // This oldPos actually contains the destination position if the Unit is a Player.
    Position oldPos = { GetPositionX(), GetPositionY(), GetPositionZMinusOffset(), GetOrientation() };

    if (GetTypeId() == TYPEID_UNIT)
        Relocate(&pos); // Relocate the unit to its new position in order to build the packets correctly.

    WorldPacket data(SMSG_MOVE_UPDATE_TELEPORT);
    BuildTeleportUpdateData(&data);

    if (GetTypeId() == TYPEID_PLAYER)
    {
        WorldPacket data2(SMSG_MOVE_TELEPORT, 1 + 8 + 1 + 8 + 1 + 4 + 4 + 4 + 4);
        WriteMovementInfo(data2);
        ToPlayer()->SendDirectMessage(&data2); // Send the SMSG_MOVE_TELEPORT packet to self.
    }

    // Relocate the player/creature to its old position, so we can broadcast to nearby players correctly
    if (GetTypeId() == TYPEID_PLAYER)
        Relocate(&pos);
    else
        Relocate(&oldPos);

    // Broadcast the packet to everyone except self.
    SendMessageToSet(&data, false);
}

bool Unit::UpdatePosition(float x, float y, float z, float orientation, bool teleport)
{
    // prevent crash when a bad coord is sent by the client
    if (!Trinity::IsValidMapCoord(x, y, z, orientation))
    {
        TC_LOG_DEBUG("entities.unit", "Unit::UpdatePosition(%f, %f, %f) .. bad coordinates!", x, y, z);
        return false;
    }

    // Check if angular distance changed
    bool const turn = G3D::fuzzyGt(M_PI - fabs(fabs(GetOrientation() - orientation) - M_PI), 0.0f);

    // G3D::fuzzyEq won't help here, in some cases magnitudes differ by a little more than G3D::eps, but should be considered equal
    bool const relocated = (teleport ||
        std::fabs(GetPositionX() - x) > 0.001f ||
        std::fabs(GetPositionY() - y) > 0.001f ||
        std::fabs(GetPositionZ() - z) > 0.001f);

    // TODO: Check if orientation transport offset changed instead of only global orientation
    if (turn)
        RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TURNING);

    if (relocated)
    {
        if (!GetVehicle())
            RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_MOVE);

        // move and update visible state if need
        if (GetTypeId() == TYPEID_PLAYER)
            GetMap()->PlayerRelocation(ToPlayer(), x, y, z, orientation);
        else
            GetMap()->CreatureRelocation(ToCreature(), x, y, z, orientation);
    }
    else if (turn)
        UpdateOrientation(orientation);

    UpdatePositionData();

    return (relocated || turn);
}

bool Unit::UpdatePosition(const Position &pos, bool teleport)
{
    return UpdatePosition(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), teleport);
}

//! Only server-side orientation update, does not broadcast to client
void Unit::UpdateOrientation(float orientation)
{
    SetOrientation(orientation);
    if (IsVehicle())
        GetVehicleKit()->RelocatePassengers();
}

//! Only server-side height update, does not broadcast to client
void Unit::UpdateHeight(float newZ)
{
    Relocate(GetPositionX(), GetPositionY(), newZ);
    if (IsVehicle())
        GetVehicleKit()->RelocatePassengers();
}

void Unit::SendThreatListUpdate()
{
    if (!getThreatManager().isThreatListEmpty())
    {
        uint32 count = getThreatManager().getThreatList().size();

        TC_LOG_DEBUG("entities.unit", "WORLD: Send SMSG_THREAT_UPDATE Message");

        ObjectGuid Guid = GetGUID();
        WorldPacket data(SMSG_THREAT_UPDATE, 24);

        data.WriteBit(Guid[5]);
        data.WriteBit(Guid[6]);
        data.WriteBit(Guid[1]);
        data.WriteBit(Guid[3]);
        data.WriteBit(Guid[7]);
        data.WriteBit(Guid[0]);
        data.WriteBit(Guid[4]);
        data.WriteBits(count, 21);

        ThreatContainer::StorageType const &tlist = getThreatManager().getThreatList();
        for (ThreatContainer::StorageType::const_iterator itr = tlist.begin(); itr != tlist.end(); ++itr)
        {
            ObjectGuid unitGuid = (*itr)->getUnitGuid();
            data.WriteBit(unitGuid[2]);
            data.WriteBit(unitGuid[3]);
            data.WriteBit(unitGuid[6]);
            data.WriteBit(unitGuid[5]);
            data.WriteBit(unitGuid[1]);
            data.WriteBit(unitGuid[4]);
            data.WriteBit(unitGuid[0]);
            data.WriteBit(unitGuid[7]);
        }

        data.WriteBit(Guid[2]);

        data.FlushBits();

        for (ThreatContainer::StorageType::const_iterator itr = tlist.begin(); itr != tlist.end(); ++itr)
        {
            ObjectGuid unitGuid = (*itr)->getUnitGuid();
            data.WriteByteSeq(unitGuid[6]);
            data.WriteByteSeq(unitGuid[7]);
            data.WriteByteSeq(unitGuid[0]);
            data.WriteByteSeq(unitGuid[1]);
            data.WriteByteSeq(unitGuid[2]);
            data.WriteByteSeq(unitGuid[5]);
            data.WriteByteSeq(unitGuid[3]);
            data.WriteByteSeq(unitGuid[4]);
            data << uint32((*itr)->getThreat());
        }

        data.WriteByteSeq(Guid[1]);
        data.WriteByteSeq(Guid[4]);
        data.WriteByteSeq(Guid[2]);
        data.WriteByteSeq(Guid[3]);
        data.WriteByteSeq(Guid[5]);
        data.WriteByteSeq(Guid[6]);
        data.WriteByteSeq(Guid[0]);
        data.WriteByteSeq(Guid[7]);

        SendMessageToSet(&data, false);
    }
}

void Unit::SendChangeCurrentVictimOpcode(HostileReference* pHostileReference)
{
    if (!getThreatManager().isThreatListEmpty())
    {
        uint32 count = getThreatManager().getThreatList().size();

        TC_LOG_DEBUG("entities.unit", "WORLD: Send SMSG_HIGHEST_THREAT_UPDATE Message");
        ObjectGuid unitGuid = pHostileReference->getUnitGuid();
        ObjectGuid guid = GetGUID(); 
        WorldPacket data(SMSG_HIGHEST_THREAT_UPDATE, 8 + 8 + count * 8);

        data.WriteBit(unitGuid[3]);
        data.WriteBit(unitGuid[0]);
        data.WriteBit(guid[3]);
        data.WriteBit(guid[6]);
        data.WriteBit(guid[1]);
        data.WriteBit(unitGuid[5]);
        data.WriteBit(unitGuid[1]);
        data.WriteBit(unitGuid[6]);
        data.WriteBit(guid[2]);
        data.WriteBit(guid[5]);
        data.WriteBit(unitGuid[7]);
        data.WriteBit(unitGuid[4]);
        data.WriteBit(guid[4]);
        data.WriteBits(count, 21);

        ThreatContainer::StorageType const &tlist = getThreatManager().getThreatList();
        for (ThreatContainer::StorageType::const_iterator itr = tlist.begin(); itr != tlist.end(); ++itr)
        {
            ObjectGuid UnitGuid = (*itr)->getUnitGuid();
            data.WriteBit(UnitGuid[6]);
            data.WriteBit(UnitGuid[1]);
            data.WriteBit(UnitGuid[0]);
            data.WriteBit(UnitGuid[2]);
            data.WriteBit(UnitGuid[7]);
            data.WriteBit(UnitGuid[4]);
            data.WriteBit(UnitGuid[3]);
            data.WriteBit(UnitGuid[5]);
        }

        data.WriteBit(guid[7]);
        data.WriteBit(guid[0]);
        data.WriteBit(unitGuid[2]);

        data.FlushBits();

        data.WriteByteSeq(guid[4]);

        for (ThreatContainer::StorageType::const_iterator itr = tlist.begin(); itr != tlist.end(); ++itr)
        {
            ObjectGuid UnitGuid = (*itr)->getUnitGuid();

            data.WriteByteSeq(UnitGuid[6]);

            data << uint32((*itr)->getThreat());

            data.WriteByteSeq(UnitGuid[4]);
            data.WriteByteSeq(UnitGuid[0]);
            data.WriteByteSeq(UnitGuid[3]);
            data.WriteByteSeq(UnitGuid[5]);
            data.WriteByteSeq(UnitGuid[2]);
            data.WriteByteSeq(UnitGuid[1]);
            data.WriteByteSeq(UnitGuid[7]);
        }

        data.WriteByteSeq(unitGuid[3]);
        data.WriteByteSeq(guid[5]);
        data.WriteByteSeq(unitGuid[2]);
        data.WriteByteSeq(guid[1]);
        data.WriteByteSeq(guid[0]);
        data.WriteByteSeq(guid[2]);
        data.WriteByteSeq(unitGuid[6]);
        data.WriteByteSeq(unitGuid[1]);
        data.WriteByteSeq(guid[7]);
        data.WriteByteSeq(unitGuid[0]);
        data.WriteByteSeq(unitGuid[4]);
        data.WriteByteSeq(unitGuid[7]);
        data.WriteByteSeq(guid[3]);
        data.WriteByteSeq(guid[6]);
        data.WriteByteSeq(unitGuid[5]);

        SendMessageToSet(&data, false);
    }
}

void Unit::SendClearThreatListOpcode()
{
    TC_LOG_DEBUG("entities.unit", "WORLD: Send SMSG_THREAT_CLEAR Message");
    WorldPacket data(SMSG_THREAT_CLEAR, 8);
    ObjectGuid UnitGUID = GetGUID();

    data.WriteBit(UnitGUID [6]);
    data.WriteBit(UnitGUID [7]);
    data.WriteBit(UnitGUID [4]);
    data.WriteBit(UnitGUID [5]);
    data.WriteBit(UnitGUID [2]);
    data.WriteBit(UnitGUID [1]);
    data.WriteBit(UnitGUID [0]);
    data.WriteBit(UnitGUID [3]);

    data.WriteByteSeq(UnitGUID [7]);
    data.WriteByteSeq(UnitGUID [0]);
    data.WriteByteSeq(UnitGUID [4]);
    data.WriteByteSeq(UnitGUID [3]);
    data.WriteByteSeq(UnitGUID [2]);
    data.WriteByteSeq(UnitGUID [1]);
    data.WriteByteSeq(UnitGUID [6]);
    data.WriteByteSeq(UnitGUID [5]);

    SendMessageToSet(&data, false);
}

void Unit::SendRemoveFromThreatListOpcode(HostileReference* pHostileReference)
{
    TC_LOG_DEBUG("entities.unit", "WORLD: Send SMSG_THREAT_REMOVE Message");
    ObjectGuid victimGUID = GetGUID();
    ObjectGuid hostileGUID = pHostileReference->getUnitGuid();

    WorldPacket data(SMSG_THREAT_REMOVE, 1 + 1 + 8 + 8);

    data.WriteBit(victimGUID [0]);
    data.WriteBit(victimGUID [1]);
    data.WriteBit(victimGUID [5]);
    data.WriteBit(hostileGUID [4]);
    data.WriteBit(hostileGUID [0]);
    data.WriteBit(victimGUID [4]);
    data.WriteBit(victimGUID [6]);
    data.WriteBit(hostileGUID [7]);
    data.WriteBit(hostileGUID [6]);
    data.WriteBit(hostileGUID [3]);
    data.WriteBit(victimGUID [2]);
    data.WriteBit(hostileGUID [1]);
    data.WriteBit(victimGUID [3]);
    data.WriteBit(victimGUID [7]);
    data.WriteBit(hostileGUID [5]);
    data.WriteBit(hostileGUID [2]);

    data.WriteByteSeq(hostileGUID [3]);
    data.WriteByteSeq(hostileGUID [0]);
    data.WriteByteSeq(hostileGUID [2]);
    data.WriteByteSeq(victimGUID [5]);
    data.WriteByteSeq(victimGUID [4]);
    data.WriteByteSeq(victimGUID [7]);
    data.WriteByteSeq(victimGUID [3]);
    data.WriteByteSeq(victimGUID [0]);
    data.WriteByteSeq(hostileGUID [4]);
    data.WriteByteSeq(victimGUID [1]);
    data.WriteByteSeq(hostileGUID [1]);
    data.WriteByteSeq(victimGUID [6]);
    data.WriteByteSeq(hostileGUID [7]);
    data.WriteByteSeq(hostileGUID [6]);
    data.WriteByteSeq(victimGUID [2]);
    data.WriteByteSeq(hostileGUID [5]);

    SendMessageToSet(&data, false);
}

// baseRage means damage taken when attacker = false
void Unit::RewardRage(float baseRage, bool attacker)
{
    float addRage;

    if (attacker)
    {
        addRage = baseRage;
        // talent who gave more rage on attack
        AddPct(addRage, GetTotalAuraModifier(SPELL_AURA_MOD_RAGE_FROM_DAMAGE_DEALT));
    }
    else
    {
        // Generate rage from damage taken only in Berserker Stance
        if (!HasAura(2458))
            return;
        // Calculate rage from health and damage taken
        //! ToDo: Check formula
        addRage = floor(0.5f + (25.7f * baseRage / GetMaxHealth()));
        // Berserker Rage effect
        if (HasAura(18499))
            addRage *= 2.0f;
    }

    addRage *= sWorld->getRate(RATE_POWER_RAGE_INCOME);

    ModifyPower(POWER_RAGE, uint32(addRage * 10));
}

void Unit::StopAttackFaction(uint32 faction_id)
{
    if (Unit* victim = GetVictim())
    {
        if (victim->GetFactionTemplateEntry()->faction == faction_id)
        {
            AttackStop();
            if (IsNonMeleeSpellCasted(false))
                InterruptNonMeleeSpells(false);

            // melee and ranged forced attack cancel
            if (GetTypeId() == TYPEID_PLAYER)
                ToPlayer()->SendAttackSwingCancelAttack();
        }
    }

    AttackerSet const& attackers = getAttackers();
    for (AttackerSet::const_iterator itr = attackers.begin(); itr != attackers.end();)
    {
        if ((*itr)->GetFactionTemplateEntry()->faction == faction_id)
        {
            (*itr)->AttackStop();
            itr = attackers.begin();
        }
        else
            ++itr;
    }

    getHostileRefManager().deleteReferencesForFaction(faction_id);

    for (ControlList::const_iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
        (*itr)->StopAttackFaction(faction_id);
}

void Unit::GetAttackableUnitListInRange(std::list<Unit*> &list, float fMaxSearchRange) const
{
    CellCoord p(Trinity::ComputeCellCoord(GetPositionX(), GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Trinity::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck check(this, fMaxSearchRange);
    Trinity::UnitListSearcher<Trinity::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck> searcher(this, list, check);

    TypeContainerVisitor<Trinity::UnitListSearcher<Trinity::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck>, WorldTypeMapContainer> worldUnitSearcher(searcher);
    TypeContainerVisitor<Trinity::UnitListSearcher<Trinity::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck>, GridTypeMapContainer>  gridUnitSearcher(searcher);

    cell.Visit(p, worldUnitSearcher, *GetMap(), *this, fMaxSearchRange);
    cell.Visit(p, gridUnitSearcher, *GetMap(), *this, fMaxSearchRange);
}

bool Unit::CanLeadFollowTarget(Unit * target) const
{
    if (IsSummon() && GetEntry() == 72908)
        return true;

    if (GetTypeId() != TYPEID_UNIT)
        return false;

    // Only allow to lead owner
    if (target && target->GetGUID() != GetCharmerOrOwnerGUID())
        return false;

    // Allow pets, guardians and warlock summons to lead target
    if (IsPet() || IsGuardian())
        return true;

    // Allow companions to lead target
    if (TempSummon const* summon = ToTempSummon())
        if (summon->m_Properties && summon->m_Properties->Type == SUMMON_TYPE_MINIPET)
            return true;

    return false;
}

void Unit::OutDebugInfo() const
{
    TC_LOG_ERROR("entities.unit", "Unit::OutDebugInfo");
    TC_LOG_INFO("entities.unit", "GUID " UI64FMTD ", entry %u, type %u, name %s", GetGUID().GetRawValue(), GetEntry(), (uint32) GetTypeId(), GetName().c_str());
    TC_LOG_INFO("entities.unit", "OwnerGUID " UI64FMTD ", MinionGUID " UI64FMTD ", CharmerGUID " UI64FMTD ", CharmedGUID " UI64FMTD, GetOwnerGUID().GetRawValue(), GetMinionGUID().GetRawValue(), GetCharmerGUID().GetRawValue(), GetCharmGUID().GetRawValue());
    TC_LOG_INFO("entities.unit", "In world %u, unit type mask %u", (uint32) (IsInWorld() ? 1 : 0), m_unitTypeMask);
    if (IsInWorld())
        TC_LOG_INFO("entities.unit", "Mapid %u", GetMapId());

    std::ostringstream o;
    o << "Summon Slot: ";
    for (uint32 i = 0; i < SUMMON_SLOT_MAX; ++i)
        o << m_SummonSlot [i] << ", ";

    TC_LOG_INFO("entities.unit", "%s", o.str().c_str());
    o.str("");

    o << "Controlled List: ";
    for (ControlList::const_iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
        o << (*itr)->GetGUID() << ", ";
    TC_LOG_INFO("entities.unit", "%s", o.str().c_str());
    o.str("");

    o << "Aura List: ";
    for (AuraApplicationMap::const_iterator itr = m_appliedAuras.begin(); itr != m_appliedAuras.end(); ++itr)
        o << itr->first << ", ";
    TC_LOG_INFO("entities.unit", "%s", o.str().c_str());
    o.str("");

    if (IsVehicle())
    {
        o << "Passenger List: ";
        for (SeatMap::iterator itr = GetVehicleKit()->Seats.begin(); itr != GetVehicleKit()->Seats.end(); ++itr)
            if (Unit* passenger = ObjectAccessor::GetUnit(*GetVehicleBase(), itr->second.Passenger.Guid))
                o << passenger->GetGUID() << ", ";
        TC_LOG_INFO("entities.unit", "%s", o.str().c_str());
    }

    if (GetVehicle())
        TC_LOG_INFO("entities.unit", "On vehicle %u.", GetVehicleBase()->GetEntry());
}

Unit::RemainingPeriodicAmount Unit::GetRemainingPeriodicAmount(uint64 caster, uint32 spellId, AuraType auraType, uint8 effectIndex) const
{
    for (auto&& it : GetAuraEffectsByType(auraType))
    {
        if (it->GetCasterGUID() != caster || it->GetId() != spellId || it->GetEffIndex() != effectIndex || !it->GetTotalTicks())
            continue;

        // If aura is still exist then effect must do last tick.
        int32 ticks = it->GetBase()->GetDuration() / it->GetAmplitude() + 1;
        int32 total = it->GetAmount() * ticks;
        return RemainingPeriodicAmount(total, ticks);
    }

    return RemainingPeriodicAmount(0,0);
}

void Unit::SendClearTarget()
{
    WorldPacket data(SMSG_BREAK_TARGET, 8);
    ObjectGuid UnitGUID = GetGUID();

    data.WriteBit(UnitGUID [2]);
    data.WriteBit(UnitGUID [3]);
    data.WriteBit(UnitGUID [7]);
    data.WriteBit(UnitGUID [5]);
    data.WriteBit(UnitGUID [4]);
    data.WriteBit(UnitGUID [6]);
    data.WriteBit(UnitGUID [0]);
    data.WriteBit(UnitGUID [1]);

    data.WriteByteSeq(UnitGUID [2]);
    data.WriteByteSeq(UnitGUID [1]);
    data.WriteByteSeq(UnitGUID [3]);
    data.WriteByteSeq(UnitGUID [0]);
    data.WriteByteSeq(UnitGUID [7]);
    data.WriteByteSeq(UnitGUID [4]);
    data.WriteByteSeq(UnitGUID [6]);
    data.WriteByteSeq(UnitGUID [5]);
    SendMessageToSet(&data, false);
}

uint32 Unit::GetResistance(SpellSchoolMask mask) const
{
    int32 resist = -1;
    for (int i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
        if (mask & (1 << i) && (resist < 0 || resist > int32(GetResistance(SpellSchools(i)))))
            resist = int32(GetResistance(SpellSchools(i)));

    // resist value will never be negative here
    return uint32(resist);
}

void CharmInfo::SetIsCommandAttack(bool val)
{
    _isCommandAttack = val;
}

bool CharmInfo::IsCommandAttack()
{
    return _isCommandAttack;
}

void CharmInfo::SetIsCommandFollow(bool val)
{
    _isCommandFollow = val;
}

bool CharmInfo::IsCommandFollow()
{
    return _isCommandFollow;
}

void CharmInfo::SaveStayPosition()
{
    if (_unit->movespline->Initialized())
    {
        G3D::Vector3 stayPos = _unit->movespline->FinalDestination();

        if (_unit->movespline->onTransport)
            if (TransportBase* transport = _unit->GetDirectTransport())
                transport->CalculatePassengerPosition(stayPos.x, stayPos.y, stayPos.z);

        _stayX = stayPos.x;
        _stayY = stayPos.y;
        _stayZ = stayPos.z;
    }
    else
    {
        _stayX = _unit->GetPositionX();
        _stayY = _unit->GetPositionY();
        _stayZ = _unit->GetPositionZMinusOffset();
    }
}

void CharmInfo::SaveStayPosition(float x, float y, float z)
{
    if (_unit->movespline->onTransport)
        if (TransportBase* transport = _unit->GetDirectTransport())
            transport->CalculatePassengerPosition(x, y, z);

    _stayX = x;
    _stayY = y;
    _stayZ = z;
}

void CharmInfo::GetStayPosition(float &x, float &y, float &z)
{
    x = _stayX;
    y = _stayY;
    z = _stayZ;
}

void CharmInfo::SetIsAtStay(bool val)
{
    _isAtStay = val;
}

bool CharmInfo::IsAtStay()
{
    return _isAtStay;
}

void CharmInfo::SetIsFollowing(bool val)
{
    _isFollowing = val;
}

bool CharmInfo::IsFollowing()
{
    return _isFollowing;
}

void CharmInfo::SetIsReturning(bool val)
{
    _isReturning = val;
}

bool CharmInfo::IsReturning()
{
    return _isReturning;
}

void Unit::SetInFront(WorldObject const* target)
{
    if (!HasUnitState(UNIT_STATE_CANNOT_TURN))
        UpdateOrientation(GetAngle(target));
}

void Unit::SetFacingTo(float ori)
{
    Movement::MoveSplineInit init(this);
    init.MoveTo(GetPositionX(), GetPositionY(), GetPositionZMinusOffset(), false);
    if (GetTransport())
        init.DisableTransportPathTransformations(); // It makes no sense to target global orientation
    init.SetFacing(ori);
    init.Launch();

    // Update spline now, otherwise we'd be stuck with this spline active until next update, which might interfere with spells, facing etc
    UpdateSplineMovement(1);
}

void Unit::SetFacingToObject(WorldObject* object)
{
    // never face when already moving
    if (!IsStopped())
        return;

    /// @todo figure out under what conditions creature will move towards object instead of facing it where it currently is.
    Movement::MoveSplineInit init(this);
    init.MoveTo(GetPositionX(), GetPositionY(), GetPositionZMinusOffset(), false);
    init.SetFacing(GetAngle(object));   // when on transport, GetAngle will still return global coordinates (and angle) that needs transforming
    init.Launch();

    // Update spline now, otherwise we'd be stuck with this spline active until next update, which might interfere with spells, facing etc
    UpdateSplineMovement(1);
}

bool Unit::SetWalk(bool enable)
{
    if (enable == IsWalking())
        return false;

    if (enable)
        AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
    else
        RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);

    ///@ TODO: Find proper opcode for walk mode setting in player mind controlling a player case
    if (enable)
        Movement::PacketSender(this, SMSG_SPLINE_MOVE_SET_WALK_MODE, SMSG_SPLINE_MOVE_SET_WALK_MODE).Send();
    else
        Movement::PacketSender(this, SMSG_SPLINE_MOVE_SET_RUN_MODE, SMSG_SPLINE_MOVE_SET_RUN_MODE).Send();

    return true;
}

bool Unit::SetDisableGravity(bool disable, bool packetOnly /*= false*/)
{
    if (!packetOnly)
    {
        if (disable == IsGravityDisabled())
            return false;

        if (disable)
        {
            AddUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY);
            RemoveUnitMovementFlag(MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_SPLINE_ELEVATION);
            SetFall(false);
        }
        else
        {
            RemoveUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY);
            if (!HasUnitMovementFlag(MOVEMENTFLAG_CAN_FLY) && (GetTypeId() != TYPEID_PLAYER || HasUnitMovementFlag(MOVEMENTFLAG_FLYING)))
                SetFall(true);
        }
    }

    if (disable)
        Movement::PacketSender(this, SMSG_SPLINE_MOVE_GRAVITY_DISABLE, SMSG_MOVE_GRAVITY_DISABLE).Send();
    else
        Movement::PacketSender(this, SMSG_SPLINE_MOVE_GRAVITY_ENABLE, SMSG_MOVE_GRAVITY_ENABLE).Send();

    return true;
}

bool Unit::SetFall(bool enable)
{
    if (enable == HasUnitMovementFlag(MOVEMENTFLAG_FALLING))
        return false;

    if (enable)
    {
        AddUnitMovementFlag(MOVEMENTFLAG_FALLING);
        m_movementInfo.SetFallTime(0);
    }
    else
        RemoveUnitMovementFlag(MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLING_FAR);

    return true;
}

bool Unit::SetSwim(bool enable)
{
    if (enable == HasUnitMovementFlag(MOVEMENTFLAG_SWIMMING))
        return false;

    if (enable)
        AddUnitMovementFlag(MOVEMENTFLAG_SWIMMING);
    else
        RemoveUnitMovementFlag(MOVEMENTFLAG_SWIMMING);

    if (enable)
        Movement::PacketSender(this, SMSG_SPLINE_MOVE_START_SWIM, NULL_OPCODE).Send();
    else
        Movement::PacketSender(this, SMSG_SPLINE_MOVE_STOP_SWIM, NULL_OPCODE).Send();

    return true;
}

bool Unit::SetCanFly(bool enable)
{
    if (enable == HasUnitMovementFlag(MOVEMENTFLAG_CAN_FLY))
        return false;

    if (enable)
    {
        AddUnitMovementFlag(MOVEMENTFLAG_CAN_FLY);
        RemoveUnitMovementFlag(MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_SPLINE_ELEVATION);
        SetFall(false);
    }
    else
    {
        RemoveUnitMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_MASK_MOVING_FLY);
        if (!IsGravityDisabled() && (GetTypeId() != TYPEID_PLAYER || HasUnitMovementFlag(MOVEMENTFLAG_FLYING)))
            SetFall(true);
    }

    if (enable)
        Movement::PacketSender(this, SMSG_SPLINE_MOVE_SET_FLYING, SMSG_MOVE_SET_CAN_FLY).Send();
    else
        Movement::PacketSender(this, SMSG_SPLINE_MOVE_UNSET_FLYING, SMSG_MOVE_UNSET_CAN_FLY).Send();

    return true;
}

bool Unit::SetWaterWalking(bool enable, bool packetOnly /*= false */)
{
    if (!packetOnly)
    {
        if (enable == HasUnitMovementFlag(MOVEMENTFLAG_WATERWALKING))
            return false;

        if (enable)
            AddUnitMovementFlag(MOVEMENTFLAG_WATERWALKING);
        else
            RemoveUnitMovementFlag(MOVEMENTFLAG_WATERWALKING);
    }

    if (enable)
        Movement::PacketSender(this, SMSG_SPLINE_MOVE_SET_WATER_WALK, SMSG_MOVE_WATER_WALK).Send();
    else
        Movement::PacketSender(this, SMSG_SPLINE_MOVE_SET_LAND_WALK, SMSG_MOVE_LAND_WALK).Send();

    return true;
}

bool Unit::SetFeatherFall(bool enable, bool packetOnly /*= false */)
{
    if (!packetOnly)
    {
        if (enable == HasUnitMovementFlag(MOVEMENTFLAG_FALLING_SLOW))
            return false;

        if (enable)
            AddUnitMovementFlag(MOVEMENTFLAG_FALLING_SLOW);
        else
            RemoveUnitMovementFlag(MOVEMENTFLAG_FALLING_SLOW);
    }

    if (enable)
        Movement::PacketSender(this, SMSG_SPLINE_MOVE_SET_FEATHER_FALL, SMSG_MOVE_FEATHER_FALL).Send();
    else
        Movement::PacketSender(this, SMSG_SPLINE_MOVE_SET_NORMAL_FALL, SMSG_MOVE_NORMAL_FALL).Send();

    return true;
}

bool Unit::SetHover(bool enable, bool packetOnly /*= false*/)
{
    if (!packetOnly)
    {
        if (enable == IsHovering())
            return false;

        if (enable)
        {
            //! No need to check height on ascent
            AddUnitMovementFlag(MOVEMENTFLAG_HOVER);
            if (float hh = GetFloatValue(UNIT_FIELD_HOVER_HEIGHT))
                UpdateHeight(GetPositionZ() + hh);
        }
        else
        {
            RemoveUnitMovementFlag(MOVEMENTFLAG_HOVER);
            if (float hh = GetFloatValue(UNIT_FIELD_HOVER_HEIGHT))
            {
                float newZ = GetPositionZ() - hh;
                UpdateAllowedPositionZ(GetPositionX(), GetPositionY(), newZ);
                UpdateHeight(newZ);
            }
        }

        //! Unconfirmed for players:
        if (enable)
            SetAnimTier(AnimTier::Hover);
        else if (IsGravityDisabled())
            SetAnimTier(AnimTier::Fly);
        else
            SetAnimTier(AnimTier::Ground);
    }

    if (enable)
        Movement::PacketSender(this, SMSG_SPLINE_MOVE_SET_HOVER, SMSG_MOVE_SET_HOVER).Send();
    else
        Movement::PacketSender(this, SMSG_SPLINE_MOVE_UNSET_HOVER, SMSG_MOVE_UNSET_HOVER).Send();

    return true;
}

void Unit::SendSetPlayHoverAnim(bool PlayHoverAnim)
{
    ObjectGuid UnitGUID = GetGUID();
    WorldPacket data(SMSG_SET_PLAY_HOVER_ANIM, 10);
    data.WriteBit(UnitGUID [3]);
    data.WriteBit(UnitGUID [6]);
    data.WriteBit(UnitGUID [1]);
    data.WriteBit(UnitGUID [4]);
    data.WriteBit(UnitGUID [5]);
    data.WriteBit(UnitGUID [0]);
    data.WriteBit(PlayHoverAnim);
    data.WriteBit(UnitGUID [2]);
    data.WriteBit(UnitGUID [7]);

    data.WriteByteSeq(UnitGUID [5]);
    data.WriteByteSeq(UnitGUID [1]);
    data.WriteByteSeq(UnitGUID [6]);
    data.WriteByteSeq(UnitGUID [2]);
    data.WriteByteSeq(UnitGUID [3]);
    data.WriteByteSeq(UnitGUID [0]);
    data.WriteByteSeq(UnitGUID [4]);
    data.WriteByteSeq(UnitGUID [7]);

    SendMessageToSet(&data, true);
}

bool Unit::IsSplineEnabled() const
{
    return movespline->Initialized() && !movespline->Finalized();
}

int32 Unit::GetSplineDuration() const
{
    return IsSplineEnabled() ? movespline->Duration() : 0;
}

void Unit::BuildValuesUpdate(uint8 updateType, ByteBuffer* data, Player* target) const
{
    if (!target)
        return;

    UpdateBuilder builder;
    builder.SetSource(updateType == UPDATETYPE_VALUES ? _changesMask.GetBits() : m_uint32Values, m_valuesCount);
    builder.SetDest(data);

    uint32 valCount = m_valuesCount;

    uint32* flags = nullptr;
    uint32 visibleFlag = GetUpdateFieldData(target, flags);

    if (target == this)
        visibleFlag |= UF_FLAG_PRIVATE;

    Player* plr = GetCharmerOrOwnerPlayerOrPlayerItself();
    if (GetOwnerGUID() == target->GetGUID())
        visibleFlag |= UF_FLAG_OWNER;

    if (HasFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_SPECIALINFO))
        if (HasAuraTypeWithCaster(SPELL_AURA_EMPATHY, target->GetGUID()))
            visibleFlag |= UF_FLAG_SPECIAL_INFO;

    if (plr && plr->IsInSameRaidWith(target))
        visibleFlag |= UF_FLAG_PARTY_MEMBER;

    if (GetTypeId() == TYPEID_PLAYER && !(visibleFlag & UF_FLAG_PRIVATE))
        valCount = PLAYER_FIELD_INV_SLOTS;

    Creature const* creature = ToCreature();
    for (uint16 index = 0; index < valCount; ++index)
    {
        if (_fieldNotifyFlags & flags[index] ||
            ((flags[index] & visibleFlag) & UF_FLAG_SPECIAL_INFO) ||
            ((flags[index] & visibleFlag) && builder.GetSrcBit(index)))    // there are many private fields for player, avoid them first
        {
            builder.SetDestBit(index);

            if (index == UNIT_FIELD_NPC_FLAGS)
            {
                uint32 appendValue = m_uint32Values [UNIT_FIELD_NPC_FLAGS];

                if (creature)
                    if (!target->CanSeeSpellClickOn(creature))
                        appendValue &= ~UNIT_NPC_FLAG_SPELLCLICK;

                if (Battleground* bg = target->GetBattleground())
                    if (!bg->CanSeeSpellClick(target, this))
                        appendValue &= ~UNIT_NPC_FLAG_SPELLCLICK;

                *data << uint32(appendValue);
            }
            else if (index == UNIT_FIELD_AURA_STATE)
            {
                // Check per caster aura states to not enable using a spell in client if specified aura is not by target
                *data << BuildAuraStateUpdateForTarget(target);
            }
            // FIXME: Some values at server stored in float format but must be sent to client in uint32 format
            else if (index >= UNIT_FIELD_ATTACK_ROUND_BASE_TIME && index <= UNIT_FIELD_RANGED_ATTACK_ROUND_BASE_TIME)
            {
                // convert from float to uint32 and send
                *data << uint32(m_floatValues [index] < 0 ? 0 : m_floatValues [index]);
            }
            // there are some float values which may be negative or can't get negative due to other checks
            else if ((index >= UNIT_FIELD_STAT_NEG_BUFF   && index <= UNIT_FIELD_STAT_NEG_BUFF + 4) ||
                     (index >= UNIT_FIELD_RESISTANCE_BUFF_MODS_POSITIVE  && index <= (UNIT_FIELD_RESISTANCE_BUFF_MODS_POSITIVE + 6)) ||
                     (index >= UNIT_FIELD_RESISTANCE_BUFF_MODS_NEGATIVE  && index <= (UNIT_FIELD_RESISTANCE_BUFF_MODS_NEGATIVE + 6)) ||
                     (index >= UNIT_FIELD_STAT_POS_BUFF   && index <= UNIT_FIELD_STAT_POS_BUFF + 4))
            {
                *data << uint32(m_floatValues [index]);
            }
            // Gamemasters should be always able to select units - remove not selectable flag
            else if (index == UNIT_FIELD_FLAGS)
            {
                uint32 appendValue = m_uint32Values [UNIT_FIELD_FLAGS];
                if (target->IsGameMaster())
                    appendValue &= ~UNIT_FLAG_NOT_SELECTABLE;

                *data << uint32(appendValue);
            }
            // use modelid_a if not gm, _h if gm for CREATURE_FLAG_EXTRA_TRIGGER creatures
            else if (index == UNIT_FIELD_DISPLAY_ID)
            {
                uint32 displayId = m_uint32Values [UNIT_FIELD_DISPLAY_ID];
                if (creature)
                {
                    CreatureTemplate const* cinfo = creature->GetCreatureTemplate();

                    // this also applies for transform auras
                    if (SpellInfo const* transform = sSpellMgr->GetSpellInfo(getTransForm()))
                        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                            if (transform->Effects [i].IsAura(SPELL_AURA_TRANSFORM))
                                if (CreatureTemplate const* transformInfo = sObjectMgr->GetCreatureTemplate(transform->Effects [i].MiscValue))
                                {
                                    cinfo = transformInfo;
                                    break;
                                }

                    if (cinfo->flags_extra & CREATURE_FLAG_EXTRA_TRIGGER)
                    {
                        if (target->IsGameMaster())
                        {
                            if (cinfo->Modelid1)
                                displayId = cinfo->Modelid1;    // Modelid1 is a visible model for gms
                            else
                                displayId = 17519;              // world visible trigger's model
                        }
                        else
                        {
                            if (cinfo->Modelid2)
                                displayId = cinfo->Modelid2;    // Modelid2 is an invisible model for players
                            else
                                displayId = 11686;              // world invisible trigger's model
                        }
                    }
                }

                *data << uint32(displayId);
            }
            // hide lootable animation for unallowed players
            else if (index == OBJECT_FIELD_DYNAMIC_FLAGS)
            {
                uint32 dynamicFlags = m_uint32Values [OBJECT_FIELD_DYNAMIC_FLAGS] & ~(UNIT_DYNFLAG_TAPPED | UNIT_DYNFLAG_TAPPED_BY_PLAYER);

                if (creature)
                {
                    if (creature->HasLootRecipient())
                    {
                        dynamicFlags |= UNIT_DYNFLAG_TAPPED;
                        if (creature->IsTappedBy(target))
                            dynamicFlags |= UNIT_DYNFLAG_TAPPED_BY_PLAYER;
                    }

                    if (!target->IsAllowedToLoot(creature))
                        dynamicFlags &= ~UNIT_DYNFLAG_LOOTABLE;
                }

                // unit UNIT_DYNFLAG_TRACK_UNIT should only be sent to caster of SPELL_AURA_MOD_STALKED auras
                if (dynamicFlags & UNIT_DYNFLAG_TRACK_UNIT)
                    if (!HasAuraTypeWithCaster(SPELL_AURA_MOD_STALKED, target->GetGUID()))
                        dynamicFlags &= ~UNIT_DYNFLAG_TRACK_UNIT;

                if (dynamicFlags & UNIT_DYNFLAG_DEAD)
                    if (HasFlag(UNIT_FIELD_FLAGS2, UNIT_FLAG2_FEIGN_DEATH) && IsInRaidWith(target))
                        dynamicFlags &= ~UNIT_DYNFLAG_DEAD;

                *data << dynamicFlags;
            }
            // FG: pretend that OTHER players in own group are friendly ("blue")
            else if (index == UNIT_FIELD_SHAPESHIFT_FORM || index == UNIT_FIELD_FACTION_TEMPLATE)
            {
                FactionTemplateEntry const* ft1 = GetFactionTemplateEntry();
                FactionTemplateEntry const* ft2 = target->GetFactionTemplateEntry();
                if (IsControlledByPlayer() && target != this && (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP) || target->GetGroup() && target->GetGroup()->isLFGGroup() && sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_LFG)) && IsInRaidWith(target))
                {
                    if (ft1 && ft2 && !ft1->IsFriendlyTo(*ft2))
                    {
                        if (index == UNIT_FIELD_SHAPESHIFT_FORM)
                            // Allow targetting opposite faction in party when enabled in config
                            *data << (m_uint32Values [UNIT_FIELD_SHAPESHIFT_FORM] & ((UNIT_BYTE2_FLAG_SANCTUARY /*| UNIT_BYTE2_FLAG_AURAS | UNIT_BYTE2_FLAG_UNK5*/) << 8)); // this flag is at uint8 offset 1 !!
                        else
                            // pretend that all other HOSTILE players have own faction, to allow follow, heal, rezz (trade wont work)
                            *data << uint32(target->GetFaction());
                    }
                    else
                        *data << m_uint32Values [index];
                }
                else if (GetMapId() == 37 && sWorld->getBoolConfig(CONFIG_ICORE_ROYALE_EVENT_ENABLED) &&  index == UNIT_FIELD_FACTION_TEMPLATE && IsControlledByPlayer() && target != this && ft1->IsFriendlyTo(*ft2))
                {
                    // pretend that all other ALLY players have opposing team's faction
                    *data << uint32(target->GetTeamId() == TEAM_ALLIANCE ? 2 : 1);
                }
                else
                    *data << m_uint32Values [index];
            }
            else if (index >= UNIT_FIELD_SUMMONED_BY && index < UNIT_FIELD_CREATED_BY)
            {
                if (IsSummon() && ToTempSummon()->IsSummonedByHidden())
                    *data << 0;
                else
                    *data << m_uint32Values[index];
            }
            else
            {
                // send in current format (float as float, uint32 as uint32)
                *data << m_uint32Values [index];
            }
        }
    }

    builder.Finish();

    BuildDynamicValuesUpdate(updateType, data);
}

// Returns collisionheight of the unit. If it is 0, it returns DEFAULT_COLLISION_HEIGHT.
float Unit::GetCollisionHeight() const
{
    float scaleMod = GetObjectScale(); // 99% sure about this

    if (IsMounted())
    {
        if (CreatureDisplayInfoEntry const* mountDisplayInfo = sCreatureDisplayInfoStore.LookupEntry(GetMountDisplayId()))
        {
            if (CreatureModelDataEntry const* mountModelData = sCreatureModelDataStore.LookupEntry(mountDisplayInfo->ModelID))
            {
                CreatureDisplayInfoEntry const* displayInfo = sCreatureDisplayInfoStore.AssertEntry(GetNativeDisplayId());
                CreatureModelDataEntry const* modelData = sCreatureModelDataStore.AssertEntry(displayInfo->ModelID);
                float const collisionHeight = scaleMod * (mountModelData->MountHeight + modelData->CollisionHeight * modelData->ModelScale * displayInfo->CreatureModelScale * 0.5f);
                return collisionHeight == 0.0f ? DEFAULT_COLLISION_HEIGHT : collisionHeight;
            }
        }
    }

    //! Dismounting case - use basic default model data
    CreatureDisplayInfoEntry const* displayInfo = sCreatureDisplayInfoStore.AssertEntry(GetNativeDisplayId());
    CreatureModelDataEntry const* modelData = sCreatureModelDataStore.AssertEntry(displayInfo->ModelID);

    float const collisionHeight = scaleMod * modelData->CollisionHeight * modelData->ModelScale * displayInfo->CreatureModelScale;
    return collisionHeight == 0.0f ? DEFAULT_COLLISION_HEIGHT : collisionHeight;
}

void Unit::SendSetVehicleRecId(uint32 vehicleId)
{
    WorldPacket data;
    ObjectGuid vehicleGuid = GetGUID();
    Player* player = ToPlayer();
    if (player && !player->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNT))
    {
        uint32 index = m_movementCounter++;
        data.Initialize(SMSG_MOVE_SET_VEHICLE_REC_ID, 8 + 4 + 4);
        data.WriteBit(vehicleGuid[0]);
        data.WriteBit(vehicleGuid[6]);
        data.WriteBit(vehicleGuid[1]);
        data.WriteBit(vehicleGuid[3]);
        data.WriteBit(vehicleGuid[7]);
        data.WriteBit(vehicleGuid[4]);
        data.WriteBit(vehicleGuid[5]);
        data.WriteBit(vehicleGuid[2]);
        data.WriteByteSeq(vehicleGuid[6]);
        data.WriteByteSeq(vehicleGuid[7]);
        data.WriteByteSeq(vehicleGuid[0]);
        data.WriteByteSeq(vehicleGuid[3]);
        data << uint32(vehicleId);
        data << uint32(index);
        data.WriteByteSeq(vehicleGuid[1]);
        data.WriteByteSeq(vehicleGuid[5]);
        data.WriteByteSeq(vehicleGuid[2]);
        data.WriteByteSeq(vehicleGuid[4]);
        player->SendDirectMessage(&data);
    }
    else
    {
        data.Initialize(SMSG_SET_VEHICLE_REC_ID, 8 + 4);
        data.WriteBit(vehicleGuid[5]);
        data.WriteBit(vehicleGuid[7]);
        data.WriteBit(vehicleGuid[2]);
        data.WriteBit(vehicleGuid[1]);
        data.WriteBit(vehicleGuid[4]);
        data.WriteBit(vehicleGuid[0]);
        data.WriteBit(vehicleGuid[3]);
        data.WriteBit(vehicleGuid[6]);
        data.WriteByteSeq(vehicleGuid[5]);
        data.WriteByteSeq(vehicleGuid[7]);
        data.WriteByteSeq(vehicleGuid[4]);
        data.WriteByteSeq(vehicleGuid[6]);
        data.WriteByteSeq(vehicleGuid[2]);
        data.WriteByteSeq(vehicleGuid[1]);
        data.WriteByteSeq(vehicleGuid[3]);
        data.WriteByteSeq(vehicleGuid[0]);
        data << uint32(vehicleId);
        SendMessageToSet(&data, true);
    }
}

void Unit::BuildTeleportUpdateData(WorldPacket* data)
{
    // TODO: more research and move it to movement structures
    MovementInfo const& mi = m_movementInfo;

    bool hasTransportData = GetTransGUID() != 0;
    bool hasSplineElevation = HasUnitMovementFlag(MOVEMENTFLAG_SPLINE_ELEVATION);
    bool hasFallDirection = HasUnitMovementFlag(MOVEMENTFLAG_FALLING);
    bool hasFallData = hasFallDirection || m_movementInfo.jump.fallTime != 0;
    bool hasTransportTime2 = hasTransportData && m_movementInfo.transport.time2 != 0;
    bool hasTransportTime3 = false;
    bool hasSwimSpeed = GetSpeed(MOVE_SWIM) != 0;
    bool hasRunBackSpeed = GetSpeed(MOVE_RUN_BACK) != 0;
    bool hasFlightSpeed = GetSpeed(MOVE_FLIGHT) != 0;
    bool hasRunSpeed = GetSpeed(MOVE_RUN) != 0;
    bool hasMovementFlags = GetUnitMovementFlags() != 0;
    bool hasMovementFlags2 = GetExtraUnitMovementFlags() != 0;
    bool hasTimestamp = true;
    bool hasOrientation = !G3D::fuzzyEq(GetOrientation(), 0.0f);
    bool unk189 = false, unk98h = false, unk188 = false, unk228 = false, unk52 = false, unk28 = false, unk260 = false, unkb8h = false, unk212 = false, unk236 = false;
    int32 count240 = 0, unk268 = 0;
    // int32 unk260 = 0;
    float unk248 = 0, unk252 = 0.0f, unk256 = 0.0f, unk264 = 0.0f, unk100h = 0.0f, unk244 = 0.0f, unke8h = 0.0f, unk18h = 0.0f, unk30h = 0.0f, unke0h = 0.0f;
    // float unkb8h = 0.0f, unk98h = 0.0f;
    float flightSpeed = GetSpeed(MOVE_FLIGHT);
    float swimSpeed = GetSpeed(MOVE_SWIM);
    float runSpeed = GetSpeed(MOVE_RUN);
    float runBackSpeed = GetSpeed(MOVE_RUN_BACK);

    ObjectGuid guid = GetGUID();
    ObjectGuid tguid = hasTransportData ? GetTransGUID() : ObjectGuid::Empty;

    data->WriteBit(hasTransportData); // 144
    data->WriteBit(!hasTimestamp);
    data->WriteBit(hasFallData); // 180
    data->WriteBit(unk189); // 189
    data->WriteBit(!unk98h); // 98h

    if (hasTransportData)
    {
        data->WriteBit(tguid[7]);
        data->WriteBit(tguid[3]);
        data->WriteBit(tguid[0]); // 96
        data->WriteBit(tguid[2]);
        data->WriteBit(tguid[6]);
        data->WriteBit(hasTransportTime3); // 140
        data->WriteBit(tguid[5]);
        data->WriteBit(tguid[4]);
        data->WriteBit(hasTransportTime2); // 132
        data->WriteBit(tguid[1]);
    }

    data->WriteBit(guid[0]);
    data->WriteBit(hasSwimSpeed); // 44
    data->WriteBit(unk188); // 188
    data->WriteBit(!hasMovementFlags); // 64
    data->WriteBit(guid[1]);
    data->WriteBit(unk228); // 228
    data->WriteBit(hasRunBackSpeed); // 20
    data->WriteBit(unk52); // 52

    data->WriteBits(count240, 19); // unused, in all sniffs is 0

    data->WriteBit(!hasSplineElevation);
    data->WriteBit(guid[6]);
    data->WriteBit(!hasOrientation); // 58h
    data->WriteBit(unk28); // 28
    if (hasFallData) // 180
        data->WriteBit(hasFallDirection); // 176
    data->WriteBit(hasFlightSpeed); // 220
    data->WriteBit(unk260); // 260
    data->WriteBit(guid[3]);
    data->WriteBit(guid[4]);
    data->WriteBit(hasRunSpeed); // 36
    data->WriteBit(!unkb8h); // b8h
    data->WriteBit(unk212); // 212
    data->WriteBit(guid[5]);
    for (int i = 0; i < count240; i++)
        data->WriteBits(unk268, 2); // 268
    data->WriteBit(unk236); // 236
    data->WriteBit(guid[2]);
    data->WriteBits(0, 22); // ForcesCount, in our implementation always 0
    data->WriteBit(!hasMovementFlags2); // 68
    if (hasMovementFlags) // 64
        data->WriteBits(GetUnitMovementFlags(), 30);
    data->WriteBit(guid[7]);
    if (hasMovementFlags2) // 68
        data->WriteBits(GetExtraUnitMovementFlags(), 13);
    if (hasFlightSpeed)
        *data << flightSpeed; // d8h

    if (hasTransportData) // 144
    {
        data->WriteByteSeq(tguid[3]);
        data->WriteByteSeq(tguid[5]);
        *data << GetTransTime(); // 124
        *data << GetTransOffsetZ(); // 70h
        *data << GetTransOffsetO(); // 74h
        data->WriteByteSeq(tguid[4]);
        data->WriteByteSeq(tguid[1]);
        data->WriteByteSeq(tguid[7]);
        *data << GetTransOffsetY(); // 6ch

        if (hasTransportTime2) // 132
            *data << mi.transport.time2; // 128

        data->WriteByteSeq(tguid[0]);
        data->WriteByteSeq(tguid[2]);

        if (hasTransportTime3) // 140
            *data << mi.transport.time3; // 136

        *data << GetTransSeat(); // 120
        *data << GetTransOffsetX(); // 68h
        data->WriteByteSeq(tguid[6]);
    }

    data->WriteByteSeq(guid[3]);

    if (hasFallData) // 180
    {
        if (hasFallDirection) // 176
        {
            *data << mi.jump.sinAngle; // a8h
            *data << mi.jump.xyspeed; // ach
            *data << mi.jump.cosAngle; // a4h
        }
        *data << mi.jump.fallTime; // 156
        *data << mi.jump.zspeed; // a0h
    }
    data->WriteByteSeq(guid[4]);

    for (int i = 0; i < count240; i++)
    {
        *data << int32(unk260); // 260
        *data << int32(unk244); // 244
        *data << float(unk248); // 248
        *data << float(unk252); // 252
        *data << float(unk256); // 256
        *data << float(unk264); // 264
    }

    if (unk260)
        *data << float(unk100h); // 100h

    *data << GetPositionY(); // 50h

    if (unkb8h)
        *data << float(unkb8h); // b8h

    if (unk236)
        *data << float(unke8h); // e8h

    if (unk98h)
        *data << float(unk98h); // 98h

    if (hasRunBackSpeed)
        *data << float(runBackSpeed); // 10h

    data->WriteByteSeq(guid[2]);

    data->WriteByteSeq(guid[7]);

    if (hasSwimSpeed)
        *data << float(swimSpeed); // 28h

    *data << GetPositionX(); // 4ch

    if (hasRunSpeed)
        *data << float(runSpeed); // 20h

    if (unk28)
        *data << float(unk18h); // 18h

    //for (int i = 0; i < 0; i++) // ForcesCount, placeholder
    //    *data << int32(0);

    *data << GetPositionZ(); // 54h

    if (hasTimestamp)
        *data << getMSTime();

    if (hasSplineElevation)
        *data << mi.splineElevation;

    data->WriteByteSeq(guid[6]);

    if (unk52)
        *data << float(unk30h);

    data->WriteByteSeq(guid[5]);
    data->WriteByteSeq(guid[1]);

    if (hasOrientation)
        *data << GetOrientation(); // 58h

    data->WriteByteSeq(guid[0]);

    if (unk228)
        *data << float(unke0h); // e0h
}

uint32 Unit::GetNpcDamageTakenInPastSecs(uint32 secs) const
{
    auto const offset = std::min<size_t>(secs, _npcDamageTaken.size());
    return std::accumulate(_npcDamageTaken.begin(), _npcDamageTaken.begin() + offset, 0u);
}

uint32 Unit::GetPlayerDamageTakenInPastSecs(uint32 secs) const
{
    auto const offset = std::min<size_t>(secs, _playerDamageTaken.size());
    return std::accumulate(_playerDamageTaken.begin(), _playerDamageTaken.begin() + offset, 0u);
}

uint32 Unit::GetDamageTakenInPastSecs(uint32 secs) const
{
    auto const offset = std::min<size_t>(secs, _npcDamageTaken.size());

    uint32 damage = 0;
    for (size_t i = 0; i < offset; ++i)
        damage += _npcDamageTaken[i] + _playerDamageTaken[i];

    return damage;
}

bool Unit::HasStealthBreakImmunity() const
{
    for (auto&& it : GetAuraEffectsByType(SPELL_AURA_MOD_STEALTH))
        if (it->GetId() == 115191 || it->GetId() == 115193 || it->GetId() == 11327)
            return true;
    return false;
}

float Unit::GetScallingDamageMod() const
{
    uint32 members = 0;
    // calculate flex scalling mod
    if (GetMap()->IsRaid() && GetMap()->GetDifficulty() == RAID_DIFFICULTY_1025MAN_FLEX)
    {
        if (IsHostileToPlayers() && GetTypeId() != TYPEID_PLAYER)
        {
            members = GetMap()->GetPlayersCountExceptGMs();
            if (members > 10)
                members -= 10;
            else
                members = 0;
        }
    }

    // calculate celestials scalling mod
    if (GetEntry() == 71955 || GetEntry() == 71953 || GetEntry() == 71952 || GetEntry() == 71954)
    {
        auto threatList = const_cast<Unit*>(this)->getThreatManager().getThreatList();
        for (auto&& itr : threatList)
            if (Unit* unit = ObjectAccessor::GetUnit(*this, itr->getUnitGuid()))
                if (unit->GetTypeId() == TYPEID_PLAYER && unit->IsWithinDist(this, 100.0f))
                    members++;
    }

    auto info = sObjectMgr->GetCreatureScalingData(GetEntry(), members);
    if (!info)
        return 1.0f;

    return (info->DamageMod / 100.0f) + 1.0f;
}

bool Unit::PetNeedsLOSCheckOnAttackStart() const
{
    if (GetMap()->IsBattlegroundOrArena())
    {
        // Ring of Valor hack (force LOS checks when ascending on starting elevators)
        if (GetMapId() == 618 && GetTransport() && (GetTransport()->GetEntry() == 194582 || GetTransport()->GetEntry() == 194586)) // Doodad_org_arena_elevator03 / Doodad_org_arena_yellow_elevator01
            return true;

        return false;
    }
    return true;
}

void Unit::SendMissileCancel(uint32 spellId, bool cancel)
{
    ObjectGuid guid = GetGUID();
    WorldPacket data{ SMSG_MISSILE_CANCEL };
    data.WriteBit(guid[6]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[5]);
    data.WriteBit(cancel);
    data.WriteBit(guid[3]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[1]);
    data << spellId;
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[6]);
    SendMessageToSet(&data, true);
}

void Unit::SendResumeCastBar(Player* player)
{
    Spell* spell = GetCurrentSpell(CURRENT_GENERIC_SPELL);
    if (!spell)
        spell = GetCurrentSpell(CURRENT_CHANNELED_SPELL);

    if (!spell)
        return;

    ObjectGuid targetGuid = spell->m_targets.GetObjectTargetGUID();
    ObjectGuid casterGuid = GetGUID();

    WorldPacket data{ SMSG_RESUME_CAST_BAR };

    data.WriteBit(casterGuid[3]);
    data.WriteBit(targetGuid[5]);
    data.WriteBit(targetGuid[6]);
    data.WriteBit(casterGuid[0]);
    data.WriteBit(targetGuid[2]);
    data.WriteBit(casterGuid[1]);
    data.WriteBit(casterGuid[4]);
    data.WriteBit(casterGuid[5]);
    data.WriteBit(targetGuid[0]);

    bool unk48 = false;

    data.WriteBit(unk48);

    data.WriteBit(casterGuid[7]);
    data.WriteBit(casterGuid[6]);
    data.WriteBit(targetGuid[4]);
    data.WriteBit(targetGuid[3]);
    data.WriteBit(targetGuid[1]);
    data.WriteBit(casterGuid[2]);
    data.WriteBit(targetGuid[7]);

    if (unk48)
    {
        data << uint32(0);
        data << uint32(0);
    }

    data.WriteByteSeq(casterGuid[6]);
    data.WriteByteSeq(targetGuid[5]);
    data.WriteByteSeq(targetGuid[7]);
    data.WriteByteSeq(targetGuid[4]);

    data << uint32(spell->GetSpellInfo()->Id);

    data.WriteByteSeq(targetGuid[0]);
    data.WriteByteSeq(casterGuid[0]);
    data.WriteByteSeq(casterGuid[1]);

    data << uint32(spell->GetCurrentCastTimer());

    data.WriteByteSeq(casterGuid[2]);
    data.WriteByteSeq(targetGuid[2]);
    data.WriteByteSeq(casterGuid[7]);
    data.WriteByteSeq(targetGuid[6]);
    data.WriteByteSeq(casterGuid[4]);

    data << uint32(spell->GetCurrentContainer() == CURRENT_CHANNELED_SPELL ? spell->GetChannelTime() : spell->GetCastTime());

    data.WriteByteSeq(casterGuid[3]);
    data.WriteByteSeq(casterGuid[5]);
    data.WriteByteSeq(targetGuid[3]);
    data.WriteByteSeq(targetGuid[1]);

    player->SendDirectMessage(&data);
}

void Unit::GetMovementSpeedModifiers(int32& mainBonus, float& stackBonus, float& nonStackBonus)
{
    int32 nonStackModifier = 0;
    int32 nonStackModifierId = 0;

    for (auto&& itr : GetAuraEffectsByType(SPELL_AURA_MOD_SPEED_NOT_STACK))
    {
        if (itr->GetAmount() > nonStackModifier)
        {
            nonStackModifier = itr->GetAmount();
            nonStackModifierId = itr->GetId();
        }
    }

    static std::set<uint32> const nonStackBuffs
    {
        2983,   // Sprint
        23451,  // Speed (BG buff)
        58875,  // Spirit Walk
        64129,  // Body and Soul
        68992,  // Darkflight
        85499,  // Speed of Light
        108843, // Blazing Speed
        111400, // Burning Rush
        116841, // Tiger's Lust
        121557, // Angelic Feather
        137573, // Burst of Speed
    };

    int32 mainModifier = 0;
    int32 mainModifierId = 0;

    for (auto&& itr : GetAuraEffectsByType(SPELL_AURA_MOD_INCREASE_SPEED))
    {
        if (nonStackModifierId == 54861)    // Nitro Boosts
            if (nonStackBuffs.find(itr->GetId()) != nonStackBuffs.end())
                continue;

        if (itr->GetAmount() > mainModifier)
        {
            mainModifier = itr->GetAmount();
            mainModifierId = itr->GetId();
        }
    }

    std::map<SpellGroup, int32> sameEffectSpellGroup;
    float multiplier = 1.0f;

    for (auto&& itr : GetAuraEffectsByType(SPELL_AURA_MOD_SPEED_ALWAYS))
    {
        if (itr->GetId() == 68992) // Darkflight doesn't stack with...
            if (nonStackBuffs.find(mainModifierId) != nonStackBuffs.end())
                continue;

        if (!sSpellMgr->AddSameEffectStackRuleSpellGroups(itr->GetSpellInfo(), itr->GetAmount(), sameEffectSpellGroup))
            AddPct(multiplier, itr->GetAmount());
    }

    for (auto&& itr : sameEffectSpellGroup)
        AddPct(multiplier, itr.second);


    mainBonus = mainModifier;
    stackBonus = multiplier;
    nonStackBonus += nonStackModifier / 100.0f;
}

uint32 Unit::RecaculateDamage(uint32 damage) // Here we calculate the damage done
{
    if (GetTypeId() == TYPEID_PLAYER)
    {
        switch (ToPlayer()->GetSpecialization())
        {
            // MAGE
        case SPEC_MAGE_ARCANE:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECMAGEARCANE);
            break;
        case SPEC_MAGE_FIRE:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECMAGEFIRE);
            break;
        case SPEC_MAGE_FROST:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECMAGEFROST);
            break;
            // PALADIN
        case SPEC_PALADIN_HOLY:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECPALADINHOLY);
            break;
        case SPEC_PALADIN_PROTECTION:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECPALADINPROTECTION);
            break;
        case SPEC_PALADIN_RETRIBUTION:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECPALADINRETRIBUTION);
            break;
            // WARRIOR
        case SPEC_WARRIOR_ARMS:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECWARRIORARMS);
            break;
        case SPEC_WARRIOR_FURY:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECWARRIORFURY);
            break;
        case SPEC_WARRIOR_PROTECTION:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECWARRIORPROTECTION);
            break;
            // DRUID
        case SPEC_DRUID_BALANCE:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECDRUIDBALANCE);
            break;
        case SPEC_DRUID_FERAL:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECDRUIDFERAL);
            break;
        case SPEC_DRUID_GUARDIAN:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECDRUIDGUARDIAN);
            break;
        case SPEC_DRUID_RESTORATION:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECDRUIDRESTORATION);
            break;
            // DEATH KNIGHT
        case SPEC_DEATH_KNIGHT_BLOOD:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECDEATHKNIGHTBLOOD);
            break;
        case SPEC_DEATH_KNIGHT_FROST:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECDEATHKNIGHTFROST);
            break;
        case SPEC_DEATH_KNIGHT_UNHOLY:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECDEATHKNIGHTUNHOLY);
            break;
            // HUNTER
        case SPEC_HUNTER_BEAST_MASTERY:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECHUNTERBEASTMASTERY);
            break;
        case SPEC_HUNTER_MARKSMANSHIP:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECHUNTERMARKSMANSHIP);
            break;
        case SPEC_HUNTER_SURVIVAL:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECHUNTERSURVIVAL);
            break;
            //PRIEST
        case SPEC_PRIEST_DISCIPLINE:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECPRIESTDISCIPLINE);
            break;
        case SPEC_PRIEST_HOLY:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECPRIESTHOLY);
            break;
        case SPEC_PRIEST_SHADOW:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECPRIESTSHADOW);
            break;
            // ROGUE
        case SPEC_ROGUE_ASSASSINATION:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECROGUEASSASSINATION);
            break;
        case SPEC_ROGUE_COMBAT:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECROGUECOMBAT);
            break;
        case SPEC_ROGUE_SUBTLETY:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECROGUESUBTLETY);
            break;
            //SHAMAN
        case SPEC_SHAMAN_ELEMENTAL:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECSHAMANELEMENTAL);
            break;
        case SPEC_SHAMAN_ENHANCEMENT:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECSHAMANENHANCEMENT);
            break;
        case SPEC_SHAMAN_RESTORATION:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECSHAMANRESTORATION);
            break;
            //WARLOCK
        case SPEC_WARLOCK_AFFLICTION:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECWARLOCKAFFLICTION);
            break;
        case SPEC_WARLOCK_DEMONOLOGY:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECWARLOCKDEMONOLOGY);
            break;
        case SPEC_WARLOCK_DESTRUCTION:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECWARLOCKDESTRUCTION);
            break;
            // MONK
        case SPEC_MONK_BREWMASTER:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECMONKBREWMASTER);
            break;
        case SPEC_MONK_WINDWALKER:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECMONKWINDWALKER);
            break;
        case SPEC_MONK_MISTWEAVER:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIGSPECMONKMISTWEAVER);
            break;
        default:
            break;
        }
    }
    else if (ToCreature()->IsPet())
    {
    switch (ToPet()->GetSpecializationId())
    {
        case SPEC_PET_FEROCITY:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIG_SPEC_PET_FEROCITY);
            break;
        case SPEC_PET_TENACITY:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIG_SPEC_PET_TENACITY);
            break;
        case SPEC_PET_CUNNING:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIG_SPEC_PET_CUNNING);
            break;
        default:
            damage = (damage / 100.0f) * sWorld->getFloatConfig(CONFIG_PET_NO_SPEC);
            break;
        }
    }

    return damage;
}

uint32 Unit::RecaculateHealing(uint32 healamount) // calculate healing here
{
    // Custom Balance System For TorghastWOW 
    if (GetTypeId() == TYPEID_PLAYER)
    {
        switch (ToPlayer()->GetSpecialization())
        {
            // MAGE
        case SPEC_MAGE_ARCANE:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECMAGEARCANE);
            break;
        case SPEC_MAGE_FIRE:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECMAGEFIRE);
            break;
        case SPEC_MAGE_FROST:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECMAGEFROST);
            break;
            // PALADIN
        case SPEC_PALADIN_HOLY:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECPALADINHOLY);
            break;
        case SPEC_PALADIN_PROTECTION:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECPALADINPROTECTION);
            break;
        case SPEC_PALADIN_RETRIBUTION:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECPALADINRETRIBUTION);
            break;
            // WARRIOR
        case SPEC_WARRIOR_ARMS:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECWARRIORARMS);
            break;
        case SPEC_WARRIOR_FURY:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECWARRIORFURY);
            break;
        case SPEC_WARRIOR_PROTECTION:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECWARRIORPROTECTION);
            break;
            // DRUID
        case SPEC_DRUID_BALANCE:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECDRUIDBALANCE);
            break;
        case SPEC_DRUID_FERAL:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECDRUIDFERAL);
            break;
        case SPEC_DRUID_GUARDIAN:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECDRUIDGUARDIAN);
            break;
        case SPEC_DRUID_RESTORATION:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECDRUIDRESTORATION);
            break;
            // DEATH KNIGHT
        case SPEC_DEATH_KNIGHT_BLOOD:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECDEATHKNIGHTBLOOD);
            break;
        case SPEC_DEATH_KNIGHT_FROST:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECDEATHKNIGHTFROST);
            break;
        case SPEC_DEATH_KNIGHT_UNHOLY:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECDEATHKNIGHTUNHOLY);
            break;
            // HUNTER
        case SPEC_HUNTER_BEAST_MASTERY:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECHUNTERBEASTMASTERY);
            break;
        case SPEC_HUNTER_MARKSMANSHIP:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECHUNTERMARKSMANSHIP);
            break;
        case SPEC_HUNTER_SURVIVAL:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECHUNTERSURVIVAL);
            break;
            //PRIEST
        case SPEC_PRIEST_DISCIPLINE:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECPRIESTDISCIPLINE);
            break;
        case SPEC_PRIEST_HOLY:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECPRIESTHOLY);
            break;
        case SPEC_PRIEST_SHADOW:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECPRIESTSHADOW);
            break;
            // ROGUE
        case SPEC_ROGUE_ASSASSINATION:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECROGUEASSASSINATION);
            break;
        case SPEC_ROGUE_COMBAT:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECROGUECOMBAT);
            break;
        case SPEC_ROGUE_SUBTLETY:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECROGUESUBTLETY);
            break;
            //SHAMAN
        case SPEC_SHAMAN_ELEMENTAL:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECSHAMANELEMENTAL);
            break;
        case SPEC_SHAMAN_ENHANCEMENT:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECSHAMANENHANCEMENT);
            break;
        case SPEC_SHAMAN_RESTORATION:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECSHAMANRESTORATION);
            break;
            //WARLOCK
        case SPEC_WARLOCK_AFFLICTION:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECWARLOCKAFFLICTION);
            break;
        case SPEC_WARLOCK_DEMONOLOGY:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECWARLOCKDEMONOLOGY);
            break;
        case SPEC_WARLOCK_DESTRUCTION:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECWARLOCKDESTRUCTION);
            break;
            // MONK
        case SPEC_MONK_BREWMASTER:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECMONKBREWMASTER);
            break;
        case SPEC_MONK_WINDWALKER:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECMONKWINDWALKER);
            break;
        case SPEC_MONK_MISTWEAVER:
            healamount = (healamount / 100.0f) * sWorld->getFloatConfig(HEALCONFIGSPECMONKMISTWEAVER);
            break;
        }
    }
    return healamount;
}

void Unit::HandleEmoteCommandWithDelay(uint32 Delay, uint8 Id)
{
    class HandleEmoteCommandWithDelay : public BasicEvent
    {
        public:
            HandleEmoteCommandWithDelay(Unit* _creature, uint8 _id, uint32 const& _delay) : creature(_creature), id(_id), delay(_delay) { }

        bool Execute(uint64 /*execTime*/, uint32 /*diff*/)
        {
            sCreatureTextMgr->SendEmote(creature, id);
            return true;
        }

        private:
            Unit* creature;
            uint8 id;
            uint32 delay;
    };

    m_Events.AddEvent(new HandleEmoteCommandWithDelay(this, Id, Delay), m_Events.CalculateTime(Delay));
}

void Unit::CastWithDelay(uint32 delay, Unit* victim, uint32 spellid, bool triggered, bool repeat)
{
    class CastDelayEvent : public BasicEvent
    {
    public:
        CastDelayEvent(Unit* _me, ObjectGuid _victimGuid, uint32 const& _spellId, bool const& _triggered, bool const& _repeat, uint32 const& _delay) :
          me(_me), victimGuid(_victimGuid), spellId(_spellId), triggered(_triggered), repeat(_repeat), delay(_delay) { }

        bool Execute(uint64 /*execTime*/, uint32 /*diff*/)
        {
            Unit* victim = ObjectAccessor::GetUnit(*me, victimGuid);
            me->CastSpell(victim, spellId, triggered);

            if (repeat)
                me->CastWithDelay(delay, victim, spellId, triggered, repeat);
            return true;
        }

    private:
        Unit* me;
        ObjectGuid victimGuid;
        uint32 const spellId;
        uint32 const delay;
        bool const triggered;
        bool const repeat;
    };

    m_Events.AddEvent(new CastDelayEvent(this, victim ? victim->GetGUID() : ObjectGuid::Empty, spellid, triggered, repeat, delay), m_Events.CalculateTime(delay));
}

void Unit::JumpWithDelay(uint32 delay, float x, float y, float z, float speedXY, float speedZ, uint32 id)
{
    struct DelayJumpEvent : public BasicEvent
    {
        DelayJumpEvent(Unit *unit, float x, float y, float z, float speedXY, float speedZ, uint32 id) : BasicEvent(), _unit(unit), _x(x), _y(y), _z(z), _speedXY(speedXY), _speedZ(speedZ), _id(id) { }

        bool Execute(uint64, uint32)
        {
            _unit->ExitVehicle();
            _unit->GetMotionMaster()->MoveJump(_x, _y, _z, _speedXY, _speedZ, _id);
            return true;
        }

    private:
        Unit *_unit;
        float _x;
        float _y;
        float _z;
        float _speedXY;
        float _speedZ;
        uint32 _id;
    };

    m_Events.AddEvent(new DelayJumpEvent(this, x, y, z, speedXY, speedZ, id), m_Events.CalculateTime(delay));
}
