#version 450

layout(location=408) uniform sampler2D atlas;

in vec4 color;
in vec2 uv;

void main()
{
    gl_FragColor = texture(atlas, uv) * color;
}