#ifndef MENU_H
#define MENU_H

#include "raylib_shim.h"

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

MainMenu Menu_Init();
void Menu_Update(Render *render);
void Menu_Draw(Render *render);
void Menu_DrawDebug(Render *render);

void Menu_DrawTextDefault(Render *render, RaylibVector2 firstTextPos,
                          RaylibVector2 *currentTextPos, const char *textToDraw,
                          void (*onCollisionFn)(Render *render));

void Menu_DrawText(Render *render, RaylibVector2 firstTextPos,
                   RaylibVector2 *currentTextPos, const char *textToDraw,
                   int fontSize, RaylibColor rectColor, RaylibColor textColor,
                   void (*onCollisionFn)(Render *render));

static void __Init_2D_Mode(Render *render);
static void __Init_3D_Mode(Render *render);
static void __Close_Window(Render *render);

#endif