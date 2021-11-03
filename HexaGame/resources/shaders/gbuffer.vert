#version 450

uniform mat4 mvp;
uniform mat4 vp;

in vec4 vertex;
in vec2 uv0;
in vec3 normal;

out vec3 camera_pos;
out vec3 world_normal;
out vec2 uv;

void main()
{
    gl_Position = mvp * vertex;
    world_normal = (vp * vec4(normal, 0)).xyz;
    camera_pos = (vp * vertex).xyz;
    uv = uv0;
}
