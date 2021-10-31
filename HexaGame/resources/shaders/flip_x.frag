#version 330

uniform sampler2D RT;

in vec2 uv;

out vec4 frag_color;

void main()
{
    frag_color = texture2D(RT, vec2(-uv.x, uv.y));
}