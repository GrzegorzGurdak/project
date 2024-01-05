#pragma once

#include "PhysicBody2d.h"

class PhysicLink2d
{
public:
	PhysicLink2d(PhysicBody2d* pb1, PhysicBody2d* pb2, float ll) : physicBody1{ pb1 }, physicBody2{ pb2 }, link_length{ ll } {
		//line[0].position = physicBody1.current_position;
		//line[1].position = physicBody2.current_position;
	}
	void update_link() {
		Vec2 diff = physicBody2->getPos() - physicBody1->getPos();
		float diff_l = diff.length();
		const float stiffness = 0.01f; // (0,1) 0 - no stiffness, 1 - max stiffness
		line[0].position = physicBody1->current_position;
		line[1].position = physicBody2->current_position;
		//if (diff_l < 1e-6) diff_l = 1e-6;
		Vec2 transform = diff * ((diff_l - link_length) / diff_l / 2) * stiffness;
		if (physicBody1->isKinematic) physicBody1->current_position += transform;
		if (physicBody2->isKinematic) physicBody2->current_position -= transform;
	}

	const PhysicBody2d* getPB1() const { return physicBody1; }
	const PhysicBody2d* getPB2() const { return physicBody2; }

private:
	PhysicBody2d* physicBody1;
	PhysicBody2d* physicBody2;

	sf::VertexArray line{ sf::Lines, 2 };

	float link_length;
};

