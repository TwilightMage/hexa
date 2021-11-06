#ifndef INSTANCED
float4x4 world;
#endif
float4x4 viewProj;
float4 ambientLight;
float4 sunDiff;
float4 sunDir;

sampler2D diffuseMap : register(s0);

struct v2f
{
    float2 uv   : TEXCOORD0;
    float3 norm : NORMAL;
};

v2f vert(float4 posIn      : POSITION,
         float2 uv         : TEXCOORD0,
         float3 norm       : NORMAL,
#ifdef INSTANCED
         float4 mat14      : TEXCOORD1,
         float4 mat24      : TEXCOORD2,
         float4 mat34      : TEXCOORD3,
#endif
         out float4 posOut : POSITION
)
{
    v2f Out;

#ifdef INSTANCED
    float4x4 world;
    world[0] = mat14;
    world[1] = mat24;
    world[2] = mat34;
    world[3] = float4(0, 0, 0, 1);
#endif

    posOut = mul(viewProj, mul(world, posIn));
    Out.uv = uv;

    world[0].w = 0;
    world[1].w = 0;
    world[2].w = 0;
    
    Out.norm = mul(world, norm);

    return Out;
}

float4 frag(v2f In) : COLOR0
{
    const float4 diff = tex2D(diffuseMap, In.uv);

#ifdef ALPHA_CUT
    if (diff.a < 0.9) discard;
#endif
    
    const float4 lightDiff = max(dot(normalize(In.norm), -sunDir.xyz), 0.0) * sunDiff;
    
    return (ambientLight + 1 * lightDiff) * diff;
}