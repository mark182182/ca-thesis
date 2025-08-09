#include "render_3d.h"
#include <stdio.h>

#include <raylib.h>
#include "raymath.h"

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#define NUM_TO_DRAW 50000

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

  Mesh cube = GenMeshCube(1.0f, 1.0f, 1.0f);

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

  // Translate and rotate cubes randomly
  for (int i = 0; i < NUM_TO_DRAW; i++) {
    Matrix translation = MatrixTranslate((float)GetRandomValue(0, 100),
                                         (float)GetRandomValue(0, 100),
                                         (float)GetRandomValue(0, 100));

    transforms[i] = translation;
  }

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
  // TOOD: update the light shader here, if needed

  // TODO: decide on at what time should we start drawing
  Render_BeginDrawing();

  // Matrix mtx = MatrixTranslate(10.0f, 10.0f, 10.0f);
  // Material matDefault = LoadMaterialDefault();
  // matDefault.maps[MATERIAL_MAP_DIFFUSE].color = WHITE;

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

  // DrawMesh(render->render3d->cube, matDefault, mtx);
  // Translate and rotate cubes randomly
  for (int i = 0; i < NUM_TO_DRAW; i++) {
    Matrix translation =
        MatrixTranslate((float)GetRandomValue(randLowerLimit, randUpperLimit),
                        (float)GetRandomValue(randLowerLimit, randUpperLimit),
                        (float)GetRandomValue(randLowerLimit, randUpperLimit));

    render->render3d->transforms[i] = translation;
  }

  DrawMeshInstanced(render->render3d->cube, render->render3d->matInstances,
                    render->render3d->transforms, NUM_TO_DRAW);
  Render_LogGlError();

  DrawGrid(100, 1.0f);

  EndMode3D();
  frameNum++;
}

// void Render3D_Teardown() {
//   RL_FREE(transforms);
// }