const float pi = 3.14159265359;

float computeShadow(sampler2D shadow_map, vec4 shadow_pos)
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
    
    const float shadowPixelOffset = 1.0 / 8192;
    vec2 shadowSampleArray[] = vec2[]
    (
        vec2(0, 0)
        #if SHADOW_SAMPLES >= 2
        , vec2(shadowPixelOffset, 0)
        , vec2(-shadowPixelOffset, 0)
        , vec2(0, shadowPixelOffset)
        , vec2(0, -shadowPixelOffset)
        #endif
        #if SHADOW_SAMPLES >= 3
        , vec2(shadowPixelOffset, shadowPixelOffset)
        , vec2(-shadowPixelOffset, -shadowPixelOffset)
        , vec2(shadowPixelOffset, -shadowPixelOffset)
        , vec2(-shadowPixelOffset, shadowPixelOffset)
        #endif
        #if SHADOW_SAMPLES >= 4
        , vec2(shadowPixelOffset * 2, 0)
        , vec2(-shadowPixelOffset * 2, 0)
        , vec2(0, shadowPixelOffset * 2)
        , vec2(0, -shadowPixelOffset * 2)
        , vec2(shadowPixelOffset * 2, shadowPixelOffset * 2)
        , vec2(-shadowPixelOffset * 2, -shadowPixelOffset * 2)
        , vec2(shadowPixelOffset * 2, -shadowPixelOffset * 2)
        , vec2(-shadowPixelOffset * 2, shadowPixelOffset * 2)
        #endif
    );
    
    shadow_pos.xyz = shadow_pos.xyz / shadow_pos.w;
    
    const float shadowPercentagePerSample = 1.0 / numberOfShadowSamples;
    const float shadowBias = 0.000000;
    float shadow = 0.0;
    for(int i = 0; i < numberOfShadowSamples; i++)
    {
        float shadowSample = texture2D(shadow_map, shadow_pos.xy + shadowSampleArray[i]).x;
        if(shadowSample + shadowBias < shadow_pos.z)
            shadow += shadowPercentagePerSample;
    }
    
    return shadow;
}

float rand(float co)
{
    return fract(sin(co * 12.9898) * 43758.5453);
}

float rand2(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

float rand3(vec3 co)
{
    return fract(sin(dot(co, vec3(12.9898, 78.233, 36.668))) * 43758.5453);
}

const int ditherIndexMatrix[16] = int[](0,  8,  2,  10,
                                     12, 4,  14, 6,
                                     3,  11, 1,  9,
                                     15, 7,  13, 5);

float dither(float alpha, vec2 pos)
{
    float closestAlpha = (alpha < 0.5) ? 0 : 1;
    float secondClosestAlpha = 1 - closestAlpha;
    int x = int(mod(pos.x, 4));
    int y = int(mod(pos.y, 4));
    float d = ditherIndexMatrix[(x + y * 4)] / 16.0;
    float distance = abs(closestAlpha - alpha);
    return (distance < d) ? closestAlpha : secondClosestAlpha;
}

bool distanceDisolve(vec3 frag_pos, vec3 source_pos, float distance_max, float transition_distance)
{
    float dist = distance(frag_pos, source_pos);
    if (dist > distance_max - transition_distance)
    {
        if (dist < distance_max)
        {
            float trans = (dist - (distance_max - transition_distance)) / transition_distance;
            if (trans < dither(trans, gl_FragCoord.xy)) return true;
        }
        else
        {
            return true;
        }
    }
    
    return false;
}