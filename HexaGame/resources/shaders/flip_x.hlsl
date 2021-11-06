sampler RT : register(s0);

float4 frag(float2 uv : TEXCOORD0) : COLOR0
{
    return tex2D(RT, float2(-uv.x, uv.y));
}