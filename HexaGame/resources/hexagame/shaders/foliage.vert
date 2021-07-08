#version 450

uniform mat4 INST_model[180];
uniform float INST_height[180];

uniform mat4 view;
uniform mat4 projection;
uniform float time;

in vec3 vPos;
in vec2 vUV;
in vec3 vNorm;

out int InstanceID;
out vec3 cam_pos;
out vec2 uv;
out vec3 world_pos;
out vec3 obj_pos;
out vec3 norm;

float rand(vec3 co)
{
    return fract(sin(dot(co, vec3(12.9898, 78.233, 36.668))) * 43758.5453);
}

void main()
{
    mat4 model = INST_model[gl_InstanceID];

    obj_pos = vPos;
    float alpha = obj_pos.z / INST_height[gl_InstanceID];
    vec3 orig_world_pos = (model * vec4(obj_pos, 1)).xyz;
    
    float time_offset = length(orig_world_pos.xy);
    vec2 shift = vec2(sin(time * 4) * pow(cos(time * 4), 2), cos(time * 4) * pow(sin(time * 4), 2));
    vec4 shifted_position = model * vec4(vPos + vec3(shift, 0) * 0.1 * rand(orig_world_pos) * alpha * sin((time + time_offset) / 4), 1.0);
    
    InstanceID = gl_InstanceID;
    gl_Position = projection * view * shifted_position;
    uv = vUV;
    world_pos = shifted_position.xyz;
    norm = mat3(transpose(inverse(model))) * vNorm;
    cam_pos = inverse(view)[3].xyz;
}