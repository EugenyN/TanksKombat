#ifndef  _PATHFINDER_H_
#define  _PATHFINDER_H_

#include "Engine.h"

class ShortestPathStep : public cocos2d::Ref
{
public:
	ShortestPathStep();
	~ShortestPathStep();

	static ShortestPathStep *createWithPosition(const Pos2 &pos);
	bool initWithPosition(const Pos2 &pos);

	int getFScore() const;
	bool isEqual(const ShortestPathStep *other) const;
	std::string getDescription() const;

	CC_SYNTHESIZE(Pos2, _position, Position);
	CC_SYNTHESIZE(int, _gScore, GScore);
	CC_SYNTHESIZE(int, _hScore, HScore);
	CC_SYNTHESIZE(ShortestPathStep*, _parent, Parent);
};

class Pathfinder : public cocos2d::Component
{
public:
	const static std::string COMPONENT_NAME;

	CREATE_FUNC(Pathfinder);

	bool isPathExist(const Pos2& from, const Pos2& to, bool getUpClose = false);
	const cocos2d::Vector<ShortestPathStep*>& getShortestPath(const Pos2& from, const Pos2& to, bool getUpClose = false);
private:
	cocos2d::Vector<ShortestPathStep*> _spOpenSteps;
	cocos2d::Vector<ShortestPathStep*> _spClosedSteps;
	cocos2d::Vector<ShortestPathStep*> _moveTowardPath;

	Pathfinder();
	virtual ~Pathfinder();

	void insertInOpenSteps(ShortestPathStep *step);
	int computeHScoreFromCoordToCoord(const Pos2& from, const Pos2& to);
	int costToMoveFromStepToAdjacentStep(const ShortestPathStep* fromStep, const ShortestPathStep* toStep);
	ssize_t getStepIndex(const cocos2d::Vector<ShortestPathStep*>& steps, const ShortestPathStep* step);

	ShortestPathStep* constructPath(const Pos2& from, const Pos2& to, bool getUpClose = false);
};


#endif // _PATHFINDER_H_