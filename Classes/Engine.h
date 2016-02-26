#ifndef  _ENGINE_H_
#define  _ENGINE_H_

#include "cocos2d.h"
#include "Math\Pos2.h"
#include "Utils\multicast.hpp"
#include <string>
#include <array>
#include <sstream>


#define GAME_TITLE "Tanks Kombat"
#define GAME_VERSION "1.0.13"

#define DESIGN_WIDTH 640
#define DESIGN_HEIGHT 480
#define TILE_SIZE 32
#define DISPLAY_STATS false

#define JOYSTICK_SCALE 0.6f

#if CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
	#define JOYSTICK_OFFSET Point(40, 8)
#else
	#define JOYSTICK_OFFSET Point(8, 8)
#endif

#define FIRE_BUTTON_OFFSET Point(8, 48)

#define INT2COLOR4B(color) Color4B((color & 0xFF0000) >> 16, (color & 0x00FF00) >> 8, color & 0x0000FF, 255)
#define INT2COLOR4BA(color, alpha) Color4B((color & 0xFF0000) >> 16, (color & 0x00FF00) >> 8, color & 0x0000FF, alpha)
#define INT2COLOR4F(color) Color4F(((color & 0xFF0000) >> 16)/255.0f, ((color & 0x00FF00) >> 8)/255.0f, (color & 0x0000FF)/255.0f, 1.0f)
#define INT2COLOR4FA(color, alpha) Color4F(((color & 0xFF0000) >> 16)/255.0f, ((color & 0x00FF00) >> 8)/255.0f, (color & 0x0000FF)/255.0f, alpha)
#define INT2COLOR3B(color) Color3B((color & 0xFF0000) >> 16, (color & 0x00FF00) >> 8, color & 0x0000FF)

#define FONT_MAIN "fonts/main_font.png", 17, 16, ' '
#define FONT_LOGO "fonts/logo_font.png", 32, 32, ' '


#ifndef NDEBUG
	#define SCENE_TRANSITION false
	#define PATHFINDING_DEBUG true
#else
	#define SCENE_TRANSITION true
	#define PATHFINDING_DEBUG false
#endif


#define UD_FULLSCREEN "fullscreen"
#define UD_SCREEN_WIDTH "screenWidth"
#define UD_SCREEN_HEIGHT "screenHeight"
#define UD_MUSIC_VOLUME "musicVolume"
#define UD_SOUND_VOLUME "soundVolume"
#define UD_MUSIC_ENABLED "musicEnabled"
#define UD_SOUND_ENABLED "soundEnabled"

#define LIGHTGREEN_COLOR Color3B(66, 99, 0)
#define DARKGREEN_COLOR Color3B(20, 30, 20)

#define MAX_TEAMS_COUNT 4
#define GRID_MAX_WIDTH 20
#define GRID_MAX_HEIGHT 14

// define audio extension
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define AUDIO_EXT ".ogg"
#define MUSIC_EXT ".ogg"
#define stdp stdPatchForMinGW
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#define AUDIO_EXT ".ogg" // ".wav"
#define MUSIC_EXT ".mp3" // win32 audio engine can not play ogg music
#define stdp std
#else
#define AUDIO_EXT ".ogg"
#define MUSIC_EXT ".ogg"
#define stdp std
#endif

#define BACKGROUND_MUSIC "music/main" MUSIC_EXT

struct TouchEventsFunc
{
	int id;
	std::function<bool(cocos2d::Touch*, cocos2d::Event*)> onTouchBegan;
	std::function<void(cocos2d::Touch*, cocos2d::Event*)> onTouchMoved;
	std::function<void(cocos2d::Touch*, cocos2d::Event*)> onTouchEnded;
	std::function<void(cocos2d::Touch*, cocos2d::Event*)> onTouchCancelled;
};

struct KeyboardEventsFunc
{
	int id;
	std::function<bool(cocos2d::EventKeyboard::KeyCode, cocos2d::Event*)> onKeyPressed;
	std::function<bool(cocos2d::EventKeyboard::KeyCode, cocos2d::Event*)> onKeyReleased;
};

struct Settings 
{
	bool fullscreen;
	cocos2d::Size windowsSize;

	int musicVolume;
	int soundVolume;
	bool musicEnabled;
	bool soundEnabled;
};

struct GameSettings
{
	enum class TankType
	{
		NONE = 0, HUMAN, CPU
	};

	enum class MapType
	{
		RANDOM = 0, CA_CAVE, FILE
	};

	enum class MapSize
	{
		SMALL = 0, MEDIUM, LARGE
	};

	std::array<TankType, MAX_TEAMS_COUNT> tankTypes = std::array<TankType, MAX_TEAMS_COUNT> {
		TankType::HUMAN, TankType::CPU, TankType::NONE, TankType::NONE 
	};
	MapType mapType = MapType::RANDOM;
	MapSize mapSize = MapSize::MEDIUM;
	std::string map;
	std::string currentGameMode;
};

struct GameMode
{
	int startLives = 5;
	int startAmmo = 20;
	float ghostTime = 3.0f;
	float newBonusTime = 5.0f;
	int ammoInBonus = 10;

	static GameMode createFromDict(const cocos2d::ValueMap& dict);
	void writeToDict(cocos2d::ValueMap& dict);
};

enum LayerZOrder
{
	BACKGROUND = 0, GRID, GROUND, WALL, OBJECTS, HUD, MODAL_DIALOGS
};

enum class Team
{
	RED = 0, BLUE, GREEN, YELLOW
};

enum class TeamColor
{
	RED = 0xff8080,
	BLUE = 0x8080ff,
	GREEN = 0xff80ff,
	YELLOW = 0xffff80
};

const std::array<TeamColor, MAX_TEAMS_COUNT> teamColors = {
	TeamColor::RED, TeamColor::BLUE, TeamColor::GREEN, TeamColor::YELLOW 
};

#define GET_TEAM_COLOR3B(team) Color3B(INT2COLOR3B((int)teamColors[(int)team]))

enum class TankAction
{
	MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT, FIRE
};

typedef std::map<cocos2d::EventKeyboard::KeyCode, TankAction> KeyMap;

class BaseScene;

class Engine
{
public:
	static Engine* getInstance();
	static void destroyInstance();

	struct Settings Settings;
	struct GameSettings GameSettings;
	struct GameMode GameMode;

	void launchGame();
	void enterBackground();
	void enterForeground();
	void loadSettings();
	void saveSettings();

	void playSound(const std::string& filePath);
	void stopAllSound();
	void playMusic(const std::string& filePath);
	void stopMusic();
	void setMusicVolume(float volume);

	BaseScene* getCurrentScene() const;

	template<typename T> T* getCurrentScene() const // where T : BaseScene
	{
		return dynamic_cast<T*>(getCurrentScene());
	}

	std::vector<std::string> getGameModes() const;
	void setGameMode(const std::string& modeName);

	void setEnableRecursiveCascading(cocos2d::Node* node, bool enable);

	bool isActionExist(Team team, cocos2d::EventKeyboard::KeyCode code) const;
	TankAction getActionForKey(Team team, cocos2d::EventKeyboard::KeyCode code) const;
private:
	static Engine* _instance;

	cocos2d::ValueMap _modes;
	cocos2d::SpriteBatchNode* _spriteBatch;
	int _bgMusicId = -1;

	std::array<KeyMap, MAX_TEAMS_COUNT> _keymaps;

	Engine();
	void loadGameModes();
	std::array<KeyMap, MAX_TEAMS_COUNT> createKeyMaps();
	void pauseAudio();
	void resumeAudio();
};

#endif // _ENGINE_H_
