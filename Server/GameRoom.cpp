#include "pch.h"
#include "GameRoom.h"
#include "Player.h"
#include "Monster.h"
#include "GameSession.h"
#include "GameSessionManager.h"

GameRoomRef GRoom = make_shared<GameRoom>();

GameRoom::GameRoom() : _tileMap(new TileMap())
{
}

GameRoom::~GameRoom()
{
}

void GameRoom::Init()
{
	MonsterRef monster = GameObject::CreateMonster();
	monster->info.set_posx(8);
	monster->info.set_posy(8);
	AddObject(monster);

	_tileMap->LoadFile(L"E:\\VS repo\\WinGameCoding\\Resources\\TileMap\\TileMap_01.txt");
}

void GameRoom::Update()
{
	for (auto& item : _players)
	{
		item.second->Update();
	}

	for (auto& item : _monsters)
	{
		item.second->Update();
	}

	TickMonsterSpawn();
}

void GameRoom::EnterRoom(GameSessionRef session)
{
	PlayerRef player = GameObject::CreatePlayer();

	session->gameRoom = GetRoomRef();
	session->player = player;
	player->session = session;

	player->info.set_posx(5);
	player->info.set_posy(5);

	// 입장한 클라에게 정보 보내기
	{
		SendBufferRef sendBuffer = ServerPacketHandler::Make_S_MyPlayer(player->info);
		session->Send(sendBuffer);
	}

	AddObject(player);

	// 모든 오브젝트 정보 보내기
	{
		Protocol::S_AddObject pkt;

		for (auto& item : _players)
		{
			auto info = pkt.add_objects();
			*info = item.second->info;
		}

		for (auto& item : _monsters)
		{
			auto info = pkt.add_objects();
			*info = item.second->info;
		}

		SendBufferRef sendBuffer = ServerPacketHandler::Make_S_AddObject(pkt);
		session->Send(sendBuffer);
	}
}

void GameRoom::LeaveRoom(GameSessionRef session)
{
	if (session == nullptr) return;
	if (session->player.lock() == nullptr) return;

	uint64 id = session->player.lock()->info.objectid();
	RemoveObject(id);
}

GameObjectRef GameRoom::FindObjectById(uint64 id)
{
	{
		auto findit = _players.find(id);
		if (findit != _players.end()) return findit->second;
	}
	{
		auto findit = _monsters.find(id);
		if (findit != _monsters.end()) return findit->second;
	}

	return nullptr;
}

void GameRoom::Handle_C_Move(Protocol::C_Move& pkt)
{
	uint64 id = pkt.info().objectid();
	GameObjectRef gameObject = FindObjectById(id);

	if (gameObject == nullptr) return;

	gameObject->info.set_state(pkt.info().state());
	gameObject->info.set_dir(pkt.info().dir());
	gameObject->info.set_posx(pkt.info().posx());
	gameObject->info.set_posy(pkt.info().posy());

	{
		SendBufferRef sendBuffer = ServerPacketHandler::Make_S_Move(pkt.info());
		BroadCast(sendBuffer);
	}
}

void GameRoom::AddObject(GameObjectRef object)
{
	uint64 id = object->info.objectid();
	auto objectType = object->info.objecttype();

	switch (objectType)
	{
	case Protocol::OBJECT_TYPE_PLAYER:
		_players[id] = static_pointer_cast<Player>(object);
		break;
	case Protocol::OBJECT_TYPE_MONSTER:
		_monsters[id] = static_pointer_cast<Monster>(object);
		_monsterCount++;
		break;
	default:
		return;
	}

	object->room = GetRoomRef();

	// 신규 오브젝트 정보 전송
	{
		Protocol::S_AddObject pkt;

		Protocol::ObjectInfo* info = pkt.add_objects();
		*info = object->info;

		SendBufferRef sendBuffer = ServerPacketHandler::Make_S_AddObject(pkt);
		BroadCast(sendBuffer);
	}
}

void GameRoom::RemoveObject(uint64 id)
{
	GameObjectRef object = FindObjectById(id);
	if (object == nullptr) return;

	switch (object->info.objecttype())
	{
	case Protocol::OBJECT_TYPE_PLAYER:
		_players.erase(id);
		break;
	case Protocol::OBJECT_TYPE_MONSTER:
		_monsters.erase(id);
		_monsterCount--;
		break;
	default:
		return;
	}

	object->room = nullptr;

	// 오브젝트 삭제 보내기
	{
		Protocol::S_RemoveObject pkt;
		pkt.add_ids(id);

		SendBufferRef sendBuffer = ServerPacketHandler::Make_S_RemvoeObject(pkt);
		BroadCast(sendBuffer);
	}
}

void GameRoom::BroadCast(SendBufferRef sendBuffer)
{
	for (auto& item : _players)
	{
		item.second->session->Send(sendBuffer);
	}
}

PlayerRef GameRoom::FindClosestPlayer(Vec2Int cellPos)
{
	float best = FLT_MAX;
	PlayerRef ret = nullptr;

	for (auto& item : _players)
	{
		PlayerRef player = item.second;
		if (player)
		{
			Vec2Int dir = player->GetCellPos() - cellPos;
			float dist = dir.LengthSquared();

			if (dist < best)
			{
				best = dist;
				ret = player;
			}
		}
	}

	return ret;
}

bool GameRoom::FindPath(Vec2Int src, Vec2Int dest, vector<Vec2Int>& path, int32 maxDepth)
{
	int32 depth = abs(dest.y - src.y) + abs(dest.x - src.x);
	if (depth >= maxDepth) return false;

	priority_queue<PQNode, vector<PQNode>, greater<PQNode>> pq;
	map<Vec2Int, int32> best;
	map<Vec2Int, Vec2Int> parent;

	// 초기 값
	{
		int32 cost = abs(dest.y - src.y) + abs(dest.x - src.x);
		pq.push(PQNode(cost, src));
		best[src] = cost;
		parent[src] = src;
	}

	Vec2Int front[4] =
	{
		{ 0, -1 }, // UP
		{ 0, 1 }, // DOWN
		{ -1, 0}, // RIGHT
		{ 1, 0 }, // LEFT
	};

	bool found = false;
	while (!pq.empty())
	{
		// 제일 좋은 후보를 찾는다.
		PQNode now = pq.top();
		pq.pop();

		// 더 짧은 경로를 뒤늦게 찾았다면 스킵
		if (best[now.pos] < now.cost) continue;
		if (now.pos == dest)
		{
			found == true;
			break;
		}

		// 방문
		for (int32 dir = 0; dir < 4; dir++)
		{
			Vec2Int nextPos = now.pos + front[dir];

			if (CanGo(nextPos) == false) continue;

			int32 depth = abs(nextPos.y - src.y) + abs(nextPos.x - src.x);
			if (depth >= maxDepth) continue;

			// 가중치 계산
			int32 cost = abs(dest.y - nextPos.y) + abs(dest.x - nextPos.x);
			int32 bestCost = best[nextPos];

			if (bestCost != 0)
			{
				if (bestCost <= cost) continue;
			}

			pq.push(PQNode(cost, nextPos));

			best[nextPos] = cost;
			parent[nextPos] = now.pos;
		}
	}

	if (found == false)
	{
		// TODO
		float bestScore = FLT_MAX;

		for (auto& item : best)
		{
			Vec2Int pos = item.first;
			int32 score = item.second;

			if (bestScore == score)
			{
				int32 dist1 = abs(dest.x - src.x) + abs(dest.y - src.y);
				int32 dist2 = abs(pos.x - src.x) + abs(pos.y - src.y);

				if (dist1 > dist2) dest = pos;
			}

			else if (bestScore > score)
			{
				dest = pos;
				bestScore = score;
			}
		}
	}

	path.clear();
	Vec2Int pos = dest;

	while (!(parent[pos] == pos))
	{
		path.push_back(pos);
		pos = parent[pos];
	}

	std::reverse(path.begin(), path.end());
	return true;
}

bool GameRoom::CanGo(Vec2Int cellPos)
{
	if (_tileMap == nullptr) return false;

	Tile* tile = _tileMap->GetTileAt(cellPos);
	if (tile == nullptr) return false;

	if (GetGameObjectAt(cellPos) != nullptr) return false;

	return tile->value != 1;
}

Vec2Int GameRoom::GetRandomEmptyCellPos()
{
	Vec2Int ret = { -1, -1 };

	if (_tileMap == nullptr) return ret;

	TileMap* tm = _tileMap;
	if (tm == nullptr) return ret;

	Vec2Int size = tm->GetMapSize();

	while (true)
	{
		int32 x = rand() % size.x;
		int32 y = rand() % size.y;
		Vec2Int cellPos{ x, y };

		if (CanGo(cellPos)) return cellPos;
	}
}

GameObjectRef GameRoom::GetGameObjectAt(Vec2Int cellPos)
{
	for (auto& item : _players)
	{
		if (item.second->GetCellPos() == cellPos)
			return item.second;
	}

	for (auto& item : _monsters)
	{
		if (item.second->GetCellPos() == cellPos)
			return item.second;
	}

	return nullptr;
}

void GameRoom::TickMonsterSpawn()
{

	if (_monsterCount < DESIRED_MONSTER_COUNT)
	{
		MonsterRef monster = GameObject::CreateMonster();
		Vec2Int randomPos = GetRandomEmptyCellPos();
		monster->SetCellPos(randomPos);
		AddObject(monster);
	}
}
