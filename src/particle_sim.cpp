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
// #include "PhysicBody2d.h"
#include "PhysicBody3d.h"
// #include "PhysicSolver.h"
#include "PhysicSolver3d.h"
#include "PhysicExamples.h"
#include "OpenGLGraphics.h"
#include "KeyboardHandler.h"
#include "ColorConv.h"
// #include "ImageGenerator.h"
#include "GUI_elements.h"
#include <gl/glu.h>
#include <GL/glut.h>


int main(int argc, char** argv)
{
	OpenGLGraphics oglGraphics(700, 1.5f, 0);
	sf::Clock clock;
	sf::RenderWindow window(sf::VideoMode(770, 730), "Orbiting", sf::Style::Titlebar | sf::Style::Close, sf::ContextSettings(24, 8, 8, 4, 5)); //730
	KeyboardHandler keyboardHandler(window);
	oglGraphics.reshapeScreen(window.getSize());
    oglGraphics.initOpenGL(argc, argv);
	//sf::Event event;
	window.setFramerateLimit(60);
	std::cout << "hello" << std::endl;
	std::cout << GL_MAX_NAME_STACK_DEPTH << std::endl;

	sf::Font font; font.loadFromFile("fonts/arial.ttf");

	StatElement statElement(font);

	PhysicSolver3d sandbox(ChunkGrid3d(10, window.getSize().x, window.getSize().y, 300));
	PhysicDrawer3d sandbox_draw(sandbox);

	// bool mousePressed = false;

	bool paused = false;

	// std::pair<bool, PhysicBody3d*> dragBuffer;

	long long timeResults[7];

	#include "controls.tt"

	//srand(time(NULL));
	srand(5);
	sf::err().rdbuf(NULL);

	sandbox.set_acceleration(Vec3(0, 100, 0));
	// sandbox.set_acceleration(PhysicExamples::Acceleration::centreAcceleration({ 350,350 }, 10000));
	/*sandbox.set_acceleration([](const PhysicBody2d* obj, const std::vector<PhysicBody2d*>&) {
		return (obj->getOldPos() - obj->getPos()) * 600 * 0.9;
	});*/
	/*sandbox.set_acceleration([](const PhysicBody2d* obj, const std::vector<PhysicBody2d*>&) {
		float r = obj->getRadius();
		return Vec2(0, 1) * r * r * r;
		});*/
	// sandbox.set_constraints_def();
	sandbox.set_constraints(PhysicExamples3d::Constrains::boxRestrain({ 50,50,50 }, { 730,690,250 }));
	// sandbox.set_constraints(PhysicExamples::Constrains::boxRestrain({ 30,30 }, { 730,690 }));
	sandbox.getChunkGrid().set_collision_def();
	// sandbox.getChunkGrid().set_collision(PhysicExamples::Collisions::squishy_collision);
	//sandbox.set_constraints(PhysicExamples::Constrains::defaultConstrain);
	#if defined(ENABLE_OPENMP)
		std::cout << "OpenMP enabled\n";
	#endif

	bool dragEnable = false;
	bool shiftPressed = false;
	// bool kinematicStateBefore;

	int acc = 100;

	while (window.isOpen()) {
		window.clear();
		keyboardHandler.update();
		// if (window.pollEvent(event)) {
		// 	if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
		// 		window.close();
		// // 	//oglGraphics.reshapeScreen(window.getSize());
		// // 	keyboardHandler.handleKeyboard(event);

		// // 	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
		// // 		acc *= -1;
		// // 		sandbox.set_acceleration(Vec3(0, acc, 0));
		// // 	}
		// // 	else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
		// // 		paused = !paused;
		// // 	}
		// // 	else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::LShift) {
		// // 		shiftPressed = true;
		// // 	}
		// // 	else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::LShift) {
		// // 		shiftPressed = false;
		// // 	}
		// // 	else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up) {
		// // 		phiKey = 1;
		// // 	}else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down) {
		// // 		phiKey = -1;
		// // 	}else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left) {
		// // 		thetaKey = -1;
		// // 	}else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right) {
		// // 		thetaKey = 1;
		// // 	}else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Equal) {
		// // 		RKey = -1;
		// // 	}else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Dash) {
		// // 		RKey = 1;
		// // 	}
		// // 	else if (event.type == sf::Event::KeyReleased && (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down)) phiKey = 0;
		// // 	else if (event.type == sf::Event::KeyReleased && (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right)) thetaKey = 0;
		// // 	else if (event.type == sf::Event::KeyReleased && (event.key.code == sf::Keyboard::Equal || event.key.code == sf::Keyboard::Dash)) RKey = 0;
		// // 	//else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::L) oglGraphics.switchLight();
		// // 	// if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
		// // 	// 	mousePressed = true;
		// // 	// 	mousePosition.set(event.mouseButton.x, event.mouseButton.y);
		// // 	// }
		// // 	// else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
		// // 	// 	mousePressed = false;
		// // 	// 	mousePosition.set(event.mouseButton.x, event.mouseButton.y);
		// // 	// }
		// 	if (event.type == sf::Event::MouseMoved)
		// 		mousePosition.set(event.mouseMove.x, event.mouseMove.y);

		// // 	// if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right && shiftPressed) {
		// // 	// 	dragBuffer = sandbox.pop_from_position({ event.mouseButton.x,event.mouseButton.y });
		// // 	// 	if (dragBuffer.second != &PhysicBody2d::nullPB) {
		// // 	// 		dragEnable = true;
		// // 	// 		kinematicStateBefore = dragBuffer.second->isKinematic;
		// // 	// 		dragBuffer.second->isKinematic = false;
		// // 	// 	}
		// // 	// }
		// // 	// else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
		// // 	// 	dragBuffer = sandbox.pop_from_position({ event.mouseButton.x,event.mouseButton.y });
		// // 	// }
		// // 	// else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
		// // 	// 	if (dragBuffer.first && !dragEnable) {
		// // 	// 		auto pr = sandbox.pop_from_position({ event.mouseButton.x,event.mouseButton.y });
		// // 	// 		if (pr.first && pr.second != dragBuffer.second) sandbox.addLink(new PhysicLink2d(*dragBuffer.second, *pr.second, 100));
		// // 	// 		dragBuffer.first = false;
		// // 	// 	}
		// // 	// 	if (dragBuffer.first) dragBuffer.second->isKinematic = kinematicStateBefore;
		// // 	// 	dragEnable = false;
		// // 	// }

		// // 	// if (dragEnable && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
		// // 	// 	if (dragBuffer.first)
		// // 	// 		kinematicStateBefore = !kinematicStateBefore;
		// // 	// }
		// }

		// oglGraphics.theta += thetaKey * 0.1f;
		// oglGraphics.phi += phiKey * 0.1f;
		// oglGraphics.R += RKey * 10.f;
		// if(thetaKey || phiKey || RKey) oglGraphics.setCamera();

		constexpr int subStep = 2;

		if (!paused) {
			auto start = std::chrono::steady_clock::now();
			sandbox.update(timeResults, 1 / 30.f, subStep);
			auto end = std::chrono::steady_clock::now();

			statElement.simTimeAdd(int(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()));
		}

		std::chrono::steady_clock::time_point symBegin = std::chrono::steady_clock::now();

		statElement.update();

		oglGraphics.drawScreen(window.getSize());
        oglGraphics.drawAxes();
		window.draw(sandbox_draw);
		// if (dragEnable) {
		// 	dragBuffer.second->move(mousePosition);
		// }
		window.pushGLStates();
		window.draw(statElement);
		window.popGLStates();
		window.display();

		std::chrono::steady_clock::time_point symEnd = std::chrono::steady_clock::now();
        timeResults[6] = std::chrono::duration_cast<std::chrono::microseconds>(symEnd - symBegin).count();

		for (int i = 0; i < 7; i++)
			std::cout  << std::setw(7) << std::setprecision(2) << (timeResults[i]/1000.f * (i<6 ? subStep : 1));
		std::cout << "\n";
		// std::cout << mousePosition << "\n";
	}
}

//ACC | CON | LINK | GRID | COLL | POS | RNDR