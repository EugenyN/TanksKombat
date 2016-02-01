#ifndef __ABOUT_SCENE_H__
#define __ABOUT_SCENE_H__

#include "BaseScene.h"

class AboutScene : public BaseScene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();  
    CREATE_FUNC(AboutScene);

private:
	void onKeyEvent(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
};

#endif // __ABOUT_SCENE_H__
