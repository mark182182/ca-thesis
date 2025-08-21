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
 * [ ] By default the menu should be in expanded mode with the ability to
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
  if (render->render2d->menu2d.isVisible) {
    // NOTE: ideally this should be dynamically set based on the screen size in
    // order to accommodate smaller/larger displays
    float screenDivRatio = 3.5F;

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
    Vector2 currentTextPos = {.x = firstTextPos.x, .y = firstTextPos.y};

    MenuDrawParams commonParams = MenuParams_InitWithDefaults(render);
    commonParams.render = render;
    commonParams.fontSize = SUB_FONT_SIZE;
    commonParams.firstTextPos = firstTextPos;
    commonParams.currentTextPos = &currentTextPos;

    // NOTE: revise this, only temp solution
    MenuDrawParams statusParams = MenuParams_ShallowCopy(&commonParams);
    statusParams.textToDraw = "==Status==";
    Menu_DrawText(&statusParams);

    MenuDrawParams blankParams = MenuParams_ShallowCopy(&commonParams);
    blankParams.textToDraw = " ";
    Menu_DrawText(&blankParams);

    char render2DSpeedText[32];
    snprintf(render2DSpeedText, sizeof(render2DSpeedText), "Render speed: %.3f",
             render->render2d->render2DSpeed);

    MenuDrawParams renderSpeedParams = MenuParams_ShallowCopy(&commonParams);
    renderSpeedParams.textToDraw = render2DSpeedText;
    Menu_DrawText(&renderSpeedParams);

    MenuDrawParams ruleParams = MenuParams_ShallowCopy(&commonParams);
    ruleParams.textToDraw = "Current rule: Game of Life";
    Menu_DrawText(&ruleParams);

    char currGenText[32];
    snprintf(currGenText, sizeof(currGenText), "Current generation: %d",
             render->render2d->currentGeneration);

    MenuDrawParams currGenParams = MenuParams_ShallowCopy(&commonParams);
    currGenParams.textToDraw = currGenText;
    Menu_DrawText(&currGenParams);

    char aliveCellsText[32];
    snprintf(aliveCellsText, sizeof(aliveCellsText),
             "Alive current gen cells: %d",
             render->render2d->firstC2d.aliveCells);

    MenuDrawParams aliveCellsParams = MenuParams_ShallowCopy(&commonParams);
    aliveCellsParams.textToDraw = aliveCellsText;
    Menu_DrawText(&aliveCellsParams);

    char pauseText[32];
    snprintf(pauseText, sizeof(currGenText), "Pause state: %s",
             boolToString(render->isPaused));

    MenuDrawParams controlParams = MenuParams_ShallowCopy(&commonParams);
    controlParams.textToDraw = "==Controls==";
    Menu_DrawText(&controlParams);

    Menu_DrawText(&blankParams);

    MenuDrawParams pauseParams = MenuParams_ShallowCopy(&commonParams);
    pauseParams.textToDraw = pauseText;
    pauseParams.onCollisionFn = Menu_OnCollPauseRender;
    Menu_DrawText(&pauseParams);

    MenuDrawParams resetParams = MenuParams_ShallowCopy(&commonParams);
    resetParams.textToDraw = "Reset cells";
    resetParams.onCollisionFn = Render2D_ResetCells;
    Menu_DrawText(&resetParams);

    MenuDrawParams randFirstGenParams = MenuParams_ShallowCopy(&commonParams);
    randFirstGenParams.textToDraw = "Randomize 0st gen";
    randFirstGenParams.onCollisionFn = Render2D_RandomizeZeroGen;
    Menu_DrawText(&randFirstGenParams);
  }

  char *minimizeText = "=Min=";
  if (!render->render2d->menu2d.isVisible) {
    minimizeText = "=Max=";
  }

  int minimizeTextSize = MeasureText(minimizeText, SUB_FONT_SIZE);
  Vector2 minimizeTextPos = {.x = SCREEN_WIDTH - minimizeTextSize, .y = 0};
  Vector2 minimizeCurrentTextPos = {.x = minimizeTextPos.x,
                                    .y = minimizeTextPos.y};

  MenuDrawParams minimizeParams = MenuParams_InitWithDefaults(render);
  minimizeParams.render = render;
  minimizeParams.fontSize = SUB_FONT_SIZE;
  minimizeParams.firstTextPos = minimizeTextPos;
  minimizeParams.currentTextPos = &minimizeCurrentTextPos;
  minimizeParams.textToDraw = minimizeText;
  minimizeParams.onCollisionFn = __ToggleMinimize2DMenu;
  Menu_DrawText(&minimizeParams);
}

static void __ToggleMinimize2DMenu(Render *render) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    render->render2d->menu2d.isVisible = !render->render2d->menu2d.isVisible;
  }
}