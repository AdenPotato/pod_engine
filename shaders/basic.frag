#version 460 core

in vec3 vertexColor;
in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D texture1;
uniform bool useTexture;

void main()
{
    if (useTexture) {
        vec4 texColor = texture(texture1, texCoords);
        FragColor = texColor * vec4(vertexColor, 1.0);
    } else {
        FragColor = vec4(vertexColor, 1.0);
    }
}
