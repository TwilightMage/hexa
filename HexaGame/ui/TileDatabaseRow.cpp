#include "TileDatabaseRow.h"


#include "Engine/Math.h"
#include "Engine/ui/Image.h"
#include "Engine/ui/TextBlock.h"

TileDatabaseRow::TileDatabaseRow(const String& key, const Shared<Texture>& texture)
    : key_(key)
    , texture_(texture)
{
}

void TileDatabaseRow::on_construct()
{   
    auto key_text = MakeShared<TextBlock>(key_);
    key_text->set_font_size(12);
    key_text->set_position(Vector2(0, 0));
    add_child(key_text);

    /*auto texture_text = MakeShared<TextBlock>(texture_->get_name());
    texture_text->set_font_size(12);
    texture_text->set_size(Vector2(800, 1));
    texture_text->set_wrap(true);
    texture_text->set_position(Vector2(80, 0));
    add_child(texture_text);*/

    auto texture_image = MakeShared<Image>();
    texture_image->use_texture(texture_);
    texture_image->set_size(texture_->get_size() * 0.5f);
    texture_image->set_position(Vector2(80, 0));
    add_child(texture_image);

    set_size(Vector2(1, Math::max(key_text->get_size().y, texture_image->get_size().y)));
}
