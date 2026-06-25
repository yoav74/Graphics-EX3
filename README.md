# Exercise 3 – OpenGL Car Viewer

An OpenGL application that renders a lit 3D car model with interactive viewing controls.

## Requirements

- WSL (Windows Subsystem for Linux) with Ubuntu
- g++ compiler
- OpenGL, GLU, and freeglut libraries

```bash
sudo apt install build-essential libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev
```

## Build & Run

```bash
make
./ex3
```

```bash
make clean
```

## Controls

| Input | Action |
|-------|--------|
| Left mouse drag | Rotate model (virtual trackball) |
| Mouse wheel | Zoom in / out |
| `p` | Toggle wireframe / filled polygon mode |
| `a` | Toggle XYZ axis display |
| `l` | Toggle light source indicator spheres |
| ESC | Quit |

## Implementation

### Viewing
The camera uses a perspective projection (FOV 45°). Rotation is handled by a virtual trackball: mouse positions are projected onto a sphere, and the rotation between consecutive sphere vectors is accumulated into a 4×4 matrix applied each frame. Zoom moves the camera along the Z axis (perspective zoom, not orthographic scale).

### Car model
The car is built from two primitives as required — polygons and GLU quadrics — organised as a tree using the OpenGL matrix stack:

| Part | Primitive | Notes |
|------|-----------|-------|
| Chassis (lower body) | GL_QUADS box | Filled, per-face normals |
| Cabin (upper body) | GL_QUADS box | Filled, per-face normals |
| Front windshield | GL_QUADS | Symmetric, drawn once |
| Rear window | GL_QUADS | Symmetric, drawn once |
| Side windows + quarter glass | GL_QUADS | One side, mirrored |
| Door outlines (×2 per side) | GL_LINE_LOOP | Wireframe, lighting disabled |
| Tires (×4) | gluCylinder + gluDisk | Cylinder bounded by disk |
| Headlights (×2) | gluDisk + gluCylinder | Disk face + bezel cylinder |

The left half of the car mirrors the right using `glScalef(-1, 1, 1)`. Because negative scaling reverses vertex winding, `glFrontFace(GL_CW)` is set for the mirrored draw and restored to `GL_CCW` after, keeping back-face culling correct throughout.

### Lighting
Two positional light sources (w = 1):

| Light | World position | Character |
|-------|---------------|-----------|
| Key light | (1, 1.5, 2) | Warm white-yellow |
| Fill light | (-1, 1.5, -2) | Cool blue |

Lights are positioned before the trackball rotation is applied, so they remain world-fixed while the model rotates. `GL_NORMALIZE` corrects normals distorted by the mirror scale. `GL_COLOR_MATERIAL` maps `glColor3f` to ambient + diffuse, while `setMaterial()` in `drawing.cpp` handles specular, shininess, and emission per part:

| Part | Shininess | Emission |
|------|-----------|---------|
| Car paint | 64 | — |
| Glass (all windows) | 120 | — |
| Tyre rubber | 0 (matte) | — |
| Hubcaps | 80 | — |
| Headlight glass | 128 | soft glow |

Small indicator spheres are drawn at each light's world position with lighting disabled (pure emittance, unaffected by scene lights). Press `l` to show/hide them; the lights themselves remain active.

## Project Structure

```
Graphics-EX3/
├── main.cpp        # GLUT setup, event callbacks, lighting configuration
├── trackball.cpp   # Virtual trackball rotation math (Appendix A)
├── trackball.h
├── drawing.cpp     # Car geometry, materials, axes, light spheres
├── drawing.h
├── MayCode.cpp     # Prototype / reference (not compiled)
└── Makefile
```
