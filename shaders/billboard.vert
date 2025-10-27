#version 460 core

layout (location = 0) in vec2 aPos;      // Quad vertex position
layout (location = 1) in vec2 aTexCoord; // Texture coordinates

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 spritePos;      // World position of sprite
uniform vec2 spriteSize;     // Size of sprite in meters
uniform vec3 cameraRight;    // Camera right vector
uniform vec3 cameraUp;       // Camera up vector

void main()
{
    // Calculate billboard position facing camera
    vec3 worldPos = spritePos
        + cameraRight * aPos.x * spriteSize.x
        + cameraUp * aPos.y * spriteSize.y;

    gl_Position = projection * view * vec4(worldPos, 1.0);
    TexCoord = aTexCoord;
}
