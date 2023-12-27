#pragma once

#include "MVec.h"

struct PhysicBody2d
{
	PhysicBody2d(Vec2 cp, float r, bool iK, sf::Color  cl):
		current_position{ cp }, old_position{ cp }, radius{ r }, isKinematic{ iK }, color{ cl } {}

	PhysicBody2d(Vec2 cp = {}, float r = { 20 }, bool iK = false) :
		current_position{ cp }, old_position{ cp }, radius{ r }, isKinematic{ iK }
	{
		color = sf::Color(rand() % 256, rand() % 256, rand() % 256);
	}

	void update_position(const float dtime) {
		if (isKinematic) {
			const Vec2 velocity = current_position - old_position; // velocity [/dtime]
			old_position = current_position;
			current_position += velocity + acceleration * dtime * dtime; //verlet
		}
		else{
			old_position = current_position;
		}
		acceleration = 0;
	}

	float getRadius() const { return radius; }
	void setPos(const Vec2 p) { current_position = p; }
	Vec2 getOldPos() const { return old_position; }
	Vec2 getAcceleration() const { return acceleration; }
	const sf::Color& getColor() const { return color; }

	Vec2 getPos() const { return current_position; }

	void accelerate(const Vec2 add_acc) { acceleration += add_acc; }
	void move(Vec2 p) { old_position = current_position = p;} // cs.setPosition(p - Vec2{ radius,radius }); 

	bool isHere(Vec2 here) {return (current_position - here).length() < radius; }
	bool isKinematic = true;

	static PhysicBody2d nullPB;

protected:
	float radius;
	Vec2 current_position;
	Vec2 old_position;
	Vec2 acceleration = {};
	sf::Color color;
	friend class PhysicSolver;
	friend class ChunkGrid;
	friend class PhysicLink2d;
};

