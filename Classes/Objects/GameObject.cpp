#include "GameObject.h"
#include "Engine.h"
#include "Objects\LevelGrid.h"
#include "Scenes\GameplayScene.h"

USING_NS_CC;

GameObject::GameObject(Type type)
	: _direction(Direction::UP), _sprite(nullptr), _engine(Engine::getInstance()), 
	_type(type), _isPassable(true)
{ }

GameObject::~GameObject()
{ 
	_sprite = nullptr;
	_engine = nullptr;

	if (!_isPassable)
		removeFromPassableLayer(_position);
}

bool GameObject::init()
{
	return Node::init();
}

GameObject::Direction GameObject::getGridDirection()
{
	return _direction;
}

void GameObject::setGridDirection(Direction direction)
{
	_direction = direction;

	switch (direction)
	{
	case Direction::UP:
		_sprite->setRotation(0.0f);
		break;
	case Direction::DOWN:
		_sprite->setRotation(180.0f);
		break;
	case Direction::LEFT:
		_sprite->setRotation(-90.0f);
		break;
	case Direction::RIGHT:
		_sprite->setRotation(90.0f);
		break;
	default:
		break;
	}
}


Pos2 GameObject::getGridPosition() const
{
	return _position;
}

void GameObject::setGridPosition(const Pos2& p)
{
	auto prevPos = _position;
	_position = p;
	_sprite->setPosition(Vec2(p.x, p.y)  * TILE_SIZE + Vec2::ONE * TILE_SIZE / 2);

	if (!_isPassable)
		updatePassableLayer(p, prevPos);
}

void GameObject::setGridPosition(int x, int y)
{
	setGridPosition(Pos2(x, y));
}

Pos2 GameObject::getGridPosition(Node* node)
{
	return Pos2();
}

void GameObject::setGridPosition(Node* node, const Pos2& p)
{
	node->setPosition(Vec2(p.x, p.y)  * TILE_SIZE + Vec2::ONE * TILE_SIZE / 2);
}

void GameObject::setGridPosition(Node* node, int x, int y)
{
	setGridPosition(node, Pos2(x, y));
}

void GameObject::setGridDirection(Node* node, Direction direction)
{
	switch (direction)
	{
	case Direction::UP:
		node->setRotation(0.0f);
		break;
	case Direction::DOWN:
		node->setRotation(180.0f);
		break;
	case Direction::LEFT:
		node->setRotation(-90.0f);
		break;
	case Direction::RIGHT:
		node->setRotation(90.0f);
		break;
	default:
		break;
	}
}

Sprite * GameObject::getSprite()
{
	return _sprite;
}

void GameObject::setSprite(Sprite * sprite)
{
	if (_sprite != nullptr && _sprite->getParent() == this)
		this->removeChild(_sprite);

	_sprite = sprite;

	this->addChild(_sprite);
}

Pos2 GameObject::directionToOffset(Direction direction)
{
	switch (direction)
	{
	case Direction::DOWN:
		return Pos2(0, -1);
	case Direction::UP:
		return Pos2(0, +1);
	case Direction::LEFT:
		return Pos2(-1, 0);
	case Direction::RIGHT:
		return Pos2(+1, 0);
	}
	return Pos2(0, 0);
}

void GameObject::updatePassableLayer(const Pos2& pos, const Pos2& prevPos)
{
	auto scene = Engine::getInstance()->getCurrentScene<GameplayScene>();
	if (scene == nullptr)
		return;
	auto grid = scene->getGrid();

	grid->setDynamicPassable(prevPos, PassableValue::PASSABLE);
	grid->setDynamicPassable(pos, PassableValue::INPASSABLE);
}

void GameObject::removeFromPassableLayer(const Pos2& pos)
{
	auto scene = Engine::getInstance()->getCurrentScene<GameplayScene>();
	if (scene == nullptr)
		return;
	auto grid = scene->getGrid();

	grid->setDynamicPassable(pos, PassableValue::PASSABLE);
}