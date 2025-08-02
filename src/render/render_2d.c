#include "render_2d.h"
#include "render.h"
#include "cellular/evolve.h"
#include <assert.h>
#include <raylib.h>
#include "cellular/cells.h"
#include "dstructs/arena.h"
#include <time.h>
#include "rlgl.h"
#include <stdio.h>

int currentGeneration = 0;

Render2D Render2D_Init() {
  Render2D render2d = {
      .render2DSpeed = 0.4f, .firstC2d = {0}, .secondC2d = {0}};
  return render2d;
}

void Render2D_RenderMode(Render *render) {
  // initialize everything once
  Render2D *render2d = render->render2d;

  if (render->isModeFirstFrame) {
    Cells2D_InitArraysBasedOnCellSize(render->mode2DArena, &render2d->firstC2d);
    Cells2D_InitArraysBasedOnCellSize(render->mode2DArena,
                                      &render2d->secondC2d);

    Evolve2D_InitializeCells(&render2d->firstC2d, true);
    Evolve2D_InitializeCells(&render2d->secondC2d, false);
  }

  // would implement batched rendering, draw calls here

  // TODO: These should come from the 2D menu
  /*
    if (pressed == 'r') {
      EvolveGOL2D_InitializeCells(&firstCd, true);
      EvolveGOL2D_InitializeCells(&secondCd, false);
      currentGeneration = 0;
    }

    if (pressed == 'p') {
      isPaused = !isPaused;
    }

    if (pressed == 'w') {
      render2DSpeed -= 0.001f;
    }

    if (pressed == 's') {
      render2DSpeed += 0.001f;
    }
  */

  if (render->isPaused == 0 && render->deltaTime >= render2d->render2DSpeed) {
    clock_t time = clock();
    if (currentGeneration != 0) {
      // TODO: The display is weird, somehow the display of the cells doesn't
      // match up with the state
      if (currentGeneration % 2 == 0) {
        EvolveGOL2D_NextGeneration(&render2d->firstC2d, &render2d->secondC2d);
      } else {
        EvolveGOL2D_NextGeneration(&render2d->secondC2d, &render2d->firstC2d);
      }
    }
    currentGeneration++;
    render->deltaTime = 0;
  }
  render->deltaTime += GetFrameTime();

  Cells2D actualCd =
      currentGeneration % 2 == 0 ? render2d->secondC2d : render2d->firstC2d;

  // update variables here

  BeginMode2D(render->camera2d);

  // TODO: This should be drawn in a single call
  for (int i = 0; i < CELL_COUNT; i++) {
    Color color = *actualCd.colors[i];
    // TODO: Revise this, just an example
    Rectangle rect = {.height = CELL_HEIGHT_RATIO,
                      .width = CELL_WIDTH_RATIO,
                      .x = actualCd.positionsX[i],
                      .y = actualCd.positionsY[i]};
    if (CheckCollisionPointRec(GetMousePosition(), rect)) {
      Vector2 position = {.x = 100, .y = 200};
      char posText[32];
      snprintf(posText, sizeof(posText), "Pos: %f, cellX: %f",
               GetMousePosition().x, rect.x);

      DrawTextEx(render->menu->selectedFont, posText, GetMousePosition(),
                 FONT_SIZE, 0, TEXT_COLOR);
      color = YELLOW;
    }

    if (actualCd.cells[i].is_alive) {
      DrawRectangle(actualCd.positionsX[i], actualCd.positionsY[i],
                    CELL_HEIGHT_RATIO, CELL_WIDTH_RATIO, color);
    } else if (color.a == YELLOW.a && color.b == YELLOW.b) {
      DrawRectangle(actualCd.positionsX[i], actualCd.positionsY[i],
                    CELL_HEIGHT_RATIO, CELL_WIDTH_RATIO, color);
    }
  }

  EndMode2D();

  // free objects after each frame
  Arena_Free(render->frame2DArena);

  // TODO: Check, if the mode changed, then free the modeArena
}