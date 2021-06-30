#version 450
#extension GL_ARB_bindless_texture : require

layout(bindless_sampler, location=230) uniform sampler2D bindless[230];
layout(location=462) uniform vec3 ambient_light;
layout(location=463) uniform vec3 sun_light;
layout(location=464) uniform vec3 sun_dir;

flat in int InstanceID;
in vec3 color;
in vec2 uv;
in vec3 frag_pos;
in vec3 norm;

void main()
{
    float diff = max(dot(norm, sun_dir), 0.0);
    vec3 diffuse = diff * sun_light;
    
    gl_FragColor = texture(bindless[InstanceID], uv) * vec4((ambient_light + diffuse) * color, 1.0);
}