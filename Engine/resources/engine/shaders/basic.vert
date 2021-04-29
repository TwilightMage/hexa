#version 450

uniform mat4 MVPs[10];
attribute vec3 vCol;
attribute vec2 vUV;
attribute vec3 vPos;
varying vec3 color;

void main()
{
    gl_Position = MVPs[gl_InstanceID] * vec4(vPos, 1.0);
    color = vCol;
}