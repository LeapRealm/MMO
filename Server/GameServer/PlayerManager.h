#pragma once

class PlayerManager
{
public:
	PlayerRef Add();
	void Remove(uint64 playerID);
	PlayerRef Find(uint64 playerID);

public:
	USE_LOCK;
	map<uint64, PlayerRef> _players;
	uint64 _playerID = 1;
};

extern PlayerManager GPlayerManager;
