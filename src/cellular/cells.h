#ifndef CELLS_H
#define CELLS_H

#include "stdint.h"
#include "const.h"

typedef enum Cells2DParams {
  CELL_HEIGHT_RATIO = 8, // power of two is recommended
  CELL_WIDTH_RATIO = 8,  // power of two is recommended
  CELL_HEIGHT_SIZE = SCREEN_HEIGHT / CELL_HEIGHT_RATIO,
  CELL_WIDTH_SIZE = SCREEN_WIDTH / CELL_WIDTH_RATIO,
  CELL_COUNT = CELL_HEIGHT_SIZE * CELL_WIDTH_SIZE,
  CELL_INITIAL_GRID_DENSITY = 4, // number of cells cells are alive at start,
                                 // when the grid is randomized, lower is higher
  CELL_NEIGHBOUR_SIZE = 4        // Moore neighbourhood
} Cells2DParams;

typedef enum Cells3DParams {
  // using power of 2 would be recommended
  MAX_CUBES_X = 16,
  MAX_CUBES_Y = 16,
  MAX_CUBES_Z = 16,
  CUBE_COUNT = MAX_CUBES_X * MAX_CUBES_Y * MAX_CUBES_Z,
  CUBE_INITIAL_GRID_DENSITY = 16,
  // the 3D Moore neighbourhood consits of the 26 neighbouring cubes
  TOP_NEIGHBOUR_SIZE = 9,
  BOTTOM_NEIGHBOUR_SIZE = 9,
  SIDE_NEIGHBOUR_SIZE = 8
} Cells3DParams;

extern float CUBE_SIZE;

// Moore neighborhood
extern const int DIAGONAL_INDEXES_2D[];
extern const int ADJECENT_INDEXES_2D[];

extern const int TOP_INDEXES_3D[];
extern const int BOTTOM_INDEXES_3D[];
extern const int SIDE_INDEXES_3D[];

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
  bool *is_alive;
  int *positionsX;
  int *positionsY;
  Color **colors;
} Cells2D;

typedef struct Cells3D {
  // TODO: Before trying to re-creating the same logic what the render 2d is
  // doing with Cells2D, take a look at what instanced 3d rendering requires
  bool *is_alive;
  // NOTE: might want to use float here for the positions
  int *positionsX;
  int *positionsY;
  int *positionsZ;

  int aliveCells;
} Cells3D;

void Cells2D_InitArraysBasedOnCellSize(Arena *genArena, Cells2D *c2d);
void Cells3D_InitArraysBasedOnCellSize(Arena *genArena, Cells3D *c3d);

#endif