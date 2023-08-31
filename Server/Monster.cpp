#include "pch.h"
#include "Monster.h"
#include "GameRoom.h"
#include "Player.h"

Monster::Monster()
{

}

Monster::~Monster()
{

}

void Monster::Init()
{

}

void Monster::Update()
{
	Super::Update();

	switch (info.state())
	{
	case IDLE:
		TickIdle();
		break;
	case MOVE:
		TickMove();
		break;
	case SKILL:
		TickSkill();
		break;
	}
}


void Monster::TickIdle()
{
	if (room == nullptr) return;

	// if (_target.lock() == nullptr) 
		_target = room->FindClosestPlayer(GetCellPos());

	PlayerRef target = _target.lock();
	if (target)
	{
		Vec2Int dir = target->GetCellPos() - GetCellPos();

		if (abs(dir.x) + abs(dir.y) == 1)
		{
			SetDir(GetLookAtDir(target->GetCellPos()));
			SetState(SKILL, true);
			_waitUntil = GetTickCount64() + 500;
		}
		else
		{
			vector<Vec2Int> path;

			if (room->FindPath(GetCellPos(), target->GetCellPos(), OUT path))
			{
				if (path.size() >= 1)
				{
					Vec2Int nextPos = path[0];

					if (room->CanGo(nextPos))
					{
						SetDir(GetLookAtDir(nextPos));
						SetCellPos(nextPos);
						_waitUntil = GetTickCount64() + 1000;
						SetState(MOVE, true);
					}
				}
			}
		}
	}
}

void Monster::TickMove()
{
	uint64 now = GetTickCount64();
	if (_waitUntil > now) return;

	SetState(IDLE);
}

void Monster::TickSkill()
{
	uint64 now = GetTickCount64();
	if (_waitUntil > now) return;

	SetState(IDLE);
}