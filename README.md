# 🎮 Path of Doom

**A GZDoom-style 3D looter-shooter built from scratch in C++ with OpenGL**

Path of Doom combines the classic Doom aesthetic with modern looter-shooter mechanics. Think GZDoom's 3D engine meets Borderlands' loot system - procedurally generated weapons, state-driven animations, and fast-paced demon slaying.

---

## 🎯 Project Vision

**Core Concept:**
- **3D world with billboard sprites** - Full 3D movement (look up/down, jump, crouch) like GZDoom
- **8-directional sprite system** - Doom-style enemies and items that rotate to face the camera
- **State machine-driven gameplay** - Weapon animations and enemy AI controlled by declarative state files
- **Procedural loot system** - Randomized weapons with affixes, rarity tiers, and unique perks
- **Fast-paced action** - 60 FPS gunplay with responsive controls and game feel

**Rendering Philosophy:**
- Levels: True 3D geometry (meshes, not sectors)
- Entities: Billboard sprites with 8-directional rotation
- Weapons: HUD-space sprite rendering with state-driven animations
- Effects: Particle systems, dynamic lighting, muzzle flashes

---

## 🏗️ Current Architecture

### **ECS (Entity Component System)**
- **Engine:** EnTT v3.13.2 - Fast, header-only ECS library
- **Components:** Transform, Velocity, Sprite, Billboard, Health, Player/Enemy tags
- **Systems:** Input, Camera, Render (in development)

### **Rendering Pipeline**
- **API:** OpenGL 4.6 Core Profile
- **Billboard Renderer:** Y-axis locked sprites for 3D world entities
- **Shader System:** Modular vertex/fragment shader loading
- **Camera:** 3D FPS-style camera with full pitch/yaw control

### **State Machine System** (Planned)
- **Format:** Custom `.states` text files (inspired by ZScript)
- **Tick Rate:** 35 Hz for authentic Doom timing
- **Actions:** C++ callback system (FireBullets, PlaySound, GunFlash, etc.)
- **No VM:** Pure data-driven with registered C++ functions

### **Sprite System** (In Progress)
- **Format:** Individual PNG files per sprite frame (GZDoom-style)
- **Naming:** `SPRITEA0.png` convention with rotation indicators
- **Mirroring:** Automatic mirroring for 5-rotation sprites → 8 directions
- **Variable sizes:** Each sprite can have unique dimensions and offsets

---

## 🔧 Technical Stack

| System | Technology | Status |
|--------|-----------|--------|
| **Build System** | CMake 3.20+ | ✅ Working |
| **ECS** | EnTT v3.13.2 | ✅ Integrated |
| **Windowing** | GLFW 3.4 | ✅ Working |
| **OpenGL Loader** | GLAD 2.0.5 | ✅ Working |
| **Math** | GLM 1.0.1 | ✅ Working |
| **Image Loading** | stb_image | ✅ Working |
| **UI** | ImGui v1.90.1 | ✅ Working |
| **Audio** | OpenAL (planned) | ⏳ Pending |
| **Loot System** | Custom affix generation | ⏳ Pending |
| **State Machines** | Custom parser + executor | ⏳ Pending |

---

## 🚀 Building the Project

### Prerequisites
- CMake 3.20 or higher
- C++17 compatible compiler (MSVC, Clang, or GCC)
- Git (for dependency fetching)

### Windows (Visual Studio)
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Debug
```

### Windows (Clang)
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -T ClangCL
cmake --build . --config Debug
```

### Linux/macOS
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

### Running
```bash
# Windows
.\build\Debug\PodEngine.exe

# Linux/macOS
./build/PodEngine
```

---

## 🎮 Controls

**Movement:**
- `W/A/S/D` - Move forward/left/backward/right
- `Space` - Move up (noclip mode)
- `Left Shift` - Move down (noclip mode)
- `Mouse` - Look around (FPS camera)

**UI:**
- `TAB` - Toggle cursor lock (switch between FPS and UI mode)
- `ESC` - Exit application

**Game Speed:** 5.0 m/s movement, 1.7m player eye height

---

## 📁 Project Structure

```
pod_engine/
├── CMakeLists.txt              # Build configuration
├── README.md                   # This file
├── docs/                       # Design documentation
│   ├── CURRENT_PRIORITIES.md  # What to work on RIGHT NOW
│   ├── DOOM_SPRITE_DESIGN.md  # Sprite system architecture
│   └── ENGINE_CONVERSION_GUIDE.md  # Full conversion plan
├── src/
│   ├── application/           # Main application loop
│   ├── camera/                # 3D FPS camera
│   ├── components/            # ECS components (Transform, Sprite, etc.)
│   ├── imgui_layer/           # Debug UI integration
│   ├── mesh/                  # 3D mesh rendering
│   ├── renderer/              # Low-level OpenGL renderer
│   ├── shader/                # Shader loading and management
│   ├── sprite/                # Billboard sprite rendering
│   ├── texture/               # Texture loading (stb_image)
│   ├── voxel/                 # Voxel system (legacy, unused)
│   └── window/                # GLFW window management
├── shaders/
│   ├── basic.vert/frag        # Level geometry shaders
│   └── billboard.vert/frag    # Sprite billboard shaders
└── assets/
    ├── textures/              # Prototype textures
    └── sprites/               # Entity sprites (planned)
```

---

## 🎯 Development Roadmap

### **Phase 1: Foundation** ✅ (Current)
- [x] OpenGL context and window
- [x] FPS camera system
- [x] Basic 3D mesh rendering
- [x] Billboard sprite renderer (Y-axis locked)
- [x] ECS integration (EnTT)
- [x] ImGui debug UI

### **Phase 2: ECS Architecture** ⚙️ (Next)
- [ ] Extract InputSystem from Application
- [ ] Create RenderSystem for ECS-driven rendering
- [ ] Make Camera an entity component (CameraSystem)
- [ ] Refactor Application to use systems pattern
- [ ] Add multiple test entities (enemies, items, decorations)

### **Phase 3: State Machines** 🎬
- [ ] State machine parser (`.states` file format)
- [ ] State machine executor (35 Hz tick rate)
- [ ] Action registration system (C++ callbacks)
- [ ] Test weapon state (pistol with fire/reload/ready)

### **Phase 4: Weapons** 🔫
- [ ] WeaponComponent + WeaponDefinition
- [ ] Core actions (FireBullets, PlaySound, ReFire, CheckReload)
- [ ] Raycast hit detection
- [ ] Muzzle flash rendering
- [ ] Ammo system

### **Phase 5: Gameplay** 👾
- [ ] Enemy AI (basic state machines)
- [ ] Health/damage system
- [ ] Collision detection (2D AABB)
- [ ] Particle effects (bullet impacts, gibs)
- [ ] HUD system (health, ammo, crosshair)

### **Phase 6: Loot System** 💰
- [ ] Affix generator (IncreasedDamage, FireRate, etc.)
- [ ] Rarity tiers (Common → Exotic)
- [ ] Inventory system (Tetris-style grid)
- [ ] Weapon stat calculation (base + affixes)
- [ ] Procedural weapon naming

### **Phase 7: Polish** ✨
- [ ] Audio system (OpenAL + OGG Vorbis)
- [ ] Screen shake and camera effects
- [ ] Dynamic lighting
- [ ] Save/load system
- [ ] Multiple levels/scenes

---

## 🧠 Key Design Decisions

### Why EnTT?
- **Header-only** - No linking complexity
- **Blazing fast** - Cache-friendly iteration
- **Battle-tested** - Used in production games
- **Flexible** - Views, groups, signals/delegates

### Why Individual Sprite Files?
- **GZDoom compatibility** - Easy to use existing assets
- **Variable sizes** - Each sprite has unique dimensions
- **Artist-friendly** - No atlas packing required
- **Hotswappable** - Change sprites without recompiling

### Why State Machines?
- **Data-driven** - Designers can edit without code
- **Responsive gameplay** - Frame-perfect timing
- **Modular** - Easy to add new weapons/enemies
- **Debuggable** - States are visible and traceable

### Why 3D Billboard Sprites?
- **Classic aesthetic** - Doom's iconic look
- **Performance** - Simpler than full 3D models
- **Art efficiency** - 8 rotations vs full 3D rigging
- **Authenticity** - True to the genre's roots

---

## 📚 Documentation

- **[CURRENT_PRIORITIES.md](docs/CURRENT_PRIORITIES.md)** - What to work on right now
- **[DOOM_SPRITE_DESIGN.md](docs/DOOM_SPRITE_DESIGN.md)** - Sprite system architecture
- **[ENGINE_CONVERSION_GUIDE.md](docs/ENGINE_CONVERSION_GUIDE.md)** - Full Phase 1-6 guide

---

## 🐛 Troubleshooting

### CMake Configuration Issues
```bash
# Clean build directory
rm -rf build
mkdir build && cd build
cmake ..
```

### Shader Compilation Errors
Shaders are automatically copied to the build directory. Ensure you're running from:
```bash
./build/Debug/PodEngine.exe  # Windows
./build/PodEngine            # Linux/macOS
```

### Missing Dependencies
Dependencies are fetched automatically by CMake. If fetch fails:
```bash
rm -rf build/_deps
cmake --build . --clean-first
```

---

## 🎓 Learning Resources

### OpenGL & Rendering
- [LearnOpenGL](https://learnopengl.com/) - Modern OpenGL tutorials
- [OpenGL SuperBible](https://www.openglsuperbible.com/) - Advanced techniques

### Game Architecture
- [Game Programming Patterns](https://gameprogrammingpatterns.com/) - ECS, state machines, etc.
- [EnTT Documentation](https://github.com/skypjack/entt) - ECS library guide

### Doom Modding (for reference)
- [ZDoom Wiki](https://zdoom.org/wiki/) - State machines, sprite naming
- [Doom Wiki](https://doomwiki.org/) - Original game mechanics

---

## 📜 License

This is a learning/hobby project. Code is provided as-is for educational purposes.

**Assets:** Sprite assets in `assets/` may have separate licenses - check individual sources.

---

## 🚧 Project Status

**Current Phase:** ECS Architecture Refactoring
**Last Updated:** 2025-10-27
**Active Development:** Yes

**Working Features:**
- ✅ 3D FPS camera
- ✅ Billboard sprite rendering
- ✅ EnTT ECS integration
- ✅ Basic test level rendering

**Next Milestone:**
- ⚙️ Extract systems from Application (InputSystem, RenderSystem, CameraSystem)
- ⚙️ Make application loop ECS-driven
- ⚙️ Add multiple entities to test sprite rendering

---

**Ready to build Path of Doom!** 🔥
