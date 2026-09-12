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

#ifndef TRINITYCORE_TEMPSUMMON_H
#define TRINITYCORE_TEMPSUMMON_H

#include "Creature.h"

enum SummonerType
{
    SUMMONER_TYPE_CREATURE      = 0,
    SUMMONER_TYPE_GAMEOBJECT    = 1,
    SUMMONER_TYPE_MAP           = 2
};

/// Stores data for temp summons
struct TempSummonData
{
    uint32 entry;        ///< Entry of summoned creature
    Position pos;        ///< Position, where should be creature spawned
    TempSummonType type; ///< Summon type, see TempSummonType for available types
    uint32 time;         ///< Despawn time, usable only with certain temp summon types
};

class TC_GAME_API TempSummon : public Creature
{
    public:
        explicit TempSummon(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject);
        virtual ~TempSummon() { }
        void Update(uint32 time) override;
        bool Create(ObjectGuid::LowType guidlow, Map* map, uint32 phaseMask, uint32 entry, uint32 vehId, uint32 team, float x, float y, float z, float ang, CreatureData const* data = nullptr);
        virtual void InitStats(uint32 lifetime);
        virtual void InitSummon();
        virtual void UnSummon(uint32 msTime = 0);
        void RemoveFromWorld() override;
        void SetTempSummonType(TempSummonType type);
        void SaveToDB(uint32 /*mapid*/, uint16 /*spawnMask*/, uint32 /*phaseMask*/) override { }
        Unit* GetSummoner() const;
        Creature* GetSummonerCreatureBase() const;
        ObjectGuid GetSummonerGUID() const { return m_summonerGUID; }
        TempSummonType const& GetSummonType() { return m_type; }
        uint32 GetTimer() { return m_timer; }
        bool CanFollowOwner() const { return m_canFollowOwner; }
        void SetCanFollowOwner(bool can) { m_canFollowOwner = can; }

        const SummonPropertiesEntry* const m_Properties;

        bool IsMinorGuardian() const;
        bool IsWarBanner() const { return m_Properties && m_Properties->Type == SUMMON_TYPE_WARBANNER; }

        Player* GetSpellHasteOwner() const;

        uint32 GetBaseAttackTimer() const;

        // Some summons have to have 0 in UNIT_FIELD_SUMMONED_BY (This confirmed by sniffs at least for wild imps and grimoire pets).
        // Otherwise this summon will share gcd and cooldown with our main pet for same spells, if any.
        bool IsSummonedByHidden() const { return m_hideSummonedBy; }
        void HideSummonedBy() { m_hideSummonedBy = true; }

    protected:
        int32  m_slot = -1;
        bool   m_hideSummonedBy = false;
    private:
        TempSummonType m_type;
        uint32 m_timer;
        uint32 m_lifetime;
        ObjectGuid m_summonerGUID;
        bool m_canFollowOwner;
    };

class TC_GAME_API Minion : public TempSummon
{
    public:
        Minion(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject);
        void InitStats(uint32 duration) override;
        void RemoveFromWorld() override;
        Unit* GetOwner() const { return m_owner; }
        float GetFollowAngle() const { return m_followAngle; }
        void SetFollowAngle(float angle) { m_followAngle = angle; }
        bool IsPetGhoul() const {return GetEntry() == 26125;} // Ghoul may be guardian or pet
        bool IsSpiritWolf() const {return GetEntry() == 29264;} // Spirit wolf from feral spirits
        bool IsGuardianPet() const;
    protected:
        Unit* const m_owner;
        float m_followAngle;
};

class TC_GAME_API Guardian : public Minion
{
    public:
        Guardian(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject);
        void InitStats(uint32 duration) override;
        bool InitStatsForLevel(uint8 level);
        void InitSummon() override;

        bool UpdateStats(Stats stat) override;
        bool UpdateAllStats() override;
        void UpdateResistances(uint32 school) override;
        void UpdateArmor() override;
        void UpdateMaxHealth() override;
        void UpdateMaxPower(Powers power) override;
        void UpdateAttackPowerAndDamage(bool ranged = false) override;
        void UpdateDamagePhysical(WeaponAttackType attType) override;

        int32 GetBonusDamage() const { return m_bonusSpellDamage; }
        void SetBonusDamage(int32 damage);

        void Regenerate() override;
        void Regenerate(Powers power);

    protected:
        int32   m_bonusSpellDamage;
        float   m_statFromOwner[MAX_STATS];

        float   m_powerFraction = 0.0f;
        uint32  m_regenTimerCount = 0;
};

class TC_GAME_API Puppet : public Minion
{
    public:
        Puppet(SummonPropertiesEntry const* properties, Unit* owner);
        void InitStats(uint32 duration) override;
        void InitSummon() override;
        void Update(uint32 time) override;
        void RemoveFromWorld() override;
};

class TC_GAME_API ForcedUnsummonDelayEvent : public BasicEvent
{
public:
    ForcedUnsummonDelayEvent(TempSummon& owner) : BasicEvent(), m_owner(owner) { }
    bool Execute(uint64 e_time, uint32 p_time) override;

private:
    TempSummon& m_owner;
};
#endif
