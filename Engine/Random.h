#pragma once

#include "framework.h"

#define RANDOM_STEP 137591

class Random
{
public:
    Random(uint seed)
        : seed_(seed)
    {
    }

    template<typename T>
    T number()
    {
        srand(seed_ + offset_);
        offset_ += RANDOM_STEP;
        return rand() % RAND_MAX;
    }
    template<typename T>
    T number(T max)
    {
        srand(seed_ + offset_);
        offset_ += RANDOM_STEP;
        return rand() % max;
    }
    template<typename T>
    T number(T min, T max)
    {
        srand(seed_ + offset_);
        offset_ += RANDOM_STEP;
        return min + rand() % (max - min);
    }

    bool boolean()
    {
        srand(seed_ + offset_);
        offset_ += RANDOM_STEP;
        return rand() % 2 == 1;
    }
    
    template<typename T>
    static T static_number(uint seed)
    {
        srand(seed);
        return rand() % RAND_MAX;
    }
    template<typename T>
    static T static_number(uint seed, T max)
    {
        srand(seed);
        return rand() % max;
    }
    template<typename T>
    static T static_number(uint seed, T min, T max)
    {
        srand(seed);
        return min + rand() % (max - min);
    }

    static bool static_boolean(uint seed)
    {
        srand(seed);
        return rand() % 2 == 1;
    }

    static uint random_seed()
    {
        const uint copy = static_seed_;
        static_seed_ += RANDOM_STEP;
        return copy;
    }

private:
    uint seed_ = 1;
    uint offset_ = 0;

    inline static uint static_seed_ = 1;
};

template<>
inline float Random::number()
{
    srand(seed_ + offset_);
    offset_ += RANDOM_STEP;
    return rand() / static_cast<float>(RAND_MAX);
}
template<>
inline float Random::number(float max)
{
    return number<float>() * max;
}
template<>
inline float Random::number(float min, float max)
{
    return min + number<float>() * (max - min);
}

template<>
inline float Random::static_number(uint seed)
{
    srand(seed);
    return rand() / static_cast<float>(RAND_MAX);
}
template<>
inline float Random::static_number(uint seed, float max)
{
    return static_number<float>(seed) * max;
}
template<>
inline float Random::static_number(uint seed, float min, float max)
{
    return min + static_number<float>(seed) * (max - min);
}