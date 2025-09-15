#ifndef MENU_3D_H
#define MENU_3D_H

#include <stdbool.h>
#include "raylib_shim.h"

typedef struct Render Render;

typedef struct Menu3D {
  // TODO: add fields
  bool isVisible;
  Rectangle containerRect;
} Menu3D;

Menu3D Menu3D_Init(Render *render);
void Menu3D_Draw(Render *render);

static void __ToggleMinimize3DMenu(Render *render);

#endif