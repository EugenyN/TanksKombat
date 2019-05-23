#ifndef __HIGHSCORES_SCENE_H__
#define __HIGHSCORES_SCENE_H__

#include "BaseScene.h"


class HighScoresScene : public BaseScene
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(HighScoresScene);

private:
	void menuItemActivated(Ref * pSender);
	void onMenuItemActivated(int menuItem);
	void onKeyEvent(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
};

#endif // __HIGHSCORES_SCENE_H__
