#version 450
#extension GL_ARB_bindless_texture : require

#define transparency

layout(bindless_sampler) uniform sampler2D INST_texture[145];

flat in int InstanceID;
in vec4 color;
in vec2 uv;

out vec4 frag_color;

void main()
{
    frag_color = texture(INST_texture[InstanceID], uv) * color;
    if (frag_color.a < 0.1)
    {
        discard;
    }
}