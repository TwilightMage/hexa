const float shadowResolution = 2048;

float rand(float co)
{
    return frac(sin(co * 12.9898) * 43758.5453);
}

float rand2(float2 co)
{
    return frac(sin(dot(co, float2(12.9898, 78.233))) * 43758.5453);
}

float rand3(float3 co)
{
    return frac(sin(dot(co, float3(12.9898, 78.233, 36.668))) * 43758.5453);
}

const int ditherIndexMatrix[16] = {0,  8,  2,  10,
                                   12, 4,  14, 6,
                                   3,  11, 1,  9,
                                   15, 7,  13, 5};

float dither(float alpha, float2 pos)
{
    const float closestAlpha = (alpha < 0.5) ? 0 : 1;
    const float secondClosestAlpha = 1 - closestAlpha;
    const int x = int(fmod(pos.x, 4));
    const int y = int(fmod(pos.y, 4));
    const float d = ditherIndexMatrix[(x + y * 4)] / 16.0;
    const float distance = abs(closestAlpha - alpha);
    return (distance < d) ? closestAlpha : secondClosestAlpha;
}

bool inRange(float a, float m1, float m2)
{
    return (a / m1 - 1) * (a / m2 - 1) < 0;
}

float3 calculateWind(float3 vertexPos, float3 worldPos, float time, float2 windData)
{
    const float wind_alpha = vertexPos.z / windData.x;
    const float timeOffset = length(worldPos.xy);
    const float2 shift = float2(sin(time * 4 + timeOffset) * pow(cos(time * 4 + timeOffset), 2), cos(time * 4 + timeOffset) * pow(sin(time * 4 + timeOffset), 2)) * windData.y * 10;
    return worldPos + float3(shift * rand3(worldPos) * wind_alpha * sin((time + timeOffset) / 4), 0);
}

bool distanceDisolve(float3 worldPos, float3 camPos, float distanceMax, float disolveRange, float2 pixelCoord)
{
    const float dist = distance(worldPos.xy, camPos.xy);
    float visible = 1.0;
    if (dist > distanceMax - disolveRange)
    {
        if (dist < distanceMax)
        {
            visible *= dither((dist - (distanceMax - disolveRange)) / disolveRange, pixelCoord);
        }
        else
        {
            return true;
        }
    }
    
    return visible == 1.0;
}

float computeShadow(sampler shadowMap, float4 shadowUV)
{
#if !defined(SHADOW_SAMPLES) || SHADOW_SAMPLES == 1
    const int numberOfShadowSamples = 1;
#elif SHADOW_SAMPLES == 2
    const int numberOfShadowSamples = 5;
#elif SHADOW_SAMPLES == 3
    const int numberOfShadowSamples = 9;
#elif SHADOW_SAMPLES == 4
    const int numberOfShadowSamples = 17;
#endif
    
    const float shadowPixelOffset = 1.0 / shadowResolution;
    float2 shadowSampleArray[] = {
        float2(0, 0),
        #if SHADOW_SAMPLES >= 2
        float2(shadowPixelOffset, 0),
        float2(-shadowPixelOffset, 0),
        float2(0, shadowPixelOffset),
        float2(0, -shadowPixelOffset),
        #endif
        #if SHADOW_SAMPLES >= 3
        float2(shadowPixelOffset, shadowPixelOffset),
        float2(-shadowPixelOffset, -shadowPixelOffset),
        float2(shadowPixelOffset, -shadowPixelOffset),
        float2(-shadowPixelOffset, shadowPixelOffset),
        #endif
        #if SHADOW_SAMPLES >= 4
        float2(shadowPixelOffset * 2, 0),
        float2(-shadowPixelOffset * 2, 0),
        float2(0, shadowPixelOffset * 2),
        float2(0, -shadowPixelOffset * 2),
        float2(shadowPixelOffset * 2, shadowPixelOffset * 2),
        float2(-shadowPixelOffset * 2, -shadowPixelOffset * 2),
        float2(shadowPixelOffset * 2, -shadowPixelOffset * 2),
        float2(-shadowPixelOffset * 2, shadowPixelOffset * 2),
        #endif
    };
    
    shadowUV.xyz = shadowUV.xyz / shadowUV.w;
    
    const float shadowPercentagePerSample = 1.0 / numberOfShadowSamples;
    const float shadowBias = 0.000000;
    float shadow = 0.0;
    for(int i = 0; i < numberOfShadowSamples; i++)
    {
        float shadowSample = tex2D(shadowMap, shadowUV.xy + shadowSampleArray[i]).x;
        if(shadowSample + shadowBias < shadowUV.z)
            shadow += shadowPercentagePerSample;
    }
    
    return shadow;
}
