#pragma once

void drawCar();

// Draws a small emissive sphere at (x,y,z) with the given RGB colour.
// Diffuse/specular are zeroed so the sphere is not affected by scene lights.
// Call before trackballApply() so it stays world-fixed.
void drawLightSphere(float x, float y, float z, float r, float g, float b);
