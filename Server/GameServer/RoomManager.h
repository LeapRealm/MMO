#pragma once

class RoomManager
{
public:
	GameRoomRef Add();
	void Remove(uint64 roomID);
	GameRoomRef Find(uint64 roomID);

public:
	USE_LOCK;
	map<uint64, GameRoomRef> _rooms;
	uint64 _roomID = 1;
};

extern RoomManager GRoomManager;
