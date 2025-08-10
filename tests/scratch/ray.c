#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>
#include "render/rlights.h"

float deltaTime = 0.0F;

float renderSpeed = 0.4f;

void can_render_cubes() {
  InitWindow(640, 480, "can_render_cubes");

  Camera3D camera = {0};
  camera.position = (Vector3){.x = 10.0F, .y = 10.0F, .z = 10.0F};
  camera.target = (Vector3){.x = 0.0F, .y = 0.0F, .z = 0.0F};
  camera.up = (Vector3){.x = 0.0F, .y = 1.0F, .z = 0.0F};
  camera.fovy = 90.0F;
  camera.projection = CAMERA_PERSPECTIVE;

  float cubeSize = 1.0F; // 12 uniformly sized edges
  Mesh cube = GenMeshCube(cubeSize, cubeSize, cubeSize);

  Shader shader = LoadShader("resources/shaders/lighting_instancing.vs",
                             "resources/shaders/lighting.fs");

  if (!IsShaderValid(shader)) {
    printf("Invalid shader: %d\n", shader.id);
    exit(1);
  }

  // TODO: take a look on what these actually do
  // model-view-projection
  shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
  shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
  shader.locs[SHADER_LOC_MATRIX_MODEL] =
      GetShaderLocationAttrib(shader, "instanceTransform");

  int ambientLoc = GetShaderLocation(shader, "ambient");
  SetShaderValue(shader, ambientLoc, (float[4]){0.2f, 0.2f, 0.2f, 1.0F},
                 SHADER_UNIFORM_VEC4);

  CreateLight(LIGHT_DIRECTIONAL, (Vector3){50.0F, 50.0F, 0.0F},
              (Vector3){0.0F, 0.0F, 0.0F}, WHITE, shader);

  Material matInstances = LoadMaterialDefault();
  matInstances.shader = shader;
  matInstances.maps[MATERIAL_MAP_DIFFUSE].color = RED;

  Material matWhite = LoadMaterialDefault();
  matWhite.maps[MATERIAL_MAP_DIFFUSE].color = WHITE;

  Material matRed = LoadMaterialDefault();
  matRed.maps[MATERIAL_MAP_DIFFUSE].color = RED;

  Material matGreen = LoadMaterialDefault();
  matGreen.maps[MATERIAL_MAP_DIFFUSE].color = GREEN;

  Material matBlue = LoadMaterialDefault();
  matBlue.maps[MATERIAL_MAP_DIFFUSE].color = BLUE;

  const int numToDraw = 4;
  // Matrix *transforms = (Matrix *)RL_CALLOC(
  //     numToDraw,
  //     sizeof(Matrix)); // Pre-multiplied transformations passed to rlgl

  // ---------- translation part ----------
  // OpenGL is using a right-handed coordinate system in object space and
  // left-handed in window/screen space
  // The below is right-handed, since it's object space

  // origin
  Matrix translation0 = MatrixTranslate(0.0F, 0.0F, 0.0F);

  // x is vertical (positive is right, negative is left)
  Matrix translationX1 = MatrixTranslate(1.0F, 0.0F, 0.0F);
  Matrix translationX2 = MatrixTranslate(3.0F, 0.0F, 0.0F);
  Matrix translationX3 = MatrixTranslate(-6.0F, 0.0F, 0.0F);

  // y is horizonal (positive is up, negative is down)
  Matrix translationY1 = MatrixTranslate(0.0F, 1.0F, 0.0F);
  Matrix translationY2 = MatrixTranslate(0.0F, 3.0F, 0.0F);
  Matrix translationY3 = MatrixTranslate(0.0F, -6.0F, 0.0F);

  // z depth, positive is nearer, negative is further
  Matrix translationZ1 = MatrixTranslate(0.0F, 0.0F, 1.0F);
  Matrix translationZ2 = MatrixTranslate(0.0F, 0.0F, 3.0F);
  Matrix translationZ3 = MatrixTranslate(0.0F, 0.0F, -6.0F);

  // transforms[0] = translation0;
  // transforms[1] = translation1;
  // transforms[2] = translation2;
  // transforms[3] = translation3;

  // ---------------------------------------
  while (!WindowShouldClose()) {
    UpdateCamera(&camera, CAMERA_FREE);
    float cameraPos[3] = {camera.position.x, camera.position.y,
                          camera.position.z};
    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos,
                   SHADER_UNIFORM_VEC3);

    int deltaTimeLoc = GetShaderLocation(shader, "deltaTime");
    SetShaderValue(shader, deltaTimeLoc, &deltaTime, SHADER_UNIFORM_FLOAT);
    // TOOD: update the light shader here, if needed

    // TODO: decide on at what time should we start drawing
    BeginDrawing();
    ClearBackground(DARKBLUE);

    BeginMode3D(camera);

    DrawMesh(cube, matWhite, translation0);

    DrawMesh(cube, matRed, translationX1);
    DrawMesh(cube, matRed, translationX2);

    translationX3.m12 = -6.0F; // m12 controls the X coordinate
    DrawMesh(cube, matRed, translationX3);

    DrawMesh(cube, matBlue, translationY1);
    DrawMesh(cube, matBlue, translationY2);

    translationY3.m13 = -6.0F; // m13 controls the Y coordinate
    DrawMesh(cube, matBlue, translationY3);

    DrawMesh(cube, matGreen, translationZ1);
    DrawMesh(cube, matGreen, translationZ2);

    // if (deltaTime >= renderSpeed) {
    //   deltaTime = 0;
    // } else {
    // }
    deltaTime += GetFrameTime();
    float z3Value = sin(deltaTime) * -6.0F;
    // clamp between 0.0F and -6.0F
    if (z3Value > 0.0F) {
      z3Value = 0.0F;
    }
    translationZ3.m14 = z3Value; // m14 controls the Z coordinate

    DrawMesh(cube, matGreen, translationZ3);

    // DrawMeshInstanced(cube, matInstances, transforms, 10);

    DrawGrid(100, 1.0F);

    EndMode3D();
    EndDrawing();
  }

  CloseWindow();
}

int main() {
  can_render_cubes();
  // TODO: add more calls, as needed
  return 0;
}