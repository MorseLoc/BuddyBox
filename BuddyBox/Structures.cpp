#include "Structures.h"

#include <utility>

#include "lighting.h"
#include "NPC.h"
#include "player.h"
#include "world.h"


// ============================================================
// Remember a changed cell and its lighting consequences
// ============================================================

void Structures::recordChangedCell(
    World& world,
    Lighting& lighting,
    int x,
    int y,
    int z
)
{
    StructureChange change;

    change.x = x;
    change.y = y;
    change.z = z;

    // World must already contain the new block state.
    change.dirtyChunks = lighting.updateBlockChange(
        world,
        x,
        y,
        z
    );

    changes.push_back(std::move(change));
}


// ============================================================
// Start growing one fixed tree
// ============================================================

bool Structures::beginSaplingGrowth(
    int saplingX,
    int saplingY,
    int saplingZ,
    World& world,
    Lighting& lighting,
    const Player& player
)
{
    std::vector<PendingTreeBlock> tree;

    // --------------------------------------------------------
    // Four-block wood trunk
    // --------------------------------------------------------

    for (int offsetY = 0; offsetY < 4; ++offsetY)
    {
        tree.push_back(
            {
                saplingX,
                saplingY + offsetY,
                saplingZ,
                BlockType::Wood
            }
        );
    }

    // --------------------------------------------------------
    // Leaf ring around the top trunk block
    // --------------------------------------------------------

    for (int offsetX = -1; offsetX <= 1; ++offsetX)
    {
        for (int offsetZ = -1; offsetZ <= 1; ++offsetZ)
        {
            if (offsetX == 0 && offsetZ == 0)
            {
                continue;
            }

            tree.push_back(
                {
                    saplingX + offsetX,
                    saplingY + 3,
                    saplingZ + offsetZ,
                    BlockType::Leaf
                }
            );
        }
    }

    // --------------------------------------------------------
    // Main 3 x 3 leaf layer
    // --------------------------------------------------------

    for (int offsetX = -1; offsetX <= 1; ++offsetX)
    {
        for (int offsetZ = -1; offsetZ <= 1; ++offsetZ)
        {
            tree.push_back(
                {
                    saplingX + offsetX,
                    saplingY + 4,
                    saplingZ + offsetZ,
                    BlockType::Leaf
                }
            );
        }
    }

    // --------------------------------------------------------
    // Small plus-shaped top
    // --------------------------------------------------------

    const int topOffsets[5][2] =
    {
        { 0, 0 },
        { 1, 0 },
        {-1, 0 },
        { 0, 1 },
        { 0,-1 }
    };

    for (const auto& offset : topOffsets)
    {
        tree.push_back(
            {
                saplingX + offset[0],
                saplingY + 5,
                saplingZ + offset[1],
                BlockType::Leaf
            }
        );
    }

    // --------------------------------------------------------
    // Do not grow through existing blocks or the player
    // --------------------------------------------------------

    const glm::vec3 playerMin =
        player.position - player.size * 0.5f;

    const glm::vec3 playerMax =
        player.position + player.size * 0.5f;

    for (const PendingTreeBlock& part : tree)
    {
        const bool isSaplingCell =
            part.x == saplingX &&
            part.y == saplingY &&
            part.z == saplingZ;

        // The tree may replace its own sapling only.
        if (
            !isSaplingCell &&
            world.hasBlock(part.x, part.y, part.z)
            )
        {
            return false;
        }

        const glm::vec3 blockMin(
            static_cast<float>(part.x) - 0.5f,
            static_cast<float>(part.y) - 0.5f,
            static_cast<float>(part.z) - 0.5f
        );

        const glm::vec3 blockMax(
            static_cast<float>(part.x) + 0.5f,
            static_cast<float>(part.y) + 0.5f,
            static_cast<float>(part.z) + 0.5f
        );

        const bool overlapsPlayer =
            playerMax.x > blockMin.x &&
            playerMin.x < blockMax.x &&
            playerMax.y > blockMin.y &&
            playerMin.y < blockMax.y &&
            playerMax.z > blockMin.z &&
            playerMin.z < blockMax.z;

        if (overlapsPlayer)
        {
            return false;
        }
    }

    // Remove the mature sapling. The tree itself appears
    // gradually through pendingTreeBlocks.
    world.removeBlock(
        saplingX,
        saplingY,
        saplingZ
    );

    recordChangedCell(
        world,
        lighting,
        saplingX,
        saplingY,
        saplingZ
    );

    for (const PendingTreeBlock& part : tree)
    {
        pendingTreeBlocks.push_back(part);
    }

    return true;
}


// ============================================================
// Update all structure blocks
// ============================================================

void Structures::update(
    float deltaTime,
    World& world,
    Lighting& lighting,
    const Player& player,
    std::vector<NPC>& npcs
)
{
    std::vector<std::tuple<int, int, int>> readySaplings;

    // Do not edit world.activeBlocks while looping through it.
    for (const auto& position : world.activeBlocks)
    {
        auto blockIterator = world.blocks.find(position);

        if (blockIterator == world.blocks.end())
        {
            continue;
        }

        Block& block = blockIterator->second;

        const int x = std::get<0>(position);
        const int y = std::get<1>(position);
        const int z = std::get<2>(position);

        if (
            block.update(
                deltaTime,
                glm::vec3(
                    static_cast<float>(x),
                    static_cast<float>(y),
                    static_cast<float>(z)
                ),
                npcs
            )
            )
        {
            readySaplings.push_back(position);
        }
    }

    // Now it is safe to remove mature saplings.
    for (const auto& position : readySaplings)
    {
        beginSaplingGrowth(
            std::get<0>(position),
            std::get<1>(position),
            std::get<2>(position),
            world,
            lighting,
            player
        );
    }

    // Grow a couple of tree blocks each frame.
    // This avoids a lighting/mesh hitch when a tree matures.
    const int TREE_BLOCKS_PER_FRAME = 2;

    for (
        int placed = 0;
        placed < TREE_BLOCKS_PER_FRAME &&
        !pendingTreeBlocks.empty();
        ++placed
        )
    {
        const PendingTreeBlock part =
            pendingTreeBlocks.front();

        pendingTreeBlocks.pop_front();

        world.placeBlock(
            part.x,
            part.y,
            part.z,
            Block(part.type)
        );

        recordChangedCell(
            world,
            lighting,
            part.x,
            part.y,
            part.z
        );
    }
}


// ============================================================
// Give changed cells back to BuddyBox.cpp
// ============================================================

std::vector<StructureChange> Structures::takeChanges()
{
    std::vector<StructureChange> result =
        std::move(changes);

    changes.clear();

    return result;
}