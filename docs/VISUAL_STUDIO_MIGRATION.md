# 🚀 Visual Studio Migration Guide

**Last Updated:** 2025-10-27
**Goal:** Migrate from CMake to Visual Studio with clean folder-based organization
**Approach:** Single project with Engine/Game separation via folders (NOT separate projects)

---

## 🎯 Philosophy: One Project, Clean Folders

### **Why NOT Separate Projects?**

**You're building ONE game (Path of Doom), not multiple games.**

❌ **Don't do this:**
```
Engine.lib      (Separate project)
PathOfDoom.exe  (Separate project, links to Engine.lib)
```

✅ **Do this instead:**
```
PathOfDoom.exe  (Single project, organized with folders)
├── Core/       (Folder - "engine" code)
├── Renderer/   (Folder - "engine" code)
├── Game/       (Folder - game-specific code)
└── Assets/     (Folder - sprites, textures, states)
```

---

## 📊 Benefits of Single Project

| Aspect | Separate Projects | Single Project (Folders) |
|--------|-------------------|--------------------------|
| **Build Speed** | Slower (build lib, then exe) | Faster (build once) |
| **Complexity** | High (linking, include paths) | Low (just add files) |
| **Iteration** | 2-step rebuild | 1-step rebuild |
| **Organization** | Clean separation | Clean separation (same!) |
| **Focus** | Architecture | Game development |
| **When to Use** | Multiple games | One game (your case) |

**Folders give you the SAME organization without the complexity.**

---

## 🗂️ Recommended Folder Structure

```
PathOfDoom/
├── PathOfDoom.sln              # Visual Studio Solution
├── PathOfDoom.vcxproj          # Single Project (All code here)
├── PathOfDoom.vcxproj.filters  # Folder organization
│
├── src/
│   ├── Core/                   # "Engine" - Application infrastructure
│   │   ├── Application.h/cpp
│   │   ├── Window.h/cpp
│   │   └── EntryPoint.h
│   │
│   ├── RHI/                    # "Engine" - Rendering Hardware Interface
│   │   ├── RenderDevice.h      # Abstract render API interface
│   │   ├── RenderTypes.h       # API-agnostic types (handles, descriptors)
│   │   ├── Resources.h         # GPU resource handles
│   │   └── OpenGL/             # OpenGL backend implementation
│   │       ├── OpenGLDevice.h/cpp
│   │       ├── OpenGLBuffer.h/cpp
│   │       └── OpenGLTexture.h/cpp
│   │
│   ├── Renderer/               # "Engine" - High-level rendering systems
│   │   ├── Renderer.h/cpp      # Uses RHI::RenderDevice
│   │   ├── Shader.h/cpp
│   │   ├── Mesh.h/cpp
│   │   ├── Texture.h/cpp
│   │   └── BillboardRenderer.h/cpp
│   │
│   ├── ECS/                    # "Engine" - ECS foundation
│   │   ├── Components.h
│   │   ├── InputState.h
│   │   └── Systems/
│   │       ├── InputSystem.h/cpp
│   │       ├── CameraSystem.h/cpp
│   │       └── RenderSystem.h/cpp
│   │
│   ├── Platform/               # "Engine" - Platform abstraction
│   │   └── GLFW/
│   │       └── Window implementation
│   │
│   ├── Game/                   # Game-specific code
│   │   ├── PathOfDoomApp.h/cpp
│   │   ├── Systems/
│   │   │   ├── WeaponSystem.h/cpp
│   │   │   ├── EnemySystem.h/cpp
│   │   │   ├── LootSystem.h/cpp
│   │   │   └── HealthSystem.h/cpp
│   │   ├── StateMachine/
│   │   │   ├── StateMachine.h/cpp
│   │   │   ├── StateExecutor.h/cpp
│   │   │   └── ActionRegistry.h/cpp
│   │   └── Components/
│   │       ├── WeaponComponent.h
│   │       ├── EnemyComponent.h
│   │       └── LootComponent.h
│   │
│   ├── Utilities/              # Shared utilities
│   │   ├── FileIO.h/cpp
│   │   └── Math.h
│   │
│   └── main.cpp                # Entry point
│
├── Assets/
│   ├── textures/
│   ├── sprites/
│   ├── states/                 # .states files
│   └── levels/
│
├── shaders/
│   ├── basic.vert
│   ├── basic.frag
│   ├── billboard.vert
│   └── billboard.frag
│
└── external/                   # Third-party libraries
    ├── glad/
    ├── glfw/
    ├── glm/
    ├── entt/
    └── imgui/
```

---

## 🎨 Code Organization Principles

### **What Goes Where?**

#### **Core/ - Application Infrastructure**
"Could another game use this?"
- Application base class
- Window management
- Input handling (platform level)
- Timing/delta time

**Example:**
```cpp
// Core/Application.h
class Application {
public:
    Application();
    virtual ~Application();

    void Run();

protected:
    virtual void OnInit() = 0;
    virtual void OnUpdate(float deltaTime) = 0;
    virtual void OnRender() = 0;
};
```

---

#### **RHI/ - Rendering Hardware Interface**
"Is this graphics API abstraction?"
- **RenderDevice** - Abstract interface for graphics APIs
- **RenderTypes** - API-agnostic handles and descriptors
- **OpenGL/** - OpenGL implementation (backend)

**Purpose:** Decouple game code from OpenGL so we can add Vulkan/DX12 later.

**Example:**
```cpp
// RHI/RenderDevice.h
namespace RHI {
    class RenderDevice {
    public:
        virtual ~RenderDevice() = default;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Clear(ClearFlags flags, const glm::vec4& color) = 0;
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        virtual BufferHandle CreateBuffer(const BufferDesc& desc, const void* data = nullptr) = 0;
        virtual TextureHandle CreateTexture(const TextureDesc& desc, const void* data = nullptr) = 0;
        virtual ShaderHandle CreateShader(const ShaderDesc& desc) = 0;

        virtual void DrawIndexed(uint32_t indexCount, uint32_t startIndex = 0) = 0;
    };

    // Factory function to create platform-specific device
    RenderDevice* CreateRenderDevice();
}

// RHI/OpenGL/OpenGLDevice.h
namespace RHI {
    class OpenGLDevice : public RenderDevice {
    public:
        void Clear(ClearFlags flags, const glm::vec4& color) override;
        BufferHandle CreateBuffer(const BufferDesc& desc, const void* data) override;
        // ... implements all virtual methods

    private:
        std::unordered_map<uint32_t, GLuint> m_buffers;
        std::unordered_map<uint32_t, GLuint> m_textures;
        uint32_t m_nextBufferId = 1;
    };
}
```

**Benefits:**
- Game code doesn't know about OpenGL
- Can swap to Vulkan/DX12 by implementing new backend
- Still using OpenGL now (low risk)
- Clean architecture for future

---

#### **Renderer/ - High-Level Rendering Systems**
"Is this about drawing game objects?"
- **Renderer** - Uses RHI::RenderDevice (no direct OpenGL calls)
- Shader loading/compilation
- Mesh/Texture management
- Billboard renderer

**Example:**
```cpp
// Renderer/Renderer.h
class Renderer {
public:
    Renderer(RHI::RenderDevice* device);

    void Clear();
    void DrawMesh(const Mesh& mesh, const Shader& shader);
    void SetViewport(int x, int y, int width, int height);

private:
    RHI::RenderDevice* m_device;  // Uses RHI, not OpenGL directly
};

// Renderer/Renderer.cpp
void Renderer::Clear() {
    m_device->Clear(RHI::ClearFlags::Both, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
}
```

---

#### **ECS/ - Entity Component System Foundation**
"Is this core ECS infrastructure?"
- Component definitions (Transform, Velocity, Sprite)
- Base systems (Input, Camera, Render)
- Registry wrapper (if needed)
- InputState (global state)

**Example:**
```cpp
// ECS/Components.h
struct Transform {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

struct SpriteComponent {
    Sprite* sprite;
    glm::vec4 color;
};
```

---

#### **Game/ - Path of Doom Specific**
"Is this unique to Path of Doom?"
- PathOfDoomApp (game entry point)
- Weapon system
- Enemy AI
- Loot generation
- State machines (weapon/enemy behavior)
- Game-specific components (Weapon, Ammo, Health)

**Example:**
```cpp
// Game/PathOfDoomApp.h
#include "Core/Application.h"

class PathOfDoomApp : public Application {
protected:
    void OnInit() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;

private:
    void LoadLevel(const std::string& levelName);
    void CreatePlayer();
};

// Game/Systems/WeaponSystem.h
class WeaponSystem {
public:
    void Update(entt::registry& registry, float deltaTime);
};
```

---

## 🔧 Visual Studio Project Setup

### **Step 1: Create Solution**

1. Open Visual Studio 2022
2. "Create a new project"
3. Select "Empty Project" or "Console App"
4. Name: `PathOfDoom`
5. Location: `D:\projects\C++\`
6. ✅ "Place solution and project in the same directory"

### **Step 2: Project Settings**

**Configuration:** Debug / Release
**Platform:** x64

#### **General Settings:**
```
Configuration Type: Application (.exe)
Output Directory: $(SolutionDir)bin\$(Configuration)-$(Platform)\
Intermediate Directory: $(SolutionDir)bin-int\$(Configuration)-$(Platform)\
C++ Language Standard: C++17
```

#### **C/C++ Settings:**
```
Additional Include Directories:
    $(SolutionDir)src
    $(SolutionDir)external\glad\include
    $(SolutionDir)external\glfw\include
    $(SolutionDir)external\glm
    $(SolutionDir)external\entt\include
    $(SolutionDir)external\imgui
    $(SolutionDir)external\stb

Preprocessor Definitions (Debug):
    _DEBUG
    _CONSOLE
    GLFW_INCLUDE_NONE

Preprocessor Definitions (Release):
    NDEBUG
    _CONSOLE
    GLFW_INCLUDE_NONE

Warnings Level: Level4 (/W4)
Treat Warnings As Errors: No (for now)
```

#### **Linker Settings:**
```
Additional Library Directories:
    $(SolutionDir)external\glfw\lib-vc2022
    (Add others as needed)

Additional Dependencies:
    glfw3.lib
    opengl32.lib
    (Add others as needed)
```

---

## 📦 Migrating Code from CMake

### **Step 1: Copy Source Files**

```bash
# From old structure:
src/application/application.h/cpp
src/window/window.h/cpp
src/renderer/renderer.h/cpp
# ... etc

# To new structure:
src/Core/Application.h/cpp
src/Core/Window.h/cpp
src/Renderer/Renderer.h/cpp
# ... etc
```

### **Step 2: Update Include Paths**

**Old (CMake):**
```cpp
#include "application.h"
#include "renderer.h"
#include "components.h"
```

**New (VS with folders):**
```cpp
#include "Core/Application.h"
#include "Renderer/Renderer.h"
#include "ECS/Components.h"
```

**Search & Replace in VS:**
- Find: `#include "application.h"`
- Replace: `#include "Core/Application.h"`
- (Repeat for all includes)

### **Step 3: Add Files to Project**

**Option A: Drag & Drop**
1. In Solution Explorer, right-click project
2. Add → Existing Item
3. Select all .h and .cpp files
4. VS will flatten them initially

**Option B: Filters (Recommended)**
1. Right-click project → Add → New Filter
2. Create filters: Core, Renderer, ECS, Game, etc.
3. Drag files into appropriate filters
4. Filters appear as folders in Solution Explorer

**Filters are VIRTUAL** - they don't have to match disk layout (but should for sanity).

---

## 🎯 Entry Point Pattern

### **Core/EntryPoint.h**

```cpp
#pragma once

// Forward declare the application creation function
// This will be defined in Game/PathOfDoomApp.cpp
extern Application* CreateApplication();

// Main entry point - defined once in Core
int main(int argc, char** argv) {
    auto app = CreateApplication();
    app->Run();
    delete app;
    return 0;
}
```

### **Core/Application.h**

```cpp
#pragma once
#include <memory>

class Window;
class Renderer;

class Application {
public:
    Application();
    virtual ~Application();

    void Run();

protected:
    // Game overrides these
    virtual void OnInit() = 0;
    virtual void OnUpdate(float deltaTime) = 0;
    virtual void OnRender() = 0;

    // Utilities available to game
    Window* GetWindow() { return m_window.get(); }

private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;

    float m_deltaTime;
    float m_lastFrame;
    bool m_running;
};
```

### **Game/PathOfDoomApp.h**

```cpp
#pragma once
#include "Core/Application.h"
#include <entt/entt.hpp>

class PathOfDoomApp : public Application {
public:
    PathOfDoomApp();
    ~PathOfDoomApp();

protected:
    void OnInit() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;

private:
    entt::registry m_registry;

    // Systems
    class InputSystem* m_inputSystem;
    class CameraSystem* m_cameraSystem;
    class RenderSystem* m_renderSystem;

    void CreatePlayer();
    void LoadTestLevel();
};
```

### **Game/PathOfDoomApp.cpp**

```cpp
#include "PathOfDoomApp.h"

PathOfDoomApp::PathOfDoomApp() {
    // Constructor
}

PathOfDoomApp::~PathOfDoomApp() {
    // Cleanup
}

void PathOfDoomApp::OnInit() {
    // Setup ECS
    m_registry.ctx().emplace<InputState>();
    m_registry.ctx().emplace<DeltaTime>(0.0f);

    // Create systems
    m_inputSystem = new InputSystem();
    m_cameraSystem = new CameraSystem();
    m_renderSystem = new RenderSystem();

    // Load game content
    CreatePlayer();
    LoadTestLevel();
}

void PathOfDoomApp::OnUpdate(float deltaTime) {
    // Update systems
    m_inputSystem->Update(m_registry);
    m_cameraSystem->Update(m_registry);
}

void PathOfDoomApp::OnRender() {
    // Render game
    m_renderSystem->Render(m_registry, GetWindow());
}

// ===== APPLICATION CREATION =====
// This is how the game "connects" to the Core/EntryPoint.h
Application* CreateApplication() {
    return new PathOfDoomApp();
}
```

### **main.cpp**

```cpp
#include "Core/EntryPoint.h"

// That's it! EntryPoint.h defines main()
// Game defines CreateApplication() in PathOfDoomApp.cpp
```

---

## 🔄 Migration Checklist

### **Phase 1: Setup (1-2 hours)**
- [ ] Create Visual Studio solution
- [ ] Configure project settings (C++17, include paths)
- [ ] Copy external libraries (GLAD, GLFW, GLM, EnTT, ImGui)
- [ ] Create folder structure (Core/, Renderer/, ECS/, Game/)

### **Phase 2: Create RHI Abstraction (3-4 hours)**
- [ ] Create RHI/RenderTypes.h (handles, descriptors, enums)
- [ ] Create RHI/RenderDevice.h (abstract interface)
- [ ] Create RHI/OpenGL/OpenGLDevice.h
- [ ] Create RHI/OpenGL/OpenGLDevice.cpp (implement interface)
- [ ] Test RHI with simple triangle rendering

### **Phase 3: Copy Core Files (2-3 hours)**
- [ ] Copy application.h/cpp → Core/Application.h/cpp
- [ ] Copy window.h/cpp → Core/Window.h/cpp
- [ ] Copy shader.h/cpp → Renderer/Shader.h/cpp
- [ ] Copy mesh.h/cpp → Renderer/Mesh.h/cpp
- [ ] Copy texture.h/cpp → Renderer/Texture.h/cpp
- [ ] Copy camera.h/cpp → Renderer/Camera.h/cpp (will become component later)
- [ ] Copy billboard_renderer.h/cpp → Renderer/BillboardRenderer.h/cpp
- [ ] Copy sprite.h/cpp → Renderer/Sprite.h/cpp
- [ ] **Refactor** renderer.h/cpp → Renderer/Renderer.h/cpp (use RHI::RenderDevice)

### **Phase 4: Copy ECS Files (1 hour)**
- [ ] Copy components.h → ECS/Components.h
- [ ] Create ECS/InputState.h
- [ ] Create ECS/Systems/ folder
- [ ] (Systems will be created in next phase)

### **Phase 5: Create Game Structure (1 hour)**
- [ ] Create Game/PathOfDoomApp.h/cpp
- [ ] Implement Application inheritance
- [ ] Create main.cpp with EntryPoint pattern
- [ ] Move old application init code to PathOfDoomApp::OnInit()

### **Phase 6: Update Includes (1 hour)**
- [ ] Update all #include paths to use folder structure
- [ ] Fix any broken references
- [ ] Compile and fix errors

### **Phase 7: Test Build (30 min)**
- [ ] Build Debug configuration
- [ ] Build Release configuration
- [ ] Run and verify window opens
- [ ] Verify rendering works with RHI abstraction

**Total Time: ~10-13 hours**

---

## 🎨 Visual Studio Filters Setup

**Filters organize files in Solution Explorer (they're virtual folders).**

### **Recommended Filter Structure:**

```
PathOfDoom (Project)
├── Core
│   ├── Application.h
│   ├── Application.cpp
│   ├── Window.h
│   ├── Window.cpp
│   └── EntryPoint.h
│
├── RHI
│   ├── RenderDevice.h
│   ├── RenderTypes.h
│   ├── Resources.h
│   └── OpenGL
│       ├── OpenGLDevice.h
│       ├── OpenGLDevice.cpp
│       ├── OpenGLBuffer.h
│       └── OpenGLTexture.h
│
├── Renderer
│   ├── Renderer.h
│   ├── Renderer.cpp
│   ├── Shader.h
│   ├── Shader.cpp
│   ├── BillboardRenderer.h
│   └── ...
│
├── ECS
│   ├── Components.h
│   ├── InputState.h
│   └── Systems
│       ├── InputSystem.h
│       ├── InputSystem.cpp
│       └── ...
│
├── Game
│   ├── PathOfDoomApp.h
│   ├── PathOfDoomApp.cpp
│   ├── Systems
│   └── Components
│
└── External (No filter, or separate filter)
```

**How to create:**
1. Right-click project → Add → New Filter → Name: "Core"
2. Repeat for RHI, Renderer, ECS, Game
3. Drag files into filters
4. For nested filters (RHI/OpenGL, ECS/Systems), create sub-filters:
   - Right-click RHI filter → Add → New Filter → Name: "OpenGL"
   - Right-click ECS filter → Add → New Filter → Name: "Systems"

---

## 🚀 Benefits After Migration

### **What You Gain:**
1. ✅ **Clean organization** - Clear Core/Renderer/Game separation
2. ✅ **Visual Studio IntelliSense** - Better autocomplete than CMake
3. ✅ **Easier debugging** - VS debugger is excellent
4. ✅ **Fast iteration** - Single project builds fast
5. ✅ **Professional structure** - Ready to scale

### **What You Keep:**
6. ✅ **Flexibility** - Easy to refactor, move files
7. ✅ **Simplicity** - No library linking complexity
8. ✅ **Focus** - Spend time on game, not build systems

---

## 📝 Post-Migration: Next Steps

After migration is complete:

1. **Implement ECS Systems** (from CURRENT_PRIORITIES.md)
   - InputSystem
   - CameraSystem
   - RenderSystem

2. **Build State Machine System** (Phase 3)
   - StateMachine parser
   - StateExecutor
   - ActionRegistry

3. **Implement Weapon System** (Phase 4)
   - WeaponComponent
   - WeaponSystem
   - First pistol

4. **Add Enemies** (Phase 5)
   - EnemySystem
   - Basic AI
   - Health/damage

---

## 🎓 Learning Resources

**Visual Studio:**
- [VS Project Settings Guide](https://docs.microsoft.com/en-us/cpp/build/reference/c-cpp-prop-page)
- [Managing C++ Projects](https://docs.microsoft.com/en-us/cpp/build/creating-and-managing-visual-cpp-projects)

**Code Organization:**
- [Game Programming Patterns](https://gameprogrammingpatterns.com/)
- [Clean Code (book)](https://www.amazon.com/Clean-Code-Handbook-Software-Craftsmanship/dp/0132350882)

---

## 💡 Pro Tips

1. **Use Filters liberally** - They help navigate large projects
2. **Keep disk layout matching filters** - Easier to find files
3. **Use #pragma once** - Simpler than include guards
4. **Precompiled headers optional** - Don't need them yet
5. **Build often** - Don't write 1000 lines before compiling
6. **Git commit before migration** - Easy to revert if needed

---

## 🛑 Common Mistakes to Avoid

1. ❌ **Don't create separate projects** - One project is enough
2. ❌ **Don't use precompiled headers yet** - Adds complexity
3. ❌ **Don't over-abstract** - YAGNI (You Ain't Gonna Need It)
4. ❌ **Don't forget to copy shaders/assets** - Include in project
5. ❌ **Don't use relative paths in code** - Use absolute from src/

---

## 🎯 Success Criteria

You've successfully migrated when:
- ✅ Visual Studio solution builds without errors
- ✅ Window opens and displays test level
- ✅ Code is organized in Core/Renderer/ECS/Game folders
- ✅ All includes use folder paths (`Core/Application.h`)
- ✅ Can add new files easily
- ✅ Debugging works in VS

---

## 🎮 RHI (Rendering Hardware Interface) Architecture

### **What is RHI?**

The **Rendering Hardware Interface** is an abstraction layer between your game code and the graphics API (OpenGL, Vulkan, DirectX 12).

**Goal:** Write rendering code once, swap backends easily.

### **Why Add RHI Now?**

✅ **Prepare for the future** - Vulkan/DX12 support later
✅ **Learn abstraction patterns** - Industry-standard approach
✅ **Cleaner architecture** - Game code doesn't know about OpenGL
✅ **Low risk** - Still using OpenGL initially

### **RHI File Structure**

```
src/RHI/
├── RenderTypes.h          # API-agnostic types
│   ├── BufferHandle       # Opaque handle to GPU buffer
│   ├── TextureHandle      # Opaque handle to GPU texture
│   ├── ShaderHandle       # Opaque handle to shader program
│   ├── BufferDesc         # Buffer creation parameters
│   ├── TextureDesc        # Texture creation parameters
│   └── ClearFlags         # Enum for clear operations
│
├── RenderDevice.h         # Abstract interface (pure virtual)
│   ├── BeginFrame()
│   ├── EndFrame()
│   ├── Clear()
│   ├── SetViewport()
│   ├── CreateBuffer()
│   ├── CreateTexture()
│   ├── CreateShader()
│   ├── BindBuffer()
│   ├── BindTexture()
│   ├── DrawIndexed()
│   └── DestroyBuffer/Texture/Shader()
│
└── OpenGL/                # OpenGL implementation
    ├── OpenGLDevice.h/cpp # Implements RenderDevice
    ├── OpenGLBuffer.h/cpp # OpenGL buffer wrapper (optional)
    └── OpenGLTexture.h/cpp # OpenGL texture wrapper (optional)
```

### **Key Concepts**

#### **1. Opaque Handles**
Instead of exposing `GLuint` or `VkBuffer` directly, use opaque handles:

```cpp
// RHI/RenderTypes.h
namespace RHI {
    struct BufferHandle {
        uint32_t id = 0;  // Internal ID, maps to GL object
        bool IsValid() const { return id != 0; }
    };

    struct TextureHandle {
        uint32_t id = 0;
        bool IsValid() const { return id != 0; }
    };
}
```

**Benefits:**
- Game code can't accidentally use OpenGL functions on handles
- Easy to swap backend (Vulkan uses different handle types)
- Type-safe (can't pass BufferHandle where TextureHandle expected)

#### **2. Descriptors**
Describe how to create resources without API-specific details:

```cpp
// RHI/RenderTypes.h
namespace RHI {
    struct BufferDesc {
        uint32_t size = 0;
        bool cpuAccessible = false;  // Can we map/read back?
        bool isVertexBuffer = false;
        bool isIndexBuffer = false;
        bool isUniform = false;
    };

    struct TextureDesc {
        uint32_t width = 0;
        uint32_t height = 0;
        TextureFormat format = TextureFormat::RGBA8;
        bool generateMipmaps = false;
        TextureFilter filter = TextureFilter::Linear;
    };
}
```

#### **3. Abstract Interface**
Pure virtual class that all backends implement:

```cpp
// RHI/RenderDevice.h
namespace RHI {
    class RenderDevice {
    public:
        virtual ~RenderDevice() = default;

        // Frame management
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        // State management
        virtual void Clear(ClearFlags flags, const glm::vec4& color) = 0;
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
        virtual void SetDepthTest(bool enabled) = 0;

        // Resource creation
        virtual BufferHandle CreateBuffer(const BufferDesc& desc, const void* data = nullptr) = 0;
        virtual TextureHandle CreateTexture(const TextureDesc& desc, const void* data = nullptr) = 0;
        virtual ShaderHandle CreateShader(const ShaderDesc& desc) = 0;

        // Resource binding
        virtual void BindVertexBuffer(BufferHandle buffer) = 0;
        virtual void BindIndexBuffer(BufferHandle buffer) = 0;
        virtual void BindTexture(uint32_t slot, TextureHandle texture) = 0;
        virtual void BindShader(ShaderHandle shader) = 0;

        // Drawing
        virtual void Draw(uint32_t vertexCount, uint32_t startVertex = 0) = 0;
        virtual void DrawIndexed(uint32_t indexCount, uint32_t startIndex = 0) = 0;

        // Resource destruction
        virtual void DestroyBuffer(BufferHandle buffer) = 0;
        virtual void DestroyTexture(TextureHandle texture) = 0;
        virtual void DestroyShader(ShaderHandle shader) = 0;
    };

    // Factory function - creates platform-specific device
    RenderDevice* CreateRenderDevice();
}
```

#### **4. OpenGL Implementation**
Implements the interface using OpenGL:

```cpp
// RHI/OpenGL/OpenGLDevice.h
namespace RHI {
    class OpenGLDevice : public RenderDevice {
    public:
        OpenGLDevice();
        ~OpenGLDevice();

        // Implement all virtual methods
        void Clear(ClearFlags flags, const glm::vec4& color) override;
        void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        BufferHandle CreateBuffer(const BufferDesc& desc, const void* data) override;
        TextureHandle CreateTexture(const TextureDesc& desc, const void* data) override;

        void BindVertexBuffer(BufferHandle buffer) override;
        void DrawIndexed(uint32_t indexCount, uint32_t startIndex) override;

        // ... etc

    private:
        // Handle → OpenGL object mapping
        std::unordered_map<uint32_t, GLuint> m_buffers;
        std::unordered_map<uint32_t, GLuint> m_textures;
        std::unordered_map<uint32_t, GLuint> m_shaders;

        uint32_t m_nextBufferId = 1;
        uint32_t m_nextTextureId = 1;
        uint32_t m_nextShaderId = 1;

        GLuint GetGLBuffer(BufferHandle handle) const;
        GLuint GetGLTexture(TextureHandle handle) const;
    };
}
```

### **Usage Example**

**Old Code (Direct OpenGL):**
```cpp
// Renderer.cpp - BEFORE RHI
void Renderer::Init() {
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void Renderer::Clear() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
```

**New Code (Using RHI):**
```cpp
// Renderer.cpp - AFTER RHI
Renderer::Renderer(RHI::RenderDevice* device)
    : m_device(device) {}

void Renderer::Init() {
    RHI::BufferDesc desc;
    desc.size = sizeof(vertices);
    desc.isVertexBuffer = true;

    m_vertexBuffer = m_device->CreateBuffer(desc, vertices);
}

void Renderer::Clear() {
    m_device->Clear(RHI::ClearFlags::Both, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
}
```

**Application Setup:**
```cpp
// Application.cpp
void Application::Init() {
    // Create RHI device
    m_renderDevice = RHI::CreateRenderDevice();  // Returns OpenGLDevice for now

    // Pass to renderer
    m_renderer = std::make_unique<Renderer>(m_renderDevice);
    m_renderer->Init();
}
```

### **Benefits for Path of Doom**

1. **Cleaner Code**
   - Renderer doesn't know about OpenGL
   - API calls hidden behind clean interface
   - Easier to understand high-level rendering logic

2. **Future-Proof**
   - Want Vulkan later? Implement `VulkanDevice : public RenderDevice`
   - Want DirectX 12? Implement `DX12Device : public RenderDevice`
   - Game code doesn't change at all

3. **Learning Opportunity**
   - Understand how engines abstract graphics APIs
   - Learn about opaque handles, descriptors, command patterns
   - Industry-standard architecture (Unreal, Unity, etc. use similar patterns)

4. **Testing**
   - Can create `MockRenderDevice` for unit tests
   - Test game logic without GPU

### **Migration Strategy**

**Phase 1: Create RHI Foundation (1-2 hours)**
1. Create `RHI/RenderTypes.h` with handles and descriptors
2. Create `RHI/RenderDevice.h` with pure virtual interface
3. Don't implement yet, just define the API

**Phase 2: Implement OpenGL Backend (2-3 hours)**
1. Create `RHI/OpenGL/OpenGLDevice.h/cpp`
2. Implement each virtual method using OpenGL calls
3. Test with simple triangle rendering

**Phase 3: Refactor Renderer (1-2 hours)**
1. Add `RHI::RenderDevice* m_device` to Renderer
2. Replace OpenGL calls with `m_device->` calls
3. Test that existing rendering still works

**Phase 4: Update Application (30 min)**
1. Create RenderDevice in Application::Init()
2. Pass to Renderer constructor
3. Verify everything builds and runs

### **What NOT to Abstract (Yet)**

- **Shaders** - Keep shader code as GLSL files for now
- **Vertex formats** - OpenGL-specific vertex attribute layout is fine
- **Window/context** - GLFW is fine for now

**Reason:** These are harder to abstract and provide less benefit. Focus on the core rendering commands first.

### **Future: Adding Vulkan Backend**

When you're ready to add Vulkan (maybe 6-12 months from now):

```cpp
// RHI/Vulkan/VulkanDevice.h
namespace RHI {
    class VulkanDevice : public RenderDevice {
    public:
        BufferHandle CreateBuffer(const BufferDesc& desc, const void* data) override {
            // Create VkBuffer instead of glGenBuffers
            VkBufferCreateInfo bufferInfo = { /* ... */ };
            VkBuffer vkBuffer;
            vkCreateBuffer(m_device, &bufferInfo, nullptr, &vkBuffer);

            uint32_t id = m_nextBufferId++;
            m_buffers[id] = vkBuffer;
            return BufferHandle{id};
        }

        // ... implement all other methods

    private:
        std::unordered_map<uint32_t, VkBuffer> m_buffers;
        VkDevice m_device;
    };
}

// RHI/RenderDevice.cpp
RenderDevice* CreateRenderDevice() {
    #ifdef USE_VULKAN
        return new VulkanDevice();
    #else
        return new OpenGLDevice();
    #endif
}
```

**Game code doesn't change at all!** Just recompile with Vulkan flag.

---

**Ready to build Path of Doom!** 🔥

**Next Steps:**
1. Create RHI abstraction during VS migration
2. Implement ECS systems (InputSystem, CameraSystem, RenderSystem)
3. Build state machines for weapons/enemies
4. Add gameplay features!
