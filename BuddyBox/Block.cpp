#include "Block.h"
#include "NPC.h"
#include "Item.h"


// ============================================================
// Block constructor
// ============================================================

Block::Block(BlockType blockType)
{
    type = blockType;

    // --------------------------------------------------------
    // Default block properties
    // --------------------------------------------------------

    size = glm::vec3(
        1.0f,
        1.0f,
        1.0f
    );

    solid = true;

    durability = 1.0f;

    dropItem = ItemType::None;

    emittedLight = 0;

    crossedSprite = false;

    spawnsJebub = false;
    jebubSpawnTimer = 0.0f;

    growsTree = false;
    treeGrowthTimer = 0.0f;

    textureRow = 0;

    // --------------------------------------------------------
    // Block-specific properties
    // --------------------------------------------------------

    if (type == BlockType::Grass)
    {
        textureRow = 0;
        durability = 0.5f;
        dropItem = ItemType::GrassBlock;
    }
    else if (type == BlockType::Spawner)
    {
        textureRow = 1;
        spawnsJebub = true;
        durability = 999.0f;
    }
    else if (type == BlockType::Dirt)
    {
        textureRow = 2;
        durability = 0.5f;
        dropItem = ItemType::DirtBlock;
    }
    else if (type == BlockType::Wood)
    {
        textureRow = 3;
        durability = 2.0f;
        dropItem = ItemType::WoodBlock;
    }
    else if (type == BlockType::Leaf)
    {
        textureRow = 4;
        dropItem = ItemType::Stick;
    }
    else if (type == BlockType::Stone)
    {
        textureRow = 5;
        durability = 3.0f;
        dropItem = ItemType::Pebble;
    }
    else if (type == BlockType::Wood2)
    {
        textureRow = 6;
        durability = 2.0f;
        dropItem = ItemType::Wood2Block;
    }
    else if (type == BlockType::Wood3)
    {
        textureRow = 7;
        durability = 2.0f;
        dropItem = ItemType::Wood3Block;
    }
    else if (type == BlockType::Wood4)
    {
        textureRow = 8;
        durability = 2.0f;
        dropItem = ItemType::Wood4Block;
    }
    else if (type == BlockType::YellowFlower)
    {
        textureRow = 9;
        durability = 0.5f;
        dropItem = ItemType::GrassBlock;
    }
    else if (type == BlockType::RedFlower)
    {
        textureRow = 10;
        durability = 0.5f;
        dropItem = ItemType::GrassBlock;
    }
    else if (type == BlockType::BlueFlower)
    {
        textureRow = 11;
        durability = 0.5f;
        dropItem = ItemType::GrassBlock;
    }
    else if (type == BlockType::Bulb)
    {
        textureRow = 12;

        size = glm::vec3(0.35f);
        solid = false;

        durability = 0.2f;
        dropItem = ItemType::Bulb;

        emittedLight = 15;
    }
    else if (type == BlockType::Sapling)
    {
        textureRow = 13;

        // The mesh turns this into two intersecting planes.
        size = glm::vec3(0.8f, 1.0f, 0.8f);

        solid = false;
        crossedSprite = true;

        durability = 0.2f;
        dropItem = ItemType::Sapling;

        growsTree = true;
    }
}


// ============================================================
// Update block behavior
// ============================================================

bool Block::update(
    float deltaTime,
    const glm::vec3& position,
    std::vector<NPC>& npcs
)
{
    // --------------------------------------------------------
    // Spawner behavior
    // --------------------------------------------------------

    if (spawnsJebub)
    {
        jebubSpawnTimer += deltaTime;

        if (jebubSpawnTimer >= 60.0f)
        {
            npcs.emplace_back(
                NPCType::Jebub,
                glm::vec3(
                    position.x,
                    position.y + 1.15f,
                    position.z
                )
            );

            jebubSpawnTimer = 0.0f;
        }
    }

    // --------------------------------------------------------
    // Sapling behavior
    // --------------------------------------------------------

    if (growsTree)
    {
        treeGrowthTimer += deltaTime;

        // Once mature, keep reporting ready until the game
        // finds enough empty space to grow the tree.
        return treeGrowthTimer >= 10.0f;
    }

    return false;
}


// ============================================================
// Does this block need per-frame updates?
// ============================================================

bool Block::needsUpdate() const
{
    return spawnsJebub || growsTree;
}