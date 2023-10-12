#pragma once

class Room;

class Monster : public enable_shared_from_this<Monster>
{
public:
	Monster();
	virtual ~Monster();

public:
	void Update();

	void UpdatePatrol();
	void UpdateChase();
	void UpdateAttack();

public:
	void SetMonsterState(Protocol::MonsterState newState);

public:
	Protocol::MonsterState _monsterState;
	Protocol::MonsterInfo* _monsterInfo;
	atomic<weak_ptr<Room>> _room;
	weak_ptr<Player> _targetPlayer;

private:
	float chaseRange = 500.f;
	float attackRange = 250.f;

	float currAttackTime = 0.f;
	float targetAttackTime = 1.5f;
};
