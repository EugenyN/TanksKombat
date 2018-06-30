#include "cocos2d.h"
#include "BaseScene.h"

USING_NS_CC;


cocos2d::EventKeyboard::KeyCode JoystickWithSimulator::getKeyCodeFromVelocity() const
{
	EventKeyboard::KeyCode code = EventKeyboard::KeyCode::KEY_UP_ARROW;
	if (getVelocity().y > 0.5f)
		code = EventKeyboard::KeyCode::KEY_UP_ARROW;
	else if (getVelocity().y < -0.5f)
		code = EventKeyboard::KeyCode::KEY_DOWN_ARROW;
	else if (getVelocity().x > 0.5f)
		code = EventKeyboard::KeyCode::KEY_RIGHT_ARROW;
	else if (getVelocity().x < -0.5f)
		code = EventKeyboard::KeyCode::KEY_LEFT_ARROW;
	return code;
}

bool JoystickWithSimulator::onTouchBegan(Touch * touch, Event * event)
{
	if (SneakyJoystick::onTouchBegan(touch, event)) {
		if (getVelocity().isZero())
			return true;
		EventKeyboard simEvent(getKeyCodeFromVelocity(), true);
		_eventDispatcher->dispatchEvent(&simEvent);
		return true;
	}
	return false;
}

void JoystickWithSimulator::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
	EventKeyboard simEvent(getKeyCodeFromVelocity(), false);
	_eventDispatcher->dispatchEvent(&simEvent);

	SneakyJoystick::onTouchEnded(touch, event);
}

bool ButtonWithSimulator::onTouchBegan(Touch * touch, Event * event)
{
	if (SneakyButton::onTouchBegan(touch, event)) {
		EventKeyboard simEvent(EventKeyboard::KeyCode::KEY_ENTER, true);
		_eventDispatcher->dispatchEvent(&simEvent);
	}
	return false;
}

void ButtonWithSimulator::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
	SneakyButton::onTouchEnded(touch, event);

	EventKeyboard simEvent(EventKeyboard::KeyCode::KEY_ENTER, false);
	_eventDispatcher->dispatchEvent(&simEvent);
}

//

static BaseScene* _currentScene = nullptr;

bool BaseScene::init(const Color4B& backgroundColor)
{
	_currentScene = this;

	if (!LayerColor::initWithColor(backgroundColor) )
		return false;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	this->setContentSize(Size(visibleSize.width, visibleSize.height));
	this->setAnchorPoint(Vec2(0, 0));

	_eventListenerTouch = EventListenerTouchOneByOne::create();
	_eventListenerTouch->setSwallowTouches(true);
	_eventListenerTouch->onTouchBegan = CC_CALLBACK_2(BaseScene::onTouchBegan, this);
	_eventListenerTouch->onTouchMoved = CC_CALLBACK_2(BaseScene::onTouchMoved, this);
	_eventListenerTouch->onTouchEnded = CC_CALLBACK_2(BaseScene::onTouchEnded, this);
	_eventListenerTouch->onTouchCancelled = CC_CALLBACK_2(BaseScene::onTouchCancelled, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(_eventListenerTouch, this);

	// creating a keyboard event listener
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(BaseScene::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(BaseScene::onKeyReleased, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	_onScreenJoystickLayer = Layer::create();
	addChild(_onScreenJoystickLayer, 2);
	addJoystick();
#endif

	return true;
}

void BaseScene::onExit()
{
	_eventDispatcher->removeEventListener(_eventListenerTouch);
	LayerColor::onExit();
}

void BaseScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
	for (const auto &ev : _keyboardEvents)
	{
		if (ev.onKeyPressed && ev.onKeyPressed(keyCode, event))
			return;
	}
}

void BaseScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_BACK)
		event->stopPropagation(); // avoid exit on WINRT

	for (const auto &ev : _keyboardEvents)
	{
		if (ev.onKeyReleased && ev.onKeyReleased(keyCode, event))
			return;
	}

	onKeyEvent(keyCode, event);
}

void BaseScene::onKeyEvent(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
}

bool BaseScene::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
	for (const auto &ev : _touchEvents)
	{
		if (ev.onTouchBegan && ev.onTouchBegan(touch, event))
			return true;
	}

	return false;
}

void BaseScene::onTouchMoved(Touch* touch, Event* event)
{
	for (const auto &ev : _touchEvents)
	{
		if (ev.onTouchMoved)
			ev.onTouchMoved(touch, event);
	}
}

void BaseScene::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
	for (const auto &ev : _touchEvents)
	{
		if (ev.onTouchEnded)
			ev.onTouchEnded(touch, event);
	}
}

void BaseScene::onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event)
{
	for (const auto &ev : _touchEvents)
	{
		if (ev.onTouchCancelled)
			ev.onTouchCancelled(touch, event);
	}
}

BaseScene* BaseScene::getCurrentScene()
{
	return _currentScene;
}

int BaseScene::addTouchEvents(TouchEventsFunc touchEvent)
{
	static int id_generator = 0;
	id_generator++;
	touchEvent.id = id_generator;
	_touchEvents.push_front(touchEvent);
	return id_generator;
};


bool BaseScene::removeTouchEvents(int id)
{
	for (auto it = _touchEvents.begin(); it != _touchEvents.end(); ++it)
	{
		if (it->id == id) {
			_touchEvents.erase(it);
			return (true);
		}
	}
	return false;
};

int BaseScene::addKeyboardEvents(KeyboardEventsFunc keyboardEvent)
{
	static int id_generator = 0;
	id_generator++;
	keyboardEvent.id = id_generator;
	_keyboardEvents.push_front(keyboardEvent);
	return id_generator;
};

bool BaseScene::removeKeyboardEvents(int id)
{
	for (auto it = _keyboardEvents.begin(); it != _keyboardEvents.end(); ++it)
	{
		if (it->id == id) {
			_keyboardEvents.erase(it);
			return (true);
		}
	}
	return false;
};

void BaseScene::addJoystick()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();
	auto glview = Director::getInstance()->getOpenGLView();

	float dpiScale = Device::getDPI() / 86.0f;
	float resolutionScale = 1.0f / glview->getScaleX();

	// add joystick

	float scale =  JOYSTICK_SCALE * resolutionScale * dpiScale;

#ifndef NDEBUG
	Size frameSize = glview->getFrameSize();
	debugPrint(StringUtils::format("Frame: %.0f,%.0f DPI: %d Scale: %.3f", 
		frameSize.width, frameSize.height, Device::getDPI(), scale));
#endif

	Rect joystickBaseDimensions = Rect(0, 0, 160.0f, 160.0f);
	Point joystickHalfSize = joystickBaseDimensions.size / 2;
	Point joystickOffset = JOYSTICK_OFFSET;
	Point joystickBasePosition = (origin + joystickOffset + joystickHalfSize) * scale + joystickHalfSize * (scale - 1);

	Rect fireButtonDimensions = Rect(0, 0, 80.0f, 80.0f);
	Point fireButtonHalfSize = fireButtonDimensions.size / 2;
	Point fireButtonOffset = FIRE_BUTTON_OFFSET;
	Point fireButtonPosition1 = (origin + fireButtonOffset + fireButtonHalfSize) * scale + 
		Point(- fireButtonHalfSize.x , + fireButtonHalfSize.y) * (scale - 1);
	Point fireButtonPosition = Point(visibleSize.width - fireButtonPosition1.x, fireButtonPosition1.y);

	// add joystick base
	auto joystickBase = SneakyJoystickSkinnedBase::create();
	auto joystick = new JoystickWithSimulator();
	joystick->initWithRect(joystickBaseDimensions);
	joystick->setIsDPad(true);
	joystick->setInnerThumb(true);
	joystickBase->setBackgroundSprite(CCSprite::create("graphics/joystick/dpadDown.png"));
	joystickBase->setThumbSprite(CCSprite::create("graphics/joystick/joystickDown.png"));
	joystickBase->setJoystick(joystick);
	joystickBase->setPosition(joystickBasePosition);
	joystickBase->setScale(scale);
	_onScreenJoystickLayer->addChild(joystickBase);

	// add joystick button
	auto action1ButtonBase = SneakyButtonSkinnedBase::create();
	auto button1 = new ButtonWithSimulator();
	button1->initWithRect(fireButtonDimensions);
	// button1->setIsHoldable(true);
	action1ButtonBase->setDefaultSprite(CCSprite::create("graphics/joystick/action1Down.png"));
	action1ButtonBase->setActivatedSprite(CCSprite::create("graphics/joystick/action1Down.png"));
	action1ButtonBase->setPressSprite(CCSprite::create("graphics/joystick/action1Up.png"));
	action1ButtonBase->setButton(button1);
	action1ButtonBase->setPosition(fireButtonPosition);
	action1ButtonBase->setScale(scale);
	_onScreenJoystickLayer->addChild(action1ButtonBase);
}


#ifndef NDEBUG

void BaseScene::debugPrint(const std::string& output)
{
	static Label* label = nullptr;
	static BaseScene *thisScene = nullptr;
	if(label == nullptr || thisScene != getCurrentScene()){
		Size visibleSize = Director::getInstance()->getVisibleSize();
		label = Label::createWithCharMap(FONT_MAIN);
		label->setString(output);
		label->setPosition(Vec2(visibleSize.width / 2, 64));
		//label->enableShadow(Color4B(0, 0, 0, 128), Size(1, -1));
		getCurrentScene()->addChild(label, 10000);
		thisScene = getCurrentScene();
	}
	label->setString(output);
};

#endif
