#version 450
#extension GL_ARB_bindless_texture : require

layout(bindless_sampler, location=230) uniform sampler2D bindless[230];

flat in int InstanceID;
in vec3 color;
in vec2 uv;

void main()
{
    gl_FragColor = texture(bindless[InstanceID], uv) * vec4(color, 1.0);
}