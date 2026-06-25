/*
 *  Exercise 3 - OpenGL Model & Viewer
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
// Scene state
// ---------------------------------------------------------------------------

static bool showLights = true;

// Positional light positions in world space (car is at origin).
// w=1 makes them positional (not directional).
// Positions chosen so the indicator spheres are visible in the default view.
// At camera distance 6, the frustum at z_world=-2 covers roughly x∈[-3.3,3.3],
// y∈[-2.5,2.5] — both positions sit comfortably inside that.
static const GLfloat LIGHT0_POS[4] = {  1.0f, 1.5f,  2.0f, 1.0f }; // warm key, rear-right-above
static const GLfloat LIGHT1_POS[4] = { -1.0f, 1.5f, -2.0f, 1.0f }; // cool fill, front-left-above

// ---------------------------------------------------------------------------
// Callbacks
// ---------------------------------------------------------------------------

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

void initGL()
{
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);

    // GL_NORMALIZE recalculates normal length after glScalef(-1,1,1) distorts them.
    glEnable(GL_NORMALIZE);

    // GL_COLOR_MATERIAL lets glColor3f drive the ambient+diffuse material,
    // so drawing.cpp doesn't need to call glMaterialfv for every color change.
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glEnable(GL_LIGHTING);

    // Soft global ambient so shadowed faces aren't pure black
    GLfloat globalAmb[] = { 0.12f, 0.12f, 0.14f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);

    // Light 0 — warm key light (demonstrates diffuse + specular)
    GLfloat l0amb[]  = { 0.00f, 0.00f, 0.00f, 1.0f };
    GLfloat l0diff[] = { 1.00f, 0.95f, 0.80f, 1.0f };
    GLfloat l0spec[] = { 1.00f, 1.00f, 0.90f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT,  l0amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  l0diff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, l0spec);
    glEnable(GL_LIGHT0);

    // Light 1 — cool fill light (second positional source)
    GLfloat l1amb[]  = { 0.00f, 0.00f, 0.00f, 1.0f };
    GLfloat l1diff[] = { 0.40f, 0.45f, 0.65f, 1.0f };
    GLfloat l1spec[] = { 0.30f, 0.35f, 0.50f, 1.0f };
    glLightfv(GL_LIGHT1, GL_AMBIENT,  l1amb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  l1diff);
    glLightfv(GL_LIGHT1, GL_SPECULAR, l1spec);
    glEnable(GL_LIGHT1);

    trackballInit();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Pull the camera back so the car at the origin is fully visible.
    glTranslatef(0.0f, 0.0f, -6.0f);

    // Set light positions NOW (after camera pullback but BEFORE trackball).
    // OpenGL transforms positions by the current modelview matrix and stores
    // them in eye space, so placing them here keeps the lights world-fixed
    // while the model rotates under the trackball.
    glLightfv(GL_LIGHT0, GL_POSITION, LIGHT0_POS);
    glLightfv(GL_LIGHT1, GL_POSITION, LIGHT1_POS);

    // Draw light indicator spheres at the same world positions (also world-fixed).
    if (showLights)
    {
        drawLightSphere(LIGHT0_POS[0], LIGHT0_POS[1], LIGHT0_POS[2],
                        1.0f, 0.95f, 0.75f);   // warm yellow
        drawLightSphere(LIGHT1_POS[0], LIGHT1_POS[1], LIGHT1_POS[2],
                        0.5f, 0.60f, 0.90f);   // cool blue
    }

    // Apply the trackball rotation — only the model rotates, lights stay put.
    trackballApply();

    drawCar();

    glutSwapBuffers();
}

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

void mouseMotion(int x, int y)
{
    trackballMouseMove(x, y);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'l': case 'L':
        showLights = !showLights;
        glutPostRedisplay();
        break;
    case 27:
        glutLeaveMainLoop();
        break;
    }
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
