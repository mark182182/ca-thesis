#ifndef RENDER_3D
#define RENDER_3D

#include "render.h"

typedef struct Render3DThreadCubes {
  Cells3D *actualCd;
  Camera *camera;
  Mesh *cube;
  Matrix *transforms;

  HANDLE startEvent;
  HANDLE doneEvent;
  int startIdx;
  int endIdx;
} Render3DThreadCubes;

typedef struct Render3D {
  // higher values mean slower updates
  Camera3D camera;
  Mesh cube;
  Material matInstances;
  Material matSelection;
  Matrix *transforms;

  Render3DThreadCubes *allThreadCubes;
  HANDLE *transformMatrixThreads;

  Cells3D firstC3d;
  Cells3D secondC3d;
  float render3DSpeed;

} Render3D;

Render3D Render3D_Init(Render *render);
void Render3D_RenderMode(Render *render);

static void __Render3D_ResolveTransformMatrix(Render3DThreadCubes *threadCubes);

#endif