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

#include "Common.h"
#include "WorldPacket.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Opcodes.h"
#include "Log.h"
#include "UpdateMask.h"
#include "World.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Player.h"
#include "Pet.h"
#include "Unit.h"
#include "Totem.h"
#include "Spell.h"
#include "DynamicObject.h"
#include "G3DPosition.hpp"
#include "Guild.h"
#include "Group.h"
#include "UpdateData.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "CellImpl.h"
#include "SharedDefines.h"
#include "LootMgr.h"
#include "VMapFactory.h"
#include "Battleground.h"
#include "Util.h"
#include "TemporarySummon.h"
#include "Vehicle.h"
#include "SpellAuraEffects.h"
#include "ScriptMgr.h"
#include "ConditionMgr.h"
#include "DisableMgr.h"
#include "SpellScript.h"
#include "InstanceScript.h"
#include "SpellInfo.h"
#include "DB2Stores.h"
#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "SpellHistory.h"
#include "LootLockoutMap.h"
#include "BattlePetMgr.h"
#include "TradeData.h"
#include "Random.h"
#include "VMapManager2.h"

extern pEffect SpellEffects[TOTAL_SPELL_EFFECTS];

SpellDestination::SpellDestination()
{
    _position.Relocate(0, 0, 0, 0);
    _transportGUID = ObjectGuid::Empty;
    _transportOffset.Relocate(0, 0, 0, 0);
}

SpellDestination::SpellDestination(float x, float y, float z, float orientation, uint32 mapId)
{
    _position.Relocate(x, y, z, orientation);
    _transportGUID = ObjectGuid::Empty;
    _position.m_mapId = mapId;
}

SpellDestination::SpellDestination(Position const& pos)
{
    _position.Relocate(pos);
    _transportGUID = ObjectGuid::Empty;
}

void SpellDestination::Relocate(Position const& pos)
{
    if (_transportGUID)
    {
        Position offset;
        _position.GetPositionOffsetTo(pos, offset);
        _transportOffset.RelocateOffset(offset);
    }
    _position.Relocate(pos);
}

void SpellDestination::RelocateOffset(Position const& offset)
{
    if (_transportGUID)
        _transportOffset.RelocateOffset(offset);

    _position.RelocateOffset(offset);
}

SpellDestination::SpellDestination(WorldObject const& wObj)
{
    _transportGUID = wObj.GetTransGUID();
    _transportOffset.Relocate(wObj.GetTransOffsetX(), wObj.GetTransOffsetY(), wObj.GetTransOffsetZ(), wObj.GetTransOffsetO());
    _position.Relocate(wObj);
    _position.SetOrientation(wObj.GetOrientation());
}

SpellCastTargets::SpellCastTargets() : m_elevation(0), m_speed(0), m_strTarget()
{
    m_objectTarget = nullptr;
    m_itemTarget = nullptr;

    m_objectTargetGUID   = ObjectGuid::Empty;
    m_itemTargetGUID   = ObjectGuid::Empty;
    m_itemTargetEntry  = 0;

    m_targetMask = 0;
}

SpellCastTargets::SpellCastTargets(Unit* caster, uint32 targetMask, ObjectGuid targetGuid, ObjectGuid itemTargetGuid, ObjectGuid srcTransportGuid, ObjectGuid destTransportGuid, Position srcPos, Position destPos, float elevation, float missileSpeed, std::string targetString) :
    m_targetMask(targetMask), m_objectTargetGUID(targetGuid), m_itemTargetGUID(itemTargetGuid), m_elevation(elevation), m_speed(missileSpeed), m_strTarget(targetString)
{
    m_objectTarget = nullptr;
    m_itemTarget = nullptr;
    m_itemTargetEntry = 0;

    m_src._transportGUID = srcTransportGuid;
    if (m_src._transportGUID)
        m_src._transportOffset.Relocate(srcPos);
    else
        m_src._position.Relocate(srcPos);

    m_dst._transportGUID = destTransportGuid;
    if (m_dst._transportGUID)
        m_dst._transportOffset.Relocate(destPos);
    else
        m_dst._position.Relocate(destPos);

    Update(caster);
}

SpellCastTargets::~SpellCastTargets() { }

void SpellCastTargets::Read(ByteBuffer& data, Unit* caster)
{
    data >> m_targetMask;

    if (m_targetMask == TARGET_FLAG_NONE)
        return;

    if (m_targetMask & (TARGET_FLAG_UNIT | TARGET_FLAG_UNIT_MINIPET | TARGET_FLAG_GAMEOBJECT | TARGET_FLAG_CORPSE_ENEMY | TARGET_FLAG_CORPSE_ALLY))
    {
        uint64 obj;
        data.readPackGUID(obj);
        m_objectTargetGUID.Set(obj);
    }

    if (m_targetMask & (TARGET_FLAG_ITEM | TARGET_FLAG_TRADE_ITEM))
    {
        uint64 item;
        data.readPackGUID(item);
        m_itemTargetGUID.Set(item);
    }

    if (m_targetMask & TARGET_FLAG_SOURCE_LOCATION)
    {
        uint64 tg;
        data.readPackGUID(tg);
        m_src._transportGUID.Set(tg);
        if (m_src._transportGUID)
            data >> m_src._transportOffset.PositionXYZStream();
        else
            data >> m_src._position.PositionXYZStream();
    }
    else
    {
        m_src._transportGUID = caster->GetTransGUID();
        if (m_src._transportGUID)
            m_src._transportOffset.Relocate(caster->GetTransOffsetX(), caster->GetTransOffsetY(), caster->GetTransOffsetZ(), caster->GetTransOffsetO());
        else
            m_src._position.Relocate(caster);
    }

    if (m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        uint64 tg;
        data.readPackGUID(tg);
        m_dst._transportGUID.Set(tg);
        if (m_dst._transportGUID)
            data >> m_dst._transportOffset.PositionXYZStream();
        else
            data >> m_dst._position.PositionXYZStream();
    }
    else
    {
        m_dst._transportGUID = caster->GetTransGUID();
        if (m_dst._transportGUID)
            m_dst._transportOffset.Relocate(caster->GetTransOffsetX(), caster->GetTransOffsetY(), caster->GetTransOffsetZ(), caster->GetTransOffsetO());
        else
            m_dst._position.Relocate(caster);
    }

    if (m_targetMask & TARGET_FLAG_STRING)
        data >> m_strTarget;

    Update(caster);
}

void SpellCastTargets::Write(ByteBuffer& data)
{
    data << uint32(m_targetMask);

    if (m_targetMask & (TARGET_FLAG_UNIT | TARGET_FLAG_CORPSE_ALLY | TARGET_FLAG_GAMEOBJECT | TARGET_FLAG_CORPSE_ENEMY | TARGET_FLAG_UNIT_MINIPET))
        data.appendPackGUID(m_objectTargetGUID);

    if (m_targetMask & (TARGET_FLAG_ITEM | TARGET_FLAG_TRADE_ITEM))
    {
        if (m_itemTarget)
            data << m_itemTarget->GetPackGUID();
        else
            data << uint8(0);
    }

    if (m_targetMask & TARGET_FLAG_SOURCE_LOCATION)
    {
        data.appendPackGUID(m_src._transportGUID); // relative position guid here - transport for example
        if (m_src._transportGUID)
            data << m_src._transportOffset.PositionXYZStream();
        else
            data << m_src._position.PositionXYZStream();
    }

    if (m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        data.appendPackGUID(m_dst._transportGUID); // relative position guid here - transport for example
        if (m_dst._transportGUID)
            data << m_dst._transportOffset.PositionXYZStream();
        else
            data << m_dst._position.PositionXYZStream();
    }

    if (m_targetMask & TARGET_FLAG_STRING)
        data << m_strTarget;
}

ObjectGuid SpellCastTargets::GetUnitTargetGUID() const
{
    switch (m_objectTargetGUID.GetHigh())
    {
        case HighGuid::Player:
        case HighGuid::Vehicle:
        case HighGuid::Unit:
        case HighGuid::Pet:
            return m_objectTargetGUID;
        default:
            return ObjectGuid::Empty;
    }
}

Unit* SpellCastTargets::GetUnitTarget() const
{
    if (m_objectTarget)
        return m_objectTarget->ToUnit();
    return NULL;
}

void SpellCastTargets::SetUnitTarget(Unit* target)
{
    if (!target)
        return;

    m_objectTarget = target;
    m_objectTargetGUID = target->GetGUID();
    m_targetMask |= TARGET_FLAG_UNIT;
}

ObjectGuid SpellCastTargets::GetGOTargetGUID() const
{
    switch (m_objectTargetGUID.GetHigh())
    {
        case HighGuid::Transport:
        case HighGuid::Mo_Transport:
        case HighGuid::GameObject:
            return m_objectTargetGUID;
        default:
            return ObjectGuid::Empty;
    }
}

GameObject* SpellCastTargets::GetGOTarget() const
{
    if (m_objectTarget)
        return m_objectTarget->ToGameObject();
    return NULL;
}

void SpellCastTargets::SetGOTarget(GameObject* target)
{
    if (!target)
        return;

    m_objectTarget = target;
    m_objectTargetGUID = target->GetGUID();
    m_targetMask |= TARGET_FLAG_GAMEOBJECT;
}

ObjectGuid SpellCastTargets::GetCorpseTargetGUID() const
{
    switch (m_objectTargetGUID.GetHigh())
    {
        case HighGuid::Corpse:
            return m_objectTargetGUID;
        default:
            return ObjectGuid::Empty;
    }
}

Corpse* SpellCastTargets::GetCorpseTarget() const
{
    if (m_objectTarget)
        return m_objectTarget->ToCorpse();
    return NULL;
}

WorldObject* SpellCastTargets::GetObjectTarget() const
{
    return m_objectTarget;
}

ObjectGuid SpellCastTargets::GetObjectTargetGUID() const
{
    return m_objectTargetGUID;
}

void SpellCastTargets::RemoveObjectTarget()
{
    m_objectTarget = NULL;
    m_objectTargetGUID = ObjectGuid::Empty;
    m_targetMask &= ~(TARGET_FLAG_UNIT_MASK | TARGET_FLAG_CORPSE_MASK | TARGET_FLAG_GAMEOBJECT_MASK);
}

void SpellCastTargets::SetItemTarget(Item* item)
{
    if (!item)
        return;

    m_itemTarget = item;
    m_itemTargetGUID = item->GetGUID();
    m_itemTargetEntry = item->GetEntry();
    m_targetMask |= TARGET_FLAG_ITEM;
}

void SpellCastTargets::SetTradeItemTarget(Player* caster)
{
    m_itemTargetGUID = ObjectGuid(uint64(TRADE_SLOT_NONTRADED));
    m_itemTargetEntry = 0;
    m_targetMask |= TARGET_FLAG_TRADE_ITEM;

    Update(caster);
}

void SpellCastTargets::UpdateTradeSlotItem()
{
    if (m_itemTarget && (m_targetMask & TARGET_FLAG_TRADE_ITEM))
    {
        m_itemTargetGUID = m_itemTarget->GetGUID();
        m_itemTargetEntry = m_itemTarget->GetEntry();
    }
}

SpellDestination const* SpellCastTargets::GetSrc() const
{
    return &m_src;
}

Position const* SpellCastTargets::GetSrcPos() const
{
    return &m_src._position;
}

void SpellCastTargets::SetSrc(float x, float y, float z)
{
    m_src = SpellDestination(x, y, z);
    m_targetMask |= TARGET_FLAG_SOURCE_LOCATION;
}

void SpellCastTargets::SetSrc(Position const& pos)
{
    m_src = SpellDestination(pos);
    m_targetMask |= TARGET_FLAG_SOURCE_LOCATION;
}

void SpellCastTargets::SetSrc(WorldObject const& wObj)
{
    m_src = SpellDestination(wObj);
    m_targetMask |= TARGET_FLAG_SOURCE_LOCATION;
}

void SpellCastTargets::ModSrc(Position const& pos)
{
    ASSERT(m_targetMask & TARGET_FLAG_SOURCE_LOCATION);

    if (m_src._transportGUID)
    {
        Position offset;
        m_src._position.GetPositionOffsetTo(pos, offset);
        m_src._transportOffset.RelocateOffset(offset);
    }
    m_src._position.Relocate(pos);
}

void SpellCastTargets::RemoveSrc()
{
    m_targetMask &= ~(TARGET_FLAG_SOURCE_LOCATION);
}

SpellDestination const* SpellCastTargets::GetDst() const
{
    return &m_dst;
}

WorldLocation const* SpellCastTargets::GetDstPos() const
{
    return &m_dst._position;
}

void SpellCastTargets::SetDst(float x, float y, float z, float orientation, uint32 mapId)
{
    m_dst = SpellDestination(x, y, z, orientation, mapId);
    m_targetMask |= TARGET_FLAG_DEST_LOCATION;
}

void SpellCastTargets::SetDst(Position const& pos)
{
    m_dst = SpellDestination(pos);
    m_targetMask |= TARGET_FLAG_DEST_LOCATION;
}

void SpellCastTargets::SetDst(WorldObject const& wObj)
{
    m_dst = SpellDestination(wObj);
    m_targetMask |= TARGET_FLAG_DEST_LOCATION;
}

void SpellCastTargets::SetDst(SpellDestination const& spellDest)
{
    m_dst = spellDest;
    m_targetMask |= TARGET_FLAG_DEST_LOCATION;
}

void SpellCastTargets::SetDst(SpellCastTargets const& spellTargets)
{
    m_dst = spellTargets.m_dst;
    m_targetMask |= TARGET_FLAG_DEST_LOCATION;
}

void SpellCastTargets::ModDst(Position const& pos)
{
    ASSERT(m_targetMask & TARGET_FLAG_DEST_LOCATION);

    if (m_dst._transportGUID)
    {
        Position offset;
        m_dst._position.GetPositionOffsetTo(pos, offset);
        m_dst._transportOffset.RelocateOffset(offset);
    }
    m_dst._position.Relocate(pos);
}

void SpellCastTargets::ModDst(SpellDestination const& spellDest)
{
    ASSERT(m_targetMask & TARGET_FLAG_DEST_LOCATION);
    m_dst = spellDest;
}

void SpellCastTargets::RemoveDst()
{
    m_targetMask &= ~(TARGET_FLAG_DEST_LOCATION);
}

void SpellCastTargets::Update(Unit* caster)
{
    m_objectTarget = m_objectTargetGUID ? ((m_objectTargetGUID == caster->GetGUID()) ? caster : ObjectAccessor::GetWorldObject(*caster, m_objectTargetGUID)) : NULL;

    m_itemTarget = NULL;
    if (caster->GetTypeId() == TYPEID_PLAYER)
    {
        Player* player = caster->ToPlayer();
        if (m_targetMask & TARGET_FLAG_ITEM)
            m_itemTarget = player->GetItemByGuid(m_itemTargetGUID);
        else if (m_targetMask & TARGET_FLAG_TRADE_ITEM)
            if (m_itemTargetGUID == TRADE_SLOT_NONTRADED) // here it is not guid but slot. Also prevents hacking slots
                if (TradeData* pTrade = player->GetTradeData())
                    m_itemTarget = pTrade->GetTraderData()->GetItem(TRADE_SLOT_NONTRADED);

        if (m_itemTarget)
            m_itemTargetEntry = m_itemTarget->GetEntry();
    }

    // update positions by transport move
    if (HasSrc() && m_src._transportGUID)
    {
        if (WorldObject* transport = ObjectAccessor::GetWorldObject(*caster, m_src._transportGUID))
        {
            m_src._position.Relocate(transport);
            m_src._position.RelocateOffset(m_src._transportOffset);
        }
    }

    if (HasDst() && m_dst._transportGUID)
    {
        if (WorldObject* transport = ObjectAccessor::GetWorldObject(*caster, m_dst._transportGUID))
        {
            m_dst._position.Relocate(transport);
            m_dst._position.RelocateOffset(m_dst._transportOffset);
        }
    }
}

void SpellCastTargets::OutDebug() const
{
    if (!m_targetMask)
        TC_LOG_INFO("spells", "No targets");

    TC_LOG_INFO("spells", "target mask: %u", m_targetMask);
    if (m_targetMask & (TARGET_FLAG_UNIT_MASK | TARGET_FLAG_CORPSE_MASK | TARGET_FLAG_GAMEOBJECT_MASK))
        TC_LOG_INFO("spells", "Object target: " UI64FMTD, m_objectTargetGUID.GetRawValue());
    if (m_targetMask & TARGET_FLAG_ITEM)
        TC_LOG_INFO("spells", "Item target: " UI64FMTD, m_itemTargetGUID.GetRawValue());
    if (m_targetMask & TARGET_FLAG_TRADE_ITEM)
        TC_LOG_INFO("spells", "Trade item target: " UI64FMTD, m_itemTargetGUID.GetRawValue());
    if (m_targetMask & TARGET_FLAG_SOURCE_LOCATION)
        TC_LOG_INFO("spells", "Source location: transport guid:" UI64FMTD " trans offset: %s position: %s", m_src._transportGUID.GetRawValue(), m_src._transportOffset.ToString().c_str(), m_src._position.ToString().c_str());
    if (m_targetMask & TARGET_FLAG_DEST_LOCATION)
        TC_LOG_INFO("spells", "Destination location: transport guid:" UI64FMTD " trans offset: %s position: %s", m_dst._transportGUID.GetRawValue(), m_dst._transportOffset.ToString().c_str(), m_dst._position.ToString().c_str());
    if (m_targetMask & TARGET_FLAG_STRING)
        TC_LOG_INFO("spells", "String: %s", m_strTarget.c_str());
    TC_LOG_INFO("spells", "speed: %f", m_speed);
    TC_LOG_INFO("spells", "elevation: %f", m_elevation);
}

SpellValue::SpellValue(SpellInfo const* proto)
{
    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        EffectBasePoints[i] = proto->Effects[i].BasePoints;
    MaxAffectedTargets = proto->MaxAffectedTargets;
    RadiusMod = 1.0f;
    AuraStackAmount = 1;
}

Spell::Spell(Unit* caster, SpellInfo const* info, TriggerCastFlags triggerFlags, ObjectGuid originalCasterGUID, bool skipCheck) :
m_spellInfo(sSpellMgr->GetSpellForDifficultyFromSpell(info, caster)),
m_caster((info->AttributesEx6 & SPELL_ATTR6_CAST_BY_CHARMER && caster->GetCharmerOrOwner()) ? caster->GetCharmerOrOwner() : caster),
m_spellValue(new SpellValue(m_spellInfo)), m_researchData(NULL)
{
    m_customError = SPELL_CUSTOM_ERROR_NONE;
    m_skipCheck = skipCheck;
    m_selfContainer = NULL;
    m_referencedFromCurrentSpell = false;
    m_executedCurrently = false;
    m_needComboPoints = m_spellInfo->NeedsComboPoints();
    m_comboPointGain = 0;
    m_delayStart = 0;
    m_delayAtDamageCount = 0;

    m_applyMultiplierMask = 0;
    m_auraScaleMask = 0;

    // Get data for type of attack
    switch (m_spellInfo->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_MELEE:
            if (m_spellInfo->AttributesEx3 & SPELL_ATTR3_REQ_OFFHAND)
                m_attackType = OFF_ATTACK;
            else
                m_attackType = BASE_ATTACK;
            break;
        case SPELL_DAMAGE_CLASS_RANGED:
            m_attackType = m_spellInfo->IsRangedWeaponSpell() ? RANGED_ATTACK : BASE_ATTACK;
            break;
        default:
                                                            // Wands
            if (m_spellInfo->AttributesEx2 & SPELL_ATTR2_AUTOREPEAT_FLAG)
                m_attackType = RANGED_ATTACK;
            else
                m_attackType = BASE_ATTACK;
            break;
    }

    m_spellSchoolMask = info->GetSchoolMask();           // Can be override for some spell (wand shoot for example)

    if (m_attackType == RANGED_ATTACK)
        // wand case
        if ((m_caster->GetClassMask() & CLASSMASK_WAND_USERS) != 0 && m_caster->GetTypeId() == TYPEID_PLAYER)
            if (Item* pItem = m_caster->ToPlayer()->GetWeaponForAttack(RANGED_ATTACK))
                m_spellSchoolMask = SpellSchoolMask(1 << pItem->GetTemplate()->DamageType);

    if (originalCasterGUID)
        m_originalCasterGUID = originalCasterGUID;
    else
        m_originalCasterGUID = m_caster->GetGUID();

    if (m_originalCasterGUID == m_caster->GetGUID())
        m_originalCaster = m_caster;
    else
    {
        m_originalCaster = ObjectAccessor::GetUnit(*m_caster, m_originalCasterGUID);
        if (m_originalCaster && !m_originalCaster->IsInWorld())
            m_originalCaster = NULL;
    }

    m_spellState = SPELL_STATE_NULL;
    _triggeredCastFlags = triggerFlags;
    if (info->AttributesEx4 & SPELL_ATTR4_TRIGGERED)
        _triggeredCastFlags = TriggerCastFlags(_triggeredCastFlags | TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_IGNORE_AURA_INTERRUPT_FLAGS);

    m_CastItem = NULL;
    m_castItemGUID = ObjectGuid::Empty;

    unitTarget = NULL;
    itemTarget = NULL;
    gameObjTarget = NULL;
    focusObject = NULL;
    m_cast_count = 0;
    m_glyphIndex = 0;
    m_preCastSpell = 0;
    m_triggeredByAuraSpell  = NULL;
    m_spellAura = NULL;

    //Auto Shot & Shoot (wand)
    m_autoRepeat = m_spellInfo->IsAutoRepeatRangedSpell();

    m_runesState = 0;
    m_casttime = 0;                                         // setup to correct value in Spell::prepare, must not be used before.
    m_timer = 0;                                            // will set to castime in prepare

    m_channelTargetEffectMask = 0;

    // Determine if spell can be reflected back to the caster
    // Patch 1.2 notes: Spell Reflection no longer reflects abilities
    m_canReflect = m_spellInfo->DmgClass == SPELL_DAMAGE_CLASS_MAGIC && !(m_spellInfo->Attributes & SPELL_ATTR0_ABILITY)
        && !(m_spellInfo->AttributesEx & SPELL_ATTR1_CANT_BE_REFLECTED) && !(m_spellInfo->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)
        && !m_spellInfo->IsPassive() && !m_spellInfo->IsPositive();

    CleanupTargetList();
    m_effectExecuteData.clear();

    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        m_destTargets[i] = SpellDestination(*m_caster);

    m_dropModsPhase = SpellModDropPhase::OnSpellFinish;

    if (GetCaster()->GetClass() == CLASS_DEATH_KNIGHT)
        if (AuraEffect const* effect = GetCaster()->GetAuraEffect(77616, EFFECT_0))
            m_darkSimulacrum = effect->GetAmount() == GetSpellInfo()->Id;
}

Spell::~Spell()
{
    // unload scripts
    while (!m_loadedScripts.empty())
    {
        std::list<SpellScript*>::iterator itr = m_loadedScripts.begin();
        (*itr)->_Unload();
        delete (*itr);
        m_loadedScripts.erase(itr);
    }

    if (m_referencedFromCurrentSpell && m_selfContainer && *m_selfContainer == this)
    {
        // Clean the reference to avoid later crash.
        // If this error is repeating, we may have to add an ASSERT to better track down how we get into this case.
        TC_LOG_ERROR("spells", "SPELL: deleting spell for spell ID %u. However, spell still referenced.", m_spellInfo->Id);
        *m_selfContainer = NULL;
    }

    if (m_caster && m_caster->GetTypeId() == TYPEID_PLAYER)
        ASSERT(m_caster->ToPlayer()->m_spellModTakingSpell != this);

    delete m_spellValue;
    delete m_researchData;
}

void Spell::InitExplicitTargets(SpellCastTargets const& targets)
{
    m_targets = targets;
    // this function tries to correct spell explicit targets for spell
    // client doesn't send explicit targets correctly sometimes - we need to fix such spells serverside
    // this also makes sure that we correctly send explicit targets to client (removes redundant data)
    uint32 neededTargets = m_spellInfo->GetExplicitTargetMask();

    if (WorldObject* target = m_targets.GetObjectTarget())
    {
        // check if object target is valid with needed target flags
        // for unit case allow corpse target mask because player with not released corpse is a unit target
        if ((target->ToUnit() && !(neededTargets & (TARGET_FLAG_UNIT_MASK | TARGET_FLAG_CORPSE_MASK)))
            || (target->ToGameObject() && !(neededTargets & TARGET_FLAG_GAMEOBJECT_MASK))
            || (target->ToCorpse() && !(neededTargets & TARGET_FLAG_CORPSE_MASK)))
            if (!m_spellInfo->HasPersistenAura() || (m_spellInfo->HasPersistenAura() && !(m_targets.GetTargetMask() & TARGET_FLAG_UNIT_MASK)))
            m_targets.RemoveObjectTarget();
    }
    else
    {
        // try to select correct unit target if not provided by client or by serverside cast
        if (neededTargets & (TARGET_FLAG_UNIT_MASK))
        {
            Unit* unit = NULL;
            // try to use player selection as a target
            if (Player* playerCaster = m_caster->ToPlayer())
            {
                // selection has to be found and to be valid target for the spell
                if (Unit* selectedUnit = ObjectAccessor::GetUnit(*m_caster, playerCaster->GetTarget()))
                    if (m_spellInfo->CheckExplicitTarget(m_caster, selectedUnit) == SPELL_CAST_OK)
                        unit = selectedUnit;
            }
            // try to use attacked unit as a target
            else if ((m_caster->GetTypeId() == TYPEID_UNIT) && neededTargets & (TARGET_FLAG_UNIT_ENEMY | TARGET_FLAG_UNIT))
                unit = m_caster->GetVictim();

            // didn't find anything - let's use self as target
            if (!unit && neededTargets & (TARGET_FLAG_UNIT_RAID | TARGET_FLAG_UNIT_PARTY | TARGET_FLAG_UNIT_ALLY))
                unit = m_caster;

            m_targets.SetUnitTarget(unit);
        }
    }

    // check if spell needs dst target
    if (neededTargets & TARGET_FLAG_DEST_LOCATION)
    {
        // and target isn't set
        if (!m_targets.HasDst())
        {
            // try to use unit target if provided
            if (WorldObject* target = targets.GetObjectTarget())
                m_targets.SetDst(*target);
            // or use self if not available
            else
                m_targets.SetDst(*m_caster);
        }
    }
    else
        m_targets.RemoveDst();

    if (neededTargets & TARGET_FLAG_SOURCE_LOCATION)
    {
        if (!targets.HasSrc())
            m_targets.SetSrc(*m_caster);
    }
    else
        m_targets.RemoveSrc();
}

void Spell::SelectExplicitTargets()
{
    // here go all explicit target changes made to explicit targets after spell prepare phase is finished
    if (Unit* target = m_targets.GetUnitTarget())
    {
        // check for explicit target redirection, for Grounding Totem for example
        if (m_spellInfo->GetExplicitTargetMask() & TARGET_FLAG_UNIT_ENEMY
            || (m_spellInfo->GetExplicitTargetMask() & TARGET_FLAG_UNIT && !m_spellInfo->IsPositive()))
        {
            Unit* redirect;
            switch (m_spellInfo->DmgClass)
            {
                case SPELL_DAMAGE_CLASS_MAGIC:
                    redirect = m_caster->GetMagicHitRedirectTarget(target, m_spellInfo);
                    break;
                case SPELL_DAMAGE_CLASS_MELEE:
                case SPELL_DAMAGE_CLASS_RANGED:
                    redirect = m_caster->GetMeleeHitRedirectTarget(target, m_spellInfo);
                    break;
                default:
                    redirect = NULL;
                    break;
            }
            if (redirect && (redirect != target))
            {
                m_targets.SetUnitTarget(redirect);
                m_redirect = true;
            }
        }
    }
}

void Spell::SelectSpellTargets()
{
    // select targets for cast phase
    SelectExplicitTargets();

    uint32 processedAreaEffectsMask = 0;
    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        // not call for empty effect.
        // Also some spells use not used effect targets for store targets for dummy effect in triggered spells
        if (!m_spellInfo->Effects[i].IsEffect())
            continue;

        // set expected type of implicit targets to be sent to client
        uint32 implicitTargetMask = GetTargetFlagMask(m_spellInfo->Effects[i].TargetA.GetObjectType()) | GetTargetFlagMask(m_spellInfo->Effects[i].TargetB.GetObjectType());
        if (implicitTargetMask & TARGET_FLAG_UNIT)
            m_targets.SetTargetFlag(TARGET_FLAG_UNIT);
        if (implicitTargetMask & (TARGET_FLAG_GAMEOBJECT | TARGET_FLAG_GAMEOBJECT_ITEM))
            m_targets.SetTargetFlag(TARGET_FLAG_GAMEOBJECT);

        SelectEffectImplicitTargets(SpellEffIndex(i), m_spellInfo->Effects[i].TargetA, processedAreaEffectsMask);
        SelectEffectImplicitTargets(SpellEffIndex(i), m_spellInfo->Effects[i].TargetB, processedAreaEffectsMask);

        // Select targets of effect based on effect type
        // those are used when no valid target could be added for spell effect based on spell target type
        // some spell effects use explicit target as a default target added to target map (like SPELL_EFFECT_LEARN_SPELL)
        // some spell effects add target to target map only when target type specified (like SPELL_EFFECT_WEAPON)
        // some spell effects don't add anything to target map (confirmed with sniffs) (like SPELL_EFFECT_DESTROY_ALL_TOTEMS)
        SelectEffectTypeImplicitTargets(i);

        if (m_targets.HasDst())
            AddDestTarget(*m_targets.GetDst(), i);

        if (m_spellInfo->IsChanneled())
        {
            uint32 mask = (1 << i);
            for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
            {
                if (ihit->effectMask & mask)
                {
                    m_channelTargetEffectMask |= mask;
                    break;
                }
            }
        }
        else if (m_auraScaleMask)
        {
            bool checkLvl = !m_UniqueTargetInfo.empty();
            for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end();)
            {
                // remove targets which did not pass min level check
                if (m_auraScaleMask && ihit->effectMask == m_auraScaleMask)
                {
                    // Do not check for selfcast
                    if (!ihit->scaleAura && ihit->targetGUID != m_caster->GetGUID())
                    {
                         m_UniqueTargetInfo.erase(ihit++);
                         continue;
                    }
                }
                ++ihit;
            }
            if (checkLvl && m_UniqueTargetInfo.empty())
            {
                SendCastResult(SPELL_FAILED_LOWLEVEL);
                finish(false);
            }
        }
    }

    if (m_targets.HasDst())
    {
        if (m_targets.HasTraj())
        {
            float speed = m_targets.GetSpeedXY();
            if (speed > 0.0f)
                m_delayMoment = (uint64)floor(m_targets.GetDist2d() / speed * 1000.0f);
        }
        else if (m_spellInfo->Speed > 0.0f)
        {
            float dist = m_caster->GetDistance(*m_targets.GetDstPos());
            if (!(m_spellInfo->AttributesEx9 & SPELL_ATTR9_SPECIAL_DELAY_CALCULATION))
                m_delayMoment = uint64(floor(dist / m_spellInfo->Speed * 1000.0f));
            else
                m_delayMoment = uint64(m_spellInfo->Speed * 1000.0f);
        }
    }
}

void Spell::SelectEffectImplicitTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType, uint32& processedEffectMask)
{
    if (!targetType.GetTarget())
        return;

    uint32 effectMask = 1 << effIndex;
    // set the same target list for all effects
    // some spells appear to need this, however this requires more research
    switch (targetType.GetSelectionCategory())
    {
        case TARGET_SELECT_CATEGORY_NEARBY:
        case TARGET_SELECT_CATEGORY_CONE:
        case TARGET_SELECT_CATEGORY_AREA:
            // targets for effect already selected
            if (effectMask & processedEffectMask)
                return;
            // choose which targets we can select at once
            for (uint32 j = effIndex + 1; j < MAX_SPELL_EFFECTS; ++j)
            {
                auto& effects = GetSpellInfo()->Effects;
                if (GetSpellInfo()->IsRequireAdditionalTargetCheck() && effects[j].IsEffect() &&
                    effects[effIndex].TargetA.GetTarget() == effects[j].TargetA.GetTarget() &&
                    effects[effIndex].TargetB.GetTarget() == effects[j].TargetB.GetTarget() &&
                    effects[effIndex].ImplicitTargetConditions == effects[j].ImplicitTargetConditions &&
                    effects[effIndex].CalcRadius(m_caster) == effects[j].CalcRadius(m_caster) &&
                    CheckScriptEffectImplicitTargets(effIndex, j))
                {
                    effectMask |= 1 << j;
                }
            }
            processedEffectMask |= effectMask;
            break;
        default:
            break;
    }

    switch (targetType.GetSelectionCategory())
    {
        case TARGET_SELECT_CATEGORY_CHANNEL:
            SelectImplicitChannelTargets(effIndex, targetType);
            break;
        case TARGET_SELECT_CATEGORY_NEARBY:
            SelectImplicitNearbyTargets(effIndex, targetType, effectMask);
            break;
        case TARGET_SELECT_CATEGORY_CONE:
            SelectImplicitConeTargets(effIndex, targetType, effectMask);
            break;
        case TARGET_SELECT_CATEGORY_AREA:
            SelectImplicitAreaTargets(effIndex, targetType, effectMask);
            break;
        case TARGET_SELECT_CATEGORY_RECIPIENT:
            SelectImplicitRecipientTargets(effIndex, targetType, effectMask);
            break;            
        case TARGET_SELECT_CATEGORY_DEFAULT:
            switch (targetType.GetObjectType())
            {
                case TARGET_OBJECT_TYPE_SRC:
                    switch (targetType.GetReferenceType())
                    {
                        case TARGET_REFERENCE_TYPE_CASTER:
                            m_targets.SetSrc(*m_caster);
                            break;
                        default:
                            ASSERT(false && "Spell::SelectEffectImplicitTargets: received not implemented select target reference type for TARGET_TYPE_OBJECT_SRC");
                            break;
                    }
                    break;
                case TARGET_OBJECT_TYPE_DEST:
                     switch (targetType.GetReferenceType())
                     {
                         case TARGET_REFERENCE_TYPE_CASTER:
                             SelectImplicitCasterDestTargets(effIndex, targetType);
                             break;
                         case TARGET_REFERENCE_TYPE_TARGET:
                             SelectImplicitTargetDestTargets(effIndex, targetType);
                             break;
                         case TARGET_REFERENCE_TYPE_DEST:
                             SelectImplicitDestDestTargets(effIndex, targetType);
                             break;
                         default:
                             ASSERT(false && "Spell::SelectEffectImplicitTargets: received not implemented select target reference type for TARGET_TYPE_OBJECT_DEST");
                             break;
                     }
                     break;
                default:
                    switch (targetType.GetReferenceType())
                    {
                        case TARGET_REFERENCE_TYPE_CASTER:
                            SelectImplicitCasterObjectTargets(effIndex, targetType);
                            break;
                        case TARGET_REFERENCE_TYPE_TARGET:
                            SelectImplicitTargetObjectTargets(effIndex, targetType);
                            break;
                        default:
                            ASSERT(false && "Spell::SelectEffectImplicitTargets: received not implemented select target reference type for TARGET_TYPE_OBJECT");
                            break;
                    }
                    break;
            }
            break;
        case TARGET_SELECT_CATEGORY_NYI:
            TC_LOG_DEBUG("spells", "SPELL: target type %u, found in spellID %u, effect %u is not implemented yet!", m_spellInfo->Id, effIndex, targetType.GetTarget());
            break;
        default:
            ASSERT(false && "Spell::SelectEffectImplicitTargets: received not implemented select target category");
            break;
    }
}

void Spell::SelectImplicitChannelTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType)
{
    if (targetType.GetReferenceType() != TARGET_REFERENCE_TYPE_CASTER)
    {
        ASSERT(false && "Spell::SelectImplicitChannelTargets: received not implemented target reference type");
        return;
    }

    Spell* channeledSpell = m_originalCaster->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
    if (!channeledSpell)
    {
        TC_LOG_DEBUG("spells", "Spell::SelectImplicitChannelTargets: cannot find channel spell for spell ID %u, effect %u", m_spellInfo->Id, effIndex);
        return;
    }
    switch (targetType.GetTarget())
    {
        case TARGET_UNIT_CHANNEL_TARGET:
        {
            WorldObject* target = ObjectAccessor::GetUnit(*m_caster, m_originalCaster->GetGuidValue(UNIT_FIELD_CHANNEL_OBJECT));
            CallScriptObjectTargetSelectHandlers(target, effIndex);
            // unit target may be no longer avalible - teleported out of map for example
            if (target && target->ToUnit())
                AddUnitTarget(target->ToUnit(), 1 << effIndex);
            else
                TC_LOG_DEBUG("spells", "SPELL: cannot find channel spell target for spell ID %u, effect %u", m_spellInfo->Id, effIndex);
            break;
        }
        case TARGET_DEST_CHANNEL_TARGET:
            if (channeledSpell->m_targets.HasDst())
                m_targets.SetDst(channeledSpell->m_targets);
            else if (WorldObject* target = ObjectAccessor::GetWorldObject(*m_caster, m_originalCaster->GetGuidValue(UNIT_FIELD_CHANNEL_OBJECT)))
            {
                CallScriptObjectTargetSelectHandlers(target, effIndex);
                if (target)
                    m_targets.SetDst(*target);
            }
            else
                TC_LOG_DEBUG("spells", "SPELL: cannot find channel spell destination for spell ID %u, effect %u", m_spellInfo->Id, effIndex);
            break;
        case TARGET_DEST_CHANNEL_CASTER:
            m_targets.SetDst(*channeledSpell->GetCaster());
            break;
        default:
            ASSERT(false && "Spell::SelectImplicitChannelTargets: received not implemented target type");
            break;
    }
}

void Spell::SelectImplicitNearbyTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType, uint32 effMask)
{
    if (targetType.GetReferenceType() != TARGET_REFERENCE_TYPE_CASTER)
    {
        ASSERT(false && "Spell::SelectImplicitNearbyTargets: received not implemented target reference type");
        return;
    }

    float range = 0.0f;
    switch (targetType.GetCheckType())
    {
        case TARGET_CHECK_ENEMY:
            range = m_spellInfo->GetMaxRange(false, m_caster, this);
            break;
        case TARGET_CHECK_ALLY:
        case TARGET_CHECK_PARTY:
        case TARGET_CHECK_RAID:
        case TARGET_CHECK_RAID_CLASS:
            range = m_spellInfo->GetMaxRange(true, m_caster, this);
            break;
        case TARGET_CHECK_ENTRY:
        case TARGET_CHECK_DEFAULT:
            range = m_spellInfo->GetMaxRange(m_spellInfo->IsPositive(), m_caster, this);
            break;
        default:
            ASSERT(false && "Spell::SelectImplicitNearbyTargets: received not implemented selection check type");
            break;
    }

    ConditionContainer* condList = m_spellInfo->Effects[effIndex].ImplicitTargetConditions;

    // handle emergency case - try to use other provided targets if no conditions provided
    if (targetType.GetCheckType() == TARGET_CHECK_ENTRY && (!condList || condList->empty()))
    {
        TC_LOG_DEBUG("spells", "Spell::SelectImplicitNearbyTargets: no conditions entry for target with TARGET_CHECK_ENTRY of spell ID %u, effect %u - selecting default targets", m_spellInfo->Id, effIndex);
        switch (targetType.GetObjectType())
        {
            case TARGET_OBJECT_TYPE_GOBJ:
                if (m_spellInfo->RequiresSpellFocus)
                {
                    if (focusObject)
                        AddGOTarget(focusObject, effMask);
                    return;
                }
                break;
            case TARGET_OBJECT_TYPE_DEST:
                if (m_spellInfo->RequiresSpellFocus)
                {
                    if (focusObject)
                        m_targets.SetDst(*focusObject);
                    return;
                }
                break;
            default:
                break;
        }
    }

    WorldObject* target = SearchNearbyTarget(range, targetType, condList);
    if (!target)
    {
        TC_LOG_DEBUG("spells", "Spell::SelectImplicitNearbyTargets: cannot find nearby target for spell ID %u, effect %u", m_spellInfo->Id, effIndex);
        return;
    }

    CallScriptObjectTargetSelectHandlers(target, effIndex);

    switch (targetType.GetObjectType())
    {
        case TARGET_OBJECT_TYPE_UNIT:
            if (Unit* unitTarget = target->ToUnit())
                AddUnitTarget(unitTarget, effMask, true, false);
            break;
        case TARGET_OBJECT_TYPE_GOBJ:
            if (GameObject* gobjTarget = target->ToGameObject())
                AddGOTarget(gobjTarget, effMask);
            break;
        case TARGET_OBJECT_TYPE_DEST:
            m_targets.SetDst(*target);
            break;
        default:
            ASSERT(false && "Spell::SelectImplicitNearbyTargets: received not implemented target object type");
            break;
    }

    SelectImplicitChainTargets(effIndex, targetType, target, effMask);
}

void Spell::SelectImplicitConeTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType, uint32 effMask)
{
    if (targetType.GetReferenceType() != TARGET_REFERENCE_TYPE_CASTER)
    {
        ASSERT(false && "Spell::SelectImplicitConeTargets: received not implemented target reference type");
        return;
    }
    std::list<WorldObject*> targets;
    SpellTargetObjectTypes objectType = targetType.GetObjectType();
    SpellTargetCheckTypes selectionType = targetType.GetCheckType();
    SpellTargetSelectionCategories categoryType = targetType.GetSelectionCategory();

    ConditionContainer* condList = m_spellInfo->Effects[effIndex].ImplicitTargetConditions;
    float coneAngle = ((m_caster->GetTypeId() == TYPEID_PLAYER) ? M_PI * (2.0f / 3.0f) : M_PI / 2.0f);
    float coneOffset = 0;
    if (SpellTargetRestrictionsEntry const* restrictions = m_spellInfo->GetSpellTargetRestrictions())
        if (restrictions->ConeAngle != 0)
        {
            if (targetType.GetTarget() == TARGET_UNIT_CONE_ENEMY_WITH_OFFSET)
                coneOffset = M_PI * restrictions->ConeAngle / 180;
            else
                coneAngle = M_PI * restrictions->ConeAngle / 180;            
        }

    float radius = m_spellInfo->Effects[effIndex].CalcRadius(m_caster) * m_spellValue->RadiusMod;

    if (uint32 containerTypeMask = GetSearcherTypeMask(objectType, condList))
    {
        Trinity::WorldObjectSpellConeTargetCheck check(coneAngle, coneOffset, radius, m_caster, m_spellInfo, categoryType, selectionType, condList);
        Trinity::WorldObjectListSearcher<Trinity::WorldObjectSpellConeTargetCheck> searcher(m_caster, targets, check, containerTypeMask);
        SearchTargets<Trinity::WorldObjectListSearcher<Trinity::WorldObjectSpellConeTargetCheck> >(searcher, containerTypeMask, m_caster, m_caster, radius);

        CallScriptObjectAreaTargetSelectHandlers(targets, effIndex);

        if (!targets.empty())
        {
            // Other special target selection goes here
            if (uint32 maxTargets = m_spellValue->MaxAffectedTargets)
                Trinity::Containers::RandomResizeList(targets, maxTargets);

            // for compability with older code - add only unit and go targets
            /// @todo remove this
            std::list<Unit*> unitTargets;
            std::list<GameObject*> gObjTargets;

            for (std::list<WorldObject*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
            {
                if (Unit* unitTarget = (*itr)->ToUnit())
                    unitTargets.push_back(unitTarget);
                else if (GameObject* gObjTarget = (*itr)->ToGameObject())
                    gObjTargets.push_back(gObjTarget);
            }

            for (std::list<Unit*>::iterator itr = unitTargets.begin(); itr != unitTargets.end(); ++itr)
                AddUnitTarget(*itr, effMask, false);

            for (std::list<GameObject*>::iterator itr = gObjTargets.begin(); itr != gObjTargets.end(); ++itr)
                AddGOTarget(*itr, effMask);
        }
    }
}

void Spell::SelectImplicitAreaTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType, uint32 effMask)
{
    Unit* referer = NULL;
    switch (targetType.GetReferenceType())
    {
        case TARGET_REFERENCE_TYPE_SRC:
        case TARGET_REFERENCE_TYPE_DEST:
        case TARGET_REFERENCE_TYPE_CASTER:
            referer = m_caster;
            break;
        case TARGET_REFERENCE_TYPE_TARGET:
            referer = m_targets.GetUnitTarget();
            break;
        case TARGET_REFERENCE_TYPE_LAST:
        {
            // find last added target for this effect
            for (std::list<TargetInfo>::reverse_iterator ihit = m_UniqueTargetInfo.rbegin(); ihit != m_UniqueTargetInfo.rend(); ++ihit)
            {
                if (ihit->effectMask & (1<<effIndex))
                {
                    referer = ObjectAccessor::GetUnit(*m_caster, ihit->targetGUID);
                    break;
                }
            }
            break;
        }
        default:
            ASSERT(false && "Spell::SelectImplicitAreaTargets: received not implemented target reference type");
            return;
    }
    if (!referer)
        return;

    Position const* center = NULL;
    switch (targetType.GetReferenceType())
    {
        case TARGET_REFERENCE_TYPE_SRC:
            center = m_targets.GetSrcPos();
            break;
        case TARGET_REFERENCE_TYPE_DEST:
            center = m_targets.GetDstPos();
            break;
        case TARGET_REFERENCE_TYPE_CASTER:
        case TARGET_REFERENCE_TYPE_TARGET:
        case TARGET_REFERENCE_TYPE_LAST:
            center = referer;
            break;
         default:
             ASSERT(false && "Spell::SelectImplicitAreaTargets: received not implemented target reference type");
             return;
    }
    std::list<WorldObject*> targets;
    float radius = m_spellInfo->Effects[effIndex].CalcRadius(m_caster) * m_spellValue->RadiusMod;
    SearchAreaTargets(targets, radius, center, referer, targetType, m_spellInfo->Effects[effIndex].ImplicitTargetConditions);

    // Custom entries
    /// @todo remove those
    switch (m_spellInfo->Id)
    {
        case 117002: // Tap to the Spirit World
        {
            // remove existing targets
            CleanupTargetList();

            if (!targets.empty())
                for (std::list<WorldObject*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                    if ((*itr) && (*itr)->ToUnit())
                        if ((*itr)->GetEntry() == 60512)
                            AddUnitTarget((*itr)->ToUnit(), 1 << effIndex, false);
            return;
        }
        default:
            break;
    }

    CallScriptObjectAreaTargetSelectHandlers(targets, effIndex);

    std::list<Unit*> unitTargets;
    std::list<GameObject*> gObjTargets;
    std::list<Corpse*> corpseTargets;
    // for compability with older code - add only unit and go targets
    /// @todo remove this
    for (std::list<WorldObject*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
    {
        if (Unit* unitTarget = (*itr)->ToUnit())
            unitTargets.push_back(unitTarget);
        else if (GameObject* gObjTarget = (*itr)->ToGameObject())
            gObjTargets.push_back(gObjTarget);
        else if (Corpse* corpseTarget = (*itr)->ToCorpse())
            corpseTargets.push_back(corpseTarget);
    }

    if (!unitTargets.empty())
    {
        // Special target selection for smart heals and energizes
        uint32 maxSize = 0;
        int32 power = -1;
        switch (m_spellInfo->SpellFamilyName)
        {
            case SPELLFAMILY_GENERIC:
                switch (m_spellInfo->Id)
                {
                    case 71610: // Echoes of Light (Althor's Abacus normal version)
                    case 71641: // Echoes of Light (Althor's Abacus heroic version)
                        maxSize = 1;
                        power = POWER_HEALTH;
                        break;
                    case 54968: // Glyph of Holy Light
                        maxSize = m_spellInfo->MaxAffectedTargets;
                        power = POWER_HEALTH;
                        break;
                    case 57669: // Replenishment
                        // In arenas Replenishment may only affect the caster
                        if (m_caster->GetTypeId() == TYPEID_PLAYER && m_caster->ToPlayer()->InArena())
                        {
                            unitTargets.clear();
                            unitTargets.push_back(m_caster);
                            break;
                        }
                        maxSize = 10;
                        power = POWER_MANA;
                        break;
                    case 107145: // Light Wall Active
                        m_targets.AddExtraTarget(ObjectGuid::Empty, WorldLocation(996, -989.4236f, -2821.757f, 38.25466f, 0.0f));
                        m_targets.AddExtraTarget(ObjectGuid::Empty, WorldLocation(996, -1045.602f, -2822.323f, 38.25466f, 0.0f));
                        break;
                    default:
                        break;
                }
                break;
            case SPELLFAMILY_PRIEST:
                if (m_spellInfo->Id == 64904) // Hymn of Hope
                {
                    maxSize = 3;
                    power = POWER_MANA;
                }
                else
                    break;

                // Remove targets outside caster's raid
                for (std::list<Unit*>::iterator itr = unitTargets.begin(); itr != unitTargets.end();)
                {
                    if (!(*itr)->IsInRaidWith(m_caster))
                        itr = unitTargets.erase(itr);
                    else
                        ++itr;
                }
                break;
            default:
                break;
        }

        if (maxSize && power != -1)
        {
            if (Powers(power) == POWER_HEALTH)
            {
                if (unitTargets.size() > maxSize)
                {
                    unitTargets.sort(Trinity::HealthPctOrderPred());
                    unitTargets.resize(maxSize);
                }
            }
            else
            {
                for (std::list<Unit*>::iterator itr = unitTargets.begin(); itr != unitTargets.end();)
                    if ((*itr)->GetPowerType() != (Powers)power)
                        itr = unitTargets.erase(itr);
                    else
                        ++itr;

                if (unitTargets.size() > maxSize)
                {
                    unitTargets.sort(Trinity::PowerPctOrderPred((Powers)power));
                    unitTargets.resize(maxSize);
                }
            }
        }

        // Other special target selection goes here
        if (uint32 maxTargets = m_spellValue->MaxAffectedTargets)
        {
            // in Flex Difficulty we should override this value depend of group size. Also check if diff exist between 10/25 targets. Also prevent override if more than available count of 25N targets [for expmple we forgot anything (in another case it`ll affect ALL RAID instead max value)]
            if (m_caster && m_caster->GetMap()->GetDifficulty() == RAID_DIFFICULTY_1025MAN_FLEX && sSpellMgr->GetSpellInfo(m_spellInfo->Id, RAID_DIFFICULTY_10MAN_NORMAL)->MaxAffectedTargets != sSpellMgr->GetSpellInfo(m_spellInfo->Id, RAID_DIFFICULTY_25MAN_NORMAL)->MaxAffectedTargets && unitTargets.size() <= sSpellMgr->GetSpellInfo(m_spellInfo->Id, RAID_DIFFICULTY_25MAN_NORMAL)->MaxAffectedTargets)
                maxTargets = unitTargets.size(); // controlled in scripts for flex

            Trinity::Containers::RandomResizeList(unitTargets, maxTargets);
        }

        for (std::list<Unit*>::iterator itr = unitTargets.begin(); itr != unitTargets.end(); ++itr)
            AddUnitTarget(*itr, effMask, false);
    }

    if (!gObjTargets.empty())
    {
        if (uint32 maxTargets = m_spellValue->MaxAffectedTargets)
            Trinity::Containers::RandomResizeList(gObjTargets, maxTargets);

        for (std::list<GameObject*>::iterator itr = gObjTargets.begin(); itr != gObjTargets.end(); ++itr)
            AddGOTarget(*itr, effMask);
    }

    if (!corpseTargets.empty())
    {
        if (uint32 maxTargets = m_spellValue->MaxAffectedTargets)
            Trinity::Containers::RandomResizeList(corpseTargets, maxTargets);

        for (auto&& corpse : corpseTargets)
            AddCorpseTarget(corpse, effMask);
    }
}

void Spell::SelectImplicitRecipientTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType, uint32 effMask)
{
    std::list<WorldObject*> targets;
    float radius = sWorld->getFloatConfig(CONFIG_GROUP_XP_DISTANCE);
    Creature* caster = GetCaster()->ToCreature();
    if (!caster)
        return;
    Player* referer = caster->GetLootRecipient();
    if (!referer)
        return;
    SearchAreaTargets(targets, radius, caster, referer, targetType, m_spellInfo->Effects[effIndex].ImplicitTargetConditions);

    if (auto const* bonusLoot = sLootMgr->GetBonusLootForSpell(GetSpellInfo()->Id))
    {
        targets.remove_if([=](WorldObject const* target)
        {
            Player const* player = target->ToPlayer();
            if (!player)
                return true;
            if (!caster->HasLootRecipient(player))
                return true;
            uint32 lootId = bonusLoot->GetLootIdForDifficulty(caster->GetMap()->GetDifficulty());
            if (player->HasLootLockout(LootLockoutType::BonusLoot, lootId, player->GetMap()->GetDifficulty()))
                return true;
            return !player->HasCurrency(bonusLoot->Currency, 1);
        });
    }

    for (auto&& it : targets)
        if (Unit* target = it->ToUnit())
            AddUnitTarget(target, effMask, false);
}

void Spell::SelectImplicitCasterDestTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType)
{
    SpellDestination dest(*m_caster);

    switch (targetType.GetTarget())
    {
        case TARGET_DEST_CASTER:
            break;
        case TARGET_DEST_HOME:
            if (Player* playerCaster = m_caster->ToPlayer())
                dest = SpellDestination(playerCaster->m_homebindX, playerCaster->m_homebindY, playerCaster->m_homebindZ, playerCaster->GetOrientation(), playerCaster->m_homebindMapId);
            break;
        case TARGET_DEST_DB:
            if (SpellTargetPosition const* st = sSpellMgr->GetSpellTargetPosition(m_spellInfo->Id, effIndex))
            {
                /// @todo fix this check
                if (m_spellInfo->HasEffect(SPELL_EFFECT_TELEPORT_UNITS) || m_spellInfo->HasEffect(SPELL_EFFECT_BIND))
                    dest = SpellDestination(st->target_X, st->target_Y, st->target_Z, st->target_Orientation, (int32)st->target_mapId);
                else if (st->target_mapId == m_caster->GetMapId())
                    dest = SpellDestination(st->target_X, st->target_Y, st->target_Z, st->target_Orientation);
            }
            else
            {
                TC_LOG_DEBUG("spells", "SPELL: unknown target coordinates for spell ID %u", m_spellInfo->Id);
                if (WorldObject* target = m_targets.GetObjectTarget())
                    dest = SpellDestination(target ? *target : *m_caster);
            }
            break;
        case TARGET_DEST_CASTER_FISHING:
        {
             float minDist = m_spellInfo->GetMinRange(true);
             float maxDist = m_spellInfo->GetMaxRange(true);
             float dist = frand(minDist, maxDist);
             float x, y, z;
             float angle = float(rand_norm()) * static_cast<float>(M_PI * 35.0f / 180.0f) - static_cast<float>(M_PI * 17.5f / 180.0f);
             m_caster->GetClosePoint(x, y, z, DEFAULT_PLAYER_BOUNDING_RADIUS, dist, angle);

             float ground = m_caster->GetMapHeight(x, y, z);
             float liquidLevel = VMAP_INVALID_HEIGHT_VALUE;
             LiquidData liquidData;             
             if (m_caster->GetMap()->GetLiquidStatus(m_caster->GetPhaseMask(), x, y, z, MAP_ALL_LIQUIDS, &liquidData, m_caster->GetCollisionHeight()))
                liquidLevel = liquidData.level;

             if (liquidLevel <= ground) // When there is no liquid Map::GetWaterOrGroundLevel returns ground level
             {
                 SendCastResult(SPELL_FAILED_NOT_HERE);
                 SendChannelUpdate(0);
                 finish(false);
                 return;
             }

             if (ground + 0.75 > liquidLevel)
             {
                 SendCastResult(SPELL_FAILED_TOO_SHALLOW);
                 SendChannelUpdate(0);
                 finish(false);
                 return;
             }

             dest = SpellDestination(x, y, liquidLevel, m_caster->GetOrientation());
             break;
        }
        case TARGET_DEST_CASTER_FRONT_LEAP:
        {
            Unit* unitCaster = m_caster->ToUnit();
            if (!unitCaster)
                break;

            //float dist = spellEffectInfo.CalcRadius(unitCaster);
            float dist = m_spellInfo->Effects[effIndex].CalcRadius(m_caster);
            float angle = targetType.CalcDirectionAngle();

            Position pos = dest._position;

            unitCaster->MovePositionToFirstCollision(pos, dist, angle);
            dest.Relocate(pos);
            break;
        }        
        default:
        {
            float dist = m_spellInfo->Effects[effIndex].CalcRadius(m_caster);
            float angle = targetType.CalcDirectionAngle();
            float objSize = m_caster->GetObjectSize();

            switch (targetType.GetTarget())
            {
                case TARGET_DEST_CASTER_SUMMON:
                    dist = PET_FOLLOW_DIST;
                    break;
                case TARGET_DEST_CASTER_RANDOM:
                    if (dist > objSize)
                        dist = objSize + (dist - objSize) * float(rand_norm());
                    break;
                case TARGET_DEST_CASTER_FRONT_LEFT:
                case TARGET_DEST_CASTER_BACK_LEFT:
                case TARGET_DEST_CASTER_FRONT_RIGHT:
                case TARGET_DEST_CASTER_BACK_RIGHT:
                {
                    static float const DefaultTotemDistance = 3.0f;
                    if (!m_spellInfo->Effects[effIndex].HasRadius())
                        dist = DefaultTotemDistance;
                    break;
                }                   
                default:
                    break;
            }                

            if (targetType.GetTarget() == TARGET_DEST_CASTER_MOVEMENT_DIR)
            {
                if (m_caster->HasUnitMovementFlag(MOVEMENTFLAG_STRAFE_RIGHT))
                {
                    if (m_caster->HasUnitMovementFlag(MOVEMENTFLAG_FORWARD))
                        angle = -M_PI / 4;
                    else if (m_caster->HasUnitMovementFlag(MOVEMENTFLAG_BACKWARD))
                        angle = -3 * M_PI / 4;
                    else
                        angle = -M_PI / 2;
                }
                else if (m_caster->HasUnitMovementFlag(MOVEMENTFLAG_STRAFE_LEFT))
                {
                    if (m_caster->HasUnitMovementFlag(MOVEMENTFLAG_FORWARD))
                        angle = M_PI / 4;
                    else if (m_caster->HasUnitMovementFlag(MOVEMENTFLAG_BACKWARD))
                        angle = 3 * M_PI / 4;
                    else
                        angle = M_PI / 2;
                }
                else if (m_caster->HasUnitMovementFlag(MOVEMENTFLAG_BACKWARD))
                    angle = M_PI;
            }

            if (dist < objSize)
                dist = objSize;

            Position pos;
            // bool teleport = GetSpellInfo()->Effects[effIndex].Effect == SPELL_EFFECT_LEAP &&
            //     (targetType.GetTarget() == TARGET_DEST_CASTER_FRONT_LEAP ||
            //      targetType.GetTarget() == TARGET_DEST_CASTER_FRONT ||
            //      targetType.GetTarget() == TARGET_DEST_CASTER_MOVEMENT_DIR);

            // if (teleport)
            //     pos = m_caster->GetBlinkPosition(dist, angle);
            // else
            pos = m_caster->GetFirstCollisionPosition(dist, angle);

            dest.Relocate(pos);
            break;
        }
    }

    CallScriptDestinationTargetSelectHandlers(dest, effIndex);
    m_targets.SetDst(dest);
}

void Spell::SelectImplicitTargetDestTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType)
{
    WorldObject* target = m_targets.GetObjectTarget();

    SpellDestination dest(*target);

    switch (targetType.GetTarget())
    {
        case TARGET_DEST_TARGET_ENEMY:
        case TARGET_DEST_TARGET_ANY:
            m_targets.SetDst(*target);
            break;
        default:
        {
            float angle = targetType.CalcDirectionAngle();
            float objSize = target->GetObjectSize();
            float dist = m_spellInfo->Effects[effIndex].CalcRadius(m_caster);
            if (dist < objSize)
                dist = objSize;
            else if (targetType.GetTarget() == TARGET_DEST_TARGET_RANDOM)
                dist = objSize + (dist - objSize) * (float)rand_norm();

            target->MovePositionToFirstCollision(dest._position, dist, angle);
            break;
        }
    }

    CallScriptDestinationTargetSelectHandlers(dest, effIndex);
    m_targets.SetDst(dest);
}

void Spell::SelectImplicitDestDestTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType)
{
    // set destination to caster if no dest provided
    // can only happen if previous destination target could not be set for some reason
    // (not found nearby target, or channel target for example
    // maybe we should abort the spell in such case?
    if (!m_targets.HasDst())
        m_targets.SetDst(*m_caster);

    SpellDestination dest(*m_targets.GetDst());

    switch (targetType.GetTarget())
    {
        case TARGET_DEST_DYNOBJ_ENEMY:
        case TARGET_DEST_DYNOBJ_ALLY:
        case TARGET_DEST_DYNOBJ_NONE:
        case TARGET_DEST_DEST:
            return;
        case TARGET_DEST_TRAJ:
            SelectImplicitTrajTargets(effIndex);
            return;
        default:
        {
            float angle = targetType.CalcDirectionAngle();
            float dist = m_spellInfo->Effects[effIndex].CalcRadius(m_caster);
            if (targetType.GetTarget() == TARGET_DEST_DEST_RANDOM)
                dist *= (float)rand_norm();

            Position pos = *m_targets.GetDstPos();
            m_caster->MovePositionToFirstCollision(pos, dist, angle);
            
            dest.Relocate(pos);
            break;
        }
    }

    CallScriptDestinationTargetSelectHandlers(dest, effIndex);
    m_targets.ModDst(dest);
}

void Spell::SelectImplicitCasterObjectTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType)
{
    WorldObject* target = NULL;
    bool checkIfValid = true;

    switch (targetType.GetTarget())
    {
        case TARGET_UNIT_CASTER:
        case TARGET_UNK_120:
            target = m_caster;
            checkIfValid = false;
            break;
        case TARGET_UNIT_MASTER:
            target = m_caster->GetCharmerOrOwner();
            break;
        case TARGET_UNIT_PET:
            target = m_caster->GetGuardianPet();
            break;
        case TARGET_UNIT_SUMMONER:
            if (m_caster->IsSummon())
                target = m_caster->ToTempSummon()->GetSummoner();
            break;
        case TARGET_UNIT_VEHICLE:
            target = m_caster->GetVehicleBase();
            break;
        case TARGET_UNIT_PASSENGER_0:
        case TARGET_UNIT_PASSENGER_1:
        case TARGET_UNIT_PASSENGER_2:
        case TARGET_UNIT_PASSENGER_3:
        case TARGET_UNIT_PASSENGER_4:
        case TARGET_UNIT_PASSENGER_5:
        case TARGET_UNIT_PASSENGER_6:
        case TARGET_UNIT_PASSENGER_7:
            if (m_caster->GetTypeId() == TYPEID_UNIT && m_caster->ToCreature()->IsVehicle())
                target = m_caster->GetVehicleKit()->GetPassenger(targetType.GetTarget() - TARGET_UNIT_PASSENGER_0);
            break;
        default:
            break;
    }

    CallScriptObjectTargetSelectHandlers(target, effIndex);

    if (target && target->ToUnit())
        AddUnitTarget(target->ToUnit(), 1 << effIndex, checkIfValid);
}

void Spell::SelectImplicitTargetObjectTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType)
{
    ASSERT((m_targets.GetObjectTarget() || m_targets.GetItemTarget()) && "Spell::SelectImplicitTargetObjectTargets - no explicit object or item target available!");

    WorldObject* target = m_targets.GetObjectTarget();

    CallScriptObjectTargetSelectHandlers(target, effIndex);

    if (target)
    {
        if (Unit* unit = target->ToUnit())
        {
            if (targetType.GetTarget() != TARGET_UNIT_TARGET_OR_UNIT_PARTY || !SelectImplicitRaidTargetsIfNeed(effIndex))
                AddUnitTarget(unit, 1 << effIndex, !m_redirect, false);
        }
        else if (GameObject* gobj = target->ToGameObject())
            AddGOTarget(gobj, 1 << effIndex);

        SelectImplicitChainTargets(effIndex, targetType, target, 1 << effIndex);
    }
    // Script hook can remove object target and we would wrongly land here
    else if (Item* item = m_targets.GetItemTarget())
        AddItemTarget(item, 1 << effIndex);
}

void Spell::SelectImplicitChainTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType, WorldObject* target, uint32 effMask)
{
    uint32 maxTargets = m_spellInfo->Effects[effIndex].ChainTarget;
    if (Player* modOwner = m_caster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_JUMP_TARGETS, maxTargets, this);

    if (maxTargets > 1)
    {
        // mark damage multipliers as used
        for (uint32 k = effIndex; k < MAX_SPELL_EFFECTS; ++k)
            if (effMask & (1 << k))
                m_damageMultipliers[k] = 1.0f;
        m_applyMultiplierMask |= effMask;

        std::list<WorldObject*> targets;
        SearchChainTargets(targets, maxTargets - 1, target, targetType,
            m_spellInfo->Effects[effIndex].ImplicitTargetConditions, targetType.GetTarget() == TARGET_UNIT_TARGET_CHAINHEAL_ALLY);

        // Chain primary target is added earlier
        CallScriptObjectAreaTargetSelectHandlers(targets, effIndex);

        for (std::list<WorldObject*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
            if (Unit* unitTarget = (*itr)->ToUnit())
                AddUnitTarget(unitTarget, effMask, false);
    }
}

float tangent(float x)
{
    x = tan(x);
    //if (x < std::numeric_limits<float>::max() && x > -std::numeric_limits<float>::max()) return x;
    //if (x >= std::numeric_limits<float>::max()) return std::numeric_limits<float>::max();
    //if (x <= -std::numeric_limits<float>::max()) return -std::numeric_limits<float>::max();
    if (x < 100000.0f && x > -100000.0f) return x;
    if (x >= 100000.0f) return 100000.0f;
    if (x <= 100000.0f) return -100000.0f;
    return 0.0f;
}

#define DEBUG_TRAJ(a) //a

void Spell::SelectImplicitTrajTargets(SpellEffIndex effIndex)
{
    if (!m_targets.HasTraj())
        return;

    float dist2d = m_targets.GetDist2d();
    if (!dist2d)
        return;

    float srcToDestDelta = m_targets.GetDstPos()->m_positionZ - m_targets.GetSrcPos()->m_positionZ;

    std::list<WorldObject*> targets;
    Trinity::WorldObjectSpellTrajTargetCheck check(dist2d, m_targets.GetSrcPos(), m_caster, m_spellInfo);
    Trinity::WorldObjectListSearcher<Trinity::WorldObjectSpellTrajTargetCheck> searcher(m_caster, targets, check, GRID_MAP_TYPE_MASK_ALL);
    SearchTargets<Trinity::WorldObjectListSearcher<Trinity::WorldObjectSpellTrajTargetCheck> > (searcher, GRID_MAP_TYPE_MASK_ALL, m_caster, m_targets.GetSrcPos(), dist2d);
    if (targets.empty())
        return;

    targets.sort(Trinity::ObjectDistanceOrderPred(m_caster));

    float b = tangent(m_targets.GetElevation());
    float a = (srcToDestDelta - dist2d * b) / (dist2d * dist2d);
    if (a > -0.0001f)
        a = 0;
    DEBUG_TRAJ(TC_LOG_ERROR("spells", "Spell::SelectTrajTargets: a %f b %f", a, b);)

    float bestDist = m_spellInfo->GetMaxRange(false);

    std::list<WorldObject*>::const_iterator itr = targets.begin();
    for (; itr != targets.end(); ++itr)
    {
        if (Unit* unitTarget = (*itr)->ToUnit())
            if (m_caster == *itr || m_caster->IsOnVehicle(unitTarget) || (unitTarget)->GetVehicle())//(*itr)->IsOnVehicle(m_caster))
                continue;

        const float size = std::max((*itr)->GetObjectSize() * 0.7f, 1.0f); // 1/sqrt(3)
        /// @todo all calculation should be based on src instead of m_caster
        const float objDist2d = m_targets.GetSrcPos()->GetExactDist2d(*itr) * std::cos(m_targets.GetSrcPos()->GetRelativeAngle(*itr));
        const float dz = (*itr)->GetPositionZ() - m_targets.GetSrcPos()->m_positionZ;

        DEBUG_TRAJ(TC_LOG_ERROR("spells", "Spell::SelectTrajTargets: check %u, dist between %f %f, height between %f %f.", (*itr)->GetEntry(), objDist2d - size, objDist2d + size, dz - size, dz + size);)

        float dist = objDist2d - size;
        float height = dist * (a * dist + b);
        DEBUG_TRAJ(TC_LOG_ERROR("spells", "Spell::SelectTrajTargets: dist %f, height %f.", dist, height);)
        if (dist < bestDist && height < dz + size && height > dz - size)
        {
            bestDist = dist > 0 ? dist : 0;
            break;
        }

#define CHECK_DIST {\
            DEBUG_TRAJ(TC_LOG_ERROR("spells", "Spell::SelectTrajTargets: dist %f, height %f.", dist, height);)\
            if (dist > bestDist)\
                continue;\
            if (dist < objDist2d + size && dist > objDist2d - size)\
            {\
                bestDist = dist;\
                break;\
            }\
        }

        if (!a)
        {
            height = dz - size;
            dist = height / b;
            CHECK_DIST;

            height = dz + size;
            dist = height / b;
            CHECK_DIST;

            continue;
        }

        height = dz - size;
        float sqrt1 = b * b + 4 * a * height;
        if (sqrt1 > 0)
        {
            sqrt1 = sqrt(sqrt1);
            dist = (sqrt1 - b) / (2 * a);
            CHECK_DIST;
        }

        height = dz + size;
        float sqrt2 = b * b + 4 * a * height;
        if (sqrt2 > 0)
        {
            sqrt2 = sqrt(sqrt2);
            dist = (sqrt2 - b) / (2 * a);
            CHECK_DIST;

            dist = (-sqrt2 - b) / (2 * a);
            CHECK_DIST;
        }

        if (sqrt1 > 0)
        {
            dist = (-sqrt1 - b) / (2 * a);
            CHECK_DIST;
        }
    }

    if (m_targets.GetSrcPos()->GetExactDist2d(m_targets.GetDstPos()) > bestDist)
    {
        float x = m_targets.GetSrcPos()->m_positionX + std::cos(m_caster->GetOrientation()) * bestDist;
        float y = m_targets.GetSrcPos()->m_positionY + std::sin(m_caster->GetOrientation()) * bestDist;
        float z = m_targets.GetSrcPos()->m_positionZ + bestDist * (a * bestDist + b);

        if (itr != targets.end())
        {
            float distSq = (*itr)->GetExactDistSq(x, y, z);
            float sizeSq = (*itr)->GetObjectSize();
            sizeSq *= sizeSq;
            DEBUG_TRAJ(TC_LOG_ERROR("spells", "Initial %f %f %f %f %f", x, y, z, distSq, sizeSq);)
            if (distSq > sizeSq)
            {
                float factor = 1 - sqrt(sizeSq / distSq);
                x += factor * ((*itr)->GetPositionX() - x);
                y += factor * ((*itr)->GetPositionY() - y);
                z += factor * ((*itr)->GetPositionZ() - z);

                distSq = (*itr)->GetExactDistSq(x, y, z);
                DEBUG_TRAJ(TC_LOG_ERROR("spells", "Initial %f %f %f %f %f", x, y, z, distSq, sizeSq);)
            }
        }

        Position trajDst;
        trajDst.Relocate(x, y, z, m_caster->GetOrientation());
        SpellDestination dest(*m_targets.GetDst());
        dest.Relocate(trajDst);

        CallScriptDestinationTargetSelectHandlers(dest, effIndex);
        m_targets.ModDst(dest);
    }

    if (Vehicle* veh = m_caster->GetVehicleKit())
        veh->SetLastShootPos(*m_targets.GetDstPos());
}

bool Spell::SelectImplicitRaidTargetsIfNeed(SpellEffIndex effIndex)
{
    Unit* caster = GetCaster();
    Unit* targetUnit = m_targets.GetUnitTarget();
    if (!targetUnit || !targetUnit->IsInRaidWith(caster))
        return false;

    float radius = 100.0f;  // For some spells radius is missed
    UnitList targetList;
    Trinity::AnyGroupedUnitInObjectRangeCheck check(caster, caster, radius, true);
    Trinity::UnitListSearcher<Trinity::AnyGroupedUnitInObjectRangeCheck> searcher(caster, targetList, check);
    caster->VisitNearbyObject(radius, searcher);

    targetList.remove_if([](Unit const* target) { return target->IsPetGuardianStuff(); });

    std::list<WorldObject*> targets;
    for (auto&& target : targetList)
        targets.push_back(target);

    CallScriptObjectAreaTargetSelectHandlers(targets, effIndex);

    for (auto&& target : targets)
        AddUnitTarget(target->ToUnit(), 1 << effIndex, false);

    return true;
}

void Spell::SelectEffectTypeImplicitTargets(uint8 effIndex)
{
    // special case for SPELL_EFFECT_SUMMON_RAF_FRIEND and SPELL_EFFECT_SUMMON_PLAYER
    /// @todo this is a workaround - target shouldn't be stored in target map for those spells
    switch (m_spellInfo->Effects[effIndex].Effect)
    {
        case SPELL_EFFECT_SUMMON_RAF_FRIEND:
        case SPELL_EFFECT_SUMMON_PLAYER:
            if (m_caster->GetTypeId() == TYPEID_PLAYER && m_caster->GetTarget())
            {
                WorldObject* target = ObjectAccessor::FindPlayer(m_caster->GetTarget());

                CallScriptObjectTargetSelectHandlers(target, SpellEffIndex(effIndex));

                if (target && target->ToPlayer())
                    AddUnitTarget(target->ToUnit(), 1 << effIndex, false);
            }
            return;
        default:
            break;
    }

    // select spell implicit targets based on effect type
    if (!m_spellInfo->Effects[effIndex].GetImplicitTargetType())
        return;

    uint32 targetMask = m_spellInfo->Effects[effIndex].GetMissingTargetMask();

    if (!targetMask)
        return;

    WorldObject* target = NULL;

    switch (m_spellInfo->Effects[effIndex].GetImplicitTargetType())
    {
        // add explicit object target or self to the target map
        case EFFECT_IMPLICIT_TARGET_EXPLICIT:
            // player which not released his spirit is Unit, but target flag for it is TARGET_FLAG_CORPSE_MASK
            if (targetMask & (TARGET_FLAG_UNIT_MASK | TARGET_FLAG_CORPSE_MASK))
            {
                if (Unit* unitTarget = m_targets.GetUnitTarget())
                    target = unitTarget;
                else if (targetMask & TARGET_FLAG_CORPSE_MASK)
                {
                    if (Corpse* corpseTarget = m_targets.GetCorpseTarget())
                    {
                        /// @todo this is a workaround - corpses should be added to spell target map too, but we can't do that so we add owner instead
                        if (Player* owner = ObjectAccessor::FindPlayer(corpseTarget->GetOwnerGUID()))
                            target = owner;
                    }
                }
                else //if (targetMask & TARGET_FLAG_UNIT_MASK)
                    target = m_caster;
            }
            if (targetMask & TARGET_FLAG_ITEM_MASK)
            {
                if (Item* itemTarget = m_targets.GetItemTarget())
                    AddItemTarget(itemTarget, 1 << effIndex);
                return;
            }
            if (targetMask & TARGET_FLAG_GAMEOBJECT_MASK)
                target = m_targets.GetGOTarget();
            break;
        // add self to the target map
        case EFFECT_IMPLICIT_TARGET_CASTER:
            if (targetMask & TARGET_FLAG_UNIT_MASK)
                target = m_caster;
            break;
        default:
            break;
    }

    CallScriptObjectTargetSelectHandlers(target, SpellEffIndex(effIndex));

    if (target)
    {
        if (target->ToUnit())
            AddUnitTarget(target->ToUnit(), 1 << effIndex, false);
        else if (target->ToGameObject())
            AddGOTarget(target->ToGameObject(), 1 << effIndex);
    }
}

uint32 Spell::GetSearcherTypeMask(SpellTargetObjectTypes objType, ConditionContainer* condList)
{
    // this function selects which containers need to be searched for spell target
    uint32 retMask = GRID_MAP_TYPE_MASK_ALL;

    // filter searchers based on searched object type
    switch (objType)
    {
        case TARGET_OBJECT_TYPE_UNIT:
        case TARGET_OBJECT_TYPE_UNIT_AND_DEST:
        case TARGET_OBJECT_TYPE_CORPSE:
        case TARGET_OBJECT_TYPE_CORPSE_ENEMY:
        case TARGET_OBJECT_TYPE_CORPSE_ALLY:
            retMask &= GRID_MAP_TYPE_MASK_PLAYER | GRID_MAP_TYPE_MASK_CORPSE | GRID_MAP_TYPE_MASK_CREATURE;
            break;
        case TARGET_OBJECT_TYPE_GOBJ:
        case TARGET_OBJECT_TYPE_GOBJ_ITEM:
            retMask &= GRID_MAP_TYPE_MASK_GAMEOBJECT;
            break;
        default:
            break;
    }
    if (!m_spellInfo->IsAllowingDeadTarget())
        retMask &= ~GRID_MAP_TYPE_MASK_CORPSE;
    if (m_spellInfo->AttributesEx3 & SPELL_ATTR3_ONLY_TARGET_PLAYERS)
        retMask &= GRID_MAP_TYPE_MASK_CORPSE | GRID_MAP_TYPE_MASK_PLAYER;
    if (m_spellInfo->AttributesEx3 & SPELL_ATTR3_ONLY_TARGET_GHOSTS)
        retMask &= GRID_MAP_TYPE_MASK_PLAYER;

    if (condList)
        retMask &= sConditionMgr->GetSearcherTypeMaskForConditionList(*condList);
    return retMask;
}

template<class SEARCHER>
void Spell::SearchTargets(SEARCHER& searcher, uint32 containerMask, Unit* referer, Position const* pos, float radius)
{
    if (!containerMask)
        return;

    // search world and grid for possible targets
    bool searchInGrid = containerMask & (GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_GAMEOBJECT);
    bool searchInWorld = containerMask & (GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER | GRID_MAP_TYPE_MASK_CORPSE);
    if (searchInGrid || searchInWorld)
    {
        float x, y;
        x = pos->GetPositionX();
        y = pos->GetPositionY();

        CellCoord p(Trinity::ComputeCellCoord(x, y));
        Cell cell(p);
        cell.SetNoCreate();

        Map& map = *(referer->GetMap());

        if (searchInWorld)
        {
            TypeContainerVisitor<SEARCHER, WorldTypeMapContainer> world_object_notifier(searcher);
            cell.Visit(p, world_object_notifier, map, radius, x, y);
        }
        if (searchInGrid)
        {
            TypeContainerVisitor<SEARCHER, GridTypeMapContainer >  grid_object_notifier(searcher);
            cell.Visit(p, grid_object_notifier, map, radius, x, y);
        }
    }
}

WorldObject* Spell::SearchNearbyTarget(float range, SpellImplicitTargetInfo const& targetType, ConditionContainer* condList)
{
    WorldObject* target = NULL;
    uint32 containerTypeMask = GetSearcherTypeMask(targetType.GetObjectType(), condList);
    if (!containerTypeMask)
        return NULL;
    Trinity::WorldObjectSpellNearbyTargetCheck check(range, m_caster, m_spellInfo, targetType.GetSelectionCategory(), targetType.GetCheckType(), condList);
    Trinity::WorldObjectLastSearcher<Trinity::WorldObjectSpellNearbyTargetCheck> searcher(m_caster, target, check, containerTypeMask);
    SearchTargets<Trinity::WorldObjectLastSearcher<Trinity::WorldObjectSpellNearbyTargetCheck> > (searcher, containerTypeMask, m_caster, m_caster, range);
    return target;
}

void Spell::SearchAreaTargets(std::list<WorldObject*>& targets, float range, Position const* position, Unit* referer, SpellImplicitTargetInfo const& targetType, ConditionContainer* condList)
{
    uint32 containerTypeMask = GetSearcherTypeMask(targetType.GetObjectType(), condList);
    if (!containerTypeMask)
        return;
    Trinity::WorldObjectSpellAreaTargetCheck check(range, position, m_caster, referer, m_spellInfo, targetType.GetSelectionCategory(), targetType.GetCheckType(), condList);
    Trinity::WorldObjectListSearcher<Trinity::WorldObjectSpellAreaTargetCheck> searcher(m_caster, targets, check, containerTypeMask);

    // Hackishly increase target search range, without actually increasing distance check, to allow targets with large hitboxes to be tested
    if (targetType.GetObjectType() == TARGET_OBJECT_TYPE_GOBJ)
        range = std::max(range, 100.0f);
    if (m_caster->FindMap() && m_caster->GetMap()->IsRaid())
        range = std::max(range, 50.0f);

    SearchTargets<Trinity::WorldObjectListSearcher<Trinity::WorldObjectSpellAreaTargetCheck> > (searcher, containerTypeMask, m_caster, position, range);
}

void Spell::SearchChainTargets(std::list<WorldObject*>& targets, uint32 chainTargets, WorldObject* target, SpellImplicitTargetInfo const& targetType, ConditionContainer* condList, bool isChainHeal)
{
    // max dist for jump target selection
    float jumpRadius = 0.0f;
    switch (m_spellInfo->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_RANGED:
            // 7.5y for multi shot
            jumpRadius = 7.5f;
            break;
        case SPELL_DAMAGE_CLASS_MELEE:
            // 5y for swipe, cleave and similar
            jumpRadius = 5.0f;
            break;
        case SPELL_DAMAGE_CLASS_NONE:
        case SPELL_DAMAGE_CLASS_MAGIC:
            // 12.5y for chain heal spell since 3.2 patch
            if (isChainHeal)
                jumpRadius = 12.5f;
            // 10y as default for magic chain spells
            else if (m_spellInfo->Id == 7922 || m_spellInfo->Id == 105771) // Charge Stun and Warbringer is SPELL_DAMAGE_CLASS_MAGIC
                jumpRadius = 5.0f;
            else
                jumpRadius = 10.0f;
            break;
    }

    bool searchNearTarget = false;

    Position* pos = target;
    if (m_spellInfo->Id == 22482) // Blade Flurry
    {
        jumpRadius = 8.0f;
        pos = m_caster;
        searchNearTarget = true;
    }

    if (m_spellInfo->Id == 1490)  // Curse of the Elements
        searchNearTarget = true;

    // chain lightning/heal spells and similar - allow to jump at larger distance and go out of los
    bool isBouncingFar = (m_spellInfo->AttributesEx4 & SPELL_ATTR4_AREA_TARGET_CHAIN
        || m_spellInfo->DmgClass == SPELL_DAMAGE_CLASS_NONE
        || m_spellInfo->DmgClass == SPELL_DAMAGE_CLASS_MAGIC);

    // max dist which spell can reach
    float searchRadius = jumpRadius;
    if (isBouncingFar)
        searchRadius *= chainTargets;

    std::list<WorldObject*> tempTargets;
    SearchAreaTargets(tempTargets, searchRadius, pos, m_caster, targetType, condList);
    tempTargets.remove(target);

    // remove targets which are always invalid for chain spells
    // for some spells allow only chain targets in front of caster (swipe for example)
    if (!isBouncingFar)
    {
        for (std::list<WorldObject*>::iterator itr = tempTargets.begin(); itr != tempTargets.end();)
        {
            std::list<WorldObject*>::iterator checkItr = itr++;
            if (!m_caster->HasInArc(static_cast<float>(M_PI), *checkItr))
                tempTargets.erase(checkItr);
        }
    }

    while (chainTargets)
    {
        // try to get unit for next chain jump
        std::list<WorldObject*>::iterator foundItr = tempTargets.end();
        // get unit with highest hp deficit in dist
        if (isChainHeal)
        {
            uint32 maxHPDeficit = 0;
            for (std::list<WorldObject*>::iterator itr = tempTargets.begin(); itr != tempTargets.end(); ++itr)
            {
                if (Unit* unitTarget = (*itr)->ToUnit())
                {
                    uint32 deficit = unitTarget->GetMaxHealth() - unitTarget->GetHealth();
                    if ((deficit > maxHPDeficit || foundItr == tempTargets.end()) && target->IsWithinDist(unitTarget, jumpRadius) && target->IsWithinLOSInMap(unitTarget))
                    {
                        foundItr = itr;
                        maxHPDeficit = deficit;
                    }
                }
            }
        }
        // get closest object
        else
        {
            for (std::list<WorldObject*>::iterator itr = tempTargets.begin(); itr != tempTargets.end(); ++itr)
            {
                if (foundItr == tempTargets.end())
                {
                    if ((!isBouncingFar || target->IsWithinDist(*itr, jumpRadius)) && target->IsWithinLOSInMap(*itr))
                        foundItr = itr;
                }
                else if (target->GetDistanceOrder(*itr, *foundItr) && target->IsWithinLOSInMap(*itr))
                    foundItr = itr;
            }
        }
        // not found any valid target - chain ends
        if (foundItr == tempTargets.end())
            break;
        if (!searchNearTarget)
            target = *foundItr;
        tempTargets.erase(foundItr);
        targets.push_back(*foundItr);
        --chainTargets;
    }
}

GameObject* Spell::SearchSpellFocus()
{
    GameObject* focus = NULL;
    Trinity::GameObjectFocusCheck check(m_caster, m_spellInfo->RequiresSpellFocus);
    Trinity::GameObjectSearcher<Trinity::GameObjectFocusCheck> searcher(m_caster, focus, check);
    SearchTargets<Trinity::GameObjectSearcher<Trinity::GameObjectFocusCheck> > (searcher, GRID_MAP_TYPE_MASK_GAMEOBJECT, m_caster, m_caster, m_caster->GetVisibilityRange());
    return focus;
}

void Spell::prepareDataForTriggerSystem(AuraEffect const* /*triggeredByAura*/)
{
    //==========================================================================================
    // Now fill data for trigger system, need know:
    // can spell trigger another or not (m_canTrigger)
    // Create base triggers flags for Attacker and Victim (m_procAttacker, m_procVictim and m_procEx)
    //==========================================================================================

    m_procVictim = m_procAttacker = 0;
    // Get data for type of attack and fill base info for trigger
    switch (m_spellInfo->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_MELEE:
            m_procAttacker = PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS;
            if (m_attackType == OFF_ATTACK)
                m_procAttacker |= PROC_FLAG_DONE_OFFHAND_ATTACK;
            else
                m_procAttacker |= PROC_FLAG_DONE_MAINHAND_ATTACK;
            m_procVictim   = PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS;
            break;
        case SPELL_DAMAGE_CLASS_RANGED:
            // Auto attack
            if (m_spellInfo->AttributesEx2 & SPELL_ATTR2_AUTOREPEAT_FLAG)
            {
                m_procAttacker = PROC_FLAG_DONE_RANGED_AUTO_ATTACK;
                m_procVictim   = PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK;
            }
            else // Ranged spell attack
            {
                m_procAttacker = PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS;
                m_procVictim   = PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS;
            }
            break;
        default:
            if (m_spellInfo->EquippedItemClass == ITEM_CLASS_WEAPON &&
                m_spellInfo->EquippedItemSubClassMask & (1<<ITEM_SUBCLASS_WEAPON_WAND)
                && m_spellInfo->AttributesEx2 & SPELL_ATTR2_AUTOREPEAT_FLAG) // Wands auto attack
            {
                m_procAttacker = PROC_FLAG_DONE_RANGED_AUTO_ATTACK;
                m_procVictim   = PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK;
            }
            // For other spells trigger procflags are set in Spell::DoAllEffectOnTarget
            // Because spell positivity is dependant on target
    }
    m_procEx = PROC_EX_NONE;

    // Hunter trap spells - activation proc for Lock and Load, Entrapment and Misdirection
    if (m_spellInfo->SpellFamilyName == SPELLFAMILY_HUNTER &&
        (m_spellInfo->SpellFamilyFlags[0] & 0x8 ||      // Freezing and Frost Trap, Freezing Arrow
        m_spellInfo->Id == 57879 ||                     // Snake Trap - done this way to avoid double proc
        m_spellInfo->SpellFamilyFlags[2] & 0x00064000)) // Explosive, Immolation and Ice Trap
    {
        m_procAttacker |= PROC_FLAG_DONE_TRAP_ACTIVATION;
    }

    /* Effects which are result of aura proc from triggered spell cannot proc
        to prevent chain proc of these spells */

    // Hellfire Effect - trigger as DOT
    if (m_spellInfo->SpellFamilyName == SPELLFAMILY_WARLOCK && m_spellInfo->SpellFamilyFlags[0] & 0x00000040)
    {
        m_procAttacker = PROC_FLAG_DONE_PERIODIC;
        m_procVictim   = PROC_FLAG_TAKEN_PERIODIC;
    }

    // Ranged autorepeat attack is set as triggered spell - ignore it
    if (!(m_procAttacker & PROC_FLAG_DONE_RANGED_AUTO_ATTACK))
    {
        if (_triggeredCastFlags & TRIGGERED_DISALLOW_PROC_EVENTS &&
            (m_spellInfo->AttributesEx2 & SPELL_ATTR2_TRIGGERED_CAN_TRIGGER_PROC ||
            m_spellInfo->AttributesEx3 & SPELL_ATTR3_TRIGGERED_CAN_TRIGGER_PROC_2))
            m_procEx |= PROC_EX_INTERNAL_CANT_PROC;
        else if (_triggeredCastFlags & TRIGGERED_DISALLOW_PROC_EVENTS)
            m_procEx |= PROC_EX_INTERNAL_TRIGGERED;
    }
    // Totem casts require spellfamilymask defined in spell_proc_event to proc
    if (m_originalCaster && m_caster != m_originalCaster && m_caster->GetTypeId() == TYPEID_UNIT && m_caster->ToCreature()->IsTotem() && m_caster->IsControlledByPlayer())
        m_procEx |= PROC_EX_INTERNAL_REQ_FAMILY;
}

Spell::TargetInfo const* Spell::GetTargetInfo(uint64 guid) const
{
    for (auto&& itr : m_UniqueTargetInfo)
        if (itr.targetGUID == guid)
            return &itr;
    return nullptr;
}

Unit* Spell::GetExplTargetUnit() const
{
    if (!m_targets.GetObjectTargetGUID())
        return nullptr;

    return ObjectAccessor::GetUnit(*m_caster, m_targets.GetObjectTargetGUID());
}

void Spell::CleanupTargetList()
{
    m_UniqueTargetInfo.clear();
    m_UniqueGOTargetInfo.clear();
    m_UniqueItemInfo.clear();
    m_uniqueCorpseTargetInfo.clear();
    m_delayMoment = 0;
}

uint64 Spell::CaluclateFlyTimeForTarget(Unit* target)
{
    // Spell have speed - need calculate incoming time
    // Incoming time is zero for self casts. At least I think so.
    if (m_spellInfo->Speed > 0.0f && m_caster != target)
    {
        // calculate spell incoming interval
        /// @todo this is a hack
        float dist = m_caster->GetDistance(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());

        if (dist < 1.5f && !m_spellInfo->IsTargetingArea())
            dist = 1.5f;

        if (!(m_spellInfo->AttributesEx9 & SPELL_ATTR9_SPECIAL_DELAY_CALCULATION))
            return uint64(floor(dist / m_spellInfo->Speed * 1000.0f));
        return uint64(m_spellInfo->Speed * 1000.0f);
    }
    else if ((m_caster->GetTypeId() == TYPEID_PLAYER || m_caster->ToCreature()->IsPet()) && m_caster != target)
    {
        if (m_spellInfo->HasAttribute(SPELL_ATTR0_ONLY_STEALTHED) && target->GetTypeId() == TYPEID_UNIT)
            return 0ULL;

        if (!IsTriggered() && m_spellInfo->HasAttribute(SPELL_ATTR0_CU_AURA_CC))
            return 100ULL;
    }

    return 0LL;
}

void Spell::AddUnitTarget(Unit* target, uint32 effectMask, bool checkIfValid /*= true*/, bool implicit /*= true*/)
{
    for (uint32 effIndex = 0; effIndex < MAX_SPELL_EFFECTS; ++effIndex)
        if (!m_spellInfo->Effects[effIndex].IsEffect() || (!CheckEffectTarget(target, effIndex) && !m_redirect))
            effectMask &= ~(1 << effIndex);

    // no effects left
    if (!effectMask)
        return;

    if (checkIfValid)
        if (m_spellInfo->CheckTarget(m_caster, target, IsTriggered()) != SPELL_CAST_OK)
            return;

    // Check for effect immune skip if immuned
    for (uint32 effIndex = 0; effIndex < MAX_SPELL_EFFECTS; ++effIndex)
        if (target->IsImmunedToSpellEffect(m_spellInfo, effIndex))
            effectMask &= ~(1 << effIndex);

    ObjectGuid targetGUID = target->GetGUID();

    // Lookup target in already in list
    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (targetGUID == ihit->targetGUID)             // Found in list
        {
            ihit->effectMask |= effectMask;             // Immune effects removed from mask
            ihit->scaleAura = false;
            if (m_auraScaleMask && ihit->effectMask == m_auraScaleMask && m_caster != target)
            {
                SpellInfo const* auraSpell = m_spellInfo->GetFirstRankSpell();
                if (uint32(target->GetLevel() + 10) >= auraSpell->SpellLevel)
                    ihit->scaleAura = true;
            }
            return;
        }
    }

    // This is new target calculate data for him

    // Get spell hit result on target
    TargetInfo targetInfo;
    targetInfo.targetGUID = targetGUID;                         // Store target GUID
    targetInfo.effectMask = effectMask;                         // Store all effects not immune
    targetInfo.processed  = false;                              // Effects not apply on target
    targetInfo.alive      = target->IsAlive();
    targetInfo.damage     = 0;
    targetInfo.crit       = false;
    targetInfo.scaleAura  = false;
    targetInfo.unmitigatedDamage = 0;

    if (m_auraScaleMask && targetInfo.effectMask == m_auraScaleMask && m_caster != target)
    {
        SpellInfo const* auraSpell = m_spellInfo->GetFirstRankSpell();
        if (uint32(target->GetLevel() + 10) >= auraSpell->SpellLevel)
            targetInfo.scaleAura = true;
    }

    // Calculate hit result
    if (m_originalCaster)
    {
        targetInfo.missCondition = m_originalCaster->SpellHitResult(target, m_spellInfo, effectMask, m_canReflect, this);
        if (m_skipCheck && targetInfo.missCondition != SPELL_MISS_IMMUNE)
            targetInfo.missCondition = SPELL_MISS_NONE;
        if (targetInfo.missCondition == SPELL_MISS_IMMUNE && m_caster == target)
        {
            for (int32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            {
                if ((1 << i) & effectMask)
                {
                    if (m_spellInfo->Effects[i].Effect == SPELL_EFFECT_ENERGIZE)
                    {
                        targetInfo.missCondition = SPELL_MISS_NONE;
                        break;
                    }
                }
            }
        }
    }
    else
        targetInfo.missCondition = SPELL_MISS_EVADE; //SPELL_MISS_NONE;

    targetInfo.timeDelay = CaluclateFlyTimeForTarget(target);

    // Calculate minimum incoming time
    if (m_delayMoment == 0 || m_delayMoment > targetInfo.timeDelay)
        m_delayMoment = targetInfo.timeDelay;

    // If target reflect spell back to caster
    if (targetInfo.missCondition == SPELL_MISS_REFLECT)
    {
        // Calculate reflected spell result on caster
        targetInfo.reflectResult = m_caster->SpellHitResult(m_caster, m_spellInfo, effectMask, m_canReflect, this);

        if (targetInfo.reflectResult == SPELL_MISS_REFLECT)     // Impossible reflect again, so simply deflect spell
            targetInfo.reflectResult = SPELL_MISS_PARRY;

        ReflectTargetInfo reflect;
        reflect.processed = false;
        reflect.targetGUID = targetInfo.targetGUID;
        reflect.timeDelay = targetInfo.timeDelay;
        m_reflectTargetInfo.push_back(reflect);

        // Increase time interval for reflected spells by 1.5
        targetInfo.timeDelay += targetInfo.timeDelay >> 1;
    }
    else
        targetInfo.reflectResult = SPELL_MISS_NONE;

    // Add target to list
    m_UniqueTargetInfo.push_back(targetInfo);
}

void Spell::AddGOTarget(GameObject* go, uint32 effectMask)
{
    for (uint32 effIndex = 0; effIndex < MAX_SPELL_EFFECTS; ++effIndex)
    {
        if (!m_spellInfo->Effects[effIndex].IsEffect())
            effectMask &= ~(1 << effIndex);
        else
        {
            switch (m_spellInfo->Effects[effIndex].Effect)
            {
            case SPELL_EFFECT_GAMEOBJECT_DAMAGE:
            case SPELL_EFFECT_GAMEOBJECT_REPAIR:
            case SPELL_EFFECT_GAMEOBJECT_SET_DESTRUCTION_STATE:
                if (go->GetGoType() != GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
                    effectMask &= ~(1 << effIndex);
                break;
            default:
                break;
            }
        }
    }

    if (!effectMask)
        return;

    ObjectGuid targetGUID = go->GetGUID();

    // Lookup target in already in list
    for (std::list<GOTargetInfo>::iterator ihit = m_UniqueGOTargetInfo.begin(); ihit != m_UniqueGOTargetInfo.end(); ++ihit)
    {
        if (targetGUID == ihit->targetGUID)                 // Found in list
        {
            ihit->effectMask |= effectMask;                 // Add only effect mask
            return;
        }
    }

    // This is new target calculate data for him

    GOTargetInfo target;
    target.targetGUID = targetGUID;
    target.effectMask = effectMask;
    target.processed  = false;                              // Effects not apply on target

    // Spell have speed - need calculate incoming time
    if (m_spellInfo->Speed > 0.0f)
    {
        // calculate spell incoming interval
        float dist = m_caster->GetDistance(go->GetPositionX(), go->GetPositionY(), go->GetPositionZ());
        if (dist < 5.0f)
            dist = 5.0f;

        if (!(m_spellInfo->AttributesEx9 & SPELL_ATTR9_SPECIAL_DELAY_CALCULATION))
            target.timeDelay = uint64(floor(dist / m_spellInfo->Speed * 1000.0f));
        else
            target.timeDelay = uint64(m_spellInfo->Speed * 1000.0f);

        if (m_delayMoment == 0 || m_delayMoment > target.timeDelay)
            m_delayMoment = target.timeDelay;
    }
    else
        target.timeDelay = 0LL;

    // Add target to list
    m_UniqueGOTargetInfo.push_back(target);
}

void Spell::AddCorpseTarget(Corpse* target, uint32 effectMask)
{
    for (uint32 effIndex = 0; effIndex < MAX_SPELL_EFFECTS; ++effIndex)
    {
        if (effectMask & (1 << effIndex))
        {
            auto& effect = m_spellInfo->Effects[effIndex];
            if (!effect.IsEffect())
                effectMask &= ~(1 << effIndex);
            else
            {
                if (effect.TargetA.GetObjectType() != TARGET_OBJECT_TYPE_CORPSE &&
                    effect.TargetB.GetObjectType() != TARGET_OBJECT_TYPE_CORPSE)
                    effectMask &= ~(1 << effIndex);
                switch (effect.Effect)
                {
                    case SPELL_EFFECT_RESURRECT_WITH_AURA:
                        // We can't check here players at another map
                        if (Player* player = ObjectAccessor::GetPlayer(*m_caster, target->GetOwnerGUID()))
                            if (player->HasAura(effect.TriggerSpell))
                                effectMask &= ~(1 << effIndex);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    if (!effectMask)
        return;

    ObjectGuid targetGUID = target->GetGUID();
    // Lookup target in already in list
    for (auto&& ihit : m_uniqueCorpseTargetInfo)
    {
        if (targetGUID == ihit.targetGUID) // Found in list
        {
            ihit.effectMask |= effectMask; // Immune effects removed from mask
            return;
        }
    }

    // This is new target calculate data for him

    // Get spell hit result on target
    CorpseTargetInfo targetInfo;
    targetInfo.targetGUID = targetGUID;                         // Store target GUID
    targetInfo.effectMask = effectMask;                         // Store all effects not immune
    targetInfo.processed = false;                               // Effects not apply on target
    targetInfo.timeDelay = 0;

    m_uniqueCorpseTargetInfo.push_back(targetInfo);
}


void Spell::AddItemTarget(Item* item, uint32 effectMask)
{
    for (uint32 effIndex = 0; effIndex < MAX_SPELL_EFFECTS; ++effIndex)
        if (!m_spellInfo->Effects[effIndex].IsEffect())
            effectMask &= ~(1 << effIndex);

    // no effects left
    if (!effectMask)
        return;

    // Lookup target in already in list
    for (std::list<ItemTargetInfo>::iterator ihit = m_UniqueItemInfo.begin(); ihit != m_UniqueItemInfo.end(); ++ihit)
    {
        if (item == ihit->item)                            // Found in list
        {
            ihit->effectMask |= effectMask;                 // Add only effect mask
            return;
        }
    }

    // This is new target add data

    ItemTargetInfo target;
    target.item       = item;
    target.effectMask = effectMask;

    m_UniqueItemInfo.push_back(target);
}

void Spell::AddDestTarget(SpellDestination const& dest, uint32 effIndex)
{
    m_destTargets[effIndex] = dest;
}

int32 Spell::GetHealTargetCap() const
{
    switch (m_spellInfo->Id)
    {
        case 15290:  // Vampiric Embrace
            return uint32(m_UniqueTargetInfo.size());
        case 115310: // Revival
            if (m_caster->GetMap()->Is25ManRaid())
                return 15;
            break;
        case 114083: // Restorative Mists
            return uint32(m_UniqueTargetInfo.size());
        default:
            break;
    }
    return 6;
}

void Spell::DoAllEffectOnTarget(TargetInfo* target)
{
    if (!target || target->processed)
        return;

    target->processed = true;                               // Target checked in apply effects procedure

    // Get mask of effects for target
    uint32 mask = target->effectMask;

    Unit* unit = m_caster->GetGUID() == target->targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, target->targetGUID);
    if (!unit)
    {
        uint32 farMask = 0;
        // create far target mask
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            if (m_spellInfo->Effects[i].IsFarUnitTargetEffect())
                if ((1 << i) & mask)
                    farMask |= (1 << i);

        if (!farMask)
            return;
        // find unit in world
        unit = ObjectAccessor::GetUnit(*m_caster, target->targetGUID);
        if (!unit)
            return;

        // do far effects on the unit
        // can't use default call because of threading, do stuff as fast as possible
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            if (farMask & (1 << i))
                HandleEffects(unit, nullptr, nullptr, nullptr, i, SPELL_EFFECT_HANDLE_HIT_TARGET);
        return;
    }

    if (unit->IsAlive() != target->alive)
        return;

    if (getState() == SPELL_STATE_DELAYED && !m_spellInfo->IsPositive() && (getMSTime() - target->timeDelay) <= unit->m_lastSanctuaryTime)
        return;                                             // No missinfo in that case

    // Get original caster (if exist) and calculate damage/healing from him data
    Unit* caster = m_originalCaster ? m_originalCaster : m_caster;

    // Skip if m_originalCaster not avaiable
    if (!caster)
        return;

    SpellMissInfo missInfo = target->missCondition;

    // Need init unitTarget by default unit (can changed in code on reflect)
    // Or on missInfo != SPELL_MISS_NONE unitTarget undefined (but need in trigger subsystem)
    unitTarget = unit;

    // Reset damage/healing counter
    m_damage = target->damage;
    m_healing = -target->damage;

    // Fill base trigger info
    uint32 procAttacker = m_procAttacker;
    uint32 procVictim   = m_procVictim;
    uint32 procEx = m_procEx;

    m_spellAura = NULL; // Set aura to null for every target-make sure that pointer is not used for unit without aura applied

    bool canEffectTrigger = (GetSpellInfo()->CanTriggerProc() || m_damage > 0 || unitTarget->HasAuraType(SPELL_AURA_MOD_STEALTH)) && // CanTriggerProc checked another time in Unit::IsTriggeredAtSpellProcEvent because of damage-breakable auras
        unitTarget->CanProc() && CanExecuteTriggersOnHit(mask);

    Unit* spellHitTarget = NULL;

    if (missInfo == SPELL_MISS_NONE)                          // In case spell hit target, do all effect on that target
        spellHitTarget = unit;
    else if (missInfo == SPELL_MISS_REFLECT)                // In case spell reflect from target, do all effect on caster (if hit)
    {
        if (target->reflectResult == SPELL_MISS_NONE)       // If reflected spell hit caster -> do all effect on him
        {
            spellHitTarget = m_caster;
            if (m_caster->GetTypeId() == TYPEID_UNIT)
                m_caster->ToCreature()->LowerPlayerDamageReq(target->damage);
        }
    }

    m_missInfo = missInfo;

    if (spellHitTarget)
    {
        SpellMissInfo missInfo2 = DoSpellHitOnUnit(spellHitTarget, mask, target->scaleAura, target->timeDelay > 0);
        if (missInfo2 != SPELL_MISS_NONE)
        {
            if (missInfo2 != SPELL_MISS_MISS)
                m_caster->SendSpellMiss(unit, m_spellInfo->Id, missInfo2);
            m_damage = 0;
            spellHitTarget = NULL;
            m_missInfo = missInfo2;
        }
    }
    else if (m_caster->GetTypeId() == TYPEID_PLAYER && unit == m_caster && m_spellInfo->IsCooldownStartedOnEvent()) // Stealth + Immune
        m_caster->ToPlayer()->SendCooldownEvent(m_spellInfo);

    // Do not take combo points on dodge and miss
    if (missInfo != SPELL_MISS_NONE && m_needComboPoints && m_targets.GetUnitTargetGUID() == target->targetGUID)
        m_needComboPoints = false;

    // Trigger info was not filled in spell::preparedatafortriggersystem - we do it now
    if (canEffectTrigger && (!procAttacker || procAttacker == PROC_FLAG_DONE_TRAP_ACTIVATION) && !procVictim)
    {
        bool positive = true;
        if (m_damage > 0)
            positive = false;
        else if (!m_healing)
        {
            for (uint8 i = 0; i< MAX_SPELL_EFFECTS; ++i)
                // If at least one effect negative spell is negative hit
                if (mask & (1<<i) && !m_spellInfo->IsPositiveEffect(i))
                {
                    positive = false;
                    break;
                }
        }
        switch (m_spellInfo->DmgClass)
        {
            case SPELL_DAMAGE_CLASS_MAGIC:
                if (positive)
                {
                    procAttacker |= PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS;
                    procVictim   |= PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS;
                }
                else
                {
                    procAttacker |= PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG;
                    procVictim   |= PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG;
                }
            break;
            case SPELL_DAMAGE_CLASS_NONE:
                if (positive)
                {
                    procAttacker |= PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS;
                    procVictim   |= PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_POS;
                }
                else
                {
                    procAttacker |= PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_NEG;
                    procVictim   |= PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_NEG;
                }
            break;
        }
    }

    procAttacker |= PROC_FLAG_DONE_ABILITY;

    CallScriptOnHitHandlers();

    // All calculated do it!
    bool delayedDamage = false;
    // Do healing and triggers
    if (m_healing > 0)
    {
        bool crit = target->crit;
        uint32 addhealth = m_healing * m_spellValue->Multiplier;
        caster->ApplyPvPHealMods(unitTarget, addhealth, GetSpellInfo());
        if (crit)
        {
            procEx |= PROC_EX_CRITICAL_HIT;
            addhealth = caster->SpellCriticalHealingBonus(m_spellInfo, addhealth, NULL);
        }
        else
            procEx |= PROC_EX_NORMAL_HIT;
        procEx |= PROC_EX_INTERNAL_ACTIVE;

        uint32 absorb = 0;
        uint32 heal = addhealth;
        // calculate heal absorb and reduce healing
        caster->CalcHealAbsorb(unitTarget, GetSpellInfo(), heal, absorb);

        int32 gain = caster->DealHeal(unitTarget, heal);
        caster->SendHealSpellLog(unitTarget, GetSpellInfo()->Id, heal, uint32(heal - gain), absorb, crit);

        unitTarget->getHostileRefManager().threatAssist(caster, float(gain) * 0.5f, m_spellInfo);
        m_healing = gain;

        // Do triggers for unit (reflect triggers passed on hit phase for correct drop charge)
        if (canEffectTrigger && missInfo != SPELL_MISS_REFLECT)
        {
            ProcTriggerContext ctx(this);
            ctx.EffectiveHeal = uint32(gain);
            ctx.Overheal = addhealth - uint32(gain) - absorb;
            caster->ProcDamageAndSpell(unitTarget, procAttacker, procVictim, procEx, addhealth, m_attackType, m_spellInfo, m_triggeredByAuraSpell, ctx);
        }
    }
    // Do damage and triggers
    else if (m_damage > 0)
    {
        // Fill base damage struct (unitTarget - is real spell target)
        SpellNonMeleeDamage damageInfo(caster, unitTarget, m_spellInfo->Id, m_spellSchoolMask);

        ProcTriggerContext ctx{ this, &damageInfo };
        ctx.UnmitigatedDamage = uint32(target->unmitigatedDamage);

        // Add bonuses and fill damageInfo struct
        caster->CalculateSpellDamageTaken(&damageInfo, m_damage * m_spellValue->Multiplier, m_spellInfo, m_attackType,  target->crit, procVictim, createProcExtendMask(&damageInfo, missInfo));
        caster->DealDamageMods(damageInfo.target, damageInfo.damage, &damageInfo.absorb);

        if (Creature* target = damageInfo.target->ToCreature())
        {
            if (caster->GetTypeId() == TYPEID_UNIT && !caster->IsCharmedOwnedByPlayerOrPlayer())
            {
                float sparringLimitPct = target->GetSparringHealthLimit();

                if (sparringLimitPct != 0.0f)
                {
                    if (target->GetHealthPct() <= sparringLimitPct)
                    {
                        damageInfo.damage = 0;
                    }
                }
            }
        }

        // Send log damage message to client
        caster->SendSpellNonMeleeDamageLog(&damageInfo);
        caster->DealSpellDamage(&damageInfo, true);

        procEx |= createProcExtendMask(&damageInfo, missInfo);
        procEx |= PROC_EX_INTERNAL_ACTIVE;
        procVictim |= PROC_FLAG_TAKEN_DAMAGE;

        // Do triggers for unit (reflect triggers passed on hit phase for correct drop charge)
        if (canEffectTrigger && missInfo != SPELL_MISS_REFLECT)
        {
            caster->ProcDamageAndSpell(unitTarget, procAttacker, procVictim, procEx, damageInfo.damage, m_attackType, m_spellInfo, m_triggeredByAuraSpell, ctx);
            if (caster->GetTypeId() == TYPEID_PLAYER && (m_spellInfo->Attributes & SPELL_ATTR0_STOP_ATTACK_TARGET) == 0 &&
               (m_spellInfo->DmgClass == SPELL_DAMAGE_CLASS_MELEE || m_spellInfo->DmgClass == SPELL_DAMAGE_CLASS_RANGED))
                caster->ToPlayer()->CastItemCombatSpell(unitTarget, m_attackType, procAttacker, procVictim, procEx, GetSpellInfo());
        }

        m_damage = damageInfo.damage;

        // Used in spell scripts
        m_final_damage = damageInfo.damage;
        m_absorbed_damage = damageInfo.absorb;

        if (Player* player = m_caster->ToPlayer())
            if (m_cast_count && spellHitTarget && spellHitTarget == m_targets.GetUnitTarget())
                player->AddToAsisstList(spellHitTarget->GetGUID());
    }
    // Passive spell hits/misses or active spells only misses (only triggers)
    else
    {
        // Fill base damage struct (unitTarget - is real spell target)
        SpellNonMeleeDamage damageInfo(caster, unitTarget, m_spellInfo->Id, m_spellSchoolMask);
        procEx |= createProcExtendMask(&damageInfo, missInfo);
        for (int8 effIndex = 0; effIndex < MAX_SPELL_EFFECTS && !delayedDamage; ++effIndex)
        {
            if (m_spellInfo->Effects[effIndex].IsEffect())
            {
                switch (m_spellInfo->Effects[effIndex].ApplyAuraName)
                {
                    case SPELL_AURA_PERIODIC_DAMAGE: case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
                    case SPELL_AURA_PERIODIC_HEAL:   case SPELL_AURA_PERIODIC_LEECH:
                        delayedDamage = true;
                        break;
                }
            }
        }

        // Do triggers for unit (reflect triggers passed on hit phase for correct drop charge)
        if (canEffectTrigger && missInfo != SPELL_MISS_REFLECT)
            caster->ProcDamageAndSpell(unit, procAttacker, procVictim, procEx, 0, m_attackType, m_spellInfo, m_triggeredByAuraSpell, ProcTriggerContext(this));

        // Failed Pickpocket, reveal rogue
        if (missInfo == SPELL_MISS_RESIST && m_spellInfo->AttributesCu & SPELL_ATTR0_CU_PICKPOCKET && unitTarget->GetTypeId() == TYPEID_UNIT)
        {
            m_caster->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TALK);
            if (unitTarget->ToCreature()->IsAIEnabled)
                unitTarget->ToCreature()->AI()->AttackStart(m_caster);
        }
    }

    // If taret is unfrendly and we deal damage - do enter combat anyway, event if spell is "positive" due to TARGET_UNIT_TARGET_ANY
    if (missInfo != SPELL_MISS_EVADE && !caster->IsFriendlyTo(unit) && m_spellInfo->NeedsToApplyCombatOnHit() && (!m_spellInfo->IsPositive() || m_damage > 0))
    {
        bool initialAggro = !m_spellInfo->HasAttribute(SPELL_ATTR3_NO_INITIAL_AGGRO) && !m_spellInfo->HasAttribute(SPELL_ATTR0_CU_DONT_SET_IN_COMBAT_STATE);
        caster->CombatStart(unit, initialAggro);

        // Add caster to target's threatlist in case they both are in combat after Unit::CombatStart, but the spell did no direct damage and no threat was added in Spell::HandleThreatSpells.
        // Otherwise combat will be removed from the caster on next update due to him not having any hostile references.
        if (delayedDamage && initialAggro && caster->IsInCombat() && unit->IsInCombat() && m_spellInfo->AttributesCu & SPELL_ATTR0_CU_NO_INITIAL_THREAT)
            unit->AddThreat(caster, 0.0f);

        if (m_spellInfo->AttributesCu & SPELL_ATTR0_CU_AURA_CC)
            if (!unit->IsStandState())
                unit->SetStandState(UNIT_STAND_STATE_STAND);
    }

    if (GetSpellInfo()->Id == 53351) // FIXME: kill shot cooldown
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            if ((!spellHitTarget || spellHitTarget->IsAlive()) && !player->HasAura(90967))
            {
                player->CastSpell(player, 90967, true);
                player->GetSpellHistory()->RemoveCooldown(GetSpellInfo()->Id, true);
            }
        }
    }

    // TODO: Optimise this, like custom flag or something
    if (Player* player = unitTarget->ToPlayer())
        player->ResetCriterias(CRITERIA_RESET_TYPE_SPELL_HIT, m_spellInfo->Id);

    if (spellHitTarget)
    {
        //AI functions
        if (spellHitTarget->GetTypeId() == TYPEID_UNIT)
            if (spellHitTarget->ToCreature()->IsAIEnabled)
                spellHitTarget->ToCreature()->AI()->SpellHit(m_caster, m_spellInfo);

        if (m_caster->GetTypeId() == TYPEID_UNIT && m_caster->ToCreature()->IsAIEnabled)
            m_caster->ToCreature()->AI()->SpellHitTarget(spellHitTarget, m_spellInfo);

        // Needs to be called after dealing damage/healing to not remove breaking on damage auras
        DoTriggersOnSpellHit(spellHitTarget, mask);

        // if target is fallged for pvp also flag caster if a player
        if (unit->IsPvP() && unit->GetCharmerOrOwnerOrSelf() != m_caster && m_caster->GetTypeId() == TYPEID_PLAYER)
            m_caster->ToPlayer()->UpdatePvP(true);

        CallScriptAfterHitHandlers();
    }
}

SpellMissInfo Spell::DoSpellHitOnUnit(Unit* unit, uint32 effectMask, bool scaleAura, bool delayed)
{
    if (!unit || !effectMask)
        return SPELL_MISS_EVADE;

    // For delayed spells immunity may be applied between missile launch and hit - check immunity for that case
    if (delayed)
    {
        if (unit->IsImmunedToSpell(m_spellInfo, effectMask))
            return SPELL_MISS_IMMUNE;

        bool positive = !(effectMask & m_spellInfo->NegativeEffectMask);
        if (!positive && unit->IsImmunedToDamage(m_spellInfo))
            return SPELL_MISS_IMMUNE;
    }

    // disable effects to which unit is immune
    SpellMissInfo returnVal = SPELL_MISS_IMMUNE;
    for (uint32 effectNumber = 0; effectNumber < MAX_SPELL_EFFECTS; ++effectNumber)
        if (effectMask & (1 << effectNumber))
            if (unit->IsImmunedToSpellEffect(m_spellInfo, effectNumber))
                effectMask &= ~(1 << effectNumber);

    if (!effectMask)
        return returnVal;

    PrepareScriptHitHandlers();
    CallScriptBeforeHitHandlers();

    if (Player* player = unit->ToPlayer())
    {
        player->StartCriteria(CRITERIA_START_TYPE_SPELL_TARGET, m_spellInfo->Id);
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, m_spellInfo->Id, 0, 0, m_caster);
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2, m_spellInfo->Id, 0, 0, unit);
    }

    if (Player* player = m_caster->ToPlayer())
    {
        player->StartCriteria(CRITERIA_START_TYPE_SPELL_CASTER, m_spellInfo->Id);
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL2, m_spellInfo->Id, 0, 0, unit);
    }

    if (m_caster != unit)
    {
        // Recheck  UNIT_FLAG_NON_ATTACKABLE for delayed spells
        if (m_spellInfo->Speed > 0.0f && unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE) && unit->GetCharmerOrOwnerGUID() != m_caster->GetGUID())
            return SPELL_MISS_EVADE;

        if (m_caster->IsValidAttackTarget(unit, m_spellInfo))
        {
            if (!m_spellInfo->HasEffect(SPELL_EFFECT_DISTRACT))
                unit->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_HITBYSPELL, 0, m_spellInfo);
        }
        else if (m_caster->IsFriendlyTo(unit))
        {
            // for delayed spells ignore negative spells (after duel end) for friendly targets
            /// @todo this cause soul transfer bugged
            // 63881 - Malady of the Mind jump spell (Yogg-Saron)
            if (m_spellInfo->Speed > 0.0f && unit->GetTypeId() == TYPEID_PLAYER && (m_spellInfo->NegativeEffectMask & effectMask) && m_spellInfo->Id != 63881 && m_spellInfo->Id != 136193 && m_spellInfo->Id != 137643 && m_spellInfo->Id != 143842 && m_spellInfo->Id != 147235)
                return SPELL_MISS_EVADE;

            // assisting case, healing and resurrection
            if (unit->HasUnitState(UNIT_STATE_ATTACK_PLAYER))
            {
                m_caster->SetContestedPvP();
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    m_caster->ToPlayer()->UpdatePvP(true);
            }
            if (unit->IsInCombat() && !(m_spellInfo->AttributesEx3 & SPELL_ATTR3_NO_INITIAL_AGGRO) && !(m_spellInfo->AttributesEx & (SPELL_ATTR1_NOT_BREAK_STEALTH | SPELL_ATTR1_NO_THREAT)))
            {
                m_caster->SetInCombatState(unit->GetPvPCombatTimer() > 0, unit);
                unit->getHostileRefManager().threatAssist(m_caster, 0.0f);
            }
        }
    }

    m_diminishGroup = DIMINISHING_NONE;

    uint32 auraEffmask = 0;
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (effectMask & (1 << i) && m_spellInfo->Effects[i].IsUnitOwnedAuraEffect())
            auraEffmask |= 1 << i;

    bool refresh = false;
    if (auraEffmask)
    {
        // Select rank for aura with level requirements only in specific cases
        // Unit has to be target only of aura effect, both caster and target have to be players, target has to be other than unit target
        SpellInfo const* aurSpellInfo = m_spellInfo;
        int32 basePoints[MAX_SPELL_EFFECTS];

        if (scaleAura)
        {
            aurSpellInfo = m_spellInfo->GetAuraRankForLevel(unitTarget->GetLevel());
            ASSERT(aurSpellInfo);
            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            {
                basePoints[i] = aurSpellInfo->Effects[i].BasePoints;
                if (m_spellInfo->Effects[i].Effect != aurSpellInfo->Effects[i].Effect)
                {
                    aurSpellInfo = m_spellInfo;
                    break;
                }
            }
        }

        if (m_originalCaster)
        {
            m_spellAura = Aura::TryRefreshStackOrCreate(aurSpellInfo, effectMask, unit,
                m_originalCaster, (aurSpellInfo == m_spellInfo)? &m_spellValue->EffectBasePoints[0] : &basePoints[0], m_CastItem, ObjectGuid::Empty, &refresh);
            if (m_spellAura)
            {
                // Okay, aura created, only now increase the diminishing if any.
                // Get Data Needed for Diminishing Returns, some effects may have multiple auras, so this must be done on spell hit, not aura add
                m_diminishGroup = GetDiminishingReturnsGroupForSpell(m_spellInfo);
                if (m_diminishGroup)
                {
                    m_diminishLevel = unit->GetDiminishing(m_diminishGroup);
                    DiminishingReturnsType type = GetDiminishingReturnsGroupType(m_diminishGroup);
                    // Increase Diminishing on unit, current informations for actually casts will use values above
                    if ((type == DRTYPE_PLAYER &&
                        (unit->GetCharmerOrOwnerPlayerOrPlayerItself() || (unit->GetTypeId() == TYPEID_UNIT && unit->ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_ALL_DIMINISH))) ||
                        type == DRTYPE_ALL)
                        unit->IncrDiminishing(m_diminishGroup);
                }

                // Set aura stack amount to desired value
                if (m_spellValue->AuraStackAmount > 1)
                {
                    if (!refresh)
                        m_spellAura->SetStackAmount(m_spellValue->AuraStackAmount);
                    else
                        m_spellAura->ModStackAmount(m_spellValue->AuraStackAmount - 1); // Because refresh gives +1 stack.
                }

                // Now Reduce spell duration using data received at spell hit
                int32 duration = m_spellAura->GetMaxDuration();
                float diminishMod = unit->ApplyDiminishingToDuration(m_diminishGroup, duration, m_originalCaster, m_diminishLevel);

                // unit is immune to aura if it was diminished to 0 duration
                if (diminishMod == 0.0f)
                {
                    m_spellAura->Remove();
                    bool found = false;
                    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                        if (effectMask & (1 << i) && m_spellInfo->Effects[i].Effect != SPELL_EFFECT_APPLY_AURA)
                            found = true;
                    if (!found)
                        return SPELL_MISS_IMMUNE;
                }
                else
                {
                    ((UnitAura*)m_spellAura)->SetDiminishGroup(m_diminishGroup);

                    // From previous interrupted channel
                    duration += m_additionalChannelDuration;

                    if (duration != m_spellAura->GetMaxDuration())
                    {
                        m_spellAura->SetMaxDuration(duration);
                        m_spellAura->SetDuration(duration);
                        if (m_additionalChannelDuration)
                            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                                if (AuraEffect* eff = m_spellAura->GetEffect(i))
                                    if (int32 amplitude = eff->GetAmplitude())
                                        eff->SetPeriodicTimer(m_additionalChannelDuration);
                    }
                    m_spellAura->_RegisterForTargets();
                }
            }
        }
    }

    // So, this is here and not in Unit::_AddAura because we don't want to bind auras which applied in non cast way, e.g. after Alter Time (108978) and all possible shit from scripts.
    // And loading from DB for such auras is also impossible, they will be removed at caster logout.
    // Don't do anything if aura refreshed, it's already in list.
    if (!refresh && m_spellAura && !m_spellAura->IsRemoved())
    {
        if (m_spellInfo->HasAttribute(SPELL_ATTR5_SINGLE_TARGET_SPELL))
        {
            uint32 maxTargets = m_spellInfo->MaxAffectedTargets ? m_spellInfo->MaxAffectedTargets : 1;
            if (auto* list = m_originalCaster->GetBoundAurasBySpellId(m_spellInfo->Id))
            {
                ASSERT(list->size() <= maxTargets);
                if (list->size() == maxTargets)
                    list->front()->Remove();
                ASSERT(list->size() < maxTargets);
            }
            m_originalCaster->BindAura(m_spellAura);
        }
    }

    // SOME SERIOUS SHIT HERE.
    // I don't know how it suppose to work on the official server, but the only thing what I see in sniffs is...reverse packet sequence.
    // And more, our code doesn't launch movement when unit is rooted.
    // But where are 2 packet with Normal and FacingAngle splines (start and end pos are equal) in sniffs (that part doesn't produce bugs, so whatever).
    // Intimidating Shout, Psychic Scream (roots with glyph)
    if (GetSpellInfo()->Id == 5246 || GetSpellInfo()->Id == 8122 || GetSpellInfo()->Id == 113004)
    {
        uint32 effectNumber = MAX_SPELL_EFFECTS;

        do
        {
            --effectNumber;
            if (effectMask & (1 << effectNumber))
                HandleEffects(unit, nullptr, nullptr, nullptr, effectNumber, SPELL_EFFECT_HANDLE_HIT_TARGET);
        } while (effectNumber);

        return SPELL_MISS_NONE;
    }

    for (uint32 effectNumber = 0; effectNumber < MAX_SPELL_EFFECTS; ++effectNumber)
        if (effectMask & (1 << effectNumber))
            HandleEffects(unit, nullptr, nullptr, nullptr, effectNumber, SPELL_EFFECT_HANDLE_HIT_TARGET);

    return SPELL_MISS_NONE;
}

void Spell::DoTriggersOnSpellHit(Unit* unit, uint32 effMask)
{
    // Apply additional spell effects to target
    /// @todo move this code to scripts
    if (m_preCastSpell)
    {
        // Paladin immunity shields
        if (m_preCastSpell == 61988)
        {
            // Cast Forbearance
            m_caster->CastSpell(unit, 25771, true);
            // Cast Avenging Wrath Marker
            unit->CastSpell(unit, 61987, true);
        }

        // Avenging Wrath
        if (m_preCastSpell == 61987)
            // Cast the serverside immunity shield marker
            m_caster->CastSpell(unit, 61988, true);

        if (sSpellMgr->GetSpellInfo(m_preCastSpell))
            // Blizz seems to just apply aura without bothering to cast
            m_caster->AddAura(m_preCastSpell, unit);
    }

    // handle SPELL_AURA_ADD_TARGET_TRIGGER auras
    // this is executed after spell proc spells on target hit
    // spells are triggered for each hit spell target
    // info confirmed with retail sniffs of permafrost and shadow weaving
    if (!m_hitTriggerSpells.empty())
    {
        int duration = 0;
        for (auto i = m_hitTriggerSpells.begin(); i != m_hitTriggerSpells.end(); )
        {
            if (CanExecuteTriggersOnHit(effMask, i->triggeredByAura) && roll_chance_i(i->chance))
            {
                m_caster->CastSpell(unit, i->triggeredSpell, TriggerCastFlags(TRIGGERED_FULL_MASK & ~TRIGGERED_IGNORE_TARGET_CHECK));
                TC_LOG_DEBUG("spells", "Spell %d triggered spell %d by SPELL_AURA_ADD_TARGET_TRIGGER aura", m_spellInfo->Id, i->triggeredSpell->Id);

                // SPELL_AURA_ADD_TARGET_TRIGGER auras shouldn't trigger auras without duration
                // set duration of current aura to the triggered spell
                if (i->triggeredSpell->GetDuration() == -1)
                {
                    if (Aura* triggeredAur = unit->GetAura(i->triggeredSpell->Id, m_caster->GetGUID()))
                    {
                        // get duration from aura-only once
                        if (!duration)
                        {
                            Aura* aur = unit->GetAura(m_spellInfo->Id, m_caster->GetGUID());
                            duration = aur ? aur->GetDuration() : -1;
                        }
                        triggeredAur->SetDuration(duration);
                    }
                }
            }

            // Ruthlessness and Relentless Strikes must proc only for one target
            if (i->triggeredByAura->Id == 14161 || i->triggeredByAura->Id == 58423)
                m_hitTriggerSpells.erase(i++);
            else
                ++i;
        }
    }

    // trigger linked auras remove/apply
    /// @todo remove/cleanup this, as this table is not documented and people are doing stupid things with it
    if (std::vector<int32> const* spellTriggered = sSpellMgr->GetSpellLinked(m_spellInfo->Id + SPELL_LINK_HIT))
    {
        for (std::vector<int32>::const_iterator i = spellTriggered->begin(); i != spellTriggered->end(); ++i)
        {
            if (*i < 0)
                unit->RemoveAurasDueToSpell(-(*i));
            else
                unit->CastSpell(unit, *i, true, 0, 0, m_caster->GetGUID());
        }
    }
}

void Spell::DoAllEffectOnTarget(GOTargetInfo* target)
{
    if (target->processed)                                  // Check target
        return;
    target->processed = true;                               // Target checked in apply effects procedure

    uint32 effectMask = target->effectMask;
    if (!effectMask)
        return;

    GameObject* go = m_caster->GetMap()->GetGameObject(target->targetGUID);
    if (!go)
        return;

    PrepareScriptHitHandlers();
    CallScriptBeforeHitHandlers();

    for (uint32 effectNumber = 0; effectNumber < MAX_SPELL_EFFECTS; ++effectNumber)
        if (effectMask & (1 << effectNumber))
            HandleEffects(nullptr, nullptr, go, nullptr, effectNumber, SPELL_EFFECT_HANDLE_HIT_TARGET);

    CallScriptOnHitHandlers();
    CallScriptAfterHitHandlers();
}

void Spell::DoAllEffectOnTarget(CorpseTargetInfo* target)
{
    if (target->processed)                                  // Check target
        return;

    target->processed = true;                               // Target checked in apply effects procedure

    uint32 effectMask = target->effectMask;
    if (!effectMask)
        return;

    Corpse* corpse = ObjectAccessor::GetCorpse(*m_caster, target->targetGUID);
    if (!corpse)
        return;

    PrepareScriptHitHandlers();
    CallScriptBeforeHitHandlers();

    for (uint32 effectNumber = 0; effectNumber < MAX_SPELL_EFFECTS; ++effectNumber)
        if (effectMask & (1 << effectNumber))
            HandleEffects(nullptr, nullptr, nullptr, corpse, effectNumber, SPELL_EFFECT_HANDLE_HIT_TARGET);

    CallScriptOnHitHandlers();

    CallScriptAfterHitHandlers();
}

void Spell::DoAllEffectOnTarget(ItemTargetInfo* target)
{
    uint32 effectMask = target->effectMask;
    if (!target->item || !effectMask)
        return;

    PrepareScriptHitHandlers();
    CallScriptBeforeHitHandlers();

    for (uint32 effectNumber = 0; effectNumber < MAX_SPELL_EFFECTS; ++effectNumber)
        if (effectMask & (1 << effectNumber))
            HandleEffects(nullptr, target->item, nullptr, nullptr, effectNumber, SPELL_EFFECT_HANDLE_HIT_TARGET);

    CallScriptOnHitHandlers();

    CallScriptAfterHitHandlers();
}

bool Spell::UpdateChanneledTargetList()
{
    // Not need check return true
    if (m_channelTargetEffectMask == 0)
        return true;

    uint8 channelTargetEffectMask = m_channelTargetEffectMask;
    uint8 channelAuraMask = 0;
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (m_spellInfo->Effects[i].Effect == SPELL_EFFECT_APPLY_AURA)
            channelAuraMask |= 1<<i;

    channelAuraMask &= channelTargetEffectMask;

    float range = 0;
    if (channelAuraMask)
    {
        range = m_spellInfo->GetMaxRange(m_spellInfo->IsPositive());
        if (Player* modOwner = m_caster->GetSpellModOwner())
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RANGE, range, this);
    }

    for (std::list<TargetInfo>::iterator ihit= m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->missCondition == SPELL_MISS_NONE && (channelTargetEffectMask & ihit->effectMask))
        {
            Unit* unit = m_caster->GetGUID() == ihit->targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, ihit->targetGUID);

            if (!unit)
                continue;

            if (IsValidDeadOrAliveTarget(unit))
            {
                if (channelAuraMask & ihit->effectMask)
                {
                    if (AuraApplication * aurApp = unit->GetAuraApplication(m_spellInfo->Id, m_originalCasterGUID))
                    {
                        if (m_caster != unit && !m_caster->IsWithinDistInMap(unit, range))
                        {
                            ihit->effectMask &= ~aurApp->GetEffectMask();
                            unit->RemoveAura(aurApp);
                            continue;
                        }
                    }
                    else // aura is dispelled
                        continue;
                }

                channelTargetEffectMask &= ~ihit->effectMask;   // remove from need alive mask effect that have alive target
            }
        }
    }

    // is all effects from m_needAliveTargetMask have alive targets
    return channelTargetEffectMask == 0;
}

void Spell::prepare(SpellCastTargets const* targets, AuraEffect const* triggeredByAura)
{
    if (m_CastItem)
        m_castItemGUID = m_CastItem->GetGUID();
    else
        m_castItemGUID = ObjectGuid::Empty;

    InitExplicitTargets(*targets);

    // Fill aura scaling information
    if (m_caster->IsControlledByPlayer() && !m_spellInfo->IsPassive() && m_spellInfo->SpellLevel && !m_spellInfo->IsChanneled() && !(_triggeredCastFlags & TRIGGERED_IGNORE_AURA_SCALING))
    {
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (m_spellInfo->Effects[i].Effect == SPELL_EFFECT_APPLY_AURA)
            {
                // Change aura with ranks only if basepoints are taken from spellInfo and aura is positive
                if (m_spellInfo->IsPositiveEffect(i))
                {
                    m_auraScaleMask |= (1 << i);
                    if (m_spellValue->EffectBasePoints[i] != m_spellInfo->Effects[i].BasePoints)
                    {
                        m_auraScaleMask = 0;
                        break;
                    }
                }
            }
        }
    }

    m_spellState = SPELL_STATE_PREPARING;

    if (triggeredByAura)
        m_triggeredByAuraSpell  = triggeredByAura->GetSpellInfo();

    // create and add update event for this spell
    SpellEvent* Event = new SpellEvent(this);
    m_caster->m_Events.AddEvent(Event, m_caster->m_Events.CalculateTime(1));

    //Prevent casting at cast another spell (ServerSide check)
    if (!(_triggeredCastFlags & TRIGGERED_IGNORE_CAST_IN_PROGRESS) && m_caster->IsNonMeleeSpellCasted(false, true, true, m_spellInfo->Id == 75))
    {
        SendCastResult(SPELL_FAILED_SPELL_IN_PROGRESS);
        finish(false);
        return;
    }

    if (DisableMgr::IsDisabledFor(DISABLE_TYPE_SPELL, m_spellInfo->Id, m_caster))
    {
        SendCastResult(SPELL_FAILED_SPELL_UNAVAILABLE);
        finish(false);
        return;
    }
    LoadScripts();

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, true);
    // Fill cost data (not use power for item casts

    // Basically this is wrong, but I won't change all this shit in the near future.
    m_powerType = m_spellInfo->GetPowerType(m_caster, &m_powerEntryIndex);
    if (!m_CastItem && !m_darkSimulacrum && m_caster->IsAlive()) // For Ghost Wolf during dead (Glyph of Spirit Wolf (147770))
        m_powerCost = m_spellInfo->CalcPowerCost(m_caster, m_spellSchoolMask, m_powerEntryIndex);

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, false);

    // Set combo point requirement
    if ((_triggeredCastFlags & TRIGGERED_IGNORE_COMBO_POINTS) || m_CastItem || !m_caster->m_movedPlayer)
        m_needComboPoints = false;

    SpellCastResult result = CheckCast(true);
    // target is checked in too many locations and with different results to handle each of them
    // handle just the general SPELL_FAILED_BAD_TARGETS result which is the default result for most DBC target checks
    if (_triggeredCastFlags & TRIGGERED_IGNORE_TARGET_CHECK && result == SPELL_FAILED_BAD_TARGETS)
        result = SPELL_CAST_OK;
    if (result != SPELL_CAST_OK)
    {
        // Periodic auras should be interrupted when aura triggers a spell which can't be cast
        // for example bladestorm aura should be removed on disarm as of patch 3.3.5
        // channeled periodic spells should be affected by this (arcane missiles, penance, etc)
        // a possible alternative sollution for those would be validating aura target on unit state change
        if (triggeredByAura && triggeredByAura->GetSpellEffectInfo().ApplyAuraName == SPELL_AURA_PERIODIC_TRIGGER_SPELL && !triggeredByAura->GetBase()->IsPassive())
        {
            SendChannelUpdate(0);
            triggeredByAura->GetBase()->SetDuration(0);
        }

        m_castResult = result;
        SendCastResult(result);

        // queue autorepeat spells for future repeating
        if (GetCurrentContainer() == CURRENT_AUTOREPEAT_SPELL && m_caster->IsUnit())
            m_caster->ToUnit()->SetCurrentCastedSpell(this);

        if (Player* player = m_caster->ToPlayer())
            player->RestoreSpellMods(this);
        finish(false);
        return;
    }

    // Prepare data for triggers
    prepareDataForTriggerSystem(triggeredByAura);

    if (Player* player = m_caster->ToPlayer())
    {
        if (!player->GetCommandStatus(CHEAT_CASTTIME))
        {
            player->SetSpellModTakingSpell(this, true);
            // calculate cast time (calculated after first CheckCast check to prevent charge counting for first CheckCast fail)
            m_casttime = m_spellInfo->CalcCastTime(player->GetLevel(), this);
            player->SetSpellModTakingSpell(this, false);
        }
        else
            m_casttime = 0; // Set cast time to 0 if .cheat casttime is enabled.
    }
    else
        m_casttime = m_spellInfo->CalcCastTime(m_caster->GetLevel(), this);

    if (_triggeredCastFlags & TRIGGERED_CAST_DIRECTLY)
        m_casttime = 0;

    if (m_darkSimulacrum)
    {
        m_casttime = 0;
        m_powerCost = 0;
    }

    // don't allow channeled spells / spells with cast time to be casted while moving
    // (even if they are interrupted on moving, spells with almost immediate effect get to have their effect processed before movement interrupter kicks in)
    // don't cancel spells which are affected by a SPELL_AURA_CAST_WHILE_WALKING effect
    if (((m_spellInfo->IsChanneled() || m_casttime) && m_caster->GetTypeId() == TYPEID_PLAYER && (!m_caster->IsCharmed() || !m_caster->GetCharmerGUID().IsCreatureOrVehicle()) &&
        m_caster->isMoving() && m_spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT) && !m_caster->HasAuraTypeWithAffectMask(SPELL_AURA_CAST_WHILE_WALKING, m_spellInfo))
    {
        SendCastResult(SPELL_FAILED_MOVING);
        finish(false);
        return;
    }

    // set timer base at cast time
    ReSetTimer();

    TC_LOG_DEBUG("spells", "Spell::prepare: spell id %u source %u caster %d customCastFlags %u mask %u", m_spellInfo->Id, m_caster->GetEntry(), m_originalCaster ? m_originalCaster->GetEntry() : -1, _triggeredCastFlags, m_targets.GetTargetMask());

    //Containers for channeled spells have to be set
    /// @todoApply this to all casted spells if needed
    // Why check duration? 29350: channelled triggers channelled
    if ((_triggeredCastFlags & TRIGGERED_CAST_DIRECTLY) && (!m_spellInfo->IsChanneled() || !m_spellInfo->GetMaxDuration()))
        cast(true);
    else
    {
        // stealth must be removed at cast starting (at show channel bar)
        // skip triggered spell (item equip spell casting and other not explicit character casts/item uses)
        if (!(_triggeredCastFlags & TRIGGERED_IGNORE_AURA_INTERRUPT_FLAGS) && m_spellInfo->IsBreakingStealth())
        {
            if (GetSpellInfo()->IsChanneled() && GetSpellInfo()->AttributesEx8 & SPELL_ATTR8_DONT_RESET_PERIODIC_TIMER && m_targets.GetUnitTarget())
            {
                if (Aura* existing = m_targets.GetUnitTarget()->GetAura(GetSpellInfo()->Id))
                    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                        if (AuraEffect const* eff = existing->GetEffect(i))
                            if (eff->GetPeriodicTimer() && (!m_additionalChannelDuration || m_additionalChannelDuration > eff->GetPeriodicTimer()))
                                m_additionalChannelDuration = eff->GetPeriodicTimer();
            }
            m_caster->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_CAST, 0, GetSpellInfo());
            for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                if (m_spellInfo->Effects[i].GetUsedTargetObjectType() == TARGET_OBJECT_TYPE_UNIT)
                {
                    m_caster->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_SPELL_ATTACK, 0, GetSpellInfo());
                    break;
                }
            if (targets->GetGOTarget())
                m_caster->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_LOOTING);
            if (m_spellInfo->Effects[EFFECT_0].Effect == SPELL_EFFECT_OPEN_LOCK && m_caster->IsMounted() && !m_caster->HasAura(134359)) // Sky Golem
            {
                auto bounds = sSpellMgr->GetSkillLineAbilityMapBounds(m_spellInfo->Id);
                for (auto it = bounds.first; it != bounds.second; ++it)
                    if (it->second->skillId == SKILL_HERBALISM)
                        m_caster->RemoveAurasByType(SPELL_AURA_MOUNTED);
            }
        }

        if (!(_triggeredCastFlags & TRIGGERED_IGNORE_CAST_IN_PROGRESS) || (m_spellInfo->IsChanneled() && m_spellInfo->GetMaxDuration()))
            m_caster->SetCurrentCastedSpell(this);

        CallScriptOnSpellStartHandlers();

        SendSpellStart();

        // set target for proper facing
        if ((m_casttime || m_spellInfo->IsChanneled()) && !(_triggeredCastFlags & TRIGGERED_IGNORE_SET_FACING))
            if (m_caster->GetTypeId() == TYPEID_UNIT && m_targets.GetObjectTarget() && m_caster != m_targets.GetObjectTarget())
                m_caster->ToCreature()->FocusTarget(this, m_targets.GetObjectTarget());

        // set target for proper facing
        if (m_spellInfo->HasAttribute(SPELL_ATTR5_DONT_TURN_DURING_CAST) || m_spellInfo->HasAttribute(SPELL_ATTR11_DONT_TURN_DURING_CAST))
        {
            if ((m_spellInfo->IsChanneled() || m_casttime) && !(_triggeredCastFlags & TRIGGERED_IGNORE_SET_FACING) && m_caster->GetTypeId() == TYPEID_UNIT)
            {
                if (Unit* target = m_targets.GetUnitTarget())
                    m_caster->SetFacingToObject(target);
                else
                    m_caster->SetFacingTo(m_caster->GetOrientation());
                m_caster->AddUnitState(UNIT_STATE_ROTATING);
            }
        }

        if (!(_triggeredCastFlags & TRIGGERED_IGNORE_GCD))
            TriggerGlobalCooldown();

        //item: first cast may destroy item and second cast causes crash
        if (!m_casttime && (!m_spellInfo->StartRecoveryTime || m_caster->GetTypeId() == TYPEID_PLAYER) && !m_castItemGUID && GetCurrentContainer() == CURRENT_GENERIC_SPELL)
            cast(true);
        else if (!m_casttime && m_spellInfo->StartRecoveryTime && GetCurrentContainer() == CURRENT_GENERIC_SPELL)
            m_instantSpellDelayed = true;
    }
}

void Spell::cancel()
{
    if (m_spellState == SPELL_STATE_FINISHED)
        return;

    uint32 oldState = m_spellState;
    m_spellState = SPELL_STATE_FINISHED;

    m_autoRepeat = false;
    switch (oldState)
    {
        case SPELL_STATE_PREPARING:
            CancelGlobalCooldown();
            if (m_caster->GetTypeId() == TYPEID_PLAYER)
                m_caster->ToPlayer()->RestoreSpellMods(this);
            // no break
        case SPELL_STATE_DELAYED:
            SendInterrupted(0);
            SendCastResult(SPELL_FAILED_INTERRUPTED);
            break;

        case SPELL_STATE_CASTING:
            for (std::list<TargetInfo>::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                if ((*ihit).missCondition == SPELL_MISS_NONE)
                    if (Unit* unit = m_caster->GetGUID() == ihit->targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, ihit->targetGUID))
                        unit->RemoveOwnedAura(m_spellInfo->Id, m_originalCasterGUID, 0, AURA_REMOVE_BY_CANCEL);

            SendChannelUpdate(0);
            SendInterrupted(0);
            SendCastResult(SPELL_FAILED_INTERRUPTED);

            // spell is canceled-take mods and clear list
            if (m_caster->GetTypeId() == TYPEID_PLAYER && m_dropModsPhase == SpellModDropPhase::OnSpellFinish)
                m_caster->ToPlayer()->RemoveSpellMods(this);

            m_appliedMods.clear();
            break;

        default:
            break;
    }

    SetReferencedFromCurrent(false);
    if (m_selfContainer && *m_selfContainer == this)
        *m_selfContainer = NULL;

    m_caster->RemoveDynObject(m_spellInfo->Id);
    if (m_spellInfo->IsChanneled()) // if not channeled then the object for the current cast wasn't summoned yet
        m_caster->RemoveGameObject(m_spellInfo->Id, true);

    //set state back so finish will be processed
    m_spellState = oldState;

    finish(false);
}

void Spell::cast(bool skipCheck)
{
    // update pointers base at GUIDs to prevent access to non-existed already object
    UpdatePointers();

    // cancel at lost explicit target during cast
    if (m_targets.GetObjectTargetGUID() && !m_targets.GetObjectTarget())
    {
        cancel();
        return;
    }

    if (Player* playerCaster = m_caster->ToPlayer())
    {
        // now that we've done the basic check, now run the scripts
        // should be done before the spell is actually executed
        sScriptMgr->OnPlayerSpellCast(playerCaster, this, skipCheck);

        // As of 3.0.2 pets begin attacking their owner's target immediately
        // Let any pets know we've attacked something. Check DmgClass for harmful spells only
        // This prevents spells such as Hunter's Mark from triggering pet attack
        if (this->GetSpellInfo()->DmgClass != SPELL_DAMAGE_CLASS_NONE)
            if (Pet* playerPet = playerCaster->GetPet())
                if (Unit* target = m_targets.GetUnitTarget())
                    if (!m_spellInfo->HasAttribute(SPELL_ATTR3_NO_INITIAL_AGGRO) && !m_spellInfo->HasAttribute(SPELL_ATTR0_CU_DONT_SET_IN_COMBAT_STATE) && !playerCaster->IsFriendlyTo(target))
                        if (playerPet->IsAlive() && playerPet->isControlled() && (m_targets.GetTargetMask() & TARGET_FLAG_UNIT))
                            playerPet->AI()->OwnerAttacked(m_targets.GetObjectTarget()->ToUnit());
    }

    SetExecutedCurrently(true);

    if (!(_triggeredCastFlags & TRIGGERED_IGNORE_SET_FACING))
        if (m_caster->GetTypeId() == TYPEID_UNIT && m_targets.GetObjectTarget() && m_caster != m_targets.GetObjectTarget())
            m_caster->SetInFront(m_targets.GetObjectTarget());

    // Should this be done for original caster?
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        // Set spell which will drop charges for triggered cast spells
        // if not successfully casted, will be remove in finish(false)
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, true);
    }

    CallScriptBeforeCastHandlers();

    // skip check if done already (for instant cast spells for example)
    if (!skipCheck)
    {
        SpellCastResult castResult = CheckCast(false);
        if (castResult != SPELL_CAST_OK)
        {
            SendCastResult(castResult);
            SendInterrupted(0);
            //restore spell mods
            if (m_caster->GetTypeId() == TYPEID_PLAYER)
            {
                m_caster->ToPlayer()->RestoreSpellMods(this);
                // cleanup after mod system
                // triggered spell pointer can be not removed in some cases
                m_caster->ToPlayer()->SetSpellModTakingSpell(this, false);
            }
            finish(false);
            SetExecutedCurrently(false);
            return;
        }

        // additional check after cast bar completes (must not be in CheckCast)
        // if trade not complete then remember it in trade data
        if (m_targets.GetTargetMask() & TARGET_FLAG_TRADE_ITEM)
        {
            if (m_caster->GetTypeId() == TYPEID_PLAYER)
            {
                if (TradeData* my_trade = m_caster->ToPlayer()->GetTradeData())
                {
                    if (!my_trade->IsInAcceptProcess())
                    {
                        // Spell will be casted at completing the trade. Silently ignore at this place
                        my_trade->SetSpell(m_spellInfo->Id, m_CastItem);
                        SendCastResult(SPELL_FAILED_DONT_REPORT);
                        SendInterrupted(0);
                        m_caster->ToPlayer()->RestoreSpellMods(this);
                        // cleanup after mod system
                        // triggered spell pointer can be not removed in some cases
                        m_caster->ToPlayer()->SetSpellModTakingSpell(this, false);
                        finish(false);
                        SetExecutedCurrently(false);
                        return;
                    }
                }
            }
        }
    }

    SelectSpellTargets();

    // Spell may be finished after target map check
    if (m_spellState == SPELL_STATE_FINISHED)
    {
        SendInterrupted(0);
        //restore spell mods
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
        {
            m_caster->ToPlayer()->RestoreSpellMods(this);
            // cleanup after mod system
            // triggered spell pointer can be not removed in some cases
            m_caster->ToPlayer()->SetSpellModTakingSpell(this, false);
        }
        finish(false);
        SetExecutedCurrently(false);
        return;
    }

    PrepareTriggersExecutedOnHit();

    CallScriptOnCastHandlers();

    PrepareScriptHitHandlers();

    HandleLaunchPhase();                // Before procs, we need hit result here

    DoTriggersOnCast(ProcPhase::Cast);  // Before TakePower

    // traded items have trade slot instead of guid in m_itemTargetGUID
    // set to real guid to be sent later to the client
    m_targets.UpdateTradeSlotItem();

    if (Player* player = m_caster->ToPlayer())
    {
        if (!(_triggeredCastFlags & TRIGGERED_IGNORE_CAST_ITEM) && m_CastItem)
        {
            player->StartCriteria(CRITERIA_START_TYPE_ITEM, m_CastItem->GetEntry());
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_USE_ITEM, m_CastItem->GetEntry(), 0, 0, player);
        }

        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL, m_spellInfo->Id, 0, 0, player);
    }

    if (!(_triggeredCastFlags & TRIGGERED_IGNORE_POWER_AND_REAGENT_COST))
    {
        // Powers have to be taken before SendSpellGo
        TakePower();
        TakeReagents();                                         // we must remove reagents before HandleEffects to allow place crafted item in same slot
    }
    else if (Item* targetItem = m_targets.GetItemTarget())
    {
        /// Not own traded item (in trader trade slot) req. reagents including triggered spell case
        if (targetItem->GetOwnerGUID() != m_caster->GetGUID())
            TakeReagents();
    }

    if (!(_triggeredCastFlags & TRIGGERED_CAST_DIRECTLY) && (m_casttime || m_spellInfo->AttributesEx5 & SPELL_ATTR5_HASTE_AFFECT_DURATION))
    {
        Unit* caster = m_caster;
        if (m_caster->HasAura(114108)) // Soul of the Forest haste buff
            m_caster->m_Events.Schedule(1, [caster] { caster->RemoveAurasDueToSpell(114108); });
    }

    // CAST SPELL
    SendSpellCooldown();

    // we must send smsg_spell_go packet before m_castItem delete in TakeCastItem()...
    SendSpellGo();

    // Okay, everything is prepared. Now we need to distinguish between immediate and evented delayed spells
    if (((m_spellInfo->Speed > 0.0f || m_delayMoment || m_spellInfo->AttributesEx4 & SPELL_ATTR4_UNK4) && !m_spellInfo->IsChanneled())
        || m_spellInfo->Id == 54957 || m_spellInfo->Id == 131547) // FIXME
    {
        // Remove used for cast item if need (it can be already NULL after TakeReagents call
        // in case delayed spell remove item at cast delay start
        TakeCastItem();

        // Okay, maps created, now prepare flags
        m_immediateHandled = false;
        m_spellState = SPELL_STATE_DELAYED;
        SetDelayStart(0);

        if (m_caster->HasUnitState(UNIT_STATE_CASTING) && !m_caster->IsNonMeleeSpellCasted(false, false, true))
            m_caster->ClearUnitState(UNIT_STATE_CASTING);

        if (m_caster->GetTypeId() == TYPEID_UNIT)
        {
            if (m_caster->HasUnitState(UNIT_STATE_ROTATING) && (m_spellInfo->HasAttribute(SPELL_ATTR5_DONT_TURN_DURING_CAST) || m_spellInfo->HasAttribute(SPELL_ATTR11_DONT_TURN_DURING_CAST)))
            {
                auto checkSpell = [=](CurrentSpellTypes spellType)
                {
                    Spell* spell = m_caster->GetCurrentSpell(spellType);
                    return !spell || spell == this || spell->getState() == SPELL_STATE_FINISHED || (!spell->GetSpellInfo()->HasAttribute(SPELL_ATTR5_DONT_TURN_DURING_CAST) && !spell->GetSpellInfo()->HasAttribute(SPELL_ATTR11_DONT_TURN_DURING_CAST));
                };
                if (checkSpell(CURRENT_GENERIC_SPELL) && checkSpell(CURRENT_CHANNELED_SPELL))
                    m_caster->ClearUnitState(UNIT_STATE_ROTATING);
            }
        }
    }
    else
    {
        if (!m_spellInfo->IsChanneled())
            m_dropModsPhase = SpellModDropPhase::OnSpellFinish;
        // Immediate spell, no big deal
        handle_immediate();
    }

    HandleAfterCast();

    if (const std::vector<int32> *spell_triggered = sSpellMgr->GetSpellLinked(m_spellInfo->Id))
    {
        for (std::vector<int32>::const_iterator i = spell_triggered->begin(); i != spell_triggered->end(); ++i)
            if (*i < 0)
                m_caster->RemoveAurasDueToSpell(-(*i));
            else
                m_caster->CastSpell(m_targets.GetUnitTarget() ? m_targets.GetUnitTarget() : m_caster, *i, true);
    }

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, false);

        //Clear spell cooldowns after every spell is cast if .cheat cooldown is enabled.
        if (m_caster->ToPlayer()->GetCommandStatus(CHEAT_COOLDOWN))
            m_caster->ToPlayer()->RemoveSpellCooldown(m_spellInfo->Id, true);

        if (m_caster->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PREPARATION) && !(GetSpellInfo()->AttributesEx & SPELL_ATTR0_TRADESPELL))
            if (Battleground* bg = m_caster->ToPlayer()->GetBattleground())
                if (sWorld->IsArenaPrecastSpell(GetSpellInfo()->Id))
                    bg->UpdatePlayerScore(m_caster->ToPlayer(), SCORE_SPELLS_PRECAST, 1);

    }

    SetExecutedCurrently(false);
}

void Spell::handle_immediate()
{
    Unit* target = GetExplTargetUnit();
    bool explicitTarget = target && m_originalCaster && (
        (m_UniqueTargetInfo.size() == 1 && target != m_caster && GetTargetInfo(target->GetGUID())) ||
        (m_UniqueTargetInfo.size() == 2 && GetTargetInfo(target->GetGUID()) && GetTargetInfo(m_caster->GetGUID()))
        );

    // start channeling if applicable
    if (m_spellInfo->IsChanneled())
    {
        int32 duration = m_spellInfo->GetDuration();

        if (duration > 0)
        {
            // First mod_duration then haste - see Missile Barrage
            // Apply duration mod
            if (Player* modOwner = m_caster->GetSpellModOwner())
                modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_DURATION, duration);

            if (m_spellInfo->HasAttribute(SPELL_ATTR5_HASTE_AFFECT_DURATION))
                duration *= m_caster->GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED);
            else if (m_spellInfo->HasAttribute(SPELL_ATTR8_HASTE_AFFECT_DURATION))
                duration *= m_caster->GetFloatValue(UNIT_FIELD_MOD_HASTE_REGEN);

            if (explicitTarget)
            {
                duration = m_caster->ModSpellDuration(GetSpellInfo(), target, duration, m_spellInfo->IsPositive(), GetTargetInfo(target->GetGUID())->effectMask);

                if (DiminishingGroup diminishGroup = GetDiminishingReturnsGroupForSpell(m_spellInfo))
                {
                    int32 limitduration = GetDiminishingReturnsLimitDuration(diminishGroup, m_spellInfo);
                    if (limitduration && duration > limitduration)
                        duration = limitduration;

                    target->ApplyDiminishingToDuration(diminishGroup, duration, m_originalCaster, target->GetDiminishing(diminishGroup));
                }
            }

            if (duration)
                duration += m_additionalChannelDuration;
        }

        if (duration)
        {
            m_spellState = SPELL_STATE_CASTING;
            m_caster->AddInterruptMask(m_spellInfo->ChannelInterruptFlags);
            SendChannelStart(duration);
        }
    }

    // process immediate effects (items, ground, etc.) also initialize some variables
    _handle_immediate_phase();

    for (auto&& itr : m_reflectTargetInfo)
    {
        // Start triggers for remove charges if need (trigger only for victim, and mark as active spell)
        if (Unit* target = ObjectAccessor::GetUnit(*m_caster, itr.targetGUID))
            m_caster->ProcDamageAndSpell(target, PROC_FLAG_NONE, PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG, PROC_EX_REFLECT, 1, BASE_ATTACK, m_spellInfo, nullptr, ProcTriggerContext(this));
    }

    for (std::list<TargetInfo>::iterator ihit= m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
        DoAllEffectOnTarget(&(*ihit));

    for (std::list<GOTargetInfo>::iterator ihit= m_UniqueGOTargetInfo.begin(); ihit != m_UniqueGOTargetInfo.end(); ++ihit)
        DoAllEffectOnTarget(&(*ihit));

    for (auto&& ihit : m_uniqueCorpseTargetInfo)
        DoAllEffectOnTarget(&ihit);

    // Interrupt channel if aura wasn't applied
    if (m_spellInfo->IsChanneled() && explicitTarget)
        if (!target->HasAura(m_spellInfo->Id, m_originalCaster->GetGUID()))
            SendChannelUpdate(0);

    FinishTargetProcessing();

    // spell is finished, perform some last features of the spell here
    _handle_finish_phase();

    // Remove used for cast item if need (it can be already NULL after TakeReagents call
    TakeCastItem();

    // handle ammo consumption for thrown weapons
    //if (m_spellInfo->IsRangedWeaponSpell() && m_spellInfo->IsChanneled())
    //   TakeAmmo();

    if (m_spellState != SPELL_STATE_CASTING)
        finish(true);                                       // successfully finish spell cast (not last in case autorepeat or channel spell)
}

uint64 Spell::handle_delayed(uint64 t_offset)
{
    UpdatePointers();

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, true);

    uint64 next_time = 0;

    if (!m_immediateHandled)
    {
        _handle_immediate_phase();
        m_immediateHandled = true;
    }

    bool single_missile = (m_targets.HasDst());

    // now recheck units targeting correctness (need before any effects apply to prevent adding immunity at first effect not allow apply second spell effect and similar cases)
    for (std::list<TargetInfo>::iterator ihit= m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->processed == false)
        {
            if (single_missile || ihit->timeDelay <= t_offset)
            {
                ihit->timeDelay = t_offset;
                DoAllEffectOnTarget(&(*ihit));
            }
            else if (next_time == 0 || ihit->timeDelay < next_time)
                next_time = ihit->timeDelay;
        }
    }

    // now recheck gameobject targeting correctness
    for (std::list<GOTargetInfo>::iterator ighit= m_UniqueGOTargetInfo.begin(); ighit != m_UniqueGOTargetInfo.end(); ++ighit)
    {
        if (ighit->processed == false)
        {
            if (single_missile || ighit->timeDelay <= t_offset)
                DoAllEffectOnTarget(&(*ighit));
            else if (next_time == 0 || ighit->timeDelay < next_time)
                next_time = ighit->timeDelay;
        }
    }

    // now recheck corpse targeting correctness
    for (auto&& itr : m_uniqueCorpseTargetInfo)
    {
        if (itr.processed == false)
        {
            if (single_missile || itr.timeDelay <= t_offset)
                DoAllEffectOnTarget(&itr);
            else if (next_time == 0 || itr.timeDelay < next_time)
                next_time = itr.timeDelay;
        }
    }

    for (auto&& itr : m_reflectTargetInfo)
    {
        if (!itr.processed)
        {
            if (itr.timeDelay <= t_offset)
            {
                // Start triggers for remove charges if need (trigger only for victim, and mark as active spell)
                if (Unit* target = ObjectAccessor::GetUnit(*m_caster, itr.targetGUID))
                    m_caster->ProcDamageAndSpell(target, PROC_FLAG_NONE, PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG, PROC_EX_REFLECT, 1, BASE_ATTACK, m_spellInfo, nullptr, ProcTriggerContext(this));
                itr.processed = true;
            }
        }
    }

    FinishTargetProcessing();

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, false);

    // All targets passed - need finish phase
    if (next_time == 0)
    {
        // spell is finished, perform some last features of the spell here
        _handle_finish_phase();

        finish(true);                                       // successfully finish spell cast

        // return zero, spell is finished now
        return 0;
    }
    else
    {
        // spell is unfinished, return next execution time
        return next_time;
    }
}

void Spell::_handle_immediate_phase()
{
    m_spellAura = NULL;
    // initialize Diminishing Returns Data
    m_diminishLevel = DIMINISHING_LEVEL_1;
    m_diminishGroup = DIMINISHING_NONE;

    // handle some immediate features of the spell here
    HandleThreatSpells();

    PrepareScriptHitHandlers();

    // handle effects with SPELL_EFFECT_HANDLE_HIT mode
    for (uint32 j = 0; j < MAX_SPELL_EFFECTS; ++j)
    {
        // don't do anything for empty effect
        if (!m_spellInfo->Effects[j].IsEffect())
            continue;

        // call effect handlers to handle destination hit
        HandleEffects(nullptr, nullptr, nullptr, nullptr, j, SPELL_EFFECT_HANDLE_HIT);
    }

    // process items
    for (std::list<ItemTargetInfo>::iterator ihit= m_UniqueItemInfo.begin(); ihit != m_UniqueItemInfo.end(); ++ihit)
        DoAllEffectOnTarget(&(*ihit));

    if (!m_originalCaster)
        return;
    // Handle procs on cast
    /// @todo finish new proc system:P
    if (m_UniqueTargetInfo.empty() && m_targets.HasDst())
    {
        uint32 procAttacker = m_procAttacker;
        if (!procAttacker)
            procAttacker |= PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS;

        // Proc the spells that have DEST target
        m_originalCaster->ProcDamageAndSpell(nullptr, procAttacker, 0, m_procEx | PROC_EX_NORMAL_HIT, 0, BASE_ATTACK, m_spellInfo, m_triggeredByAuraSpell, ProcTriggerContext(this));
    }
}

void Spell::_handle_finish_phase()
{
    if (m_caster->m_movedPlayer)
    {
        // Take for real after all targets are processed
        if (m_needComboPoints)
            m_caster->m_movedPlayer->ClearComboPoints();
    }

    if (m_caster->m_extraAttacks && GetSpellInfo()->HasEffect(SPELL_EFFECT_ADD_EXTRA_ATTACKS))
        m_caster->HandleProcExtraAttackFor(m_caster->GetVictim());

    /// @todo trigger proc phase finish here
}

void Spell::SendSpellCooldown()
{
    bool startCooldown = m_caster->GetTypeId() == TYPEID_PLAYER || m_caster->HasUnitTypeMask(UNIT_MASK_CONTROLABLE_GUARDIAN);
    if (!startCooldown)
        return;

    // Block spell and category
    if (m_spellInfo->IsCooldownStartedOnEvent() || (m_CastItem && m_CastItem->IsPotion()))
        m_caster->GetSpellHistory()->SetCooldownOnHold(m_spellInfo, m_CastItem ? m_CastItem->GetEntry() : 0);

    // mana/health/etc potions, disabled by client (until combat out as declarate)
    if (m_CastItem && (m_CastItem->IsPotion() || m_spellInfo->IsCooldownStartedOnEvent()))
    {
        // need in some way provided data for Spell::finish SendCooldownEvent
        m_caster->ToPlayer()->SetLastPotionId(m_CastItem->GetEntry());
        return;
    }

    // have infinity cooldown but set at aura apply                  // do not set cooldown for triggered spells (needed by reincarnation)
    if ((m_spellInfo->IsCooldownStartedOnEvent() || m_spellInfo->IsPassive() || (_triggeredCastFlags & TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD)) && !m_spellInfo->IsTriggeredHasSpellCooldown())
        return;

    m_caster->GetSpellHistory()->StartCooldown(m_spellInfo, m_CastItem ? m_CastItem->GetEntry() : 0, this);

    if (IsAutoRepeat())
        m_caster->ToUnit()->resetAttackTimer(RANGED_ATTACK);
}

void Spell::update(uint32 difftime)
{
    // update pointers based at it's GUIDs
    UpdatePointers();

    if (m_targets.GetUnitTargetGUID() && !m_targets.GetUnitTarget())
    {
        TC_LOG_DEBUG("spells", "Spell %u is cancelled due to removal of target.", m_spellInfo->Id);
        cancel();
        return;
    }

    // check if the player caster has moved before the spell finished
    // with the exception of spells affected with SPELL_AURA_CAST_WHILE_WALKING effect
    if ((m_caster->GetTypeId() == TYPEID_PLAYER && m_timer != 0) &&
        m_caster->isMoving() && (m_spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT) &&
        (m_spellInfo->Effects[0].Effect != SPELL_EFFECT_STUCK || !m_caster->HasUnitMovementFlag(MOVEMENTFLAG_FALLING_FAR)) &&
        !m_caster->HasAuraTypeWithAffectMask(SPELL_AURA_CAST_WHILE_WALKING, m_spellInfo))
    {
        // don't cancel for melee, autorepeat, triggered and instant spells
        if (!IsNextMeleeSwingSpell() && !IsAutoRepeat() && !IsTriggered())
        {
            // if charmed by creature, trust the AI not to cheat and allow the cast to proceed
            // @todo this is a hack, "creature" movesplines don't differentiate turning/moving right now
            // however, checking what type of movement the spline is for every single spline would be really expensive
            if (!m_caster->GetCharmerGUID().IsCreatureOrVehicle())
                cancel();
        }
    }

    switch (m_spellState)
    {
        case SPELL_STATE_PREPARING:
        {
            if (m_timer > 0)
            {
                if (difftime >= (uint32)m_timer)
                    m_timer = 0;
                else
                    m_timer -= difftime;
            }

            if (m_timer == 0 && !IsNextMeleeSwingSpell())
                // don't CheckCast for instant spells - done in spell::prepare, skip duplicate checks, needed for range checks for example
                cast(!m_casttime);
            break;
        }
        case SPELL_STATE_CASTING:
        {
            if (m_timer)
            {
                // check if there are alive targets left
                if (!UpdateChanneledTargetList())
                {
                    TC_LOG_DEBUG("spells", "Channeled spell %d is removed due to lack of targets", m_spellInfo->Id);
                    SendChannelUpdate(0);
                    finish();
                }

                if (m_timer > 0)
                {
                    if (difftime >= (uint32)m_timer)
                        m_timer = 0;
                    else
                        m_timer -= difftime;
                }
            }

            if (m_timer == 0)
            {
                SendChannelUpdate(0);
                finish();
            }
            break;
        }
        default:
            break;
    }
}

void Spell::finish(bool ok)
{
    if (m_spellState == SPELL_STATE_FINISHED)
        return;
    m_spellState = SPELL_STATE_FINISHED;

    if (!m_caster)
        return;

    // successful cast of the initial autorepeat spell is moved to idle state so that it is not deleted as long as autorepeat is active
    if (IsAutoRepeat() && m_caster->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL) == this)
        m_spellState = SPELL_STATE_IDLE;

    if (m_spellInfo->IsChanneled())
        m_caster->UpdateInterruptMask();

    if (m_caster->HasUnitState(UNIT_STATE_CASTING) && !m_caster->IsNonMeleeSpellCasted(false, false, true))
        m_caster->ClearUnitState(UNIT_STATE_CASTING);

    // Unsummon summon as possessed creatures on spell cancel
    if (m_spellInfo->IsChanneled() && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        if (Unit* charm = m_caster->GetCharm())
            if (charm->GetTypeId() == TYPEID_UNIT
                && charm->ToCreature()->HasUnitTypeMask(UNIT_MASK_PUPPET)
                && charm->GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL) == m_spellInfo->Id)
                ((Puppet*)charm)->UnSummon();
    }

    if (Creature* creatureCaster = m_caster->ToCreature())
        creatureCaster->ReleaseFocus(this);

    if (m_caster->GetTypeId() == TYPEID_UNIT)
    {
        if (m_caster->HasUnitState(UNIT_STATE_ROTATING) && (m_spellInfo->HasAttribute(SPELL_ATTR5_DONT_TURN_DURING_CAST) || m_spellInfo->HasAttribute(SPELL_ATTR11_DONT_TURN_DURING_CAST)))
        {
            auto checkSpell = [=](CurrentSpellTypes spellType)
            {
                Spell* spell = m_caster->GetCurrentSpell(spellType);
                return !spell || spell == this || spell->getState() == SPELL_STATE_FINISHED || (!spell->GetSpellInfo()->HasAttribute(SPELL_ATTR5_DONT_TURN_DURING_CAST) && !spell->GetSpellInfo()->HasAttribute(SPELL_ATTR11_DONT_TURN_DURING_CAST));
            };
            if (checkSpell(CURRENT_GENERIC_SPELL) && checkSpell(CURRENT_CHANNELED_SPELL))
                m_caster->ClearUnitState(UNIT_STATE_ROTATING);
        }
    }

    if (!ok)
        return;

    if (m_caster->GetTypeId() == TYPEID_UNIT && m_caster->ToCreature()->IsSummon())
    {
        // Unsummon statue
        uint32 spell = m_caster->GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL);
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell);
        if (spellInfo && spellInfo->SpellIconID == 2056)
        {
            TC_LOG_DEBUG("spells", "Statue %d is unsummoned in spell %d finish", m_caster->GetGUID().GetCounter(), m_spellInfo->Id);
            m_caster->setDeathState(JUST_DIED);
            return;
        }
    }

    // TODO: optimize this
    if (m_caster->GetTypeId() == TYPEID_UNIT && m_caster->GetMap()->IsDungeon() &&
        !m_caster->GetOwnerGUID().IsPlayer() && m_spellInfo->IsTargetingArea())
    {
        if (Group* group = m_caster->GetMap()->GetInstanceGroup())
        {
            Player* player;
            if (Guild* guild = group->GetGroupGuild(m_caster, &player))
            {
                guild->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, m_spellInfo->Id, 0, 0, m_caster, player);
                guild->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2, m_spellInfo->Id, 0, 0, player, player);
            }
        }
    }

    // potions disabled by client, send event "not in combat" if need
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        if (!m_triggeredByAuraSpell)
            m_caster->ToPlayer()->UpdatePotionCooldown(this);

        if (m_dropModsPhase == SpellModDropPhase::OnSpellFinish)
        {
            // triggered spell pointer can be not set in some cases
            // this is needed for proper apply of triggered spell mods
            m_caster->ToPlayer()->SetSpellModTakingSpell(this, true);

            // Take mods after trigger spell (needed for 14177 to affect 48664)
            // mods are taken only on succesfull cast and independantly from targets of the spell

            m_caster->ToPlayer()->RemoveSpellMods(this);
            m_caster->ToPlayer()->SetSpellModTakingSpell(this, false);
        }
    }

    // Stop Attack for some spells
    if (m_spellInfo->Attributes & SPELL_ATTR0_STOP_ATTACK_TARGET)
        m_caster->AttackStop();

    // handle archaeology research
    if (m_researchData)
        if (Player* player = m_caster->ToPlayer())
            player->SolveResearchProject(this);
}

void Spell::SendCastResult(SpellCastResult result)
{
    if (result == SPELL_CAST_OK)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    if (m_caster->ToPlayer()->GetSession()->PlayerLoading())  // don't send cast results at loading time
        return;

    SendCastResult(m_caster->ToPlayer(), m_spellInfo, m_cast_count, result, m_customError, SMSG_CAST_FAILED);
}

void Spell::SendPetCastResult(SpellCastResult result)
{
    if (result == SPELL_CAST_OK)
        return;

    Unit* owner = m_caster->GetCharmerOrOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    SendCastResult(owner->ToPlayer(), m_spellInfo, m_cast_count, result, SPELL_CUSTOM_ERROR_NONE, SMSG_PET_CAST_FAILED);
}

void Spell::SendCastResult(Player* caster, SpellInfo const* spellInfo, uint8 cast_count, SpellCastResult result, SpellCustomErrors customError /*= SPELL_CUSTOM_ERROR_NONE*/, OpcodeServer opcode /*= SMSG_CAST_FAILED*/)
{
    if (result == SPELL_CAST_OK)
        return;

    WorldPacket data(opcode, (4+1+1));
    if (opcode == SMSG_CAST_FAILED)
    {
        data << uint32(spellInfo->Id);
        data << uint32(result);                                  // problem
    }
    else
    {
        data << uint32(result);                                  // problem
        data << uint32(spellInfo->Id);
    }
    data << uint8(cast_count);

    size_t pos = data.bitwpos();
    data.WriteBit(1);
    data.WriteBit(1);
    data.FlushBits();

    size_t dataPos = data.wpos();
    switch (result)
    {
        case SPELL_FAILED_NOT_READY:
            data << uint32(0);                              // unknown (value 1 update cooldowns on client flag)
            break;
        case SPELL_FAILED_REQUIRES_SPELL_FOCUS:
            data << uint32(spellInfo->RequiresSpellFocus);  // SpellFocusObject.dbc id
            break;
        case SPELL_FAILED_REQUIRES_AREA:                    // AreaTable.dbc id
            // hardcode areas limitation case
            switch (spellInfo->Id)
            {
                case 41617:                                 // Cenarion Mana Salve
                case 41619:                                 // Cenarion Healing Salve
                    data << uint32(3905);
                    break;
                case 41618:                                 // Bottled Nethergon Energy
                case 41620:                                 // Bottled Nethergon Vapor
                    data << uint32(3842);
                    break;
                case 45373:                                 // Bloodberry Elixir
                    data << uint32(4075);
                    break;
                default:                                    // default case (don't must be)
                    data << uint32(0);
                    break;
            }
            break;
        case SPELL_FAILED_TOTEMS:
            if (spellInfo->Totem[0])
                data << uint32(spellInfo->Totem[0]);
            if (spellInfo->Totem[1])
                data << uint32(spellInfo->Totem[1]);
            break;
        case SPELL_FAILED_TOTEM_CATEGORY:
            if (spellInfo->TotemCategory[0])
                data << uint32(spellInfo->TotemCategory[0]);
            if (spellInfo->TotemCategory[1])
                data << uint32(spellInfo->TotemCategory[1]);
            break;
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS:
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS_MAINHAND:
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS_OFFHAND:
            data << uint32(spellInfo->EquippedItemSubClassMask);
            data << uint32(spellInfo->EquippedItemClass);
            break;
        case SPELL_FAILED_TOO_MANY_OF_ITEM:
        {
             uint32 item = 0;
             for (int8 eff = 0; eff < MAX_SPELL_EFFECTS; eff++)
                 if (spellInfo->Effects[eff].ItemType)
                     item = spellInfo->Effects[eff].ItemType;
             ItemTemplate const* proto = sObjectMgr->GetItemTemplate(item);
             if (proto && proto->ItemLimitCategory)
                 data << uint32(proto->ItemLimitCategory);
             break;
        }
        case SPELL_FAILED_PREVENTED_BY_MECHANIC:
            // SpellMechanic.dbc id
            if (caster->HasAuraType(SPELL_AURA_MOD_POSSESS) || caster->HasAuraType(SPELL_AURA_MOD_CHARM) || caster->HasAuraType(SPELL_AURA_AOE_CHARM))
                data << MECHANIC_CHARM;
            else
                data << MECHANIC_POLYMORPH;
            break;
        case SPELL_FAILED_NEED_EXOTIC_AMMO:
            data << uint32(spellInfo->EquippedItemSubClassMask); // seems correct...
            break;
        case SPELL_FAILED_NEED_MORE_ITEMS:
            data << uint32(0);                              // Item id
            data << uint32(0);                              // Item count?
            break;
        case SPELL_FAILED_MIN_SKILL:
            data << uint32(0);                              // SkillLine.dbc id
            data << uint32(0);                              // required skill value
            break;
        case SPELL_FAILED_FISHING_TOO_LOW:
            data << uint32(0);                              // required fishing skill
            break;
        case SPELL_FAILED_CUSTOM_ERROR:
            data << uint32(customError);
            break;
        case SPELL_FAILED_SILENCED:
            data << uint32(0);                              // Unknown
            break;
        case SPELL_FAILED_REAGENTS:
        {
            uint32 missingItem = 0;
            for (uint32 i = 0; i < MAX_SPELL_REAGENTS; i++)
            {
                if (spellInfo->Reagent[i] <= 0)
                    continue;

                uint32 itemid    = spellInfo->Reagent[i];
                uint32 itemcount = spellInfo->ReagentCount[i];

                if (!caster->HasItemCount(itemid, itemcount))
                {
                    missingItem = itemid;
                    break;
                }
            }

            data << uint32(missingItem);  // first missing item
            break;
        }
        // TODO: SPELL_FAILED_NOT_STANDING
        default:
            break;
    }

    if (int32 diff = data.wpos() - dataPos)
    {
        if (result == SPELL_FAILED_REQUIRES_SPELL_FOCUS || result == SPELL_FAILED_CUSTOM_ERROR || result == SPELL_FAILED_REAGENTS || result == SPELL_FAILED_PREVENTED_BY_MECHANIC)
            data.PutBits(pos + 1, 0, 1);  // 1 and 0
        else if ((diff / 4) == 1)
            data.PutBits(pos, 0, 1); // 0 and 1
        else
            data.PutBits(pos, 0, 2); // 1 and 1
    }
    caster->GetSession()->SendPacket(&data);
}

void Spell::SendSpellStart()
{
    if (!IsNeedSendToClient())
        return;

    uint32 castFlags = CAST_FLAG_HAS_TRAJECTORY;
    if (((IsTriggered() && !m_spellInfo->IsAutoRepeatRangedSpell()) || m_triggeredByAuraSpell) && !m_cast_count)
        castFlags |= CAST_FLAG_PENDING;

    if ((m_caster->GetTypeId() == TYPEID_PLAYER ||
        (m_caster->GetTypeId() == TYPEID_UNIT && m_caster->ToCreature()->IsPet()))
         && m_powerType != POWER_HEALTH)
        castFlags |= CAST_FLAG_POWER_LEFT_SELF;

    if (m_spellInfo->RuneCostID && m_powerType == POWER_RUNES)
        castFlags |= CAST_FLAG_UNKNOWN_19;

    ObjectGuid casterGuid = m_CastItem ? m_CastItem->GetGUID() : m_caster->GetGUID();
    ObjectGuid casterUnitGuid = m_caster->GetGUID();
    ObjectGuid targetGuid = m_targets.GetObjectTargetGUID();
    ObjectGuid itemTargetGuid = m_targets.GetItemTargetGUID();
    ObjectGuid unkGuid = ObjectGuid::Empty;
    bool hasDestLocation = (m_targets.GetTargetMask() & TARGET_FLAG_DEST_LOCATION) && m_targets.GetDst();
    bool hasSourceLocation = (m_targets.GetTargetMask() & TARGET_FLAG_SOURCE_LOCATION) && m_targets.GetSrc();
    bool hasTargetString = m_targets.GetTargetMask() & TARGET_FLAG_STRING;
    bool hasPredictedHeal = castFlags & CAST_FLAG_HEAL_PREDICTION;
    bool hasPredictedType = castFlags & CAST_FLAG_HEAL_PREDICTION;
    bool hasTargetMask = m_targets.GetTargetMask() != 0;
    bool hasCastImmunities = castFlags & CAST_FLAG_IMMUNITY;
    bool hasCastSchoolImmunities = castFlags & CAST_FLAG_IMMUNITY;
    bool hasElevation = castFlags & CAST_FLAG_ADJUST_MISSILE;
    bool hasVisualChain = castFlags & CAST_FLAG_VISUAL_CHAIN;
    bool hasAmmoInventoryType = castFlags & CAST_FLAG_PROJECTILE;
    bool hasAmmoDisplayId = castFlags & CAST_FLAG_PROJECTILE;
    uint8 runeCooldownPassedCount = (castFlags & CAST_FLAG_RUNE_LIST) && m_caster->GetTypeId() == TYPEID_PLAYER ? MAX_RUNES : 0;
    uint8 predictedPowerCount = castFlags & CAST_FLAG_POWER_LEFT_SELF ? 1 : 0;

    WorldPacket data(SMSG_SPELL_START, 25);

    data.WriteBits(0, 24); // Miss Count (not used currently in SMSG_SPELL_START)
    data.WriteBit(casterGuid[5]);

    //for (uint32 i = 0; i < missCount; ++i)
    //{
    //}

    data.WriteBit(1); // Unk read int8
    data.WriteBit(0); // Fake Bit
    data.WriteBit(casterUnitGuid[4]);
    data.WriteBit(casterGuid[2]);
    data.WriteBits(runeCooldownPassedCount, 3); // Rune Cooldown Passed Count
    data.WriteBit(casterUnitGuid[2]);
    data.WriteBit(casterUnitGuid[6]);
    data.WriteBits(0, 25); // MissType Count (not used currently in SMSG_SPELL_START)
    data.WriteBits(0, 13); // Unknown Bits
    data.WriteBit(casterGuid[4]);
    data.WriteBits(0, 24); // Hit Count (not used currently in SMSG_SPELL_START)
    data.WriteBit(casterUnitGuid[7]);

    //for (uint32 i = 0; i < hitCount; ++i)
    //{
    //}

    data.WriteBit(hasSourceLocation);
    data.WriteBits(predictedPowerCount, 21);

    data.WriteBit(itemTargetGuid[3]);
    data.WriteBit(itemTargetGuid[0]);
    data.WriteBit(itemTargetGuid[1]);
    data.WriteBit(itemTargetGuid[7]);
    data.WriteBit(itemTargetGuid[2]);
    data.WriteBit(itemTargetGuid[6]);
    data.WriteBit(itemTargetGuid[4]);
    data.WriteBit(itemTargetGuid[5]);

    data.WriteBit(!hasElevation);
    data.WriteBit(!hasTargetString);
    data.WriteBit(!hasAmmoInventoryType);
    data.WriteBit(hasDestLocation);
    data.WriteBit(1); // Unk Read32
    data.WriteBit(casterGuid[3]);

    if (hasDestLocation)
    {
        ObjectGuid destTransportGuid = m_targets.GetDst()->_transportGUID;
        data.WriteBit(destTransportGuid[1]);
        data.WriteBit(destTransportGuid[6]);
        data.WriteBit(destTransportGuid[2]);
        data.WriteBit(destTransportGuid[7]);
        data.WriteBit(destTransportGuid[0]);
        data.WriteBit(destTransportGuid[3]);
        data.WriteBit(destTransportGuid[5]);
        data.WriteBit(destTransportGuid[4]);
    }

    data.WriteBit(!hasAmmoDisplayId);

    if (hasSourceLocation)
    {
        ObjectGuid srcTransportGuid = m_targets.GetSrc()->_transportGUID;
        data.WriteBit(srcTransportGuid[4]);
        data.WriteBit(srcTransportGuid[3]);
        data.WriteBit(srcTransportGuid[5]);
        data.WriteBit(srcTransportGuid[1]);
        data.WriteBit(srcTransportGuid[7]);
        data.WriteBit(srcTransportGuid[0]);
        data.WriteBit(srcTransportGuid[6]);
        data.WriteBit(srcTransportGuid[2]);
    }

    data.WriteBit(0); // Fake Bit
    data.WriteBit(casterGuid[6]);

    data.WriteBit(unkGuid[2]);
    data.WriteBit(unkGuid[1]);
    data.WriteBit(unkGuid[7]);
    data.WriteBit(unkGuid[6]);
    data.WriteBit(unkGuid[0]);
    data.WriteBit(unkGuid[5]);
    data.WriteBit(unkGuid[3]);
    data.WriteBit(unkGuid[4]);

    data.WriteBit(!hasTargetMask);

    if (hasTargetMask)
        data.WriteBits(m_targets.GetTargetMask(), 20);

    data.WriteBit(casterGuid[1]);
    data.WriteBit(!hasPredictedHeal);
    data.WriteBit(1); // Unk read int8
    data.WriteBit(!hasCastSchoolImmunities);
    data.WriteBit(casterUnitGuid[5]);
    data.WriteBit(0); // Fake Bit
    data.WriteBits(0, 20); // Extra Target Count (not used currently in SMSG_SPELL_START)

    //for (uint32 i = 0; i < extraTargetCount; ++i)
    //{
    //}

    data.WriteBit(targetGuid[1]);
    data.WriteBit(targetGuid[4]);
    data.WriteBit(targetGuid[6]);
    data.WriteBit(targetGuid[7]);
    data.WriteBit(targetGuid[5]);
    data.WriteBit(targetGuid[3]);
    data.WriteBit(targetGuid[0]);
    data.WriteBit(targetGuid[2]);

    data.WriteBit(casterGuid[0]);
    data.WriteBit(casterUnitGuid[3]);
    data.WriteBit(1); // Unk uint8

    if (hasTargetString)
        data.WriteBits(uint32(m_targets.GetTargetString().length()), 7);

    //for (uint32 i = 0; i < missTypeCount; ++i)
    //{
    //}

    data.WriteBit(!hasCastImmunities);
    data.WriteBit(casterUnitGuid[1]);
    data.WriteBit(hasVisualChain);
    data.WriteBit(casterGuid[7]);
    data.WriteBit(!hasPredictedType);
    data.WriteBit(casterUnitGuid[0]);

    data.FlushBits();

    data.WriteByteSeq(itemTargetGuid[1]);
    data.WriteByteSeq(itemTargetGuid[7]);
    data.WriteByteSeq(itemTargetGuid[6]);
    data.WriteByteSeq(itemTargetGuid[0]);
    data.WriteByteSeq(itemTargetGuid[4]);
    data.WriteByteSeq(itemTargetGuid[2]);
    data.WriteByteSeq(itemTargetGuid[3]);
    data.WriteByteSeq(itemTargetGuid[5]);

    //for (uint32 i = 0; i < hitCount; ++i)
    //{
    //}

    data.WriteByteSeq(targetGuid[4]);
    data.WriteByteSeq(targetGuid[5]);
    data.WriteByteSeq(targetGuid[1]);
    data.WriteByteSeq(targetGuid[7]);
    data.WriteByteSeq(targetGuid[6]);
    data.WriteByteSeq(targetGuid[3]);
    data.WriteByteSeq(targetGuid[2]);
    data.WriteByteSeq(targetGuid[0]);

    data << uint32(m_casttime);

    data.WriteByteSeq(unkGuid[4]);
    data.WriteByteSeq(unkGuid[5]);
    data.WriteByteSeq(unkGuid[3]);
    data.WriteByteSeq(unkGuid[2]);
    data.WriteByteSeq(unkGuid[1]);
    data.WriteByteSeq(unkGuid[6]);
    data.WriteByteSeq(unkGuid[7]);
    data.WriteByteSeq(unkGuid[0]);

    if (hasDestLocation)
    {
        float x, y, z;
        ObjectGuid destTransportGuid = m_targets.GetDst()->_transportGUID;
        if (destTransportGuid)
            m_targets.GetDst()->_transportOffset.GetPosition(x, y, z);
        else
            m_targets.GetDst()->_position.GetPosition(x, y, z);

        data.WriteByteSeq(destTransportGuid[4]);
        data.WriteByteSeq(destTransportGuid[0]);
        data.WriteByteSeq(destTransportGuid[5]);
        data.WriteByteSeq(destTransportGuid[7]);
        data.WriteByteSeq(destTransportGuid[1]);
        data.WriteByteSeq(destTransportGuid[2]);
        data.WriteByteSeq(destTransportGuid[3]);
        data << y;
        data << z;
        data.WriteByteSeq(destTransportGuid[6]);
        data << x;
    }

    //for (uint32 i = 0; i < extraTargetCount; ++i)
    //{
    //}

    if (hasSourceLocation)
    {
        float x, y, z;
        ObjectGuid srcTransportGuid = m_targets.GetSrc()->_transportGUID;
        if (srcTransportGuid)
            m_targets.GetSrc()->_transportOffset.GetPosition(x, y, z);
        else
            m_targets.GetSrc()->_position.GetPosition(x, y, z);

        data.WriteByteSeq(srcTransportGuid[0]);
        data.WriteByteSeq(srcTransportGuid[5]);
        data.WriteByteSeq(srcTransportGuid[4]);
        data.WriteByteSeq(srcTransportGuid[7]);
        data.WriteByteSeq(srcTransportGuid[3]);
        data.WriteByteSeq(srcTransportGuid[6]);
        data << x;
        data.WriteByteSeq(srcTransportGuid[2]);
        data << z;
        data.WriteByteSeq(srcTransportGuid[1]);
        data << y;
    }

    data.WriteByteSeq(casterGuid[4]);

    //for (uint32 i = 0; i < missCount; ++i)
    //{
    //}

    if (hasCastSchoolImmunities)
        data << uint32(0);

    data.WriteByteSeq(casterGuid[2]);

    if (hasCastImmunities)
        data << uint32(0);

    if (hasVisualChain)
    {
        data << uint32(0);
        data << uint32(0);
    }

    if (predictedPowerCount > 0)
    {
        data << int32(m_caster->GetPower(m_powerType));
        data << uint8(m_powerType);
    }

    data << uint32(castFlags);

    data.WriteByteSeq(casterGuid[5]);
    data.WriteByteSeq(casterGuid[7]);
    data.WriteByteSeq(casterGuid[1]);

    data << uint8(m_cast_count);

    data.WriteByteSeq(casterUnitGuid[7]);
    data.WriteByteSeq(casterUnitGuid[0]);
    data.WriteByteSeq(casterGuid[6]);
    data.WriteByteSeq(casterGuid[0]);
    data.WriteByteSeq(casterUnitGuid[1]);

    if (hasAmmoInventoryType)
        data << uint8(0);

    if (hasPredictedHeal)
        data << uint32(0);

    data.WriteByteSeq(casterUnitGuid[6]);
    data.WriteByteSeq(casterUnitGuid[3]);

    data << uint32(m_spellInfo->Id);

    if (hasAmmoDisplayId)
        data << uint32(0);

    data.WriteByteSeq(casterUnitGuid[4]);
    data.WriteByteSeq(casterUnitGuid[5]);
    data.WriteByteSeq(casterUnitGuid[2]);

    if (hasTargetString)
        data.WriteString(m_targets.GetTargetString());

    if (hasPredictedType)
        data << uint8(0);

    data.WriteByteSeq(casterGuid[3]);

    if (hasElevation)
        data << float(m_targets.GetElevation());

    for (uint8 i = 0; i < runeCooldownPassedCount; ++i)
        data << uint8(float(m_caster->ToPlayer()->GetRune(i).Readiness) * 255.f / RUNE_READINESS_FULL);

    m_caster->SendMessageToSet(&data, true);
}

void Spell::SendSpellGo()
{
    // not send invisible spell casting
    if (!IsNeedSendToClient())
        return;

    //TC_LOG_DEBUG("spells", "Sending SMSG_SPELL_GO id=%u", m_spellInfo->Id);

    uint32 castFlags = CAST_FLAG_UNKNOWN_9 | m_spellValue->SpellGoFlags;

    // triggered spells with spell visual != 0
    if (((IsTriggered() && !m_spellInfo->IsAutoRepeatRangedSpell()) || m_triggeredByAuraSpell) && !m_cast_count)
        castFlags |= CAST_FLAG_PENDING;

    if ((m_caster->GetTypeId() == TYPEID_PLAYER ||
        (m_caster->GetTypeId() == TYPEID_UNIT && m_caster->ToCreature()->IsPet()))
        && m_powerType != POWER_HEALTH)
        castFlags |= CAST_FLAG_POWER_LEFT_SELF; // should only be sent to self, but the current messaging doesn't make that possible

    if ((m_caster->GetTypeId() == TYPEID_PLAYER)
        && (m_caster->GetClass() == CLASS_DEATH_KNIGHT)
        && m_spellInfo->RuneCostID
        && m_powerType == POWER_RUNES)
    {
        castFlags |= CAST_FLAG_UNKNOWN_19;                   // same as in SMSG_SPELL_START
        castFlags |= CAST_FLAG_RUNE_LIST;                    // rune cooldowns list
    }

    if (m_spellInfo->HasEffect(SPELL_EFFECT_ACTIVATE_RUNE))
    {
        castFlags |= CAST_FLAG_RUNE_LIST;                    // rune cooldowns list
        castFlags |= CAST_FLAG_UNKNOWN_19;                   // same as in SMSG_SPELL_START
    }

    if (m_targets.HasTraj())
        castFlags |= CAST_FLAG_ADJUST_MISSILE;

    if (m_spellInfo->Id == 24450)
        castFlags = 768;

    ObjectGuid casterGuid = m_CastItem ? m_CastItem->GetGUID() : m_caster->GetGUID();
    ObjectGuid casterUnitGuid = m_caster->GetGUID();
    ObjectGuid targetGuid = m_targets.GetObjectTargetGUID();
    ObjectGuid itemTargetGuid = m_targets.GetItemTargetGUID();
    ObjectGuid unkGuid = ObjectGuid::Empty;
    bool hasDestLocation = (m_targets.GetTargetMask() & TARGET_FLAG_DEST_LOCATION) && m_targets.GetDst();
    bool hasSourceLocation = (m_targets.GetTargetMask() & TARGET_FLAG_SOURCE_LOCATION) && m_targets.GetSrc();
    bool hasDestUnkByte = m_targets.GetTargetMask() & TARGET_FLAG_DEST_LOCATION;
    bool hasTargetString = m_targets.GetTargetMask() & TARGET_FLAG_STRING;
    bool hasPredictedHeal = castFlags & CAST_FLAG_HEAL_PREDICTION;
    bool hasPredictedType = castFlags & CAST_FLAG_HEAL_PREDICTION;
    bool hasTargetMask = m_targets.GetTargetMask() != 0;
    bool hasCastImmunities = castFlags & CAST_FLAG_IMMUNITY;
    bool hasCastSchoolImmunities = castFlags & CAST_FLAG_IMMUNITY;
    bool hasElevation = castFlags & CAST_FLAG_ADJUST_MISSILE;
    bool hasDelayTime = castFlags & CAST_FLAG_ADJUST_MISSILE;
    bool hasVisualChain = castFlags & CAST_FLAG_VISUAL_CHAIN;
    bool hasAmmoInventoryType = castFlags & CAST_FLAG_PROJECTILE;
    bool hasAmmoDisplayId = castFlags & CAST_FLAG_PROJECTILE;
    bool hasRunesStateBefore = (castFlags & CAST_FLAG_RUNE_LIST) && m_caster->GetTypeId() == TYPEID_PLAYER;
    bool hasRunesStateAfter = (castFlags & CAST_FLAG_RUNE_LIST) && m_caster->GetTypeId() == TYPEID_PLAYER;
    uint8 predictedPowerCount = castFlags & CAST_FLAG_POWER_LEFT_SELF ? 1 : 0;
    uint8 runeCooldownPassedCount = (castFlags & CAST_FLAG_RUNE_LIST) && m_caster->GetTypeId() == TYPEID_PLAYER ? MAX_RUNES : 0;

    // This function also fill data for channeled spells:
    // m_needAliveTargetMask req for stop channelig if one target die
    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if ((*ihit).effectMask == 0)                  // No effect apply - all immuned add state
            // possibly SPELL_MISS_IMMUNE2 for this??
            ihit->missCondition = SPELL_MISS_IMMUNE2;
    }

    WorldPacket data(SMSG_SPELL_GO, 25);

    data.WriteBit(casterUnitGuid[2]);
    data.WriteBit(1); // hasAmmoDisplayType
    data.WriteBit(hasSourceLocation);
    data.WriteBit(casterGuid[2]);

    if (hasSourceLocation)
    {
        ObjectGuid srcTransportGuid = m_targets.GetSrc()->_transportGUID;
        data.WriteBit(srcTransportGuid[3]);
        data.WriteBit(srcTransportGuid[7]);
        data.WriteBit(srcTransportGuid[4]);
        data.WriteBit(srcTransportGuid[2]);
        data.WriteBit(srcTransportGuid[0]);
        data.WriteBit(srcTransportGuid[6]);
        data.WriteBit(srcTransportGuid[1]);
        data.WriteBit(srcTransportGuid[5]);
    }

    data.WriteBit(casterGuid[6]);
    data.WriteBit(!hasDestUnkByte);
    data.WriteBit(casterUnitGuid[7]);
    data.WriteBits(0, 20); // Extra Target Count

    size_t missTypeCountPos = data.bitwpos();
    data.WriteBits(0, 25); // Miss Type Count

    size_t missCountPos = data.bitwpos();
    data.WriteBits(0, 24); // Miss Count

    data.WriteBit(casterUnitGuid[1]);
    data.WriteBit(casterGuid[0]);
    data.WriteBits(IsTriggered() ? 0 : 0, 13); // Unknown bits

    uint32 missCount = 0;
    for (std::list<TargetInfo>::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->missCondition != SPELL_MISS_NONE)
        {
            ObjectGuid missGuid = ihit->targetGUID;
            data.WriteBit(missGuid[1]);
            data.WriteBit(missGuid[3]);
            data.WriteBit(missGuid[6]);
            data.WriteBit(missGuid[4]);
            data.WriteBit(missGuid[5]);
            data.WriteBit(missGuid[2]);
            data.WriteBit(missGuid[0]);
            data.WriteBit(missGuid[7]);
            missCount++;
        }
    }

    //for (uint32 i = 0; i < extraTargetCount; ++i)
    //{
    //}

    data.WriteBit(casterUnitGuid[5]);
    data.WriteBit(0); // Fake bit
    data.WriteBit(0); // Fake bit
    data.WriteBit(!hasTargetString);

    data.WriteBit(itemTargetGuid[7]);
    data.WriteBit(itemTargetGuid[2]);
    data.WriteBit(itemTargetGuid[1]);
    data.WriteBit(itemTargetGuid[3]);
    data.WriteBit(itemTargetGuid[6]);
    data.WriteBit(itemTargetGuid[0]);
    data.WriteBit(itemTargetGuid[5]);
    data.WriteBit(itemTargetGuid[4]);

    data.WriteBit(casterGuid[7]);

    data.WriteBit(targetGuid[0]);
    data.WriteBit(targetGuid[6]);
    data.WriteBit(targetGuid[5]);
    data.WriteBit(targetGuid[7]);
    data.WriteBit(targetGuid[4]);
    data.WriteBit(targetGuid[2]);
    data.WriteBit(targetGuid[3]);
    data.WriteBit(targetGuid[1]);

    data.WriteBit(!hasRunesStateBefore);
    data.WriteBits(predictedPowerCount, 21); // predictedPowerCount
    data.WriteBit(casterGuid[1]);
    data.WriteBit(!hasPredictedType);
    data.WriteBit(!hasTargetMask);
    data.WriteBit(casterUnitGuid[3]);

    if (hasTargetString)
        data.WriteBits(uint32(m_targets.GetTargetString().length()), 7);

    data.WriteBit(1); // Missing Predict heal
    data.WriteBit(0); // hasPowerData
    data.WriteBit(1); // has castImmunitiy
    data.WriteBit(casterUnitGuid[6]);
    data.WriteBit(0); // Fake bit
    data.WriteBit(hasVisualChain);

    data.WriteBit(unkGuid[7]);
    data.WriteBit(unkGuid[6]);
    data.WriteBit(unkGuid[1]);
    data.WriteBit(unkGuid[2]);
    data.WriteBit(unkGuid[0]);
    data.WriteBit(unkGuid[5]);
    data.WriteBit(unkGuid[3]);
    data.WriteBit(unkGuid[4]);

    data.WriteBit(!hasDelayTime);
    data.WriteBit(1); // has School Immunities
    data.WriteBits(runeCooldownPassedCount, 3); // runeCooldownPassedCount
    data.WriteBit(casterUnitGuid[0]);

    uint32 missTypeCount = 0;
    for (std::list<TargetInfo>::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->missCondition != SPELL_MISS_NONE)
        {
            data.WriteBits(ihit->missCondition, 4);
            if (ihit->missCondition == SPELL_MISS_REFLECT)
                data.WriteBits(ihit->reflectResult, 4);

            ++missTypeCount;
        }
    }

    if (hasTargetMask)
        data.WriteBits(m_targets.GetTargetMask(), 20);

    data.WriteBit(!hasElevation);
    data.WriteBit(!hasRunesStateAfter);
    data.WriteBit(casterGuid[4]);
    data.WriteBit(1); // hasAmmodisplayID
    data.WriteBit(hasDestLocation);
    data.WriteBit(casterGuid[5]);

    size_t hitCountPos = data.bitwpos();
    data.WriteBits(0, 24); // Hit Count

    if (hasDestLocation)
    {
        ObjectGuid destTransportGuid = m_targets.GetDst()->_transportGUID;
        data.WriteBit(destTransportGuid[0]);
        data.WriteBit(destTransportGuid[3]);
        data.WriteBit(destTransportGuid[2]);
        data.WriteBit(destTransportGuid[1]);
        data.WriteBit(destTransportGuid[4]);
        data.WriteBit(destTransportGuid[5]);
        data.WriteBit(destTransportGuid[6]);
        data.WriteBit(destTransportGuid[7]);
    }

    data.WriteBit(casterUnitGuid[4]);

    uint32 hitCount = 0;
    for (auto&& itr : m_UniqueTargetInfo)
    {
        if (itr.missCondition == SPELL_MISS_NONE)
        {
            data.WriteBit(itr.targetGUID[2]);
            data.WriteBit(itr.targetGUID[7]);
            data.WriteBit(itr.targetGUID[1]);
            data.WriteBit(itr.targetGUID[6]);
            data.WriteBit(itr.targetGUID[4]);
            data.WriteBit(itr.targetGUID[5]);
            data.WriteBit(itr.targetGUID[0]);
            data.WriteBit(itr.targetGUID[3]);
            m_channelTargetEffectMask |= itr.effectMask;
            ++hitCount;
        }
    }

    for (auto&& itr : m_UniqueGOTargetInfo)
    {
        data.WriteBit(itr.targetGUID[2]);
        data.WriteBit(itr.targetGUID[7]);
        data.WriteBit(itr.targetGUID[1]);
        data.WriteBit(itr.targetGUID[6]);
        data.WriteBit(itr.targetGUID[4]);
        data.WriteBit(itr.targetGUID[5]);
        data.WriteBit(itr.targetGUID[0]);
        data.WriteBit(itr.targetGUID[3]);
        ++hitCount;
    }

    for (auto&& itr : m_uniqueCorpseTargetInfo)
    {
        data.WriteBit(itr.targetGUID[2]);
        data.WriteBit(itr.targetGUID[7]);
        data.WriteBit(itr.targetGUID[1]);
        data.WriteBit(itr.targetGUID[6]);
        data.WriteBit(itr.targetGUID[4]);
        data.WriteBit(itr.targetGUID[5]);
        data.WriteBit(itr.targetGUID[0]);
        data.WriteBit(itr.targetGUID[3]);
        ++hitCount;
    }

    data.WriteBit(casterGuid[3]);
    data.FlushBits();

    data.PutBits(missCountPos, missCount, 24);
    data.PutBits(missTypeCountPos, missTypeCount, 25);
    data.PutBits(hitCountPos, hitCount, 24);

    data.WriteByteSeq(targetGuid[5]);
    data.WriteByteSeq(targetGuid[2]);
    data.WriteByteSeq(targetGuid[1]);
    data.WriteByteSeq(targetGuid[6]);
    data.WriteByteSeq(targetGuid[0]);
    data.WriteByteSeq(targetGuid[3]);
    data.WriteByteSeq(targetGuid[4]);
    data.WriteByteSeq(targetGuid[7]);

    data.WriteByteSeq(itemTargetGuid[5]);
    data.WriteByteSeq(itemTargetGuid[2]);
    data.WriteByteSeq(itemTargetGuid[0]);
    data.WriteByteSeq(itemTargetGuid[6]);
    data.WriteByteSeq(itemTargetGuid[7]);
    data.WriteByteSeq(itemTargetGuid[3]);
    data.WriteByteSeq(itemTargetGuid[1]);
    data.WriteByteSeq(itemTargetGuid[4]);

    data.WriteByteSeq(casterGuid[2]);

    //for (uint32 i = 0; i < extraTargetCount; ++i)
    //{
    //}

    for (auto&& itr : m_UniqueTargetInfo)
        if (itr.missCondition == SPELL_MISS_NONE)
        {
            data.WriteByteSeq(itr.targetGUID[0]);
            data.WriteByteSeq(itr.targetGUID[6]);
            data.WriteByteSeq(itr.targetGUID[2]);
            data.WriteByteSeq(itr.targetGUID[7]);
            data.WriteByteSeq(itr.targetGUID[5]);
            data.WriteByteSeq(itr.targetGUID[4]);
            data.WriteByteSeq(itr.targetGUID[3]);
            data.WriteByteSeq(itr.targetGUID[1]);
        }

    for (auto&& itr : m_UniqueGOTargetInfo)
    {
        data.WriteByteSeq(itr.targetGUID[0]);
        data.WriteByteSeq(itr.targetGUID[6]);
        data.WriteByteSeq(itr.targetGUID[2]);
        data.WriteByteSeq(itr.targetGUID[7]);
        data.WriteByteSeq(itr.targetGUID[5]);
        data.WriteByteSeq(itr.targetGUID[4]);
        data.WriteByteSeq(itr.targetGUID[3]);
        data.WriteByteSeq(itr.targetGUID[1]);
    }

    for (auto&& itr : m_uniqueCorpseTargetInfo)
    {
        data.WriteByteSeq(itr.targetGUID[0]);
        data.WriteByteSeq(itr.targetGUID[6]);
        data.WriteByteSeq(itr.targetGUID[2]);
        data.WriteByteSeq(itr.targetGUID[7]);
        data.WriteByteSeq(itr.targetGUID[5]);
        data.WriteByteSeq(itr.targetGUID[4]);
        data.WriteByteSeq(itr.targetGUID[3]);
        data.WriteByteSeq(itr.targetGUID[1]);
    }

    data.WriteByteSeq(unkGuid[6]);
    data.WriteByteSeq(unkGuid[2]);
    data.WriteByteSeq(unkGuid[7]);
    data.WriteByteSeq(unkGuid[1]);
    data.WriteByteSeq(unkGuid[4]);
    data.WriteByteSeq(unkGuid[3]);
    data.WriteByteSeq(unkGuid[5]);
    data.WriteByteSeq(unkGuid[0]);

    if (hasDelayTime)
        data << uint32(m_delayMoment);

    m_spellGoTime = getMSTime();
    data << uint32(m_spellGoTime);

    for (std::list<TargetInfo>::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->missCondition != SPELL_MISS_NONE)
        {
            ObjectGuid missGuid = ihit->targetGUID;
            data.WriteByteSeq(missGuid[4]);
            data.WriteByteSeq(missGuid[2]);
            data.WriteByteSeq(missGuid[0]);
            data.WriteByteSeq(missGuid[6]);
            data.WriteByteSeq(missGuid[7]);
            data.WriteByteSeq(missGuid[5]);
            data.WriteByteSeq(missGuid[1]);
            data.WriteByteSeq(missGuid[3]);
        }
    }

    if (hasDestLocation)
    {
        float x, y, z;
        ObjectGuid destTransportGuid = m_targets.GetDst()->_transportGUID;
        if (destTransportGuid)
            m_targets.GetDst()->_transportOffset.GetPosition(x, y, z);
        else
            m_targets.GetDst()->_position.GetPosition(x, y, z);

        data << z;
        data << y;
        data.WriteByteSeq(destTransportGuid[4]);
        data.WriteByteSeq(destTransportGuid[5]);
        data.WriteByteSeq(destTransportGuid[7]);
        data.WriteByteSeq(destTransportGuid[6]);
        data.WriteByteSeq(destTransportGuid[1]);
        data.WriteByteSeq(destTransportGuid[2]);
        data << x;
        data.WriteByteSeq(destTransportGuid[0]);
        data.WriteByteSeq(destTransportGuid[3]);
    }

    data.WriteByteSeq(casterGuid[6]);
    data.WriteByteSeq(casterUnitGuid[7]);
    data.WriteByteSeq(casterGuid[1]);

    if (hasVisualChain)
    {
        data << uint32(0);
        data << uint32(0);
    }

    data << uint32(castFlags);

    if (hasSourceLocation)
    {
        float x, y, z;
        ObjectGuid srcTransportGuid = m_targets.GetSrc()->_transportGUID;
        if (srcTransportGuid)
            m_targets.GetSrc()->_transportOffset.GetPosition(x, y, z);
        else
            m_targets.GetSrc()->_position.GetPosition(x, y, z);

        data.WriteByteSeq(srcTransportGuid[2]);
        data << y;
        data << x;
        data.WriteByteSeq(srcTransportGuid[6]);
        data.WriteByteSeq(srcTransportGuid[5]);
        data.WriteByteSeq(srcTransportGuid[1]);
        data.WriteByteSeq(srcTransportGuid[7]);
        data << z;
        data.WriteByteSeq(srcTransportGuid[3]);
        data.WriteByteSeq(srcTransportGuid[0]);
        data.WriteByteSeq(srcTransportGuid[4]);
    }

    data.WriteByteSeq(casterUnitGuid[6]);

    if (hasPredictedType)
        data << uint8(0);

    data.WriteByteSeq(casterGuid[4]);
    data.WriteByteSeq(casterUnitGuid[1]);

    if (predictedPowerCount > 0)
    {
        //for (uint32 i = 0; i < powerUnitPowerCount; ++i)
        //{
        //    data << int32(powerValue);
        //    data << uint8(powerType);
        //}

        data << uint8(m_powerType);
        data << int32(m_caster->GetPower(m_powerType));
    }

    if (hasRunesStateAfter)
        data << uint8(m_caster->ToPlayer()->GetRunesState());

    for (uint8 i = 0; i < runeCooldownPassedCount; ++i)
        data << uint8(m_caster->ToPlayer()->GetRune(i).Readiness * 255.f / RUNE_READINESS_FULL);

    if (hasRunesStateBefore)
        data << uint8(m_runesState);

    data.WriteByteSeq(casterGuid[0]);

    if (hasDestUnkByte)
        data << uint8(0);

    data << uint8(m_cast_count);

    data.WriteByteSeq(casterGuid[5]);
    data.WriteByteSeq(casterUnitGuid[2]);
    data.WriteByteSeq(casterGuid[3]);
    data.WriteByteSeq(casterUnitGuid[5]);

    if (hasTargetString)
        data.WriteString(m_targets.GetTargetString());

    data << uint32(m_spellInfo->Id);

    if (hasElevation)
        data << m_targets.GetElevation();

    data.WriteByteSeq(casterUnitGuid[0]);
    data.WriteByteSeq(casterUnitGuid[3]);
    data.WriteByteSeq(casterUnitGuid[4]);
    data.WriteByteSeq(casterGuid[7]);

    // Reset m_needAliveTargetMask for non channeled spell
    if (!m_spellInfo->IsChanneled())
        m_channelTargetEffectMask = 0;

    m_caster->SendMessageToSet(&data, true);
}

void Spell::SendLogExecute()
{
    if (!m_effectExecuteData.size())
        return;

    ObjectGuid guid = m_caster->GetGUID();
    bool unkBit = false;
    uint32 unkBitsSize = 0;

    WorldPacket data(SMSG_SPELL_EXECUTE_LOG);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[2]);
    data.WriteBits(m_effectExecuteData.size(), 19);
    data.WriteBit(guid[4]);

    for (auto itr : m_effectExecuteData)
    {
        auto helper = itr.second;

        data.WriteBits(helper.ExtraAttacks.size(), 21);
        for (auto extraAttack : helper.ExtraAttacks)
        {
            data.WriteBit(extraAttack.Guid[5]);
            data.WriteBit(extraAttack.Guid[4]);
            data.WriteBit(extraAttack.Guid[2]);
            data.WriteBit(extraAttack.Guid[3]);
            data.WriteBit(extraAttack.Guid[1]);
            data.WriteBit(extraAttack.Guid[0]);
            data.WriteBit(extraAttack.Guid[6]);
            data.WriteBit(extraAttack.Guid[7]);
        }

        data.WriteBits(helper.Energizes.size(), 20);
        for (auto energize : helper.Energizes)
        {
            data.WriteBit(energize.Guid[0]);
            data.WriteBit(energize.Guid[3]);
            data.WriteBit(energize.Guid[1]);
            data.WriteBit(energize.Guid[5]);
            data.WriteBit(energize.Guid[6]);
            data.WriteBit(energize.Guid[4]);
            data.WriteBit(energize.Guid[7]);
            data.WriteBit(energize.Guid[2]);
        }

        data.WriteBits(0, 21); // unk counter
        data.WriteBits(helper.PetFeed.size(), 22); // petFeedSize
        data.WriteBits(helper.CreatedItems.size(), 22);

        data.WriteBits(helper.Targets.size(), 24);
        for (auto target : helper.Targets)
        {
            data.WriteBit(target[6]);
            data.WriteBit(target[5]);
            data.WriteBit(target[1]);
            data.WriteBit(target[0]);
            data.WriteBit(target[3]);
            data.WriteBit(target[4]);
            data.WriteBit(target[7]);
            data.WriteBit(target[2]);
        }
    }

    data.WriteBit(guid[1]);
    data.WriteBit(guid[3]);

    data.WriteBit(unkBit);
    if (unkBit)
        data.WriteBits(unkBitsSize, 21);

    data.FlushBits();

    if (unkBit)
    {
        for (uint32 i = 0; i < unkBitsSize; ++i)
        {
            data << uint32(0);
            data << uint32(0);
        }

        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
    }

    for (auto itr : m_effectExecuteData)
    {
        auto effIndex = itr.first;
        auto helper = itr.second;

        for (auto target : helper.Targets)
        {
            data.WriteByteSeq(target[7]);
            data.WriteByteSeq(target[5]);
            data.WriteByteSeq(target[1]);
            data.WriteByteSeq(target[2]);
            data.WriteByteSeq(target[6]);
            data.WriteByteSeq(target[4]);
            data.WriteByteSeq(target[0]);
            data.WriteByteSeq(target[3]);
        }

        for (auto energize : helper.Energizes)
        {
            data.WriteByteSeq(energize.Guid[3]);
            data.WriteByteSeq(energize.Guid[7]);
            data.WriteByteSeq(energize.Guid[5]);
            data.WriteByteSeq(energize.Guid[2]);
            data.WriteByteSeq(energize.Guid[0]);
            data << uint32(energize.PowerType);
            data << uint32(energize.Value);
            data.WriteByteSeq(energize.Guid[4]);
            data.WriteByteSeq(energize.Guid[1]);
            data << float(energize.Multiplier);
            data.WriteByteSeq(energize.Guid[6]);
        }

        for (auto extraAttack : helper.ExtraAttacks)
        {
            data.WriteByteSeq(extraAttack.Guid[0]);
            data.WriteByteSeq(extraAttack.Guid[6]);
            data.WriteByteSeq(extraAttack.Guid[4]);
            data.WriteByteSeq(extraAttack.Guid[7]);
            data.WriteByteSeq(extraAttack.Guid[2]);
            data.WriteByteSeq(extraAttack.Guid[5]);
            data.WriteByteSeq(extraAttack.Guid[3]);
            data << uint32(extraAttack.Count);
            data.WriteByteSeq(extraAttack.Guid[1]);
        }
        
        for (auto petFeedEntry : helper.PetFeed)
            data << uint32(petFeedEntry);

        data << uint32(m_spellInfo->Effects[effIndex].Effect);

        for (auto itemEntry : helper.CreatedItems)
            data << uint32(itemEntry);
    }

    data << uint32(m_spellInfo->Id);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[3]);

    m_caster->SendMessageToSet(&data, true);

    m_effectExecuteData.clear();
}

void Spell::ExecuteLogEffectTakeTargetPower(uint8 effIndex, Unit* target, uint32 powerType, uint32 powerTaken, float gainMultiplier)
{
    m_effectExecuteData[effIndex].AddEnergize(target->GetGUID(), gainMultiplier, powerTaken, powerType);
}

void Spell::ExecuteLogEffectExtraAttacks(uint8 effIndex, Unit* victim, uint32 attCount)
{
    m_effectExecuteData[effIndex].AddExtraAttacks(victim->GetGUID(), attCount);
}

void Spell::ExecuteLogEffectInterruptCast(uint8 /*effIndex*/, Unit* victim, uint32 spellId)
{
    ObjectGuid casterGuid = m_caster->GetGUID();
    ObjectGuid targetGuid = victim->GetGUID();

    WorldPacket data(SMSG_SPELLINTERRUPTLOG, 8 + 8 + 4 + 4);
    data.WriteBit(targetGuid[7]);
    data.WriteBit(targetGuid[2]);
    data.WriteBit(targetGuid[4]);
    data.WriteBit(targetGuid[6]);
    data.WriteBit(casterGuid[0]);
    data.WriteBit(casterGuid[2]);
    data.WriteBit(casterGuid[5]);
    data.WriteBit(casterGuid[1]);
    data.WriteBit(casterGuid[4]);
    data.WriteBit(targetGuid[0]);
    data.WriteBit(targetGuid[3]);
    data.WriteBit(casterGuid[7]);
    data.WriteBit(casterGuid[6]);
    data.WriteBit(targetGuid[1]);
    data.WriteBit(casterGuid[3]);
    data.WriteBit(targetGuid[5]);

    data.WriteByteSeq(targetGuid[0]);
    data.WriteByteSeq(casterGuid[2]);
    data << uint32(m_spellInfo->Id);
    data.WriteByteSeq(casterGuid[1]);
    data.WriteByteSeq(targetGuid[2]);
    data.WriteByteSeq(casterGuid[3]);
    data << uint32(spellId);
    data.WriteByteSeq(targetGuid[4]);
    data.WriteByteSeq(casterGuid[4]);
    data.WriteByteSeq(targetGuid[3]);
    data.WriteByteSeq(targetGuid[1]);
    data.WriteByteSeq(casterGuid[5]);
    data.WriteByteSeq(casterGuid[6]);
    data.WriteByteSeq(casterGuid[7]);
    data.WriteByteSeq(targetGuid[5]);
    data.WriteByteSeq(targetGuid[6]);
    data.WriteByteSeq(casterGuid[0]);
    data.WriteByteSeq(targetGuid[7]);

    m_caster->SendMessageToSet(&data, true);
}

void Spell::ExecuteLogEffectDurabilityDamage(uint8 effIndex, Unit* victim, int32 itemId, int32 slot)
{
    /*m_effectExecuteData[effIndex]->append(victim->GetPackGUID());
    *m_effectExecuteData[effIndex] << int32(itemId);
    *m_effectExecuteData[effIndex] << int32(slot);*/
}

void Spell::ExecuteLogEffectOpenLock(uint8 effIndex, Object* obj)
{
    m_effectExecuteData[effIndex].AddTarget(obj->GetGUID());
}

void Spell::ExecuteLogEffectCreateItem(uint8 effIndex, uint32 entry)
{
    m_effectExecuteData[effIndex].AddCreatedItem(entry);
}

void Spell::ExecuteLogEffectDestroyItem(uint8 effIndex, uint32 entry)
{
    m_effectExecuteData[effIndex].AddPetFeed(entry);
}

void Spell::ExecuteLogEffectSummonObject(uint8 effIndex, WorldObject* obj)
{
    m_effectExecuteData[effIndex].AddTarget(obj->GetGUID());
}

void Spell::ExecuteLogEffectUnsummonObject(uint8 effIndex, WorldObject* obj)
{
    m_effectExecuteData[effIndex].AddTarget(obj->GetGUID());
}

void Spell::ExecuteLogEffectResurrect(uint8 effIndex, Unit* target)
{
    m_effectExecuteData[effIndex].AddTarget(target->GetGUID());
}

void Spell::SendInterrupted(uint8 result)
{
    ObjectGuid guid = m_caster->GetGUID();

    WorldPacket data(SMSG_SPELL_FAILURE, (8 + 4 + 1));

    data.WriteBit(guid[7]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[4]);

    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[1]);

    data << uint8(result);                                  // problem
    data << uint32(m_spellInfo->Id);
    data << uint8(m_cast_count);

    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[5]);

    m_caster->SendMessageToSet(&data, true);

    data.Initialize(SMSG_SPELL_FAILED_OTHER, (8 + 4 + 1));
    data.WriteBit(guid[7]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[2]);

    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[1]);

    data << uint8(result);

    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[6]);

    data << uint32(m_spellInfo->Id);
    data << uint8(m_cast_count);

    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[3]);

    m_caster->SendMessageToSet(&data, true);
}

void Spell::SendChannelUpdate(uint32 time)
{
    if (time == 0)
    {
        m_caster->SetGuidValue(UNIT_FIELD_CHANNEL_OBJECT, ObjectGuid::Empty);
        m_caster->SetUInt32Value(UNIT_FIELD_CHANNEL_SPELL, 0);

        for (auto&& itr : m_UniqueTargetInfo)
            if (itr.effectMask & m_channelTargetEffectMask)
                if (Unit* unit = m_caster->GetGUID() == itr.targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, itr.targetGUID))
                    if (Aura* aura = unit->GetAura(m_spellInfo->Id, m_originalCasterGUID))
                        aura->SetDuration(0); // Do not remove aura, because caster can be updated before target and last tick won't happens
    }

    ObjectGuid CasterGUID = m_caster->GetGUID();

    WorldPacket data(SMSG_SPELL_CHANNEL_UPDATE, 8+4);

    data.WriteBit(CasterGUID[0]);
    data.WriteBit(CasterGUID[3]);
    data.WriteBit(CasterGUID[4]);
    data.WriteBit(CasterGUID[1]);
    data.WriteBit(CasterGUID[5]);
    data.WriteBit(CasterGUID[2]);
    data.WriteBit(CasterGUID[6]);
    data.WriteBit(CasterGUID[7]);

    data.WriteByteSeq(CasterGUID[4]);
    data.WriteByteSeq(CasterGUID[7]);
    data.WriteByteSeq(CasterGUID[1]);
    data.WriteByteSeq(CasterGUID[2]);
    data.WriteByteSeq(CasterGUID[6]);
    data.WriteByteSeq(CasterGUID[5]);
    data << uint32(time);
    data.WriteByteSeq(CasterGUID[0]);
    data.WriteByteSeq(CasterGUID[3]);
    m_caster->SendMessageToSet(&data, true);
}

void Spell::SendChannelStart(uint32 duration)
{
    ObjectGuid channelTarget = m_targets.GetObjectTargetGUID();
    if (!channelTarget && !m_spellInfo->NeedsExplicitUnitTarget())
        if (m_UniqueTargetInfo.size() + m_UniqueGOTargetInfo.size() == 1)   // this is for TARGET_SELECT_CATEGORY_NEARBY
            channelTarget = !m_UniqueTargetInfo.empty() ? m_UniqueTargetInfo.front().targetGUID : m_UniqueGOTargetInfo.front().targetGUID;

    uint32 SchoolImmunityMask = m_caster->GetSchoolImmunityMask();
    uint32 MechanicImmunityMask = m_caster->GetMechanicImmunityMask();
    ObjectGuid CasterGUID = m_caster->GetGUID();

    WorldPacket data(SMSG_SPELL_CHANNEL_START, (8+4+4/*+8+4+1+8*/));

    data.WriteBit(CasterGUID[7]);
    data.WriteBit(CasterGUID[5]);
    data.WriteBit(CasterGUID[4]);
    data.WriteBit(CasterGUID[1]);

    data.WriteBit(0); // healPrediction

    data.WriteBit(CasterGUID[3]);
    data.WriteBit(CasterGUID[2]);
    data.WriteBit(CasterGUID[0]);
    data.WriteBit(CasterGUID[6]);

    if (SchoolImmunityMask || MechanicImmunityMask)
        data.WriteBit(1);
    else
        data.WriteBit(0);

    /*
    if (healPrediction)
    {
    data.appendPackGUID(channelTarget);     // target packguid
    data << uint32();                       // spellid
    data << uint8(0);                       // unk3
    if (unk3 == 2)
    data.append();                      // unk packed guid (unused ?)
    }
    */

    if (SchoolImmunityMask || MechanicImmunityMask)
    {
        data << uint32(SchoolImmunityMask);                       // SchoolImmunityMask
        data << uint32(MechanicImmunityMask);                     // MechanicImmunityMask
    }

    data.WriteByteSeq(CasterGUID[6]);
    data.WriteByteSeq(CasterGUID[7]);
    data.WriteByteSeq(CasterGUID[3]);
    data.WriteByteSeq(CasterGUID[1]);
    data.WriteByteSeq(CasterGUID[0]);
    data << uint32(duration);
    data.WriteByteSeq(CasterGUID[5]);
    data.WriteByteSeq(CasterGUID[4]);
    data.WriteByteSeq(CasterGUID[2]);
    data << uint32(m_spellInfo->Id);  

    m_caster->SendMessageToSet(&data, true);

    m_timer = duration;
    m_channelTime = duration;
    if (channelTarget)
        m_caster->SetGuidValue(UNIT_FIELD_CHANNEL_OBJECT, channelTarget);

    m_caster->SetUInt32Value(UNIT_FIELD_CHANNEL_SPELL, m_spellInfo->Id);
}

void Spell::SendResurrectRequest(Player* target)
{
    Spell::SendResurrectRequest(m_caster, target, m_spellInfo->Id);
}

void Spell::SendResurrectRequest(Unit* caster, Player* target, uint32 spellId)
{
    // get ressurector name for creature resurrections, otherwise packet will be not accepted
    // for player resurrections the name is looked up by guid
    std::string const sentName(caster->GetTypeId() == TYPEID_PLAYER
                               ? ""
                               : caster->GetNameForLocaleIdx(target->GetSession()->GetSessionDbLocaleIndex()));

    ObjectGuid guid = caster->GetGUID();
    WorldPacket data(SMSG_RESURRECT_REQUEST, (8 + 4 + sentName.size() + 1 + 1 + 1 + 4));
    data << uint32(sentName.size() + 1);
    data << uint32(spellId);
    data << uint32(0);

    data.WriteBit(guid[3]);
    data.WriteBit(caster->GetTypeId() == TYPEID_PLAYER ? 0 : 1);
    data.WriteBit(false);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[7]);
    data.WriteBits(sentName.size(), 6);

    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[5]);
    data.WriteString(sentName);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[0]);

    target->GetSession()->SendPacket(&data);
}

void Spell::TakeCastItem()
{
    if (!m_CastItem)
        return;

    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    // not remove cast item at triggered spell (equipping, weapon damage, etc)
    if (_triggeredCastFlags & TRIGGERED_IGNORE_CAST_ITEM)
        return;

    ItemTemplate const* proto = m_CastItem->GetTemplate();

    if (!proto)
    {
        // This code is to avoid a crash
        // I'm not sure, if this is really an error, but I guess every item needs a prototype
        TC_LOG_ERROR("spells", "Cast item has no item prototype highId=%d, lowId=%d", m_CastItem->GetGUID().GetHigh(), m_CastItem->GetGUID().GetCounter());
        return;
    }

    bool expendable = false;
    bool withoutCharges = false;

    for (int i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
    {
        if (proto->Spells[i].SpellId)
        {
            // item has limited charges
            if (proto->Spells[i].SpellCharges)
            {
                if (proto->Spells[i].SpellCharges < 0)
                    expendable = true;

                int32 charges = m_CastItem->GetSpellCharges(i);

                // item has charges left
                if (charges)
                {
                    (charges > 0) ? --charges : ++charges;  // abs(charges) less at 1 after use
                    if (proto->Stackable == 1)
                        m_CastItem->SetSpellCharges(i, charges);
                    m_CastItem->SetState(ITEM_CHANGED, player);
                }

                // all charges used
                withoutCharges = (charges == 0);
            }
        }
    }

    if (expendable && withoutCharges)
    {
        uint32 count = 1;
        m_caster->ToPlayer()->DestroyItemCount(m_CastItem, count, true);

        // prevent crash at access to deleted m_targets.GetItemTarget
        if (m_CastItem == m_targets.GetItemTarget())
            m_targets.SetItemTarget(NULL);

        m_CastItem = NULL;
    }
}

void Spell::TakePower()
{
    if (m_CastItem || m_triggeredByAuraSpell)
        return;

    //Don't take power if the spell is cast while .cheat power is enabled.
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        if (m_caster->ToPlayer()->GetCommandStatus(CHEAT_POWER))
            return;
    }

    Powers powerType = m_powerType;
    bool hit = true;
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        if (powerType == POWER_RAGE || powerType == POWER_ENERGY || powerType == POWER_RUNES || powerType == POWER_CHI || powerType == POWER_HOLY_POWER)
            if (uint64 targetGUID = m_targets.GetUnitTargetGUID())
                for (std::list<TargetInfo>::iterator ihit= m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                    if (ihit->targetGUID == targetGUID)
                    {
                        if (ihit->missCondition != SPELL_MISS_NONE && ihit->missCondition != SPELL_MISS_BLOCK)
                        {
                            hit = false;
                            //lower spell cost on fail (by talent aura)
                            if (Player* modOwner = m_caster->ToPlayer()->GetSpellModOwner())
                                modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_SPELL_COST_REFUND_ON_FAIL, m_powerCost);
                        }
                        break;
                    }
    }

    if (powerType == POWER_RUNES)
    {
        TakeRunePower(hit);
        return;
    }

    if (!m_powerCost)
        return;

    // health as power used
    if (powerType == POWER_HEALTH)
    {
        m_caster->ModifyHealth(-(int32)m_powerCost);
        return;
    }

    if (powerType >= MAX_POWERS)
    {
        TC_LOG_ERROR("spells", "Spell::TakePower: Unknown power type '%d'", powerType);
        return;
    }

    if (hit)
        m_caster->ModifyPower(powerType, -m_powerCost);
    else
        m_caster->ModifyPower(powerType, -irand(0, m_powerCost/4));
}

/*void Spell::TakeAmmo()
{
    if (m_attackType == RANGED_ATTACK && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        Item* pItem = m_caster->ToPlayer()->GetWeaponForAttack(RANGED_ATTACK);

        // wands don't have ammo
        if (!pItem  || pItem->IsBroken() || pItem->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_WAND)
            return;

        if (pItem->GetTemplate()->InventoryType == INVTYPE_THROWN)
        {
            if (pItem->GetMaxStackCount() == 1)
            {
                // decrease durability for non-stackable throw weapon
                m_caster->ToPlayer()->DurabilityPointLossForEquipSlot(EQUIPMENT_SLOT_RANGED);
            }
            else
            {
                // decrease items amount for stackable throw weapon
                uint32 count = 1;
                m_caster->ToPlayer()->DestroyItemCount(pItem, count, true);
            }
        }
    }
}*/

SpellCastResult Spell::CheckRuneCost(uint32 runeCostID)
{
    if (m_powerType != POWER_RUNES || !runeCostID)
        return SPELL_CAST_OK;

    Player* player = m_caster->ToPlayer();
    if (!player)
        return SPELL_CAST_OK;

    if (player->GetClass() != CLASS_DEATH_KNIGHT)
        return SPELL_CAST_OK;

    SpellRuneCostEntry const* src = sSpellRuneCostStore.LookupEntry(runeCostID);
    if (!src)
        return SPELL_CAST_OK;

    if (src->NoRuneCost())
        return SPELL_CAST_OK;

    int32 runeCost[NUM_RUNE_TYPES];                         // blood, frost, unholy, death

    for (uint32 i = 0; i < NUM_RUNE_TYPES; ++i)
    {
        runeCost[i] = src->RuneCost[i];
        if (Player* modOwner = m_caster->GetSpellModOwner())
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_COST, runeCost[i], this);
    }

    for (uint32 i = 0; i < MAX_RUNES; ++i)
    {
        RuneType rune = player->GetCurrentRune(i);
        if (player->GetRune(i).State == RuneState::Ready)   // If death - just decrease below zero, creating "reserve"
            runeCost[rune]--;
    }

    for (uint32 i = 0; i < RUNE_DEATH; ++i)
        if (runeCost[i] > 0)
            runeCost[RUNE_DEATH] += runeCost[i];            // And here use "reserve" of death runes

    if (runeCost[RUNE_DEATH] > 0)
        return SPELL_FAILED_NO_POWER;

    return SPELL_CAST_OK;
}

void Spell::TakeRunePower(bool didHit)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER || m_caster->GetClass() != CLASS_DEATH_KNIGHT)
        return;

    SpellRuneCostEntry const* runeCostData = sSpellRuneCostStore.LookupEntry(m_spellInfo->RuneCostID);
    if (!runeCostData || (runeCostData->NoRuneCost() && runeCostData->NoRunicPowerGain()))
        return;

    if (!didHit && GetSpellInfo()->Id != 49998)    // Death Strike
        return;

    Player* player = m_caster->ToPlayer();
    m_runesState = player->GetRunesState();                 // store previous state

    int32 runeCost[NUM_RUNE_TYPES];                         // blood, frost, unholy, death
    player->ClearLastUsedRuneMask();
    SpellSchools school = GetFirstSchoolInMask(m_spellSchoolMask);

    for (uint32 i = 0; i < RUNE_DEATH; ++i)
    {
        runeCost[i] = runeCostData->RuneCost[i];
        if (Player* modOwner = m_caster->GetSpellModOwner())
        {
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_COST, runeCost[i], this);

            // PCT mod from user auras by school
            runeCost[i] = int32(runeCost[i] * (1.0f + m_caster->GetFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER + school)));
            if (runeCost[i] < 0)
                runeCost[i] = 0;
        }
    }

    runeCost[RUNE_DEATH] = 0;                               // calculated later

    bool gain_runic = runeCostData->NoRuneCost();           //  if spell doesn't have runecost - player can have some runic power, Horn of Winter for example

    for (uint32 i = 0; i < MAX_RUNES; ++i)
    {
        RuneType rune = player->GetCurrentRune(i);
        if (player->HasRuneCooldown(i) || !runeCost[rune])
            continue;

        player->ConsumeRune(i);
        player->SetLastUsedRuneIndex(i);

        runeCost[rune]--;
        gain_runic = true;
    }

    runeCost[RUNE_DEATH] = runeCost[RUNE_BLOOD] + runeCost[RUNE_UNHOLY] + runeCost[RUNE_FROST];
    if (!runeCost[RUNE_DEATH])
        runeCost[RUNE_DEATH] = runeCostData->RuneCost[RUNE_DEATH];

    if (runeCost[RUNE_DEATH] > 0)
    {
        for (uint32 i = 0; i < MAX_RUNES; ++i)
        {
            RuneType rune = player->GetCurrentRune(i);
            if (!player->HasRuneCooldown(i) && rune == RUNE_DEATH)
            {
                player->ConsumeRune(i);
                player->SetLastUsedRuneIndex(i);    // As well as normal

                //player->SetLastUsedRuneIndex(i); If GetLastUsedRuneMask will be used somethere else expect spell_dk_rune_converter - change this.
                runeCost[rune]--;

                gain_runic = true;

                // keep Death Rune type if missed or player has Blood of the North or rune is permanently converted
                if (!player->IsRunePermanentlyConverted(i))
                    player->RestoreBaseRune(i);
                
                if (runeCost[RUNE_DEATH] == 0)
                    break;
            }
        }
    }

    // you can gain some runic power when use runes
    if (gain_runic)
    {
        int32 rp = int32(runeCostData->runePowerGain * sWorld->getRate(RATE_POWER_RUNICPOWER_INCOME));

        if (m_spellInfo->Id == 48721) // FIXME Blood Boil
            rp = 100;

        if (rp)
        {
            AddPct(rp, player->GetTotalAuraModifier(SPELL_AURA_MOD_RUNE_REGEN_SPEED));
            player->ModifyPower(POWER_RUNIC_POWER, int32(rp));
        }
    }
}

void Spell::TakeReagents()
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    ItemTemplate const* castItemTemplate = m_CastItem ? m_CastItem->GetTemplate() : NULL;

    // do not take reagents for these item casts
    if (castItemTemplate && castItemTemplate->Flags & ITEM_PROTO_FLAG_TRIGGERED_CAST)
        return;

    Player* p_caster = m_caster->ToPlayer();
    if (p_caster->CanNoReagentCast(m_spellInfo))
        return;

    for (uint32 x = 0; x < MAX_SPELL_REAGENTS; ++x)
    {
        if (m_spellInfo->Reagent[x] <= 0)
            continue;

        uint32 itemid = m_spellInfo->Reagent[x];
        uint32 itemcount = m_spellInfo->ReagentCount[x];

        // if CastItem is also spell reagent
        if (castItemTemplate && castItemTemplate->ItemId == itemid)
        {
            for (int s = 0; s < MAX_ITEM_PROTO_SPELLS; ++s)
            {
                // CastItem will be used up and does not count as reagent
                int32 charges = m_CastItem->GetSpellCharges(s);
                if (castItemTemplate->Spells[s].SpellCharges < 0 && abs(charges) < 2)
                {
                    ++itemcount;
                    break;
                }
            }

            m_CastItem = NULL;
        }

        // if GetItemTarget is also spell reagent
        if (m_targets.GetItemTargetEntry() == itemid)
            m_targets.SetItemTarget(NULL);

        p_caster->DestroyItemCount(itemid, itemcount, true);
    }
}

void Spell::HandleThreatSpells()
{
    if (m_UniqueTargetInfo.empty())
        return;

    if ((m_spellInfo->AttributesEx  & SPELL_ATTR1_NO_THREAT) ||
        (m_spellInfo->AttributesEx3 & SPELL_ATTR3_NO_INITIAL_AGGRO))
        return;

    float threat = 0.0f;
    if (SpellThreatEntry const* threatEntry = sSpellMgr->GetSpellThreatEntry(m_spellInfo->Id))
    {
        if (threatEntry->apPctMod != 0.0f)
            threat += threatEntry->apPctMod * m_caster->GetTotalAttackPowerValue(BASE_ATTACK);

        threat += threatEntry->flatMod;
    }
    else if ((m_spellInfo->AttributesCu & SPELL_ATTR0_CU_NO_INITIAL_THREAT) == 0)
        threat += m_spellInfo->SpellLevel;

    // past this point only multiplicative effects occur
    if (threat == 0.0f)
        return;

    // since 2.0.1 threat from positive effects also is distributed among all targets, so the overall caused threat is at most the defined bonus
    threat /= m_UniqueTargetInfo.size();

    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->missCondition != SPELL_MISS_NONE)
            continue;

        Unit* target = ObjectAccessor::GetUnit(*m_caster, ihit->targetGUID);
        if (!target)
            continue;

        // positive spells distribute threat among all units that are in combat with target, like healing
        if (m_spellInfo->_IsPositiveSpell())
            target->getHostileRefManager().threatAssist(m_caster, threat, m_spellInfo);
        // for negative spells threat gets distributed among affected targets
        else
        {
            if (!target->CanHaveThreatList())
                continue;

            target->AddThreat(m_caster, threat, m_spellInfo->GetSchoolMask(), m_spellInfo);
        }
    }
    TC_LOG_DEBUG("spells", "Spell %u, added an additional %f threat for %s %u target(s)", m_spellInfo->Id, threat, m_spellInfo->_IsPositiveSpell() ? "assisting" : "harming", uint32(m_UniqueTargetInfo.size()));
}

void Spell::HandleEffects(Unit* pUnitTarget, Item* pItemTarget, GameObject* pGOTarget, Corpse* corpse, uint32 i, SpellEffectHandleMode mode)
{
    effectHandleMode = mode;
    unitTarget = pUnitTarget;
    itemTarget = pItemTarget;
    gameObjTarget = pGOTarget;
    corpseTarget = corpse;
    destTarget = &m_destTargets[i]._position;

    uint8 eff = m_spellInfo->Effects[i].Effect;

    TC_LOG_DEBUG("spells", "Spell: %u Effect : %u", m_spellInfo->Id, eff);

    // FIXME: Prevent consuming Molten Core (122355) by Soul Fire (6353) that tigger it
    if (mode > SPELL_EFFECT_HANDLE_LAUNCH_TARGET && m_spellInfo->SpellFamilyName == SPELLFAMILY_WARLOCK && m_spellInfo->SpellFamilyFlags[1] & 0x80)
        return;

    damage = CalculateDamage(i, unitTarget);

    bool preventDefault = CallScriptEffectHandlers((SpellEffIndex)i, mode);

    if (unitTarget)
        if (auto targetInfo = GetTargetInfo(unitTarget->GetGUID()))
            if (!(targetInfo->effectMask & (1 << i)))
                preventDefault = true;

    if (!preventDefault && eff < TOTAL_SPELL_EFFECTS)
    {
        (this->*SpellEffects[eff])((SpellEffIndex)i);
    }
}

SpellCastResult Spell::CheckCast(bool strict)
{
    // check death state
    if (!m_caster->IsAlive() && !m_spellInfo->HasAttribute(SPELL_ATTR0_PASSIVE) && !m_spellInfo->HasAttribute(SPELL_ATTR0_CASTABLE_WHILE_DEAD) &&
        !(_triggeredCastFlags & TRIGGERED_IGNORE_CASTER_DEATH_STATE && !m_triggeredByAuraSpell))
        return (_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_DONT_REPORT;

    // check cooldowns to prevent cheating
    if (!(_triggeredCastFlags & TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD) || m_spellInfo->IsTriggeredHasSpellCooldown())
    {
        //can cast triggered (by aura only?) spells while have this flag
        if (!(_triggeredCastFlags & TRIGGERED_IGNORE_CASTER_AURASTATE))
            if (m_caster->HasAuraType(SPELL_AURA_ALLOW_ONLY_ABILITY))
                if (AuraEffect const* eff = m_caster->GetAuraEffectsByType(SPELL_AURA_ALLOW_ONLY_ABILITY).front()) // Only one?
                    if (!eff->IsAffectingSpell(m_spellInfo))
                        return SPELL_FAILED_SPELL_IN_PROGRESS;

        if (!m_spellInfo->IsPassive() && !m_caster->HasAuraTypeWithAffectMask(SPELL_AURA_ALLOW_CAST_WHILE_IN_COOLDOWN, m_spellInfo))
        {
            if (!m_caster->GetSpellHistory()->IsReady(GetSpellInfo(), m_CastItem ? m_CastItem->GetEntry() : 0, false))
            {
                if (m_triggeredByAuraSpell)
                    return SPELL_FAILED_DONT_REPORT;
                else
                    return SPELL_FAILED_NOT_READY;
            }
        }
        if (Creature const* creature = m_caster->ToCreature())
            if (strict && creature->HasSpellCooldown(m_spellInfo->Id))
                return SPELL_FAILED_NOT_READY;
    }

    if (m_spellInfo->AttributesEx7 & SPELL_ATTR7_IS_CHEAT_SPELL && !m_caster->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_ALLOW_CHEAT_SPELLS))
    {
        m_customError = SPELL_CUSTOM_ERROR_GM_ONLY;
        return SPELL_FAILED_CUSTOM_ERROR;
    }

    // Check global cooldown
    if (strict && !(_triggeredCastFlags & TRIGGERED_IGNORE_GCD) && HasGlobalCooldown())
        return SPELL_FAILED_NOT_READY;

    // only triggered spells can be processed an ended battleground
    if (!IsTriggered() && m_caster->GetTypeId() == TYPEID_PLAYER)
        if (Battleground* bg = m_caster->ToPlayer()->GetBattleground())
            if (bg->GetStatus() == STATUS_WAIT_LEAVE)
                return SPELL_FAILED_DONT_REPORT;

    if (m_caster->GetTypeId() == TYPEID_PLAYER && VMAP::VMapFactory::createOrGetVMapManager()->isLineOfSightCalcEnabled())
    {
        if (m_spellInfo->Attributes & SPELL_ATTR0_OUTDOORS_ONLY &&
                !m_caster->GetMap()->IsOutdoors(m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ()))
            return SPELL_FAILED_ONLY_OUTDOORS;

        if (m_spellInfo->Attributes & SPELL_ATTR0_INDOORS_ONLY &&
                m_caster->GetMap()->IsOutdoors(m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ()))
            return SPELL_FAILED_ONLY_INDOORS;
    }

    // only check at first call, Stealth auras are already removed at second call
    // for now, ignore triggered spells
    if (strict && !(_triggeredCastFlags & TRIGGERED_IGNORE_SHAPESHIFT) && ! m_darkSimulacrum)
    {
        bool checkForm = true;
        // Ignore form req aura
        Unit::AuraEffectList const& ignore = m_caster->GetAuraEffectsByType(SPELL_AURA_MOD_IGNORE_SHAPESHIFT);
        for (Unit::AuraEffectList::const_iterator i = ignore.begin(); i != ignore.end(); ++i)
        {
            if (!(*i)->IsAffectingSpell(m_spellInfo))
                continue;
            checkForm = false;
            break;
        }
        if (checkForm)
        {
            // Cannot be used in this stance/form
            SpellCastResult shapeError = m_spellInfo->CheckShapeshift(m_caster->GetShapeshiftForm());
            if (shapeError != SPELL_CAST_OK)
                return shapeError;

            if ((m_spellInfo->Attributes & SPELL_ATTR0_ONLY_STEALTHED) && !(m_caster->HasStealthAura()))
                return SPELL_FAILED_ONLY_STEALTHED;
        }
    }

    Unit::AuraEffectList const& blockSpells = m_caster->GetAuraEffectsByType(SPELL_AURA_BLOCK_SPELL_FAMILY);
    for (Unit::AuraEffectList::const_iterator blockItr = blockSpells.begin(); blockItr != blockSpells.end(); ++blockItr)
        if (uint32((*blockItr)->GetMiscValue()) == m_spellInfo->SpellFamilyName)
            return SPELL_FAILED_SPELL_UNAVAILABLE;

    bool reqCombat = true;
    Unit::AuraEffectList const& stateAuras = m_caster->GetAuraEffectsByType(SPELL_AURA_ABILITY_IGNORE_AURASTATE);
    for (Unit::AuraEffectList::const_iterator j = stateAuras.begin(); j != stateAuras.end(); ++j)
    {
        if ((*j)->IsAffectingSpell(m_spellInfo))
        {
            m_needComboPoints = false;
            if ((*j)->GetMiscValue() == 1)
            {
                reqCombat=false;
                break;
            }
        }
    }

    // caster state requirements
    // not for triggered spells (needed by execute)
    if (!(_triggeredCastFlags & TRIGGERED_IGNORE_CASTER_AURASTATE))
    {
        if (m_spellInfo->CasterAuraState && !m_caster->HasAuraState(AuraStateType(m_spellInfo->CasterAuraState), m_spellInfo, m_caster))
            return SPELL_FAILED_CASTER_AURASTATE;
        if (m_spellInfo->CasterAuraStateNot && m_caster->HasAuraState(AuraStateType(m_spellInfo->CasterAuraStateNot), m_spellInfo, m_caster))
            return SPELL_FAILED_CASTER_AURASTATE;

        // Note: spell 62473 requres casterAuraSpell = triggering spell
        if (m_spellInfo->CasterAuraSpell && !m_caster->HasAura(sSpellMgr->GetSpellIdForDifficulty(m_spellInfo->CasterAuraSpell, m_caster)))
            return SPELL_FAILED_CASTER_AURASTATE;
        if (m_spellInfo->ExcludeCasterAuraSpell && m_caster->HasAura(sSpellMgr->GetSpellIdForDifficulty(m_spellInfo->ExcludeCasterAuraSpell, m_caster)))
            return SPELL_FAILED_CASTER_AURASTATE;

        if (reqCombat && m_caster->IsInCombat() && !m_spellInfo->CanBeUsedInCombat())
            return SPELL_FAILED_AFFECTING_COMBAT;
    }

    // cancel autorepeat spells if cast start when moving
    // (not wand currently autorepeat cast delayed to moving stop anyway in spell update code)
    // Do not cancel spells which are affected by a SPELL_AURA_CAST_WHILE_WALKING effect
    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_caster->ToPlayer()->isMoving() && !m_caster->HasAuraTypeWithAffectMask(SPELL_AURA_CAST_WHILE_WALKING, m_spellInfo)
        && (!m_caster->IsCharmed() || !m_caster->GetCharmerGUID().IsCreatureOrVehicle()))
    {
        // skip stuck spell to allow use it in falling case and apply spell limitations at movement
        if ((!m_caster->HasUnitMovementFlag(MOVEMENTFLAG_FALLING_FAR) || m_spellInfo->Effects[0].Effect != SPELL_EFFECT_STUCK) &&
            (IsAutoRepeat() || m_spellInfo->HasAuraInterruptFlag(AURA_INTERRUPT_FLAG_NOT_SEATED)))
            return SPELL_FAILED_MOVING;
    }

    // Check vehicle flags
    if (!(_triggeredCastFlags & TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE))
    {
        SpellCastResult vehicleCheck = m_spellInfo->CheckVehicle(m_caster);
        if (vehicleCheck != SPELL_CAST_OK)
            return vehicleCheck;
    }

    // check spell cast conditions from database
    {
        ConditionSourceInfo condInfo = ConditionSourceInfo(m_caster, m_targets.GetObjectTarget());
        if (!sConditionMgr->IsObjectMeetingNotGroupedConditions(CONDITION_SOURCE_TYPE_SPELL, m_spellInfo->Id, condInfo))
        {
            // mLastFailedCondition can be NULL if there was an error processing the condition in Condition::Meets (i.e. wrong data for ConditionTarget or others)
            if (condInfo.mLastFailedCondition && condInfo.mLastFailedCondition->ErrorType)
            {
                if (condInfo.mLastFailedCondition->ErrorType == SPELL_FAILED_CUSTOM_ERROR)
                    m_customError = SpellCustomErrors(condInfo.mLastFailedCondition->ErrorTextId);
                return SpellCastResult(condInfo.mLastFailedCondition->ErrorType);
            }
            if (!condInfo.mLastFailedCondition || !condInfo.mLastFailedCondition->ConditionTarget)
                return SPELL_FAILED_CASTER_AURASTATE;
            return SPELL_FAILED_BAD_TARGETS;
        }
    }

    // Don't check explicit target for passive spells (workaround) (check should be skipped only for learn case)
    // those spells may have incorrect target entries or not filled at all (for example 15332)
    // such spells when learned are not targeting anyone using targeting system, they should apply directly to caster instead
    // also, such casts shouldn't be sent to client
    if (!((m_spellInfo->Attributes & SPELL_ATTR0_PASSIVE) && (!m_targets.GetUnitTarget() || m_targets.GetUnitTarget() == m_caster)))
    {
        /* HERE */
        // Check explicit target for m_originalCaster - todo: get rid of such workarounds
        SpellCastResult castResult = m_spellInfo->CheckExplicitTarget(m_originalCaster ? m_originalCaster : m_caster, m_targets.GetObjectTarget(), m_targets.GetItemTarget());
        if (castResult != SPELL_CAST_OK)
        {
            TC_LOG_DEBUG("", "");
            return castResult;
        }
    }

    if (Unit* target = m_targets.GetUnitTarget())
    {
        SpellCastResult castResult = m_spellInfo->CheckTarget(m_caster, target, IsTriggered());
        if (castResult != SPELL_CAST_OK)
            return castResult;

        if (target != m_caster)
        {
            // Must be behind the target
            if ((m_spellInfo->AttributesCu & SPELL_ATTR0_CU_REQ_CASTER_BEHIND_TARGET) && target->HasInArc(static_cast<float>(M_PI), m_caster)
                && !(target->GetEntry() == 56471 || target->GetEntry() == 56168 || target->GetEntry() == 56846 || target->GetEntry() == 56167)) // Hack for Madness of Deathwing limbs and tentacles
                return SPELL_FAILED_NOT_BEHIND;

            // Target must be facing you
            if ((m_spellInfo->AttributesCu & SPELL_ATTR0_CU_REQ_TARGET_FACING_CASTER) && !target->HasInArc(static_cast<float>(M_PI), m_caster))
                return SPELL_FAILED_NOT_INFRONT;

            if (m_caster->GetEntry() != WORLD_TRIGGER) // Ignore LOS for gameobjects casts (wrongly casted by a trigger)
                if (!(m_spellInfo->AttributesEx2 & SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS) && !DisableMgr::IsDisabledFor(DISABLE_TYPE_SPELL, m_spellInfo->Id, NULL, SPELL_DISABLE_LOS) && !m_caster->IsWithinLOSInMap(target))
                    return IsTriggered() ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_LINE_OF_SIGHT;

            if (!IsTriggered() || GetCurrentContainer() == CURRENT_AUTOREPEAT_SPELL ||
                m_caster->GetAutoAttackSpell(m_spellInfo->HasAttribute(SPELL_ATTR3_REQ_OFFHAND) ? OFF_ATTACK : BASE_ATTACK) == m_spellInfo->Id)
            {
                SpellRangeFlag rangeType = SPELL_RANGE_DEFAULT;
                if (m_spellInfo->RangeEntry)
                    rangeType = SpellRangeFlag(m_spellInfo->RangeEntry->type);
                if (rangeType != SPELL_RANGE_MELEE || !target->HasAuraType(SPELL_AURA_MOD_CAMOUFLAGE))
                    if (m_caster->HasVisionObscured(target))
                        return SPELL_FAILED_VISION_OBSCURED;
            }
        }
    }

    // Check for line of sight for spells with dest
    if (m_targets.HasDst())
    {
        float x, y, z;
        m_targets.GetDstPos()->GetPosition(x, y, z);

        if (!(m_spellInfo->AttributesEx2 & SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS) && !DisableMgr::IsDisabledFor(DISABLE_TYPE_SPELL, m_spellInfo->Id, NULL, SPELL_DISABLE_LOS) && !m_caster->IsWithinLOS(x, y, z))
            return IsTriggered() ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_LINE_OF_SIGHT;
    }

    // check pet presence
    for (int j = 0; j < MAX_SPELL_EFFECTS; ++j)
    {
        if (m_spellInfo->Effects[j].TargetA.GetTarget() == TARGET_UNIT_PET)
        {
            if (!m_caster->GetGuardianPet())
            {
                if (m_triggeredByAuraSpell)              // not report pet not existence for triggered spells
                    return SPELL_FAILED_DONT_REPORT;
                else
                    return SPELL_FAILED_NO_PET;
            }
            break;
        }
    }

    // Spell casted only on battleground
    if ((m_spellInfo->AttributesEx3 & SPELL_ATTR3_BATTLEGROUND) &&  m_caster->GetTypeId() == TYPEID_PLAYER)
        if (!m_caster->ToPlayer()->InBattleground())
            return SPELL_FAILED_ONLY_BATTLEGROUNDS;

    // do not allow spells to be cast in arenas or rated battlegrounds
    if (Battleground const* bg = m_caster->GetBattlegorund())
    {
        SpellCastResult castResult = m_spellInfo->CheckArenaAndBattlegroundCastRules(bg);
        if (castResult != SPELL_CAST_OK)
            return IsTriggered() ? SPELL_FAILED_DONT_REPORT : castResult;
    }

    if (m_spellInfo->HasAttribute(SPELL_ATTR11_NOT_USABLE_IN_CHALLENGES) && m_caster->GetMap()->IsChallengeDungeon())
    {
        if (IsTriggered())
            return SPELL_FAILED_DONT_REPORT;

        m_customError = SPELL_CUSTOM_ERROR_NOT_DURING_CHALLENGE_MODE;
        return SPELL_FAILED_CUSTOM_ERROR;
    }

    // zone check
    if (m_caster->GetTypeId() == TYPEID_UNIT || !m_caster->ToPlayer()->IsGameMaster())
    {
        uint32 zone, area;
        m_caster->GetZoneAndAreaId(zone, area);

        SpellCastResult locRes= m_spellInfo->CheckLocation(m_caster->GetMapId(), zone, area,
            m_caster->GetTypeId() == TYPEID_PLAYER ? m_caster->ToPlayer() : NULL);
        if (locRes != SPELL_CAST_OK)
            return locRes;
    }

    // not let players cast spells at mount (and let do it to creatures)
    if (m_caster->IsMounted() && m_caster->GetTypeId() == TYPEID_PLAYER && !(_triggeredCastFlags & TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE) &&
        !m_spellInfo->IsPassive() && !(m_spellInfo->Attributes & SPELL_ATTR0_CASTABLE_WHILE_MOUNTED))
    {
        if (m_caster->IsInFlight())
            return SPELL_FAILED_NOT_ON_TAXI;
        else
            return SPELL_FAILED_NOT_MOUNTED;
    }

    // check spell focus object
    if (m_spellInfo->RequiresSpellFocus)
    {
        focusObject = SearchSpellFocus();
        if (!focusObject)
        {
            bool allow = false;
            // Pierre
            // Apparently this little dipshit suppose to summon GO like every 12 seconds (Spells: 139929, 139930), or may be not, but anyway, just hack (we even don't have template for this GO)
            if (m_spellInfo->RequiresSpellFocus == 4 && m_caster->GetTypeId() == TYPEID_PLAYER)
                if (auto battlePet = m_caster->ToPlayer()->GetBattlePetMgr().GetCurrentSummon())
                    allow = battlePet->GetEntry() == 70082;

            if (!allow)
                return SPELL_FAILED_REQUIRES_SPELL_FOCUS;
        }
    }

    SpellCastResult castResult = SPELL_CAST_OK;

    // always (except passive spells) check items (only player related checks)
    if (!m_spellInfo->IsPassive())
    {
        castResult = CheckItems();
        if (castResult != SPELL_CAST_OK)
            return castResult;
    }

    // Triggered spells also have range check
    /// @todo determine if there is some flag to enable/disable the check
    castResult = CheckRange(strict);
    if (castResult != SPELL_CAST_OK)
        return castResult;

    if (!(_triggeredCastFlags & TRIGGERED_IGNORE_POWER_AND_REAGENT_COST))
    {
        castResult = CheckPower();
        if (castResult != SPELL_CAST_OK)
            return castResult;
    }

    if (!(_triggeredCastFlags & TRIGGERED_IGNORE_CASTER_AURAS))
    {
        castResult = CheckCasterAuras();
        if (castResult != SPELL_CAST_OK)
            return m_triggeredByAuraSpell ? SPELL_FAILED_DONT_REPORT : castResult;
    }

    // script hook
    castResult = CallScriptCheckCastHandlers();
    if (castResult != SPELL_CAST_OK)
        return castResult;

    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        // for effects of spells that have only one target
        switch (m_spellInfo->Effects[i].Effect)
        {
            case SPELL_EFFECT_DUMMY:
            {
                if (m_spellInfo->Id == 19938)          // Awaken Peon
                {
                    Unit* unit = m_targets.GetUnitTarget();
                    if (!unit || !unit->HasAura(17743))
                        return SPELL_FAILED_BAD_TARGETS;
                }
                else if (m_spellInfo->Id == 31789)          // Righteous Defense
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return SPELL_FAILED_DONT_REPORT;

                    Unit* target = m_targets.GetUnitTarget();
                    if (!target || !target->IsFriendlyTo(m_caster) || target->getAttackers().empty())
                        return SPELL_FAILED_BAD_TARGETS;

                }
                break;
            }
            case SPELL_EFFECT_LEARN_SPELL:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;

                if (m_spellInfo->Effects[i].TargetA.GetTarget() != TARGET_UNIT_PET)
                    break;

                Pet* pet = m_caster->ToPlayer()->GetPet();

                if (!pet)
                    return SPELL_FAILED_NO_PET;

                SpellInfo const* learn_spellproto = sSpellMgr->GetSpellInfo(m_spellInfo->Effects[i].TriggerSpell);

                if (!learn_spellproto)
                    return SPELL_FAILED_NOT_KNOWN;

                if (m_spellInfo->SpellLevel > pet->GetLevel())
                    return SPELL_FAILED_LOWLEVEL;

                break;
            }
            case SPELL_EFFECT_UNLOCK_GUILD_VAULT_TAB:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;
                if (Guild* guild = m_caster->ToPlayer()->GetGuild())
                    if (guild->GetLeaderGUID() != m_caster->ToPlayer()->GetGUID())
                        return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                break;
            }
            case SPELL_EFFECT_LEARN_PET_SPELL:
            {
                // check target only for unit target case
                if (Unit* unitTarget = m_targets.GetUnitTarget())
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return SPELL_FAILED_BAD_TARGETS;

                    Pet* pet = unitTarget->ToPet();
                    if (!pet || pet->GetOwner() != m_caster)
                        return SPELL_FAILED_BAD_TARGETS;

                    SpellInfo const* learn_spellproto = sSpellMgr->GetSpellInfo(m_spellInfo->Effects[i].TriggerSpell);

                    if (!learn_spellproto)
                        return SPELL_FAILED_NOT_KNOWN;

                    if (m_spellInfo->SpellLevel > pet->GetLevel())
                        return SPELL_FAILED_LOWLEVEL;
                }
                break;
            }
            case SPELL_EFFECT_APPLY_GLYPH:
            {
                uint32 glyphId = m_spellInfo->Effects[i].MiscValue;
                if (GlyphPropertiesEntry const* gp = sGlyphPropertiesStore.LookupEntry(glyphId))
                    if (m_caster->HasAura(gp->SpellId))
                        return SPELL_FAILED_UNIQUE_GLYPH;
                break;
            }
            case SPELL_EFFECT_FEED_PET:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;

                Item* foodItem = m_targets.GetItemTarget();
                if (!foodItem)
                    return SPELL_FAILED_BAD_TARGETS;

                Pet* pet = m_caster->ToPlayer()->GetPet();

                if (!pet)
                    return SPELL_FAILED_NO_PET;

                if (!pet->HaveInDiet(foodItem->GetTemplate()))
                    return SPELL_FAILED_WRONG_PET_FOOD;

                if (!pet->GetCurrentFoodBenefitLevel(foodItem->GetTemplate()->ItemLevel))
                    return SPELL_FAILED_FOOD_LOWLEVEL;

                if (m_caster->IsInCombat() || pet->IsInCombat())
                    return SPELL_FAILED_AFFECTING_COMBAT;

                break;
            }
            case SPELL_EFFECT_POWER_BURN:
            case SPELL_EFFECT_POWER_DRAIN:
            {
                // Can be area effect, Check only for players and not check if target - caster (spell can have multiply drain/burn effects)
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    if (Unit* target = m_targets.GetUnitTarget())
                        if (target != m_caster && target->GetPowerType() != Powers(m_spellInfo->Effects[i].MiscValue))
                            return SPELL_FAILED_BAD_TARGETS;
                break;
            }
            case SPELL_EFFECT_CHARGE:
            {
                if (m_caster->HasUnitState(UNIT_STATE_ROOT) && GetSpellInfo()->Id != 114029 && GetSpellInfo()->Id != 54216) // Safeguard, Master's Call
                    return SPELL_FAILED_ROOTED;

                if (GetSpellInfo()->NeedsExplicitUnitTarget())
                {
                    if (m_caster->GetTypeId() == TYPEID_PLAYER || m_caster->IsPet())
                    {
                        if (Unit* target = m_targets.GetUnitTarget())
                        {
                            if (!target->IsAlive())
                                return SPELL_FAILED_BAD_TARGETS;

                            float objSize = target->GetObjectSize();
                            float range = m_spellInfo->GetMaxRange(true, m_caster, this) * 1.5f + objSize; // can't be overly strict

                            m_preGeneratedPath.reset(new PathGenerator(m_caster));
                            m_preGeneratedPath->SetPathLengthLimit(range);
                            
                            // first try with raycast, if it fails fall back to normal path
                            bool result = m_preGeneratedPath->CalculatePath(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), false, true, true);
                            if (m_preGeneratedPath->GetPathType() & PATHFIND_SHORT)
                                return SPELL_FAILED_OUT_OF_RANGE;
                            else if (!result || m_preGeneratedPath->GetPathType() & (PATHFIND_NOPATH | PATHFIND_INCOMPLETE))
                            {
                                result = m_preGeneratedPath->CalculatePath(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), false, false, true);
                                if (m_preGeneratedPath->GetPathType() & PATHFIND_SHORT)
                                    return SPELL_FAILED_OUT_OF_RANGE;
                                else if (!result || m_preGeneratedPath->GetPathType() & (PATHFIND_NOPATH | PATHFIND_INCOMPLETE))
                                    return SPELL_FAILED_NOPATH;
                            }
                            else if (std::all_of(m_preGeneratedPath->GetPath().begin(), m_preGeneratedPath->GetPath().end(), [this](G3D::Vector3 const& point) { return point.z <= m_caster->GetPositionZ(); }))
                                m_preGeneratedPath->SetPathType(PATHFIND_BLANK); // Clear path if straight line succeeded - let caster boost through the air (but only if we don't have any point higher than caster, otherwise we risk falling below ground)

                            if (m_preGeneratedPath->GetPathType() != PATHFIND_BLANK)
                                m_preGeneratedPath->ShortenPathUntilDist(PositionToVector3(target), objSize); // move back
                        }
                    }
                }
                break;
            }
            case SPELL_EFFECT_SKINNING:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER || !m_targets.GetUnitTarget() || m_targets.GetUnitTarget()->GetTypeId() != TYPEID_UNIT)
                    return SPELL_FAILED_BAD_TARGETS;

                if (!(m_targets.GetUnitTarget()->GetUInt32Value(UNIT_FIELD_FLAGS) & UNIT_FLAG_SKINNABLE))
                    return SPELL_FAILED_TARGET_UNSKINNABLE;

                Creature* creature = m_targets.GetUnitTarget()->ToCreature();
                if (creature->GetCreatureType() != CREATURE_TYPE_CRITTER && !creature->loot.isLooted())
                    return SPELL_FAILED_TARGET_NOT_LOOTED;

                uint32 skill = creature->GetCreatureTemplate()->GetRequiredLootSkill();

                int32 skillValue = m_caster->ToPlayer()->GetSkillValue(skill);
                int32 TargetLevel = m_targets.GetUnitTarget()->GetLevel();
                int32 ReqValue = TargetLevel < 10 ? 0 :
                                 TargetLevel < 20 ? (TargetLevel - 10) * 10 :
                                 TargetLevel < 74 ? TargetLevel * 5 :
                                 TargetLevel < 80 ? (TargetLevel * 2 - 73) * 5 :
                                 TargetLevel < 85 ? TargetLevel * 5 + 35 :
                                 TargetLevel < 88 ? TargetLevel * 5 + 35 + (TargetLevel - 84) * 10 :
                                                    TargetLevel * 5 + 35 + (TargetLevel - 85) * 15;
                if (ReqValue > skillValue)
                    return SPELL_FAILED_LOW_CASTLEVEL;

                // chance for fail at orange skinning attempt
                if ((m_selfContainer && (*m_selfContainer) == this) &&
                    skillValue < sWorld->GetConfigMaxSkillValue() &&
                    (ReqValue < 0 ? 0 : ReqValue) > irand(skillValue - 25, skillValue + 37))
                    return SPELL_FAILED_TRY_AGAIN;

                break;
            }
            case SPELL_EFFECT_OPEN_LOCK:
            {
                if (m_spellInfo->Effects[i].TargetA.GetTarget() != TARGET_GAMEOBJECT_TARGET &&
                    m_spellInfo->Effects[i].TargetA.GetTarget() != TARGET_GAMEOBJECT_ITEM_TARGET)
                    break;

                if (m_caster->GetTypeId() != TYPEID_PLAYER  // only players can open locks, gather etc.
                    // we need a go target in case of TARGET_GAMEOBJECT_TARGET
                    || (m_spellInfo->Effects[i].TargetA.GetTarget() == TARGET_GAMEOBJECT_TARGET && !m_targets.GetGOTarget()))
                    return SPELL_FAILED_BAD_TARGETS;

                Item* pTempItem = NULL;
                if (m_targets.GetTargetMask() & TARGET_FLAG_TRADE_ITEM)
                {
                    if (TradeData* pTrade = m_caster->ToPlayer()->GetTradeData())
                        pTempItem = pTrade->GetTraderData()->GetItem(TradeSlots(m_targets.GetItemTargetGUID().GetRawValue()));
                }
                else if (m_targets.GetTargetMask() & TARGET_FLAG_ITEM)
                    pTempItem = m_caster->ToPlayer()->GetItemByGuid(m_targets.GetItemTargetGUID());

                // we need a go target, or an openable item target in case of TARGET_GAMEOBJECT_ITEM_TARGET
                if (m_spellInfo->Effects[i].TargetA.GetTarget() == TARGET_GAMEOBJECT_ITEM_TARGET &&
                    !m_targets.GetGOTarget() &&
                    (!pTempItem || !pTempItem->GetTemplate()->LockID || !pTempItem->IsLocked()))
                    return SPELL_FAILED_BAD_TARGETS;

                if (m_spellInfo->Id != 1842 || (m_targets.GetGOTarget() && m_targets.GetGOTarget()->GetGOInfo()->type != GAMEOBJECT_TYPE_TRAP))
                    if (m_caster->ToPlayer()->InBattleground() && // In Battleground players can use only flags and banners
                        (!m_targets.GetGOTarget() || m_targets.GetGOTarget()->GetEntry() != 192834) && // ...and, SUDDENLY, Titan Relics. Assumptions, assumptions...
                        !m_caster->ToPlayer()->CanUseBattlegroundObject(m_targets.GetGOTarget()))
                        return SPELL_FAILED_TRY_AGAIN;

                // get the lock entry
                uint32 lockId = 0;
                if (GameObject* go = m_targets.GetGOTarget())
                {
                    if (!go->IsAtInteractDistance(m_caster->ToPlayer(), m_spellInfo))
                        return SPELL_FAILED_OUT_OF_RANGE;

                    lockId = go->GetGOInfo()->GetLockId();
                    if (!lockId)
                        return SPELL_FAILED_BAD_TARGETS;
                }
                else if (Item* itm = m_targets.GetItemTarget())
                    lockId = itm->GetTemplate()->LockID;

                SkillType skillId = SKILL_NONE;
                int32 reqSkillValue = 0;
                int32 skillValue = 0;

                // check lock compatibility
                SpellCastResult res = CanOpenLock(i, lockId, skillId, reqSkillValue, skillValue);
                if (res != SPELL_CAST_OK)
                    return res;

                // chance for fail at LockPicking attempt
                // second check prevent fail at rechecks
                if (skillId != SKILL_NONE && (!m_selfContainer || ((*m_selfContainer) != this)))
                {
                    bool canFailAtMax = skillId == SKILL_LOCKPICKING;

                    // chance for failure in orange lockpick
                    if ((canFailAtMax && skillValue < sWorld->GetConfigMaxSkillValue()) && reqSkillValue > irand(skillValue - 25, skillValue + 37))
                        return SPELL_FAILED_TRY_AGAIN;
                }
                break;
            }
            case SPELL_EFFECT_RESURRECT_PET:
            {
                Creature* pet = m_caster->GetGuardianPet();

                if (pet && pet->IsAlive())
                    return SPELL_FAILED_ALREADY_HAVE_SUMMON;

                break;
            }
            // This is generic summon effect
            case SPELL_EFFECT_SUMMON:
            {
                SummonPropertiesEntry const* SummonProperties = sSummonPropertiesStore.LookupEntry(m_spellInfo->Effects[i].MiscValueB);
                if (!SummonProperties)
                    break;
                switch (SummonProperties->Category)
                {
                    case SUMMON_CATEGORY_PET:
                        if (m_caster->GetPetGUID())
                            return SPELL_FAILED_ALREADY_HAVE_SUMMON;
                    case SUMMON_CATEGORY_PUPPET:
                        if (m_caster->GetCharmGUID())
                            return SPELL_FAILED_ALREADY_HAVE_CHARM;
                        break;
                }
                break;
            }
            case SPELL_EFFECT_CREATE_TAMED_PET:
            {
                if (m_targets.GetUnitTarget())
                {
                    if (m_targets.GetUnitTarget()->GetTypeId() != TYPEID_PLAYER)
                        return SPELL_FAILED_BAD_TARGETS;
                    if (m_targets.GetUnitTarget()->GetPetGUID())
                        return SPELL_FAILED_ALREADY_HAVE_SUMMON;
                }
                break;
            }
            case SPELL_EFFECT_SUMMON_PET:
            {
                if (m_caster->GetPetGUID())                  //let warlock do a replacement summon
                {
                    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_caster->GetClass() == CLASS_WARLOCK)
                    {
                        if (strict)                         //starting cast, trigger pet stun (cast by pet so it doesn't attack player)
                            if (Pet* pet = m_caster->ToPlayer()->GetPet())
                                pet->CastSpell(pet, 32752, true, NULL, NULL, pet->GetGUID());
                    }
                    else
                        return SPELL_FAILED_ALREADY_HAVE_SUMMON;
                }

                if (m_caster->GetCharmGUID())
                    return SPELL_FAILED_ALREADY_HAVE_CHARM;
                break;
            }
            case SPELL_EFFECT_SUMMON_PLAYER:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;
                if (!m_caster->GetTarget())
                    return SPELL_FAILED_BAD_TARGETS;

                Player* target = ObjectAccessor::FindPlayer(m_caster->ToPlayer()->GetTarget());
                if (!target || m_caster->ToPlayer() == target || (!target->IsInSameRaidWith(m_caster->ToPlayer()) && m_spellInfo->Id != 48955)) // refer-a-friend spell
                    return SPELL_FAILED_BAD_TARGETS;

                // check if our map is dungeon
                MapEntry const* map = sMapStore.LookupEntry(m_caster->GetMapId());
                if (map->IsDungeon())
                {
                    uint32 mapId = m_caster->GetMap()->GetId();
                    Difficulty difficulty = m_caster->GetMap()->GetDifficulty();
                    if (map->IsRaid())
                        if (InstancePlayerBind* targetBind = target->GetBoundInstance(mapId, difficulty))
                            if (InstancePlayerBind* casterBind = m_caster->ToPlayer()->GetBoundInstance(mapId, difficulty))
                                if (targetBind->perm && targetBind->save != casterBind->save)
                                    return SPELL_FAILED_TARGET_LOCKED_TO_RAID_INSTANCE;

                    InstanceTemplate const* instance = sObjectMgr->GetInstanceTemplate(mapId);
                    if (!instance)
                        return SPELL_FAILED_TARGET_NOT_IN_INSTANCE;
                    if (!target->Satisfy(sObjectMgr->GetAccessRequirement(mapId, difficulty), mapId))
                        return SPELL_FAILED_BAD_TARGETS;
                }
                break;
            }
            // RETURN HERE
            case SPELL_EFFECT_SUMMON_RAF_FRIEND:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;

                Player* playerCaster = m_caster->ToPlayer();
                    //
                if (!(playerCaster->GetTarget()))
                    return SPELL_FAILED_BAD_TARGETS;

                Player* target = playerCaster->GetSelectedPlayer();

                if (!target ||
                    !(target->GetSession()->GetRecruiterId() == playerCaster->GetSession()->GetAccountId() || target->GetSession()->GetAccountId() == playerCaster->GetSession()->GetRecruiterId()))
                    return SPELL_FAILED_BAD_TARGETS;

                break;
            }
            case SPELL_EFFECT_LEAP:
            case SPELL_EFFECT_TELEPORT_UNITS_FACE_CASTER:
            {
              //Do not allow to cast it before BG starts.
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    if (Battleground const* bg = m_caster->ToPlayer()->GetBattleground())
                        if (bg->GetStatus() != STATUS_IN_PROGRESS)
                            return SPELL_FAILED_TRY_AGAIN;
                break;
            }
            case SPELL_EFFECT_STEAL_BENEFICIAL_BUFF:
            {
                if (m_targets.GetUnitTarget() == m_caster)
                    return SPELL_FAILED_BAD_TARGETS;
                break;
            }
            case SPELL_EFFECT_LEAP_BACK:
            {
                if (m_caster->HasUnitState(UNIT_STATE_ROOT))
                {
                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                        return SPELL_FAILED_ROOTED;
                    else
                        return SPELL_FAILED_DONT_REPORT;
                }
                break;
            }
            case SPELL_EFFECT_TALENT_SPEC_SELECT:
                // can't change during already started arena/battleground
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    if (Battleground const* bg = m_caster->ToPlayer()->GetBattleground())
                        if (bg->GetStatus() == STATUS_IN_PROGRESS)
                            return SPELL_FAILED_NOT_IN_BATTLEGROUND;
                break;
            case SPELL_EFFECT_RESURRECT:
            case SPELL_EFFECT_SELF_RESURRECT:
                if (m_spellInfo->AttributesEx8 & SPELL_ATTR8_BATTLE_RESURRECTION &&
                    m_spellInfo->Id != 72423) // Mass Resurrection (The Lich King ICC encounter)
                {
                    if (InstanceScript* instance = m_caster->GetInstanceScript())
                        if (!instance->CanUseResurrection())
                            return SPELL_FAILED_IN_COMBAT_RES_LIMIT_REACHED;
                }
                break;
            default:
                break;
        }
    }

    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        switch (m_spellInfo->Effects[i].ApplyAuraName)
        {
            case SPELL_AURA_MOD_POSSESS:
            case SPELL_AURA_MOD_CHARM:
            case SPELL_AURA_AOE_CHARM:
            {
                if (m_caster->GetCharmerGUID())
                    return SPELL_FAILED_CHARMED;

                if (m_spellInfo->Effects[i].ApplyAuraName == SPELL_AURA_MOD_CHARM
                    || m_spellInfo->Effects[i].ApplyAuraName == SPELL_AURA_MOD_POSSESS)
                {
                    if (m_caster->GetPetGUID())
                        return SPELL_FAILED_ALREADY_HAVE_SUMMON;

                    if (m_caster->GetCharmGUID())
                        return SPELL_FAILED_ALREADY_HAVE_CHARM;
                }

                if (Unit* target = m_targets.GetUnitTarget())
                {
                    if (target->GetTypeId() == TYPEID_UNIT && target->ToCreature()->IsVehicle())
                        return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                    if (target->IsMounted())
                        return SPELL_FAILED_CANT_BE_CHARMED;

                    if (target->GetCharmerGUID())
                        return SPELL_FAILED_CHARMED;

                    if (target->GetOwner() && target->GetOwner()->GetTypeId() == TYPEID_PLAYER)
                        return SPELL_FAILED_TARGET_IS_PLAYER_CONTROLLED;

                    int32 damage = CalculateDamage(i, target);
                    if (damage && int32(target->GetLevel()) > damage)
                        return SPELL_FAILED_HIGHLEVEL;
                }

                break;
            }
            case SPELL_AURA_MOUNTED:
            {
                // Ignore map check if spell have AreaId. AreaId already checked and this prevent special mount spells
                bool allowMount = !m_caster->GetMap()->IsDungeon() || m_caster->GetMap()->IsBattlegroundOrArena();
                InstanceTemplate const* it = sObjectMgr->GetInstanceTemplate(m_caster->GetMapId());
                if (it)
                    allowMount = it->AllowMount;
                if (m_caster->GetTypeId() == TYPEID_PLAYER && !allowMount && !m_spellInfo->AreaGroupId)
                    return SPELL_FAILED_NO_MOUNTS_ALLOWED;

                if (m_caster->IsInDisallowedMountForm())
                    return SPELL_FAILED_NOT_SHAPESHIFT;

                break;
            }
            case SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS:
            {
                if (!m_targets.GetUnitTarget())
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                // can be casted at non-friendly unit or own pet/charm
                if (m_caster->IsFriendlyTo(m_targets.GetUnitTarget()))
                    return SPELL_FAILED_TARGET_FRIENDLY;

                break;
            }
            case SPELL_AURA_FLY:
            case SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED:
            {
                // not allow cast fly spells if not have req. skills  (all spells is self target)
                // allow always ghost flight spells
                if (m_originalCaster && m_originalCaster->GetTypeId() == TYPEID_PLAYER && m_originalCaster->IsAlive())
                {
                    Battlefield* Bf = sBattlefieldMgr->GetBattlefieldToZoneId(m_originalCaster->GetZoneId());
                    if (AreaTableEntry const* area = sAreaTableStore.LookupEntry(m_originalCaster->GetAreaId()))
                        if (area->Flags & AREA_FLAG_NO_FLY_ZONE  || (Bf && !Bf->CanFlyIn()))
                            return (_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_NOT_HERE;
                }
                break;
            }
            case SPELL_AURA_PERIODIC_MANA_LEECH:
            {
                if (m_spellInfo->Effects[i].IsTargetingArea())
                    break;

                if (!m_targets.GetUnitTarget())
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                if (m_caster->GetTypeId() != TYPEID_PLAYER || m_CastItem)
                    break;

                if (m_targets.GetUnitTarget()->GetPowerType() != POWER_MANA)
                    return SPELL_FAILED_BAD_TARGETS;

                break;
            }
            case SPELL_AURA_MOD_STEALTH:
            case SPELL_AURA_MOD_INVISIBILITY:
                if (m_caster->HasAura(34709) || m_caster->HasAura(52415))
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                break;
            default:
                break;
        }
    }

    // check trade slot case (last, for allow catch any another cast problems)
    if (m_targets.GetTargetMask() & TARGET_FLAG_TRADE_ITEM)
    {
        if (m_CastItem)
            return SPELL_FAILED_ITEM_ENCHANT_TRADE_WINDOW;

        if (m_caster->GetTypeId() != TYPEID_PLAYER)
            return SPELL_FAILED_NOT_TRADING;

        TradeData* my_trade = m_caster->ToPlayer()->GetTradeData();

        if (!my_trade)
            return SPELL_FAILED_NOT_TRADING;

        TradeSlots slot = TradeSlots(m_targets.GetItemTargetGUID().GetRawValue());
        if (slot != TRADE_SLOT_NONTRADED)
            return SPELL_FAILED_BAD_TARGETS;

        if (!IsTriggered())
            if (my_trade->GetSpell())
                return SPELL_FAILED_ITEM_ALREADY_ENCHANTED;
    }

    // check if caster has at least 1 combo point for spells that require combo points
    if (m_needComboPoints)
        if (Player* plrCaster = m_caster->ToPlayer())
            if (!plrCaster->GetComboPoints())
                return SPELL_FAILED_NO_COMBO_POINTS;

    // all ok
    return SPELL_CAST_OK;
}

SpellCastResult Spell::CheckPetCast(Unit* target)
{
    if (m_caster->HasUnitState(UNIT_STATE_CASTING) && !(_triggeredCastFlags & TRIGGERED_IGNORE_CAST_IN_PROGRESS))              //prevent spellcast interruption by another spellcast
        return SPELL_FAILED_SPELL_IN_PROGRESS;

    // dead owner (pets still alive when owners ressed?)
    if (Unit* owner = m_caster->GetCharmerOrOwner())
        if (!owner->IsAlive())
            return SPELL_FAILED_CASTER_DEAD;

    if (!target && m_targets.GetUnitTarget())
        target = m_targets.GetUnitTarget();

    if (m_spellInfo->NeedsExplicitUnitTarget() || (m_spellInfo->GetExplicitTargetMask() & TARGET_FLAG_CORPSE_MASK))
    {
        if (!target)
            return SPELL_FAILED_BAD_IMPLICIT_TARGETS;
        m_targets.SetUnitTarget(target);
    }

    // check cooldown
    if (Creature const* creatureCaster = m_caster->ToCreature())
        if (creatureCaster->HasSpellCooldown(m_spellInfo->Id))
            return SPELL_FAILED_NOT_READY;

    // Check if spell is affected by GCD
    if (m_spellInfo->StartRecoveryCategory > 0)
        if (m_caster->GetCharmInfo() && m_caster->GetCharmInfo()->GetGlobalCooldownMgr().HasGlobalCooldown(m_spellInfo))
            return SPELL_FAILED_NOT_READY;

    // Basically this is wrong, but I won't change all this shit in the near future.
    m_powerType = m_spellInfo->GetPowerType(m_caster, &m_powerEntryIndex);
    if (!m_CastItem)
        m_powerCost = m_spellInfo->CalcPowerCost(m_caster, m_spellSchoolMask, m_powerEntryIndex);
    LoadScripts();
    return CheckCast(true);
}

SpellCastResult Spell::CheckCasterAuras() const
{
    // FIXME
    if (m_spellInfo->HasAttribute(SPELL_ATTR6_IGNORE_CASTER_AURAS))
        if (m_spellInfo->AttributesEx5 & (SPELL_ATTR5_USABLE_WHILE_STUNNED | SPELL_ATTR5_USABLE_WHILE_FEARED | SPELL_ATTR5_USABLE_WHILE_CONFUSED))
            return SPELL_CAST_OK;

    // Triggered spells don't get this check anyway
    // And this is from LK. Probably outdated, need to investigate
    // Currently only for spells casted by player, not from code (event if not triggered)
    if (m_cast_count && m_caster->IsCharmed() && !m_caster->IsVehicle() && m_caster->GetTypeId() == TYPEID_PLAYER)
        if (!CanBreakAura(SPELL_AURA_MOD_CHARM) && !CanBreakAura(SPELL_AURA_AOE_CHARM) && !CanBreakAura(SPELL_AURA_MOD_POSSESS))
            return SPELL_FAILED_PREVENTED_BY_MECHANIC;

    uint8 school_immune = 0;
    uint32 mechanic_immune = 0;
    uint32 dispel_immune = 0;

    // Check if the spell grants school or mechanic immunity.
    // We use bitmasks so the loop is done only once and not on every aura check below.
    if (m_spellInfo->AttributesEx & SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY)
    {
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (m_spellInfo->Effects[i].ApplyAuraName == SPELL_AURA_SCHOOL_IMMUNITY)
                school_immune |= uint32(m_spellInfo->Effects[i].MiscValue);
            else if (m_spellInfo->Effects[i].ApplyAuraName == SPELL_AURA_MECHANIC_IMMUNITY)
                mechanic_immune |= 1 << uint32(m_spellInfo->Effects[i].MiscValue);
            else if (m_spellInfo->Effects[i].ApplyAuraName == SPELL_AURA_DISPEL_IMMUNITY)
                dispel_immune |= SpellInfo::GetDispelMask(DispelType(m_spellInfo->Effects[i].MiscValue));
        }
        // immune movement impairment and loss of control
        if (m_spellInfo->Id == 42292 || m_spellInfo->Id == 59752 || // PvP Trinket, Every Man for Himself
            m_spellInfo->Id == 19574 || m_spellInfo->Id == 53490) // Bestial Wrath, Bullheaded
            mechanic_immune = IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK;
    }

    if (m_spellInfo->Id == 108482) // Unbound Will
        mechanic_immune = IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK;

    bool usableInStun = m_spellInfo->AttributesEx5 & SPELL_ATTR5_USABLE_WHILE_STUNNED ||
        m_spellInfo->AttributesEx11 & SPELL_ATTR11_USABLE_WHILE_STUNNED;

    // Check whether the cast should be prevented by any state you might have.
    SpellCastResult prevented_reason = SPELL_CAST_OK;
    uint32 errorData = 0;

    uint32 unitflag = m_caster->GetUInt32Value(UNIT_FIELD_FLAGS);     // Get unit state
    // First check prevention type...
    if (unitflag & UNIT_FLAG_SILENCED && m_spellInfo->PreventionType & SPELL_PREVENTION_TYPE_SILENCE)
        prevented_reason = SPELL_FAILED_SILENCED;
    else if (unitflag & UNIT_FLAG_PACIFIED && m_spellInfo->PreventionType & SPELL_PREVENTION_TYPE_PACIFY)
        prevented_reason = SPELL_FAILED_PACIFIED;
    else if ((m_spellInfo->PreventionType & SPELL_PREVENTION_TYPE_POLYMORPH || m_CastItem) && m_caster->HasAuraType(SPELL_AURA_TRANSFORM) && m_caster->HasAuraWithMechanic(1 << MECHANIC_POLYMORPH))
        prevented_reason = SPELL_FAILED_PREVENTED_BY_MECHANIC;

    // ...then crowd control
    // Have to check if there is a stun aura. Otherwise will have problems with ghost aura apply while logging out
    if (prevented_reason == SPELL_CAST_OK && unitflag & UNIT_FLAG_STUNNED)
    {
        // spell is usable while stunned, check if caster has only mechanic stun auras, another stun types must prevent cast spell
        if (usableInStun || GetSpellInfo()->Id == 8143)
        {
            bool canCast = true;
            Unit::AuraEffectList const& stunAuras = m_caster->GetAuraEffectsByType(SPELL_AURA_MOD_STUN);
            int32 mechanic = 0;
            if (usableInStun)
                mechanic |= (1 << MECHANIC_STUN) | (1 << MECHANIC_FREEZE);
            if (GetSpellInfo()->Id == 8143)
                mechanic |= 1 << MECHANIC_SLEEP;

            for (Unit::AuraEffectList::const_iterator i = stunAuras.begin(); i != stunAuras.end(); ++i)
            {
                if ((*i)->GetSpellInfo()->GetAllEffectsMechanicMask() && !((*i)->GetSpellInfo()->GetAllEffectsMechanicMask() & mechanic))
                {
                    canCast = false;
                    break;
                }
            }
            if (!canCast && m_spellInfo->Id != 22812 && m_spellInfo->Id != 11958 && m_spellInfo->Id != 108201) // Barkskin, Cold Snap, Desecrated Ground. Why we event need this?
                prevented_reason = SPELL_FAILED_STUNNED;
        }
        else
            prevented_reason = SPELL_FAILED_STUNNED;
    }

    if (prevented_reason == SPELL_CAST_OK && unitflag & UNIT_FLAG_CONFUSED && !(m_spellInfo->AttributesEx5 & SPELL_ATTR5_USABLE_WHILE_CONFUSED))
        prevented_reason = SPELL_FAILED_CONFUSED;
    if (prevented_reason == SPELL_CAST_OK && unitflag & UNIT_FLAG_FLEEING && !(m_spellInfo->AttributesEx5 & SPELL_ATTR5_USABLE_WHILE_FEARED))
        prevented_reason = SPELL_FAILED_FLEEING;

    // Attr must make flag drop spell totally immune from all effects
    if (prevented_reason != SPELL_CAST_OK)
    {
        if (school_immune || mechanic_immune || dispel_immune)
        {
            //Checking auras is needed now, because you are prevented by some state but the spell grants immunity.
            Unit::AuraApplicationMap const& auras = m_caster->GetAppliedAuras();
            for (Unit::AuraApplicationMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
            {
                Aura const* aura = itr->second->GetBase();
                SpellInfo const* auraInfo = aura->GetSpellInfo();
                if (auraInfo->GetAllEffectsMechanicMask() & mechanic_immune)
                    continue;
                if (auraInfo->GetSchoolMask() & school_immune && !(auraInfo->AttributesEx & SPELL_ATTR1_UNAFFECTED_BY_SCHOOL_IMMUNE))
                    continue;
                if (auraInfo->GetDispelMask() & dispel_immune)
                    continue;

                //Make a second check for spell failed so the right SPELL_FAILED message is returned.
                //That is needed when your casting is prevented by multiple states and you are only immune to some of them.
                for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                {
                    if (AuraEffect* part = aura->GetEffect(i))
                    {
                        switch (part->GetAuraType())
                        {
                            case SPELL_AURA_MOD_STUN:
                                if (!usableInStun || !(auraInfo->GetAllEffectsMechanicMask() & (1<<MECHANIC_STUN)))
                                    return SPELL_FAILED_STUNNED;
                                break;
                            case SPELL_AURA_MOD_CONFUSE:
                                if (!(m_spellInfo->AttributesEx5 & SPELL_ATTR5_USABLE_WHILE_CONFUSED))
                                    return SPELL_FAILED_CONFUSED;
                                break;
                            case SPELL_AURA_MOD_FEAR:
                            case SPELL_AURA_MOD_FEAR_2:
                                if (!(m_spellInfo->AttributesEx5 & SPELL_ATTR5_USABLE_WHILE_FEARED))
                                    return SPELL_FAILED_FLEEING;
                                break;
                            case SPELL_AURA_MOD_SILENCE:
                            case SPELL_AURA_MOD_PACIFY:
                            case SPELL_AURA_MOD_PACIFY_SILENCE:
                                if (m_spellInfo->PreventionType & SPELL_PREVENTION_TYPE_PACIFY)
                                    return SPELL_FAILED_PACIFIED;
                                else if (m_spellInfo->PreventionType & SPELL_PREVENTION_TYPE_SILENCE)
                                    return SPELL_FAILED_SILENCED;
                                break;
                            case SPELL_AURA_TRANSFORM:
                                if (m_spellInfo->PreventionType & SPELL_PREVENTION_TYPE_POLYMORPH || m_CastItem)
                                    if (aura->GetSpellInfo()->GetAllEffectsMechanicMask() & (1 << MECHANIC_POLYMORPH) && !(mechanic_immune & (1 << MECHANIC_POLYMORPH)))
                                        return SPELL_FAILED_PREVENTED_BY_MECHANIC;
                            default: break;
                        }
                    }
                }
            }
        }
        // You are prevented from casting and the spell casted does not grant immunity. Return a failed error.
        else
            return prevented_reason;
    }
    return SPELL_CAST_OK;
}

bool Spell::CanBreakAura(AuraType auraType) const
{
    for (auto&& auraEffect : m_caster->GetAuraEffectsByType(auraType))
    {
        for (auto&& effect : m_spellInfo->Effects)
        {
            if (effect.Effect == SPELL_EFFECT_APPLY_AURA)
            {
                switch (effect.ApplyAuraName)
                {
                    case SPELL_AURA_STATE_IMMUNITY:
                        if (effect.MiscValue == auraEffect->GetAuraType())
                            return true;
                        break;
                    case SPELL_AURA_SCHOOL_IMMUNITY:
                    case SPELL_AURA_MOD_IMMUNE_AURA_APPLY_SCHOOL:
                        if (!auraEffect->GetSpellInfo()->HasAttribute(SPELL_ATTR2_NOT_DISPELED_BY_SCHOOL_IMMUNITY) && auraEffect->GetSpellInfo()->SchoolMask & effect.MiscValue)
                            return true;
                        break;
                    case SPELL_AURA_DISPEL_IMMUNITY:
                        if (effect.MiscValue == auraEffect->GetSpellInfo()->Dispel)
                            return true;
                        break;
                    case SPELL_AURA_MECHANIC_IMMUNITY:
                        if (effect.MiscValue == auraEffect->GetSpellInfo()->Mechanic || effect.MiscValue == auraEffect->GetSpellInfo()->Effects[auraEffect->GetEffIndex()].Mechanic)
                            return true;
                        break;
                    case SPELL_AURA_APPLY_CREATURE_IMMUNITIES:
                        if (CreatureImmunitiesEntry const* entry = sCreatureImmunitiesStore.LookupEntry(effect.MiscValue))
                            if (entry->Mechanic & auraEffect->GetSpellInfo()->GetAllEffectsMechanicMask())
                                return true;
                        break;
                }
            }
        }
    }

    return false;
}

bool Spell::CanAutoCast(Unit* target)
{
    if (!target)
    {
        if (CheckPetCast(target) != SPELL_CAST_OK)  // Power cost calulation and script loading
            return false;
        return CallScriptCheckAutoCastHandlers() == SPELL_CAST_OK;
    }

    if (m_spellInfo->HasAttribute(SPELL_ATTR7_CONSOLIDATED_RAID_BUFF) && target == GetCaster())
        return false;

    uint64 targetguid = target->GetGUID();

    for (uint32 j = 0; j < MAX_SPELL_EFFECTS; ++j)
    {
        if (m_spellInfo->Effects[j].Effect == SPELL_EFFECT_APPLY_AURA)
        {
            // Mortal Cleave, Legion Strike
            if (m_spellInfo->Id == 115625 || m_spellInfo->Id == 30213)
                continue;
            if (m_spellInfo->StackAmount <= 1)
            {
                if (target->HasAuraEffect(m_spellInfo->Id, j))
                    return false;
            }
            else
            {
                if (AuraEffect* aureff = target->GetAuraEffect(m_spellInfo->Id, j))
                    if (aureff->GetBase()->GetStackAmount() >= m_spellInfo->StackAmount)
                        return false;
            }
        }
        else if (m_spellInfo->Effects[j].IsAreaAuraEffect())
        {
            if (target->HasAuraEffect(m_spellInfo->Id, j))
                return false;
        }
    }

    SpellCastResult result = CheckPetCast(target); // Power cost calulation and script loading

    if (result == SPELL_CAST_OK || result == SPELL_FAILED_UNIT_NOT_INFRONT)
    {
        if (CallScriptCheckAutoCastHandlers() != SPELL_CAST_OK)
            return false;

        if (m_targets.GetObjectTargetGUID()) // Set in script call
            targetguid = m_targets.GetObjectTargetGUID();

        // do not check targets for ground-targeted spells (we target them on top of the intended target anyway)
        if (GetSpellInfo()->ExplicitTargetMask & TARGET_FLAG_DEST_LOCATION)
            return true;

        if (GetSpellInfo()->HasEffect(SPELL_EFFECT_APPLY_AREA_AURA_RAID) && GetCaster()->IsInRaidWith(target))
            return true;

        SelectSpellTargets();

        //check if among target units, our WANTED target is as well (->only self cast spells return false)
        for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
            if (ihit->targetGUID == targetguid)
                return true;
    }
    // either the cast failed or the intended target wouldn't be hit
    return false;
}

SpellCastResult Spell::CheckRange(bool strict)
{
    // Don't check for instant cast spells
    if (!strict && m_casttime == 0)
        return SPELL_CAST_OK;

    uint32 range_type = 0;

    if (m_spellInfo->RangeEntry)
    {
        // check needed by 68766 51693 - both spells are cast on enemies and have 0 max range
        // these are triggered by other spells - possibly we should omit range check in that case?
        if (m_spellInfo->RangeEntry->ID == 1)
            return SPELL_CAST_OK;

        range_type = m_spellInfo->RangeEntry->type;
    }

    Unit* target = m_targets.GetUnitTarget();
    float max_range = m_caster->GetSpellMaxRangeForTarget(target, m_spellInfo);
    float min_range = m_caster->GetSpellMinRangeForTarget(target, m_spellInfo);

    if (range_type == SPELL_RANGE_MELEE)
        max_range = NOMINAL_MELEE_RANGE;

    if (Player* modOwner = m_caster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RANGE, max_range, this);

    if (target && target != m_caster)
    {
        if (range_type == SPELL_RANGE_MELEE)
        {
            // Because of lag, we can not check too strictly here.
            if (!m_caster->IsWithinMeleeRange(target, max_range))
                return !(_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_OUT_OF_RANGE : SPELL_FAILED_DONT_REPORT;
        }
        else if (!m_caster->IsWithinCombatRange(target, max_range))
            return !(_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_OUT_OF_RANGE : SPELL_FAILED_DONT_REPORT; //0x5A;

        if (range_type == SPELL_RANGE_RANGED)
        {
            if (m_caster->IsWithinMeleeRange(target) && target->GetEntry() != 55294) // Ultraxion
                return !(_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_TOO_CLOSE : SPELL_FAILED_DONT_REPORT;
        }
        else if (min_range && m_caster->IsWithinCombatRange(target, min_range)) // skip this check if min_range = 0
            return !(_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_TOO_CLOSE : SPELL_FAILED_DONT_REPORT;

        if (m_caster->GetTypeId() == TYPEID_PLAYER &&
            (m_spellInfo->FacingCasterFlags & SPELL_FACING_FLAG_INFRONT) && !m_caster->HasInArc(static_cast<float>(M_PI), target))
        {
            // Cloak and Dagger
            bool ignoreFacing = m_spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE && m_spellInfo->SpellFamilyFlags[0] & 0x00000700 && m_caster->HasAura(138106);
            if (!ignoreFacing)
                return !(_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_UNIT_NOT_INFRONT : SPELL_FAILED_DONT_REPORT;
        }
    }
    // Some shit here. We have few spells (e.g. Hand of Gul'dan (105174), Chaos Wave(124916)) which have unit target (not caster) and one effect with TARGET_DEST_DEST.
    // If I'm not wrong, this is needed to put animation to the dest and not to the unit target.
    // And ofc check below will fail becasue of not including target's combat reach.
    // I don't know is this safe but it seems to be (need to check client tho).
    else if (m_targets.HasDst() && !m_targets.HasTraj())
    {
        Unit* caster = m_caster;
        if (m_spellInfo->Id == 119909 || m_spellInfo->Id == 119913) // Whiplash & Fellash casterd through Command Deamon
            if (Unit* succubus = m_caster->GetGuardianPet())
                caster = succubus;

        if (!caster->IsInDist(m_targets.GetDstPos(), max_range))
            return SPELL_FAILED_OUT_OF_RANGE;
        if (min_range && caster->IsInDist(m_targets.GetDstPos(), min_range))
            return SPELL_FAILED_TOO_CLOSE;
    }

    return SPELL_CAST_OK;
}

SpellCastResult Spell::CheckPower()
{
    // item cast not used power
    if (m_CastItem)
        return SPELL_CAST_OK;

    // health as power used - need check health amount
    if (m_powerType == POWER_HEALTH)
    {
        if (int32(m_caster->GetHealth()) <= m_powerCost)
            return SPELL_FAILED_CASTER_AURASTATE;
        return SPELL_CAST_OK;
    }
    // Check valid power type
    if (m_powerType >= MAX_POWERS)
    {
        TC_LOG_ERROR("spells", "Spell::CheckPower: Unknown power type '%d'", m_powerType);
        return SPELL_FAILED_UNKNOWN;
    }

    //check rune cost only if a spell has PowerType == POWER_RUNES
    if (m_powerType == POWER_RUNES)
    {
        SpellCastResult failReason = CheckRuneCost(m_spellInfo->RuneCostID);
        if (failReason != SPELL_CAST_OK)
            return failReason;
    }

    // Check power amount
    if (int32(m_caster->GetPower(m_powerType)) < m_powerCost)
        return SPELL_FAILED_NO_POWER;
    else
        return SPELL_CAST_OK;
}

SpellCastResult Spell::CheckItems()
{
    Player* player = m_caster->ToPlayer();
    if (!player)
        return SPELL_CAST_OK;

    if (!m_CastItem)
    {
        if (m_castItemGUID)
            return SPELL_FAILED_ITEM_NOT_READY;
    }
    else
    {
        uint32 itemid = m_CastItem->GetEntry();
        if (!player->HasItemCount(itemid))
            return SPELL_FAILED_ITEM_NOT_READY;

        ItemTemplate const* proto = m_CastItem->GetTemplate();
        if (!proto)
            return SPELL_FAILED_ITEM_NOT_READY;

        for (uint8 i = 0; i < MAX_ITEM_SPELLS; ++i)
            if (proto->Spells[i].SpellCharges)
                if (m_CastItem->GetSpellCharges(i) == 0)
                    return SPELL_FAILED_NO_CHARGES_REMAIN;

        // consumable cast item checks
        if (proto->Class == ITEM_CLASS_CONSUMABLE && m_targets.GetUnitTarget())
        {
            // such items should only fail if there is no suitable effect at all - see Rejuvenation Potions for example
            SpellCastResult failReason = SPELL_CAST_OK;
            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            {
                // skip check, pet not required like checks, and for TARGET_UNIT_PET m_targets.GetUnitTarget() is not the real target but the caster
                if (m_spellInfo->Effects[i].TargetA.GetTarget() == TARGET_UNIT_PET)
                    continue;

                if (m_spellInfo->Effects[i].Effect == SPELL_EFFECT_HEAL)
                {
                    if (m_targets.GetUnitTarget()->IsFullHealth())
                    {
                        failReason = SPELL_FAILED_ALREADY_AT_FULL_HEALTH;
                        continue;
                    }
                    else
                    {
                        failReason = SPELL_CAST_OK;
                        break;
                    }
                }

                // Mana Potion, Rage Potion, Thistle Tea(Rogue), ...
                if (m_spellInfo->Effects[i].Effect == SPELL_EFFECT_ENERGIZE)
                {
                    if (m_spellInfo->Effects[i].MiscValue < 0 || m_spellInfo->Effects[i].MiscValue >= int8(MAX_POWERS))
                    {
                        failReason = SPELL_FAILED_ALREADY_AT_FULL_POWER;
                        continue;
                    }

                    Powers power = Powers(m_spellInfo->Effects[i].MiscValue);
                    if (m_targets.GetUnitTarget()->GetPower(power) == m_targets.GetUnitTarget()->GetMaxPower(power))
                    {
                        failReason = SPELL_FAILED_ALREADY_AT_FULL_POWER;
                        continue;
                    }
                    else
                    {
                        failReason = SPELL_CAST_OK;
                        break;
                    }
                }
            }
            if (failReason != SPELL_CAST_OK)
                return failReason;
        }
    }

    // check target item
    if (m_targets.GetItemTargetGUID())
    {
        if (!m_targets.GetItemTarget())
            return SPELL_FAILED_ITEM_GONE;

        if (!m_targets.GetItemTarget()->IsFitToSpellRequirements(m_spellInfo))
            return SPELL_FAILED_EQUIPPED_ITEM_CLASS;
    }
    // if not item target then required item must be equipped
    else
    {
        if (!(_triggeredCastFlags & TRIGGERED_IGNORE_EQUIPPED_ITEM_REQUIREMENT))
            if (!player->HasItemFitToSpellRequirements(m_spellInfo))
                return SPELL_FAILED_EQUIPPED_ITEM_CLASS;
    }

    // do not take reagents for these item casts
    if (!(m_CastItem && m_CastItem->GetTemplate()->Flags & ITEM_PROTO_FLAG_TRIGGERED_CAST))
    {
        bool checkReagents = !(_triggeredCastFlags & TRIGGERED_IGNORE_POWER_AND_REAGENT_COST) && !player->CanNoReagentCast(m_spellInfo);
        // Not own traded item (in trader trade slot) requires reagents even if triggered spell
        if (!checkReagents)
            if (Item* targetItem = m_targets.GetItemTarget())
                if (targetItem->GetOwnerGUID() != m_caster->GetGUID())
                    checkReagents = true;

        // check reagents (ignore triggered spells with reagents processed by original spell) and special reagent ignore case.
        if (checkReagents)
        {
            for (uint32 i = 0; i < MAX_SPELL_REAGENTS; i++)
            {
                if (m_spellInfo->Reagent[i] <= 0)
                    continue;

                uint32 itemid    = m_spellInfo->Reagent[i];
                uint32 itemcount = m_spellInfo->ReagentCount[i];

                // if CastItem is also spell reagent
                if (m_CastItem && m_CastItem->GetEntry() == itemid)
                {
                    ItemTemplate const* proto = m_CastItem->GetTemplate();
                    if (!proto)
                        return SPELL_FAILED_ITEM_NOT_READY;
                    for (uint8 s = 0; s < MAX_ITEM_PROTO_SPELLS; ++s)
                    {
                        // CastItem will be used up and does not count as reagent
                        int32 charges = m_CastItem->GetSpellCharges(s);
                        if (proto->Spells[s].SpellCharges < 0 && abs(charges) < 2)
                        {
                            ++itemcount;
                            break;
                        }
                    }
                }
                if (!player->HasItemCount(itemid, itemcount))
                    return IsTriggered() ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_REAGENTS;
            }
        }

        // check totem-item requirements (items presence in inventory)
        uint32 totems = 2;
        for (uint8 i = 0; i < 2; ++i)
        {
            if (m_spellInfo->Totem[i] != 0)
            {
                if (player->HasItemCount(m_spellInfo->Totem[i]))
                {
                    totems -= 1;
                    continue;
                }
            }
            else
                totems -= 1;
        }
        if (totems != 0)
            return SPELL_FAILED_TOTEMS;
    }

    // special checks for spell effects
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        switch (m_spellInfo->Effects[i].Effect)
        {
            case SPELL_EFFECT_CREATE_ITEM:
            case SPELL_EFFECT_CREATE_ITEM_2:
            {
                if (!IsTriggered() && m_spellInfo->Effects[i].ItemType)
                {
                    ItemPosCountVec dest;
                    InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, m_spellInfo->Effects[i].ItemType, 1);
                    if (msg != EQUIP_ERR_OK)
                    {
                        ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(m_spellInfo->Effects[i].ItemType);
                        /// @todo Needs review
                        if (pProto && !(pProto->ItemLimitCategory))
                        {
                            player->SendEquipError(msg, NULL, NULL, m_spellInfo->Effects[i].ItemType);
                            return SPELL_FAILED_DONT_REPORT;
                        }
                        if (GetSpellInfo()->Id == 759 || GetSpellInfo()->Id == 119316)
                        {
                            player->CastSpell(player, GetSpellInfo()->Id == 119316 ? 119318 : 54408, false);
                            return SPELL_FAILED_DONT_REPORT;
                        }
                        return SPELL_FAILED_TOO_MANY_OF_ITEM;
                    }
                }
                break;
            }
            case SPELL_EFFECT_ENCHANT_ITEM:
                if (m_spellInfo->Effects[i].ItemType && m_targets.GetItemTarget()
                    && (m_targets.GetItemTarget()->IsVellum()))
                {
                    // cannot enchant vellum for other player
                    if (m_targets.GetItemTarget()->GetOwner() != m_caster)
                        return SPELL_FAILED_NOT_TRADEABLE;
                    // do not allow to enchant vellum from scroll made by vellum-prevent exploit
                    if (m_CastItem && m_CastItem->GetTemplate()->Flags & ITEM_PROTO_FLAG_TRIGGERED_CAST)
                        return SPELL_FAILED_TOTEM_CATEGORY;
                    ItemPosCountVec dest;
                    InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, m_spellInfo->Effects[i].ItemType, 1);
                    if (msg != EQUIP_ERR_OK)
                    {
                        player->SendEquipError(msg, NULL, NULL, m_spellInfo->Effects[i].ItemType);
                        return SPELL_FAILED_DONT_REPORT;
                    }
                }
                // no break
            case SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC:
            {
                Item* targetItem = m_targets.GetItemTarget();
                if (!targetItem)
                    return SPELL_FAILED_ITEM_NOT_FOUND;

                if (targetItem->GetTemplate()->ItemLevel < m_spellInfo->BaseLevel)
                    return SPELL_FAILED_LOWLEVEL;

                bool isItemUsable = false;
                for (uint8 e = 0; e < MAX_ITEM_PROTO_SPELLS; ++e)
                {
                    ItemTemplate const* proto = targetItem->GetTemplate();
                    if (proto->Spells[e].SpellId && (
                        proto->Spells[e].SpellTrigger == ITEM_SPELLTRIGGER_ON_USE ||
                        proto->Spells[e].SpellTrigger == ITEM_SPELLTRIGGER_ON_NO_DELAY_USE))
                    {
                        isItemUsable = true;
                        break;
                    }
                }

                SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(m_spellInfo->Effects[i].MiscValue);
                // do not allow adding usable enchantments to items that have use effect already
                if (pEnchant && isItemUsable)
                    for (uint8 s = 0; s < MAX_ITEM_ENCHANTMENT_EFFECTS; ++s)
                        if (pEnchant->Type[s] == ITEM_ENCHANTMENT_TYPE_USE_SPELL)
                            return SPELL_FAILED_ON_USE_ENCHANT;

                // Not allow enchant in trade slot for some enchant type
                if (targetItem->GetOwner() != m_caster)
                {
                    if (!pEnchant)
                        return SPELL_FAILED_ERROR;
                    if (pEnchant->Flags & ENCHANTMENT_FLAG_CAN_SOULBOUND)
                        return SPELL_FAILED_NOT_TRADEABLE;
                }
                break;
            }
            case SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY:
            {
                Item* item = m_targets.GetItemTarget();
                if (!item)
                    return SPELL_FAILED_ITEM_NOT_FOUND;
                // Not allow enchant in trade slot for some enchant type
                if (item->GetOwner() != m_caster)
                {
                    uint32 enchant_id = m_spellInfo->Effects[i].MiscValue;
                    SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
                    if (!pEnchant)
                        return SPELL_FAILED_ERROR;
                    if (pEnchant->Flags & ENCHANTMENT_FLAG_CAN_SOULBOUND)
                        return SPELL_FAILED_NOT_TRADEABLE;
                }
                break;
            }
            case SPELL_EFFECT_ENCHANT_HELD_ITEM:
                // check item existence in effect code (not output errors at offhand hold item effect to main hand for example
                break;
            case SPELL_EFFECT_DISENCHANT:
            {
                if (!m_targets.GetItemTarget())
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;

                // prevent disenchanting in trade slot
                if (m_targets.GetItemTarget()->GetOwnerGUID() != m_caster->GetGUID())
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;

                ItemTemplate const* itemProto = m_targets.GetItemTarget()->GetTemplate();
                if (!itemProto)
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;

                uint32 item_quality = itemProto->Quality;
                // 2.0.x addon: Check player enchanting level against the item disenchanting requirements
                uint32 item_disenchantskilllevel = itemProto->RequiredDisenchantSkill;
                if (item_disenchantskilllevel == uint32(-1))
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;
                if (item_disenchantskilllevel > player->GetSkillValue(SKILL_ENCHANTING))
                    return SPELL_FAILED_LOW_CASTLEVEL;
                if (item_quality > 4 || item_quality < 2)
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;
                if (itemProto->Class != ITEM_CLASS_WEAPON && itemProto->Class != ITEM_CLASS_ARMOR)
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;
                if (!itemProto->DisenchantID)
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;
                break;
            }
            case SPELL_EFFECT_PROSPECTING:
            {
                if (!m_targets.GetItemTarget())
                    return SPELL_FAILED_CANT_BE_PROSPECTED;
                //ensure item is a prospectable ore
                if (!(m_targets.GetItemTarget()->GetTemplate()->Flags & ITEM_PROTO_FLAG_PROSPECTABLE))
                    return SPELL_FAILED_CANT_BE_PROSPECTED;
                //prevent prospecting in trade slot
                if (m_targets.GetItemTarget()->GetOwnerGUID() != m_caster->GetGUID())
                    return SPELL_FAILED_CANT_BE_PROSPECTED;
                //Check for enough skill in jewelcrafting
                uint32 item_prospectingskilllevel = m_targets.GetItemTarget()->GetTemplate()->RequiredSkillRank;
                if (item_prospectingskilllevel >player->GetSkillValue(SKILL_JEWELCRAFTING))
                    return SPELL_FAILED_LOW_CASTLEVEL;
                //make sure the player has the required ores in inventory
                if (m_targets.GetItemTarget()->GetCount() < 5)
                    return SPELL_FAILED_NEED_MORE_ITEMS;

                if (!LootTemplates_Prospecting.HaveLootFor(m_targets.GetItemTargetEntry()))
                    return SPELL_FAILED_CANT_BE_PROSPECTED;

                break;
            }
            case SPELL_EFFECT_MILLING:
            {
                if (!m_targets.GetItemTarget())
                    return SPELL_FAILED_CANT_BE_MILLED;
                //ensure item is a millable herb
                if (!(m_targets.GetItemTarget()->GetTemplate()->Flags & ITEM_PROTO_FLAG_MILLABLE))
                    return SPELL_FAILED_CANT_BE_MILLED;
                //prevent milling in trade slot
                if (m_targets.GetItemTarget()->GetOwnerGUID() != m_caster->GetGUID())
                    return SPELL_FAILED_CANT_BE_MILLED;
                //Check for enough skill in inscription
                uint32 item_millingskilllevel = m_targets.GetItemTarget()->GetTemplate()->RequiredSkillRank;
                if (item_millingskilllevel > player->GetSkillValue(SKILL_INSCRIPTION))
                    return SPELL_FAILED_LOW_CASTLEVEL;
                //make sure the player has the required herbs in inventory
                if (m_targets.GetItemTarget()->GetCount() < 5)
                    return SPELL_FAILED_NEED_MORE_ITEMS;

                if (!LootTemplates_Milling.HaveLootFor(m_targets.GetItemTargetEntry()))
                    return SPELL_FAILED_CANT_BE_MILLED;

                break;
            }
            case SPELL_EFFECT_WEAPON_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
            {
                if (m_attackType != RANGED_ATTACK)
                    break;

                Item* pItem = player->GetWeaponForAttack(m_attackType);
                if (!pItem || pItem->IsBroken())
                    return SPELL_FAILED_EQUIPPED_ITEM;

                switch (pItem->GetTemplate()->SubClass)
                {
                    case ITEM_SUBCLASS_WEAPON_THROWN:
                    {
                        uint32 ammo = pItem->GetEntry();
                        if (!player->HasItemCount(ammo))
                            return SPELL_FAILED_NO_AMMO;
                        break;
                    }
                    case ITEM_SUBCLASS_WEAPON_GUN:
                    case ITEM_SUBCLASS_WEAPON_BOW:
                    case ITEM_SUBCLASS_WEAPON_CROSSBOW:
                    case ITEM_SUBCLASS_WEAPON_WAND:
                        break;
                    default:
                        break;
                }
                break;
            }
            case SPELL_EFFECT_CREATE_MANA_GEM:
            {
                 uint32 item_id = m_spellInfo->Effects[i].ItemType;
                 ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(item_id);

                 if (!pProto)
                     return SPELL_FAILED_ITEM_AT_MAX_CHARGES;

                 if (Item* pitem = player->GetItemByEntry(item_id))
                 {
                     for (int x = 0; x < MAX_ITEM_PROTO_SPELLS; ++x)
                         if (pProto->Spells[x].SpellCharges != 0 && pitem->GetSpellCharges(x) == pProto->Spells[x].SpellCharges)
                             return SPELL_FAILED_ITEM_AT_MAX_CHARGES;
                 }
                 break;
            }
            default:
                break;
        }
    }

    bool skipWeaponCheck = _triggeredCastFlags & TRIGGERED_IGNORE_EQUIPPED_ITEM_REQUIREMENT && m_caster->GetTypeId() != TYPEID_PLAYER;

    // check weapon presence in slots for main/offhand weapons
    if (!skipWeaponCheck && m_spellInfo->EquippedItemClass >=0)
    {
        // main hand weapon required
        if (m_spellInfo->AttributesEx3 & SPELL_ATTR3_MAIN_HAND)
        {
            Item* item = m_caster->ToPlayer()->GetWeaponForAttack(BASE_ATTACK);

            // skip spell if no weapon in slot or broken
            if (!item || item->IsBroken())
                return (_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_EQUIPPED_ITEM_CLASS;

            // skip spell if weapon not fit to triggered spell
            if (!item->IsFitToSpellRequirements(m_spellInfo))
                return (_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_EQUIPPED_ITEM_CLASS;
        }

        // offhand hand weapon required
        if (m_spellInfo->AttributesEx3 & SPELL_ATTR3_REQ_OFFHAND)
        {
            Item* item = m_caster->ToPlayer()->GetWeaponForAttack(OFF_ATTACK);

            // skip spell if no weapon in slot or broken
            if (!item || item->IsBroken())
                return (_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_EQUIPPED_ITEM_CLASS;

            // skip spell if weapon not fit to triggered spell
            if (!item->IsFitToSpellRequirements(m_spellInfo))
                return (_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_EQUIPPED_ITEM_CLASS;
        }
    }

    if (m_researchData)
    {
        ResearchProjectEntry const* project = nullptr;
        for (uint32 i = 0; i < sResearchProjectStore.GetNumRows(); ++i)
        {
            if (auto entry = sResearchProjectStore.LookupEntry(i))
            {
                if (entry->SpellId == m_spellInfo->Id)
                {
                    project = entry;
                    break;
                }
            }
        }

        if (!project)
            return SPELL_FAILED_DONT_REPORT;

        auto branch = sResearchBranchStore.LookupEntry(project->ResearchBranchId);
        if (m_researchData->FragmentCurrencyId != branch->FragmentCurrencyId)
            return SPELL_FAILED_DONT_REPORT;

        if (m_researchData->KeystoneItemId && m_researchData->KeystoneItemId != branch->KeystoneItemId)
            return SPELL_FAILED_DONT_REPORT;

        if (m_researchData->KeystoneCount > project->KeystoneCount)
            return SPELL_FAILED_DONT_REPORT;

        uint32 keystonebonus = branch->Id >= 29 ? 20 : 12;

        if (m_researchData->FragmentCount != project->RequiredFragmentCount - m_researchData->KeystoneCount * keystonebonus)
            return SPELL_FAILED_DONT_REPORT;

        if (!player->HasCurrency(m_researchData->FragmentCurrencyId, m_researchData->FragmentCount))
            return SPELL_FAILED_DONT_REPORT;

        if (m_researchData->KeystoneCount && !player->HasItemCount(m_researchData->KeystoneItemId, m_researchData->KeystoneCount))
            return SPELL_FAILED_DONT_REPORT;
    }

    return SPELL_CAST_OK;
}

void Spell::Delayed() // only called in DealDamage()
{
    if (!m_caster)// || m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    //if (m_spellState == SPELL_STATE_DELAYED)
    //    return;                                             // spell is active and can't be time-backed

    if (isDelayableNoMore())                                 // Spells may only be delayed twice
        return;

    // spells not loosing casting time (slam, dynamites, bombs..)
    //if (!(m_spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_DAMAGE))
    //    return;

    //check pushback reduce
    int32 delaytime = 500;                                  // spellcasting delay is normally 500ms
    int32 delayReduce = 100;                                // must be initialized to 100 for percent modifiers
    m_caster->ToPlayer()->ApplySpellMod(m_spellInfo->Id, SPELLMOD_NOT_LOSE_CASTING_TIME, delayReduce, this);
    delayReduce += m_caster->GetTotalAuraModifier(SPELL_AURA_REDUCE_PUSHBACK) - 100;
    if (delayReduce >= 100)
        return;

    AddPct(delaytime, -delayReduce);

    if (m_timer + delaytime > m_casttime)
    {
        delaytime = m_casttime - m_timer;
        m_timer = m_casttime;
    }
    else
        m_timer += delaytime;

    TC_LOG_INFO("spells", "Spell %u partially interrupted for (%d) ms at damage", m_spellInfo->Id, delaytime);

    ObjectGuid guid = m_caster->GetGUID();

    WorldPacket data(SMSG_SPELL_DELAYED, 8+4);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[5]);

    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[3]);
    data << uint32(delaytime);
    data.WriteByteSeq(guid[4]);
    m_caster->SendMessageToSet(&data, true);
}

void Spell::DelayedChannel()
{
    if (!m_caster || m_caster->GetTypeId() != TYPEID_PLAYER || getState() != SPELL_STATE_CASTING)
        return;

    if (isDelayableNoMore())                                    // Spells may only be delayed twice
        return;

    //check pushback reduce
    int32 delaytime = CalculatePct(m_spellInfo->GetDuration(), 25); // channeling delay is normally 25% of its time per hit
    int32 delayReduce = 100;                                    // must be initialized to 100 for percent modifiers
    m_caster->ToPlayer()->ApplySpellMod(m_spellInfo->Id, SPELLMOD_NOT_LOSE_CASTING_TIME, delayReduce, this);
    delayReduce += m_caster->GetTotalAuraModifier(SPELL_AURA_REDUCE_PUSHBACK) - 100;
    if (delayReduce >= 100)
        return;

    AddPct(delaytime, -delayReduce);

    bool periodic = false;

    for (uint32 i = 0; i < MAX_SPELL_EFFECTS && !periodic; ++i)
        if (m_spellInfo->Effects[i].IsEffect() && m_spellInfo->Effects[i].ApplyAuraTickCount)
            periodic = true;

    if (periodic)
    {
        if (m_timer + delaytime > m_channelTime)
        {
            delaytime = m_channelTime - m_timer;
            m_timer = m_channelTime;
        }
        else
            m_timer += delaytime;
    }
    else
    {
        if (m_timer <= delaytime)
        {
            delaytime = m_timer;
            m_timer = 0;
        }
        else
            m_timer -= delaytime;

        delaytime = -delaytime;
    }

    TC_LOG_DEBUG("spells", "Spell %u partially interrupted for %i ms, new duration: %u ms", m_spellInfo->Id, delaytime, m_timer);

    for (std::list<TargetInfo>::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
        if ((*ihit).missCondition == SPELL_MISS_NONE)
            if (Unit* unit = (m_caster->GetGUID() == ihit->targetGUID) ? m_caster : ObjectAccessor::GetUnit(*m_caster, ihit->targetGUID))
                unit->DelayOwnedAuras(m_spellInfo->Id, m_originalCasterGUID, delaytime);

    // partially interrupt persistent area auras
    if (DynamicObject* dynObj = m_caster->GetDynObject(m_spellInfo->Id))
        dynObj->Delay(delaytime);

    SendChannelUpdate(m_timer);
}

void Spell::UpdatePointers()
{
    if (m_originalCasterGUID == m_caster->GetGUID())
        m_originalCaster = m_caster;
    else
    {
        m_originalCaster = ObjectAccessor::GetUnit(*m_caster, m_originalCasterGUID);
        if (m_originalCaster && !m_originalCaster->IsInWorld())
            m_originalCaster = NULL;
    }

    if (m_castItemGUID && m_caster->GetTypeId() == TYPEID_PLAYER)
        m_CastItem = m_caster->ToPlayer()->GetItemByGuid(m_castItemGUID);

    m_targets.Update(m_caster);

    // further actions done only for dest targets
    if (!m_targets.HasDst())
        return;

    // cache last transport
    WorldObject* transport = NULL;

    // update effect destinations (in case of moved transport dest target)
    for (uint8 effIndex = 0; effIndex < MAX_SPELL_EFFECTS; ++effIndex)
    {
        SpellDestination& dest = m_destTargets[effIndex];
        if (!dest._transportGUID)
            continue;

        if (!transport || transport->GetGUID() != dest._transportGUID)
            transport = ObjectAccessor::GetWorldObject(*m_caster, dest._transportGUID);

        if (transport)
        {
            dest._position.Relocate(transport);
            dest._position.RelocateOffset(dest._transportOffset);
        }
    }
}

CurrentSpellTypes Spell::GetCurrentContainer() const
{
    if (IsNextMeleeSwingSpell())
        return (CURRENT_MELEE_SPELL);
    else if (IsAutoRepeat())
        return (CURRENT_AUTOREPEAT_SPELL);
    else if (m_spellInfo->IsChanneled())
        return (CURRENT_CHANNELED_SPELL);
    else
        return (CURRENT_GENERIC_SPELL);
}

bool Spell::CheckEffectTarget(Unit const* target, uint32 eff) const
{
    auto& effect = m_spellInfo->Effects[eff];

    switch (effect.ApplyAuraName)
    {
        case SPELL_AURA_MOD_POSSESS:
        case SPELL_AURA_MOD_CHARM:
        case SPELL_AURA_AOE_CHARM:
            if (target->GetTypeId() == TYPEID_UNIT && target->IsVehicle())
                return false;
            if (target->IsMounted())
                return false;
            if (target->GetCharmerGUID())
                return false;
            if (int32 damage = CalculateDamage(eff, target))
                if ((int32)target->GetLevel() > damage)
                    return false;
            break;
        default:
            break;
    }

    switch (effect.Effect)
    {
        case SPELL_EFFECT_RESURRECT_WITH_AURA:
            if (target->IsAlive() || target->HasAura(effect.TriggerSpell))
                return false;
            break;
        default:
            break;
    }

    bool alwaysCheck = m_spellInfo->HasAttribute(SPELL_ATTR0_CU_ALWAYS_CHECK_LOS) ||
        (!m_spellInfo->HasAttribute(SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS) && effect.GetProvidedTargetMask() & (TARGET_FLAG_DEST_LOCATION | TARGET_FLAG_SOURCE_LOCATION) && effect.IsTargetingArea());

    if (!alwaysCheck &&
        (IsTriggered() || m_instantSpellDelayed || m_spellInfo->AttributesEx2 & SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS || DisableMgr::IsDisabledFor(DISABLE_TYPE_SPELL, m_spellInfo->Id, NULL, SPELL_DISABLE_LOS)))
        return true;

    /// @todo shit below shouldn't be here, but it's temporary
    //Check targets for LOS visibility (except spells without range limitations)
    switch (m_spellInfo->Effects[eff].Effect)
    {
        case SPELL_EFFECT_RESURRECT_NEW:
            // player far away, maybe his corpse near?
            if (target != m_caster && !target->IsWithinLOSInMap(m_caster))
            {
                if (!m_targets.GetCorpseTargetGUID())
                    return false;

                Corpse* corpse = ObjectAccessor::GetCorpse(*m_caster, m_targets.GetCorpseTargetGUID());
                if (!corpse)
                    return false;

                if (target->GetGUID() != corpse->GetOwnerGUID())
                    return false;

                if (!corpse->IsWithinLOSInMap(m_caster))
                    return false;
            }

            // all ok by some way or another, skip normal check
            break;
        default:                                            // normal case
            // Get GO cast coordinates if original caster -> GO
            WorldObject* caster = NULL;
            if (m_originalCasterGUID.IsGameObject())
                caster = m_caster->GetMap()->GetGameObject(m_originalCasterGUID);
            if (!caster)
                caster = m_caster;
            if (m_targets.HasDst())
            {
                // Skip LOS check for self-targeting effects of a spell, that was dest-targeted via spell_target_position
                // Fixes cases with self-targeted effects following a TARGET_DEST_DB teleport
                if (m_spellInfo->Effects[eff].TargetA.GetTarget() == TARGET_UNIT_CASTER || m_spellInfo->Effects[eff].TargetB.GetTarget() == TARGET_UNIT_CASTER)
                    for (uint8 i = 0; i <= eff; ++i)
                        if (m_spellInfo->Effects[i].TargetA.GetTarget() == TARGET_DEST_DB || m_spellInfo->Effects[i].TargetB.GetTarget() == TARGET_DEST_DB)
                            return true;

                float x, y, z;
                m_targets.GetDstPos()->GetPosition(x, y, z);

                if (!target->IsWithinLOS(x, y, z))
                    return false;
            }
            else if (target != m_caster && (!target->IsWithinLOSInMap(caster) && m_spellInfo->Effects[eff].Effect != SPELL_EFFECT_RESURRECT) && m_spellInfo->Effects[eff].TargetA.GetTarget() != TARGET_UNIT_TARGET_OR_UNIT_PARTY)
                return false;
            break;
    }

    return true;
}

bool Spell::IsNextMeleeSwingSpell() const
{
    return m_spellInfo->Attributes & SPELL_ATTR0_ON_NEXT_SWING;
}

bool Spell::IsAutoActionResetSpell() const
{
    /// @todo changed SPELL_INTERRUPT_FLAG_AUTOATTACK -> SPELL_INTERRUPT_FLAG_INTERRUPT to fix compile - is this check correct at all?
    return !IsTriggered() && (m_spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_INTERRUPT)
        && m_spellInfo->Id != 16166; // Elemental Mastery
}

bool Spell::IsCritForTarget(Unit* target) const
{
    if (!target)
        return false;

    for (auto itr : m_UniqueTargetInfo)
        if (itr.targetGUID == target->GetGUID() && itr.crit)
            return true;

    return false;
}

bool Spell::IsNeedSendToClient() const
{
    return m_cast_count || (!m_triggeredByAuraSpell && !IsTriggered()) || GetSpellInfo()->HasAttribute(SPELL_ATTR0_CU_ALWAYS_SEND_TO_CLIENT);
}

bool Spell::HaveTargetsForEffect(uint8 effect) const
{
    for (std::list<TargetInfo>::const_iterator itr = m_UniqueTargetInfo.begin(); itr != m_UniqueTargetInfo.end(); ++itr)
        if (itr->effectMask & (1 << effect))
            return true;

    for (std::list<GOTargetInfo>::const_iterator itr = m_UniqueGOTargetInfo.begin(); itr != m_UniqueGOTargetInfo.end(); ++itr)
        if (itr->effectMask & (1 << effect))
            return true;

    for (std::list<ItemTargetInfo>::const_iterator itr = m_UniqueItemInfo.begin(); itr != m_UniqueItemInfo.end(); ++itr)
        if (itr->effectMask & (1 << effect))
            return true;

    return false;
}

SpellEvent::SpellEvent(Spell* spell) : BasicEvent()
{
    m_Spell = spell;
}

SpellEvent::~SpellEvent()
{
    if (m_Spell->getState() != SPELL_STATE_FINISHED)
        m_Spell->cancel();

    if (m_Spell->IsDeletable())
    {
        delete m_Spell;
    }
    else
    {
        TC_LOG_ERROR("shitlog", "~SpellEvent: %s %u tried to delete non-deletable spell %u. Was not deleted, causes memory leak.\n",
            (m_Spell->GetCaster()->GetTypeId() == TYPEID_PLAYER ? "Player" : "Creature"), m_Spell->GetCaster()->GetGUID().GetCounter(), m_Spell->m_spellInfo->Id);
        //ASSERT(false);
    }
}

bool SpellEvent::Execute(uint64 e_time, uint32 p_time)
{
    // update spell if it is not finished
    if (m_Spell->getState() != SPELL_STATE_FINISHED)
        m_Spell->update(p_time);

    // check spell state to process
    switch (m_Spell->getState())
    {
        case SPELL_STATE_FINISHED:
        {
            // spell was finished, check deletable state
            if (m_Spell->IsDeletable())
            {
                // check, if we do have unfinished triggered spells
                return true;                                // spell is deletable, finish event
            }
            // event will be re-added automatically at the end of routine)
        } break;

        case SPELL_STATE_DELAYED:
        {
            // first, check, if we have just started
            if (m_Spell->GetDelayStart() != 0)
            {
                // no, we aren't, do the typical update
                // check, if we have channeled spell on our hands
                /*
                if (m_Spell->m_spellInfo->IsChanneled())
                {
                    // evented channeled spell is processed separately, casted once after delay, and not destroyed till finish
                    // check, if we have casting anything else except this channeled spell and autorepeat
                    if (m_Spell->GetCaster()->IsNonMeleeSpellCasted(false, true, true))
                    {
                        // another non-melee non-delayed spell is casted now, abort
                        m_Spell->cancel();
                    }
                    else
                    {
                        // Set last not triggered spell for apply spellmods
                        ((Player*)m_Spell->GetCaster())->SetSpellModTakingSpell(m_Spell, true);
                        // do the action (pass spell to channeling state)
                        m_Spell->handle_immediate();

                        // And remove after effect handling
                        ((Player*)m_Spell->GetCaster())->SetSpellModTakingSpell(m_Spell, false);
                    }
                    // event will be re-added automatically at the end of routine)
                }
                else
                */
                {
                    // run the spell handler and think about what we can do next
                    uint64 t_offset = e_time - m_Spell->GetDelayStart();
                    uint64 n_offset = m_Spell->handle_delayed(t_offset);
                    if (n_offset)
                    {
                        // re-add us to the queue
                        m_Spell->GetCaster()->m_Events.AddEvent(this, m_Spell->GetDelayStart() + n_offset, false);
                        return false;                       // event not complete
                    }
                    // event complete
                    // finish update event will be re-added automatically at the end of routine)
                }
            }
            else
            {
                // delaying had just started, record the moment
                m_Spell->SetDelayStart(e_time);
                // re-plan the event for the delay moment
                m_Spell->GetCaster()->m_Events.AddEvent(this, e_time + m_Spell->GetDelayMoment(), false);
                return false;                               // event not complete
            }
        } break;

        default:
        {
            // all other states
            // event will be re-added automatically at the end of routine)
        } break;
    }

    // spell processing not complete, plan event on the next update interval
    m_Spell->GetCaster()->m_Events.AddEvent(this, e_time + 1, false);
    return false;                                           // event not complete
}

void SpellEvent::Abort(uint64 /*e_time*/)
{
    // oops, the spell we try to do is aborted
    if (m_Spell->getState() != SPELL_STATE_FINISHED)
        m_Spell->cancel();
}

bool SpellEvent::IsDeletable() const
{
    return m_Spell->IsDeletable();
}

bool Spell::IsValidDeadOrAliveTarget(Unit const* target) const
{
    if (target->IsAlive())
        return !m_spellInfo->IsRequiringDeadTarget();
    if (m_spellInfo->IsAllowingDeadTarget())
        return true;
    return false;
}

void Spell::HandleLaunchPhase()
{
    // handle effects with SPELL_EFFECT_HANDLE_LAUNCH mode
    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        // don't do anything for empty effect
        if (!m_spellInfo->Effects[i].IsEffect())
            continue;

        HandleEffects(nullptr, nullptr, nullptr, nullptr, i, SPELL_EFFECT_HANDLE_LAUNCH);
    }

    float multiplier[MAX_SPELL_EFFECTS];
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (m_applyMultiplierMask & (1 << i))
            multiplier[i] = m_spellInfo->Effects[i].CalcDamageMultiplier(m_originalCaster, this);

    for (std::list<TargetInfo>::iterator ihit= m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        TargetInfo& target = *ihit;

        uint32 mask = target.effectMask;
        if (!mask)
            continue;

        DoAllEffectOnLaunchTarget(target, multiplier);
    }
}

void Spell::DoTriggersOnCast(ProcPhase phase)
{
    uint32 procflags = m_procAttacker | PROC_FLAG_DONE_ABILITY;
    uint32 procEx = PROC_EX_NORMAL_HIT;
    switch (m_spellInfo->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_MAGIC:
            if (GetSpellInfo()->IsPositive())
                procflags |= PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS;
            else
                procflags |= PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG;
            break;
        case SPELL_DAMAGE_CLASS_NONE:
            if (GetSpellInfo()->IsPositive())
                procflags |= PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS;
            else
                procflags |= PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_NEG;
            break;
    }

    Unit* target = m_targets.GetUnitTarget();
    if (target)
    {
        auto itr = std::find_if(m_UniqueTargetInfo.begin(), m_UniqueTargetInfo.end(), [=](Spell::TargetInfo const& info) { return info.targetGUID == target->GetGUID(); });
        if (itr != m_UniqueTargetInfo.end())
        {
            switch (itr->missCondition)
            {
                case SPELL_MISS_MISS:    procEx = PROC_EX_MISS;   break;
                case SPELL_MISS_RESIST:  procEx = PROC_EX_RESIST; break;
                case SPELL_MISS_DODGE:   procEx = PROC_EX_DODGE;  break;
                case SPELL_MISS_PARRY:   procEx = PROC_EX_PARRY;  break;
                case SPELL_MISS_BLOCK:   procEx = PROC_EX_BLOCK;  break;
                case SPELL_MISS_EVADE:   procEx = PROC_EX_EVADE;  break;
                case SPELL_MISS_IMMUNE:  procEx = PROC_EX_IMMUNE; break;
                case SPELL_MISS_IMMUNE2: procEx = PROC_EX_IMMUNE; break;
                case SPELL_MISS_DEFLECT: procEx = PROC_EX_DEFLECT; break;
                case SPELL_MISS_ABSORB:  procEx = PROC_EX_ABSORB; break;
                case SPELL_MISS_REFLECT: procEx = PROC_EX_REFLECT; break;
            }

            if (itr->damage && itr->crit)
                procEx |= PROC_EX_CRITICAL_HIT;
        }
    }

    ProcTriggerContext ctx{ this };
    ctx.Phase = phase;
    GetCaster()->ProcDamageAndSpellFor(false, target, procflags, procEx, m_attackType, GetSpellInfo(), 0, m_triggeredByAuraSpell, ctx);
}

void Spell::HandleAfterCast()
{
    CallScriptAfterCastHandlers();

    if (IsAutoActionResetSpell())
    {
        bool found = false;
        Unit::AuraEffectList const& vIgnoreReset = m_caster->GetAuraEffectsByType(SPELL_AURA_IGNORE_MELEE_RESET);
        for (Unit::AuraEffectList::const_iterator i = vIgnoreReset.begin(); i != vIgnoreReset.end(); ++i)
        {
            if ((*i)->IsAffectingSpell(m_spellInfo))
            {
                found = true;
                break;
            }
        }
        if (!found && !(m_spellInfo->AttributesEx2 & SPELL_ATTR2_NOT_RESET_AUTO_ACTIONS))
        {
            m_caster->resetAttackTimer(BASE_ATTACK);
            if (m_caster->HasOffhandWeapon())
                m_caster->resetAttackTimer(OFF_ATTACK);
            m_caster->resetAttackTimer(RANGED_ATTACK);
        }
    }

    if (m_dropModsPhase == SpellModDropPhase::OnCast)
        if (Player* player = m_caster->ToPlayer())
            player->RemoveSpellMods(this);

    if (Creature* caster = m_caster->ToCreature())
        if (caster->IsAIEnabled)
            caster->AI()->SpellCasted(this);

    // FIXME:
    if (m_casttime && GetSpellInfo()->SpellFamilyName == SPELLFAMILY_MAGE)
    {
        if (AuraEffect* iceFloes = m_caster->GetAuraEffect(108839, EFFECT_0))
            if (iceFloes->IsAffectingSpell(GetSpellInfo()))
                iceFloes->GetBase()->ModStackAmount(-1);
    }

    if (m_caster->HasAuraType(SPELL_AURA_MOD_CAMOUFLAGE))
    {
        auto list = m_caster->GetAuraEffectsByType(SPELL_AURA_MOD_CAMOUFLAGE); // Copy, because we can have two auras (main + invisibility)
        for (auto&& itr : list)
            if (itr->IsAffectingSpell(GetSpellInfo()))
                if (GetSpellInfo()->GetExplicitTargetMask() & (TARGET_FLAG_UNIT_ENEMY | TARGET_FLAG_CORPSE_ENEMY) && GetSpellInfo()->Id != 1130) // Exclude Hunter's Mark
                    itr->GetBase()->Remove();
    }

    if (m_darkSimulacrum)
        GetCaster()->RemoveAurasDueToSpell(77616);

    DoTriggersOnCast(ProcPhase::AfterCast);
}

void Spell::DoAllEffectOnLaunchTarget(TargetInfo& targetInfo, float* multiplier)
{
    Unit* unit = NULL;
    // In case spell hit target, do all effect on that target
    if (targetInfo.missCondition == SPELL_MISS_NONE)
        unit = m_caster->GetGUID() == targetInfo.targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, targetInfo.targetGUID);
    // In case spell reflect from target, do all effect on caster (if hit)
    else if (targetInfo.missCondition == SPELL_MISS_REFLECT && targetInfo.reflectResult == SPELL_MISS_NONE)
        unit = m_caster;
    if (!unit)
        return;

    Unit* caster = m_originalCaster ? m_originalCaster : m_caster;

    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (targetInfo.effectMask & (1<<i))
        {
            m_damage = 0;
            m_healing = 0;
            m_unmitigatedDamage = 0;

            HandleEffects(unit, nullptr, nullptr, nullptr, i, SPELL_EFFECT_HANDLE_LAUNCH_TARGET);

            if (m_damage > 0)
            {
                if (m_spellInfo->Effects[i].IsAffectedByAvoidance(false, caster))
                {
                    // Where are one spell 146343, that has misc value 4 but still must reduce all damage
                    m_damage *= unit->GetTotalAuraMultiplier(SPELL_AURA_MOD_AOE_DAMAGE_AVOIDANCE);
                    if (caster->GetTypeId() == TYPEID_UNIT)
                        m_damage *= unit->GetTotalAuraMultiplier(SPELL_AURA_MOD_CREATURE_AOE_DAMAGE_AVOIDANCE);

                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        uint32 targetAmount = m_UniqueTargetInfo.size();
                        if (targetAmount > 20)
                            m_damage = m_damage * 20 / targetAmount;
                    }
                }
            }
            else if (m_damage < 0)
            {
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                {
                    int32 targetAmount = int32(m_UniqueTargetInfo.size());

                    int32 maxTargetCount = GetHealTargetCap();
                    if (targetAmount > maxTargetCount)
                        m_damage = m_damage * maxTargetCount / targetAmount;
                }
            }

            if (m_applyMultiplierMask & (1 << i))
            {
                m_damage = int32(m_damage * m_damageMultipliers[i]);
                m_damageMultipliers[i] *= multiplier[i];
            }
            targetInfo.damage += m_damage;
            targetInfo.unmitigatedDamage += m_unmitigatedDamage;
        }
    }

    targetInfo.crit = m_caster->isSpellCrit(unit, m_spellInfo, m_spellSchoolMask, m_attackType);
}

SpellCastResult Spell::CanOpenLock(uint32 effIndex, uint32 lockId, SkillType& skillId, int32& reqSkillValue, int32& skillValue)
{
    if (!lockId)                                             // possible case for GO and maybe for items.
        return SPELL_CAST_OK;

    // Get LockInfo
    LockEntry const* lockInfo = sLockStore.LookupEntry(lockId);

    if (!lockInfo)
        return SPELL_FAILED_BAD_TARGETS;

    bool reqKey = false;                                    // some locks not have reqs

    for (int j = 0; j < MAX_LOCK_CASE; ++j)
    {
        switch (lockInfo->Type[j])
        {
            // check key item (many fit cases can be)
            case LOCK_KEY_ITEM:
                if (lockInfo->Index[j] && m_CastItem && m_CastItem->GetEntry() == lockInfo->Index[j])
                    return SPELL_CAST_OK;
                reqKey = true;
                break;
            // check key skill (only single first fit case can be)
            case LOCK_KEY_SKILL:
            {
                reqKey = true;

                // wrong locktype, skip
                if (uint32(m_spellInfo->Effects[effIndex].MiscValue) != lockInfo->Index[j])
                    continue;

                skillId = SkillByLockType(LockType(lockInfo->Index[j]));

                if (skillId != SKILL_NONE)
                {
                    reqSkillValue = lockInfo->Skill[j];

                    // castitem check: rogue using skeleton keys. the skill values should not be added in this case.
                    skillValue = m_CastItem || m_caster->GetTypeId()!= TYPEID_PLAYER ?
                        0 : m_caster->ToPlayer()->GetSkillValue(skillId);

                    // skill bonus provided by casting spell (mostly item spells)
                    // add the effect base points modifier from the spell casted (cheat lock / skeleton key etc.)
                    if (m_spellInfo->Effects[effIndex].TargetA.GetTarget() == TARGET_GAMEOBJECT_ITEM_TARGET || m_spellInfo->Effects[effIndex].TargetB.GetTarget() == TARGET_GAMEOBJECT_ITEM_TARGET)
                    {
                        if (GetSpellInfo()->SpellFamilyName == SPELLFAMILY_ROGUE)
                            skillValue += GetCaster()->GetLevel() * 5;
                        else
                            skillValue += m_spellInfo->Effects[effIndex].CalcValue();
                    }

                    if (skillValue < reqSkillValue)
                        return SPELL_FAILED_LOW_CASTLEVEL;
                }

                return SPELL_CAST_OK;
            }
            case LOCK_KEY_SPELL:
                if (m_spellInfo->Id == lockInfo->Index[j])
                    return SPELL_CAST_OK;

                reqKey = true;
                break;
        }
    }

    if (reqKey)
        return SPELL_FAILED_BAD_TARGETS;

    return SPELL_CAST_OK;
}

void Spell::SetContext(CustomSpellValues const& values)
{
    for (auto&& it : values)
        SetSpellValue(it.first, it.second);
    m_spellValue->HealingBonusDone = values.m_healingBonus;
    m_spellValue->Multiplier = values.m_multiplier;
}

void Spell::SetSpellValue(SpellValueMod mod, int32 value)
{
    switch (mod)
    {
        case SPELLVALUE_BASE_POINT0:
            m_spellValue->EffectBasePoints[0] = m_spellInfo->Effects[EFFECT_0].CalcBaseValue(value);
            break;
        case SPELLVALUE_BASE_POINT1:
            m_spellValue->EffectBasePoints[1] = m_spellInfo->Effects[EFFECT_1].CalcBaseValue(value);
            break;
        case SPELLVALUE_BASE_POINT2:
            m_spellValue->EffectBasePoints[2] = m_spellInfo->Effects[EFFECT_2].CalcBaseValue(value);
            break;
        case SPELLVALUE_RADIUS_MOD:
            m_spellValue->RadiusMod = (float)value / 10000;
            break;
        case SPELLVALUE_MAX_TARGETS:
            m_spellValue->MaxAffectedTargets = (uint32)value;
            break;
        case SPELLVALUE_AURA_STACK:
            m_spellValue->AuraStackAmount = uint8(value);
            break;
        case SPELLVALUE_SPELL_GO_GLAGS:
            m_spellValue->SpellGoFlags = SpellCastFlags(value);
            break;
    }
}

void Spell::FinishTargetProcessing()
{
    SendLogExecute();
}

void Spell::LoadScripts()
{
    if (!m_loadedScripts.empty())
        return;

    sScriptMgr->CreateSpellScripts(m_spellInfo->Id, m_loadedScripts);
    for (std::list<SpellScript*>::iterator itr = m_loadedScripts.begin(); itr != m_loadedScripts.end();)
    {
        if (!(*itr)->_Load(this))
        {
            std::list<SpellScript*>::iterator bitr = itr;
            ++itr;
            delete (*bitr);
            m_loadedScripts.erase(bitr);
            continue;
        }
        TC_LOG_DEBUG("spells", "Spell::LoadScripts: Script `%s` for spell `%u` is loaded now", (*itr)->_GetScriptName()->c_str(), m_spellInfo->Id);
        (*itr)->Register();
        ++itr;
    }
}

void Spell::CallScriptOnSpellStartHandlers()
{
    for (auto&& script : m_loadedScripts)
    {
        script->_PrepareScriptCall(SPELL_SCRIPT_HOOK_BEFORE_CAST);
        for (auto&& hook : script->OnSpellStart)
            hook.Call(script);

        script->_FinishScriptCall();
    }
}

void Spell::CallScriptBeforeCastHandlers()
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_BEFORE_CAST);
        std::list<SpellScript::CastHandler>::iterator hookItrEnd = (*scritr)->BeforeCast.end(), hookItr = (*scritr)->BeforeCast.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            (*hookItr).Call(*scritr);

        (*scritr)->_FinishScriptCall();
    }
}

void Spell::CallScriptOnCastHandlers()
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_ON_CAST);
        std::list<SpellScript::CastHandler>::iterator hookItrEnd = (*scritr)->OnCast.end(), hookItr = (*scritr)->OnCast.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            (*hookItr).Call(*scritr);

        (*scritr)->_FinishScriptCall();
    }
}

void Spell::CallScriptAfterCastHandlers()
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_AFTER_CAST);
        std::list<SpellScript::CastHandler>::iterator hookItrEnd = (*scritr)->AfterCast.end(), hookItr = (*scritr)->AfterCast.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            (*hookItr).Call(*scritr);

        (*scritr)->_FinishScriptCall();
    }
}

SpellCastResult Spell::CallScriptCheckCastHandlers()
{
    SpellCastResult retVal = SPELL_CAST_OK;
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_CHECK_CAST);
        std::list<SpellScript::CheckCastHandler>::iterator hookItrEnd = (*scritr)->OnCheckCast.end(), hookItr = (*scritr)->OnCheckCast.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
        {
            SpellCastResult tempResult = (*hookItr).Call(*scritr);
            if (retVal == SPELL_CAST_OK)
                retVal = tempResult;
        }

        (*scritr)->_FinishScriptCall();
    }
    return retVal;
}

SpellCastResult Spell::CallScriptCheckAutoCastHandlers()
{
    for (auto&& script : m_loadedScripts)
    {
        script->_PrepareScriptCall(SPELL_SCRIPT_HOOK_CHECK_CAST);
        for (auto&& hook : script->OnCheckAutoCast)
        {
            SpellCastResult result = hook.Call(script);
            if (result != SPELL_CAST_OK)
            {
                script->_FinishScriptCall();
                return result;
            }
        }
        script->_FinishScriptCall();
    }
    return SPELL_CAST_OK;
}


void Spell::PrepareScriptHitHandlers()
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
        (*scritr)->_InitHit();
}

bool Spell::CallScriptEffectHandlers(SpellEffIndex effIndex, SpellEffectHandleMode mode)
{
    // execute script effect handler hooks and check if effects was prevented
    bool preventDefault = false;
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        std::list<SpellScript::EffectHandler>::iterator effItr, effEndItr;
        SpellScriptHookType hookType;
        switch (mode)
        {
            case SPELL_EFFECT_HANDLE_LAUNCH:
                effItr = (*scritr)->OnEffectLaunch.begin();
                effEndItr = (*scritr)->OnEffectLaunch.end();
                hookType = SPELL_SCRIPT_HOOK_EFFECT_LAUNCH;
                break;
            case SPELL_EFFECT_HANDLE_LAUNCH_TARGET:
                effItr = (*scritr)->OnEffectLaunchTarget.begin();
                effEndItr = (*scritr)->OnEffectLaunchTarget.end();
                hookType = SPELL_SCRIPT_HOOK_EFFECT_LAUNCH_TARGET;
                break;
            case SPELL_EFFECT_HANDLE_HIT:
                effItr = (*scritr)->OnEffectHit.begin();
                effEndItr = (*scritr)->OnEffectHit.end();
                hookType = SPELL_SCRIPT_HOOK_EFFECT_HIT;
                break;
            case SPELL_EFFECT_HANDLE_HIT_TARGET:
                effItr = (*scritr)->OnEffectHitTarget.begin();
                effEndItr = (*scritr)->OnEffectHitTarget.end();
                hookType = SPELL_SCRIPT_HOOK_EFFECT_HIT_TARGET;
                break;
            default:
                ASSERT(false);
                return false;
        }
        (*scritr)->_PrepareScriptCall(hookType);
        for (; effItr != effEndItr; ++effItr)
            // effect execution can be prevented
            if (!(*scritr)->_IsEffectPrevented(effIndex) && (*effItr).IsEffectAffected(m_spellInfo, effIndex))
                (*effItr).Call(*scritr, effIndex);

        if (!preventDefault)
            preventDefault = (*scritr)->_IsDefaultEffectPrevented(effIndex);

        (*scritr)->_FinishScriptCall();
    }
    return preventDefault;
}

void Spell::CallScriptBeforeHitHandlers()
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_BEFORE_HIT);
        std::list<SpellScript::HitHandler>::iterator hookItrEnd = (*scritr)->BeforeHit.end(), hookItr = (*scritr)->BeforeHit.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            (*hookItr).Call(*scritr);

        (*scritr)->_FinishScriptCall();
    }
}

void Spell::CallScriptOnHitHandlers()
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_HIT);
        std::list<SpellScript::HitHandler>::iterator hookItrEnd = (*scritr)->OnHit.end(), hookItr = (*scritr)->OnHit.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            (*hookItr).Call(*scritr);

        (*scritr)->_FinishScriptCall();
    }
}

void Spell::CallScriptAfterHitHandlers()
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_AFTER_HIT);
        std::list<SpellScript::HitHandler>::iterator hookItrEnd = (*scritr)->AfterHit.end(), hookItr = (*scritr)->AfterHit.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            (*hookItr).Call(*scritr);

        (*scritr)->_FinishScriptCall();
    }
}

void Spell::CallScriptObjectAreaTargetSelectHandlers(std::list<WorldObject*>& targets, SpellEffIndex effIndex)
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_OBJECT_AREA_TARGET_SELECT);
        std::list<SpellScript::ObjectAreaTargetSelectHandler>::iterator hookItrEnd = (*scritr)->OnObjectAreaTargetSelect.end(), hookItr = (*scritr)->OnObjectAreaTargetSelect.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            if ((*hookItr).IsEffectAffected(m_spellInfo, effIndex))
                (*hookItr).Call(*scritr, targets);

        (*scritr)->_FinishScriptCall();
    }
}

void Spell::CallScriptObjectTargetSelectHandlers(WorldObject*& target, SpellEffIndex effIndex)
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_OBJECT_TARGET_SELECT);
        std::list<SpellScript::ObjectTargetSelectHandler>::iterator hookItrEnd = (*scritr)->OnObjectTargetSelect.end(), hookItr = (*scritr)->OnObjectTargetSelect.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            if ((*hookItr).IsEffectAffected(m_spellInfo, effIndex))
                (*hookItr).Call(*scritr, target);

        (*scritr)->_FinishScriptCall();
    }
}

bool Spell::CheckScriptEffectImplicitTargets(uint32 effIndex, uint32 effIndexToCheck)
{
    // Skip if there are not any script
    if (!m_loadedScripts.size())
        return true;

    for (std::list<SpellScript*>::iterator itr = m_loadedScripts.begin(); itr != m_loadedScripts.end(); ++itr)
    {
        std::list<SpellScript::ObjectTargetSelectHandler>::iterator targetSelectHookEnd = (*itr)->OnObjectTargetSelect.end(), targetSelectHookItr = (*itr)->OnObjectTargetSelect.begin();
        for (; targetSelectHookItr != targetSelectHookEnd; ++targetSelectHookItr)
            if (((*targetSelectHookItr).IsEffectAffected(m_spellInfo, effIndex) && !(*targetSelectHookItr).IsEffectAffected(m_spellInfo, effIndexToCheck)) ||
                (!(*targetSelectHookItr).IsEffectAffected(m_spellInfo, effIndex) && (*targetSelectHookItr).IsEffectAffected(m_spellInfo, effIndexToCheck)))
                return false;

        std::list<SpellScript::ObjectAreaTargetSelectHandler>::iterator areaTargetSelectHookEnd = (*itr)->OnObjectAreaTargetSelect.end(), areaTargetSelectHookItr = (*itr)->OnObjectAreaTargetSelect.begin();
        for (; areaTargetSelectHookItr != areaTargetSelectHookEnd; ++areaTargetSelectHookItr)
            if (((*areaTargetSelectHookItr).IsEffectAffected(m_spellInfo, effIndex) && !(*areaTargetSelectHookItr).IsEffectAffected(m_spellInfo, effIndexToCheck)) ||
                (!(*areaTargetSelectHookItr).IsEffectAffected(m_spellInfo, effIndex) && (*areaTargetSelectHookItr).IsEffectAffected(m_spellInfo, effIndexToCheck)))
                return false;
    }
    return true;
}

void Spell::CallScriptDestinationTargetSelectHandlers(SpellDestination& target, SpellEffIndex effIndex)
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_DESTINATION_TARGET_SELECT);
        std::list<SpellScript::DestinationTargetSelectHandler>::iterator hookItrEnd = (*scritr)->OnDestinationTargetSelect.end(), hookItr = (*scritr)->OnDestinationTargetSelect.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            if (hookItr->IsEffectAffected(m_spellInfo, effIndex))
                hookItr->Call(*scritr, target);

        (*scritr)->_FinishScriptCall();
    }
}

bool Spell::CanExecuteTriggersOnHit(uint32 effMask, SpellInfo const* triggeredByAura) const
{
    bool only_on_caster = (triggeredByAura && (triggeredByAura->AttributesEx4 & SPELL_ATTR4_PROC_ONLY_ON_CASTER));
    // If triggeredByAura has SPELL_ATTR4_PROC_ONLY_ON_CASTER then it can only proc on a casted spell with TARGET_UNIT_CASTER
    for (uint8 i = 0;i < MAX_SPELL_EFFECTS; ++i)
    {
        if ((effMask & (1 << i)) && (!only_on_caster || (m_spellInfo->Effects[i].TargetA.GetTarget() == TARGET_UNIT_CASTER)))
            return true;
    }
    return false;
}

void Spell::PrepareTriggersExecutedOnHit()
{
    /// @todo move this to scripts
    if (m_spellInfo->SpellFamilyName)
    {
        SpellInfo const* excludeTargetSpellInfo = sSpellMgr->GetSpellInfo(m_spellInfo->ExcludeTargetAuraSpell);
        if (excludeTargetSpellInfo && !excludeTargetSpellInfo->IsPositive())
            m_preCastSpell = m_spellInfo->ExcludeTargetAuraSpell;
    }

    /// @todo move this to scripts
    switch (m_spellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_MAGE:
        {
             // Permafrost
             if (m_spellInfo->SpellFamilyFlags[1] & 0x00001000 ||  m_spellInfo->SpellFamilyFlags[0] & 0x00100220)
                 m_preCastSpell = 68391;
             break;
        }
    }

    // handle SPELL_AURA_ADD_TARGET_TRIGGER auras:
    // save auras which were present on spell caster on cast, to prevent triggered auras from affecting caster
    // and to correctly calculate proc chance when combopoints are present
    Unit::AuraEffectList const& targetTriggers = m_caster->GetAuraEffectsByType(SPELL_AURA_ADD_TARGET_TRIGGER);
    for (Unit::AuraEffectList::const_iterator i = targetTriggers.begin(); i != targetTriggers.end(); ++i)
    {
        if (!(*i)->IsAffectingSpell(m_spellInfo))
            continue;
        if (m_spellInfo->Id == 137576) // Interrupt from Deadly Throw, same class mask as Deadly Throw, don't do trigger twice. Probably need just to check m_cast_count
            continue;

        SpellInfo const* auraSpellInfo = (*i)->GetSpellInfo();
        uint32 auraSpellIdx = (*i)->GetEffIndex();
        if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(auraSpellInfo->Effects[auraSpellIdx].TriggerSpell))
        {
            // calculate the chance using spell base amount, because aura amount is not updated on combo-points change
            // this possibly needs fixing
            int32 auraBaseAmount = (*i)->GetBaseAmount();
            // proc chance is stored in effect amount
            int32 chance = m_caster->CalculateSpellDamage(NULL, auraSpellInfo, auraSpellIdx, &auraBaseAmount);
            // build trigger and add to the list
            HitTriggerSpell spellTriggerInfo;
            spellTriggerInfo.triggeredSpell = spellInfo;
            spellTriggerInfo.triggeredByAura = auraSpellInfo;
            spellTriggerInfo.chance = chance * (*i)->GetBase()->GetStackAmount();
            m_hitTriggerSpells.push_back(spellTriggerInfo);
        }
    }
}

void Spell::ApplySmartHeal(Unit const* caster, std::list<WorldObject*>& targets, uint32 count, bool ignoreLos, Position const* losPos)
{
    if (!losPos)
        losPos = caster;

    // Remove non units and spirit of redemption
    targets.remove_if([](WorldObject const* target)
    {
        return !target->ToUnit() || target->ToUnit()->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION);
    });

    targets.remove_if([](WorldObject const* target)
    {
        return target->ToUnit()->IsSummon() && target->ToUnit()->ToTempSummon()->IsMinorGuardian();
    });

    // Don't heal targets out of raid if they have different pvp state
    targets.remove_if([=](WorldObject const* target)
    {
        Unit const* unit = target->ToUnit();
        if (!ignoreLos && !unit->IsWithinLOS(losPos->GetPositionX(), losPos->GetPositionY(), losPos->GetPositionZ()))
            return true;

        if (unit->GetMap()->IsDungeon())
            return false;

        if (unit->IsInRaidWith(caster))
            return false;

        if (unit->IsPvP() && !caster->IsPvP())
            return true;
        if (!unit->IsPvP() && caster->IsPvP())
            return true;
        if (unit->IsInCombat() && !caster->IsInCombat())
            return true;
        if (!unit->IsInCombat() && caster->IsInCombat())
            return true;
        return false;
    });

    // Sort by health and players always are first
    targets.sort([=](WorldObject const* lhs, WorldObject const* rhs)
    {
        Unit const* l = lhs->ToUnit();
        Unit const* r = rhs->ToUnit();

        return l->GetHealthPct() < r->GetHealthPct();
    });

    if (targets.size() > count)
    {
        std::list<WorldObject*> selected;

        std::function<bool(Unit*)> playersUnder50 = [](Unit* target) { return target->GetTypeId() == TYPEID_PLAYER && target->GetHealthPct() < 50.0f; };
        std::function<bool(Unit*)> playersUnder75 = [](Unit* target) { return target->GetTypeId() == TYPEID_PLAYER && target->GetHealthPct() < 75.0f; };
        std::function<bool(Unit*)> injuredPlayers = [](Unit* target) { return target->GetTypeId() == TYPEID_PLAYER && !target->IsFullHealth(); };
        std::function<bool(Unit*)> allUnder25 = [](Unit* target) { return target->ToUnit()->GetHealthPct() < 25.0f; };
        std::function<bool(Unit*)> allUnder50 = [](Unit* target) { return target->ToUnit()->GetHealthPct() < 50.0f; };
        std::function<bool(Unit*)> anyInjured = [](Unit* target) { return !target->ToUnit()->IsFullHealth(); };

        for (auto&& fn : { playersUnder50, allUnder25, playersUnder75, allUnder50, injuredPlayers, anyInjured })
        {
            for (auto itr = targets.begin(); itr != targets.end() && selected.size() < count;)
            {
                Unit* unit = (*itr)->ToUnit();
                if (fn(unit))
                {
                    selected.push_back(unit);
                    itr = targets.erase(itr);
                }
                else
                    ++itr;
            }
        }

        // Full hp targets just to fit number
        for (auto itr = targets.begin(); itr != targets.end() && selected.size() < count; ++itr)
            selected.push_back(*itr);

        targets.swap(selected);
    }
}

// Global cooldowns management
enum GCDLimits
{
    MIN_GCD = 1000,
    MAX_GCD = 1500
};

bool Spell::HasGlobalCooldown() const
{
    // Only player or controlled units have global cooldown
    if (m_caster->GetCharmInfo())
        return m_caster->GetCharmInfo()->GetGlobalCooldownMgr().HasGlobalCooldown(m_spellInfo);
    else if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        for (auto&& eff : m_caster->GetAuraEffectsByType(SPELL_AURA_ADD_PCT_MODIFIER))
            if (eff->GetMiscValue() == SPELLMOD_GLOBAL_COOLDOWN && eff->IsAffectingSpell(GetSpellInfo()))
                if (eff->GetAmount() == -100)
                    return false;
        return m_caster->ToPlayer()->GetGlobalCooldownMgr().HasGlobalCooldown(m_spellInfo);
    }
    else
        return false;
}

void Spell::TriggerGlobalCooldown()
{
    int32 gcd = m_spellInfo->StartRecoveryTime;
    if (!gcd)
        return;

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        if (m_caster->ToPlayer()->GetCommandStatus(CHEAT_COOLDOWN))
            return;

    // Global cooldown can't leave range 1..1.5 secs
    // There are some spells (mostly not casted directly by player) that have < 1 sec and > 1.5 sec global cooldowns
    // but as tests show are not affected by any spell mods.
    if (m_spellInfo->StartRecoveryTime >= MIN_GCD && m_spellInfo->StartRecoveryTime <= MAX_GCD)
    {
        // It seems that there are no lower limit for gcd.
        // Adrenaline Rush (13750) must reduce gcd below 1s. But let's allow it only for spell mods.

        // apply haste rating (check ripped straight from the client)
        if (m_spellInfo->StartRecoveryCategory == 133 &&
            m_spellInfo->StartRecoveryTime == 1500 &&
            m_spellInfo->DmgClass != SPELL_DAMAGE_CLASS_MELEE &&
            m_spellInfo->DmgClass != SPELL_DAMAGE_CLASS_RANGED &&
            !(m_spellInfo->Attributes & (SPELL_ATTR0_REQ_AMMO | SPELL_ATTR0_ABILITY)))
            gcd = int32(m_caster->GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED) * 1500.0f);

        for (auto&& itr : GetCaster()->GetAuraEffectsByType(SPELL_AURA_MOD_GLOBAL_COOLDOWN_BY_HASTE))
        {
            if (itr->IsAffectingSpell(GetSpellInfo()))
            {
                float mult = GetCaster()->GetHasteMultiplier();
                float hastepct = (1.0f - CalculatePct(1.0f / mult, itr->GetAmount())) * 100.0f;
                AddPct(gcd, -hastepct);
            }
        }

        if (gcd < MIN_GCD)
            gcd = MIN_GCD;
        else if (gcd > MAX_GCD)
            gcd = MAX_GCD;

        // gcd modifier auras are applied only to own spells and only players have such mods
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
            m_caster->ToPlayer()->ApplySpellMod(m_spellInfo->Id, SPELLMOD_GLOBAL_COOLDOWN, gcd, this);
    }

    // Only players or controlled units have global cooldown
    if (m_caster->GetCharmInfo())
        m_caster->GetCharmInfo()->GetGlobalCooldownMgr().AddGlobalCooldown(m_spellInfo, gcd);
    else if (m_caster->GetTypeId() == TYPEID_PLAYER)
        m_caster->ToPlayer()->GetGlobalCooldownMgr().AddGlobalCooldown(m_spellInfo, gcd);
}

void Spell::CancelGlobalCooldown()
{
    if (!m_spellInfo->StartRecoveryTime)
        return;

    // Cancel global cooldown when interrupting current cast
    if (m_caster->GetCurrentSpell(CURRENT_GENERIC_SPELL) != this)
        return;

    // Only players or controlled units have global cooldown
    if (m_caster->GetCharmInfo())
        m_caster->GetCharmInfo()->GetGlobalCooldownMgr().CancelGlobalCooldown(m_spellInfo);
    else if (m_caster->GetTypeId() == TYPEID_PLAYER)
        m_caster->ToPlayer()->GetGlobalCooldownMgr().CancelGlobalCooldown(m_spellInfo);
}

namespace Trinity
{

WorldObjectSpellTargetCheck::WorldObjectSpellTargetCheck(Unit* caster, Unit* referer, SpellInfo const* spellInfo,
    SpellTargetSelectionCategories category, SpellTargetCheckTypes selectionType, ConditionContainer* condList) : _caster(caster), _referer(referer), _spellInfo(spellInfo),
    _targetSelectionType(selectionType), _condList(condList), _category(category)
{
    if (condList)
        _condSrcInfo = new ConditionSourceInfo(NULL, caster);
    else
        _condSrcInfo = NULL;
}

WorldObjectSpellTargetCheck::~WorldObjectSpellTargetCheck()
{
    if (_condSrcInfo)
        delete _condSrcInfo;
}

bool WorldObjectSpellTargetCheck::operator()(WorldObject* target)
{
    if (_spellInfo->CheckTarget(_caster, target, true) != SPELL_CAST_OK)
        return false;
    Unit* unitTarget = target->ToUnit();
    if (Corpse* corpseTarget = target->ToCorpse())
    {
        // use ofter for party/assistance checks
        if (Player* owner = ObjectAccessor::FindPlayer(corpseTarget->GetOwnerGUID()))
            unitTarget = owner;
        else
            return false;
    }

    if (unitTarget)
    {
        // NOTE: I think there are not spells with TARGET_CHECK_ENRY that can target totems
        if (_category == TARGET_SELECT_CATEGORY_AREA || _category == TARGET_SELECT_CATEGORY_CONE)
            if (unitTarget->IsTotem() || unitTarget->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION) || (unitTarget->IsSummon() && unitTarget->ToTempSummon()->IsWarBanner()))
                return false;

        switch (_targetSelectionType)
        {
            case TARGET_CHECK_ENEMY:
                if (unitTarget->IsTotem())
                    return false;
                if (!_caster->IsValidAttackTarget(unitTarget, _spellInfo, nullptr, target->GetTypeId() == TYPEID_CORPSE))
                    return false;
                break;
            case TARGET_CHECK_ALLY:
                if (unitTarget->IsTotem())
                    return false;
                if (!_caster->_IsValidAssistTarget(unitTarget, _spellInfo))
                    return false;
                break;
            case TARGET_CHECK_PARTY:
                if (unitTarget->IsTotem())
                    return false;
                if (!_caster->_IsValidAssistTarget(unitTarget, _spellInfo))
                    return false;
                if (!_referer->IsInPartyWith(unitTarget))
                    return false;
                if (unitTarget->IsPetGuardianStuff() && (_spellInfo->AttributesEx7 & SPELL_ATTR7_CONSOLIDATED_RAID_BUFF || _spellInfo->AttributesEx9 & SPELL_ATTR9_NOT_USABLE_IN_ARENA))
                    return false;
                break;
            case TARGET_CHECK_RAID_CLASS:
                if (_referer->GetClass() != unitTarget->GetClass())
                    return false;
                // nobreak;
            case TARGET_CHECK_RAID:
            {
                if (unitTarget->IsTotem())
                    return false;

                bool dispelCharm = false;
                for (auto&& effect : _spellInfo->Effects)
                    if (effect.Effect == SPELL_EFFECT_DISPEL_MECHANIC && effect.MiscValue == MECHANIC_CHARM)
                        dispelCharm = true;

                if (!dispelCharm && !_caster->_IsValidAssistTarget(unitTarget, _spellInfo) && target->GetTypeId() != TYPEID_CORPSE) // Mass Resurrection
                    return false;
                if (!_referer->IsInRaidWith(unitTarget, dispelCharm))
                    return false;
                if (unitTarget->IsPetGuardianStuff() && (_spellInfo->AttributesEx7 & SPELL_ATTR7_CONSOLIDATED_RAID_BUFF || _spellInfo->AttributesEx9 & SPELL_ATTR9_NOT_USABLE_IN_ARENA))
                    return false;
                break;
            }
            default:
                break;
        }
    }
    if (!_condSrcInfo)
        return true;
    _condSrcInfo->mConditionTargets[0] = target;
    return sConditionMgr->IsObjectMeetToConditions(*_condSrcInfo, *_condList);
}

WorldObjectSpellNearbyTargetCheck::WorldObjectSpellNearbyTargetCheck(float range, Unit* caster, SpellInfo const* spellInfo,
    SpellTargetSelectionCategories category, SpellTargetCheckTypes selectionType, ConditionContainer* condList)
    : WorldObjectSpellTargetCheck(caster, caster, spellInfo, category, selectionType, condList), _range(range), _caster(caster) { }

bool WorldObjectSpellNearbyTargetCheck::operator()(WorldObject* target)
{
    float dist = target->GetDistance(_caster);
    if (dist > _range)
    {
        if (!(target->ToGameObject() && target->ToGameObject()->IsAtInteractDistance(*_caster, _range)))
            return false;
    }
    if (WorldObjectSpellTargetCheck::operator()(target))
    {
        _range = dist;
        return true;
    }
    return false;
}

WorldObjectSpellAreaTargetCheck::WorldObjectSpellAreaTargetCheck(float range, Position const* position, Unit* caster,
    Unit* referer, SpellInfo const* spellInfo, SpellTargetSelectionCategories category, SpellTargetCheckTypes selectionType, ConditionContainer* condList)
    : WorldObjectSpellTargetCheck(caster, referer, spellInfo, category, selectionType, condList), _range(range), _position(position) { }

bool WorldObjectSpellAreaTargetCheck::operator()(WorldObject* target)
{
    if (!target->IsWithinDist3d(_position, _range) && !(target->ToGameObject() && target->ToGameObject()->IsAtInteractDistance(*_position, _range)))
        return false;
    return WorldObjectSpellTargetCheck::operator ()(target);
}

WorldObjectSpellConeTargetCheck::WorldObjectSpellConeTargetCheck(float coneAngle, float coneOffset, float range, Unit* caster,
    SpellInfo const* spellInfo, SpellTargetSelectionCategories category, SpellTargetCheckTypes selectionType, ConditionContainer* condList)
    : WorldObjectSpellAreaTargetCheck(range, caster, caster, caster, spellInfo, category, selectionType, condList), _coneAngle(coneAngle), _coneOffset(coneOffset) { }

bool WorldObjectSpellConeTargetCheck::operator()(WorldObject* target)
{
    float originalOrientation = _caster->GetOrientation();
    if (_coneOffset)
        _caster->SetOrientation(originalOrientation+_coneOffset);

    if (_coneAngle < 0)
    {
        if (!_caster->isInBack(target, -_coneAngle))
        {
            if (_coneOffset)
                _caster->SetOrientation(originalOrientation);
            return false;
        }
    }
    else if (_spellInfo->AttributesCu & SPELL_ATTR0_CU_CONE_LINE)
    {
        if (!_caster->HasInLine(target, _caster->GetObjectSize()))
        {
            if (_coneOffset)
                _caster->SetOrientation(originalOrientation);
            return false;
        }
    }
    else
    {
        if (!_caster->isInFront(target, _coneAngle))
        {
            if (_caster->GetTypeId() != TYPEID_PLAYER || _caster->GetDistance2d(target) > 3.0f || !_caster->isInFront(target, M_PI))
            {
                if (_coneOffset)
                    _caster->SetOrientation(originalOrientation);
                return false;
            }
        }
    }
    if (_coneOffset)
        _caster->SetOrientation(originalOrientation);
    return WorldObjectSpellAreaTargetCheck::operator ()(target);
}

WorldObjectSpellTrajTargetCheck::WorldObjectSpellTrajTargetCheck(float range, Position const* position, Unit* caster, SpellInfo const* spellInfo)
    : WorldObjectSpellAreaTargetCheck(range, position, caster, caster, spellInfo, TARGET_SELECT_CATEGORY_DEFAULT, TARGET_CHECK_DEFAULT, NULL) { }

bool WorldObjectSpellTrajTargetCheck::operator()(WorldObject* target)
{
    // return all targets on missile trajectory (0 - size of a missile)
    if (!_caster->HasInLine(target, 0))
        return false;
    return WorldObjectSpellAreaTargetCheck::operator ()(target);
}

} //namespace Trinity

void MissileData::Initialize(Spell const* spell, float dist)
{
    m_spellId = spell->GetSpellInfo()->Id;
    m_currentTime = spell->GetSpellGoTime();
    m_srcPos = *spell->GetCaster();
    m_destPos = *spell->m_targets.GetDstPos();
    m_calculatedDist = m_destPos.GetExactDist(&m_srcPos);
    m_currentDist = 0.f;
    m_totalDist = dist;
    m_timer = spell->GetDelayMoment();
    m_speed = spell->GetSpellInfo()->Speed;
    m_currentPos = m_srcPos;
}

void MissileData::Update(uint32 currentTime, Unit* caster, AuraEffect const * eff)
{
    m_prevPos = m_currentPos;
    m_prevTime = m_currentTime;

    m_currentTime = getMSTime();

    // Update caster position. It is very roughly, but better then nothing
    if (m_reversed)
    {
        m_destPos = *caster;
        m_timer = m_currentPos.GetExactDist(&m_destPos) / m_speed * 1000.f;
    }

    if (m_done && !m_reversed)
    {
        m_srcPos = m_currentPos;   // Last hit pos
        m_destPos = *caster;
        m_calculatedDist = m_destPos.GetExactDist(&m_srcPos);
        m_timer = m_calculatedDist / m_speed * 1000.f;  // Yes we must do it before m_timer -= diff because SMSG_MISSILE_CANCEL was sent on previous tick
        m_currentDist = 0.0f;
        m_affectedTargets.clear();
        m_reversed = true;
        m_done = false;
    }

    uint32 diff = m_currentTime - m_prevTime;
    if (m_timer > diff)
        m_timer -= diff;
    else
        m_timer = 0;

    float angle = m_currentPos.GetAngle(&m_destPos);
    float dist = diff * m_speed / 1000.f;
    m_currentDist += dist;
    m_currentPos.m_positionX += dist * std::cos(angle);
    m_currentPos.m_positionY += dist * std::sin(angle);
    // Just raw approximating ¯\_(ツ)_/¯
    m_currentPos.m_positionZ += dist / m_calculatedDist * (m_destPos.GetPositionZ() - m_srcPos.GetPositionZ());

    if (m_reversed)
    {
        if (!m_timer)
        {
            caster->SendMissileCancel(m_spellId, false);    // I dunno why it is needed, but sniffs has it.
            eff->GetBase()->SetDuration(0);
        }
    }
    else
    {
        // Our m_calculatedDist is slightly bigger than m_totalDist.
        // On client, missile returning is possible only if missile doesn't reach the destination.
        // We must be sure that server will be in time to send packet even if it lags.
        // So, m_timer is bigger than needed and we need to check current distance (destination is static, it can't move).
        // m_timer < uint32(eff->GetAmplitude() is just to be sure that we have enought time to next tick (I think it is generally useless but I'm not sure).
        if (m_currentDist >= m_totalDist || m_timer < uint32(eff->GetAmplitude()))
        {
            m_done = true;
            caster->SendMissileCancel(m_spellId, true);
        }
    }
}

bool MissileData::CheckTarget(WorldObject const* target, float dist)
{
    if (m_affectedTargets.find(target->GetGUID()) != m_affectedTargets.end())
        return false;
    // TODO: Still not perfect
    if (!target->IsInBetween(&m_prevPos, &m_currentPos, 4.f))
        return false;

    if (std::abs(target->GetPositionZ() - m_currentPos.GetPositionZ()) > 4.f) // Check Z
        return false;

    m_affectedTargets.insert(target->GetGUID());
    return true;
}
