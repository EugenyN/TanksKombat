#ifndef _MODALDIALOG_H_
#define _MODALDIALOG_H_

#include "cocos2d.h"

class SimpleMenu;

class ModalDialog : public cocos2d::Sprite
{
public:
	ModalDialog();
	~ModalDialog();
	static ModalDialog* create(const std::string& = "",
		const std::function<void(void)>& callbackOK = nullptr,
		const std::function<void(void)>& callbackCancel = nullptr,
		const std::string& okText = "OK",
		const std::string& cancelText = "CANCEL");

	bool isValid() { return _listenerId != -1; };
	void setReferencer(ModalDialog** referencer) { _referencer = referencer; };

private:
	int _listenerId;
	ModalDialog** _referencer;

	cocos2d::Label* _contentLabel;
	cocos2d::Sprite* _innerSprite;
	SimpleMenu* _menu;
	std::string _okText;
	std::string _cancelText;
	std::function<void(void)> _callbackOK;
	std::function<void(void)> _callbackCancel;

	void addEvents();
	bool onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
	virtual void onEnter() override;
	virtual void onExit() override;
	void initOpt(const std::string& content, const std::string& okText, const std::string& cancelText);
	void menuItemActivated(cocos2d::Ref* pSender);
	void runAnimations(int menuItem, bool);
};

#endif // _MODALDIALOG_H_
