#include "SettingsScene.h"
#include "MainMenuScene.h"
#include "AboutScene.h"
#include "GUI\ModalDialog.h"
#include "GUI\SimpleMenu.h"

USING_NS_CC;

#define TAG_FULLSCREEN	0
#define TAG_MUSIC		1
#define TAG_SOUND_FX	2
#define TAG_ABOUT		3
#define TAG_DONE		4

Scene* SettingsScene::createScene()
{
	auto scene = Scene::create();
	auto layer = SettingsScene::create();
	scene->addChild(layer);
	return scene;
}

bool SettingsScene::init()
{
	if (!BaseScene::init(Color4B(DARKGREEN_COLOR)))
		return false;

	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto title = Label::createWithCharMap(FONT_MAIN);
	title->setString("SETTINGS");
	title->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 32));
	title->enableShadow(Color4B(LIGHTGREEN_COLOR), Size(1, -1));
	this->addChild(title, 2);

	//

	auto tip = Label::createWithCharMap(FONT_MAIN);
	tip->setString("Press 'FIRE' to select");
	tip->setPosition(Vec2(visibleSize.width / 2, 32));
	tip->enableShadow(Color4B(LIGHTGREEN_COLOR), Size(1, -1));
	this->addChild(tip, 2);

	// start, options exit menu

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)

	auto fullscreenItem = SimpleMenuItem::create("FULLSCREEN\t%s", FONT_MAIN, CC_CALLBACK_1(SettingsScene::menuItemActivated, this));
	fullscreenItem->setValues({ "OFF", "ON" });
	fullscreenItem->setCurrentValueIndex((int)_engine->Settings.fullscreen);
	fullscreenItem->setTag(TAG_FULLSCREEN);
	fullscreenItem->setValueChangedCallback(CC_CALLBACK_1(SettingsScene::menuItemValueChanged, this));
	fullscreenItem->setAnchorPoint(Vec2(0, 0.5f));

#endif

	auto musicItem = SimpleMenuItem::create("MUSIC\t%s", FONT_MAIN, CC_CALLBACK_1(SettingsScene::menuItemActivated, this));
	musicItem->setValues({ "OFF", "ON" });
	musicItem->setCurrentValueIndex((int)_engine->Settings.musicEnabled);
	musicItem->setTag(TAG_MUSIC);
	musicItem->setValueChangedCallback(CC_CALLBACK_1(SettingsScene::menuItemValueChanged, this));
	musicItem->setAnchorPoint(Vec2(0, 0.5f));

	auto sfxItem = SimpleMenuItem::create("SFX\t\t\t%s", FONT_MAIN, CC_CALLBACK_1(SettingsScene::menuItemActivated, this));
	sfxItem->setValues({ "OFF", "ON" });
	sfxItem->setCurrentValueIndex((int)_engine->Settings.soundEnabled);
	sfxItem->setTag(TAG_SOUND_FX);
	sfxItem->setValueChangedCallback(CC_CALLBACK_1(SettingsScene::menuItemValueChanged, this));
	sfxItem->setAnchorPoint(Vec2(0, 0.5f));

	auto aboutItem = SimpleMenuItem::create("ABOUT", FONT_MAIN, CC_CALLBACK_1(SettingsScene::menuItemActivated, this));
	aboutItem->setTag(TAG_ABOUT);
	aboutItem->setAnchorPoint(Vec2(0, 0.5f));

	auto doneItem = SimpleMenuItem::create("DONE", FONT_MAIN, CC_CALLBACK_1(SettingsScene::menuItemActivated, this));
	doneItem->setTag(TAG_DONE);
	doneItem->setAnchorPoint(Vec2(0, 0.5f));

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
	auto menu = SimpleMenu::create(SimpleMenu::Type::VERTICAL, fullscreenItem, musicItem, sfxItem, aboutItem, doneItem, NULL);
#else
	auto menu = SimpleMenu::create(SimpleMenu::Type::VERTICAL, musicItem, sfxItem, aboutItem, doneItem, NULL);
#endif

	menu->setPosition(Vec2(visibleSize.width / 2 - 90, visibleSize.height / 2));
	menu->alignItemsVerticallyWithPadding(16);
	menu->setCurrentMenuItemTag(TAG_DONE);
	this->addChild(menu, 1);

	return true;
}

void SettingsScene::onExit()
{
	BaseScene::onExit();
	_engine->saveSettings();
}

void SettingsScene::menuItemActivated(Ref* pSender)
{
	auto pMenuItem = (MenuItem*)pSender;
	onMenuItemActivated(pMenuItem->getTag());
}

void SettingsScene::menuItemValueChanged(Ref* pSender)
{
	auto pMenuItem = (SimpleMenuItem*)pSender;
	int  currentValue = pMenuItem->getCurrentValueIndex();

	switch (pMenuItem->getTag())
	{
	case TAG_FULLSCREEN: {
		_engine->Settings.fullscreen = currentValue == 1;
		this->addChild(ModalDialog::create("Changes will take effect after restart."), MODAL_DIALOGS_LAYER_ZORDER);
		break;
	}
	case TAG_MUSIC:
		if (_engine->Settings.musicEnabled != (currentValue == 1)) {
			_engine->Settings.musicEnabled = currentValue == 1;

			if (_engine->Settings.musicEnabled)
				_engine->playMusic(BACKGROUND_MUSIC);
			else
				_engine->stopMusic();
		}
		break;
	case TAG_SOUND_FX:
		_engine->Settings.soundEnabled = currentValue == 1;
		_engine->playSound("menu_select");
		break;
	default:
		break;
	}
}

void SettingsScene::onKeyEvent(EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_BACK) {
		CHANGE_SCENE(MainMenuScene);
	}
}

void SettingsScene::onMenuItemActivated(int menuItem)
{
	switch (menuItem)
	{
	case TAG_ABOUT:
		CHANGE_SCENE(AboutScene);
		break;
	case TAG_DONE:
		CHANGE_SCENE(MainMenuScene);
		return;
	}
}
