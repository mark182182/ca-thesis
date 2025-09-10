#include "menu_2d.h"
#include "raylib_shim.h"
#include "menu.h"
#include "render.h"
#include "render_2d.h"
#include <stdio.h>
#include "const.h"
#include "common.h"

/*
 * TODO: The menu should be rendered on the right hand side of the screen.
 * [x] By default the menu should be in expanded mode with the ability to
 * minimize it to a ≡ icon.
 * The menu should contain the following options:
 * [x] Display the current generation
 * [x] Pause the rendering
 * [x] Revert the rendering to the 0th generation
 * [ ] Generate random cells based on the density
 * NOTE: After 3D is done and have the time: multiple rules
 * - Select built-in rules to use
 * - The user can write their custom rules (using fixed UI elements or maybe
 * Lua?)
 */
void Menu2D_Draw(Render *render) {
  char *minimizeText = "=Max=";

  if (render->render2d->menu2d.isVisible) {
    // NOTE: ideally this should be dynamically set based on the screen size in
    // order to accommodate smaller/larger displays
    float screenDivRatio = 3.5F;
    minimizeText = "=Min=";

    // draw a container on the right hand side of the screen
    Rectangle containerRect = {
        .x = SCREEN_WIDTH - (SCREEN_WIDTH / screenDivRatio),
        .y = 0,
        .width = SCREEN_WIDTH / screenDivRatio,
        .height = SCREEN_HEIGHT,
    };

    DrawRectangleRec(containerRect,
                     (Color){DEFAULT_RECT_COLOR.r, DEFAULT_RECT_COLOR.b,
                             DEFAULT_RECT_COLOR.g, 150});

    Vector2 firstTextPos = {.x = SCREEN_WIDTH - (SCREEN_WIDTH / screenDivRatio),
                            .y = 0};

    MenuDrawItem commonDrawItem = MenuDrawItem_InitWithDefaults(render);
    commonDrawItem.fontSize = SUB_FONT_SIZE;

    // NOTE: revise this, only temp solution
    MenuDrawItem statusItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    statusItem.textToDraw = "==Status==";

    MenuDrawItem blankItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    blankItem.textToDraw = " ";

    char render2DSpeedText[32];
    snprintf(render2DSpeedText, sizeof(render2DSpeedText),
             "Render speed: % .3f", render->render2d->render2DSpeed);

    MenuDrawItem renderSpeedItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    renderSpeedItem.textToDraw = render2DSpeedText;

    MenuDrawItem ruleItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    ruleItem.textToDraw = "Current rule: Game of Life";

    char currGenText[32];
    snprintf(currGenText, sizeof(currGenText), "Current generation: %d",
             render->render2d->currentGeneration);

    MenuDrawItem currGenItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    currGenItem.textToDraw = currGenText;

    char aliveCellsText[32];
    snprintf(aliveCellsText, sizeof(aliveCellsText),
             "Alive current gen cells: %d",
             render->render2d->firstC2d.aliveCells);

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

    char gridDensityText[32];
    snprintf(gridDensityText, sizeof(gridDensityText), "Grid density: %d ",
             render->render2d->gridDensity);

    MenuDrawItem gridDensityItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    gridDensityItem.textToDraw = gridDensityText;

    MenuDrawItem densitySubItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    densitySubItem.textToDraw = "[-]";
    densitySubItem.alignment = MENU_DRAW_ALIGNMENT_HORIZONTAL;
    densitySubItem.onCollisionFn = Render2D_DecrementGridDensity;

    MenuDrawItem densityPlusItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    densityPlusItem.textToDraw = "[+]";
    densityPlusItem.alignment = MENU_DRAW_ALIGNMENT_HORIZONTAL;
    densityPlusItem.onCollisionFn = Render2D_IncrementGridDensity;

    MenuDrawItem resetItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    resetItem.textToDraw = "Reset cells";
    resetItem.onCollisionFn = Render2D_ResetCells;

    MenuDrawItem randFirstGenItem = MenuDrawItem_ShallowCopy(&commonDrawItem);
    randFirstGenItem.textToDraw = "Randomize 0st gen";
    randFirstGenItem.onCollisionFn = Render2D_RandomizeZeroGen;

    MenuDrawItem items[] = {statusItem,      blankItem,       renderSpeedItem,
                            ruleItem,        currGenItem,     aliveCellsItem,
                            controlItem,     blankItem,       pauseItem,
                            gridDensityItem, densitySubItem,  densityPlusItem,
                            resetItem,       randFirstGenItem};

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
  minimizeItem.onCollisionFn = __ToggleMinimize2DMenu;

  MenuDrawItem minimItems[] = {minimizeItem};

  MenuDrawParams drawMinimizeParams = MenuDrawParams_InitWithDefaults(render);
  drawMinimizeParams.startingTextPos = minimizeTextPos;
  drawMinimizeParams.items = minimItems;
  drawMinimizeParams.numOfItems = sizeof(minimItems) / sizeof(minimItems[0]);
  Menu_DrawText(&drawMinimizeParams);
}

static void __ToggleMinimize2DMenu(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    render->render2d->menu2d.isVisible = !render->render2d->menu2d.isVisible;
  }
}