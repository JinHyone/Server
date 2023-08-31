#pragma once


class GameObject : public enable_shared_from_this<GameObject>
{
public:
	static PlayerRef CreatePlayer();
	static MonsterRef CreateMonster();

public:
	GameObject();
	virtual ~GameObject();

	virtual void Update();

public:
	GameObjectRef GetGameObjectRef() { return shared_from_this(); }
	Vec2Int GetCellPos() { return Vec2Int(info.posx(), info.posy()); }

	void SetState(ObjectState state, bool broadcast = false);
	void SetDir(Dir dir, bool broadcast = false);
	Dir  GetLookAtDir(Vec2Int cellPos);
	bool CanGo(Vec2Int cellPos);

	void SetCellPos(Vec2Int cellPos, bool broadcast = false);
	Vec2Int GetFrontCellPos();

	uint64 GetObjectID() { return info.objectid(); }
	void SetObjectID(uint64 id) { info.set_objectid(id); }

	void BroadcastMove();

public:
	Protocol::ObjectInfo info;
	GameRoomRef room;

private:
	static atomic<uint64> s_idGenerator;
};

