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
#include "PhysicSolver2d.h"
#include "PhysicExamples.h"
#include "OpenGLGraphics.h"
#include "ColorConv.h"
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

	SimStat statElement(font);

	// PhysicSolver2d sandbox(ChunkGrid(int(particleSize) * 2, window.getSize().x, window.getSize().y)); //30,33,36
	PhysicSolver2d sandbox = *PhysicExamples::Sandbox::game(window.getSize(), int(particleSize));
	Selector selector(sandbox.getChunkGrid());
	PhysicDrawer sandbox_draw(sandbox, window.getSize(), particleSize);
	GameLogic gameLogic(sandbox); //WIP

	bool lMousePressed = false;
	bool rMousePressed = false;
	Vec2 mousePosition;
	float cursorSize = 40;

	bool paused = false;

	std::pair<bool, PhysicBody2d*> dragBuffer;

	srand(5);
	sf::err().rdbuf(NULL);

	sandbox.set_acceleration(Vec2(0, 100));
	sandbox.set_constraints(PhysicExamples::Constrains::boxRestrain({ 30,30 }, { 730,690 }));

	window.setMouseCursorVisible(false);

	while (window.isOpen()) {
		window.clear();
		if (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				if(event.key.code == sf::Keyboard::Space) paused = !paused;
				else if(event.key.code == sf::Keyboard::Equal && cursorSize < 100) cursorSize += 1;
				else if(event.key.code == sf::Keyboard::Dash && cursorSize > 1) cursorSize -= 1;
				else if(event.key.code == sf::Keyboard::C) {
					sandbox.clear();
				}
			}

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				lMousePressed = true;
				mousePosition.set(event.mouseButton.x, event.mouseButton.y);
			}
			else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
				lMousePressed = false;
				mousePosition.set(event.mouseButton.x, event.mouseButton.y);
			}
			else if (event.type == sf::Event::MouseMoved)
				mousePosition.set(event.mouseMove.x, event.mouseMove.y);

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
				rMousePressed = true;
				mousePosition.set(event.mouseButton.x, event.mouseButton.y);
			}
			if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
				rMousePressed = false;
				mousePosition.set(event.mouseButton.x, event.mouseButton.y);
			}
		}

		if (rMousePressed && clock.getElapsedTime().asMilliseconds() > 10) {
			for (int i = 0; i < 30; i++){
				sandbox.insert(
					mousePosition + Vec2::random_rad(cursorSize),
					particleSize, false, sf::Color(90,50,20));
			}

			clock.restart();
			statElement.objectAmountUpdate(sandbox.getObjectAmount());
		}
		else if (lMousePressed){
			selector.select(mousePosition, cursorSize);

			for( auto &i : selector.getSelected())
				if(!i->isKinematic)
					sandbox.deleteObject(i);
		}

		if (!paused) {
			auto start = std::chrono::steady_clock::now();
			sandbox.update(1 / 60.f, 8);
			auto end = std::chrono::steady_clock::now();

			statElement.simTimeAdd(int(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()));
		}

		selector.select(Vec2(570,570), 100);
		if(selector.getSelected().size() > 400){
			statElement.setWinStatus(true);
		}


		statElement.update(event);

		oglGraphics.drawScreen(window.getSize());
        oglGraphics.drawAxes();

		window.draw(sandbox_draw);

		window.pushGLStates();
		window.draw(statElement);
		window.popGLStates();
		oglGraphics.drawCursor(mousePosition.x, window.getSize().y - mousePosition.y, cursorSize, 20, false);
		window.display();
	}
}

//ACC | CON | LINK | GRID | COLL | POS