# Arena Memory System Documentation

The engine uses an **Arena (Bump) Allocator** to manage memory efficiently, eliminating the "choppiness" caused by standard `malloc` and `free` cycles in the high-frequency render loop.

## Core Concepts

- **Zero-Latency Allocation**: Allocating memory is just an integer addition (moving a pointer).
- **Batch Deallocation**: Memory is not freed individually. Instead, an entire "Arena" is reset at once, making deallocation instantaneous.
- **Cache Friendly**: Objects allocated together stay together in memory.

## Existing Arenas & Lifecycles

| Arena       | Variable           | Size  | Lifetime     | Purpose                                               |
| :---------- | :----------------- | :---- | :----------- | :---------------------------------------------------- |
| **Session** | `gameSessionArena` | 4MB   | Game Session | High-level assets (BMPs, Sprites, Animations).        |
| **Scene**   | `sceneArena`       | 1MB   | Level/Scene  | Assets, Actors, and Stats for the current map.        |
| **Frame**   | `frameArena`       | 128KB | Single Frame | Transitional data (Screen coordinates, math results). |
| **Test**    | `testArena`        | 512KB | Prototyping  | Sandbox for engineering tests and experiments.        |

## Lifecycle Management

1. **Initialization**: `mem_init()` must be called at the very beginning of the program (in `MAIN.C`), before any asset loading occurs.
2. **Resetting**: `mem_arena_reset(arena)` clears an arena without freeing the underlying buffer.
   - **`frameArena`** is reset automatically at the start of every `eng_renderFrame()`.
3. **Shutdown**: `mem_shutdown()` is called upon exiting the game loop to release all physical memory back to the OS.

## Usage Guide

### Allocating Memory

Instead of `malloc(sizeof(MyStruct))`, use:

```c
MyStruct *s = (MyStruct*)mem_arena_alloc(sceneArena, sizeof(MyStruct));
```

### Creating a New Arena

If you need a new memory zone (e.g., for a UI-specific cache):

1. **Header (`MEM.H`)**:

   - Define a size: `#define ARENA_SIZE_UI (256 * 1024)`
   - Declare the pointer: `extern MemoryArena *uiArena;`

2. **Implementation (`MEM.C`)**:
   - Define the pointer: `MemoryArena *uiArena = NULL;`
   - In `mem_init()`:
     ```c
     uiArena = (MemoryArena*)malloc(sizeof(MemoryArena));
     mem_arena_init(uiArena, "UI", MEM_ARENA_CACHE, ARENA_SIZE_UI);
     ```
   - In `mem_shutdown()`:
     ```c
     mem_arena_free(uiArena);
     free(uiArena);
     ```

## Best Practices

- **For a zero-initializaton, use memset(PointerInstance, 0, TypeSize)**
- **Do NOT use `free()`** on pointers returned by `mem_arena_alloc`.
- **Prefer `frameArena`** for any memory that doesn't need to exist beyond the current frame.
- **Alignment**: The allocator automatically aligns requests to 4 bytes for optimal 32-bit performance on 386+ hardware.
