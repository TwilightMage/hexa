float4x4 worldViewProj;

struct v2f
{
    float2 uv : TEXCOORD0;
};

v2f vert(float4 posIn      : POSITION,
         out float4 posOut : POSITION
)
{
    v2f Out;
    
    posOut = mul(worldViewProj, posIn);

    const float2 p = sign(posIn.xy);
    Out.uv = (float2(p.x, -p.y) + 1.0) / 2.0;

    return Out;
}