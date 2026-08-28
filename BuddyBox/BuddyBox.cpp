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


    // Temporary test NPC.
    //
    // Later this will be replaced by
    // NPCs created by Spawner blocks.
    NPC testJebub(
        NPCType::Jebub,
        glm::vec3(
            2.0f,
            1.15f,
            2.0f
        )
    );


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
        // NPC update
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


        // ----------------------------------------------------
        // Draw NPCs
        // ----------------------------------------------------

        npcRenderer.drawNPC(
            testJebub,
            renderer,
            npcAtlasTexture
        );


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