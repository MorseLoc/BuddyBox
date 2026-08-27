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


// STB_IMAGE_IMPLEMENTATION must exist in exactly one .cpp file.
// It provides the actual image-loading code used by stb_image.
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


// ============================================================
// Mouse wheel input
// ============================================================

// Stores mouse-wheel movement until the game loop handles it.
double scrollAmount = 0.0;


// GLFW automatically calls this whenever the mouse wheel moves.
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
    // 2. Create the game window
    // --------------------------------------------------------

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


    // Tell OpenGL which window we want to draw into.
    glfwMakeContextCurrent(
        window
    );


    // V-Sync:
    //
    // 1 = match monitor refresh rate.
    // 0 = render as fast as possible.
    glfwSwapInterval(
        1
    );


    // Hide and lock the cursor
    // so the mouse controls the camera.
    glfwSetInputMode(
        window,
        GLFW_CURSOR,
        GLFW_CURSOR_DISABLED
    );


    // Tell GLFW which function handles mouse-wheel movement.
    glfwSetScrollCallback(
        window,
        scrollCallback
    );


    // --------------------------------------------------------
    // 3. Load OpenGL functions
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

    UIRenderer uiRenderer;

    TextureManager textureManager;

    Player player;

    World world;

    Camera camera;

    Inventory inventory;


    // --------------------------------------------------------
    // Temporary test Jebub
    //
    // This will be removed once Spawner blocks
    // create NPCs automatically.
    // --------------------------------------------------------

    NPC testJebub(
        NPCType::Jebub,
        glm::vec3(
            2.0f,
            1.15f,
            2.0f
        )
    );


    // --------------------------------------------------------
    // Initialize UI renderer
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


    // Depth testing makes closer 3D objects
    // cover objects behind them.
    glEnable(
        GL_DEPTH_TEST
    );


    // --------------------------------------------------------
    // 5. Load textures
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


    // Get the block shader created by Renderer.
    unsigned int shaderProgram =
        renderer.getShaderProgram();


    // --------------------------------------------------------
    // 6. Load inventory and world
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
    }


    // --------------------------------------------------------
    // 7. Variables used by the game loop
    // --------------------------------------------------------

    // Time between frames.
    float deltaTime =
        0.0f;


    float lastFrame =
        0.0f;


    // Previous mouse-button states.
    //
    // These let one click perform one action
    // instead of repeating every frame.
    bool leftMouseWasPressed =
        false;


    bool rightMouseWasPressed =
        false;


    // ========================================================
    // 8. Main game loop
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


        // Prevent giant movement jumps
        // after temporary freezes or window dragging.
        if (deltaTime > 0.05f)
        {
            deltaTime =
                0.05f;
        }


        // ----------------------------------------------------
        // Camera and player movement
        // ----------------------------------------------------

        // Update camera direction first.
        camera.update(
            window
        );


        // Then move the player using that direction.
        player.move(
            window,
            deltaTime,
            camera.getFront(),
            camera.getUp(),
            world
        );


        // Move the camera to the player's new position.
        camera.updatePosition(
            player.position
        );


        // ----------------------------------------------------
        // Update test Jebub
        //
        // Temporary until we create a real NPC list.
        // ----------------------------------------------------

        testJebub.update(
            deltaTime,
            world
        );


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


            // Shoot a ray forward from the camera.
            //
            // If it hits a block within 5 blocks,
            // remove that block.
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
                // Create the block currently selected
                // in the player's hotbar.
                Block block(
                    inventory.getSelectedBlockType()
                );


                // --------------------------------------------
                // Player collision box
                // --------------------------------------------

                glm::vec3 playerMin =
                    player.position -
                    (player.size / 2.0f);


                glm::vec3 playerMax =
                    player.position +
                    (player.size / 2.0f);


                // --------------------------------------------
                // New block collision box
                // --------------------------------------------

                // BuddyBox blocks are centered on
                // their grid coordinates.
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


                // Check whether the new block
                // would overlap the player's body.
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


        // Save mouse-button states
        // for the next frame.
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


        // Prevent division by zero
        // if the window is minimized.
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
        // Clear previous frame
        // ----------------------------------------------------

        // Sky color.
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
        // Prepare block rendering
        // ----------------------------------------------------

        glUseProgram(
            shaderProgram
        );


        // Use texture slot 0 for the block atlas.
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


        // Make sure normal world blocks
        // are using texture mode.
        int useSolidColorLocation =
            glGetUniformLocation(
                shaderProgram,
                "useSolidColor"
            );


        glUniform1i(
            useSolidColorLocation,
            0
        );


        // ----------------------------------------------------
        // Find shader variables
        // ----------------------------------------------------

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


        // ----------------------------------------------------
        // Send camera matrices to shader
        // ----------------------------------------------------

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
        // Draw the world
        // ----------------------------------------------------

        for (const auto& entry : world.blocks)
        {
            const Block& block =
                entry.second;


            glUniform1f(
                textureRowLocation,
                static_cast<float>(
                    block.textureRow
                    )
            );


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


            glDrawArrays(
                GL_TRIANGLES,
                0,
                36
            );
        }


        // ====================================================
        // Draw test Jebub
        // ====================================================

        glm::vec3 jebubPosition =
            testJebub.getPosition();


        glm::vec3 jebubColor(
            1.0f,
            1.0f,
            0.0f
        );


        // ----------------------------------------------------
        // Jebub body
        // ----------------------------------------------------

        renderer.drawColoredCube(
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

            jebubColor
        );


        // ----------------------------------------------------
        // Jebub left leg
        // ----------------------------------------------------

        renderer.drawColoredCube(
            jebubPosition +
            glm::vec3(
                -0.20f,
                -0.40f,
                0.0f
            ),

            glm::vec3(
                0.22f,
                0.50f,
                0.22f
            ),

            jebubColor
        );


        // ----------------------------------------------------
        // Jebub right leg
        // ----------------------------------------------------

        renderer.drawColoredCube(
            jebubPosition +
            glm::vec3(
                0.20f,
                -0.40f,
                0.0f
            ),

            glm::vec3(
                0.22f,
                0.50f,
                0.22f
            ),

            jebubColor
        );


        // ----------------------------------------------------
        // Draw UI
        //
        // UI goes last so it stays on top
        // of the 3D world and NPCs.
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

        // Show the frame we just rendered.
        glfwSwapBuffers(
            window
        );


        // Process keyboard, mouse,
        // window, and close events.
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


        // Scroll input has now been handled.
        scrollAmount =
            0.0;
    }


    // ========================================================
    // 9. Shutdown
    // ========================================================

    glfwDestroyWindow(
        window
    );


    glfwTerminate();


    return 0;
}