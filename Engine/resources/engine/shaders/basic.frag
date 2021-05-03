#version 450

uniform sampler2D sampler;

in vec3 color;
in vec2 uv;

void main()
{
    gl_FragColor = texture(sampler, uv) * vec4(color, 1.0);
}