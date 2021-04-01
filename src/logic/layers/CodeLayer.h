#ifndef __CODELAYER_H__
#define __CODELAYER_H__

#include "pch.h"

class CodeLayer : public gd::FLAlertLayer {
protected:
	gd::LoadingCircle* m_pLoading;

protected:
	virtual bool init();

	void onCancel(cocos2d::CCObject*);
	void onSubmit(cocos2d::CCObject*);
	void onFinish();

public:
	static CodeLayer* create();
};

#endif