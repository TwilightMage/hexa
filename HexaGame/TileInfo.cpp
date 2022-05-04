#include "TileInfo.h"

#include "HexaGame.h"
#include "Engine/Material.h"
#include "Engine/Texture.h"

void TileInfo::convert_to(Compound::Object& to) const
{
    to = metadata;

    TableRecordBase::convert_to(to);

    //if (mesh) to["mesh"] = mesh->get_id().to_string();
    to["material"] = material->get_id().to_string();
    List<Compound::Value> texture_names(material->get_textures_count());
    for (uint i = 0; i < texture_names.length(); i++)
    {
        if (const auto tex = material->get_texture(i))
        {
            texture_names[i] = tex->get_id().to_string();
        }
    }
    to["textures"] = texture_names;
    to["tags"] = tags.to_list().convert<String>().convert<Compound::Value>();
    to["hardness"] = hardness;
    to["block_nav"] = block_nav;
}

void TileInfo::convert_from(const Compound::Object& from)
{
    TableRecordBase::convert_from(from);
    
    metadata = from;

    type = TileType::Solid;
    
    mesh_name = ModuleAssetID(metadata.get_string("mesh"), id.module_name);
    
    material_name = ModuleAssetID(metadata.get_string("material"), id.module_name);
    
    texture_names = metadata.get_array("textures").convert<String>().convert<ModuleAssetID>();
    
    tags = Set(metadata.get_array("tags").convert<String>().convert<Name>().without(""));
    hardness = metadata.get_float("hardness", 1);
    block_nav = metadata.get_bool("block_nav", true);

    metadata.remove("key");
    metadata.remove("mesh");
    metadata.remove("material");
    metadata.remove("textures");
    metadata.remove("tags");
    metadata.remove("hardness");
    metadata.remove("block_nav");
}

void TileInfo::post_load()
{
    if (!material_name.is_valid()) material_name = Game::get_instance()->get_asset_id("materials/basic");

    if (texture_names.length() == 0) texture_names.add(ModuleAssetID(mesh_name.is_valid() ? "textures/complex_tiles/" : "textures/tiles/" + id.asset_name.to_string(), id.module_name));
}

void TileInfo::init_assets()
{
    if (mesh_name.is_valid()) type = TileType::Complex;
    
    if (mesh_name.is_valid()) mesh = StaticMesh::load_file_obj(mesh_name.evaluate_asset_path().with_extension(".obj"));

    if (material_name.is_valid()) material = Game::load_material(material_name);
    
    for (uint i = 0; i < texture_names.length(); i++)
    {
       if (texture_names[i].is_valid()) material->set_texture(Game::load_texture(texture_names[i]), i);
    }
}

/*TileInfo::TileInfo(
    const Name& key,
    TileType type,
    const Set<Name> tags,
    float hardness,
    bool block_nav
    )
    : DatabaseEntry(key)
    , type(type)
    , tags(tags)
    , hardness(hardness)
    , block_nav(block_nav)
{
    material = Game::get_instance()->load_material("Engine/Basic")->clone(("Hexa/Tiles/" + key.to_string()).c());
    material->set_texture(key.to_string() + ".png", 0);
}*/

void TileInfo::neighbor_changed(const TileIndex& index, TileSide side, const Shared<HexaWorld>& world, ConstPtr<TileInfo> new_neighbor) const
{
}

void TileInfo::on_tile_break(const TileIndex& index, const Shared<HexaWorld>& world) const
{
}

List<ItemContainer> TileInfo::get_drops(const TileIndex& index, const Shared<HexaWorld>& world) const
{
     return {}; //return { HexaGame::item_database->get(key) };
}

//void ComplexTileInfo::post_loading()
//{
//    if (material == nullptr)
//    {
//        material = Game::get_instance()->load_material("Engine/Basic_Instanced")->clone(key.to_string());
//        material->set_texture(key.to_string() + ".png", 0);
//    }
//}

void ComplexTileInfo::setup_spawned_entity(const Shared<ComplexTile>& new_tile, const Shared<ComplexTileCustomData>& custom_data) const
{
}

void ComplexTileInfo::on_tile_destroyed(const TileIndex& index, const Shared<ComplexTile>& destroyed_tile, const Shared<HexaWorld>& world) const
{
}

bool ComplexTileInfo::can_place_at(const Shared<ComplexTile>& new_tile, const ItemContainer& item) const
{
    return true;
}

Shared<ComplexTileCustomData> ComplexTileInfo::create_custom_data() const
{
    return nullptr;
}
