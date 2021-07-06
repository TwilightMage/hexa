#version 450

uniform mat4 INST_model[230];

uniform mat4 view;
uniform mat4 projection;

in vec3 vPos;
in vec2 vUV;
in vec3 vNorm;

out int InstanceID;
out vec3 cam_pos;
out vec2 uv;
out vec3 world_pos;
out vec3 norm;

void main()
{
    mat4 model = INST_model[gl_InstanceID];
    
    InstanceID = gl_InstanceID;
    gl_Position = projection * view * model * vec4(vPos, 1.0);
    uv = vUV;
    world_pos = (model * vec4(vPos, 1.0)).xyz;
    norm = mat3(transpose(inverse(model))) * vNorm;
    cam_pos = inverse(view)[3].xyz;
}