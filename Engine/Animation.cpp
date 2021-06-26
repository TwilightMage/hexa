#include "Animation.h"

#include "BinaryStream.h"
#include "Game.h"

Animation::Animation(List<NamedCurve> named_curves)
    : named_curves_(named_curves)
{
}

void Animation::change_curve_name(uint id, const String& new_name)
{
    named_curves_[id].name = new_name;
}

void Animation::create_curve(const String& name, float value)
{
    named_curves_.Add({ name, MakeShared<Curve<float>>(value) });
}

void Animation::save_to_file(const Path& path) const
{
    if (auto writer = BinaryWriter::open(path))
    {
        writer->write<uint>(named_curves_.length());
        for (auto& named_curve : named_curves_)
        {
            writer->write<String>(named_curve.name);

            writer->write<uint>(named_curve.curve->get_points().length());
            for (auto& point : named_curve.curve->get_points())
            {
                writer->write<float>(point.time);
                writer->write<float>(point.value);
            }

            writer->write<uint>(named_curve.curve->get_segments().length());
            for (auto& segment : named_curve.curve->get_segments())
            {
                if (cast<Curve<float>::LinearInterpolator>(segment.get_interpolator()))
                {
                    writer->write<byte>(1);
                }
                else if (cast<Curve<float>::SinusoidInterpolator>(segment.get_interpolator()))
                {
                    writer->write<byte>(2);
                }
                else
                {
                    writer->write<byte>(0);
                }
            }
        }
        writer->close();
    }
}

Shared<Animation> Animation::load_from_file(const Path& path)
{
    if (auto found = Game::instance_->animations_.find(path.get_absolute_string()))
    {
        return *found;
    }
    
    try
    {
        if (path.exists())
        {
            if (auto reader = BinaryReader::open(path))
            {
                List<NamedCurve> named_curves = List<NamedCurve>(reader->read<uint>());

                for (auto& named_curve : named_curves)
                {
                    named_curve.name = reader->read<String>();

                    List<Curve<float>::Point> points = List<Curve<float>::Point>(reader->read<uint>());
                    for (auto& point : points)
                    {
                        point.time = reader->read<float>();
                        point.value = reader->read<float>();
                    }

                    List<Curve<float>::Segment> segments = List<Curve<float>::Segment>(reader->read<uint>());
                    for (auto& segment : segments)
                    {
                        byte type = reader->read<byte>();
                        switch (type)
                        {
                            case 1:
                                segment = Curve<float>::Segment(new Curve<float>::LinearInterpolator());
                                break;
                            case 2:
                                segment = Curve<float>::Segment(new Curve<float>::SinusoidInterpolator());
                                break;
                            default:
                                segment = Curve<float>::Segment(nullptr);
                                break;
                        }
                    }
                    
                    named_curve.curve = MakeShared<Curve<float>>(points, segments);
                }

                auto animation = MakeShared<Animation>(named_curves);
                Game::instance_->animations_.insert(path.get_absolute_string(), animation);
                return animation;
            }
        }

        return nullptr;
    }
    catch (...)
    {
        return nullptr;
    }
}

float Animation::evaluate_curve(uint index, float time) const
{
    return named_curves_[index].curve->evaluate(time);
}

float Animation::evaluate_curve_from(uint index, float time, uint& point_from) const
{
    return named_curves_[index].curve->evaluate_from(time, point_from);
}

float Animation::evaluate_curve_from_backwards(uint index, float time, uint& point_from) const
{
    return named_curves_[index].curve->evaluate_from_backwards(time, point_from);
}

float Animation::get_length() const
{
    float result = 0;
    for (auto& named_curve : named_curves_)
    {
        result = Math::max(result, named_curve.curve->get_length());
    }

    return result;
}
