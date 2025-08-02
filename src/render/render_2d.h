#ifndef RENDER_2D
#define RENDER_2D

#include "cellular/cells.h"

// forward declaration of Render, since Render is using Render2D and Render2D is
// using Render this is only possible, because a pointer is used
struct Render;
typedef struct Render Render;

typedef struct Render2D {
  // higher values mean slower updates
  float render2DSpeed;
  Cells2D firstC2d;
  Cells2D secondC2d;
} Render2D;

Render2D Render2D_Init();
void Render2D_RenderMode(Render *render);

#endif