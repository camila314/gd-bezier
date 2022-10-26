#include "BezierLayer.hpp"
#include "misc.hpp"
#include <math.h>

using namespace cocos2d;
using namespace cocos2d::extension;

BezierLayer* BezierLayer::create(CCSize size, float radius) {
	auto pRet = new BezierLayer();
	
	if (pRet && pRet->CCLayer::init()) {
		if (pRet->init(size, radius)) {
			pRet->autorelease();
			return pRet;
		}
	}
	
	CC_SAFE_DELETE(pRet);
	return NULL;
}

bool BezierLayer::init(CCSize size, float radius) {
	registerWithTouchDispatcher();
	setTouchEnabled(true);
	setTouchMode(kCCTouchesOneByOne);
	CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);
	m_size = size;
	m_controlRadius = radius;

	auto background = CCScale9Sprite::create("square02b_small.png");
	background->setColor(ccc3(45,45,60));
	background->setContentSize(m_size);
	addChild(background);
	setAnchorPoint({0.5, 0.5});

	m_controlPoint1 = clampControl(ccp(-m_size.width, -m_size.height)) + ccp(m_size.width / 4, 0);
	m_controlPoint2 = clampControl(ccp(m_size.width, m_size.height)) - ccp(m_size.height / 4, 0);
	
	m_canvas = CCDrawNode::create();
	addChild(m_canvas);

	updateCanvas();
	return true;
}

void BezierLayer::setCallback(std::function<void(BezierLayer*)> cb) {
	m_callback = cb;
}

std::pair<CCPoint, CCPoint> BezierLayer::getControls() {
	return std::make_pair(normalizeControl(m_controlPoint1), normalizeControl(m_controlPoint2));
}

CCPoint BezierLayer::clampControl(CCPoint p) {
	return ccp(
		clamp(p.x, -m_size.width / 2 + m_controlRadius, m_size.width / 2 - m_controlRadius),
		clamp(p.y, -m_size.height / 2 + m_controlRadius, m_size.height / 2 - m_controlRadius)
	);
}

CCPoint BezierLayer::controlToGlobal(CCPoint p) {
	return this->convertToWorldSpace(p);
}
CCPoint BezierLayer::globalToControl(CCPoint p) {
	return clampControl(this->convertToNodeSpace(p));
}

CCPoint BezierLayer::normalizeControl(CCPoint control) {
	float w = m_size.width - m_controlRadius * 2;
	float h = m_size.height - m_controlRadius * 2;
	return ccp(
		(control.x + w / 2) / w * 100,
		(control.y + h / 2) / h * 100
	);
}

bool BezierLayer::ccTouchBegan(CCTouch* touch, CCEvent* ev) {
	auto touchInBezier = this->convertToNodeSpace(touch->getLocation());
	if (fabs(touchInBezier.x) > m_size.width / 2 || fabs(touchInBezier.y) > m_size.height / 2) {
		return false;
	}

	m_dragState.currTouch = touchInBezier;
	if (distance(touchInBezier, m_controlPoint2) < m_controlRadius) {
		m_dragState.controlPoint = 2;
	} else if (distance(touchInBezier, m_controlPoint1) < m_controlRadius) {
		m_dragState.controlPoint = 1;
	} else {
		m_dragState.controlPoint = 0;
	}

	return true;
}
void BezierLayer::ccTouchMoved(CCTouch* touch, CCEvent* ev) {
	auto touchInBezier = this->convertToNodeSpace(touch->getLocation());
	m_dragState.prevTouch = m_dragState.currTouch;
	m_dragState.currTouch = touchInBezier;

	if (m_dragState.controlPoint == 2) {
		m_controlPoint2 = clampControl(m_controlPoint2 + m_dragState.currTouch - m_dragState.prevTouch);
	} else if (m_dragState.controlPoint == 1) {
		m_controlPoint1 = clampControl(m_controlPoint1 + m_dragState.currTouch - m_dragState.prevTouch);
	} else {
		return;
	}

	updateCanvas();
	if (m_callback)
		m_callback(this);
}

void BezierLayer::updateCanvas() {
	m_canvas->clear();

	auto bottomLeft = clampControl(ccp(-m_size.width, -m_size.height));
	auto topRight = clampControl(ccp(m_size.width, m_size.height));

	auto endPoint = ccc4FFromccc3B(ccc3(255, 255, 255));
	auto control1 = ccc4FFromccc3B(ccc3(255, 16, 135));
	auto control2 = ccc4FFromccc3B(ccc3(16, 135, 255));
	auto secondaryLine = ccc4FFromccc3B(ccc3(150, 150, 150));
	auto primaryLine = ccc4FFromccc3B(ccc3(20, 20, 20));

	// big line
	drawBezier(m_canvas, bottomLeft, m_controlPoint1, m_controlPoint2, topRight, 50, primaryLine);

	// connecting lines
	m_canvas->drawSegment(bottomLeft, m_controlPoint1, 1, secondaryLine);
	m_canvas->drawSegment(topRight, m_controlPoint2, 1, secondaryLine);

	// start/stop
	m_canvas->drawSegment(bottomLeft, bottomLeft, m_controlRadius , endPoint);
	m_canvas->drawSegment(topRight, topRight, m_controlRadius, endPoint);

	// control points
	m_canvas->drawSegment(m_controlPoint1, m_controlPoint1, m_controlRadius, control1);
	m_canvas->drawSegment(m_controlPoint2, m_controlPoint2, m_controlRadius, control2);
}