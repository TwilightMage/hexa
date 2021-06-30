#version 450

layout(location=0) uniform mat4 models[230];
layout(location=460) uniform mat4 view;
layout(location=461) uniform mat4 projection;

in vec3 vCol;
in vec2 vUV;
in vec3 vPos;
in vec3 vNorm;

out int InstanceID;
out vec3 color;
out vec2 uv;
out vec3 frag_pos;
out vec3 norm;
out vec3 cam_pos;

void main()
{
    mat4 model = models[gl_InstanceID];
    
    InstanceID = gl_InstanceID;
    gl_Position = projection * view * model * vec4(vPos, 1.0);
    color = vCol;
    uv = vUV;
    frag_pos = (model * vec4(vPos, 1.0)).xyz;
    mat4 normal_matrix = transpose(inverse(model));
    norm = normalize((normal_matrix * vec4(vNorm, 1.0)).xyz);
    cam_pos = inverse(view)[3].xyz;
}