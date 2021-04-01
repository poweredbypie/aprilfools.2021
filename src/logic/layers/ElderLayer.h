#ifndef __ELDERLAYER_H__
#define __ELDERLAYER_H__

#include "pch.h"

class ElderLayer : public cocos2d::CCLayer {
protected:
	cocos2d::CCPoint m_obNext;
	cocos2d::CCMenu* m_pButtonMenu;
	bool m_bButtonsEntered;

protected:
	virtual bool init();

	void onExit(cocos2d::CCObject*);
	virtual void keyDown(cocos2d::enumKeyCodes key);
	void addButton(const char* frameName, const char* text);

public:
	static ElderLayer* create();
	static void scene();
};

#endif