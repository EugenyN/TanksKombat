#include "Pathfinder.h"
#include "Engine.h"
#include "Objects\LevelGrid.h"
#include "Objects\GameObject.h"
#include "Scenes\GameplayScene.h"

USING_NS_CC;

const std::string Pathfinder::COMPONENT_NAME = "Pathfinder";


ShortestPathStep::ShortestPathStep() :
	_position(Pos2(0, 0)),
	_gScore(0),
	_hScore(0),
	_parent(nullptr)
{
}

ShortestPathStep::~ShortestPathStep()
{
}

ShortestPathStep* ShortestPathStep::createWithPosition(const Pos2& pos)
{
	ShortestPathStep *pRet = new ShortestPathStep();
	if (pRet && pRet->initWithPosition(pos))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

bool ShortestPathStep::initWithPosition(const Pos2& pos)
{
	bool bRet = false;
	do
	{
		this->setPosition(pos);

		bRet = true;
	} while (0);

	return bRet;
}

int ShortestPathStep::getFScore() const
{
	return this->getGScore() + this->getHScore();
}

bool ShortestPathStep::isEqual(const ShortestPathStep* other) const
{
	return this->getPosition() == other->getPosition();
}

std::string ShortestPathStep::getDescription() const
{
	return StringUtils::format("pos=[%d;%d]  g=%d  h=%d  f=%d",
		this->getPosition().x, this->getPosition().y,
		this->getGScore(), this->getHScore(), this->getFScore());
}


///////////////////////////////////////////////////////////////////////////////////


Pathfinder::Pathfinder()
{
	_name = COMPONENT_NAME;
}

Pathfinder::~Pathfinder()
{
}

bool Pathfinder::isPathExist(const Pos2& from, const Pos2& to, bool getUpClose)
{
	const auto& path = getShortestPath(from, to, getUpClose);
	return path.size() != 0;
}

const Vector<ShortestPathStep*>& Pathfinder::getShortestPath(const Pos2& from, const Pos2& to, bool getUpClose)
{
	_moveTowardPath.clear();

	auto step = constructPath(from, to, getUpClose);
	if (step == nullptr)
		return _moveTowardPath;

	do
	{
		if (step->getParent() != nullptr)
			_moveTowardPath.insert(0, step);
		step = step->getParent();
	} while (step);

	return _moveTowardPath;
}

ShortestPathStep* Pathfinder::constructPath(const Pos2& from, const Pos2& to, bool getUpClose)
{
	const auto scene = Engine::getInstance()->getCurrentScene<GameplayScene>();
	if (scene == nullptr)
		return nullptr;

	if (from == to)
	{
		CCLOG("You're already there!");
		return nullptr;
	}

	if (!scene->getGrid()->isValidTileCoord(to) || !scene->getGrid()->isTilePassable(to))
	{
		CCLOG("Tile unwalkable!");
		return nullptr;
	}

	// CCLOG("From: %d, %d - To: %d, %d", from.x, from.y, to.x, to.y);

	_spOpenSteps.clear();
	_spClosedSteps.clear();

	this->insertInOpenSteps(ShortestPathStep::createWithPosition(from));

	do
	{
		auto currentStep = _spOpenSteps.at(0);
		_spClosedSteps.pushBack(currentStep);
		_spOpenSteps.erase(0);

		bool pathFound = getUpClose ?
			computeHScoreFromCoordToCoord(currentStep->getPosition(), to) == 1 :
			currentStep->getPosition() == to;

		if (pathFound)
		{
			_spOpenSteps.clear();
			_spClosedSteps.clear();
			return currentStep;
			break;
		}

		PointArray* adjSteps = scene->getGrid()->walkableAdjacentTilesCoordForTileCoord(currentStep->getPosition());
		for (ssize_t i = 0; i < adjSteps->count(); ++i)
		{
			auto step = ShortestPathStep::createWithPosition(adjSteps->getControlPointAtIndex(i));

			if (this->getStepIndex(_spClosedSteps, step) != -1)
				continue;

			int moveCost = this->costToMoveFromStepToAdjacentStep(currentStep, step);

			ssize_t index = this->getStepIndex(_spOpenSteps, step);

			if (index == -1)
			{
				step->setParent(currentStep);
				step->setGScore(currentStep->getGScore() + moveCost);
				step->setHScore(this->computeHScoreFromCoordToCoord(step->getPosition(), to));
				this->insertInOpenSteps(step);
			}
			else
			{
				step = _spOpenSteps.at(index);
				if ((currentStep->getGScore() + moveCost) < step->getGScore())
				{
					step->setGScore(currentStep->getGScore() + moveCost);
					step->retain();
					_spOpenSteps.erase(index);
					this->insertInOpenSteps(step);
					step->release();
				}
			}
		}
	} while (_spOpenSteps.size() > 0);

	return nullptr;
}

void Pathfinder::insertInOpenSteps(ShortestPathStep* step)
{
	int stepFScore = step->getFScore();
	ssize_t count = _spOpenSteps.size();
	ssize_t i = 0;
	for (; i < count; ++i) {
		if (stepFScore <= _spOpenSteps.at(i)->getFScore())
			break;
	}
	_spOpenSteps.insert(i, step);
}

int Pathfinder::computeHScoreFromCoordToCoord(const Pos2& from, const Pos2& to)
{
	return abs(to.x - from.x) + abs(to.y - from.y);
}

int Pathfinder::costToMoveFromStepToAdjacentStep(const ShortestPathStep* fromStep, const ShortestPathStep* toStep)
{
	// because we can't move diagonally and because terrain is just walkable or unwalkable the cost is always the same.
	return 1;

	//return ((fromStep->getPosition().x != toStep->getPosition().x)
	//	&& (fromStep->getPosition().y != toStep->getPosition().y)) ? 14 : 10;
}

ssize_t Pathfinder::getStepIndex(const Vector<ShortestPathStep *>& steps, const ShortestPathStep* step)
{
	for (ssize_t i = 0; i < steps.size(); ++i) {
		if (steps.at(i)->isEqual(step))
			return i;
	}
	return -1;
}
