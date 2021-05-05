#version 450

layout(location=0) uniform mat4 MVPs[230];

in vec3 vCol;
in vec2 vUV;
in vec3 vPos;

out int InstanceID;
out vec3 color;
out vec2 uv;

void main()
{
    InstanceID = gl_InstanceID;
    gl_Position = MVPs[gl_InstanceID] * vec4(vPos, 1.0);
    color = vCol;
    uv = vUV;
}