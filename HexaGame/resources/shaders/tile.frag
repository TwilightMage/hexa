#version 330

uniform sampler2D tile_texture;
uniform vec4 ambient_light;
uniform vec4 sun_diff, sun_dir;

in vec2 uv;
in vec3 norm;

out vec4 frag_color;

void main()
{
    float diff = max(dot(norm, -sun_dir.xyz), 0.0);
    vec4 diffuse = diff * sun_diff;

    frag_color = texture2D(tile_texture, uv) * (ambient_light + diffuse);
}
