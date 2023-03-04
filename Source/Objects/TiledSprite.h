#ifndef  _TILEDSPRITE_H_
#define  _TILEDSPRITE_H_

#include "cocos2d.h"

class TiledSprite : public cocos2d::Sprite
{
public:
	static TiledSprite* createWithSprite(cocos2d::Sprite* sourceSprite, int width, int height);
private:
	bool initWithTX(cocos2d::Sprite* sourceSprite, int width, int height);
};

#endif // _TILEDSPRITE_H_