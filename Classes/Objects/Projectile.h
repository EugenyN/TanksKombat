#ifndef  _PROJECTILE_H_
#define  _PROJECTILE_H_

#include "GameObject.h"
#include "Objects\LevelGrid.h"

class Tank;

class Projectile : public GameObject
{
public:
	CREATE_FUNC(Projectile);

	bool init() override;
	void move(Direction direction);
	void remove();

	CC_SYNTHESIZE(GameObject*, _ownerTank, OwnerTank);
private:
	Projectile();
	~Projectile();
	void onMoved();
	bool onCollidedWithBlock(Direction direction, const TileValue& value);
	bool onCollidedWithGameObject(Direction direction, GameObject* obj);
};

#endif // _PROJECTILE_H_