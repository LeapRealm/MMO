#pragma once

class GameRoom : public enable_shared_from_this<GameRoom>
{
public:
	GameRoom();
	~GameRoom();

public:
	void EnterGame(PlayerRef newPlayer);
	void LeaveGame(uint64 playerID);

public:
	USE_LOCK;
	uint64 _roomID = 0;
	map<uint64, PlayerRef> _players;
};
