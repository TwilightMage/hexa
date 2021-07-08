#version 450
#extension GL_ARB_bindless_texture : require

layout(bindless_sampler) uniform sampler2D INST_texture[230];

uniform vec3 ambient_light;
uniform vec3 sun_light;
uniform vec3 sun_dir;

flat in int InstanceID;
in vec2 uv;
in vec3 norm;

out vec4 frag_color;

void main()
{
    vec3 N = normalize(norm);
    
    float diff = max(dot(N, sun_dir), 0.0);
    vec3 diffuse = diff * sun_light;

    frag_color = texture(INST_texture[InstanceID], uv) * vec4((ambient_light + diffuse), 1.0);
}