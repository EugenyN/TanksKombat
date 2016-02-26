#ifndef __BASE_SCENE_H__
#define __BASE_SCENE_H__

#include "Engine.h"

#include "SneakyInput/SneakyButton.h"
#include "SneakyInput/SneakyButtonSkinnedBase.h"
#include "SneakyInput/SneakyJoystick.h"
#include "SneakyInput/SneakyJoystickSkinnedBase.h"

#define CHANGE_SCENE(__TYPE__) \
{ \
if(SCENE_TRANSITION) \
	Director::getInstance()->replaceScene(TransitionTurnOffTiles::create(0.5f, __TYPE__::createScene())); \
else \
	Director::getInstance()->replaceScene(__TYPE__::createScene()); \
} \


class JoystickWithSimulator : public SneakyJoystick
{
	cocos2d::EventKeyboard::KeyCode getKeyCodeFromVelocity() const;
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;
};

//

class ButtonWithSimulator : public SneakyButton
{
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;
};

//

class BaseScene : public cocos2d::LayerColor
{
public:
    virtual bool init(const cocos2d::Color4B& backgroundColor);
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	virtual void onKeyEvent(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	static BaseScene* getCurrentScene();

	int addTouchEvents(TouchEventsFunc touchEvent);
	bool removeTouchEvents(int);
	int addKeyboardEvents(KeyboardEventsFunc touchEvent);
	bool removeKeyboardEvents(int);
	void addJoystick();
	virtual void onExit() override;
#ifndef NDEBUG //if DEBUG
	static void debugPrint(const std::string& output);
#endif
protected:
	Engine* _engine = Engine::getInstance();
private:
	std::deque<TouchEventsFunc> _touchEvents;
	std::deque<KeyboardEventsFunc> _keyboardEvents;
	cocos2d::EventListenerTouchOneByOne * _eventListenerTouch;

	cocos2d::Layer* _onScreenJoystickLayer;
};

#endif // __BASE_SCENE_H__
