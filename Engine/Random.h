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

    inline const static Shared<Random> global = MakeShared<Random>(1);

private:
    uint seed_ = 1;
    uint offset_ = 0;
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
