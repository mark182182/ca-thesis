#ifndef CELLS_H
#define CELLS_H

#include "stdint.h"
#include "const.h"

enum {
  CELL_HEIGHT_RATIO = 8, // power of two is recommended
  CELL_WIDTH_RATIO = 8,  // power of two is recommended
  CELL_HEIGHT_SIZE = SCREEN_HEIGHT / CELL_HEIGHT_RATIO,
  CELL_WIDTH_SIZE = SCREEN_WIDTH / CELL_WIDTH_RATIO,
  CELL_COUNT = CELL_HEIGHT_SIZE * CELL_WIDTH_SIZE,
  CELL_INITIAL_GRID_DENSITY = 4, // number of cells cells are alive at start,
                                 // when the grid is randomized, lower is higher
  CELL_NEIGHBOUR_SIZE = 4        // Moore neighbourhood
};

extern const int DIAGONAL_INDEXES_2D[];
extern const int ADJECENT_INDEXES_2D[];

// NOTE: Having a bool in a single struct is totally unnecessary
typedef struct Cell {
  bool is_alive; // populated or unpopulated
} Cell;

/*
 * SOA based container for cells to be drawn on the screen in 2D. The cells are
 * stored in height by width in one continous memory block, from top to bottom.
 *
 * NOTE: Whenever the fields change, the doubleGenStorage size has to
 * change as well (see formula in arena.h)
 *
 * The actual cell values (0 or 1) represent the populated/unpopulated cells.
 * E.g. alignment:
 *   x012
 * y ____
 * 0|1100|
 * 1|0111|
 * 2|1001|
 * ........
 */
typedef struct Cells2D {
  Cell *cells;
  int *positionsX;
  int *positionsY;
  Color **colors;
} Cells2D;

void Cells2D_InitArraysBasedOnCellSize(Arena *genArena, Cells2D *c2d);

typedef struct Cells3D {
  // TODO: Before trying to re-creating the same logic what the render 2d is doing with Cells2D, take a look at what instanced 3d rendering requires
} Cells3D;

#endif