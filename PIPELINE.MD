# 2D Orthographic Rendering Pipeline Explaination

The rendering pipeline implements a grid-based spatial partitioning system with camera culling for efficient 2D orthographic rendering in a DOS environment (320x200 resolution, Mode X VGA).

## Core Concepts

### Coordinate Systems

#### World Coordinates

Coordinates

- Absolute position in the game world
- Uses fixed-point math (16.16 format) for sub-pixel precision
- Example: (0, 0, 0) is world origin, (100, 50, 0) is 100 units east, 50 units south

#### Grid Coordinates (Spatial Partitioning)

- Divides world into a 63×63×63 grid (SP_GRID_SIZE)
- Each grid cell represents 100 world units (SP_GRID_SCALE)
- Grid [31][31][31] is the center (world origin 0,0,0)
- Used for fast visibility culling

#### Screen Coordinates

- Final pixel position on the 320×200 display
- Calculated via camera projection
- Origin (0, 0) is top-left of screen

### Architecture Components

#### Spatial Grid (visGrid)

`Asset *visGrid[SP_GRID_SIZE][SP_GRID_SIZE][SP_GRID_SIZE];`

- 3D array storing pointers to assets by grid position
- Enables O(1) lookup of assets in a spatial region
- Memory: ~1MB (63³ × 4 bytes per pointer)

#### Render Queue (renderQueue)

`Asset *renderQueue[SP_GRID_SIZE] = {NULL};`

- Temporary array of visible assets for current frame
- Populated by camera culling each frame
- Max 63 assets (one per grid cell in view)

#### Camera

```
Camera *globalCamera;

typedef struct Camera {
    Coordinates *position;          // World position
    Coordinates *prevPos;           // Previous position (for change detection)
    ScreenCoordinates *resolution;  // Screen resolution (320×200)
    
    // Grid bounds (calculated from position)
    unsigned char gridMinX, gridMinY, gridMinZ;
    unsigned char gridMaxX, gridMaxY, gridMaxZ;
} Camera;
```

- Defines viewport in world space
- Grid bounds cover 1 unit radius (SP_GRID_VIS_SIZE = 1) around camera
- Automatically recalculates bounds when position changes

### Pipeline Flow

```
1. t_initTests()           → Create actors/assets
2. sp_initCameras()        → Build initial render queue
3. Main Loop:
   - sp_checkCameras()     → Rebuild queue if camera moved
   - eng_renderFrame()     → Draw visible assets
```
##### Asset Creation & Insertion (TEST.C → GAME.C)

```
// 1. Create actor with actions
Actor *actor = gm_createActor(name, desc, stats, actions);
// 2. Create asset instance at world position
Asset *asset = gm_createAsset(actor, sp_createCoordinates(x, y, z));
// 3. Insert into spatial grid
gm_insertAsset(asset);
```

#### Grid Insertion Logic (gm_insertAsset)

```
// Convert world coords to grid indices
vis_x = (int)(worldX / SP_GRID_SCALE) + SP_GRID_HALF;
vis_y = (int)(worldY / SP_GRID_SCALE) + SP_GRID_HALF;
vis_z = (int)(worldZ / SP_GRID_SCALE) + SP_GRID_HALF;
// Store in grid
visGrid[vis_x][vis_y][vis_z] = asset;
```

#### Examples

- World (0, 0, 0) → Grid [31][31][31]
- World (100, 0, 0) → Grid [32][31][31]
- World (-100, 50, 0) → Grid [30][31][31]

### Camera Setup (sp_createCamera)

```
Camera *camera = sp_createCamera(
    sp_createCoordinates(0, 0, 0),      // World position
    sp_createScreenCoordinates(320, 200) // Screen resolution
);
```

#### Grid Bounds Calculation:

```
// Convert camera world position to grid
camGridX = (int)(cameraX / SP_GRID_SCALE) + SP_GRID_HALF;
// Set bounds to 1 grid unit radius
gridMinX = camGridX - SP_GRID_VIS_SIZE;  // -1
gridMaxX = camGridX + SP_GRID_VIS_SIZE;  // +1
```

##### Example (Camera at world 0,0,0):

- Camera grid: [31][31][31]
- Bounds: [30-32][30-32][30-32]
- Covers world: (-100, -100, -100) to (200, 200, 200)

### Culling & Queue Building ( sp_initCameras )

Called once at startup and whenever camera moves.

```
1. Clear render queue
2. Scan entire visGrid (63×63×63)
3. For each non-NULL grid cell:
   - Check if inside camera bounds
   - If visible, add to renderQueue
   - Update asset's vis_current* attributes
```

#### Bounds Check:

```
if (gridX < camera->gridMinX || gridX > camera->gridMaxX ||
    gridY < camera->gridMinY || gridY > camera->gridMaxY ||
    gridZ < camera->gridMinZ || gridZ > camera->gridMaxZ) {
    continue;  // Outside camera view
}
```

### Rendering (eng_renderFrame)

Called every frame in main loop.

```
for each asset in renderQueue:
    1. Get current action's animation
    2. Calculate frame index (gametick % animation->length)
    3. Get sprite for current frame
    4. Project world coords to screen coords
    5. Apply transformations (rotation/translation)
    6. Draw sprite at screen position
```

### World-to-Screen Projection ( sp_worldToScreen ):

```
// Calculate offset from camera
offsetX = assetWorldX - cameraWorldX;
offsetY = assetWorldY - cameraWorldY;
// Center camera on screen and add offset
screenX = (screenWidth / 2) + offsetX;   // 160 + offset
screenY = (screenHeight / 2) + offsetY;  // 100 + offset
```

#### Example:

- Camera at (0, 0, 0)
- Asset at (100, 50, 0)
- Offset: (100, 50)
- Screen: (260, 150) ← 160+100, 100+50

## Key Functions Reference

### Spatial (SPACE.C/H)

|Function|	Purpose
|----|----
|sp_createCoordinates(x, y, z)|Create world position
|sp_createCamera(pos, res)|Create camera with grid bounds
|sp_initCameras()|Build render queue from visGrid
|sp_checkCameras()|Rebuild queue if camera moved
|sp_worldToScreen(world, cam)|Project world → screen coords

### Game (GAME.C/H)

|Function|Purpose
|----|----
|gm_createActor(...)|Create actor definition
|gm_createAsset(actor, coords)|Create asset instance
|gm_insertAsset(asset)|Add to spatial grid

### Engine (ENGINE.C/H)

|Function|Purpose
|----|----
|eng_renderFrame(tick)|Main render loop
|eng_setPalette(palette)|Set VGA palette

### Assets (ASSETS.C/H)

|Function|Purpose
|----|----
|as_loadBMPfile(path)|Load BMP image
|as_createAnimation()|Create animation
|as_drawBitmap(...)|Draw sprite to screen

#### Performance Characteristics

- Grid Scan: O(N³) where N = 63 (250k cells, but most are empty)
- Culling: O(1) per visible asset
- Rendering: O(M) where M = visible assets (typically < 10)
- Memory: ~1MB for visGrid + minimal overhead

### Limitations & Future Improvements

#### Current Limitations

- No Z-sorting - Assets drawn in queue order (not depth-sorted)
- Fixed camera bounds - Always 1 grid unit radius
- No frustum culling - Doesn't check if asset is actually on screen
- Memory leak - screenPos allocated but never freed in eng_renderFrame

#### Suggested Improvements

- Add Z-sorting - Sort renderQueue by Z coordinate before drawing
- Dynamic camera zoom - Adjust SP_GRID_VIS_SIZE based on zoom level
- Screen bounds check - Skip drawing if screenX/Y outside [0, 320]×[0, 200]
- Free screenPos - Add free(screenPos) after drawing each asset
- Optimize grid scan - Only scan camera bounds instead of entire grid

### Example Walkthrough

```
// Camera at world origin
camera->position = (0, 0, 0)
camera->gridBounds = [30-32][30-32][30-32]
// Renamon at world origin
renamon->coordinates = (0, 0, 0)
renamon->gridPosition = [31][31][31]
```

##### Frame Execution:


1. sp_checkCameras() - Camera hasn't moved, skip rebuild
2. eng_renderFrame(tick) - Start rendering
3. Loop through renderQueue, find Renamon at slot 0
4. Get Renamon's Idle animation, frame 0
5. Project (0, 0, 0) → screen (160, 100) (center)
6. Draw sprite at (160, 100)

##### Move Renamon:

```
renamon->coordinates = (100, 0, 0)  // Move 100 units east
```

- Grid position unchanged: [32][31][31] (still in [30-32] bounds)
- Still in renderQueue
- Projects to screen (260, 100) ← 160 + 100

##### Move Camera:

```
camera->position = (200, 0, 0)  // Move 200 units east
camera->gridBounds = [32-34][30-32][30-32]
```

- sp_checkCameras() detects movement 
- Rebuilds queue: Renamon at [32][31][31] still visible
- Renamon now projects to screen (60, 100) ← 160 + (100 - 200)