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

#include "zlib.h"
#include "Common.h"
#include "Language.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Player.h"
#include "TicketMgr.h"
#include "Util.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"

static const char InvalidCharacters[] = ":-\\";

void WorldSession::HandleGMTicketCreateOpcode(WorldPackets::Ticket::GMTicketCreate& packet)
{
    // Don't accept tickets if the ticket queue is disabled. (Ticket UI is greyed out but not fully dependable)
    if (sTicketMgr->GetGmTicketSystemStatus() == GMTICKET_QUEUE_STATUS_DISABLED)
        return;

    if (GetPlayer()->GetLevel() < sWorld->getIntConfig(CONFIG_TICKET_LEVEL_REQ))
    {
        SendNotification(GetTrinityString(LANG_TICKET_REQ), sWorld->getIntConfig(CONFIG_TICKET_LEVEL_REQ));
        return;
    }

    GMTicketResponse response = GMTICKET_RESPONSE_CREATE_ERROR;
    GmTicket* ticket = sTicketMgr->GetGmTicketByPlayerGuid(GetPlayer()->GetGUID());

    if (ticket && ticket->IsCompleted())
        sTicketMgr->CloseTicket<GmTicket>(ticket->GetTicketId(), GetPlayer()->GetGUID());;

    // Player must not have ticket
    if (!ticket || ticket->IsClosed())
    {
        ticket = new GmTicket(GetPlayer());

        if (!packet.DecompressOk)
        {
            delete ticket;
            return;
        }

        G3D::Vector3 Position;
        Position.x = packet.X;
        Position.y = packet.Y;
        Position.z = packet.Z;

        ticket->SetChatLog(packet.Times, packet.ChatLog);
        ticket->SetMessage(packet.Message);
        ticket->SetPosition(packet.MapId, Position);
        ticket->SetGmAction(packet.NeedResponse, packet.HaveTicket);

        sTicketMgr->AddTicket(ticket);
        sTicketMgr->UpdateLastChange();

        sWorld->SendGMText(LANG_COMMAND_TICKETNEW, GetPlayer()->GetName().c_str(), ticket->GetTicketId());

        response = GMTICKET_RESPONSE_CREATE_SUCCESS;
    }

    sTicketMgr->SendGmTicketUpdate(SMSG_GM_TICKET_UPDATE, response, GetPlayer());
}

void WorldSession::HandleGMTicketUpdateOpcode(WorldPackets::Ticket::GMTicketUpdateText& packet)
{
    std::string message = packet.Message;

    if (!message.empty())
        for (uint8 X = 0; X < strlen(InvalidCharacters); ++X)
            message.erase(std::remove(message.begin(), message.end(), InvalidCharacters[X]), message.end());

    GMTicketResponse response = GMTICKET_RESPONSE_UPDATE_ERROR;
    if (GmTicket* ticket = sTicketMgr->GetGmTicketByPlayerGuid(GetPlayer()->GetGUID()))
    {
        CharacterDatabaseTransaction trans = CharacterDatabaseTransaction(nullptr);
        ticket->SetMessage(message);
        ticket->SaveToDB(trans);

        sWorld->SendGMText(LANG_COMMAND_TICKETUPDATED, GetPlayer()->GetName().c_str(), ticket->GetTicketId());

        response = GMTICKET_RESPONSE_UPDATE_SUCCESS;
    }

    // sTicketMgr->SendGmTicketUpdate(SMSG_GM_TICKET_UPDATE_TEXT, response, GetPlayer()); opcode not vaild
}

void WorldSession::HandleGMTicketDeleteOpcode(WorldPackets::Ticket::GMTicketDeleteTicket& /*packet*/)
{
    if (GmTicket* ticket = sTicketMgr->GetGmTicketByPlayerGuid(GetPlayer()->GetGUID()))
    {
        sTicketMgr->SendGmTicketUpdate(SMSG_GM_TICKET_UPDATE, GMTICKET_RESPONSE_TICKET_DELETED, GetPlayer());

        sWorld->SendGMText(LANG_COMMAND_TICKETPLAYERABANDON, GetPlayer()->GetName().c_str(), ticket->GetTicketId());

        sTicketMgr->CloseTicket<GmTicket>(ticket->GetTicketId(), GetPlayer()->GetGUID());
        sTicketMgr->SendGmTicket(this, NULL);
    }
}

void WorldSession::HandleGMTicketGetTicketOpcode(WorldPackets::Ticket::GMTicketGetTicket& /*packet*/)
{
    SendQueryTimeResponse();

    if (GmTicket* ticket = sTicketMgr->GetGmTicketByPlayerGuid(GetPlayer()->GetGUID()))
    {
        if (ticket->IsCompleted())
            sTicketMgr->SendGmResponsee(this, ticket);
        else
            sTicketMgr->SendGmTicket(this, ticket);
    }
    else
        sTicketMgr->SendGmTicket(this, NULL);
}

void WorldSession::HandleGMTicketSystemStatusOpcode(WorldPackets::Ticket::GMTicketGetSystemStatus& /*packet*/)
{
    // Note: This only disables the ticket UI at client side and is not fully reliable
    // are we sure this is a uint32? Should ask Zor
    WorldPacket data(SMSG_GM_TICKET_SYSTEM_STATUS, 4);
    data << uint32(sTicketMgr->GetGmTicketSystemStatus() ? GMTICKET_QUEUE_STATUS_ENABLED : GMTICKET_QUEUE_STATUS_DISABLED);
    SendPacket(&data);
}

void WorldSession::HandleGMTicketCaseStatusOpcode(WorldPackets::Ticket::GMTicketGetCaseStatus& /*packet*/)
{
    time_t UpdateTime = 0, OldestTicketTime = time(NULL);
    WorldPacket data(SMSG_GM_TICKET_CASE_STATUS, 4+4+3);
    data.WriteBits(0, 20);
    data.FlushBits();
    data.AppendPackedTime(UpdateTime);
    data.AppendPackedTime(OldestTicketTime);
    SendPacket(&data);
}

void WorldSession::HandleGMSurveySubmit(WorldPacket& recvData)
{
    /*
    uint32 nextSurveyID = sTicketMgr->GetNextSurveyID();
    // just put the survey into the database
    uint32 mainSurvey; // GMSurveyCurrentSurvey.dbc, column 1 (all 9) ref to GMSurveySurveys.dbc
    recvData >> mainSurvey;

    // sub_survey1, r1, comment1, sub_survey2, r2, comment2, sub_survey3, r3, comment3, sub_survey4, r4, comment4, sub_survey5, r5, comment5, sub_survey6, r6, comment6, sub_survey7, r7, comment7, sub_survey8, r8, comment8, sub_survey9, r9, comment9, sub_survey10, r10, comment10,
    for (uint8 i = 0; i < 15; i++)
    {
        uint32 subSurveyId; // ref to i'th GMSurveySurveys.dbc field (all fields in that dbc point to fields in GMSurveyQuestions.dbc)
        recvData >> subSurveyId;
        if (!subSurveyId)
            break;

        uint8 rank; // probably some sort of ref to GMSurveyAnswers.dbc
        recvData >> rank;
        std::string comment; // comment ("Usage: GMSurveyAnswerSubmit(question, rank, comment)")
        recvData >> comment;

        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GM_SUBSURVEY);
        stmt->setUInt32(0, nextSurveyID);
        stmt->setUInt32(1, subSurveyId);
        stmt->setUInt32(2, rank);
        stmt->setString(3, comment);
        CharacterDatabase.Execute(stmt);
    }

    std::string comment; // just a guess
    recvData >> comment;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GM_SURVEY);
    stmt->setUInt32(0, GUID_LOPART(GetPlayer()->GetGUID()));
    stmt->setUInt32(1, nextSurveyID);
    stmt->setUInt32(2, mainSurvey);
    stmt->setString(3, comment);

    CharacterDatabase.Execute(stmt);
    */
}

void WorldSession::HandleReportLag(WorldPacket& recvData)
{
    // just put the lag report into the database...
    // can't think of anything else to do with it
    uint32 lagType, mapId;
    recvData >> lagType;
    recvData >> mapId;
    float x, y, z;
    recvData >> x;
    recvData >> y;
    recvData >> z;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_LAG_REPORT);
    stmt->setUInt32(0, GetPlayer()->GetGUID().GetCounter());
    stmt->setUInt8 (1, lagType);
    stmt->setUInt16(2, mapId);
    stmt->setFloat (3, x);
    stmt->setFloat (4, y);
    stmt->setFloat (5, z);
    stmt->setUInt32(6, GetLatency());
    stmt->setUInt32(7, time(NULL));
    CharacterDatabase.Execute(stmt);
}

void WorldSession::HandleGMResponseResolve(WorldPacket& /*recvPacket*/)
{
    // empty packet
    if (GmTicket* ticket = sTicketMgr->GetGmTicketByPlayerGuid(GetPlayer()->GetGUID()))
    {
        uint8 getSurvey = 0;
        if (float(rand_chance()) < sWorld->getFloatConfig(CONFIG_CHANCE_OF_GM_SURVEY))
            getSurvey = 1;

        // WorldPacket data(SMSG_GM_RESPONSE_STATUS_UPDATE, 4); // opcode not vaild
        // data << uint8(getSurvey);
        // SendPacket(&data);

        WorldPacket data2(SMSG_GM_TICKET_UPDATE, 4);
        data2 << uint32(GMTICKET_RESPONSE_TICKET_DELETED);
        SendPacket(&data2);

        sTicketMgr->CloseTicket<GmTicket>(ticket->GetTicketId(), GetPlayer()->GetGUID());
        sTicketMgr->SendGmTicket(this, NULL);
    }
}

void WorldSession::HandleSubmitBugOpcode(WorldPacket& recvPacket)
{
    if (!sTicketMgr->GetFeedBackSystemStatus())
        return;

    if (GetPlayer()->GetLevel() < sWorld->getIntConfig(CONFIG_TICKET_LEVEL_REQ))
    {
        SendNotification(GetTrinityString(LANG_TICKET_REQ), sWorld->getIntConfig(CONFIG_TICKET_LEVEL_REQ));
        return;
    }

    if (BugTicket* ticket = new BugTicket(GetPlayer(), recvPacket))
        sTicketMgr->AddTicket(ticket);
}
