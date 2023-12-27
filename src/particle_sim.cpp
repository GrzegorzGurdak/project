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
#include "Selector.hpp"
#include "PhysicBody2d.h"
#include "PhysicSolver.h"
#include "PhysicExamples.h"
#include "OpenGLGraphics.h"
#include "ColorConv.h"
// #include "ImageGenerator.h"
#include "GUI_elements.h"
#include "GameLogic.h"
#include <gl/glu.h>

int main(int argc, char** argv)
{
	OpenGLGraphics oglGraphics(700, 1.5f, 0);

	float particleSize = 4;

	sf::Clock clock;
	sf::RenderWindow window(sf::VideoMode(770, 730), "Orbiting", sf::Style::Titlebar | sf::Style::Close, sf::ContextSettings(24, 8, 8, 4, 5)); //730
	sf::Event event;
	window.setFramerateLimit(60);

	oglGraphics.initOpenGL(argc, argv);
	oglGraphics.reshapeScreen(window.getSize());

	sf::Font font; font.loadFromFile("fonts/arial.ttf");

	StatElement statElement(font);

	PhysicSolver sandbox(ChunkGrid(int(particleSize) * 2, window.getSize().x, window.getSize().y)); //30,33,36
	Selector selector(sandbox.getChunkGrid());
	// \/  \/  \/  MEMORY LEAK!!!!!!!!!!!!!!!!!!!!!!!!!!!!\/  \/  \/
	//PhysicSolver sandbox = *PhysicExamples::Sandbox::cloth(window.getSize(), {250,200},10,15); // losing pointer to allocated data
	PhysicDrawer sandbox_draw(sandbox, window.getSize(), particleSize);
	GameLogic gameLogic(sandbox);
	//sandbox.add(PhysicBody2d(Vec2(150, 180),5)).add(PhysicBody2d(Vec2(450, 180),5));

	bool mousePressed = false;
	Vec2 mousePosition;
	float cursorSize = 40;

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
	// sandbox.set_constraints_def();
	// sandbox.getChunkGrid().set_collision(PhysicExamples::Collisions::collision_with_viscosity);
	sandbox.set_constraints(PhysicExamples::Constrains::boxRestrain({ 30,30 }, { 730,690 }));
	// sandbox.getChunkGrid().set_collision(PhysicExamples::Collisions::squishy_collision);
	// sandbox.set_constraints(PhysicExamples::Constrains::circleRestrain({ 350,350 }, 300));

	// ColorMap colormap("fonts/mem.png", "result", { 30,30 }, { 730,690 });

	bool dragEnable = false;
	bool shiftPressed = false;
	bool kinematicStateBefore;

	PhysicBody2d* debugBuffer;

	window.setMouseCursorVisible(false);

	while (window.isOpen()) {
		window.clear();
		if (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
				window.close();

			// if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
			// 	ImageGenerator::exportResult(sandbox, "result");
			// }
			if (event.type == sf::Event::KeyPressed) {
				if(event.key.code == sf::Keyboard::Space) paused = !paused;
				else if(event.key.code == sf::Keyboard::Equal && cursorSize < 100) cursorSize += 1;
				else if(event.key.code == sf::Keyboard::Dash && cursorSize > 1) cursorSize -= 1;
				else if (event.key.code == sf::Keyboard::LShift) shiftPressed = true;
				else if (event.key.code == sf::Keyboard::R && dragEnable){
					if (dragBuffer.first)
						kinematicStateBefore = !kinematicStateBefore;
				}
				else if(event.key.code == sf::Keyboard::D) {
					selector.select(mousePosition, cursorSize);
					std::cout<<selector.getSelected().size()<<"\n";

					for( auto &i : selector.getSelected())
						sandbox.deleteObject(i);
				}
				else if(event.key.code == sf::Keyboard::M) {
					selector.select(mousePosition, cursorSize);
					std::cout<<selector.getSelected().size()<<"\n";

					for( auto &i : selector.getSelected())
						if (i != dragBuffer.second)
							sandbox.addLink(new PhysicLink2d(*dragBuffer.second, *i, 100));
				}
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
					debugBuffer = dragBuffer.second;
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
		}

		if (mousePressed && clock.getElapsedTime().asMilliseconds() > 10) {
			for (int i = 0; i < 30; i++){
				sandbox.add(
					mousePosition + Vec2::random_rad(cursorSize),
					particleSize,
					!shiftPressed,
					(shiftPressed ? sf::Color(90,50,20) : sf::Color(20,20,255)));
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

		// for (int i = 0; i < 6; i++)
		// 	std::cout << simResult[i] << std::setw(6);
		// std::cout << "\n";
		std::cout << "Body: pos:" << debugBuffer->getPos() << ", oldPos:" << debugBuffer->getOldPos() << "\n";

		statElement.update(event);

		oglGraphics.drawScreen(window.getSize());
        oglGraphics.drawAxes();

		window.draw(sandbox_draw);
		// if (dragEnable) {
		// 	dragBuffer.second->move(mousePosition);
		// }
		window.pushGLStates();
		window.draw(statElement);
		window.popGLStates();
		if (dragEnable) {
			dragBuffer.second->move(mousePosition);
		}
		oglGraphics.drawCursor(mousePosition.x, window.getSize().y - mousePosition.y, cursorSize, 20, shiftPressed);
		window.display();
	}
}

//ACC | CON | LINK | GRID | COLL | POS