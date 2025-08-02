#include "menu.h"

#include <raylib.h>
#include "const.h"
#include "font.h"
#include <stdlib.h>
#include "common.h"
#include <string.h>

Menu Menu_Init() {
  LoadFonts();
  Font font = fonts_by_type[FONT_TYPE_FIRA_CODE_RETINA];
  Menu menu = {.selectedFont = font,
               .cursorPosition = {.x = 0, .y = 0},
               .currentMode = RENDER_MODE_INIT};
  return menu;
}

void Menu_Update(Menu *menu) { menu->cursorPosition = GetMousePosition(); }

static void __Init_2D_Mode(Menu *menu) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    menu->currentMode = RENDER_MODE_2D;
  }
}

static void __Close_Window(Menu *menu) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    exit(0);
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

  char *wireFrameText = "Wireframe mode";
  char *quitText = "Quit";

  Menu_DrawTextDefault(menu, firstTextPos, &currentTextPos, fpsText, noOp);
  Menu_DrawTextDefault(menu, firstTextPos, &currentTextPos, frameTimeText,
                       noOp);

  Menu_DrawTextDefault(menu, firstTextPos, &currentTextPos, wireFrameText,
                       noOp);
  Menu_DrawTextDefault(menu, firstTextPos, &currentTextPos, quitText, noOp);
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

  if (CheckCollisionPointRec(menu->cursorPosition, textRect)) {
    currentRectColor = DARKGRAY;
    onCollisionFn(menu);
  }

  DrawRectangleRec(textRect, currentRectColor);
  Vector2 position = {.x = firstTextPos.x, .y = currentTextPos->y};
  DrawTextEx(menu->selectedFont, textToDraw, position, fontSize, 0, textColor);

  currentTextPos->x += textLength.x;
  currentTextPos->y += textLength.y;
}