#pragma once

#include <deque>
#include <set>
#include <tuple>
#include <vector>

#include "Block.h"


struct World;
struct Player;
class Lighting;
class NPC;


// ============================================================
// StructureChange
//
// A world cell changed because of a special block ability.
// BuddyBox.cpp uses this to refresh its lighting and mesh.
// ============================================================

struct StructureChange
{
    int x;
    int y;
    int z;

    std::set<std::tuple<int, int, int>> dirtyChunks;
};


// ============================================================
// Structures
//
// Owns special behavior for blocks that do more than sit there.
//
// Current:
// - Saplings grow fixed trees.
//
// Future:
// - Crafting table UI
// - Doors
// - Furnaces
// - Other interactive structures
// ============================================================

class Structures
{
public:
    // Run every active structure block and grow queued tree cells.
    void update(
        float deltaTime,
        World& world,
        Lighting& lighting,
        const Player& player,
        std::vector<NPC>& npcs
    );

    // Gives BuddyBox.cpp every cell changed during update().
    std::vector<StructureChange> takeChanges();


private:
    // One block belonging to a tree that is growing gradually.
    struct PendingTreeBlock
    {
        int x;
        int y;
        int z;
        BlockType type;
    };

    std::deque<PendingTreeBlock> pendingTreeBlocks;

    std::vector<StructureChange> changes;


    // Start a fixed tree if every required cell is clear.
    bool beginSaplingGrowth(
        int saplingX,
        int saplingY,
        int saplingZ,
        World& world,
        Lighting& lighting,
        const Player& player
    );

    // Place or remove a block and remember its lighting changes.
    void recordChangedCell(
        World& world,
        Lighting& lighting,
        int x,
        int y,
        int z
    );
};