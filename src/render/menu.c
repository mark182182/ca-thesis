#include "menu.h"

#include "raylib_shim.h"
#include "const.h"
#include "font.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "render.h"
#include "common.h"

MenuDrawItem MenuDrawItem_InitWithDefaults(Render *render) {
  return (MenuDrawItem){.alignment = MENU_DRAW_ALIGNMENT_VERTICAL,
                        .fontSize = MAIN_FONT_SIZE,
                        .rectColor = DEFAULT_RECT_COLOR,
                        .textColor = DEFAULT_TEXT_COLOR,
                        .font = render->menu->selectedFont,
                        .onCollisionFn = NULL};
}

MenuDrawItem MenuDrawItem_ShallowCopy(MenuDrawItem *drawParams) {
  return (MenuDrawItem){.alignment = drawParams->alignment,
                        .textToDraw = drawParams->textToDraw,
                        .fontSize = drawParams->fontSize,
                        .font = drawParams->font,
                        .rectColor = drawParams->rectColor,
                        .textColor = drawParams->textColor,
                        .onCollisionFn = drawParams->onCollisionFn};
}

MenuDrawParams MenuDrawParams_InitWithDefaults(Render *render) {
  return (MenuDrawParams){.render = render,
                          .startingTextPos = Vector2_Zeroed(),
                          .items = {0},
                          .numOfItems = 0,
                          .iteratorPosStart = Vector2_Zeroed(),
                          .iteratorPosEnd = Vector2_Zeroed()};
}

MainMenu Menu_Init() {
  LoadFonts();
  Font font = fonts_by_type[FONT_TYPE_FIRA_CODE_RETINA];
  MainMenu menu = {.selectedFont = font, .isVisible = true};
  return menu;
}

void Menu_Update(Render *render) {
  if (IsKeyPressed(KEY_ESCAPE)) {
    render->menu->isVisible = !render->menu->isVisible;
    if (render->currentMode == RENDER_MODE_3D) {
      render->isMouseRestricted = !render->menu->isVisible;
    }
  }
}

void Menu_Draw(Render *render) {
  // TODO: using 3 for better centering, but this should be calculated in
  // another way to be perfectly centered
  uint8_t centerRatio = 3;

  Vector2 firstTextPos = {
      .x = (SCREEN_WIDTH / 2) - (MAIN_FONT_SIZE * centerRatio),
      .y = (SCREEN_HEIGHT / 2) - (MAIN_FONT_SIZE * centerRatio)};
  Vector2 longestTextLength = {.x = 0, .y = 0};

  MenuDrawItem mode2DItem = MenuDrawItem_InitWithDefaults(render);
  // NOTE: usually there should be an existing dictionary that contains the user
  // facing texts/messages to draw instead of hard-coding them
  mode2DItem.textToDraw = "2D Mode";
  mode2DItem.onCollisionFn = __OnCollInit2DMode;

  MenuDrawItem mode3DItem = MenuDrawItem_InitWithDefaults(render);
  mode3DItem.textToDraw = "3D Mode";
  mode3DItem.onCollisionFn = __OnCollInit3DMode;

  MenuDrawItem settingsItem = MenuDrawItem_InitWithDefaults(render);
  settingsItem.textToDraw = "Settings";

  MenuDrawItem exitItem = MenuDrawItem_InitWithDefaults(render);
  exitItem.textToDraw = "Exit";
  exitItem.onCollisionFn = __OnCollCloseWindow;

  // NOTE: could be better, if it was possible to initialize all of these items
  // here instead of creating and assigning them separately
  MenuDrawItem items[] = {mode2DItem, mode3DItem, settingsItem, exitItem};

  MenuDrawParams drawParams = MenuDrawParams_InitWithDefaults(render);
  drawParams.startingTextPos = firstTextPos;
  drawParams.items = items;
  drawParams.numOfItems = sizeof(items) / sizeof(items[0]);

  Menu_DrawText(&drawParams);
}

void Menu_DrawDebug(Render *render) {
  Vector2 firstTextPos = {.x = 10, .y = 40};
  Vector2 longestTextLength = {.x = 0, .y = 0};

  // TODO: Revise this, only a temp solution
  char fpsText[16];
  snprintf(fpsText, sizeof(fpsText), "FPS: %d", GetFPS());

  char frameTimeText[32];
  snprintf(frameTimeText, sizeof(frameTimeText), "Frametime: %0.8f",
           GetFrameTime());

  MenuDrawItem fpsItem = MenuDrawItem_InitWithDefaults(render);
  fpsItem.textToDraw = fpsText;

  MenuDrawItem frameTimeItem = MenuDrawItem_InitWithDefaults(render);
  frameTimeItem.textToDraw = frameTimeText;

  MenuDrawItem items[] = {fpsItem, frameTimeItem};

  MenuDrawParams drawParams = MenuDrawParams_InitWithDefaults(render);
  drawParams.startingTextPos = firstTextPos;
  drawParams.items = items;
  drawParams.numOfItems = sizeof(items) / sizeof(items[0]);

  Menu_DrawText(&drawParams);
}

void Menu_DrawText(MenuDrawParams *drawParams) {

  for (int i = 0; i < drawParams->numOfItems; i++) {
    MenuDrawItem item = drawParams->items[i];

    Color currentRectColor = item.rectColor;
    Vector2 textLength =
        MeasureTextEx(item.font, item.textToDraw, item.fontSize, 0);

    if (i == 0) {
      drawParams->iteratorPosStart.x = drawParams->startingTextPos.x;
      drawParams->iteratorPosStart.y = drawParams->startingTextPos.y;
    } else {
      if (item.alignment == MENU_DRAW_ALIGNMENT_VERTICAL) {
        // all vertical items should be in the same column
        drawParams->iteratorPosStart.x = drawParams->startingTextPos.x;
        // start drawing from the previous text's end vertically
        drawParams->iteratorPosStart.y = drawParams->iteratorPosEnd.y;
      } else if (item.alignment == MENU_DRAW_ALIGNMENT_HORIZONTAL) {
        // start drawing from the previous text's end horizontally
        drawParams->iteratorPosStart.x = drawParams->iteratorPosEnd.x;
      }
    }

    // always draw the current text based on the iterator's start position (that
    // should be resolved at this point)
    Vector2 posToDraw = drawParams->iteratorPosStart;

    Rectangle textRect = {.x = posToDraw.x,
                          .y = posToDraw.y,
                          .width = textLength.x,
                          .height = textLength.y};

    if (item.onCollisionFn != NULL &&
        CheckCollisionPointRec(GetMousePosition(), textRect)) {
      currentRectColor = DARKGRAY;
      item.onCollisionFn(drawParams->render);
    }
    DrawRectangleRec(textRect, currentRectColor);
    DrawTextEx(item.font, item.textToDraw, posToDraw, item.fontSize, 0,
               item.textColor);

    drawParams->iteratorPosEnd.x =
        drawParams->iteratorPosStart.x + textLength.x;
    drawParams->iteratorPosEnd.y =
        drawParams->iteratorPosStart.y + textLength.y;
  }
}

void Menu_OnCollPauseRender(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    render->isPaused = !render->isPaused;
  }
}

static void __OnCollInit2DMode(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    render->currentMode = RENDER_MODE_2D;
    render->menu->isVisible = false;
  }
}

static void __OnCollInit3DMode(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    render->currentMode = RENDER_MODE_3D;
    render->menu->isVisible = false;
    render->isMouseRestricted = true;
  }
}

static void __OnCollCloseWindow(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    exit(0);
  }
}