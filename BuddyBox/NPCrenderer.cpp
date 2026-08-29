#include "NPCRenderer.h"

#include "NPC.h"
#include "Renderer.h"

#include <glm/glm.hpp>
#include <cmath>


// ============================================================
// Draw NPC
//
// Chooses the correct drawing function
// based on the NPC's type.
// ============================================================

void NPCRenderer::drawNPC(
    const NPC& npc,
    Renderer& renderer,
    unsigned int npcAtlasTexture
)
{
    if (npc.getType() == NPCType::Jebub)
    {
        drawJebub(
            npc,
            renderer,
            npcAtlasTexture
        );
    }
}


// ============================================================
// Draw Jebub
//
// Builds a Jebub from:
// - One textured cube body
// - Two rectangular yellow legs
//
// The legs rotate around the body and
// swing back and forth while walking.
// ============================================================

void NPCRenderer::drawJebub(
    const NPC& npc,
    Renderer& renderer,
    unsigned int npcAtlasTexture
)
{
    glm::vec3 jebubPosition =
        npc.getPosition();


    float jebubYaw =
        npc.getFacingYaw();


    float yawRadians =
        glm::radians(
            jebubYaw
        );


    // --------------------------------------------------------
    // Walking animation
    // --------------------------------------------------------

    float legSwing =
        0.0f;


    if (npc.isMoving())
    {
        legSwing =
            std::sin(
                npc.getWalkAnimationTime() * 8.0f
            ) * 0.12f;
    }


    // Match the yellow used in Jebub's face texture.
    glm::vec3 jebubColor(
        1.0f,
        0.847f,
        0.0f
    );


    // --------------------------------------------------------
    // Body
    // --------------------------------------------------------

    renderer.drawTexturedCube(
        jebubPosition +
        glm::vec3(
            0.0f,
            0.25f,
            0.0f
        ),

        glm::vec3(
            0.8f,
            0.8f,
            0.8f
        ),

        npcAtlasTexture,

        0,  // Row 0 = Jebub body

        1,  // NPCdex currently has 1 rows

        jebubYaw
    );


    // --------------------------------------------------------
    // Left leg
    // --------------------------------------------------------

    renderer.drawColoredCube(
        jebubPosition +
        glm::vec3(
            -0.20f * std::cos(yawRadians)
            + legSwing * std::sin(yawRadians),

            -0.40f,

            0.20f * std::sin(yawRadians)
            + legSwing * std::cos(yawRadians)
        ),

        glm::vec3(
            0.22f,
            0.50f,
            0.22f
        ),

        jebubColor,
        jebubYaw
    );


    // --------------------------------------------------------
    // Right leg
    // --------------------------------------------------------

    renderer.drawColoredCube(
        jebubPosition +
        glm::vec3(
            0.20f * std::cos(yawRadians)
            - legSwing * std::sin(yawRadians),

            -0.40f,

            -0.20f * std::sin(yawRadians)
            - legSwing * std::cos(yawRadians)
        ),

        glm::vec3(
            0.22f,
            0.50f,
            0.22f
        ),

        jebubColor,
        jebubYaw
    );
}