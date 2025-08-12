#include "render_3d.h"
#include <stdio.h>
#include <string.h>

#include <raylib.h>
#include "raymath.h"

#include "dstructs/arena.h"
#include "cellular/cells.h"
#include "cellular/evolve.h"

static int currentGeneration = 0;

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

  Mesh cube = GenMeshCube(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);

  // transforms to be uploaded to GPU for instances
  Matrix *transforms = Arena_AllocAlignedZeroed(
      &mode3DArena, CUBE_COUNT * sizeof(Matrix), DEFAULT_MATRIX_ALIGNMENT);

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

  if (!IsMaterialValid(matInstances)) {
    printf("Invalid material with shader id: %d\n", matInstances.shader.id);
    exit(1);
  }

  // TODO: optional: draw lines around the whole partitioned space, so the
  // bounds are clearly visible

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
                       .render3DSpeed = 0.2f,
                       .transforms = transforms};
  return render3d;
}

void Render3D_RenderMode(Render *render) {
  if (render->isModeFirstFrame) {
    printf("Entering 3D mode");
    Cells3D_InitArraysBasedOnCellSize(render->mode3DArena,
                                      &render->render3d->firstC3d);
    Cells3D_InitArraysBasedOnCellSize(render->mode3DArena,
                                      &render->render3d->secondC3d);

    Evolve3D_InitializeCells(&render->render3d->firstC3d, true);
    Evolve3D_InitializeCells(&render->render3d->secondC3d, false);
  }

  UpdateCamera(&render->render3d->camera, CAMERA_FREE);
  float cameraPos[3] = {render->render3d->camera.position.x,
                        render->render3d->camera.position.y,
                        render->render3d->camera.position.z};

  SetShaderValue(
      render->render3d->matInstances.shader,
      render->render3d->matInstances.shader.locs[SHADER_LOC_VECTOR_VIEW],
      cameraPos, SHADER_UNIFORM_VEC3);

  int deltaTimeLoc =
      GetShaderLocation(render->render3d->matInstances.shader, "deltaTime");
  SetShaderValue(render->render3d->matInstances.shader, deltaTimeLoc,
                 &render->deltaTime, SHADER_UNIFORM_FLOAT);
  // TOOD: update the light shader here, if needed

  // TODO: invoke evolve next gen here
  if (!render->isModeFirstFrame) {
    if (currentGeneration % 2 == 0) {
      EvolveGOL3D_NextGeneration(&render->render3d->firstC3d,
                                 &render->render3d->secondC3d);
    } else {
      EvolveGOL3D_NextGeneration(&render->render3d->secondC3d,
                                 &render->render3d->firstC3d);
    }
  }
  currentGeneration++;

  Cells3D actualCd = currentGeneration % 2 == 0 ? render->render3d->secondC3d
                                                : render->render3d->firstC3d;

  for (int tIdx = 0; tIdx < CUBE_COUNT; tIdx++) {
    bool is_alive = actualCd.is_alive[tIdx];
    int x = actualCd.positionsX[tIdx];
    int y = actualCd.positionsY[tIdx];
    int z = actualCd.positionsZ[tIdx];

    if (is_alive) {
      Matrix translation = MatrixTranslate(x, y, z);
      // printf("\nx: %d, y: %d, z: %d\n", x, y, z);

      render->render3d->transforms[tIdx] = translation;
    }
  }
  // printf("\nDrawn %d cubes\n", tIdx + 1);

  Render_BeginDrawing();

  BeginMode3D(render->render3d->camera);

  // NOTE: DrawMeshInstanced MUST HAVE A SHADER ASSIGNED! Otherwise it will
  // throw error code 1281
  DrawMeshInstanced(render->render3d->cube, render->render3d->matInstances,
                    render->render3d->transforms, CUBE_COUNT);
  // TODO: DO NOT CALLOC AND FREE IN ALL FRAMES!
  // Would need to only free this once, when the mode changes
  // RL_FREE(render->render3d->transforms);

  Render_LogGlError();

  DrawGrid(100, 1.0f);

  EndMode3D();

  render->deltaTime += GetFrameTime();

  Arena_Free(render->frame3DArena);
}

// void Render3D_Teardown() {
//   RL_FREE(transforms);
// }