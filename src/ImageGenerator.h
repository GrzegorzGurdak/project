#ifndef IMG_GEN
#define IMG_GEN

#pragma once

//#include <string>
#include <fstream>
#include <deque>
#include <SFML/Graphics.hpp>
#include "PhysicBody2d.h"
#include "PhysicSolver.h"
#include "GUI_elements.h"

class ColorMap {
public:
	ColorMap(std::string imgName, std::string result, Vec2 beg ,Vec2 size) {
		sf::Image image;
		image.loadFromFile(imgName);
		// float scaleX;
		// float scaleY;

		std::ifstream input(result);
		while (input.good()) {
			Vec2 pos;
			float radius;
			input >> pos.x >> pos.y >> radius;
			sf::Color col;
			unsigned x = unsigned((pos.x - beg.x)/(size.x - beg.x) * image.getSize().x);
			unsigned y = unsigned((pos.y - beg.y) / (size.y - beg.y) * image.getSize().y);
			if (x >= 0 && y >= 0 && x < image.getSize().x && y < image.getSize().y)
				col = image.getPixel(x, y);
			deq.push_back(col);
		}
	}

	void pour(PhysicSolver& sandbox, StatElement& statElement, int numOfParticle = 9e3){
		if (sandbox.getObjectAmount() < numOfParticle) {
			for (int i = 0; i < 5; i++)
				sandbox.add(
					new PhysicBody2d(Vec2{ 350, 300 } + Vec2::random_rad(40),
						(double)rand() / RAND_MAX * 3 + 2, this->getNext()));
			statElement.objectAmountUpdate(sandbox.getObjectAmount());
		}
	}

	sf::Color getNext() {
		if (deq.empty())
			return sf::Color::Black;
		sf::Color col = deq.front();
		deq.pop_front();
		return col;
	}

private:
	std::deque<sf::Color> deq;
};

namespace ImageGenerator
{

	void exportResult(const PhysicSolver& obj, std::string fname) {
		const std::vector<PhysicBody2d*> &vec = obj.getObjects();
		std::ofstream out(fname);
		for (auto& i : vec) {
			out << i->getPos().x << " " << i->getPos().y << " " << i->getRadius() << std::endl;
		}
	}

	//void generateColor(std::)
};

#endif // !IMG_GEN