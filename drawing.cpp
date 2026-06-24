#include "drawing.h"
#include <GL/gl.h>

// Draws a unit cube (side length 2, from -1 to 1 on each axis) centered at
// the origin. Each of the 6 faces is drawn as a quad with a distinct color,
// making it easy to verify orientation and rotation during development.
void drawCube()
{
    glBegin(GL_QUADS);
        // Front  (red)
        glColor3f(1,0,0);
        glVertex3f(-1,-1, 1); glVertex3f( 1,-1, 1);
        glVertex3f( 1, 1, 1); glVertex3f(-1, 1, 1);
        // Back   (green)
        glColor3f(0,1,0);
        glVertex3f(-1,-1,-1); glVertex3f(-1, 1,-1);
        glVertex3f( 1, 1,-1); glVertex3f( 1,-1,-1);
        // Top    (blue)
        glColor3f(0,0,1);
        glVertex3f(-1, 1,-1); glVertex3f(-1, 1, 1);
        glVertex3f( 1, 1, 1); glVertex3f( 1, 1,-1);
        // Bottom (yellow)
        glColor3f(1,1,0);
        glVertex3f(-1,-1,-1); glVertex3f( 1,-1,-1);
        glVertex3f( 1,-1, 1); glVertex3f(-1,-1, 1);
        // Right  (cyan)
        glColor3f(0,1,1);
        glVertex3f( 1,-1,-1); glVertex3f( 1, 1,-1);
        glVertex3f( 1, 1, 1); glVertex3f( 1,-1, 1);
        // Left   (magenta)
        glColor3f(1,0,1);
        glVertex3f(-1,-1,-1); glVertex3f(-1,-1, 1);
        glVertex3f(-1, 1, 1); glVertex3f(-1, 1,-1);
    glEnd();
}
