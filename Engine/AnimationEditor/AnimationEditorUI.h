#pragma once

#include "Engine/AnimationHandle.h"
#include "Engine/SystemIO.h"
#include "Engine/ui/UIElement.h"

class ButtonImagePanel;
class Animated;
class TextInputField;
class Texture;
class CurveSegmentWidget;
class TextBlock;
class AnimatorComponent;
class ButtonImage;
class Animation;
class Button;
class ButtonTextPanel;
class Panel;

class AnimationEditorUI : public UIElement
{
    struct EditedCurveData
    {
        Shared<ButtonImagePanel> remove_button;
        Shared<ButtonTextPanel> name_button;
        List<Shared<Button>> point_buttons;
        List<Shared<CurveSegmentWidget>> segment_widgets;
        float min_point_value = 0;
        float max_point_value = 0;
    };
    
public:
    void on_construct() override;

    void on_size_changed() override;
    void on_parent_size_changed() override;

    Shared<AnimatorComponent> target;
    Shared<Animated> animated;

    inline static const FileExtension animation_extension = {"*.anim", "Animation file"};

private:
    void update_geometry();

    void update_animation_display();

    void open_pressed(const Weak<Button>& sender);
    void save_pressed(const Weak<Button>& sender);

    void play_reversed_pressed(const Weak<Button>& sender);
    void play_pressed(const Weak<Button>& sender);
    void pause_pressed(const Weak<Button>& sender);
    void stop_pressed(const Weak<Button>& sender);

    void add_curve(uint id);

    void point_selected(const Weak<Button>& sender);
    void segment_selected(const Weak<CurveSegmentWidget>& sender);

    void edit_point(uint curve_id, uint point_id);
    void edit_segment(uint curve_id, uint segment_id);

    void add_linear_segment(const Weak<Button>& sender);
    void add_sinusoid_segment(const Weak<Button>& sender);

    void remove_point_clicked(const Weak<Button>& sender);

    bool update_min_max(uint curve_id);
    void segment_added(uint curve_id, uint id);

    void use_linear_segment(const Weak<Button>& sender);
    void use_sinusoid_segment(const Weak<Button>& sender);

    void update_point_position(uint curve_id, uint id);
    void redraw_segment(uint curve_id, uint id);

    void point_time_submitted(const String& input);
    void point_value_submitted(const String& input);

    void curve_name_clicked(const Weak<Button>& sender);
    void submit_change_curve(const String& new_name);

    void add_curve_clicked(const Weak<Button>& sender);
    void submit_add_curve(const String& name);
    
    Shared<Texture> point_texture_;
    Shared<Texture> point_active_texture_;

    uint curve_id = 0;
    uint element_id = 0;
    
    Shared<Panel> panel_;
    
    Shared<ButtonTextPanel> open_button_;
    Shared<ButtonTextPanel> save_button_;

    Shared<ButtonImage> play_reversed_button_;
    Shared<ButtonImage> play_button_;
    Shared<ButtonImage> pause_button_;
    Shared<ButtonImage> stop_button_;

    Shared<TextBlock> name_title_;
    
    Shared<Animation> edited_animation_;

    AnimationHandle current_handle_;

    List<EditedCurveData> curve_data_list_;
    float names_length = 100;

    Shared<Panel> play_controls_;
    Shared<Panel> point_controls_;    
    Shared<Panel> segment_controls_;

    Shared<ButtonTextPanel> add_curve_button_;

    Shared<TextBlock> point_time_input_label_;
    Shared<TextInputField> point_time_input_;
    Shared<TextBlock> point_value_input_label_;
    Shared<TextInputField> point_value_input_;

    Shared<UIElement> selected_element_;

    int curve_to_change_ = -1;
};
