/**
 * Evolve one, two and three-dimensional cellular automata.
 *
 * The rendering is expected to be done in 2D or 3D.
 *
 */

#ifndef EVOLVE_H
#define EVOLVE_H

#include <raylib.h>
#include "const.h"
#include "cells.h"

// 8 bits representing all one-dimensional cellular automata rules
extern int rule1DBits;

/*
 * Generate initial values (e.g. position, color) for each cells in the grid,
 * with optionally setting the alive state at random.
 */
void Evolve2D_InitializeCells(Cells2D *c2d, bool randomizeAlive);
/*
 *Same as in 2D
 */
void Evolve3D_InitializeCells(Cells3D *c3d, bool randomizeAlive);

/*
 * Evolves the given grid to the next generation for 2D rendering.
 *
 * Ruleset (from Wikipedia):
 * https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life): "Any live cell with
 * fewer than two live neighbours dies, as if by underpopulation. Any live
 * cell with two or three live neighbours lives on to the next generation. Any
 * live cell with more than three live neighbours dies, as if by
 * overpopulation. Any dead cell with exactly three live neighbours becomes a
 * live cell, as if by reproduction."
 *
 */
void EvolveGOL2D_NextGeneration(Cells2D *outC2d, const Cells2D *inC2d);
int __GOL2DCheckNeighbours(Cells2D *inC2d, int i);

// void EvolveGOL3D_NextGeneration(Cells3D *outC3d, const Cells2D *inC3d);

/**
 * TODO: Do this after 3D
 */
void EvolveCA1D_NextGeneration(Cells2D *outC2d, Cells2D *inC2d);

#endif
