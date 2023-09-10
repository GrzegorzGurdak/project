#pragma once

#include "PhysicBody2d.h"
#include "MVec.h"

namespace PhysicExamples {
	namespace Acceleration {
		auto centreAcceleration(Vec2 centre, float strength) {
			return[centre, strength](PhysicBody2d* i, const std::vector<PhysicBody2d*>&) {
				Vec2 diff = centre - i->getPos();
				float diffLen = diff.length();
				
				if(diffLen<1)
					return Vec2();
				return diff / diffLen / diffLen * strength;
				
			};
		}
	}

	namespace Constrains {
		auto circleRestrain(Vec2 centre, float radius) {
			return[centre, radius](PhysicBody2d* i) {
				Vec2 diff = i->getPos() - centre;
				float diffLen = diff.length();
				if (diffLen + i->getRadius() > radius) {
					return i->getPos() - diff / diffLen * (diffLen + i->getRadius() - radius);

				}
				return i->getPos();
			};
		}
		auto boxRestrain(Vec2 pos1, Vec2 pos2) {
			Vec2 minPos{ std::fminf(pos1.x,pos2.x),std::fminf(pos1.y,pos2.y) };
			Vec2 maxPos{ std::fmaxf(pos1.x,pos2.x),std::fmaxf(pos1.y,pos2.y) };
			pos1 = minPos;
			pos2 = maxPos;
			return[pos1, pos2](PhysicBody2d* i) {
				Vec2 npos = i->getPos();
				//Vec2 radVec{ i.getRadius(),i.getRadius() };
				if (npos.x < pos1.x + i->getRadius()) npos.x = pos1.x + i->getRadius();
				else if (npos.x > pos2.x - i->getRadius()) npos.x = pos2.x - i->getRadius();
				if (npos.y < pos1.y + i->getRadius()) npos.y = pos1.y + i->getRadius();
				else if (npos.y > pos2.y - i->getRadius()) npos.y = pos2.y - i->getRadius();
				return npos;
			};
		}
		Vec2 defaultConstrain(PhysicBody2d* i) {
			Vec2 diff = i->getPos() - Vec2(350, 350);
			float diffLen = diff.length();
			if (diffLen + i->getRadius() > 300) {
				return i->getPos() - diff / diffLen * (diffLen + i->getRadius() - 300);
			}
			return i->getPos();
		}
	}

	namespace Sandbox {
		PhysicSolver* cloth(Vec2 windowSize, Vec2 startPos, int sizex, int sizey, float linkLen = 30 ,int ballSize = 10) {
			PhysicSolver* sandbox = new PhysicSolver(ChunkGrid(33, windowSize.x, windowSize.y));
			sandbox->set_constraints(Constrains::boxRestrain(Vec2(30, 30), windowSize - Vec2(30, 30)));

			std::vector<std::vector<PhysicBody2d*>> pBs;
			//int sizex = 10;
			//int sizey = 10;
			//Vec2 startPos = { 100,100 };
			//float linkLen = 30;
			for (int i = 0; i < sizex; i++) {
				pBs.push_back(std::vector<PhysicBody2d*>());
				Vec2 tmp = startPos + Vec2(linkLen * i, 0.f);
				PhysicBody2d* upper = new PhysicBody2d(startPos + Vec2(linkLen * i, 0.f), ballSize);
				upper->isKinematic = false;
				pBs.back().push_back(upper);
				sandbox->add(upper);
				for (int j = 1; j < sizey; j++) {
					PhysicBody2d* npb = new PhysicBody2d(startPos + Vec2(linkLen * i, linkLen * j), ballSize);
					pBs.back().push_back(npb);
					sandbox->add(npb);
				}
			}
			for (int x = 0; x < sizex - 1; x++)
				for (int y = 0; y < sizey - 1; y++)
					for (int i = 0; i < 2; i++)
						for (int j = 0; j < 2; j++){
							if(i!=j)
								sandbox->addLink(
									new PhysicLink2d(*pBs.at(x).at(y), *pBs.at(x + i).at(y + j), linkLen)
								);
						}
			for (int x = 0; x < sizex - 1; x++)
				sandbox->addLink(new PhysicLink2d(*pBs.at(x).back(), *pBs.at(x + 1).back(), linkLen));

			for (int y = 0; y < sizey - 1; y++)
				sandbox->addLink(new PhysicLink2d(*pBs.back().at(y), *pBs.back().at(y + 1), linkLen));

			return sandbox;
		}
	}

	namespace Collisions {
		void collision_with_viscosity(PhysicBody2d* i, PhysicBody2d* j) {
			Vec2 diff = i->getPos() - j->getPos();
			float diffLen = diff.length();
			float dist = diffLen - (i->getRadius() + j->getRadius());
			if (dist < 0) {
				if (i->isKinematic) i->setPos(i->getPos() - diff / diffLen * (dist / 2)); // * 0.5; //squishiness
				if (j->isKinematic) j->setPos(j->getPos() + diff / diffLen * (dist / 2)); // * 0.5;
			}
			else if (dist < 2) {
				if (i->isKinematic) i->setPos(i->getPos() - diff / diffLen * (dist / 2) * 0.001); // * 0.5; //squishiness
				if (j->isKinematic) j->setPos(j->getPos() + diff / diffLen * (dist / 2) * 0.001); // * 0.5;
			}
		}
		void squishy_collision(PhysicBody2d* i, PhysicBody2d* j) {
			Vec2 diff = i->getPos() - j->getPos();
			float diffLen = diff.length();
			float dist = diffLen - (i->getRadius() + j->getRadius());
			if (dist < 0) {
				if (i->isKinematic) i->setPos(i->getPos() - diff / diffLen * (dist / 2) * 0.5);
				if (j->isKinematic) j->setPos(j->getPos() + diff / diffLen * (dist / 2) * 0.5);
			}
		}
	}
}