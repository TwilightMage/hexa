#version 450
#extension GL_ARB_bindless_texture : require

layout(bindless_sampler) uniform sampler2D INST_texture[230];
//uniform samplerCube INST_texture[230];

flat in int InstanceID;
flat in vec3 cam_pos;
in vec2 uv;
in vec3 world_pos;
in vec3 norm;

out vec4 frag_color;

void main()
{
    frag_color = /*texture(INST_texture[InstanceID], normalize(world_pos - cam_pos));*/texture(INST_texture[InstanceID], world_pos.xy / 5 + world_pos.z * 10);
}