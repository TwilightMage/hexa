#pragma once

#include "Engine/ui/UIElement.h"

class VerticalList;
class TableBase;
class DatabaseViewerRow;

class EXPORT DatabaseViewer : public UIElement
{
public:
    explicit DatabaseViewer(const Shared<TableBase>& database);
    
    const Shared<TableBase>& get_database() const;

protected:
    void on_parent_size_changed() override;
    void on_construct() override;
    
    virtual List<Shared<UIElement>> construct_all_rows() = 0;

private:
    void update_geometry();
    
    Shared<TableBase> database_;
    Shared<VerticalList> records_list_;
};
