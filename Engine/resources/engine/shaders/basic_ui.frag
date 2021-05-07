#version 450
#extension GL_ARB_bindless_texture : require

layout(bindless_sampler, location=438) uniform sampler2D bindless[146];

flat in int InstanceID;
in vec4 color;
in vec2 uv;

void main()
{
    gl_FragColor = texture(bindless[InstanceID], uv) * color;
}