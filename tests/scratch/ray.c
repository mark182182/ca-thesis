#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "dstructs/arena.h"
#include "cellular/cells.h"
#include "cellular/evolve.h"

float deltaTime = 0.0F;

float renderSpeed = 0.4f;

static const size_t arenaStorageSize = 16 * 1024 * 1024;
static uint8_t arenaStorage[arenaStorageSize];

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

  Material matWhite = LoadMaterialDefault();
  matWhite.maps[MATERIAL_MAP_DIFFUSE].color = WHITE;

  Material matRed = LoadMaterialDefault();
  matRed.maps[MATERIAL_MAP_DIFFUSE].color = RED;

  Material matGreen = LoadMaterialDefault();
  matGreen.maps[MATERIAL_MAP_DIFFUSE].color = GREEN;

  Material matBlue = LoadMaterialDefault();
  matBlue.maps[MATERIAL_MAP_DIFFUSE].color = BLUE;

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

    DrawGrid(100, 1.0F);

    EndMode3D();
    EndDrawing();
  }

  CloseWindow();
}

void can_identify_moore_neighbours_3d() {
  InitWindow(640, 480, "can_identify_moore_neighbours_3d");

  Camera3D camera = {0};
  camera.position = (Vector3){.x = 10.0F, .y = 10.0F, .z = 10.0F};
  camera.target = (Vector3){.x = 0.0F, .y = 0.0F, .z = 0.0F};
  camera.up = (Vector3){.x = 0.0F, .y = 1.0F, .z = 0.0F};
  camera.fovy = 90.0F;
  camera.projection = CAMERA_PERSPECTIVE;

  float cubeSize = 1.0F; // 12 uniformly sized edges
  Mesh cube = GenMeshCube(cubeSize, cubeSize, cubeSize);

  Material matWhite = LoadMaterialDefault();
  matWhite.maps[MATERIAL_MAP_DIFFUSE].color = WHITE;

  Material matRed = LoadMaterialDefault();
  matRed.maps[MATERIAL_MAP_DIFFUSE].color = RED;

  Material matGreen = LoadMaterialDefault();
  matGreen.maps[MATERIAL_MAP_DIFFUSE].color = GREEN;

  Material matBlue = LoadMaterialDefault();
  matBlue.maps[MATERIAL_MAP_DIFFUSE].color = BLUE;

  // origin
  Matrix translation0 = MatrixTranslate(0.0F, 0.0F, 0.0F);
  /*
   * Top 9 cubes that are above the current one
   */
  Matrix translationTop1 = MatrixTranslate(0.0F, 1.0F, 0.0F);
  Matrix translationTop2 = MatrixTranslate(0.0F, 1.0F, 1.0F);
  Matrix translationTop3 = MatrixTranslate(1.0F, 1.0F, 0.0F);

  Matrix translationTop4 = MatrixTranslate(1.0F, 1.0F, 1.0F);
  Matrix translationTop5 = MatrixTranslate(0.0F, 1.0F, -1.0F);
  Matrix translationTop6 = MatrixTranslate(-1.0F, 1.0F, 0.0F);

  Matrix translationTop7 = MatrixTranslate(-1.0F, 1.0F, -1.0F);
  Matrix translationTop8 = MatrixTranslate(-1.0F, 1.0F, 1.0F);
  Matrix translationTop9 = MatrixTranslate(1.0F, 1.0F, -1.0F);

  /*
   * Bottom 9 cubes that are below the current one
   */
  Matrix translationBottom1 = MatrixTranslate(0.0F, -1.0F, 0.0F);
  Matrix translationBottom2 = MatrixTranslate(0.0F, -1.0F, 1.0F);
  Matrix translationBottom3 = MatrixTranslate(1.0F, -1.0F, 0.0F);

  Matrix translationBottom4 = MatrixTranslate(1.0F, -1.0F, 1.0F);
  Matrix translationBottom5 = MatrixTranslate(0.0F, -1.0F, -1.0F);
  Matrix translationBottom6 = MatrixTranslate(-1.0F, -1.0F, 0.0F);

  Matrix translationBottom7 = MatrixTranslate(-1.0F, -1.0F, -1.0F);
  Matrix translationBottom8 = MatrixTranslate(-1.0F, -1.0F, 1.0F);
  Matrix translationBottom9 = MatrixTranslate(1.0F, -1.0F, -1.0F);

  /*
   * The 8 cubes
   */
  Matrix translationSide1 = MatrixTranslate(1.0F, 0.0F, 0.0F);
  Matrix translationSide2 = MatrixTranslate(0.0F, 0.0F, 1.0F);
  Matrix translationSide3 = MatrixTranslate(1.0F, 0.0F, 1.0F);

  Matrix translationSide4 = MatrixTranslate(-1.0F, 0.0F, 0.0F);
  Matrix translationSide5 = MatrixTranslate(0.0F, 0.0F, -1.0F);
  Matrix translationSide6 = MatrixTranslate(-1.0F, 0.0F, -1.0F);

  Matrix translationSide7 = MatrixTranslate(1.0F, 0.0F, -1.0F);
  Matrix translationSide8 = MatrixTranslate(-1.0F, 0.0F, 1.0F);
  while (!WindowShouldClose()) {
    UpdateCamera(&camera, CAMERA_FREE);
    float cameraPos[3] = {camera.position.x, camera.position.y,
                          camera.position.z};
    BeginDrawing();
    ClearBackground(DARKBLUE);

    BeginMode3D(camera);

    DrawMesh(cube, matWhite, translation0);

    DrawMesh(cube, matRed, translationTop1);
    DrawMesh(cube, matRed, translationTop2);
    DrawMesh(cube, matRed, translationTop3);

    DrawMesh(cube, matRed, translationTop4);
    DrawMesh(cube, matRed, translationTop5);
    DrawMesh(cube, matRed, translationTop6);

    DrawMesh(cube, matRed, translationTop7);
    DrawMesh(cube, matRed, translationTop8);
    DrawMesh(cube, matRed, translationTop9);

    DrawMesh(cube, matGreen, translationBottom1);
    DrawMesh(cube, matGreen, translationBottom2);
    DrawMesh(cube, matGreen, translationBottom3);

    DrawMesh(cube, matGreen, translationBottom4);
    DrawMesh(cube, matGreen, translationBottom5);
    DrawMesh(cube, matGreen, translationBottom6);

    DrawMesh(cube, matGreen, translationBottom7);
    DrawMesh(cube, matGreen, translationBottom8);
    DrawMesh(cube, matGreen, translationBottom9);

    DrawMesh(cube, matBlue, translationSide1);
    DrawMesh(cube, matBlue, translationSide2);
    DrawMesh(cube, matBlue, translationSide3);

    DrawMesh(cube, matBlue, translationSide4);
    DrawMesh(cube, matBlue, translationSide5);
    DrawMesh(cube, matBlue, translationSide6);

    DrawMesh(cube, matBlue, translationSide7);
    DrawMesh(cube, matBlue, translationSide8);

    DrawGrid(100, 1.0F);

    EndMode3D();
    EndDrawing();
  }

  CloseWindow();
}

void can_specify_neighbour_indexes() {
  InitWindow(640, 480, "can_specify_neighbour_indexes");

  Camera3D camera = {0};
  camera.position = (Vector3){.x = 10.0F, .y = 10.0F, .z = 10.0F};
  camera.target = (Vector3){.x = 0.0F, .y = 0.0F, .z = 0.0F};
  camera.up = (Vector3){.x = 0.0F, .y = 1.0F, .z = 0.0F};
  camera.fovy = 90.0F;
  camera.projection = CAMERA_PERSPECTIVE;

  float cubeSize = 1.0F; // 12 uniformly sized edges
  Mesh cube = GenMeshCube(cubeSize, cubeSize, cubeSize);

  Material matWhite = LoadMaterialDefault();
  matWhite.maps[MATERIAL_MAP_DIFFUSE].color = WHITE;

  Material matRed = LoadMaterialDefault();
  matRed.maps[MATERIAL_MAP_DIFFUSE].color = RED;

  Material matGreen = LoadMaterialDefault();
  matGreen.maps[MATERIAL_MAP_DIFFUSE].color = GREEN;

  Material matBlue = LoadMaterialDefault();
  matBlue.maps[MATERIAL_MAP_DIFFUSE].color = BLUE;

  Arena arena = Arena_Init("modeArena", &arenaStorage, arenaStorageSize);

  Cells3D c3d = {0};
  Cells3D_InitArraysBasedOnCellSize(&arena, &c3d);

  int testMaxCubesX = 3;
  int testMaxCubesY = 3;
  int testMaxCubesZ = 3;
  int testCubeCount = testMaxCubesX * testMaxCubesY * testMaxCubesZ;

  int i = 0;
  for (int x = 0; x < testMaxCubesX; x++) {
    for (int y = 0; y < testMaxCubesY; y++) {
      for (int z = 0; z < testMaxCubesZ; z++) {
        c3d.positionsX[i] = x;
        c3d.positionsY[i] = y;
        c3d.positionsZ[i] = z;

        i++;
      }
    }
  }

  /*
    The cube at the center: x=1; y=1; z=1; which is at tIdx=13
    Top indexes to find: x=0,1,2; y=2; z=0,1,2
    Bottom indexes to find: x=0,1,2; y=0; z=0,1,2
    Side indexes to find: x=0,1,2 y=1; z=0,1,2, where x != 1 && y != 1 && z !=1

    c = tIdx: 13; x=1; y=1; z=1, cIdx = c.tIdx

    bottom = tIdx: 0; x=0; y=0; z=0
    bottom = tIdx: 1; x=0; y=0; z=1
    bottom = tIdx: 2; x=0; y=0; z=2
    bottom = tIdx: 9; x=1; y=0; z=0
    bottom = tIdx: 10; x=1; y=0; z=1
    bottom = tIdx: 11; x=1; y=0; z=2
    bottom = tIdx: 18; x=2; y=0; z=0
    bottom = tIdx: 19; x=2; y=0; z=1
    bottom = tIdx: 20; x=2; y=0; z=2
    top = tIdx: 6; x=0; y=2; z=0
    top = tIdx: 7; x=0; y=2; z=1
    top = tIdx: 8; x=0; y=2; z=2
    top = tIdx: 15; x=1; y=2; z=0
    top = tIdx: 16; x=1; y=2; z=1
    top = tIdx: 17; x=1; y=2; z=2
    top = tIdx: 24; x=2; y=2; z=0
    top = tIdx: 25; x=2; y=2; z=1
    top = tIdx: 26; x=2; y=2; z=2
    sides = tIdx: 3; x=0; y=1; z=0
    sides = tIdx: 4; x=0; y=1; z=1
    sides = tIdx: 5; x=0; y=1; z=2
    sides = tIdx: 12; x=1; y=1; z=0
    sides = tIdx: 14; x=1; y=1; z=2
    sides = tIdx: 21; x=2; y=1; z=0
    sides = tIdx: 22; x=2; y=1; z=1
    sides = tIdx: 23; x=2; y=1; z=2

    neighbour tIdx  relativeIdx
    bottom  0 -13, to get this we need to cIdx
    bottom  1 -12
    bottom  2 -11
    bottom  9 -4
    bottom  10  -3
    bottom  11  -2
    bottom  18  5
    bottom  19  6
    bottom  20  7
    top 6 -7
    top 7 -6
    top 8 -5
    top 15  2
    top 16  3
    top 17  4
    top 24  11
    top 25  12
    top 26  13
    sides 3 -10
    sides 4 -9
    sides 5 -8
    sides 12  -1
    sides 14  1
    sides 21  8
    sides 22  9
    sides 23  10
  */

  for (int tIdx = 0; tIdx < testCubeCount; tIdx++) {
    int x = c3d.positionsX[tIdx];
    int y = c3d.positionsY[tIdx];
    int z = c3d.positionsZ[tIdx];

    if (x == 1 && y == 1 && z == 1) {
      printf("c = tIdx: %d; x=%d; y=%d; z=%d\n", tIdx, x, y, z);
    }

    // top
    if ((x == 0 && y == 2 && z == 0) || (x == 0 && y == 2 && z == 1) ||
        (x == 0 && y == 2 && z == 2) || (x == 1 && y == 2 && z == 0) ||
        (x == 1 && y == 2 && z == 1) || (x == 1 && y == 2 && z == 2) ||
        (x == 2 && y == 2 && z == 0) || (x == 2 && y == 2 && z == 1) ||
        (x == 2 && y == 2 && z == 2)) {
      printf("top = tIdx: %d; x=%d; y=%d; z=%d\n", tIdx, x, y, z);
    }

    // bottom
    if ((x == 0 && y == 0 && z == 0) || (x == 0 && y == 0 && z == 1) ||
        (x == 0 && y == 0 && z == 2) || (x == 1 && y == 0 && z == 0) ||
        (x == 1 && y == 0 && z == 1) || (x == 1 && y == 0 && z == 2) ||
        (x == 2 && y == 0 && z == 0) || (x == 2 && y == 0 && z == 1) ||
        (x == 2 && y == 0 && z == 2)) {
      printf("bottom = tIdx: %d; x=%d; y=%d; z=%d\n", tIdx, x, y, z);
    }

    // sides
    if ((x == 0 && y == 1 && z == 0) || (x == 0 && y == 1 && z == 1) ||
        (x == 0 && y == 1 && z == 2) || (x == 1 && y == 1 && z == 0) ||
        (x == 1 && y == 1 && z == 2) || (x == 2 && y == 1 && z == 0) ||
        (x == 2 && y == 1 && z == 1) || (x == 2 && y == 1 && z == 2)) {
      printf("sides = tIdx: %d; x=%d; y=%d; z=%d\n", tIdx, x, y, z);
    }
  }

  printf("foo");

  Matrix *transforms = Arena_AllocAlignedZeroed(
      &arena, testCubeCount * sizeof(Matrix), DEFAULT_MATRIX_ALIGNMENT);

  Shader shader = LoadShader("resources/shaders/lighting_instancing.vs",
                             "resources/shaders/lighting.fs");
  if (!IsShaderValid(shader)) {
    printf("Invalid shader: %d\n", shader.id);
    exit(1);
  }

  // model-view-projection
  // Local/Object to Clip space
  shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
  // World space
  shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
  // Local/Object to World space
  shader.locs[SHADER_LOC_MATRIX_MODEL] =
      GetShaderLocationAttrib(shader, "instanceTransform");

  int ambientLoc = GetShaderLocation(shader, "ambient");
  SetShaderValue(shader, ambientLoc, (float[4]){0.2f, 0.2f, 0.2f, 1.0f},
                 SHADER_UNIFORM_VEC4);

  Material matInstances = LoadMaterialDefault();
  matInstances.shader = shader;
  matInstances.maps[MATERIAL_MAP_DIFFUSE].color = RED;

  while (!WindowShouldClose()) {
    UpdateCamera(&camera, CAMERA_FREE);
    float cameraPos[3] = {camera.position.x, camera.position.y,
                          camera.position.z};
    BeginDrawing();
    ClearBackground(DARKBLUE);

    BeginMode3D(camera);

    for (int tIdx = 0; tIdx < testCubeCount; tIdx++) {
      int x = c3d.positionsX[tIdx];
      int y = c3d.positionsY[tIdx];
      int z = c3d.positionsZ[tIdx];

      Matrix translation = MatrixTranslate(x, y, z);

      transforms[tIdx] = translation;
      // }
    }
    DrawMeshInstanced(cube, matInstances, transforms, testCubeCount);

    DrawGrid(100, 1.0F);

    EndMode3D();
    EndDrawing();
  }

  CloseWindow();
}

int main() {
  // can_render_cubes();
  // can_render_moore_neighbours_3d();
  can_specify_neighbour_indexes();
  return 0;
}