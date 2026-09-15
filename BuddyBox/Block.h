#pragma once

#include <glm/glm.hpp>
#include <vector>

class NPC;

// ItemType is defined in Item.h.
enum class ItemType;


// ============================================================
// Block types
// ============================================================

enum class BlockType
{
    Grass,
    Spawner,
    Dirt,
    Wood,
    Leaf,
    Stone,
    Wood2,
    Wood3,
    Wood4,
    YellowFlower,
    RedFlower,
    BlueFlower,
    Bulb,
    Sapling
};


// ============================================================
// Block
// ============================================================

struct Block
{
    // --------------------------------------------------------
    // Basic block properties
    // --------------------------------------------------------

    BlockType type;

    glm::vec3 size;

    bool solid;

    float durability;

    ItemType dropItem;

    int emittedLight;

    // True for blocks drawn as intersecting transparent planes
    // instead of a normal cube.
    bool crossedSprite;


    // --------------------------------------------------------
    // Spawner properties
    // --------------------------------------------------------

    bool spawnsJebub;

    float jebubSpawnTimer;


    // --------------------------------------------------------
    // Sapling properties
    // --------------------------------------------------------

    bool growsTree;

    float treeGrowthTimer;


    // --------------------------------------------------------
    // Texture
    // --------------------------------------------------------

    int textureRow;


    // --------------------------------------------------------
    // Block behavior
    // --------------------------------------------------------

    // Returns true when a sapling has reached its growth time.
    bool update(
        float deltaTime,
        const glm::vec3& position,
        std::vector<NPC>& npcs
    );


    // --------------------------------------------------------
    // Constructor
    // --------------------------------------------------------

    Block(BlockType type);

    // True when this block needs an update every frame.
    bool needsUpdate() const;
};