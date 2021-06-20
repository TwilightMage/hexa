#pragma once

#include "ModalFrame.h"

class TextBlock;
class Button;
class ButtonTextPanel;
class Panel;

class ModalEnumFrame : public ModalFrame
{
public:
    ModalEnumFrame(const List<String>& options);

    static Shared<ModalEnumFrame> show(const List<String>& options, float z);

    Delegate<uint> on_select;
    Delegate<const String&> on_select_name;

protected:
    void on_construct() override;

private:
    void option_selected(const Weak<Button>& sender);
    void prev_clicked(const Weak<Button>& sender);
    void next_clicked(const Weak<Button>& sender);

    void page_changed();

    List<String> options_;
    uint page = 0;
    uint page_size = 8;

    Shared<Panel> panel_;
    List<Shared<ButtonTextPanel>> option_buttons_;
    Shared<ButtonTextPanel> prev_button_;
    Shared<ButtonTextPanel> next_button_;
    Shared<TextBlock> page_display_;
};
