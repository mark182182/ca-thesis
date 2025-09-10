#include "arena.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "raylib_shim.h"

#ifdef DEBUG_MODE
#include "tracy/TracyC.h"
#endif

static void __ExitWithMsg(char *msg) {
  fprintf(stderr, "Error: %s\n", msg);
  exit(1);
}

static void __ExitWithArenaMsg(Arena *arena, char *msg) {
  fprintf(stderr, "Error: %s\n", msg);
  if (arena != NULL) {
    fprintf(stderr, "Arena was: %s, capacity: %zu, used: %zu\n", arena->name,
            arena->capacity, arena->used);
  }
  exit(1);
}

Arena Arena_Init(char *name, uint8_t *memory, size_t capacity) {
  if (!memory) {
    __ExitWithMsg("Memory cannot be null");
  }
  if (capacity == 0) {
    __ExitWithMsg("Capacity must be larger than 0");
  };

  Arena arena = {
      .name = name, .memory = memory, .capacity = capacity, .used = 0};
  return arena;
}

void *Arena_AllocAlignedZeroed(Arena *arena, size_t size, size_t alignment) {
  return __Arena_Alloc(arena, size, alignment, true);
}

void *Arena_AllocAligned(Arena *arena, size_t size, size_t alignment) {
  return __Arena_Alloc(arena, size, alignment, false);
}

void *__Arena_Alloc(Arena *arena, size_t size, size_t alignment,
                    bool shouldZeroOut) {
  // TODO: debug allocations, if DEBUG_MODE is enabled
  // printf_s("\nCurrent size to allocate: %zu\n", size);
  // printf_s("\nCurrent used: %zu\n", arena->used);

  uintptr_t currentAddr = (uintptr_t)(arena->memory + arena->used);

  // need to align the current address by zeroing out the least significant bits
  // this would round up
  uintptr_t alignedAddr =
      (currentAddr + (alignment - 1)) & ~(uintptr_t)(alignment - 1);

  size_t paddingSize = alignedAddr - currentAddr;
  size_t newUsedSize = arena->used + paddingSize + size;

  if (newUsedSize > arena->capacity) {
    printf_s("Allocation would be over the limit");
    printf_s("\nWanted newUsedSize: %zu\n", newUsedSize);
    __ExitWithArenaMsg(arena,
                       "Cannot allocate memory, arena would be over capacity");
  }

  if (shouldZeroOut && arena->memory != NULL) {
    memset((void *)alignedAddr, 0, newUsedSize);
  }

  arena->used = newUsedSize;

#ifdef DEBUG_MODE
  TracyCAlloc(alignedAddr, newUsedSize);
#endif
  return (void *)alignedAddr;
}

/*
 * Rolls back the pointer to the original position, so the pre-allocated backing
 * byte storage can be reused.
 */
void Arena_Free(Arena *arena) {
  if (arena != NULL) {
    arena->used = 0;
  }
}

void Arena_FreeZeroed(Arena *arena) {
  if (arena != NULL) {
    arena->used = 0;
    if (arena->memory != NULL) {
      // NOTE: this can be costly, as it sets every byte one-by-one
      memset(arena->memory, 0, arena->capacity);
    }
  }
}
