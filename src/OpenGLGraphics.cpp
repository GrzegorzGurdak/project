#include "OpenGLGraphics.h"
#include <SFML/OpenGL.hpp>
#include <gl/glu.h>
// #include <GL/glut.h>

void OpenGLGraphics::initOpenGL(int argc, char** argv) {
    glClearColor(1.f, 1.f, 1.f, 0.f);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	// glutInit(&argc, argv);
    enableLight();
}

void OpenGLGraphics::setCamera() {
    glLoadIdentity();
    // gluLookAt(
    //     400 + R * cosf(theta) * cosf(phi), 300 + R * sinf(phi), 150 + R * sinf(theta) * cosf(phi),
    //     400,   300,   150,
    //     0,   R * cosf(phi),   0
    // );
}

void OpenGLGraphics::reshapeScreen(sf::Vector2u size) {
    glViewport(0, 0, (GLsizei)size.x, (GLsizei)size.y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // gluPerspective(60.0, (GLdouble)size.x / (GLdouble)size.y, 0.1, 2000.0);
    gluOrtho2D(0, size.x, 0, size.y);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    setCamera();
}

void OpenGLGraphics::drawScreen(sf::Vector2u size) {
    glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void OpenGLGraphics::drawAxes() {
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

void OpenGLGraphics::enableLight() {
    glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
}

void OpenGLGraphics::disableLight() {
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
}

void OpenGLGraphics::switchLight() {
    if(glIsEnabled(GL_LIGHTING)){
        disableLight();
    } else {
        enableLight();
    }
}

void OpenGLGraphics::drawCursor(float cx, float cy, float r, int num_segments, bool isShiftPressed)
{
    glColor3f(1,1,1);
    if (isShiftPressed) {
        int size = 4;
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex3f(cx - size, cy, 1);
        glVertex3f(cx + size, cy, 1);
        glVertex3f(cx, cy - size, 1);
        glVertex3f(cx, cy + size, 1);
        glEnd();
    }
    else
        drawCircle(cx, cy, r, num_segments, 1);
}

void OpenGLGraphics::drawCircle(float cx, float cy, float r, int num_segments, int depth)
{
    float theta = 3.1415926f * 2 / float(num_segments);
    float tangetial_factor = tanf(theta);

    float radial_factor = cosf(theta);

    float x = r;

    float y = 0;
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < num_segments; i++)
    {
        glVertex3f(x + cx, y + cy, depth);

        float tx = -y;
        float ty = x;

        x += tx * tangetial_factor;
        y += ty * tangetial_factor;

        x *= radial_factor;
        y *= radial_factor;
    }
    glEnd();
}