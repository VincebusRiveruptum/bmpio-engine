# BMPio Engine - Core Documentation & Pipeline

This document describes the spatial partitioning system, coordinate conventions, and rendering pipeline for the BMPio engine, targeting 32-bit MS-DOS (Mode X 320x200).

## 1. Coordinate Systems

The engine uses three distinct coordinate spaces:

### 1.1 World Coordinates (Absolute)

- **Convention**: **CENTER-based**. (0,0,0) represents the center of the world.
- **Data Type**: `long` (using 16.16 fixed-point math for sub-pixel precision).
- **Z-Axis**: Used for depth sorting (layering). Higher Z means "closer" to camera.

### 1.2 Grid Coordinates (Spatial Partitioning)

- **Structure**: A 3D grid (`visGrid`) of dimensions 63x63x63.
- **Scale**: Each grid cell covers `100` world units (`SP_GRID_SCALE`).
- **Insertion**: High-performance spatial lookup using O(1) grid hashing.
- **Origin**: Grid cell `[31][31][31]` maps to world origin `(0,0,0)`.

### 1.3 Screen Coordinates (Projected)

- **Resolution**: 320x200 pixels.
- **Origin**: (0,0) is top-left.
- **Logic**: During projection, `sp_worldToScreen` returns the **CENTER** of the asset on screen. Drawing functions take this center and subtract half the sprite's width/height to align it.

---

## 2. Architecture Components

### 2.1 Multi-Asset Grid (`visGrid`)

`struct List *visGrid[63][63][63];`

- Refactored from single pointers to **Linked Lists**.
- Allows multiple assets (players, enemies, decorations) to occupy the same grid cell.
- Managed via `gm_insertAsset` and `sp_removeAssetFromVisGrid`.

### 2.2 Render Queue (`renderQueue`)

`struct Asset *renderQueue[256];`

- Capacity increased to **256 assets** (`SP_MAX_RENDER_ASSETS`).
- Populated each frame by scanning cells within the camera's `gridBounds`.

### 2.3 Z-Index Sorting

- **Algorithm**: Insertion Sort (optimized for near-sorted lists).
- **Function**: `sp_renderQueueApplyZOrdering()`.
- **Criteria**: Sorted by `asset->coordinates->z`.
- **Usage**: Backgrounds should be set to large negative Z (e.g., -100), while interactive actors usually stay at Z = 0.

---

## 3. The Rendering Pipeline

The frame life cycle follows these steps:

1.  **Event Handling**: `gm_listenEvents()` processes BIOS keyboard interrupts.
2.  **Logic Update**: Actions/Animations updated based on game rules.
3.  **Culling & Queueing** (`sp_initCameras`):
    - Scans grid cells visible to `globalCamera`.
    - Adds all assets in those cells' lists to the `renderQueue`.
4.  **Z-Sorting**: `sp_renderQueueApplyZOrdering()` sorts the queue by depth.
5.  **Frustum Culling**: `sp_isInFrustrum()` checks if the asset's center + radius is actually within the 320x200 viewport.
6.  **Projection**: `sp_worldToScreen()` converts world center to screen center.
7.  **Drawing**: `as_drawBitmap` (or `as_drawBitmapTransform`) renders the sprite with transparency support.

---

## 4. Performance Reference

| System            | Complexity | Peak Load (DOS 386+)                  |
| :---------------- | :--------- | :------------------------------------ |
| **Grid Scan**     | O(N³) scan | Efficient due to empty cell skipping. |
| **Z-Sorting**     | O(M log M) | Fast for < 100 assets.                |
| **Rasterization** | O(Pixels)  | Mode X planar-optimized in Assembly.  |

---

## 5. Development Tips

- **Backgrounds**: Always create an Actor/Asset for backgrounds and set Z to a low value.
- **C89**: Maintain all variable declarations at the start of blocks for Watcom compatibility.
- **Memory**: Always `free()` ephemeral `ScreenCoordinates` objects returned by projection functions.
