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

#include "CreatureAI.h"
#include "AreaBoundary.h"
#include "CreatureAIImpl.h"
#include "Creature.h"
#include "World.h"
#include "SpellMgr.h"
#include "Vehicle.h"
#include "Log.h"
#include "MapReference.h"
#include "Player.h"
#include "CreatureTextMgr.h"
#include "Group.h"
#include "Chat.h"
#include "CellImpl.h"
#include "InstanceScript.h"

CreatureAI::CreatureAI(Creature* creature) : UnitAI(creature), me(creature), _boundary(nullptr), _negateBoundary(false), m_MoveInLineOfSight_locked(false), m_canSeeEvenInPassiveMode(false), _isEngaged(false)
{ 

}

CreatureAI::~CreatureAI() 
{ 
    delete _boundary;
}

//Disable CreatureAI when charmed
void CreatureAI::OnCharmed(bool /*apply*/)
{
    //me->IsAIEnabled = !apply;*/
    me->NeedChangeAI = true;
    me->IsAIEnabled = false;
}

AISpellInfoType* UnitAI::AISpellInfo;
AISpellInfoType* GetAISpellInfo(uint32 i) { return &CreatureAI::AISpellInfo[i]; }

void CreatureAI::Talk(uint8 id, WorldObject const* whisperTarget /*= nullptr*/)
{
    sCreatureTextMgr->SendChat(me, id, whisperTarget);
}

void CreatureAI::TalkToMap(uint8 id, WorldObject const* whisperTarget /*= nullptr*/)
{
    sCreatureTextMgr->SendChat(me, id, whisperTarget, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_MAP);
}

void CreatureAI::DoZoneInCombat(Creature* creature /*= NULL*/, float maxRangeToNearestTarget /* = 50.0f*/)
{
    if (!creature)
        creature = me;

    if (!creature->CanHaveThreatList())
        return;

    Map* map = creature->GetMap();
    if (!map->IsDungeon())                                  //use IsDungeon instead of Instanceable, in case battlegrounds will be instantiated
    {
        TC_LOG_ERROR("misc", "DoZoneInCombat call for map that isn't an instance (creature entry = %d)", creature->GetTypeId() == TYPEID_UNIT ? creature->ToCreature()->GetEntry() : 0);
        return;
    }

    if (!creature->HasReactState(REACT_PASSIVE) && !creature->GetVictim())
    {
        if (Unit* nearTarget = creature->SelectNearestTarget(maxRangeToNearestTarget))
            creature->AI()->AttackStart(nearTarget);
        else if (creature->IsSummon())
        {
            if (Unit* summoner = creature->ToTempSummon()->GetSummoner())
            {
                Unit* target = summoner->getAttackerForHelper();
                if (!target && summoner->CanHaveThreatList() && !summoner->GetThreatManager().isThreatListEmpty())
                    target = summoner->GetThreatManager().getHostilTarget();
                if (target && (creature->IsFriendlyTo(summoner) || creature->IsHostileTo(target)))
                    creature->AI()->AttackStart(target);
            }
        }
    }

    // Intended duplicated check, the code above this should select a victim
    // If it can't find a suitable attack target then we should error out.
    if (!creature->HasReactState(REACT_PASSIVE) && !creature->GetVictim())
    {
        TC_LOG_ERROR("misc", "DoZoneInCombat called for creature that has empty threat list (creature entry = %u)", creature->GetEntry());
        return;
    }

    Map::PlayerList const& playerList = map->GetPlayers();

    if (playerList.isEmpty())
        return;

    for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
    {
        if (Player* player = itr->GetSource())
        {
            if (player->IsGameMaster())
                continue;

            if (player->IsAlive())
            {
                creature->SetInCombatWith(player);
                player->SetInCombatWith(creature);
                creature->AddThreat(player, 0.0f);
            }

            /* Causes certain things to never leave the threat list (Priest Lightwell, etc):
            for (Unit::ControlList::const_iterator itr = player->m_Controlled.begin(); itr != player->m_Controlled.end(); ++itr)
            {
                creature->SetInCombatWith(*itr);
                (*itr)->SetInCombatWith(creature);
                creature->AddThreat(*itr, 0.0f);
            }*/
        }
    }
}

void CreatureAI::DoAttackerAreaInCombat(Unit* attacker, float range, Unit* pUnit)
{
    if (!attacker)
        attacker = me;

    if (!pUnit)
        pUnit = me;

    Map* map = pUnit->GetMap();

    if (!map->IsDungeon())
        return;

    if (!pUnit->CanHaveThreatList() || pUnit->GetThreatManager().isThreatListEmpty())
        return;

    Map::PlayerList const& PlayerList = map->GetPlayers();
    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
    {
        if (Player* i_pl = i->GetSource())
            if (i_pl->IsAlive() && attacker->GetDistance(i_pl) <= range)
            {
                pUnit->SetInCombatWith(i_pl);
                i_pl->SetInCombatWith(pUnit);
                pUnit->AddThreat(i_pl, 0.0f);
            }
    }
}

void CreatureAI::DoAttackerGroupInCombat(Player* attacker)
{
    if (attacker)
    {
        if (Group* pGroup = attacker->GetGroup())
        {
            for (GroupReference* itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* pGroupGuy = itr->GetSource();

                if (pGroupGuy && pGroupGuy->IsAlive() && pGroupGuy->GetMapId() == me->GetMapId())
                {
                    me->SetInCombatWith(pGroupGuy);
                    pGroupGuy->SetInCombatWith(me);
                    me->AddThreat(pGroupGuy, 0.0f);
                }
            }
        }
    }
}

// scripts does not take care about MoveInLineOfSight loops
// MoveInLineOfSight can be called inside another MoveInLineOfSight and cause stack overflow
void CreatureAI::MoveInLineOfSight_Safe(Unit* who)
{
    if (m_MoveInLineOfSight_locked == true)
        return;
    m_MoveInLineOfSight_locked = true;
    MoveInLineOfSight(who);
    m_MoveInLineOfSight_locked = false;
}

void CreatureAI::SpellRequiresMovement(Unit* /*target*/, Spell* spell)
{
    TC_LOG_ERROR("shitlog", "CreatureAI::SpellRequiresMovement me %u",  me->GetEntry());

    spell->finish(false);
    delete spell;
}

void CreatureAI::MoveInLineOfSight(Unit* who)
{
    if (me->GetVictim())
        return;

    if (me->GetCreatureType() == CREATURE_TYPE_NON_COMBAT_PET) // non-combat pets should just stand there and look good;)
        return;

    if (me->CanStartAttack(who, false))
        AttackStart(who);
    //else if (who->GetVictim() && me->IsFriendlyTo(who)
    //    && me->IsWithinDistInMap(who, sWorld->getIntConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_RADIUS))
    //    && me->CanStartAttack(who->GetVictim(), true)) /// @todo if we use true, it will not attack it when it arrives
    //    me->GetMotionMaster()->MoveChase(who->GetVictim());
}

void CreatureAI::EnterEvadeMode(EvadeReason why)
{
    if (!_EnterEvadeMode(why))
        return;

    TC_LOG_DEBUG("entities.unit", "Creature %u enters evade mode.", me->GetEntry());

    if (!me->GetVehicle()) // otherwise me will be in evade mode forever
    {
        if (Unit* owner = me->GetCharmerOrOwner())
        {
            me->GetMotionMaster()->Clear(false);
            me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, me->GetFollowAngle(), MOTION_SLOT_ACTIVE);
        }
        else
        {
            // Required to prevent attacking creatures that are evading and cause them to reenter combat
            // Does not apply to MoveFollow
            me->AddUnitState(UNIT_STATE_EVADE);
            me->GetMotionMaster()->MoveTargetedHome();
        }
    }

    Reset();

    if (me->IsVehicle()) // use the same sequence of addtoworld, aireset may remove all summons!
        me->GetVehicleKit()->Reset(true);
}

/*void CreatureAI::AttackedBy(Unit* attacker)
{
    if (!me->GetVictim())
        AttackStart(attacker);
}*/

void CreatureAI::SetGazeOn(Unit* target)
{
    if (me->IsValidAttackTarget(target))
    {
        AttackStart(target);
        me->SetReactState(REACT_PASSIVE);
    }
}

void CreatureAI::JustEnteredCombat(Unit* victim)
{
    UnitAI::JustEnteredCombat(victim);
    EngagementStart(victim);
}

void CreatureAI::EngagementStart(Unit* who)
{
    if (IsEngaged())
    {
        TC_LOG_ERROR("scripts.ai", "CreatureAI::EngagementStart called even though creature is already engaged.");
        return;
    }

    _isEngaged = true;
    JustEngagedWith(who);
}

void CreatureAI::EngagementOver()
{
    if (!IsEngaged())
    {
        TC_LOG_DEBUG("scripts.ai", "CreatureAI::EngagementOver called even though creature is not currently engaged.");
        return;
    }

    _isEngaged = false;
}

bool CreatureAI::CheckInRoom()
{
    if (!_boundary || _boundary->empty())
        return true;

    if (IsInBoundary())
        return true;

    TC_LOG_DEBUG("scripts", "Creature %s (unit %s) has left its designated room area!", me->GetName(), me->GetGUID().ToString().c_str());
    EnterEvadeMode();
    return false;
}

bool CreatureAI::IsInBoundary(Position const* who) const
{
    if (!who)
        who = me;

    for (AreaBoundary const* areaBoundary : *_boundary)
    {
        if (!areaBoundary->IsWithinBoundary(who))
            return _negateBoundary;
    }

    return !_negateBoundary;
}

void CreatureAI::SetBoundary(CreatureBoundary const* boundary, bool negateBoundaries)
{
    delete _boundary;

    if (!boundary || boundary->empty())
    {
        _boundary = nullptr;
        _negateBoundary = false;
    }
    else
    {
        _boundary = new CreatureBoundary(*boundary);
        _negateBoundary = negateBoundaries;
    }
}

bool CreatureAI::IsInBounds(CreatureBoundary const& boundary, Position const* who)
{
    for (AreaBoundary const* areaBoundary : boundary)
    {
        if (!areaBoundary->IsWithinBoundary(who))
            return false;
    }

    return true;
}

int32 CreatureAI::VisualizeBoundary(Seconds duration, Unit* owner /*= nullptr*/, bool fill /*= false*/) const
{
    return 0;
}

bool CreatureAI::UpdateVictimWithGaze()
{
    if (!me->IsInCombat())
        return false;

    if (me->HasReactState(REACT_PASSIVE))
    {
        if (me->GetVictim())
            return true;
        else
            me->SetReactState(REACT_AGGRESSIVE);
    }

    if (Unit* victim = me->SelectVictim())
        AttackStart(victim);
    return me->GetVictim();
}

bool NeedEvadeIfChallengeNotStarted(uint32 entry, uint32 mapId)
{
    if (mapId == 1148)
        return false;

    switch (entry)
    {
        case 62171: // Jade Serpent Defender
        case 57109: // Sha
        case 58319:
        case 56875: // Krikthik Demolisher
        case 65337: // Serpent Spine Defender
            return false;
    }

    return true;
}

bool CreatureAI::UpdateVictim()
{
    if (!me->IsInCombat())
        return false;

    if (InstanceScript* instance = me->GetInstanceScript())
    {
        // Exclude some npc from Temple of the Jade Serpent, Gates of the Setting Sun & Proving Grounds
        if (instance->instance->IsChallengeDungeon() && !instance->IsChallengeModeStarted() && NeedEvadeIfChallengeNotStarted(me->GetEntry(), instance->instance->GetId()))
        {
            EnterEvadeMode();
            return false;
        }
    }

    if (!me->HasReactState(REACT_PASSIVE))
    {
        if (Unit* victim = me->SelectVictim())
            AttackStart(victim);
        return me->GetVictim();
    }
    else if (me->GetThreatManager().isThreatListEmpty())
    {
        EnterEvadeMode();
        return false;
    }

    return true;
}

bool CreatureAI::_EnterEvadeMode(EvadeReason why)
{
    if (!me->IsAlive())
        return false;

    EngagementOver();

    // don't remove vehicle auras, passengers aren't supposed to drop off the vehicle
    // don't remove clone caster on evade (to be verified)
    // And don't remove any auras for fucking player's summons
    if (!me->GetCreatorGUID().IsPlayer())
        me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE, SPELL_AURA_CLONE_CASTER);

    // sometimes bosses stuck in combat?
    me->DeleteThreatList();
    me->CombatStop(true);
    me->LoadCreaturesAddon();
    me->ApplyInstanceAuraIfNeeded();
    me->SetLootRecipient(NULL);
    me->ResetPlayerDamageReq();

    if (me->IsInEvadeMode())
        return false;

    return true;
}

// Optional<QuestGiverStatus> CreatureAI::GetDialogStatus(Player* /*player*/)
// {
//     return {};
// }

Creature* CreatureAI::DoSummon(uint32 entry, const Position& pos, Milliseconds despawnTime, TempSummonType summonType)
{
    return me->SummonCreature(entry, pos, summonType, despawnTime);
}

Creature* CreatureAI::DoSummon(uint32 entry, WorldObject* obj, float radius, Milliseconds despawnTime, TempSummonType summonType)
{
    Position pos = obj->GetRandomNearPosition(radius);
    return me->SummonCreature(entry, pos, summonType, despawnTime);
}

Creature* CreatureAI::DoSummonFlyer(uint32 entry, WorldObject* obj, float flightZ, float radius, Milliseconds despawnTime, TempSummonType summonType)
{
    Position pos = obj->GetRandomNearPosition(radius);
    pos.m_positionZ += flightZ;
    return me->SummonCreature(entry, pos, summonType, despawnTime);
}

VehicleAIBase::VehicleAIBase(Creature* c) : m_vehicleBase(c), m_vehicle(c->GetVehicleKit())
{
    LoadConditions();
}

VehicleAIBase::~VehicleAIBase()
{
    if (m_registered)
    {
        m_registered = false;
        sConditionMgr->UnregisterVehicleAI(this);
    }
}

void VehicleAIBase::UpdateAI(uint32 const diff)
{
    CheckConditions(diff);

    if (m_DoDismiss)
    {
        if (m_DismissTimer < diff)
        {
            m_DoDismiss = false;
            m_vehicleBase->SetVisible(false);
            m_vehicleBase->DespawnOrUnsummon();
        }
        else
            m_DismissTimer -= diff;
    }
}

void VehicleAIBase::Reset()
{
    m_vehicleBase->SetVisible(true);
}

void VehicleAIBase::OnCharmed(bool apply)
{
    if (m_IsVehicleInUse && !apply && !m_HasConditions)//was used and has conditions
    {
        m_DoDismiss = true;//needs reset
        m_vehicleBase->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_PLAYER_VEHICLE);
        m_vehicleBase->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    }
    else if (apply)
        m_DoDismiss = false;//in use again

    m_DismissTimer = VEHICLE_DISMISS_TIME;//reset timer
    m_IsVehicleInUse = apply;
}

void VehicleAIBase::LoadConditions()
{
    if (!m_registered)
    {
        m_registered = true;
        sConditionMgr->RegisterVehicleAI(this);
    }

    m_HasConditions = sConditionMgr->HasConditionsForNotGroupedEntry(CONDITION_SOURCE_TYPE_CREATURE_TEMPLATE_VEHICLE, m_vehicleBase->GetEntry());
}

void VehicleAIBase::CheckConditions(uint32 const diff)
{
    if (m_ConditionsTimer < diff)
    {
        if (m_HasConditions)
        {
            for (auto&& seat : m_vehicle->Seats)
            {
                if (Unit* passenger = ObjectAccessor::GetUnit(*m_vehicle->GetBase(), seat.second.Passenger.Guid))
                {
                    if (Player* player = passenger->ToPlayer())
                    {
                        //if (!sConditionMgr->IsObjectMeetToConditions(player, m_vehicleBase, conditions))
                        if (!sConditionMgr->IsObjectMeetingNotGroupedConditions(CONDITION_SOURCE_TYPE_CREATURE_TEMPLATE_VEHICLE, m_vehicleBase->GetEntry(), player, m_vehicleBase))
                        {
                            player->ExitVehicle();
                            return;//check other pessanger in next tick
                        }
                    }
                }
            }
        }
        m_ConditionsTimer = VEHICLE_CONDITION_CHECK_TIME;
    }
    else
        m_ConditionsTimer -= diff;
}

SummonablePremiumNpcAI::SummonablePremiumNpcAI(Creature* creature, uint32 displayId) : CreatureAI(creature)
{
    if (me->IsSummon())
    {
        me->SetDisplayId(displayId ? displayId : me->GetNativeDisplayId());
        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1); // Makes nameplate disappear, without this it may pop-up for a frame or two after summoning
        me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 50000.0f);
        me->SetPhaseMask(PHASEMASK_ANYWHERE, false);
        me->SetVisible(false); // Exploiting our visibility system to make summoned .premium NPCs only visible to the summoner
    }
}

SummonablePremiumNpcAI::PlayerOrChatHandler::PlayerOrChatHandler(ChatHandler* handler)
{
    m_player = handler->GetSession()->GetPlayer();
}

struct SummonablePremiumNpcSearcher
{
    uint32 Entry;
    uint64 PlayerGUID;
    Creature* Found = nullptr;

    SummonablePremiumNpcSearcher(uint32 entry, uint64 playerGUID) : Entry(entry), PlayerGUID(playerGUID) { }

    template<class NOT_INTERESTED>
    void Visit(GridRefManager<NOT_INTERESTED>&) { }
    void Visit(CreatureMapType& m)
    {
        if (!Found)
            for (auto&& i : m)
                if (Creature* c = i.GetSource())
                    if (c->GetEntry() == Entry)
                        if (TempSummon* summon = c->ToTempSummon())
                            if (summon->GetSummonerGUID() == PlayerGUID)
                            {
                                Found = c;
                                return;
                            }
    }
};

Creature* SummonablePremiumNpcAI::FindOrSpawn(PlayerOrChatHandler player, uint32 entry)
{
    ASSERT(entry >= 190000);
    ASSERT(sObjectMgr->GetCreatureTemplate(entry));

    SummonablePremiumNpcSearcher searcher { entry, player->GetGUID() };
    player->GetMap()->LoadGrid(0, 0);
    player->GetMap()->VisitGrid(0, 0, 1, searcher);
    player->UpdateVisibilityOf(searcher.Found ? searcher.Found : (searcher.Found = player->SummonCreature(entry, Position { 0, 0, 0 })));
    return searcher.Found;
}

Creature* SummonablePremiumNpcAI::OpenGossip(PlayerOrChatHandler player, uint32 entry)
{
    return OpenGossip(player, FindOrSpawn(player, entry));
}

Creature* SummonablePremiumNpcAI::OpenGossip(PlayerOrChatHandler player, Creature* creature)
{
    if (!creature)
        return nullptr;

    // Yeah, lol. Just to simulate the whole process, with aura interruptions and such, and avoid duplicating code
    ObjectGuid guid = creature->GetGUID();
    WorldPacket data(CMSG_GOSSIP_HELLO, 8);

    data.WriteBit(guid[2]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[1]);

    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[2]);

    player->GetSession()->HandleGossipHelloOpcode(data);
    return creature;
}
