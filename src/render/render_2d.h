#ifndef RENDER_2D
#define RENDER_2D

#include "cellular/cells.h"
#include "menu_2d.h"

// forward declaration of Render, since Render is using Render2D and Render2D is
// using Render this is only possible, because a pointer is used
struct Render;
typedef struct Render Render;

typedef struct Render2D {
  Menu2D menu2d;

  Camera2D camera;

  Cells2D firstC2d;
  Cells2D secondC2d;

  // higher values mean slower updates
  float render2DSpeed;
  int currentGeneration;
  int gridDensity;
} Render2D;

Render2D Render2D_Init(Render *render);
void Render2D_RenderMode(Render *render);

void Render2D_IncrementGridDensity(Render *render);
void Render2D_DecrementGridDensity(Render *render);
void Render2D_ResetCells(Render *render);
void Render2D_RandomizeZeroGen(Render *render);

#endif