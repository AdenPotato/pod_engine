# GZDoom-Style Looter Shooter - Design Document

## Project Overview
Building a **GZDoom-style 3D looter shooter** from scratch in C++ with OpenGL.

**Rendering Philosophy (GZDoom approach):**
- **Levels:** True 3D geometry - Polygonal meshes, textured surfaces, no sector limitations
- **Entities:** Doom-style billboard sprites - 8-directional rotation, variable sizes
- **Movement:** Full 3D - Look up/down, jump, crouch (like GZDoom, not vanilla Doom)
- **Feel:** Classic Doom aesthetic with modern 3D engine capabilities

**Think:** GZDoom's rendering + looter-shooter mechanics (randomized weapons, loot drops, etc.)

---

## Core Architecture Decisions

### 1. **3D Space with Billboard Sprites**
- NOT a 2D game - full 3D world space
- Sprites are billboarded quads that face the camera
- Enemies, items, decorations = billboard sprites
- Level geometry = traditional 3D meshes (not voxels for now)
- Weapon sprites = HUD-space rendering (no billboarding)

### 2. **Sprite System (GZDoom-Style)**
Unlike typical sprite atlases, we handle sprites like GZDoom:
- **Each sprite frame = separate image file**
- **Variable sizes** - sprites can be different dimensions
- **Naming convention**: `SPRITEA0.png`, `SPRITEB0.png`, etc.
- **8-directional sprites**: Angle-based frame selection
- **Offset/pivot support**: Sprites have custom center points

**Sprite Structure:**
```cpp
struct Sprite {
    Texture* texture;
    glm::vec2 size;        // Actual pixel dimensions
    glm::vec2 offset;      // Drawing offset from center
    glm::vec2 pivot;       // Rotation pivot point
};
```

**Sprite Loading:**
```
assets/sprites/
├── PISGA0.png  (64x64)
├── PISGB0.png  (48x72)
├── TROOA0.png  (128x128)
└── TROOA1.png  (96x128)
```

### 3. **State Machine System**
**Purpose:** Handle weapon/enemy animation and logic flow

**ZScript Replacement:**
Instead of a full VM with ZScript, we use:
- **Text-based state files** (`.states` format from conversion guide)
- **C++ action callbacks** - Actions are registered C++ functions
- **35 Hz tick rate** - State machine updates at 35 ticks/second
- **Event-driven** - Actions trigger C++ code (FireBullets, PlaySound, etc.)

**State File Example:**
```
Ready {
    PISGA, 1, WeaponReady
    Loop
}

Fire {
    PISGA, 0, GunFlash, PlaySound("pistol/fire")
    PISGB, 1, FireBullets(1, 5.0, 2.0, 10), Light(200)
    PISGC, 2
    PISGB, 1, ReFire
    Goto Ready
}
```

**Implementation:**
- State parser reads `.states` files into data structures
- State executor runs at 35 Hz, advances frames
- Action executor calls registered C++ functions
- No scripting VM needed - pure data-driven with C++ callbacks

### 4. **Voxel System Status**
- **Keep code** - Don't delete voxel system
- **Deprioritize** - Move to back burner for now
- **Future use** - Possible destructible cover, special effects
- **Current focus** - Traditional mesh-based levels

---

## Phase 1: Core Systems

### **1.1 FPS Player Controller**
- Movement: WASD (forward/back/strafe)
- Look: Mouse movement (yaw/pitch)
- Jump: Space (if we add gravity)
- Sprint: Shift (optional)
- Camera height: ~1.7m (eye level)
- Movement speed: 5.0 m/s base

**Reuse existing Camera class:**
```cpp
// Already have Camera with:
// - Position, Front, Up, Right vectors
// - ProcessKeyboard(direction, deltaTime)
// - ProcessMouseMovement(xoffset, yoffset)
// Just need to integrate as player controller
```

### **1.2 Simple Test Level**
**Geometry:**
- Simple square room (10x10 meters)
- Floor plane at y=0
- 4 walls at edges
- Ceiling at y=3m (optional)

**Texture:**
- Use `assets/textures/prototype_square.png`
- 512x512 texture = 1x1 meter
- UV mapping: 10 repeats across 10m floor

**Mesh Creation:**
```cpp
Mesh* CreateTestLevel() {
    // Floor: 10x10m quad at y=0
    // Walls: 4 quads, 10m wide x 3m tall
    // UV scale: texture_coords * 10 (for 10 repetitions)
}
```

### **1.3 Billboard Sprite Renderer**
**Features:**
- Render sprites in 3D world space
- Auto-rotate to face camera (billboarding)
- Support variable sprite sizes
- Depth sorting with level geometry
- Batch rendering for performance

**Shader Approach:**
```glsl
// Vertex shader
// - Take sprite world position (center point)
// - Create quad facing camera using camera right/up vectors
// - Apply sprite size and offset
// - Transform to clip space

// Fragment shader
// - Sample sprite texture
// - Discard transparent pixels (alpha < threshold)
// - Optional: apply lighting/fog
```

**Billboarding Math:**
```cpp
// In vertex shader:
vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);

vec3 worldPos = spriteCenter
    + cameraRight * vertexOffset.x * spriteSize.x
    + cameraUp * vertexOffset.y * spriteSize.y;
```

---

## Phase 2: Sprite System Details

### **2.1 Sprite Loading (GZDoom-Style)**
```cpp
class SpriteManager {
public:
    // Load individual sprite with metadata
    Sprite* LoadSprite(const std::string& filepath);

    // Load sprite set (all rotations)
    // e.g., "TROO" loads TROOA0, TROOA1, ..., TROOA8
    std::vector<Sprite*> LoadSpriteSet(const std::string& baseName);

    // Get sprite by name
    Sprite* GetSprite(const std::string& name);

private:
    std::unordered_map<std::string, Sprite*> sprites;
};
```

**Sprite Metadata (optional sidecar file):**
```json
// PISGA0.json
{
    "offset": [32, 32],
    "pivot": [0.5, 0.0],
    "bright": false
}
```

### **2.2 8-Directional Sprite Selection**
```cpp
// Calculate which sprite frame to show based on view angle
int GetSpriteRotation(glm::vec3 entityPos, glm::vec3 entityFacing, glm::vec3 cameraPos) {
    glm::vec3 toCamera = glm::normalize(cameraPos - entityPos);

    // Angle between entity facing and camera direction
    float angle = atan2(toCamera.z, toCamera.x) - atan2(entityFacing.z, entityFacing.x);

    // Normalize to 0-2π
    if (angle < 0) angle += 2 * M_PI;

    // Map to 8 directions (0-7)
    int rotation = (int)((angle + M_PI / 8) / (M_PI / 4)) % 8;

    return rotation;
}
```

**Sprite Naming:**
- `A0` = front view
- `A1` = 45° right
- `A2-A3` = side views
- `A4` = back view (optional, often mirrors front)
- `A5-A7` = left side (often mirrors right)

---

## Phase 3: State Machine System

### **3.1 Action Registration System**
Instead of ZScript VM, use C++ function registration:

```cpp
// ActionRegistry.h
class ActionRegistry {
public:
    using ActionFunc = std::function<void(Entity*, const std::vector<std::string>&)>;

    static void RegisterAction(const std::string& name, ActionFunc func);
    static void ExecuteAction(Entity* entity, const ActionCall& action);

private:
    static std::unordered_map<std::string, ActionFunc> actions;
};

// Register actions at startup
void InitWeaponActions() {
    ActionRegistry::RegisterAction("WeaponReady", Actions::WeaponReady);
    ActionRegistry::RegisterAction("FireBullets", Actions::FireBullets);
    ActionRegistry::RegisterAction("PlaySound", Actions::PlaySound);
    ActionRegistry::RegisterAction("GunFlash", Actions::GunFlash);
    ActionRegistry::RegisterAction("Light", Actions::Light);
    ActionRegistry::RegisterAction("ReFire", Actions::ReFire);
    ActionRegistry::RegisterAction("CheckReload", Actions::CheckReload);
}
```

### **3.2 Core Weapon Actions**
```cpp
namespace Actions {
    // Check if fire button still held, continue firing
    void ReFire(Entity* weapon, const std::vector<std::string>& params);

    // Fire hitscan bullets
    void FireBullets(Entity* weapon, const std::vector<std::string>& params);
    // Params: [count, spreadH, spreadV, damage]

    // Play weapon sound
    void PlaySound(Entity* weapon, const std::vector<std::string>& params);
    // Params: ["sound_key"]

    // Show muzzle flash
    void GunFlash(Entity* weapon, const std::vector<std::string>& params);

    // Dynamic light effect
    void Light(Entity* weapon, const std::vector<std::string>& params);
    // Params: [radius]

    // Set weapon to ready state (can fire)
    void WeaponReady(Entity* weapon, const std::vector<std::string>& params);

    // Check if reload needed
    void CheckReload(Entity* weapon, const std::vector<std::string>& params);
}
```

---

## Technical Specifications

### **Coordinate System**
- **Units**: Meters
- **Y-axis**: Up
- **Player height**: 1.7m eye level
- **Movement speed**: 5.0 m/s
- **Texture scale**: 512px = 1m

### **Rendering**
- **API**: OpenGL 4.6 Core
- **Projection**: Perspective (90° FOV)
- **Depth test**: Enabled
- **Alpha blending**: Enabled for sprites
- **Culling**: Back-face culling for geometry, disabled for sprites

### **Performance Targets**
- 60 FPS minimum
- 1000+ sprites on screen
- Batched sprite rendering
- Frustum culling for sprites

### **File Formats**
- **Sprites**: PNG with alpha channel
- **Textures**: PNG (512x512 for prototype)
- **States**: Custom `.states` text format
- **Audio**: OGG Vorbis (future)
- **Levels**: Custom format (TBD) or OBJ for now

---

## Implementation Priority

**Phase 1: Foundation (CURRENT)**
1. ✅ FPS player controller
2. ✅ Simple square test level with prototype texture
3. ✅ Basic mesh rendering for level

**Phase 2: Sprite System**
4. Billboard sprite renderer
5. Sprite loading (variable size support)
6. Sprite manager/resource system

**Phase 3: Weapons**
7. State machine parser
8. State machine executor (35 Hz)
9. Action registration system
10. Basic weapon implementation (pistol)

**Phase 4: Gameplay**
11. Entity/Component system
12. Player weapon management
13. Input handling for shooting
14. Raycast hit detection

**Phase 5: Advanced**
15. Enemy AI (basic)
16. Health/damage system
17. Particle effects
18. Audio system
19. HUD/UI

---

## Notes

- **Keep it simple initially** - Get one weapon working before building loot system
- **Test incrementally** - Each system should be testable in isolation
- **Voxels on hold** - Don't delete, just ignore for now
- **No atlas required** - Individual sprite files like GZDoom
- **State machine is data** - Not a VM, just data files with C++ callbacks
- **Focus on feel** - Doom's gunplay is all about responsive state machines

---

**Next Step:** Implement FPS player controller and test level
