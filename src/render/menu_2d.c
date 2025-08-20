#include "menu_2d.h"
#include "render.h"
#include "render_2d.h"
#include <stdio.h>
#include "common.h"

void Menu2D_Draw(Render *render) {
  // NOTE: ideally this should be dynamically set based on the screen size in
  // order to accommodate smaller/larger displays
  uint8_t screenDivRatio = 5;

  // draw a container on the right hand side of the screen
  Rectangle containerRect = {
      .x = SCREEN_WIDTH - (SCREEN_WIDTH / screenDivRatio),
      .y = 0,
      .width = SCREEN_WIDTH / screenDivRatio,
      .height = SCREEN_HEIGHT,
  };

  DrawRectangleRec(containerRect, LIME);

  char render2DSpeedText[64];

  snprintf(render2DSpeedText, sizeof(render2DSpeedText), "Render speed: %.3f",
           render->render2d->render2DSpeed);

  char *ruleText = "Current rule: Game of Life";

  Vector2 firstTextPos = {.x = SCREEN_WIDTH, .y = 0};
  Vector2 currentTextPos = {.x = firstTextPos.x, .y = firstTextPos.y};

  Menu_DrawTextDefault((MenuDrawParams){.render = render,
                                        .firstTextPos = firstTextPos,
                                        .currentTextPos = &currentTextPos,
                                        .textToDraw = render2DSpeedText,
                                        .onCollisionFn = noOp});

  Menu_DrawTextDefault((MenuDrawParams){.render = render,
                                        .firstTextPos = firstTextPos,
                                        .currentTextPos = &currentTextPos,
                                        .textToDraw = render2DSpeedText,
                                        .onCollisionFn = noOp});

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
