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

/**
 * Alignment of the current element based on the previous one
 */
typedef enum MenuDrawAlignment {
  MENU_DRAW_ALIGNMENT_HORIZONTAL,
  MENU_DRAW_ALIGNMENT_VERTICAL
} MenuDrawAlignment;

typedef struct MenuDrawParams {
  Render *render;
  Vector2 firstTextPos;
  Vector2 *currentTextPos;
  MenuDrawAlignment alignment;

  const char *textToDraw;
  int fontSize;

  Font font;
  Color rectColor;
  Color textColor;

  bool shouldCollide;
  void (*onCollisionFn)(Render *render);
} MenuDrawParams;

// NOTE: __may__ be inlined at call sites
inline MenuDrawParams MenuParams_InitWithDefaults(Render *render);
inline MenuDrawParams MenuParams_ShallowCopy(MenuDrawParams *drawParams);

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

MainMenu Menu_Init();
void Menu_Update(Render *render);
void Menu_Draw(Render *render);
void Menu_DrawDebug(Render *render);

void Menu_DrawText(MenuDrawParams *drawParams);

void Menu_OnCollPauseRender(Render *render);

static void __OnCollInit2DMode(Render *render);
static void __OnCollInit3DMode(Render *render);
static void __OnCollCloseWindow(Render *render);

#endif