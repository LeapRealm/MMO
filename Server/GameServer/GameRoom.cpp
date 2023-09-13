#include "pch.h"
#include "GameRoom.h"

#include "GameSession.h"
#include "Player.h"

GameRoom::GameRoom()
{

}

GameRoom::~GameRoom()
{

}

void GameRoom::EnterGame(PlayerRef newPlayer)
{
	if (newPlayer == nullptr)
		return;

	WRITE_LOCK;
	_players.emplace(newPlayer->_info.playerid(), newPlayer);
	newPlayer->_room = shared_from_this();

	// 입장한 플레이어에게 자신의 정보 전송
	{
		Protocol::S_ENTER_GAME enterGamePkt;
		enterGamePkt.mutable_playerinfo()->CopyFrom(newPlayer->_info);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
		if (GameSessionRef gameSession = newPlayer->_session.lock())
			gameSession->Send(sendBuffer);
	}

	// 입장한 플레이어에게 다른 플레이어들의 정보 전송
	{
		Protocol::S_SPAWN spawnPkt;
		for (auto& player : _players)
		{
			if (newPlayer != player.second)
			{
				Protocol::PlayerInfo* playerInfo = spawnPkt.add_players();
				playerInfo->CopyFrom(player.second->_info);
			}
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
		if (GameSessionRef gameSession = newPlayer->_session.lock())
			gameSession->Send(sendBuffer);
	}

	// 입장한 플레이어를 제외한 다른 플레이어들에게 입장한 플레이어에 대한 정보 전송
	{
		Protocol::S_SPAWN spawnPkt;
		Protocol::PlayerInfo* playerInfo = spawnPkt.add_players();
		playerInfo->CopyFrom(newPlayer->_info);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
		for (auto& player : _players)
		{
			if (newPlayer != player.second)
			{
				if (GameSessionRef gameSession = player.second->_session.lock())
					gameSession->Send(sendBuffer);
			}
		}
	}
}

void GameRoom::LeaveGame(uint64 playerID)
{
	WRITE_LOCK;

	auto leavePlayerIt = _players.find(playerID);
	if (leavePlayerIt == _players.end())
		return;

	PlayerRef leavePlayer = leavePlayerIt->second;

	// 본인한테 Leave 패킷 전송
	{
		Protocol::S_LEAVE_GAME leaveGamePkt;
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(leaveGamePkt);

		if (GameSessionRef gameSession = leavePlayer->_session.lock())
			gameSession->Send(sendBuffer);
	}

	// 다른 사람들한테 Despawn 패킷 전송
	{
		Protocol::S_DESPAWN despawnPkt;
		despawnPkt.add_playerids(leavePlayer->_info.playerid());

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(despawnPkt);
		for (auto& player : _players)
		{
			if (leavePlayer != player.second)
			{
				if (GameSessionRef gameSession = player.second->_session.lock())
					gameSession->Send(sendBuffer);
			}
		}
	}

	_players.erase(leavePlayerIt);
}
