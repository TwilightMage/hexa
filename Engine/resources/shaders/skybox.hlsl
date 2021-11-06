#define up      float3(0, 0, 1)

#define top     float4(0.5, 0.75, 1,   1)
#define horizon float4(1,   1,    1,   1)
#define bottom  float4(0.2, 0.2,  0.2, 1)

float4x4 worldViewProj;

struct v2f
{
    float3 pos : TEXCOORD0;
};

v2f vert(float4     posIn  : POSITION,
         out float4 posOut : POSITION
)
{
    v2f Out;

    posOut = mul(worldViewProj, posIn);
    Out.pos = posIn.xyz;

    return Out;
}

float4 frag(v2f f) : COLOR0
{
    const float y = (dot(up, normalize(f.pos)) + 1) * 0.5;

    if (y > 0.5)
    {
        return lerp(horizon, top, (y - 0.5) * 2);
    }
    else
    {
        return lerp(bottom, horizon, y * 2);
    }
}
