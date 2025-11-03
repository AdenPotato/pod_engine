cmake_minimum_required(VERSION 3.20)
project(PathOfDoom VERSION 1.0.0 LANGUAGES C CXX)

# ===== PROJECT CONFIGURATION =====

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE integration (CLion, VSCode, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add FetchContent for dependency management
include(FetchContent)

# ===== DEPENDENCIES =====

# GLFW - Window and input handling
message(STATUS "Fetching GLFW...")
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)



# GLAD - OpenGL function loader
# Note: You need to generate GLAD files first from https://glad.dav1d.de/
# Select: Language=C/C++, Specification=OpenGL, API gl=Version 4.6, Profile=Core
# Download and extract to external/glad/ folder
# Alternatively, use the pre-generated files approach below

# Check if GLAD exists in external/glad
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/src/glad.c)
    message(STATUS "Using GLAD from external/glad/")
    add_library(glad STATIC
            ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/src/glad.c
    )
    target_include_directories(glad PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
    )
else()
    # Fallback: Fetch a pre-generated GLAD library
    message(STATUS "Fetching pre-generated GLAD...")
    FetchContent_Declare(
            glad
            URL https://github.com/castano/nvidia-texture-tools/raw/master/extern/glad.c
            URL_HASH MD5=SKIP
            DOWNLOAD_NO_EXTRACT TRUE
    )

    # Use a simpler approach - download pre-built GLAD
    message(WARNING "GLAD not found in external/glad/. Please generate GLAD files:")
    message(WARNING "  1. Go to https://glad.dav1d.de/")
    message(WARNING "  2. Language: C/C++, GL: 4.6, Profile: Core")
    message(WARNING "  3. Download and extract to: ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/")
    message(FATAL_ERROR "GLAD files not found. Please generate them first.")
endif()

# GLM - Mathematics library
message(STATUS "Fetching GLM...")
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# EnTT - Entity Component System
message(STATUS "Fetching EnTT...")
FetchContent_Declare(
        entt
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.13.2
)
FetchContent_MakeAvailable(entt)

# ImGui - Immediate mode GUI
message(STATUS "Fetching ImGui...")
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.90.1
)
FetchContent_MakeAvailable(imgui)

# spdlog - Fast C++ logging library
message(STATUS "Fetching spdlog...")
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(spdlog)

# Suppress warnings from spdlog on MSVC
if(MSVC)
    target_compile_options(spdlog INTERFACE /W0)
endif()

# stb_image - Image loading (header-only)
message(STATUS "Fetching stb...")
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
)
FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
    # stb is header-only, just need the include path
endif()

# Build ImGui as a static library
add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC glfw glad)

# ===== PROJECT SOURCE FILES =====

set(PROJECT_SOURCES
        src/main.cpp
        src/Core/Application/Application.cpp
        src/Core/Logger/Log.cpp
        src/Core/Platform/Windows/WindowsWindow.cpp
        src/Core/Application/Layer.cpp
        src/Core/Application/LayerStack.cpp
        src/Core/ImGui/ImGuiLayer.cpp
        src/Core/ImGui/ImGuiLayer.h
        src/Core/Platform/Windows/WindowsInput.cpp
        src/Core/Platform/Windows/WindowsInput.h
)

set(PROJECT_HEADERS
        src/Core/Application/Application.h
        src/Core/Logger/Log.h
        src/Core/Core.h
        src/Core/Events/Event.h
        src/Core/Events/ApplicationEvent.h
        src/Core/Events/KeyEvent.h
        src/Core/Events/MouseEvent.h
        src/Core/Window/Window.h
        src/Core/Platform/Windows/WindowsWindow.h
        src/Core/Application/Layer.h
        src/Core/Application/LayerStack.h
        src/podpch.h
        src/Core/ImGui/ImGuiLayer.cpp
        src/Core/ImGui/ImGuiLayer.h
        src/Core/Input/Input.h
        src/Core/Platform/Windows/WindowsInput.cpp
        src/Core/Platform/Windows/WindowsInput.h
        src/Core/Input/KeyCodes.h
        src/Core/Input/MouseButtonCodes.h
)

# ===== EXECUTABLE =====

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

# Include directories - src/ is the root
target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${stb_SOURCE_DIR}
)

# Enable precompiled headers (speeds up compilation)
target_precompile_headers(${PROJECT_NAME}
        PRIVATE
        src/podpch.h
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        glfw
        glad
        glm::glm
        imgui
        EnTT::EnTT
        spdlog::spdlog
)

# Tell GLFW not to include OpenGL headers in our code (we use GLAD)
target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)

# ===== PLATFORM-SPECIFIC SETTINGS =====

if(WIN32)
    # Link OpenGL on Windows
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)

elseif(APPLE)
    # Link OpenGL framework on macOS
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGL")

else()
    # Link OpenGL on Linux
    target_link_libraries(${PROJECT_NAME} PRIVATE GL)
endif()

# ===== COMPILER SETTINGS =====

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
            /W4                 # Warning level 4
            /permissive-        # Standards conformance
            /wd4996             # Disable deprecated warnings
            /external:W0        # Suppress warnings from external headers
    )

    # Set PathOfDoom as startup project in Visual Studio
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

else()
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()

# ===== POST-BUILD: COPY ASSETS =====

# Copy shaders to build directory (if they exist)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/shaders
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
            COMMENT "Copying shaders to build directory..."
    )
endif()

# Copy assets to build directory (if they exist)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
            COMMENT "Copying assets to build directory..."
    )
endif()

# ===== CONFIGURATION SUMMARY =====

message(STATUS "===========================================")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "===========================================")
message(STATUS "Dependencies:")
message(STATUS "  GLFW:   ${glfw_VERSION}")
message(STATUS "  GLAD:   OpenGL 4.6 Core")
message(STATUS "  GLM:    Header-only")
message(STATUS "  EnTT:   v3.13.2")
message(STATUS "  ImGui:  v1.90.1")
message(STATUS "  spdlog: v1.13.0")
message(STATUS "  stb:    Header-only")
message(STATUS "===========================================")
