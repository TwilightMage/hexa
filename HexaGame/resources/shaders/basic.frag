#version 450

#include "utils_fs.glsl"

uniform sampler2D tex;
uniform sampler2D shadow_map_0;
uniform vec4 ambient_light;
uniform vec4 sun_diff, sun_dir;

in vec2 uv;
in vec3 norm;
in vec4 shadow_pos_0;
in vec2 depth;
in vec3 cam_pos;
in vec3 world_pos;

out vec4 frag_color;

void main()
{
    vec4 tex_color = texture2D(tex, uv);

    #ifdef CUTOUT
    if (tex_color.a < CUTOUT) discard;
    #endif
    
    #ifdef SHADOW_PASS
    float finalDepth = depth.x / depth.y;
    frag_color = vec4(finalDepth, finalDepth, finalDepth, 1.0);
    return;
    #endif
    
    //if (distanceDisolve(world_pos, cam_pos, shadow_distance, shadow_disolve)) discard;
    
    float diff = max(dot(norm, -sun_dir.xyz), 0.0);
    vec4 diffuse = diff * sun_diff;

    float shadow = 0;
    //if (!distanceDisolve(world_pos, cam_pos, shadow_distance, shadow_disolve))
    //{
        shadow = computeShadow(shadow_map_0, shadow_pos_0);
    //}

    frag_color = (ambient_light + (1 - shadow) * diffuse) * tex_color;
}
