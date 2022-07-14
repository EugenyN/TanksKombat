#include "Bonus.h"
#include "Engine.h"
#include "Objects\Explosion.h"
#include "Scenes\GameplayScene.h"

USING_NS_CC;

Bonus::Bonus()
	: GameObject(Type::BONUS)
{
}

Bonus::~Bonus()
{
}

bool Bonus::init()
{
	GameObject::init();

	_sprite = Sprite::createWithSpriteFrameName("bonus.png");
	this->addChild(_sprite);

	auto seq = Sequence::createWithTwoActions(
		ScaleBy::create(1.0f, 0.8f), ScaleBy::create(1.0f, 1.0f / 0.8f));
	_sprite->runAction(RepeatForever::create(seq));

	return true;
}

void Bonus::take()
{
	_engine->playSound("take_bonus");

	auto scene = _engine->getCurrentScene<GameplayScene>();
	if (scene != nullptr)
		scene->removeGameObject(this);
}

void Bonus::kill()
{
	Explosion::createFromPool(getGridPosition());

	_engine->playSound("bonus_kill");

	auto scene = _engine->getCurrentScene<GameplayScene>();
	if (scene != nullptr)
		scene->removeGameObject(this);
}