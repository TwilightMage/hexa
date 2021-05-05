#version 450
#extension GL_ARB_bindless_texture : require

uniform sampler2D sampler;

in vec3 color;
in vec2 uv;

void main()
{
    gl_FragColor = texture(sampler, uv) * vec4(color, 1.0);
}