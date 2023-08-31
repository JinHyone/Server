#pragma once

class GameObject;

struct Tile
{
	// TODO 
	int32 value = 0;
	GameObject* creaturePtr = nullptr;
};

enum TILE_SIZE
{
	TILE_WIDTH = 63,
	TILE_HEIGHT = 43,
	TILE_SIZEX = 48,
	TILE_SIZEY = 48,
};

class TileMap
{
public:
	TileMap();
	virtual ~TileMap();

	void LoadFile(const wstring& path);

	Vec2Int GetMapSize() { return _mapSize; }
	int32 GetTileSize() { return _tileSize; }
	Tile* GetTileAt(Vec2Int pos);
	vector<vector<Tile>>& GetTiles() { return _tiles; }

	void SetMapSize(Vec2Int size);
	void SetTileSize(int32 size);

private:
	Vec2Int _mapSize = {};
	int32 _tileSize = 0;
	vector<vector<Tile>> _tiles;
	// _tiles[y][x]
};

