#include "trackball.h"
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <math.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Internal state
// ---------------------------------------------------------------------------

static bool mouseDown = false;
static float lastVec[3];    // sphere vector recorded at last mouse event
static float rotMatrix[16]; // cumulative 4x4 column-major rotation matrix

// ---------------------------------------------------------------------------
// Math helpers
// ---------------------------------------------------------------------------

// Normalizes a 3D vector in place.
static void normalize3(float *v)
{
    float len = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (len > 0.0f)
    {
        v[0] /= len;
        v[1] /= len;
        v[2] /= len;
    }
}

// Maps a 2D canvas pixel (px, py) onto the surface of the trackball sphere.
// The sphere is centered at the origin and has radius sqrt(2), so it passes
// through the corners of the [-1,1]^2 view plane.
// Formula from Appendix A:
//   x = 2*px/width  - 1
//   y = 1 - 2*py/height   (screen Y is flipped relative to OpenGL Y)
//   z = sqrt(2 - x^2 - y^2)  if inside sphere, else project onto equator
static void projectOnSphere(int px, int py, float *v)
{
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    v[0] = 2.0f * px / w - 1.0f; //Current mouse position in normalized device coordinates
    v[1] = 1.0f - 2.0f * py / h;

    float d = v[0] * v[0] + v[1] * v[1]; //Distance squared from the center of the sphere
    if (d <= 2.0f)
        v[2] = sqrtf(2.0f - d);
    else
    {
        // Outside the sphere: clamp to the equator ring
        float len = sqrtf(d);
        v[0] /= len;
        v[1] /= len;
        v[2] = 0.0f;
    }
}

// Builds a 4x4 column-major rotation matrix that rotates unit vector a onto b.
// Uses Rodrigues' formula: R = I + [v]x + [v]x^2 * (1-c)/s^2
// where v = a x b, c = a . b, s = |v|.
static void rotationBetween(const float *a, const float *b, float *mat)
{
    //Cross Product a X b
    float v[3] = {
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]};

    //Dot Product a . b
    float c = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
    //|v| = sqrt(vx^2 + vy^2 + vz^2)
    float s = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

    memset(mat, 0, 16 * sizeof(float));
    mat[0] = 1; mat[5] = 1; mat[10] = 1; mat[15] = 1;

    if (s < 0.001f)
    {
        if (c < 0.0f)
        {
            // Anti-parallel: 180° rotation around any perpendicular axis
            float perp[3] = {1, 0, 0};
            if (fabsf(a[0]) > 0.9f) { perp[0] = 0; perp[1] = 1; }
            float ax[3] = {
                a[1] * perp[2] - a[2] * perp[1],
                a[2] * perp[0] - a[0] * perp[2],
                a[0] * perp[1] - a[1] * perp[0]};
            normalize3(ax);
            float x = ax[0], y = ax[1], z = ax[2];
            mat[0] = 2*x*x-1; mat[4] = 2*x*y;    mat[8]  = 2*x*z;
            mat[1] = 2*x*y;   mat[5] = 2*y*y-1;  mat[9]  = 2*y*z;
            mat[2] = 2*x*z;   mat[6] = 2*y*z;    mat[10] = 2*z*z-1;
        }
        return;
    }

    float factor = (1.0f - c) / (s * s);
    float vx = v[0], vy = v[1], vz = v[2];

    mat[0] = 1 + factor * (-vz*vz - vy*vy);  mat[4] = factor * vx*vy - vz;      mat[8]  = factor * vx*vz + vy;
    mat[1] =     factor *  vx*vy + vz;        mat[5] = 1 + factor * (-vz*vz - vx*vx);  mat[9]  = factor * vy*vz - vx;
    mat[2] =     factor *  vx*vz - vy;        mat[6] = factor * vy*vz + vx;      mat[10] = 1 + factor * (-vy*vy - vx*vx);

    mat[15] = 1;
}

// Multiplies two 4x4 column-major matrices: out = a * b
static void mulMatrix(const float *a, const float *b, float *out)
{
    float tmp[16] = {};
    for (int col = 0; col < 4; col++)
        for (int row = 0; row < 4; row++)
            for (int k = 0; k < 4; k++)
                tmp[col * 4 + row] += a[k * 4 + row] * b[col * 4 + k];
    memcpy(out, tmp, 16 * sizeof(float));
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void trackballInit()
{
    // Set cumulative rotation to identity
    memset(rotMatrix, 0, sizeof(rotMatrix));
    rotMatrix[0] = 1;
    rotMatrix[5] = 1;
    rotMatrix[10] = 1;
    rotMatrix[15] = 1;
}

void trackballMouseDown(int x, int y)
{
    mouseDown = true;
    projectOnSphere(x, y, lastVec);
    normalize3(lastVec);
}

void trackballMouseMove(int x, int y)
{
    if (!mouseDown)
        return;

    float currVec[3];
    projectOnSphere(x, y, currVec);
    normalize3(currVec);

    // Compute the rotation that moves lastVec to currVec
    float newRot[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    rotationBetween(lastVec, currVec, newRot);

    // Accumulate: R_new * R_stored  (apply newest rotation on top)
    float combined[16];
    mulMatrix(newRot, rotMatrix, combined);
    memcpy(rotMatrix, combined, sizeof(rotMatrix));

    memcpy(lastVec, currVec, sizeof(lastVec));
}

void trackballMouseUp()
{
    mouseDown = false;
}

void trackballApply()
{
    glMultMatrixf(rotMatrix);
}
