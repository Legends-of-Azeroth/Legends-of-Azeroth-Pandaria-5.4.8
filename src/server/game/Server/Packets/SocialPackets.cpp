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

#include "SocialPackets.h"

void WorldPackets::Social::SendContactList::Read()
{
    _worldPacket >> Unknown;   // always 1
}

void WorldPackets::Social::AddFriend::Read()
{
    _worldPacket >> FriendName;
    _worldPacket >> FriendNote;
}

void WorldPackets::Social::DelFriend::Read()
{
    _worldPacket >> FriendGuid;
}

void WorldPackets::Social::AddIgnore::Read()
{
    _worldPacket >> IgnoreName;
}

void WorldPackets::Social::DelIgnore::Read()
{
    _worldPacket >> IgnoreGuid;
}

void WorldPackets::Social::SetContactNotes::Read()
{
    _worldPacket >> Guid;
    _worldPacket >> Note;
}
