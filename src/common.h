#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>

// Can be passed as a function pointer to do no operations.
void noOp();

inline const char *boolToString(bool value);
inline const char *boolToOnOff(bool value);
inline const char *boolToYesNo(bool value);

#endif