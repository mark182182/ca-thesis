#ifndef MENU_2D_H
#define MENU_2D_H

#include <stdbool.h>

typedef struct Render Render;

typedef struct Menu2D {
  // TODO: add fields
  bool isVisible;
} Menu2D;

Menu2D Menu2D_Init(Render *render);
void Menu2D_Draw(Render *render);

#endif