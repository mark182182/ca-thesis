#include "menu.h"

#include <raylib.h>
#include "const.h"
#include "font.h"
#include <stdlib.h>
#include "common.h"
#include <string.h>
#include <stdbool.h>
#include "render.h"

Menu Menu_Init() {
  LoadFonts();
  Font font = fonts_by_type[FONT_TYPE_FIRA_CODE_RETINA];
  Menu menu = {
      .selectedFont = font, .currentMode = RENDER_MODE_INIT, .isVisible = true};
  return menu;
}

void Menu_Update(Menu *menu) {
  if (IsKeyPressed(KEY_ESCAPE)) {
    menu->isVisible = !menu->isVisible;
  }
}

void Menu_Draw(Menu *menu) {
  uint8_t optionCount = 3;
  Vector2 firstTextPos = {.x = (SCREEN_WIDTH / 2) - (FONT_SIZE * optionCount),
                          .y = (SCREEN_HEIGHT / 2) - (FONT_SIZE * optionCount)};
  Vector2 longestTextLength = {.x = 0, .y = 0};
  Vector2 currentTextPos = {.x = firstTextPos.x, .y = firstTextPos.y};

  Menu_DrawTextDefault(menu, firstTextPos, &currentTextPos, "2D Mode",
                       __Init_2D_Mode);
  Menu_DrawTextDefault(menu, firstTextPos, &currentTextPos, "3D Mode", noOp);
  Menu_DrawTextDefault(menu, firstTextPos, &currentTextPos, "Settings", noOp);
  Menu_DrawTextDefault(menu, firstTextPos, &currentTextPos, "Exit",
                       __Close_Window);
}

void Menu_DrawDebug(Menu *menu) {
  Vector2 firstTextPos = {.x = 10, .y = 40};
  Vector2 longestTextLength = {.x = 0, .y = 0};
  Vector2 currentTextPos = {.x = firstTextPos.x, .y = firstTextPos.y};

  // TODO: Revise this, only a temp solution
  char fpsText[16];
  snprintf(fpsText, sizeof(fpsText), "FPS: %d", GetFPS());

  char frameTimeText[32];
  snprintf(frameTimeText, sizeof(frameTimeText), "Frametime: %0.8f",
           GetFrameTime());

  Menu_DrawTextDefault(menu, firstTextPos, &currentTextPos, fpsText, NULL);
  Menu_DrawTextDefault(menu, firstTextPos, &currentTextPos, frameTimeText,
                       NULL);
}

void Menu_DrawTextDefault(Menu *menu, Vector2 firstTextPos,
                          Vector2 *currentTextPos, const char *textToDraw,
                          void (*onCollisionFn)(Menu *menu)) {
  Menu_DrawText(menu, firstTextPos, currentTextPos, textToDraw, FONT_SIZE,
                RECT_COLOR, TEXT_COLOR, onCollisionFn);
}

void Menu_DrawText(Menu *menu, Vector2 firstTextPos, Vector2 *currentTextPos,
                   const char *textToDraw, int fontSize, Color rectColor,
                   Color textColor, void (*onCollisionFn)(Menu *menu)) {
  Color currentRectColor = rectColor;
  Vector2 textLength =
      MeasureTextEx(menu->selectedFont, textToDraw, fontSize, 0);

  Rectangle textRect = {.x = firstTextPos.x,
                        .y = currentTextPos->y,
                        .width = textLength.x,
                        .height = textLength.y};

  if (CheckCollisionPointRec(GetMousePosition(), textRect)) {
    currentRectColor = DARKGRAY;
    if (onCollisionFn != NULL) {
      onCollisionFn(menu);
    }
  }

  DrawRectangleRec(textRect, currentRectColor);
  Vector2 position = {.x = firstTextPos.x, .y = currentTextPos->y};
  DrawTextEx(menu->selectedFont, textToDraw, position, fontSize, 0, textColor);

  currentTextPos->x += textLength.x;
  currentTextPos->y += textLength.y;
}

static void __Init_2D_Mode(Menu *menu) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    menu->currentMode = RENDER_MODE_2D;
    menu->isVisible = false;
  }
}

static void __Init_3D_mode(Menu *menu) {
  // TODO
}

static void __Close_Window(Menu *menu) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    exit(0);
  }
}