#include "const.h"
#include "render/render.h"
#include <stdbool.h>
#include <stdlib.h>

int main() {
  InitializeConstants();

  // load any long living resources here
  Render render = {
      .mode2DArena = NULL,
      .mode3DArena = NULL,
      .frame2DArena = NULL,
      .frame3DArena = NULL,

      .menu = NULL,

      .charPressed = 0,
      .deltaTime = 0,
      .fpsCap = FPS_CAP,

      .isWireframeMode = false,
      .isPaused = false,
      .isDebugOn = false,
  };

  Render_RenderWindow(&render);
  return 0;
}
