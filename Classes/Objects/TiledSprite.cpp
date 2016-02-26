#include "TiledSprite.h"

USING_NS_CC;

TiledSprite* TiledSprite::createWithSprite(Sprite* sourceSprite, int width, int height)
{
	auto ts = new (std::nothrow) TiledSprite();
	if (ts && ts->initWithTX(sourceSprite, width, height))
	{
		ts->autorelease();
		return ts;
	}
	CC_SAFE_DELETE(ts);
	return nullptr;
}

bool TiledSprite::initWithTX(Sprite* sourceSprite, int width, int height)
{
	CC_ASSERT(width > 0 && height > 0);

	if (!Sprite::init())
		return false;

    const Rect& spriteBounds = sourceSprite->getTextureRect();
    int sourceX = spriteBounds.origin.x;
    int sourceY = spriteBounds.origin.y;
    int sourceWidth = spriteBounds.size.width;
    int sourceHeight = spriteBounds.size.height;
    auto texture = sourceSprite->getTexture();
            
    // Case 1: both width and height are smaller than source sprite, just clip
    if (width <= sourceWidth && height <= sourceHeight)
    {
		auto sprite = CCSprite::createWithTexture(texture, Rect(sourceX, sourceY + sourceHeight - height, width, height));
        sprite->setAnchorPoint(Vec2(0, 0));
		addChild(sprite);
    }
    // Case 2: only width is larger than source sprite
    else if (width > sourceWidth && height <= sourceHeight)
    {
        // Stamp sideways until we can
        int ix = 0;
        while (ix < width - sourceWidth)
        {
			auto sprite = CCSprite::createWithTexture(texture, Rect(sourceX, sourceY + sourceHeight - height, sourceWidth, height));
			sprite->setAnchorPoint(Vec2(0, 0));
            sprite->setPosition(Vec2(ix, 0));
			addChild(sprite);
                    
            ix += sourceWidth;
        }
                
        // Stamp the last one
		auto sprite = CCSprite::createWithTexture(texture, Rect(sourceX, sourceY + sourceHeight - height, width - ix, height));
		sprite->setAnchorPoint(Vec2(0, 0));
		sprite->setPosition(Vec2(ix, 0));
		addChild(sprite);
    }
    // Case 3: only height is larger than source sprite
    else if (height >= sourceHeight && width <= sourceWidth)
    {
        // Stamp down until we can
        int iy = 0;
        while (iy < height - sourceHeight)
        {
			auto sprite = CCSprite::createWithTexture(texture, Rect(sourceX, sourceY, width, sourceHeight));
			sprite->setAnchorPoint(Vec2(0, 0));
			sprite->setPosition(Vec2(0, iy));
			addChild(sprite);
                    
            iy += sourceHeight;
        }
            
        // Stamp the last one
        int remainingHeight = height - iy;
		auto sprite = CCSprite::createWithTexture(texture, Rect(sourceX, sourceY + sourceHeight - remainingHeight, width, remainingHeight));
		sprite->setAnchorPoint(Vec2(0, 0));
		sprite->setPosition(Vec2(0, iy));
		addChild(sprite);
    }
    // Case 4: both width and height are larger than source sprite (Composite together several Case 2's, as needed)
    else
    {            
        // Stamp down until we can
        int iy = 0;
        while (iy < height - sourceHeight)
        {
			auto sprite = TiledSprite::createWithSprite(sourceSprite, width, sourceHeight);
			sprite->setAnchorPoint(Vec2(0, 0));
			sprite->setPosition(Vec2(0, iy));
			addChild(sprite);
                    
            iy += sourceHeight;
        }
                
        // Stamp the last one
		auto sprite = TiledSprite::createWithSprite(sourceSprite, width, height - iy);
		sprite->setAnchorPoint(Vec2(0, 0));
		sprite->setPosition(Vec2(0, iy));
		addChild(sprite);
    }

    return true;
}