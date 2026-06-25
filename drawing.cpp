#include "drawing.h"
#include <GL/gl.h>
#include <GL/glu.h>

// ---------------------------------------------------------------------------
// Car layout (all units in OpenGL world space, car centred at origin)
//
//  Z axis  : car length  (front = -2.0, rear = +2.0)
//  X axis  : car width   (right side x > 0)
//  Y axis  : height      (tire bottoms ~-0.88, cabin roof ~+0.85)
//
//  Symmetry: right half is modelled; left half is obtained with
//  glScalef(-1,1,1) + glFrontFace(GL_CW) to maintain correct winding.
// ---------------------------------------------------------------------------

static GLUquadric *quad = nullptr;

static void ensureQuad()
{
    if (!quad)
    {
        quad = gluNewQuadric();
        gluQuadricNormals(quad, GLU_SMOOTH);
    }
}

// Sets specular colour, shininess, and emission in one call.
// glColor3f (via GL_COLOR_MATERIAL) still drives ambient+diffuse,
// so callers only need this for the non-colour properties.
static void setMaterial(float sr, float sg, float sb, float shine,
                        float er = 0.0f, float eg = 0.0f, float eb = 0.0f)
{
    GLfloat spec[] = { sr, sg, sb, 1.0f };
    GLfloat emit[] = { er, eg, eb, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR,  spec);
    glMaterialf (GL_FRONT, GL_SHININESS, shine);
    glMaterialfv(GL_FRONT, GL_EMISSION,  emit);
}

// ---------------------------------------------------------------------------
// Box helper — all six faces wound CCW from outside, with per-face normals.
// ---------------------------------------------------------------------------
static void drawBox(float x1, float y1, float z1,
                    float x2, float y2, float z2)
{
    glBegin(GL_QUADS);

    // Front  (z = z1), normal  -Z
    glNormal3f(0, 0, -1);
    glVertex3f(x1, y1, z1); glVertex3f(x1, y2, z1);
    glVertex3f(x2, y2, z1); glVertex3f(x2, y1, z1);

    // Back   (z = z2), normal  +Z
    glNormal3f(0, 0, 1);
    glVertex3f(x2, y1, z2); glVertex3f(x2, y2, z2);
    glVertex3f(x1, y2, z2); glVertex3f(x1, y1, z2);

    // Left   (x = x1), normal -X
    glNormal3f(-1, 0, 0);
    glVertex3f(x1, y1, z2); glVertex3f(x1, y2, z2);
    glVertex3f(x1, y2, z1); glVertex3f(x1, y1, z1);

    // Right  (x = x2), normal +X
    glNormal3f(1, 0, 0);
    glVertex3f(x2, y1, z1); glVertex3f(x2, y2, z1);
    glVertex3f(x2, y2, z2); glVertex3f(x2, y1, z2);

    // Bottom (y = y1), normal -Y
    glNormal3f(0, -1, 0);
    glVertex3f(x1, y1, z1); glVertex3f(x2, y1, z1);
    glVertex3f(x2, y1, z2); glVertex3f(x1, y1, z2);

    // Top    (y = y2), normal +Y
    glNormal3f(0, 1, 0);
    glVertex3f(x1, y2, z2); glVertex3f(x2, y2, z2);
    glVertex3f(x2, y2, z1); glVertex3f(x1, y2, z1);

    glEnd();
}

// ---------------------------------------------------------------------------
// Symmetric parts (drawn once, shared by both halves)
// ---------------------------------------------------------------------------

static void drawChassis()
{
    // Shiny car paint — specular highlight demonstrates specular lighting
    setMaterial(0.80f, 0.82f, 1.00f, 64.0f);
    glColor3f(0.20f, 0.28f, 0.72f);
    drawBox(-0.90f, -0.50f, -2.00f,
             0.90f,  0.15f,  2.00f);
}

static void drawCabin()
{
    setMaterial(0.80f, 0.82f, 1.00f, 64.0f);
    glColor3f(0.25f, 0.33f, 0.80f);
    drawBox(-0.78f, 0.15f, -1.05f,
             0.78f, 0.85f,  1.25f);
}

// Front windshield and rear window — symmetric, drawn once each.
// Placed 0.01 units proud of the cabin faces to avoid z-fighting.
static void drawWindshields()
{
    // Glass — very high specular demonstrates specular lighting on a flat surface
    setMaterial(0.95f, 0.98f, 1.00f, 120.0f);
    glColor3f(0.60f, 0.85f, 1.00f);

    // Front windshield (normal -Z)
    glNormal3f(0, 0, -1);
    glBegin(GL_QUADS);
    glVertex3f(-0.76f, 0.22f, -1.06f);
    glVertex3f(-0.76f, 0.78f, -1.06f);
    glVertex3f( 0.76f, 0.78f, -1.06f);
    glVertex3f( 0.76f, 0.22f, -1.06f);
    glEnd();

    // Rear window (normal +Z)
    glNormal3f(0, 0, 1);
    glBegin(GL_QUADS);
    glVertex3f( 0.76f, 0.22f, 1.26f);
    glVertex3f( 0.76f, 0.78f, 1.26f);
    glVertex3f(-0.76f, 0.78f, 1.26f);
    glVertex3f(-0.76f, 0.22f, 1.26f);
    glEnd();
}

// ---------------------------------------------------------------------------
// Side parts — modelled for the RIGHT side (x > 0).
// drawRightSide() is called twice: once as-is, once under glScalef(-1,1,1).
// ---------------------------------------------------------------------------

static void drawWindow()
{
    // Side windows on the cabin's right wall (x = 0.79), normal +X.
    // Winding: CCW from outside (+X direction) → v1×v2 gives +X normal.
    setMaterial(0.95f, 0.98f, 1.00f, 120.0f);
    glColor3f(0.60f, 0.85f, 1.00f);
    glNormal3f(1, 0, 0);
    glBegin(GL_QUADS);

    // Main side window (spans front and rear doors)
    glVertex3f(0.79f, 0.22f, -0.85f);
    glVertex3f(0.79f, 0.78f, -0.85f);
    glVertex3f(0.79f, 0.78f,  1.05f);
    glVertex3f(0.79f, 0.22f,  1.05f);

    // Rear quarter glass (small pane behind the rear door)
    glVertex3f(0.79f, 0.22f,  1.10f);
    glVertex3f(0.79f, 0.60f,  1.10f);
    glVertex3f(0.79f, 0.60f,  1.22f);
    glVertex3f(0.79f, 0.22f,  1.22f);

    glEnd();
}

static void drawDoors()
{
    // Wireframe lines have no meaningful face normal, so disable lighting to
    // ensure they always show their flat colour regardless of light direction.
    glDisable(GL_LIGHTING);
    glColor3f(0.85f, 0.85f, 0.85f);

    // Front door
    glBegin(GL_LINE_LOOP);
    glVertex3f(0.91f, -0.44f, -1.80f);
    glVertex3f(0.91f,  0.10f, -1.80f);
    glVertex3f(0.91f,  0.10f, -0.15f);
    glVertex3f(0.91f, -0.44f, -0.15f);
    glEnd();

    // Rear door
    glBegin(GL_LINE_LOOP);
    glVertex3f(0.91f, -0.44f, -0.05f);
    glVertex3f(0.91f,  0.10f, -0.05f);
    glVertex3f(0.91f,  0.10f,  1.70f);
    glVertex3f(0.91f, -0.44f,  1.70f);
    glEnd();

    glEnable(GL_LIGHTING);
}

// One tire: cylinder + outer hubcap disk.
// Caller translates to the axle centre before calling.
// The cylinder is oriented along +X (away from car centre).
static void drawTire()
{
    ensureQuad();
    glPushMatrix();
    // glRotatef(90, Y) maps local +Z onto world +X, so gluCylinder
    // draws outward from the axle position.
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    // Rubber tread — matte (rubber absorbs light, no specular)
    setMaterial(0.0f, 0.0f, 0.0f, 0.0f);
    glColor3f(0.10f, 0.10f, 0.10f);
    gluCylinder(quad, 0.38f, 0.38f, 0.25f, 24, 1);

    // Outer hubcap — shiny metal (demonstrates specular on a curved surface)
    glTranslatef(0.0f, 0.0f, 0.25f);
    setMaterial(0.85f, 0.85f, 0.90f, 80.0f);
    glColor3f(0.55f, 0.55f, 0.60f);
    gluDisk(quad, 0.06f, 0.38f, 24, 1);

    glPopMatrix();
}

// One headlight: a short yellow cylinder with a bright disk face,
// positioned flush with the front bumper and protruding slightly.
// Caller translates to the light centre before calling.
static void drawHeadlight()
{
    ensureQuad();
    glPushMatrix();
    // Rotate 180° around Y so local +Z points toward world -Z (front of car).
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

    // Glass face — emissive so it glows regardless of scene lighting
    setMaterial(1.0f, 1.0f, 0.90f, 128.0f, 0.35f, 0.35f, 0.05f);
    glColor3f(1.0f, 1.0f, 0.75f);
    gluDisk(quad, 0.0f, 0.12f, 16, 1);

    // Bezel — shiny yellow chrome, no emission
    setMaterial(0.80f, 0.80f, 0.20f, 60.0f);
    glColor3f(0.75f, 0.75f, 0.20f);
    gluCylinder(quad, 0.12f, 0.12f, 0.10f, 16, 1);

    // Reset emission so it doesn't bleed into the next part drawn
    setMaterial(0.0f, 0.0f, 0.0f, 0.0f);

    glPopMatrix();
}

static void drawRightSide()
{
    drawWindow();
    drawDoors();

    // Front tire  (z = -1.4)
    glPushMatrix();
    glTranslatef(0.90f, -0.50f, -1.40f);
    drawTire();
    glPopMatrix();

    // Rear tire   (z = +1.4)
    glPushMatrix();
    glTranslatef(0.90f, -0.50f, 1.40f);
    drawTire();
    glPopMatrix();

    // Front headlight — slightly proud of the bumper face to avoid z-fighting
    glPushMatrix();
    glTranslatef(0.52f, -0.10f, -2.02f);
    drawHeadlight();
    glPopMatrix();
}

// ---------------------------------------------------------------------------
// Public: light indicator sphere
// ---------------------------------------------------------------------------

// Draws a small sphere that emits the given colour and is not affected by
// any scene light (diffuse = specular = 0, emission = rgb).
// Must be called before trackballApply() so the sphere stays world-fixed.
void drawLightSphere(float x, float y, float z, float r, float g, float b)
{
    ensureQuad();

    // Disable lighting entirely — the sphere shows its pure glColor3f value,
    // unaffected by scene lights (which is what the assignment requires).
    // This also avoids fighting with GL_COLOR_MATERIAL overriding glMaterialfv.
    glDisable(GL_LIGHTING);
    glColor3f(r, g, b);

    glPushMatrix();
    glTranslatef(x, y, z);
    gluSphere(quad, 0.15f, 12, 12);
    glPopMatrix();

    glEnable(GL_LIGHTING);
}

// ---------------------------------------------------------------------------
// Public: car
// ---------------------------------------------------------------------------
void drawCar()
{
    ensureQuad();

    // Symmetric body parts (drawn once)
    drawChassis();
    drawCabin();
    drawWindshields();

    // Right side
    drawRightSide();

    // Left side: mirror across the X=0 plane.
    // glScalef(-1,1,1) reverses vertex winding, so swap the front-face
    // convention for the duration of this draw.
    glPushMatrix();
    glScalef(-1.0f, 1.0f, 1.0f);
    glFrontFace(GL_CW);
    drawRightSide();
    glFrontFace(GL_CCW);
    glPopMatrix();
}
