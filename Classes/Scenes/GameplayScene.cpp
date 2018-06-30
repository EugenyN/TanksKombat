#include "GameplayScene.h"
#include "MainMenuScene.h"
#include "GUI\ModalDialog.h"
#include "AppDelegate.h"
#include "Objects\LevelGrid.h"
#include "Objects\Bonus.h"
#include "Objects\Explosion.h"
#include "Objects\TiledSprite.h"
#include "Objects\LevelGrid.h"
#include "Components\Pathfinder.h"


USING_NS_CC;

Scene* GameplayScene::createScene()
{
    auto scene = Scene::create();
    auto layer = GameplayScene::create();
    scene->addChild(layer);
    return scene;
}

GameplayScene::GameplayScene()
	: _statusLabel(nullptr), _levelGrid(nullptr)
{
}

GameplayScene::~GameplayScene()
{
	_gameObjects.clear();
}

bool GameplayScene::init()
{
	_paused = false;

	auto engine = Engine::getInstance();
	if (!BaseScene::init(Color4B(DARKGREEN_COLOR)))
		return false;

	createStatusLable();
	restartGame();

	std::fill(_highScores, _highScores + MAX_TEAMS_COUNT, 0);

	// bonus action

	auto bonusCreation = RepeatForever::create(Sequence::create(
		DelayTime::create(random(_engine->GameMode.newBonusTime / 2, _engine->GameMode.newBonusTime)),
		CallFunc::create([this]() { addRandomBonus(); }), nullptr)
		);
	this->runAction(bonusCreation);

	return true;
}

void GameplayScene::leaveGame() 
{
	CHANGE_SCENE(MainMenuScene);
}

void GameplayScene::restartGame()
{
	resumeGame();

	_engine->setGameMode(_engine->GameSettings.currentGameMode);

	// background

	if (_background != nullptr)
		this->removeChild(_background);

	auto designSize = Director::getInstance()->getWinSize();

	const int backgroundsCount = 23;
	auto bgSpriteName = StringUtils::format("graphics/background/%d.png", 
		RandomHelper::random_int(1, backgroundsCount));
	auto bgSprite = Sprite::create(bgSpriteName);
	bgSprite->getTexture()->setAliasTexParameters();
	_background = TiledSprite::createWithSprite(bgSprite, designSize.width, designSize.height);
	_background->setColor(Color3B(90, 90, 90));
	_engine->setEnableRecursiveCascading(_background, true);
	addChild(_background, LayerZOrder::BACKGROUND);

	// level grid

	if (_levelGrid != nullptr)
		this->removeChild(_levelGrid);

	Size size;
	if (_engine->GameSettings.mapSize == GameSettings::MapSize::SMALL)
		size = Size(11, 8);
	else if(_engine->GameSettings.mapSize == GameSettings::MapSize::MEDIUM)
		size = Size(13, 13);
	else
		size = Size(GRID_MAX_WIDTH, GRID_MAX_HEIGHT);

	if (_engine->GameSettings.mapType == GameSettings::MapType::RANDOM) {
		_levelGrid = LevelGrid::createRandom(size, 0.2f, 0.05f);
	} else if (_engine->GameSettings.mapType == GameSettings::MapType::CA_CAVE) {
		CAParams params;
		params.birthLimit = 4;
		params.deathLimit = 3;
		params.chanceToStartAlive = 0.4f;
		params.numberOfSteps = 1;
		_levelGrid = LevelGrid::createUsingCA(size, params, 0.05f);
	}

	float yOffset = 0;
	if (_levelGrid->getMapSize().height == GRID_MAX_HEIGHT)
		yOffset = _levelGrid->getTileSize().height / 2;
	_levelGrid->setPosition(designSize.width / 2, designSize.height / 2 - yOffset);
	this->addChild(_levelGrid, LayerZOrder::GRID);

	// game objects

	Explosion::clearPool();

	_gameObjects.clear();

	auto emptyTiles = _levelGrid->findTiles(TileValue::GROUND);

	std::random_device rng;
	std::mt19937 urng(rng());
	std::shuffle(emptyTiles.begin(), emptyTiles.end(), rng);

	int tileIndex = 0;

	Tank* prevTank = nullptr;
	for (int i = 0; i < MAX_TEAMS_COUNT; i++)
	{
		if (_engine->GameSettings.tankTypes[i] == GameSettings::TankType::NONE)
			continue;

		auto tank = Tank::create();
		tank->setTeam((Team)i);
		tank->setLives(_engine->GameMode.startLives);
		tank->setAmmo(_engine->GameMode.startAmmo);
		tank->setAIEnabled(_engine->GameSettings.tankTypes[i] == GameSettings::TankType::CPU);

		tank->setGridPosition(emptyTiles[tileIndex++]);
		tank->setGridDirection((Tank::Direction)RandomHelper::random_int(0, 3));

		addGameObject(tank);

		if (prevTank != nullptr) {
			if (!checkReachability(tank, prevTank))
				_levelGrid->digPath(tank->getGridPosition(), prevTank->getGridPosition());
		}
		prevTank = tank;
	}

	int bonusCount = RandomHelper::random_int(0, 5);
	for (int i = 0; i < bonusCount; i++)
		addRandomBonus(false);

	//

	updateStatusLabel();
}

bool GameplayScene::checkReachability(Tank* tankA, Tank* tankB)
{
	auto pf = dynamic_cast<Pathfinder*>(tankA->getComponent(Pathfinder::COMPONENT_NAME));
	if (pf != nullptr)
		return pf->isPathExist(tankA->getGridPosition(), tankB->getGridPosition());

	return false;
}

bool GameplayScene::addRandomBonus(bool playSound)
{
	Pos2 emptyTile;
	if (findRandomEmptyTile(emptyTile))
	{
		auto bonus = Bonus::create();
		bonus->setGridPosition(emptyTile);
		addGameObject(bonus);

		if (playSound)
			_engine->playSound("bonus_respawn");
	}

	return false;
}

bool GameplayScene::findRandomEmptyTile(Pos2& result)
{
	auto emptyTiles = _levelGrid->findTiles(TileValue::GROUND);
	if (emptyTiles.size() == 0)
		return false;

	std::random_device rng;
	std::mt19937 urng(rng());
	std::shuffle(emptyTiles.begin(), emptyTiles.end(), rng);

	for (size_t i = 0; i < emptyTiles.size(); i++)
	{
		if (findGameObject(emptyTiles[i]) == nullptr) {
			result = emptyTiles[i];
			return true;
		}
	}
	return false;
}

bool GameplayScene::findRandomEmptyTile(Pos2& result, const Pos2& accessibleFrom)
{
	auto emptyTiles = _levelGrid->findTiles(TileValue::GROUND);
	if (emptyTiles.size() == 0)
		return false;

	auto pf = Pathfinder::create();

	std::random_device rng;
	std::mt19937 urng(rng());
	std::shuffle(emptyTiles.begin(), emptyTiles.end(), rng);

	for (size_t i = 0; i < emptyTiles.size(); i++)
	{
		if (findGameObject(emptyTiles[i]) == nullptr) {

			if (!pf->isPathExist(accessibleFrom, emptyTiles[i]))
				continue;

			result = emptyTiles[i];
			return true;
		}
	}
	return false;
}

int GameplayScene::getTankScore(Team team) const
{
	return _highScores[(int)team];
}

void GameplayScene::createStatusLable()
{
	_statusLabel = Label::createWithCharMap(FONT_MAIN);
	_statusLabel->setString("T1 00-00 T2 00-00 T3 00-00 T4 00-00");
	_statusLabel->getLetter(0)->setColor(Color3B(207, 120, 75));
	_statusLabel->getLetter(1)->setColor(Color3B(207, 120, 75));
	_statusLabel->getLetter(9)->setColor(Color3B(104, 120, 150));
	_statusLabel->getLetter(10)->setColor(Color3B(104, 120, 150));
	_statusLabel->getLetter(18)->setColor(Color3B(207, 120, 150));
	_statusLabel->getLetter(19)->setColor(Color3B(207, 120, 150));
	_statusLabel->getLetter(27)->setColor(Color3B(207, 239, 75));
	_statusLabel->getLetter(28)->setColor(Color3B(207, 239, 75));

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	_statusLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - _statusLabel->getContentSize().height));

	this->addChild(_statusLabel, LayerZOrder::HUD);
}

void GameplayScene::updateStatusLabel()
{
	std::string status;

	for (int i = 0; i < MAX_TEAMS_COUNT; i++)
	{
		const auto tank = findTank((Team)i);
		if (tank == nullptr || tank->getLives() == 0)
			status += StringUtils::format("T%d ----- ", i + 1);
		else
			status += StringUtils::format("T%d %02d-%02d ", i + 1, tank->getLives(), tank->getAmmo());
	}

	_statusLabel->setString(status);
}

#define GAME_END_ACTION 600

void GameplayScene::onTankKilled(Tank* tank)
{
	int tanksCount = 0;
	Tank* winner = nullptr;
	for (auto obj : _gameObjects) {
		if (dynamic_cast<Tank*>(obj) != nullptr) {
			tanksCount++;
			winner = dynamic_cast<Tank*>(obj);
		}
	}

	if (tanksCount == 0 || tanksCount == 1)
	{
		this->stopActionByTag(GAME_END_ACTION);

		auto gameEndAction = Sequence::createWithTwoActions(
			DelayTime::create(1.0f),
			CallFunc::create([this, winner]() { roundFinished(winner); })
		);
		gameEndAction->setTag(GAME_END_ACTION);
		this->runAction(gameEndAction);
	}
}

void GameplayScene::roundFinished(const Tank* winner)
{
	pauseGame();

	if (winner != nullptr)
		_highScores[(int)winner->getTeam()]++;

	std::string highScore = "SCORES:\n\n";
	for (int i = 0; i < MAX_TEAMS_COUNT; i++)
		highScore += StringUtils::format("TANK %d : %d\n", i + 1, _highScores[i]);

	std::string message;
	if (winner != nullptr)
		message = StringUtils::format("TANK %d WINS !\n\n", (int)winner->getTeam() + 1);
	else
		message = "NOBODY WINS !\n\n";

	message += highScore;

	auto roundFinishedDialog = ModalDialog::create(message, CC_CALLBACK_0(GameplayScene::restartGame, this),
		CC_CALLBACK_0(GameplayScene::leaveGame, this), "CONTINUE", "EXIT");
	this->addChild(roundFinishedDialog, LayerZOrder::MODAL_DIALOGS);
}

void GameplayScene::pauseGame()
{
	_paused = true;

	_actionManager->pauseTarget(this);

	for (auto obj : _gameObjects)
		obj->pause();
}

void GameplayScene::resumeGame()
{
	_paused = false;

	_actionManager->resumeTarget(this);

	for (auto obj : _gameObjects)
		obj->resume();
}

void GameplayScene::onKeyEvent(EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_BACK)
		backHomeDialog();
}

void GameplayScene::backHomeDialog()
{
	pauseGame();

	static ModalDialog* leaveDialog = nullptr;
	if (leaveDialog == nullptr || leaveDialog->isValid() == false) {
		leaveDialog = ModalDialog::create("If you go back to home page, your current game will be lost, are you sure?",
			CC_CALLBACK_0(GameplayScene::leaveGame, this), CC_CALLBACK_0(GameplayScene::backHomeDialogCanceled, this));
		leaveDialog->setReferencer(&leaveDialog);
		this->addChild(leaveDialog, LayerZOrder::MODAL_DIALOGS);
	}
}

void GameplayScene::backHomeDialogCanceled()
{
	resumeGame();
}

void GameplayScene::onKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
	BaseScene::onKeyPressed(code, event);

	if (_paused)
		return;

	for (int i = 0; i < MAX_TEAMS_COUNT; i++)
	{
		auto team = (Team)i;
		if (_engine->isActionExist(team, code))
		{
			auto tank = findTank(team);
			if (tank != nullptr && !tank->isAIEnabled())
				tank->doAction(_engine->getActionForKey(team, code));
		}
	}
}

void GameplayScene::addGameObject(GameObject * obj)
{
	CCASSERT(obj != nullptr, "Argument must be non-nil");

	_gameObjects.pushBack(obj);
	_levelGrid->addChild(obj, LayerZOrder::OBJECTS);

	_gameObjectAddedEvent(obj);
}

void GameplayScene::removeGameObject(GameObject * obj)
{
	CCASSERT(_gameObjects.contains(obj), "Scene should contain the object");

	ssize_t index = _gameObjects.getIndex(obj);
	if (index != CC_INVALID_INDEX) {
		_gameObjects.erase(index);

		_gameObjectRemovedEvent(obj);

		_levelGrid->removeChild(obj);
	}
}

GameObject* GameplayScene::findGameObject(Pos2 pos) const
{
	return findGameObject(pos.x, pos.y);
}

GameObject* GameplayScene::findGameObject(int x, int y) const
{
	return findGameObject([x, y](const GameObject* obj)
	{ 
		auto pos = obj->getGridPosition();
		return pos.x == x && pos.y == y;
	});
}

GameObject* GameplayScene::findGameObject(std::function<bool(GameObject*)> pred) const
{
	auto iter = std::find_if(_gameObjects.begin(), _gameObjects.end(), pred);
	if (iter != _gameObjects.end())
		return *iter;
	return nullptr;
}

Tank* GameplayScene::findTank(Team team) const
{
	return dynamic_cast<Tank*>(findGameObject([team](GameObject* obj)
	{
		if (obj->getType() == GameObject::Type::TANK)
		{
			const auto tank = dynamic_cast<Tank*>(obj);
			return tank->getTeam() == team;
		}
		return false;
	}));
}