#include "Panel.h"

#include "Image.h"
#include "Engine/Texture.h"

Panel::Panel(const Shared<Texture>& texture, const Margins& texture_offsets)
    : margins_(texture_offsets)
    , have_rect_(false)
    , lt_(nullptr)
    , mt_(nullptr)
    , rt_(nullptr)
    , lm_(nullptr)
    , mm_(nullptr)
    , rm_(nullptr)
    , lb_(nullptr)
    , mb_(nullptr)
    , rb_(nullptr)
    , texture_(texture)
{
}

void Panel::use_texture(const Shared<Texture>& texture)
{
    if (texture_ != texture)
    {
        const auto should_update_geometry = texture_->get_width() != texture->get_width() || texture_->get_height() != texture->get_height();
        
        texture_ = texture;
    
        update_texture();

        if (should_update_geometry)
        {
            update_geometry();
        }
    }
}

const Rect& Panel::get_rect() const
{
    return rect_;
}

void Panel::set_rect(const Rect& rect)
{
    rect_ = rect;
    have_rect_ = true;
    update_geometry();
}

void Panel::clear_rect()
{
    have_rect_ = false;
    update_geometry();
}

void Panel::on_construct()
{
    lt_ = MakeShared<Image>();
    lt_->set_mouse_detection(false);
    add_child(lt_);

    mt_ = MakeShared<Image>();
    mt_->set_mouse_detection(false);
    add_child(mt_);

    rt_ = MakeShared<Image>();
    rt_->set_mouse_detection(false);
    add_child(rt_);

    lm_ = MakeShared<Image>();
    lm_->set_mouse_detection(false);
    add_child(lm_);

    mm_ = MakeShared<Image>();
    mm_->set_mouse_detection(false);
    add_child(mm_);

    rm_ = MakeShared<Image>();
    rm_->set_mouse_detection(false);
    add_child(rm_);

    lb_ = MakeShared<Image>();
    lb_->set_mouse_detection(false);
    add_child(lb_);

    mb_ = MakeShared<Image>();
    mb_->set_mouse_detection(false);
    add_child(mb_);

    rb_ = MakeShared<Image>();
    rb_->set_mouse_detection(false);
    add_child(rb_);

    update_texture();
    update_geometry();
}

void Panel::on_size_changed()
{
    update_geometry();
}

void Panel::update_geometry() const
{
    if (is_started_construction() && texture_)
    {
        const auto rect = have_rect_ ? rect_ : Rect(0, 0, texture_->get_width(), texture_->get_height());
        const auto size = get_size();
        const auto left = static_cast<float>(margins_.left);
        const auto right = static_cast<float>(margins_.right);
        const auto top = static_cast<float>(margins_.top);
        const auto bottom = static_cast<float>(margins_.bottom);

        const auto mx = rect.w - (margins_.left + margins_.right);
        const auto my = rect.h - (margins_.top + margins_.bottom);
        
        lt_->set_position(Vector2(0.0f, 0.0f));
        lt_->set_size(Vector2(left, top));
        lt_->set_rect(Rect(rect.x, rect.y, margins_.left, margins_.top));

        mt_->set_position(Vector2(left, 0.0f));
        mt_->set_size(Vector2(size.x - (left + right), top));
        mt_->set_rect(Rect(rect.x + margins_.left, rect.y, mx, margins_.top));

        rt_->set_position(Vector2(size.x - right, 0.0f));
        rt_->set_size(Vector2(right, top));
        rt_->set_rect(Rect(rect.x + rect.w - margins_.right, rect.y, margins_.right, margins_.top));

        lm_->set_position(Vector2(0.0f, top));
        lm_->set_size(Vector2(left, size.y - (top + bottom)));
        lm_->set_rect(Rect(rect.x, rect.y + margins_.top, margins_.left, my));

        mm_->set_position(Vector2(left, top));
        mm_->set_size(Vector2(size.x - (left + right), size.y - (top + bottom)));
        mm_->set_rect(Rect(rect.x + margins_.left, rect.y + margins_.top, mx, my));

        rm_->set_position(Vector2(size.x - right, top));
        rm_->set_size(Vector2(right, size.y - (top + bottom)));
        rm_->set_rect(Rect(rect.x + rect.w - margins_.right, rect.y + margins_.top, margins_.right, my));

        lb_->set_position(Vector2(0.0f, size.y - bottom));
        lb_->set_size(Vector2(left, bottom));
        lb_->set_rect(Rect(rect.x, rect.y + rect.h - margins_.bottom, margins_.left, margins_.bottom));

        mb_->set_position(Vector2(left, size.y - bottom));
        mb_->set_size(Vector2(size.x - (left + right), bottom));
        mb_->set_rect(Rect(rect.x + margins_.left, rect.y + rect.h - margins_.bottom, mx, margins_.bottom));

        rb_->set_position(Vector2(size.x - right, size.y - bottom));
        rb_->set_size(Vector2(right, bottom));
        rb_->set_rect(Rect(rect.x + rect.w - margins_.right, rect.y + rect.h - margins_.bottom, margins_.right, margins_.bottom));
    }
}

void Panel::update_texture() const
{
    if (lt_) lt_->use_texture(texture_);
    if (mt_) mt_->use_texture(texture_);
    if (rt_) rt_->use_texture(texture_);
    if (lm_) lm_->use_texture(texture_);
    if (mm_) mm_->use_texture(texture_);
    if (rm_) rm_->use_texture(texture_);
    if (lb_) lb_->use_texture(texture_);
    if (mb_) mb_->use_texture(texture_);
    if (rb_) rb_->use_texture(texture_);
}
