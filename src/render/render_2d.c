#include "render_2d.h"
#include "render.h"
#include "cellular/evolve.h"
#include <assert.h>
#include "raylib_shim.h"
#include "cellular/cells.h"
#include "dstructs/arena.h"
#include <time.h>
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

  Menu2D menu2d = {.isVisible = true};

  Render2D render2d = {.menu2d = menu2d,
                       .camera = camera,
                       .firstC2d = {0},
                       .secondC2d = {0},
                       .render2DSpeed = 0.4f,
                       .currentGeneration = 0,
                       .gridDensity = CELL_INITIAL_GRID_DENSITY};
  return render2d;
}

void Render2D_RenderMode(Render *render) {
  // initialize everything once
  Render2D *render2d = render->render2d;

  if (render->isModeFirstFrame) {
    printf("Rendering 2D mode\n");
    Cells2D_InitArraysBasedOnCellSize(&render->mode2DArena,
                                      &render2d->firstC2d);
    Cells2D_InitArraysBasedOnCellSize(&render->mode2DArena,
                                      &render2d->secondC2d);

    // TODO: Randomization should only be set, if the user clicks on the button
    // or similar
    Evolve2D_InitializeCells(&render2d->firstC2d, false, render2d->gridDensity);
    Evolve2D_InitializeCells(&render2d->secondC2d, false,
                             render2d->gridDensity);
  }

  // would implement batched rendering, draw calls here

  if (render->isPaused == 0 && render->deltaTime >= render2d->render2DSpeed) {
    clock_t time = clock();
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
                          ? &render2d->secondC2d
                          : &render2d->firstC2d;

  // update variables here

  // TODO: decide on at what time should we start drawing
  Render_BeginDrawing();
  BeginMode2D(render2d->camera);

  // TODO: This should be drawn in a single call
  int aliveCells = 0;
  
  for (int i = 0; i < CELL_COUNT; i++) {
    Color color = *actualCd->colors[i];
    // TODO: Revise this, just an example
    Rectangle rect = {.height = CELL_HEIGHT_RATIO,
                      .width = CELL_WIDTH_RATIO,
                      .x = actualCd->positionsX[i],
                      .y = actualCd->positionsY[i]};
    if (CheckCollisionPointRec(GetMousePosition(), rect)) {
      Vector2 position = {.x = 100, .y = 200};
      char posText[32];
      snprintf(posText, sizeof(posText), "PosX: %f, cellX: %f",
               GetMousePosition().x, rect.x);

      DrawTextEx(render->menu->selectedFont, posText, GetMousePosition(),
                 SUB_FONT_SIZE, 0, DEFAULT_TEXT_COLOR);
      color = YELLOW;

      // TODO: Make sure the user can set the current cell's alive state
      if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        actualCd->is_alive[i] = !actualCd->is_alive[i];
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

void Render2D_IncrementGridDensity(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
      render->render2d->gridDensity < 20) {
    render->render2d->gridDensity++;
  }
}

void Render2D_DecrementGridDensity(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
      render->render2d->gridDensity > 0) {
    render->render2d->gridDensity--;
  }
}

void Render2D_ResetCells(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Evolve2D_InitializeCells(&render->render2d->firstC2d, false,
                             render->render2d->gridDensity);
    Evolve2D_InitializeCells(&render->render2d->secondC2d, false,
                             render->render2d->gridDensity);
    render->render2d->currentGeneration = 0;
  }
}

void Render2D_RandomizeZeroGen(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Evolve2D_InitializeCells(&render->render2d->firstC2d, true,
                             render->render2d->gridDensity);
    render->render2d->currentGeneration = 0;
  }
}