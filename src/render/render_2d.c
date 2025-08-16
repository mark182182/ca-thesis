#include "render_2d.h"
#include "render.h"
#include "cellular/evolve.h"
#include <assert.h>
#include <raylib.h>
#include "cellular/cells.h"
#include "dstructs/arena.h"
#include <time.h>
#include <stdio.h>
#include "const.h"

static int currentGeneration = 0;

Render2D Render2D_Init(Render *render) {
  Arena mode2DArena =
      Arena_Init("modeArena", &mode2DArenaStorage, MODE_2D_STORAGE_SIZE);
  Arena frame2DArena =
      Arena_Init("frame2DArena", &frame2DArenaStorage, FRAME_2D_STORAGE_SIZE);
  render->mode2DArena = &mode2DArena;
  render->frame2DArena = &frame2DArena;

  Camera2D camera = {0};
  camera.zoom = 1.0f;

  Render2D render2d = {.camera = camera,
                       .firstC2d = {0},
                       .secondC2d = {0},
                       .render2DSpeed = 0.4f};
  return render2d;
}

void Render2D_RenderMode(Render *render) {
  // initialize everything once
  Render2D *render2d = render->render2d;

  if (render->isModeFirstFrame) {
    printf("Entering 2D mode");
    Cells2D_InitArraysBasedOnCellSize(render->mode2DArena, &render2d->firstC2d);
    Cells2D_InitArraysBasedOnCellSize(render->mode2DArena,
                                      &render2d->secondC2d);

    // TODO: Randomization should only be set, if the user clicks on the button
    // or similar
    Evolve2D_InitializeCells(&render2d->firstC2d, false);
    Evolve2D_InitializeCells(&render2d->secondC2d, false);
  }

  // would implement batched rendering, draw calls here

  // TODO: These should come from the 2D menu
  if (IsKeyPressed(KEY_P)) {
    render->isPaused = !render->isPaused;
  }
  /*
    if (pressed == 'r') {
      // TODO: here we should set the randomization based on the user
    interaction EvolveGOL2D_InitializeCells(&firstCd, true);
      EvolveGOL2D_InitializeCells(&secondCd, false);
      currentGeneration = 0;
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
    if (currentGeneration != 0 && currentGeneration % 2 == 0) {
      // TODO: The display is weird, somehow the display of the cells doesn't
      // match up with the state
      EvolveGOL2D_NextGeneration(&render2d->firstC2d, &render2d->secondC2d);
    } else {
      EvolveGOL2D_NextGeneration(&render2d->secondC2d, &render2d->firstC2d);
    }
    currentGeneration++;
    render->deltaTime = 0;
  }
  render->deltaTime += GetFrameTime();

  Cells2D actualCd =
      currentGeneration % 2 == 0 ? render2d->secondC2d : render2d->firstC2d;

  // update variables here

  // TODO: decide on at what time should we start drawing
  Render_BeginDrawing();
  BeginMode2D(render2d->camera);

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

      // TODO: Make sure the user can set the current cell's alive state
      if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        actualCd.is_alive[i] = !actualCd.is_alive[i];
      }
    }

    if (actualCd.is_alive[i]) {
      DrawRectangle(actualCd.positionsX[i], actualCd.positionsY[i],
                    CELL_HEIGHT_RATIO, CELL_WIDTH_RATIO, color);
    } else if (color.a == YELLOW.a && color.b == YELLOW.b) {
      DrawRectangle(actualCd.positionsX[i], actualCd.positionsY[i],
                    CELL_HEIGHT_RATIO, CELL_WIDTH_RATIO, color);
    }
  }

  // TODO: revise this, only an example
  char *ruleText = "Current rule: Game of Life";
  Vector2 textLength =
      MeasureTextEx(render->menu->selectedFont, ruleText, FONT_SIZE, 0);

  Vector2 position = {.x = SCREEN_WIDTH - textLength.x,
                      .y = SCREEN_HEIGHT - textLength.y};
  DrawTextEx(render->menu->selectedFont, ruleText, position, FONT_SIZE, 0,
             TEXT_COLOR);

  EndMode2D();

  // TODO: The menu should be rendered on the right hand side of the screen
  // The menu should contain the following options:
  // - Display the current generation
  // - Pause the rendering
  // - Revert the rendering to the 0th generation
  // - Generate random cells based on the density
  // NOTE: After 3D is done and have the time: multiple rules
  // - Select built-in rules to use
  // - The user can write their custom rules (maybe Lua?)
  // if (menu2d.isVisible) {
  //   Menu2D_Draw(&menu);
  // }

  // free objects after each frame
  Arena_Free(render->frame2DArena);

  // TODO: Check, if the mode changed, then free the modeArena
}