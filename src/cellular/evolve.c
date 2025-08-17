#include "evolve.h"
#include "const.h"
#include "cells.h"
#include <stdlib.h>

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

// TODO: use a compute shader instead (since OpenGL 4.3)
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
int __GOL2DCheckNeighbours(Cells2D *inC2d, int i) {
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

// TODO: It should receive the exact ruleset as well (e.g. R(4555) or R(5766))
void EvolveGOL3D_NextGeneration(Cells3D *outC3d, const Cells3D *inC3d) {
  for (int i = 0; i < CUBE_COUNT; i++) {
    int neighbours = __GOL3DCheckNeighbours(inC3d, i);
    bool isCurrentAlive = inC3d->is_alive[i];

    if (isCurrentAlive) {
      // under or overpopulation when < underpop && > overpop
      // lives on when == underpop && == overpop
      outC3d->is_alive[i] = neighbours >= UNDERPOPULATION_UPPER_CAP_3D &&
                            neighbours <= OVERPOPULATION_UPPER_CAP_3D;
    } else {
      // reproduction
      outC3d->is_alive[i] = neighbours == OVERPOPULATION_UPPER_CAP_3D;
    }
  }
}

int __GOL3DCheckNeighbours(Cells3D *inC3d, int i) {
  // TODO: finish this

  // TODO: use Cells_CalcNeighbourOffsets3D instead
  int offsets[26];
  int idx = 0;

  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      for (int dz = -1; dz <= 1; dz++) {
        if (dx == 0 && dy == 0 && dz == 0)
          continue; // Skip self

        // Calculate 1D offset directly
        int offset = dy * (MAX_CUBES_X * MAX_CUBES_Z) + dx * MAX_CUBES_Z + dz;
        offsets[idx++] = offset;
      }
    }
  }

  // calc neighbours
  int neighbours = 0;

  for (int j = 0; j < 26; j++) {
    int relativeIdx = i + offsets[j];
    if (relativeIdx >= 0 && relativeIdx < CUBE_COUNT &&
        inC3d->is_alive[relativeIdx]) {
      neighbours++;
    }
  }

  return neighbours;
  // int neighbours = 0;
  // int arraySize = (CUBE_COUNT - 1);

  // for (int j = 0; j < TOP_NEIGHBOUR_SIZE; j++) {
  //   int relativeIdx = i + TOP_INDEXES_3D[j];
  //   if (relativeIdx >= 0 && relativeIdx <= arraySize &&
  //       inC3d->is_alive[relativeIdx]) {
  //     // the cube at the top
  //     neighbours++;
  //   }
  // }

  // for (int j = 0; j < BOTTOM_NEIGHBOUR_SIZE; j++) {
  //   int relativeIdx = i + BOTTOM_INDEXES_3D[j];
  //   if (relativeIdx >= 0 && relativeIdx <= arraySize &&
  //       inC3d->is_alive[relativeIdx]) {
  //     // the cube at the bottom
  //     neighbours++;
  //   }
  // }

  // for (int j = 0; j < SIDE_NEIGHBOUR_SIZE; j++) {
  //   int relativeIdx = i + SIDE_INDEXES_3D[j];
  //   if (relativeIdx >= 0 && relativeIdx <= arraySize &&
  //       inC3d->is_alive[relativeIdx]) {
  //     // the cube at the side
  //     neighbours++;
  //   }
  // }

  return neighbours;
}