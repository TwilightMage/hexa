#version 330

uniform mat4 mvp;
uniform mat4 model;

in vec4 vertex;
in vec2 uv0;
in vec3 normal;

out vec2 uv;
out vec3 norm;

void main()
{
    gl_Position = mvp * vertex;
    uv = uv0;
    norm = mat3(transpose(inverse(model))) * normal;
}