#pragma once

// Initializes the trackball: sets the cumulative rotation to the identity matrix.
void trackballInit();

// Records the sphere vector at the position where the mouse was pressed.
// Call this on mouse button down.
void trackballMouseDown(int x, int y);

// Updates the cumulative rotation based on the new mouse position.
// Call this on mouse drag (motion with button held).
void trackballMouseMove(int x, int y);

void trackballMouseUp();

// Applies the cumulative rotation to the current OpenGL ModelView matrix.
// Call this inside your display function before drawing the scene.
void trackballApply();
