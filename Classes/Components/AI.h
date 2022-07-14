#ifndef  _AI_H_
#define  _AI_H_

#include "Engine.h"
#include "Objects\GameObject.h"
#include "Objects\Tank.h"

class Bonus;
class EventCustom;

class AI : public cocos2d::Component
{
public:
	enum class States
	{
		UNDEFINED,
		PURSUIT,
		ATTACK,
		ESCAPE,
		THINK,
		GET_BONUS
	};

	const static std::string COMPONENT_NAME;

	CREATE_FUNC(AI);

	bool init() override;
	void onAdd() override;
	void onRemove() override;
private:
	States _currentState = States::UNDEFINED;
	GameObject* _target = nullptr;
	Pos2 _escapePoint;
	cocos2d::EventListenerCustom* _gameObjectRemovedEventListener;

	float _aiSpeed = 0.5f;
	float _chanceToEscape = 0.5f;
	float _thinkTime = 3.0f;

	AI();
	~AI() override;

	void updateStep();
	void makeDecision();

	void onGameObjectRemoved(cocos2d::EventCustom* eventCustom);
	bool isItPossibleToAttack();
	bool isItPossibleToAttackEnemy(const Tank* enemy);
	bool isItPossibleToAttackInDirection(Tank::Direction direction);

	void pursuitStart(Tank* target);
	void pursuitUpdate();
	void attackStart(Tank::Direction fireLineDirection, Tank* enemyOnLine);
	void attackUpdate();
	void escapeStart(const Pos2& escapePoint);
	void escapeUpdate();
	void thinkStart();
	void thinkUpdate();
	void getBonusStart(Bonus* target);
	void getBonusUpdate();

	Tank* findNearestTank() const;
	Bonus* findNearestBonus() const;
	bool findEscapePoint(/*out*/ Pos2& point) const;
	bool isEnemyOnFiringLine(/*out*/ GameObject::Direction& fireLineDirection, /*out*/ Tank*& enemyOnLine) const;
	static bool isEnemyOnFiringLine(GameObject::Direction direction, const Pos2& ownerPos, const Pos2& enemyPos);
};

#endif // _AI_H_