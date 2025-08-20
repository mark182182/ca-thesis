#include "evolve.h"
#include "dstructs/arena.h"
#include "const.h"
#include "cells.h"
#include <stdlib.h>
#include "raylib_shim.h"

int rule1DBits = 0x00000000;

void Evolve2D_InitializeCells(Cells2D *c2d, bool randomizeAlive) {
  int i = 0;
  for (int posX = 0; posX < SCREEN_WIDTH; posX += CELL_WIDTH_RATIO) {
    for (int posY = 0; posY < SCREEN_HEIGHT; posY += CELL_HEIGHT_RATIO) {
      if (randomizeAlive) {
        bool is_alive = rand() % CELL_INITIAL_GRID_DENSITY == 0;
        c2d->is_alive[i] = is_alive;
      } else {
        c2d->is_alive[i] = 0;
      }
      c2d->positionsX[i] = posX;
      c2d->positionsY[i] = posY;
      c2d->colors[i] = &RANDOM_COLORS[rand() % 2];
      i++;
    }
  }
}

void Evolve3D_InitializeCells(Cells3D *c3d, bool randomizeAlive) {
  int i = 0;
  for (int x = 0; x < MAX_CUBES_X; x = x + CUBE_SIZE) {
    for (int y = 0; y < MAX_CUBES_Y; y = y + CUBE_SIZE) {
      for (int z = 0; z < MAX_CUBES_Z; z = z + CUBE_SIZE) {
        if (randomizeAlive) {
          bool is_alive = rand() % CUBE_INITIAL_GRID_DENSITY == 0;
          c3d->is_alive[i] = is_alive;
        } else {
          c3d->is_alive[i] = 0;
        }
        c3d->positionsX[i] = x;
        c3d->positionsY[i] = y;
        c3d->positionsZ[i] = z;

        i++;
      }
    }
  }
}

// TODO: use a compute shader instead, if needed (since OpenGL 4.3)
// possibly binding the 2 SSBOs and call glDispatchCompute and glMemoryBarrier
// or paralellize on the CPU side
void EvolveGOL2D_NextGeneration(Cells2D *outC2d, const Cells2D *inC2d) {
  for (int i = 0; i < CELL_COUNT; i++) {
    int neighbours = __GOL2DCheckNeighbours(inC2d, i);
    bool isAlive = inC2d->is_alive[i];

    if (isAlive) {
      // under or overpopulation when < underpop && > overpop
      // lives on when == underpop && == overpop
      outC2d->is_alive[i] = neighbours >= UNDERPOPULATION_UPPER_CAP_2D &&
                            neighbours <= OVERPOPULATION_UPPER_CAP_2D;
    } else {
      // reproduction
      outC2d->is_alive[i] = neighbours == OVERPOPULATION_UPPER_CAP_2D;
    }
  }
}

/**
 * Counts the neighbours for the given cell.
 * Every cell interacts with its eight neighbours, which are the cells that
 * are horizontally, vertically, or diagonally adjacent.
 *
 * @return int: the number of neighbours
 */
static int __GOL2DCheckNeighbours(Cells2D *inC2d, int i) {
  int neighbours = 0;
  int arraySize = (CELL_COUNT - 1);

  /*
   * Top 9 cubes that are above the current one
   */
  const int TOP_INDEXES_3D[] = {};
  /*
   * Bottom 9 cubes that are below the current one
   */
  const int BOTTOM_INDEXES_3D[] = {};
  /*
   * The 8 cubes that alongside the current cube's height
   */
  const int SIDE_INDEXES_3D[] = {};

  for (int j = 0; j < CELL_NEIGHBOUR_SIZE; j++) {
    int relativeIdx = i + DIAGONAL_INDEXES_2D[j];
    if (relativeIdx >= 0 && relativeIdx <= arraySize &&
        inC2d->is_alive[relativeIdx]) {
      // the relative diagonal cell
      neighbours++;
    }
  }

  for (int j = 0; j < CELL_NEIGHBOUR_SIZE; j++) {
    int relativeIdx = i + ADJECENT_INDEXES_2D[j];
    if (relativeIdx >= 0 && relativeIdx <= arraySize &&
        inC2d->is_alive[relativeIdx]) {
      // the adjecent diagonal cell
      neighbours++;
    }
  }

  return neighbours;
}

// TODO: It should receive the exact ruleset as well (e.g. R(4555) or
// R(5766)) NOTE: Since C11 it would be possible to use threads.h, but it
// would add more overhead etc., so the Windows API version is used instead
void EvolveGOL3D_NextGeneration(Arena *frame3DArena, Cells3D *outC3d,
                                const Cells3D *inC3d) {

  HANDLE threads[numOfProcessors];
  Evolve3DThreadCells *allThreadCells = Arena_AllocAligned(
      frame3DArena, numOfProcessors * sizeof(Evolve3DThreadCells),
      DEFAULT_ARENA_ALIGNMENT);

  int chunkSizePerThread = CUBE_COUNT / numOfProcessors;

  for (int i = 0; i < numOfProcessors; i++) {
    allThreadCells[i] = (Evolve3DThreadCells){
        .inC3d = inC3d,
        .outC3d = outC3d,
        .startIdx = i * chunkSizePerThread,
        // get the next chunk based on the thread size and for the final thread,
        // process all remaining cells
        .endIdx = (i + 1) * chunkSizePerThread};
    if (i == numOfProcessors - 1) {
      allThreadCells[i].endIdx = CUBE_COUNT;
    }

    threads[i] =
        CreateThread(THREAD_DEFAULT_SEC_ATTRIBUTES, THREAD_DEFAULT_STACK_SIZE,
                     __GOL3D_NextGenerationMultiThread, &allThreadCells[i],
                     THREAD_DEFAULT_CREATION_FLAGS, THREAD_DEFAULT_THREAD_ID);

    if (threads[i] == NULL) {
      // TODO: handle thread creation failure properly
      printf("Thread creation failed");
    }
  }

  WaitForMultipleObjects(numOfProcessors, threads, THREAD_DEFAULT_WAIT_FOR_ALL,
                         THREAD_DEFAULT_WAIT_MS);

  for (int i = 0; i < numOfProcessors; i++) {
    CloseHandle(threads[i]);
  }
}

static void
__GOL3D_NextGenerationMultiThread(Evolve3DThreadCells *threadCells) {
  for (int i = threadCells->startIdx; i < threadCells->endIdx; i++) {
    int neighbours = __GOL3DCheckNeighbours(threadCells->inC3d, i);
    bool isCurrentAlive = threadCells->inC3d->is_alive[i];

    if (isCurrentAlive) {
      // under or overpopulation when < underpop && > overpop
      // lives on when == underpop && == overpop
      threadCells->outC3d->is_alive[i] =
          neighbours >= UNDERPOPULATION_UPPER_CAP_3D &&
          neighbours <= OVERPOPULATION_UPPER_CAP_3D;
    } else {
      // reproduction
      threadCells->outC3d->is_alive[i] =
          neighbours == OVERPOPULATION_UPPER_CAP_3D;
    }
  }
}

static int __GOL3DCheckNeighbours(Cells3D *inC3d, int i) {
  int neighbours = 0;

  for (int j = 0; j < CUBE_NEIGHBOUR_SIZE; j++) {
    int relativeIdx = i + NEIGHBOUR_INDEXES_3D[j];
    if (relativeIdx >= 0 && relativeIdx < CUBE_COUNT &&
        inC3d->is_alive[relativeIdx]) {
      neighbours++;
    }
  }

  return neighbours;
}