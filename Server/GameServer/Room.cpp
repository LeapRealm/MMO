#include "pch.h"
#include "Room.h"

#include "GameSession.h"
#include "Player.h"

RoomRef GRoom = make_shared<Room>();

Room::Room()
{

}

Room::~Room()
{

}

bool Room::HandleEnterPlayerLocked(PlayerRef player)
{
	WRITE_LOCK;

	bool success = EnterPlayer(player);

	Protocol::Transform* transform = player->playerInfo->mutable_transform();
	transform->set_yaw(Utils::GetRandom(0.f, 100.f));

	Protocol::Vector3D* position = transform->mutable_position();
	position->set_x(Utils::GetRandom(0.f, 500.f));
	position->set_y(Utils::GetRandom(0.f, 500.f));
	position->set_z(Utils::GetRandom(0.f, 500.f));

	{
		Protocol::S_ENTER_GAME enterGamePkt;
		enterGamePkt.set_success(success);

		Protocol::PlayerInfo* playerInfo = enterGamePkt.mutable_player();
		playerInfo->CopyFrom(*player->playerInfo);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
		if (GameSessionRef session = player->session.lock())
			session->Send(sendBuffer);
	}
	{
		Protocol::S_SPAWN spawnPkt;

		Protocol::PlayerInfo* playerInfo = spawnPkt.add_players();
		playerInfo->CopyFrom(*player->playerInfo);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
		Broadcast(sendBuffer, player->playerInfo->objectid());
	}
	{
		Protocol::S_SPAWN spawnPkt;

		for (const auto& pair : _players)
		{
			Protocol::PlayerInfo* playerInfo = spawnPkt.add_players();
			playerInfo->CopyFrom(*pair.second->playerInfo);
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
		if (SessionRef session = player->session.lock())
			session->Send(sendBuffer);
	}

	return success;
}

bool Room::HandleLeavePlayerLocked(PlayerRef player)
{
	if (player == nullptr)
		return false;

	WRITE_LOCK;

	const uint64 objectID = player->playerInfo->objectid();
	if (LeavePlayer(objectID) == false)
		return false;

	{
		Protocol::S_LEAVE_GAME leaveGamePkt;

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(leaveGamePkt);
		if (SessionRef session = player->session.lock())
			session->Send(sendBuffer);
	}
	{
		Protocol::S_DESPAWN despawnPkt;
		despawnPkt.add_objectids(objectID);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(despawnPkt);
		Broadcast(sendBuffer, objectID);

		if (SessionRef session = player->session.lock())
			session->Send(sendBuffer);
	}

	return true;
}

bool Room::EnterPlayer(PlayerRef player)
{
	if (_players.contains(player->playerInfo->objectid()))
		return false;

	_players.emplace(player->playerInfo->objectid(), player);
	player->room.store(shared_from_this());

	return true;
}

bool Room::LeavePlayer(uint64 objectID)
{
	if (_players.contains(objectID) == false)
		return false;

	PlayerRef player = _players[objectID];
	player->room.store(weak_ptr<Room>());

	_players.erase(objectID);

	return true;
}

void Room::Broadcast(SendBufferRef sendBuffer, uint64 exceptID)
{
	for (const auto& pair : _players)
	{
		PlayerRef player = pair.second;
		if (player->playerInfo->objectid() == exceptID)
			continue;

		if (GameSessionRef session = player->session.lock())
			session->Send(sendBuffer);
	}
}
