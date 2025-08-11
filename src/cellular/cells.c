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

float CUBE_SIZE = 1.0f; // 12 uniformly sized edges

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
const int TOP_INDEXES_3D[] = {};
/*
 * Bottom 9 cubes that are below the current one
 */
const int BOTTOM_INDEXES_3D[] = {};
/*
 * The 8 cubes that alongside the current cube's height
 */
const int SIDE_INDEXES_3D[] = {};

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