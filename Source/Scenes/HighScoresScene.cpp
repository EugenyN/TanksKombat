#include "AboutScene.h"
#include "HighScoresScene.h"
#include "GameplayScene.h"
#include "GUI\SimpleMenu.h"

USING_NS_CC;

#define TAG_CONTINUE	0
#define TAG_EXIT		1

Scene* HighScoresScene::createScene()
{
	auto scene = Scene::create();
	auto layer = AboutScene::create();
	scene->addChild(layer);
	return scene;
}

bool HighScoresScene::init()
{
	if (!BaseScene::init(Color4B(DARKGREEN_COLOR)))
		return false;

	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto title = Label::createWithCharMap(FONT_MAIN);
	title->setString("HIGH SCORES");
	title->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 32));
	title->enableShadow(Color4B(LIGHTGREEN_COLOR), Size(1, -1));
	this->addChild(title, 2);

	const auto scene = Engine::getInstance()->getCurrentScene<GameplayScene>();

	for (int i = 0; i < MAX_TEAMS_COUNT; i++)
	{
		auto content = Label::createWithCharMap(FONT_MAIN);
		content->setAlignment(TextHAlignment::CENTER);
		content->setColor(teamColors[i]);
		content->setString(StringUtils::format("TANK %d : %d", i + 1, scene->getTankScore((Team)i)));
		content->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 32 * i));
		this->addChild(content, 2);
	}

	//

	auto continueItem = SimpleMenuItem::create("CONTINUE", FONT_MAIN, CC_CALLBACK_1(HighScoresScene::menuItemActivated, this));
	continueItem->setTag(TAG_CONTINUE);
	continueItem->setAnchorPoint(Vec2(0, 0.5f));

	auto exitItem = SimpleMenuItem::create("EXIT", FONT_MAIN, CC_CALLBACK_1(HighScoresScene::menuItemActivated, this));
	exitItem->setTag(TAG_EXIT);
	exitItem->setAnchorPoint(Vec2(0, 0.5f));

	auto menu = SimpleMenu::create(SimpleMenu::Type::HORIZONTAL, continueItem, exitItem, NULL);
	menu->setPosition(Vec2(visibleSize.width / 2 - 60, visibleSize.height / 2 - 110));
	menu->alignItemsHorizontallyWithPadding(30);
	this->addChild(menu, 1);

	return true;
}

void HighScoresScene::menuItemActivated(Ref* pSender)
{
	auto pMenuItem = (MenuItem*)pSender;
	onMenuItemActivated(pMenuItem->getTag());
}

void HighScoresScene::onMenuItemActivated(int menuItem)
{
	switch (menuItem)
	{
	case TAG_CONTINUE:
		//CHANGE_SCENE(GameSettingsScene);
		break;
	case TAG_EXIT:
		//CHANGE_SCENE(SettingsScene);
		break;
	}
}

void HighScoresScene::onKeyEvent(EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
	// CHANGE_SCENE(HighScoresScene);

	// Back button pressed
	//if (keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
	//	CHANGE_SCENE(MainMenuScene);
	//}
}