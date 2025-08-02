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

  while (!WindowShouldClose()) {
    render->charPressed = GetCharPressed();
    Menu_Update(render->menu);

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
      render->isModeFirstFrame = true;

      if (render->mode2DArena != NULL) {
        Arena_FreeZeroed(render->mode2DArena);
        render->mode2DArena = NULL;
      }

      if (render->frame2DArena != NULL) {
        Arena_FreeZeroed(render->frame2DArena);
        render->frame2DArena = NULL;
      }

      if (render->mode3DArena != NULL) {
        Arena_FreeZeroed(render->mode3DArena);
        render->mode3DArena = NULL;
      }

      if (render->frame3DArena != NULL) {
        Arena_FreeZeroed(render->frame3DArena);
        render->frame3DArena = NULL;
      }

      menu.prevMode = menu.currentMode;
    }

    // update variables here
    ClearBackground(DARKBLUE);

    BeginDrawing();

    // select the current mode
    // TODO: implement the switching on modes in the main menu
    switch (menu.currentMode) {
    case RENDER_MODE_INIT:
      menu.isVisible = true;
      break;
    case RENDER_MODE_2D:
      if (render->isModeFirstFrame) {
        Arena mode2DArena =
            Arena_Init("modeArena", &mode2DArenaStorage, MODE_2D_STORAGE_SIZE);
        Arena frame2DArena = Arena_Init("frame2DArena", &frame2DArenaStorage,
                                        FRAME_2D_STORAGE_SIZE);
        render->mode2DArena = &mode2DArena;
        render->frame2DArena = &frame2DArena;

        Render2D render2d = Render2D_Init();
        render->render2d = &render2d;
      }
      // can always call this, since at the first frame render2d should be
      // initialized
      Render2D_RenderMode(render);

      // only set the isModeFirstFrame to false, after the Render2D was called
      // once
      if (render->isModeFirstFrame) {
        render->isModeFirstFrame = false;
      }
      break;
    case RENDER_MODE_3D:
      if (render->isModeFirstFrame) {
        Arena mode3DArena =
            Arena_Init("modeArena", &mode3DArenaStorage, MODE_3D_STORAGE_SIZE);

        Arena frame3DArena = Arena_Init("frame3DArena", &frame3DArenaStorage,
                                        FRAME_3D_STORAGE_SIZE);
        render->mode3DArena = &mode3DArena;
        render->frame3DArena = &frame3DArena;

        // TODO: implement this
        /*
        Render3D render3D = Render3D_Init();
        */
      }
      // Render3D_RenderMode(render);
      if (render->isModeFirstFrame) {
        render->isModeFirstFrame = false;
      }
      break;
    default:
      break;
    }

    if (menu.isVisible) {
      Menu_Draw(&menu);
    }

    if (IsKeyPressed(KEY_TAB)) {
      render->isDebugOn = !render->isDebugOn;
    }

    if (render->isDebugOn) {
      Menu_DrawDebug(&menu);
    }

    EndDrawing();
  }

  // teardown the objects after the window has been closed
}
