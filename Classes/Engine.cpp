#include "Engine.h"
#include "Objects\LevelGrid.h"
#include "Scenes\BaseScene.h"
#include "Scenes\GameplayScene.h"
#include "Scenes\MainMenuScene.h"
//#include "SimpleAudioEngine.h"
#include "2d/CCFontAtlasCache.h"
#include "2d/CCFontAtlas.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;
using namespace cocos2d::experimental;


GameMode GameMode::createFromDict(const ValueMap& dict)
{
	GameMode mode;

	mode.startLives = dict.at("startLives").asInt();
	mode.startAmmo = dict.at("startAmmo").asInt();
	mode.ghostTime = dict.at("ghostTime").asFloat();
	mode.newBonusTime = dict.at("newBonusTime").asFloat();
	mode.ammoInBonus = dict.at("ammoInBonus").asInt();

	return mode;
}

void GameMode::writeToDict(ValueMap& dict)
{
	dict["startLives"] = startLives;
	dict["startAmmo"] = startAmmo;
	dict["ghostTime"] = ghostTime;
	dict["newBonusTime"] = newBonusTime;
	dict["ammoInBonus"] = ammoInBonus;
}


Engine* Engine::_instance = nullptr;

Engine* Engine::getInstance()
{
	if (!_instance)
	{
		_instance = new (std::nothrow) Engine();
		CCASSERT(_instance, "FATAL: Not enough memory");
	}

	return _instance;
}

Engine::Engine()
{ }

void Engine::destroyInstance()
{
	CC_SAFE_DELETE(_instance);
}

BaseScene* Engine::getCurrentScene() const
{
	return BaseScene::getCurrentScene();
}

void Engine::launchGame()
{
	loadSettings();

	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();

	if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		if (Settings.fullscreen)
			glview = GLViewImpl::createWithFullScreen(GAME_TITLE);
		else
			glview = GLViewImpl::createWithRect(GAME_TITLE, Rect(0, 0, Settings.windowsSize.width, Settings.windowsSize.height));
#else
		glview = GLViewImpl::create(GAME_TITLE);
#endif
		director->setOpenGLView(glview);
	}

	director->setDisplayStats(DISPLAY_STATS);
	director->setAnimationInterval(1.0f / 60.0f);
	glview->setDesignResolutionSize(DESIGN_WIDTH, DESIGN_HEIGHT, ResolutionPolicy::FIXED_HEIGHT);

	auto spritecache = SpriteFrameCache::getInstance();
	spritecache->addSpriteFramesWithFile("graphics/sprites32.plist");

	_spriteBatch = SpriteBatchNode::create("graphics/sprites32.png");
	_spriteBatch->getTexture()->setAliasTexParameters();
	_spriteBatch->retain();

	auto mainFontAtlas = FontAtlasCache::getFontAtlasCharMap(FONT_MAIN);
	mainFontAtlas->setAliasTexParameters();

	loadGameModes();

	_keymaps = createKeyMaps();

	playMusic(BACKGROUND_MUSIC);

	auto scene = MainMenuScene::createScene();
	director->runWithScene(scene);
}

void Engine::enterBackground()
{
	Director::getInstance()->stopAnimation();
	pauseAudio();
}

void Engine::enterForeground()
{
	Director::getInstance()->startAnimation();
	resumeAudio();
}

void Engine::loadSettings()
{
	auto ud = UserDefault::getInstance();

	Settings.fullscreen = ud->getBoolForKey(UD_FULLSCREEN, false);
	Settings.windowsSize = Size(ud->getIntegerForKey(UD_SCREEN_WIDTH, 960),
		ud->getIntegerForKey(UD_SCREEN_HEIGHT, 720));
	Settings.musicVolume = ud->getIntegerForKey(UD_MUSIC_VOLUME, 50);
	Settings.soundVolume = ud->getIntegerForKey(UD_SOUND_VOLUME, 100);
	Settings.musicEnabled = ud->getBoolForKey(UD_MUSIC_ENABLED, true);
	Settings.soundEnabled = ud->getBoolForKey(UD_SOUND_ENABLED, true);

	saveSettings();
}

void Engine::saveSettings()
{
	auto ud = UserDefault::getInstance();

	ud->setBoolForKey(UD_FULLSCREEN, Settings.fullscreen);
	ud->setIntegerForKey(UD_SCREEN_WIDTH, Settings.windowsSize.width);
	ud->setIntegerForKey(UD_SCREEN_HEIGHT, Settings.windowsSize.height);
	ud->setIntegerForKey(UD_MUSIC_VOLUME, Settings.musicVolume);
	ud->setIntegerForKey(UD_SOUND_VOLUME, Settings.soundVolume);
	ud->setBoolForKey(UD_MUSIC_ENABLED, Settings.musicEnabled);
	ud->setBoolForKey(UD_SOUND_ENABLED, Settings.soundEnabled);
}

void Engine::playSound(const std::string& filePath)
{
	if (!Settings.soundEnabled || Settings.soundVolume < 1.0f)
		return;

	std::string fullPath = "sfx/" + filePath + AUDIO_EXT;

	//auto ae = CocosDenshion::SimpleAudioEngine::getInstance();
	//ae->stopAllEffects();
	//ae->setEffectsVolume(Settings.soundVolume / 100.0f);
	//ae->playEffect(fullPath);

	AudioEngine::play2d(fullPath, false, Settings.soundVolume / 100.0f);
}

void Engine::stopAllSound()
{
	AudioEngine::stopAll();
}

void Engine::playMusic(const std::string& filePath)
{
	if (!Settings.musicEnabled)
		return;

	//setMusicVolume(Settings.musicVolume);
	//CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(filePath, true);

	_bgMusicId = AudioEngine::play2d(filePath, true, Settings.musicVolume / 100.0f);
}

void Engine::stopMusic()
{
	AudioEngine::stop(_bgMusicId);
}

void Engine::setMusicVolume(float volume)
{
	//CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(volume / 100.0f);
	if (_bgMusicId != AudioEngine::INVALID_AUDIO_ID)
		AudioEngine::setVolume(_bgMusicId, volume / 100.0f);
}

void Engine::pauseAudio()
{
	// if you use SimpleAudioEngine, it must be pause
	// SimpleAudioEngine::getInstance()->pauseBackgroundMusic();

	AudioEngine::pauseAll();
}

void Engine::resumeAudio()
{
	// if you use SimpleAudioEngine, it must resume here
	// SimpleAudioEngine::getInstance()->resumeBackgroundMusic();

	AudioEngine::resumeAll();
}

void Engine::loadGameModes()
{
	//auto config = ValueMap::create();
	//{
	//	auto modes = ValueMap::create();
	//	{
	//		auto standardMode = ValueMap::create();
	//		GameMode.writeToDict(standardMode);
	//		modes->setObject(standardMode, "standard");
	//	}
	//	config->setObject(modes, "modes");
	//}
	//config->writeToFile("config.plist");

	auto path = FileUtils::getInstance()->fullPathForFilename("config.plist");
	ValueMap config = FileUtils::getInstance()->getValueMapFromFile(path);
	if (config.size() > 0)
		_modes = config.at("modes").asValueMap();

	const auto& modes = getGameModes();
	if (modes.size() > 0)
		GameSettings.currentGameMode = modes[0];
}

std::vector<std::string> Engine::getGameModes() const
{
	std::vector<std::string> result;

	for(auto key : _modes)
		result.push_back(key.first);

	return result;
}

void Engine::setGameMode(const std::string& modeName)
{
	auto mode = _modes.at(modeName).asValueMap();
	GameMode = GameMode::createFromDict(mode);
}

void Engine::setEnableRecursiveCascading(Node* node, bool enable)
{
	node->setCascadeColorEnabled(enable);
	node->setCascadeOpacityEnabled(enable);

	auto& children = node->getChildren();
	for (const auto& child : children)
		setEnableRecursiveCascading(child, enable);
}

bool Engine::isActionExist(Team team, EventKeyboard::KeyCode code) const
{
	return _keymaps[(int)team].count(code) > 0;
}

TankAction Engine::getActionForKey(Team team, EventKeyboard::KeyCode code) const
{
	return _keymaps[(int)team].at(code);
}

std::array<KeyMap, MAX_TEAMS_COUNT> Engine::createKeyMaps()
{
	KeyMap t1
	{
		{ EventKeyboard::KeyCode::KEY_UP_ARROW , TankAction::MOVE_UP },
		{ EventKeyboard::KeyCode::KEY_DOWN_ARROW , TankAction::MOVE_DOWN },
		{ EventKeyboard::KeyCode::KEY_RIGHT_ARROW , TankAction::MOVE_RIGHT },
		{ EventKeyboard::KeyCode::KEY_LEFT_ARROW , TankAction::MOVE_LEFT },
		{ EventKeyboard::KeyCode::KEY_ENTER , TankAction::FIRE }
	};

	KeyMap t2
	{
		{ EventKeyboard::KeyCode::KEY_W, TankAction::MOVE_UP },
		{ EventKeyboard::KeyCode::KEY_S, TankAction::MOVE_DOWN },
		{ EventKeyboard::KeyCode::KEY_D, TankAction::MOVE_RIGHT },
		{ EventKeyboard::KeyCode::KEY_A, TankAction::MOVE_LEFT },
		{ EventKeyboard::KeyCode::KEY_TAB, TankAction::FIRE }
	};

	KeyMap t3
	{
		{ EventKeyboard::KeyCode::KEY_I,  TankAction::MOVE_UP },
		{ EventKeyboard::KeyCode::KEY_K, TankAction::MOVE_DOWN },
		{ EventKeyboard::KeyCode::KEY_J, TankAction::MOVE_RIGHT },
		{ EventKeyboard::KeyCode::KEY_L, TankAction::MOVE_LEFT },
		{ EventKeyboard::KeyCode::KEY_U, TankAction::FIRE }
	};

	KeyMap t4
	{
		{ EventKeyboard::KeyCode::KEY_8,  TankAction::MOVE_UP },
		{ EventKeyboard::KeyCode::KEY_2, TankAction::MOVE_DOWN },
		{ EventKeyboard::KeyCode::KEY_4, TankAction::MOVE_RIGHT },
		{ EventKeyboard::KeyCode::KEY_6, TankAction::MOVE_LEFT },
		{ EventKeyboard::KeyCode::KEY_MINUS, TankAction::FIRE }
	};

	return { t1, t2, t3, t4 };
}