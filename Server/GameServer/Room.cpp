#include "pch.h"
#include "Room.h"

#include "GameSession.h"
#include "Monster.h"
#include "ObjectUtils.h"
#include "Player.h"

RoomRef GRoom = make_shared<Room>();

Room::Room()
{
	
}

Room::~Room()
{

}

bool Room::HandleEnterPlayer(PlayerRef player)
{
	bool success = EnterPlayer(player);

	Protocol::Transform* transform = player->playerInfo->mutable_transform();
	transform->set_x(Utils::GetRandom(0.f, 500.f));
	transform->set_y(Utils::GetRandom(0.f, 500.f));
	transform->set_z(100.f);
	transform->set_yaw(Utils::GetRandom(0.f, 100.f));

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

		for (const auto& pair : _monsters)
		{
			Protocol::MonsterInfo* monsterInfo = spawnPkt.add_monsters();
			monsterInfo->CopyFrom(*pair.second->_monsterInfo);
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
		if (SessionRef session = player->session.lock())
			session->Send(sendBuffer);
	}

	return success;
}

bool Room::HandleLeavePlayer(PlayerRef player)
{
	if (player == nullptr)
		return false;

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

void Room::HandleMovePlayer(Protocol::C_MOVE_PLAYER pkt)
{
	const uint64 objectID = pkt.info().objectid();
	if (_players.contains(objectID) == false)
		return;

	PlayerRef& player = _players[objectID];
	player->playerInfo->CopyFrom(pkt.info());

	Protocol::S_MOVE_PLAYER movePkt;
	Protocol::PlayerInfo* info = movePkt.mutable_info();
	info->CopyFrom(pkt.info());

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
	Broadcast(sendBuffer);
}

void Room::Init()
{
	MonsterRef monster = ObjectUtils::CreateMonster();
	Protocol::Transform* transform = monster->_monsterInfo->mutable_transform();
	transform->set_x(Utils::GetRandom(1000.f, 1500.f));
	transform->set_y(Utils::GetRandom(1000.f, 1500.f));
	transform->set_z(100.f);
	transform->set_yaw(Utils::GetRandom(0.f, 100.f));

	monster->_room = static_pointer_cast<Room>(shared_from_this());
	_monsters[monster->_monsterInfo->objectid()] = monster;
}

void Room::Update()
{
	for (auto& monsterPair : _monsters)
	{
		monsterPair.second->Update();
	}

	GRoom->DoTimer(100, [this]()
	{
		Update();
	});
}

RoomRef Room::GetRoomRef()
{
	return static_pointer_cast<Room>(shared_from_this());
}

bool Room::EnterPlayer(PlayerRef player)
{
	if (_players.contains(player->playerInfo->objectid()))
		return false;

	_players.emplace(player->playerInfo->objectid(), player);
	player->room.store(GetRoomRef());

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
