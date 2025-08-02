#include "const.h"
#include <raylib.h>
#include "dstructs/arena.h"

// bool DEBUG_MODE = false;

static const Color BLUISH_GREY = CLITERAL(Color){105, 105, 130, 255};

const Color RECT_COLOR = LIGHTGRAY;
const Color TEXT_COLOR = BLUISH_GREY;
const Color RANDOM_COLORS[24] = {
    LIGHTGRAY, GRAY,   DARKGRAY, YELLOW,     GOLD,      ORANGE,  PINK,
    RED,       MAROON, GREEN,    LIME,       DARKGREEN, SKYBLUE, BLUE,
    DARKBLUE,  PURPLE, VIOLET,   DARKPURPLE, BEIGE,     BROWN,   DARKBROWN};

Arena permanentArena;

void InitializeConstants() {
  permanentArena =
      Arena_Init("permanentArena", &permanentStorage, PERMANENT_STORAGE_SIZE);
}