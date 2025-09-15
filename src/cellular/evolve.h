/**
 * Evolve one, two and three-dimensional cellular automata.
 *
 * The rendering is expected to be done in 2D or 3D.
 *
 */

#ifndef EVOLVE_H
#define EVOLVE_H

#include "const.h"
#include "cells.h"

// 8 bits representing all one-dimensional cellular automata rules
extern int rule1DBits;

typedef enum Evolve2DParams {
  UNDERPOPULATION_UPPER_CAP_2D = 2,
  OVERPOPULATION_UPPER_CAP_2D = 3
} Evolve2DParams;

// TODO: hard-coded rule 4555 would need to be changed to use dynamic rulesets
// instead
typedef enum Evolve3DParams {
  UNDERPOPULATION_UPPER_CAP_3D = 4,
  OVERPOPULATION_UPPER_CAP_3D = 5
} Evolve3DParams;

/*
 * The current thread data for processing the next generation.
 * Overlapping indexes are not permitted, one thread can only process one
 * continuous range at a time.
 */
typedef struct Evolve3DThreadCells {
  Cells3D *outC3d;
  Cells3D *inC3d;

  HANDLE startEvent;
  HANDLE doneEvent;
  int startIdx;
  int endIdx;
} Evolve3DThreadCells;

/*
 * Generate initial values (e.g. position, color) for each cells in the
 * grid, with optionally setting the alive state at random.
 */
void Evolve2D_InitializeCells(Cells2D *c2d, bool randomizeAlive,
                              int randGridDensity);
/*
 *Same as in 2D
 */
void Evolve3D_InitializeCells(Cells3D *c3d, bool randomizeAlive,
                              int randGridDensity);

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
static int __GOL2DCheckNeighbours(Cells2D *inC2d, int i);

/**
 * Create the thread cells before creating the threads for the next generation;
 */
Evolve3DThreadCells *EvolveGOL3D_CreateThreadCells(Arena *mode3DArena);

/**
 * Create threads before running the next generation.
 */
HANDLE *EvolveGOL3D_CreateNextGenThreads(Evolve3DThreadCells *allThreadCells);

/**
 * Evolve the given partitioned space to the next generation for 3D rendering.
 *
 * The default rules for 2D can be applied, but in 3D there are many more
 * neighbours, so most configurations quickly die out. Because of this, the
 * rules that are implemented are usually are different for Game of Life in 3D.
 *
 * These were most notably researched by Carter Bays, which is generally called
 * "3D Life". These are different rulesets which support more stable and complex
 * patterns that don't go extinct that rapidly.
 *
 * From his work "Candidates for the Game of Life in Three Dimensions" the
 * rulesets are defined by the number of minimum and maximum neighbours that a
 * living cell needs to survive El≤E≤Eu and the number of minimum and maximum
 * neighbours that a living cell needs to reproduce is Fl≤F≤Fu. This is usually
 * written in the form of R(El, Eu, Fl, Fu).
 *
 * Examples:
 * Rule R = (5766): A living cell survives if it has 5, 6, or 7 neighbours. It
 * dies if it has fewer than 5 or more than 7. A dead cell becomes alive if it
 * has 6 neighbours.
 *
 * Rule R = (4555): A living cell survives if it has 4 or 5 neighbours. It dies
 * if it has fewer than 4 or more than 5. A
 * dead cell becomes alive if it has 5 neighbours.
 */
void EvolveGOL3D_NextGeneration(Evolve3DThreadCells *allThreadCells,
                                Cells3D *outC3d, Cells3D *inC3d);

static void __GOL3D_NextGenerationMultiThread(Evolve3DThreadCells *threadCells);
static int __GOL3DCheckNeighbours(Cells3D *inC3d, int i);

/**
 * TODO: Do this after 3D
 */
void EvolveCA1D_NextGeneration(Cells2D *outC2d, Cells2D *inC2d);

#endif
