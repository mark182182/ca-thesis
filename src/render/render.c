#include "render.h"
#include "const.h"
#include "dstructs/arena.h"
#include <assert.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "const.h"
#include "menu.h"
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include "render_2d.h"
#include "render_3d.h"

bool shouldClose = false;

void Render_RenderWindow(Render *render) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CA Renderer");
  SetWindowFocused();

  // must be called after InitWindow
  Menu menu = Menu_Init();

  render->menu = &menu;

  SetTargetFPS(render->fpsCap);

  while (!shouldClose) {
    render->charPressed = GetCharPressed();
    Menu_Update(render);

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

    // select the current mode
    // TODO: implement the switching on modes in the main menu
    switch (menu.currentMode) {
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
      // NOTE: restricting the mouse to a specific square, so the user can turn around in 3D, but cannot go over the limit, which prevents the mouse going out of the screen
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