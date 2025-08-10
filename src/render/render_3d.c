#include "render_3d.h"
#include <stdio.h>

#include <raylib.h>
#include "raymath.h"

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

// TODO: rewrite this, onaly an example
// NOTE: see scratch/ray.c examples
// using power of 2 would be recommended
static const int MAX_CUBES_X = 32; // how many cubes to draw in a row
static const int MAX_CUBES_Y =
    32; // how many rows to draw in the current height
static const int MAX_CUBES_Z = 32;
static const int NUM_TO_DRAW = MAX_CUBES_X * MAX_CUBES_Y * MAX_CUBES_Z;

Render3D Render3D_Init(Render *render) {
  Arena mode3DArena =
      Arena_Init("modeArena", &mode3DArenaStorage, MODE_3D_STORAGE_SIZE);

  Arena frame3DArena =
      Arena_Init("frame3DArena", &frame3DArenaStorage, FRAME_3D_STORAGE_SIZE);
  render->mode3DArena = &mode3DArena;
  render->frame3DArena = &frame3DArena;

  Camera3D camera = {0};
  // TODO: adjust these
  camera.position = (Vector3){.x = 10.0f, .y = 10.0f, .z = 10.0f};
  camera.target = (Vector3){.x = 0.0f, .y = 0.0f, .z = 0.0f};
  camera.up = (Vector3){.x = 0.0f, .y = 1.0f, .z = 0.0f};
  camera.fovy = 90.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  float cubeSize = 1.0f; // 12 uniformly sized edges
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
  SetShaderValue(shader, ambientLoc, (float[4]){0.2f, 0.2f, 0.2f, 1.0f},
                 SHADER_UNIFORM_VEC4);

  CreateLight(LIGHT_DIRECTIONAL, (Vector3){50.0f, 50.0f, 0.0f},
              (Vector3){0.0f, 0.0f, 0.0f}, WHITE, shader);

  Material matInstances = LoadMaterialDefault();
  matInstances.shader = shader;
  matInstances.maps[MATERIAL_MAP_DIFFUSE].color = RED;

  if (!IsMaterialValid(matInstances)) {
    printf("Invalid material with shader id: %d\n", matInstances.shader.id);
    exit(1);
  }

  // Define transforms to be uploaded to GPU for instances
  Matrix *transforms = (Matrix *)RL_CALLOC(
      NUM_TO_DRAW,
      sizeof(Matrix)); // Pre-multiplied transformations passed to rlgl

  int currX = 0;
  int currY = 0;
  int currZ = 0;

  // TODO: optional: draw lines around the whole partitioned space, so the
  // bounds are clearly visible
  int tIdx = 0;
  for (int x = 0; x < MAX_CUBES_X; x = x + cubeSize) {
    for (int y = 0; y < MAX_CUBES_Y; y = y + cubeSize) {
      for (int z = 0; z < MAX_CUBES_Z; z = z + cubeSize) {
        Matrix translation = MatrixTranslate(x, y, z);

        // printf("\nx: %d, y: %d, z: %d\n", x, y, z);
        transforms[tIdx] = translation;
        tIdx++;
      }
    }
    // printf("\nDrawn %d cubes\n", tIdx + 1);
  }

  // TODO: Remove this, it is just to better understand what's going on
  // Matrix translation = MatrixTranslate(currentCol, currentHeight,
  // currentDepth);

  // transforms[i] = translation;

  /*
  TODO: How to render 3D cellular automata:
  - Would need some kind of partitioned space with clear bounds that can be used
  to render the cubes into
  - The size of a single cube in this partitioned space will be a single voxel,
  the smallest element that can be differentiated in the space
  - The number and placement of the voxels would then correspond to the initial
  state of the cellular automata, much like in 2D
  - Every generation would place the voxels into this partitioned space
  according to the given rule, and only the given generation would be rendered,
  much like in 2D
  - Since we only partition the world into a single big octant that defines the
  boundaries of what can be rendered within, it will only contain the different
  voxels at one level deep.
  - Based on the above, no complex structure, like an octree would need to be
  implemented
  - Much like in 2D, we can store the coordinates (x,y,z in this case) for the
  vectors that point to the given voxel, so for checking the neighbours we only
  have to check which voxel points to with vector
  */

  Render3D render3d = {.camera = camera,
                       .cube = cube,
                       .matInstances = matInstances,
                       .shader = shader,
                       .render3DSpeed = 0.2f,
                       .transforms = transforms};
  return render3d;
}

static int frameNum = 0;
static int randLowerLimit = -500;
static int randUpperLimit = 500;

void Render3D_RenderMode(Render *render) {
  if (render->isModeFirstFrame) {
    printf("Entering 3D mode");
  }
  UpdateCamera(&render->render3d->camera, CAMERA_FREE);
  float cameraPos[3] = {render->render3d->camera.position.x,
                        render->render3d->camera.position.y,
                        render->render3d->camera.position.z};
  SetShaderValue(render->render3d->shader,
                 render->render3d->shader.locs[SHADER_LOC_VECTOR_VIEW],
                 cameraPos, SHADER_UNIFORM_VEC3);

  int deltaTimeLoc = GetShaderLocation(render->render3d->shader, "deltaTime");
  SetShaderValue(render->render3d->shader, deltaTimeLoc, &render->deltaTime,
                 SHADER_UNIFORM_FLOAT);
  // TOOD: update the light shader here, if needed

  // TODO: decide on at what time should we start drawing
  Render_BeginDrawing();

  BeginMode3D(render->render3d->camera);

  if (frameNum < 500) {
    randLowerLimit++;
    randUpperLimit--;
  } else {
    randLowerLimit--;
    randUpperLimit++;
  }
  if (frameNum == 1000) {
    frameNum = 0;
  }

  for (int i = 0; i < NUM_TO_DRAW; i++) {
    render->render3d->transforms[i].m12 =
        GetRandomValue(-1 * render->deltaTime, render->deltaTime) * 2;
    render->render3d->transforms[i].m13 =
        GetRandomValue(-1 * render->deltaTime, render->deltaTime) * 2;
    render->render3d->transforms[i].m14 =
        GetRandomValue(-1 * render->deltaTime, render->deltaTime) * 2;
  }

  DrawMeshInstanced(render->render3d->cube, render->render3d->matInstances,
                    render->render3d->transforms, NUM_TO_DRAW);
  Render_LogGlError();

  DrawGrid(100, 1.0f);

  EndMode3D();
  frameNum++;

  render->deltaTime += GetFrameTime();
}

// void Render3D_Teardown() {
//   RL_FREE(transforms);
// }