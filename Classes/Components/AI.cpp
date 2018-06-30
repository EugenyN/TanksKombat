#include "AI.h"
#include "Engine.h"
#include "Pathfinder.h"
#include "Objects\Bonus.h"
#include "Scenes\GameplayScene.h"

USING_NS_CC;

const std::string AI::COMPONENT_NAME = "AI";

AI::AI()
	: _escapePoint(-1, -1)
{
	_name = COMPONENT_NAME;
}

AI::~AI()
{
}

bool AI::init()
{
	if (!Component::init())
		return false;

	auto scene = Engine::getInstance()->getCurrentScene<GameplayScene>();
	scene->getGameObjectRemovedEvent() += CC_CALLBACK_1(AI::onGameObjectRemoved, this);

	return true;
}

void AI::onAdd()
{
	auto delayedState = Sequence::createWithTwoActions(
		DelayTime::create(random(0.0f, 2.0f)),
		CallFunc::create([this]() 
		{
			auto state = RepeatForever::create(
				Sequence::createWithTwoActions(
					DelayTime::create(_aiSpeed),
					CallFunc::create(CC_CALLBACK_0(AI::updateStep, this))
					)
				);
			_owner->runAction(state);
		}));

	_owner->runAction(delayedState);
}

void AI::updateStep()
{
	if (_currentState == States::UNDEFINED)
		makeDecision();

	switch (_currentState)
	{
	case States::PURSUIT:
		pursuitUpdate();
		break;
	case States::ATTACK:
		attackUpdate();
		break;
	case States::ESCAPE:
		escapeUpdate();
		break;
	case States::THINK:
		thinkUpdate();
		break;
	case States::GET_BONUS:
		getBonusUpdate();
		break;
	default:
		break;
	}
}

void AI::makeDecision()
{
	CCLOG("AI::makeDecision()");

	// If enemy on line, is possible to attack.
	//	 - Attack.

	Tank::Direction fireLineDirection;
	Tank* enemyOnLine = nullptr;
	bool onFireLine = isEnemyOnFiringLine(fireLineDirection, enemyOnLine);

	if (onFireLine && isItPossibleToAttackEnemy(enemyOnLine))
	{
		if (isItPossibleToAttackInDirection(fireLineDirection))
		{
			auto owner = dynamic_cast<Tank*>(_owner);
			if (owner->getGridDirection() != fireLineDirection)
				owner->tryMove(fireLineDirection);

			attackStart(fireLineDirection, enemyOnLine);
			return;
		}
	}

	// If there are enemies and ammo
	//	 - Go to optimal enemy.

	auto tank = findNearestTank();
	if (tank != nullptr && isItPossibleToAttackEnemy(tank))
	{
		pursuitStart(tank);
		return;
	}

	// If there is a bonus
	//	 - Go to bonus.

	auto bonus = findNearestBonus();
	if (bonus != nullptr)
	{
		getBonusStart(bonus);
		return;
	}

	// - Else, escape to a random point or to wait

	if (onFireLine || (random(0.0f, 1.0f) < _chanceToEscape))
	{
		Pos2 escapePoint;
		if (findEscapePoint(escapePoint))
			escapeStart(escapePoint);
		else
			thinkStart(); // completely blocked ?
	}
	else
	{
		thinkStart();
	}
}

void AI::onGameObjectRemoved(const GameObject* gameObject)
{
	if (gameObject == _target)
	{
		CCLOG("AI::onGameObjectRemoved()");

		auto owner = dynamic_cast<Tank*>(_owner);
		_target = nullptr;
		owner->stopMoveToward();

		//_owner->runAction(Sequence::create(
		//	DelayTime::create(1.0f),
		//	CallFunc::create(CC_CALLBACK_0(AI::makeDecision, this)),
		//	nullptr
		//));

		_currentState = States::UNDEFINED;
	}
}

bool AI::isItPossibleToAttack()
{
	auto owner = dynamic_cast<Tank*>(_owner);
	return owner->getAmmo() > 0 && !owner->isGhostState() && !owner->isFallIntoHoleState();
}

bool AI::isItPossibleToAttackEnemy(const Tank* enemy)
{
	auto owner = dynamic_cast<Tank*>(_owner);
	if (!isItPossibleToAttack())
		return false;

	return !enemy->isGhostState() && !enemy->isFallIntoHoleState();
}

bool AI::isItPossibleToAttackInDirection(Tank::Direction direction)
{
	auto owner = dynamic_cast<Tank*>(_owner);

	if (owner->getGridDirection() == direction)
		return true;

	auto newPos = owner->getGridPosition() + owner->directionToOffset(direction);
	auto scene = Engine::getInstance()->getCurrentScene<GameplayScene>();
	return scene->getGrid()->isTilePassable(newPos);
}

void AI::pursuitStart(Tank* target)
{
	_currentState = States::PURSUIT;
	_target = target;

	CCLOG("AI::pursuitStart()");
}

void AI::pursuitUpdate()
{
	auto owner = dynamic_cast<Tank*>(_owner);

	if (!isItPossibleToAttackEnemy(dynamic_cast<Tank*>(_target)))
	{
		_target = nullptr;
		owner->stopMoveToward();
		makeDecision();
		return;
	}

	Tank::Direction fireLineDirection;
	Tank* enemyOnLine = nullptr;
	if (isEnemyOnFiringLine(fireLineDirection, enemyOnLine) && isItPossibleToAttackEnemy(enemyOnLine))
	{
		if (isItPossibleToAttackInDirection(fireLineDirection))
		{
			_target = nullptr;
			owner->stopMoveToward();
			makeDecision();
			return;
		}
	}

	if (_target->getGridPosition() != owner->getMoveTowardTarget())
		owner->moveToward(_target->getGridPosition(), true);
}

void AI::attackStart(Tank::Direction fireLineDirection, Tank* enemyOnLine)
{
	_currentState = States::ATTACK;
	CCLOG("AI::attackStart()");

	auto owner = dynamic_cast<Tank*>(_owner);
	owner->tryShoot();
}

void AI::attackUpdate()
{
	auto owner = dynamic_cast<Tank*>(_owner);
	if (!owner->isShooting())
	{
		makeDecision();
	}
}

void AI::escapeStart(Pos2 escapePoint)
{
	_currentState = States::ESCAPE;
	_escapePoint = escapePoint;

	CCLOG("AI::escapeStart()");
}

void AI::escapeUpdate()
{
	auto owner = dynamic_cast<Tank*>(_owner);

	Tank::Direction fireLineDirection;
	Tank* enemyOnLine = nullptr;
	if (isEnemyOnFiringLine(fireLineDirection, enemyOnLine) && isItPossibleToAttackEnemy(enemyOnLine))
	{
		if (isItPossibleToAttackInDirection(fireLineDirection))
		{
			_escapePoint = Pos2(-1, -1);
			owner->stopMoveToward();
			makeDecision();
			return;
		}
	}

	if (owner->getGridPosition() == _escapePoint) {
		_escapePoint = Pos2(-1, -1);
		owner->stopMoveToward();
		makeDecision();
		return;
	}

	if (!owner->moveToward(_escapePoint)) {
		_escapePoint = Pos2(-1, -1);
		owner->stopMoveToward();
		makeDecision();
		return;
	}
}

void AI::thinkStart()
{
	_currentState = States::THINK;
	CCLOG("AI::thinkStart()");

	auto owner = dynamic_cast<Tank*>(_owner);

	float time = _thinkTime / 5;

	auto thinkSequence = Sequence::create(
		DelayTime::create(time),
		CallFunc::create([owner]() { owner->setGridDirection(static_cast<GameObject::Direction>(random(0, 3))); }),
		DelayTime::create(time),
		CallFunc::create([owner]() { owner->setGridDirection(static_cast<GameObject::Direction>(random(0, 3))); }),
		DelayTime::create(time),
		CallFunc::create([owner]() { owner->setGridDirection(static_cast<GameObject::Direction>(random(0, 3))); }),
		DelayTime::create(time),
		CallFunc::create([owner]() { owner->setGridDirection(static_cast<GameObject::Direction>(random(0, 3))); }),
		DelayTime::create(time),
		CallFunc::create(CC_CALLBACK_0(AI::makeDecision, this)),
		nullptr
		);

	_owner->runAction(thinkSequence);
}

void AI::thinkUpdate()
{
	Tank::Direction fireLineDirection;
	Tank* enemyOnLine = nullptr;
	if (isEnemyOnFiringLine(fireLineDirection, enemyOnLine) && isItPossibleToAttackEnemy(enemyOnLine))
	{
		if (isItPossibleToAttackInDirection(fireLineDirection))
		{
			// stop thinking
			makeDecision();
			return;
		}
	}
}

void AI::getBonusStart(Bonus* target)
{
	_currentState = States::GET_BONUS;
	_target = target;

	CCLOG("AI::getBonusStart()");
}

void AI::getBonusUpdate()
{
	auto owner = dynamic_cast<Tank*>(_owner);

	Tank::Direction fireLineDirection;
	Tank* enemyOnLine = nullptr;
	if (isEnemyOnFiringLine(fireLineDirection, enemyOnLine) && isItPossibleToAttackEnemy(enemyOnLine))
	{
		_target = nullptr;
		owner->stopMoveToward();
		makeDecision();
		return;
	}

	owner->moveToward(_target->getGridPosition());
}

Tank* AI::findNearestTank() const
{
	Tank* nearest = nullptr;

	const auto scene = Engine::getInstance()->getCurrentScene<GameplayScene>();
	if (scene == nullptr)
		return nullptr;

	auto owner = dynamic_cast<Tank*>(_owner);
	auto pf = dynamic_cast<Pathfinder*>(owner->getComponent(Pathfinder::COMPONENT_NAME));
	if (pf == nullptr)
		return nullptr;

	int shortestPathSize = 1000;
	for (auto obj : scene->getGameObjects())
	{
		auto tank = dynamic_cast<Tank*>(obj);
		if (tank != nullptr && tank != _owner)
		{
			if (tank->isGhostState())
				continue;

			if (tank->isFallIntoHoleState())
				continue;

			const auto& shortestPath = pf->getShortestPath(
				owner->getGridPosition(), tank->getGridPosition(), true);
			if (shortestPath.size() != 0 && shortestPath.size() < shortestPathSize) {
				shortestPathSize = shortestPath.size();
				nearest = tank;
			}
		}
	}

	return nearest;
}

Bonus* AI::findNearestBonus() const
{
	Bonus* nearest = nullptr;

	const auto scene = Engine::getInstance()->getCurrentScene<GameplayScene>();
	if (scene == nullptr)
		return nullptr;

	auto owner = dynamic_cast<Tank*>(_owner);
	auto pf = dynamic_cast<Pathfinder*>(owner->getComponent(Pathfinder::COMPONENT_NAME));
	if (pf == nullptr)
		return nullptr;

	int shortestPathSize = 1000;
	for (auto obj : scene->getGameObjects())
	{
		auto bonus = dynamic_cast<Bonus*>(obj);
		if (bonus != nullptr)
		{
			const auto& shortestPath = pf->getShortestPath(owner->getGridPosition(), bonus->getGridPosition());
			if (shortestPath.size() != 0 && shortestPath.size() < shortestPathSize) {
				shortestPathSize = shortestPath.size();
				nearest = bonus;
			}
		}
	}

	return nearest;
}

bool AI::findEscapePoint(/*out*/ Pos2& point) const
{
	auto scene = Engine::getInstance()->getCurrentScene<GameplayScene>();
	if (scene == nullptr)
		return false;

	auto owner = dynamic_cast<Tank*>(_owner);
	return scene->findRandomEmptyTile(point, owner->getGridPosition());
}

bool AI::isEnemyOnFiringLine(/*out*/ GameObject::Direction& fireLineDirection, /*out*/ Tank*& enemyOnLine) const
{
	const auto scene = Engine::getInstance()->getCurrentScene<GameplayScene>();
	if (scene == nullptr)
		return false;

	auto owner = dynamic_cast<Tank*>(_owner);
	auto ownerPos = owner->getGridPosition();

	for (auto obj : scene->getGameObjects())
	{
		auto enemy = dynamic_cast<Tank*>(obj);
		if (enemy != nullptr && enemy != _owner)
		{
			auto enemyPos = enemy->getGridPosition();
			if (enemyPos.x == ownerPos.x) {
				auto dir = enemyPos.y > ownerPos.y ? Tank::Direction::UP : Tank::Direction::DOWN;
				if (isEnemyOnFiringLine(dir, ownerPos, enemyPos)) {
					fireLineDirection = dir;
					enemyOnLine = enemy;
					return true;
				}
			}

			if (enemyPos.y == ownerPos.y) {
				auto dir = enemyPos.x > ownerPos.x ? Tank::Direction::RIGHT : Tank::Direction::LEFT;
				if (isEnemyOnFiringLine(dir, ownerPos, enemyPos)) {
					fireLineDirection = dir;
					enemyOnLine = enemy;
					return true;
				}
			}
		}
	}

	enemyOnLine = nullptr;
	return false;
}

bool AI::isEnemyOnFiringLine(GameObject::Direction direction, const Pos2& ownerPos, const Pos2& enemyPos) const
{
	const auto scene = Engine::getInstance()->getCurrentScene<GameplayScene>();
	auto grid = scene->getGrid();

	if (direction == Tank::Direction::UP)
	{
		for (int y = ownerPos.y + 1; y < enemyPos.y; y++) {
			if (grid->getTile(ownerPos.x, y) == TileValue::BLOCK)
				return false;
		}
		return true;
	}

	if (direction == Tank::Direction::DOWN)
	{
		for (int y = ownerPos.y - 1; y > enemyPos.y; y--) {
			if (grid->getTile(ownerPos.x, y) == TileValue::BLOCK)
				return false;
		}
		return true;
	}

	if (direction == Tank::Direction::RIGHT)
	{
		for (int x = ownerPos.x + 1; x < enemyPos.x; x++) {
			if (grid->getTile(x, ownerPos.y) == TileValue::BLOCK)
				return false;
		}
		return true;
	}

	if (direction == Tank::Direction::LEFT)
	{
		for (int x = ownerPos.x - 1; x > enemyPos.x; x--) {
			if (grid->getTile(x, ownerPos.y) == TileValue::BLOCK)
				return false;
		}
		return true;
	}

	return false;
}
