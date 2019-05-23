#ifndef __MAINMENUSCENE_H__
#define __MAINMENUSCENE_H__

#include "cocos2d.h"
#include "BaseScene.h"

class MainMenuScene : public BaseScene
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();

	CREATE_FUNC(MainMenuScene);

private:
	void onKeyEvent(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
	void menuItemActivated(cocos2d::Ref* pSender);
	void onMenuItemActivated(int menuItem);
	void exitGame();
};

#endif // __MAINMENUSCENE_H__
