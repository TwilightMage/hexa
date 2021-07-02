#version 450
#extension GL_ARB_bindless_texture : require

layout(bindless_sampler, location=230) uniform sampler2D INST_texture[230];
layout(location=462) uniform vec3 ambient_light;
layout(location=463) uniform vec4 sun_light;
layout(location=464) uniform vec3 sun_dir;

flat in int InstanceID;
in vec3 color;
in vec2 uv;
in vec3 frag_pos;
in vec3 norm;
in vec3 cam_pos;

const float diff_strength = 0.6;
const float specular_strength = 0.4;

void main()
{
    vec3 N = normalize(norm);
    
    vec3 cam_dir = normalize(cam_pos - frag_pos);
    vec3 reflect_dir = reflect(-sun_dir, N);
    float spec = specular_strength * pow(max(dot(cam_dir, reflect_dir), 0.0), 32);
    
    float diff = diff_strength * max(dot(N, sun_dir), 0.0);

    float intensity = diff + spec;

    if (intensity > 0.9)
    {
        intensity = 1.1;
    }
    else if (intensity > 0.5)
    {
        intensity = 0.7;
    }
    else
    {
        intensity = 0.5;
    }
    
    gl_FragColor = texture(INST_texture[InstanceID], uv) * vec4((ambient_light + intensity * sun_light.xyz * sun_light.w) * color, 1.0);
}