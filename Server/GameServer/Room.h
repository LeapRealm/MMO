#pragma once

class Room : public enable_shared_from_this<Room>
{
public:
	Room();
	virtual ~Room();

public:
	bool HandleEnterPlayerLocked(PlayerRef player);
	bool HandleLeavePlayerLocked(PlayerRef player);

private:
	bool EnterPlayer(PlayerRef player);
	bool LeavePlayer(uint64 objectID);
	void Broadcast(SendBufferRef sendBuffer, uint64 exceptID = 0);

private:
	USE_LOCK;
	unordered_map<uint64, PlayerRef> _players;
};

extern RoomRef GRoom;
