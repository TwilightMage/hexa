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
out vec3 obj_pos;
out vec3 norm;

void main()
{
    mat4 model = INST_model[gl_InstanceID];

    obj_pos = vPos;
    vec4 pos_model = model * vec4(vPos, 1.0);
    InstanceID = gl_InstanceID;
    gl_Position = projection * view * pos_model;
    uv = vUV;
    world_pos = pos_model.xyz;
    norm = mat3(transpose(inverse(model))) * vNorm;
    cam_pos = inverse(view)[3].xyz;
}