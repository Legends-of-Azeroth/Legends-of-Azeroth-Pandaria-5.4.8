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

#include "SceneMgr.h"
#include "Chat.h"
#include "Language.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"

SceneMgr::SceneMgr(Player* player) : _player(player)
{
    _standaloneSceneInstanceID = 0;
    _isDebuggingScenes = false;
}

uint32 SceneMgr::PlayScene(uint32 sceneId, Position const* position /*= nullptr*/)
{
    SceneTemplate const* sceneTemplate = sObjectMgr->GetSceneTemplate(sceneId);
    return PlaySceneByTemplate(sceneTemplate, position);
}

uint32 SceneMgr::PlaySceneByTemplate(SceneTemplate const* sceneTemplate, Position const* position /*= nullptr*/)
{
    if (!sceneTemplate)
        return 0;

    SceneScriptPackageEntry const* entry = sSceneScriptPackageStore.LookupEntry(sceneTemplate->ScenePackageId);
    if (!entry)
        return 0;

    // By default, take player position
    if (!position)
        position = GetPlayer();

    uint32 sceneInstanceID = GetNewStandaloneSceneInstanceID();

    if (_isDebuggingScenes)
        ChatHandler(GetPlayer()->GetSession()).PSendSysMessage(LANG_COMMAND_SCENE_DEBUG_PLAY, sceneInstanceID, sceneTemplate->ScenePackageId, sceneTemplate->PlaybackFlags);

    ObjectGuid guid = GetPlayer()->GetTransGUID();
    WorldPacket data(SMSG_PLAY_SCENE, 34);
    data << position->GetPositionY();
    data << position->GetPositionZ();
    data << position->GetPositionX();
    data.WriteBit(sceneTemplate->PlaybackFlags == 0);
    data.WriteBit(sceneInstanceID == 0);
    data.WriteBit(position->GetOrientation() == 0);
    data.WriteBit(sceneTemplate->SceneId == 0);
    data.WriteBit(sceneTemplate->ScenePackageId == 0);
    data.WriteBit(true); // unk

    data.WriteBit(guid[3]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[0]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[7]);

    data.FlushBits();

    if (sceneTemplate->PlaybackFlags)
        data << sceneTemplate->PlaybackFlags;
    if (position->GetOrientation() > 0)
        data << position->GetOrientation();
    if (sceneTemplate->SceneId)
        data << sceneTemplate->SceneId;
    if (sceneInstanceID)
        data << sceneInstanceID;
    if (sceneTemplate->ScenePackageId)
        data << sceneTemplate->ScenePackageId;

    GetPlayer()->SendDirectMessage(&data);

    AddInstanceIdToSceneMap(sceneInstanceID, sceneTemplate);

    sScriptMgr->OnSceneStart(GetPlayer(), sceneInstanceID, sceneTemplate);

    return sceneInstanceID;
}

uint32 SceneMgr::PlaySceneByPackageId(uint32 sceneScriptPackageId, uint32 playbackflags /*= SCENEFLAG_UNK16*/, Position const* position /*= nullptr*/)
{
    SceneTemplate sceneTemplate;
    sceneTemplate.SceneId = 0;
    sceneTemplate.ScenePackageId = sceneScriptPackageId;
    sceneTemplate.PlaybackFlags = playbackflags;
    sceneTemplate.ScriptId = 0;

    return PlaySceneByTemplate(&sceneTemplate, position);
}

void SceneMgr::CancelScene(uint32 sceneInstanceID, bool removeFromMap /*= true*/)
{
    if (removeFromMap)
        RemoveSceneInstanceId(sceneInstanceID);

    WorldPacket data(SMSG_CANCEL_SCENE, 4);
    data.WriteBit(sceneInstanceID == 0);
    if (sceneInstanceID)
        data << sceneInstanceID;

    GetPlayer()->SendDirectMessage(&data);
}

void SceneMgr::OnSceneTrigger(uint32 sceneInstanceID, std::string const& triggerName)
{
    if (!HasScene(sceneInstanceID))
        return;

    if (_isDebuggingScenes)
        ChatHandler(GetPlayer()->GetSession()).PSendSysMessage(LANG_COMMAND_SCENE_DEBUG_TRIGGER, sceneInstanceID, triggerName.c_str());

    SceneTemplate const* sceneTemplate = GetSceneTemplateFromInstanceId(sceneInstanceID);
    sScriptMgr->OnSceneTrigger(GetPlayer(), sceneInstanceID, sceneTemplate, triggerName);
}

void SceneMgr::OnSceneCancel(uint32 sceneInstanceID)
{
    if (!HasScene(sceneInstanceID))
        return;

    if (_isDebuggingScenes)
        ChatHandler(GetPlayer()->GetSession()).PSendSysMessage(LANG_COMMAND_SCENE_DEBUG_CANCEL, sceneInstanceID);

    SceneTemplate const* sceneTemplate = GetSceneTemplateFromInstanceId(sceneInstanceID);

    // Must be done before removing aura
    RemoveSceneInstanceId(sceneInstanceID);

    if (sceneTemplate->SceneId != 0)
        RemoveAurasDueToSceneId(sceneTemplate->SceneId);

    sScriptMgr->OnSceneCancel(GetPlayer(), sceneInstanceID, sceneTemplate);

    if (sceneTemplate->PlaybackFlags & SCENEFLAG_CANCEL_AT_END)
        CancelScene(sceneInstanceID, false);
}

void SceneMgr::OnSceneComplete(uint32 sceneInstanceID)
{
    if (!HasScene(sceneInstanceID))
        return;

    if (_isDebuggingScenes)
        ChatHandler(GetPlayer()->GetSession()).PSendSysMessage(LANG_COMMAND_SCENE_DEBUG_COMPLETE, sceneInstanceID);

    SceneTemplate const* sceneTemplate = GetSceneTemplateFromInstanceId(sceneInstanceID);

    // Must be done before removing aura
    RemoveSceneInstanceId(sceneInstanceID);

    if (sceneTemplate->SceneId != 0)
        RemoveAurasDueToSceneId(sceneTemplate->SceneId);

    sScriptMgr->OnSceneComplete(GetPlayer(), sceneInstanceID, sceneTemplate);

    if (sceneTemplate->PlaybackFlags & SCENEFLAG_CANCEL_AT_END)
        CancelScene(sceneInstanceID, false);
}

bool SceneMgr::HasScene(uint32 sceneInstanceID, uint32 sceneScriptPackageId /*= 0*/) const
{
    auto itr = _scenesByInstance.find(sceneInstanceID);

    if (itr != _scenesByInstance.end())
        return !sceneScriptPackageId || sceneScriptPackageId == itr->second->ScenePackageId;

    return false;
}

void SceneMgr::AddInstanceIdToSceneMap(uint32 sceneInstanceID, SceneTemplate const* sceneTemplate)
{
    _scenesByInstance[sceneInstanceID] = sceneTemplate;
}

void SceneMgr::CancelSceneByPackageId(uint32 sceneScriptPackageId)
{
    std::vector<uint32> instancesIds;

    for (auto itr : _scenesByInstance)
        if (itr.second->ScenePackageId == sceneScriptPackageId)
            instancesIds.push_back(itr.first);

    for (uint32 sceneInstanceID : instancesIds)
        CancelScene(sceneInstanceID);
}

void SceneMgr::RemoveSceneInstanceId(uint32 sceneInstanceID)
{
    _scenesByInstance.erase(sceneInstanceID);
}

void SceneMgr::RemoveAurasDueToSceneId(uint32 sceneId)
{
    Player::AuraEffectList const& scenePlayAuras = GetPlayer()->GetAuraEffectsByType(SPELL_AURA_PLAY_SCENE);
    for (AuraEffect* scenePlayAura : scenePlayAuras)
    {
        if (uint32(scenePlayAura->GetMiscValue()) == sceneId)
        {
            GetPlayer()->RemoveAura(scenePlayAura->GetBase());
            break;
        }
    }
}

SceneTemplate const* SceneMgr::GetSceneTemplateFromInstanceId(uint32 sceneInstanceID)
{
    auto itr = _scenesByInstance.find(sceneInstanceID);

    if (itr != _scenesByInstance.end())
        return itr->second;

    return nullptr;
}

uint32 SceneMgr::GetActiveSceneCount(uint32 sceneScriptPackageId /*= 0*/)
{
    uint32 activeSceneCount = 0;

    for (auto itr : _scenesByInstance)
        if (!sceneScriptPackageId || itr.second->ScenePackageId == sceneScriptPackageId)
            ++activeSceneCount;

    return activeSceneCount;
}
