#include "pch.h"
#include "ObjectUtils.h"

#include "GameSession.h"
#include "Monster.h"
#include "Player.h"

atomic<int64> ObjectUtils::s_idGenerator = 1;

PlayerRef ObjectUtils::CreatePlayer(GameSessionRef session)
{
	const int64 newID = s_idGenerator.fetch_add(1);

	PlayerRef player = make_shared<Player>();
	player->playerInfo->set_objectid(newID);

	player->session = session;
	session->player.store(player);

	return player;
}

MonsterRef ObjectUtils::CreateMonster()
{
	const int64 newID = s_idGenerator.fetch_add(1);

	MonsterRef monster = make_shared<Monster>();
	monster->_monsterInfo->set_objectid(newID);

	return monster;
}
