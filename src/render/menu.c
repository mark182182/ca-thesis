#include "menu.h"

#include "raylib_shim.h"
#include "const.h"
#include "font.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "render.h"

MenuDrawParams MenuParams_InitWithDefaults(Render *render) {
  return (MenuDrawParams){.render = render,
                          .alignment = MENU_DRAW_ALIGNMENT_VERTICAL,
                          .fontSize = MAIN_FONT_SIZE,
                          .rectColor = DEFAULT_RECT_COLOR,
                          .textColor = DEFAULT_TEXT_COLOR,
                          .font = render->menu->selectedFont,
                          .onCollisionFn = NULL};
}
MenuDrawParams MenuParams_ShallowCopy(MenuDrawParams *drawParams) {
  return (MenuDrawParams){.render = drawParams->render,
                          .firstTextPos = drawParams->firstTextPos,
                          .currentTextPos = drawParams->currentTextPos,
                          .alignment = drawParams->alignment,
                          .textToDraw = drawParams->textToDraw,
                          .fontSize = drawParams->fontSize,
                          .font = drawParams->font,
                          .rectColor = drawParams->rectColor,
                          .textColor = drawParams->textColor,
                          .onCollisionFn = drawParams->onCollisionFn};
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
  Vector2 currentTextPos = {.x = firstTextPos.x, .y = firstTextPos.y};

  MenuDrawParams commonParams = MenuParams_InitWithDefaults(render);
  commonParams.firstTextPos = firstTextPos;
  commonParams.currentTextPos = &currentTextPos;

  MenuDrawParams mode2DParams = MenuParams_ShallowCopy(&commonParams);
  // NOTE: usually there should be an existing dictionary that contains the user
  // facing texts/messages to draw instead of hard-coding them
  mode2DParams.textToDraw = "2D Mode";
  mode2DParams.onCollisionFn = __OnCollInit2DMode;
  Menu_DrawText(&mode2DParams);

  MenuDrawParams mode3DParams = MenuParams_ShallowCopy(&commonParams);
  mode3DParams.textToDraw = "3D Mode";
  mode3DParams.onCollisionFn = __OnCollInit3DMode;
  Menu_DrawText(&mode3DParams);

  MenuDrawParams settingsParams = MenuParams_ShallowCopy(&commonParams);
  settingsParams.textToDraw = "Settings";
  Menu_DrawText(&settingsParams);

  MenuDrawParams exitParams = MenuParams_ShallowCopy(&commonParams);
  exitParams.textToDraw = "Exit";
  exitParams.onCollisionFn = __OnCollCloseWindow;
  Menu_DrawText(&exitParams);
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

  MenuDrawParams commonParams = MenuParams_InitWithDefaults(render);
  commonParams.firstTextPos = firstTextPos;
  commonParams.currentTextPos = &currentTextPos;

  MenuDrawParams fpsParams = MenuParams_ShallowCopy(&commonParams);
  fpsParams.textToDraw = fpsText;
  Menu_DrawText(&fpsParams);

  MenuDrawParams frameTimeParams = MenuParams_ShallowCopy(&commonParams);
  frameTimeParams.textToDraw = frameTimeText;
  Menu_DrawText(&frameTimeParams);
}

void Menu_DrawText(MenuDrawParams *drawParams) {
  Color currentRectColor = drawParams->rectColor;
  Vector2 textLength = MeasureTextEx(drawParams->font, drawParams->textToDraw,
                                     drawParams->fontSize, 0);

  Vector2 textRectPosition;
  Vector2 positionToDraw;
  if (MENU_DRAW_ALIGNMENT_VERTICAL == drawParams->alignment) {
    textRectPosition = (Vector2){.x = drawParams->firstTextPos.x,
                                 .y = drawParams->currentTextPos->y};

    positionToDraw = (Vector2){.x = drawParams->firstTextPos.x,
                               .y = drawParams->currentTextPos->y};

  } else if (MENU_DRAW_ALIGNMENT_HORIZONTAL == drawParams->alignment) {
    textRectPosition = (Vector2){.x = drawParams->currentTextPos->x,
                                 .y = drawParams->currentTextPos->y};

    positionToDraw = (Vector2){.x = 50,
                               .y = 50};
  }

  Rectangle textRect = {.x = textRectPosition.x,
                        .y = textRectPosition.y,
                        .width = textLength.x,
                        .height = textLength.y};

  if (drawParams->onCollisionFn != NULL &&
      CheckCollisionPointRec(GetMousePosition(), textRect)) {
    currentRectColor = DARKGRAY;
    drawParams->onCollisionFn(drawParams->render);
  }

  DrawRectangleRec(textRect, currentRectColor);
  DrawTextEx(drawParams->render->menu->selectedFont, drawParams->textToDraw,
             positionToDraw, drawParams->fontSize, 0, drawParams->textColor);

  drawParams->currentTextPos->x += textLength.x;
  drawParams->currentTextPos->y += textLength.y;
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