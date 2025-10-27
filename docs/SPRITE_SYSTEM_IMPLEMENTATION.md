# Sprite System Implementation Plan

## Overview
Building a GZDoom-style sprite system with 8-directional rotation, mirroring, and brightmap support.

---

## Current Asset Structure

### Sprite Assets Location
```
assets/Enemies/
├── Afrit/
│   ├── BMFRITA1.png - BMFRITA5.png (5 rotations)
│   ├── BMFRITB1.png - BMFRITB5.png
│   └── ... (multiple animation frames)
├── Agaures/
│   ├── Sprites/
│   │   ├── AGURA1C1, AGURA2C8, etc. (GZDoom mirroring notation)
│   └── Brightmaps/
│       └── Agaures/
│           └── BMAGURE1, BMAGURE2, etc.
└── [Other enemies...]
```

### Sprite Naming Conventions

**Two formats found:**

1. **Simple format (Afrit):**
   - `BMFRITA1.png` to `BMFRITA5.png`
   - No rotation 0 (front view)
   - Rotations 1-5 only
   - Need automatic mirroring for rotations 6-8

2. **GZDoom format (Agaures):**
   - `AGURA1C1` - Rotation 1, mirrored to rotation C1
   - `AGURA2C8` - Rotation 2, mirrored to rotation 8
   - `AGURA3C7` - Rotation 3, mirrored to rotation 7
   - Explicit mirroring notation in filename

### Rotation Mapping
```
Rotation 0: Front view (0°)
Rotation 1: 45° right
Rotation 2: 90° right (side view)
Rotation 3: 135° right
Rotation 4: Back view (180°)
Rotation 5: 135° left   (mirror of 3, or unique)
Rotation 6: 90° left    (mirror of 2, or unique)
Rotation 7: 45° left    (mirror of 1, or unique)
Rotation 8: Same as rotation 0 (front view)
```

### Brightmaps
- Separate texture files in `Brightmaps/` subdirectory
- Define which pixels should be fullbright (ignore lighting)
- White pixels = fullbright, Black pixels = normal lighting
- Examples: glowing eyes, energy effects, weapon muzzle flashes

---

## Implementation Phases

### Phase 1: Entity/Component System ✅ NEXT
**Goal:** Build foundation for game objects

**Files to Create:**
```
src/entity/
├── entity.h/cpp          - Entity class with built-in Transform
├── component.h           - Component base class
├── entity_manager.h/cpp  - System to manage all entities
```

**Entity Class Structure:**
```cpp
class Entity {
public:
    Transform transform;  // Always present (position, rotation, scale)

    template<typename T>
    T* addComponent();

    template<typename T>
    T* getComponent();

    template<typename T>
    bool hasComponent();

private:
    std::vector<Component*> components;
};
```

**Transform Structure:**
```cpp
struct Transform {
    glm::vec3 position;
    glm::vec3 rotation;  // Euler angles
    glm::vec3 scale;

    glm::mat4 getModelMatrix() const;
};
```

**Components to Create:**
- `SpriteComponent` - Holds sprite data, animation state
- `BillboardComponent` - Billboard mode, facing direction

**Note:** Systems (like BillboardRenderer, EntityManager) are NOT entities

---

### Phase 2: SpriteSet with GZDoom Mirroring ⏳ PENDING
**Goal:** Load and manage 8-directional sprites with automatic mirroring

**Files to Create:**
```
src/sprite/
├── sprite_set.h/cpp  - Manages 8 rotations of a single animation frame
```

**SpriteSet Class:**
```cpp
class SpriteSet {
public:
    Sprite* rotations[8];  // All 8 rotation slots
    bool flipX[8];         // Should this rotation be rendered flipped?

    // Parse GZDoom naming convention
    void loadSprite(const std::string& filepath);

    // Get sprite for viewing angle
    Sprite* getRotationForAngle(float viewAngle, float entityFacing);

private:
    void parseFilename(const std::string& filename, int& rot1, int& rot2);
    void setupMirroring();  // Auto-fill missing rotations
};
```

**Parsing Logic:**
```cpp
// "AGURA2C8.png" -> rotation 2, mirrored to rotation 8
// "BMFRITA1.png" -> rotation 1 only
// "AGASA0" -> rotation 0 only (single rotation)

void parseFilename(filename) {
    // Extract rotation number(s) from filename
    // If format is XXXXYZNZN (like AGURA2C8):
    //   - rotation1 = 2
    //   - rotation2 = 8
    //   - Set flipX[8] = true
    // If format is XXXXY0 (like BMFRITA1):
    //   - rotation1 = 1
    //   - No mirror
}
```

**Automatic Mirroring (for 5-rotation sprites):**
```cpp
// If only rotations 1-5 exist, auto-mirror:
if (rotations[1] && !rotations[7]) {
    rotations[7] = rotations[1];
    flipX[7] = true;
}
if (rotations[2] && !rotations[6]) {
    rotations[6] = rotations[2];
    flipX[6] = true;
}
if (rotations[3] && !rotations[5]) {
    rotations[5] = rotations[3];
    flipX[5] = true;
}
```

**Angle Calculation:**
```cpp
Sprite* getRotationForAngle(float viewAngle, float entityFacing) {
    // Calculate relative angle between camera and entity facing
    float relativeAngle = viewAngle - entityFacing;

    // Normalize to 0-360
    while (relativeAngle < 0) relativeAngle += 360.0f;
    while (relativeAngle >= 360) relativeAngle -= 360.0f;

    // Map to 8 rotations (45° per rotation)
    int rotation = (int)((relativeAngle + 22.5f) / 45.0f) % 8;

    return rotations[rotation];
}
```

---

### Phase 3: Update Billboard Renderer ⏳ PENDING
**Goal:** Render entities with correct sprite rotation

**Changes to BillboardRenderer:**
```cpp
// Old: Draw single sprite
void drawSprite(Sprite* sprite, ...);

// New: Draw entity with rotation
void drawEntity(Entity* entity, Camera& camera, float aspectRatio);
```

**Add to billboard.frag:**
```glsl
uniform bool flipX;  // Should sprite be horizontally flipped?

void main() {
    vec2 texCoord = TexCoord;

    // Flip texture coordinates if needed
    if (flipX) {
        texCoord.x = 1.0 - texCoord.x;
    }

    vec4 texColor = texture(spriteTexture, texCoord);

    if (texColor.a < 0.1)
        discard;

    FragColor = texColor * spriteColor;
}
```

---

### Phase 4: Hardcoded Test ⏳ PENDING
**Goal:** Get ONE enemy sprite working with 8-way rotation

**Test Setup:**
```cpp
// In Application::init()
Entity* testEnemy = entityManager->createEntity();
testEnemy->transform.position = glm::vec3(0, 1, -5);
testEnemy->transform.rotation.y = 0.0f;  // Facing forward

auto* sprite = testEnemy->addComponent<SpriteComponent>();
sprite->spriteSet = new SpriteSet();

// Load Afrit idle animation (frame A)
sprite->spriteSet->loadSprite("assets/Enemies/Afrit/BMFRITA1.png");
sprite->spriteSet->loadSprite("assets/Enemies/Afrit/BMFRITA2.png");
sprite->spriteSet->loadSprite("assets/Enemies/Afrit/BMFRITA3.png");
sprite->spriteSet->loadSprite("assets/Enemies/Afrit/BMFRITA4.png");
sprite->spriteSet->loadSprite("assets/Enemies/Afrit/BMFRITA5.png");

// In render loop:
for (Entity* entity : entityManager->getEntitiesWithComponent<SpriteComponent>()) {
    billboardRenderer->drawEntity(entity, *camera, aspectRatio);
}
```

**Test:** Walk around the enemy in a circle - should see different angles

---

### Phase 5: Brightmap Support 🔮 FUTURE
**Goal:** Add glowing effects to sprites

**Implementation:**
1. **Load brightmap alongside main texture**
   ```cpp
   class Sprite {
       Texture* mainTexture;
       Texture* brightmapTexture;  // Optional
   };
   ```

2. **Update shader to use brightmap**
   ```glsl
   uniform sampler2D spriteTexture;
   uniform sampler2D brightmapTexture;
   uniform bool hasBrightmap;

   void main() {
       vec4 texColor = texture(spriteTexture, texCoord);

       if (hasBrightmap) {
           float brightValue = texture(brightmapTexture, texCoord).r;
           // Mix lit and unlit based on brightmap
           vec3 litColor = texColor.rgb * lighting;
           vec3 unlitColor = texColor.rgb;
           texColor.rgb = mix(litColor, unlitColor, brightValue);
       }

       FragColor = texColor;
   }
   ```

3. **Auto-load brightmaps**
   ```cpp
   // When loading "assets/Enemies/Agaures/Sprites/AGURA1C1"
   // Also try to load "assets/Enemies/Agaures/Brightmaps/Agaures/BMAGURE1"
   ```

---

### Phase 6: State Machine Integration 🔮 FUTURE
**Goal:** Control sprite animations through state machine

**State machine controls:**
- Which SpriteSet to use (idle vs walking vs attacking)
- Animation frame timing (35 Hz tick rate)
- Transitions between animation states

**Example:**
```
Idle State {
    BMFRITA, 10  // Frame A for 10 tics
    BMFRITB, 10  // Frame B for 10 tics
    Loop
}

Walking State {
    BMFRITC, 4
    BMFRITD, 4
    BMFRITE, 4
    BMFRITF, 4
    Loop
}
```

---

## Current Task List

### Immediate (This Session):
- [ ] Create Entity/Component system
  - [ ] `entity.h/cpp` - Entity class with Transform
  - [ ] `component.h` - Component base class
  - [ ] `entity_manager.h/cpp` - Entity management system
  - [ ] Core components: `SpriteComponent`, `BillboardComponent`

### Next (After Entity System):
- [ ] Create SpriteSet class with GZDoom naming support
- [ ] Implement sprite rotation angle calculation
- [ ] Update BillboardRenderer to work with entities
- [ ] Add flipX support to billboard shader
- [ ] Hardcoded test with one enemy sprite

### Later:
- [ ] Brightmap support
- [ ] State machine system for animation control
- [ ] Sprite manager for resource management
- [ ] Multiple enemy types
- [ ] Weapon sprites (HUD-space rendering)

---

## Technical Notes

### Sprite Rendering Pipeline
```
1. EntityManager loops through entities with SpriteComponent
2. Calculate angle between camera and entity
3. SpriteSet selects correct rotation (0-7)
4. Check if rotation needs horizontal flip
5. BillboardRenderer draws sprite with flipX flag
6. Shader applies flip if needed
7. Billboard faces camera (Y-axis locked for enemies)
```

### Performance Considerations
- Batch rendering (future optimization)
- Sprite depth sorting
- Frustum culling for off-screen sprites
- Texture atlas for sprite sheets (optional)

### Compatibility
- Follow GZDoom sprite naming where possible
- Support both explicit mirroring (AGURA2C8) and implicit (BMFRITA1-5)
- Handle missing rotations gracefully (single-rotation sprites)

---

## References
- GZDoom Wiki: https://zdoom.org/wiki/Sprite
- Doom Sprite Format: https://doomwiki.org/wiki/Sprite
- Billboard Rendering: Camera-facing quads in 3D space

---

**Last Updated:** 2025-10-27
**Status:** Phase 1 (Entity/Component System) - Ready to implement
