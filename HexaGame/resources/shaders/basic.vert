#version 330

#include "utils_vs.glsl"

#ifndef INSTANCING
uniform mat4 model;
#endif
uniform mat4 view;
uniform mat4 proj;
uniform float time;
uniform mat4 shadow_vp_0;
uniform vec4 texel_offsets; // SHADOW_PASS
uniform vec4 wind_data;     // WIND / (box_height, scale, -, -)

const vec4 wind_data_ = vec4(33, 1, 0, 0);

in vec4 vertex;
in vec4 uv0;
#ifdef INSTANCING
in vec4 uv1;
in vec4 uv2;
in vec4 uv3;
#endif
in vec3 normal;

out vec3 cam_pos;      // camera world-space position
out vec2 uv;           // vertex uv
out vec3 world_pos;    // vertex world-space position
out vec3 obj_pos;      // vertex object-space position
out vec3 norm;         // vertex normal
out vec4 shadow_pos_0;
out vec2 depth;

void main()
{
    #ifdef INSTANCING
    mat4 model;
    model[0] = uv1;
    model[1] = uv2;
    model[2] = uv3;
    model[3] = vec4( 0, 0, 0, 1 );
    #endif
    
    obj_pos = vertex.xyz;
    uv = uv0.xy;

    #ifdef INSTANCING
    vec4 world_pos_raw = vertex * model;
    norm = normal * mat3(transpose(inverse(model)));
    #else
    vec4 world_pos_raw = model * vertex;
    norm = mat3(transpose(inverse(model))) * normal;
    #endif
    cam_pos = inverse(view)[3].xyz;
    shadow_pos_0 = shadow_vp_0 * world_pos_raw;
    
    #ifdef WIND
    float wind_alpha = vertex.z / wind_data_.x;
    float time_offset = length(world_pos_raw.xy);
    vec2 shift = vec2(sin(time * 4 + time_offset) * pow(cos(time * 4 + time_offset), 2), cos(time * 4 + time_offset) * pow(sin(time * 4 + time_offset), 2)) * wind_data_.y * 10;
    world_pos_raw = world_pos_raw + vec4(shift, 0, 0) * rand3(world_pos_raw.xyz) * wind_alpha * sin((time + time_offset) / 4);
    #endif

    vec4 out_pos = proj * view * world_pos_raw;
    
    #ifdef SHADOW_PASS
    out_pos.xy += texel_offsets.zw * out_pos.w;
    #endif

    world_pos = world_pos_raw.xyz;
    gl_Position = out_pos;
    depth = out_pos.zw;
}