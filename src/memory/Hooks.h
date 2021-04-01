#ifndef __HOOKS_H__
#define __HOOKS_H__

#include "pch.h"
#include "../logic/layers/CodeLayer.h"
#include "../logic/layers/ElderLayer.h"

#define ZORDER 4

bool g_bCoin = false;

enum {
	kCoin = 314,
	kScratch
};

class Callbacks {
public:
	void onUnlock(cocos2d::CCObject* sender) {
		using namespace cocos2d;
		
		auto old = static_cast<gd::CCMenuItemSpriteExtra*>(sender);
		old->setVisible(false);
		old->setEnabled(false);

		auto spr = CCSprite::createWithSpriteFrameName("GJ_lock_open_001.png");
		spr->setScale(0.6f);
		auto btn = gd::CCMenuItemSpriteExtra::create(
			spr,
			sender,
			menu_selector(Callbacks::onLayer)
		);
		btn->setPosition(127.0f, 27.0f);
		auto menu = static_cast<CCMenu*>(old->getParent());
		menu->addChild(btn);
	}
	void onLayer(cocos2d::CCObject* sender) {
		using namespace cocos2d;

		auto old = static_cast<gd::CCMenuItemSpriteExtra*>(sender);
		old->setVisible(false);
		old->setEnabled(false);

		auto spr = CCSprite::createWithSpriteFrameName("GJ_lock_open_001.png");
		spr->setScale(0.6f);
		auto btn = gd::CCMenuItemSpriteExtra::create(
			spr,
			sender,
			menu_selector(Callbacks::onScene)
		);
		btn->setPosition(127.0f, 27.0f);
		auto menu = static_cast<CCMenu*>(old->getParent());
		menu->addChild(btn);
		CodeLayer::create()->show();
	}
	void onScene(cocos2d::CCObject* sender) {
		ElderLayer::scene();
	}
};

namespace gates {
	inline void(__thiscall* GaragePage_create)(cocos2d::CCLayer*);
	inline bool(__thiscall* SecretLayer3_init)(cocos2d::CCLayer*);
	inline bool(__thiscall* CCKeyboardDispatcher_dispatchKeyboardMSG)(cocos2d::CCKeyboardDispatcher*, cocos2d::enumKeyCodes, bool);
}

namespace hooks {
	void __fastcall GaragePage_create(cocos2d::CCLayer* gjGarageLayer) {
		using namespace cocos2d;

		auto winSize = CCDirector::sharedDirector()->getWinSize();
		auto spr = CCSprite::createWithSpriteFrameName("GJ_lockGray_001.png");
		spr->setScale(0.6f);
		auto btn = gd::CCMenuItemSpriteExtra::create(
			spr,
			gjGarageLayer,
			menu_selector(Callbacks::onUnlock)
		);
		btn->setPosition(127.0f, 27.0f);
		gjGarageLayer->addChild(CCMenu::createWithItem(btn));
		return gates::GaragePage_create(gjGarageLayer);
	}
	bool __fastcall CCKeyboardDispatcher_dispatchKeyboardMSG(cocos2d::CCKeyboardDispatcher* This, void*, cocos2d::enumKeyCodes key, bool down) {
		if (GetAsyncKeyState(VK_OEM_PERIOD)) {
			g_bCoin = !g_bCoin;
		}
		return gates::CCKeyboardDispatcher_dispatchKeyboardMSG(This, key, down);
	}
	bool __fastcall SecretLayer3_init(cocos2d::CCLayer* This) {
		using namespace cocos2d;
		using namespace extension;

		auto pRet = gates::SecretLayer3_init(This);
		
		//if demon is freed add my stuff otherwise ignore
		if (gd::GameManager::sharedState()->getUGV("13")) {
			auto winSize = CCDirector::sharedDirector()->getWinSize();

			if (g_bCoin) {
				auto coin = gd::GameObject::createWithFrame("secretCoin_01_001.png");
				coin->setTag(kCoin);
				coin->setPosition(winSize / 2);
				This->addChild(coin);
				coin->setZOrder(ZORDER);

				auto frames = CCArray::create();

				for (unsigned int i = 1; i <= 4; ++i) {
					frames->addObject(
						CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
							CCString::createWithFormat("secretCoin_0%i_001.png", i)->getCString()
						)
					);
				}
				coin->runAction(
					CCRepeatForever::create(
						CCAnimate::create(
							CCAnimation::createWithSpriteFrames(frames, 0.115f)
						)
					)
				);

				This->setTouchEnabled(true);
			}
			else {
				auto scratch = gd::AnimatedShopKeeper::create(gd::kShopTypeSecret);
				scratch->setTag(kScratch);
				scratch->setPosition({ winSize.width / 2 - 1.0f, winSize.height / 2 + 10.0f });
				scratch->setScale(0.8f);

				auto animSprite = static_cast<CCSprite*>(scratch->getChildren()->objectAtIndex(0));

				This->addChild(scratch);
				scratch->setZOrder(ZORDER);
				scratch->startAnimating();

				CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
				CCTexture2D::PVRImagesHavePremultipliedAlpha(false);

				if (auto overlay = CCSprite::create("the_fucking_void.png")) {
					overlay->setScale(2.25f);
					overlay->setBlendFunc({ GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA });
					animSprite->addChild(overlay);
					overlay->setZOrder(68);
				}

				auto bars = CCSprite::createWithSpriteFrameName("dungeon_bars_001.png");
				bars->setPosition({ winSize.width / 2 - 2.0f, winSize.height / 2 });
				This->addChild(bars);
				bars->setZOrder(ZORDER + 1);
			}
		}

		return pRet;
	}
	void __fastcall SecretLayer3_ccTouchesBegan(cocos2d::CCLayer* This, void*, cocos2d::CCSet* pTouches, cocos2d::CCEvent* pEvent) {
		using namespace cocos2d;

		//we're in the wrong vtable otherwise lol, get the right one instead
		This = reinterpret_cast<CCLayer*>(reinterpret_cast<char*>(This) - 0xEC);

		auto touch = pTouches->anyObject();
		if (touch) {
			auto touchPos = static_cast<CCTouch*>(touch)->getLocation();
			auto coin = static_cast<CCNode*>(This->getChildByTag(kCoin));
			auto pos = coin->getPosition();
			if (touchPos.x < pos.x + 25.0f && touchPos.x > pos.x - 25.0f &&
				touchPos.y < pos.y + 25.0f && touchPos.y > pos.y - 25.0f) {
				auto particles = CCParticleSystemQuad::create("coinPickupEffect.plist");
				particles->setPosition(pos);
				particles->setZOrder(ZORDER);
				This->addChild(particles);

				auto wave1 = gd::CCCircleWave::create(false, false, 25.0f, 5.0f, 0.3f);
				wave1->setColor({ 255, 200, 0 });
				wave1->setPosition(pos);
				wave1->setZOrder(ZORDER);
				This->addChild(wave1);

				auto wave2 = gd::CCCircleWave::create(false, true, 5.0f, 60.0f, 0.3f);
				wave2->setColor({ 255, 255, 0 });
				wave2->setPosition(pos);
				wave2->setZOrder(ZORDER);
				This->addChild(wave2);

				//this is SO ugly idk if it's rob or compiler optimizations but please just stop
				auto end = ccp(((rand() / 32767.0f) * 2.0f - 1.0f) * 50.f + pos.x, pos.y - 60.0f);
				coin->runAction(
					CCBezierTo::create(0.75f, { { end.x, end.y - 20.0f }, { end.x, end.y + 180.0f }, { end.x, end.y + 90.0f } })
				);
				coin->runAction(
					CCSequence::createWithTwoActions(
						CCDelayTime::create(0.5f),
						CCFadeOut::create(0.3f)
					)
				);
			}
		}
	}
}

#endif