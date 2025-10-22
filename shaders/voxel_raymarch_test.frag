#version 460 core

in vec3 nearPoint;
in vec3 farPoint;

out vec4 FragColor;

void main() {
    // Simple test - just output a color to verify shader is running
    FragColor = vec4(1.0, 0.0, 1.0, 1.0);  // Magenta
}
