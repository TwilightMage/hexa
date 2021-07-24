#pragma once
#include "PathConfig.h"
#include "Engine/Pointers.h"
#include "Engine/SimpleMap.h"

class NavNode;
    
class NavConnection
{
public:
    NavConnection(const Shared<NavNode>& node_a, const Shared<NavNode>& node_b);
    
    virtual const NavNode* pass(const NavNode* from, const PathConfig& path_config) const = 0;
    virtual float cost(const NavNode* from, const PathConfig& path_config) const = 0;
    virtual void destroy();

    FORCEINLINE const NavNode* get_opposite(const NavNode* from) const { return from == a.get() ? b.get() : a.get(); }

    Shared<NavNode> a = nullptr;
    Shared<NavNode> b = nullptr;
};    

class NavNode
{
public:
    TileIndex tile_index;
    SimpleMap<TileIndex, NavConnection*> connected_nodes;
};

class NavStepConnection : public NavConnection
{
public:
    NavStepConnection(const Shared<NavNode>& node_a, const Shared<NavNode>& node_b);

    FORCEINLINE const NavNode* pass(const NavNode* from, const PathConfig& path_config) const override { return a.get() == from ? b.get() : a.get(); }
    FORCEINLINE float cost(const NavNode* from, const PathConfig& path_config) const override { return 1.0f; }
};

class NavClimbFallConnection : public NavConnection
{
public:
    NavClimbFallConnection(const Shared<NavNode>& node_a, const Shared<NavNode>& node_b, uint bottom_offset, uint top_lowpass);

    const NavNode* pass(const NavNode* from, const PathConfig& path_config) const override;
    float cost(const NavNode* from, const PathConfig& path_config) const override;

    uint bottom_offset;
    uint top_lowpass;
};

class NavFallConnection : public NavConnection
{
public:
    NavFallConnection(const Shared<NavNode>& node_a, const Shared<NavNode>& node_b, uint top_lowpass);

    const NavNode* pass(const NavNode* from, const PathConfig& path_config) const override;
    float cost(const NavNode* from, const PathConfig& path_config) const override;

    uint top_lowpass;
};