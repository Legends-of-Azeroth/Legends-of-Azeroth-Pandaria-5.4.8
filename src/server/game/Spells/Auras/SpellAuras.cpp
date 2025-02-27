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

#include "Common.h"
#include "WorldPacket.h"
#include "Opcodes.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Player.h"
#include "Unit.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "ObjectAccessor.h"
#include "Util.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "Vehicle.h"
#include "SpellHistory.h"

AuraApplication::AuraApplication(Unit* target, Unit* caster, Aura* aura, uint32 effMask):
_target(target), _base(aura), _removeMode(AURA_REMOVE_NONE), _slot(MAX_AURAS),
_flags(AFLAG_NONE), _effectsToApply(effMask), _needClientUpdate(false), _effMask(0)
{
    ASSERT(GetTarget() && GetBase());

    if (GetBase()->CanBeSentToClient(target))
    {
        // Try find slot for aura
        uint8 slot = MAX_AURAS;
        // Lookup for auras already applied from spell
        if (AuraApplication * foundAura = GetTarget()->GetAuraApplication(GetBase()->GetId(), GetBase()->GetCasterGUID(), GetBase()->GetCastItemGUID()))
        {
            // allow use single slot only by auras from same caster
            slot = foundAura->GetSlot();
        }
        else
        {
            Unit::VisibleAuraMap const* visibleAuras = GetTarget()->GetVisibleAuras();
            // lookup for free slots in units visibleAuras
            Unit::VisibleAuraMap::const_iterator itr = visibleAuras->find(0);
            for (uint32 freeSlot = 0; freeSlot < MAX_AURAS; ++itr, ++freeSlot)
            {
                if (itr == visibleAuras->end() || itr->first != freeSlot)
                {
                    slot = freeSlot;
                    break;
                }
            }
        }

        // Register Visible Aura
        if (slot < MAX_AURAS)
        {
            _slot = slot;
            GetTarget()->SetVisibleAura(slot, this);
            SetNeedClientUpdate();
            TC_LOG_DEBUG("spells", "Aura: %u Effect: %d put to unit visible auras slot: %u", GetBase()->GetId(), GetEffectMask(), slot);
        }
        else
            TC_LOG_DEBUG("spells", "Aura: %u Effect: %d could not find empty unit visible slot", GetBase()->GetId(), GetEffectMask());
    }

    _InitFlags(caster, effMask);
}

void AuraApplication::_Remove()
{
    uint8 slot = GetSlot();

    if (slot >= MAX_AURAS)
        return;

    if (AuraApplication * foundAura = _target->GetAuraApplication(GetBase()->GetId(), GetBase()->GetCasterGUID(), GetBase()->GetCastItemGUID()))
    {
        // Reuse visible aura slot by aura which is still applied - prevent storing dead pointers
        if (slot == foundAura->GetSlot())
        {
            if (GetTarget()->GetVisibleAura(slot) == this)
            {
                GetTarget()->SetVisibleAura(slot, foundAura);
                foundAura->SetNeedClientUpdate();
            }
            // set not valid slot for aura - prevent removing other visible aura
            slot = MAX_AURAS;
        }
    }

    // update for out of range group members
    if (slot < MAX_AURAS)
    {
        GetTarget()->RemoveVisibleAura(slot);
        ClientUpdate(true);
    }
}

void AuraApplication::_InitFlags(Unit* caster, uint32 effMask)
{
    // mark as selfcasted if needed
    _flags |= (GetBase()->GetCasterGUID() == GetTarget()->GetGUID()) ? AFLAG_CASTER : AFLAG_NONE;

    // aura is casted by self or an enemy
    // one negative effect and we know aura is negative
    if (IsSelfcasted() || !caster || !caster->IsFriendlyTo(GetTarget()))
    {
        bool negativeFound = false;
        for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (((1<<i) & effMask) && !GetBase()->GetSpellInfo()->IsPositiveEffect(i))
            {
                negativeFound = true;
                break;
            }
        }
        _flags |= negativeFound ? AFLAG_NEGATIVE : AFLAG_POSITIVE;
    }
    // aura is casted by friend
    // one positive effect and we know aura is positive
    else
    {
        bool positiveFound = false;
        for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (((1<<i) & effMask) && GetBase()->GetSpellInfo()->IsPositiveEffect(i))
            {
                positiveFound = true;
                break;
            }
        }
        _flags |= positiveFound ? AFLAG_POSITIVE : AFLAG_NEGATIVE;
    }

    if (GetBase()->GetSpellInfo()->AttributesEx8 & SPELL_ATTR8_AURA_SEND_AMOUNT)
        _flags |= AFLAG_ANY_EFFECT_AMOUNT_SENT;
}

void AuraApplication::_HandleEffect(uint8 effIndex, bool apply)
{
    AuraEffect* aurEff = GetBase()->GetEffect(effIndex);
    ASSERT(aurEff);
    ASSERT(HasEffect(effIndex) == (!apply));
    ASSERT((1<<effIndex) & _effectsToApply);
    TC_LOG_DEBUG("spells", "AuraApplication::_HandleEffect: %u, apply: %u: amount: %u", aurEff->GetAuraType(), apply, aurEff->GetAmount());

    if (apply)
    {
        ASSERT(!(_effMask & (1 << effIndex)));
        _effMask |= 1 << effIndex;
        aurEff->HandleEffect(this, AURA_EFFECT_HANDLE_REAL, true);
    }
    else
    {
        ASSERT(_effMask & (1 << effIndex));
        _effMask &= ~( 1<< effIndex);
        aurEff->HandleEffect(this, AURA_EFFECT_HANDLE_REAL, false);

        // Remove all triggered by aura spells vs unlimited duration
        aurEff->CleanupTriggeredSpells(GetTarget());
    }
    SetNeedClientUpdate();
}

void AuraApplication::ClientUpdate(bool remove)
{
    _needClientUpdate = false;

    ObjectGuid targetGuid = _target->GetGUID();
    Aura const* aura = GetBase();
    uint32 flags = _flags;

    if (aura->GetMaxDuration() > 0 && !(aura->GetSpellInfo()->AttributesEx5 & SPELL_ATTR5_HIDE_DURATION))
        flags |= AFLAG_DURATION;

    WorldPacket data(SMSG_AURA_UPDATE);
    data.WriteBit(targetGuid[7]);
    data.WriteBit(0);                                   // Is AURA_UPDATE_ALL
    data.WriteBits(1, 24);                              // Aura Count
    data.WriteBit(targetGuid[6]);
    data.WriteBit(targetGuid[1]);
    data.WriteBit(targetGuid[3]);
    data.WriteBit(targetGuid[0]);
    data.WriteBit(targetGuid[4]);
    data.WriteBit(targetGuid[2]);
    data.WriteBit(targetGuid[5]);
    data.WriteBit(!remove);                             // Not remove

    uint8 effCount = 0;
    if (!remove)
    {
        if (flags & AFLAG_ANY_EFFECT_AMOUNT_SENT)
            for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                if ((GetEffectMask() & (1 << i)) && aura->HasEffect(i))
                    effCount = i + 1;

        data.WriteBits(effCount, 22);                      // Effect Count

        data.WriteBit(!(flags & AFLAG_CASTER));         // HasCasterGuid

        if (!(flags & AFLAG_CASTER))
        {
            ObjectGuid casterGuid = aura->GetCasterGUID();
            data.WriteBit(casterGuid[3]);
            data.WriteBit(casterGuid[4]);
            data.WriteBit(casterGuid[6]);
            data.WriteBit(casterGuid[1]);
            data.WriteBit(casterGuid[5]);
            data.WriteBit(casterGuid[2]);
            data.WriteBit(casterGuid[0]);
            data.WriteBit(casterGuid[7]);
        }

        data.WriteBits(0, 22);                          // Unk effect count
        data.WriteBit(flags & AFLAG_DURATION);          // HasDuration
        data.WriteBit(flags & AFLAG_DURATION);          // HasMaxDuration
    }

    data.FlushBits();

    if (!remove)
    {
        if (!(flags & AFLAG_CASTER))
        {
            ObjectGuid casterGuid = aura->GetCasterGUID();
            data.WriteByteSeq(casterGuid[3]);
            data.WriteByteSeq(casterGuid[2]);
            data.WriteByteSeq(casterGuid[1]);
            data.WriteByteSeq(casterGuid[6]);
            data.WriteByteSeq(casterGuid[4]);
            data.WriteByteSeq(casterGuid[0]);
            data.WriteByteSeq(casterGuid[5]);
            data.WriteByteSeq(casterGuid[7]);
        }

        data << uint8(flags);

        uint32 itemLevel = 0;
        if (ObjectGuid itemGUID = aura->GetCastItemGUID())
            if (Unit* caster = aura->GetCaster())
                if (Player* player = caster->ToPlayer())
                    if (Item* castItem = player->GetItemByGuid(itemGUID))
                        itemLevel = player->GetItemLevel(castItem);

        data << uint16(aura->GetCastItemGUID() ? itemLevel : aura->GetCasterLevel());
        data << uint32(aura->GetId());

        if (flags & AFLAG_DURATION)
            data << uint32(aura->GetMaxDuration());

        if (flags & AFLAG_DURATION)
            data << uint32(aura->GetDuration());

        // send stack amount for aura which could be stacked (never 0 - causes incorrect display) or charges
        // stack amount has priority over charges (checked on retail with spell 50262)
        // I dunno wtf is this ^ but we have few spells with stack amount 1 and charges > 1 and it must show charges count and not 1
        data << uint8(aura->GetSpellInfo()->StackAmount > 1 ? aura->GetStackAmount() : aura->GetCharges());
        data << uint32(GetEffectMask());

        if (flags & AFLAG_ANY_EFFECT_AMOUNT_SENT)
        {
            for (uint32 i = 0; i < effCount; ++i)
            {
                float amount = 0.0f;
                if (GetEffectMask() & (1 << i))
                    if (AuraEffect const* eff = aura->GetEffect(i))
                        amount = eff->GetFloatAmount();
                data << float(amount);
            }
        }
    }

    data << uint8(GetSlot());

    data.WriteByteSeq(targetGuid[2]);
    data.WriteByteSeq(targetGuid[6]);
    data.WriteByteSeq(targetGuid[7]);
    data.WriteByteSeq(targetGuid[1]);
    data.WriteByteSeq(targetGuid[3]);
    data.WriteByteSeq(targetGuid[4]);
    data.WriteByteSeq(targetGuid[0]);
    data.WriteByteSeq(targetGuid[5]);

    _target->SendMessageToSet(&data, true);

    if (_target->GetTypeId() != TYPEID_PLAYER)
        return;

    if (aura->GetSpellInfo()->HasAttribute(SPELL_ATTR0_HIDDEN_CLIENTSIDE) || aura->GetSpellInfo()->HasAttribute(SPELL_ATTR1_DONT_DISPLAY_IN_AURA_BAR))
        return;
    if (IsPositive())
        return;

    std::vector<ControlInfo> controls;
    controls.reserve(MAX_SPELL_EFFECTS);

    std::function<void(uint32, uint32, uint8)> emplaceControl = [this, &controls](uint32 auraType, uint32 mechanic, uint8 index)
    {
        switch (auraType)
        {
            case SPELL_AURA_MOD_CONFUSE:
                switch (mechanic)
                {
                    case MECHANIC_DISORIENTED:  // 19503
                        controls.emplace_back(mechanic, CONTROL_TYPE_CONFUSE, index);
                        break;
                    case MECHANIC_POLYMORPH:    // 11641
                        controls.emplace_back(mechanic, CONTROL_TYPE_CONFUSE, index);
                        break;
                }
                break;
            case SPELL_AURA_MOD_CHARM: // 15859
                controls.emplace_back(MECHANIC_CHARM, CONTROL_TYPE_CHARM, index);
                break;
            case SPELL_AURA_MOD_FEAR: // 137143
                controls.emplace_back(MECHANIC_HORROR, CONTROL_TYPE_FEAR, index);
                break;
            case SPELL_AURA_MOD_FEAR_2:
                switch (mechanic)
                {
                    case MECHANIC_HORROR: // 6789
                        controls.emplace_back(mechanic, CONTROL_TYPE_FEAR, index);
                        break;
                    case MECHANIC_FEAR:   // 5246, 5484, 8122, 113792
                        controls.emplace_back(mechanic, CONTROL_TYPE_FEAR, index);
                }
                break;
            case SPELL_AURA_STRANGULATE:    // 108194
            case SPELL_AURA_MOD_STUN:
                switch (mechanic)
                {
                    case MECHANIC_SAPPED:   // 115078
                    case MECHANIC_FEAR:     // 146002
                    case MECHANIC_SLEEP:    // 8399, 19386, 115268, 126246
                    case MECHANIC_FREEZE:   // 3355
                    case MECHANIC_KNOCKOUT: // 1776
                    case MECHANIC_HORROR:   // 64044
                    case MECHANIC_BANISH:   // 71298, 134735
                        controls.emplace_back(mechanic, CONTROL_TYPE_LOST_CONTROL, index);
                        break;
                    case MECHANIC_STUN: // 1833, 7922, 30283, 75008, 56427, 115001, 132168
                        controls.emplace_back(mechanic, CONTROL_TYPE_STUN, index);
                        break;
                }
                break;
            case SPELL_AURA_MOD_ROOT:
                switch (mechanic)
                {
                    case MECHANIC_ROOT:  // 122, 15532, 29849, 33395, 63685, 96294, 119584, 136634
                        controls.emplace_back(mechanic, CONTROL_TYPE_ROOT, index);
                        break;
                    case MECHANIC_STUN:  // 70980
                        controls.emplace_back(mechanic, CONTROL_TYPE_ROOT, index);
                        break;
                    case MECHANIC_FEAR:  // 5246
                        controls.emplace_back(mechanic, CONTROL_TYPE_ROOT, index);
                        break;
                }
                break;
            case SPELL_AURA_MOD_SILENCE: // 28730, 47476, 55334, 137460
                controls.emplace_back(MECHANIC_SILENCE, CONTROL_TYPE_SILENCE, index);
                break;
            case SPELL_AURA_MOD_PACIFY_SILENCE:  // 71103
                controls.emplace_back(MECHANIC_POLYMORPH, CONTROL_TYPE_PACIFY_AND_SILENCE, index);
                break;
            case SPELL_AURA_MOD_DISARM: // 676, 64058, 117368, 142896
                controls.emplace_back(MECHANIC_DISARM, CONTROL_TYPE_DISARM, index);
                break;
            case SPELL_AURA_MOD_POSSESS: // 605
                controls.emplace_back(MECHANIC_CHARM, CONTROL_TYPE_POSSESS, index);
                break;
            case SPELL_AURA_MOD_PACIFY: // 1022
                controls.emplace_back(MECHANIC_IMMUNE_SHIELD, CONTROL_TYPE_PACIFY, index);
                break;
            default:
                break;
        }
    };

    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        auto const eff = aura->GetEffect(i);
        if (!eff)
            continue;

        emplaceControl(eff->GetAuraType(), aura->GetSpellInfo()->Mechanic, i);
        emplaceControl(eff->GetAuraType(), aura->GetSpellInfo()->GetEffectMechanic(i), i);
    }

    if (controls.empty())
        return;

    if (remove && aura->GetDuration() > 0)
    {
        ObjectGuid guid = _target->GetGUID();
        WorldPacket data(SMSG_REMOVE_LOSS_OF_CONTROL);
        data.WriteBit(guid[1]);
data.WriteBit(guid[3]);
data.WriteBit(guid[2]);
data.WriteBit(guid[5]);
data.WriteBit(guid[4]);
        data.WriteBits(controls[0].mechanic, 8);
        data.WriteBit(guid[7]);
data.WriteBit(guid[0]);
data.WriteBit(guid[6]);

        data.FlushBits();

        data.WriteByteSeq(guid[1]);
data.WriteByteSeq(guid[0]);
data.WriteByteSeq(guid[3]);
data.WriteByteSeq(guid[6]);
data.WriteByteSeq(guid[2]);
data.WriteByteSeq(guid[4]);
        data << uint32(aura->GetSpellInfo()->DmgClass); // not sure
        data.WriteByteSeq(guid[5]);
data.WriteByteSeq(guid[7]);
        _target->ToPlayer()->SendDirectMessage(&data);
    }
    else if (!remove)
    {
        WorldPacket data(SMSG_LOSS_OF_CONTROL_AURA_UPDATE);
        data.WriteBits(controls.size(), 22);
        for (auto&& control : controls)
        {
            data.WriteBits(control.controlType, 8);
            data.WriteBits(control.mechanic, 8);
        }
        for (auto&& control : controls)
        {
            data << uint8(control.effectIndex);
            data << uint8(GetSlot());
        }
        _target->ToPlayer()->SendDirectMessage(&data);
    }
}

uint32 Aura::BuildEffectMaskForOwner(SpellInfo const* spellProto, uint32 avalibleEffectMask, WorldObject* owner)
{
    ASSERT(spellProto);
    ASSERT(owner);
    uint32 effMask = 0;
    switch (owner->GetTypeId())
    {
        case TYPEID_UNIT:
        case TYPEID_PLAYER:
            for (uint32 i = 0; i< MAX_SPELL_EFFECTS; ++i)
            {
                if (spellProto->Effects[i].IsUnitOwnedAuraEffect())
                    effMask |= 1 << i;
            }
            break;
        case TYPEID_DYNAMICOBJECT:
            for (uint32 i = 0; i< MAX_SPELL_EFFECTS; ++i)
            {
                if (spellProto->Effects[i].Effect == SPELL_EFFECT_PERSISTENT_AREA_AURA)
                    effMask |= 1 << i;
            }
            break;
        default:
            break;
    }
    return effMask & avalibleEffectMask;
}

Aura* Aura::TryRefreshStackOrCreate(SpellInfo const* spellproto, uint32 tryEffMask, WorldObject* owner, Unit* caster, int32* baseAmount /*= NULL*/, Item* castItem /*= NULL*/, ObjectGuid casterGUID /*= 0*/, bool* refresh /*= NULL*/)
{
    ASSERT(spellproto);
    ASSERT(owner);
    ASSERT(caster || casterGUID);
    ASSERT(tryEffMask <= MAX_EFFECT_MASK);
    if (refresh)
        *refresh = false;
    uint32 effMask = Aura::BuildEffectMaskForOwner(spellproto, tryEffMask, owner);
    if (!effMask)
        return NULL;
    if (Aura* foundAura = owner->ToUnit()->_TryStackingOrRefreshingExistingAura(spellproto, effMask, caster, baseAmount, castItem, casterGUID))
    {
        // we've here aura, which script triggered removal after modding stack amount
        // check the state here, so we won't create new Aura object
        if (foundAura->IsRemoved())
            return NULL;

        if (refresh)
            *refresh = true;
        return foundAura;
    }
    else
        return Create(spellproto, effMask, owner, caster, baseAmount, castItem, casterGUID);
}

Aura* Aura::TryCreate(SpellInfo const* spellproto, uint32 tryEffMask, WorldObject* owner, Unit* caster, int32* baseAmount /*= NULL*/, Item* castItem /*= NULL*/, ObjectGuid casterGUID /*= 0*/)
{
    ASSERT(spellproto);
    ASSERT(owner);
    ASSERT(caster || casterGUID);
    ASSERT(tryEffMask <= MAX_EFFECT_MASK);
    uint32 effMask = Aura::BuildEffectMaskForOwner(spellproto, tryEffMask, owner);
    if (!effMask)
        return NULL;
    return Create(spellproto, effMask, owner, caster, baseAmount, castItem, casterGUID);
}

Aura* Aura::Create(SpellInfo const* spellproto, uint32 effMask, WorldObject* owner, Unit* caster, int32* baseAmount, Item* castItem, ObjectGuid casterGUID)
{
    ASSERT(effMask);
    ASSERT(spellproto);
    ASSERT(owner);
    ASSERT(caster || casterGUID);
    ASSERT(effMask <= MAX_EFFECT_MASK);
    // try to get caster of aura
    if (casterGUID)
    {
        if (owner->GetGUID() == casterGUID)
            caster = owner->ToUnit();
        else
            caster = ObjectAccessor::GetUnit(*owner, casterGUID);
    }
    else
        casterGUID = caster->GetGUID();

    // check if aura can be owned by owner
    if (owner->isType(TYPEMASK_UNIT))
        if (!owner->IsInWorld() || ((Unit*)owner)->IsDuringRemoveFromWorld())
            // owner not in world so don't allow to own not self casted single target auras
            if (casterGUID != owner->GetGUID() && spellproto->IsSingleTarget())
                return nullptr;

    Aura* aura = nullptr;
    switch (owner->GetTypeId())
    {
        case TYPEID_UNIT:
        case TYPEID_PLAYER:
            aura = new UnitAura(spellproto, effMask, owner, caster, baseAmount, castItem, casterGUID);
            break;
        case TYPEID_DYNAMICOBJECT:
            aura = new DynObjAura(spellproto, effMask, owner, caster, baseAmount, castItem, casterGUID);
            break;
        default:
            ASSERT(false);
            return nullptr;
    }
    // aura can be removed in Unit::_AddAura call
    if (aura->IsRemoved())
        return nullptr;
    return aura;
}

Aura::Aura(SpellInfo const* spellproto, WorldObject* owner, Unit* caster, Item* castItem, ObjectGuid casterGUID, uint32 effMask, int32* baseAmount) :
m_spellInfo(spellproto), m_casterGuid(casterGUID ? casterGUID : caster->GetGUID()),
m_castItemGuid(castItem ? castItem->GetGUID() : ObjectGuid::Empty), m_applyTime(time(NULL)),
m_owner(owner), m_powerTakeTimer(0), m_updateTargetMapInterval(0),
m_casterLevel(caster ? caster->GetLevel() : m_spellInfo->SpellLevel), m_procCharges(0), m_stackAmount(1),
m_isRemoved(false), m_isBoundToCaster(false), m_isUsingCharges(false)
{
    // Before effects construction
    m_procCharges = CalcMaxCharges(caster);

    m_maxStackAmount = m_spellInfo->StackAmount;
    if (Unit* caster = GetCaster())
        if (Player* modOwner = caster->GetSpellModOwner())
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_STACK_AMOUNT, m_maxStackAmount);

    LoadScripts();
    _InitEffects(effMask, caster, baseAmount);

    m_powerEntry = m_spellInfo->GetSpellPowerEntry(caster);
    if (m_powerEntry && (m_powerEntry->CostPerSecond || m_powerEntry->CostPercentage))
        m_powerTakeTimer = 1 * IN_MILLISECONDS;

    m_duration = 0; // Need to calculate Pandemic in CalcMaxDuration
    // Periodic was initialized in _InitEffects, so now we have valid tick amplitude
    m_calculatedMaxDuration = CalcMaxDuration(caster);
    m_maxDuration = m_calculatedMaxDuration;
    m_duration = m_maxDuration;
    m_isUsingCharges = m_procCharges != 0;

    // And now calculate amount of effects, because some scripts needs duration to do this
    for (auto&& effect : m_effects)
        if (effect)
            effect->InitAmount(caster);

    // m_casterLevel = cast item level/caster level, caster level should be saved to db, confirmed with sniffs
}

AuraScript* Aura::GetScriptByName(std::string const& scriptName) const
{
    for (std::list<AuraScript*>::const_iterator itr = m_loadedScripts.begin(); itr != m_loadedScripts.end(); ++itr)
        if ((*itr)->_GetScriptName()->compare(scriptName) == 0)
            return *itr;
    return NULL;
}

void Aura::SaveParameters(AuraData& data)
{
    data.Id = GetId();
    data.CasterGuid = GetCasterGUID();
    data.EffectMask = GetEffectMask();
    data.Charges = GetCharges();
    data.Duration = GetDuration();
    data.Stacks = GetStackAmount();
    data.MaxDuration = GetMaxDuration();
    data.RecalculateMask = 0;

    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (AuraEffect const* effect = GetEffect(i))
        {
            data.BaseAmount[i] = effect->GetBaseAmount();
            data.Amount[i] = effect->GetAmount();
            data.Crit[i] = effect->GetPeriodicCrit();
            data.PeriodicTimer[i] = effect->GetPeriodicTimer();
            data.TickNumber[i] = effect->GetTickNumber();
            if (effect->CanBeRecalculated())
                data.RecalculateMask |= (1 << i);
        }
        else
        {
            data.BaseAmount[i] = 0;
            data.Amount[i] = 0.0f;
            data.Crit[i] = 0.0f;
            data.PeriodicTimer[i] = 0;
            data.TickNumber[i] = 0;
        }
    }
}

void Aura::LoadParameters(AuraData const& data)
{
    SetStackAmount(data.Stacks);
    SetMaxDuration(data.MaxDuration);
    SetDuration(data.Duration);
    SetCharges(data.Charges);

    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (AuraEffect* effect = GetEffect(i))
        {
            effect->ChangeAmount(data.Amount[i], false);
            effect->SetPeriodicCrit(data.Crit[i]);
            effect->SetPeriodicTimer(data.PeriodicTimer[i]);
            effect->SetTickNumber(data.TickNumber[i]);
            effect->SetCanBeRecalculated(data.RecalculateMask & (1 << i));
        }
    }
}

void Aura::_InitEffects(uint32 effMask, Unit* caster, int32 *baseAmount)
{
    CallScriptInitEffectsHandlers(effMask);

    // shouldn't be in constructor - functions in AuraEffect::AuraEffect use polymorphism
    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (effMask & (uint32(1) << i))
        {
            m_effects[i] = new AuraEffect(this, i, baseAmount ? baseAmount + i : NULL, caster);
            if (m_effects[i]->GetAuraType() == SPELL_AURA_PERIODIC_DAMAGE)
                m_hasPeriodicDamage = true;
        }
        else
            m_effects[i] = NULL;
    }
}

Aura::~Aura()
{
    // unload scripts
    while (!m_loadedScripts.empty())
    {
        std::list<AuraScript*>::iterator itr = m_loadedScripts.begin();
        (*itr)->_Unload();
        delete (*itr);
        m_loadedScripts.erase(itr);
    }

    // free effects memory
    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
         delete m_effects[i];

    ASSERT(m_applications.empty());
    _DeleteRemovedApplications();
}

Unit* Aura::GetCaster() const
{
    if (GetOwner()->GetGUID() == GetCasterGUID())
        return GetUnitOwner();
    if (AuraApplication const* aurApp = GetApplicationOfTarget(GetCasterGUID()))
        return aurApp->GetTarget();

    return ObjectAccessor::GetUnit(*GetOwner(), GetCasterGUID());
}

AuraObjectType Aura::GetType() const
{
    return (m_owner->GetTypeId() == TYPEID_DYNAMICOBJECT) ? DYNOBJ_AURA_TYPE : UNIT_AURA_TYPE;
}

void Aura::_ApplyForTarget(Unit* target, Unit* caster, AuraApplication * auraApp)
{
    ASSERT(target);
    ASSERT(auraApp);
    // aura mustn't be already applied on target
    ASSERT (!IsAppliedOnTarget(target->GetGUID()) && "Aura::_ApplyForTarget: aura musn't be already applied on target");

    m_applications[target->GetGUID()] = auraApp;

    // set infinity cooldown state for spells
    if (caster && caster->GetTypeId() == TYPEID_PLAYER)
    {
        if (m_spellInfo->IsCooldownStartedOnEvent())
        {
            Item* item = GetCastItemGUID() ? caster->ToPlayer()->GetItemByGuid(GetCastItemGUID()) : nullptr;
            caster->ToPlayer()->GetSpellHistory()->SetCooldownOnHold(m_spellInfo, item ? item->GetEntry() : 0);
        }
    }
}

void Aura::_UnapplyForTarget(Unit* target, Unit* caster, AuraApplication * auraApp)
{
    ASSERT(target);
    ASSERT(auraApp->GetRemoveMode());
    ASSERT(auraApp);

    ApplicationMap::iterator itr = m_applications.find(target->GetGUID());

    /// @todo Figure out why this happens
    if (itr == m_applications.end())
    {
        TC_LOG_ERROR("spells", "Aura::_UnapplyForTarget, target:%u, caster:%u, spell:%u was not found in owners application map!",
        target->GetGUID().GetCounter(), caster ? caster->GetGUID().GetCounter() : 0, auraApp->GetBase()->GetSpellInfo()->Id);
        ASSERT(false);
    }

    // aura has to be already applied
    ASSERT(itr->second == auraApp);
    m_applications.erase(itr);

    m_removedApplications.push_back(auraApp);

    // reset cooldown state for spells
    if (caster && GetSpellInfo()->IsCooldownStartedOnEvent())
        caster->GetSpellHistory()->SendCooldownEvent(GetSpellInfo());
}

// removes aura from all targets
// and marks aura as removed
void Aura::_Remove(AuraRemoveMode removeMode)
{
    ASSERT (!m_isRemoved);
    m_isRemoved = true;
    ApplicationMap::iterator appItr = m_applications.begin();
    for (appItr = m_applications.begin(); appItr != m_applications.end();)
    {
        AuraApplication * aurApp = appItr->second;
        Unit* target = aurApp->GetTarget();
        target->_UnapplyAura(aurApp, removeMode);
        appItr = m_applications.begin();
    }
}

void Aura::RemoveEffect(uint8 effIndex)
{
    if (AuraEffect*& auraEffect = m_effects[effIndex])
    {
        std::list<AuraApplication*> applications;
        auraEffect->GetApplicationList(applications);
        for (auto&& app : applications)
            app->_HandleEffect(effIndex, false);

        delete auraEffect;
        auraEffect = nullptr;
    }
}

void Aura::UpdateTargetMap(Unit* caster, bool apply)
{
    if (IsRemoved())
        return;

    if (GetSpellInfo()->IsPositive() && IsPermanent())
        m_updateTargetMapInterval = UPDATE_TARGET_MAP_INTERVAL * 6;
    else if (GetType() == DYNOBJ_AURA_TYPE && !IsPermanent()) // Can be permanent anyway?...
        m_updateTargetMapInterval = UPDATE_TARGET_MAP_INTERVAL / 5;
    else
        m_updateTargetMapInterval = UPDATE_TARGET_MAP_INTERVAL;

    // fill up to date target list
    //       target, effMask
    std::map<Unit*, uint32> targets;

    FillTargetMap(targets, caster);

    UnitList targetsToRemove;

    // mark all auras as ready to remove
    for (ApplicationMap::iterator appIter = m_applications.begin(); appIter != m_applications.end();appIter++)
    {
        std::map<Unit*, uint32>::iterator existing = targets.find(appIter->second->GetTarget());
        // not found in current area - remove the aura
        if (existing == targets.end())
            targetsToRemove.push_back(appIter->second->GetTarget());
        else
        {
            // needs readding - remove now, will be applied in next update cycle
            // (dbcs do not have auras which apply on same type of targets but have different radius, so this is not really needed)
            if (appIter->second->GetEffectMask() != existing->second || !CanBeAppliedOn(existing->first))
                targetsToRemove.push_back(appIter->second->GetTarget());
            // nothing todo - aura already applied
            // remove from auras to register list
            targets.erase(existing);
        }
    }

    // register auras for units
    for (std::map<Unit*, uint32>::iterator itr = targets.begin(); itr!= targets.end();)
    {
        // aura mustn't be already applied on target
        if (AuraApplication * aurApp = GetApplicationOfTarget(itr->first->GetGUID()))
        {
            // the core created 2 different units with same guid
            // this is a major failue, which i can't fix right now
            // let's remove one unit from aura list
            // this may cause area aura "bouncing" between 2 units after each update
            // but because we know the reason of a crash we can remove the assertion for now
            if (aurApp->GetTarget() != itr->first)
            {
                // remove from auras to register list
                targets.erase(itr++);
                continue;
            }
            else
            {
                // ok, we have one unit twice in target map (impossible, but...)
                ASSERT(false);
            }
        }

        bool addUnit = true;
        // check target immunities
        for (uint32 effIndex = 0; effIndex < MAX_SPELL_EFFECTS; ++effIndex)
        {
            if (itr->first->IsImmunedToSpellEffect(GetSpellInfo(), effIndex))
                itr->second &= ~(1 << effIndex);
        }
        if (!itr->second
            || itr->first->IsImmunedToSpell(GetSpellInfo(), itr->second)
            || !CanBeAppliedOn(itr->first))
            addUnit = false;

        if (addUnit)
        {
            // persistent area aura does not hit flying targets
            if (GetType() == DYNOBJ_AURA_TYPE)
            {
                if (itr->first->IsInFlight())
                    addUnit = false;
            }
            // unit auras can not stack with each other
            else // (GetType() == UNIT_AURA_TYPE)
            {
                // Allow to remove by stack when aura is going to be applied on owner
                if (itr->first != GetOwner())
                {
                    // check if not stacking aura already on target
                    // this one prevents unwanted usefull buff loss because of stacking and prevents overriding auras periodicaly by 2 near area aura owners
                    for (Unit::AuraApplicationMap::iterator iter = itr->first->GetAppliedAuras().begin(); iter != itr->first->GetAppliedAuras().end(); ++iter)
                    {
                        Aura const* aura = iter->second->GetBase();
                        if (!CanStackWith(aura))
                        {
                            addUnit = false;
                            break;
                        }
                    }
                }
            }
        }
        if (!addUnit)
            targets.erase(itr++);
        else
        {
            // owner has to be in world, or effect has to be applied to self
            if (!GetOwner()->IsSelfOrInSameMap(itr->first))
            {
                /// @todo There is a crash caused by shadowfiend load addon
                TC_LOG_FATAL("spells", "Aura %u: Owner %s (map %u) is not in the same map as target %s (map %u).", GetSpellInfo()->Id,
                    GetOwner()->GetName().c_str(), GetOwner()->IsInWorld() ? GetOwner()->GetMap()->GetId() : uint32(-1),
                    itr->first->GetName().c_str(), itr->first->IsInWorld() ? itr->first->GetMap()->GetId() : uint32(-1));
                ASSERT(false);
            }
            itr->first->_CreateAuraApplication(this, itr->second);
            ++itr;
        }
    }

    // remove auras from units no longer needing them
    for (UnitList::iterator itr = targetsToRemove.begin(); itr != targetsToRemove.end();++itr)
        if (AuraApplication * aurApp = GetApplicationOfTarget((*itr)->GetGUID()))
            (*itr)->_UnapplyAura(aurApp, AURA_REMOVE_BY_DEFAULT);

    if (!apply)
        return;

    // apply aura effects for units
    for (std::map<Unit*, uint32>::iterator itr = targets.begin(); itr!= targets.end();++itr)
    {
        if (AuraApplication * aurApp = GetApplicationOfTarget(itr->first->GetGUID()))
        {
            // owner has to be in world, or effect has to be applied to self
            ASSERT((!GetOwner()->IsInWorld() && GetOwner() == itr->first) || GetOwner()->IsInMap(itr->first));
            itr->first->_ApplyAura(aurApp, itr->second);
        }
    }
}

// targets have to be registered and not have effect applied yet to use this function
void Aura::_ApplyEffectForTargets(uint8 effIndex)
{
    // prepare list of aura targets
    UnitList targetList;
    for (ApplicationMap::iterator appIter = m_applications.begin(); appIter != m_applications.end(); ++appIter)
    {
        if ((appIter->second->GetEffectsToApply() & (1<<effIndex)) && !appIter->second->HasEffect(effIndex))
            targetList.push_back(appIter->second->GetTarget());
    }

    // apply effect to targets
    for (UnitList::iterator itr = targetList.begin(); itr != targetList.end(); ++itr)
    {
        if (GetApplicationOfTarget((*itr)->GetGUID()))
        {
            // owner has to be in world, or effect has to be applied to self
            ASSERT((!GetOwner()->IsInWorld() && GetOwner() == *itr) || GetOwner()->IsInMap(*itr));
            (*itr)->_ApplyAuraEffect(this, effIndex);
        }
    }
}
void Aura::UpdateOwner(uint32 diff, WorldObject* owner)
{
    ASSERT(owner == m_owner);

    Unit* caster = GetCaster();
    // Apply spellmods for channeled auras
    // used for example when triggered spell of spell:10 is modded
    Spell* modSpell = NULL;
    Player* modOwner = NULL;
    if (caster)
    {
        modOwner = caster->GetSpellModOwner();
        if (modOwner)
        {
            modSpell = modOwner->FindCurrentSpellBySpellId(GetId());
            if (modSpell)
                modOwner->SetSpellModTakingSpell(modSpell, true);
        }
    }

    Update(diff, caster);

    if (m_updateTargetMapInterval <= int32(diff))
        UpdateTargetMap(caster);
    else
        m_updateTargetMapInterval -= diff;

    // update aura effects
    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (m_effects[i])
            m_effects[i]->Update(diff, caster);

    // remove spellmods after effects update
    if (modSpell)
        modOwner->SetSpellModTakingSpell(modSpell, false);

    _DeleteRemovedApplications();
}

void Aura::Update(uint32 diff, Unit* caster)
{
    if (m_duration > 0)
    {
        m_duration -= diff;
        if (m_duration < 0)
            m_duration = 0;

        CallScriptAuraUpdateHandlers(diff);
    }

    // handle manaPerSecond/manaPerSecondPerLevel
    if (m_powerTakeTimer && m_powerEntry && GetType() == UNIT_AURA_TYPE) // I don't know but same AoE channeling spells (i.e. 5740) have two auras on caster and on dynamic object
    {
        if (m_powerTakeTimer > int32(diff))
            m_powerTakeTimer -= diff;
        else if (caster)
        {
            int32 powerPerSecond = 0;
            if (m_powerEntry->CostPerSecond)
                powerPerSecond = m_powerEntry->CostPerSecond;
            else if (m_powerEntry->CostPerSecondPercentage)
                powerPerSecond = CalculatePct(caster->GetMaxPower(Powers(m_powerEntry->PowerType)), m_powerEntry->CostPerSecondPercentage);

            if (powerPerSecond)
            {
                m_powerTakeTimer += 1000 - diff;

                Powers powertype = Powers(m_powerEntry->PowerType);
                if (powertype == POWER_HEALTH)
                {
                    if (int32(caster->GetHealth()) > powerPerSecond)
                        caster->ModifyHealth(-powerPerSecond);
                    else
                    {
                        Remove();
                        return;
                    }
                }
                else
                {
                    if (int32(caster->GetPower(powertype)) >= powerPerSecond)
                        caster->ModifyPower(powertype, -powerPerSecond);
                    else
                    {
                        Remove();
                        return;
                    }
                }
            }
        }
    }
}

int32 Aura::CalcMaxDuration(Unit* caster) const
{
    Player* modOwner = NULL;
    int32 maxDuration;

    if (caster)
    {
        modOwner = caster->GetSpellModOwner();
        maxDuration = caster->CalcSpellDuration(m_spellInfo);
    }
    else
        maxDuration = m_spellInfo->GetDuration();

    if (IsPassive() && !m_spellInfo->DurationEntry)
        maxDuration = -1;

    int32 limitduration = 0;
    bool limited = false;

    if (caster && maxDuration > 0 && GetType() == UNIT_AURA_TYPE)
    {
        limitduration = GetDiminishingReturnsLimitDuration(GetDiminishingReturnsGroupForSpell(m_spellInfo), m_spellInfo);

        // test pet/charm masters instead pets/charmeds
        Unit const* tarGetOwner = GetUnitOwner()->GetCharmerOrOwner();
        Unit const* casterOwner = caster->GetCharmerOrOwner();

        Unit const* target = tarGetOwner ? tarGetOwner : GetUnitOwner();
        Unit const* source = casterOwner ? casterOwner : caster;

        if ((target->GetTypeId() == TYPEID_PLAYER
            || ((Creature*)target)->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_ALL_DIMINISH)
            && source->GetTypeId() == TYPEID_PLAYER)
        {
            limited = limitduration > 0;

            // Duration of crowd control abilities on pvp target is limited by 10 sec. (2.2.0)
            if (limitduration > 0 && maxDuration > limitduration)
                maxDuration = limitduration;
        }
    }

    // IsPermanent() checks max duration (which we are supposed to calculate here)
    if (maxDuration != -1 && modOwner)
        modOwner->ApplySpellMod(GetId(), SPELLMOD_DURATION, maxDuration);

    if (maxDuration > 0)
    {
        bool positive = GetSpellInfo()->IsPositive();
        AuraApplication const* aurApp = GetApplicationOfTarget(m_casterGuid);
        if (aurApp)
            positive = aurApp->IsPositive();

        if (caster && GetType() == UNIT_AURA_TYPE)
            maxDuration = caster->ModSpellDuration(GetSpellInfo(), GetUnitOwner(), maxDuration, positive, GetEffectMask());

        // Pandemic, exclude Chaos Bolt
        if (m_hasPeriodicDamage && GetDuration() && caster && caster->HasAura(131973) && !GetSpellInfo()->HasAttribute(SPELL_ATTR10_CUMULATIVE_PERIODIC))
        {
            int32 max = maxDuration * 1.5;
            maxDuration += GetDuration();
            maxDuration = std::min(max, maxDuration);
        }

        // Haste modifies duration of channeled spells
        if ((m_spellInfo->IsChanneled() || m_spellInfo->HasAttribute(SPELL_ATTR8_HASTE_AFFECT_DURATION)) && caster)
        {
            if (m_spellInfo->HasAttribute(SPELL_ATTR5_HASTE_AFFECT_DURATION))
                maxDuration *= caster->GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED);
            else if (m_spellInfo->HasAttribute(SPELL_ATTR8_HASTE_AFFECT_DURATION))
                maxDuration *= caster->GetFloatValue(UNIT_FIELD_MOD_HASTE_REGEN);
        }
        else if (m_spellInfo->HasAttribute(SPELL_ATTR5_HASTE_AFFECT_DURATION))
        {
            int32 origDuration = maxDuration;
            maxDuration = 0;
            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            {
                if (AuraEffect const* eff = GetEffect(i))
                {
                    if (int32 amplitude = eff->GetAmplitude())  // amplitude is hastened by UNIT_FIELD_MOD_CASTING_SPEED
                    {
                        int32 ticks = int32(std::round(float(origDuration) / amplitude));
                        ticks = std::max(ticks, 1);
                        maxDuration = std::max(ticks * amplitude, maxDuration);
                    }
                }
            }
            // if there is no periodic effect
            if (!maxDuration && caster)
                maxDuration = int32(origDuration * caster->GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED));
        }
    }

    if (limited && maxDuration > limitduration)
        maxDuration = limitduration;

    return maxDuration;
}

void Aura::SetDuration(int32 duration, bool withMods)
{
    if (withMods)
    {
        if (Unit* caster = GetCaster())
            if (Player* modOwner = caster->GetSpellModOwner())
                modOwner->ApplySpellMod(GetId(), SPELLMOD_DURATION, duration);
    }
    m_duration = duration;
    SetNeedClientUpdateForTargets();
}

void Aura::RefreshDuration(bool recalculate)
{
    if (m_spellInfo->AttributesEx8 & SPELL_ATTR8_DONT_RESET_PERIODIC_TIMER)
    {
        int32 remainingTick = m_calculatedMaxDuration;
        bool found = false;
        for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (AuraEffect const* eff = GetEffect(i))
            {
                if (int32 timer = eff->GetPeriodicTimer())
                {
                    if (timer != eff->GetAmplitude())
                    {
                        remainingTick = timer;
                        found = true;
                    }
                }
            }
        }
        if (found)
            m_maxDuration = m_calculatedMaxDuration + remainingTick;
    }

    SetDuration(GetMaxDuration());

    if (m_powerEntry && (m_powerEntry->CostPerSecond || m_powerEntry->CostPercentage))
        m_powerTakeTimer = 1 * IN_MILLISECONDS;

    if (GetSpellInfo()->IsChanneled())
    {
        if (Unit * caster = GetCaster())
            if (Spell * channeled = caster->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                if (channeled->GetSpellInfo()->Id == GetSpellInfo()->Id)
                    channeled->SendChannelStart(GetMaxDuration());
    }

    if (recalculate)
        RecalculateAmountOfEffects();
}

void Aura::RefreshTimers(bool recalculate)
{
    Unit* caster = GetCaster();
    bool resetPeriodic = !(m_spellInfo->AttributesEx8 & SPELL_ATTR8_DONT_RESET_PERIODIC_TIMER);

    for (auto&& eff : m_effects)
        if (eff)
            eff->CalculatePeriodic(caster, resetPeriodic, false);

    m_calculatedMaxDuration = CalcMaxDuration(caster);
    m_maxDuration = m_calculatedMaxDuration;

    RefreshDuration(recalculate);
}

void Aura::SetCharges(uint8 charges)
{
    if (m_procCharges == charges)
        return;
    m_procCharges = charges;
    m_isUsingCharges = m_procCharges != 0;
    SetNeedClientUpdateForTargets();
}

uint8 Aura::CalcMaxCharges(Unit* caster) const
{
    uint32 maxProcCharges = m_spellInfo->ProcCharges;
    if (SpellProcEntry const* procEntry = sSpellMgr->GetSpellProcEntry(GetId()))
        maxProcCharges = procEntry->charges;

    if (caster)
        if (Player* modOwner = caster->GetSpellModOwner())
            modOwner->ApplySpellMod(GetId(), SPELLMOD_CHARGES, maxProcCharges);
    return uint8(maxProcCharges);
}

bool Aura::ModCharges(int32 num, AuraRemoveMode removeMode)
{
    if (IsUsingCharges())
    {
        int32 charges = m_procCharges + num;
        int32 maxCharges = CalcMaxCharges();

        // limit charges (only on charges increase, charges may be changed manually)
        if ((num > 0) && (charges > int32(maxCharges)))
            charges = maxCharges;
        // we're out of charges, remove
        else if (charges <= 0)
        {
            Remove(removeMode);
            return true;
        }

        SetCharges(charges);
    }
    return false;
}

void Aura::SetStackAmount(uint8 stackAmount)
{
    m_stackAmount = stackAmount;
    Unit* caster = GetCaster();

    std::list<AuraApplication*> applications;
    GetApplicationList(applications);

    for (std::list<AuraApplication*>::const_iterator apptItr = applications.begin(); apptItr != applications.end(); ++apptItr)
        if (!(*apptItr)->GetRemoveMode())
            HandleAuraSpecificMods(*apptItr, caster, false, true);

    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (HasEffect(i))
            m_effects[i]->ChangeAmount(m_effects[i]->CalculateAmount(caster, true), false, true);

    for (std::list<AuraApplication*>::const_iterator apptItr = applications.begin(); apptItr != applications.end(); ++apptItr)
        if (!(*apptItr)->GetRemoveMode())
            HandleAuraSpecificMods(*apptItr, caster, true, true);

    SetNeedClientUpdateForTargets();
}

bool Aura::ModStackAmount(int32 num, AuraRemoveMode removeMode)
{
    int32 stackAmount = m_stackAmount + num;

    // limit the stack amount (only on stack increase, stack amount may be changed manually)
    if ((num > 0) && (stackAmount > int32(m_spellInfo->StackAmount)))
    {
        // not stackable aura - set stack amount to 1
        if (!m_spellInfo->StackAmount)
            stackAmount = 1;
        else
            stackAmount = m_spellInfo->StackAmount;
    }
    // we're out of stacks, remove
    else if (stackAmount <= 0)
    {
        Remove(removeMode);
        return true;
    }

    bool refresh = stackAmount >= GetStackAmount();
    bool resetCharges = true;

    if (m_spellInfo->Id == 138349)
        refresh = false;

    // Save old amount used for tick-rolling
    int32 oldPeriodicAmount = 0;
    for (auto&& effect : m_effects)
    {
        if (effect&& effect->GetAuraType() == SPELL_AURA_PERIODIC_DAMAGE)
        {
            oldPeriodicAmount = effect->GetAmount();
            break;
        }
    }

    // Update stack amount
    SetStackAmount(stackAmount);

    // Will add this because i couldn't figure out a proper rule yet
    switch (m_spellInfo->Id)
    {
        case 88819:
            resetCharges = false;
            break;
    }

    if (refresh)
    {
        RefreshSpellMods();
        RefreshTimers(false);

        // reset charges
        auto charges = CalcMaxCharges();
        CallScriptRefreshChargesHandlers(charges);
        if (resetCharges)
        {
            SetCharges(charges);

            for (auto&& eff : m_effects)
                if (eff && (eff->GetAuraType() == SPELL_AURA_ADD_FLAT_MODIFIER || eff->GetAuraType() == SPELL_AURA_ADD_PCT_MODIFIER))
                    if (auto mod = eff->GetSpellModifier())
                        mod->charges = GetCharges();
        }
    }
    SetNeedClientUpdateForTargets();
    return false;
}

void Aura::RefreshSpellMods()
{
    for (Aura::ApplicationMap::const_iterator appIter = m_applications.begin(); appIter != m_applications.end(); ++appIter)
        if (Player* player = appIter->second->GetTarget()->ToPlayer())
            player->RestoreAllSpellMods(0, this);
}

bool Aura::IsArea() const
{
    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (HasEffect(i) && GetSpellInfo()->Effects[i].IsAreaAuraEffect())
            return true;
    }
    return false;
}

bool Aura::IsPassive() const
{
    return GetSpellInfo()->IsPassive();
}

bool Aura::IsDeathPersistent() const
{
    return GetSpellInfo()->IsDeathPersistent();
}

bool Aura::IsRemovedOnShapeLost(Unit* target) const
{
    return GetCasterGUID() == target->GetGUID()
        && m_spellInfo->Stances
        && !(m_spellInfo->AttributesEx2 & SPELL_ATTR2_NOT_NEED_SHAPESHIFT)
        && !(m_spellInfo->Attributes & SPELL_ATTR0_NOT_SHAPESHIFT)
        && m_spellInfo->Id != 114842;
}

bool Aura::CanBeSaved() const
{
    if (IsPassive())
        return false;

    if (GetSpellInfo()->IsChanneled())
        return false;

    // Check if aura is single target, not only spell info
    if (GetCasterGUID() != GetOwner()->GetGUID())
    {
        for (SpellEffectInfo const& spellEffectInfo : GetSpellInfo()->GetEffects())
        {
            if (!spellEffectInfo.IsEffect())
                continue;

            if (spellEffectInfo.IsTargetingArea() || spellEffectInfo.IsAreaAuraEffect())
                return false;
        }

        // TODO: if (IsSingleTarget() || GetSpellInfo()->IsSingleTarget())
        if (GetSpellInfo()->IsSingleTarget())
            return false;
    }

    // Can't be saved - aura handler relies on calculated amount and changes it
    if (HasEffectType(SPELL_AURA_CONVERT_RUNE))
        return false;

    // No point in saving this, since the stable dialog can't be open on aura load anyway.
    if (HasEffectType(SPELL_AURA_OPEN_STABLE))
        return false;

    // Can't save vehicle auras, it requires both caster & target to be in world
    if (HasEffectType(SPELL_AURA_CONTROL_VEHICLE))
        return false;

    if (m_spellInfo->AttributesCu & SPELL_ATTR0_CU_DONT_SAVE_AURA_TO_DB)
        return false;

    // don't save auras removed by proc system
    if (IsUsingCharges() && !GetCharges())
        return false;

    return true;
}

bool Aura::CanBeSentToClient(Unit* target) const
{
    if (GetSpellInfo()->AttributesCu & SPELL_ATTR0_CU_SERVERSIDE_ONLY)
        return false;

    if (!IsPassive())
        return true;
    if ((GetSpellInfo()->AttributesEx8 & SPELL_ATTR8_AURA_SEND_AMOUNT) != 0)
        return true;

    // Healing Tide animation
    if (GetSpellInfo()->Id == 114941)
        return true;

    for (auto&& it : GetSpellInfo()->Effects)
    {
        if (it.IsAreaAuraEffect())
            return true;
        switch (it.ApplyAuraName)
        {
            case SPELL_AURA_ABILITY_IGNORE_AURASTATE:
            case SPELL_AURA_CAST_WHILE_WALKING:
            case SPELL_AURA_OVERRIDE_SPELL_VISUAL:
            case SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS:
            case SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS_2:
            case SPELL_AURA_MOD_COOLDOWN_BY_SPELL_HASTE:
            case SPELL_AURA_MOD_COOLDOWN_BY_HASTE:
            case SPELL_AURA_MOD_GLOBAL_COOLDOWN_BY_HASTE:
                return true;
            default:
                break;
        }
    }
    return false;
}

int32 Aura::CalcDispelChance(Unit* auraTarget, bool offensive) const
{
    // we assume that aura dispel chance is 100% on start
    // need formula for level difference based chance
    int32 resistChance = 0;

    // Apply dispel mod from aura caster
    if (Unit* caster = GetCaster())
        if (Player* modOwner = caster->GetSpellModOwner())
            modOwner->ApplySpellMod(GetId(), SPELLMOD_RESIST_DISPEL_CHANCE, resistChance);

    // Dispel resistance from target SPELL_AURA_MOD_DISPEL_RESIST
    // Only affects offensive dispels
    if (offensive && auraTarget)
        resistChance += auraTarget->GetTotalAuraModifier(SPELL_AURA_MOD_DISPEL_RESIST);

    RoundToInterval(resistChance, 0, 100);
    return 100 - resistChance;
}

void Aura::SetLoadedState(int32 maxduration, int32 duration, int32 charges, uint8 stackamount, uint32 recalculateMask, int32* amount)
{
    m_maxDuration = maxduration;
    m_duration = duration;
    m_procCharges = charges;
    m_isUsingCharges = m_procCharges != 0;
    m_stackAmount = stackamount;
    Unit* caster = GetCaster();
    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (m_effects[i])
        {
            m_effects[i]->SetAmount(amount[i]);
            m_effects[i]->SetCanBeRecalculated(recalculateMask & (1<<i));
            m_effects[i]->CalculatePeriodic(caster, false, true);
            m_effects[i]->CalculateSpellMod();
        }
    }
}

bool Aura::HasEffectType(AuraType type) const
{
    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (HasEffect(i) && m_effects[i]->GetAuraType() == type)
            return true;
    }
    return false;
}

void Aura::RecalculateAmountOfEffects()
{
    ASSERT (!IsRemoved());
    Unit* caster = GetCaster();
    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (HasEffect(i))
            m_effects[i]->RecalculateAmount(caster);
}

void Aura::HandleAllEffects(AuraApplication * aurApp, uint8 mode, bool apply)
{
    ASSERT (!IsRemoved());
    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (m_effects[i] && !IsRemoved())
            m_effects[i]->HandleEffect(aurApp, mode, apply);
}

void Aura::GetApplicationList(std::list<AuraApplication*> & applicationList) const
{
    for (Aura::ApplicationMap::const_iterator appIter = m_applications.begin(); appIter != m_applications.end(); ++appIter)
    {
        if (appIter->second->GetEffectMask())
            applicationList.push_back(appIter->second);
    }
}

void Aura::SetNeedClientUpdateForTargets() const
{
    for (ApplicationMap::const_iterator appIter = m_applications.begin(); appIter != m_applications.end(); ++appIter)
        appIter->second->SetNeedClientUpdate();
}

// trigger effects on real aura apply/remove
void Aura::HandleAuraSpecificMods(AuraApplication const* aurApp, Unit* caster, bool apply, bool onReapply)
{
    Unit* target = aurApp->GetTarget();
    AuraRemoveMode removeMode = aurApp->GetRemoveMode();
    // handle spell_area table
    SpellAreaForAreaMapBounds saBounds = sSpellMgr->GetSpellAreaForAuraMapBounds(GetId());
    if (saBounds.first != saBounds.second)
    {
        uint32 zone, area;
        target->GetZoneAndAreaId(zone, area);

        for (SpellAreaForAreaMap::const_iterator itr = saBounds.first; itr != saBounds.second; ++itr)
        {
            // some auras remove at aura remove
            if (!itr->second->IsFitToRequirements(target->ToPlayer(), zone, area))
                target->RemoveAurasDueToSpell(itr->second->spellId);
            // some auras applied at aura apply
            else if (itr->second->autocast)
            {
                if (!target->HasAura(itr->second->spellId))
                    target->CastSpell(target, itr->second->spellId, true);
            }
        }
    }

    // handle spell_linked_spell table
    if (!onReapply)
    {
        // apply linked auras
        if (apply)
        {
            if (std::vector<int32> const* spellTriggered = sSpellMgr->GetSpellLinked(GetId() + SPELL_LINK_AURA))
            {
                for (std::vector<int32>::const_iterator itr = spellTriggered->begin(); itr != spellTriggered->end(); ++itr)
                {
                    if (*itr < 0)
                        target->ApplySpellImmune(GetId(), IMMUNITY_ID, -(*itr), true);
                    else if (caster)
                        caster->AddAura(*itr, target);
                }
            }
        }
        else
        {
            // remove linked auras
            if (std::vector<int32> const* spellTriggered = sSpellMgr->GetSpellLinked(-(int32)GetId()))
            {
                for (std::vector<int32>::const_iterator itr = spellTriggered->begin(); itr != spellTriggered->end(); ++itr)
                {
                    if (*itr < 0)
                        target->RemoveAurasDueToSpell(-(*itr));
                    else if (removeMode != AURA_REMOVE_BY_DEATH)
                        target->CastSpell(target, *itr, true, NULL, NULL, GetCasterGUID());
                }
            }
            if (std::vector<int32> const* spellTriggered = sSpellMgr->GetSpellLinked(GetId() + SPELL_LINK_AURA))
            {
                for (std::vector<int32>::const_iterator itr = spellTriggered->begin(); itr != spellTriggered->end(); ++itr)
                {
                    if (*itr < 0)
                        target->ApplySpellImmune(GetId(), IMMUNITY_ID, -(*itr), false);
                    else
                        target->RemoveAura(*itr, GetCasterGUID(), 0, removeMode);
                }
            }
        }
    }
    else if (apply)
    {
        // modify stack amount of linked auras
        if (std::vector<int32> const* spellTriggered = sSpellMgr->GetSpellLinked(GetId() + SPELL_LINK_AURA))
        {
            for (std::vector<int32>::const_iterator itr = spellTriggered->begin(); itr != spellTriggered->end(); ++itr)
                if (*itr > 0)
                    if (Aura* triggeredAura = target->GetAura(*itr, GetCasterGUID()))
                        triggeredAura->ModStackAmount(GetStackAmount() - triggeredAura->GetStackAmount());
        }
    }

    // mods at aura apply
    if (apply)
    {
        // Need for worshipping, Cho'gall, unique mechanic
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (!HasEffect(i))
                continue;

            if (AuraEffect* aurEff = GetEffect(i))
            {
                switch (aurEff->GetAuraType())
                {
                    case SPELL_AURA_MOD_CONFUSE:
                    case SPELL_AURA_MOD_CHARM:
                    case SPELL_AURA_MOD_FEAR:
                    case SPELL_AURA_MOD_STUN:
                    case SPELL_AURA_MOD_SILENCE:
                        target->RemoveAurasDueToSpell(91317);
                        target->RemoveAurasDueToSpell(93365);
                        target->RemoveAurasDueToSpell(93366);
                        target->RemoveAurasDueToSpell(93367);
                        break;
                }
            }
        }

        switch (GetSpellInfo()->SpellFamilyName)
        {
            case SPELLFAMILY_GENERIC:
                switch (GetId())
                {
                    case 42292: // PvP Trinket
                        if (target && target->GetTypeId() == TYPEID_PLAYER)
                            target->CastSpell(target, (target->ToPlayer()->GetTeam() == ALLIANCE ? 97403 : 97404), true);
                        break;
                    case 101619: // Magma, Echo of Baine
                        if (target && target->GetTypeId() == TYPEID_PLAYER && !target->HasAura(101866))
                            target->CastSpell(target, 101866, true);
                        break;
                    case 99461: // Blazing Power, Alysrazor
                        if (Aura* aur = target->GetAura(98619))
                            aur->RefreshDuration();

                        if (!target->HasAura(100029) && aurApp->GetBase()->GetStackAmount() >= 25)
                            target->CastSpell(target, 100029, true);
                        break;
                    case 83500: // Arion - Swirling Winds
                        target->RemoveAurasDueToSpell(83581);
                        break;
                    case 83581: // Terrastra - Grounded
                        target->RemoveAurasDueToSpell(83500);
                        break;
                    case 32474: // Buffeting Winds of Susurrus
                        if (target->GetTypeId() == TYPEID_PLAYER)
                            target->ToPlayer()->ActivateTaxiPathTo(506, GetId());
                        break;
                    case 33572: // Gronn Lord's Grasp, becomes stoned
                        if (GetStackAmount() >= 5 && !target->HasAura(33652))
                            target->CastSpell(target, 33652, true);
                        break;
                    case 50836: //Petrifying Grip, becomes stoned
                        if (GetStackAmount() >= 5 && !target->HasAura(50812))
                            target->CastSpell(target, 50812, true);
                        break;
                    case 60970: // Heroic Fury (remove Intercept cooldown)
                        if (target->GetTypeId() == TYPEID_PLAYER)
                            target->ToPlayer()->RemoveSpellCooldown(20252, true);
                        break;
                    case 66296: // Shadow Veil
                        if (target->GetTypeId() == TYPEID_UNIT)
                        {
                            target->ToCreature()->_AddCreatureSpellCooldown(GetId(), time(NULL) + 40000 / IN_MILLISECONDS);
                            if (Vehicle* vehicle = target->GetVehicleKit())
                                if (Unit* passenger = vehicle->GetPassenger(0))
                                    if (Player* player = passenger->ToPlayer())
                                        player->GetSpellHistory()->SendCooldown(GetId(), 40000);
                        }
                        break;
                }
                break;
            case SPELLFAMILY_DRUID:
                if (!caster)
                    break;
                // Rejuvenation
                if (GetSpellInfo()->SpellFamilyFlags[0] & 0x10 && GetEffect(EFFECT_0))
                {
                    // Druid T8 Restoration 4P Bonus
                    if (caster->HasAura(64760))
                    {
                        int32 heal = GetEffect(EFFECT_0)->GetAmount();
                        caster->CastCustomSpell(target, 64801, &heal, NULL, NULL, true, NULL, GetEffect(EFFECT_0));
                    }
                }
                break;
            case SPELLFAMILY_MAGE:
                if (!caster)
                    break;
                switch (GetId())
                {
                    case 12536: // Clearcasting
                    case 12043: // Presence of Mind
                        // Arcane Potency
                        if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_MAGE, 2120, 0))
                        {
                            uint32 spellId = 0;

                            switch (aurEff->GetId())
                            {
                                case 31571: spellId = 57529; break;
                                case 31572: spellId = 57531; break;
                                default:
                                    TC_LOG_ERROR("spells", "Aura::HandleAuraSpecificMods: Unknown rank of Arcane Potency (%d) found", aurEff->GetId());
                            }
                            if (spellId)
                                caster->CastSpell(caster, spellId, true);
                        }
                        break;
                    default:
                        break;
                }
                break;
            case SPELLFAMILY_PRIEST:
                if (!caster)
                    break;
                // Devouring Plague
                if (GetSpellInfo()->SpellFamilyFlags[0] & 0x02000000 && GetEffect(0))
                {
                    // Improved Devouring Plague
                    if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_PRIEST, 3790, 0))
                    {
                        uint32 damage = caster->SpellDamageBonusDone(target, GetSpellInfo(), EFFECT_0, GetEffect(0)->GetAmount(), DOT);
                        damage = target->SpellDamageBonusTaken(caster, GetSpellInfo(), EFFECT_0, damage, DOT);
                        int32 basepoints0 = aurEff->GetAmount() * GetEffect(0)->GetTotalTicks() * int32(damage) / 100;
                        int32 heal = int32(CalculatePct(basepoints0, 15));

                        caster->CastCustomSpell(target, 63675, &basepoints0, NULL, NULL, true, NULL, GetEffect(0));
                        caster->CastCustomSpell(caster, 75999, &heal, NULL, NULL, true, NULL, GetEffect(0));
                    }
                }
                // Power Word: Shield
                else if (m_spellInfo->SpellFamilyFlags[0] & 0x1 && m_spellInfo->SpellFamilyFlags[2] & 0x400 && GetEffect(0))
                {
                    // Glyph of Power Word: Shield
                    if (AuraEffect* glyph = caster->GetAuraEffect(55672, 0))
                    {
                        // instantly heal m_amount% of the absorb-value
                        int32 heal = glyph->GetAmount() * GetEffect(0)->GetAmount()/100;
                        caster->CastCustomSpell(GetUnitOwner(), 56160, &heal, NULL, NULL, true, 0, GetEffect(0));
                    }
                }
                break;
        }
    }
    // mods at aura remove
    else
    {
        switch (GetSpellInfo()->SpellFamilyName)
        {
            case SPELLFAMILY_GENERIC:
                switch (GetId())
                {
                    case 103434: // Disrupting Shadows, Warlord Zon'ozz, Dragon Soul
                    case 104599:
                    case 104600:
                    case 104601:
                        if (removeMode == AURA_REMOVE_BY_ENEMY_SPELL)
                            target->CastSpell(target, 103948, true);
                        break;
                    case 99516: // Final Countdown dmg, Baleroc
                        if (target)
                        {
                            target->RemoveAura(99519);
                            if (removeMode == AURA_REMOVE_BY_EXPIRE)
                                target->CastSpell(target, 99518, true);
                        }
                        break;
                    case 99256: // Torment, Baleroc
                    case 100230:
                    case 100231:
                    case 100232:
                        if (removeMode == AURA_REMOVE_BY_EXPIRE)
                            target->CastSpell(target, 99257, true);
                        break;
                    case 99263: // Vital Flame, Baleroc
                    {//if (InstanceScript* pInstance = target->GetInstanceScript())
                     //if (pInstance->GetBossState(4) == IN_PROGRESS)
                        uint8 stacks = GetEffect(EFFECT_0)->GetAmount() / 5;
                        target->CastCustomSpell(99262, SPELLVALUE_AURA_STACK, stacks ? stacks : 1, target, true);
                        break;
                    }
                    case 82705: // Finkle's Mixture, Chimaeron
                        target->RemoveAurasDueToSpell(89084);
                        break;
                    case 61987: // Avenging Wrath
                        // Remove the immunity shield marker on Avenging Wrath removal if Forbearance is not present
                        if (target->HasAura(61988) && !target->HasAura(25771))
                            target->RemoveAura(61988);
                        break;
                    case 72368: // Shared Suffering
                    case 72369:
                        if (caster)
                        {
                            if (AuraEffect* aurEff = GetEffect(0))
                            {
                                int32 remainingDamage = aurEff->GetAmount() * (aurEff->GetTotalTicks() - aurEff->GetTickNumber());
                                if (remainingDamage > 0)
                                    caster->CastCustomSpell(caster, 72373, NULL, &remainingDamage, NULL, true);
                            }
                        }
                        break;
                    case 45839: // Vengeance of the Blue Flight
                        if (caster)
                            caster->RemoveAurasDueToSpell(GetId());
                        break;
                    case 53768: // Haunted
                        target->CastSpell(target, 54041, true); // Remove Haunted Memento
                        break;
                    case 75731: // Instant Statue
                        target->ExitVehicle();
                        target->RemoveAurasDueToSpell(74890); // Instant Statue
                        target->RemoveAurasDueToSpell(75055); // Instant Statue
                        break;
                    case 50101: // Ethereal Pet OnKill Steal Essence
                        if (target != caster)
                            caster->CastSpell(caster, 50063, true);
                        break;
                    case 30019: // Control Piece
                        caster->RemoveAurasDueToSpell(30532); // In Game
                        if (target != caster)
                            caster->RemoveAurasDueToSpell(GetId());
                        else if (Unit* charm = caster->GetCharm())
                        {
                            charm->RemoveAurasDueToSpell(GetId());
                            caster->CastSpell(caster, 30529, true); // Recently In Game
                        }
                        break;
                }
                break;
            case SPELLFAMILY_WARLOCK:
                if (!caster)
                    break;
                // Improved Fear
                if (GetSpellInfo()->SpellFamilyFlags[1] & 0x00000400)
                {
                    if (AuraEffect* aurEff = caster->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_WARLOCK, 98, 0))
                    {
                        uint32 spellId = 0;
                        switch (aurEff->GetId())
                        {
                            case 53759: spellId = 60947; break;
                            case 53754: spellId = 60946; break;
                            default:
                                TC_LOG_ERROR("spells", "Aura::HandleAuraSpecificMods: Unknown rank of Improved Fear (%d) found", aurEff->GetId());
                        }
                        if (spellId)
                            caster->CastSpell(target, spellId, true);
                    }
                }
                break;
            case SPELLFAMILY_PALADIN:
                // Remove the immunity shield marker on Forbearance removal if AW marker is not present
                if (GetId() == 25771 && target->HasAura(61988) && !target->HasAura(61987))
                    target->RemoveAura(61988);
                break;
            case SPELLFAMILY_DEATHKNIGHT:
                switch (GetSpellInfo()->Id)
                {
                    case 81256: // Dancing Rune Weapon
                    {
                        // Item - Death Knight T12 Tank 4P Bonus
                        if (target->HasAura(98966) && (removeMode == AURA_REMOVE_BY_EXPIRE))
                            target->CastSpell(target, 101162, true); // +15% parry
                        break;
                    }
                    default:
                        break;
                }
                break;
            case SPELLFAMILY_HUNTER:
                // Glyph of Freezing Trap
                if (GetSpellInfo()->SpellFamilyFlags[0] & 0x00000008)
                    if (caster && caster->HasAura(56845))
                        target->CastSpell(target, 61394, true);
                break;
        }
    }

    // mods at aura apply or remove
    switch (GetSpellInfo()->SpellFamilyName)
    {
        case SPELLFAMILY_DRUID:
            // Enrage
            if ((GetSpellInfo()->SpellFamilyFlags[0] & 0x80000) && GetSpellInfo()->SpellIconID == 961)
            {
                if (target->HasAura(70726)) // Item - Druid T10 Feral 4P Bonus
                    if (apply)
                        target->CastSpell(target, 70725, true);
                break;
            }
            break;
        case SPELLFAMILY_PALADIN:
            switch (GetId())
            {
                case 31842: // Divine Favor
                    // Item - Paladin T10 Holy 2P Bonus
                    if (target->HasAura(70755))
                    {
                        if (apply)
                            target->CastSpell(target, 71166, true);
                        else
                            target->RemoveAurasDueToSpell(71166);
                    }
                    break;
            }
            break;
    }
}

bool Aura::CanBeAppliedOn(Unit* target)
{
    // unit not in world or during remove from world
    if (!target->IsInWorld() || target->IsDuringRemoveFromWorld())
    {
        // area auras mustn't be applied
        if (GetOwner() != target)
            return false;
        // not selfcasted single target auras mustn't be applied
        if (GetCasterGUID() != GetOwner()->GetGUID() && GetSpellInfo()->IsSingleTarget())
            return false;
        return true;
    }
    else
    {
        if (GetSpellInfo()->AttributesEx7 & SPELL_ATTR7_CONSOLIDATED_RAID_BUFF && target->IsPetGuardianStuff())
            return false;

        return CheckAreaTarget(target);
    }
}

bool Aura::CheckAreaTarget(Unit* target)
{
    return CallScriptCheckAreaTargetHandlers(target);
}

bool Aura::CanStackWith(Aura const* existingAura) const
{
    // Can stack with self
    if (this == existingAura)
        return true;

    // Dynobj auras always stack
    if (GetType() == DYNOBJ_AURA_TYPE || existingAura->GetType() == DYNOBJ_AURA_TYPE)
        return true;

    SpellInfo const* existingSpellInfo = existingAura->GetSpellInfo();
    bool sameCaster = GetCasterGUID() == existingAura->GetCasterGUID();

    // passive auras don't stack with another rank of the spell cast by same caster
    if (IsPassive() && sameCaster && m_spellInfo->IsDifferentRankOf(existingSpellInfo))
        return false;

    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        // prevent remove triggering aura by triggered aura
        if (existingSpellInfo->Effects[i].TriggerSpell == GetId()
            // prevent remove triggered aura by triggering aura refresh
            || m_spellInfo->Effects[i].TriggerSpell == existingAura->GetId())
            return true;
    }

    // check spell specific stack rules
    if (m_spellInfo->IsAuraExclusiveBySpecificWith(existingSpellInfo)
        || (sameCaster && m_spellInfo->IsAuraExclusiveBySpecificPerCasterWith(existingSpellInfo)))
        return false;

    // check spell group stack rules
    SpellGroupStackRule stackRule = sSpellMgr->CheckSpellGroupStackRules(m_spellInfo, existingSpellInfo);
    if (stackRule)
    {
        if (stackRule == SPELL_GROUP_STACK_RULE_EXCLUSIVE)
            return false;
        if (sameCaster && stackRule == SPELL_GROUP_STACK_RULE_EXCLUSIVE_FROM_SAME_CASTER)
            return false;
    }

    if (m_spellInfo->SpellFamilyName != existingSpellInfo->SpellFamilyName)
        return true;

    if (!sameCaster)
    {
        // Channeled auras can stack if not forbidden by db or aura type
        if (existingAura->GetSpellInfo()->IsChanneled())
            return true;

        if (m_spellInfo->AttributesEx3 & SPELL_ATTR3_STACK_FOR_DIFF_CASTERS)
            return true;

        // check same periodic auras
        for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            switch (m_spellInfo->Effects[i].ApplyAuraName)
            {
                // DOT or HOT from different casters will stack
                case SPELL_AURA_PERIODIC_DAMAGE:
                case SPELL_AURA_PERIODIC_DUMMY:
                case SPELL_AURA_PERIODIC_HEAL:
                case SPELL_AURA_PERIODIC_TRIGGER_SPELL:
                case SPELL_AURA_PERIODIC_ENERGIZE:
                case SPELL_AURA_PERIODIC_MANA_LEECH:
                case SPELL_AURA_PERIODIC_LEECH:
                case SPELL_AURA_POWER_BURN:
                case SPELL_AURA_OBS_MOD_POWER:
                case SPELL_AURA_OBS_MOD_HEALTH:
                case SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE:
                    // periodic auras which target areas are not allowed to stack this way (replenishment for example)
                    if (m_spellInfo->Effects[i].IsTargetingArea() || existingSpellInfo->Effects[i].IsTargetingArea())
                        break;
                    return true;
                case SPELL_AURA_BYPASS_ARMOR_FOR_CASTER:
                case SPELL_AURA_MOD_DAMAGE_FROM_CASTER:
                case SPELL_AURA_MOD_CRIT_CHANCE_FOR_CASTER:
                    return true;
                default:
                    break;
            }
        }
    }

    if (HasEffectType(SPELL_AURA_CONTROL_VEHICLE) && existingAura->HasEffectType(SPELL_AURA_CONTROL_VEHICLE))
    {
        Vehicle* veh = NULL;
        if (GetOwner()->ToUnit())
            veh = GetOwner()->ToUnit()->GetVehicleKit();

        if (!veh)           // We should probably just let it stack. Vehicle system will prevent undefined behaviour later
            return true;

        if (!veh->GetAvailableSeatCount())
            return false;   // No empty seat available

        return true; // Empty seat available (skip rest)
    }

    // spell of same spell rank chain
    if (m_spellInfo->IsRankOf(existingSpellInfo))
    {
        // Same poison from different casters
        if (!sameCaster && m_spellInfo->Dispel == DISPEL_POISON)
            return true;

        // don't allow passive area auras to stack
        if (m_spellInfo->IsMultiSlotAura() && !IsArea())
            return true;
        if (GetCastItemGUID() && existingAura->GetCastItemGUID())
            if (GetCastItemGUID() != existingAura->GetCastItemGUID() && (m_spellInfo->AttributesCu & SPELL_ATTR0_CU_ENCHANT_PROC))
                return true;
        // same spell with same caster should not stack
        return false;
    }

    return true;
}

bool Aura::IsProcOnCooldown() const
{
    /*if (m_procCooldown)
    {
        if (m_procCooldown > time(NULL))
            return true;
    }*/
    return false;
}

void Aura::AddProcCooldown(uint32 /*msec*/)
{
    //m_procCooldown = time(NULL) + msec;
}

void Aura::PrepareProcToTrigger(AuraApplication* aurApp, ProcEventInfo& eventInfo)
{
    bool prepare = CallScriptPrepareProcHandlers(aurApp, eventInfo);
    if (!prepare)
        return;

    // take one charge, aura expiration will be handled in Aura::TriggerProcOnEvent (if needed)
    if (IsUsingCharges())
    {
        --m_procCharges;
        SetNeedClientUpdateForTargets();
    }

    SpellProcEntry const* procEntry = sSpellMgr->GetSpellProcEntry(GetId());

    ASSERT(procEntry);

    // cooldowns should be added to the whole aura (see 51698 area aura)
    AddProcCooldown(procEntry->cooldown);
}

bool Aura::IsProcTriggeredOnEvent(AuraApplication* aurApp, ProcEventInfo& eventInfo) const
{
    SpellProcEntry const* procEntry = sSpellMgr->GetSpellProcEntry(GetId());
    // only auras with spell proc entry can trigger proc
    if (!procEntry)
        return false;

    // check if we have charges to proc with
    if (IsUsingCharges() && !GetCharges())
        return false;

    // check proc cooldown
    if (IsProcOnCooldown())
        return false;

    /// @todo
    // something about triggered spells triggering, and add extra attack effect

    // do checks against db data
    if (!sSpellMgr->CanSpellTriggerProcOnEvent(*procEntry, eventInfo))
        return false;

    // do checks using conditions table
    if (!sConditionMgr->IsObjectMeetingNotGroupedConditions(CONDITION_SOURCE_TYPE_SPELL_PROC, GetId(), eventInfo.GetActor(), eventInfo.GetActionTarget()))    
        return false;

    // AuraScript Hook
    bool check = const_cast<Aura*>(this)->CallScriptCheckProcHandlers(aurApp, eventInfo);
    if (!check)
        return false;

    /// @todo
    // do allow additional requirements for procs
    // this is needed because this is the last moment in which you can prevent aura charge drop on proc
    // and possibly a way to prevent default checks (if there're going to be any)

    // Check if current equipment meets aura requirements
    // do that only for passive spells
    /// @todo this needs to be unified for all kinds of auras
    Unit* target = aurApp->GetTarget();
    if (IsPassive() && target->GetTypeId() == TYPEID_PLAYER)
    {
        if (GetSpellInfo()->EquippedItemClass == ITEM_CLASS_WEAPON)
        {
            if (target->ToPlayer()->IsInFeralForm())
                return false;

            if (eventInfo.GetDamageInfo())
            {
                WeaponAttackType attType = eventInfo.GetDamageInfo()->GetAttackType();
                Item* item = NULL;
                if (attType == BASE_ATTACK || attType == RANGED_ATTACK)
                    item = target->ToPlayer()->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
                else if (attType == OFF_ATTACK)
                    item = target->ToPlayer()->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);

                if (!item || item->IsBroken() || item->GetTemplate()->Class != ITEM_CLASS_WEAPON || !((1<<item->GetTemplate()->SubClass) & GetSpellInfo()->EquippedItemSubClassMask))
                    return false;
            }
        }
        else if (GetSpellInfo()->EquippedItemClass == ITEM_CLASS_ARMOR)
        {
            // Check if player is wearing shield
            Item* item = target->ToPlayer()->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || item->IsBroken() || item->GetTemplate()->Class != ITEM_CLASS_ARMOR || !((1<<item->GetTemplate()->SubClass) & GetSpellInfo()->EquippedItemSubClassMask))
                return false;
        }
    }

    return roll_chance_f(CalcProcChance(*procEntry, eventInfo));
}

float Aura::CalcProcChance(SpellProcEntry const& procEntry, ProcEventInfo& eventInfo) const
{
    float chance = procEntry.chance;
    // calculate chances depending on unit with caster's data
    // so talents modifying chances and judgements will have properly calculated proc chance
    if (Unit* caster = GetCaster())
    {
        // calculate ppm chance if present and we're using weapon
        if (eventInfo.GetDamageInfo() && procEntry.ratePerMinute != 0)
        {
            uint32 WeaponSpeed = caster->GetAttackTime(eventInfo.GetDamageInfo()->GetAttackType());
            chance = caster->GetPPMProcChance(WeaponSpeed, procEntry.ratePerMinute, GetSpellInfo());
        }
        // apply chance modifer aura, applies also to ppm chance (see improved judgement of light spell)
        if (Player* modOwner = caster->GetSpellModOwner())
            modOwner->ApplySpellMod(GetId(), SPELLMOD_CHANCE_OF_SUCCESS, chance);
    }
    return chance;
}

void Aura::_DeleteRemovedApplications()
{
    while (!m_removedApplications.empty())
    {
        delete m_removedApplications.front();
        m_removedApplications.pop_front();
    }
}

void Aura::LoadScripts()
{
    sScriptMgr->CreateAuraScripts(m_spellInfo->Id, m_loadedScripts);
    for (std::list<AuraScript*>::iterator itr = m_loadedScripts.begin(); itr != m_loadedScripts.end();)
    {
        if (!(*itr)->_Load(this))
        {
            std::list<AuraScript*>::iterator bitr = itr;
            ++itr;
            delete (*bitr);
            m_loadedScripts.erase(bitr);
            continue;
        }
        TC_LOG_DEBUG("spells", "Aura::LoadScripts: Script `%s` for aura `%u` is loaded now", (*itr)->_GetScriptName()->c_str(), m_spellInfo->Id);
        (*itr)->Register();
        ++itr;
    }
}

void Aura::CallScriptRefreshChargesHandlers(uint8& charges)
{
    for (auto&& script : m_loadedScripts)
    {
        script->_PrepareScriptCall(AURA_SCRIPT_HOOK_REFRESH_CHARGES);
        for (auto &hook : script->OnRefreshCharges)
            hook.Call(script, charges);
        script->_FinishScriptCall();
    }
}

void Aura::CallScriptInitEffectsHandlers(uint32& effectMask)
{
    for (auto&& script : m_loadedScripts)
    {
        script->_PrepareScriptCall(AURA_SCRIPT_HOOK_INIT_EFFECTS);
        for (auto&& hook : script->OnInitEffects)
            hook.Call(script, effectMask);
        script->_FinishScriptCall();
    }
}

bool Aura::CallScriptCheckAreaTargetHandlers(Unit* target)
{
    bool result = true;
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_CHECK_AREA_TARGET);
        std::list<AuraScript::CheckAreaTargetHandler>::iterator hookItrEnd = (*scritr)->DoCheckAreaTarget.end(), hookItr = (*scritr)->DoCheckAreaTarget.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            result &= hookItr->Call(*scritr, target);

        (*scritr)->_FinishScriptCall();
    }
    return result;
}

void Aura::CallScriptDispel(DispelInfo* dispelInfo)
{
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_DISPEL);
        std::list<AuraScript::AuraDispelHandler>::iterator hookItrEnd = (*scritr)->OnDispel.end(), hookItr = (*scritr)->OnDispel.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            hookItr->Call(*scritr, dispelInfo);

        (*scritr)->_FinishScriptCall();
    }
}

void Aura::CallScriptAfterDispel(DispelInfo* dispelInfo)
{
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_AFTER_DISPEL);
        std::list<AuraScript::AuraDispelHandler>::iterator hookItrEnd = (*scritr)->AfterDispel.end(), hookItr = (*scritr)->AfterDispel.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            hookItr->Call(*scritr, dispelInfo);

        (*scritr)->_FinishScriptCall();
    }
}

bool Aura::CallScriptEffectApplyHandlers(AuraEffect const* aurEff, AuraApplication const* aurApp, AuraEffectHandleModes mode)
{
    bool preventDefault = false;
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_APPLY, aurApp);
        std::list<AuraScript::EffectApplyHandler>::iterator effEndItr = (*scritr)->OnEffectApply.end(), effItr = (*scritr)->OnEffectApply.begin();
        for (; effItr != effEndItr; ++effItr)
            if (effItr->IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                effItr->Call(*scritr, aurEff, mode);

        if (!preventDefault)
            preventDefault = (*scritr)->_IsDefaultActionPrevented();

        (*scritr)->_FinishScriptCall();
    }

    return preventDefault;
}

bool Aura::CallScriptEffectRemoveHandlers(AuraEffect const* aurEff, AuraApplication const* aurApp, AuraEffectHandleModes mode)
{
    bool preventDefault = false;
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_REMOVE, aurApp);
        std::list<AuraScript::EffectApplyHandler>::iterator effEndItr = (*scritr)->OnEffectRemove.end(), effItr = (*scritr)->OnEffectRemove.begin();
        for (; effItr != effEndItr; ++effItr)
            if (effItr->IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                effItr->Call(*scritr, aurEff, mode);

        if (!preventDefault)
            preventDefault = (*scritr)->_IsDefaultActionPrevented();

        (*scritr)->_FinishScriptCall();
    }
    return preventDefault;
}

void Aura::CallScriptAfterEffectApplyHandlers(AuraEffect const* aurEff, AuraApplication const* aurApp, AuraEffectHandleModes mode)
{
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_AFTER_APPLY, aurApp);
        std::list<AuraScript::EffectApplyHandler>::iterator effEndItr = (*scritr)->AfterEffectApply.end(), effItr = (*scritr)->AfterEffectApply.begin();
        for (; effItr != effEndItr; ++effItr)
            if (effItr->IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                effItr->Call(*scritr, aurEff, mode);

        (*scritr)->_FinishScriptCall();
    }
}

void Aura::CallScriptAfterEffectRemoveHandlers(AuraEffect const* aurEff, AuraApplication const* aurApp, AuraEffectHandleModes mode)
{
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_AFTER_REMOVE, aurApp);
        std::list<AuraScript::EffectApplyHandler>::iterator effEndItr = (*scritr)->AfterEffectRemove.end(), effItr = (*scritr)->AfterEffectRemove.begin();
        for (; effItr != effEndItr; ++effItr)
            if (effItr->IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                effItr->Call(*scritr, aurEff, mode);

        (*scritr)->_FinishScriptCall();
    }
}

bool Aura::CallScriptEffectPeriodicHandlers(AuraEffect const* aurEff, AuraApplication const* aurApp)
{
    bool preventDefault = false;
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_PERIODIC, aurApp);
        std::list<AuraScript::EffectPeriodicHandler>::iterator effEndItr = (*scritr)->OnEffectPeriodic.end(), effItr = (*scritr)->OnEffectPeriodic.begin();
        for (; effItr != effEndItr; ++effItr)
            if (effItr->IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                effItr->Call(*scritr, aurEff);

        if (!preventDefault)
            preventDefault = (*scritr)->_IsDefaultActionPrevented();

        (*scritr)->_FinishScriptCall();
    }

    return preventDefault;
}

void Aura::CallScriptAuraUpdateHandlers(uint32 diff)
{
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_PERIODIC);
        std::list<AuraScript::AuraUpdateHandler>::iterator effEndItr = (*scritr)->OnAuraUpdate.end(), effItr = (*scritr)->OnAuraUpdate.begin();
        for (; effItr != effEndItr; ++effItr)
            effItr->Call(*scritr, diff);

        (*scritr)->_FinishScriptCall();
    }
}

void Aura::CallScriptEffectUpdateHandlers(uint32 diff, AuraEffect* aurEff)
{
    for (auto&& script : m_loadedScripts)
    {
        script->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_UPDATE);
        for (auto &hook : script->OnEffectUpdate)
            if (hook.IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                hook.Call(script, diff, aurEff);
        script->_FinishScriptCall();
    }
}

void Aura::CallScriptEffectUpdatePeriodicHandlers(AuraEffect* aurEff)
{
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_UPDATE_PERIODIC);
        std::list<AuraScript::EffectUpdatePeriodicHandler>::iterator effEndItr = (*scritr)->OnEffectUpdatePeriodic.end(), effItr = (*scritr)->OnEffectUpdatePeriodic.begin();
        for (; effItr != effEndItr; ++effItr)
            if (effItr->IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                effItr->Call(*scritr, aurEff);

        (*scritr)->_FinishScriptCall();
    }
}

void Aura::CallScriptEffectCalcAmountHandlers(AuraEffect const* aurEff, float& amount, bool & canBeRecalculated)
{
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_CALC_AMOUNT);
        std::list<AuraScript::EffectCalcAmountHandler>::iterator effEndItr = (*scritr)->DoEffectCalcAmount.end(), effItr = (*scritr)->DoEffectCalcAmount.begin();
        for (; effItr != effEndItr; ++effItr)
            if (effItr->IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                effItr->Call(*scritr, aurEff, amount, canBeRecalculated);

        (*scritr)->_FinishScriptCall();
    }
}

void Aura::CallScriptEffectAfterCalcAmountHandlers(AuraEffect const* aurEff, float& amount, bool& canBeRecalculated)
{
    for (auto&& script : m_loadedScripts)
    {
        script->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_CALC_AMOUNT);
        for (auto&& it : script->AfterEffectCalcAmount)
            if (it.IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                it.Call(script, aurEff, amount, canBeRecalculated);

        script->_FinishScriptCall();
    }
}

void Aura::CallScriptEffectCalcPeriodicHandlers(AuraEffect const* aurEff, bool & isPeriodic, int32 & amplitude)
{
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_CALC_PERIODIC);
        std::list<AuraScript::EffectCalcPeriodicHandler>::iterator effEndItr = (*scritr)->DoEffectCalcPeriodic.end(), effItr = (*scritr)->DoEffectCalcPeriodic.begin();
        for (; effItr != effEndItr; ++effItr)
            if (effItr->IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                effItr->Call(*scritr, aurEff, isPeriodic, amplitude);

        (*scritr)->_FinishScriptCall();
    }
}

void Aura::CallScriptEffectCalcTickAmountHandlers(AuraEffect const* eff, AuraApplication* app, int32& amount)
{
    for (auto&& script : m_loadedScripts)
    {
        script->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_CALC_TICK_AMOUNT, app);
        for (auto&& it : script->DoEffectCalcTickAmount)
            if (it.IsEffectAffected(m_spellInfo, eff->GetEffIndex()))
                it.Call(script, eff, amount);

        script->_FinishScriptCall();
    }
}

void Aura::CallScriptEffectCalcSpellModHandlers(AuraEffect const* aurEff, SpellModifier* & spellMod)
{
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_CALC_SPELLMOD);
        std::list<AuraScript::EffectCalcSpellModHandler>::iterator effEndItr = (*scritr)->DoEffectCalcSpellMod.end(), effItr = (*scritr)->DoEffectCalcSpellMod.begin();
        for (; effItr != effEndItr; ++effItr)
            if (effItr->IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                effItr->Call(*scritr, aurEff, spellMod);

        (*scritr)->_FinishScriptCall();
    }
}

void Aura::CallScriptEffectAbsorbHandlers(AuraEffect* aurEff, AuraApplication const* aurApp, DamageInfo & dmgInfo, uint32 & absorbAmount, bool& defaultPrevented)
{
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_ABSORB, aurApp);
        std::list<AuraScript::EffectAbsorbHandler>::iterator effEndItr = (*scritr)->OnEffectAbsorb.end(), effItr = (*scritr)->OnEffectAbsorb.begin();
        for (; effItr != effEndItr; ++effItr)

            if (effItr->IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                effItr->Call(*scritr, aurEff, dmgInfo, absorbAmount);

        if (!defaultPrevented)
            defaultPrevented = (*scritr)->_IsDefaultActionPrevented();

        (*scritr)->_FinishScriptCall();
    }
}

void Aura::CallScriptEffectAbsorbHealHandler(AuraEffect* aurEff, AuraApplication const* aurApp, uint32& heal, uint32& absorb, bool& defaultPrevented)
{
    for (auto&& script : m_loadedScripts)
    {
        script->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_ABSORB_HEAL, aurApp);
        for (auto&& itr : script->OnEffectAbsorbHeal)
            if (itr.IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                itr.Call(script, aurEff, heal, absorb);

        if (!defaultPrevented)
            defaultPrevented = script->_IsDefaultActionPrevented();

        script->_FinishScriptCall();
    }
}

void Aura::CallScriptEffectAfterAbsorbHandlers(AuraEffect* aurEff, AuraApplication const* aurApp, DamageInfo & dmgInfo, uint32 & absorbAmount)
{
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_AFTER_ABSORB, aurApp);
        std::list<AuraScript::EffectAbsorbHandler>::iterator effEndItr = (*scritr)->AfterEffectAbsorb.end(), effItr = (*scritr)->AfterEffectAbsorb.begin();
        for (; effItr != effEndItr; ++effItr)
            if (effItr->IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                effItr->Call(*scritr, aurEff, dmgInfo, absorbAmount);

        (*scritr)->_FinishScriptCall();
    }
}

void Aura::CallScriptEffectManaShieldHandlers(AuraEffect* aurEff, AuraApplication const* aurApp, DamageInfo & dmgInfo, uint32 & absorbAmount, bool & /*defaultPrevented*/)
{
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_MANASHIELD, aurApp);
        std::list<AuraScript::EffectManaShieldHandler>::iterator effEndItr = (*scritr)->OnEffectManaShield.end(), effItr = (*scritr)->OnEffectManaShield.begin();
        for (; effItr != effEndItr; ++effItr)
            if (effItr->IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                effItr->Call(*scritr, aurEff, dmgInfo, absorbAmount);

        (*scritr)->_FinishScriptCall();
    }
}

void Aura::CallScriptEffectAfterManaShieldHandlers(AuraEffect* aurEff, AuraApplication const* aurApp, DamageInfo & dmgInfo, uint32 & absorbAmount)
{
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_AFTER_MANASHIELD, aurApp);
        std::list<AuraScript::EffectManaShieldHandler>::iterator effEndItr = (*scritr)->AfterEffectManaShield.end(), effItr = (*scritr)->AfterEffectManaShield.begin();
        for (; effItr != effEndItr; ++effItr)
            if (effItr->IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                effItr->Call(*scritr, aurEff, dmgInfo, absorbAmount);

        (*scritr)->_FinishScriptCall();
    }
}

void Aura::CallScriptEffectSplitHandlers(AuraEffect* aurEff, AuraApplication const* aurApp, DamageInfo & dmgInfo, uint32 & splitAmount)
{
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_SPLIT, aurApp);
        std::list<AuraScript::EffectSplitHandler>::iterator effEndItr = (*scritr)->OnEffectSplit.end(), effItr = (*scritr)->OnEffectSplit.begin();
        for (; effItr != effEndItr; ++effItr)
            if (effItr->IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                effItr->Call(*scritr, aurEff, dmgInfo, splitAmount);

        (*scritr)->_FinishScriptCall();
    }
}

void Aura::SetScriptData(uint32 type, uint32 data)
{
    for (auto &script : m_loadedScripts)
        script->SetData(type, data);
}

void Aura::SetScriptGuid(uint32 type, ObjectGuid data)
{
    for (auto &script : m_loadedScripts)
        script->SetGuid(type, data);
}

bool Aura::CallScriptCheckProcHandlers(AuraApplication const* aurApp, ProcEventInfo& eventInfo)
{
    bool result = true;
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_CHECK_PROC, aurApp);
        std::list<AuraScript::CheckProcHandler>::iterator hookItrEnd = (*scritr)->DoCheckProc.end(), hookItr = (*scritr)->DoCheckProc.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            result &= hookItr->Call(*scritr, eventInfo);

        (*scritr)->_FinishScriptCall();
    }

    return result;
}

bool Aura::CallScriptPrepareProcHandlers(AuraApplication const* aurApp, ProcEventInfo& eventInfo)
{
    bool prepare = true;
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_PREPARE_PROC, aurApp);
        std::list<AuraScript::AuraProcHandler>::iterator effEndItr = (*scritr)->DoPrepareProc.end(), effItr = (*scritr)->DoPrepareProc.begin();
        for (; effItr != effEndItr; ++effItr)
            effItr->Call(*scritr, eventInfo);

        if (prepare)
            prepare = !(*scritr)->_IsDefaultActionPrevented();

        (*scritr)->_FinishScriptCall();
    }

    return prepare;
}

bool Aura::CallScriptProcHandlers(AuraApplication const* aurApp, ProcEventInfo& eventInfo)
{
    bool handled = false;
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_PROC, aurApp);
        std::list<AuraScript::AuraProcHandler>::iterator hookItrEnd = (*scritr)->OnProc.end(), hookItr = (*scritr)->OnProc.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            hookItr->Call(*scritr, eventInfo);

        handled |= (*scritr)->_IsDefaultActionPrevented();
        (*scritr)->_FinishScriptCall();
    }

    return handled;
}

void Aura::CallScriptAfterProcHandlers(AuraApplication const* aurApp, ProcEventInfo& eventInfo)
{
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_AFTER_PROC, aurApp);
        std::list<AuraScript::AuraProcHandler>::iterator hookItrEnd = (*scritr)->AfterProc.end(), hookItr = (*scritr)->AfterProc.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            hookItr->Call(*scritr, eventInfo);

        (*scritr)->_FinishScriptCall();
    }
}

bool Aura::CallScriptEffectProcHandlers(AuraEffect const* aurEff, AuraApplication const* aurApp, ProcEventInfo& eventInfo)
{
    bool preventDefault = false;
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_PROC, aurApp);
        std::list<AuraScript::EffectProcHandler>::iterator effEndItr = (*scritr)->OnEffectProc.end(), effItr = (*scritr)->OnEffectProc.begin();
        for (; effItr != effEndItr; ++effItr)
            if (effItr->IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                effItr->Call(*scritr, aurEff, eventInfo);

        if (!preventDefault)
            preventDefault = (*scritr)->_IsDefaultActionPrevented();

        (*scritr)->_FinishScriptCall();
    }
    return preventDefault;
}

void Aura::CallScriptAfterEffectProcHandlers(AuraEffect const* aurEff, AuraApplication const* aurApp, ProcEventInfo& eventInfo)
{
    for (std::list<AuraScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_AFTER_PROC, aurApp);
        std::list<AuraScript::EffectProcHandler>::iterator effEndItr = (*scritr)->AfterEffectProc.end(), effItr = (*scritr)->AfterEffectProc.begin();
        for (; effItr != effEndItr; ++effItr)
            if (effItr->IsEffectAffected(m_spellInfo, aurEff->GetEffIndex()))
                effItr->Call(*scritr, aurEff, eventInfo);

        (*scritr)->_FinishScriptCall();
    }
}

bool Aura::CallScriptAuraDropModChargeHandlers(Spell* bySpell)
{
    bool prevented = false;
    for (auto&& script : m_loadedScripts)
    {
        script->_PrepareScriptCall(AURA_SCRIPT_HOOK_EFFECT_DROP_MOD_CHARGE);
        for (auto&& hook : script->OnDropModCharge)
        {
            hook.Call(script, bySpell);
            if (!prevented)
                prevented = script->_IsDefaultActionPrevented();
        }
        script->_FinishScriptCall();
    }
    return prevented;
}

UnitAura::UnitAura(SpellInfo const* spellproto, uint32 effMask, WorldObject* owner, Unit* caster, int32 *baseAmount, Item* castItem, ObjectGuid casterGUID)
    : Aura(spellproto, owner, caster, castItem, casterGUID, effMask, baseAmount)
{
    m_AuraDRGroup = DIMINISHING_NONE;
    // LoadScripts();
    // _InitEffects(effMask, caster, baseAmount);
    GetUnitOwner()->_AddAura(this, caster);
}

void UnitAura::_ApplyForTarget(Unit* target, Unit* caster, AuraApplication * aurApp)
{
    Aura::_ApplyForTarget(target, caster, aurApp);

    // register aura diminishing on apply
    if (DiminishingGroup group = GetDiminishGroup())
        target->ApplyDiminishingAura(group, true);
}

void UnitAura::_UnapplyForTarget(Unit* target, Unit* caster, AuraApplication * aurApp)
{
    Aura::_UnapplyForTarget(target, caster, aurApp);

    // unregister aura diminishing (and store last time)
    if (DiminishingGroup group = GetDiminishGroup())
        target->ApplyDiminishingAura(group, false);
}

void UnitAura::Remove(AuraRemoveMode removeMode)
{
    if (IsRemoved())
        return;
    GetUnitOwner()->RemoveOwnedAura(this, removeMode);
}

void UnitAura::FillTargetMap(std::map<Unit*, uint32> & targets, Unit* caster)
{
    for (uint32 effIndex = 0; effIndex < MAX_SPELL_EFFECTS; ++effIndex)
    {
        if (!HasEffect(effIndex))
            continue;

        ConditionContainer* conditions = nullptr;
        bool area = false;

        UnitList targetList;
        // non-area aura
        if (GetSpellInfo()->Effects[effIndex].Effect == SPELL_EFFECT_APPLY_AURA)
        {
            targetList.push_back(GetUnitOwner());
        }
        else
        {
            float radius = GetSpellInfo()->Effects[effIndex].CalcRadius(caster);
            area = true;

            if ((!GetUnitOwner()->HasUnitState(UNIT_STATE_ISOLATED) || GetSpellInfo()->HasAttribute(SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)) &&
                (GetUnitOwner()->IsAlive() || GetSpellInfo()->HasAttribute(SPELL_ATTR3_DEATH_PERSISTENT)))
            {
                switch (GetSpellInfo()->Effects[effIndex].Effect)
                {
                    case SPELL_EFFECT_APPLY_AREA_AURA_PARTY:
                    case SPELL_EFFECT_APPLY_AREA_AURA_RAID:
                    {
                        targetList.push_back(GetUnitOwner());
                        Trinity::AnyGroupedUnitInObjectRangeCheck u_check(GetUnitOwner(), GetUnitOwner(), radius, GetSpellInfo()->Effects[effIndex].Effect == SPELL_EFFECT_APPLY_AREA_AURA_RAID);
                        Trinity::UnitListSearcher<Trinity::AnyGroupedUnitInObjectRangeCheck> searcher(GetUnitOwner(), targetList, u_check);
                        GetUnitOwner()->VisitNearbyObject(radius, searcher);
                        break;
                    }
                    case SPELL_EFFECT_APPLY_AREA_AURA_FRIEND:
                    {
                        targetList.push_back(GetUnitOwner());
                        Trinity::AnyFriendlyUnitInObjectRangeCheck u_check(GetUnitOwner(), GetUnitOwner(), radius);
                        Trinity::UnitListSearcher<Trinity::AnyFriendlyUnitInObjectRangeCheck> searcher(GetUnitOwner(), targetList, u_check);
                        GetUnitOwner()->VisitNearbyObject(radius, searcher);
                        break;
                    }
                    case SPELL_EFFECT_APPLY_AREA_AURA_ENEMY:
                    {
                        Trinity::AnyAoETargetUnitInObjectRangeCheck u_check(GetUnitOwner(), GetUnitOwner(), radius); // No GetCharmer in searcher
                        Trinity::UnitListSearcher<Trinity::AnyAoETargetUnitInObjectRangeCheck> searcher(GetUnitOwner(), targetList, u_check);
                        GetUnitOwner()->VisitNearbyObject(radius, searcher);
                        break;
                    }
                    case SPELL_EFFECT_APPLY_AREA_AURA_PET:
                        targetList.push_back(GetUnitOwner());
                        // no break
                    case SPELL_EFFECT_APPLY_AREA_AURA_OWNER:
                    {
                        if (Unit* owner = GetUnitOwner()->GetCharmerOrOwner())
                            if (GetUnitOwner()->IsWithinDistInMap(owner, radius))
                                targetList.push_back(owner);
                        if (targetList.empty() && GetUnitOwner()->GetTypeId() == TYPEID_PLAYER) // For Grimoire of Sacrifice (108503)
                            targetList.push_back(GetUnitOwner());
                        break;
                    }
                    case SPELL_EFFECT_APPLY_AREA_AURA_ENTRY:
                    {
                        bool canDead = GetSpellInfo()->AttributesEx3 & SPELL_ATTR3_DEATH_PERSISTENT;
                        Trinity::AnyUnitInObjectRangeCheck check{ GetUnitOwner(), radius, canDead };
                        Trinity::UnitListSearcher<Trinity::AnyUnitInObjectRangeCheck> searcher{ GetUnitOwner(), targetList, check };
                        GetUnitOwner()->VisitNearbyObject(radius, searcher);
                        break;
                    }
                }

                conditions = m_spellInfo->Effects[effIndex].ImplicitTargetConditions;
                if (conditions && conditions->empty())
                    conditions = nullptr;
            }
        }

        for (auto&& itr : targetList)
        {
            if (conditions)
            {
                ConditionSourceInfo sourceInfo = ConditionSourceInfo(itr, caster);
                if (!sConditionMgr->IsObjectMeetToConditions(sourceInfo, *conditions))
                    continue;
            }

            if (area)
            {
                if (GetSpellInfo()->HasAttribute(SPELL_ATTR1_CANT_TARGET_SELF) && itr == GetOwner())
                    continue;

                if (GetSpellInfo()->HasAttribute(SPELL_ATTR3_ONLY_TARGET_PLAYERS) && itr->GetTypeId() != TYPEID_PLAYER)
                    continue;
            }

            std::map<Unit*, uint32>::iterator existing = targets.find(itr);
            if (existing != targets.end())
                existing->second |= 1<<effIndex;
            else
                targets[itr] = 1<<effIndex;
        }
    }
}

DynObjAura::DynObjAura(SpellInfo const* spellproto, uint32 effMask, WorldObject* owner, Unit* caster, int32 *baseAmount, Item* castItem, ObjectGuid casterGUID)
    : Aura(spellproto, owner, caster, castItem, casterGUID, effMask, baseAmount)
{
    ASSERT(GetDynobjOwner());
    ASSERT(GetDynobjOwner()->IsInWorld());
    ASSERT(GetDynobjOwner()->GetMap() == caster->GetMap());
    GetDynobjOwner()->SetAura(this);
}

void DynObjAura::Remove(AuraRemoveMode removeMode)
{
    if (IsRemoved())
        return;
    _Remove(removeMode);
}

void DynObjAura::FillTargetMap(std::map<Unit*, uint32> & targets, Unit* /*caster*/)
{
    Unit* dynObjOwnerCaster = GetDynobjOwner()->GetCaster();
    float radius = GetDynobjOwner()->GetRadius();

    for (uint32 effIndex = 0; effIndex < MAX_SPELL_EFFECTS; ++effIndex)
    {
        if (!HasEffect(effIndex))
            continue;

        ConditionContainer* conditions = m_spellInfo->Effects[effIndex].ImplicitTargetConditions;
        if (conditions && conditions->empty())
            conditions = nullptr;

        UnitList targetList;
        if (GetSpellInfo()->Effects[effIndex].TargetB.GetTarget() == TARGET_DEST_DYNOBJ_ALLY
            || GetSpellInfo()->Effects[effIndex].TargetB.GetTarget() == TARGET_UNIT_DEST_AREA_ALLY)
        {
            Trinity::AnyFriendlyUnitInObjectRangeCheck u_check(GetDynobjOwner(), dynObjOwnerCaster, radius);
            Trinity::UnitListSearcher<Trinity::AnyFriendlyUnitInObjectRangeCheck> searcher(GetDynobjOwner(), targetList, u_check);
            GetDynobjOwner()->VisitNearbyObject(radius, searcher);
        }
        else if (conditions && (GetSpellInfo()->Effects[effIndex].TargetB.GetTarget() == TARGET_UNIT_SRC_AREA_ENTRY || GetSpellInfo()->Effects[effIndex].TargetB.GetTarget() == TARGET_UNIT_DEST_AREA_ENTRY))
        {
            WorldObject* owner = GetDynobjOwner();
            auto check = [=](Unit const* u) { return u->IsAlive() && owner->IsWithinDistInMap(u, radius); };
            Trinity::UnitListSearcher<decltype(check)> searcher{ GetDynobjOwner(), targetList, check };
            GetDynobjOwner()->VisitNearbyObject(radius, searcher);
        }
        else
        {
            Trinity::AnyAoETargetUnitInObjectRangeCheck u_check(GetDynobjOwner(), dynObjOwnerCaster, radius);
            Trinity::UnitListSearcher<Trinity::AnyAoETargetUnitInObjectRangeCheck> searcher(GetDynobjOwner(), targetList, u_check);
            GetDynobjOwner()->VisitNearbyObject(radius, searcher);
        }

        for (auto&& itr : targetList)
        {
            if (conditions)
            {
                ConditionSourceInfo sourceInfo = ConditionSourceInfo(itr, dynObjOwnerCaster);
                if (!sConditionMgr->IsObjectMeetToConditions(sourceInfo, *conditions))
                    continue;
            }

            if (GetSpellInfo()->HasAttribute(SPELL_ATTR1_CANT_TARGET_SELF) && itr == GetOwner())
                continue;

            if (GetSpellInfo()->HasAttribute(SPELL_ATTR3_ONLY_TARGET_PLAYERS) && itr->GetTypeId() != TYPEID_PLAYER)
                continue;

            std::map<Unit*, uint32>::iterator existing = targets.find(itr);
            if (existing != targets.end())
                existing->second |= 1<<effIndex;
            else
                targets[itr] = 1<<effIndex;
        }
    }
}
