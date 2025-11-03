# 🚀 Visual Studio + CMake + NVRHI Setup Guide

**Project:** Path of Doom
**IDE:** Visual Studio 2022
**Build System:** CMake (built into VS)
**Rendering:** NVRHI (Vulkan/DirectX 12)

---

## 📋 Prerequisites

### **Install Visual Studio 2022**
Make sure you have these workloads installed:
- **Desktop development with C++**
- **C++ CMake tools for Windows** (important!)
- **Windows 10/11 SDK** (for DirectX 12)

### **Install Vulkan SDK** (Optional but recommended)
1. Download from: https://vulkan.lunarg.com/
2. Install to default location
3. CMake will auto-detect it

---

## 🎯 Opening Project in Visual Studio

### **Method 1: Open Folder (Recommended)**

1. **Launch Visual Studio 2022**
2. Click **"Open a local folder"** on start screen
   (OR: File → Open → Folder)
3. Navigate to `d:\projects\C++\pod_engine\`
4. Click **"Select Folder"**

Visual Studio will:
- ✅ Detect `CMakeLists.txt` automatically
- ✅ Run CMake configuration
- ✅ Download dependencies (NVRHI, GLFW, GLM, EnTT, ImGui)
- ✅ Generate build files
- ✅ Enable IntelliSense

### **Method 2: From File Explorer**

1. Navigate to `d:\projects\C++\pod_engine\` in File Explorer
2. Right-click in folder
3. Select **"Open with Visual Studio"**

---

## 📁 Project Structure in Visual Studio

### **Solution Explorer View:**

```
📁 Folder View
├── 📁 src/
│   ├── 📁 application/
│   ├── 📁 window/
│   ├── 📁 renderer/
│   ├── 📁 ecs/
│   ├── 📁 sprite/
│   └── main.cpp
├── 📁 shaders/
├── 📁 assets/
├── 📁 docs/
├── 📄 CMakeLists.txt        ← Your build script
├── 📄 CMakeSettings.json    ← VS auto-generates this
└── 📁 out/                  ← VS build output (auto-created)
    └── build/
        ├── x64-Debug/
        └── x64-Release/
```

### **Switching to CMake Targets View:**

Click the dropdown in Solution Explorer:
- **Folder View** - Shows actual filesystem
- **CMake Targets View** - Shows build targets (PathOfDoom.exe)

Both are useful! Use Folder View for editing, CMake Targets View for building specific targets.

---

## ⚙️ Configuration: CMakeSettings.json

Visual Studio auto-generates this file. To customize:

**Project → CMake Settings for PathOfDoom**

### **Default Configurations:**

```json
{
  "configurations": [
    {
      "name": "x64-Debug",
      "generator": "Ninja",
      "configurationType": "Debug",
      "buildRoot": "${projectDir}\\out\\build\\${name}",
      "installRoot": "${projectDir}\\out\\install\\${name}",
      "cmakeCommandArgs": "",
      "buildCommandArgs": "",
      "ctestCommandArgs": "",
      "inheritEnvironments": [ "msvc_x64_x64" ],
      "variables": [
        {
          "name": "NVRHI_WITH_VULKAN",
          "value": "True",
          "type": "BOOL"
        },
        {
          "name": "NVRHI_WITH_DX12",
          "value": "True",
          "type": "BOOL"
        },
        {
          "name": "NVRHI_DEFAULT_BACKEND",
          "value": "VK",
          "type": "STRING"
        }
      ]
    },
    {
      "name": "x64-Release",
      "generator": "Ninja",
      "configurationType": "RelWithDebInfo",
      "buildRoot": "${projectDir}\\out\\build\\${name}",
      "installRoot": "${projectDir}\\out\\install\\${name}",
      "cmakeCommandArgs": "",
      "buildCommandArgs": "",
      "ctestCommandArgs": "",
      "inheritEnvironments": [ "msvc_x64_x64" ],
      "variables": [
        {
          "name": "NVRHI_WITH_VULKAN",
          "value": "True",
          "type": "BOOL"
        },
        {
          "name": "NVRHI_WITH_DX12",
          "value": "True",
          "type": "BOOL"
        },
        {
          "name": "NVRHI_DEFAULT_BACKEND",
          "value": "D3D12",
          "type": "STRING"
        }
      ]
    }
  ]
}
```

### **Customizing CMake Options:**

To change NVRHI backends or other settings:
1. **Project → CMake Settings for PathOfDoom**
2. Find the configuration (x64-Debug, x64-Release)
3. Scroll to **CMake variables**
4. Toggle options:
   - `NVRHI_WITH_VULKAN` - Enable Vulkan backend
   - `NVRHI_WITH_DX12` - Enable DirectX 12 backend
   - `NVRHI_DEFAULT_BACKEND` - Set default (VK or D3D12)

---

## 🔨 Building the Project

### **Build Configurations:**

Top toolbar in Visual Studio:
- **Configuration dropdown**: Debug / Release
- **Target dropdown**: PathOfDoom.exe

### **Build Commands:**

| Action | Shortcut | Menu |
|--------|----------|------|
| **Build All** | Ctrl+Shift+B | Build → Build All |
| **Clean** | - | Build → Clean All |
| **Rebuild** | - | Build → Rebuild All |
| **Build Target** | - | Right-click target → Build |

### **Build Output:**

Executable location:
```
out/build/x64-Debug/PathOfDoom.exe
out/build/x64-Release/PathOfDoom.exe
```

Shaders and assets are auto-copied to build directory (see CMakeLists.txt).

---

## 🐞 Debugging

### **Start Debugging:**

1. Set `PathOfDoom.exe` as startup target (it's usually default)
2. Press **F5** (Start Debugging)
   - OR: Click green ▶️ button
   - OR: Debug → Start Debugging

### **Breakpoints:**

- Click left margin in code editor to set breakpoints
- F9 to toggle breakpoint on current line
- Breakpoints work perfectly with CMake projects!

### **Debug Settings:**

To set command-line arguments or working directory:
1. **Debug → Debug and Launch Settings for PathOfDoom**
2. Edit `launch.vs.json`

Example:
```json
{
  "version": "0.2.1",
  "defaults": {},
  "configurations": [
    {
      "type": "default",
      "project": "CMakeLists.txt",
      "projectTarget": "PathOfDoom.exe",
      "name": "PathOfDoom.exe",
      "currentDir": "${workspaceRoot}",
      "args": []
    }
  ]
}
```

---

## 📦 Dependencies (Managed by CMake)

### **FetchContent Dependencies:**
CMake will automatically download and build these:

| Library | Purpose | Version |
|---------|---------|---------|
| **NVRHI** | Rendering abstraction (Vulkan/DX12) | Latest |
| **GLFW** | Window/input handling | 3.4 |
| **GLM** | Math library | 1.0.1 |
| **EnTT** | Entity Component System | 3.13.2 |
| **ImGui** | Immediate mode GUI | 1.90.1 |
| **stb_image** | Image loading | Latest |

### **Manual Dependencies:**
- **GLAD** - OpenGL loader (pre-generated, in `external/glad/`)
- **Vulkan SDK** - Install separately (optional)

---

## 🔄 CMake Cache and Regeneration

### **When CMake Regenerates:**

Visual Studio auto-regenerates CMake cache when:
- ✅ You modify `CMakeLists.txt`
- ✅ You switch configurations (Debug ↔ Release)
- ✅ You change CMake settings

You'll see:
```
1> [CMake] -- Configuring done
1> [CMake] -- Generating done
1> [CMake] -- Build files have been written to: out/build/x64-Debug
```

### **Manual Regeneration:**

If CMake gets confused:
1. **Project → Delete Cache and Reconfigure**
2. Wait for configuration to complete

### **Clean Cache:**

If things are really broken:
1. Close Visual Studio
2. Delete `out/` folder
3. Reopen Visual Studio
4. VS will regenerate everything

---

## 🎮 Using NVRHI in Your Code

### **Include NVRHI Headers:**

```cpp
// In your renderer or application code
#include <nvrhi/nvrhi.h>
#include <nvrhi/utils.h>

#ifdef _WIN32
    #ifdef USE_DX12
        #include <nvrhi/d3d12.h>
    #endif
    #ifdef USE_VULKAN
        #include <nvrhi/vulkan.h>
    #endif
#else
    #include <nvrhi/vulkan.h>
#endif
```

### **Creating NVRHI Device:**

```cpp
// Example: Creating Vulkan device
nvrhi::DeviceHandle device = nvrhi::vulkan::createDevice(deviceParams);

// Example: Creating DirectX 12 device (Windows only)
nvrhi::DeviceHandle device = nvrhi::d3d12::createDevice(deviceParams);
```

### **NVRHI Basics:**

NVRHI provides API-agnostic abstractions:
- `nvrhi::BufferHandle` - GPU buffers
- `nvrhi::TextureHandle` - Textures
- `nvrhi::GraphicsPipelineHandle` - Graphics pipelines
- `nvrhi::CommandListHandle` - Command recording

**Example:**
```cpp
// Create vertex buffer
nvrhi::BufferDesc bufferDesc;
bufferDesc.byteSize = sizeof(vertices);
bufferDesc.isVertexBuffer = true;
bufferDesc.debugName = "VertexBuffer";
bufferDesc.initialState = nvrhi::ResourceStates::VertexBuffer;

nvrhi::BufferHandle vertexBuffer = device->createBuffer(bufferDesc);

// Upload data
commandList->writeBuffer(vertexBuffer, vertices, sizeof(vertices));
```

---

## 🛠️ Workflow Tips

### **Adding New Files:**

#### **Option 1: In Visual Studio**
1. Right-click folder in Solution Explorer
2. Add → New Item → C++ File (.cpp) or Header File (.h)
3. CMakeLists.txt will auto-detect (if using globbing)

#### **Option 2: Manually**
1. Create file in filesystem
2. Add to `ENGINE_SOURCES` or `ENGINE_HEADERS` in CMakeLists.txt
3. Save CMakeLists.txt → VS will auto-regenerate

**Recommended approach for now:**
Keep manually listing files in CMakeLists.txt (more explicit, better for build systems).

### **Adding New Dependencies:**

To add a new library (e.g., some physics library):

1. Open `CMakeLists.txt`
2. Add FetchContent declaration:
   ```cmake
   FetchContent_Declare(
       mylibrary
       GIT_REPOSITORY https://github.com/user/mylibrary.git
       GIT_TAG v1.0.0
   )
   FetchContent_MakeAvailable(mylibrary)
   ```
3. Link to project:
   ```cmake
   target_link_libraries(${PROJECT_NAME}
       PRIVATE
           mylibrary
   )
   ```
4. Save → VS regenerates → IntelliSense picks it up

### **Output Window:**

View → Output → Show output from: **CMake**

This shows:
- CMake configuration progress
- Dependency downloads
- Build errors/warnings

---

## 🎯 Common Tasks

### **1. Switching Rendering Backend**

**Vulkan → DirectX 12:**
1. Project → CMake Settings for PathOfDoom
2. Find `NVRHI_DEFAULT_BACKEND` variable
3. Change from `VK` to `D3D12`
4. Save → Regenerate

**OR** edit `CMakeLists.txt`:
```cmake
set(NVRHI_DEFAULT_BACKEND "D3D12" CACHE STRING "Default rendering backend")
```

### **2. Changing C++ Standard**

Edit `CMakeLists.txt`:
```cmake
set(CMAKE_CXX_STANDARD 20)  # Change from 17 to 20
```

### **3. Adding Compiler Flags**

Edit `CMakeLists.txt`:
```cmake
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)  # Treat warnings as errors
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror)
endif()
```

### **4. Multi-Configuration Build**

Visual Studio supports multiple configs simultaneously:
- Debug build → `out/build/x64-Debug/`
- Release build → `out/build/x64-Release/`

Switch configs in toolbar dropdown, build separately.

---

## 🚨 Troubleshooting

### **Problem: "CMake Error: Could not find Vulkan"**

**Solution:**
1. Install Vulkan SDK: https://vulkan.lunarg.com/
2. Restart Visual Studio
3. Project → Delete Cache and Reconfigure

**OR** disable Vulkan:
```cmake
option(NVRHI_WITH_VULKAN "Enable Vulkan backend for NVRHI" OFF)
```

### **Problem: "NVRHI headers not found"**

**Solution:**
CMake might still be downloading NVRHI. Check Output window:
- View → Output
- Show output from: CMake

Wait for:
```
[CMake] -- Fetching nvrhi
[CMake] -- nvrhi populated
```

### **Problem: IntelliSense not working**

**Solution:**
1. Wait for CMake configuration to complete (check Output window)
2. Tools → Options → Text Editor → C/C++ → Advanced
3. Check **"Enable Faster Project Load"** is OFF
4. Restart Visual Studio

### **Problem: Executable doesn't run**

**Solution:**
Check working directory contains shaders and assets:
1. Debug → Debug and Launch Settings for PathOfDoom
2. Set `"currentDir": "${workspaceRoot}"`
3. OR check that post-build copy commands ran (see CMakeLists.txt)

### **Problem: Build is slow**

**Solution:**
1. Use Ninja generator (default in CMakeSettings.json)
2. Enable parallel builds:
   - Tools → Options → Projects and Solutions → Build and Run
   - Set "maximum number of parallel project builds" to CPU cores

---

## 📚 Learning Resources

### **NVRHI Documentation:**
- GitHub: https://github.com/NVIDIAGameWorks/nvrhi
- Examples: https://github.com/NVIDIAGameWorks/donut (NVRHI example framework)

### **Visual Studio CMake:**
- Official Docs: https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio
- CMake Tutorial: https://cmake.org/cmake/help/latest/guide/tutorial/index.html

### **Vulkan:**
- Vulkan Tutorial: https://vulkan-tutorial.com/
- Vulkan Spec: https://registry.khronos.org/vulkan/

### **DirectX 12:**
- Microsoft Docs: https://learn.microsoft.com/en-us/windows/win32/direct3d12/directx-12-programming-guide

---

## ✅ Next Steps

### **Phase 1: Verify Setup (30 min)**
1. ✅ Open project in Visual Studio
2. ✅ Wait for CMake configuration
3. ✅ Build Debug configuration
4. ✅ Run and verify window opens

### **Phase 2: Integrate NVRHI (2-3 hours)**
1. Create `src/renderer/nvrhi_device.h/cpp`
2. Initialize NVRHI device in `Application::init()`
3. Replace OpenGL calls with NVRHI
4. Test rendering with Vulkan backend

### **Phase 3: Implement ECS Systems (4-6 hours)**
1. Create InputSystem (src/ecs/systems/input_system.h/cpp)
2. Create CameraSystem
3. Create RenderSystem using NVRHI
4. Test all systems together

### **Phase 4: Build Game Features**
1. Implement weapon state machines
2. Add enemy AI
3. Create loot system
4. Build first level

---

**You're ready to build Path of Doom with professional tooling!** 🔥

**Rendering:** NVRHI (Vulkan/DX12)
**Build System:** CMake (in Visual Studio)
**Architecture:** ECS with EnTT
**Next:** Integrate NVRHI and start rendering!
