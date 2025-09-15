#ifndef MENU_2D_H
#define MENU_2D_H

#include <stdbool.h>
#include "raylib_shim.h"

typedef struct Render Render;

typedef struct Menu2D {
  // TODO: add fields
  bool isVisible;
  Rectangle containerRect;
} Menu2D;

Menu2D Menu2D_Init(Render *render);
void Menu2D_Draw(Render *render);

static void __ToggleMinimize2DMenu(Render *render);

#endif