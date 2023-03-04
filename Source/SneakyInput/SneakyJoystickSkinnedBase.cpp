#include "SneakyJoystickSkinnedBase.h"

using namespace cocos2d;

SneakyJoystickSkinnedBase::SneakyJoystickSkinnedBase()
	: backgroundSprite(nullptr), thumbSprite(nullptr), joystick(nullptr)
{

}

SneakyJoystickSkinnedBase::~SneakyJoystickSkinnedBase()
{
	if (backgroundSprite)
	{
		backgroundSprite->release();
		backgroundSprite = nullptr;
	}
	if (thumbSprite)
	{
		thumbSprite->release();
		thumbSprite = nullptr;
	}

	if (joystick)
	{
		joystick->release();
		joystick = nullptr;
	}
}

bool SneakyJoystickSkinnedBase::init()
{
	bool pRet = false;
	if (Layer::init()) {
		this->backgroundSprite = nullptr;

		this->thumbSprite = nullptr;

		this->joystick = nullptr;

		this->schedule(CC_SCHEDULE_SELECTOR(SneakyJoystickSkinnedBase::updatePositions));
		pRet = true;
	}
	return pRet;
}

void SneakyJoystickSkinnedBase::updatePositions(float delta)
{
	if (joystick && thumbSprite)
		thumbSprite->setPosition(joystick->getStickPosition());
}

void SneakyJoystickSkinnedBase::setContentSize(Size s)
{
	Layer::setContentSize(s);
	backgroundSprite->setContentSize(s);
	//joystick.joystickRadius = s.width/2;
}

void SneakyJoystickSkinnedBase::setBackgroundSprite(Sprite* aSprite)
{
	if (backgroundSprite) {
		if (backgroundSprite->getParent())
			backgroundSprite->getParent()->removeChild(backgroundSprite, true);
		backgroundSprite->release();
	}
	aSprite->retain();
	backgroundSprite = aSprite;
	this->setContentSize(backgroundSprite->getContentSize());
	this->addChild(backgroundSprite, 0);
}

void SneakyJoystickSkinnedBase::setThumbSprite(Sprite* aSprite)
{
	if (thumbSprite) {
		if (thumbSprite->getParent())
			thumbSprite->getParent()->removeChild(thumbSprite, true);
		thumbSprite->release();
	}
	aSprite->retain();
	thumbSprite = aSprite;
	this->addChild(thumbSprite, 1);
	//joystick->setThumbRadius(thumbSprite->getContentSize().width/2);
}

void SneakyJoystickSkinnedBase::setJoystick(SneakyJoystick* aJoystick)
{
	if (joystick) {
		if (joystick->getParent())
			joystick->getParent()->removeChild(joystick, true);
		joystick->release();
	}
	aJoystick->retain();
	joystick = aJoystick;
	this->addChild(aJoystick, 2);
	if (thumbSprite)
		joystick->setThumbRadius(thumbSprite->getBoundingBox().size.width / 2);
	else
		joystick->setThumbRadius(0);

	if (backgroundSprite)
		joystick->setJoystickRadius(backgroundSprite->getBoundingBox().size.width / 2);
}