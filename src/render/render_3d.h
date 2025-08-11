#ifndef RENDER_3D
#define RENDER_3D

#include "render.h"

typedef struct Render3D {
  // higher values mean slower updates
  Camera3D camera;
  Mesh cube;
  Material matInstances;
  Matrix *transforms;

  Cells3D firstC3d;
  Cells3D secondC3d;
  float render3DSpeed;

} Render3D;

Render3D Render3D_Init(Render *render);
void Render3D_RenderMode(Render *render);

#endif