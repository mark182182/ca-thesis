#include "render.h"
#include "const.h"
#include "dstructs/arena.h"
#include <assert.h>
#include <raylib.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "const.h"
#include "menu.h"
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include "render_2d.h"

void Render_RenderWindow(Render *render) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CA Renderer");
  // must be called after InitWindow
  Menu menu = Menu_Init();

  render->menu = &menu;

  SetTargetFPS(render->fpsCap);

  bool isFirstFrame = true;
  while (!WindowShouldClose()) {
    render->charPressed = GetCharPressed();

    if (render->fpsCap == 60 && render->charPressed == 'c') {
      render->fpsCap = 0;
      SetTargetFPS(render->fpsCap);
    } else if (render->fpsCap == 0 && render->charPressed == 'c') {
      render->fpsCap = 60;
      SetTargetFPS(render->fpsCap);
    }

    if (!render->isWireframeMode && render->charPressed == 't') {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      render->isWireframeMode = true;
    } else if (render->isWireframeMode && render->charPressed == 't') {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      render->isWireframeMode = false;
    }

    if (menu.currentMode != menu.prevMode) {
      // whenever we switch modes, we free everything
      if (render->mode2DArena != NULL) {
        render->mode2DArena = NULL;
        Arena_FreeZeroed(render->mode2DArena);
      }

      if (render->frame2DArena != NULL) {
        render->frame2DArena = NULL;
        Arena_FreeZeroed(render->frame2DArena);
      }

      if (render->mode3DArena != NULL) {
        render->mode3DArena = NULL;
        Arena_FreeZeroed(render->mode3DArena);
      }

      if (render->frame3DArena != NULL) {
        render->frame3DArena = NULL;
        Arena_FreeZeroed(render->frame3DArena);
      }
    }

    // update variables here
    ClearBackground(DARKBLUE);

    BeginDrawing();

    // select the current mode
    // TODO: implement the switching on modes in the main menu
    switch (menu.currentMode) {
    case RENDER_MODE_INIT:
      break;
    case RENDER_MODE_2D:
      if (isFirstFrame) {
        Arena mode2DArena =
            Arena_Init("modeArena", &mode2DArenaStorage, MODE_2D_STORAGE_SIZE);
        Arena frame2DArena = Arena_Init("frame2DArena", &frame2DArenaStorage,
                                        FRAME_2D_STORAGE_SIZE);
        render->mode2DArena = &mode2DArena;
        render->frame2DArena = &frame2DArena;

        Render2D render2d = Render2D_Init();
        Render2D_RenderMode(&render2d, render);
      }
      break;
    case RENDER_MODE_3D:
      if (isFirstFrame) {
        Arena mode3DArena =
            Arena_Init("modeArena", &mode3DArenaStorage, MODE_3D_STORAGE_SIZE);

        Arena frame3DArena = Arena_Init("frame3DArena", &frame3DArenaStorage,
                                        FRAME_3D_STORAGE_SIZE);
        render->mode3DArena = &mode3DArena;
        render->frame3DArena = &frame3DArena;

        // TODO: implement this
        /*
        Render3D render3D = Render3D_Init();
        Render3D_RenderMode(render);
        */
      }
      break;
    default:
      break;
    }

    if (isFirstFrame) {
      isFirstFrame = false;
    }

    Menu_Draw(&menu);

    if (IsKeyPressed(KEY_TAB)) {
      render->isDebugOn = !render->isDebugOn;
    }

    if (render->isDebugOn) {
      Menu_DrawDebug(&menu);
    }

    EndDrawing();
  }

  // teardown the objects after the window has been closed
  Arena_Free(render->mode2DArena);
}
