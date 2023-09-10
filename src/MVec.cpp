#include "MVec.h"

#include <iostream>

const Vec2 Vec2::nullV = Vec2();
const Vec2 Vec2::UP = Vec2(0,1);
const Vec2 Vec2::DOWN = Vec2(0,-1);
const Vec2 Vec2::LEFT = Vec2(-1,0);
const Vec2 Vec2::RIGHT = Vec2(1,0);

std::ostream& operator<< (std::ostream& out, const Vec2& obj)
{
	return out << "(" << obj.x << ", " << obj.y << ")";
}

std::ostream& operator<< (std::ostream& out, const Vec2&& obj)
{
	return out << "(" << obj.x << ", " << obj.y << ")";
}

float Vec2::angleBetween(Vec2 base, Vec2 target)
{
	float det = (target.x * base.y - target.y * base.x);

	float dot = (target.x * base.x + target.y * base.y);


	//if(det>0 && dot > 0) return atan2f(det, dot) * 180 / M_PI;

	//if(dot>0) return atan2f(det, dot) * 180 / M_PI; // det<0

	//if (det < 0) return 360 + atan2f(det, dot);// *180.f / M_PIF; // det <0 -180

	return atan2f(det, dot);// *180.f / M_PIF;
}