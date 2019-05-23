#ifndef __SETTINGS_SCENE_H__
#define __SETTINGS_SCENE_H__

#include "BaseScene.h"


class SettingsScene : public BaseScene
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();  
	virtual void onExit() override;
	CREATE_FUNC(SettingsScene);

private:
	void onKeyEvent(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
	void menuItemActivated(cocos2d::Ref* pSender);
	void menuItemValueChanged(Ref * pSender);
	void onMenuItemActivated(int menuItem);
};

#endif // __SETTINGS_SCENE_H__
