#pragma once
#include "TileMap.h"
#include "GameObject.h"

struct PQNode
{
	PQNode(int32 cost, Vec2Int pos) : cost(cost), pos(pos) {}

	bool operator<(const PQNode& other) const { return cost < other.cost; }
	bool operator>(const PQNode& other) const { return cost > other.cost; }

	int32 cost;
	Vec2Int pos;
};
class GameRoom : public enable_shared_from_this<GameRoom>
{
public:
	GameRoom();
	~GameRoom();

	void Init();
	void Update();

	void EnterRoom(GameSessionRef session);
	void LeaveRoom(GameSessionRef session);

	GameObjectRef FindObjectById(uint64 id);
	GameRoomRef GetRoomRef() { return shared_from_this(); }

public:
	void Handle_C_Move(Protocol::C_Move& pkt);

public:
	void AddObject(GameObjectRef object);
	void RemoveObject(uint64 id);
	void BroadCast(SendBufferRef snedBUffer);

public:
	PlayerRef FindClosestPlayer(Vec2Int cellPos);
	bool FindPath(Vec2Int src, Vec2Int dest, vector<Vec2Int>& path, int32 maxDepth = 10);
	bool CanGo(Vec2Int cellPos);
	Vec2Int GetRandomEmptyCellPos();
	GameObjectRef GetGameObjectAt(Vec2Int cellPos);

private:
	void TickMonsterSpawn();


private:
	unordered_map<uint64, PlayerRef> _players;
	unordered_map<uint64, MonsterRef> _monsters;

	const int32 DESIRED_MONSTER_COUNT = 20;
	int32 _monsterCount = 0;

	TileMap* _tileMap;
};

extern GameRoomRef GRoom;
