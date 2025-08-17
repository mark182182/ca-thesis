#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "test_util.h"
#include "cellular/cells.h"

void can_calc_neighbour_offsets_2d() { Cells_CalcNeighbourOffsets2D(); }

void can_calc_neighbour_offsets_3d() {
  int neighborsToFill[CUBE_NEIGHBOUR_SIZE];
  int maxCubesX = 3;
  int maxCubesY = 3;
  int maxCubesZ = 3;

  Cells_CalcNeighbourOffsets3D(neighborsToFill, maxCubesX, maxCubesY,
                               maxCubesZ);

  // assert the expected cells and indexes

  assert("foo" == "bar");

  // NOTE: these were previously calculated in the scratch/ray.c test
  /*
   * Top 9 cubes that are above the current one
   */
  const int topIndexes[] = {-7, -6, -5, 2, 3, 4, 11, 12, 13};
  /*
   * Bottom 9 cubes that are below the current one
   */
  const int bottomIndexes[] = {-13, -12, -11, -4, -3, -2, 5, 6, 7};
  /*
   * The 8 cubes that alongside the current cube's height
   */
  const int sideIndexes[] = {-10, -9, -8, -1, 1, 8, 9, 10};

  bool foundIndexes[CUBE_NEIGHBOUR_SIZE];
  for (int i = 0; i < CUBE_NEIGHBOUR_SIZE; i++) {
    int offsetIndex = neighborsToFill[i];
    for (int j = 0; i < 9; i++) {
      if (offsetIndex == topIndexes[j]) {
        foundIndexes[i] = offsetIndex;
      }
      if (offsetIndex == bottomIndexes[j]) {
        foundIndexes[i] = offsetIndex;
      }

      if (i < 8) {
        if (offsetIndex == sideIndexes[j]) {
          foundIndexes[i] = offsetIndex;
        }
      }
    }
  }

  int actualFoundIndexesCount = sizeof(foundIndexes) / sizeof(foundIndexes[0]);
  char *expectedIndexesMsg[64];

  snprintf(expectedIndexesMsg, sizeof(expectedIndexesMsg),
           "Expected %d indexes, but got %d", CUBE_NEIGHBOUR_SIZE,
           actualFoundIndexesCount);

  assert(actualFoundIndexesCount == CUBE_NEIGHBOUR_SIZE && expectedIndexesMsg);

  memset(neighborsToFill, 0, sizeof(int) * CUBE_NEIGHBOUR_SIZE);
  maxCubesX = 10;
  maxCubesY = 10;
  maxCubesZ = 10;

  Cells_CalcNeighbourOffsets3D(neighborsToFill, maxCubesX, maxCubesY,
                               maxCubesZ);

  // these neighbour offsets can be later offsetted by the actual index of the
  // cube that we are checking the neighbours for

  assert("foo" == "bar");
}

void can_evolve_next_generation_3d() {
  // TODO: this
}

int main() {
  TestCase tests[] = {{.name = "can_evolve_next_generation_3d",
                       .testFn = can_evolve_next_generation_3d}};

  int testsLength = sizeof(tests) / sizeof(tests[0]);
  runTests(__FILE__, tests, testsLength);

  return 0;
}
