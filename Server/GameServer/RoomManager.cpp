#include "pch.h"
#include "RoomManager.h"

#include "GameRoom.h"

RoomManager GRoomManager;

GameRoomRef RoomManager::Add()
{
	GameRoomRef room = make_shared<GameRoom>();

	WRITE_LOCK;

	room->_roomID = _roomID;
	_rooms.emplace(_roomID, room);
	_roomID++;

	return room;
}

void RoomManager::Remove(uint64 roomID)
{
	WRITE_LOCK;

	auto roomIt = _rooms.find(roomID);
	if (roomIt == _rooms.end())
		return;

	_rooms.erase(roomIt);
}

GameRoomRef RoomManager::Find(uint64 roomID)
{
	WRITE_LOCK;

	auto roomIt = _rooms.find(roomID);
	if (roomIt == _rooms.end())
		return nullptr;

	return roomIt->second;
}
