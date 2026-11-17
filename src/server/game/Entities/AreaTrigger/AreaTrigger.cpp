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

#include "ObjectAccessor.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "Unit.h"
#include "SpellInfo.h"
#include "Log.h"
#include "AreaTrigger.h"
#include "Object.h"
#include "SpellAuraEffects.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Spline.h"

class IAreaTriggerOnceChecker
{
    public:
        IAreaTriggerOnceChecker(IAreaTriggerOnce* areaTrigger)
            : m_areaTrigger(areaTrigger)
        {
        }

        bool operator()(WorldObject* object)
        {
            if (m_areaTrigger->GetTarget()->GetExactDist2d(object) > m_areaTrigger->GetRange() + object->GetObjectSize())
                return false;
            if (std::abs(object->GetPositionZ() - m_areaTrigger->GetTarget()->GetPositionZ()) > 10.0f)
                return false;

            return m_areaTrigger->CheckTriggering(object);
        }

    private:
        IAreaTriggerOnce* m_areaTrigger;
};

class IAreaTriggerAuraUpdater
{
    public:
        IAreaTriggerAuraUpdater(IAreaTriggerAura* areaTrigger)
            : m_areaTrigger(areaTrigger)
        {
        }

        void operator()(WorldObject* object) const
        {
            // This is during WorldObjectWorker::Visit so do it right here
            if (!m_areaTrigger->GetAreaTrigger()->IsInWorld())
                return;

            if (!object->IsInWorld())
                return;

            if (m_areaTrigger->GetTarget()->GetExactDist2d(object) > m_areaTrigger->GetRange() + object->GetObjectSize())
                return;
            if (std::abs(object->GetPositionZ() - m_areaTrigger->GetTarget()->GetPositionZ()) > 10.0f)
                return;

            if (!m_areaTrigger->CheckTriggering(object))
                return;

            IAreaTriggerAura::TriggeringList::iterator iter;
            for (iter = m_areaTrigger->m_triggerings.begin(); iter != m_areaTrigger->m_triggerings.end(); ++iter)
            {
                if (iter->Guid == object->GetGUID())
                {
                    iter->UpdateNumber = m_areaTrigger->m_updateNumber;
                    m_areaTrigger->OnTriggeringUpdate(object);
                    return;
                }
            }

            IAreaTriggerAura::Triggering triggering;
            triggering.Guid = object->GetGUID();
            triggering.UpdateNumber = m_areaTrigger->m_updateNumber;
            m_areaTrigger->m_triggerings.push_back(triggering);
            m_areaTrigger->OnTriggeringApply(object);
        }

    private:
        IAreaTriggerAura* m_areaTrigger;
};

void IAreaTrigger::Initialize(AreaTrigger* trigger, Unit* caster, WorldObject* target)
{
    m_caster = caster;
    ASSERT(target);
    m_target = target;
    m_spellInfo = sSpellMgr->GetSpellInfo(trigger->GetSpellId());
    m_location = m_target;
    m_areaTrigger = trigger;
    OnInit();
}

void IAreaTrigger::OnRemoveFromWorld()
{
    m_duringRemoveFromWorld = true;
    OnDestroy();
}

void IAreaTriggerOnce::OnUpdate(uint32)
{
    WorldObject* triggering = NULL;
    IAreaTriggerOnceChecker checker(this);
    Trinity::WorldObjectSearcher<IAreaTriggerOnceChecker> searcher(m_target, triggering, checker, m_mapTypeMask);
    m_target->VisitNearbyObject(m_range, searcher);

    if (triggering)
    {
        OnTrigger(triggering);
        GetAreaTrigger()->Remove();
    }
}

void IAreaTriggerOnce::OnRemoveFromWorld()
{
    return IAreaTrigger::OnRemoveFromWorld();
}

IAreaTriggerAura::IAreaTriggerAura()
    : m_mapTypeMask(GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER)
{
}

IAreaTriggerAura::~IAreaTriggerAura()
{
    ASSERT(m_triggerings.empty());
}

void IAreaTriggerAura::OnUpdate(uint32)
{
    ++m_updateNumber;

    IAreaTriggerAuraUpdater updater(this);
    Trinity::WorldObjectWorker<IAreaTriggerAuraUpdater> worker(m_target, updater, m_mapTypeMask);
    m_target->VisitNearbyObject(m_range, worker);

    for (TriggeringList::iterator iter = m_triggerings.begin(); iter != m_triggerings.end(); )
    {
        if (iter->UpdateNumber != m_updateNumber)
        {
            // remove
            if (WorldObject* object = ObjectAccessor::GetWorldObject(*m_target, iter->Guid))
                OnTriggeringRemove(object);
            else if (Player* player = ObjectAccessor::FindConnectedPlayer(iter->Guid))
                if (player->GetMap() == m_areaTrigger->GetMap())
                    OnTriggeringRemove(player);

            iter = m_triggerings.erase(iter);
        }
        else
            ++iter;
    }
}

bool IAreaTriggerAura::IsInBox(Unit* target, float extentsX, float extentsY, float extentsZ)
{
    float halfExtentsZ = extentsZ / 2.0f;

    float minX = GetCaster()->GetPositionX() - extentsX;
    float maxX = GetCaster()->GetPositionX() + extentsX;

    float minY = GetCaster()->GetPositionY() - extentsY;
    float maxY = GetCaster()->GetPositionY() + extentsY;

    float minZ = GetCaster()->GetPositionZ() - halfExtentsZ;
    float maxZ = GetCaster()->GetPositionZ() + halfExtentsZ;

    G3D::AABox const box({ minX, minY, minZ }, { maxX, maxY, maxZ });
    return box.contains({ target->GetPositionX(), target->GetPositionY(), target->GetPositionZ() });
}

void IAreaTriggerAura::OnRemoveFromWorld()
{
    m_duringRemoveFromWorld = true;

    for (auto&& it : m_triggerings)
    {
        if (WorldObject* object = ObjectAccessor::GetWorldObject(*m_target, it.Guid))
            OnTriggeringRemove(object);
    }
    m_triggerings.clear();

    OnDestroy();
}

AreaTrigger::AreaTrigger() : AreaTrigger(nullptr)
{
}

AreaTrigger::AreaTrigger(AuraEffect const* eff)
    : WorldObject(false), m_auraOwner(eff ? eff->GetBase()->GetOwner()->GetGUID() : ObjectGuid::Empty), m_effectIndex(eff ? eff->GetEffIndex() : 0)
{
    m_objectType |= TYPEMASK_AREATRIGGER;
    m_objectTypeId = TYPEID_AREATRIGGER;

    m_updateFlag = UPDATEFLAG_STATIONARY_POSITION;

    m_valuesCount = AREATRIGGER_END;
}

AreaTrigger::~AreaTrigger()
{
    ASSERT(!m_caster);
}

void AreaTrigger::AddToWorld()
{
    ///- Register the AreaTrigger for guid lookup and for caster
    if (!IsInWorld())
    {
        GetMap()->GetObjectsStore().Insert<AreaTrigger>(GetGUID(), this);
        WorldObject::AddToWorld();
        BindToCaster();

        if (auto script = sScriptMgr->CreateAreaTriggerInterface(GetEntry()))
        {
            m_script.reset(script);
            m_script->Initialize(this, m_caster, m_effectTarget);
        }
    }
}

void AreaTrigger::RemoveFromWorld()
{
    ///- Remove the AreaTrigger from the accessor and from all lists of objects in world
    if (IsInWorld())
    {
        if (m_script)
            m_script->OnRemoveFromWorld();

        UnbindFromCaster();
        WorldObject::RemoveFromWorld();
        GetMap()->GetObjectsStore().Remove<AreaTrigger>(GetGUID());
    }
}

bool AreaTrigger::CreateAreaTrigger(uint32 guidlow, uint32 triggerEntry, Unit* caster, SpellInfo const* spell, Position const& pos, Unit* effectTarget)
{
    SetMap(caster->GetMap());
    Relocate(pos);
    if (!IsPositionValid())
    {
        TC_LOG_ERROR("misc", "AreaTrigger (spell {}) not created. Invalid coordinates (X: {} Y: {})", spell->Id, GetPositionX(), GetPositionY());
        return false;
    }

    WorldObject::_Create(guidlow, HighGuid::AreaTrigger, caster->GetPhaseMask(), caster->GetPhases());

    int32 duration = spell->GetDuration();
    if (Player* modOwner = caster->GetSpellModOwner())
        modOwner->ApplySpellMod(spell->Id, SPELLMOD_DURATION, duration);

    SetEntry(triggerEntry);
    SetDuration(duration);
    SetObjectScale(1);

    SetGuidValue(AREATRIGGER_FIELD_CASTER, caster->GetGUID());
    SetUInt32Value(AREATRIGGER_FIELD_SPELL_ID, spell->Id);
    SetUInt32Value(AREATRIGGER_FIELD_SPELL_VISUAL_ID, spell->SpellVisual[0]);
    SetInt32Value(AREATRIGGER_FIELD_DURATION, duration);
    // TODO: Find proper scaling of areatriggers
    float scale = 1.0f;
    switch (spell->Id)
    {
        case 136049:
            scale = 0.14f;
            break;
        case 133292: // Flames of Fallen Glory
            scale = 0.5f;
            break;
        default:
            scale = 1.0f;
            break;
    }

    SetFloatValue(AREATRIGGER_FIELD_EXPLICIT_SCALE, scale);

    switch (spell->Id)
    {
        case 125301: // Blade Tempest
            SetVisualRadius(12.0f);
            break;
        case 133292: // Flames of Fallen Glory
            SetVisualRadius(5.0f);
        default:
            break;
    }

    if (AreaTriggerTemplate const* atTemplate = sObjectMgr->GetAreaTriggerTemplate(triggerEntry))
    {
        m_scaleX = atTemplate->ScaleX;
        m_scaleY = atTemplate->ScaleY;
    }

    m_effectTarget = effectTarget;
    if (!m_effectTarget)
        m_effectTarget = this;

    if (!GetMap()->AddToMap(this))
        return false;

    return true;
}

void AreaTrigger::Expire()
{
    if (m_script)
        m_script->OnExpire();

    Remove();
}

void AreaTrigger::SetRadius(float radius)
{
    if (m_script)
        m_script->SetRange(radius);
}

void AreaTrigger::Update(uint32 diff)
{
    // Before Expire
    UpdateSplinePosition(diff);

    // Despawned in movement hooks
    if (!IsInWorld())
        return;

    if (GetMaxDuration() != -1)
    {
        if (GetDuration() > int32(diff))
            m_duration -= diff;
        else
            Expire();
    }

    WorldObject::Update(diff);

    if (m_script && IsInWorld())    // Expired
        m_script->OnUpdate(diff);

    if (!IsInWorld())
        return;

    auto caster = GetCaster();
    if (!caster)
    {
        Remove();
        return;
    }
}

void AreaTrigger::Remove()
{
    if (IsInWorld())
    {
        if (m_auraOwner)
        {
            Unit* auraOwner = ObjectAccessor::GetUnit(*this, m_auraOwner);
            if (!auraOwner)
            {
                TC_LOG_ERROR("shitlog", "!auraOwner, spell {}, owner " "{}" ", caster " "{}" "\n", GetSpellId(), m_auraOwner.GetRawValue(), GetCasterGUID().GetRawValue());
            }
            else
            {
                // If aura expired before it can be removed first, don't report
                if (AuraEffect* effect = auraOwner->GetAuraEffect(GetSpellId(), m_effectIndex, GetCasterGUID()))
                    effect->GetBase()->Remove();
            }
        }

        SendObjectDeSpawnAnim(GetGUID());
        RemoveFromWorld();
        AddObjectToRemoveList();
    }
}

void AreaTrigger::BindToCaster()
{
    ASSERT(!m_caster);
    m_caster = ObjectAccessor::GetUnit(*this, GetCasterGUID());
    if (m_caster)
        m_caster->RegisterAreaTrigger(this);
}

void AreaTrigger::UnbindFromCaster()
{
    if (!m_caster)
        return;

    m_caster->UnregisterAreaTrigger(this);
    m_caster = nullptr;
}

std::vector<G3D::Vector3> const& AreaTrigger::GetPath() const
{
    return m_spline->getPoints();
}

void AreaTrigger::InitSpline(std::vector<Position> const& path, int32 duration)
{
    if (path.size() < 2)
    {
        TC_LOG_ERROR("entities.areatrigger", "AreaTrigger::InitSpline - Size of path < 2 (entry: {})", GetEntry());
        return;
    }

    if (duration < 0)
    {
        TC_LOG_ERROR("entities.areatrigger", "AreaTrigger::InitSpline - duration < 0 (entry: {})", GetEntry());
        return;
    }

    std::vector<G3D::Vector3> splinePoints;
    splinePoints.reserve(path.size());
    for (auto&& itr : path)
        splinePoints.push_back({ itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ() });
    m_spline.reset(new Movement::Spline<int32>());
    m_spline->init_spline(splinePoints.data(), splinePoints.size(), GetOrientation(), Movement::SplineBase::ModeLinear);
    m_spline->initLengths();
    SetInt32Value(AREATRIGGER_FIELD_DURATION, duration);
    SetDuration(duration);
}

void AreaTrigger::AttachToTarget()
{
    m_movementInfo.transport.guid = m_effectTarget->GetGUID();
    m_movementInfo.transport.seat = 0;
    m_attachedToTransport = true;
}

void AreaTrigger::UpdateSplinePosition(uint32 diff)
{
    if (m_reachedDestination)
        return;

    if (!m_spline)
        return;

    m_movementTime += diff;

    if (int32(m_movementTime) >= GetMaxDuration())
    {
        m_reachedDestination = true;
        m_lastSplineIndex = int32(m_spline->last());

        G3D::Vector3 lastSplinePosition = m_spline->getPoint(m_lastSplineIndex);
        GetMap()->AreaTriggerRelocation(this, lastSplinePosition.x, lastSplinePosition.y, lastSplinePosition.z, GetOrientation());

        if (m_script)
        {
            m_script->OnPathPointRiched();
            m_script->OnDestinationReached();
        }
        return;
    }

    float currentTimePercent = float(m_movementTime) / float(GetMaxDuration());

    if (currentTimePercent <= 0.0f)
        return;

    /*
    if (GetMiscTemplate()->MoveCurveId)
    {
        float progress = sDB2Manager.GetCurveValueAt(GetMiscTemplate()->MoveCurveId, currentTimePercent);
        if (progress < 0.f || progress > 1.f)
        {
            TC_LOG_ERROR("entities.areatrigger", "AreaTrigger (Id: {}, SpellMiscId: {}) has wrong progress ({}) caused by curve calculation (MoveCurveId: {})",
                GetTemplate()->Id, GetMiscTemplate()->MiscId, progress, GetMiscTemplate()->MorphCurveId);
        }
        else
            currentTimePercent = progress;
    }
    */

    int32 lastPositionIndex = 0;
    float percentFromLastPoint = 0;
    m_spline->computeIndex(currentTimePercent, lastPositionIndex, percentFromLastPoint);

    G3D::Vector3 currentPosition;
    m_spline->evaluate_percent(lastPositionIndex, percentFromLastPoint, currentPosition);

    float orientation = GetOrientation();
    /*
    if (GetTemplate()->HasFlag(AREATRIGGER_FLAG_HAS_FACE_MOVEMENT_DIR))
    {
        G3D::Vector3 const& nextPoint = _spline->getPoint(lastPositionIndex + 1);
        orientation = GetAngle(nextPoint.x, nextPoint.y);
    }
    */
    GetMap()->AreaTriggerRelocation(this, currentPosition.x, currentPosition.y, currentPosition.z, orientation);

    if (m_lastSplineIndex != lastPositionIndex)
    {
        m_lastSplineIndex = lastPositionIndex;
        if (m_script)
            m_script->OnPathPointRiched();
    }
}

void AreaTrigger::BuildValuesUpdate(uint8 updateType, ByteBuffer* data, Player* target) const
{
    if (!target)
        return;

    UpdateBuilder builder;
    builder.SetSource(updateType == UPDATETYPE_VALUES ? _changesMask.GetBits() : m_uint32Values, m_valuesCount);
    builder.SetDest(data);

    uint32* flags = nullptr;
    uint32 visibleFlag = GetUpdateFieldData(target, flags);

    for (uint16 index = 0; index < m_valuesCount; ++index)
    {
        if (_fieldNotifyFlags & flags[index] || (builder.GetSrcBit(index) && (flags[index] & visibleFlag)))
        {
            builder.SetDestBit(index);

            if (index == AREATRIGGER_FIELD_SPELL_VISUAL_ID)
                *data << GetVisualForTarget(target);
            else
                *data << m_uint32Values[index];
        }
    }

    builder.Finish();
    BuildDynamicValuesUpdate(updateType, data);
}

uint32 AreaTrigger::GetVisualForTarget(Player const* target) const
{
    auto getVisualIfHostile = [=](Player const* target, uint32 hostileViusal)
    {
        if (GetMap()->IsBattlegroundOrArena())
        {
            if (GetCaster()->ToPlayer())
                if (GetCaster()->ToPlayer()->GetBGTeam() != target->GetBGTeam())
                    return hostileViusal;
        }
        else if (GetCaster()->IsHostileTo(target))
            return hostileViusal;
        return m_uint32Values[AREATRIGGER_FIELD_SPELL_VISUAL_ID];
    };

    switch (GetSpellId())
    {
        case 13810:     // Ice trap
            return getVisualIfHostile(target, 29567);
        case 102793:    // Ursol's Vortex
            return getVisualIfHostile(target, 25824);
        case 115460:    // Healing Sphere (doesn't have sniffs but originals have same PersistentAreaKit, so whatever)
        case 119031:    // Gift of the Serpent
            return getVisualIfHostile(target, 25768);
        case 116011:    // Rune of Power
            if (target == GetCaster())
                return 25943;
            break;
        case 121536:    // Angelic Feather
            return getVisualIfHostile(target, 27404);
        case 132950:    // Flare
            return getVisualIfHostile(target, 20730);
        default:
            break;
    }

    return m_uint32Values[AREATRIGGER_FIELD_SPELL_VISUAL_ID];
}
