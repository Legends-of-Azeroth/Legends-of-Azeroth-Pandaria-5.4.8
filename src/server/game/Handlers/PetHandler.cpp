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

void WorldSession::HandleDismissCritter(WorldPacket& recvData)
{
    ObjectGuid guid;

    guid[4] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[6]);

    TC_LOG_DEBUG("network", "WORLD: Received CMSG_DISMISS_CRITTER for GUID " UI64FMTD, uint64(guid));

    Unit* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*_player, guid);

    if (!pet)
    {
        TC_LOG_DEBUG("network", "Vanitypet (guid: %u) does not exist - player '%s' (guid: %u / account: %u) attempted to dismiss it (possibly lagged out)",
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

void WorldSession::HandlePetAction(WorldPacket& recvData) //  sub_68C8FD [5.4.8 18291]
{
    ObjectGuid petGUID, targetGUID;
    uint32 Action;
    float x, y, z;
    recvData >> Action;

    // Position
    recvData >> y >> z >> x;
    petGUID[1] = recvData.ReadBit();
    petGUID[0] = recvData.ReadBit();
    petGUID[6] = recvData.ReadBit();
    petGUID[7] = recvData.ReadBit();
    petGUID[5] = recvData.ReadBit();
    targetGUID[7] = recvData.ReadBit();
    petGUID[2] = recvData.ReadBit();
    petGUID[3] = recvData.ReadBit();
    targetGUID[6] = recvData.ReadBit();
    targetGUID[3] = recvData.ReadBit();
    targetGUID[0] = recvData.ReadBit();
    targetGUID[2] = recvData.ReadBit();
    targetGUID[5] = recvData.ReadBit();
    petGUID[4] = recvData.ReadBit();
    targetGUID[4] = recvData.ReadBit();
    targetGUID[1] = recvData.ReadBit();

    recvData.ReadByteSeq(petGUID[7]);
    recvData.ReadByteSeq(petGUID[6]);
    recvData.ReadByteSeq(petGUID[1]);
    recvData.ReadByteSeq(petGUID[2]);
    recvData.ReadByteSeq(petGUID[5]);
    recvData.ReadByteSeq(petGUID[4]);
    recvData.ReadByteSeq(targetGUID[5]);
    recvData.ReadByteSeq(petGUID[3]);
    recvData.ReadByteSeq(targetGUID[0]);
    recvData.ReadByteSeq(targetGUID[1]);
    recvData.ReadByteSeq(targetGUID[7]);
    recvData.ReadByteSeq(targetGUID[4]);
    recvData.ReadByteSeq(targetGUID[6]);
    recvData.ReadByteSeq(targetGUID[2]);
    recvData.ReadByteSeq(targetGUID[3]);
    recvData.ReadByteSeq(petGUID[0]);

    uint32 spellID = UNIT_ACTION_BUTTON_ACTION(Action);
    uint8 flag = UNIT_ACTION_BUTTON_TYPE(Action);             //delete = 0x07 CastSpell = C1

    // used also for charmed creature
    Unit* pet = ObjectAccessor::GetUnit(*_player, petGUID);
    TC_LOG_DEBUG("network", "HandlePetAction: Pet (GUID: %u) - flag: %u, spellID: %u, Target: %u.", petGUID.GetCounter(), uint32(flag), spellID, targetGUID.GetCounter());

    if (!pet)
    {
        TC_LOG_ERROR("network", "HandlePetAction: Pet (GUID: %u) doesn't exist for player %s (GUID: %u)", petGUID.GetCounter(), GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter());
        return;
    }

    if (pet != GetPlayer()->GetFirstControlled())
    {
        TC_LOG_ERROR("network", "HandlePetAction: Pet (GUID: %u) does not belong to player %s (GUID: %u)", petGUID.GetCounter(), GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter());
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

void WorldSession::HandlePetStopAttack(WorldPacket &recvData)
{
    ObjectGuid guid;

    guid[7] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[3]);

    TC_LOG_DEBUG("network", "WORLD: Received CMSG_PET_STOP_ATTACK for GUID " UI64FMTD "", (uint64)guid);

    Unit* pet = ObjectAccessor::GetUnit(*_player, guid);

    if (!pet)
    {
        TC_LOG_ERROR("network", "HandlePetStopAttack: Pet %u does not exist", uint32(guid.GetCounter()));
        return;
    }

    if (pet != GetPlayer()->GetPet() && pet != GetPlayer()->GetCharm())
    {
        TC_LOG_ERROR("network", "HandlePetStopAttack: Pet GUID %u isn't a pet or charmed creature of player %s",
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
        TC_LOG_ERROR("network", "WorldSession::HandlePetAction(petGuid: " UI64FMTD ", tagGuid: " UI64FMTD ", spellId: %u, flag: %u): object (entry: %u TypeId: %u) is considered pet-like but doesn't have a charminfo!",
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
                        _player->StopCastingCharm();
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
                    TC_LOG_ERROR("network", "WORLD: unknown PET flag Action %i and spellid %i.", uint32(flag), spellid);
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
                TC_LOG_ERROR("network", "WORLD: unknown PET spell id %i", spellid);
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
            TC_LOG_ERROR("network", "WORLD: unknown PET flag Action %i and spellid %i.", uint32(flag), spellid);
    }
}

void WorldSession::HandlePetNameQuery(WorldPacket& recvData)
{
    TC_LOG_INFO("network", "HandlePetNameQuery. CMSG_PET_NAME_QUERY");

    ObjectGuid petGuid;
    ObjectGuid petNumber;

    petNumber[0] = recvData.ReadBit();
    petNumber[5] = recvData.ReadBit();
    petGuid[1] = recvData.ReadBit();
    petGuid[7] = recvData.ReadBit();
    petNumber[7] = recvData.ReadBit();
    petGuid[6] = recvData.ReadBit();
    petGuid[4] = recvData.ReadBit();
    petGuid[5] = recvData.ReadBit();
    petGuid[0] = recvData.ReadBit();
    petNumber[3] = recvData.ReadBit();
    petNumber[6] = recvData.ReadBit();
    petNumber[2] = recvData.ReadBit();
    petGuid[3] = recvData.ReadBit();
    petGuid[2] = recvData.ReadBit();
    petNumber[1] = recvData.ReadBit();
    petNumber[4] = recvData.ReadBit();

    recvData.ReadByteSeq(petNumber[2]);
    recvData.ReadByteSeq(petNumber[1]);
    recvData.ReadByteSeq(petNumber[0]);
    recvData.ReadByteSeq(petNumber[7]);
    recvData.ReadByteSeq(petGuid[5]);
    recvData.ReadByteSeq(petGuid[0]);
    recvData.ReadByteSeq(petNumber[6]);
    recvData.ReadByteSeq(petGuid[4]);
    recvData.ReadByteSeq(petNumber[5]);
    recvData.ReadByteSeq(petGuid[2]);
    recvData.ReadByteSeq(petGuid[6]);
    recvData.ReadByteSeq(petNumber[3]);
    recvData.ReadByteSeq(petGuid[3]);
    recvData.ReadByteSeq(petNumber[4]);
    recvData.ReadByteSeq(petGuid[1]);
    recvData.ReadByteSeq(petGuid[7]);

    SendPetNameQuery(petGuid, petNumber);
}

void WorldSession::SendPetNameQuery(ObjectGuid petGuid, uint64 petNumber)
{
    Creature* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*_player, petGuid);
    if (!pet)
    {
        WorldPacket data(SMSG_PET_NAME_QUERY_RESPONSE, (8 + 1));
        data.WriteBit(0);
        data.FlushBits();
        data << uint64(petNumber);

        _player->GetSession()->SendPacket(&data);
        return;
    }

    WorldPacket data(SMSG_PET_NAME_QUERY_RESPONSE, (8 + 1 + 1 + 5 + pet->GetName().size() + 4));
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
            TC_LOG_DEBUG("network", "Player (GUID:%u) attempt open stable in cheating way.", guid.GetCounter());
            return false;
        }
    }
    // stable master case
    else
    {
        if (!GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_STABLEMASTER))
        {
            TC_LOG_DEBUG("network", "Stablemaster (GUID:%u) not found or you can't interact with him.", guid.GetCounter());
            return false;
        }
    }
    return true;
}

void WorldSession::HandlePetSetAction(WorldPacket& recvData)
{
    TC_LOG_INFO("network", "HandlePetSetAction. CMSG_PET_SET_ACTION");

    ObjectGuid petguid;
    uint32 position;
    uint32 data;
    bool move_command = false;

    recvData >> position >> data;
    petguid[1] = recvData.ReadBit();
    petguid[0] = recvData.ReadBit();
    petguid[5] = recvData.ReadBit();
    petguid[3] = recvData.ReadBit();
    petguid[2] = recvData.ReadBit();
    petguid[7] = recvData.ReadBit();
    petguid[6] = recvData.ReadBit();
    petguid[4] = recvData.ReadBit();
    recvData.ReadByteSeq(petguid[5]);
    recvData.ReadByteSeq(petguid[6]);
    recvData.ReadByteSeq(petguid[7]);
    recvData.ReadByteSeq(petguid[3]);
    recvData.ReadByteSeq(petguid[2]);
    recvData.ReadByteSeq(petguid[1]);
    recvData.ReadByteSeq(petguid[4]);
    recvData.ReadByteSeq(petguid[0]);

    Unit* pet = ObjectAccessor::GetUnit(*_player, petguid);

    if (!pet || pet != _player->GetFirstControlled())
    {
        TC_LOG_ERROR("network", "HandlePetSetAction: Unknown pet (GUID: %u) or pet owner (GUID: %u)", petguid.GetCounter(), _player->GetGUID().GetCounter());
        return;
    }

    CharmInfo* charmInfo = pet->GetCharmInfo();
    if (!charmInfo)
    {
        TC_LOG_ERROR("network", "WorldSession::HandlePetSetAction: object (GUID: %u TypeId: %u) is considered pet-like but doesn't have a charminfo!", pet->GetGUID().GetCounter(), pet->GetTypeId());
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

void WorldSession::HandlePetRename(WorldPacket& recvData)
{
    TC_LOG_INFO("network", "HandlePetRename. CMSG_PET_RENAME");

    std::string name;
    DeclinedName declinedname;
    uint32 petNumber;
    uint8 declineNameLenght[MAX_DECLINED_NAME_CASES] = { 0, 0, 0, 0, 0 };
    uint8 nameLenght = 0;

    recvData >> petNumber;

    bool hasName = !recvData.ReadBit();
    bool isdeclined = recvData.ReadBit();

    if (isdeclined)
        for (int i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            declineNameLenght[i] = recvData.ReadBits(7);

    if (hasName)
        nameLenght = recvData.ReadBits(8);

    if (hasName)
        name = recvData.ReadString(nameLenght);

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

    if (isdeclined)
    {
        for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
        {
            declinedname.name[i] = recvData.ReadString(declineNameLenght[i]);
        }

        if (!ObjectMgr::CheckDeclinedNames(name, declinedname))
        {
            SendPetNameInvalid(PET_NAME_DECLENSION_DOESNT_MATCH_BASE_NAME, name, &declinedname);
            return;
        }
    }

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    if (isdeclined)
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

void WorldSession::HandlePetAbandon(WorldPacket& recvData)
{
    ObjectGuid guid;

    guid[7] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[3]);

    TC_LOG_INFO("network", "HandlePetAbandon. CMSG_PET_ABANDON pet guid is %u", guid.GetCounter());

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
            _player->StopCastingCharm();
    }
}

void WorldSession::HandlePetSpellAutocastOpcode(WorldPacket& recvPacket)
{
    TC_LOG_INFO("network", "CMSG_PET_SPELL_AUTOCAST");
    ObjectGuid PetGUID;
    uint32 SpellID;

    recvPacket >> SpellID;

    PetGUID[0] = recvPacket.ReadBit();
    PetGUID[4] = recvPacket.ReadBit();
    PetGUID[2] = recvPacket.ReadBit();
    PetGUID[6] = recvPacket.ReadBit();
    PetGUID[1] = recvPacket.ReadBit();
    PetGUID[5] = recvPacket.ReadBit();
    PetGUID[3] = recvPacket.ReadBit();
    PetGUID[7] = recvPacket.ReadBit();

    bool State = recvPacket.ReadBit();

    recvPacket.ReadByteSeq(PetGUID[5]);
    recvPacket.ReadByteSeq(PetGUID[0]);
    recvPacket.ReadByteSeq(PetGUID[4]);
    recvPacket.ReadByteSeq(PetGUID[1]);
    recvPacket.ReadByteSeq(PetGUID[7]);
    recvPacket.ReadByteSeq(PetGUID[2]);
    recvPacket.ReadByteSeq(PetGUID[3]);
    recvPacket.ReadByteSeq(PetGUID[6]);

    if (!_player->GetGuardianPet() && !_player->GetCharm())
        return;

    if (ObjectAccessor::FindPlayer(PetGUID))
        return;

    Creature* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*_player, PetGUID);

    if (!pet || (pet != _player->GetGuardianPet() && pet != _player->GetCharm()))
    {
        TC_LOG_ERROR("network", "HandlePetSpellAutocastOpcode.Pet %u isn't pet of player %s (GUID: %u).", PetGUID.GetCounter(), GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter());
        return;
    }

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SpellID);
    // do not add not learned spells/ passive spells
    if (!pet->HasSpell(SpellID) || !spellInfo->IsAutocastable())
        return;

    CharmInfo* charmInfo = pet->GetCharmInfo();
    if (!charmInfo)
    {
        TC_LOG_ERROR("network", "WorldSession::HandlePetSpellAutocastOpcod: object (GUID: %u TypeId: %u) is considered pet-like but doesn't have a charminfo!", pet->GetGUID().GetCounter(), pet->GetTypeId());
        return;
    }

    if (pet->IsPet())
        ((Pet*)pet)->ToggleAutocast(spellInfo, State);
    else
        pet->GetCharmInfo()->ToggleCreatureAutocast(spellInfo, State);

    charmInfo->SetSpellAutocast(spellInfo, State);
}

void WorldSession::HandlePetCastSpellOpcode(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_PET_CAST_SPELL");

    uint8 castCount;
    uint32 spellId;
    uint8 castFlags;

    ObjectGuid targetGUID;
    ObjectGuid transportDstGUID;
    ObjectGuid itemGUID;
    ObjectGuid transportSrcGUID;
    ObjectGuid petGuid;
    ObjectGuid transportGUID;
    ObjectGuid guid20;

    bool bit388 = false;
    bool bit389 = false;
    bool bit412 = false;
    bool hasUnkMovementField = false;
    bool hasTransport = false;
    bool hasTransportTime2 = false;
    bool hasTransportTime3 = false;
    bool hasMovementFlags = false;
    bool hasMovementFlags2 = false;
    bool hasFallDirection = false;
    bool hasFallData = false;
    bool hasPitch = false;
    bool hasOrientation = false;
    bool hasSplineElevation = false;
    bool hasTimestamp = false;

    uint32 targetStrLenght = 0;
    uint32 movementForcesCount = 0;
    uint32 archeologyCount = 0;
    uint32 targetFlags = 0;

    WorldLocation dstLoc, srcLoc;

    float missileSpeed = 0.0f;
    float elevation = 0.0f;

    bool hasDestLocation = recvPacket.ReadBit();
    petGuid[7] = recvPacket.ReadBit();
    bool hasMissileSpeed = !recvPacket.ReadBit();
    bool hasSrcLocation = recvPacket.ReadBit();
    petGuid[1] = recvPacket.ReadBit();
    archeologyCount = recvPacket.ReadBits(2);
    bool hasTargetMask = !recvPacket.ReadBit();
    petGuid[4] = recvPacket.ReadBit();
    recvPacket.ReadBit();
    petGuid[6] = recvPacket.ReadBit();
    bool hasTargetString = !recvPacket.ReadBit();
    recvPacket.ReadBit();
    bool hasMovement = recvPacket.ReadBit();
    bool hasCastFlags = !recvPacket.ReadBit();
    bool hasSpellId = !recvPacket.ReadBit();
    petGuid[0] = recvPacket.ReadBit();
    petGuid[5] = recvPacket.ReadBit();
    petGuid[2] = recvPacket.ReadBit();
    for (uint32 i = 0; i < archeologyCount; ++i)
        recvPacket.ReadBits(2);                            // archeologyType
    petGuid[3] = recvPacket.ReadBit();
    bool hasGlyphIndex = !recvPacket.ReadBit();
    bool hasCastCount = !recvPacket.ReadBit();
    bool hasElevation = !recvPacket.ReadBit();

    if (hasMovement)
    {
        hasOrientation = !recvPacket.ReadBit();
        hasSplineElevation = !recvPacket.ReadBit();
        bit388 = recvPacket.ReadBit();
        guid20[5] = recvPacket.ReadBit();
        guid20[7] = recvPacket.ReadBit();
        hasMovementFlags2 = !recvPacket.ReadBit();
        hasTimestamp = !recvPacket.ReadBit();
        hasFallData = recvPacket.ReadBit();
        hasMovementFlags = !recvPacket.ReadBit();
        hasUnkMovementField = !recvPacket.ReadBit();

        if (hasMovementFlags)
            recvPacket.ReadBits(30);                       // Movement Flags

        bit389 = recvPacket.ReadBit();
        guid20[6] = recvPacket.ReadBit();
        hasTransport = recvPacket.ReadBit();
        guid20[0] = recvPacket.ReadBit();
        movementForcesCount = recvPacket.ReadBits(22);

        if (hasTransport)
        {
            hasTransportTime2 = recvPacket.ReadBit();
            hasTransportTime3 = recvPacket.ReadBit();
            transportGUID[5] = recvPacket.ReadBit();
            transportGUID[6] = recvPacket.ReadBit();
            transportGUID[4] = recvPacket.ReadBit();
            transportGUID[0] = recvPacket.ReadBit();
            transportGUID[1] = recvPacket.ReadBit();
            transportGUID[2] = recvPacket.ReadBit();
            transportGUID[7] = recvPacket.ReadBit();
            transportGUID[3] = recvPacket.ReadBit();
        }

        guid20[1] = recvPacket.ReadBit();

        if (hasMovementFlags2)
            recvPacket.ReadBits(13);                       // Movement Flags 2

        guid20[3] = recvPacket.ReadBit();
        guid20[2] = recvPacket.ReadBit();
        bit412 = recvPacket.ReadBit();
        hasPitch = !recvPacket.ReadBit();
        guid20[4] = recvPacket.ReadBit();

        if (hasFallData)
            hasFallDirection = recvPacket.ReadBit();
    }

    if (hasDestLocation)
    {
        transportDstGUID[2] = recvPacket.ReadBit();
        transportDstGUID[0] = recvPacket.ReadBit();
        transportDstGUID[1] = recvPacket.ReadBit();
        transportDstGUID[4] = recvPacket.ReadBit();
        transportDstGUID[5] = recvPacket.ReadBit();
        transportDstGUID[6] = recvPacket.ReadBit();
        transportDstGUID[3] = recvPacket.ReadBit();
        transportDstGUID[7] = recvPacket.ReadBit();
    }

    if (hasCastFlags)
        castFlags = recvPacket.ReadBits(5);                // Cast Flags

    itemGUID[2] = recvPacket.ReadBit();
    itemGUID[4] = recvPacket.ReadBit();
    itemGUID[7] = recvPacket.ReadBit();
    itemGUID[0] = recvPacket.ReadBit();
    itemGUID[6] = recvPacket.ReadBit();
    itemGUID[1] = recvPacket.ReadBit();
    itemGUID[5] = recvPacket.ReadBit();
    itemGUID[3] = recvPacket.ReadBit();

    if (hasTargetMask)
        targetFlags = recvPacket.ReadBits(20);             // Target Flags

    if (hasTargetString)
        targetStrLenght = recvPacket.ReadBits(7);          // Target String

    if (hasSrcLocation)
    {
        transportSrcGUID[2] = recvPacket.ReadBit();
        transportSrcGUID[0] = recvPacket.ReadBit();
        transportSrcGUID[3] = recvPacket.ReadBit();
        transportSrcGUID[1] = recvPacket.ReadBit();
        transportSrcGUID[6] = recvPacket.ReadBit();
        transportSrcGUID[7] = recvPacket.ReadBit();
        transportSrcGUID[4] = recvPacket.ReadBit();
        transportSrcGUID[5] = recvPacket.ReadBit();
    }

    targetGUID[6] = recvPacket.ReadBit();
    targetGUID[0] = recvPacket.ReadBit();
    targetGUID[3] = recvPacket.ReadBit();
    targetGUID[4] = recvPacket.ReadBit();
    targetGUID[2] = recvPacket.ReadBit();
    targetGUID[1] = recvPacket.ReadBit();
    targetGUID[5] = recvPacket.ReadBit();
    targetGUID[7] = recvPacket.ReadBit();

    recvPacket.ReadByteSeq(petGuid[2]);
    recvPacket.ReadByteSeq(petGuid[6]);
    recvPacket.ReadByteSeq(petGuid[3]);

    for (uint32 i = 0; i < archeologyCount; ++i)
    {
        recvPacket.read_skip<uint32>();                    // unk1
        recvPacket.read_skip<uint32>();                    // unk2
    }
    recvPacket.ReadByteSeq(petGuid[1]);
    recvPacket.ReadByteSeq(petGuid[7]);
    recvPacket.ReadByteSeq(petGuid[0]);
    recvPacket.ReadByteSeq(petGuid[4]);
    recvPacket.ReadByteSeq(petGuid[5]);

    if (hasDestLocation)
    {
        recvPacket.ReadByteSeq(transportDstGUID[4]);
        recvPacket.ReadByteSeq(transportDstGUID[1]);
        recvPacket.ReadByteSeq(transportDstGUID[7]);
        dstLoc.m_positionZ = recvPacket.read<float>();     // Position Z
        dstLoc.m_positionY = recvPacket.read<float>();     // Position Y
        recvPacket.ReadByteSeq(transportDstGUID[6]);
        recvPacket.ReadByteSeq(transportDstGUID[3]);
        dstLoc.m_positionX = recvPacket.read<float>();     // Position X
        recvPacket.ReadByteSeq(transportDstGUID[2]);
        recvPacket.ReadByteSeq(transportDstGUID[5]);
        recvPacket.ReadByteSeq(transportDstGUID[0]);
    }

    if (hasMovement)
    {
        if (hasPitch)
            recvPacket.read_skip<float>();                 // Pitch

        if (hasTransport)
        {
            if (hasTransportTime3)
                recvPacket.read_skip<int32>();             // Transport Time 3

            if (hasTransportTime2)
                recvPacket.read_skip<int32>();             // Transport Time 2

            recvPacket.read_skip<uint8>();                 // Transport Seat
            recvPacket.read_skip<float>();                 // Orientation
            recvPacket.read_skip<float>();                 // Position Z
            recvPacket.ReadByteSeq(transportGUID[2]);
            recvPacket.read_skip<int32>();                 // Transport Time
            recvPacket.ReadByteSeq(transportGUID[3]);
            recvPacket.read_skip<float>();                 // Position X
            recvPacket.ReadByteSeq(transportGUID[6]);
            recvPacket.ReadByteSeq(transportGUID[5]);
            recvPacket.ReadByteSeq(transportGUID[7]);
            recvPacket.ReadByteSeq(transportGUID[0]);
            recvPacket.read_skip<float>();                 // Position Y
            recvPacket.ReadByteSeq(transportGUID[4]);
            recvPacket.ReadByteSeq(transportGUID[1]);
        }

        if (hasUnkMovementField)
            recvPacket.read_skip<int32>();

        for (uint32 i = 0; i < movementForcesCount; ++i)
            recvPacket.read_skip<uint32>();                // Movement Forces

        recvPacket.ReadByteSeq(guid20[3]);

        if (hasOrientation)
            recvPacket.read_skip<float>();                 // Orientation

        recvPacket.ReadByteSeq(guid20[5]);

        if (hasFallData)
        {
            recvPacket.read_skip<float>();                 // Z Speed

            if (hasFallDirection)
            {
                recvPacket.read_skip<float>();             // CosAngle
                recvPacket.read_skip<float>();             // XY Speed
                recvPacket.read_skip<float>();             // SinAngle
            }
            recvPacket.read_skip<int32>();                 // FallTime
        }

        if (hasTimestamp)
            recvPacket.read_skip<int32>();                 // Timestamp

        recvPacket.ReadByteSeq(guid20[6]);
        recvPacket.read_skip<float>();                     // Position X
        recvPacket.ReadByteSeq(guid20[1]);
        recvPacket.read_skip<float>();                     // Position Z
        recvPacket.ReadByteSeq(guid20[2]);
        recvPacket.ReadByteSeq(guid20[7]);
        recvPacket.ReadByteSeq(guid20[0]);
        recvPacket.read_skip<float>();                     // Position Y
        recvPacket.ReadByteSeq(guid20[4]);

        if (hasSplineElevation)
            recvPacket.read_skip<float>();                 // Spline Elevation
    }

    if (hasSrcLocation)
    {
        recvPacket.ReadByteSeq(transportSrcGUID[3]);
        recvPacket.ReadByteSeq(transportSrcGUID[4]);
        recvPacket.ReadByteSeq(transportSrcGUID[2]);
        recvPacket.ReadByteSeq(transportSrcGUID[1]);
        recvPacket.ReadByteSeq(transportSrcGUID[0]);
        recvPacket.ReadByteSeq(transportSrcGUID[7]);
        srcLoc.m_positionZ = recvPacket.read<float>();     // Position Z
        recvPacket.ReadByteSeq(transportSrcGUID[6]);
        recvPacket.ReadByteSeq(transportSrcGUID[5]);
        srcLoc.m_positionX = recvPacket.read<float>();     // Position X
        srcLoc.m_positionY = recvPacket.read<float>();     // Position Y
    }

    if (hasMissileSpeed)
        missileSpeed = recvPacket.read<float>();           // missileSpeed

    recvPacket.ReadByteSeq(itemGUID[1]);
    recvPacket.ReadByteSeq(itemGUID[2]);
    recvPacket.ReadByteSeq(itemGUID[5]);
    recvPacket.ReadByteSeq(itemGUID[7]);
    recvPacket.ReadByteSeq(itemGUID[4]);
    recvPacket.ReadByteSeq(itemGUID[6]);
    recvPacket.ReadByteSeq(itemGUID[3]);
    recvPacket.ReadByteSeq(itemGUID[0]);

    recvPacket.ReadByteSeq(targetGUID[1]);
    recvPacket.ReadByteSeq(targetGUID[5]);
    recvPacket.ReadByteSeq(targetGUID[7]);
    recvPacket.ReadByteSeq(targetGUID[3]);
    recvPacket.ReadByteSeq(targetGUID[0]);
    recvPacket.ReadByteSeq(targetGUID[2]);
    recvPacket.ReadByteSeq(targetGUID[4]);
    recvPacket.ReadByteSeq(targetGUID[6]);

    if (hasElevation)
        elevation = recvPacket.read<float>();              // Elevation

    if (hasCastCount)
        castCount = recvPacket.read<uint8>();              // Cast Count

    if (hasTargetString)
        recvPacket.ReadString(targetStrLenght);

    if (hasGlyphIndex)
        recvPacket.read_skip<int32>();                     // Glyph Index

    if (hasSpellId)
        spellId = recvPacket.read<uint32>();

    TC_LOG_DEBUG("network", "WORLD: CMSG_PET_CAST_SPELL, guid: " UI64FMTD ", castCount: %u, spellId %u, castFlags %u", uint64(petGuid), castCount, spellId, castFlags);

    // This opcode is also sent from charmed and possessed units (players and creatures)
    if (!_player->GetGuardianPet() && !_player->GetCharm())
        return;

    Unit* caster = ObjectAccessor::GetUnit(*_player, petGuid);

    if (!caster || (caster != _player->GetGuardianPet() && caster != _player->GetCharm()))
    {
        TC_LOG_ERROR("network", "HandlePetCastSpellOpcode: Pet %u isn't pet of player %s (GUID: %u).", petGuid.GetCounter(), GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter());
        return;
    }

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
    {
        TC_LOG_ERROR("network", "WORLD: unknown PET spell id %i", spellId);
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

void WorldSession::HandleLearnPetSpecialization(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_LEARN_PET_SPECIALIZATION_GROUP");

    ObjectGuid guid;
    uint32 index = recvData.read<uint32>();
    guid[5] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[1]);

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
