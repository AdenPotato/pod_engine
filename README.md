# Pod Engine - OpenGL Learning Project

A low-level OpenGL engine focused on learning modern rendering techniques including lighting, shadows, and global illumination.

## Features

- Modern OpenGL 4.6 Core Profile
- CMake build system with automatic dependency management
- FPS camera system with mouse and keyboard controls
- Shader abstraction system
- Cross-platform support (Windows, Linux, macOS)

## Prerequisites

- CMake 3.20 or higher
- C++17 compatible compiler (Clang recommended, GCC or MSVC also work)
- Git (for fetching dependencies)

## Building the Project

### Windows (with Clang)

```bash
# Create build directory
mkdir build
cd build

# Configure with Clang (if installed)
cmake .. -G "Ninja" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++

# Or use Visual Studio generator with Clang
cmake .. -G "Visual Studio 17 2022" -T ClangCL

# Build
cmake --build . --config Release
```

### Windows (with Visual Studio)

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

### Linux/macOS

```bash
mkdir build
cd build
cmake .. -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
make -j$(nproc)
```

## Running

After building, run the executable:

```bash
# Windows
.\build\Release\PodEngine.exe

# Linux/macOS
./build/PodEngine
```

## Controls

- **WASD** - Move camera (forward, left, backward, right)
- **Space** - Move up
- **Left Shift** - Move down
- **Mouse** - Look around
- **Mouse Scroll** - Zoom in/out
- **ESC** - Exit

## Project Structure

```
pod_engine/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── include/                # Header files
│   ├── shader.h           # Shader loading and management
│   └── camera.h           # FPS camera implementation
├── src/                    # Source files
│   ├── main.cpp           # Application entry point
│   ├── shader.cpp         # Shader implementation
│   └── camera.cpp         # Camera implementation
├── shaders/                # GLSL shader files
│   ├── basic.vert         # Basic vertex shader
│   └── basic.frag         # Basic fragment shader
└── external/               # External dependencies (auto-fetched)
```

## Learning Roadmap

This project is designed to progressively learn advanced rendering techniques:

### Phase 1: Fundamentals (Current)
- [x] OpenGL context and window creation
- [x] Basic shader pipeline
- [x] 3D transformations (Model-View-Projection)
- [x] FPS camera system
- [ ] Texture loading and mapping
- [ ] Basic mesh loading (OBJ format)

### Phase 2: Lighting
- [ ] Phong lighting model (ambient, diffuse, specular)
- [ ] Multiple light types (directional, point, spotlight)
- [ ] Normal mapping for surface detail
- [ ] Blinn-Phong shading
- [ ] Physically Based Rendering (PBR) basics

### Phase 3: Shadows
- [ ] Shadow mapping (directional lights)
- [ ] PCF (Percentage Closer Filtering)
- [ ] Cascaded shadow maps for large scenes
- [ ] Point light shadows (cubemap shadows)
- [ ] Soft shadows

### Phase 4: Global Illumination
- [ ] Ambient occlusion (SSAO)
- [ ] Screen-space reflections (SSR)
- [ ] Light probes and environment mapping
- [ ] Voxel cone tracing (simple GI)
- [ ] Path tracing basics (optional, for learning)

### Phase 5: Advanced Features
- [ ] Scene hierarchy and entity system
- [ ] Collision detection and response
- [ ] Particle systems
- [ ] UI rendering system
- [ ] Audio system integration
- [ ] Performance profiling and optimization

## Dependencies

All dependencies are automatically fetched by CMake:

- **GLFW** (3.4) - Window and input handling
- **GLAD** (2.0.5) - OpenGL function loader
- **GLM** (1.0.1) - Mathematics library for graphics
- **stb_image** (latest) - Image loading for textures

## Resources for Learning

### OpenGL Tutorials
- [LearnOpenGL](https://learnopengl.com/) - Comprehensive modern OpenGL tutorials
- [OpenGL SuperBible](https://www.openglsuperbible.com/) - Advanced rendering techniques
- [Rendering Algorithms](https://www.pbr-book.org/) - Physically based rendering theory

### Lighting & Shadows
- Real-Time Rendering (book) - Industry standard reference
- GPU Gems series - Advanced rendering techniques
- [Sébastien Lagarde's blog](https://seblagarde.wordpress.com/) - PBR and lighting

### Global Illumination
- [GPU-based GI Techniques](https://www.gdcvault.com/browse/gdc-18/play/1024801) - GDC talks
- [Voxel Cone Tracing](https://research.nvidia.com/publication/interactive-indirect-illumination-using-voxel-cone-tracing)
- [Path Tracing in One Weekend](https://raytracing.github.io/) - Ray tracing basics


## Troubleshooting

### CMake can't find Clang
If Clang is installed but CMake can't find it, specify the full path:
```bash
cmake .. -DCMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang.exe" \
         -DCMAKE_CXX_COMPILER="C:/Program Files/LLVM/bin/clang++.exe"
```

### GLAD compilation errors
If you encounter GLAD-related errors, the dependency might not have fetched correctly:
```bash
rm -rf build/_deps
cmake --build . --clean-first
```

### Shaders not found
Make sure you're running the executable from the build directory, or the shaders have been copied correctly.

## License

This is a learning project. Feel free to use and modify as needed.
