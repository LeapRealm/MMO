#include "pch.h"
#include "ServerPacketHandler.h"

#include "GameRoom.h"
#include "GameSession.h"
#include "Player.h"
#include "PlayerManager.h"
#include "RoomManager.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	return false;
}

float pos = 0.f;

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	gameSession->_player = GPlayerManager.Add();
	{
		string name = "Player_" + to_string(gameSession->_player->_info.playerid());
		gameSession->_player->_info.set_name(name);

		Protocol::Vector* position = gameSession->_player->_info.mutable_positon();
		position->set_x(0);
		position->set_y(pos);
		pos += 400;
		position->set_z(0);

		gameSession->_player->_session = gameSession;
	}
	GRoomManager.Find(1)->EnterGame(gameSession->_player);

	return true;
}

bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt)
{
	return true;
}

bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	cout << pkt.msg() << endl;
	return true;
}
