#include "DatabaseViewer.h"

#include "Engine/ui/VerticalList.h"

DatabaseViewer::DatabaseViewer(const Shared<TableBase>& database)
    : database_(database)
{
}

const Shared<TableBase>& DatabaseViewer::get_database() const
{
    return database_;
}

void DatabaseViewer::on_parent_size_changed()
{
    update_geometry();
}

void DatabaseViewer::on_construct()
{
    set_position(Vector2(10, 10));

    records_list_ = MakeShared<VerticalList>();
    records_list_->set_spacing(5);
    add_child(records_list_);

    for (const auto& row : construct_all_rows())
    {
        records_list_->add_child(row);
    }
}

void DatabaseViewer::update_geometry()
{
    if (const auto parent = get_parent())
    {
        set_size(parent->get_size() = Vector2(20, 20));

        records_list_->set_size(get_size());
    }
}
