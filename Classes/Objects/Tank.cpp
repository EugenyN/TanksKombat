#include "Tank.h"
#include "Projectile.h"
#include "Engine.h"
#include "Objects\Explosion.h"
#include "Objects\Bonus.h"
#include "Scenes\GameplayScene.h"
#include "Components\Pathfinder.h"
#include "Components\AI.h"

USING_NS_CC;

#define SHOT_STATE_TAG 10
#define GHOST_STATE_TAG 11
#define FALL_INTO_HOLE_STATE_TAG 12
#define MOVE_TOWARD_STATE_TAG 13


Tank::Tank() 
	: Tank(Team::BLUE, 5, 20)
{
}

Tank::Tank(Team team, int lives, int ammo)
	: GameObject(Type::TANK), 
	_team(team), _lives(lives), _ammo(ammo),
	_shotState(nullptr), _shot(nullptr), _moveTowardState(nullptr)
{
	_isPassable = false;
}

Tank::~Tank()
{
	for (auto anim : _idleStates)
		CC_SAFE_RELEASE_NULL(anim.second);
	_idleStates.clear();

	CC_SAFE_RELEASE_NULL(_moveTowardState);
	CC_SAFE_RELEASE_NULL(_shotState);
	CC_SAFE_RELEASE_NULL(_shot);
}

Team Tank::getTeam() const
{
	return _team;
}

void Tank::setTeam(Team team)
{
	_team = team;

	if (_sprite != nullptr) {
		//_sprite->setBlendFunc(BlendFunc::ADDITIVE);
		_sprite->setColor(GET_TEAM_COLOR3B(_team));
	}
}

int Tank::getAmmo() const
{
	return _ammo;
}

void Tank::setAmmo(int ammo)
{
	if (_ammo == ammo)
		return;

	_ammo = ammo;

	if (_ammo > 99)
		_ammo = 99;

	auto scene = _engine->getCurrentScene<GameplayScene>();
	if (scene != nullptr)
		scene->updateStatusLabel();
}

int Tank::getLives() const
{
	return _lives;
}

void Tank::setLives(int lives)
{
	if (_lives == lives)
		return;

	_lives = lives;

	if (_lives > 99)
		_lives = 99;

	auto scene = _engine->getCurrentScene<GameplayScene>();
	if (scene != nullptr)
		scene->updateStatusLabel();
}

bool Tank::init()
{
	GameObject::init();

	// main sprite

	_sprite = Sprite::createWithSpriteFrameName("u_tank_0.png");
	//_sprite->setColor(INT2COLOR3B((int)_team));

	if (_idleStates[Direction::UP] == nullptr)
		_idleStates[Direction::UP] = createIdleAction(Direction::UP, "u_tank_%d.png");
	if (_idleStates[Direction::DOWN] == nullptr)
		_idleStates[Direction::DOWN] = createIdleAction(Direction::DOWN, "d_tank_%d.png");
	if (_idleStates[Direction::LEFT] == nullptr)
		_idleStates[Direction::LEFT] = createIdleAction(Direction::LEFT, "l_tank_%d.png");
	if (_idleStates[Direction::RIGHT] == nullptr)
		_idleStates[Direction::RIGHT] = createIdleAction(Direction::RIGHT, "r_tank_%d.png");


	this->addChild(_sprite);

	// shot object

	_shot = Sprite::createWithSpriteFrameName("shot.png");
	_shot->retain();

	// pathfinder

	addComponent(Pathfinder::create());

	//

	startIdleState();

	return true;
}

Action* Tank::createIdleAction(Direction direction, const char* frame)
{
	const float IDLE_ANIMATION_TIME = 0.2f;

	Vector<SpriteFrame*> animFrames(2);
	auto spritecache = SpriteFrameCache::getInstance();
	for (int i = 0; i <= 1; ++i)
		animFrames.pushBack(spritecache->getSpriteFrameByName(StringUtils::format(frame, i)));

	auto action = RepeatForever::create(Animate::create(
		Animation::createWithSpriteFrames(animFrames, IDLE_ANIMATION_TIME)));
	action->setTag((int)direction);
	action->retain();
	return action;
}

bool Tank::tryMove(GameObject::Direction direction)
{
	auto scene = _engine->getCurrentScene<GameplayScene>();
	if (scene == nullptr)
		return false;

	if (isFallIntoHoleState())
		return false;

	auto newPos = getGridPosition() + directionToOffset(direction);
	if (!scene->getGrid()->isValidTileCoord(newPos)) {
		setGridDirection(direction);
		return false;
	}

	auto obj = scene->findGameObject(newPos);

	if (obj != nullptr) {
		if (onCollidedWithGameObject(direction, obj)) {
			setGridDirection(direction);
			return false;
		}
	}

	auto value = scene->getGrid()->getTile(newPos);
	if (onCollidedWithBlock(direction, value)) {
		setGridDirection(direction);
		return false;
	}

	move(direction);

	if (obj != nullptr)
		onPostCollidedWithGameObject(obj);

	return true;
}

bool Tank::moveToward(const Pos2 & target, bool getUpClose)
{
	return startMoveTowardState(target, getUpClose);
}

void Tank::stopMoveToward()
{
	stopMoveTowardState();
}

bool Tank::onCollidedWithBlock(Direction direction, const TileValue& value)
{
	if (value == TileValue::BLOCK)
		return true;

	if (value == TileValue::HOLE) {
		startFallIntoHoleState();
		return false;
	}
	return false;
}

bool Tank::onCollidedWithGameObject(Direction direction, GameObject* obj)
{
	auto tank = dynamic_cast<Tank*>(obj);
	if (tank != nullptr) {
		if (tank->isFallIntoHoleState())
			return false;
		return !tank->tryMove(direction);
	}

	return false;
}

void Tank::onPostCollidedWithGameObject(GameObject * obj)
{
	auto bonus = dynamic_cast<Bonus*>(obj);
	if (bonus != nullptr) {
		bonus->take();
		setAmmo(getAmmo() + _engine->GameMode.ammoInBonus);
		return;
	}

	auto proj = dynamic_cast<Projectile*>(obj);
	if (proj != nullptr) {
		if (!isGhostState() && !isFallIntoHoleState()) {
			damage();
			proj->remove();
			return;
		}
	}
}

void Tank::move(Direction direction)
{
	_prevPosition = getGridPosition();
	setGridPosition(getGridPosition() + directionToOffset(direction));
	setGridDirection(direction);
}

bool Tank::tryShoot()
{
	auto scene = _engine->getCurrentScene<GameplayScene>();
	if (scene == nullptr)
		return false;

	if (isFallIntoHoleState())
		return false;

	if (isGhostState()) {
		_engine->playSound("out_of_ammo");
		return false;
	}

	if (_ammo <= 0) {
		_engine->playSound("out_of_ammo");
		return false;
	}

	startShotState();

	setAmmo(getAmmo() - 1);

	auto pos = getGridPosition() + directionToOffset(getGridDirection());
	if (!scene->getGrid()->isValidTileCoord(pos))
		return false;

	auto value = scene->getGrid()->getTile(pos);

	if (value != TileValue::BLOCK)
	{
		auto proj = Projectile::create();
		proj->setGridPosition(getGridPosition());
		proj->setGridDirection(getGridDirection());
		scene->addGameObject(proj);
		return true;
	}

	return false;
}

void Tank::doAction(TankAction action)
{
	switch (action)
	{
	case TankAction::MOVE_LEFT:
		tryMove(GameObject::Direction::LEFT);
		break;
	case TankAction::MOVE_RIGHT:
		tryMove(GameObject::Direction::RIGHT);
		break;
	case TankAction::MOVE_UP:
		tryMove(GameObject::Direction::UP);
		break;
	case TankAction::MOVE_DOWN:
		tryMove(GameObject::Direction::DOWN);
		break;
	case TankAction::FIRE:
		tryShoot();
		break;
	}
}

void Tank::setGridDirection(Direction direction)
{
	// ignoring default behavior
	// GameObject::setDirection(direction);

	_direction = direction;

	startIdleState();
}

void Tank::startIdleState()
{
	// action already active
	if (_sprite->getActionByTag((int)_direction) != nullptr)
		return;

	stopShotState();
	stopIdleState();
	_sprite->runAction(_idleStates[_direction]);
}

void Tank::stopIdleState()
{
	for (auto action : _idleStates) {
		if (_sprite->getActionByTag(action.second->getTag()) != nullptr)
			_sprite->stopAction(action.second);
	}
}


bool Tank::isShooting() const
{
	if (_shotState == nullptr)
		return false;
	return _sprite->getActionByTag(SHOT_STATE_TAG) != nullptr;
}

void Tank::startShotState()
{
	const float SHOT_ANIMATION_TIME = 0.2f;

	if (isShooting())
		_sprite->stopAction(_shotState);


	if (_shotState == nullptr) {
		_shotState = Sequence::create
			(
				CallFunc::create(CC_CALLBACK_0(Tank::shotAnimationStarted, this)),
				DelayTime::create(SHOT_ANIMATION_TIME),
				CallFunc::create(CC_CALLBACK_0(Tank::shotAnimationFinished, this)),
				nullptr
			);
		_shotState->setTag(SHOT_STATE_TAG);
		_shotState->retain();
	}

	stopIdleState();
	_sprite->runAction(_shotState);

	Engine::getInstance()->playSound("tank_shot");
}

void Tank::stopShotState()
{
	if (!isShooting())
		return;

	_sprite->stopAction(_shotState);

	this->removeChild(_shot);
}

void Tank::shotAnimationStarted()
{
	if (_shot->getParent() == nullptr)
		this->addChild(_shot, 100);

	GameObject::setGridPosition(_shot, getGridPosition());

	// adjust shot position.
	Vec2 offset(0, 0);

	switch (_direction)
	{
	case GameObject::Direction::UP:
		offset = Vec2(0, 10) * 2;
		break;
	case GameObject::Direction::DOWN:
		offset = Vec2(0, -6) * 2;
		break;
	case GameObject::Direction::LEFT:
		offset = Vec2(-10, 1) * 2;
		break;
	case GameObject::Direction::RIGHT:
		offset = Vec2(10, 1) * 2;
		break;
	default:
		break;
	}

	_shot->setPosition(getSprite()->getPosition() + offset);
	GameObject::setGridDirection(_shot, getGridDirection());

	switch (_direction)
	{
	case GameObject::Direction::UP:
		_sprite->setSpriteFrame("u_tank_s.png");
		break;
	case GameObject::Direction::DOWN:
		_sprite->setSpriteFrame("d_tank_s.png");
		break;
	case GameObject::Direction::LEFT:
		_sprite->setSpriteFrame("l_tank_s.png");
		break;
	case GameObject::Direction::RIGHT:
		_sprite->setSpriteFrame("r_tank_s.png");
		break;
	default:
		break;
	}
}

void Tank::shotAnimationFinished()
{
	this->removeChild(_shot);

	startIdleState();
}

void Tank::startGhostState()
{
	float ghostTime = _engine->GameMode.ghostTime;
	if (ghostTime == 0)
		return;
	auto state = Blink::create(ghostTime, ghostTime * 3);
	state->setTag(GHOST_STATE_TAG);
	_sprite->runAction(state);
}

void Tank::stopGhostState()
{
	_sprite->stopActionByTag(GHOST_STATE_TAG);
	_sprite->setVisible(true);
}

bool Tank::isGhostState() const
{
	return _sprite->getActionByTag(GHOST_STATE_TAG) != nullptr;
}

bool Tank::isFallIntoHoleState() const
{
	return _sprite->getActionByTag(FALL_INTO_HOLE_STATE_TAG) != nullptr;
}

bool Tank::isAIEnabled()
{
	return getComponent(AI::COMPONENT_NAME) != nullptr;
}

void Tank::setAIEnabled(bool enabled)
{
	if (enabled == isAIEnabled())
		return;

	if (enabled)
		addComponent(AI::create());
	else
		removeComponent(AI::COMPONENT_NAME);
}

void Tank::startFallIntoHoleState()
{
	if (isGhostState())
		stopGhostState();

	Vector<FiniteTimeAction*> rotation;

	rotation.pushBack(RotateBy::create(0, 45.0f));
	rotation.pushBack(DelayTime::create(0.1f));
	rotation.pushBack(RotateBy::create(0, 45.0f));
	rotation.pushBack(DelayTime::create(0.1f));
	rotation.pushBack(RotateBy::create(0, 45.0f));
	rotation.pushBack(DelayTime::create(0.1f));
	rotation.pushBack(RotateBy::create(0, 45.0f));
	rotation.pushBack(DelayTime::create(0.1f));
	rotation.pushBack(RotateBy::create(0, 45.0f));
	rotation.pushBack(DelayTime::create(0.1f));
	rotation.pushBack(RotateBy::create(0, 45.0f));
	rotation.pushBack(DelayTime::create(0.1f));
	rotation.pushBack(RotateBy::create(0, 45.0f));
	rotation.pushBack(DelayTime::create(0.1f));
	rotation.pushBack(CallFunc::create([this]() { _sprite->setVisible(false); }));

	auto state = Sequence::create
		(
			DelayTime::create(1.0f),
			Spawn::create
			(
				Sequence::create(rotation),
				CallFunc::create([]() { Engine::getInstance()->playSound("tank_fall"); }),
				ScaleBy::create(1.0f, 1/10.0f),
				nullptr
			),
			CallFunc::create([this]() { Tank::damage(false); }),
			DelayTime::create(1.0f),
			CallFunc::create([this]() { respawnIntoPosition(_prevPosition); }),
			nullptr
		);

	state->setTag(FALL_INTO_HOLE_STATE_TAG);
	_sprite->runAction(state);
}

bool Tank::startMoveTowardState(const Pos2 & target, bool getUpClose)
{
	auto pf = dynamic_cast<Pathfinder*>(getComponent(Pathfinder::COMPONENT_NAME));
	if (pf == nullptr)
		return false;

	stopMoveTowardState();

	_moveTowardPath = pf->getShortestPath(this->getGridPosition(), target, getUpClose);
	if (_moveTowardPath.size() == 0)
		return false;

	if (PATHFINDING_DEBUG)
		addMoveTowardDebugPath();

	if (_moveTowardState == nullptr) {
		_moveTowardState = RepeatForever::create(
			Sequence::create(
				CallFunc::create(CC_CALLBACK_0(Tank::updateMoveTowardState, this)),
				DelayTime::create(_moveTowardStepDelay),
				nullptr
			)
		);

		_moveTowardState->setTag(MOVE_TOWARD_STATE_TAG);
		_moveTowardState->retain();
	}

	this->runAction(_moveTowardState);
	return true;
}

void Tank::addMoveTowardDebugPath()
{
	if (!PATHFINDING_DEBUG)
		return;

	auto scene = _engine->getCurrentScene<GameplayScene>();
	if (_debugPathLine != nullptr)
		removeChild(_debugPathLine);

	_debugPathLine = DrawNode::create();

	_debugPathLine->drawLine(
		scene->getGrid()->positionForTileCoord(this->getGridPosition()),
		scene->getGrid()->positionForTileCoord(_moveTowardPath.at(0)->getPosition()),
		Color4F(GET_TEAM_COLOR3B(_team)));

	for (int i = 0; i < _moveTowardPath.size() - 1; i++)
	{
		_debugPathLine->drawLine(
			scene->getGrid()->positionForTileCoord(_moveTowardPath.at(i)->getPosition()),
			scene->getGrid()->positionForTileCoord(_moveTowardPath.at(i + 1)->getPosition()),
			Color4F(GET_TEAM_COLOR3B(_team)));
	}
	addChild(_debugPathLine, 100);
}

void Tank::stopMoveTowardState()
{
	this->stopActionByTag(MOVE_TOWARD_STATE_TAG);
}

bool Tank::isMoveTowardState()
{
	return this->getActionByTag(MOVE_TOWARD_STATE_TAG) != nullptr;
}

Pos2 Tank::getMoveTowardTarget() const
{
	if (_moveTowardPath.size() == 0)
		return Pos2(-1, -1);
	return _moveTowardPath.back()->getPosition();
}

void Tank::updateMoveTowardState()
{
	CCASSERT(_moveTowardPath.size() > 0, "_moveTowardPath.size() == 0");

	if (getGridPosition() != _moveTowardPath.at(0)->getPosition()) {
		tryMove(getDirectionFromOffset(getGridPosition(), _moveTowardPath.at(0)->getPosition()));

		_moveTowardPath.erase(0);
	}

	if (_moveTowardPath.size() == 0)
		stopMoveTowardState();
}

void Tank::respawnIntoPosition(Pos2 position)
{
	setGridPosition(position);
	_sprite->setScale(1.0f);
	_sprite->setRotation(0.0F);
	_sprite->setVisible(true);
	_sprite->stopActionByTag(FALL_INTO_HOLE_STATE_TAG);

	startGhostState();

	_engine->playSound("tank_respawn");
}

void Tank::damage(bool ghostState)
{
	CCASSERT(getLives() > 0, "getLives() <= 0");

	setLives(getLives() - 1);

	Explosion::createFromPool(getGridPosition());

	if (ghostState)
		startGhostState();

	_engine->playSound("tank_damage");

	if (_lives == 0)
		_engine->playSound("tank_kill");

	if (_lives == 0)
		kill();
}

void Tank::kill()
{
	auto scene = _engine->getCurrentScene<GameplayScene>();
	if (scene != nullptr) {
		scene->removeGameObject(this);
		scene->onTankKilled(this);
	}
}

Tank::Direction Tank::getDirectionFromOffset(Pos2 from, Pos2 to)
{
	CCASSERT(to != from, "There is no direction for null offset!");

	if (to.x > from.x)
		return Direction::RIGHT;
	if (to.x < from.x)
		return Direction::LEFT;
	if (to.y > from.y)
		return Direction::UP;
	if (to.y < from.y)
		return Direction::DOWN;

	return Direction::DOWN;
}