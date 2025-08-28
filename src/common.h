#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include "raylib_shim.h"

// Can be passed as a function pointer to do no operations.
void noOp();

inline const char *boolToString(bool value);
inline const char *boolToOnOff(bool value);
inline const char *boolToYesNo(bool value);

Vector2 Vector2_Zeroed();
Vector3 Vector3_Zeroed();
Vector4 Vector4_Zeroed();

#endif