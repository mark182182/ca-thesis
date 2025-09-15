#include "menu_3d.h"
#include "raylib_shim.h"
#include "menu.h"
#include "render.h"
#include "render_3d.h"
#include <stdio.h>
#include "const.h"
#include "common.h"

Menu3D Menu3D_Init(Render *render) {
  // draw a container on the right hand side of the screen
  Rectangle containerRect = (Rectangle){
      .x = SCREEN_WIDTH - (SCREEN_WIDTH / SCRREN_DIV_RATIO),
      .y = 0,
      .width = SCREEN_WIDTH / SCRREN_DIV_RATIO,
      .height = SCREEN_HEIGHT,
  };

  return (Menu3D){.isVisible = true, .containerRect = containerRect};
}

/*
 * NOTE: After 3D is done and have the time: multiple rules
 * - Select built-in rules to use
 * - The user can write their custom rules (using fixed UI elements or maybe
 * Lua?)
 */
void Menu3D_Draw(Render *render) {
  char *minimizeText = "=Max=";

  if (render->render3d->menu3d.isVisible) {
    minimizeText = "=Min=";

    DrawRectangleRec(render->render3d->menu3d.containerRect,
                     (Color){DEFAULT_RECT_COLOR.r, DEFAULT_RECT_COLOR.b,
                             DEFAULT_RECT_COLOR.g, 150});

    Vector2 firstTextPos = {
        .x = SCREEN_WIDTH - (SCREEN_WIDTH / SCRREN_DIV_RATIO), .y = 0};

    MenuDrawItem commonDrawItem = MenuDrawItem_InitWithDefaults(render);
    commonDrawItem.fontSize = SUB_FONT_SIZE;

    // NOTE: revise this, only temp solution
    MenuDrawItem statusItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    statusItem.textToDraw = "==Status==";

    MenuDrawItem blankItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    blankItem.textToDraw = " ";

    char render3DSpeedText[32];
    snprintf(render3DSpeedText, sizeof(render3DSpeedText),
             "Render speed: % .3f", render->render3d->render3dSpeed);

    MenuDrawItem renderSpeedItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    renderSpeedItem.textToDraw = render3DSpeedText;

    MenuDrawItem ruleItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    ruleItem.textToDraw = "Current rule: Game of Life";

    char currGenText[32];
    snprintf(currGenText, sizeof(currGenText), "Current generation: %d",
             render->render3d->currentGeneration);

    MenuDrawItem currGenItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    currGenItem.textToDraw = currGenText;

    char aliveCellsText[32];
    snprintf(aliveCellsText, sizeof(aliveCellsText),
             "Alive current gen cells: %d",
             render->render3d->firstC3d.aliveCells);

    MenuDrawItem aliveCellsItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    aliveCellsItem.textToDraw = aliveCellsText;

    char pauseText[32];
    snprintf(pauseText, sizeof(pauseText), "Pause state: %s",
             boolToString(render->isPaused));

    MenuDrawItem controlItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    controlItem.textToDraw = "==Controls==";

    MenuDrawItem pauseItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    pauseItem.textToDraw = pauseText;
    pauseItem.onCollisionFn = Menu_OnCollPauseRender;

    char editText[32];
    snprintf(editText, sizeof(editText), "Edit state: %s",
             boolToString(render->render3d->isEditing));

    MenuDrawItem editItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    editItem.textToDraw = editText;
    editItem.onCollisionFn = Render3D_EditCells;

    char gridDensityText[32];
    snprintf(gridDensityText, sizeof(gridDensityText),
             "Rand. grid density: %d ", render->render3d->randGridDensity);

    MenuDrawItem gridDensityItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    gridDensityItem.textToDraw = gridDensityText;

    MenuDrawItem densitySubItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    densitySubItem.textToDraw = "[-]";
    densitySubItem.alignment = MENU_DRAW_ALIGNMENT_HORIZONTAL;
    densitySubItem.onCollisionFn = Render3D_DecrementGridDensity;

    MenuDrawItem densityPlusItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    densityPlusItem.textToDraw = "[+]";
    densityPlusItem.alignment = MENU_DRAW_ALIGNMENT_HORIZONTAL;
    densityPlusItem.onCollisionFn = Render3D_IncrementGridDensity;

    MenuDrawItem resetItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    resetItem.textToDraw = "Reset cells";
    resetItem.onCollisionFn = Render3D_ResetCells;

    MenuDrawItem randFirstGenItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    randFirstGenItem.textToDraw = "Randomize 0st gen";
    randFirstGenItem.onCollisionFn = Render3D_RandomizeZeroGen;

    MenuDrawItem items[] = {
        statusItem,     blankItem,       renderSpeedItem, ruleItem,
        currGenItem,    aliveCellsItem,  blankItem,       controlItem,
        blankItem,      pauseItem,       editItem,        gridDensityItem,
        densitySubItem, densityPlusItem, resetItem,       randFirstGenItem};

    MenuDrawParams drawParams = MenuDrawParams_InitWithDefaults(render);
    drawParams.startingTextPos = firstTextPos;
    drawParams.items = items;
    drawParams.numOfItems = sizeof(items) / sizeof(items[0]);

    Menu_DrawText(&drawParams);
  }

  int minimizeTextSize = MeasureText(minimizeText, SUB_FONT_SIZE);
  Vector2 minimizeTextPos = {.x = SCREEN_WIDTH - minimizeTextSize, .y = 0};

  MenuDrawItem minimizeItem = MenuDrawItem_InitWithDefaults(render);
  minimizeItem.fontSize = SUB_FONT_SIZE;
  minimizeItem.textToDraw = minimizeText;
  minimizeItem.onCollisionFn = __ToggleMinimize3DMenu;

  MenuDrawItem minimItems[] = {minimizeItem};

  MenuDrawParams drawMinimizeParams = MenuDrawParams_InitWithDefaults(render);
  drawMinimizeParams.startingTextPos = minimizeTextPos;
  drawMinimizeParams.items = minimItems;
  drawMinimizeParams.numOfItems = sizeof(minimItems) / sizeof(minimItems[0]);
  Menu_DrawText(&drawMinimizeParams);
}

static void __ToggleMinimize3DMenu(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    render->render3d->menu3d.isVisible = !render->render3d->menu3d.isVisible;
  }
}