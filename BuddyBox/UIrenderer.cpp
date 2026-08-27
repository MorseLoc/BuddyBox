#include "UIRenderer.h"

#include <glad/glad.h>


// ============================================================
// UIRenderer constructor
//
// Starts with no OpenGL resources created.
// initialize() creates them later.
// ============================================================

UIRenderer::UIRenderer()
{
    VAO = 0;
    VBO = 0;
    shaderProgram = 0;
}


// ============================================================
// Initialize UI renderer
//
// Creates:
// - A reusable rectangle
// - The rectangle's VAO and VBO
// - The UI vertex shader
// - The UI fragment shader
// - The final UI shader program
//
// Returns true when initialization finishes.
// ============================================================

bool UIRenderer::initialize()
{
    // ========================================================
    // 1. Rectangle vertex data
    // ========================================================

    // Two triangles forming one rectangle.
    //
    // Each vertex stores:
    // X, Y = position
    // U, V = texture coordinates
    //
    // This same rectangle is resized and repositioned
    // to draw all current UI elements.
    float vertices[] =
    {
        // position      // texture coordinates
        -1.0f, -1.0f,    0.0f, 0.0f,
         1.0f, -1.0f,    1.0f, 0.0f,
         1.0f,  1.0f,    1.0f, 1.0f,

        -1.0f, -1.0f,    0.0f, 0.0f,
         1.0f,  1.0f,    1.0f, 1.0f,
        -1.0f,  1.0f,    0.0f, 1.0f
    };


    // ========================================================
    // 2. Create VAO and VBO
    // ========================================================

    glGenVertexArrays(
        1,
        &VAO
    );


    glGenBuffers(
        1,
        &VBO
    );


    glBindVertexArray(
        VAO
    );


    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );


    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );


    // --------------------------------------------------------
    // Vertex attribute 0: position
    // --------------------------------------------------------

    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)0
    );


    glEnableVertexAttribArray(
        0
    );


    // --------------------------------------------------------
    // Vertex attribute 1: texture coordinates
    // --------------------------------------------------------

    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)(2 * sizeof(float))
    );


    glEnableVertexAttribArray(
        1
    );


    // ========================================================
    // 3. Vertex shader
    // ========================================================

    // The vertex shader resizes and moves the reusable
    // rectangle to the correct place on the screen.
    const char* vertexShaderSource = R"(
        #version 330 core

        layout(location = 0) in vec2 position;
        layout(location = 1) in vec2 textureCoordinate;

        out vec2 uv;

        uniform vec2 uiScale;
        uniform vec2 uiPosition;

        void main()
        {
            vec2 screenPosition =
                position * uiScale + uiPosition;

            gl_Position =
                vec4(screenPosition, 0.0, 1.0);

            uv = textureCoordinate;
        }
    )";


    // ========================================================
    // 4. Fragment shader
    // ========================================================

    // The fragment shader has three drawing modes:
    //
    // 0 = hotbar frame
    // 1 = block icon
    // 2 = solid white shape
    const char* fragmentShaderSource = R"(
        #version 330 core

        in vec2 uv;

        out vec4 finalColor;

        uniform sampler2D uiTexture;

        uniform int drawMode;

        // Hotbar frame information.
        uniform float frameStart;
        uniform float frameHeight;

        // Block atlas information.
        uniform float textureRow;
        uniform float atlasRows;

        void main()
        {
            // -----------------------------------------------
            // Mode 0: hotbar frame
            // -----------------------------------------------

            if (drawMode == 0)
            {
                vec2 frameUV = uv;

                frameUV.y =
                    frameStart +
                    uv.y * frameHeight;

                finalColor =
                    texture(
                        uiTexture,
                        frameUV
                    );
            }

            // -----------------------------------------------
            // Mode 1: block icon
            // -----------------------------------------------

            else if (drawMode == 1)
            {
                // BuddyBox block textures contain
                // six cube faces horizontally.
                //
                // Face 1 is the front face.
                float atlasU =
                    (1.0 + uv.x) / 6.0;

                float atlasV =
                    (
                        textureRow +
                        (1.0 - uv.y)
                    )
                    / atlasRows;

                finalColor =
                    texture(
                        uiTexture,
                        vec2(
                            atlasU,
                            atlasV
                        )
                    );
            }

            // -----------------------------------------------
            // Mode 2: solid white shape
            // -----------------------------------------------

            else if (drawMode == 2)
            {
                finalColor =
                    vec4(
                        1.0,
                        1.0,
                        1.0,
                        1.0
                    );
            }
        }
    )";


    // ========================================================
    // 5. Compile shaders
    // ========================================================

    unsigned int vertexShader =
        glCreateShader(
            GL_VERTEX_SHADER
        );


    glShaderSource(
        vertexShader,
        1,
        &vertexShaderSource,
        nullptr
    );


    glCompileShader(
        vertexShader
    );


    unsigned int fragmentShader =
        glCreateShader(
            GL_FRAGMENT_SHADER
        );


    glShaderSource(
        fragmentShader,
        1,
        &fragmentShaderSource,
        nullptr
    );


    glCompileShader(
        fragmentShader
    );


    // ========================================================
    // 6. Create shader program
    // ========================================================

    shaderProgram =
        glCreateProgram();


    glAttachShader(
        shaderProgram,
        vertexShader
    );


    glAttachShader(
        shaderProgram,
        fragmentShader
    );


    glLinkProgram(
        shaderProgram
    );


    // The individual shader objects are no longer
    // needed after the program has been linked.
    glDeleteShader(
        vertexShader
    );


    glDeleteShader(
        fragmentShader
    );


    return true;
}


// ============================================================
// Draw hotbar
//
// Draws:
// 1. The selected hotbar frame
// 2. One block icon inside each slot
// ============================================================

void UIRenderer::drawHotbar(
    unsigned int hotbarTexture,
    unsigned int blockAtlasTexture,
    int selectedSlot,
    const Inventory& inventory,
    int atlasRows
)
{
    glUseProgram(
        shaderProgram
    );


    // UI should always appear in front of the 3D world.
    glDisable(
        GL_DEPTH_TEST
    );


    glBindVertexArray(
        VAO
    );


    // --------------------------------------------------------
    // Find shader variables
    // --------------------------------------------------------

    int drawModeLocation =
        glGetUniformLocation(
            shaderProgram,
            "drawMode"
        );


    int textureLocation =
        glGetUniformLocation(
            shaderProgram,
            "uiTexture"
        );


    int scaleLocation =
        glGetUniformLocation(
            shaderProgram,
            "uiScale"
        );


    int positionLocation =
        glGetUniformLocation(
            shaderProgram,
            "uiPosition"
        );


    int frameStartLocation =
        glGetUniformLocation(
            shaderProgram,
            "frameStart"
        );


    int frameHeightLocation =
        glGetUniformLocation(
            shaderProgram,
            "frameHeight"
        );


    int atlasRowsLocation =
        glGetUniformLocation(
            shaderProgram,
            "atlasRows"
        );


    int textureRowLocation =
        glGetUniformLocation(
            shaderProgram,
            "textureRow"
        );


    // Use texture slot 0 for UI textures.
    glActiveTexture(
        GL_TEXTURE0
    );


    glUniform1i(
        textureLocation,
        0
    );


    // ========================================================
    // Draw selected hotbar frame
    // ========================================================

    glUniform1i(
        drawModeLocation,
        0
    );


    glBindTexture(
        GL_TEXTURE_2D,
        hotbarTexture
    );


    // Size of the hotbar frame.
    glUniform2f(
        scaleLocation,
        0.45f,
        0.075f
    );


    // Position near the bottom-center of the screen.
    glUniform2f(
        positionLocation,
        0.0f,
        -0.85f
    );


    // ScrollWheel.png contains six frames stacked vertically.
    float frameHeight =
        1.0f / 6.0f;


    // Choose the frame matching the selected slot.
    float frameStart =
        static_cast<float>(selectedSlot)
        * frameHeight;


    glUniform1f(
        frameStartLocation,
        frameStart
    );


    glUniform1f(
        frameHeightLocation,
        frameHeight
    );


    glDrawArrays(
        GL_TRIANGLES,
        0,
        6
    );


    // ========================================================
    // Draw block icons
    // ========================================================

    // Switch the shader into block-icon mode.
    glUniform1i(
        drawModeLocation,
        1
    );


    // Block icons come from the main block atlas.
    glBindTexture(
        GL_TEXTURE_2D,
        blockAtlasTexture
    );


    glUniform1f(
        atlasRowsLocation,
        static_cast<float>(atlasRows)
    );


    // Size of each block icon.
    glUniform2f(
        scaleLocation,
        0.055f,
        0.065f
    );


    // Draw one icon for each current hotbar slot.
    for (int slot = 0; slot < 6; slot++)
    {
        // Find which block belongs in this slot.
        BlockType blockType =
            inventory.getBlockTypeAtSlot(
                slot
            );


        // Create a temporary block so we can use
        // its texture-row information.
        Block block(
            blockType
        );


        glUniform1f(
            textureRowLocation,
            static_cast<float>(
                block.textureRow
                )
        );


        // Calculate this icon's horizontal position.
        float slotX =
            -0.375f +
            (
                static_cast<float>(slot)
                * 0.15f
                );


        glUniform2f(
            positionLocation,
            slotX,
            -0.85f
        );


        glDrawArrays(
            GL_TRIANGLES,
            0,
            6
        );
    }


    // Restore normal 3D depth testing.
    glEnable(
        GL_DEPTH_TEST
    );
}


// ============================================================
// Draw crosshair
//
// Draws two small white rectangles in the center
// of the screen.
// ============================================================

void UIRenderer::drawCrosshair()
{
    glUseProgram(
        shaderProgram
    );


    // Crosshair should appear over the 3D world.
    glDisable(
        GL_DEPTH_TEST
    );


    glBindVertexArray(
        VAO
    );


    int scaleLocation =
        glGetUniformLocation(
            shaderProgram,
            "uiScale"
        );


    int positionLocation =
        glGetUniformLocation(
            shaderProgram,
            "uiPosition"
        );


    int drawModeLocation =
        glGetUniformLocation(
            shaderProgram,
            "drawMode"
        );


    // Mode 2 draws a solid white rectangle.
    glUniform1i(
        drawModeLocation,
        2
    );


    // --------------------------------------------------------
    // Vertical crosshair bar
    // --------------------------------------------------------

    glUniform2f(
        scaleLocation,
        0.008f,
        0.035f
    );


    glUniform2f(
        positionLocation,
        0.0f,
        0.0f
    );


    glDrawArrays(
        GL_TRIANGLES,
        0,
        6
    );


    // --------------------------------------------------------
    // Horizontal crosshair bar
    // --------------------------------------------------------

    glUniform2f(
        scaleLocation,
        0.025f,
        0.010f
    );


    glUniform2f(
        positionLocation,
        0.0f,
        0.0f
    );


    glDrawArrays(
        GL_TRIANGLES,
        0,
        6
    );


    // Restore normal world rendering.
    glEnable(
        GL_DEPTH_TEST
    );
}