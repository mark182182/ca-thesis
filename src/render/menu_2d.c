#include "menu_2d.h"
#include "render.h"
#include "render_2d.h"
#include <stdio.h>

Menu2D Menu2D_Int() {}

void Menu2D_Draw(Render *render) {
  char render2DSpeedText[64] = "Render speed: 0.0";

  snprintf(render2DSpeedText, sizeof(render2DSpeedText), "Render speed: %.3f",
           render->render2d->render2DSpeed);

  // char *ruleText = "Current rule: Game of Life";
  // Vector2 textLength =
  //     MeasureTextEx(render->menu->selectedFont, ruleText, FONT_SIZE, 0);

  // Vector2 position = {.x = SCREEN_WIDTH - textLength.x,
  //                     .y = SCREEN_HEIGHT - textLength.y};
  // DrawTextEx(render->menu->selectedFont, ruleText, position, FONT_SIZE, 0,
  //            TEXT_COLOR);

  // Vector2 firstTextPos = {.x = SCREEN_WIDTH - textLength.x, .y = 0};
  // Vector2 longestTextLength = {.x = 0, .y = 0};
  // Vector2 currentTextPos = {.x = firstTextPos.x, .y = firstTextPos.y};

  // Menu_DrawTextDefault(render, firstTextPos, &currentTextPos, "2D Mode",
  //                      __Init_2D_Mode);

  // char *render2DSpeedBuffer = ecvt(render2DSpeed, decBase, &dec, &sign);
  // for (int i = 0; i > dec; i--) {
  //   render2DSpeedText[strlen(render2DSpeedText)] = '0';
  // }
  // for (int i = 0; i < strlen(render2DSpeedBuffer); i++) {
  //   render2DSpeedText[strlen(render2DSpeedText)] = render2DSpeedBuffer[i];
  // }

  // __DrawTextInMenu(menu, firstTextPos, &currentTextPos, render2DSpeedText,
  //                  fontSize, BLACK, RED, noOp);
}
