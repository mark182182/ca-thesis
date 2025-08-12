#include <assert.h>
#include "test_util.h"

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
