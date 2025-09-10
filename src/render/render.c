#include "render.h"
#include "const.h"
#include "dstructs/arena.h"
#include <assert.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "const.h"
#include "menu.h"
#include <stdio.h>
#include <stdbool.h>
#include "render_2d.h"
#include "render_3d.h"

#ifdef DEBUG_MODE
#include "tracy/TracyC.h"
#endif

bool shouldClose = false;

void Render_RenderWindow(Render *render) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CA Renderer");
  SetWindowFocused();

  // must be called after InitWindow
  MainMenu menu = Menu_Init();

  render->menu = &menu;

  SetTargetFPS(render->fpsCap);

  while (!shouldClose) {
#ifdef DEBUG_MODE
    TracyCFrameMarkNamed("WindowLoop");
#endif

    render->charPressed = GetCharPressed();
#ifdef DEBUG_MODE
    TracyCZoneN(ctx, "Menu_Update", true);
#endif

    Menu_Update(render);

#ifdef DEBUG_MODE
    TracyCZoneEnd(ctx);
#endif

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

    if (render->currentMode != render->prevMode) {
#ifdef DEBUG_MODE
      TracyCZoneN(ctx, "RenderModeChange", true);
#endif
      // whenever we switch modes, we free everything
      render->isModeFirstFrame = true;

      if (&render->mode2DArena != NULL && render->mode2DArena.used > 0) {
        Arena_FreeZeroed(&render->mode2DArena);
      }

      if (&render->frame2DArena != NULL && render->frame2DArena.used > 0) {
        Arena_FreeZeroed(&render->frame2DArena);
      }

      if (&render->mode3DArena != NULL && render->mode3DArena.used > 0) {
        printf("currentMode: %s, prevMode: %s\n", render->currentMode,
               render->prevMode);
        Arena_FreeZeroed(&render->mode3DArena);
      }

      if (&render->frame3DArena != NULL && render->frame3DArena.used > 0) {
        Arena_FreeZeroed(&render->frame3DArena);
      }

      render->prevMode = render->currentMode;
#ifdef DEBUG_MODE
      TracyCZoneEnd(ctx);
#endif
    }

    // select the current mode
    // TODO: implement the switching on modes in the main menu
    switch (render->currentMode) {
    case RENDER_MODE_INIT:
      Render_BeginDrawing();
      menu.isVisible = true;
      break;
    case RENDER_MODE_2D:
      if (render->isModeFirstFrame) {
        Render2D render2d = Render2D_Init(render);
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
        Render3D render3d = Render3D_Init(render);
        render->render3d = &render3d;
      }

      Render3D_RenderMode(render);

      if (render->isModeFirstFrame) {
        render->isModeFirstFrame = false;
      }
      break;
    default:
      break;
    }

    if (menu.isVisible) {
      Menu_Draw(render);
    }

    if (IsKeyPressed(KEY_TAB)) {
      render->isDebugOn = !render->isDebugOn;
    }

    if (render->isDebugOn) {
      Menu_DrawDebug(render);
    }

    EndDrawing();

    if (render->isMouseRestricted) {
      // NOTE: restricting the mouse to a specific square, so the user can turn
      // around in 3D, but cannot go over the limit, which prevents the mouse
      // going out of the screen
      Vector2 mousePos = GetMousePosition();
      if (mousePos.x >= (SCREEN_WIDTH / 2) + 128 ||
          (mousePos.x <= (SCREEN_WIDTH / 2) - 128 ||
           (mousePos.y >= (SCREEN_HEIGHT / 2) + 128) ||
           (mousePos.y <= (SCREEN_HEIGHT / 2) - 128))) {
        SetMousePosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
      }
    }
  }

  // teardown the objects after the window has been closed
}

void Render_BeginDrawing() {
  BeginDrawing();

  ClearBackground(DARKBLUE);
}

void Render_LogGlError() {
  // imported via raylib
  int glError = glGetError();
  if (glError > 0) {
    printf("\nunable to render, error code: %d", glError);
  }
}