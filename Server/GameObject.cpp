#include "pch.h"
#include "GameObject.h"
#include "Player.h"
#include "Monster.h"
#include "GameRoom.h"

atomic<uint64> GameObject::s_idGenerator = 1;

GameObject::GameObject()
{

}

GameObject::~GameObject()
{

}

PlayerRef GameObject::CreatePlayer()
{
	PlayerRef player = make_shared<Player>();
	player->info.set_objectid(s_idGenerator++);
	player->info.set_objecttype(Protocol::OBJECT_TYPE_PLAYER);
	return player;
}

MonsterRef GameObject::CreateMonster()
{
	MonsterRef monster = make_shared<Monster>();
	monster->info.set_objectid(s_idGenerator++);
	monster->info.set_objecttype(Protocol::OBJECT_TYPE_MONSTER);
	return monster;
}

void GameObject::Update()
{
	
}

void GameObject::SetState(ObjectState state, bool broadcast)
{
	if (info.state() == state) return;
	info.set_state(state);

	if (broadcast) BroadcastMove();
}


void GameObject::SetDir(Dir dir, bool broadcast)
{
	info.set_dir(dir);
	if (broadcast) BroadcastMove();
}

Dir GameObject::GetLookAtDir(Vec2Int cellPos)
{
	Vec2Int dir = cellPos - GetCellPos();
	if (dir.x > 0) return Dir::DIR_TYPE_RIGHT;
	else if (dir.x < 0) return Dir::DIR_TYPE_LEFT;
	else if (dir.y > 0) return  Dir::DIR_TYPE_DOWN;
	else return  Dir::DIR_TYPE_UP;
}


bool GameObject::CanGo(Vec2Int cellPos)
{
	if (room == nullptr) return false;
	return room->CanGo(cellPos);
}

void GameObject::SetCellPos(Vec2Int cellPos, bool broadcast)
{
	info.set_posx(cellPos.x);
	info.set_posy(cellPos.y);

	if (broadcast) BroadcastMove();
}

void GameObject::BroadcastMove()
{
	if (room)
	{
		SendBufferRef sendBuffer = ServerPacketHandler::Make_S_Move(info);
		room->BroadCast(sendBuffer);
	}
}

Vec2Int GameObject::GetFrontCellPos()
{
	Vec2Int pos = GetCellPos();

	switch (info.dir())
	{
	case Dir::DIR_TYPE_DOWN:
		return pos + Vec2Int{ 0, 1 };
		break;
	case Dir::DIR_TYPE_LEFT:
		return pos + Vec2Int{ -1, 0 };
		break;
	case Dir::DIR_TYPE_RIGHT:
		return pos + Vec2Int{ 1, 0 };
		break;
	case Dir::DIR_TYPE_UP:
		return pos + Vec2Int{ 0, -1 };
		break;
	}

	return pos;
}