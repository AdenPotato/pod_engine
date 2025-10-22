#version 460 core

layout(location = 0) in vec3 aPos;

out vec3 nearPoint;
out vec3 farPoint;

uniform mat4 view;
uniform mat4 projection;

// Unproject a point from NDC to world space
vec3 unprojectPoint(float x, float y, float z, mat4 viewInv, mat4 projInv) {
    vec4 unprojectedPoint = viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);

    // Full-screen quad that covers the entire viewport
    gl_Position = vec4(aPos, 1.0);

    // Calculate ray near and far points for raymarching
    nearPoint = unprojectPoint(aPos.x, aPos.y, 0.0, viewInv, projInv);
    farPoint = unprojectPoint(aPos.x, aPos.y, 1.0, viewInv, projInv);
}
