#include "pch.h"
#include "TileMap.h"
#include "GameObject.h"
#include <fstream>

TileMap::TileMap()
{
}

TileMap::~TileMap()
{
}

void TileMap::LoadFile(const wstring& path)
{
	if (false)
	{
		FILE* file = nullptr;
		::_wfopen_s(&file, path.c_str(), L"rb");
		assert(file != nullptr);

		::fread(&_mapSize.x, sizeof(_mapSize.x), 1, file);
		::fread(&_mapSize.y, sizeof(_mapSize.y), 1, file);

		SetMapSize(_mapSize);

		for (int32 y = 0; y < _mapSize.y; y++)
		{
			for (int32 x = 0; x < _mapSize.x; x++)
			{
				int32 value = -1;
				::fread(&value, sizeof(value), 1, file);
				_tiles[y][x].value = value;
			}
		}
		::fclose(file);
	}

	wifstream ifs;
	ifs.open(path);

	ifs >> _mapSize.x >> _mapSize.y;

	SetMapSize(_mapSize);


	for (int32 y = 0; y < _mapSize.y; y++)
	{
		wstring line; ifs >> line;

		for (int32 x = 0; x < _mapSize.x; x++)
		{
			_tiles[y][x].value = line[x] - L'0';
		}
	}

	ifs.close();
}


Tile* TileMap::GetTileAt(Vec2Int pos)
{
	if (pos.x < 0 || pos.x >= _mapSize.x || pos.y < 0 || pos.y >= _mapSize.y)
		return nullptr;

	return &_tiles[pos.y][pos.x];
}

void TileMap::SetMapSize(Vec2Int size)
{
	_mapSize = size;
	_tiles = vector<vector<Tile>>(size.y, vector<Tile>(size.x));

	for (int32 x = 0; x < size.x; x++)
	{
		for (int32 y = 0; y < size.y; y++)
		{
			_tiles[y][x] = Tile{ 0 };
		}
	}
}

void TileMap::SetTileSize(int32 size)
{
	_tileSize = size;
}
