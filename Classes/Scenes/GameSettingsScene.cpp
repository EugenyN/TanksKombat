#include "GameSettingsScene.h"
#include "MainMenuScene.h"
#include "GameplayScene.h"
#include "GUI\ModalDialog.h"
#include "GUI\SimpleMenu.h"

USING_NS_CC;

#define TAG_TANK1		0
#define TAG_TANK2		1
#define TAG_TANK3		2
#define TAG_TANK4		3
#define TAG_MAP			4
#define TAG_SIZE		5
#define TAG_MODE		6
#define TAG_DONE		7

Scene* GameSettingsScene::createScene()
{
	auto scene = Scene::create();
	auto layer = GameSettingsScene::create();
	scene->addChild(layer);
	return scene;
}

bool GameSettingsScene::init()
{
	if (!BaseScene::init(Color4B(DARKGREEN_COLOR)))
		return false;

	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto title = Label::createWithCharMap(FONT_MAIN);
	title->setString("GAME SETTINGS");
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

	auto tank1Item = SimpleMenuItem::create("TANK 1\t\t%s", FONT_MAIN);
	tank1Item->setValues({ "NONE", "HUMAN", "CPU" });
	tank1Item->setCurrentValueIndex((int)_engine->GameSettings.tankTypes[0]);
	tank1Item->setActivateCallback(CC_CALLBACK_1(GameSettingsScene::menuItemActivated, this));
	tank1Item->setValueChangedCallback(CC_CALLBACK_1(GameSettingsScene::menuItemValueChanged, this));
	tank1Item->setTag(TAG_TANK1);
	tank1Item->setAnchorPoint(Vec2(0, 0.5f));

	auto tank2Item = SimpleMenuItem::create("TANK 2\t\t%s", FONT_MAIN);
	tank2Item->setValues({ "NONE", "HUMAN", "CPU" });
	tank2Item->setCurrentValueIndex((int)_engine->GameSettings.tankTypes[1]);
	tank2Item->setActivateCallback(CC_CALLBACK_1(GameSettingsScene::menuItemActivated, this));
	tank2Item->setValueChangedCallback(CC_CALLBACK_1(GameSettingsScene::menuItemValueChanged, this));
	tank2Item->setTag(TAG_TANK2);
	tank2Item->setAnchorPoint(Vec2(0, 0.5f));

	auto tank3Item = SimpleMenuItem::create("TANK 3\t\t%s", FONT_MAIN);
	tank3Item->setValues({ "NONE", "HUMAN", "CPU" });
	tank3Item->setCurrentValueIndex((int)_engine->GameSettings.tankTypes[2]);
	tank3Item->setActivateCallback(CC_CALLBACK_1(GameSettingsScene::menuItemActivated, this));
	tank3Item->setValueChangedCallback(CC_CALLBACK_1(GameSettingsScene::menuItemValueChanged, this));
	tank3Item->setTag(TAG_TANK3);
	tank3Item->setAnchorPoint(Vec2(0, 0.5f));

	auto tank4Item = SimpleMenuItem::create("TANK 4\t\t%s", FONT_MAIN);
	tank4Item->setValues({ "NONE", "HUMAN", "CPU" });
	tank4Item->setCurrentValueIndex((int)_engine->GameSettings.tankTypes[3]);
	tank4Item->setActivateCallback(CC_CALLBACK_1(GameSettingsScene::menuItemActivated, this));
	tank4Item->setValueChangedCallback(CC_CALLBACK_1(GameSettingsScene::menuItemValueChanged, this));
	tank4Item->setTag(TAG_TANK4);
	tank4Item->setAnchorPoint(Vec2(0, 0.5f));

	auto mapItem = SimpleMenuItem::create("MAP\t\t\t\t\t%s", FONT_MAIN);
	mapItem->setValues({ "RANDOM", "CA CAVE" });
	mapItem->setCurrentValueIndex((int)_engine->GameSettings.mapType);
	mapItem->setActivateCallback(CC_CALLBACK_1(GameSettingsScene::menuItemActivated, this));
	mapItem->setValueChangedCallback(CC_CALLBACK_1(GameSettingsScene::menuItemValueChanged, this));
	mapItem->setTag(TAG_MAP);
	mapItem->setAnchorPoint(Vec2(0, 0.5f));

	auto sizeItem = SimpleMenuItem::create("SIZE\t\t\t\t%s", FONT_MAIN);
	sizeItem->setValues({ "SMALL", "MEDIUM", "LARGE" });
	sizeItem->setCurrentValueIndex((int)_engine->GameSettings.mapSize);
	sizeItem->setActivateCallback(CC_CALLBACK_1(GameSettingsScene::menuItemActivated, this));
	sizeItem->setValueChangedCallback(CC_CALLBACK_1(GameSettingsScene::menuItemValueChanged, this));
	sizeItem->setTag(TAG_SIZE);
	sizeItem->setAnchorPoint(Vec2(0, 0.5f));

	auto modeItem = SimpleMenuItem::create("MODE\t\t\t\t%s", FONT_MAIN);
	modeItem->setValues(_engine->getGameModes());
	modeItem->setCurrentValue(_engine->GameSettings.currentGameMode);
	modeItem->setActivateCallback(CC_CALLBACK_1(GameSettingsScene::menuItemActivated, this));
	modeItem->setValueChangedCallback(CC_CALLBACK_1(GameSettingsScene::menuItemValueChanged, this));
	modeItem->setTag(TAG_MODE);
	modeItem->setAnchorPoint(Vec2(0, 0.5f));

	auto doneItem = SimpleMenuItem::create("DONE", FONT_MAIN, CC_CALLBACK_1(GameSettingsScene::menuItemActivated, this));
	doneItem->setTag(TAG_DONE);
	doneItem->setAnchorPoint(Vec2(0, 0.5f));

	auto menu = SimpleMenu::create(SimpleMenu::Type::VERTICAL, tank1Item, tank2Item, tank3Item, tank4Item, 
		mapItem, sizeItem, modeItem, doneItem, NULL);
	menu->setPosition(Vec2(visibleSize.width / 2 - 130, visibleSize.height / 2));
	menu->alignItemsVerticallyWithPadding(16);
	menu->setCurrentMenuItemTag(TAG_DONE);
	this->addChild(menu, 1);

	return true;
}

void GameSettingsScene::menuItemActivated(Ref* pSender)
{
	auto pMenuItem = (MenuItem*)pSender;
	onMenuItemActivated(pMenuItem->getTag());
}

void GameSettingsScene::menuItemValueChanged(Ref* pSender)
{
	auto pMenuItem = (SimpleMenuItem*)pSender;
	int  currentValue = pMenuItem->getCurrentValueIndex();

	switch (pMenuItem->getTag())
	{
	case TAG_TANK1:
		_engine->GameSettings.tankTypes[0] = (GameSettings::TankType)currentValue;
		break;
	case TAG_TANK2:
		_engine->GameSettings.tankTypes[1] = (GameSettings::TankType)currentValue;
		break;
	case TAG_TANK3:
		_engine->GameSettings.tankTypes[2] = (GameSettings::TankType)currentValue;
		break;
	case TAG_TANK4:
		_engine->GameSettings.tankTypes[3] = (GameSettings::TankType)currentValue;
		break;
	case TAG_MAP:
		_engine->GameSettings.mapType = (GameSettings::MapType)currentValue;
		break;
	case TAG_SIZE:
		_engine->GameSettings.mapSize = (GameSettings::MapSize)currentValue;
		break;
	case TAG_MODE:
		_engine->GameSettings.currentGameMode = pMenuItem->getCurrentValue();
		break;
	default:
		break;
	}
}

void GameSettingsScene::onKeyEvent(EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_BACK)
		CHANGE_SCENE(MainMenuScene);
}

void GameSettingsScene::onMenuItemActivated(int menuItem)
{
	switch (menuItem)
	{
	case TAG_DONE:

		int tanksInGame = 0;
		for (int i = 0; i < MAX_TEAMS_COUNT; i++) {
			if (_engine->GameSettings.tankTypes[i] != GameSettings::TankType::NONE)
				tanksInGame++;
		}

		if (tanksInGame < 2)
		{
			auto playersMessage = ModalDialog::create("Please select at least two tanks!", 
				nullptr, nullptr, "OK", "");
			this->addChild(playersMessage, LayerZOrder::MODAL_DIALOGS);
		}
		else 
		{
			CHANGE_SCENE(GameplayScene);
		}

		return;
	}
}