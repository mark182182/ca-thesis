#ifndef RAYLIB_SHIM
#define RAYLIB_SHIM

// since raylib.h and windows.h clashes with some names, we have to prefix
// the raylib identifiers that are used in the code and are clasing with
// windows.h

// NOTE: if one has to use e.g. a function that is defined with the same name in
// windows.h and raylib, more steps would be required to make it work

#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define NOMINMAX
#define NOGDI
#include <windows.h>

#ifdef Rectangle
#undef Rectangle
#endif

#ifdef CloseWindow
#undef CloseWindow
#endif

#ifdef ShowCursor
#undef ShowCursor
#endif

#ifdef LoadImage
#undef LoadImage
#endif

#ifdef DrawText
#undef DrawText
#endif

#ifdef DrawTextEx
#undef DrawTextEx
#endif

#endif

#define CloseWindow RaylibCloseWindow
#define ShowCursor RaylibShowCursor

// need to include these calls in order to have the renamed symbol defined for
// linking
static inline void RaylibCloseWindow(void) { CloseWindow(); }
static inline void RaylibShowCursor(void) { ShowCursor(); }

#include <raylib.h>
#include <raymath.h>

#endif