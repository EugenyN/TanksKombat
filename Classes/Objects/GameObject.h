#ifndef  _GAMEOBJECT_H_
#define  _GAMEOBJECT_H_

#include "Engine.h"

class Engine;

class GameObject : public cocos2d::Node
{
public:
	enum class Type : int
	{
		TANK = 0, BONUS, EXPLOSION, PROJECTILE
	};

	enum class Direction : int
	{
		UP, DOWN, LEFT, RIGHT
	};

	GameObject(Type type);
	virtual ~GameObject();

	Pos2 getGridPosition() const;
	void setGridPosition(const Pos2& p);
	void setGridPosition(int x, int y);

	static Pos2 getGridPosition(const cocos2d::Node* node);
	static void setGridPosition(cocos2d::Node* node, const Pos2& p);
	static void setGridPosition(cocos2d::Node* node, int x, int y);
	static void setGridDirection(cocos2d::Node* node, Direction direction);
	static Pos2 directionToOffset(Direction direction);
	static Direction offsetToDirection(Pos2 offset);
	static float directionToRotation(Direction direction);

	static void updatePassableLayer(const Pos2& pos, const Pos2& prevPos);
	static void removeFromPassableLayer(const Pos2& pos);

	CC_PROPERTY(Direction, _direction, GridDirection);
	CC_PROPERTY(cocos2d::Sprite*, _sprite, Sprite);
	CC_SYNTHESIZE_READONLY(Type, _type, Type);
	CC_SYNTHESIZE(bool, _isPassable, Passable);

	virtual bool init() override;
protected:
	Engine* _engine;
	Pos2 _position;
};

#endif // _GAMEOBJECT_H_