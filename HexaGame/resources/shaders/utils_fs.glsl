#include "utils.glsl"

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
    
    const float shadowPixelOffset = 1.0 / 2046;
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

bool distanceDisolve(vec3 frag_pos, vec3 cam_pos, float distance_max, float transition_distance)
{
    float dist = distance(frag_pos.xy, cam_pos.xy);
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