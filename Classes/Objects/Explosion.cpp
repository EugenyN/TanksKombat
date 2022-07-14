#include "Explosion.h"
#include "Engine.h"
#include "Scenes\GameplayScene.h"


USING_NS_CC;

cocos2d::Vector<Explosion*> Explosion::_explosionPool;


Explosion::Explosion()
	: GameObject(Type::EXPLOSION), _animation(nullptr)
{ }

Explosion::~Explosion()
{
	CC_SAFE_RELEASE_NULL(_animation);
}

Explosion* Explosion::createFromPool(const Pos2& pos)
{
	// find an available explosion.
	for (auto expl : _explosionPool)
	{
		if (!expl->inUse())
		{
			expl->activate(pos);
			return expl;
		}
	}

	auto expl = Explosion::create();
	expl->activate(pos);
	_explosionPool.pushBack(expl);

	auto scene = Engine::getInstance()->getCurrentScene<GameplayScene>();
	if (scene != nullptr)
		scene->addGameObject(expl);

	return expl;

}

void Explosion::clearPool()
{
	//auto scene = Engine::getInstance()->getCurrentScene<GameplayScene>();
	//if (scene != nullptr) {
	//	for (auto expl : _explosionPool)
	//		scene->removeGameObject(expl);
	//}

	_explosionPool.clear();
}

bool Explosion::init()
{
	GameObject::init();

	_sprite = Sprite::createWithSpriteFrameName("explosion1.png");
	this->addChild(_sprite);

	auto spritecache = SpriteFrameCache::getInstance();

	Vector<SpriteFrame*> animFrames(5);
	for (int i = 1; i <= 5; i++) {
		animFrames.pushBack(spritecache->getSpriteFrameByName(
			StringUtils::format("explosion%d.png", i)));
	}

	_animation = Animation::createWithSpriteFrames(animFrames, 0.05f);
	_animation->retain();

	return true;
}

void Explosion::activate(Pos2 pos)
{
	setGridPosition(pos);

	auto action = Sequence::create
	(
		Animate::create(_animation),
		CallFunc::create([this]() { _sprite->setVisible(false); }),
		nullptr
	);

	action->setTag(0);
	_sprite->setVisible(true);
	_sprite->runAction(action);
}

bool Explosion::inUse() const
{
	return _sprite->getActionByTag(0) != nullptr;
}