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

#ifndef SF_GROUPMGR_H
#define SF_GROUPMGR_H

#include "Group.h"

class GroupMgr
{
private:
    GroupMgr();
    ~GroupMgr();

public:
    static GroupMgr* instance()
    {
        static GroupMgr _instance;
        return &_instance;
    }

    typedef std::map<uint32, Group*> GroupContainer;
    typedef std::vector<Group*>      GroupDbContainer;
    typedef std::multimap<uint64, Group*> PlayerGroups;

    Group* GetGroupByGUID(ObjectGuid::LowType guid) const;

    uint32 GenerateNewGroupDbStoreId();
    void   RegisterGroupDbStoreId(uint32 storageId, Group* group);
    void   FreeGroupDbStoreId(Group* group);
    void   SetNextGroupDbStoreId(uint32 storageId) { NextGroupDbStoreId = storageId; };
    Group* GetGroupByDbStoreId(uint32 storageId) const;
    void   SetGroupDbStoreSize(uint32 newSize) { GroupDbStore.resize(newSize); }

    void   LoadGroups();
    ObjectGuid::LowType GenerateGroupId();
    void   AddGroup(Group* group);
    void   RemoveGroup(Group* group);

    void BindGroupToPlayer(uint64 playerGuid, Group* group);
    void UnbindGroupFromPlayer(uint64 playerGuid, Group* group);

    void LoadGroups(Player* player);

protected:
    uint32           NextGroupId;
    uint32           NextGroupDbStoreId;
    GroupContainer   GroupStore;
    GroupDbContainer GroupDbStore;
    PlayerGroups     GroupByPlayerStore;
};

#define sGroupMgr GroupMgr::instance()

#endif
