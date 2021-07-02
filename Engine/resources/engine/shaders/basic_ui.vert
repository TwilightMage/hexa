#version 450

layout(location=000) uniform mat4 INST_mvp[146];
layout(location=146) uniform vec4 INST_uv[146];
layout(location=292) uniform vec4 INST_color[146];

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
    gl_Position = INST_mvp[gl_InstanceID] * vec4(vertices[gl_VertexID].pos, 0.0, 1.0);
    color = INST_color[gl_InstanceID];
    uv = vertices[gl_VertexID].uv * INST_uv[gl_InstanceID].zw + INST_uv[gl_InstanceID].xy;
}