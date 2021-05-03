#version 450

layout(location=000) uniform mat4 MVPs[225];
layout(location=230) uniform int atlasIDs[225];
layout(location=460) uniform vec4 atlasMods[50];

in vec3 vCol;
in vec2 vUV;
in vec3 vPos;

out vec3 color;
out vec2 uv;

void main()
{
    gl_Position = MVPs[gl_InstanceID] * vec4(vPos, 1.0);
    color = vCol;
    uv.x =      vUV.x  * atlasMods[atlasIDs[gl_InstanceID]].r + atlasMods[atlasIDs[gl_InstanceID]].b;
    uv.y = (1 - vUV.y) * atlasMods[atlasIDs[gl_InstanceID]].g + atlasMods[atlasIDs[gl_InstanceID]].a;
}