#include "AboutScene.h"
#include "MainMenuScene.h"

USING_NS_CC;

Scene* AboutScene::createScene()
{
	auto scene = Scene::create();
	auto layer = AboutScene::create();
	scene->addChild(layer);
	return scene;
}

bool AboutScene::init()
{
	if (!BaseScene::init(Color4B(DARKGREEN_COLOR)))
		return false;

	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto title = Label::createWithCharMap(FONT_MAIN);
	title->setString("ABOUT");
	title->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 32));
	title->enableShadow(Color4B(LIGHTGREEN_COLOR), Size(1, -1));
	this->addChild(title, 2);

	const char* about =
		"Programming and game design:\n\n"
		"Eugeny Novikov\n\n"
		"Music:\n\n"
		"Abrove Speed by Rob F.\n\n"
		"Graphics and fonts:\n\n"
		"SMBX project\n"
		"DNS Design & Wildcop\n"
		"zironid_n, M-Tee\n"
		"Kenney Vleugels\n\n"
		"3d party code:\n\n"
		"Cocos2d-x 3.9, SneakyInput";

	const char* aboutYellow =
		"Programming and game design:\n\n"
		"\n\n"
		"Music:\n\n"
		"\n\n"
		"Graphics and fonts:\n\n"
		"\n"
		"\n"
		"\n"
		"\n\n"
		"3d party code:\n\n";

	auto content = Label::createWithCharMap(FONT_MAIN);
	content->setAlignment(TextHAlignment::CENTER);
	content->setString(about);
	content->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 32));
	this->addChild(content, 2);

	auto content2 = Label::createWithCharMap(FONT_MAIN);
	content2->setAlignment(TextHAlignment::CENTER);
	content2->setColor(Color3B(255, 255, 0));
	content2->setString(aboutYellow);
	content2->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 32));
	this->addChild(content2, 2);

	//

	auto tip = Label::createWithCharMap(FONT_MAIN);
	tip->setString("Press any key to go back");
	tip->setPosition(Vec2(visibleSize.width / 2, 32));
	tip->enableShadow(Color4B(LIGHTGREEN_COLOR), Size(1, -1));
	this->addChild(tip, 2);

	return true;
}

void AboutScene::onKeyEvent(EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
	CHANGE_SCENE(MainMenuScene);
}