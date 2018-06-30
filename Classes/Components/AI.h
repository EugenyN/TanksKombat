#ifndef  _AI_H_
#define  _AI_H_

#include "Engine.h"
#include "Objects\GameObject.h"
#include "Objects\Tank.h"

class Bonus;

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

	virtual bool init() override;
	virtual void onAdd() override;
private:
	States _currentState = States::UNDEFINED;
	GameObject* _target = nullptr;
	Pos2 _escapePoint;

	float _aiSpeed = 0.5f;
	float _chanceToEscape = 0.5f;
	float _thinkTime = 3.0f;

	AI();
	virtual ~AI();

	void updateStep();
	void makeDecision();

	void onGameObjectRemoved(const GameObject* gameObject);
	bool isItPossibleToAttack();
	bool isItPossibleToAttackEnemy(const Tank* enemy);
	bool isItPossibleToAttackInDirection(Tank::Direction direction);

	void pursuitStart(Tank* target);
	void pursuitUpdate();
	void attackStart(Tank::Direction fireLineDirection, Tank* enemyOnLine);
	void attackUpdate();
	void escapeStart(Pos2 escapePoint);
	void escapeUpdate();
	void thinkStart();
	void thinkUpdate();
	void getBonusStart(Bonus* target);
	void getBonusUpdate();

	Tank* findNearestTank() const;
	Bonus * findNearestBonus() const;
	bool findEscapePoint(/*out*/ Pos2& point) const;
	bool isEnemyOnFiringLine(/*out*/ GameObject::Direction& fireLineDirection, /*out*/ Tank*& enemyOnLine) const;
	bool isEnemyOnFiringLine(GameObject::Direction direction, const Pos2& ownerPos, const Pos2& enemyPos) const;
};

#endif // _AI_H_