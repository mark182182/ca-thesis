#include "raylib_shim.h"

#include "const.h"
#include "render/render.h"
#include <stdbool.h>
#include <stdlib.h>

int main() {
  // TODO: Enable debug or use Renderdoc instead
  // glEnable(GL_DEBUG_OUTPUT);
  // glDebugMessageCallback(myDebugCallback, NULL);

  InitializeConstants();

  // load any long living resources here
  Render render = {
      .mode2DArena = NULL,
      .mode3DArena = NULL,
      .frame2DArena = NULL,
      .frame3DArena = NULL,

      .menu = NULL,
      .render2d = NULL,
      .render3d = NULL,

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
