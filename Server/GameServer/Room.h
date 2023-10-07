#pragma once

class Room : public JobQueue
{
public:
	Room();
	virtual ~Room();

public:
	bool HandleEnterPlayer(PlayerRef player);
	bool HandleLeavePlayer(PlayerRef player);
	void HandleMovePlayer(Protocol::C_MOVE_PLAYER pkt);

	void Init();
	void Update();

	RoomRef GetRoomRef();

public:
	bool EnterPlayer(PlayerRef player);
	bool LeavePlayer(uint64 objectID);
	void Broadcast(SendBufferRef sendBuffer, uint64 exceptID = 0);

public:
	unordered_map<uint64, PlayerRef> _players;
	unordered_map<uint64, MonsterRef> _monsters;
};

extern RoomRef GRoom;
