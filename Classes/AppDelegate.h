#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"

class  AppDelegate : private cocos2d::Application
{
public:
	AppDelegate();
	~AppDelegate() override;

	void initGLContextAttrs() override;
	bool applicationDidFinishLaunching() override;
	void applicationDidEnterBackground() override;
	void applicationWillEnterForeground() override;
};

#endif // _APP_DELEGATE_H_

