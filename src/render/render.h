#ifndef RENDER_H
#define RENDER_H

#include "menu.h"
#include "dstructs/arena.h"
#include "cellular/cells.h"

// use this instead of WindowShouldClose(), as it checks the escape key as well
extern bool shouldClose;

// forward declarations to prevent cyclic includes
struct Render2D;
typedef struct Render2D Render2D;

struct Render3D;
typedef struct Render3D Render3D;

/**
 * The main render window that contains the render loop, where the the main
 * menu gets drawn. Also used for selecting the other render modes via the menu.
 */
typedef struct Render {
  Arena *mode2DArena;
  Arena *mode3DArena;
  Arena *frame2DArena;
  Arena *frame3DArena;

  Menu *menu;

  Render2D *render2d;
  Render3D *render3d;

  // NOTE: might be worth replacing this to a ring buffer
  int charPressed;
  float deltaTime;
  int fpsCap;

  bool isWireframeMode;
  bool isPaused;
  bool isDebugOn;
  bool isModeFirstFrame;
} Render;

void Render_RenderWindow(Render *render);
void Render_BeginDrawing();
void Render_LogGlError();

#endif