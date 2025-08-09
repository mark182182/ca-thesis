#ifndef RENDER_2D
#define RENDER_2D

#include "cellular/cells.h"

// forward declaration of Render, since Render is using Render2D and Render2D is
// using Render this is only possible, because a pointer is used
struct Render;
typedef struct Render Render;

typedef struct Render2D {
  Camera2D camera;

  Cells2D firstC2d;
  Cells2D secondC2d;

  // higher values mean slower updates
  float render2DSpeed;
} Render2D;

Render2D Render2D_Init(Render *render);
void Render2D_RenderMode(Render *render);

#endif