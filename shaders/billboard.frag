#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D spriteTexture;
uniform vec4 spriteColor;

void main()
{
    vec4 texColor = texture(spriteTexture, TexCoord);

    // Discard transparent pixels
    if (texColor.a < 0.1)
        discard;

    FragColor = texColor * spriteColor;
}
