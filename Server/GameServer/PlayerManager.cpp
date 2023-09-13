#include "pch.h"
#include "PlayerManager.h"

#include "Player.h"

PlayerManager GPlayerManager;

PlayerRef PlayerManager::Add()
{
	PlayerRef player = make_shared<Player>();

	WRITE_LOCK;

	player->_info.set_playerid(_playerID);
	_players.emplace(_playerID, player);
	_playerID++;

	return player;
}

void PlayerManager::Remove(uint64 playerID)
{
	WRITE_LOCK;

	auto playerIt = _players.find(playerID);
	if (playerIt == _players.end())
		return;

	_players.erase(playerIt);
}

PlayerRef PlayerManager::Find(uint64 playerID)
{
	WRITE_LOCK;

	auto playerIt = _players.find(playerID);
	if (playerIt == _players.end())
		return nullptr;

	return playerIt->second;
}
