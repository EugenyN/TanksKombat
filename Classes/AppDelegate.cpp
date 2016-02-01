#include "AppDelegate.h"
#include "Engine.h"

USING_NS_CC;

AppDelegate::AppDelegate() 
{ }

AppDelegate::~AppDelegate() 
{ }

void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};
    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() 
{
	auto engine = Engine::getInstance();
	engine->launchGame();
    return true;
}

void AppDelegate::applicationDidEnterBackground() 
{
	auto engine = Engine::getInstance();
	engine->enterBackground();
}

void AppDelegate::applicationWillEnterForeground()
{
	auto engine = Engine::getInstance();
	engine->enterForeground();
}
