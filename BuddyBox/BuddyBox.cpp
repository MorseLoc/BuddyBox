#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <memory>
#include <set>
#include <tuple>
#include <map>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Define this in exactly one .cpp file.
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Block.h"
#include "player.h"
#include "world.h"
#include "renderer.h"
#include "camera.h"
#include "textureManager.h"
#include "inventory.h"
#include "inventoryLayout.h"
#include "UIrenderer.h"
#include "NPC.h"
#include "NPCrenderer.h"
#include "ChunkMesh.h"
#include "DroppedItem.h"
#include "lighting.h"


// ============================================================
// Mouse wheel
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
    // 1. Start GLFW and create the window
    // --------------------------------------------------------

    if (!glfwInit())
    {
        std::cout << "GLFW failed to start.\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(
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

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetInputMode(
        window,
        GLFW_CURSOR,
        GLFW_CURSOR_DISABLED
    );

    glfwSetScrollCallback(window, scrollCallback);

    // --------------------------------------------------------
    // 2. Load OpenGL
    // --------------------------------------------------------

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD failed to start.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // --------------------------------------------------------
    // 3. Create game systems
    // --------------------------------------------------------

    Renderer renderer;
    NPCRenderer npcRenderer;
    UIRenderer uiRenderer;
    TextureManager textureManager;

    Player player;
    World world;
    Camera camera;
    Inventory inventory;
    Lighting lighting;

    std::vector<DroppedItem> droppedItems;
    std::vector<NPC> npcs;

    std::map<
        std::tuple<int, int, int>,
        std::unique_ptr<ChunkMesh>
    > chunkMeshes;

    // --------------------------------------------------------
    // 4. Chunk helpers
    // --------------------------------------------------------

    auto getChunkCoordinate = [](int blockCoordinate)
        {
            if (blockCoordinate >= 0)
            {
                return blockCoordinate / ChunkMesh::CHUNK_SIZE;
            }

            return (
                blockCoordinate - (ChunkMesh::CHUNK_SIZE - 1)
                ) / ChunkMesh::CHUNK_SIZE;
        };

    // Build all meshes when initially loading the world.
    auto rebuildAllChunks = [&]()
        {
            chunkMeshes.clear();

            std::set<std::tuple<int, int, int>> usedChunks;

            for (const auto& entry : world.blocks)
            {
                int blockX = std::get<0>(entry.first);
                int blockY = std::get<1>(entry.first);
                int blockZ = std::get<2>(entry.first);

                int chunkX = getChunkCoordinate(blockX);
                int chunkY = getChunkCoordinate(blockY);
                int chunkZ = getChunkCoordinate(blockZ);

                usedChunks.insert(
                    std::make_tuple(chunkX, chunkY, chunkZ)
                );
            }

            for (const auto& chunkPosition : usedChunks)
            {
                int chunkX = std::get<0>(chunkPosition);
                int chunkY = std::get<1>(chunkPosition);
                int chunkZ = std::get<2>(chunkPosition);

                auto chunk = std::make_unique<ChunkMesh>();

                chunk->build(
                    world,
                    lighting,
                    chunkX,
                    chunkY,
                    chunkZ
                );

                chunkMeshes[
                    std::make_tuple(chunkX, chunkY, chunkZ)
                ] = std::move(chunk);
            }
        };

    // Rebuild one chunk after a change.
    auto rebuildChunk = [&](int chunkX, int chunkY, int chunkZ)
        {
            auto chunk = std::make_unique<ChunkMesh>();

            chunk->build(
                world,
                lighting,
                chunkX,
                chunkY,
                chunkZ
            );

            chunkMeshes[
                std::make_tuple(chunkX, chunkY, chunkZ)
            ] = std::move(chunk);
        };

    // Rebuild the edited chunk and any shared boundary faces.
    auto rebuildChunksAroundBlock =
        [&](int blockX, int blockY, int blockZ)
        {
            int chunkX = getChunkCoordinate(blockX);
            int chunkY = getChunkCoordinate(blockY);
            int chunkZ = getChunkCoordinate(blockZ);

            rebuildChunk(chunkX, chunkY, chunkZ);

            int localX = blockX - chunkX * ChunkMesh::CHUNK_SIZE;
            int localY = blockY - chunkY * ChunkMesh::CHUNK_SIZE;
            int localZ = blockZ - chunkZ * ChunkMesh::CHUNK_SIZE;

            if (localX == 0)
            {
                rebuildChunk(chunkX - 1, chunkY, chunkZ);
            }

            if (localX == ChunkMesh::CHUNK_SIZE - 1)
            {
                rebuildChunk(chunkX + 1, chunkY, chunkZ);
            }

            if (localY == 0)
            {
                rebuildChunk(chunkX, chunkY - 1, chunkZ);
            }

            if (localY == ChunkMesh::CHUNK_SIZE - 1)
            {
                rebuildChunk(chunkX, chunkY + 1, chunkZ);
            }

            if (localZ == 0)
            {
                rebuildChunk(chunkX, chunkY, chunkZ - 1);
            }

            if (localZ == ChunkMesh::CHUNK_SIZE - 1)
            {
                rebuildChunk(chunkX, chunkY, chunkZ + 1);
            }
        };

    // Refresh meshes whose stored lighting changed.
    auto rebuildLightingChunks = [&](
        const std::set<std::tuple<int, int, int>>& dirtyChunks
        )
        {
            for (const auto& chunkPosition : dirtyChunks)
            {
                // Air needs lighting storage, but not its own GPU mesh.
                if (chunkMeshes.find(chunkPosition) == chunkMeshes.end())
                {
                    continue;
                }

                int chunkX = std::get<0>(chunkPosition);
                int chunkY = std::get<1>(chunkPosition);
                int chunkZ = std::get<2>(chunkPosition);

                rebuildChunk(chunkX, chunkY, chunkZ);
            }
        };

    // --------------------------------------------------------
    // 5. Initialize rendering
    // --------------------------------------------------------

    if (!uiRenderer.initialize())
    {
        std::cout << "Failed to initialize UI renderer.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );

    // --------------------------------------------------------
    // 6. Load textures
    // --------------------------------------------------------

    if (!textureManager.loadAtlas("textures/artdex.png"))
    {
        std::cout << "Failed to load artdex.png\n";
    }

    unsigned int itemAtlasTexture =
        textureManager.loadTexture("textures/Itemdex.png");

    unsigned int blockAtlasTexture =
        textureManager.getAtlasTexture();

    unsigned int scrollWheelTexture =
        textureManager.loadTexture("textures/ScrollWheel.png");

    unsigned int inventoryTexture =
        textureManager.loadTexture("textures/Inventory.png");

    unsigned int numberAtlasTexture =
        textureManager.loadTexture("textures/Numberdex.png");

    unsigned int npcAtlasTexture =
        textureManager.loadTexture("textures/NPCdex.png");

    unsigned int shaderProgram = renderer.getShaderProgram();

    // --------------------------------------------------------
    // 7. Load inventory and world
    // --------------------------------------------------------

    if (!inventory.loadFromFile("inventory.txt"))
    {
        std::cout << "Failed to load inventory.txt\n";
    }

    if (!world.loadFromFile("test.world"))
    {
        std::cout << "Failed to load test.world\n";
    }
    else
    {
        std::cout
            << "Blocks loaded: "
            << world.blocks.size()
            << "\n";

        lighting.calculateSkyLight(world);
        rebuildAllChunks();

        std::cout
            << "Chunk meshes built: "
            << chunkMeshes.size()
            << "\n";
    }

    // --------------------------------------------------------
    // 8. Game-loop state
    // --------------------------------------------------------

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    bool worldMouseArmed = false;
    bool wasFocused = true;

    bool leftMouseWasPressed = false;
    bool rightMouseWasPressed = false;

    bool inventoryOpen = false;
    bool qWasPressed = false;

    float blockBreakTimer = 0.0f;
    float blockBreakProgress = 0.0f;

    int breakingBlockX = 0;
    int breakingBlockY = 0;
    int breakingBlockZ = 0;

    bool isBreakingBlock = false;

    // ========================================================
    // 9. Main game loop
    // ========================================================

    while (!glfwWindowShouldClose(window))
    {
        // Read new events before checking input.
        glfwPollEvents();

        const bool focused =
            glfwGetWindowAttrib(window, GLFW_FOCUSED) == GLFW_TRUE;

        const bool leftMousePressed =
            glfwGetMouseButton(
                window, GLFW_MOUSE_BUTTON_LEFT
            ) == GLFW_PRESS;

        const bool rightMousePressed =
            glfwGetMouseButton(
                window, GLFW_MOUSE_BUTTON_RIGHT
            ) == GLFW_PRESS;

        if (!focused)
        {
            inventory.cancelDrag();
            worldMouseArmed = false;
        }

        if (focused != wasFocused)
        {
            camera.ignoreNextMouseMove();
        }

        wasFocused = focused;

        // ----------------------------------------------------
        // Frame timing
        // ----------------------------------------------------

        float currentFrame = static_cast<float>(glfwGetTime());

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (deltaTime > 0.05f)
        {
            deltaTime = 0.05f;
        }

        // ----------------------------------------------------
        // Open or close inventory
        // ----------------------------------------------------

        bool qPressed =
            glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;

        if (focused && qPressed && !qWasPressed)
        {
            inventoryOpen = !inventoryOpen;

            inventory.cancelDrag();
            worldMouseArmed = false;
            scrollAmount = 0.0;

            if (inventoryOpen)
            {
                // Free the mouse for dragging inventory items.
                glfwSetInputMode(
                    window,
                    GLFW_CURSOR,
                    GLFW_CURSOR_NORMAL
                );
            }
            else
            {
                // Return mouse control to the camera.
                glfwSetInputMode(
                    window,
                    GLFW_CURSOR,
                    GLFW_CURSOR_DISABLED
                );

                camera.ignoreNextMouseMove();
            }
        }

        qWasPressed = qPressed;

        // ----------------------------------------------------
        // Camera and player
        // ----------------------------------------------------

        if (!inventoryOpen && focused)
        {
            camera.update(window);
        }

        // Walking and jumping remain enabled in inventory.
        // Only losing window focus disables movement input.
        player.move(
            window,
            deltaTime,
            camera.getFront(),
            camera.getUp(),
            world,
            focused
        );

        camera.updatePosition(player.position);

        // ----------------------------------------------------
        // Active blocks
        // ----------------------------------------------------

        for (const auto& position : world.activeBlocks)
        {
            auto blockIt = world.blocks.find(position);

            if (blockIt == world.blocks.end())
            {
                continue;
            }

            Block& block = blockIt->second;

            int x = std::get<0>(position);
            int y = std::get<1>(position);
            int z = std::get<2>(position);

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
            npc.update(deltaTime, world);
        }

        // ----------------------------------------------------
        // Dropped item gravity
        // ----------------------------------------------------

        for (DroppedItem& droppedItem : droppedItems)
        {
            const float gravity = -23.0f;

            droppedItem.verticalVelocity += gravity * deltaTime;

            float nextY =
                droppedItem.position.y +
                droppedItem.verticalVelocity * deltaTime;

            int blockX = static_cast<int>(
                std::floor(droppedItem.position.x + 0.5f)
                );

            int blockZ = static_cast<int>(
                std::floor(droppedItem.position.z + 0.5f)
                );

            // The sprite extends 0.25 below its center.
            float nextBottom = nextY - 0.25f;

            int blockY = static_cast<int>(
                std::floor(nextBottom + 0.5f)
                );

            if (
                droppedItem.verticalVelocity < 0.0f &&
                world.isSolidAt(blockX, blockY, blockZ)
                )
            {
                droppedItem.position.y =
                    static_cast<float>(blockY) + 0.75f;

                droppedItem.verticalVelocity = 0.0f;
            }
            else
            {
                droppedItem.position.y = nextY;
            }
        }

        // ----------------------------------------------------
        // Pick up nearby items
        // ----------------------------------------------------

        for (
            int i = 0;
            i < static_cast<int>(droppedItems.size());
            )
        {
            float distanceToPlayer = glm::length(
                droppedItems[i].position - player.position
            );

            if (distanceToPlayer < 2.0f)
            {
                bool itemWasAdded = inventory.addItem(
                    droppedItems[i].type
                );

                if (itemWasAdded)
                {
                    droppedItems.erase(droppedItems.begin() + i);

                    // The next item moved into this same index.
                    continue;
                }
            }

            ++i;
        }

        // ----------------------------------------------------
        // Inventory mouse position and dragging
        // ----------------------------------------------------

        int cursorWidth = 0;
        int cursorHeight = 0;

        double cursorX = 0.0;
        double cursorY = 0.0;

        glfwGetWindowSize(window, &cursorWidth, &cursorHeight);
        glfwGetCursorPos(window, &cursorX, &cursorY);

        const int hoveredSlot = InventoryLayout::slotAt(
            cursorX,
            cursorY,
            cursorWidth,
            cursorHeight
        );

        const float mouseUiX = cursorWidth > 0
            ? static_cast<float>(2.0 * cursorX / cursorWidth - 1.0)
            : 0.0f;

        const float mouseUiY = cursorHeight > 0
            ? static_cast<float>(1.0 - 2.0 * cursorY / cursorHeight)
            : 0.0f;

        if (inventoryOpen)
        {
            worldMouseArmed = false;

            if (
                focused &&
                leftMousePressed &&
                !leftMouseWasPressed
                )
            {
                inventory.beginDrag(hoveredSlot);
            }

            if (
                focused &&
                !leftMousePressed &&
                leftMouseWasPressed
                )
            {
                inventory.finishDrag(hoveredSlot);
            }
        }
        else if (
            focused &&
            !leftMousePressed &&
            !rightMousePressed
            )
        {
            // Require release after closing inventory.
            // A held inventory click must not start mining.
            worldMouseArmed = true;
        }

        const bool allowWorldMouse =
            focused && !inventoryOpen && worldMouseArmed;

        // ----------------------------------------------------
        // Break blocks
        // ----------------------------------------------------

        if (allowWorldMouse && leftMousePressed)
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
                if (!isBreakingBlock)
                {
                    breakingBlockX = hitX;
                    breakingBlockY = hitY;
                    breakingBlockZ = hitZ;

                    blockBreakTimer = 0.0f;
                    blockBreakProgress = 0.0f;
                    isBreakingBlock = true;
                }

                // Restart progress when aiming at another block.
                if (
                    hitX != breakingBlockX ||
                    hitY != breakingBlockY ||
                    hitZ != breakingBlockZ
                    )
                {
                    breakingBlockX = hitX;
                    breakingBlockY = hitY;
                    breakingBlockZ = hitZ;

                    blockBreakTimer = 0.0f;
                    blockBreakProgress = 0.0f;
                }

                blockBreakTimer += deltaTime;

                auto blockIt = world.blocks.find(
                    std::make_tuple(hitX, hitY, hitZ)
                );

                if (blockIt != world.blocks.end())
                {
                    Block& block = blockIt->second;

                    float requiredBreakTime =
                        block.durability * player.breakSpeed;

                    blockBreakProgress =
                        blockBreakTimer / requiredBreakTime;

                    if (blockBreakProgress > 1.0f)
                    {
                        blockBreakProgress = 1.0f;
                    }

                    if (blockBreakTimer >= requiredBreakTime)
                    {
                        // Create the drop before removing the block.
                        if (block.dropItem != ItemType::None)
                        {
                            droppedItems.emplace_back(
                                block.dropItem,
                                glm::vec3(
                                    static_cast<float>(hitX),
                                    static_cast<float>(hitY),
                                    static_cast<float>(hitZ)
                                )
                            );
                        }

                        world.removeBlock(hitX, hitY, hitZ);

                        // Lighting must see the updated world.
                        auto dirtyLightChunks =
                            lighting.updateBlockChange(
                                world,
                                hitX,
                                hitY,
                                hitZ
                            );

                        rebuildChunksAroundBlock(hitX, hitY, hitZ);
                        rebuildLightingChunks(dirtyLightChunks);

                        blockBreakTimer = 0.0f;
                        blockBreakProgress = 0.0f;
                        isBreakingBlock = false;
                    }
                }
            }
            else
            {
                blockBreakTimer = 0.0f;
                blockBreakProgress = 0.0f;
                isBreakingBlock = false;
            }
        }
        else
        {
            // Stop mining when the button is released,
            // inventory opens, or the game loses focus.
            blockBreakTimer = 0.0f;
            blockBreakProgress = 0.0f;
            isBreakingBlock = false;
        }

        // ----------------------------------------------------
        // Place blocks
        // ----------------------------------------------------

        if (
            allowWorldMouse &&
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
                Item selectedItem(inventory.getSelectedItemType());

                // Sticks and other non-placeable items do nothing.
                if (selectedItem.feature == ItemFeature::PlaceBlock)
                {
                    Block block(selectedItem.placedBlockType);

                    glm::vec3 playerMin =
                        player.position - player.size / 2.0f;

                    glm::vec3 playerMax =
                        player.position + player.size / 2.0f;

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

                    if (
                        !overlapsPlayer &&
                        !world.hasBlock(
                            previousX, previousY, previousZ
                        ) &&
                        inventory.getAmountAtSlot(
                            inventory.getSelectedSlot()
                        ) > 0
                        )
                    {
                        world.placeBlock(
                            previousX,
                            previousY,
                            previousZ,
                            block
                        );

                        auto dirtyLightChunks =
                            lighting.updateBlockChange(
                                world,
                                previousX,
                                previousY,
                                previousZ
                            );

                        inventory.removeSelectedItem();

                        rebuildChunksAroundBlock(
                            previousX,
                            previousY,
                            previousZ
                        );

                        rebuildLightingChunks(dirtyLightChunks);
                    }
                }
            }
        }

        leftMouseWasPressed = leftMousePressed;
        rightMouseWasPressed = rightMousePressed;

        // ----------------------------------------------------
        // Camera matrices
        // ----------------------------------------------------

        glm::mat4 view = camera.getViewMatrix();

        int windowWidth;
        int windowHeight;

        glfwGetFramebufferSize(
            window,
            &windowWidth,
            &windowHeight
        );

        if (windowHeight == 0)
        {
            windowHeight = 1;
        }

        glViewport(0, 0, windowWidth, windowHeight);

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            static_cast<float>(windowWidth) /
            static_cast<float>(windowHeight),
            0.1f,
            100.0f
        );

        // ----------------------------------------------------
        // Clear the frame
        // ----------------------------------------------------

        glClearColor(0.42f, 0.75f, 1.0f, 1.0f);

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );

        // ----------------------------------------------------
        // Prepare world rendering
        // ----------------------------------------------------

        glUseProgram(shaderProgram);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, blockAtlasTexture);

        int textureLocation =
            glGetUniformLocation(shaderProgram, "blockTexture");

        glUniform1i(textureLocation, 0);

        int useSolidColorLocation =
            glGetUniformLocation(shaderProgram, "useSolidColor");

        glUniform1i(useSolidColorLocation, 0);

        int modelLocation =
            glGetUniformLocation(shaderProgram, "model");

        int viewLocation =
            glGetUniformLocation(shaderProgram, "view");

        int projectionLocation =
            glGetUniformLocation(shaderProgram, "projection");

        int atlasRowsLocation =
            glGetUniformLocation(shaderProgram, "atlasRows");

        int useVertexTextureRowLocation =
            glGetUniformLocation(
                shaderProgram,
                "useVertexTextureRow"
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
            static_cast<float>(textureManager.getBlockCount())
        );

        // Chunk vertices already use world coordinates.
        glm::mat4 chunkModel = glm::mat4(1.0f);

        glUniformMatrix4fv(
            modelLocation,
            1,
            GL_FALSE,
            glm::value_ptr(chunkModel)
        );

        glUniform1i(useVertexTextureRowLocation, 1);

        int useVertexLightLocation =
            glGetUniformLocation(shaderProgram, "useVertexLight");

        glUniform1i(useVertexLightLocation, 1);

        // ----------------------------------------------------
        // Draw chunk meshes
        // ----------------------------------------------------

        for (const auto& entry : chunkMeshes)
        {
            entry.second->draw();
        }

        glUniform1i(useVertexLightLocation, 0);

        // ----------------------------------------------------
        // Block-breaking overlay
        // ----------------------------------------------------

        if (isBreakingBlock)
        {
            float overlayOpacity = blockBreakProgress * 0.55f;

            renderer.drawColoredCube(
                glm::vec3(
                    static_cast<float>(breakingBlockX),
                    static_cast<float>(breakingBlockY),
                    static_cast<float>(breakingBlockZ)
                ),
                glm::vec3(1.01f, 1.01f, 1.01f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                0.0f,
                overlayOpacity
            );
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
        // Draw dropped items facing the camera
        // ----------------------------------------------------

        for (const DroppedItem& droppedItem : droppedItems)
        {
            Item item(droppedItem.type);

            glm::vec3 directionToCamera =
                camera.getPosition() - droppedItem.position;

            float itemYaw = glm::degrees(
                std::atan2(
                    directionToCamera.x,
                    directionToCamera.z
                )
            );

            renderer.drawDroppedItem(
                droppedItem.position,
                itemAtlasTexture,
                item.textureRow,
                8,
                itemYaw
            );
        }

        // ----------------------------------------------------
        // Draw inventory or hotbar
        // ----------------------------------------------------

        if (inventoryOpen)
        {
            uiRenderer.drawInventory(
                inventoryTexture,
                itemAtlasTexture,
                numberAtlasTexture,
                inventory,
                8,
                mouseUiX,
                mouseUiY,
                focused ? hoveredSlot : -1
            );
        }
        else
        {
            uiRenderer.drawHotbar(
                scrollWheelTexture,
                itemAtlasTexture,
                numberAtlasTexture,
                inventory.getSelectedSlot(),
                inventory,
                8
            );
        }

        if (!inventoryOpen)
        {
            uiRenderer.drawCrosshair();
        }

        // ----------------------------------------------------
        // Finish frame and handle hotbar scrolling
        // ----------------------------------------------------

        glfwSwapBuffers(window);

        if (!inventoryOpen && focused && scrollAmount > 0.0)
        {
            inventory.cycleSlot(-1);
        }
        else if (!inventoryOpen && focused && scrollAmount < 0.0)
        {
            inventory.cycleSlot(1);
        }

        scrollAmount = 0.0;
    }

    // ========================================================
    // 10. Shutdown
    // ========================================================

    // Delete GPU chunk resources before destroying the context.
    chunkMeshes.clear();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}