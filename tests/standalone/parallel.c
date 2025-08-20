#include <stdio.h>
#include <assert.h>
#include "../test_util.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

void can_use_multiple_threads_under_windows() {
#if defined(_WIN32) || defined(_WIN64)
  printf("Hello from win");
#else
  printf("This test can only be run on Windows machines.");
  exit(1);
#endif
}

int main() {
  TestCase tests[] = {{.name = "can_use_multiple_threads_under_windows",
                       .testFn = can_use_multiple_threads_under_windows}};

  int testsLength = sizeof(tests) / sizeof(tests[0]);
  runTests(__FILE__, tests, testsLength);
  return 0;
}