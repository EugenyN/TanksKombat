#include "cocos2d.h"
#include "Engine.h"
#include "Scenes/BaseScene.h"
#include "ModalDialog.h"
#include "SimpleMenu.h"

USING_NS_CC;

#define TAG_OK			0
#define TAG_CANCEL		1


ModalDialog::ModalDialog()
{
	_referencer = nullptr;
	_listenerId = -1;
}

ModalDialog::~ModalDialog()
{
}

ModalDialog* ModalDialog::create(const std::string& content,
	const std::function<void(void)>& callbackOK,
	const std::function<void(void)>& callbackCancel,
	const std::string& okText, const std::string& cancelText)
{
	ModalDialog *dialog = new (std::nothrow) ModalDialog();

	if (dialog && dialog->init())
	{
		dialog->autorelease();
		dialog->initOpt(content, okText, cancelText);
		dialog->runAnimations(-1, false);
		dialog->_callbackOK = callbackOK;
		dialog->_callbackCancel = callbackCancel;
		return dialog;
	}
	CC_SAFE_DELETE(dialog);
	return nullptr;
}

void ModalDialog::initOpt(const std::string& content, const std::string& okText, const std::string& cancelText)
{
	_okText = okText;
	_cancelText = cancelText;

	auto designSize = Director::getInstance()->getWinSize();
	setTextureRect(Rect(0, 0, 320, 240));
	setPosition(designSize.width / 2, designSize.height / 2);
	setColor(LIGHTGREEN_COLOR);

	_innerSprite = Sprite::create();
	_innerSprite->setTextureRect(Rect(Vec2::ZERO, getTextureRect().size - Size(4, 4)));
	_innerSprite->setColor(DARKGREEN_COLOR);
	_innerSprite->setAnchorPoint(Vec2(0, 0));
	_innerSprite->setPosition(2, 2);

	_contentLabel = Label::createWithCharMap(FONT_MAIN);
	_contentLabel->setString(content);
	_contentLabel->setDimensions(getContentSize().width * 0.9f, getContentSize().height);
	_contentLabel->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
	_contentLabel->setPosition(_innerSprite->getContentSize().width / 2, _innerSprite->getContentSize().height / 2 + 32);
	_contentLabel->setColor(Color3B::WHITE);

	_innerSprite->addChild(_contentLabel);
	this->addChild(_innerSprite);


	SimpleMenuItem* okItem = SimpleMenuItem::create(" " + okText, FONT_MAIN, CC_CALLBACK_1(ModalDialog::menuItemActivated, this));
	okItem->setTag(TAG_OK);

	SimpleMenuItem* cancelItem = nullptr;
	if (cancelText != "") {
		cancelItem = SimpleMenuItem::create(" " + cancelText, FONT_MAIN, CC_CALLBACK_1(ModalDialog::menuItemActivated, this));
		cancelItem->setTag(TAG_CANCEL);
	}

	// create menu

	if (cancelItem != nullptr)
		_menu = SimpleMenu::create(SimpleMenu::Type::HORIZONTAL, okItem, cancelItem, nullptr);
	else
		_menu = SimpleMenu::create(SimpleMenu::Type::HORIZONTAL, okItem, nullptr);

	_menu->setPosition(Vec2(_innerSprite->getContentSize().width / 2,
		_innerSprite->getContentSize().height / 5));
	_menu->setAnchorPoint(Vec2(0, 0));
	_menu->alignItemsHorizontallyWithPadding(30);
	_menu->setScale(0.1f);
	this->addChild(_menu, 1);

	this->setScale(0.1f);
	this->setOpacity(0);
	_menu->setOpacity(0);
	_innerSprite->setOpacity(0);
	_contentLabel->setOpacity(0);
}

void ModalDialog::onEnter()
{
	Sprite::onEnter();
	if (_listenerId != -1)
	{
		BaseScene::getCurrentScene()->removeKeyboardEvents(_listenerId);
		_listenerId = -1;
	}
	addEvents();
}

void ModalDialog::onExit()
{
	Sprite::onExit();
	if (_listenerId != -1)
	{
		BaseScene::getCurrentScene()->removeKeyboardEvents(_listenerId);
		_listenerId = -1;
	}
}

void ModalDialog::addEvents()
{
	KeyboardEventsFunc listener;
	listener.onKeyPressed = nullptr;
	listener.onKeyReleased = CC_CALLBACK_2(ModalDialog::onKeyReleased, this);
	_listenerId = BaseScene::getCurrentScene()->addKeyboardEvents(listener);
}

bool ModalDialog::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) 
{
	if (keyCode == EventKeyboard::KeyCode::KEY_BACK) {
		event->stopPropagation(); // avoid exit on WINRT
		runAnimations(TAG_CANCEL, true);
		return true; // event consumed
	}
	return false; // event pass through
}

void ModalDialog::menuItemActivated(Ref* pSender)
{
	auto pMenuItem = (MenuItem*)pSender;
	runAnimations(pMenuItem->getTag(), true);
}

void ModalDialog::runAnimations(int menuItem, bool bReverse)
{
	this->runAction(bReverse ? ScaleBy::create(0.4f, 1 / 10.0f) : ScaleBy::create(0.4f, 10.0f));
	cocos2d::Action* fade;
	if (bReverse)
		fade = FadeOut::create(0.4f);
	else
		fade = FadeIn::create(0.4f);

	this->runAction(fade);
	_menu->runAction(fade->clone());
	_menu->runAction(bReverse ? ScaleBy::create(0.4f, 1 / 10.0f) : ScaleBy::create(0.4f, 10.0f));
	_innerSprite->runAction(fade->clone());
	_contentLabel->runAction(fade->clone());

	if (bReverse)
	{
		auto callback = CallFunc::create([menuItem, this]()
		{
			switch (menuItem)
			{
			case TAG_OK:
			{
				if (_callbackOK != nullptr)
					_callbackOK();
				break;
			}

			case TAG_CANCEL:
			{
				if (_callbackCancel != nullptr)
					_callbackCancel();
				break;
			}
			default:
			{
				break;
			}
			}
			this->removeFromParentAndCleanup(true);
			if (_referencer != nullptr && *_referencer != nullptr) {
				*_referencer = nullptr;
				_referencer = nullptr;
			}
		});
		auto seq = Sequence::create(DelayTime::create(0.4f), callback, nullptr);
		this->runAction(seq);
	}
}