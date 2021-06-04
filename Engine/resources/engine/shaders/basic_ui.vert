#version 450

layout(location=000) uniform mat4 instance_MVP[146];
layout(location=146) uniform vec4 instance_uv[146];
layout(location=292) uniform vec4 instance_color[146];

const struct Vertex {
    vec2 pos;
    vec2 uv;
} vertices[6] = {
    { { 1.0, -1.0 }, { 1.0, 1.0 } },
    { { 0.0, -1.0 }, { 0.0, 1.0 } },
    { { 1.0,  0.0 }, { 1.0, 0.0 } },
    { { 1.0,  0.0 }, { 1.0, 0.0 } },
    { { 0.0, -1.0 }, { 0.0, 1.0 } },
    { { 0.0,  0.0 }, { 0.0, 0.0 } }
};

out int InstanceID;
out vec4 color;
out vec2 uv;

void main()
{
    InstanceID = gl_InstanceID;
    gl_Position = instance_MVP[gl_InstanceID] * vec4(vertices[gl_VertexID].pos, 0.0, 1.0);
    color = instance_color[gl_InstanceID];
    uv = vertices[gl_VertexID].uv * instance_uv[gl_InstanceID].xy + instance_uv[gl_InstanceID].zw;
}