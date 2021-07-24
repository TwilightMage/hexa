#include "Nav.h"

NavConnection::NavConnection(const Shared<NavNode>& node_a, const Shared<NavNode>& node_b)
    : a(node_a)
    , b(node_b)
{
}

void NavConnection::destroy()
{
    a->connected_nodes.remove(b->tile_index);
    b->connected_nodes.remove(a->tile_index);
    delete this;
}

NavStepConnection::NavStepConnection(const Shared<NavNode>& node_a, const Shared<NavNode>& node_b)
    : NavConnection(node_a, node_b)
{
}

const NavNode* NavClimbFallConnection::pass(const NavNode* from, const PathConfig& path_config) const
{
    if (path_config.agent_height / 2 > top_lowpass) return nullptr;
    
    const auto to = a.get() == from ? b.get() : a.get();
    if (from->tile_index.z < to->tile_index.z) // up
    {
        if (path_config.agent_height / 2 < bottom_offset) return nullptr;
    }
    
    return to;
}

float NavClimbFallConnection::cost(const NavNode* from, const PathConfig& path_config) const
{
    const auto to = a.get() == from ? b.get() : a.get();

    if (from->tile_index.z < to->tile_index.z) return (to->tile_index.z - from->tile_index.z) * 1.5f;
    return (from->tile_index.z - to->tile_index.z) * 1.25f;
}

NavClimbFallConnection::NavClimbFallConnection(const Shared<NavNode>& node_a, const Shared<NavNode>& node_b, uint bottom_offset, uint top_lowpass)
    : NavConnection(node_a, node_b)
    , bottom_offset(bottom_offset)
    , top_lowpass(top_lowpass)
{
}

NavFallConnection::NavFallConnection(const Shared<NavNode>& node_a, const Shared<NavNode>& node_b, uint top_lowpass)
    : NavConnection(node_a, node_b)
    , top_lowpass(top_lowpass)
{
}

const NavNode* NavFallConnection::pass(const NavNode* from, const PathConfig& path_config) const
{
    if (path_config.agent_height / 2 > top_lowpass) return nullptr;

    return a.get() == from ? b.get() : nullptr;
}

float NavFallConnection::cost(const NavNode* from, const PathConfig& path_config) const
{
    return (a->tile_index.z - b->tile_index.z) * 1.25f;
}
