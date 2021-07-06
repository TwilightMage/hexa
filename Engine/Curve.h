#pragma once

#include "List.h"
#include "Math.h"

template<typename T>
class Curve
{
public:
    struct Interpolator
    {
        virtual T interpolate(T from, T to, float alpha) const = 0;
        virtual Interpolator* copy() const = 0;
    };

    struct LinearInterpolator : Interpolator
    {
        T interpolate(T from, T to, float alpha) const override
        {
            return Math::lerp(from, to, alpha);
        }

        Interpolator* copy() const override
        {
            return new LinearInterpolator();
        }
    };

    struct SinusoidInterpolator : Interpolator
    {
        T interpolate(T from, T to, float alpha) const override
        {
            return Math::lerp(from, to, Math::cos_rad(alpha * Math::pi<float>()) * -0.5f + 0.5f);
        }

        Interpolator* copy() const override
        {
            return new SinusoidInterpolator();
        }
    };

    struct Segment
    {
        Segment()
            : interpolator(nullptr)
        {
        }
        
        Segment(Interpolator* interpolator)
            : interpolator(interpolator)
        {
        }

        Segment(const Segment& rhs)
            : interpolator(rhs.interpolator->copy())
        {
        }

        Segment& operator=(const Segment& rhs)
        {
            if (this == &rhs) return *this;

            interpolator.reset(rhs.interpolator.get() ? rhs.interpolator->copy() : nullptr);

            return *this;
        }

        T interpolate(T from, T to, float alpha) const
        {
            return interpolator->interpolate(from, to, alpha);
        }

        FORCEINLINE Interpolator* get_interpolator() const
        {
            return interpolator.get();
        }
    
    private:
        Unique<Interpolator> interpolator;
    };

    struct Point
    {
        float time;
        T value;
    };

    Curve()
        : Curve(T())
    {}

    Curve(T start_value)
        : points_({ Point(0, start_value) })
        , segments_({})
    {}

    Curve(const List<Point>& points, const List<Segment>& segments)
        : points_(points)
        , segments_(segments)
    {
        if (points_.length() == 0)
        {
            points_.add({ Point(0, T()) });
        }
        else if (points_.first().time != 0)
        {
            float delta = points_.first().time;
            for (auto& point : points_)
            {
                point.time -= delta;
            }
        }
    }
    

    void insert_linear_segment(uint index, float length, T to)
    {
        points_.insert({points_[index - 1].time + length, to}, index);
        segments_.insert(new LinearInterpolator(), index - 1);

        for (uint i = index + 1; i < points_.length(); i++)
        {
            points_[i].time += length;
        }
    }

    void insert_sinusoid_segment(uint index, float length, T to)
    {
        points_.insert({points_[index - 1].time + length, to},index);
        segments_.insert(new SinusoidInterpolator(), index - 1);

        for (uint i = index + 1; i < points_.length(); i++)
        {
            points_[i].time += length;
        }
    }
    
    
    void add_linear_segment(float length, T to)
    {
        points_.add({points_.last().time + length, to});
        segments_.add(new LinearInterpolator());
    }

    void add_sinusoid_segment(float length, T to)
    {
        points_.add({points_.last().time + length, to});
        segments_.add(new SinusoidInterpolator());
    }
    

    void use_linear_segment(uint segment_id)
    {
        segments_[segment_id] = Segment(new LinearInterpolator());
    }

    void use_sinusoid_segment(uint segment_id)
    {
        segments_[segment_id] = Segment(new SinusoidInterpolator());
    }


    void remove_point(uint point_id)
    {
        if (points_.length() <= 1) return;
        
        points_.remove_at(point_id);
        segments_.remove_at(point_id > 0 ? point_id - 1 : 0);

        if (point_id == 0)
        {
            float offset = points_[0].time;
            for (uint i = 0; i < points_.length(); i++)
            {
                points_[i].time -= offset;
            }
        }
    }
    

    T evaluate(float time) const
    {
        if (time < 0)
        {
            return points_.first().value;
        }
        
        for (uint i = 0; i < points_.length() - 1; i++)
        {
            if (points_[i].time <= time && points_[i + 1].time > time)
            {
                const auto& p1 = points_[i];
                const auto& p2 = points_[i + 1];
                return segments_[i].interpolate(p1.value, p2.value, (time - p1.time) / (p2.time - p1.time));
            }
        }

        return points_.last().value;
    }

    T evaluate_from(float time, uint& point_from) const
    {
        if (time < 0)
        {
            return points_.first().value;
        }
        
        for (; point_from < points_.length() - 1; point_from++)
        {
            const auto& p1 = points_[point_from];
            const auto& p2 = points_[point_from + 1];
            if (p1.time <= time && p2.time >= time)
            {
                return segments_[point_from].interpolate(p1.value, p2.value, (time - p1.time) / (p2.time - p1.time));
            }
        }

        return points_.last().value;
    }

    T evaluate_from_backwards(float time, uint& point_from)
    {
        if (time < 0)
        {
            return points_.first().value;
        }

        for (; point_from > 0; point_from--)
        {
            const auto& p1 = points_[point_from - 1];
            const auto& p2 = points_[point_from];
            if (p1.time <= time && p2.time >= time)
            {
                return segments_[point_from - 1].interpolate(p1.value, p2.value, (time - p1.time) / (p2.time - p1.time));
            }
        }

        return points_.last().value;
    }

    void set_point_time(uint point_id, float time)
    {
        float delta = time - points_[point_id].time;
        
        if (point_id == 0)
        {
            return;
        }
        else if (point_id > 0 && time - points_[point_id - 1].time < 0.0001f)
        {
            time = points_[point_id - 1].time + 0.0001f;
        }
        
        for (uint i = point_id; i < points_.length(); i++)
        {
            points_[i].time += delta;
        }
    }

    void set_point_value(uint point_id, float value)
    {
        points_[point_id].value = value;
    }

    float get_length() const
    {
        return points_.last().time;
    }

    FORCEINLINE const List<Point>& get_points() const { return points_; }
    FORCEINLINE const List<Segment>& get_segments() const { return segments_; }

private:
    List<Point> points_ = { Point(0, T()) };
    List<Segment> segments_;
};
