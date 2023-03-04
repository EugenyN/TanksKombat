#ifndef  _BONUS_H_
#define  _BONUS_H_

#include "GameObject.h"

class Bonus : public GameObject
{
public:
	CREATE_FUNC(Bonus);

	bool init() override;
	void kill();
	void take();
private:
	Bonus();
	~Bonus() override;
};

#endif // _BONUS_H_