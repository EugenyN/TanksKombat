#include "cocos2d.h"
#include "Engine.h"
#include "SimpleMenu.h"
#include "Scenes\BaseScene.h"

USING_NS_CC;

SimpleMenu* SimpleMenu::create(Type type)
{
	return SimpleMenu::create(type, nullptr, nullptr);
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
SimpleMenu* SimpleMenu::variadicCreate(Type type, MenuItem* item, ...)
{
	va_list args;
	va_start(args, item);

	SimpleMenu* ret = SimpleMenu::createWithItems(type, item, args);

	va_end(args);

	return ret;
}

#else

SimpleMenu* SimpleMenu::create(Type type, MenuItem* item, ...)
{
	va_list args;
	va_start(args, item);

	SimpleMenu* ret = SimpleMenu::createWithItems(type, item, args);

	va_end(args);

	return ret;
}
#endif

SimpleMenu* SimpleMenu::createWithArray(Type type, const Vector<MenuItem*>& arrayOfItems)
{
	auto ret = new (std::nothrow) SimpleMenu();
	if (ret && ret->initWithArray(arrayOfItems))
	{
		ret->autorelease();
		ret->initOpt(type);
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}

	return ret;
}

SimpleMenu* SimpleMenu::createWithItems(Type type, MenuItem* item, va_list args)
{
	Vector<MenuItem*> items;
	if (item)
	{
		items.pushBack(item);
		MenuItem* i = va_arg(args, MenuItem*);
		while (i)
		{
			items.pushBack(i);
			i = va_arg(args, MenuItem*);
		}
	}

	return SimpleMenu::createWithArray(type, items);
}

SimpleMenu::SimpleMenu()
	: _currentMenuItem(nullptr), _type(Type::HORIZONTAL)
{
	_listenerId = -1;
}

SimpleMenu::~SimpleMenu()
{
	if (_listenerId != -1)
	{
		BaseScene::getCurrentScene()->removeKeyboardEvents(_listenerId);
		_listenerId = -1;
	}
}

void SimpleMenu::setCurrentMenuItemTag(int currentMenuItem)
{
	auto item = dynamic_cast<SimpleMenuItem*>(getChildByTag(currentMenuItem));
	if (item != nullptr)
		onCurrentItemChanged(item, false);
}

void SimpleMenu::initOpt(Type type)
{
	_type = type;

	KeyboardEventsFunc listener;
	listener.onKeyPressed = CC_CALLBACK_2(SimpleMenu::onKeyPressed2, this);
	listener.onKeyReleased = nullptr;
	_listenerId = BaseScene::getCurrentScene()->addKeyboardEvents(listener);

	setCurrentMenuItemTag(0);
}

bool SimpleMenu::onKeyPressed2(EventKeyboard::KeyCode keyCode, Event* event)
{
	auto upSelectCode = EventKeyboard::KeyCode::KEY_UP_ARROW;
	auto downSelectCode = EventKeyboard::KeyCode::KEY_DOWN_ARROW;
	auto leftSelectCode = EventKeyboard::KeyCode::KEY_LEFT_ARROW;
	auto rightSelectCode = EventKeyboard::KeyCode::KEY_RIGHT_ARROW;

	if (_type == SimpleMenu::Type::HORIZONTAL) {
		upSelectCode = EventKeyboard::KeyCode::KEY_LEFT_ARROW;
		downSelectCode = EventKeyboard::KeyCode::KEY_RIGHT_ARROW;
		leftSelectCode = EventKeyboard::KeyCode::KEY_UP_ARROW;
		rightSelectCode = EventKeyboard::KeyCode::KEY_DOWN_ARROW;
	}

	if (keyCode == EventKeyboard::KeyCode::KEY_KP_ENTER || keyCode == EventKeyboard::KeyCode::KEY_ENTER)
	{
		auto item = getCurrentMenuItem();
		if (item != nullptr)
			item->activate();
		return true;
	}
	else if (keyCode == upSelectCode)
	{
		ssize_t index = _children.getIndex(_currentMenuItem);
		if (index > 0)
			onCurrentItemChanged(dynamic_cast<SimpleMenuItem*>(_children.at(--index)));
		return true;
	}
	else if (keyCode == downSelectCode)
	{
		ssize_t index = _children.getIndex(_currentMenuItem);
		if (index < _children.size() - 1)
			onCurrentItemChanged(dynamic_cast<SimpleMenuItem*>(_children.at(++index)));
		return true;
	}
	else if (keyCode == leftSelectCode)
	{
		auto item = dynamic_cast<SimpleMenuItem*>(getCurrentMenuItem());
		if (item != nullptr)
			item->selectPrevValue();
		return true;
	}
	else if (keyCode == rightSelectCode)
	{
		auto item = dynamic_cast<SimpleMenuItem*>(getCurrentMenuItem());
		if (item != nullptr)
			item->selectNextValue();
		return true;
	}

	return false;
}

void SimpleMenu::onCurrentItemChanged(SimpleMenuItem* menuItem, bool playSound)
{
	updateMenuItems(menuItem);

	if (_currentMenuItem == menuItem)
		return;

	_currentMenuItem = menuItem;

	if (playSound)
		Engine::getInstance()->playSound("menu_select");

	if (_onItemChanged != nullptr)
		_onItemChanged(dynamic_cast<SimpleMenuItem*>(menuItem));
}

void SimpleMenu::onItemActivated(SimpleMenuItem* menuItem, bool playSound)
{
	if (menuItem->selectNextValue())
		playSound = false;

	if (playSound)
		Engine::getInstance()->playSound("menu_select");

	onCurrentItemChanged(menuItem, false);
}

void SimpleMenu::updateMenuItems(SimpleMenuItem* menuItem)
{
	for (auto child : this->getChildren())
	{
		auto item = dynamic_cast<SimpleMenuItem*>(child);
		if (item != nullptr)
			item->setCurrent(item == menuItem);
	}
}
