#include "render_2d.h"
#include "render.h"
#include "cellular/evolve.h"
#include <assert.h>
#include "raylib_shim.h"
#include "cellular/cells.h"
#include "dstructs/arena.h"
#include <stdio.h>
#include <stdbool.h>
#include "const.h"

Render2D Render2D_Init(Render *render) {
  Arena mode2DArena =
      Arena_Init("modeArena", &mode2DArenaStorage, MODE_2D_STORAGE_SIZE);
  Arena frame2DArena =
      Arena_Init("frame2DArena", &frame2DArenaStorage, FRAME_2D_STORAGE_SIZE);
  render->mode2DArena = mode2DArena;
  render->frame2DArena = frame2DArena;

  Camera2D camera = {0};
  camera.zoom = 1.0f;

  Menu2D menu2d = Menu2D_Init(render);

  Render2D render2d = {.menu2d = menu2d,
                       .camera = camera,
                       .firstC2d = {0},
                       .secondC2d = {0},
                       .render2dSpeed = 0.4f,
                       .currentGeneration = 0,
                       .randGridDensity = CELL_INITIAL_GRID_DENSITY};
  return render2d;
}

void Render2D_RenderMode(Render *render) {
  Render2D *render2d = render->render2d;

  // NOTE: this could be part of Render2D_Init
  if (render->isModeFirstFrame) {
    printf("Rendering 2D mode\n");
    Cells2D_InitArraysBasedOnCellSize(&render->mode2DArena,
                                      &render2d->firstC2d);
    Cells2D_InitArraysBasedOnCellSize(&render->mode2DArena,
                                      &render2d->secondC2d);

    // TODO: Randomization should only be set, if the user clicks on the button
    // or similar
    Evolve2D_InitializeCells(&render2d->firstC2d, false,
                             render2d->randGridDensity);
    Evolve2D_InitializeCells(&render2d->secondC2d, false,
                             render2d->randGridDensity);
  }

  // would implement batched rendering, draw calls here

  if (render->isPaused == false &&
      render->deltaTime >= render2d->render2dSpeed) {
    if (render2d->currentGeneration != 0 &&
        render2d->currentGeneration % 2 == 0) {
      EvolveGOL2D_NextGeneration(&render2d->firstC2d, &render2d->secondC2d);
    } else {
      EvolveGOL2D_NextGeneration(&render2d->secondC2d, &render2d->firstC2d);
    }
    render2d->currentGeneration++;
    render->deltaTime = 0;
  }
  render->deltaTime += GetFrameTime();

  Cells2D *actualCd = render2d->currentGeneration % 2 == 0
                          ? &render2d->firstC2d
                          : &render2d->secondC2d;

  // update variables here

  // TODO: decide on at what time should we start drawing
  Render_BeginDrawing();
  BeginMode2D(render2d->camera);

  // TODO: This should be drawn in a single call
  int aliveCells = 0;
  for (int i = 0; i < CELL_COUNT; i++) {
    Color color = *actualCd->colors[i];
    // TODO: revise this, just an example and it's performance intensive
    Rectangle rect = {.height = CELL_HEIGHT_RATIO,
                      .width = CELL_WIDTH_RATIO,
                      .x = actualCd->positionsX[i],
                      .y = actualCd->positionsY[i]};

    if (render2d->isEditing) {
      Vector2 mousePos = GetMousePosition();

      bool isMouseOnMenu =
          CheckCollisionPointRec(mousePos, render2d->menu2d.containerRect);
      bool isMouseOnCell = CheckCollisionPointRec(mousePos, rect);

      if (__Render2D_CanEditCell(render, isMouseOnMenu, isMouseOnCell)) {
        Vector2 position = {.x = 100, .y = 200}; // relative to mouse
        char posText[32];
        snprintf(posText, sizeof(posText), "cellX: %.2f, cellY: %.2f", rect.x,
                 rect.y);

        DrawTextEx(render->menu->selectedFont, posText, mousePos, SUB_FONT_SIZE,
                   0, DEFAULT_TEXT_COLOR);
        color = YELLOW;

        // TODO: Make sure the user can set the current cell's alive state
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
          actualCd->is_alive[i] = !actualCd->is_alive[i];
        }
      }
    }

    if (actualCd->is_alive[i]) {
      aliveCells++;
      DrawRectangle(actualCd->positionsX[i], actualCd->positionsY[i],
                    CELL_HEIGHT_RATIO, CELL_WIDTH_RATIO, color);
    } else if (color.r == YELLOW.r && color.g == YELLOW.g &&
               color.b == YELLOW.b && color.a == YELLOW.a) {
      DrawRectangle(actualCd->positionsX[i], actualCd->positionsY[i],
                    CELL_HEIGHT_RATIO, CELL_WIDTH_RATIO, color);
    }
  }
  actualCd->aliveCells = aliveCells;

  EndMode2D();

  Menu2D_Draw(render);

  // free objects after each frame
  Arena_Free(&render->frame2DArena);
}

void Render2D_EditCells(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    render->render2d->isEditing = !render->render2d->isEditing;
  }
}

void Render2D_IncrementGridDensity(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
      // TODO: this max value should be stored along with the constants
      render->render2d->randGridDensity < CELL_MAX_GRID_DENSITY) {
    render->render2d->randGridDensity++;
  }
}

void Render2D_DecrementGridDensity(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
      // TODO: this min value should be stored along with the constants
      render->render2d->randGridDensity > CELL_MIN_GRID_DENSITY) {
    render->render2d->randGridDensity--;
  }
}

void Render2D_ResetCells(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Evolve2D_InitializeCells(&render->render2d->firstC2d, false,
                             render->render2d->randGridDensity);
    Evolve2D_InitializeCells(&render->render2d->secondC2d, false,
                             render->render2d->randGridDensity);
    render->render2d->currentGeneration = 0;
  }
}

void Render2D_RandomizeZeroGen(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Evolve2D_InitializeCells(&render->render2d->firstC2d, true,
                             render->render2d->randGridDensity);
    render->render2d->currentGeneration = 0;
  }
}

static bool __Render2D_CanEditCell(Render *render, bool isMouseOnMenu,
                                   bool isMouseOnCell) {
  return (render->render2d->menu2d.isVisible && !isMouseOnMenu &&
          isMouseOnCell) ||
         (!render->render2d->menu2d.isVisible && isMouseOnCell);
}