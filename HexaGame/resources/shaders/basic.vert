#version 330

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 shadow_vp_0;
uniform mat4 model_view_proj;
uniform vec4 texel_offsets;

in vec4 vertex;
in vec2 uv0;
in vec3 normal;

out vec3 cam_pos;      // camera world-space position
out vec2 uv;           // vertex uv
out vec3 world_pos;    // vertex world-space position
out vec3 obj_pos;      // vertex object-space position
out vec3 norm;         // vertex normal
out vec4 shadow_pos_0;
out vec2 depth;        // available only during SHADOW_PASS

void main()
{
    #ifdef SHADOW_PASS
    vec4 outPos = model_view_proj * vertex;
    outPos.xy += texel_offsets.zw * outPos.w;

    depth = outPos.zw;
    gl_Position = outPos;
    return;
    #endif
    
    obj_pos = vertex.xyz;
    vec4 pos_model = model * vertex;
    gl_Position = proj * view * pos_model;
    uv = uv0;
    world_pos = pos_model.xyz;
    norm = mat3(transpose(inverse(model))) * normal;
    cam_pos = inverse(view)[3].xyz;
    shadow_pos_0 = shadow_vp_0 * pos_model;
}