#pragma once


// Forward declarations.
//
// NPCRenderer only needs references to these types here.
class Renderer;
class NPC;


// ============================================================
// NPCRenderer
//
// Handles the visual appearance of BuddyBox NPCs.
//
// NPC.cpp:
//     Controls NPC behavior and physics.
//
// Renderer.cpp:
//     Provides low-level drawing tools.
//
// NPCRenderer.cpp:
//     Decides how each NPC type is built visually.
// ============================================================

class NPCRenderer
{
public:
    // Draws the correct model
    // based on the NPC's NPCType.
    void drawNPC(
        const NPC& npc,
        Renderer& renderer,
        unsigned int npcAtlasTexture
    );


private:
    // Draws a Jebub.
    void drawJebub(
        const NPC& npc,
        Renderer& renderer,
        unsigned int npcAtlasTexture
    );
};