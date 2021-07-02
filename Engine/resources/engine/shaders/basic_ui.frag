#version 450
#extension GL_ARB_bindless_texture : require

#define transparency

layout(bindless_sampler, location=435) uniform sampler2D INST_texture[145];

flat in int InstanceID;
in vec4 color;
in vec2 uv;

void main()
{
    gl_FragColor = texture(INST_texture[InstanceID], uv) * color;
}