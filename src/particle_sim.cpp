// particle_sim.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <chrono>

#include "MVec.h"
#include "PhysicBody2d.h"
#include "PhysicSolver.h"
#include "PhysicExamples.h"
#include "ColorConv.h"
#include "ImageGenerator.h"
#include "GUI_elements.h"
#include <gl/glu.h>

int main()
{
	sf::Clock clock;
	sf::RenderWindow window(sf::VideoMode(770, 730), "Orbiting", sf::Style::Titlebar | sf::Style::Close); //730
	sf::Event event;
	window.setFramerateLimit(60);

	sf::Font font; font.loadFromFile("fonts/arial.ttf");

	StatElement statElement(font);

	PhysicSolver sandbox(ChunkGrid(4, window.getSize().x, window.getSize().y)); //30,33,36
	// \/  \/  \/  MEMORY LEAK!!!!!!!!!!!!!!!!!!!!!!!!!!!!\/  \/  \/
	//PhysicSolver sandbox = *PhysicExamples::Sandbox::cloth(window.getSize(), {250,200},10,15); // losing pointer to allocated data
	PhysicDrawer sandbox_draw(sandbox);
	//sandbox.add(PhysicBody2d(Vec2(150, 180),5)).add(PhysicBody2d(Vec2(450, 180),5));

	bool mousePressed = false;
	Vec2 mousePosition;

	bool paused = false;

	std::pair<bool, PhysicBody2d*> dragBuffer;

	long long simResult[6];

	//srand(time(NULL));
	srand(5);
	sf::err().rdbuf(NULL);

	sandbox.set_acceleration(Vec2(0, 100));
	// sandbox.set_acceleration(PhysicExamples::Acceleration::centreAcceleration({ 350,350 }, 10000));
	/*sandbox.set_acceleration([](const PhysicBody2d* obj, const std::vector<PhysicBody2d*>&) {
		return (obj->getOldPos() - obj->getPos()) * 600 * 0.9;
	});*/
	/*sandbox.set_acceleration([](const PhysicBody2d* obj, const std::vector<PhysicBody2d*>&) {
		float r = obj->getRadius();
		return Vec2(0, 1) * r * r * r;
		});*/
	//sandbox.set_constraints_def();
	sandbox.set_constraints(PhysicExamples::Constrains::boxRestrain({ 30,30 }, { 730,690 }));
	sandbox.getChunkGrid().set_collision(PhysicExamples::Collisions::squishy_collision);
	//sandbox.set_constraints(PhysicExamples::Constrains::defaultConstrain);

	ColorMap colormap("fonts/mem.png", "result", { 30,30 }, { 730,690 });

	bool dragEnable = false;
	bool shiftPressed = false;
	bool kinematicStateBefore;

	while (window.isOpen()) {
		window.clear();
		if (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
				window.close();

			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
				ImageGenerator::exportResult(sandbox, "result");
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
				paused = !paused;
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::LShift) {
				shiftPressed = true;
			}
			else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::LShift) {
				shiftPressed = false;
			}


			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				mousePressed = true;
				mousePosition.set(event.mouseButton.x, event.mouseButton.y);
			}
			else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
				mousePressed = false;
				mousePosition.set(event.mouseButton.x, event.mouseButton.y);
			}
			else if (event.type == sf::Event::MouseMoved)
				mousePosition.set(event.mouseMove.x, event.mouseMove.y);

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right && shiftPressed) {
				dragBuffer = sandbox.pop_from_position({ event.mouseButton.x,event.mouseButton.y });
				if (dragBuffer.second != &PhysicBody2d::nullPB) {
					dragEnable = true;
					kinematicStateBefore = dragBuffer.second->isKinematic;
					dragBuffer.second->isKinematic = false;
				}
			}
			else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
				dragBuffer = sandbox.pop_from_position({ event.mouseButton.x,event.mouseButton.y });
			}
			else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
				if (dragBuffer.first && !dragEnable) {
					auto pr = sandbox.pop_from_position({ event.mouseButton.x,event.mouseButton.y });
					if (pr.first && pr.second != dragBuffer.second) sandbox.addLink(new PhysicLink2d(*dragBuffer.second, *pr.second, 100));
					dragBuffer.first = false;
				}
				if (dragBuffer.first) dragBuffer.second->isKinematic = kinematicStateBefore;
				dragEnable = false;
			}

			if (dragEnable && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
				if (dragBuffer.first)
					kinematicStateBefore = !kinematicStateBefore;
			}
		}

		// colormap.pour(sandbox, statElement,6e3);

		if (mousePressed && clock.getElapsedTime().asMilliseconds() > 10) {
			for (int i = 0; i < 10; i++){
				PhysicBody2d* pb = new PhysicBody2d(
					mousePosition + Vec2::random_rad(40), 4.f,//(double)rand() / RAND_MAX * 3 + 2,
					ColorConv::hsvToRgb((sandbox.getObjectAmount() / 5 % 256) / 256., 1, 1)
				);
				if (shiftPressed) pb->isKinematic = false;
				sandbox.add(pb);
			}

			clock.restart();
			statElement.objectAmountUpdate(sandbox.getObjectAmount());
		}
		if (!paused) {
			auto start = std::chrono::steady_clock::now();
			sandbox.update(simResult, 1 / 60.f, 8);
			auto end = std::chrono::steady_clock::now();

			statElement.simTimeAdd(int(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()));
		}

		for (int i = 0; i < 6; i++)
			std::cout << simResult[i] << std::setw(6);
		std::cout << "\n";

		statElement.update(event);

		window.draw(sandbox_draw);
		if (dragEnable) {
			dragBuffer.second->move(mousePosition);
		}
		window.draw(statElement);
		window.display();
	}
}

//ACC | CON | LINK | GRID | COLL | POS