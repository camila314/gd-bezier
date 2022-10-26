#pragma once
#include <cocos2d.h>

template<typename T>
constexpr const T& clamp( const T& v, const T& lo, const T& hi) {
    return v < lo ? lo : hi < v ? hi : v;
}

float distance(cocos2d::CCPoint a, cocos2d::CCPoint b) {
    auto s = a - b;
    return sqrt(s.x * s.x + s.y * s.y);
}

void drawBezier(cocos2d::CCDrawNode* draw, const cocos2d::CCPoint& origin, const cocos2d::CCPoint& control1, const cocos2d::CCPoint& control2, const cocos2d::CCPoint& destination, unsigned int segments, cocos2d::ccColor4F color) {
    unsigned vertex_count = segments+2;
    cocos2d::CCPoint* vertices = new cocos2d::CCPoint[vertex_count];
    vertices[0] = vertices[1] = origin;

    float t = 0;
    for(unsigned int i = 1; i <= segments; i++) {
        if (i==segments) {
            vertices[i].x = destination.x;
            vertices[i].y = destination.y;
        } else {
            vertices[i].x = powf(1 - t, 3) * origin.x + 3.0f * powf(1 - t, 2) * t * control1.x + 3.0f * (1 - t) * t * t * control2.x + t * t * t * destination.x;
            vertices[i].y = powf(1 - t, 3) * origin.y + 3.0f * powf(1 - t, 2) * t * control1.y + 3.0f * (1 - t) * t * t * control2.y + t * t * t * destination.y;
            t += 1.0f / segments;
        }
        draw->drawSegment(vertices[i-1], vertices[i], 2, color);
    }

    CC_SAFE_DELETE_ARRAY(vertices);
}
