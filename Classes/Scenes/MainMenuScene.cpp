#include "MainMenuScene.h"
#include "GameplayScene.h"
#include "SettingsScene.h"
#include "GameSettingsScene.h"
#include "GUI\ModalDialog.h"
#include "GUI\SimpleMenu.h"

USING_NS_CC;

#define TAG_NEW_GAME	0
#define TAG_OPTIONS		1
#define TAG_EXIT		2

Scene* MainMenuScene::createScene()
{
	auto scene = Scene::create();
	auto layer = MainMenuScene::create();
	scene->addChild(layer);
	return scene;
}

bool MainMenuScene::init()
{
	if (!BaseScene::init(Color4B(DARKGREEN_COLOR)))
		return false;

	Size visibleSize = Director::getInstance()->getVisibleSize();

	// label

	auto title = Label::createWithCharMap(FONT_LOGO);
	title->setString("TANKS");
	title->setPosition(Vec2(visibleSize.width / 2 - 128, visibleSize.height / 2 + 180));
	title->enableShadow(Color4B(0, 0, 0, 255), Size(5, -5));
	title->getFontAtlas()->setAliasTexParameters();
	title->setScale(2);
	this->addChild(title, 2);

	auto title2 = Label::createWithCharMap(FONT_LOGO);
	title2->setString("KOMBAT");
	title2->setPosition(Vec2(visibleSize.width / 2 + 32 + 64, visibleSize.height / 2 + 100));
	title2->enableShadow(Color4B(0, 0, 0, 255), Size(5, -5));
	title2->getFontAtlas()->setAliasTexParameters();
	title2->setScale(2);
	this->addChild(title2, 2);

	// copyright

	auto autors = Label::createWithCharMap(FONT_MAIN);
	autors->setString("EugenyN 2016 GPL License");
	autors->setPosition(Vec2(visibleSize.width / 2, 32));
	autors->enableShadow(Color4B(LIGHTGREEN_COLOR), Size(1, -1));
	this->addChild(autors, 2);

	// start, options exit menu

	auto startItem = SimpleMenuItem::create("START", FONT_MAIN, CC_CALLBACK_1(MainMenuScene::menuItemActivated, this));
	startItem->setTag(TAG_NEW_GAME);
	startItem->setAnchorPoint(Vec2(0, 0.5f));

	auto optionsItem = SimpleMenuItem::create("OPTIONS", FONT_MAIN, CC_CALLBACK_1(MainMenuScene::menuItemActivated, this));
	optionsItem->setTag(TAG_OPTIONS);
	optionsItem->setAnchorPoint(Vec2(0, 0.5f));

	auto exitItem = SimpleMenuItem::create("EXIT", FONT_MAIN, CC_CALLBACK_1(MainMenuScene::menuItemActivated, this));
	exitItem->setTag(TAG_EXIT);
	exitItem->setAnchorPoint(Vec2(0, 0.5f));

	auto menu = SimpleMenu::create(SimpleMenu::Type::VERTICAL, startItem, optionsItem, exitItem, NULL);
	menu->setPosition(Vec2(visibleSize.width / 2 - 60, visibleSize.height / 2 - 110));
	menu->alignItemsVerticallyWithPadding(16);
	this->addChild(menu, 1);

	// animated logo

	auto spriteBatch = SpriteBatchNode::create("graphics/tanks_logo.png");
	spriteBatch->getTexture()->setAliasTexParameters();
	auto spritecache = SpriteFrameCache::getInstance();
	spritecache->addSpriteFramesWithFile("graphics/tanks_logo.plist");

	auto tankLogo = Sprite::createWithSpriteFrameName("tank_logo0.png");
	tankLogo->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	tankLogo->setScale(2);
	tankLogo->getTexture()->setAliasTexParameters();
	spriteBatch->addChild(tankLogo);
	this->addChild(spriteBatch, 3);

	Vector<SpriteFrame*> animFrames(2);
	animFrames.pushBack(spritecache->getSpriteFrameByName("tank_logo0.png"));
	animFrames.pushBack(spritecache->getSpriteFrameByName("tank_logo1.png"));
	tankLogo->runAction(RepeatForever::create(Animate::create(
		Animation::createWithSpriteFrames(animFrames, 0.4f))));

	_engine->setMusicVolume(_engine->Settings.musicVolume);

	return true;
}

void MainMenuScene::menuItemActivated(Ref* pSender)
{
	auto pMenuItem = (MenuItem*)pSender;
	onMenuItemActivated(pMenuItem->getTag());
}

void MainMenuScene::onKeyEvent(EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_BACK) 
	{
		auto exitDialog = ModalDialog::create("Do you want to exit?", CC_CALLBACK_0(MainMenuScene::exitGame, this), nullptr, "YES", "NO");
		this->addChild(exitDialog, MODAL_DIALOGS_LAYER_ZORDER);
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_MENU) {
		onMenuItemActivated(TAG_OPTIONS);
	}
}

void MainMenuScene::onMenuItemActivated(int menuItem)
{
	switch (menuItem)
	{
	case TAG_NEW_GAME:
		CHANGE_SCENE(GameSettingsScene);
		break;
	case TAG_OPTIONS:
		CHANGE_SCENE(SettingsScene);
		break;
	case TAG_EXIT:
		exitGame();
		return;
	}
}

void MainMenuScene::exitGame() 
{
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}