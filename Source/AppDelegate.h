#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "axmol.h"

class  AppDelegate : private ax::Application
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

