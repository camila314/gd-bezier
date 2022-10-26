#pragma once
#include <cocos2d.h>

struct DragState {
	cocos2d::CCPoint prevTouch;
	cocos2d::CCPoint currTouch;
	int controlPoint;
};


class BezierLayer : public cocos2d::CCLayer {
 public:
	static BezierLayer* create(cocos2d::CCSize size, float radius = 5.0);

	bool init(cocos2d::CCSize size, float radius);
	void setCallback(std::function<void(BezierLayer*)> cb);
	std::pair<cocos2d::CCPoint, cocos2d::CCPoint> getControls();

 protected:

 	cocos2d::CCPoint clampControl(cocos2d::CCPoint p);
	cocos2d::CCPoint controlToGlobal(cocos2d::CCPoint p);
	cocos2d::CCPoint globalToControl(cocos2d::CCPoint p);
	cocos2d::CCPoint normalizeControl(cocos2d::CCPoint control);

	void updateCanvas();
	bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* ev) override;
	void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* ev) override;

 	cocos2d::CCSize m_size;
 	float m_controlRadius;
 	cocos2d::CCPoint m_controlPoint1;
 	cocos2d::CCPoint m_controlPoint2;
 	cocos2d::CCDrawNode* m_canvas;
 	DragState m_dragState;
 	std::function<void(BezierLayer*)> m_callback;
};
