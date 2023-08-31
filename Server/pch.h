#pragma once

#include "CorePch.h"

#include "Protocol.pb.h"
#include "Enum.pb.h"
#include "Struct.pb.h"

#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif

using GameSessionRef = std::shared_ptr<class GameSession>;
using GameRoomRef = std::shared_ptr<class GameRoom>;

using GameObjectRef = std::shared_ptr<class GameObject>;
using CreatureRef = std::shared_ptr<class Creature>;
using PlayerRef = std::shared_ptr<class Player>;
using MonsterRef = std::shared_ptr<class Monster>;

#include "ServerPacketHandler.h"

using ObjectState = Protocol::OBJECT_STATE_TYPE;
using Dir = Protocol::DIR_TYPE;
using Stat = Protocol::StatData;

#define DIR_UP Dir::DIR_TYPE_UP
#define DIR_DOWN Dir::DIR_TYPE_DOWN
#define DIR_LEFT Dir::DIR_TYPE_LEFT
#define DIR_RIGHT Dir::DIR_TYPE_RIGHT


#define IDLE ObjectState::OBJECT_STATE_TYPE_IDLE
#define MOVE ObjectState::OBJECT_STATE_TYPE_MOVE
#define SKILL ObjectState::OBJECT_STATE_TYPE_SKILL

struct VectorInt
{
	VectorInt() { }
	VectorInt(int32 x, int32 y) : x(x), y(y) { }
	VectorInt(POINT pt) : x((int32)pt.x), y((int32)pt.y) {}

	VectorInt operator+(const VectorInt& other)
	{
		VectorInt ret;
		ret.x = x + other.x;
		ret.y = y + other.y;
		return ret;
	}

	VectorInt operator-(const VectorInt& other)
	{
		VectorInt ret;
		ret.x = x - other.x;
		ret.y = y - other.y;
		return ret;
	}

	VectorInt operator*(const VectorInt& other)
	{
		VectorInt ret;
		ret.x = x * other.x;
		ret.y = y * other.y;
		return ret;
	}

	VectorInt operator*(int32 n)
	{
		VectorInt ret;
		ret.x = x * n;
		ret.y = y * n;
		return ret;
	}

	VectorInt operator/(const VectorInt& other)
	{
		VectorInt ret;
		ret.x = x / other.x;
		ret.y = y / other.y;
		return ret;
	}

	VectorInt operator/(int32 n)
	{
		VectorInt ret;
		ret.x = x / n;
		ret.y = y / n;
		return ret;
	}

	void operator+=(const VectorInt& other)
	{
		x += other.x;
		y += other.y;
	}

	void operator-=(const VectorInt& other)
	{
		x -= other.x;
		y -= other.y;
	}

	void operator*=(const VectorInt& other)
	{
		x *= other.x;
		y *= other.y;
	}

	void operator/=(const VectorInt& other)
	{
		x /= other.x;
		y /= other.y;
	}

	bool operator<(const VectorInt& other) const
	{
		if (x != other.x) return x < other.x;
		return y < other.y;
	}

	bool operator>(const VectorInt& other) const
	{
		if (x != other.x) return x > other.x;
		return y > other.y;
	}

	bool operator==(const VectorInt& other) const
	{
		return x == other.x && y == other.y;
	}

	float LengthSquared()
	{
		return x * x + y * y;
	}

	float Length()
	{
		return ::sqrt(LengthSquared());
	}

	float Dot(VectorInt other)
	{
		return x * other.x + y * other.y;
	}

	float Cross(VectorInt other)
	{
		return x * other.y - other.x * y;
	}

	int32 x = 0;
	int32 y = 0;
};

using Vec2Int = VectorInt;