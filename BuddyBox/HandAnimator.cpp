#include "HandAnimator.h"
#include "textureManager.h"

#include <glad/glad.h>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

HandAnimator::HandAnimator()
    : random(std::random_device{}())
{
    resetIdleTimer();
}

const HandAnimator::Clip& HandAnimator::getClip(
    Animation animation
) const
{
    return clips[static_cast<int>(animation)];
}

bool HandAnimator::hasFrames(Animation animation) const
{
    return !getClip(animation).frames.empty();
}

bool HandAnimator::isSecret() const
{
    return current == Animation::Secret01 ||
        current == Animation::Secret02;
}

bool HandAnimator::isOneShot() const
{
    return current == Animation::Placing ||
        current == Animation::Attacking ||
        isSecret();
}

HandAnimator::Animation HandAnimator::restingAnimation() const
{
    if (
        heldItem != ItemType::None &&
        hasFrames(Animation::Holding)
        )
    {
        return Animation::Holding;
    }

    return Animation::Idle;
}

void HandAnimator::start(Animation animation, bool restart)
{
    if (restart || current != animation)
    {
        current = animation;
        animationTime = 0.0f;
    }
}

void HandAnimator::resetIdleTimer()
{
    idleTime = 0.0f;

    std::uniform_real_distribution<float> wait(20.0f, 45.0f);
    nextSecretTime = wait(random);
}

void HandAnimator::loadClip(
    TextureManager& textures,
    Animation animation,
    const char* prefix,
    int frameCount,
    float fps
)
{
    // Zero means this recording is not ready yet.
    if (frameCount <= 0)
    {
        return;
    }

    Clip& clip = clips[static_cast<int>(animation)];
    clip.fps = std::max(1.0f, fps);

    for (int frame = 0; frame < frameCount; ++frame)
    {
        std::ostringstream filename;

        filename
            << "textures/hand/"
            << prefix
            << "_"
            << std::setw(2)
            << std::setfill('0')
            << frame
            << ".png";

        unsigned int texture = textures.loadTexture(filename.str());

        if (!texture)
        {
            std::cout
                << "Hand animation disabled: missing "
                << filename.str()
                << "\n";

            // Do not play an incomplete recording.
            for (unsigned int loaded : clip.frames)
            {
                glDeleteTextures(1, &loaded);
            }

            clip.frames.clear();
            return;
        }

        clip.frames.push_back(texture);
    }
}

void HandAnimator::load(TextureManager& textures)
{
    cleanup();

    const int IDLE_FRAME_COUNT = 46;      // BLANK
    const int HOLD_FRAME_COUNT = 46;      // BLANK
    const int BREAK_FRAME_COUNT = 20;     // BLANK
    const int PLACE_FRAME_COUNT = 20;     // BLANK
    const int ATTACK_FRAME_COUNT = 20;    // BLANK
    const int SECRET_01_FRAME_COUNT = 54; // BLANK
    const int SECRET_02_FRAME_COUNT = 96; // BLANK

    // Match these rates to the PNG export rate later.
    loadClip(textures, Animation::Idle,
        "idle", IDLE_FRAME_COUNT, 12.0f);

    loadClip(textures, Animation::Holding,
        "hold", HOLD_FRAME_COUNT, 24.0f);

    loadClip(textures, Animation::Breaking,
        "break", BREAK_FRAME_COUNT, 12.0f);

    loadClip(textures, Animation::Placing,
        "place", PLACE_FRAME_COUNT, 36.0f);
   
    loadClip(textures, Animation::Attacking,
        "attack", ATTACK_FRAME_COUNT, 48.0f);


    loadClip(textures, Animation::Secret01,
        "secret_01", SECRET_01_FRAME_COUNT, 24.0f);

    loadClip(textures, Animation::Secret02,
        "secret_02", SECRET_02_FRAME_COUNT, 24.0f);
}

void HandAnimator::update(
    float deltaTime,
    ItemType selectedItem,
    bool mining,
    bool attacked,
    bool placed,
    bool active,
    bool enabled
)
{
    bool selectionChanged = selectedItem != heldItem;
    heldItem = selectedItem;
    visible = enabled;

    if (!enabled)
    {
        start(restingAnimation(), true);
        resetIdleTimer();
        return;
    }

    float dt = std::max(0.0f, deltaTime);

    animationTime += dt;

    // Finish attacks, placements, and secret animations.
    if (isOneShot())
    {
        const Clip& clip = getClip(current);

        float duration =
            static_cast<float>(clip.frames.size()) / clip.fps;

        if (animationTime >= duration)
        {
            start(restingAnimation(), true);
        }
    }

    bool busy =
        active || mining || attacked || placed || selectionChanged;

    if (busy)
    {
        resetIdleTimer();

        // Any activity immediately interrupts a secret idle.
        if (isSecret())
        {
            start(restingAnimation(), true);
        }
    }

    // Actual gameplay actions have priority.
    if (attacked)
    {
        start(
            hasFrames(Animation::Attacking)
            ? Animation::Attacking
            : restingAnimation(),
            true
        );
    }
    else if (placed)
    {
        start(
            hasFrames(Animation::Placing)
            ? Animation::Placing
            : restingAnimation(),
            true
        );
    }
    else if (!isOneShot())
    {
        if (mining && hasFrames(Animation::Breaking))
        {
            start(Animation::Breaking);
        }
        else
        {
            start(restingAnimation());
        }
    }

    // Secret idles only run when the player has been inactive.
    if (!busy && !mining && !isOneShot())
    {
        idleTime += dt;

        if (idleTime >= nextSecretTime)
        {
            std::vector<Animation> available;

            if (hasFrames(Animation::Secret01))
                available.push_back(Animation::Secret01);

            if (hasFrames(Animation::Secret02))
                available.push_back(Animation::Secret02);

            resetIdleTimer();

            if (!available.empty())
            {
                std::uniform_int_distribution<int> choose(
                    0, static_cast<int>(available.size()) - 1
                );

                start(available[choose(random)], true);
            }
        }
    }
}

unsigned int HandAnimator::getTexture() const
{
    if (!visible)
    {
        return 0;
    }

    const Clip& clip = getClip(current);

    if (clip.frames.empty())
    {
        return 0;
    }

    double frameNumber = std::floor(
        static_cast<double>(animationTime) * clip.fps
    );

    std::size_t frame;

    if (isOneShot())
    {
        frame = static_cast<std::size_t>(std::min(
            frameNumber,
            static_cast<double>(clip.frames.size() - 1)
        ));
    }
    else
    {
        frame = static_cast<std::size_t>(std::fmod(
            frameNumber,
            static_cast<double>(clip.frames.size())
        ));
    }

    return clip.frames[frame];
}

ItemType HandAnimator::getDisplayedItem() const
{
    // Show the item only on the holding recording.
    // Action/secret recordings can move the hand elsewhere.
    if (visible && current == Animation::Holding)
    {
        return heldItem;
    }

    return ItemType::None;
}

void HandAnimator::cleanup()
{
    for (Clip& clip : clips)
    {
        for (unsigned int texture : clip.frames)
        {
            glDeleteTextures(1, &texture);
        }

        clip.frames.clear();
    }

    visible = false;
    current = Animation::Idle;
    heldItem = ItemType::None;
    animationTime = 0.0f;

    resetIdleTimer();
}