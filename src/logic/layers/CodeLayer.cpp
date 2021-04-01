#include "pch.h"
#include "CodeLayer.h"

using namespace cocos2d;
using namespace extension;


bool CodeLayer::init() {
	auto winSize = CCDirector::sharedDirector()->getWinSize();

	if (!this->initWithColor({ 0, 0, 0, 75 })) return false;
	m_pLayer = CCLayer::create();
	this->addChild(m_pLayer);

	auto bg = CCScale9Sprite::create("GJ_square01.png", { 0.0f, 0.0f, 80.0f, 80.0f });
	bg->setContentSize({ 400.0f, 240.0f });
	bg->setPosition(winSize.width / 2, winSize.height / 2);
	m_pLayer->addChild(bg);

	m_pButtonMenu = CCMenu::create();
	m_pLayer->addChild(m_pButtonMenu);

	auto title = CCLabelBMFont::create("RobTop's Offering", "bigFont.fnt");
	title->setScale(0.8f);
	title->setPosition(winSize.width / 2, winSize.height / 2 + 90.0f);
	m_pLayer->addChild(title);

	auto caption = CCLabelBMFont::create(
		"An offering is required from all\n"
		"who wish to enter RobTop's lair", "goldFont.fnt");
	caption->setAlignment(kCCTextAlignmentCenter);
	caption->setScale(0.8f);
	caption->setPosition(winSize.width / 2, winSize.height / 2 + 45.0f);
	m_pLayer->addChild(caption);

	auto prompt = CCLabelBMFont::create("Enter $20 Steam Code:", "bigFont.fnt");
	prompt->setScale(0.6f);
	prompt->setPosition(winSize.width / 2, winSize.height / 2);
	m_pLayer->addChild(prompt);

	auto textBG = CCScale9Sprite::create("square02_small.png", { 0.0f, 0.0f, 40.0f, 40.0f });
	textBG->setContentSize({ 270.0f, 30.0f });
	textBG->setPosition(winSize.width / 2, winSize.height / 2 - 40.0f);
	textBG->setOpacity(100);
	m_pLayer->addChild(textBG);

	auto textArea = gd::CCTextInputNode::create("XXXXX-XXXXX-XXXXX", nullptr, "bigFont.fnt", 250.0f, 30.0f);
	textArea->setLabelPlaceholderColor({ 120, 170, 240 });
	textArea->setLabelPlaceholerScale(0.5f);
	textArea->setMaxLabelScale(0.5f);
	textArea->setPosition(winSize.width / 2, winSize.height / 2 - 40.0f);
	m_pLayer->addChild(textArea);

	m_pButton1 = gd::ButtonSprite::create("Cancel", 0, false, "goldFont.fnt", "GJ_button_01.png", 0.0f, 1.0f);
	auto btn1 = gd::CCMenuItemSpriteExtra::create(m_pButton1, this, menu_selector(CodeLayer::onCancel));
	btn1->setPosition(-60.0f, -90.0f);
	m_pButtonMenu->addChild(btn1);

	m_pButton2 = gd::ButtonSprite::create("Submit", 0, false, "goldFont.fnt", "GJ_button_01.png", 0.0f, 1.0f);
	auto btn2 = gd::CCMenuItemSpriteExtra::create(m_pButton2, this, menu_selector(CodeLayer::onSubmit));
	btn2->setPosition(60.0f, -90.0f);
	m_pButtonMenu->addChild(btn2);

	this->setKeypadEnabled(true);
	this->setTouchEnabled(true);

	return true;
}

void CodeLayer::onCancel(cocos2d::CCObject*) {
	this->setKeypadEnabled(false);
	this->removeFromParentAndCleanup(true);
}

void CodeLayer::onSubmit(cocos2d::CCObject*) {
	m_pLoading = gd::LoadingCircle::create();
	m_pLoading->setParentLayer(this->m_pLayer);
	m_pLoading->setFade(true);
	m_pLoading->show();
	m_pLoading->setKeyboardEnabled(true);
	this->runAction(CCSequence::createWithTwoActions(
		CCDelayTime::create(1.34f),
		CCCallFunc::create(
			this,
			callfunc_selector(CodeLayer::onFinish)
		)
	));
}

void CodeLayer::onFinish() {
	this->onCancel(nullptr);
}

CodeLayer* CodeLayer::create() {
	CodeLayer* pRet = new CodeLayer();
	if (pRet && pRet->init()) {
		pRet->autorelease();
		return pRet;
	}
	else {
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}