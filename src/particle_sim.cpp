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
#include "ColorConv.h"
// #include "ImageGenerator.h"
#include "GUI_elements.h"
#include <gl/glu.h>
#include <GL/glut.h>

void initOpenGL(int argc, char** argv) {
    glClearColor(1.f, 1.f, 1.f, 0.f);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glutInit(&argc, argv);
}

void reshapeScreen(sf::Vector2u size) {
    glViewport(0, 0, (GLsizei)size.x, (GLsizei)size.y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLdouble)size.x / (GLdouble)size.y, 0.1, 2000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

float R=700,theta = 1.5f,phi;

void drawScreen(sf::Vector2u size) {
    glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(
        400 + R * cosf(theta) * cosf(phi), 300 + R * sinf(phi), 150 + R * sinf(theta) * cosf(phi),
        400,   300,   150,
        0,   1,   0
    );
}

void drawAxes() {
    glLineWidth((GLfloat)3);
    glBegin(GL_LINES);
        glColor3f(1, 0, 0);
        glVertex3f(800, 0, 0);
        glVertex3f(0, 0, 0);

        glColor3f(0, 1, 0);
        glVertex3f(0, 700, 0);
        glVertex3f(0, 0, 0);

        glColor3f(0, 0, 1);
        glVertex3f(0, 0, 1);
        glVertex3f(0, 0, 0);
    glEnd();

    glLineStipple(3, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
        glColor3f(1, 0, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(-1, 0, 0);
        glColor3f(0, 1, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, -1, 0);
        glColor3f(0, 0, 1);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, -1);
    glEnd();
	glDisable(GL_LINE_STIPPLE);
}

int main(int argc, char** argv)
{
	sf::Clock clock;
	sf::RenderWindow window(sf::VideoMode(770, 730), "Orbiting", sf::Style::Titlebar | sf::Style::Close, sf::ContextSettings(24, 8, 8, 4, 5)); //730
	sf::Event event;
	window.setFramerateLimit(60);
	std::cout << "hello" << std::endl;

	sf::Font font; font.loadFromFile("fonts/arial.ttf");

	StatElement statElement(font);

	PhysicSolver3d sandbox(ChunkGrid3d(20, window.getSize().x, window.getSize().y, 300)); //30,33,36
	// \/  \/  \/  MEMORY LEAK!!!!!!!!!!!!!!!!!!!!!!!!!!!!\/  \/  \/
	//PhysicSolver sandbox = *PhysicExamples::Sandbox::cloth(window.getSize(), {250,200},10,15); // losing pointer to allocated data
	PhysicDrawer3d sandbox_draw(sandbox);
	//sandbox.add(PhysicBody2d(Vec2(150, 180),5)).add(PhysicBody2d(Vec2(450, 180),5));

	bool mousePressed = false;
	Vec2 mousePosition;

	bool paused = false;

	// std::pair<bool, PhysicBody3d*> dragBuffer;

	long long simResult[6];

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
	sandbox.set_constraints(PhysicExamples3d::Constrains::boxRestrain({ 30,30,30 }, { 730,690,270 }));
	// sandbox.set_constraints(PhysicExamples::Constrains::boxRestrain({ 30,30 }, { 730,690 }));
	sandbox.getChunkGrid().set_collision_def();
	// sandbox.getChunkGrid().set_collision(PhysicExamples::Collisions::squishy_collision);
	//sandbox.set_constraints(PhysicExamples::Constrains::defaultConstrain);

	// ColorMap colormap("fonts/mem.png", "result", { 30,30 }, { 730,690 });

	bool dragEnable = false;
	bool shiftPressed = false;
	// bool kinematicStateBefore;

	reshapeScreen(window.getSize());
    initOpenGL(argc, argv);

	int RKey = 0;
	int thetaKey = 0;
	int phiKey = 0;

	while (window.isOpen()) {
		window.clear();
		if (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
				window.close();

			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
				// ImageGenerator::exportResult(sandbox, "result");
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
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up) {
				phiKey = 1;
			}else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down) {
				phiKey = -1;
			}else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left) {
				thetaKey = -1;
			}else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right) {
				thetaKey = 1;
			}else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Equal) {
				RKey = -1;
			}else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Dash) {
				RKey = 1;
			}
			else if (event.type == sf::Event::KeyReleased && (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down)) phiKey = 0;
			else if (event.type == sf::Event::KeyReleased && (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right)) thetaKey = 0;
			else if (event.type == sf::Event::KeyReleased && (event.key.code == sf::Keyboard::Equal || event.key.code == sf::Keyboard::Dash)) RKey = 0;
			theta += thetaKey * 0.1f;
			phi += phiKey * 0.1f;
			R += RKey * 10.f;

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

			// if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right && shiftPressed) {
			// 	dragBuffer = sandbox.pop_from_position({ event.mouseButton.x,event.mouseButton.y });
			// 	if (dragBuffer.second != &PhysicBody2d::nullPB) {
			// 		dragEnable = true;
			// 		kinematicStateBefore = dragBuffer.second->isKinematic;
			// 		dragBuffer.second->isKinematic = false;
			// 	}
			// }
			// else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
			// 	dragBuffer = sandbox.pop_from_position({ event.mouseButton.x,event.mouseButton.y });
			// }
			// else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
			// 	if (dragBuffer.first && !dragEnable) {
			// 		auto pr = sandbox.pop_from_position({ event.mouseButton.x,event.mouseButton.y });
			// 		if (pr.first && pr.second != dragBuffer.second) sandbox.addLink(new PhysicLink2d(*dragBuffer.second, *pr.second, 100));
			// 		dragBuffer.first = false;
			// 	}
			// 	if (dragBuffer.first) dragBuffer.second->isKinematic = kinematicStateBefore;
			// 	dragEnable = false;
			// }

			// if (dragEnable && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
			// 	if (dragBuffer.first)
			// 		kinematicStateBefore = !kinematicStateBefore;
			// }
		}

		// colormap.pour(sandbox, statElement,6e3);

		if (mousePressed && clock.getElapsedTime().asMilliseconds() > 10) {
			for (int i = 0; i < 2; i++){
				// PhysicBody2d* pb = new PhysicBody2d(
				// 	mousePosition + Vec2::random_rad(40), 5.f,//(double)rand() / RAND_MAX * 3 + 2,
				// 	ColorConv::hsvToRgb((sandbox.getObjectAmount() / 5 % 256) / 256., 1, 1)
				// );
				PhysicBody3d* pb = new PhysicBody3d(
					Vec3(mousePosition.x, mousePosition.y, 150.f) + Vec3::random_rad(40), 20.f,//(double)rand() / RAND_MAX * 3 + 2,
					ColorConv::hsvToRgb((sandbox.getObjectAmount() * 15 % 256) / 256., 1, 1)
				);
				if (shiftPressed) pb->isKinematic = false;
				sandbox.add(pb);
			}

			clock.restart();
			statElement.objectAmountUpdate(sandbox.getObjectAmount());
		}
		if (!paused) {
			auto start = std::chrono::steady_clock::now();
			sandbox.update(simResult, 1 / 30.f, 4);
			auto end = std::chrono::steady_clock::now();

			statElement.simTimeAdd(int(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()));
		}

		// for (int i = 0; i < 6; i++)
		// 	std::cout << simResult[i] << std::setw(6);
		// std::cout << "\n";

		statElement.update(event);

		drawScreen(window.getSize());
        drawAxes();
		window.draw(sandbox_draw);
		// if (dragEnable) {
		// 	dragBuffer.second->move(mousePosition);
		// }
		window.pushGLStates();
		window.draw(statElement);
		window.popGLStates();
		window.display();
	}
}

//ACC | CON | LINK | GRID | COLL | POS