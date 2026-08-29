// ============================================================
// BuddyBox.cpp
//
// Main entry point for the game.
//
// This file is responsible for:
// - Starting GLFW and OpenGL
// - Creating the game window
// - Creating the major game systems
// - Running the main game loop
// - Handling block breaking / placing
// - Updating NPCs
// - Drawing the world, NPCs, and UI
// - Shutting the game down cleanly
// ============================================================

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


// STB_IMAGE_IMPLEMENTATION must exist in exactly one .cpp file.
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


#include "Block.h"
#include "Player.h"
#include "World.h"
#include "Renderer.h"
#include "Camera.h"
#include "textureManager.h"
#include "inventory.h"
#include "UIRenderer.h"
#include "NPC.h"
#include "NPCRenderer.h"
#include "ChunkMesh.h"

#include <memory>
#include <set>


// ============================================================
// Mouse wheel input
// ============================================================

double scrollAmount = 0.0;


void scrollCallback(
    GLFWwindow* window,
    double xOffset,
    double yOffset
)
{
    scrollAmount += yOffset;
}


// ============================================================
// Main
// ============================================================

int main()
{
    // --------------------------------------------------------
    // 1. Start GLFW
    // --------------------------------------------------------

    if (!glfwInit())
    {
        std::cout << "GLFW failed to start.\n";
        return -1;
    }


    // --------------------------------------------------------
    // 2. Create window
    // --------------------------------------------------------

    GLFWwindow* window =
        glfwCreateWindow(
            800,
            600,
            "BuddyBox",
            nullptr,
            nullptr
        );


    if (!window)
    {
        std::cout << "Window creation failed.\n";

        glfwTerminate();

        return -1;
    }


    glfwMakeContextCurrent(
        window
    );


    glfwSwapInterval(
        1
    );


    glfwSetInputMode(
        window,
        GLFW_CURSOR,
        GLFW_CURSOR_DISABLED
    );


    glfwSetScrollCallback(
        window,
        scrollCallback
    );


    // --------------------------------------------------------
    // 3. Load OpenGL
    // --------------------------------------------------------

    if (!gladLoadGLLoader(
        (GLADloadproc)glfwGetProcAddress
    ))
    {
        std::cout << "GLAD failed to start.\n";

        glfwDestroyWindow(
            window
        );

        glfwTerminate();

        return -1;
    }


    // --------------------------------------------------------
    // 4. Create major game systems
    // --------------------------------------------------------

    Renderer renderer;

    NPCRenderer npcRenderer;

    UIRenderer uiRenderer;

    TextureManager textureManager;

    Player player;

    World world;

    Camera camera;

    Inventory inventory;


    // Stores every NPC currently alive.
    std::vector<NPC> npcs;

    // Stores the GPU mesh for every 16x16x16 chunk.
    std::vector<std::unique_ptr<ChunkMesh>> chunkMeshes;


    // --------------------------------------------------------
    // 5. Initialize rendering systems
    // --------------------------------------------------------

    if (!uiRenderer.initialize())
    {
        std::cout
            << "Failed to initialize UI renderer.\n";


        glfwDestroyWindow(
            window
        );

        glfwTerminate();

        return -1;
    }


    glEnable(
        GL_DEPTH_TEST
    );


    // --------------------------------------------------------
    // 6. Load textures
    // --------------------------------------------------------

    if (!textureManager.loadAtlas(
        "textures/artdex.png"
    ))
    {
        std::cout
            << "Failed to load artdex.png\n";
    }


    unsigned int blockAtlasTexture =
        textureManager.getAtlasTexture();


    unsigned int scrollWheelTexture =
        textureManager.loadTexture(
            "textures/ScrollWheel.png"
        );

    unsigned int npcAtlasTexture =
        textureManager.loadTexture(
            "textures/NPCdex.png"
        );

    unsigned int shaderProgram =
        renderer.getShaderProgram();


    // --------------------------------------------------------
    // 7. Load inventory and world
    // --------------------------------------------------------

    if (!inventory.loadFromFile(
        "inventory.txt"
    ))
    {
        std::cout
            << "Failed to load inventory.txt\n";
    }

    if (!world.loadFromFile(
        "test.world"
    ))
    {
        std::cout
            << "Failed to load test.world\n";
    }
    else
    {
        std::cout
            << "Blocks loaded: "
            << world.blocks.size()
            << "\n";


        // ========================================================
        // Find every chunk used by the world
        // ========================================================

        std::set<
            std::tuple<int, int, int>
        > usedChunks;


        // Converts a block coordinate into a chunk coordinate.
        //
        // This also works correctly for negative coordinates.
        auto getChunkCoordinate =
            [](int blockCoordinate)
            {
                if (blockCoordinate >= 0)
                {
                    return blockCoordinate /
                        ChunkMesh::CHUNK_SIZE;
                }

                return
                    (blockCoordinate -
                        (ChunkMesh::CHUNK_SIZE - 1))
                    /
                    ChunkMesh::CHUNK_SIZE;
            };


        for (const auto& entry : world.blocks)
        {
            int blockX =
                std::get<0>(entry.first);

            int blockY =
                std::get<1>(entry.first);

            int blockZ =
                std::get<2>(entry.first);


            int chunkX =
                getChunkCoordinate(blockX);

            int chunkY =
                getChunkCoordinate(blockY);

            int chunkZ =
                getChunkCoordinate(blockZ);


            usedChunks.insert(
                std::make_tuple(
                    chunkX,
                    chunkY,
                    chunkZ
                )
            );
        }


        // ========================================================
        // Build one GPU mesh for each chunk
        // ========================================================

        for (const auto& chunkPosition : usedChunks)
        {
            int chunkX =
                std::get<0>(chunkPosition);

            int chunkY =
                std::get<1>(chunkPosition);

            int chunkZ =
                std::get<2>(chunkPosition);


            auto chunk =
                std::make_unique<ChunkMesh>();


            chunk->build(
                world,
                chunkX,
                chunkY,
                chunkZ
            );


            chunkMeshes.push_back(
                std::move(chunk)
            );
        }


        std::cout
            << "Chunk meshes built: "
            << chunkMeshes.size()
            << "\n";
    }


    // --------------------------------------------------------
    // 8. Game-loop variables
    // --------------------------------------------------------

    float deltaTime =
        0.0f;


    float lastFrame =
        0.0f;


    bool leftMouseWasPressed =
        false;


    bool rightMouseWasPressed =
        false;


    // ========================================================
    // 9. Main game loop
    // ========================================================

    while (!glfwWindowShouldClose(window))
    {
        // ----------------------------------------------------
        // Frame timing
        // ----------------------------------------------------

        float currentFrame =
            static_cast<float>(
                glfwGetTime()
                );


        deltaTime =
            currentFrame -
            lastFrame;


        lastFrame =
            currentFrame;


        if (deltaTime > 0.05f)
        {
            deltaTime =
                0.05f;
        }


        // ----------------------------------------------------
        // Camera and player update
        // ----------------------------------------------------

        camera.update(
            window
        );


        player.move(
            window,
            deltaTime,
            camera.getFront(),
            camera.getUp(),
            world
        );


        camera.updatePosition(
            player.position
        );


        // ----------------------------------------------------
// Block updates
// ----------------------------------------------------

        for (auto& entry : world.blocks)
        {
            Block& block =
                entry.second;


            int x =
                std::get<0>(
                    entry.first
                );

            int y =
                std::get<1>(
                    entry.first
                );

            int z =
                std::get<2>(
                    entry.first
                );


            block.update(
                deltaTime,
                glm::vec3(
                    static_cast<float>(x),
                    static_cast<float>(y),
                    static_cast<float>(z)
                ),
                npcs
            );
        }


        // ----------------------------------------------------
        // NPC updates
        // ----------------------------------------------------

        for (NPC& npc : npcs)
        {
            npc.update(
                deltaTime,
                world
            );
        }


        // ----------------------------------------------------
        // Mouse input
        // ----------------------------------------------------

        bool leftMousePressed =
            glfwGetMouseButton(
                window,
                GLFW_MOUSE_BUTTON_LEFT
            ) == GLFW_PRESS;


        bool rightMousePressed =
            glfwGetMouseButton(
                window,
                GLFW_MOUSE_BUTTON_RIGHT
            ) == GLFW_PRESS;


        // ----------------------------------------------------
        // Break blocks
        // ----------------------------------------------------

        if (
            leftMousePressed &&
            !leftMouseWasPressed
            )
        {
            int hitX;
            int hitY;
            int hitZ;

            int previousX;
            int previousY;
            int previousZ;


            if (world.raycastBlock(
                camera.getPosition(),
                camera.getFront(),
                5.0f,
                hitX,
                hitY,
                hitZ,
                previousX,
                previousY,
                previousZ
            ))
            {
                world.removeBlock(
                    hitX,
                    hitY,
                    hitZ
                );
            }
        }


        // ----------------------------------------------------
        // Place blocks
        // ----------------------------------------------------

        if (
            rightMousePressed &&
            !rightMouseWasPressed
            )
        {
            int hitX;
            int hitY;
            int hitZ;

            int previousX;
            int previousY;
            int previousZ;


            if (world.raycastBlock(
                camera.getPosition(),
                camera.getFront(),
                5.0f,
                hitX,
                hitY,
                hitZ,
                previousX,
                previousY,
                previousZ
            ))
            {
                Block block(
                    inventory.getSelectedBlockType()
                );


                glm::vec3 playerMin =
                    player.position -
                    (player.size / 2.0f);


                glm::vec3 playerMax =
                    player.position +
                    (player.size / 2.0f);


                glm::vec3 blockMin(
                    previousX - 0.5f,
                    previousY - 0.5f,
                    previousZ - 0.5f
                );


                glm::vec3 blockMax(
                    previousX + 0.5f,
                    previousY + 0.5f,
                    previousZ + 0.5f
                );


                bool overlapsPlayer =
                    playerMax.x > blockMin.x &&
                    playerMin.x < blockMax.x &&

                    playerMax.y > blockMin.y &&
                    playerMin.y < blockMax.y &&

                    playerMax.z > blockMin.z &&
                    playerMin.z < blockMax.z;


                if (!overlapsPlayer)
                {
                    world.placeBlock(
                        previousX,
                        previousY,
                        previousZ,
                        block
                    );
                }
            }
        }


        leftMouseWasPressed =
            leftMousePressed;


        rightMouseWasPressed =
            rightMousePressed;


        // ----------------------------------------------------
        // Camera matrices
        // ----------------------------------------------------

        glm::mat4 view =
            camera.getViewMatrix();


        int windowWidth;
        int windowHeight;


        glfwGetFramebufferSize(
            window,
            &windowWidth,
            &windowHeight
        );


        if (windowHeight == 0)
        {
            windowHeight =
                1;
        }


        glViewport(
            0,
            0,
            windowWidth,
            windowHeight
        );


        glm::mat4 projection =
            glm::perspective(
                glm::radians(
                    45.0f
                ),

                static_cast<float>(
                    windowWidth
                    )
                /
                static_cast<float>(
                    windowHeight
                    ),

                0.1f,
                100.0f
            );


        // ----------------------------------------------------
        // Clear frame
        // ----------------------------------------------------

        glClearColor(
            0.42f,
            0.75f,
            1.0f,
            1.0f
        );


        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );


        // ----------------------------------------------------
        // Prepare world rendering
        // ----------------------------------------------------

        glUseProgram(
            shaderProgram
        );


        glActiveTexture(
            GL_TEXTURE0
        );


        glBindTexture(
            GL_TEXTURE_2D,
            blockAtlasTexture
        );


        int textureLocation =
            glGetUniformLocation(
                shaderProgram,
                "blockTexture"
            );


        glUniform1i(
            textureLocation,
            0
        );


        int useSolidColorLocation =
            glGetUniformLocation(
                shaderProgram,
                "useSolidColor"
            );


        glUniform1i(
            useSolidColorLocation,
            0
        );


        int modelLocation =
            glGetUniformLocation(
                shaderProgram,
                "model"
            );


        int viewLocation =
            glGetUniformLocation(
                shaderProgram,
                "view"
            );


        int projectionLocation =
            glGetUniformLocation(
                shaderProgram,
                "projection"
            );


        int textureRowLocation =
            glGetUniformLocation(
                shaderProgram,
                "textureRow"
            );


        int atlasRowsLocation =
            glGetUniformLocation(
                shaderProgram,
                "atlasRows"
            );


        glUniformMatrix4fv(
            viewLocation,
            1,
            GL_FALSE,
            glm::value_ptr(view)
        );


        glUniformMatrix4fv(
            projectionLocation,
            1,
            GL_FALSE,
            glm::value_ptr(projection)
        );


        glUniform1f(
            atlasRowsLocation,
            static_cast<float>(
                textureManager.getBlockCount()
                )
        );


        renderer.bindCube();


        // ----------------------------------------------------
 // Draw world
 //
 // Only draws cube faces that are exposed to air.
 //
 // A face touching another solid block is invisible,
 // so there is no reason to send it to the GPU.
 // ----------------------------------------------------

        for (const auto& entry : world.blocks)
        {
            const Block& block =
                entry.second;


            int x =
                std::get<0>(
                    entry.first
                );

            int y =
                std::get<1>(
                    entry.first
                );

            int z =
                std::get<2>(
                    entry.first
                );


            // ------------------------------------------------
            // Check which sides of this block are exposed
            // ------------------------------------------------

            bool frontVisible =
                !world.isSolidAt(
                    x,
                    y,
                    z + 1
                );


            bool backVisible =
                !world.isSolidAt(
                    x,
                    y,
                    z - 1
                );


            bool leftVisible =
                !world.isSolidAt(
                    x - 1,
                    y,
                    z
                );


            bool rightVisible =
                !world.isSolidAt(
                    x + 1,
                    y,
                    z
                );


            bool topVisible =
                !world.isSolidAt(
                    x,
                    y + 1,
                    z
                );


            bool bottomVisible =
                !world.isSolidAt(
                    x,
                    y - 1,
                    z
                );


            // ------------------------------------------------
            // Completely buried block
            //
            // Skip everything else for this block.
            // ------------------------------------------------

            if (
                !frontVisible &&
                !backVisible &&
                !leftVisible &&
                !rightVisible &&
                !topVisible &&
                !bottomVisible
                )
            {
                continue;
            }


            // ------------------------------------------------
            // Block texture
            // ------------------------------------------------

            glUniform1f(
                textureRowLocation,
                static_cast<float>(
                    block.textureRow
                    )
            );


            // ------------------------------------------------
            // Block position
            // ------------------------------------------------

            glm::mat4 model =
                glm::mat4(
                    1.0f
                );


            model =
                glm::translate(
                    model,
                    glm::vec3(
                        static_cast<float>(x),
                        static_cast<float>(y),
                        static_cast<float>(z)
                    )
                );


            glUniformMatrix4fv(
                modelLocation,
                1,
                GL_FALSE,
                glm::value_ptr(model)
            );


            // ------------------------------------------------
            // Draw only visible faces
            //
            // Each cube face contains 6 vertices.
            // ------------------------------------------------


            // Front face
            if (frontVisible)
            {
                glDrawArrays(
                    GL_TRIANGLES,
                    0,
                    6
                );
            }


            // Back face
            if (backVisible)
            {
                glDrawArrays(
                    GL_TRIANGLES,
                    6,
                    6
                );
            }


            // Left face
            if (leftVisible)
            {
                glDrawArrays(
                    GL_TRIANGLES,
                    12,
                    6
                );
            }


            // Right face
            if (rightVisible)
            {
                glDrawArrays(
                    GL_TRIANGLES,
                    18,
                    6
                );
            }


            // Top face
            if (topVisible)
            {
                glDrawArrays(
                    GL_TRIANGLES,
                    24,
                    6
                );
            }


            // Bottom face
            if (bottomVisible)
            {
                glDrawArrays(
                    GL_TRIANGLES,
                    30,
                    6
                );
            }
        }


        // ----------------------------------------------------
        // Draw NPCs
        // ----------------------------------------------------

        for (const NPC& npc : npcs)
        {
            npcRenderer.drawNPC(
                npc,
                renderer,
                npcAtlasTexture
            );
        }

        // ----------------------------------------------------
        // Draw UI
        // ----------------------------------------------------

        uiRenderer.drawHotbar(
            scrollWheelTexture,
            blockAtlasTexture,
            inventory.getSelectedSlot(),
            inventory,
            textureManager.getBlockCount()
        );


        uiRenderer.drawCrosshair();


        // ----------------------------------------------------
        // Finish frame
        // ----------------------------------------------------

        glfwSwapBuffers(
            window
        );


        glfwPollEvents();


        // ----------------------------------------------------
        // Mouse-wheel hotbar selection
        // ----------------------------------------------------

        if (scrollAmount > 0.0)
        {
            inventory.cycleSlot(
                -1
            );
        }
        else if (scrollAmount < 0.0)
        {
            inventory.cycleSlot(
                1
            );
        }


        scrollAmount =
            0.0;
    }


    // ========================================================
    // 10. Shutdown
    // ========================================================

    glfwDestroyWindow(
        window
    );


    glfwTerminate();


    return 0;
}