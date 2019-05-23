#include "SneakyButtonSkinnedBase.h"

using namespace cocos2d;

SneakyButtonSkinnedBase::~SneakyButtonSkinnedBase()
{
	if (defaultSprite)
	{
		defaultSprite->release();
		defaultSprite = nullptr;
	}
	if (activatedSprite)
	{
		activatedSprite->release();
		activatedSprite = nullptr;
	}
	if (disabledSprite)
	{
		disabledSprite->release();
		disabledSprite = nullptr;
	}
	if (pressSprite)
	{
		pressSprite->release();
		pressSprite = nullptr;
	}
	if (button)
	{
		button->release();
		button = nullptr;
	}
}

bool SneakyButtonSkinnedBase::init()  //Possible errors here
{
	bool pRet = false;
	if (Layer::init()) {
		this->defaultSprite = nullptr;
		//defaultSprite->retain();
		this->activatedSprite = nullptr;
		//activatedSprite->retain();
		this->disabledSprite = nullptr;
		//disabledSprite->retain();
		this->pressSprite = nullptr;
		//pressSprite->retain();
		this->button = nullptr;
		//button->retain();

		this->schedule(schedule_selector(SneakyButtonSkinnedBase::watchSelf));
		pRet = true;
	}
	return pRet;
}

void SneakyButtonSkinnedBase::watchSelf(float delta) //Be Careful Here
{
	if (!this->button->getStatus()) {
		if (disabledSprite) {
			disabledSprite->setVisible(true);
		}
		else {
			disabledSprite->setVisible(false);
		}
	}
	else {
		if (!this->button->getIsActive()) {
			pressSprite->setVisible(false);
			if (this->button->getValue() == 0) {
				activatedSprite->setVisible(false);
				if (defaultSprite) {
					defaultSprite->setVisible(true);
				}
			}
			else {
				activatedSprite->setVisible(true);
			}
		}
		else {
			defaultSprite->setVisible(false);
			if (pressSprite) {
				pressSprite->setVisible(true);
			}
		}
	}
}

void SneakyButtonSkinnedBase::setContentSize(Size s)
{
	Layer::setContentSize(s);
	defaultSprite->setContentSize(s);
	//button->setRadius(s.width/2);
}

void SneakyButtonSkinnedBase::setDefaultSprite(Sprite *aSprite)
{
	if (defaultSprite) {
		if (defaultSprite->getParent())
			defaultSprite->getParent()->removeChild(defaultSprite, true);
		defaultSprite->release();
	}
	aSprite->retain();
	defaultSprite = aSprite; //Check again here
	if (aSprite) {
		this->addChild(defaultSprite, 0);

		this->setContentSize(defaultSprite->getContentSize());
	}
}

void SneakyButtonSkinnedBase::setActivatedSprite(Sprite *aSprite)
{
	if (activatedSprite) {
		if (activatedSprite->getParent())
			activatedSprite->getParent()->removeChild(activatedSprite, true);
		activatedSprite->release();
	}
	aSprite->retain();
	activatedSprite = aSprite;
	if (aSprite) {
		this->addChild(activatedSprite, 1);

		this->setContentSize(activatedSprite->getContentSize());
	}
}

void SneakyButtonSkinnedBase::setDisabledSprite(Sprite *aSprite)
{
	if (disabledSprite) {
		if (disabledSprite->getParent())
			disabledSprite->getParent()->removeChild(disabledSprite, true);
		disabledSprite->release();
	}
	aSprite->retain();
	disabledSprite = aSprite;
	if (aSprite) {
		this->addChild(disabledSprite, 2);

		this->setContentSize(disabledSprite->getContentSize());
	}
}

void SneakyButtonSkinnedBase::setPressSprite(Sprite *aSprite)
{
	if (pressSprite) {
		if (pressSprite->getParent())
			pressSprite->getParent()->removeChild(pressSprite, true);
		pressSprite->release();
	}
	aSprite->retain();
	pressSprite = aSprite;
	if (aSprite) {
		this->addChild(pressSprite, 3);

		this->setContentSize(pressSprite->getContentSize());
	}
}

void SneakyButtonSkinnedBase::setButton(SneakyButton *aButton)
{
	if (button) {
		if (button->getParent())
			button->getParent()->removeChild(button, true);
		button->release();
	}
	aButton->retain();
	button = aButton;
	if (aButton) {
		this->addChild(button, 4);
		if (defaultSprite)
			button->setRadius(defaultSprite->getBoundingBox().size.width / 2);
	}
}
