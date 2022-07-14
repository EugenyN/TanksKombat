#ifndef  _LEVELGRID_H_
#define  _LEVELGRID_H_

#include "Engine.h"

enum class TileValue
{
	HOLE = 0,
	GROUND = 1,
	BLOCK = 2
};

enum class PassableValue
{
	PASSABLE = 0,
	IMPASSABLE = 1,
};

struct CAParams
{
	float chanceToStartAlive = 0.2f;
	int numberOfSteps = 1;
	int deathLimit = 2;
	int birthLimit = 5;
};

class LevelGrid : public cocos2d::Node
{
public:
	static LevelGrid* createFromFile(const std::string& fileName);
	static LevelGrid* createRandom(const cocos2d::Size& mapSize, float chanceToWall, float chanceToHole);
	static LevelGrid* createUsingCA(const cocos2d::Size& mapSize, const CAParams& params, float chanceToWall);

	inline const cocos2d::Size& getMapSize() const { return _mapSize; };
	//inline void setMapSize(const cocos2d::Size& mapSize) { _mapSize = mapSize; };
	inline const cocos2d::Size& getTileSize() const { return _tileSize; };
	//inline void setTileSize(const cocos2d::Size& tileSize) { _tileSize = tileSize; };

	Pos2 tileCoordForPosition(const cocos2d::Vec2& position) const;
	cocos2d::Vec2 positionForTileCoord(const Pos2& tileCoord) const;

	TileValue getTile(int x, int y) const;
	TileValue getTile(const Pos2& position) const;
	void setTile(int x, int y, TileValue value);
	void setTile(const Pos2& position, TileValue value);
	void setTiles(const std::vector<TileValue>& tiles);

	std::vector<Pos2> findTiles(TileValue value) const;

	void setDynamicPassable(const Pos2& tileCoord, PassableValue value);
	bool isDynamicPassable(const Pos2& tileCoord) const;

	bool isTilePassable(const Pos2& tileCoord) const;
	bool isValidTileCoord(const Pos2& tileCoord) const;
	cocos2d::PointArray* walkableAdjacentTilesCoordForTileCoord(const Pos2& tileCoord) const;

	void digPath(const Pos2& posA, const Pos2& posB);

private:
	LevelGrid();
	LevelGrid(const cocos2d::Size& mapSize, const cocos2d::Size& tileSize);
	~LevelGrid() override;

	/** the map size property measured in tiles */
	cocos2d::Size _mapSize;
	/** the tile size property measured in pixels */
	cocos2d::Size _tileSize;
	//
	std::vector<TileValue> _grid;
	std::vector<PassableValue> _dynamicPassableLayer;

	std::string _groundSprite;
	std::string _wallSprite;

	void initialise();
	void initialiseRandom(float chanceToWall, float chanceToHole);
	void getRandomTiles(std::vector<TileValue>& tiles, float chanceToWall, float chanceToHole) const;
	void initialiseUsingCA(const CAParams& params, float chanceToHole);
	void getCATiles(std::vector<TileValue>& tiles, const CAParams& params, float chanceToHole) const;

	void updateSprite(int x, int y, TileValue value);
	void setSprite(int x, int y, const std::string& sprite, LayerZOrder zOrder, int tagLayer);
	void removeSprite(int x, int y, int tagLayer);

	std::vector<TileValue> doSimulationStep(std::vector<TileValue>& oldMap, int deathLimit, int birthLimit) const;
	int countAliveNeighbours(std::vector<TileValue>& map, int x, int y) const;
};

#endif // _LEVELGRID_H_