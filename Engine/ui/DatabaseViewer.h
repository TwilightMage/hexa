#pragma once

#include "Engine/ui/UIElement.h"

class VerticalList;
class DatabaseBase;
class DatabaseViewerRow;

EXTERN class EXPORT DatabaseViewer : public UIElement
{
public:
    explicit DatabaseViewer(const Shared<DatabaseBase>& database);
    
    const Shared<DatabaseBase>& get_database() const;

protected:
    void on_parent_size_changed() override;
    void on_construct() override;
    
    virtual List<Shared<UIElement>> construct_all_rows() = 0;

private:
    void update_geometry();
    
    Shared<DatabaseBase> database_;
    Shared<VerticalList> records_list_;
};
