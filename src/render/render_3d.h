#ifndef RENDER_3D
#define RENDER_3D

#include "render.h"

typedef struct Render3D {
  // higher values mean slower updates
  float render3DSpeed;
  Cells3D firstC3d;
  Cells3D secondC3d;
} Render2D;

#endif