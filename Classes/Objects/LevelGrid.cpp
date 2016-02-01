#include "Objects\LevelGrid.h"
#include "Engine.h"

USING_NS_CC;
using namespace std;

#define GROUND_LAYER_TAGS 0
#define WALL_LAYER_TAGS 1000

#define MAX_SPRITE_NUMBER 12

#define INDEX_FROM_XY(x, y) x * _mapSize.height + y
#define INDEX_FROM_POS(pos) pos.x * _mapSize.height + pos.y
#define POS_FROM_INDEX(i) Pos2(i % _mapSize.width, i / _mapSize.width)


LevelGrid* LevelGrid::createFromFile(const std::string& fileName)
{
	return nullptr;
}

LevelGrid* LevelGrid::createRandom(const Size& mapSize, float chanceToWall, float chanceToHole)
{
	LevelGrid* grid = new (std::nothrow) LevelGrid(mapSize, Size(TILE_SIZE, TILE_SIZE));
	if (grid != nullptr && grid->initialiseRandom(chanceToWall, chanceToHole)) {
		grid->autorelease();
		return grid;
	}
	CC_SAFE_DELETE(grid);
	return nullptr;
}

LevelGrid * LevelGrid::createUsingCA(const Size & mapSize, const CAParams & params, float chanceToWall)
{
	LevelGrid* grid = new (std::nothrow) LevelGrid(mapSize, Size(TILE_SIZE, TILE_SIZE));
	if (grid != nullptr && grid->initialiseUsingCA(params, chanceToWall)) {
		grid->autorelease();
		return grid;
	}
	CC_SAFE_DELETE(grid);
	return nullptr;
}

LevelGrid::LevelGrid()
	: LevelGrid(Size(GRID_MAX_WIDTH, GRID_MAX_HEIGHT), Size(TILE_SIZE, TILE_SIZE))
{ }

LevelGrid::LevelGrid(const Size& mapSize, const Size& tileSize)
	: _mapSize(mapSize), _tileSize(tileSize), _grid(_mapSize.width * _mapSize.height),
	_dynamicPassableLayer(_mapSize.width * _mapSize.height)
{
	setContentSize(Size(_mapSize.width * _tileSize.width, _mapSize.height * _tileSize.height));
	setAnchorPoint(Size(0.5f, 0.5f));

	int spritePack = RandomHelper::random_int(1, MAX_SPRITE_NUMBER);
	_groundSprite = StringUtils::format("ground%d.png", spritePack);
	_wallSprite = StringUtils::format("wall%d.png", spritePack);
}

LevelGrid::~LevelGrid()
{
}

bool LevelGrid::initialiseRandom(float chanceToWall, float chanceToHole)
{
	vector<TileValue> tiles(_grid.size());
	getRandomTiles(tiles, chanceToWall, chanceToHole);
	setTiles(tiles);
	return true;
}

void LevelGrid::getRandomTiles(vector<TileValue>& tiles, float chanceToWall, float chanceToHole) const
{
	for (int x = 0; x < _mapSize.width; x++) {
		for (int y = 0; y < _mapSize.height; y++) {
			if (random(0.0f, 1.0f) < chanceToWall)
				tiles[INDEX_FROM_XY(x, y)] = TileValue::BLOCK;
			else
				tiles[INDEX_FROM_XY(x, y)] = TileValue::GROUND;

			if (random(0.0f, 1.0f) < chanceToHole)
				tiles[INDEX_FROM_XY(x, y)] = TileValue::HOLE;
		}
	}
}

bool LevelGrid::initialiseUsingCA(const CAParams& params, float chanceToHole)
{
	vector<TileValue> tiles(_grid.size());
	getCATiles(tiles, params, chanceToHole);
	setTiles(tiles);
	return true;
}

void LevelGrid::getCATiles(vector<TileValue>& tiles, const CAParams& params, float chanceToHole) const
{
	getRandomTiles(tiles, params.chanceToStartAlive, chanceToHole);

	for (int i = 0; i < params.numberOfSteps; i++)
		tiles = doSimulationStep(tiles, params.deathLimit, params.birthLimit);

	for (int x = 0; x < _mapSize.width; x++) {
		for (int y = 0; y < _mapSize.height; y++) {
			if (random(0.0f, 1.0f) < chanceToHole && tiles[INDEX_FROM_XY(x, y)] == TileValue::GROUND)
				tiles[INDEX_FROM_XY(x, y)] = TileValue::HOLE;
		}
	}
}

vector<TileValue> LevelGrid::doSimulationStep(vector<TileValue>& oldMap, int deathLimit, int birthLimit) const
{
	vector<TileValue> newMap(_mapSize.width * _mapSize.height);
	//Loop over each row and column of the map
	for (int x = 0; x < _mapSize.width; x++) {
		for (int y = 0; y < _mapSize.height; y++) {
			int nbs = countAliveNeighbours(oldMap, x, y);
			//The new value is based on our simulation rules
			//First, if a cell is alive but has too few neighbours, kill it.
			if (oldMap[INDEX_FROM_XY(x, y)] == TileValue::BLOCK) {
				if (nbs < deathLimit)
					newMap[INDEX_FROM_XY(x, y)] = TileValue::GROUND;
				else
					newMap[INDEX_FROM_XY(x, y)] = TileValue::BLOCK;
			} //Otherwise, if the cell is dead now, check if it has the right number of neighbours to be 'born'
			else 
			{
				if (nbs > birthLimit)
					newMap[INDEX_FROM_XY(x, y)] = TileValue::BLOCK;
				else
					newMap[INDEX_FROM_XY(x, y)] = TileValue::GROUND;
			}
		}
	}
	return newMap;
}

//Returns the number of cells in a ring around (x,y) that are alive.
int LevelGrid::countAliveNeighbours(vector<TileValue>& map, int x, int y) const
{
	int count = 0;
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			int neighbour_x = x + i;
			int neighbour_y = y + j;
			//If we're looking at the middle point
			if (i == 0 && j == 0) {
				//Do nothing, we don't want to add ourselves in!
			}
			//In case the index we're looking at it off the edge of the map
			else if (neighbour_x < 0 || neighbour_y < 0 ||
				neighbour_x >= _mapSize.width || neighbour_y >= _mapSize.height) {
				count = count + 1;
			}
			//Otherwise, a normal check of the neighbour
			else if (map[INDEX_FROM_XY(neighbour_x, neighbour_y)] == TileValue::BLOCK) {
				count = count + 1;
			}
		}
	}
	return count;
}

void LevelGrid::updateSprite(int x, int y, TileValue value)
{
	if (value == TileValue::GROUND)
	{
		removeSprite(x, y, WALL_LAYER_TAGS);
		setSprite(x, y, _groundSprite, GROUND_LAYER_ZORDER, GROUND_LAYER_TAGS);
	} 
	else if (value == TileValue::BLOCK)
	{
		setSprite(x, y, _groundSprite, GROUND_LAYER_ZORDER, GROUND_LAYER_TAGS);
		setSprite(x, y, _wallSprite, WALL_LAYER_ZORDER, WALL_LAYER_TAGS);
	}
	else if (value == TileValue::HOLE)
	{
	}
}

void LevelGrid::setSprite(int x, int y, const std::string& spriteName, int zOrder, int tagLayer)
{
	removeSprite(x, y, tagLayer);

	int tag = tagLayer + INDEX_FROM_XY(x, y);

	auto sprite = Sprite::createWithSpriteFrameName(spriteName);
	sprite->setAnchorPoint(Vec2(0, 0));
	sprite->setPosition(Vec2(x, y) * _tileSize.width);
	sprite->setTag(tag);
	this->addChild(sprite, zOrder);
}

void LevelGrid::removeSprite(int x, int y, int tagLayer)
{
	int tag = tagLayer + INDEX_FROM_XY(x, y);
	auto exist = this->getChildByTag(tag);
	if (exist != nullptr)
		this->removeChild(exist);
}

Pos2 LevelGrid::tileCoordForPosition(const Vec2& position) const
{
	return Pos2(position.x / _tileSize.width, position.y / _tileSize.height);
}

Vec2 LevelGrid::positionForTileCoord(const Pos2& tileCoord) const
{
	int x = (tileCoord.x * _tileSize.width) + _tileSize.width / 2;
	int y = (tileCoord.y * _tileSize.height) + _tileSize.height / 2;
	return Vec2(x, y);
}

TileValue LevelGrid::getTile(int x, int y) const
{
	CC_ASSERT(isValidTileCoord(Pos2(x, y)));

	return _grid[INDEX_FROM_XY(x, y)];
}

TileValue LevelGrid::getTile(const Pos2& position) const
{
	return getTile(position.x, position.y);
}

void LevelGrid::setTile(int x, int y, TileValue value)
{
	CC_ASSERT(isValidTileCoord(Pos2(x, y)));

	if (_grid[INDEX_FROM_XY(x, y)] == value)
		return;

	_grid[INDEX_FROM_XY(x, y)] = value;

	updateSprite(x, y, value);
}

void LevelGrid::setTile(const Pos2 & position, TileValue value)
{
	setTile(position.x, position.y, value);
}

void LevelGrid::setTiles(const vector<TileValue>& tiles)
{
	for (size_t y = 0; y < _mapSize.height; y++)
	{
		for (size_t x = 0; x < _mapSize.width; x++)
		{
			setTile(x, y, tiles[INDEX_FROM_XY(x, y)]);
		}
	}
}

vector<Pos2> LevelGrid::findTiles(TileValue value) const
{
	vector<Pos2> result;

	for (size_t y = 0; y < _mapSize.height; y++) {
		for (size_t x = 0; x < _mapSize.width; x++) {
			if (getTile(x, y) == value)
				result.push_back(Pos2(x, y));
		}
	}

	return result;
}

void LevelGrid::setDynamicPassable(const Pos2& tileCoord, PassableValue value)
{
	_dynamicPassableLayer[INDEX_FROM_POS(tileCoord)] = value;

	if (PATHFINDING_DEBUG) {
		auto exist = this->getChildByTag(INDEX_FROM_POS(tileCoord));
		if (exist != nullptr) {
			if (value == PassableValue::PASSABLE)
				exist->setColor(Color3B(255, 255, 255));
			else
				exist->setColor(Color3B(255, 0, 0));
		}
	}
}

bool LevelGrid::isDynamicPassable(const Pos2& tileCoord) const
{
	return _dynamicPassableLayer[INDEX_FROM_POS(tileCoord)] == PassableValue::PASSABLE;
}

bool LevelGrid::isValidTileCoord(const Pos2& tileCoord) const
{
	if (tileCoord.x < 0 || tileCoord.y < 0 || 
		tileCoord.x >= _mapSize.width || tileCoord.y >= _mapSize.height)
		return false;
	else
		return true;
}

bool LevelGrid::isTilePassable(const Pos2& tileCoord) const
{
	auto value = getTile(tileCoord);
	return value != TileValue::BLOCK && value != TileValue::HOLE;
}

PointArray* LevelGrid::walkableAdjacentTilesCoordForTileCoord(const Pos2& tileCoord) const
{
	PointArray* tmp = PointArray::create(8);

	// Top
	Pos2 p(tileCoord.x, tileCoord.y - 1);
	if (isValidTileCoord(p) && isTilePassable(p) && isDynamicPassable(p))
		tmp->addControlPoint((Point)p);

	// Left
	p.set(tileCoord.x - 1, tileCoord.y);
	if (isValidTileCoord(p) && isTilePassable(p) && isDynamicPassable(p))
		tmp->addControlPoint((Point)p);

	// Bottom
	p.set(tileCoord.x, tileCoord.y + 1);
	if (isValidTileCoord(p) && isTilePassable(p) && isDynamicPassable(p))
		tmp->addControlPoint((Point)p);

	// Right
	p.set(tileCoord.x + 1, tileCoord.y);
	if (isValidTileCoord(p) && isTilePassable(p) && isDynamicPassable(p))
		tmp->addControlPoint((Point)p);

	return tmp;
}

void LevelGrid::digPath(const Pos2& posA, const Pos2& posB)
{
	int min = std::min(posA.x, posB.x);
	int max = std::max(posA.x, posB.x);

	for (int x = min; x < max; x++) {
		setTile(x, posA.y, TileValue::GROUND);
		//auto exist = this->getChildByTag(x * _mapSize.height + posA.y);
		//exist->setColor(Color3B(255, 0, 0));
	}

	min = std::min(posA.y, posB.y);
	max = std::max(posA.y, posB.y);

	for (int y = min; y < max; y++) {
		setTile(posB.x, y, TileValue::GROUND);
		//auto exist = this->getChildByTag(posB.x * _mapSize.height + y);
		//exist->setColor(Color3B(255, 0, 0));
	}
}