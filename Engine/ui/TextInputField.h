#pragma once

#include "UIInputElement.h"
#include "UIElement.h"
#include "Engine/Delegate.h"

class Image;
class TextBlock;
class Texture;
class Panel;

class TextInputField : public UIInputElement
{
public:
    TextInputField();
    TextInputField(uint max_length);
    
    enum State
    {
        STATE_UNFOCUSED,
        STATE_FOCUSED
    };
    
    void on_focus() override;
    void on_unfocus() override;
    void key_down(int key) override;
    void key_hold(int key) override;
    void text_input(uint code) override;

    FORCEINLINE const String& get_text() const { return text_; }
    void set_text(const String& text);

    Function<bool, String&, char> input_verifier;
    Delegate<const String&> on_content_change;
    Delegate<const String&> on_finish_edit;

    FORCEINLINE State get_state() const { return state_; }

    static bool IntegerInputVerifier(String& new_input, char new_char);
    static bool FloatInputVerifier(String& new_input, char new_char);

protected:
    void on_press() override;
    void on_size_changed() override;
    void on_construct() override;

    virtual void state_updated(State state);

private:
    void update_caret_position();
    
    Shared<Panel> panel_;
    Shared<Texture> texture_;
    Shared<TextBlock> text_ui_;
    Shared<Image> caret_;
    State state_;
    uint caret_position_ = 0;
    String text_;
    uint max_length_;
};
