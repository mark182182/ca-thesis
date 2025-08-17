#include "evolve.h"
#include "const.h"
#include "cells.h"
#include <raylib.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <immintrin.h>
#include "dstructs/arena.h"
#include <assert.h>
#include <stdbool.h>

float CUBE_SIZE = 1.0F;  // 12 uniformly sized edges
float CUBE_SCALE = 0.5F; // scale size for the default Raylib mesh to be used in
                         // a scaling matrix

/*
 * Diagonals relative to the current cell, e.g. if the ratio is 10:
 * top-left  bottom-left bottom-right    top-right
 * -72 - 1   -72 + 1     +72 - 1         +72 + 1
 */
const int DIAGONAL_INDEXES_2D[] = {(-1 * CELL_HEIGHT_SIZE) - 1,
                                   (-1 * CELL_HEIGHT_SIZE) + 1,
                                   CELL_HEIGHT_SIZE - 1, CELL_HEIGHT_SIZE + 1};

/*
 * Adjecent relative idx to the current cell, e.g. if the ratio is 10:
 * top   bottom  left    right
 * -1    +1      -72     +72
 */
const int ADJECENT_INDEXES_2D[] = {-1, +1, (-1 * CELL_HEIGHT_SIZE),
                                   CELL_HEIGHT_SIZE};
/*
 * Top 9 cubes that are above the current one
 */
const int TOP_INDEXES_3D[] = {-7, -6, -5, 2, 3, 4, 11, 12, 13};
/*
 * Bottom 9 cubes that are below the current one
 */
const int BOTTOM_INDEXES_3D[] = {-13, -12, -11, -4, -3, -2, 5, 6, 7};
/*
 * The 8 cubes that alongside the current cube's height
 */
const int SIDE_INDEXES_3D[] = {-10, -9, -8, -1, 1, 8, 9, 10};

void Cells_CalcNeighbourOffsets2D() {
  // TODO: finish this
}

void Cells_CalcNeighbourOffsets3D(int *neighborsToFill, int maxCubesX,
                                  int maxCubesY, int maxCubesZ) {
  // calc offsets
  int idx = 0;

  for (int x = -1; x <= 1; x++) {
    for (int y = -1; y <= 1; y++) {
      for (int z = -1; z <= 1; z++) {
        if (x == 0 && y == 0 && z == 0) {
          continue; // ignore self from neighbour offsets
        }

        // formula=xOffset*MAX_Y*MAX_Z+yOffset*MAX_Z+zOffset
        int offset = x * MAX_CUBES_Y * MAX_CUBES_Z // the current offset in x
                     + y * MAX_CUBES_Z             // the current offset in y
                     + z;                          // remaining z offset

        // Calculate 1D offset directly
        neighborsToFill[idx++] = offset;
      }
    }
  }
}

void Cells2D_InitArraysBasedOnCellSize(Arena *arena, Cells2D *c2d) {
  c2d->is_alive = Arena_AllocAlignedZeroed(arena, CELL_COUNT * sizeof(bool),
                                           DEFAULT_ARENA_ALIGNMENT);

  c2d->positionsX = Arena_AllocAlignedZeroed(arena, CELL_COUNT * sizeof(int),
                                             DEFAULT_ARENA_ALIGNMENT);

  c2d->positionsY = Arena_AllocAlignedZeroed(arena, CELL_COUNT * sizeof(int),
                                             DEFAULT_ARENA_ALIGNMENT);

  c2d->colors = Arena_AllocAlignedZeroed(arena, CELL_COUNT * sizeof(Color *),
                                         DEFAULT_ARENA_ALIGNMENT);
}

void Cells3D_InitArraysBasedOnCellSize(Arena *arena, Cells3D *c3d) {
  c3d->is_alive = Arena_AllocAlignedZeroed(arena, CELL_COUNT * sizeof(bool),
                                           DEFAULT_ARENA_ALIGNMENT);

  c3d->positionsX = Arena_AllocAlignedZeroed(arena, CELL_COUNT * sizeof(int),
                                             DEFAULT_ARENA_ALIGNMENT);

  c3d->positionsY = Arena_AllocAlignedZeroed(arena, CELL_COUNT * sizeof(int),
                                             DEFAULT_ARENA_ALIGNMENT);

  c3d->positionsZ = Arena_AllocAlignedZeroed(arena, CELL_COUNT * sizeof(int),
                                             DEFAULT_ARENA_ALIGNMENT);
}