#ifndef INSTANCING
float4x4 world;
#endif
float4x4 view;
float4x4 proj;
float farDistance;

sampler diffuseMap : register(s0);

struct v2f
{
    float2 uv           : TEXCOORD0;
    float4 normDist     : TEXCOORD1;
};

v2f vert(float4 posIn      : POSITION,
         float2 uv         : TEXCOORD0,
         float4 norm       : NORMAL,
#ifdef INSTANCING
         float4 mat14      : TEXCOORD1,
         float4 mat24      : TEXCOORD2,
         float4 mat34      : TEXCOORD3,
#endif
         out float4 posOut : POSITION
)
{
    v2f Out;

#ifdef INSTANCING
    float4x4 world;
    world[0] = mat14;
    world[1] = mat24;
    world[2] = mat34;
    world[3] = float4(0, 0, 0, 1);
#endif
    
    const float3 worldViewPos = mul(view, mul(world, posIn));
    Out.uv = uv;

    world[0].w = 0;
    world[1].w = 0;
    world[2].w = 0;
    
    Out.normDist.xyz = mul(world, norm).xyz;
    Out.normDist.w = length(worldViewPos) / farDistance;

    posOut = mul(proj, worldViewPos);

    return Out;
}

void frag(v2f In,

          out float4 color0 : COLOR0,
          out float4 color1 : COLOR1
)
{
    color0.rgb = tex2D(diffuseMap, In.uv).rgb;  // color
    color0.a = In.normDist.w; // depth

    color1.rgb = normalize(In.normDist.xyz); // normal
    color1.a = 1;
}