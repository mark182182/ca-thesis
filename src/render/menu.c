#include "menu.h"

#include "raylib_shim.h"
#include "const.h"
#include "font.h"
#include <stdlib.h>
#include "common.h"
#include <string.h>
#include <stdbool.h>
#include "render.h"

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

  Vector2 firstTextPos = {.x = (SCREEN_WIDTH / 2) - (FONT_SIZE * centerRatio),
                          .y = (SCREEN_HEIGHT / 2) - (FONT_SIZE * centerRatio)};
  Vector2 longestTextLength = {.x = 0, .y = 0};
  Vector2 currentTextPos = {.x = firstTextPos.x, .y = firstTextPos.y};

  Menu_DrawTextDefault((MenuDrawParams){.render = render,
                                        .firstTextPos = firstTextPos,
                                        .currentTextPos = &currentTextPos,
                                        .textToDraw = "2D Mode",
                                        .onCollisionFn = __Init_2D_Mode});

  Menu_DrawTextDefault((MenuDrawParams){.render = render,
                                        .firstTextPos = firstTextPos,
                                        .currentTextPos = &currentTextPos,
                                        .textToDraw = "3D Mode",
                                        .onCollisionFn = __Init_3D_Mode});
  Menu_DrawTextDefault((MenuDrawParams){.render = render,
                                        .firstTextPos = firstTextPos,
                                        .currentTextPos = &currentTextPos,
                                        .textToDraw = "Settings",
                                        .onCollisionFn = noOp});
  Menu_DrawTextDefault((MenuDrawParams){.render = render,
                                        .firstTextPos = firstTextPos,
                                        .currentTextPos = &currentTextPos,
                                        .textToDraw = "Exit",
                                        .onCollisionFn = __Close_Window});
}

void Menu_DrawDebug(Render *render) {
  Vector2 firstTextPos = {.x = 10, .y = 40};
  Vector2 longestTextLength = {.x = 0, .y = 0};
  Vector2 currentTextPos = {.x = firstTextPos.x, .y = firstTextPos.y};

  // TODO: Revise this, only a temp solution
  char fpsText[16];
  snprintf(fpsText, sizeof(fpsText), "FPS: %d", GetFPS());

  char frameTimeText[32];
  snprintf(frameTimeText, sizeof(frameTimeText), "Frametime: %0.8f",
           GetFrameTime());

  Menu_DrawTextDefault((MenuDrawParams){.render = render,
                                        .firstTextPos = firstTextPos,
                                        .currentTextPos = &currentTextPos,
                                        .textToDraw = fpsText,
                                        .onCollisionFn = NULL});
  Menu_DrawTextDefault((MenuDrawParams){.render = render,
                                        .firstTextPos = firstTextPos,
                                        .currentTextPos = &currentTextPos,
                                        .textToDraw = frameTimeText,

                                        .onCollisionFn = NULL});
}

void Menu_DrawTextDefault(MenuDrawParams drawParams) {
  drawParams.fontSize = FONT_SIZE;
  drawParams.rectColor = RECT_COLOR;
  drawParams.textColor = TEXT_COLOR;

  Menu_DrawText(drawParams);
}

void Menu_DrawText(MenuDrawParams drawParams) {
  Color currentRectColor = drawParams.rectColor;
  Vector2 textLength =
      MeasureTextEx(drawParams.render->menu->selectedFont,
                    drawParams.textToDraw, drawParams.fontSize, 0);

  Rectangle textRect = {.x = drawParams.firstTextPos.x,
                        .y = drawParams.currentTextPos->y,
                        .width = textLength.x,
                        .height = textLength.y};

  if (CheckCollisionPointRec(GetMousePosition(), textRect)) {
    currentRectColor = DARKGRAY;
    if (drawParams.onCollisionFn != NULL) {
      drawParams.onCollisionFn(drawParams.render);
    }
  }

  DrawRectangleRec(textRect, currentRectColor);
  Vector2 position = {.x = drawParams.firstTextPos.x,
                      .y = drawParams.currentTextPos->y};
  DrawTextEx(drawParams.render->menu->selectedFont, drawParams.textToDraw,
             position, drawParams.fontSize, 0, drawParams.textColor);

  drawParams.currentTextPos->x += textLength.x;
  drawParams.currentTextPos->y += textLength.y;
}

static void __Init_2D_Mode(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    render->currentMode = RENDER_MODE_2D;
    render->menu->isVisible = false;
  }
}

static void __Init_3D_Mode(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    render->currentMode = RENDER_MODE_3D;
    render->menu->isVisible = false;
    render->isMouseRestricted = true;
  }
}

static void __Close_Window(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    exit(0);
  }
}