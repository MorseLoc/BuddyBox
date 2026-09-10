#pragma once

#include <array>
#include <random>
#include <vector>

#include "Item.h"

class TextureManager;

class HandAnimator
{
public:
    HandAnimator();

    HandAnimator(const HandAnimator&) = delete;
    HandAnimator& operator=(const HandAnimator&) = delete;

    void load(TextureManager& textures);
    void cleanup();

    void update(
        float deltaTime,
        ItemType selectedItem,
        bool mining,
        bool attacked,
        bool placed,
        bool active,
        bool enabled
    );

    unsigned int getTexture() const;
    ItemType getDisplayedItem() const;

private:
    enum class Animation
    {
        Idle,
        Holding,
        Breaking,
        Placing,
        Attacking,
        Secret01,
        Secret02,
        Count
    };

    struct Clip
    {
        std::vector<unsigned int> frames;
        float fps = 12.0f;
    };

    std::array<Clip, static_cast<int>(Animation::Count)> clips;

    Animation current = Animation::Idle;
    ItemType heldItem = ItemType::None;

    float animationTime = 0.0f;
    float idleTime = 0.0f;
    float nextSecretTime = 30.0f;

    bool visible = false;

    std::mt19937 random;

    const Clip& getClip(Animation animation) const;
    bool hasFrames(Animation animation) const;
    bool isSecret() const;
    bool isOneShot() const;

    Animation restingAnimation() const;

    void start(Animation animation, bool restart = false);
    void resetIdleTimer();

    void loadClip(
        TextureManager& textures,
        Animation animation,
        const char* prefix,
        int frameCount,
        float fps
    );
};