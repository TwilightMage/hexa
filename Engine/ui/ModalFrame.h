#pragma once

#include "UIElement.h"
#include "Engine/Delegate.h"

class Image;

class ModalFrame : public UIElement
{
public:
    explicit ModalFrame(const Shared<UIElement>& content);

    static Shared<ModalFrame> show(const Shared<UIElement>& content, float z);

    void close();

    FORCEINLINE const Shared<UIElement>& get_content() const { return content_; }

    Delegate<> on_closed;

protected:
    void on_press() override;
    void on_size_changed() override;
    void on_parent_size_changed() override;

    void on_construct() override;

private:
    Shared<UIElement> content_;
    Shared<Image> background_;
};
