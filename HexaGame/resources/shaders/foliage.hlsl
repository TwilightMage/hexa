float4x4 viewProj;
float time;

sampler2D diffuseMap : register(s0);

struct v2f
{
    float2 uv   : TEXCOORD0;
    float3 norm : NORMAL;
};

float rand3(float3 co)
{
    return frac(sin(dot(co, float3(12.9898, 78.233, 36.668))) * 43758.5453);
}

v2f vert(float4 posIn      : POSITION,
         float2 uv         : TEXCOORD0,
         float3 norm       : NORMAL,
         float4 mat14      : TEXCOORD1,
         float4 mat24      : TEXCOORD2,
         float4 mat34      : TEXCOORD3,
         out float4 posOut : POSITION
)
{
    v2f Out;

    float4x4 world;
    world[0] = mat14;
    world[1] = mat24;
    world[2] = mat34;
    world[3] = float4(0, 0, 0, 1);

    float4 worldPos = mul(world, posIn);

    const float wind_alpha = posIn.z / 33.33;
    const float time_offset = length(worldPos.xy);
    const float2 shift = float2(sin(time * 4 + time_offset) * pow(cos(time * 4 + time_offset), 2), cos(time * 4 + time_offset) * pow(sin(time * 4 + time_offset), 2)) * 10;
    worldPos.xy += shift * rand3(worldPos.xyz) * wind_alpha * sin((time + time_offset) / 4);

    posOut = mul(viewProj, worldPos);
    Out.uv = uv;

    world[0].w = 0;
    world[1].w = 0;
    world[2].w = 0;
    
    Out.norm = mul(world, norm);

    return Out;
}
