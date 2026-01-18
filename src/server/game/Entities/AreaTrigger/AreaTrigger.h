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

#ifndef TRINITYCORE_AREATRIGGER_H
#define TRINITYCORE_AREATRIGGER_H

#include "Object.h"
#include <memory>
#include <functional>

class Unit;
class SpellInfo;
class Spell;
class AuraEffect;
class AuraApplication;

enum
{
    AREATRIGGER_FLAG_SCALED = 1 << 0,
};

enum
{
    AREATRIGGER_COLLISION_TYPE_NONE         = 0,
    AREATRIGGER_COLLISION_TYPE_SPHERE       = 1,
};

enum AreatriggerInterpolation
{
    AREATRIGGER_INTERPOLATION_NONE,
    AREATRIGGER_INTERPOLATION_LINEAR,
};

class IAreaTrigger
{
    public:
        void Initialize(AreaTrigger* trigger, Unit* caster, WorldObject* target);

        IAreaTrigger() {}
        virtual ~IAreaTrigger() {}

        virtual void OnInit() { }
        virtual void OnUpdate(uint32 /*diff*/) {}
        virtual void OnExpire() {}
        virtual void OnDestroy() {}
        virtual void OnPathPointRiched() { }
        virtual void OnDestinationReached() { }

        virtual void OnRemoveFromWorld();

        Unit* GetCaster() { return m_caster; }
        WorldLocation* GetLocation() { return m_location; }
        float GetRange() { return m_range; }
        void SetRange(float radius) { m_range = radius; }
        WorldObject* GetTarget() { return m_target; }
        SpellInfo const* GetSpellInfo() { return m_spellInfo; }
        AreaTrigger* GetAreaTrigger() const { return m_areaTrigger; }
    public:
        float m_range;

    protected:
        WorldLocation* m_location = nullptr;
        Unit* m_caster = nullptr;
        WorldObject* m_target = nullptr;
        SpellInfo const* m_spellInfo = nullptr;
        uint32 m_auraEffectIndex = 0;
        bool m_duringRemoveFromWorld = false;
        AreaTrigger* m_areaTrigger = nullptr;
};

class IAreaTriggerOnce : public IAreaTrigger
{
    public:
        IAreaTriggerOnce()
            : m_mapTypeMask(GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER)
        {
        }

        virtual void OnUpdate(uint32 diff) override;

        virtual bool CheckTriggering(WorldObject* triggering) = 0;
        virtual void OnTrigger(WorldObject* triggering) = 0;
        void OnRemoveFromWorld() final;

    protected:
        uint32 m_mapTypeMask;
};

class IAreaTriggerAuraUpdater;
class IAreaTriggerAura : public IAreaTrigger
{
        friend class IAreaTriggerAuraUpdater;
    public:
        IAreaTriggerAura();

        ~IAreaTriggerAura();

        virtual void OnUpdate(uint32 diff) override;

        virtual bool CheckTriggering(WorldObject* /*triggering*/) = 0;
        virtual void OnTriggeringApply(WorldObject* /*triggering*/) {}
        virtual void OnTriggeringUpdate(WorldObject* /*triggering*/) {}
        virtual void OnTriggeringRemove(WorldObject* /*triggering*/) {}

        void OnRemoveFromWorld() final;
        bool IsInBox(Unit* target, float extentsX, float extentsY, float extentsZ);

    protected:
        uint32 m_mapTypeMask;

    private:
        struct Triggering
        {
            uint32 UpdateNumber;
            ObjectGuid Guid;
        };
        typedef std::list<Triggering> TriggeringList;

        TriggeringList m_triggerings;
        uint32 m_updateNumber;
};

namespace G3D
{
    class Vector2;
    class Vector3;
}
namespace Movement
{
    template<typename length_type>
    class Spline;
}

class AreaTrigger : public WorldObject, public GridObject<AreaTrigger>, public MapObject
{
    public:
        AreaTrigger();
        AreaTrigger(AuraEffect const* eff);
        ~AreaTrigger();

        void AddToWorld();
        void RemoveFromWorld();

        bool CreateAreaTrigger(uint32 guidlow, uint32 triggerEntry, Unit* caster, SpellInfo const* spell, Position const& pos, Unit* effectTarget = nullptr);
        void Update(uint32 p_time);
        void Remove();
        void Expire();
        void SetRadius(float radius);
        uint32 GetSpellId() const { return GetUInt32Value(AREATRIGGER_FIELD_SPELL_ID); }
        uint32 GetEffectIndex() const { return m_effectIndex; }
        int32 GetMaxDuration() const { return GetInt32Value(AREATRIGGER_FIELD_DURATION); }
        int32 GetDuration() const { return m_duration; }
        void  SetDuration(int32 newDuration) { m_duration = newDuration; }
        Unit* GetCaster() const { return m_caster; }
        ObjectGuid GetCasterGUID() const { return GetGuidValue(AREATRIGGER_FIELD_CASTER); }
        void BindToCaster();
        void UnbindFromCaster();

        float GetScaleX() const { return m_scaleX; }
        float GetScaleY() const { return m_scaleY; }
        void SetVisualRadius(float radius) { m_scaleX = radius; m_scaleY = radius; }

        // Some area trigers which are created by SPELL_AURA_AREA_TIRGGER must be attached to the fake transport.
        // Because for some reason their visual provided by area tirgger itself and not by aura, I don't know why.
        void AttachToTarget();
        bool HasAreaTriggerTransport() const { return m_attachedToTransport; }

        std::vector<G3D::Vector3> const& GetPath() const;
        uint32 GetMovementTime() const { return m_movementTime; }
        void InitSpline(std::vector<Position> const& path, int32 duration);
        bool HasSpline() const { return m_spline.get() != nullptr; }
        int32 GetCurrentSplineIndex() const { return m_lastSplineIndex; }

    private:
        void UpdateSplinePosition(uint32 diff);
        void BuildValuesUpdate(uint8 updateType, ByteBuffer* data, Player* target) const;

        uint32 GetVisualForTarget(Player const* target) const;

    protected:
        int32 m_duration = 0;
        Unit* m_caster = nullptr;
        WorldObject* m_effectTarget = nullptr;
        ObjectGuid m_auraOwner = ObjectGuid::Empty;
        uint32 m_effectIndex = 0;

        float m_scaleX = 0.0f;
        float m_scaleY = 0.0f;
        std::unique_ptr<IAreaTrigger> m_script;

        std::unique_ptr<Movement::Spline<int32>> m_spline;
        bool m_reachedDestination = false;
        uint32 m_movementTime = 0;
        int32 m_lastSplineIndex = 0;
        bool m_attachedToTransport = false;
};
#endif
