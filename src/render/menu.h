#ifndef MENU_H
#define MENU_H

#include <raylib.h>

// forward declarations
typedef struct Render Render;

typedef enum RenderMode {
  RENDER_MODE_INIT,
  RENDER_MODE_2D,
  RENDER_MODE_3D,
  MAX_RENDER_MODE
} RenderMode;

/**
 * The main menu that is used to select the render modes. Also contains the main
 * read-only settings for other menus, like the selected font or the cursor
 * position.
 */
typedef struct Menu {
  Font selectedFont;

  // TODO: A state machine to handle the mode changes would be better, but this
  // keeps it simple for now
  RenderMode prevMode;
  RenderMode currentMode;

  bool isVisible;
} Menu;

Menu Menu_Init();
void Menu_Update(Render *render);
void Menu_Draw(Render *render);
void Menu_DrawDebug(Render *render);

void Menu_DrawTextDefault(Render *render, Vector2 firstTextPos,
                          Vector2 *currentTextPos, const char *textToDraw,
                          void (*onCollisionFn)(Render *render));

void Menu_DrawText(Render *render, Vector2 firstTextPos,
                   Vector2 *currentTextPos, const char *textToDraw,
                   int fontSize, Color rectColor, Color textColor,
                   void (*onCollisionFn)(Render *render));

static void __Init_2D_Mode(Render *render);
static void __Init_3D_Mode(Render *render);
static void __Close_Window(Render *render);

#endif