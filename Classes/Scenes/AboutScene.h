#ifndef __ABOUT_SCENE_H__
#define __ABOUT_SCENE_H__

#include "BaseScene.h"

class AboutScene : public BaseScene
{
public:
	static cocos2d::Scene* createScene();
	AboutScene();
	bool init() override;
	CREATE_FUNC(AboutScene);

private:
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event) override;
	void showDebugInfo();
	cocos2d::Label* _content;
	cocos2d::Label* _content2;
};

#endif // __ABOUT_SCENE_H__
