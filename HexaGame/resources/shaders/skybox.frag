#version 450

#include "utils_fs.glsl"

const vec4 sky_top = vec4(0.5, 0.75, 1, 1);
const vec4 sky_horizon = vec4(1, 1, 1, 1);
const vec4 sky_down = vec4(0.2, 0.2, 0.2, 1);

const vec3 forward = vec3(1, 0, 0);
const vec3 right   = vec3(0, 1, 0);
const vec3 up      = vec3(0, 0, 1);

in vec3 obj_pos;

out vec4 frag_color;

void main()
{
    vec3 norm = normalize(obj_pos);
    vec2 sp_pos = vec2((atan(dot(forward, norm), dot(right, norm)) / pi + 1) * 0.5, (dot(up, norm) + 1) * 0.5);
    
    if (sp_pos.y > 0.5)
    {
        frag_color = mix(sky_horizon, sky_top, (sp_pos.y - 0.5) * 2);
    }
    else
    {
        frag_color = mix(sky_down, sky_horizon, sp_pos.y * 2);
    }
}