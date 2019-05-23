#ifndef __GAMEPLAYSCENE_H__
#define __GAMEPLAYSCENE_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Objects/Tank.h"

class TiledSprite;
class LevelGrid;

class GameplayScene : public BaseScene
{
	friend class Tank;
public:
	CREATE_FUNC(GameplayScene);

	static cocos2d::Scene* createScene();

	virtual bool init();

	~GameplayScene();

	void addGameObject(GameObject* obj);
	void removeGameObject(GameObject* obj);
	GameObject* findGameObject(Pos2 pos) const;
	GameObject* findGameObject(int x, int y) const;
	GameObject* findGameObject(std::function<bool(GameObject*)> pred) const;
	Tank* findTank(Team team) const;
	bool findRandomEmptyTile(Pos2& result);
	bool findRandomEmptyTile(Pos2& result, const Pos2& accessibleFrom);

	LevelGrid* getGrid() const { return _levelGrid; }
	const cocos2d::Vector<GameObject*>& getGameObjects() const { return _gameObjects; }

	int getTankScore(Team team) const;

	multifunction<void(GameObject*)>& getGameObjectAddedEvent() { return _gameObjectAddedEvent; }
	multifunction<void(GameObject*)>& getGameObjectRemovedEvent() { return _gameObjectRemovedEvent; }

private:
	cocos2d::Vector<GameObject*> _gameObjects;
	LevelGrid* _levelGrid;
	TiledSprite* _background;

	cocos2d::Label* _statusLabel;

	multifunction<void(GameObject*)> _gameObjectAddedEvent;
	multifunction<void(GameObject*)> _gameObjectRemovedEvent;

	bool _paused;
	int _highScores[MAX_TEAMS_COUNT];

	GameplayScene();

	void createStatusLable();
	void updateStatusLabel();
	void backHomeDialog();
	void backHomeDialogCanceled();
	void pauseGame();
	void resumeGame();
	void roundFinished(const Tank* winner);
	void restartGame();
	bool checkReachability(Tank* tankA, Tank* tankB);
	bool addRandomBonus(bool playSound = true);
	void leaveGame();
	void onTankKilled(Tank* tank);
	void onKeyEvent(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event) override;
};

#endif // __GAMEPLAYSCENE_H__
