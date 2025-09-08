#include "render_3d.h"
#include <stdio.h>
#include <string.h>
#include "const.h"

#include "raylib_shim.h"

#include "dstructs/arena.h"
#include "cellular/cells.h"
#include "cellular/evolve.h"
#include "light.h"

static int currentGeneration = 0;

Render3D Render3D_Init(Render *render) {
  Arena mode3DArena =
      Arena_Init("modeArena", &mode3DArenaStorage, MODE_3D_STORAGE_SIZE);

  Arena frame3DArena =
      Arena_Init("frame3DArena", &frame3DArenaStorage, FRAME_3D_STORAGE_SIZE);
  render->mode3DArena = mode3DArena;
  render->frame3DArena = frame3DArena;

  Camera3D camera = {0};
  camera.position = (Vector3){.x = 10.0f, .y = 10.0f, .z = 10.0f};
  camera.target = (Vector3){.x = 0.0f, .y = 0.0f, .z = 0.0f};
  camera.up = (Vector3){.x = 0.0f, .y = 1.0f, .z = 0.0f};
  camera.fovy = 90.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  Mesh cube = GenMeshCube(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);
  Mesh selectionCube = GenMeshCube(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);

  // transforms to be uploaded to GPU for instances
  Matrix *transforms = Arena_AllocAlignedZeroed(&render->frame3DArena,
                                                CUBE_COUNT * sizeof(Matrix),
                                                DEFAULT_MATRIX_ALIGNMENT);

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

  Material matSelection = LoadMaterialDefault();
  matSelection.maps[MATERIAL_MAP_DIFFUSE].color = YELLOW;

  if (!IsMaterialValid(matInstances)) {
    printf("Invalid material with shader id: %d\n", matInstances.shader.id);
    exit(1);
  }

  // TODO: optional: draw lines around the whole partitioned space, so the
  // bounds are clearly visible

  Cells_CalcNeighbourOffsets3D(&NEIGHBOUR_INDEXES_3D, MAX_CUBES_X, MAX_CUBES_Y,
                               MAX_CUBES_Z);

  // TODO: replace mallocs for arena
  HANDLE *transformMatrixThreads = malloc(numOfProcessors * sizeof(HANDLE));
  Render3DThreadCubes *allThreadCubes =
      malloc(numOfProcessors * sizeof(Render3DThreadCubes));

  // Render3DThreadCubes *allThreadCubes = Arena_AllocAlignedZeroed(
  //     &render->frame3DArena, numOfProcessors * sizeof(Render3DThreadCubes),
  //     DEFAULT_ARENA_ALIGNMENT);

  int chunkSizePerThread = CUBE_COUNT / numOfProcessors;

  HANDLE startEvent = CreateEvent(
      THREAD_DEFAULT_SEC_ATTRIBUTES,
      TRUE,  // creates a manual-reset event object, which requires the use of
             // the ResetEvent function to set the event state to nonsignaled
      FALSE, // initial state is nonsignaled
      NULL);

  if (startEvent == NULL) {
    printf("Start event creation failed: %d\n", GetLastError());
  }

  for (int i = 0; i < numOfProcessors; i++) {
    HANDLE doneEvent =
        CreateEvent(THREAD_DEFAULT_SEC_ATTRIBUTES, FALSE, FALSE, NULL);

    // auto-reset is used here, every thread gets its separate done event, in
    // order to properly wait for the completion of all of them
    if (doneEvent == NULL) {
      printf("Done event creation failed: %d\n", GetLastError());
    }

    allThreadCubes[i] =
        (Render3DThreadCubes){.actualCd = NULL, // will be set at render
                              .camera = &camera,
                              .cube = &cube,
                              .transforms = transforms,
                              .startEvent = startEvent,
                              .doneEvent = doneEvent,
                              .startIdx = i * chunkSizePerThread,
                              .endIdx = (i + 1) * chunkSizePerThread};
    if (i == numOfProcessors - 1) {
      allThreadCubes[i].endIdx = CUBE_COUNT;
    }

    transformMatrixThreads[i] =
        CreateThread(THREAD_DEFAULT_SEC_ATTRIBUTES, THREAD_DEFAULT_STACK_SIZE,
                     __Render3D_ResolveTransformMatrix, &allThreadCubes[i],
                     THREAD_DEFAULT_CREATION_FLAGS, THREAD_DEFAULT_THREAD_ID);

    if (transformMatrixThreads[i] == NULL) {
      printf("Thread creation failed: %d\n", GetLastError());
    }
  }

  Render3D render3d = {
      .camera = camera,
      .cube = cube,
      .matInstances = matInstances,
      .matSelection = matSelection,
      .render3DSpeed = 0.4f,
      .transforms = transforms,
      .allThreadCubes = allThreadCubes,
      .transformMatrixThreads = transformMatrixThreads,
  };
  return render3d;
}

//////////////////////////
// TODO: it would be best to focus on user input now instead of lighting
//////////////////////////

// TODO: PoC for lighting
// Light light = {.position = (Vector3){.x = 0.0F, .y = 0.0F, .z = 0.0F},
//                .target = (Vector3){.x = 1.0F, .y = 1.0F, .z = 1.0F},
//                .color = RED,
//                .attenuation = 1.0F};
void Render3D_RenderMode(Render *render) {
  if (render->isModeFirstFrame) {
    printf("Rendering 3D mode\n");
    Cells3D_InitArraysBasedOnCellSize(&render->mode3DArena,
                                      &render->render3d->firstC3d);
    Cells3D_InitArraysBasedOnCellSize(&render->mode3DArena,
                                      &render->render3d->secondC3d);

    Evolve3D_InitializeCells(&render->render3d->firstC3d, true);
    Evolve3D_InitializeCells(&render->render3d->secondC3d, false);
  }

  if (render->deltaTime >= render->render3d->render3DSpeed) {

    if (currentGeneration != 0 && currentGeneration % 2 == 0) {
      EvolveGOL3D_NextGeneration(&render->frame3DArena,
                                 &render->render3d->firstC3d,
                                 &render->render3d->secondC3d);
    } else {
      EvolveGOL3D_NextGeneration(&render->frame3DArena,
                                 &render->render3d->secondC3d,
                                 &render->render3d->firstC3d);
    }
    currentGeneration++;
    render->deltaTime = 0;

    // // TODO: PoC for lighting
    // if (light.position.x <= 0) {
    //   light.position.x -= +10.0F;
    // } else if (light.position.x >= 100.0F) {
    //   light.position.x -= -10.0F;
    // }

    // if (light.position.x >= 70 && light.position.x <= 100) {
    //   light.color = DARKGREEN;
    // } else {
    //   light.color = RED;
    // }
  }
  render->deltaTime += GetFrameTime();

  Cells3D *actualCd = currentGeneration % 2 == 0 ? &render->render3d->secondC3d
                                                 : &render->render3d->firstC3d;

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

  // TODO: add the user input, so one can flip the is_alive flag for a given
  // cell
  actualCd->aliveCells = 0;

  // TODO: revise this, so it's not that highly coupled, if possible
  bool isColliding = false;
  Matrix collMatrix = {0};

  for (int i = 0; i < numOfProcessors; i++) {
    render->render3d->allThreadCubes[i].actualCd = actualCd;
    // sets the specified event object to the signaled state
    SetEvent(render->render3d->allThreadCubes[i].startEvent);
  }

  HANDLE doneEvents[numOfProcessors];
  for (int i = 0; i < numOfProcessors; i++) {
    doneEvents[i] = render->render3d->allThreadCubes[i].doneEvent;
  }

  DWORD waitResult = WaitForMultipleObjects(
      numOfProcessors, doneEvents, THREAD_DEFAULT_WAIT_FOR_ALL, INFINITE);

  switch (waitResult) {
    // return value within the specified range indicates that the state of all
    // specified objects is signaled
  case WAIT_OBJECT_0:
    // TODO: fails at this part after the 2nd render call
    // printf("All threads ended successfully\n");
    break;

  default:
    printf("WaitForMultipleObjects failed: %d\n", GetLastError());
    break;
  }

  // TODO: draw this in the 3D menu instead
  // printf("aliveCells: %d\n", actualCd->aliveCells);
  // printf("\nDrawn %d cubes\n", tIdx + 1);

  Render_BeginDrawing();

  BeginMode3D(render->render3d->camera);

  // NOTE: DrawMeshInstanced MUST HAVE A SHADER ASSIGNED! Otherwise it will
  // throw error code 1281
  DrawMeshInstanced(render->render3d->cube, render->render3d->matInstances,
                    render->render3d->transforms, CUBE_COUNT);

  if (isColliding) {
    DrawMesh(render->render3d->cube, render->render3d->matSelection,
             collMatrix);
  }

  Render_LogGlError();

  DrawGrid(100, 1.0f);

  EndMode3D();

  Arena_FreeZeroed(&render->frame3DArena);
}

// TODO: teardown properly
// void Render3D_Teardown() {
// CloseEvents();

// for (int i = 0; i < numOfProcessors; i++) {
//   CloseHandle(render->render3d->transformMatrixThreads[i]);
// }
// }

// TODO: profile this and make necessary performance improvements, if needed
static void
__Render3D_ResolveTransformMatrix(Render3DThreadCubes *threadCubes) {

  for (;;) {
    DWORD waitResult = WaitForSingleObject(threadCubes->startEvent, INFINITE);

    ResetEvent(threadCubes->startEvent); // consumes the event

    // validate, if everything have been setup properly
    if (!threadCubes->actualCd) {
      continue;
    }

    Cells3D *actualCd = threadCubes->actualCd;
    Camera *camera = threadCubes->camera;
    Mesh *cube = threadCubes->cube;
    Matrix *transforms = threadCubes->transforms;

    // printf("Thread %d processing start %d - end %d\n", GetCurrentThreadId(),
    //        threadCubes->startIdx, threadCubes->endIdx);

    // printf("threadCubes->startIdx: %d, threadCubes->endIdx: %d \n",
    //        threadCubes->startIdx, threadCubes->endIdx);

    for (int i = threadCubes->startIdx; i < threadCubes->endIdx; i++) {
      bool is_alive = actualCd->is_alive[i];
      int x = actualCd->positionsX[i];
      int y = actualCd->positionsY[i];
      int z = actualCd->positionsZ[i];

      // since the arena is reset on every frame, we can just set the alive
      // cells

      Matrix translation = MatrixTranslate(x, y, z);
      // printf("\nx: %d, y: %d, z: %d\n", x, y, z);

      Matrix scale = MatrixScale(CUBE_SCALE, CUBE_SCALE, CUBE_SCALE);

      // with no gaps between the cubes
      Matrix mul = MatrixMultiply(translation, scale);
      // with gaps
      // Matrix mul = MatrixMultiply(scale, translation);

      // raycast to the nearest cube
      // TODO: not thread-safe
      // Ray cursorRay = GetScreenToWorldRay(GetMousePosition(), *camera);
      // RayCollision cursorCubeCollision =
      //     GetRayCollisionMesh(cursorRay, *cube, mul);

      // TODO: account for this function being run in multiple threads
      /*
      if (cursorCubeCollision.hit && !isColliding) {
        isColliding = true;
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
          actualCd->is_alive[i] = !actualCd->is_alive[i];
          collMatrix = mul;
        }
        printf("Collision with: x=%d, y=%d, z=%d\n", x, y, z);
      }
      */
      if (is_alive) { //&& !isColliding) {
        // TODO: the issue can be here, since multiple threads will write to the
        // same variable without locking
        actualCd->aliveCells++;
        transforms[i] = mul;
      }

      SetEvent(threadCubes->doneEvent); // threads will signal the end of the
                                        // work, render thread will consume
    }
  }
}