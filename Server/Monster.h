#pragma once
#include "GameObject.h"

class Monster: public GameObject
{
	using Super = GameObject;
public:
	Monster();
	virtual ~Monster();

	virtual void Init();
	virtual void Update();

protected:
	virtual void TickIdle();
	virtual void TickMove();
	virtual void TickSkill();

protected:
	weak_ptr<Player> _target;
	uint64 _waitUntil = 0;
};

