#version 450

#define empty_vertex 6

uniform mat4 INST_model[145];
uniform vec4 INST_uv[145];
uniform vec4 INST_color[145];
uniform mat4 projection;

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
    gl_Position = projection * INST_model[gl_InstanceID] * vec4(vertices[gl_VertexID].pos, 0.0, 1.0);
    color = INST_color[gl_InstanceID];
    uv = vertices[gl_VertexID].uv * INST_uv[gl_InstanceID].zw + INST_uv[gl_InstanceID].xy;
}