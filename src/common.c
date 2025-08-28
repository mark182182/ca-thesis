#include "common.h"
#include <stdbool.h>
#include "raylib_shim.h"

void noOp() {
  // no operation
};

const char *boolToString(bool value) { return value ? "true" : "false"; }

const char *boolToOnOff(bool value) { return value ? "ON" : "OFF"; }

const char *boolToYesNo(bool value) { return value ? "Yes" : "No"; }

Vector2 Vector2_Zeroed() { return (Vector2){.x = 0, .y = 0}; }
Vector3 Vector3_Zeroed() { return (Vector3){.x = 0, .y = 0, .z = 0}; }
Vector4 Vector4_Zeroed() { return (Vector4){.x = 0, .y = 0, .z = 0, .w = 0}; }