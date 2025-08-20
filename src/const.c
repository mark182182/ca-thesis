#include "const.h"
#include "raylib_shim.h"
#include "dstructs/arena.h"
#include <stdio.h>
#include <stdlib.h>

// bool DEBUG_MODE = false;

// thread related defaults
const LPSECURITY_ATTRIBUTES THREAD_DEFAULT_SEC_ATTRIBUTES = NULL;
const size_t THREAD_DEFAULT_STACK_SIZE = 0;
const DWORD THREAD_DEFAULT_CREATION_FLAGS = 0;
const LPDWORD THREAD_DEFAULT_THREAD_ID = NULL;
extern const bool THREAD_DEFAULT_WAIT_FOR_ALL = true;
extern const LPDWORD THREAD_DEFAULT_WAIT_MS = 1000;

// raylib related defaults
static const RaylibColor BLUISH_GREY =
    CLITERAL(RaylibColor){105, 105, 130, 255};

const RaylibColor RECT_COLOR = LIGHTGRAY;
const RaylibColor TEXT_COLOR = BLUISH_GREY;
const RaylibColor RANDOM_COLORS[24] = {
    LIGHTGRAY, GRAY,   DARKGRAY, YELLOW,     GOLD,      ORANGE,  PINK,
    RED,       MAROON, GREEN,    LIME,       DARKGREEN, SKYBLUE, BLUE,
    DARKBLUE,  PURPLE, VIOLET,   DARKPURPLE, BEIGE,     BROWN,   DARKBROWN};

// forward declarations
Arena permanentArena;
int numOfProcessors;

void InitializeConstants() {
  printf("=====Initialization start=====\n");
  permanentArena =
      Arena_Init("permanentArena", &permanentStorage, PERMANENT_STORAGE_SIZE);

  SYSTEM_INFO sysInfo;
  GetSystemInfo(&sysInfo);

  numOfProcessors = sysInfo.dwNumberOfProcessors;

  printf("\nNum of processors: %d\n", numOfProcessors);
  printf("=====Initialization end=======\n");
}