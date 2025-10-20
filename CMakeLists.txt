cmake_minimum_required(VERSION 3.20)
project(PodEngine VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use Clang if available
if(NOT DEFINED CMAKE_C_COMPILER)
    find_program(CLANG_EXECUTABLE NAMES clang)
    if(CLANG_EXECUTABLE)
        set(CMAKE_C_COMPILER ${CLANG_EXECUTABLE})
    endif()
endif()

if(NOT DEFINED CMAKE_CXX_COMPILER)
    find_program(CLANGXX_EXECUTABLE NAMES clang++)
    if(CLANGXX_EXECUTABLE)
        set(CMAKE_CXX_COMPILER ${CLANGXX_EXECUTABLE})
    endif()
endif()

# Export compile commands for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add FetchContent for dependency management
include(FetchContent)

# GLFW - Window and input handling
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLM - Mathematics library for graphics
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# GLAD - OpenGL function loader (using pre-generated files)
set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/glad)
add_library(glad STATIC
    ${GLAD_DIR}/src/glad.c
)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)

# stb_image - Image loading (for textures)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
    # stb is header-only, just need to include it
endif()

# ImGui - Immediate mode GUI library
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.1
)
FetchContent_MakeAvailable(imgui)

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

# Engine source files
set(ENGINE_SOURCES
    src/main.cpp
    src/application/application.cpp
    src/window/window.cpp
    src/renderer/renderer.cpp
    src/shader/shader.cpp
    src/camera/camera.cpp
    src/mesh/mesh.cpp
    src/texture/texture.cpp
    src/imgui_layer/imgui_layer.cpp
)

set(ENGINE_HEADERS
    src/application/application.h
    src/window/window.h
    src/renderer/renderer.h
    src/shader/shader.h
    src/camera/camera.h
    src/mesh/mesh.h
    src/texture/texture.h
    src/imgui_layer/imgui_layer.h
)

# Create executable
add_executable(${PROJECT_NAME} ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# Include directories - add all src subdirectories
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/application
        ${CMAKE_CURRENT_SOURCE_DIR}/src/window
        ${CMAKE_CURRENT_SOURCE_DIR}/src/renderer
        ${CMAKE_CURRENT_SOURCE_DIR}/src/shader
        ${CMAKE_CURRENT_SOURCE_DIR}/src/camera
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mesh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/texture
        ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_layer
        ${stb_SOURCE_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        glad
        glm::glm
        imgui
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGL")
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE GL)
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy shaders to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Also copy shaders next to executable for multi-config generators (MSVC)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)

# Print configuration info
message(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
