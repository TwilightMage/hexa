const float pi = 3.14159265359;

const float shadow_distance = 1000;
const float shadow_disolve = 500;
const float render_distance = 3000;
const float render_disolve = 500;

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
