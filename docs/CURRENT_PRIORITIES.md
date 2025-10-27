# 🎯 Current Priorities - Path of Doom

**Last Updated:** 2025-10-27
**Current Phase:** ECS Architecture Refactoring
**Blocking Issue:** Application class is monolithic, not using EnTT properly

---

## 🚨 The Problem

You just migrated from **flecs to EnTT**, but your codebase isn't actually using ECS properly yet. Your `Application` class ([application.cpp](../src/application/application.cpp)) is still a **god object** that does everything:

### What's Wrong:
1. **Only 1 test entity** - Your EnTT registry has ONE sprite entity doing nothing
2. **Hardcoded input** - Lines 240-273: WASD keys directly modify camera
3. **Manual render loop** - Lines 163-194: Application manually calls drawSprite for each entity
4. **No systems** - Application.cpp is 312 lines of monolithic logic
5. **Can't scale** - Try adding 10 enemies right now... you'd have to modify Application

### Why This Matters:
- You can't build state machines without proper ECS systems
- You can't add weapons/enemies easily
- Your code will become unmaintainable fast
- EnTT is sitting there unused

---

## ✅ What to Do RIGHT NOW

### **Priority 1: ECS Systems Refactoring** (3-4 hours)

Extract the monolithic Application into proper ECS systems. This is **blocking** everything else.

#### **Step 0: Create InputState in EnTT Context** (~15 min)

**Goal:** Store input state in registry context instead of passing Window* to systems

**Files to Create:**
```
src/ecs/input_state.h
```

**What It Does:**
- Stores current keyboard/mouse state
- Stored in EnTT registry context (not on any entity)
- Application updates it, systems read from it
- No need to pass Window* to every system

**Code:**
```cpp
// src/ecs/input_state.h
#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct InputState {
    // Keyboard state (indexed by GLFW_KEY_* constants)
    bool keys[GLFW_KEY_LAST] = {false};

    // Mouse state
    glm::vec2 mousePosition = glm::vec2(0.0f);
    glm::vec2 mouseDelta = glm::vec2(0.0f);
    bool mouseButtons[8] = {false};
    float scrollDelta = 0.0f;

    // Mouse tracking (for delta calculation)
    bool firstMouse = true;
    float lastX = 0.0f;
    float lastY = 0.0f;

    // Helper methods
    bool isKeyPressed(int key) const {
        if (key < 0 || key >= GLFW_KEY_LAST) return false;
        return keys[key];
    }

    bool isMouseButtonPressed(int button) const {
        if (button < 0 || button >= 8) return false;
        return mouseButtons[button];
    }
};

// Also add DeltaTime while we're at it
using DeltaTime = float;
```

**In Application::init():**
```cpp
// Create InputState in registry context
m_registry.ctx().emplace<InputState>();
m_registry.ctx().emplace<DeltaTime>(0.0f);
```

**In Application::run():**
```cpp
void Application::run() {
    while (!m_window->shouldClose()) {
        // Update timing
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;

        // Update context
        m_registry.ctx().get<DeltaTime>() = deltaTime;

        // Handle cursor lock toggle
        handleCursorToggle();

        // Update input state from Window (if cursor locked)
        if (m_cursorLocked) {
            updateInputState();
        }

        // Update ECS systems (no Window* or deltaTime params!)
        m_inputSystem.update(m_registry);
        m_cameraSystem.update(m_registry);

        // Render...
    }
}

void Application::updateInputState() {
    auto& inputState = m_registry.ctx().get<InputState>();

    // Update keyboard state
    for (int i = 0; i < GLFW_KEY_LAST; i++) {
        inputState.keys[i] = glfwGetKey(m_window->getHandle(), i) == GLFW_PRESS;
    }

    // Update mouse delta
    double xpos, ypos;
    glfwGetCursorPos(m_window->getHandle(), &xpos, &ypos);

    if (inputState.firstMouse) {
        inputState.lastX = static_cast<float>(xpos);
        inputState.lastY = static_cast<float>(ypos);
        inputState.firstMouse = false;
    }

    inputState.mouseDelta = glm::vec2(
        xpos - inputState.lastX,
        inputState.lastY - ypos  // Reversed: y-coordinates go from bottom to top
    );

    inputState.lastX = static_cast<float>(xpos);
    inputState.lastY = static_cast<float>(ypos);

    // Update mouse buttons
    for (int i = 0; i < 8; i++) {
        inputState.mouseButtons[i] = glfwGetMouseButton(m_window->getHandle(), i) == GLFW_PRESS;
    }
}
```

**Result:**
- InputState stored in registry context
- Application can remove: `m_firstMouse`, `m_lastX`, `m_lastY`, `m_deltaTime` members
- Systems don't need Window* parameter

---

#### **Step 1: Create InputSystem** (~30 min)

**Goal:** Read from InputState and populate InputComponent on entities

**Files to Create:**
```
src/ecs/systems/input_system.h
src/ecs/systems/input_system.cpp
```

**What It Does:**
- Reads from `InputState` in registry context
- Populates `InputComponent` on entities (player)
- No Window* dependency!

**Code Structure:**
```cpp
// components.h - ADD THIS
struct InputComponent {
    glm::vec2 moveDirection = glm::vec2(0.0f);  // WASD movement
    glm::vec2 lookDelta = glm::vec2(0.0f);      // Mouse movement
    bool jumpPressed = false;
    bool firePressed = false;
};

// input_system.h
#pragma once
#include <entt/entt.hpp>

class InputSystem {
public:
    void update(entt::registry& registry);  // No Window* needed!
};

// input_system.cpp
#include "input_system.h"
#include "input_state.h"
#include "components.h"
#include <GLFW/glfw3.h>

void InputSystem::update(entt::registry& registry) {
    // Get InputState from context
    auto& inputState = registry.ctx().get<InputState>();

    // Update all entities with InputComponent
    auto view = registry.view<InputComponent>();

    for (auto entity : view) {
        auto& input = view.get<InputComponent>(entity);

        // Read WASD keys
        input.moveDirection = glm::vec2(0.0f);
        if (inputState.isKeyPressed(GLFW_KEY_W)) input.moveDirection.y += 1.0f;
        if (inputState.isKeyPressed(GLFW_KEY_S)) input.moveDirection.y -= 1.0f;
        if (inputState.isKeyPressed(GLFW_KEY_A)) input.moveDirection.x -= 1.0f;
        if (inputState.isKeyPressed(GLFW_KEY_D)) input.moveDirection.x += 1.0f;

        // Optional: Normalize diagonal movement
        if (glm::length(input.moveDirection) > 0.0f) {
            input.moveDirection = glm::normalize(input.moveDirection);
        }

        // Read mouse delta
        input.lookDelta = inputState.mouseDelta;

        // Read jump/fire
        input.jumpPressed = inputState.isKeyPressed(GLFW_KEY_SPACE);
        input.firePressed = inputState.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);
    }
}
```

**Result:**
- Application no longer has processInput() method
- Systems are decoupled from Window
- Easy to test (just set InputState values)

---

#### **Step 2: Create CameraSystem** (~30 min)

**Goal:** Make Camera an entity component, controlled by InputComponent

**Files to Modify:**
```
src/components/components.h  (add CameraComponent)
src/ecs/systems/camera_system.h
src/ecs/systems/camera_system.cpp
```

**CameraComponent:**
```cpp
// components.h - ADD THIS
struct CameraComponent {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float yaw = -90.0f;
    float pitch = 0.0f;
    float fov = 90.0f;
    float movementSpeed = 5.0f;
    float mouseSensitivity = 0.1f;
};
```

**CameraSystem:**
```cpp
class CameraSystem {
public:
    void update(entt::registry& registry, float deltaTime);
};

void CameraSystem::update(entt::registry& registry, float deltaTime) {
    auto view = registry.view<CameraComponent, InputComponent>();

    for (auto entity : view) {
        auto& camera = view.get<CameraComponent>(entity);
        auto& input = view.get<InputComponent>(entity);

        // Update camera position based on input
        glm::vec3 right = glm::normalize(glm::cross(camera.front, camera.up));

        camera.position += camera.front * input.moveDirection.y * camera.movementSpeed * deltaTime;
        camera.position += right * input.moveDirection.x * camera.movementSpeed * deltaTime;

        // Update camera rotation based on mouse
        camera.yaw += input.lookDelta.x * camera.mouseSensitivity;
        camera.pitch += input.lookDelta.y * camera.mouseSensitivity;

        // Clamp pitch
        if (camera.pitch > 89.0f) camera.pitch = 89.0f;
        if (camera.pitch < -89.0f) camera.pitch = -89.0f;

        // Update front vector
        glm::vec3 direction;
        direction.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        direction.y = sin(glm::radians(camera.pitch));
        direction.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        camera.front = glm::normalize(direction);
    }
}
```

**Player Entity Creation:**
```cpp
// In Application constructor, replace old camera:
auto player = m_registry.create();
auto& camera = m_registry.emplace<CameraComponent>(player);
camera.position = glm::vec3(0.0f, 1.7f, 0.0f);
camera.movementSpeed = 5.0f;

auto& input = m_registry.emplace<InputComponent>(player);
m_registry.emplace<Player>(player);  // Player tag
```

**Result:** Camera is now an entity component, not a unique_ptr in Application

---

#### **Step 3: Create RenderSystem** (~1 hour)

**Goal:** Move rendering logic into ECS system

**Files to Create:**
```
src/ecs/systems/render_system.h
src/ecs/systems/render_system.cpp
```

**RenderSystem:**
```cpp
class RenderSystem {
public:
    void render(entt::registry& registry, Renderer* renderer,
                BillboardRenderer* billboardRenderer, Window* window);

private:
    void renderLevelGeometry(entt::registry& registry, Renderer* renderer, Window* window);
    void renderSprites(entt::registry& registry, BillboardRenderer* billboardRenderer, Window* window);
};

void RenderSystem::render(entt::registry& registry, Renderer* renderer,
                          BillboardRenderer* billboardRenderer, Window* window) {
    renderer->clear();
    glEnable(GL_DEPTH_TEST);

    renderLevelGeometry(registry, renderer, window);
    renderSprites(registry, billboardRenderer, window);
}

void RenderSystem::renderSprites(entt::registry& registry,
                                  BillboardRenderer* billboardRenderer,
                                  Window* window) {
    // Get camera from player entity
    auto cameraView = registry.view<CameraComponent, Player>();
    if (cameraView.empty()) return;

    auto cameraEntity = cameraView.front();
    auto& camera = cameraView.get<CameraComponent>(cameraEntity);

    // Convert CameraComponent to old Camera for now (temporary)
    Camera tempCamera(camera.position);
    tempCamera.Front = camera.front;
    tempCamera.Up = camera.up;
    tempCamera.Yaw = camera.yaw;
    tempCamera.Pitch = camera.pitch;

    // Render all sprite entities
    auto spriteView = registry.view<Transform, SpriteComponent, BillboardComponent>();
    for (auto entity : spriteView) {
        auto& transform = spriteView.get<Transform>(entity);
        auto& sprite = spriteView.get<SpriteComponent>(entity);
        auto& billboard = spriteView.get<BillboardComponent>(entity);

        if (sprite.sprite && sprite.sprite->isLoaded()) {
            billboardRenderer->drawSprite(
                sprite.sprite,
                transform.position,
                tempCamera,
                window->getAspectRatio(),
                billboard.mode,
                transform.scale,
                sprite.color
            );
        }
    }
}
```

**Result:** Application.render() is now just `m_renderSystem.render(...)`

---

#### **Step 4: Refactor Application** (~1 hour)

**Goal:** Application becomes a thin shell that owns systems

**New Application Structure:**
```cpp
// application.h
class Application {
private:
    // Core systems (not ECS, just infrastructure)
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<ImGuiLayer> m_imguiLayer;
    std::unique_ptr<BillboardRenderer> m_billboardRenderer;

    // ECS registry
    entt::registry m_registry;

    // ECS Systems
    InputSystem m_inputSystem;
    CameraSystem m_cameraSystem;
    RenderSystem m_renderSystem;

    // Test assets (temporary)
    std::unique_ptr<Mesh> m_testLevel;
    std::unique_ptr<Texture> m_prototypeTexture;
    std::unique_ptr<Sprite> m_testSprite;

    // Timing
    float m_deltaTime;
    float m_lastFrame;

    // Input state (for cursor lock, etc.)
    bool m_cursorLocked;
    bool m_tabKeyPressed;

    void init();
    void setupCallbacks();
    void createScene();
    void createPlayerEntity();
    void createTestEntities();
};

// application.cpp - run() becomes:
void Application::run() {
    while (!m_window->shouldClose()) {
        // Update timing
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;

        // Update context
        m_registry.ctx().get<DeltaTime>() = deltaTime;

        // Handle cursor lock (still in Application for now)
        handleCursorToggle();

        // Update input state from Window
        if (m_cursorLocked) {
            updateInputState();
        }

        // Update ECS systems (no parameters needed!)
        m_inputSystem.update(m_registry);
        m_cameraSystem.update(m_registry);

        // Render
        m_imguiLayer->beginFrame();
        m_renderSystem.render(m_registry, m_renderer.get(),
                              m_billboardRenderer.get(), m_window.get());
        renderUI();
        m_imguiLayer->endFrame();

        m_window->swapBuffers();
        m_window->pollEvents();
    }
}
```

**Result:** Application.cpp shrinks from 312 lines to ~200 lines, mostly setup

---

#### **Step 5: Add Multiple Test Entities** (~30 min)

**Goal:** Prove the system works with multiple entities

**Add to createTestEntities():**
```cpp
void Application::createTestEntities() {
    // Create 5 test sprites in a circle
    for (int i = 0; i < 5; i++) {
        auto entity = m_registry.create();

        auto& transform = m_registry.emplace<Transform>(entity);
        float angle = (i / 5.0f) * 2.0f * 3.14159f;
        transform.position = glm::vec3(
            cos(angle) * 3.0f,
            1.0f,
            sin(angle) * 3.0f
        );

        auto& sprite = m_registry.emplace<SpriteComponent>(entity);
        sprite.sprite = m_testSprite.get();
        sprite.color = glm::vec4(
            (i % 3 == 0) ? 1.0f : 0.5f,
            (i % 3 == 1) ? 1.0f : 0.5f,
            (i % 3 == 2) ? 1.0f : 0.5f,
            1.0f
        );

        auto& billboard = m_registry.emplace<BillboardComponent>(entity);
        billboard.mode = BillboardMode::YAxisLocked;
        billboard.scale = 1.0f;

        // Tag as enemy for testing
        m_registry.emplace<Enemy>(entity);
    }

    std::cout << "Created 5 test sprite entities\n";
}
```

**Result:** You should see 5 colored sprites in a circle when you run the game

---

## 📋 Checklist

Once you complete the above, you should have:

**New Files Created:**
- [ ] `src/ecs/input_state.h` - InputState struct + DeltaTime typedef
- [ ] `src/ecs/systems/input_system.h` - InputSystem header
- [ ] `src/ecs/systems/input_system.cpp` - InputSystem implementation
- [ ] `src/ecs/systems/camera_system.h` - CameraSystem header
- [ ] `src/ecs/systems/camera_system.cpp` - CameraSystem implementation
- [ ] `src/ecs/systems/render_system.h` - RenderSystem header
- [ ] `src/ecs/systems/render_system.cpp` - RenderSystem implementation

**Components Added (components.h):**
- [ ] `InputComponent` - Player input data
- [ ] `CameraComponent` - Camera state (position, rotation, FOV)

**Application Changes:**
- [ ] `InputState` created in registry context (init())
- [ ] `DeltaTime` created in registry context (init())
- [ ] `updateInputState()` method added to Application
- [ ] Systems created as members: `InputSystem`, `CameraSystem`, `RenderSystem`
- [ ] `run()` method refactored to use systems
- [ ] Player entity created with Camera + Input components
- [ ] Removed members: `m_camera`, `m_firstMouse`, `m_lastX`, `m_lastY`, `m_deltaTime`

**Testing:**
- [ ] 5 test sprite entities visible in game
- [ ] Application.cpp reduced to ~200 lines
- [ ] Camera movement still works via WASD/mouse
- [ ] Can add new entities easily (3 lines of code)

---

## 🎯 What Comes After This

Once ECS systems are working properly, you can tackle:

### **Phase 3: State Machines** (Next Big Feature)
With proper ECS in place, you can build:
1. State machine parser (`.states` files)
2. StateMachineComponent
3. StateMachineSystem (runs at 35 Hz)
4. Action executor (FireBullets, PlaySound, etc.)

### **Phase 4: Weapons**
5. WeaponComponent
6. Test pistol with fire/reload states
7. Raycast hit detection

### **Phase 5: Enemies**
8. Enemy state machines (idle, chase, attack)
9. Health system
10. Damage dealing

---

## 🚀 Why This Order?

**1. ECS Systems First (Current)**
- **Blocks:** Everything else
- **Benefit:** Clean architecture, scalable
- **Time:** 3-4 hours

**2. State Machines Second**
- **Blocks:** Weapons, enemies, animations
- **Benefit:** Data-driven gameplay
- **Time:** 4-6 hours

**3. Weapons Third**
- **Blocks:** Nothing, but makes the game fun
- **Benefit:** Player can shoot
- **Time:** 3-4 hours

**4. Enemies Fourth**
- **Blocks:** Nothing
- **Benefit:** Something to shoot at
- **Time:** 4-5 hours

---

## 📁 Directory Structure After Refactoring

```
src/
├── application/
│   ├── application.h         # Thin shell, owns systems
│   └── application.cpp        # ~200 lines
├── components/
│   └── components.h           # All ECS components
├── ecs/
│   └── systems/
│       ├── input_system.h/cpp
│       ├── camera_system.h/cpp
│       └── render_system.h/cpp
├── camera/
│   ├── camera.h               # OLD - keep for now (temporary)
│   └── camera.cpp
└── [rest of existing structure]
```

---

## 🧠 Understanding InputState Architecture

**Why use EnTT context for InputState?**

```
┌─────────────────────────────────────────┐
│         Application (owns Window)       │
│                                         │
│  updateInputState() {                  │
│    // Poll GLFW                        │
│    registry.ctx().get<InputState>()    │
│    // Update state                     │
│  }                                      │
└────────────┬────────────────────────────┘
             │ Updates
             ▼
┌─────────────────────────────────────────┐
│     EnTT Registry Context               │
│                                         │
│  InputState (keys[], mouseDelta, etc.)  │
│  DeltaTime (0.016f)                     │
│                                         │
└────────────┬────────────────────────────┘
             │ Read by
             ▼
┌─────────────────────────────────────────┐
│           InputSystem                   │
│                                         │
│  update(registry) {                     │
│    auto& state = registry.ctx()...     │
│    // Process into InputComponents      │
│  }                                      │
└─────────────────────────────────────────┘
```

**Benefits:**
- ✅ Systems don't depend on Window (testable!)
- ✅ All game state in one place (registry)
- ✅ Easy to serialize for replays/netcode
- ✅ Clean separation: Application handles platform, systems handle game logic

---

## 🛑 Common Mistakes to Avoid

1. **Don't skip InputState** - "I'll just pass Window* to systems" → NO, systems shouldn't know about platform APIs
2. **Don't pass deltaTime as parameter** - Use `DeltaTime` in context instead
3. **Don't keep old Camera class in Application** - Move it to CameraComponent on player entity
4. **Don't hardcode entities in Application** - Create a `createTestEntities()` function
5. **Don't forget to add multiple test entities** - ONE entity doesn't prove the system works

---

## 💡 Quick Start Commands

```bash
# 1. Create ECS directory structure
mkdir -p src/ecs/systems

# 2. Create InputState header
touch src/ecs/input_state.h

# 3. Create system files
touch src/ecs/systems/input_system.h
touch src/ecs/systems/input_system.cpp
touch src/ecs/systems/camera_system.h
touch src/ecs/systems/camera_system.cpp
touch src/ecs/systems/render_system.h
touch src/ecs/systems/render_system.cpp

# 4. Add to CMakeLists.txt ENGINE_SOURCES:
# src/ecs/systems/input_system.cpp
# src/ecs/systems/camera_system.cpp
# src/ecs/systems/render_system.cpp

# 5. Add to CMakeLists.txt ENGINE_HEADERS:
# src/ecs/input_state.h
# src/ecs/systems/input_system.h
# src/ecs/systems/camera_system.h
# src/ecs/systems/render_system.h

# 6. Add include directory to CMakeLists.txt:
# ${CMAKE_CURRENT_SOURCE_DIR}/src/ecs
# ${CMAKE_CURRENT_SOURCE_DIR}/src/ecs/systems

# 7. Build and test
cmake --build build --config Debug
./build/Debug/PodEngine.exe
```

---

## 📊 Estimated Timeline

| Task | Time | Priority |
|------|------|----------|
| Step 0: InputState + DeltaTime | 15 min | 🔴 Critical |
| Step 1: InputSystem | 30 min | 🔴 Critical |
| Step 2: CameraSystem | 30 min | 🔴 Critical |
| Step 3: RenderSystem | 1 hour | 🔴 Critical |
| Step 4: Refactor Application | 1 hour | 🔴 Critical |
| Step 5: Add test entities | 30 min | 🟡 Important |
| **Total** | **3.5-4 hours** | |

---

## 🎓 Learning Goals

By completing this refactoring, you'll learn:
- ✅ How to properly structure ECS systems
- ✅ Component-based architecture patterns
- ✅ Separation of concerns (input, logic, rendering)
- ✅ How to scale to 100+ entities easily
- ✅ Foundation for data-driven gameplay

---

## ✨ Success Criteria

You know you're done when:
1. ✅ You can add a new entity with 3 lines of code
2. ✅ Application.cpp is < 250 lines
3. ✅ 5+ sprites render correctly in the scene
4. ✅ Camera still responds to WASD + mouse
5. ✅ ImGui still shows debug info

---

**Get this done, then we build state machines!** 🚀
