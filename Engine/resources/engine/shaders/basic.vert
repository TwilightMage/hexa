#version 450

layout(std430, binding = 0) buffer atlasData
{
    vec4 uv_mod[];
} atlas_mods;

layout(location=000) uniform mat4 MVPs[230];
layout(location=230) uniform int atlasIDs[230];

in vec3 vCol;
in vec2 vUV;
in vec3 vPos;

out vec3 color;
out vec2 uv;

void main()
{
    gl_Position = MVPs[gl_InstanceID] * vec4(vPos, 1.0);
    color = vCol;
    uv.x =      vUV.x  * atlas_mods.uv_mod[atlasIDs[gl_InstanceID]].r + atlas_mods.uv_mod[atlasIDs[gl_InstanceID]].b;
    uv.y = (1 - vUV.y) * atlas_mods.uv_mod[atlasIDs[gl_InstanceID]].g + atlas_mods.uv_mod[atlasIDs[gl_InstanceID]].a;
}