#ifndef RENDER_3D
#define RENDER_3D

#include "render.h"
#include "menu_3d.h"

typedef struct Evolve3DThreadCells Evolve3DThreadCells;

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
  Menu3D menu3d;

  Camera3D camera;
  bool freezeCamera;
  CameraMode cameraMode;

  Mesh cube;
  Material matInstances;
  Matrix *transforms;

  Evolve3DThreadCells *allThreadCells;
  HANDLE *nextGenThreads;

  Render3DThreadCubes *allThreadCubes;
  HANDLE *transformMatrixThreads;

  Cells3D firstC3d;
  Cells3D secondC3d;

  float render3dSpeed;
  int currentGeneration;
  bool isEditing;
  int randGridDensity;
} Render3D;

Render3D Render3D_Init(Render *render);
void Render3D_RenderMode(Render *render);

void Render3D_EditCells(Render *render);
void Render3D_DecrementGridDensity(Render *render);
void Render3D_IncrementGridDensity(Render *render);
void Render3D_ResetCells(Render *render);
void Render3D_RandomizeZeroGen(Render *render);

static void __Render3D_ResolveTransformMatrix(Render3DThreadCubes *threadCubes);
static Matrix __Render3D_TranslatePointsIntoMatrix(float x, float y, float z);

static bool __Render3D_CanEditCell(Render *render, bool isMouseOnMenu,
                                   bool isMouseOnCube);

#endif