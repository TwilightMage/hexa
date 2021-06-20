#pragma once

#include "Assert.h"
#include "Curve.h"

class AnimationEditorUI;
class AnimatorComponent;

struct NamedCurve
{
    String name;
    Shared<Curve<float>> curve = MakeShared<Curve<float>>();
};

class EXPORT Animation
{
    friend AnimatorComponent;
    friend AnimationEditorUI;

public:    
    Animation(List<NamedCurve> named_curves);

    void change_curve_name(uint id, const String& new_name);

    void create_curve(const String& name, float value);

    void save_to_file(const Path& path) const;
    static Shared<Animation> load_from_file(const Path& path);
    
    float evaluate_curve(uint index, float time) const;
    float evaluate_curve_from(uint index, float time, uint& point_from) const;
    float evaluate_curve_from_backwards(uint index, float time, uint& point_from) const;

    float get_length() const;

private:
    List<NamedCurve> named_curves_;
};
