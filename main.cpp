/*
 *  Exercise 3 - OpenGL Model & Viewer
 *  Milestone 3: Virtual Trackball
 *
 *  This file contains only GLUT setup and event callbacks.
 *  Trackball math lives in trackball.h/cpp.
 *  Scene drawing lives in drawing.h/cpp.
 */

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include "trackball.h"
#include "drawing.h"

// ---------------------------------------------------------------------------
// Callbacks
// ---------------------------------------------------------------------------

// Called whenever the window is resized. Updates the viewport and resets the
// perspective projection to match the new aspect ratio.
void reshape(int width, int height)
{
    if (height == 0) height = 1;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Called once at startup to configure global OpenGL state.
void initGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    trackballInit();
}

// Called every frame. Clears the screen, applies the trackball rotation,
// and draws the scene.
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Move camera back so the model at the origin is visible
    glTranslatef(0.0f, 0.0f, -6.0f);

    // Apply the cumulative trackball rotation
    trackballApply();

    drawCube();

    glutSwapBuffers();
}

// Called when a mouse button is pressed or released.
// Left button down starts a trackball drag; release ends it.
void mouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
            trackballMouseDown(x, y);
        else
            trackballMouseUp();
    }
}

// Called while the mouse moves with a button held down.
// Delegates to the trackball to update the rotation.
void mouseMotion(int x, int y)
{
    trackballMouseMove(x, y);
    glutPostRedisplay();
}

// Called on keyboard input. ESC quits the application.
void keyboard(unsigned char key, int x, int y)
{
    if (key == 27) glutLeaveMainLoop();
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Exercise 3 - Car Viewer");

    initGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);

    glutMainLoop();
    return 0;
}
