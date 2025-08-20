#ifndef MENU_H
#define MENU_H

#include "raylib_shim.h"
#include <stdbool.h>

// forward declarations
typedef struct Render Render;

typedef enum RenderMode {
  RENDER_MODE_INIT,
  RENDER_MODE_2D,
  RENDER_MODE_3D,
  MAX_RENDER_MODE
} RenderMode;

/*
 * Struct for common menu fields
 */
// typedef struct Menu {
// } Menu;

/**
 * The main menu that is used to select the render modes. Also contains the main
 * read-only settings for other menus, like the selected font or the cursor
 * position.
 */
typedef struct MainMenu {
  Font selectedFont;
  bool isVisible;
} MainMenu;

typedef struct MenuDrawParams {
  Render *render;
  Vector2 firstTextPos;
  Vector2 *currentTextPos;

  const char *textToDraw;
  int fontSize;

  Color rectColor;
  Color textColor;

  void (*onCollisionFn)(Render *render);

  bool isLastToDraw;
} MenuDrawParams;

MainMenu Menu_Init();
void Menu_Update(Render *render);
void Menu_Draw(Render *render);
void Menu_DrawDebug(Render *render);

void Menu_DrawTextDefault(MenuDrawParams drawParams);

void Menu_DrawText(MenuDrawParams drawParams);

static void __Init_2D_Mode(Render *render);
static void __Init_3D_Mode(Render *render);
static void __Close_Window(Render *render);

#endif