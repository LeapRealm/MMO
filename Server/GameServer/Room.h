#pragma once

class Room : public JobQueue
{
public:
	Room();
	virtual ~Room();

public:
	bool HandleEnterPlayer(PlayerRef player);
	bool HandleLeavePlayer(PlayerRef player);

	void HandleMove(Protocol::C_MOVE pkt);

	RoomRef GetRoomRef();

private:
	bool EnterPlayer(PlayerRef player);
	bool LeavePlayer(uint64 objectID);
	void Broadcast(SendBufferRef sendBuffer, uint64 exceptID = 0);

private:
	unordered_map<uint64, PlayerRef> _players;
};

extern RoomRef GRoom;
