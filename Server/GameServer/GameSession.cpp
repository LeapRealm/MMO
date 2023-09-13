#include "pch.h"
#include "GameSession.h"

#include "GameRoom.h"
#include "GameSessionManager.h"
#include "Player.h"
#include "PlayerManager.h"
#include "RoomManager.h"
#include "ServerPacketHandler.h"

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	// TODO: For Test
	GRoomManager.Find(1)->LeaveGame(_player->_info.playerid());
	GPlayerManager.Remove(_player->_info.playerid());

	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	ServerPacketHandler::HandlePacket(session, buffer, len);
}

void GameSession::OnSend(int32 len)
{

}
