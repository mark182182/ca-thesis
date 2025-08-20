#include "menu_2d.h"
#include "render.h"
#include "render_2d.h"
#include <stdio.h>

Menu2D Menu2D_Int() {}

void Menu2D_Draw(Render *render) {
  char render2DSpeedText[64] = "Render speed: 0.0";

  snprintf(render2DSpeedText, sizeof(render2DSpeedText), "Render speed: %.3f",
           render->render2d->render2DSpeed);

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
