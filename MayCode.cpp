#include <GL/freeglut.h>
#include <iostream>
#include <cmath>

// ----------------------
// Globals
// ----------------------
float lastX, lastY;
bool isDragging = false;
float zoomDistance = 8.0f;

// rotation matrix
float rotMatrix[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

bool showAxis = true;
bool wireframe = false;
bool showLights = true;

int width = 800;
int height = 600;

GLUquadric *quad;

// ----------------------
// PROTOTYPES
// ----------------------
void drawAxes();
void drawChassis();
void drawWheel();
void drawHeadlight();
void drawCarHalf();

// ----------------------
// TRACKBALL
// ----------------------
void mapToSphere(float x, float y, float &vx, float &vy, float &vz)
{
    float d = x * x + y * y;

    if (d > 1.0f)
    {
        float norm = 1.0f / std::sqrt(d);
        vx = x * norm;
        vy = y * norm;
        vz = 0.0f;
    }
    else
    {
        vx = x;
        vy = y;
        vz = std::sqrt(1.0f - d);
    }
}

// ----------------------
// MOUSE
// ----------------------
void mouse(int button, int state, int x, int y)
{
    if (button == 3)
    {
        zoomDistance -= 0.3f;
        if (zoomDistance < 2.0f) zoomDistance = 2.0f;
        glutPostRedisplay();
        return;
    }
    if (button == 4)
    {
        zoomDistance += 0.3f;
        glutPostRedisplay();
        return;
    }

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            isDragging = true;
            lastX = (2.0f * x - width) / width;
            lastY = (height - 2.0f * y) / height;
        }
        else
        {
            isDragging = false;
        }
    }
}

// ----------------------
// MOTION
// ----------------------
void motion(int x, int y)
{
    if (!isDragging) return;

    float currX = (2.0f * x - width) / width;
    float currY = (height - 2.0f * y) / height;

    float v1x, v1y, v1z;
    float v2x, v2y, v2z;

    mapToSphere(lastX, lastY, v1x, v1y, v1z);
    mapToSphere(currX, currY, v2x, v2y, v2z);

    float ax = v1y * v2z - v1z * v2y;
    float ay = v1z * v2x - v1x * v2z;
    float az = v1x * v2y - v1y * v2x;

    float dot = v1x * v2x + v1y * v2y + v1z * v2z;

    if (dot > 1.0f) dot = 1.0f;
    if (dot < -1.0f) dot = -1.0f;

    float angle = std::acos(dot) * 180.0 / 3.14159265;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(angle, ax, ay, az);
    glMultMatrixf(rotMatrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, rotMatrix);

    lastX = currX;
    lastY = currY;

    glutPostRedisplay();
}

// ----------------------
// AXES
// ----------------------
void drawAxes()
{
    if (!showAxis) return;

    glBegin(GL_LINES);

    glColor3f(1, 0, 0);
    glVertex3f(-5, 0, 0);
    glVertex3f(5, 0, 0);

    glColor3f(0, 1, 0);
    glVertex3f(0, -5, 0);
    glVertex3f(0, 5, 0);

    glColor3f(0, 0, 1);
    glVertex3f(0, 0, -5);
    glVertex3f(0, 0, 5);

    glEnd();
}

// ----------------------
// CHASSIS
// ----------------------
void drawChassis()
{
    glBegin(GL_QUADS);

    glColor3f(0.2, 0.2, 0.8);

    glVertex3f(-2, -0.5, 1);
    glVertex3f(2, -0.5, 1);
    glVertex3f(2, 0.5, 1);
    glVertex3f(-2, 0.5, 1);

    glVertex3f(-2, -0.5, -1);
    glVertex3f(-2, 0.5, -1);
    glVertex3f(2, 0.5, -1);
    glVertex3f(2, -0.5, -1);

    glEnd();
}

// ----------------------
// WHEEL
// ----------------------
void drawWheel()
{
    glColor3f(0.05, 0.05, 0.05);

    glRotatef(90, 0, 1, 0);

    gluCylinder(quad, 0.4, 0.4, 0.3, 20, 20);

    gluDisk(quad, 0, 0.4, 20, 1);

    glTranslatef(0, 0, 0.3);
    gluDisk(quad, 0, 0.4, 20, 1);
}

// ----------------------
// HALF CAR
// ----------------------
void drawCarHalf()
{
    drawChassis();

    glColor3f(0.7, 0.9, 1.0);

    glBegin(GL_QUADS);
    glVertex3f(0.2, 0.4, 0.8);
    glVertex3f(1.5, 0.4, 0.8);
    glVertex3f(1.5, 1.0, 0.8);
    glVertex3f(0.2, 1.0, 0.8);
    glEnd();

    glPushMatrix();
    glTranslatef(1.2, -0.6, 1.1);
    drawWheel();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.2, -0.6, -1.1);
    drawWheel();
    glPopMatrix();
}

// ----------------------
// DISPLAY
// ----------------------
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    gluLookAt(0, 3, zoomDistance,
              0, 0, 0,
              0, 1, 0);

    glMultMatrixf(rotMatrix);

    drawAxes();

    glPushMatrix();

    drawCarHalf();

    glScalef(-1, 1, 1);
    drawCarHalf();

    glPopMatrix();

    glutSwapBuffers();
}

// ----------------------
// RESHAPE
// ----------------------
void reshape(int w, int h)
{
    if (h == 0) h = 1;

    width = w;
    height = h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0, (float)w / h, 0.1, 100);

    glMatrixMode(GL_MODELVIEW);
}

// ----------------------
// MAIN
// ----------------------
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Car");

    glEnable(GL_DEPTH_TEST);

    quad = gluNewQuadric();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();
}