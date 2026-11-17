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
#include "WorldSession.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Log.h"
#include "Opcodes.h"
#include "Spell.h"
#include "ObjectAccessor.h"
#include "CreatureAI.h"
#include "Util.h"
#include "Pet.h"
#include "World.h"
#include "Group.h"
#include "SpellInfo.h"
#include "Player.h"
#include "PlayerAI.h"
#include "DisableMgr.h"
#include "Vehicle.h"
#include "SpellHistory.h"
#include "MovementGenerator.h"

void WorldSession::HandleDismissCritter(WorldPackets::Pet::DismissCritter& packet)
{
    ObjectGuid const guid = packet.Guid;

    TC_LOG_DEBUG("network", "WORLD: Received CMSG_DISMISS_CRITTER for GUID " UI64FMTD, uint64(guid));

    Unit* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*_player, guid);

    if (!pet)
    {
        TC_LOG_DEBUG("network", "Vanitypet (guid: {}) does not exist - player '{}' (guid: {} / account: {}) attempted to dismiss it (possibly lagged out)",
            uint32(guid.GetCounter()), GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter(), GetAccountId());
        return;
    }

    if (_player->GetCritterGUID() == pet->GetGUID())
    {
         if (pet->GetTypeId() == TYPEID_UNIT && pet->ToCreature()->IsSummon())
             pet->ToTempSummon()->UnSummon();
    }
}

static bool ActAsMainPet(Unit const* summon, Unit const* mainpet, uint32 spellId, uint32 act, Player const* owner)
{
    if (!summon->IsAlive())
        return false;

    if (summon->IsPet())    // Stampede
    {
        if (act != ACT_DISABLED && act != ACT_PASSIVE && act != ACT_ENABLED)
            return true;

        std::vector<SpecializationSpellsEntry const*> const* specSpells = sDBCManager.GetSpecializationSpells(summon->ToPet()->GetSpecializationId());
        if (specSpells->size() == 0)
            return false;

        for (size_t j = 0; j < specSpells->size(); ++j)
        {
            SpecializationSpellsEntry const* specSpell = specSpells->at(j);
            if (specSpell)
                return true;
        }

        return false;
        // return std::find(specSpells->begin(), specSpells->end(), spellId) != specSpells->end(); // Only spec spells
    }

    if (act != ACT_DISABLED && act != ACT_PASSIVE && act != ACT_ENABLED)
        if (auto const* spellInfo = sSpellMgr->GetSpellInfo(summon->GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL)))
            if (spellInfo->GetCategory() == 1378) // Dire Beast
                return true;

    if (summon->GetEntry() == 24207) // Army of the Dead Ghoul
        return true;

    return mainpet->GetEntry() == summon->GetEntry() && owner->GetClass() != CLASS_WARLOCK; // Prevent Grimoire of Serivce;
}

void WorldSession::HandlePetAction(WorldPackets::Pet::PetAction& packet) //  sub_68C8FD [5.4.8 18291]
{
    ObjectGuid const petGUID = packet.PetGUID;
    ObjectGuid const targetGUID = packet.TargetGUID;
    uint32 const Action = packet.Action;
    float const x = packet.X;
    float const y = packet.Y;
    float const z = packet.Z;

    uint32 spellID = UNIT_ACTION_BUTTON_ACTION(Action);
    uint8 flag = UNIT_ACTION_BUTTON_TYPE(Action);             //delete = 0x07 CastSpell = C1

    // used also for charmed creature
    Unit* pet = ObjectAccessor::GetUnit(*_player, petGUID);
    TC_LOG_DEBUG("network", "HandlePetAction: Pet (GUID: {}) - flag: {}, spellID: {}, Target: {}.", petGUID.GetCounter(), uint32(flag), spellID, targetGUID.GetCounter());

    if (!pet)
    {
        TC_LOG_ERROR("network", "HandlePetAction: Pet (GUID: {}) doesn't exist for player {} (GUID: {})", petGUID.GetCounter(), GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter());
        return;
    }

    if (pet != GetPlayer()->GetFirstControlled())
    {
        TC_LOG_ERROR("network", "HandlePetAction: Pet (GUID: {}) does not belong to player {} (GUID: {})", petGUID.GetCounter(), GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter());
        return;
    }

    if (!pet->IsAlive())
    {
        SpellInfo const* spell = (flag == ACT_ENABLED || flag == ACT_PASSIVE) ? sSpellMgr->GetSpellInfo(spellID) : NULL;
        if (!spell)
            return;
        if (!(spell->Attributes & SPELL_ATTR0_CASTABLE_WHILE_DEAD))
            return;
    }

    /// @todo allow control charmed player?
    if (pet->GetTypeId() == TYPEID_PLAYER && !(flag == ACT_COMMAND && spellID == COMMAND_ATTACK))
        return;

    if (GetPlayer()->m_Controlled.size() == 1)
        HandlePetActionHelper(pet, petGUID, spellID, flag, targetGUID, x, y, z);
    else
    {
        //If a pet is dismissed, m_Controlled will change
        std::vector<Unit*> controlled;
        for (auto&& itr : GetPlayer()->m_Controlled)
            if (itr == pet || ActAsMainPet(itr, pet, spellID, flag, GetPlayer()))
                controlled.push_back(itr);
        for (auto&& pet : controlled)
            HandlePetActionHelper(pet, petGUID, spellID, flag, targetGUID, x, y, z);
    }
}

void WorldSession::HandlePetStopAttack(WorldPackets::Pet::PetStopAttack& packet)
{
    ObjectGuid const guid = packet.PetGUID;

    TC_LOG_DEBUG("network", "WORLD: Received CMSG_PET_STOP_ATTACK for GUID " "{}" "", (uint64)guid);

    Unit* pet = ObjectAccessor::GetUnit(*_player, guid);

    if (!pet)
    {
        TC_LOG_ERROR("network", "HandlePetStopAttack: Pet {} does not exist", uint32(guid.GetCounter()));
        return;
    }

    if (pet != GetPlayer()->GetPet() && pet != GetPlayer()->GetCharm())
    {
        TC_LOG_ERROR("network", "HandlePetStopAttack: Pet GUID {} isn't a pet or charmed creature of player {}",
            uint32(guid.GetCounter()), GetPlayer()->GetName().c_str());
        return;
    }

    if (!pet->IsAlive())
        return;

    pet->AttackStop();
}

void WorldSession::HandlePetActionHelper(Unit* pet, ObjectGuid guid1, uint32 spellid, uint16 flag, ObjectGuid guid2, float x, float y, float z)
{
    CharmInfo* charmInfo = pet->GetCharmInfo();
    if (!charmInfo)
    {
        TC_LOG_ERROR("network", "WorldSession::HandlePetAction(petGuid: " "{}" ", tagGuid: " "{}" ", spellId: {}, flag: {}): object (entry: {} TypeId: {}) is considered pet-like but doesn't have a charminfo!",
            guid1.GetRawValue(), guid2.GetRawValue(), spellid, flag, pet->GetGUID().GetCounter(), pet->GetTypeId());
        return;
    }

    switch (flag)
    {
        case ACT_COMMAND:                                   //0x07
            switch (spellid)
            {
                case COMMAND_STAY:                          //flat=1792  //STAY
                    pet->AttackStop();
                    pet->InterruptNonMeleeSpells(false);
                    if (pet->HasUnitState(UNIT_STATE_JUMPING | UNIT_STATE_CHARGING) && pet->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_CONTROLLED))
                    {
                        // If the pet is in the process of some controlled motion, only reset active motion slot, so that the controlled motion can properly finish
                        float x, y, z;
                        pet->GetMotionMaster()->GetDestination(x, y, z);
                        pet->GetMotionMaster()->MovePoint(0, x, y, z);
                        pet->GetMotionMaster()->MoveIdle();
                    }
                    else
                    {
                        pet->GetMotionMaster()->Clear(false);
                        pet->GetMotionMaster()->MoveIdle();
                        pet->StopMoving();
                    }
                    charmInfo->SetCommandState(COMMAND_STAY);

                    charmInfo->SetIsCommandAttack(false);
                    charmInfo->SetIsAtStay(true);
                    charmInfo->SetIsCommandFollow(false);
                    charmInfo->SetIsFollowing(false);
                    charmInfo->SetIsReturning(false);
                    charmInfo->SaveStayPosition();
                    break;
                case COMMAND_FOLLOW:                        //spellid=1792  //FOLLOW
                    pet->AttackStop();
                    pet->InterruptNonMeleeSpells(false);
                    pet->GetMotionMaster()->MoveFollow(_player, PET_FOLLOW_DIST, pet->GetFollowAngle());
                    charmInfo->SetCommandState(COMMAND_FOLLOW);

                    charmInfo->SetIsCommandAttack(false);
                    charmInfo->SetIsAtStay(false);
                    charmInfo->SetIsReturning(true);
                    charmInfo->SetIsCommandFollow(true);
                    charmInfo->SetIsFollowing(false);
                    break;
                case COMMAND_ATTACK:                        //spellid=1792  //ATTACK
                {
                    // Can't attack if owner is pacified
                    if (_player->HasAuraType(SPELL_AURA_MOD_PACIFY))
                    {
                        //pet->SendPetCastFail(spellid, SPELL_FAILED_PACIFIED);
                        /// @todo Send proper error message to client
                        return;
                    }

                    // only place where pet can be player
                    Unit* targetUnit = ObjectAccessor::GetUnit(*_player, guid2);
                    if (!targetUnit)
                        return;

                    if (Unit* owner = pet->GetOwner())
                        if (!owner->IsValidAttackTarget(targetUnit))
                            return;

                    pet->ClearUnitState(UNIT_STATE_FOLLOW);
                    // This is true if pet has no target or has target but targets differs.
                    if (pet->GetVictim() != targetUnit || (pet->GetVictim() == targetUnit && !pet->GetCharmInfo()->IsCommandAttack()))
                    {
                        if (pet->GetVictim())
                            pet->AttackStop();

                        if (pet->GetTypeId() != TYPEID_PLAYER && pet->ToCreature()->IsAIEnabled)
                        {
                            charmInfo->SetIsCommandAttack(true);
                            charmInfo->SetIsAtStay(false);
                            charmInfo->SetIsFollowing(false);
                            charmInfo->SetIsCommandFollow(false);
                            charmInfo->SetIsReturning(false);

                            pet->ToCreature()->AI()->AttackStart(targetUnit);

                            //10% chance to play special pet attack talk, else growl
                            if (pet->ToCreature()->IsPet() && ((Pet*)pet)->getPetType() == SUMMON_PET && pet != targetUnit && urand(0, 100) < 10)
                                pet->SendPetTalk((uint32)PET_TALK_ATTACK);
                            else
                            {
                                // 90% chance for pet and 100% chance for charmed creature
                                pet->SendPetAIReaction(guid1);
                            }
                        }
                        else                                // charmed player
                        {
                            if (pet->GetVictim() && pet->GetVictim() != targetUnit)
                                pet->AttackStop();

                            charmInfo->SetIsCommandAttack(true);
                            charmInfo->SetIsAtStay(false);
                            charmInfo->SetIsFollowing(false);
                            charmInfo->SetIsCommandFollow(false);
                            charmInfo->SetIsReturning(false);

                            if (pet->Attack(targetUnit, true))
                            {
                                if (pet->GetTypeId() == TYPEID_PLAYER)
                                {
                                    if (!pet->isPossessed())
                                    {
                                        if (PlayerAI::IsPlayerRangedAttacker(pet->ToPlayer()))
                                            pet->GetMotionMaster()->MoveChase(targetUnit, 28.0f, 0.0f);
                                        else
                                            pet->GetMotionMaster()->MoveChase(targetUnit);
                                    }
                                    // Don't chase target when possessing a player
                                }
                                else
                                    pet->GetMotionMaster()->MoveChase(targetUnit);
                            }
                            pet->SendPetAIReaction(guid1);
                        }
                    }
                    break;
                }
                case COMMAND_ABANDON:                       // abandon (hunter pet) or dismiss (summoned pet)
                    if (pet->GetCharmerGUID() == GetPlayer()->GetGUID())
                    {
                        
                        _player->StopCastingCharm();
                    }
                    else if (pet->GetOwnerGUID() == GetPlayer()->GetGUID())
                    {
                        ASSERT(pet->GetTypeId() == TYPEID_UNIT);
                        if (pet->IsPet())
                        {
                            if (((Pet*)pet)->getPetType() == HUNTER_PET)
                                GetPlayer()->RemovePet(PET_REMOVE_ABANDON);
                            else
                                //dismissing a summoned pet is like killing them (this prevents returning a soulshard...)
                                pet->setDeathState(CORPSE);
                        }
                        else if (pet->HasUnitTypeMask(UNIT_MASK_MINION))
                        {
                            ((Minion*)pet)->UnSummon();
                        }
                    }
                    break;
                case COMMAND_MOVE_TO:
                    if (!pet->HasUnitState(UNIT_STATE_STUNNED | UNIT_STATE_ROOT))
                    {
                        pet->GetMotionMaster()->MovePoint(0, x, y, z);
                        charmInfo->SetCommandState(COMMAND_MOVE_TO);

                        charmInfo->SetIsCommandAttack(false);
                        charmInfo->SetIsFollowing(false);
                        charmInfo->SetIsReturning(false);
                        charmInfo->SaveStayPosition(x, y, z);
                    }
                    break;
                default:
                    TC_LOG_ERROR("network", "WORLD: unknown PET flag Action {} and spellid {}.", uint32(flag), spellid);
            }
            if (pet->GetTypeId() == TYPEID_UNIT)
                pet->ToCreature()->AI()->OnPetCommand(CommandStates(spellid));
            break;
        case ACT_REACTION:                                  // 0x6
            switch (spellid)
            {
                case REACT_PASSIVE:                         //passive
                    pet->AttackStop();

                case REACT_DEFENSIVE:                       //recovery
                case REACT_AGGRESSIVE:                      //activete
                case REACT_ASSIST:                          //assist
                    if (pet->GetTypeId() == TYPEID_UNIT)
                        pet->ToCreature()->SetReactState(ReactStates(spellid));
                    break;
            }
            break;
        case ACT_DISABLED:                                  // 0x81    spell (disabled), ignore
        case ACT_PASSIVE:                                   // 0x01
        case ACT_ENABLED:                                   // 0xC1    spell
        {
            Unit* unit_target = guid2 ? ObjectAccessor::GetUnit(*_player, guid2) : nullptr;

            // do not cast unknown spells
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellid);
            if (!spellInfo)
            {
                TC_LOG_ERROR("network", "WORLD: unknown PET spell id {}", spellid);
                return;
            }

            for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            {
                if (spellInfo->Effects[i].TargetA.GetTarget() == TARGET_UNIT_SRC_AREA_ENEMY || spellInfo->Effects[i].TargetA.GetTarget() == TARGET_UNIT_DEST_AREA_ENEMY || spellInfo->Effects[i].TargetA.GetTarget() == TARGET_DEST_DYNOBJ_ENEMY)
                    return;
            }

            // do not cast not learned spells
            if (!pet->HasSpell(spellid) || spellInfo->IsPassive())
                return;

            //  Clear the flags as if owner clicked 'attack'. AI will reset them
            //  after AttackStart, even if spell failed
            if (pet->GetCharmInfo())
            {
                pet->GetCharmInfo()->SetIsAtStay(false);
                pet->GetCharmInfo()->SetIsCommandAttack(true);
                pet->GetCharmInfo()->SetIsReturning(false);
                pet->GetCharmInfo()->SetIsFollowing(false);
            }

            Spell* spell = new Spell(pet, spellInfo, TRIGGERED_NONE);

            SpellCastResult result = spell->CheckPetCast(unit_target);

            //auto turn to target unless possessed
            if (result == SPELL_FAILED_UNIT_NOT_INFRONT && !pet->isPossessed() && !pet->IsVehicle())
            {
                if (unit_target)
                {
                    pet->SetInFront(unit_target);
                    if (Player* player = unit_target->ToPlayer())
                        pet->SendUpdateToPlayer(player);
                }
                else if (Unit* unit_target2 = spell->m_targets.GetUnitTarget())
                {
                    pet->SetInFront(unit_target2);
                    if (Player* player = unit_target2->ToPlayer())
                        pet->SendUpdateToPlayer(player);
                }

                if (Unit* powner = pet->GetCharmerOrOwner())
                    if (Player* player = powner->ToPlayer())
                        pet->SendUpdateToPlayer(player);

                result = SPELL_CAST_OK;
            }

            if (result == SPELL_CAST_OK)
            {
                unit_target = spell->m_targets.GetUnitTarget();

                //10% chance to play special pet attack talk, else growl
                //actually this only seems to happen on special spells, fire shield for imp, torment for voidwalker, but it's stupid to check every spell
                if (pet->ToCreature()->IsPet() && (((Pet*)pet)->getPetType() == SUMMON_PET) && (pet != unit_target) && (urand(0, 100) < 10))
                    pet->SendPetTalk((uint32)PET_TALK_SPECIAL_SPELL);
                else
                {
                    pet->SendPetAIReaction(guid1);
                }

                if (unit_target && !GetPlayer()->IsFriendlyTo(unit_target) && !pet->isPossessed() && !pet->IsVehicle())
                {
                    // This is true if pet has no target or has target but targets differs.
                    if (pet->GetVictim() != unit_target)
                    {
                        if (pet->GetVictim())
                            pet->AttackStop();
                        pet->GetMotionMaster()->Clear();
                        if (pet->ToCreature()->IsAIEnabled)
                            pet->ToCreature()->AI()->AttackStart(unit_target);
                    }
                }

                spell->prepare(&(spell->m_targets));
            }
            else
            {
                auto canChase = [pet, spell, spellInfo](SpellCastResult result)
                {
                    // Only in these cases
                    if (result != SPELL_FAILED_OUT_OF_RANGE &&
                        result != SPELL_FAILED_LINE_OF_SIGHT)
                        return false;

                    // Don't queue spell while ghoul is jumping...
                    if (pet->GetCurrentSpell(CURRENT_GENERIC_SPELL) && pet->GetCurrentSpell(CURRENT_GENERIC_SPELL)->GetSpellInfo()->Id == 47482 ||
                        pet->HasUnitState(UNIT_STATE_JUMPING | UNIT_STATE_CHARGING) && pet->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_CONTROLLED))
                        return false;

                    if (pet->HasUnitState(UNIT_STATE_CONTROLLED))
                        return false;

                    // Pet under direct player's control, we shouldn't let it roam around freely
                    if (pet->isPossessed() || pet->IsVehicle())
                        return false;

                    // Hacks
                    switch (pet->GetEntry())
                    {
                        // Chess pieces in Karazhan
                        case 17469: case 21748: case 21750: case 21747: case 21726: case 21752:
                        case 17211: case 21664: case 21683: case 21682: case 21160: case 21684:
                            return false;
                    }

                    // Perform additional LOS check to prevent pets from passing GameObjects like doors etc
                    if (!pet->PetNeedsLOSCheckOnAttackStart())
                        return true;

                    // In this case LOS check has already failed, no need to repeat it
                    if (result == SPELL_FAILED_LINE_OF_SIGHT)
                        return true;

                    // Perform a LOS check in case it hasn't been already
                    if (spellInfo->HasAttribute(SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS) || DisableMgr::IsDisabledFor(DISABLE_TYPE_SPELL, spellInfo->Id, nullptr, SPELL_DISABLE_LOS))
                        return true;

                    if (WorldObject* target = spell->m_targets.GetObjectTarget())
                    {
                        if (pet->IsWithinLOSInMap(target))
                            return true;
                    }
                    if (spell->m_targets.HasDst())
                    {
                        float x, y, z;
                        spell->m_targets.GetDstPos()->GetPosition(x, y, z);

                        if (pet->IsWithinLOS(x, y, z))
                            return true;
                    }
                    return false;
                };

                if (pet->GetTypeId() == TYPEID_UNIT && canChase(result))
                {
                    GetPlayer()->SendClearCooldown(spellid, pet);
                    pet->ToCreature()->AI()->SpellRequiresMovement(unit_target, spell);
                    return;
                }

                if (pet->isPossessed() || pet->IsVehicle()) /// @todo: confirm this check
                    Spell::SendCastResult(GetPlayer(), spellInfo, 0, result);
                else
                    spell->SendPetCastResult(result);

                if (pet->GetSpellHistory()->IsReady(spellid))
                    GetPlayer()->SendClearCooldown(spellid, pet);

                spell->finish(false);
                delete spell;

                // reset specific flags in case of spell fail. AI will reset other flags
                if (pet->GetCharmInfo())
                    pet->GetCharmInfo()->SetIsCommandAttack(false);
            }
            break;
        }
        default:
            TC_LOG_ERROR("network", "WORLD: unknown PET flag Action {} and spellid {}.", uint32(flag), spellid);
    }
}

void WorldSession::HandlePetNameQuery(WorldPackets::Pet::PetNameQuery& packet)
{
    TC_LOG_INFO("network", "HandlePetNameQuery. CMSG_QUERY_PET_NAME");

    SendPetNameQuery(packet.PetGuid, packet.PetNumber);
}

void WorldSession::SendPetNameQuery(ObjectGuid petGuid, uint64 petNumber)
{
    Creature* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*_player, petGuid);
    if (!pet)
    {
        WorldPacket data(SMSG_QUERY_PET_NAME_RESPONSE, (8 + 1));
        data.WriteBit(0);
        data.FlushBits();
        data << uint64(petNumber);

        _player->GetSession()->SendPacket(&data);
        return;
    }

    WorldPacket data(SMSG_QUERY_PET_NAME_RESPONSE, (8 + 1 + 1 + 5 + pet->GetName().size() + 4));
    data.WriteBit(1);                               // has data

    bool declinedNames = pet->IsPet() && ((Pet*)pet)->GetDeclinedNames();

    for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
    {
        if (declinedNames)
            data.WriteBits(((Pet*)pet)->GetDeclinedNames()->name[i].size(), 7);
        else
            data.WriteBits(0, 7);
    }

    data.WriteBit(0);                               // unknown
    data.WriteBits(pet->GetName().size(), 8);
    data.FlushBits();

    if (declinedNames)
        for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            data.WriteString(((Pet*)pet)->GetDeclinedNames()->name[i]);

    data.WriteString(pet->GetName());
    data << uint32(pet->GetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP));
    data << uint64(petNumber);

    _player->GetSession()->SendPacket(&data);
}

bool WorldSession::CheckStableMaster(ObjectGuid guid)
{
    // spell case or GM
    if (guid == GetPlayer()->GetGUID())
    {
        if (!GetPlayer()->IsGameMaster() && !GetPlayer()->HasAuraType(SPELL_AURA_OPEN_STABLE))
        {
            TC_LOG_DEBUG("network", "Player (GUID:{}) attempt open stable in cheating way.", guid.GetCounter());
            return false;
        }
    }
    // stable master case
    else
    {
        if (!GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_STABLEMASTER))
        {
            TC_LOG_DEBUG("network", "Stablemaster (GUID:{}) not found or you can't interact with him.", guid.GetCounter());
            return false;
        }
    }
    return true;
}

void WorldSession::HandlePetSetAction(WorldPackets::Pet::PetSetAction& packet)
{
    TC_LOG_INFO("network", "HandlePetSetAction. CMSG_PET_SET_ACTION");

    ObjectGuid const petguid = packet.PetGUID;
    uint32 const position = packet.Index;
    uint32 const data = packet.Action;

    Unit* pet = ObjectAccessor::GetUnit(*_player, petguid);

    if (!pet || pet != _player->GetFirstControlled())
    {
        TC_LOG_ERROR("network", "HandlePetSetAction: Unknown pet (GUID: {}) or pet owner (GUID: {})", petguid.GetCounter(), _player->GetGUID().GetCounter());
        return;
    }

    CharmInfo* charmInfo = pet->GetCharmInfo();
    if (!charmInfo)
    {
        TC_LOG_ERROR("network", "WorldSession::HandlePetSetAction: object (GUID: {} TypeId: {}) is considered pet-like but doesn't have a charminfo!", pet->GetGUID().GetCounter(), pet->GetTypeId());
        return;
    }

    uint8 act_state = UNIT_ACTION_BUTTON_TYPE(data);

    // ignore invalid position
    if (position >= MAX_UNIT_ACTION_BAR_INDEX)
        return;

    uint8 act_state_0 = UNIT_ACTION_BUTTON_TYPE(data);
    // check swap (at command->spell swap client remove spell first in another packet, so check only command move correctness)
    if ((act_state_0 == ACT_COMMAND && UNIT_ACTION_BUTTON_ACTION(data) != COMMAND_MOVE_TO) || act_state_0 == ACT_REACTION)
    {
        uint32 spell_id_0 = UNIT_ACTION_BUTTON_ACTION(data);
        UnitActionBarEntry const* actionEntry_1 = charmInfo->GetActionBarEntry(position);
        if (!actionEntry_1)
            return;

        if (actionEntry_1->packedData)
        {
            //if (spell_id_0 != actionEntry_1->GetAction())
            //    return;

            if (act_state_0 != actionEntry_1->GetType())
                return;
        }
    }

    uint32 spell_id = UNIT_ACTION_BUTTON_ACTION(data);
    // if it's act for spell (en/disable/cast) and there is a spell given (0 = remove spell) which pet doesn't know, don't add
    if (!((act_state == ACT_ENABLED || act_state == ACT_DISABLED || act_state == ACT_PASSIVE) && spell_id && !pet->HasSpell(spell_id)))
    {
        if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell_id))
        {
            // sign for autocast
            if (act_state == ACT_ENABLED)
            {
                if (pet->GetTypeId() == TYPEID_UNIT && pet->ToCreature()->IsPet())
                    ((Pet*)pet)->ToggleAutocast(spellInfo, true);
                else
                    for (Unit::ControlList::iterator itr = GetPlayer()->m_Controlled.begin(); itr != GetPlayer()->m_Controlled.end(); ++itr)
                        if ((*itr)->GetEntry() == pet->GetEntry())
                            (*itr)->GetCharmInfo()->ToggleCreatureAutocast(spellInfo, true);
            }
            // sign for no/turn off autocast
            else if (act_state == ACT_DISABLED)
            {
                if (pet->GetTypeId() == TYPEID_UNIT && pet->ToCreature()->IsPet())
                    ((Pet*)pet)->ToggleAutocast(spellInfo, false);
                else
                    for (Unit::ControlList::iterator itr = GetPlayer()->m_Controlled.begin(); itr != GetPlayer()->m_Controlled.end(); ++itr)
                        if ((*itr)->GetEntry() == pet->GetEntry())
                            (*itr)->GetCharmInfo()->ToggleCreatureAutocast(spellInfo, false);
            }
        }

        charmInfo->SetActionBar(position, spell_id, ActiveStates(act_state));
    }
}

void WorldSession::HandlePetRename(WorldPackets::Pet::PetRename& packet)
{
    TC_LOG_INFO("network", "HandlePetRename. CMSG_PET_RENAME");

    std::string const name = packet.Name;
    DeclinedName declinedname;
    if (packet.IsDeclined)
        for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            declinedname.name[i] = packet.DeclinedNames[i];

    Pet* pet = GetPlayer()->GetPet();

                                                            // check it!
    if (!pet || !pet->IsPet() || ((Pet*)pet)->getPetType()!= HUNTER_PET ||
        !pet->HasByteFlag(UNIT_FIELD_BYTES_2, 2, UNIT_CAN_BE_RENAMED) ||
        pet->GetOwnerGUID() != _player->GetGUID() || !pet->GetCharmInfo())
        return;

    PetNameInvalidReason res = ObjectMgr::CheckPetName(name);
    if (res != PET_NAME_SUCCESS)
    {
        SendPetNameInvalid(res, name, NULL);
        return;
    }

    if (sObjectMgr->IsReservedName(name))
    {
        SendPetNameInvalid(PET_NAME_RESERVED, name, NULL);
        return;
    }

    pet->SetName(name);

    Player* owner = pet->GetOwner();
    if (owner && owner->GetGroup())
        owner->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_NAME);

    pet->RemoveByteFlag(UNIT_FIELD_BYTES_2, 2, UNIT_CAN_BE_RENAMED);

    if (packet.IsDeclined)
    {
        if (!ObjectMgr::CheckDeclinedNames(name, declinedname))
        {
            SendPetNameInvalid(PET_NAME_DECLENSION_DOESNT_MATCH_BASE_NAME, name, &declinedname);
            return;
        }
    }

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    if (packet.IsDeclined)
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_PET_DECLINEDNAME);
        stmt->setUInt32(0, pet->GetCharmInfo()->GetPetNumber());
        trans->Append(stmt);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_ADD_CHAR_PET_DECLINEDNAME);
        stmt->setUInt32(0, _player->GetGUID().GetCounter());

        for (uint8 i = 0; i < 5; i++)
            stmt->setString(i+1, declinedname.name[i]);

        trans->Append(stmt);
    }

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_PET_NAME);
    stmt->setString(0, name);
    stmt->setUInt32(1, _player->GetGUID().GetCounter());
    stmt->setUInt32(2, pet->GetCharmInfo()->GetPetNumber());
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);

    pet->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, uint32(time(NULL))); // cast can't be helped
}

void WorldSession::HandlePetAbandon(WorldPackets::Pet::PetAbandon& packet)
{
    ObjectGuid const guid = packet.Pet;

    TC_LOG_INFO("network", "HandlePetAbandon. CMSG_PET_ABANDON pet guid is {}", guid.GetCounter());

    if (!_player->IsInWorld())
        return;

    // pet/charmed
    Creature* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*_player, guid);
    if (pet)
    {
        if (pet->IsPet())
        {
            _player->RemovePet(PET_REMOVE_ABANDON);
            if (GetPlayer()->GetClass() == CLASS_HUNTER)
                SendPetList(ObjectGuid::Empty, PET_SLOT_ACTIVE_FIRST, PET_SLOT_ACTIVE_LAST);
        }
        else if (pet->GetGUID() == _player->GetCharmGUID())
        {
            
            _player->StopCastingCharm();
        }
    }
}

void WorldSession::HandlePetSpellAutocastOpcode(WorldPackets::Pet::PetSpellAutocast& packet)
{
    TC_LOG_INFO("network", "CMSG_PET_SPELL_AUTOCAST");
    ObjectGuid const PetGUID = packet.PetGUID;
    uint32 const SpellID = packet.SpellID;
    bool const State = packet.AutocastEnabled;

    if (!_player->GetGuardianPet() && !_player->GetCharm())
        return;

    if (ObjectAccessor::FindPlayer(PetGUID))
        return;

    Creature* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*_player, PetGUID);

    if (!pet || (pet != _player->GetGuardianPet() && pet != _player->GetCharm()))
    {
        TC_LOG_ERROR("network", "HandlePetSpellAutocastOpcode.Pet {} isn't pet of player {} (GUID: {}).", PetGUID.GetCounter(), GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter());
        return;
    }

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SpellID);
    // do not add not learned spells/ passive spells
    if (!pet->HasSpell(SpellID) || !spellInfo->IsAutocastable())
        return;

    CharmInfo* charmInfo = pet->GetCharmInfo();
    if (!charmInfo)
    {
        TC_LOG_ERROR("network", "WorldSession::HandlePetSpellAutocastOpcod: object (GUID: {} TypeId: {}) is considered pet-like but doesn't have a charminfo!", pet->GetGUID().GetCounter(), pet->GetTypeId());
        return;
    }

    if (pet->IsPet())
        ((Pet*)pet)->ToggleAutocast(spellInfo, State);
    else
        pet->GetCharmInfo()->ToggleCreatureAutocast(spellInfo, State);

    charmInfo->SetSpellAutocast(spellInfo, State);
}

void WorldSession::HandlePetCastSpellOpcode(WorldPackets::Pet::PetCastSpell& packet)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_PET_CAST_SPELL");

    uint8 const castCount = packet.CastCount;
    uint32 const spellId = packet.SpellId;
    uint8 const castFlags = packet.CastFlags;
    uint32 const targetFlags = packet.TargetFlags;
    ObjectGuid const targetGUID = packet.TargetGuid;
    ObjectGuid const itemGUID = packet.ItemGuid;
    ObjectGuid const transportDstGUID = packet.TransportDstGuid;
    ObjectGuid const transportSrcGUID = packet.TransportSrcGuid;
    ObjectGuid const petGuid = packet.PetGuid;
    WorldLocation const& dstLoc = packet.DstLoc;
    WorldLocation const& srcLoc = packet.SrcLoc;
    float const missileSpeed = packet.MissileSpeed;
    float const elevation = packet.Elevation;

    TC_LOG_DEBUG("network", "WORLD: CMSG_PET_CAST_SPELL, guid: " "{}" ", castCount: {}, spellId {}, castFlags {}", uint64(petGuid), castCount, spellId, castFlags);

    // This opcode is also sent from charmed and possessed units (players and creatures)
    if (!_player->GetGuardianPet() && !_player->GetCharm())
        return;

    Unit* caster = ObjectAccessor::GetUnit(*_player, petGuid);

    if (!caster || (caster != _player->GetGuardianPet() && caster != _player->GetCharm()))
    {
        TC_LOG_ERROR("network", "HandlePetCastSpellOpcode: Pet {} isn't pet of player {} (GUID: {}).", petGuid.GetCounter(), GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter());
        return;
    }

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
    {
        TC_LOG_ERROR("network", "WORLD: unknown PET spell id {}", spellId);
        return;
    }

    // do not cast not learned spells
    if (!caster->HasSpell(spellId) || spellInfo->IsPassive())
        return;

    SpellCastTargets targets;
    targets.Initialize(targetFlags, targetGUID, itemGUID, transportDstGUID, dstLoc, transportSrcGUID, srcLoc);
    targets.SetElevation(elevation);
    targets.SetSpeed(missileSpeed);
    targets.Update(caster);

    caster->ClearUnitState(UNIT_STATE_FOLLOW);

    Spell* spell = new Spell(caster, spellInfo, TRIGGERED_NONE);
    spell->m_cast_count = castCount;                    // probably pending spell cast
    spell->m_targets = targets;

    Unit* unit_target = targetGUID ? ObjectAccessor::GetUnit(*_player, targetGUID) : nullptr;
    if (!unit_target && caster->GetVehicleKit() && caster->GetVehicleKit()->GetPassenger(0))
        unit_target = caster->GetVehicleKit()->GetPassenger(0);
    SpellCastResult result = spell->CheckPetCast(unit_target ? unit_target : nullptr);

    if (result == SPELL_CAST_OK)
    {
        spell->prepare(&(spell->m_targets));

        if (Creature* creature = caster->ToCreature())
        {
            creature->AddCreatureSpellCooldown(spellId);
            if (Pet* pet = creature->ToPet())
            {
                // 10% chance to play special pet attack talk, else growl
                // actually this only seems to happen on special spells, fire shield for imp, torment for voidwalker, but it's stupid to check every spell
                if (pet->getPetType() == SUMMON_PET && (urand(0, 100) < 10))
                    pet->SendPetTalk(PET_TALK_SPECIAL_SPELL);
                else
                    pet->SendPetAIReaction(petGuid);
            }
        }
    }
    else
    {
        spell->SendPetCastResult(result);

        if (caster->GetTypeId() == TYPEID_PLAYER)
        {
            if (!caster->ToPlayer()->HasSpellCooldown(spellId))
                GetPlayer()->SendClearCooldown(spellId, caster);
        }
        else
        {
            if (!caster->ToCreature()->HasSpellCooldown(spellId))
                GetPlayer()->SendClearCooldown(spellId, caster);
        }

        spell->finish(false);
        delete spell;
    }
}

void WorldSession::SendPetNameInvalid(uint32 error, const std::string& name, DeclinedName* declinedName)
{
    WorldPacket data(SMSG_PET_NAME_INVALID, 4 + name.size() + 1 + 1);
    data.WriteBit(name.size() == 0);
    if (name.size())
        data.WriteBits(name.size(), 8);
    data.WriteBit(bool(declinedName));

    if (declinedName)
        for (uint32 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            data.WriteBits(declinedName->name[i].size(), 7);

    data.FlushBits();

    if (declinedName)
        for (uint32 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            if (declinedName->name[i].size())
                data.WriteString(declinedName->name[i]);

    if (name.size())
        data.WriteString(name);

    data << uint8(1);
    data << uint32(error);

    SendPacket(&data);
}

void WorldSession::HandlePetLearnTalent(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_PET_LEARN_TALENT");
}

void WorldSession::HandleLearnPreviewTalentsPet(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_LEARN_PREVIEW_TALENTS_PET");
}

void WorldSession::HandleLearnPetSpecialization(WorldPackets::Pet::LearnPetSpecializationGroup& packet)
{
    TC_LOG_DEBUG("network", "CMSG_LEARN_PET_SPECIALIZATION_GROUP");

    uint32 const index = packet.Index;

    if (_player->GetClass() != CLASS_HUNTER || _player->IsInCombat())
        return;

    uint32 specializationId = 0;
    switch (index)
    {
        case 0:
            specializationId = SPEC_PET_FEROCITY;
            break;
        case 1:
            specializationId = SPEC_PET_TENACITY;
            break;
        case 2:
            specializationId = SPEC_PET_CUNNING;
            break;
        default:
            break;
    }

    if (!specializationId)
        return;

    Pet* pet = _player->GetPet();
    if (!pet)
        return;

    if (pet->getPetType() != PetType::HUNTER_PET)
        return;

    pet->SetSpecialization(specializationId);
}
