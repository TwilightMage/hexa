#version 450

uniform sampler2D normal_map;
uniform sampler2D tex;
uniform float far_distance;

in vec3 camera_pos;
in vec3 world_normal;
//in vec3 world_tangent;
//in vec3 world_bi_normal;
in vec2 uv;

out vec4 Color0;
out vec4 Color1;

void ToGBufferFP()
{
    Color0.rgb = texture2D(tex, uv);
    Color0.a = 0;
    vec3 texNormal = (texture2D(normal_map, uv) - 0.5) * 2;
    //mat3 normalRotation = mat3(world_tangent, world_bi_normal, world_normal);
    //Color1.rgb = normalize(mul(texNormal, normalRotation));
    Color1.a = length(camera_pos) / far_distance;
}