#ifndef RENDER_H
#define RENDER_H

#include "menu.h"
#include "dstructs/arena.h"
#include "cellular/cells.h"

// use this instead of WindowShouldClose(), as it checks the escape key as well
extern bool shouldClose;

// forward declarations to prevent cyclic includes
typedef struct Render2D Render2D;
typedef struct Render3D Render3D;

/**
 * The main render window that contains the render loop, where the the main
 * menu gets drawn. Also used for selecting the other render modes via the menu.
 */
typedef struct Render {
  Arena mode2DArena;
  Arena mode3DArena;
  Arena frame2DArena;
  Arena frame3DArena;

  MainMenu *menu;

  Render2D *render2d;
  Render3D *render3d;

  // TODO: A state machine to handle the mode changes would be better, but this
  // keeps it simple for now
  RenderMode prevMode;
  RenderMode currentMode;

  // NOTE: might be worth replacing this to a ring buffer
  int charPressed;
  float deltaTime;
  int fpsCap;

  bool isWireframeMode;
  bool isPaused;
  bool isDebugOn;
  bool isModeFirstFrame;
  bool isMouseRestricted;
} Render;

void Render_RenderWindow(Render *render);
void Render_BeginDrawing();
void Render_LogGlError();

#endif