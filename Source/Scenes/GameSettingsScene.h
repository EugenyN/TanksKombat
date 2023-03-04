#ifndef __GAMESETTINGSSCENE_H__
#define __GAMESETTINGSSCENE_H__

#include "BaseScene.h"


class GameSettingsScene : public BaseScene
{
public:
	static cocos2d::Scene* createScene();
	bool init() override;
	CREATE_FUNC(GameSettingsScene);

private:
	void onKeyEvent(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;
	void menuItemActivated(cocos2d::Ref* pSender);
	void menuItemValueChanged(cocos2d::Ref* pSender);
	void onMenuItemActivated(int menuItem);
};

#endif // __GAMESETTINGSSCENE_H__
