#include "SneakyJoystick.h"

using namespace cocos2d;

#define SJ_PI 3.14159265359f
#define SJ_PI_X_2 6.28318530718f
#define SJ_RAD2DEG (180.0f/SJ_PI)
#define SJ_DEG2RAD (SJ_PI/180.0f)

SneakyJoystick::~SneakyJoystick()
{
}

void SneakyJoystick::initWithRect(const Rect& rect)
{
	stickPosition = Point::ZERO;
	degrees = 0.0f;
	velocity = Point::ZERO;
	autoCenter = true;
	isDPad = false;
	hasDeadzone = false;
	numberOfDirections = 4;

	this->setJoystickRadius(rect.size.width / 2);
	this->setThumbRadius(32.0f);
	this->setDeadRadius(0.0f);

	//Cocos node stuff
	setPosition(rect.origin);
}

void SneakyJoystick::onEnterTransitionDidFinish()
{
	// Register Touch Event
	_eventListenerTouch = EventListenerTouchOneByOne::create();
	_eventListenerTouch->setSwallowTouches(true);

	_eventListenerTouch->onTouchBegan = CC_CALLBACK_2(SneakyJoystick::onTouchBegan, this);
	_eventListenerTouch->onTouchMoved = CC_CALLBACK_2(SneakyJoystick::onTouchMoved, this);
	_eventListenerTouch->onTouchEnded = CC_CALLBACK_2(SneakyJoystick::onTouchEnded, this);
	_eventListenerTouch->onTouchCancelled = CC_CALLBACK_2(SneakyJoystick::onTouchCancelled, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(_eventListenerTouch, this);
}

void SneakyJoystick::onExit()
{
	_eventDispatcher->removeEventListener(_eventListenerTouch);
	Node::onExit();
}

void SneakyJoystick::updateVelocity(Point point)
{
	// Calculate distance and angle from the center.
	float dx = point.x;
	float dy = point.y;
	float dSq = dx * dx + dy * dy;

	if (dSq <= deadRadiusSq) {
		velocity = Point::ZERO;
		degrees = 0.0f;
		stickPosition = point;
		return;
	}

	float angle = atan2f(dy, dx); // in radians
	if (angle < 0) {
		angle += SJ_PI_X_2;
	}
	float cosAngle;
	float sinAngle;

	if (isDPad) {
		float anglePerSector = 360.0f / numberOfDirections * SJ_DEG2RAD;
		angle = round(angle / anglePerSector) * anglePerSector;
	}

	cosAngle = cosf(angle);
	sinAngle = sinf(angle);

	// NOTE: Velocity goes from -1.0 to 1.0.
	if (dSq > joystickRadiusSq || isDPad) {
		dx = cosAngle * joystickRadius;
		dy = sinAngle * joystickRadius;
	}

	velocity = Point(dx / joystickRadius, dy / joystickRadius);
	degrees = angle * SJ_RAD2DEG;

	if (innerThumb) {
		float offset = joystickRadius - thumbRadius;
		if (dx > offset)
			dx = offset;

		if (dx < -offset)
			dx = -offset;

		if (dy > offset)
			dy = offset;

		if (dy < -offset)
			dy = -offset;
	}

	// Update the thumb's position
	stickPosition = Point(dx, dy);
}

void SneakyJoystick::setIsDPad(bool b)
{
	isDPad = b;
	if (isDPad) {
		hasDeadzone = true;
		this->setDeadRadius(10.0f);
	}
}

void SneakyJoystick::setJoystickRadius(float r)
{
	joystickRadius = r;
	joystickRadiusSq = r * r;
}

void SneakyJoystick::setThumbRadius(float r)
{
	thumbRadius = r;
	thumbRadiusSq = r * r;
}

void SneakyJoystick::setDeadRadius(float r)
{
	deadRadius = r;
	deadRadiusSq = r * r;
}

bool SneakyJoystick::onTouchBegan(Touch* touch, Event* event)
{
	Point location = Director::getInstance()->convertToGL(touch->getLocationInView());

	//if([background containsPoint:[background convertToNodeSpace:location]]){
	location = this->convertToNodeSpace(location);
	//Do a fast rect check before doing a circle hit check:
	if (location.x < -joystickRadius || location.x > joystickRadius || location.y < -joystickRadius || location.y > joystickRadius) {
		return false;
	}
	else {
		float dSq = location.x * location.x + location.y * location.y;
		if (joystickRadiusSq > dSq) {
			this->updateVelocity(location);
			return true;
		}
	}
	return false;
}

void SneakyJoystick::onTouchMoved(Touch* touch, Event* event)
{
	Point location = Director::getInstance()->convertToGL(touch->getLocationInView());
	location = this->convertToNodeSpace(location);
	this->updateVelocity(location);
}

void SneakyJoystick::onTouchEnded(Touch* touch, Event* event)
{
	Point location = Point::ZERO;
	if (!autoCenter) {
		location = Director::getInstance()->convertToGL(touch->getLocationInView());
		location = this->convertToNodeSpace(location);
	}
	this->updateVelocity(location);
}

void SneakyJoystick::onTouchCancelled(Touch* touch, Event* event)
{
	this->onTouchEnded(touch, event);
}