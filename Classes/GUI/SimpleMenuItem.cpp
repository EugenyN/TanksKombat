#include "cocos2d.h"
#include "Engine.h"
#include "SimpleMenu.h"
#include "SimpleMenuItem.h"

USING_NS_CC;


SimpleMenuItem* SimpleMenuItem::create()
{
	return SimpleMenuItem::create((const ccMenuCallback&)nullptr);
}

SimpleMenuItem* SimpleMenuItem::create(const ccMenuCallback& callback)
{
	SimpleMenuItem *ret = new (std::nothrow) SimpleMenuItem();
	ret->initWithCallback(callback);
	ret->autorelease();
	return ret;
}

SimpleMenuItem * SimpleMenuItem::create(const std::string& value, const std::string& charMapFile, int itemWidth, int itemHeight, char startCharMap)
{
	return SimpleMenuItem::create(value, charMapFile, itemWidth, itemHeight, startCharMap, (const ccMenuCallback&)nullptr);
}

SimpleMenuItem * SimpleMenuItem::create(const std::string& value, const std::string& charMapFile, int itemWidth, int itemHeight, char startCharMap, const ccMenuCallback& menuItemActivate)
{
	SimpleMenuItem *ret = new (std::nothrow) SimpleMenuItem();
	ret->initWithString(value, charMapFile, itemWidth, itemHeight, startCharMap, menuItemActivate);
	ret->autorelease();

	ret->_originalString = value;

	return ret;
}

void SimpleMenuItem::activate()
{
	MenuItemAtlasFont::activate();

	auto parent = dynamic_cast<SimpleMenu*>(getParent());
	if (parent != nullptr)
		parent->onItemActivated(this, true);
}

const std::string & SimpleMenuItem::getCurrentValue() const
{
	return _values[_currentValue];
}

void SimpleMenuItem::setCurrentValue(const std::string& value)
{
	int index = std::find(_values.begin(), _values.end(), value) - _values.begin();
	if (index != -1)
		setCurrentValueIndex(index);
}

void SimpleMenuItem::setCurrentValueIndex(int index)
{
	CCASSERT(index >= 0 && index < _values.size(), "index out of range");

	_currentValue = index;
	onValueChanged(false);
}

void SimpleMenuItem::setValues(const std::vector<std::string>& values)
{
	_values = values;

	onValueChanged(false);
}

bool SimpleMenuItem::selectNextValue()
{
	if (_values.size() == 0)
		return false;

	_currentValue = (_currentValue + 1) % _values.size();

	onValueChanged();
	return true;
}

bool SimpleMenuItem::selectPrevValue()
{
	if (_values.size() == 0)
		return false;

	_currentValue = (_currentValue + _values.size() - 1) % _values.size();

	onValueChanged();
	return true;
}

void SimpleMenuItem::setCurrent(bool selected)
{
	_isCurrent = selected;

	updateLabel();
}

void SimpleMenuItem::onValueChanged(bool playSound)
{
	updateLabel();

	if (playSound)
		Engine::getInstance()->playSound("menu_select");

	if (_onValueChanged != nullptr)
		_onValueChanged(this);
}

void SimpleMenuItem::updateLabel()
{
	auto label = dynamic_cast<LabelAtlas*>(getLabel());
	if (label == nullptr)
		return;

	auto string = label->getString();

	if (_values.size() != 0 && _originalString.length() != 0)
		string = StringUtils::format(_originalString.c_str(), getCurrentValue().c_str());

	if (string[0] != ' ' && string[0] != '~')
		string.insert(0, " ");

	if (_isCurrent)
		string[0] = '~';
	else
		string[0] = ' ';

	label->setString(string);
}
