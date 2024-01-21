#include "AboutScene.h"
#include "MainMenuScene.h"

USING_NS_CC;

AboutScene::AboutScene()
	: _content(nullptr), _content2(nullptr)
{
}

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
		"Axmol 2.1, SneakyInput";

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

	_content = Label::createWithCharMap(FONT_MAIN);
	_content->setAlignment(TextHAlignment::CENTER);
	_content->setString(about);
	_content->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 32));
	this->addChild(_content, 2);

	_content2 = Label::createWithCharMap(FONT_MAIN);
	_content2->setAlignment(TextHAlignment::CENTER);
	_content2->setColor(Color3B(255, 255, 0));
	_content2->setString(aboutYellow);
	_content2->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 32));
	this->addChild(_content2, 2);

	//

	auto tip = Label::createWithCharMap(FONT_MAIN);
	tip->setString("Press any key to go back");
	tip->setPosition(Vec2(visibleSize.width / 2, 32));
	tip->enableShadow(Color4B(LIGHTGREEN_COLOR), Size(1, -1));
	this->addChild(tip, 2);

	return true;
}

void AboutScene::showDebugInfo()
{
	this->removeChild(_content2);
	_content2 = nullptr;

	_content->setAlignment(TextHAlignment::LEFT);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();
	auto glview = Director::getInstance()->getGLView();

	float dpiScale = (float)Device::getDPI() / 86.0f;
	float resolutionScale = 1.0f / glview->getScaleX();

	Size frameSize = glview->getFrameSize();

	// add joystick

	float joystickScale = JOYSTICK_SCALE * resolutionScale * dpiScale;

	const char* debugInfoText =
		"Version: %s\n\n"
		"Frame Size: %.0f,%.0f\n"
		"Visible Size: %.0f,%.0f\n"
		"ScaleX: %.3f ResScale %.3f\n"
		"DPI: %d\n"
		"Joystick Scale: %.3f\n";

	std::string debugInfo = StringUtils::format(debugInfoText, GAME_VERSION, frameSize.width,
		frameSize.height, visibleSize.width, visibleSize.height, glview->getScaleX(), resolutionScale,
		Device::getDPI(), joystickScale);

	_content->setString(debugInfo);
}

void AboutScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	BaseScene::onKeyPressed(keyCode, event);

	if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW) {
		showDebugInfo();
	}
	else {
		CHANGE_SCENE(MainMenuScene)
	}
}