#ifndef LIGHT_H
#define LIGHT_H

#include "raylib_shim.h"

typedef struct Light {
  Vector3 position;
  Vector3 target;
  Color color;
  // dimming of light over distance
  float attenuation;
} Light;

#endif