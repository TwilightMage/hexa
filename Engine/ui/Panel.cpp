#include "Panel.h"

#include "Image.h"
#include "Engine/Texture.h"

Panel::Panel(const Margins& texture_offsets)
    : margins_(texture_offsets)
{
}

void Panel::on_construct()
{
    const auto lt = MakeShared<Image>();
    lt_ = lt.get();
    add_child(lt);

    const auto mt = MakeShared<Image>();
    mt_ = mt.get();
    add_child(mt);

    const auto rt = MakeShared<Image>();
    rt_ = rt.get();
    add_child(rt);

    const auto lm = MakeShared<Image>();
    lm_ = lm.get();
    add_child(lm);

    const auto mm = MakeShared<Image>();
    mm_ = mm.get();
    add_child(mm);

    const auto rm = MakeShared<Image>();
    rm_ = rm.get();
    add_child(rm);

    const auto lb = MakeShared<Image>();
    lb_ = lb.get();
    add_child(lb);

    const auto mb = MakeShared<Image>();
    mb_ = mb.get();
    add_child(mb);

    const auto rb = MakeShared<Image>();
    rb_ = rb.get();
    add_child(rb);

    update_texture(Texture::load_png("resources/hexagame/textures/ui/panel.png"));
    update_local_geometry();
}

void Panel::on_size_changed()
{
    update_local_geometry();
}

void Panel::update_local_geometry() const
{
    if (texture_)
    {
        Vector2 size = get_render_scale();
        const auto left = static_cast<float>(margins_.left);
        const auto right = static_cast<float>(margins_.right);
        const auto top = static_cast<float>(margins_.top);
        const auto bottom = static_cast<float>(margins_.bottom);

        const int mx = texture_->get_width() - (margins_.left + margins_.right);
        const int my = texture_->get_height() - (margins_.top + margins_.bottom);
        
        lt_->set_position(Vector2(0.0f, 0.0f));
        lt_->set_size(Vector2(left, top));
        lt_->set_rect(Rect(0, 0, margins_.left, margins_.top));

        mt_->set_position(Vector2(left, 0.0f));
        mt_->set_size(Vector2(size.x - (left + right), top));
        mt_->set_rect(Rect(margins_.left, 0, mx, margins_.top));

        rt_->set_position(Vector2(size.x - right, 0.0f));
        rt_->set_size(Vector2(right, top));
        rt_->set_rect(Rect(texture_->get_width() - margins_.right, 0, margins_.right, margins_.top));

        lm_->set_position(Vector2(0.0f, top));
        lm_->set_size(Vector2(left, size.y - (top + bottom)));
        lm_->set_rect(Rect(0, margins_.top, margins_.left, my));

        mm_->set_position(Vector2(left, top));
        mm_->set_size(Vector2(size.x - (left + right), size.y - (top + bottom)));
        mm_->set_rect(Rect(margins_.left, margins_.top, mx, my));

        rm_->set_position(Vector2(size.x - right, top));
        rm_->set_size(Vector2(right, size.y - (top + bottom)));
        rm_->set_rect(Rect(texture_->get_width() - margins_.right, margins_.top, margins_.right, my));

        lb_->set_position(Vector2(0.0f, size.y - bottom));
        lb_->set_size(Vector2(left, bottom));
        lb_->set_rect(Rect(0, texture_->get_height() - margins_.bottom, margins_.left, margins_.bottom));

        mb_->set_position(Vector2(left, size.y - bottom));
        mb_->set_size(Vector2(size.x - (left + right), bottom));
        mb_->set_rect(Rect(margins_.left, texture_->get_height() - margins_.bottom, mx, margins_.bottom));

        rb_->set_position(Vector2(size.x - right, size.y - bottom));
        rb_->set_size(Vector2(right, bottom));
        rb_->set_rect(Rect(texture_->get_width() - margins_.right, texture_->get_height() - margins_.bottom, margins_.right, margins_.bottom));
    }
}

void Panel::update_texture(const Shared<Texture>& texture)
{
    texture_ = texture;
    
    if (lt_) lt_->use_texture(texture);
    if (mt_) mt_->use_texture(texture);
    if (rt_) rt_->use_texture(texture);
    if (lm_) lm_->use_texture(texture);
    if (mm_) mm_->use_texture(texture);
    if (rm_) rm_->use_texture(texture);
    if (lb_) lb_->use_texture(texture);
    if (mb_) mb_->use_texture(texture);
    if (rb_) rb_->use_texture(texture);
}
