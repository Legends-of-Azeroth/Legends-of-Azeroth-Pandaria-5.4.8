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

#include "ScriptedCreature.h"
#include "Item.h"
#include "Spell.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"
#include "ObjectMgr.h"
#include "TemporarySummon.h"

// Spell summary for ScriptedAI::SelectSpell
struct TSpellSummary
{
    uint8 Targets;                                          // set of enum SelectTarget
    uint8 Effects;                                          // set of enum SelectEffect
} extern* SpellSummary;

void SummonList::DoZoneInCombat(uint32 entry)
{
    for (StorageType::iterator i = storage_.begin(); i != storage_.end();)
    {
        Creature* summon = Unit::GetCreature(*me, *i);
        ++i;
        if (summon && summon->IsAIEnabled
                && (!entry || summon->GetEntry() == entry))
        {
            summon->AI()->DoZoneInCombat();
        }
    }
}

void SummonList::DoAction(uint32 entry, int32 info)
{
    for (StorageType::iterator i = storage_.begin(); i != storage_.end();)
    {
        Creature* summon = Unit::GetCreature(*me, *i);
        ++i;
        if (summon && summon->IsAIEnabled && (!entry || summon->GetEntry() == entry))
            summon->AI()->DoAction(info);
    }
}

void SummonList::DespawnEntry(uint32 entry)
{
    for (StorageType::iterator i = storage_.begin(); i != storage_.end();)
    {
        Creature* summon = Unit::GetCreature(*me, *i);
        if (!summon)
            i = storage_.erase(i);
        else if (summon->GetEntry() == entry)
        {
            i = storage_.erase(i);
            summon->DespawnOrUnsummon();
        }
        else
            ++i;
    }
}

void SummonList::DespawnAll(uint32 time /*=0*/)
{
    while (!storage_.empty())
    {
        Creature* summon = Unit::GetCreature(*me, storage_.front());
        storage_.pop_front();
        if (summon)
            summon->DespawnOrUnsummon(time);
    }
}

void SummonList::RemoveNotExisting()
{
    for (StorageType::iterator i = storage_.begin(); i != storage_.end();)
    {
        if (Unit::GetCreature(*me, *i))
            ++i;
        else
            i = storage_.erase(i);
    }
}

bool SummonList::HasEntry(uint32 entry) const
{
    for (StorageType::const_iterator i = storage_.begin(); i != storage_.end(); ++i)
    {
        Creature* summon = Unit::GetCreature(*me, *i);
        if (summon && summon->GetEntry() == entry)
            return true;
    }

    return false;
}

ScriptedAI::ScriptedAI(Creature* creature) : CreatureAI(creature),
    IsFleeing(false),
    _evadeCheckCooldown(2500),
    _isCombatMovementAllowed(true)
{
    _isHeroic = me->GetMap()->IsHeroic();
    _difficulty = Difficulty(me->GetMap()->GetSpawnMode());
}

void ScriptedAI::AttackStartNoMove(Unit* who)
{
    if (!who)
        return;

    if (me->Attack(who, true))
        DoStartNoMovement(who);
}

void ScriptedAI::AttackStart(Unit* who)
{
    if (IsCombatMovementAllowed())
        CreatureAI::AttackStart(who);
    else
        AttackStartNoMove(who);
}

void ScriptedAI::UpdateAI(uint32 /*diff*/)
{
    //Check if we have a current target
    if (!UpdateVictim())
        return;

    DoMeleeAttackIfReady();
}

void ScriptedAI::DoStartMovement(Unit* victim, float distance, float angle)
{
    if (victim)
        me->GetMotionMaster()->MoveChase(victim, distance, angle);
}

void ScriptedAI::DoStartNoMovement(Unit* victim)
{
    if (!victim)
        return;

    me->GetMotionMaster()->MoveIdle();
}

void ScriptedAI::DoStopAttack()
{
    if (me->GetVictim())
        me->AttackStop();
}

void ScriptedAI::DoCastSpell(Unit* target, SpellInfo const* spellInfo, bool triggered)
{
    if (!target || me->IsNonMeleeSpellCasted(false))
        return;

    me->StopMoving();
    me->CastSpell(target, spellInfo, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE);
}

void ScriptedAI::DoPlaySoundToSet(WorldObject* source, uint32 soundId)
{
    if (!source)
        return;

    if (!sSoundEntriesStore.LookupEntry(soundId))
    {
        TC_LOG_ERROR("scripts", "Invalid soundId %u used in DoPlaySoundToSet (Source: TypeId %u, GUID %u)", soundId, source->GetTypeId(), source->GetGUID().GetCounter());
        return;
    }

    source->PlayDirectSound(soundId);
}

Creature* ScriptedAI::DoSpawnCreature(uint32 entry, float offsetX, float offsetY, float offsetZ, float angle, uint32 type, Milliseconds despawntime)
{
    return me->SummonCreature(entry, me->GetPositionX() + offsetX, me->GetPositionY() + offsetY, me->GetPositionZ() + offsetZ, angle, TempSummonType(type), despawntime);
}

SpellInfo const* ScriptedAI::SelectSpell(Unit* target, uint32 school, uint32 mechanic, SelectTargetType targets, uint32 powerCostMin, uint32 powerCostMax, float rangeMin, float rangeMax, SelectEffect effects)
{
    //No target so we can't cast
    if (!target)
        return NULL;

    //Silenced so we can't cast
    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
        return NULL;

    //Using the extended script system we first create a list of viable spells
    SpellInfo const* apSpell[CREATURE_MAX_SPELLS];
    memset(apSpell, 0, CREATURE_MAX_SPELLS * sizeof(SpellInfo*));

    uint32 spellCount = 0;

    SpellInfo const* tempSpell = NULL;

    //Check if each spell is viable(set it to null if not)
    for (uint32 i = 0; i < CREATURE_MAX_SPELLS; i++)
    {
        tempSpell = sSpellMgr->GetSpellInfo(me->m_spells[i]);

        //This spell doesn't exist
        if (!tempSpell)
            continue;

        // Targets and Effects checked first as most used restrictions
        //Check the spell targets if specified
        if (targets && !(SpellSummary[me->m_spells[i]].Targets & (1 << (targets-1))))
            continue;

        //Check the type of spell if we are looking for a specific spell type
        if (effects && !(SpellSummary[me->m_spells[i]].Effects & (1 << (effects-1))))
            continue;

        //Check for school if specified
        if (school && (tempSpell->SchoolMask & school) == 0)
            continue;

        //Check for spell mechanic if specified
        if (mechanic && tempSpell->Mechanic != mechanic)
            continue;

        auto powerEntry = tempSpell->GetSpellPowerEntry(me);
        uint32 powerCost = powerEntry ? powerEntry->Cost : 0;
        uint32 powerType = powerEntry ? powerEntry->PowerType : 0;

        //Make sure that the spell uses the requested amount of power
        if (powerCostMin && powerCost < powerCostMin)
            continue;

        if (powerCostMax && powerCost > powerCostMax)
            continue;

        //Continue if we don't have the mana to actually cast this spell
        if (powerCost > (uint32)me->GetPower(Powers(powerType)))
            continue;

        //Check if the spell meets our range requirements
        if (rangeMin && me->GetSpellMinRangeForTarget(target, tempSpell) < rangeMin)
            continue;
        if (rangeMax && me->GetSpellMaxRangeForTarget(target, tempSpell) > rangeMax)
            continue;

        //Check if our target is in range
        if (me->IsWithinDistInMap(target, float(me->GetSpellMinRangeForTarget(target, tempSpell))) || !me->IsWithinDistInMap(target, float(me->GetSpellMaxRangeForTarget(target, tempSpell))))
            continue;

        //All good so lets add it to the spell list
        apSpell[spellCount] = tempSpell;
        ++spellCount;
    }

    //We got our usable spells so now lets randomly pick one
    if (!spellCount)
        return NULL;

    return apSpell[urand(0, spellCount - 1)];
}

void ScriptedAI::DoResetThreat()
{
    if (!me->CanHaveThreatList() || me->GetThreatManager().isThreatListEmpty())
    {
        TC_LOG_ERROR("scripts", "DoResetThreat called for creature that either cannot have threat list or has empty threat list (me entry = %d)", me->GetEntry());
        return;
    }

    ThreatContainer::StorageType threatlist = me->GetThreatManager().getThreatList();

    for (ThreatContainer::StorageType::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
    {
        Unit* unit = Unit::GetUnit(*me, (*itr)->getUnitGuid());
        if (unit && DoGetThreat(unit))
            DoModifyThreatPercent(unit, -100);
    }
}

float ScriptedAI::DoGetThreat(Unit* unit)
{
    if (!unit)
        return 0.0f;
    return me->GetThreatManager().getThreat(unit);
}

void ScriptedAI::DoModifyThreatPercent(Unit* unit, int32 pct)
{
    if (!unit)
        return;
    me->GetThreatManager().modifyThreatPercent(unit, pct);
}

void ScriptedAI::AddThreat(Unit* victim, float amount, Unit* who)
{
    if (!victim)
        return;
    if (!who)
        who = me;
    if (!who->CanHaveThreatList())
        return;
    who->AddThreat(victim, amount);
}

void ScriptedAI::ModifyThreatByPercent(Unit* victim, int32 pct, Unit* who)
{
    if (!victim)
        return;
    if (!who)
        who = me;
    who->GetThreatManager().modifyThreatPercent(victim, pct);
}

void ScriptedAI::ResetThreat(Unit* victim, Unit* who)
{
    if (!victim)
        return;
    if (!who)
        who = me;
    who->GetThreatManager().modifyThreatPercent(victim, -100);
}

void ScriptedAI::ResetThreatList(Unit* who)
{
    if (!who)
        who = me;
    who->GetThreatManager().resetAllAggro();
}

float ScriptedAI::GetThreat(Unit* victim, Unit* who)
{
    if (!victim)
        return 0.0f;
    if (!who)
        who = me;
    return who->GetThreatManager().getThreat(victim);
}

void ScriptedAI::ForceCombatStop(Creature* who, bool reset)
{
    if (!who || !who->IsInCombat())
        return;

    who->CombatStop(true);
    who->GetMotionMaster()->Clear(false);

    if (reset)
    {
        who->LoadCreaturesAddon();
        who->SetLootRecipient(nullptr);
        who->ResetPlayerDamageReq();
    }
}

void ScriptedAI::ForceCombatStopForCreatureEntry(uint32 entry, float maxSearchRange, bool samePhase, bool reset)
{
    std::list<Creature*> creatures;
    GetCreatureListWithEntryInGrid(creatures, me, entry, maxSearchRange);

    for (Creature* creature : creatures)
        ForceCombatStop(creature, reset);
}

void ScriptedAI::ForceCombatStopForCreatureEntry(std::vector<uint32> creatureEntries, float maxSearchRange, bool samePhase, bool reset)
{
    for (uint32 const entry : creatureEntries)
        ForceCombatStopForCreatureEntry(entry, maxSearchRange, samePhase, reset);
}

void ScriptedAI::DoTeleportTo(float x, float y, float z, uint32 time)
{
    me->Relocate(x, y, z);
    float speed = me->GetDistance(x, y, z) / ((float)time * 0.001f);
    me->MonsterMoveWithSpeed(x, y, z, speed);
}

void ScriptedAI::DoTeleportTo(const float position[4])
{
    me->NearTeleportTo(position[0], position[1], position[2], position[3]);
}

void ScriptedAI::DoTeleportPlayer(Unit* unit, float x, float y, float z, float o)
{
    if (!unit)
        return;

    if (Player* player = unit->ToPlayer())
        player->TeleportTo(unit->GetMapId(), x, y, z, o, TELE_TO_NOT_LEAVE_COMBAT);
    else
        TC_LOG_ERROR("scripts", "Creature " UI64FMTD " (Entry: %u) Tried to teleport non-player unit (Type: %u GUID: " UI64FMTD ") to x: %f y:%f z: %f o: %f. Aborted.",
            me->GetGUID().GetRawValue(), me->GetEntry(), unit->GetTypeId(), unit->GetGUID().GetRawValue(), x, y, z, o);
}

void ScriptedAI::DoTeleportAll(float x, float y, float z, float o)
{
    Map* map = me->GetMap();
    if (!map->IsDungeon())
        return;

    Map::PlayerList const& PlayerList = map->GetPlayers();
    for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        if (Player* player = itr->GetSource())
            if (player->IsAlive())
                player->TeleportTo(me->GetMapId(), x, y, z, o, TELE_TO_NOT_LEAVE_COMBAT);
}

Unit* ScriptedAI::DoSelectLowestHpFriendly(float range, uint32 minHPDiff)
{
    Unit* unit = NULL;
    Trinity::MostHPMissingInRange u_check(me, range, minHPDiff);
    Trinity::UnitLastSearcher<Trinity::MostHPMissingInRange> searcher(me, unit, u_check);
    me->VisitNearbyObject(range, searcher);

    return unit;
}

std::list<Creature*> ScriptedAI::DoFindFriendlyCC(float range)
{
    std::list<Creature*> list;
    Trinity::FriendlyCCedInRange u_check(me, range);
    Trinity::CreatureListSearcher<Trinity::FriendlyCCedInRange> searcher(me, list, u_check);
    me->VisitNearbyObject(range, searcher);

    return list;
}

std::list<Creature*> ScriptedAI::DoFindFriendlyMissingBuff(float range, uint32 uiSpellid)
{
    std::list<Creature*> list;
    Trinity::FriendlyMissingBuffInRange u_check(me, range, uiSpellid);
    Trinity::CreatureListSearcher<Trinity::FriendlyMissingBuffInRange> searcher(me, list, u_check);
    me->VisitNearbyObject(range, searcher);

    return list;
}

Player* ScriptedAI::GetPlayerAtMinimumRange(float minimumRange)
{
    Player* player = NULL;

    CellCoord pair(Trinity::ComputeCellCoord(me->GetPositionX(), me->GetPositionY()));
    Cell cell(pair);
    cell.SetNoCreate();

    Trinity::PlayerAtMinimumRangeAway check(me, minimumRange);
    Trinity::PlayerSearcher<Trinity::PlayerAtMinimumRangeAway> searcher(me, player, check);
    TypeContainerVisitor<Trinity::PlayerSearcher<Trinity::PlayerAtMinimumRangeAway>, GridTypeMapContainer> visitor(searcher);

    cell.Visit(pair, visitor, *me->GetMap(), *me, minimumRange);

    return player;
}

void ScriptedAI::SetEquipmentSlots(bool loadDefault, int32 mainHand /*= EQUIP_NO_CHANGE*/, int32 offHand /*= EQUIP_NO_CHANGE*/, int32 ranged /*= EQUIP_NO_CHANGE*/)
{
    if (loadDefault)
    {
        me->LoadEquipment(me->GetOriginalEquipmentId(), true);
        return;
    }

    if (mainHand >= 0)
        me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 0, uint32(mainHand));

    if (offHand >= 0)
        me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1, uint32(offHand));

    if (ranged >= 0)
        me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 2, uint32(ranged));
}

void ScriptedAI::SetCombatMovement(bool allowMovement)
{
    _isCombatMovementAllowed = allowMovement;
}

enum NPCs
{
    NPC_LICH_KING               = 36597,
    NPC_DB_SAURFANG             = 37813,
    NPC_BROODLORD               = 12017,
    NPC_VOID_REAVER             = 19516,
    NPC_JAN_ALAI                = 23578,
    NPC_SARTHARION              = 28860,
    NPC_IGNIS                   = 33118,
    NPC_MELJRAK                 = 62397,
    NPC_GARALON                 = 62164,
    NPC_SHEKZEER                = 62837,
    NPC_TAYAK                   = 62543,
    NPC_LEVIATHAN               = 33113,
    NPC_RA_DEN                  = 69473,
    NPC_LULIN                   = 68905,
    NPC_SUEN                    = 68904,
    NPC_PRIMORDIUS              = 69017,
    NPC_HE_SOFTFOOT             = 71479,
    NPC_ROOK_STONETOE           = 71475,
    NPC_IRON_JUGGERNAUT         = 71466,
    NPC_YULON                   = 71955,
    NPC_XUEN                    = 71953,
    NPC_CHI_JI                  = 71952,
    NPC_NIUZAO                  = 71954,
    NPC_GARROSH_HELLSCREAM      = 71865,
    NPC_HAROMM                  = 71859,
    NPC_KARDIS                  = 71858,
    NPC_KILRUK_THE_WIND_REAVER  = 71161,
    NPC_XARIL_THE_POISONED_MIND = 71157,
    NPC_KORVEN_THE_PRIME        = 71155,
    NPC_IYYOKUK_THE_LUCID       = 71160,
    NPC_KAROZ_THE_LOCUST        = 71154,
    NPC_SKEER_THE_BLOODSEEKER   = 71152,
    NPC_RIKKAL_THE_DISSECTOR    = 71158,
};

// Hacklike storage used for misc creatures that are expected to evade of outside of a certain area.
// It is assumed the information is found elswehere and can be handled by the core. So far no luck finding such information/way to extract it.
bool ScriptedAI::EnterEvadeIfOutOfCombatArea(uint32 const diff, const float distance)
{
    if (_evadeCheckCooldown <= diff)
        _evadeCheckCooldown = 2500;
    else
    {
        _evadeCheckCooldown -= diff;
        return false;
    }

    if (me->IsInEvadeMode() || !me->GetVictim())
        return false;

    float x = me->GetPositionX();
    float y = me->GetPositionY();
    float z = me->GetPositionZ();

    switch (me->GetEntry())
    {
        case NPC_LICH_KING:
            //if (fZ < 830.0f)
            if (me->GetDistance2d(505.21f, -2124.35f) < 80.0f)
                return false;
            break;
        case NPC_DB_SAURFANG:
            //if (fZ < 536.0f)
            if (me->GetDistance2d(-496.35f, 2211.33f) < 60.0f)
                return false;
            break;
        case NPC_BROODLORD:                                         // broodlord (not move down stairs)
            if (z > 448.60f)
                return false;
            break;
        case NPC_VOID_REAVER:                                         // void reaver (calculate from center of room)
            if (me->GetDistance2d(432.59f, 371.93f) < 105.0f)
                return false;
            break;
        case NPC_JAN_ALAI:                                         // jan'alai (calculate by Z)
            if (z > 12.0f)
                return false;
            break;
        case NPC_SARTHARION:                                         // sartharion (calculate box)
            if (x > 3218.86f && x < 3275.69f && y < 572.40f && y > 484.68f)
                return false;
            break;
        case NPC_IGNIS:                                         // Ignis (not move out room Ulduar)
            if (y > 170.00f)
                return false;
            break;
        case NPC_MELJRAK:
            if (z > 502.0f)
                return false;
            break;
        case NPC_GARALON:
            if (x > -1978.25f)
                return false;
            break;
        case NPC_SHEKZEER:
            if (z < 578.0f)
                return false;
            break;
        case NPC_TAYAK:
            if (x > -2189.41f)
                return false;
            break;
        case NPC_LEVIATHAN:
            if (x > 145.0f)
                return false;
            break;
        case NPC_RA_DEN:
            if (x < 5550.0f && z < 1.0f)
                return false;
            break;
        case NPC_LULIN:
        case NPC_SUEN:
            if (z < 148.5f && me->GetExactDist2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) < 100.0f)
                return false;
            break;
        case NPC_PRIMORDIUS:
            if (me->GetExactDist2d(5592.35f, 4655.88f) < 66.0f)
                return false;
            break;
        case NPC_HE_SOFTFOOT:
        case NPC_ROOK_STONETOE:
            if (me->GetExactDist2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) < 65.0f && me->GetPositionZ() < 425.0f)
                return false;
            break;
        case NPC_YULON:
        case NPC_XUEN:
        case NPC_CHI_JI:
        case NPC_NIUZAO:
            if (me->GetExactDist2d(-650.03f, -5016.83f) < 105.0f)
                return false;
            break;
        case NPC_HAROMM:
        case NPC_KARDIS:
            if (x > 1475.0f)
                return false;
            break;
        case NPC_KILRUK_THE_WIND_REAVER:
        case NPC_XARIL_THE_POISONED_MIND:
        case NPC_KORVEN_THE_PRIME:
        case NPC_IYYOKUK_THE_LUCID:
        case NPC_KAROZ_THE_LOCUST:
        case NPC_SKEER_THE_BLOODSEEKER:
        case NPC_RIKKAL_THE_DISSECTOR:
            if (me->GetExactDist2d(1582.94f, -5685.20f) < 100.0f)
                return false;
            break;
        default: // For most of creatures that certain area is their home area.
            TC_LOG_INFO("misc", "TSCR: EnterEvadeIfOutOfCombatArea used for creature entry %u, but does not have any definition. Using the default one.", me->GetEntry());
            uint32 homeAreaId = me->GetMap()->GetAreaId(me->GetPhaseMask(), me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ());
            if (me->GetAreaId() == homeAreaId && me->GetDistance(me->GetHomePosition()) <= distance)
                return false;
    }

    EnterEvadeMode();
    return true;
}

// BossAI - for instanced bosses
BossAI::BossAI(Creature* creature, uint32 bossId) : ScriptedAI(creature),
    instance(creature->GetInstanceScript()),
    summons(creature),
    _boundary(instance ? instance->GetBossBoundary(bossId) : NULL),
    _bossId(bossId)
{
    scheduler.SetValidator([this]
    {
        return !me->HasUnitState(UNIT_STATE_CASTING);
    });
}

void BossAI::_Reset()
{
    if (!me->IsAlive())
        return;

    me->SetCombatPulseDelay(0);
    me->ResetLootMode();
    events.Reset();
    summons.DespawnAll();
    scheduler.CancelAll();
    if (instance && instance->GetBossState(_bossId) != DONE)
        instance->SetBossState(_bossId, NOT_STARTED);
}

void BossAI::_JustDied()
{
    events.Reset();
    summons.DespawnAll();
    scheduler.CancelAll();
    if (instance)
    {
        instance->SetBossState(_bossId, DONE);
        instance->SaveToDB(); // will be removed
    }
}

void BossAI::_JustReachedHome()
{
    me->setActive(false);
}

bool BossAI::_JustEngagedWith()
{
    if (instance)
    {
        // bosses do not respawn, check only on enter combat
        if (!instance->CheckRequiredBosses(_bossId))
        {
            EnterEvadeMode();
            return false;
        }
    }

    me->setActive(true, ActiveFlags::InCombat);
    DoZoneInCombat();

    if (instance)
        instance->SetBossState(_bossId, IN_PROGRESS);

    ScheduleTasks();
    return true;
}

void BossAI::TeleportCheaters()
{
    float x, y, z;
    me->GetPosition(x, y, z);

    ThreatContainer::StorageType threatList = me->GetThreatManager().getThreatList();
    for (ThreatContainer::StorageType::const_iterator itr = threatList.begin(); itr != threatList.end(); ++itr)
        if (Unit* target = (*itr)->getTarget())
            if (target->GetTypeId() == TYPEID_PLAYER && !CheckBoundary(target))
                target->NearTeleportTo(x, y, z, 0);
}

bool BossAI::CheckBoundary(Unit* who)
{
    if (!GetBoundary() || !who)
        return true;

    for (BossBoundaryMap::const_iterator itr = GetBoundary()->begin(); itr != GetBoundary()->end(); ++itr)
    {
        switch (itr->first)
        {
            case BOUNDARY_N:
                if (who->GetPositionX() > itr->second)
                    return false;
                break;
            case BOUNDARY_S:
                if (who->GetPositionX() < itr->second)
                    return false;
                break;
            case BOUNDARY_E:
                if (who->GetPositionY() < itr->second)
                    return false;
                break;
            case BOUNDARY_W:
                if (who->GetPositionY() > itr->second)
                    return false;
                break;
            case BOUNDARY_NW:
                if (who->GetPositionX() + who->GetPositionY() > itr->second)
                    return false;
                break;
            case BOUNDARY_SE:
                if (who->GetPositionX() + who->GetPositionY() < itr->second)
                    return false;
                break;
            case BOUNDARY_NE:
                if (who->GetPositionX() - who->GetPositionY() > itr->second)
                    return false;
                break;
            case BOUNDARY_SW:
                if (who->GetPositionX() - who->GetPositionY() < itr->second)
                    return false;
                break;
            default:
                break;
        }
    }

    return true;
}

void BossAI::JustSummoned(Creature* summon)
{
    summons.Summon(summon);
    if (me->IsInCombat())
        DoZoneInCombat(summon);
}

void BossAI::SummonedCreatureDespawn(Creature* summon)
{
    summons.Despawn(summon);
}

void BossAI::UpdateAI(uint32 diff)
{
    if (!UpdateVictim())
        return;

    events.Update(diff);

    if (me->HasUnitState(UNIT_STATE_CASTING))
        return;

    while (uint32 eventId = events.ExecuteEvent())
    {
        ExecuteEvent(eventId);
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;
    }

    DoMeleeAttackIfReady();
}

void BossAI::_DespawnAtEvade()
{
    uint32 corpseDelay = me->GetCorpseDelay();
    uint32 respawnDelay = me->GetRespawnDelay();

    me->SetCorpseDelay(1);
    me->SetRespawnDelay(29);

    me->DespawnOrUnsummon();

    me->SetCorpseDelay(corpseDelay);
    me->SetRespawnDelay(respawnDelay);
}

// WorldBossAI - for non-instanced bosses

WorldBossAI::WorldBossAI(Creature* creature) :
    ScriptedAI(creature),
    summons(creature) { }

void WorldBossAI::_Reset()
{
    if (!me->IsAlive())
        return;

    events.Reset();
    summons.DespawnAll();
}

void WorldBossAI::_JustDied()
{
    events.Reset();
    summons.DespawnAll();
}

void WorldBossAI::_JustEngagedWith()
{
    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true);
    if (target)
        AttackStart(target);
}

void WorldBossAI::JustSummoned(Creature* summon)
{
    summons.Summon(summon);
    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true);
    if (target)
        summon->AI()->AttackStart(target);
}

void WorldBossAI::SummonedCreatureDespawn(Creature* summon)
{
    summons.Despawn(summon);
}

void WorldBossAI::UpdateAI(uint32 diff)
{
    if (!UpdateVictim())
        return;

    events.Update(diff);

    if (me->HasUnitState(UNIT_STATE_CASTING))
        return;

    while (uint32 eventId = events.ExecuteEvent())
        ExecuteEvent(eventId);

    DoMeleeAttackIfReady();
}

void CasterMovement::Chase(Unit* target)
{
    UpdateMovement(target, true);
}

void CasterMovement::Update(uint32 diff, Unit* victim)
{
    if (_me->HasUnitState(UNIT_STATE_CASTING | UNIT_STATE_LOST_CONTROL))
        return;

    if (_movementUpdateTimer > diff)
    {
        _movementUpdateTimer -= diff;
        return;
    }
    _movementUpdateTimer = _updateInterval;

    if (!victim && !(victim = _me->GetVictim()))
    {
        if (_me->IsGuardian())
        {
            if (_me->GetMotionMaster()->GetCurrentMovementGeneratorType() == FOLLOW_MOTION_TYPE)
                return;

            _chasingInMelee = false;
            if (Unit* owner = _me->GetOwner())
                _me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, _me->GetFollowAngle());
        }
        return;
    }

    UpdateMovement(victim);
}

void CasterMovement::UpdateMovement(Unit* target, bool force)
{
    static auto checkPower = [](Unit* me, SpellInfo const* spell) -> bool
    {
        int32 powerEntryIndex = -1;
        Powers power = spell->GetPowerType(me, &powerEntryIndex);
        if (power >= POWER_MANA && power <= POWER_ENERGY)
            return uint32(me->GetPower(power)) >= uint32(spell->CalcPowerCost(me, spell->GetSchoolMask(), powerEntryIndex));
        return true;
    };

    _unableToCast = false;
    if (_meleeWhenUnableToCast)
    {
        _unableToCast = _me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
        if (!_unableToCast && _spells.size())
        {
            _unableToCast = true;
            for (auto&& spell : _spells)
                if (!_me->HasSpellCooldown(spell) && checkPower(_me, sSpellMgr->GetSpellInfo(spell)) && !(_unableToCast = false))
                    break;
        }
    }

    if (!_unableToCast) // && _me->IsWithinLOSInMap(target)) // LOS check moved to ChaseMovementGenerator
    {
        if (_chasingInMelee || force)
        {
            if (!_me->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_CONTROLLED) /*&& !_me->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_CRITICAL)*/)
                _me->StopMoving();
            _me->GetMotionMaster()->MoveChase(target, _casterDistance, 0);
            _chasingInMelee = false;
        }
    }
    else
    {
        if (!_chasingInMelee || force)
        {
            if (!_me->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_CONTROLLED) /*&& !_me->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_CRITICAL)*/)
                _me->StopMoving();
            _me->GetMotionMaster()->MoveChase(target, std::max(0.01f, _meleeDistance));
            _chasingInMelee = true;
        }
    }
}

CasterMovement& CasterMovement::Spell(uint32 spellId)
{
    CasterDistanceBySpell(spellId, !_spells.empty());
    _spells.push_back(spellId);
    return *this;
}

CasterMovement& CasterMovement::CasterDistanceBySpell(uint32 spellId, bool onlyCloser)
{
    SpellInfo const* spell = sSpellMgr->GetSpellInfo(spellId);
    ASSERT(spell);
    if (float range = spell->GetMaxRange(false, _me))
        if (!onlyCloser || _casterDistance > range - 5.0f)
            _casterDistance = std::max(NOMINAL_MELEE_RANGE, range - 5.0f); // Reduced by 5 yards to account for combat reach, since spells use another algorithm to detect range
    return *this;
}

void AssistBehavior::Update(uint32 diff)
{
    m_assistTimer += diff;
    if (m_assistTimer >= 500 || !me->IsInCombat())
    {
        m_assistTimer = 0;

        Unit* owner = me->GetOwner();
        if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
            return;

        Unit* victim = nullptr;
        uint32 prio = 0;
        for (auto&& it : owner->ToPlayer()->GetAssistList())
        {
            if (it.second.Priority > prio)
            {
                if (Unit* target = ObjectAccessor::GetUnit(*owner, it.first))
                {
                    victim = target;
                    prio = it.second.Priority;
                }
            }
        }

        if (!victim)
        {
            m_assistTarget = ObjectGuid::Empty;
            return;
        }

        if (victim->GetGUID() != m_assistTarget)
        {
            m_assistTarget = victim->GetGUID();
            me->AI()->AttackStart(victim);
        }
    }
}

// SD2 grid searchers.
Creature* GetClosestCreatureWithEntry(WorldObject* source, uint32 entry, float maxSearchRange, bool alive /*= true*/)
{
    return source->FindNearestCreature(entry, maxSearchRange, alive);
}

GameObject* GetClosestGameObjectWithEntry(WorldObject* source, uint32 entry, float maxSearchRange)
{
    return source->FindNearestGameObject(entry, maxSearchRange);
}

void GetCreatureListWithEntryInGrid(std::list<Creature*>& list, WorldObject* source, uint32 entry, float maxSearchRange)
{
    source->GetCreatureListWithEntryInGrid(list, entry, maxSearchRange);
}

void GetGameObjectListWithEntryInGrid(std::list<GameObject*>& list, WorldObject* source, uint32 entry, float maxSearchRange)
{
    source->GetGameObjectListWithEntryInGrid(list, entry, maxSearchRange);
}

void GetPlayerListInGrid(std::list<Player*>& list, WorldObject* source, float maxSearchRange)
{
    source->GetPlayerListInGrid(list, maxSearchRange);
}

void GetPositionWithDistInOrientation(Unit* pUnit, float dist, float orientation, float& x, float& y)
{
    x = pUnit->GetPositionX() + (dist * cos(orientation));
    y = pUnit->GetPositionY() + (dist * sin(orientation));
}
void GetRandPosFromCenterInDist(float centerX, float centerY, float dist, float& x, float& y)
{
    float randOrientation = frand(0, 2 * M_PI);
    x = centerX + (dist * cos(randOrientation));
    y = centerY + (dist * sin(randOrientation));
}

Player* GetFarthestPlayerInArea(WorldObject* owner, float range)
{
    Player* farth = nullptr;
    float dist = 0.0f;
    std::list<Player*> PlayersInArea;
    GetPlayerListInGrid(PlayersInArea, owner, range);

    for (auto&& itr : PlayersInArea)
    {
        if (itr->IsGameMaster())
            continue;

        if (itr->GetDistance2d(owner) > dist)
        {
            dist = itr->GetDistance2d(owner);
            farth = itr;
        }
    }

    return farth;
}
