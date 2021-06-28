#include "CurveSegmentWidget.h"

#include "Engine/Array2D.h"
#include "Engine/Color.h"
#include "Engine/stb.h"
#include "Engine/Texture.h"
#include "Engine/ui/Image.h"

void CurveSegmentWidget::on_construct()
{
    ButtonImage::on_construct();
    
    texture_ = MakeShared<Texture>("Curve texture");
    texture_active_ = MakeShared<Texture>("Curve active texture");
    set_texture(texture_);

    set_stretch_image(true);
}

void CurveSegmentWidget::on_size_changed()
{
    ButtonImage::on_size_changed();
    
    update_geometry();
}

void put_white_pixel(Array2D<Color>& pixels, Array2D<Color>& pixels_active, uint x, uint y)
{
    for (uint s = 0; s < 3; s++)
    {
        pixels.at(x, pixels.get_size_y() / 3 * s + y) = Color::white();
        pixels_active.at(x, pixels.get_size_y() / 3 * s + y) = Color::white();
    }
}

void CurveSegmentWidget::update_texture()
{
    if (texture_ && texture_active_)
    {
        const uint size_y = static_cast<uint>(get_size().y * Game::get_ui_scale());
        Array2D<Color> pixels(static_cast<uint>(get_size().x * Game::get_ui_scale()), size_y * 3);
        Array2D<Color> pixels_active(static_cast<uint>(get_size().x * Game::get_ui_scale()), size_y * 3);
        for (uint x = 0; x < pixels.get_size_x(); x++)
        {
            for (uint y = 0; y < pixels.get_size_y(); y++)
            {
                uint yy = y % size_y;
                const uint ys = y / size_y;

                if (yy == 0 || yy == (pixels.get_size_y() / 3) - 1)
                {
                    pixels.at(x, y) = Color(0, 0, 0, 255);
                    pixels_active.at(x, y) = Color(0, 0, 0, 255);
                }
                else
                {
                    byte c = 110;
                    if (ys == 1)
                    {
                        c = 105;
                    }
                    else if (ys == 2)
                    {
                        c = 115;
                    }
    
                    pixels.at(x, y) = Color(c, c, c, 255);
                    pixels_active.at(x, y) = Color(c + 30, c + 30, 0, 255);
                }
            }
        }
        
        uint y;
        for (uint x = 0; x < pixels.get_size_x(); x++)
        {
            uint yt = (pixels.get_size_y() / 3) - static_cast<uint>((curve->get_segments()[segment_id].interpolate(p1->value, p2->value, x / static_cast<float>(pixels.get_size_x())) * scale + offset) * (pixels.get_size_y() / 3 - 1)) - 1;
            if (x == 0)
            {
                y = yt;
            }

            if (Math::abs(static_cast<int>(yt) - static_cast<int>(y)) <= 1)
            {
                put_white_pixel(pixels, pixels_active, x, yt);
            }
            else
            {
                const uint mid = (yt + y) / 2;
                const int step = Math::sign(static_cast<int>(yt) - static_cast<int>(y));
                uint yy = y;
                bool mid_flag = false;
                while (true)
                {
                    if (yy == mid) mid_flag = true;
                    
                    for (uint s = 0; s < 3; s++)
                    {
                        pixels.at(!mid_flag ? x - 1 : x, pixels.get_size_y() / 3 * s + yy) = Color::white();
                        pixels_active.at(!mid_flag ? x - 1 : x, pixels.get_size_y() / 3 * s + yy) = Color::white();
                    }

                    if (yy == yt) break;
                    
                    yy += step;
                }
            }

            y = yt;
        }
    
        texture_->put_pixels(pixels);
        texture_active_->put_pixels(pixels_active);
        state_updated(get_state());

        texture_->name = String::format("Curve \"%s\" texture %g->%g, %g-%g", curve_name.c(), p1->value, p2->value, p1->time, p2->time);
        texture_active_->name = String::format("Curve \"%s\" active texture %g->%g, %g-%g", curve_name.c(), p1->value, p2->value, p1->time, p2->time);
    }
}

void CurveSegmentWidget::on_press(const Vector2& point)
{
    ButtonImage::on_press(point);
    
    on_click(cast<CurveSegmentWidget>(shared_from_this()));
}

void CurveSegmentWidget::update_geometry()
{
    if (is_started_construction())
    {
        update_texture();
    }
}
