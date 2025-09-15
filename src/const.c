#include "const.h"
#include "raylib_shim.h"
#include "dstructs/arena.h"
#include "cellular/cells.h"
#include <stdio.h>
#include <stdlib.h>

// bool DEBUG_MODE = false;

const float SCRREN_DIV_RATIO = 3.5F;

// thread related defaults
const LPSECURITY_ATTRIBUTES THREAD_DEFAULT_SEC_ATTRIBUTES = NULL;
const size_t THREAD_DEFAULT_STACK_SIZE = 0;
const DWORD THREAD_DEFAULT_CREATION_FLAGS = 0;
const LPDWORD THREAD_DEFAULT_THREAD_ID = NULL;
extern const bool THREAD_DEFAULT_WAIT_FOR_ALL = true;
extern const LPDWORD THREAD_DEFAULT_WAIT_MS = 1000;

// raylib related defaults
static const Color BLUISH_GREY = CLITERAL(Color){105, 105, 130, 255};

const Color DEFAULT_RECT_COLOR = LIGHTGRAY;
const Color DEFAULT_TEXT_COLOR = BLUISH_GREY;
const Color RANDOM_COLORS[24] = {
    LIGHTGRAY, GRAY,   DARKGRAY, YELLOW,     GOLD,      ORANGE,  PINK,
    RED,       MAROON, GREEN,    LIME,       DARKGREEN, SKYBLUE, BLUE,
    DARKBLUE,  PURPLE, VIOLET,   DARKPURPLE, BEIGE,     BROWN,   DARKBROWN};

// forward declarations
Arena permanentArena;
int numOfProcessors;
int chunkSizePerThread;

void InitializeConstants() {
  printf("=====Initialization start=====\n");
  permanentArena =
      Arena_Init("permanentArena", &permanentStorage, PERMANENT_STORAGE_SIZE);

  SYSTEM_INFO sysInfo;
  GetSystemInfo(&sysInfo);

  // numOfProcessors = sysInfo.dwNumberOfProcessors;
  numOfProcessors = 1;

  chunkSizePerThread = CUBE_COUNT / numOfProcessors;

  printf("\nNum of processors: %d\n", numOfProcessors);
  printf("=====Initialization end=======\n");
}