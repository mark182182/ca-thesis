#include "common.h"
#include <stdbool.h>

void noOp() {
  // no operation
};

const char *boolToString(bool value) { return value ? "true" : "false"; }

const char *boolToOnOff(bool value) { return value ? "ON" : "OFF"; }

const char *boolToYesNo(bool value) { return value ? "Yes" : "No"; }