#include "pch.h"
#include "Monster.h"

#include "Player.h"
#include "Room.h"

Monster::Monster()
{
	_monsterInfo = new Protocol::MonsterInfo();
	SetMonsterState(Protocol::MONSTER_STATE_PATROL);
}

Monster::~Monster()
{
	delete _monsterInfo;
}

void Monster::Update()
{
	switch (_monsterState)
	{
	case Protocol::MONSTER_STATE_PATROL: UpdatePatrol();  break;
	case Protocol::MONSTER_STATE_CHASE:  UpdateChase();  break;
	case Protocol::MONSTER_STATE_ATTACK: UpdateAttack();  break;
	}

	currAttackTime += 0.1f;

	Protocol::S_MOVE_MONSTER movePkt;
	Protocol::MonsterInfo* monsterInfo = movePkt.mutable_info();
	monsterInfo->CopyFrom(*_monsterInfo);

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
	GRoom->Broadcast(sendBuffer);
}

void Monster::UpdatePatrol()
{
	RoomRef room = _room.load().lock();
	if (room == nullptr)
		return;

	PlayerRef targetPlayer = nullptr;
	float minDist = FLT_MAX;
	const Protocol::Transform& MonsterTransform = _monsterInfo->transform();

	for (auto& playerPair : room->_players)
	{
		const Protocol::Transform& PlayerTransform = playerPair.second->playerInfo->transform();
		float dist = sqrt(pow(MonsterTransform.x() - PlayerTransform.x(), 2) + pow(MonsterTransform.y() - PlayerTransform.y(), 2));
		if (dist <= chaseRange && dist < minDist)
		{
			targetPlayer = playerPair.second;
			minDist = dist;
		}
	}

	if (targetPlayer)
	{
		_targetPlayer = targetPlayer;
		SetMonsterState(Protocol::MONSTER_STATE_CHASE);
	}
}

void Monster::UpdateChase()
{
	if (_targetPlayer.expired())
	{
		_monsterState = Protocol::MONSTER_STATE_PATROL;
		return;
	}

	PlayerRef targetPlayer = _targetPlayer.lock();
	const Protocol::Transform& monsterTransform = _monsterInfo->transform();
	const Protocol::Transform& playerTransform = targetPlayer->playerInfo->transform();

	float dist = sqrt(pow(monsterTransform.x() - playerTransform.x(), 2) + pow(monsterTransform.y() - playerTransform.y(), 2));
	if (dist <= attackRange)
		SetMonsterState(Protocol::MONSTER_STATE_ATTACK);
	else if (dist > chaseRange)
		SetMonsterState(Protocol::MONSTER_STATE_PATROL);

	Protocol::Transform* transform = _monsterInfo->mutable_transform();
	transform->set_x(monsterTransform.x() + ((playerTransform.x() - monsterTransform.x()) / dist * 300.f * 0.1f));
	transform->set_y(monsterTransform.y() + ((playerTransform.y() - monsterTransform.y()) / dist * 300.f * 0.1f));
}

void Monster::UpdateAttack()
{
	if (_targetPlayer.expired())
	{
		SetMonsterState(Protocol::MONSTER_STATE_PATROL);
		return;
	}

	PlayerRef targetPlayer = _targetPlayer.lock();
	const Protocol::Transform& monsterTransform = _monsterInfo->transform();
	const Protocol::Transform& playerTransform = targetPlayer->playerInfo->transform();

	float dist = sqrt(pow(monsterTransform.x() - playerTransform.x(), 2) + pow(monsterTransform.y() - playerTransform.y(), 2));
	if (dist > attackRange)
	{
		if (currAttackTime >= targetAttackTime)
			SetMonsterState(Protocol::MONSTER_STATE_CHASE);
		return;
	}

	if (currAttackTime >= targetAttackTime)
	{
		currAttackTime = 0.f;

		Protocol::S_ATTACK attackPkt;
		attackPkt.set_fromobjectid(_monsterInfo->objectid());
		attackPkt.set_toobjectid(_targetPlayer.lock()->playerInfo->objectid());

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(attackPkt);
		GRoom->Broadcast(sendBuffer);
	}
}

void Monster::SetMonsterState(Protocol::MonsterState newState)
{
	_monsterState = newState;

	switch (newState)
	{
	case Protocol::MONSTER_STATE_PATROL:
		cout << "Monster State : [PATROL]" << endl;
		break;
	case Protocol::MONSTER_STATE_CHASE:
		cout << "Monster State : [CHASE]" << endl;
		break;
	case Protocol::MONSTER_STATE_ATTACK:
		cout << "Monster State : [ATTACK]" << endl;
		break;
	}
}
