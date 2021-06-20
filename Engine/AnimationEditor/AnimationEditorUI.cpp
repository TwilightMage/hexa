#include "AnimationEditorUI.h"

#include "CurveSegmentWidget.h"
#include "Engine/Animation.h"
#include "Engine/AnimatorComponent.h"
#include "Engine/Paths.h"
#include "Engine/SpriteFont.h"
#include "Engine/Texture.h"
#include "Engine/ui/ButtonImage.h"
#include "Engine/ui/ButtonImagePanel.h"
#include "Engine/ui/ButtonTextPanel.h"
#include "Engine/ui/ModalEnumFrame.h"
#include "Engine/ui/Panel.h"
#include "Engine/ui/TextBlock.h"
#include "Engine/ui/TextInputField.h"

void AnimationEditorUI::on_construct()
{
    point_texture_ = Texture::load_png(RESOURCES_ENGINE_TEXTURES_UI_ANIMATION_EDITOR + "point.png");
    point_active_texture_ = Texture::load_png(RESOURCES_ENGINE_TEXTURES_UI_ANIMATION_EDITOR + "point_active.png");
    
    panel_ = MakeShared<Panel>();
    add_child(panel_);

    open_button_ = MakeShared<ButtonTextPanel>();
    open_button_->set_position(Vector2(5, 5));
    open_button_->set_size(Vector2(40, 15));
    open_button_->on_click.bind(this, &AnimationEditorUI::open_pressed);
    panel_->add_child(open_button_);
    open_button_->set_text("Open");

    save_button_ = MakeShared<ButtonTextPanel>();
    save_button_->set_position(Vector2(50, 5));
    save_button_->set_size(Vector2(40, 15));
    save_button_->on_click.bind(this, &AnimationEditorUI::save_pressed);
    save_button_->set_text("Save");

    play_controls_ = MakeShared<Panel>();
    play_controls_->set_size(Vector2(90, 30));

    play_reversed_button_ = MakeShared<ButtonImage>(Texture::load_png(RESOURCES_ENGINE_TEXTURES_UI_ANIMATION_EDITOR + "play_reversed.png"));
    play_reversed_button_->set_size(Vector2(20, 20));
    play_reversed_button_->set_position(Vector2(5, 5));
    play_reversed_button_->on_click.bind(this, &AnimationEditorUI::play_reversed_pressed);
    play_controls_->add_child(play_reversed_button_);


    play_button_ = MakeShared<ButtonImage>(Texture::load_png(RESOURCES_ENGINE_TEXTURES_UI_ANIMATION_EDITOR + "play.png"));
    play_button_->set_size(Vector2(20, 20));
    play_button_->set_position(Vector2(25, 5));
    play_button_->on_click.bind(this, &AnimationEditorUI::play_pressed);
    play_controls_->add_child(play_button_);

    pause_button_ = MakeShared<ButtonImage>(Texture::load_png(RESOURCES_ENGINE_TEXTURES_UI_ANIMATION_EDITOR + "pause.png"));
    pause_button_->set_size(Vector2(20, 20));
    pause_button_->set_position(Vector2(45, 5));
    pause_button_->on_click.bind(this, &AnimationEditorUI::pause_pressed);
    play_controls_->add_child(pause_button_);

    stop_button_ = MakeShared<ButtonImage>(Texture::load_png(RESOURCES_ENGINE_TEXTURES_UI_ANIMATION_EDITOR + "stop.png"));
    stop_button_->set_size(Vector2(20, 20));
    stop_button_->set_position(Vector2(65, 5));
    stop_button_->on_click.bind(this, &AnimationEditorUI::stop_pressed);
    play_controls_->add_child(stop_button_);

    name_title_ = MakeShared<TextBlock>();
    name_title_->set_position(Vector2(110, 5));
    panel_->add_child(name_title_);

    add_curve_button_ = MakeShared<ButtonTextPanel>("Add curve...");
    add_curve_button_->set_size(Vector2(16 + names_length, 15));
    add_curve_button_->set_position(Vector2(5, 30));
    add_curve_button_->on_click.bind(this, &AnimationEditorUI::add_curve_clicked);

    point_controls_ = MakeShared<Panel>();
    point_controls_->set_size(Vector2(150, 150 - play_controls_->get_size().y - 15));
    
    Shared<ButtonImagePanel> insert_linear_segment_ = MakeShared<ButtonImagePanel>();
    insert_linear_segment_->set_position(Vector2(5, 5));
    insert_linear_segment_->set_size(Vector2(35, 25));
    insert_linear_segment_->on_click.bind(this, &AnimationEditorUI::add_linear_segment);
    insert_linear_segment_->set_image_texture(Texture::load_png(RESOURCES_ENGINE_TEXTURES_UI_ANIMATION_EDITOR + "add_linear.png"));
    point_controls_->add_child(insert_linear_segment_);

    Shared<ButtonImagePanel> insert_sinusoid_segment_ = MakeShared<ButtonImagePanel>();
    insert_sinusoid_segment_->set_position(Vector2(45, 5));
    insert_sinusoid_segment_->set_size(Vector2(35, 25));
    insert_sinusoid_segment_->on_click.bind(this, &AnimationEditorUI::add_sinusoid_segment);
    insert_sinusoid_segment_->set_image_texture(Texture::load_png(RESOURCES_ENGINE_TEXTURES_UI_ANIMATION_EDITOR + "add_sinusoid.png"));
    point_controls_->add_child(insert_sinusoid_segment_);

    Shared<ButtonImagePanel> remove_point_ = MakeShared<ButtonImagePanel>();
    remove_point_->set_size(Vector2(25, 25));
    remove_point_->set_position(Vector2(150 - remove_point_->get_size().x - 5, 5));
    remove_point_->on_click.bind(this, &AnimationEditorUI::remove_point_clicked);
    remove_point_->set_image_texture(Texture::load_png(RESOURCES_ENGINE_TEXTURES_UI_ANIMATION_EDITOR + "remove.png"));
    point_controls_->add_child(remove_point_);

    point_time_input_label_ = MakeShared<TextBlock>("Time");
    point_time_input_label_->set_position(Vector2(5, 55));
    
    point_time_input_ = MakeShared<TextInputField>();
    point_time_input_->set_size(Vector2(110, 15));
    point_time_input_->set_position(Vector2(35, 55));
    point_time_input_->input_verifier.Bind(&TextInputField::FloatInputVerifier);
    point_time_input_->on_finish_edit.bind(this, &AnimationEditorUI::point_time_submitted);

    point_value_input_label_ = MakeShared<TextBlock>("Value");
    point_value_input_label_->set_position(Vector2(5, 35));
    point_controls_->add_child(point_value_input_label_);

    point_value_input_ = MakeShared<TextInputField>();
    point_value_input_->set_size(Vector2(110, 15));
    point_value_input_->set_position(Vector2(35, 35));
    point_value_input_->input_verifier.Bind(&TextInputField::FloatInputVerifier);
    point_value_input_->on_finish_edit.bind(this, &AnimationEditorUI::point_value_submitted);
    point_controls_->add_child(point_value_input_);

    segment_controls_ = MakeShared<Panel>();
    segment_controls_->set_size(Vector2(150, 150 - play_controls_->get_size().y - 15));

    Shared<ButtonImagePanel> use_linear_segment_ = MakeShared<ButtonImagePanel>();
    use_linear_segment_->set_position(Vector2(5, 5));
    use_linear_segment_->set_size(Vector2(35, 25));
    use_linear_segment_->on_click.bind(this, &AnimationEditorUI::use_linear_segment);
    use_linear_segment_->set_image_texture(Texture::load_png(RESOURCES_ENGINE_TEXTURES_UI_ANIMATION_EDITOR + "linear.png"));
    segment_controls_->add_child(use_linear_segment_);

    Shared<ButtonImagePanel> use_sinusoid_segment_ = MakeShared<ButtonImagePanel>();
    use_sinusoid_segment_->set_position(Vector2(45, 5));
    use_sinusoid_segment_->set_size(Vector2(35, 25));
    use_sinusoid_segment_->on_click.bind(this, &AnimationEditorUI::use_sinusoid_segment);
    use_sinusoid_segment_->set_image_texture(Texture::load_png(RESOURCES_ENGINE_TEXTURES_UI_ANIMATION_EDITOR + "sinusoid.png"));
    segment_controls_->add_child(use_sinusoid_segment_);
}

void AnimationEditorUI::on_size_changed()
{
    update_geometry();
}

void AnimationEditorUI::on_parent_size_changed()
{
    if (auto parent = get_parent())
    {
        set_size(Vector2(450, parent->get_size().y));
        set_position(Vector2(parent->get_size().x - get_size().x, 0));

        point_controls_->set_position(Vector2(get_size().x - point_controls_->get_size().x - 5, 10 + play_controls_->get_size().y));
        segment_controls_->set_position(Vector2(get_size().x - segment_controls_->get_size().x - 5, 10 + play_controls_->get_size().y));
    }
}

void AnimationEditorUI::update_geometry()
{
    if (is_started_construction())
    {
        panel_->set_size(get_size());

        play_controls_->set_position(Vector2(get_size().x - play_controls_->get_size().x - 5, 5));
    }
}

const float curve_height = 40;
const float time_scale = 40;
const float curve_start = 55;
void AnimationEditorUI::update_animation_display()
{
    if (edited_animation_)
    {
        if (!save_button_->is_in_hierarchy())
        {
            panel_->add_child(save_button_);
        }

        if (!play_controls_->is_in_hierarchy())
        {
            panel_->add_child(play_controls_);
        }

        if (!add_curve_button_->is_in_hierarchy())
        {
            panel_->add_child(add_curve_button_);
        }
        
        /*for (uint i = 0; i < edited_animation_->named_curves_.length(); i++)
        {
            names_length = Math::max(names_length, Game::get_default_font()->measure_string(edited_animation_->named_curves_[i].name) / Game::get_ui_scale() + 10);
        }*/

        for (auto& curve_data : curve_data_list_)
        {
            curve_data.remove_button->remove_from_parent();
            curve_data.name_button->remove_from_parent();

            for (auto& point_button : curve_data.point_buttons)
            {
                point_button->remove_from_parent();
            }

            for (auto& segment_widget : curve_data.segment_widgets)
            {
                segment_widget->remove_from_parent();
            }
        }

        curve_data_list_.Clear();

        for (uint i = 0; i < edited_animation_->named_curves_.length(); i++)
        {
            add_curve(i);
        }
    }
}

void AnimationEditorUI::open_pressed(const Weak<Button>& sender)
{
    FileDialogInfo dialog_info;
    dialog_info.extensions = { animation_extension };
    if (const auto path = SystemIO::show_open_file_dialog(dialog_info))
    {
        edited_animation_ = Animation::load_from_file(*path);
        name_title_->set_text(path->filename);
        update_animation_display();
    }
}

void AnimationEditorUI::save_pressed(const Weak<Button>& sender)
{
    if (edited_animation_)
    {
        FileDialogInfo dialog_info;
        dialog_info.extensions = { animation_extension };
        if (const auto path = SystemIO::show_save_file_dialog(dialog_info))
        {
            edited_animation_->save_to_file(*path);
            name_title_->set_text(path->filename);
        }
    }
}

void AnimationEditorUI::play_reversed_pressed(const Weak<Button>& sender)
{
    if (target && edited_animation_)
    {
        current_handle_.stop();
        current_handle_ = target->play_animation(edited_animation_, 0, 1, true);
    }
}

void AnimationEditorUI::play_pressed(const Weak<Button>& sender)
{
    if (target && edited_animation_)
    {
        current_handle_.stop();
        current_handle_ = target->play_animation(edited_animation_, 0, 1, false);
    }
}

void AnimationEditorUI::pause_pressed(const Weak<Button>& sender)
{
    if (target)
    {
        
    }
}

void AnimationEditorUI::stop_pressed(const Weak<Button>& sender)
{
    current_handle_.stop();
}

void AnimationEditorUI::add_curve(uint id)
{
    curve_data_list_.Add(EditedCurveData());
            
    auto& curve_data = curve_data_list_.last();
    auto& curve = edited_animation_->named_curves_[id].curve;
    auto& points = curve->get_points();
    auto& segments = curve->get_segments();

    curve_data.remove_button = MakeShared<ButtonImagePanel>(Texture::load_png(RESOURCES_ENGINE_TEXTURES_UI_ANIMATION_EDITOR + "remove_small.png"));
    curve_data.remove_button->set_size(Vector2(15, 15));
    curve_data.remove_button->set_position(Vector2(5, curve_start + (curve_height + 5) * id + (curve_height - curve_data.remove_button->get_size().y) / 2));
    panel_->add_child(curve_data.remove_button);
    
    curve_data.name_button = MakeShared<ButtonTextPanel>(edited_animation_->named_curves_[id].name);
    curve_data.name_button->set_size(Vector2(names_length, 15));
    curve_data.name_button->set_position(Vector2(5 + 15 + 1, curve_start + (curve_height + 5) * id + (curve_height - curve_data.name_button->get_size().y) / 2));
    curve_data.name_button->on_click.bind(this, &AnimationEditorUI::curve_name_clicked);
    panel_->add_child(curve_data.name_button);
    
    for (auto& point : points)
    {
        curve_data.min_point_value = Math::min(curve_data.min_point_value, point.value);
        curve_data.max_point_value = Math::max(curve_data.max_point_value, point.value);
    }

    for (uint j = 0; j < segments.length(); j++)
    {
        auto curve_segment = MakeShared<CurveSegmentWidget>();
        curve_segment->curve = curve;
        curve_segment->curve_name = edited_animation_->named_curves_[id].name;
        curve_segment->segment_id = j;
        curve_segment->p1 = &points[j];
        curve_segment->p2 = &points[j + 1];
        curve_segment->offset = -curve_data.min_point_value / (curve_data.max_point_value - curve_data.min_point_value);
        curve_segment->scale = 1 / (curve_data.max_point_value - curve_data.min_point_value);
        curve_segment->set_position(Vector2(5 + 15 + 1 + names_length + 5 + points[j].time * time_scale, curve_start + (curve_height + 5) * id));
        curve_segment->set_size(Vector2((points[j + 1].time - points[j].time) * time_scale, static_cast<float>(curve_height)));
        curve_segment->set_z(0.001f);
        curve_segment->on_click.bind(this, &AnimationEditorUI::segment_selected);
        panel_->add_child(curve_segment);

        curve_data.segment_widgets.Add(curve_segment);
    }
    
    for (uint j = 0; j < points.length(); j++)
    {
        auto curve_point = MakeShared<ButtonImage>(Texture::load_png(RESOURCES_ENGINE_TEXTURES_UI_ANIMATION_EDITOR + "point.png"));
        curve_point->set_size(Vector2(5, 5));
        curve_point->set_z(0.003f);
        curve_point->on_click.bind(this, &AnimationEditorUI::point_selected);
        panel_->add_child(curve_point);

        curve_data.point_buttons.Add(curve_point);
        update_point_position(id, curve_data.point_buttons.length() - 1);
    }
}

void AnimationEditorUI::point_selected(const Weak<Button>& sender)
{
    if (auto sender_lock = sender.lock())
    {
        for (uint i = 0; i < curve_data_list_.length(); i++)
        {
            auto j = curve_data_list_[i].point_buttons.IndexOf(sender_lock);
            if (j >= 0)
            {
                edit_point(i, j);
                return;
            }
        }
    }
}

void AnimationEditorUI::segment_selected(const Weak<CurveSegmentWidget>& sender)
{
    if (auto sender_lock = sender.lock())
    {
        for (uint i = 0; i < curve_data_list_.length(); i++)
        {
            auto j = curve_data_list_[i].segment_widgets.IndexOf(sender_lock);
            if (j >= 0)
            {
                edit_segment(i, j);
                return;
            }
        }
    }
}

void AnimationEditorUI::edit_point(uint curve_id, uint point_id)
{
    this->curve_id = curve_id;
    element_id = point_id;

    if (auto selected_segment = cast<CurveSegmentWidget>(selected_element_))
    {
        selected_segment->set_texture(selected_segment->get_texture());
    }
    else if (auto selected_point = cast<ButtonImage>(selected_element_))
    {
        selected_point->set_texture(point_texture_);
    }

    selected_element_ = curve_data_list_[curve_id].point_buttons[point_id];
    if (auto selected_point = cast<ButtonImage>(selected_element_))
    {
        selected_point->set_texture(point_active_texture_);
    }

    segment_controls_->remove_from_parent();
    panel_->add_child(point_controls_);

    if (point_id > 0)
    {
        point_controls_->add_child(point_time_input_label_);
        point_controls_->add_child(point_time_input_);
    }
    else
    {
        point_time_input_label_->remove_from_parent();
        point_time_input_->remove_from_parent();
    }

    point_time_input_->set_text(String::make(edited_animation_->named_curves_[curve_id].curve->get_points()[point_id].time));
    point_value_input_->set_text(String::make(edited_animation_->named_curves_[curve_id].curve->get_points()[point_id].value));
}

void AnimationEditorUI::edit_segment(uint curve_id, uint segment_id)
{
    this->curve_id = curve_id;
    element_id = segment_id;

    if (auto selected_segment = cast<CurveSegmentWidget>(selected_element_))
    {
        selected_segment->set_texture(selected_segment->get_texture());
    }
    else if (auto selected_point = cast<ButtonImage>(selected_element_))
    {
        selected_point->set_texture(point_texture_);
    }

    selected_element_ = curve_data_list_[curve_id].segment_widgets[segment_id];
    if (auto selected_segment = cast<CurveSegmentWidget>(selected_element_))
    {
        selected_segment->set_texture(selected_segment->get_texture_active());
    }
    
    point_controls_->remove_from_parent();
    panel_->add_child(segment_controls_);
}

void AnimationEditorUI::add_linear_segment(const Weak<Button>& sender)
{
    edited_animation_->named_curves_[curve_id].curve->insert_linear_segment(element_id + 1, 1, edited_animation_->named_curves_[curve_id].curve->get_points()[element_id].value);

    segment_added(curve_id, element_id + 1);
}

void AnimationEditorUI::add_sinusoid_segment(const Weak<Button>& sender)
{
    edited_animation_->named_curves_[curve_id].curve->insert_sinusoid_segment(element_id + 1, 1, edited_animation_->named_curves_[curve_id].curve->get_points()[element_id].value);

    segment_added(curve_id, element_id + 1);
}

void AnimationEditorUI::remove_point_clicked(const Weak<Button>& sender)
{    
    auto& curve_data = curve_data_list_[curve_id];
    auto& point_buttons = curve_data.point_buttons;
    auto& segment_widgets = curve_data.segment_widgets;
    auto& points = edited_animation_->named_curves_[curve_id].curve->get_points();
    auto& segments = edited_animation_->named_curves_[curve_id].curve->get_segments();

    float old_value = points[element_id].value;
        
    if (points.length() <= 1) return;
    
    edited_animation_->named_curves_[curve_id].curve->remove_point(element_id);

    point_buttons[element_id]->remove_from_parent();
    point_buttons.RemoveAt(element_id);

    const uint segment_id = element_id > 0 ? element_id - 1 : 0;
    const float offset = segment_widgets[segment_id]->get_size().x;
    segment_widgets[segment_id]->remove_from_parent();
    segment_widgets.RemoveAt(segment_id);

    const bool all_updated = update_min_max(curve_id);

    if (element_id == 0)
    {
        for (uint i = 0; i < segment_widgets.length(); i++)
        {
            auto pos = segment_widgets[i]->get_position();
            segment_widgets[i]->set_position(Vector2(pos.x - offset, pos.y));
        }

        if (!all_updated)
        {
            for (uint i = 0; i < point_buttons.length(); i++)
            {
                update_point_position(curve_id, i);
            }
        }
    }
    else if (segment_id < segment_widgets.length())
    {
        auto size = segment_widgets[segment_id]->get_size();
        segment_widgets[segment_id]->p1 = &points[segment_id];
        segment_widgets[segment_id]->p2 = &points[segment_id + 1];
        segment_widgets[segment_id]->segment_id = segment_id;
        segment_widgets[segment_id]->set_size(Vector2(size.x + offset, size.y));

        auto pos = segment_widgets[segment_id]->get_position();
        segment_widgets[segment_id]->set_position(Vector2(pos.x - offset, pos.y));
    }

    point_controls_->remove_from_parent();
}

bool AnimationEditorUI::update_min_max(uint curve_id)
{
    auto& points = edited_animation_->named_curves_[curve_id].curve->get_points();
    auto& curve_data = curve_data_list_[curve_id];

    float new_min = Math::min(0.0f, points.first().value);
    float new_max = points.first().value;

    for (auto& point : points)
    {
        new_min = Math::min(new_min, point.value);
        new_max = Math::max(new_max, point.value);
    }

    if (new_min != curve_data.min_point_value || new_max != curve_data.max_point_value)
    {
        curve_data.min_point_value = new_min;
        curve_data.max_point_value = new_max;
        
        auto& point_buttons = curve_data.point_buttons;
        auto& segment_widgets = curve_data.segment_widgets;
        
        for (uint i = 0; i < point_buttons.length(); i++)
        {
            update_point_position(curve_id, i);
        }

        for (uint i = 0; i < segment_widgets.length(); i++)
        {
            redraw_segment(curve_id, i);
        }
        
        return true;
    }

    return false;
}

void AnimationEditorUI::segment_added(uint curve_id, uint id)
{
    auto& curve_data = curve_data_list_[curve_id];
    auto& curve = edited_animation_->named_curves_[curve_id].curve;
    auto& points = curve->get_points();
    auto& segments = curve->get_segments();

    const float offset = (points[id].time - points[id - 1].time) * time_scale;
    
    auto curve_segment = MakeShared<CurveSegmentWidget>();
    curve_segment->curve = curve;
    curve_segment->curve_name = edited_animation_->named_curves_[curve_id].name;
    curve_segment->segment_id = id - 1;
    curve_segment->p1 = &points[id - 1];
    curve_segment->p2 = &points[id];
    curve_segment->offset = -curve_data.min_point_value / (curve_data.max_point_value - curve_data.min_point_value);
    curve_segment->scale = 1 / (curve_data.max_point_value - curve_data.min_point_value);
    curve_segment->set_position(Vector2(5 + 15 + 1 + names_length + 5 + points[id - 1].time * time_scale, curve_start + (curve_height + 5) * curve_id));
    curve_segment->set_size(Vector2(offset, static_cast<float>(curve_height)));
    curve_segment->set_z(0.001f);
    curve_segment->on_click.bind(this, &AnimationEditorUI::segment_selected);
    panel_->add_child(curve_segment);

    curve_data.segment_widgets.Insert(curve_segment, id - 1);

    update_min_max(curve_id);

    for (uint i = id; i < curve_data.segment_widgets.length(); i++)
    {
        curve_data.segment_widgets[i]->set_position(curve_data.segment_widgets[i]->get_position() + Vector2(offset, 0));
        curve_data.segment_widgets[i]->p1 = &points[i];
        curve_data.segment_widgets[i]->p2 = &points[i + 1];
        curve_data.segment_widgets[i]->segment_id = i;
    }

    if (id < curve_data.segment_widgets.length())
    {
        curve_data.segment_widgets[id]->update_texture();
    }

    auto curve_point = MakeShared<ButtonImage>(point_texture_);
    curve_point->set_size(Vector2(5, 5));
    curve_point->set_z(0.003f);
    curve_point->on_click.bind(this, &AnimationEditorUI::point_selected);
    panel_->add_child(curve_point);

    curve_data_list_[curve_id].point_buttons.Insert(curve_point, id);
    update_point_position(curve_id, id);
    
    for (uint i = id + 1; i < curve_data.point_buttons.length(); i++)
    {
        curve_data.point_buttons[i]->set_position(curve_data.point_buttons[i]->get_position() + Vector2(offset, 0));
    }
}

void AnimationEditorUI::use_linear_segment(const Weak<Button>& sender)
{
    edited_animation_->named_curves_[curve_id].curve->use_linear_segment(element_id);

    redraw_segment(curve_id, element_id);
}

void AnimationEditorUI::use_sinusoid_segment(const Weak<Button>& sender)
{
    edited_animation_->named_curves_[curve_id].curve->use_sinusoid_segment(element_id);

    redraw_segment(curve_id, element_id);
}

void AnimationEditorUI::update_point_position(uint curve_id, uint id)
{
    auto& points = edited_animation_->named_curves_[curve_id].curve->get_points();
    auto& curve_data = curve_data_list_[curve_id];
    
    float relative_y;
    if (curve_data.max_point_value == curve_data.min_point_value)
    {
        relative_y = Math::clamp(points[id].value, 0.0f, 1.0f);
    }
    else
    {
        relative_y = (points[id].value - curve_data.min_point_value) / (curve_data.max_point_value - curve_data.min_point_value);
    }
    
    curve_data.point_buttons[id]->set_position(Vector2(5 + 15 + 1 + names_length + 5 + points[id].time * time_scale - 2, curve_start + (curve_height + 5) * curve_id + (curve_height - relative_y * curve_height) - 2));
}

void AnimationEditorUI::redraw_segment(uint curve_id, uint id)
{
    auto& points = edited_animation_->named_curves_[curve_id].curve->get_points();
    auto& curve_data = curve_data_list_[curve_id];
    
    curve_data_list_[curve_id].segment_widgets[id]->curve = edited_animation_->named_curves_[curve_id].curve;
    curve_data_list_[curve_id].segment_widgets[id]->curve_name = edited_animation_->named_curves_[curve_id].name;
    curve_data_list_[curve_id].segment_widgets[id]->segment_id = id;
    curve_data_list_[curve_id].segment_widgets[id]->p1 = &points[id];
    curve_data_list_[curve_id].segment_widgets[id]->p2 = &points[id + 1];
    curve_data_list_[curve_id].segment_widgets[id]->offset = -curve_data.min_point_value / (curve_data.max_point_value - curve_data.min_point_value);
    curve_data_list_[curve_id].segment_widgets[id]->scale = 1 / (curve_data.max_point_value - curve_data.min_point_value);
    curve_data_list_[curve_id].segment_widgets[id]->update_texture();
}

void AnimationEditorUI::point_time_submitted(const String& input)
{
    auto& points = edited_animation_->named_curves_[curve_id].curve->get_points();
    
    edited_animation_->named_curves_[curve_id].curve->set_point_time(element_id, String::parse<float>(input));
    point_time_input_->set_text(String::make(points[element_id].time));

    auto& segments = curve_data_list_[curve_id].segment_widgets;
    float new_segment_width = (points[element_id].time - points[element_id - 1].time) * time_scale;
    float delta = new_segment_width - segments[element_id - 1]->get_size().x;
    
    if (element_id > 0)
    {
        segments[element_id - 1]->set_size(Vector2(new_segment_width, curve_height));
        redraw_segment(curve_id, element_id - 1);
    }

    for (uint i = element_id; i < segments.length(); i++)
    {
        auto pos = segments[i]->get_position();
        segments[i]->set_position(Vector2(pos.x + delta, pos.y));

        update_point_position(curve_id, i + 1);   
    }

    update_point_position(curve_id, element_id);
}

void AnimationEditorUI::point_value_submitted(const String& input)
{
    auto& points = edited_animation_->named_curves_[curve_id].curve->get_points();
    
    edited_animation_->named_curves_[curve_id].curve->set_point_value(element_id, String::parse<float>(input));
    point_value_input_->set_text(String::make(points[element_id].value));

    if (!update_min_max(curve_id))
    {
        if (element_id > 0)
        {
            redraw_segment(curve_id, element_id - 1);
        }

        if (element_id < curve_data_list_[curve_id].segment_widgets.length())
        {
            redraw_segment(curve_id, element_id);
        }

        update_point_position(curve_id, element_id);
    }
}

void AnimationEditorUI::curve_name_clicked(const Weak<Button>& sender)
{
    if (auto sender_lock = sender.lock())
    {
        curve_to_change_ = curve_data_list_.IndexOf([&sender_lock](const EditedCurveData& item) -> bool{ return item.name_button == sender_lock; });
        if (curve_to_change_ >= 0)
        {
            auto modal = ModalEnumFrame::show(animated->get_animated_fields().get_keys() - curve_data_list_.select<String>([](const EditedCurveData& item) -> String{ return item.name_button->get_text(); }), 100);
            modal->on_select_name.bind(this, &AnimationEditorUI::submit_change_curve);
        }
    }
}

void AnimationEditorUI::submit_change_curve(const String& new_name)
{
    edited_animation_->change_curve_name(curve_to_change_, new_name);
    curve_data_list_[curve_to_change_].name_button->set_text(new_name);
}

void AnimationEditorUI::add_curve_clicked(const Weak<Button>& sender)
{
    auto modal = ModalEnumFrame::show(animated->get_animated_fields().get_keys() - curve_data_list_.select<String>([](const EditedCurveData& item) -> String{ return item.name_button->get_text(); }), 100);
    modal->on_select_name.bind(this, &AnimationEditorUI::submit_add_curve);
}

void AnimationEditorUI::submit_add_curve(const String& name)
{
    if (const auto field = cast<AnimatedField<float>>(animated->find_animated_field(name)))
    {
        edited_animation_->create_curve(name, field->get_default_value());
        add_curve(edited_animation_->named_curves_.length() - 1);
    }
}
