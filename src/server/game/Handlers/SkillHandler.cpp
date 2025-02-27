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

#include "Log.h"
#include "ObjectAccessor.h"
#include "Opcodes.h"
#include "Player.h"
#include "Pet.h"
#include "UpdateMask.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "InstanceScript.h"

void WorldSession::HandeSetTalentSpecialization(WorldPacket& recvData)
{
    uint32 specializationTabId;
    recvData >> specializationTabId;

    if (specializationTabId > MAX_TALENT_TABS)
        return;

    if (_player->GetTalentSpecialization(_player->GetActiveSpec()))
        return;

    uint32 specializationId = dbc::GetClassSpecializations(_player->GetClass())[specializationTabId];
    uint32 specializationSpell = 0;

    _player->SetTalentSpecialization(_player->GetActiveSpec(), Specializations(specializationId));
    _player->SetUInt32Value(PLAYER_FIELD_CURRENT_SPEC_ID, specializationId);
    _player->SendTalentsInfoData();

    _player->LearnSpecializationSpells();
    _player->UpdatePvpPower();

    _player->SaveToDB();
}

void WorldSession::HandleLearnTalentOpcode(WorldPacket& recvData)
{
    if (_player->GetMap()->IsDungeon() && _player->GetMap()->IsChallengeDungeon() &&
        _player->GetInstanceScript() && _player->GetInstanceScript()->IsChallengeModeStarted())
    {
        _player->SendGameError(GameError::ERR_NOT_IN_COMBAT);
        recvData.rfinish(); // prevent spam at ignore packet
        return;
    }

    uint32 talentCount = recvData.ReadBits(23);
    uint16 talentId;
    bool anythingLearned = false;

    for (int i = 0; i != talentCount; i++)
    {
        recvData >> talentId;
        if (_player->LearnTalent(talentId))
            anythingLearned = true;
    }

    if (anythingLearned)
        _player->SendTalentsInfoData();
}

void WorldSession::HandleLearnPreviewTalents(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network", "CMSG_LEARN_PREVIEW_TALENTS");
}

void WorldSession::HandleRespecWipeConfirmOpcode(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network", "CMSG_CONFIRM_RESPEC_WIPE");
    ObjectGuid guid;
    uint8 respecType = 0;
    uint32 Cost = 0;

    recvPacket >> respecType;
    guid[7] = recvPacket.ReadBit();
    guid[2] = recvPacket.ReadBit();
    guid[6] = recvPacket.ReadBit();
    guid[1] = recvPacket.ReadBit();
    guid[4] = recvPacket.ReadBit();
    guid[0] = recvPacket.ReadBit();
    guid[3] = recvPacket.ReadBit();
    guid[5] = recvPacket.ReadBit();

    recvPacket.ReadByteSeq(guid[2]);
    recvPacket.ReadByteSeq(guid[4]);
    recvPacket.ReadByteSeq(guid[1]);
    recvPacket.ReadByteSeq(guid[3]);
    recvPacket.ReadByteSeq(guid[0]);
    recvPacket.ReadByteSeq(guid[5]);
    recvPacket.ReadByteSeq(guid[7]);
    recvPacket.ReadByteSeq(guid[6]);
    
    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    if (!respecType)
    {
        if (!_player->ResetTalents(Cost, true, false))
        {
            GetPlayer()->SendTalentWipeConfirm(guid, false);
            return;
        }
    }
    else
    {
        _player->ResetTalents(Cost, false, true);
    }

    _player->SendTalentsInfoData();

    if (Unit* unit = _player->GetSelectedUnit())
        unit->CastSpell(_player, 14867, true);                  //spell: "Untalent Visual Effect"
}

void WorldSession::HandleUnlearnSkillOpcode(WorldPacket& recvData)
{
    uint32 skillId;
    recvData >> skillId;

    if (!IsProfessionSkill(skillId))
        return;

    GetPlayer()->SetSkill(skillId, 0, 0, 0);
}

#define PROFESSION_SPEC_INDEX_COUNT 5
const uint32 specSpells[PROFESSION_SPEC_INDEX_COUNT] = { 20219, 20222, 28677, 28675, 28672 };
const uint32 specRecipes[PROFESSION_SPEC_INDEX_COUNT][23] =
{
    { 12759, 12895, 12897, 12899, 12902, 12903, 12905, 12906, 12907, 20219, 23096, 23129, 23489, 30568, 30569, 30570, 30574, 30575, 36955, 56473, 95705, 0 },
    { 8895, 12715, 12716, 12717, 12718, 12754, 12755, 12758, 12760, 12908, 13240, 20222, 23078, 23486, 30558, 30560, 30563, 30565, 30566, 36954, 56514, 95707, 0 },
    { 0 },
    { 0 },
    { 0 },
};
const uint32 specQuest[PROFESSION_SPEC_INDEX_COUNT] = { 29477, 29475, 29481, 29067, 29482 };
void WorldSession::HandleUnlearnSpecializationOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_UNLEARN_SPECIALIZATION");

    uint8 specIndex = recvData.read<uint8>();

    if (specIndex >= PROFESSION_SPEC_INDEX_COUNT)
        return;

    if (GetPlayer()->HasSpell(specSpells[specIndex]))
    {
        GetPlayer()->RemoveSpell(specSpells[specIndex]);
        GetPlayer()->RemoveRewardedQuest(specQuest[specIndex]);

        std::vector<uint32> recipes(&specRecipes[specIndex][0], &specRecipes[specIndex][0] + sizeof(specRecipes[specIndex]) / sizeof(uint32));
        for (auto&& spell : recipes)
        {
            if (!spell)
                break;

            if (GetPlayer()->HasSpell(spell))
                GetPlayer()->RemoveSpell(spell);
        }

        GetPlayer()->SaveToDB();
    }
}

void WorldSession::HandleArcheologyRequestHistory(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network", "CMSG_REQUEST_RESEARCH_HISTORY");

    GetPlayer()->SendResearchHistory();
}
