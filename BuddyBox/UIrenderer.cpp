#include "UIrenderer.h"
#include "inventoryLayout.h"

#include <glad/glad.h>


// ============================================================
// Constructor
// ============================================================

UIRenderer::UIRenderer()
{
    VAO = 0;
    VBO = 0;
    shaderProgram = 0;
}


// ============================================================
// Initialize the UI rectangle and shaders
// ============================================================

bool UIRenderer::initialize()
{
    // Two triangles form one reusable rectangle.
    // Each vertex contains X, Y, U, V.
    float vertices[] =
    {
        -1.0f, -1.0f,    0.0f, 0.0f,
         1.0f, -1.0f,    1.0f, 0.0f,
         1.0f,  1.0f,    1.0f, 1.0f,

        -1.0f, -1.0f,    0.0f, 0.0f,
         1.0f,  1.0f,    1.0f, 1.0f,
        -1.0f,  1.0f,    0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    // Attribute 0: rectangle position.
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    // Attribute 1: texture coordinates.
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)(2 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    // Resize and position the rectangle on screen.
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

            gl_Position = vec4(screenPosition, 0.0, 1.0);
            uv = textureCoordinate;
        }
    )";

    // Drawing modes:
    // 0 = selected hotbar frame
    // 1 = Itemdex icon
    // 2 = solid white rectangle
    // 3 = Numberdex digit
    // 4 = full inventory background
    const char* fragmentShaderSource = R"(
        #version 330 core

        in vec2 uv;
        out vec4 finalColor;

        uniform sampler2D uiTexture;
        uniform int drawMode;

        uniform float frameStart;
        uniform float frameHeight;

        uniform float textureRow;
        uniform float atlasRows;

        uniform float numberDigit;

        void main()
        {
            if (drawMode == 0)
            {
                vec2 frameUV = uv;
                frameUV.y = frameStart + uv.y * frameHeight;

                finalColor = texture(uiTexture, frameUV);
            }
            else if (drawMode == 1)
            {
                float atlasU = uv.x;
                float atlasV =
                    (textureRow + (1.0 - uv.y)) / atlasRows;

                finalColor = texture(
                    uiTexture,
                    vec2(atlasU, atlasV)
                );
            }
            else if (drawMode == 2)
            {
                finalColor = vec4(1.0, 1.0, 1.0, 1.0);
            }
            else if (drawMode == 3)
            {
                float digitWidth = 1.0 / 10.0;
                float digitStart = numberDigit * digitWidth;
                float numberU = digitStart + uv.x * digitWidth;

                finalColor = texture(
                    uiTexture,
                    vec2(numberU, 1.0 - uv.y)
                );
            }
            else if (drawMode == 4)
            {
                finalColor = texture(uiTexture, uv);
            }
        }
    )";

    // Compile the vertex shader.
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Compile the fragment shader.
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Link both shaders into one program.
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}


// ============================================================
// Draw the hotbar
// ============================================================

void UIRenderer::drawHotbar(
    unsigned int hotbarTexture,
    unsigned int itemAtlasTexture,
    unsigned int numberAtlasTexture,
    int selectedSlot,
    const Inventory& inventory,
    int itemAtlasRows
)
{
    glUseProgram(shaderProgram);

    // Draw UI in front of the world.
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(VAO);

    int drawModeLocation =
        glGetUniformLocation(shaderProgram, "drawMode");

    int textureLocation =
        glGetUniformLocation(shaderProgram, "uiTexture");

    int scaleLocation =
        glGetUniformLocation(shaderProgram, "uiScale");

    int positionLocation =
        glGetUniformLocation(shaderProgram, "uiPosition");

    int frameStartLocation =
        glGetUniformLocation(shaderProgram, "frameStart");

    int frameHeightLocation =
        glGetUniformLocation(shaderProgram, "frameHeight");

    int atlasRowsLocation =
        glGetUniformLocation(shaderProgram, "atlasRows");

    int textureRowLocation =
        glGetUniformLocation(shaderProgram, "textureRow");

    int numberDigitLocation =
        glGetUniformLocation(shaderProgram, "numberDigit");

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(textureLocation, 0);

    // --------------------------------------------------------
    // Selected hotbar frame
    // --------------------------------------------------------

    glUniform1i(drawModeLocation, 0);
    glBindTexture(GL_TEXTURE_2D, hotbarTexture);

    glUniform2f(scaleLocation, 0.45f, 0.075f);
    glUniform2f(positionLocation, 0.0f, -0.85f);

    // ScrollWheel.png contains six frames stacked vertically.
    float frameHeight = 1.0f / 6.0f;
    float frameStart =
        static_cast<float>(selectedSlot) * frameHeight;

    glUniform1f(frameStartLocation, frameStart);
    glUniform1f(frameHeightLocation, frameHeight);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // --------------------------------------------------------
    // Item icons
    // --------------------------------------------------------

    glUniform1i(drawModeLocation, 1);
    glBindTexture(GL_TEXTURE_2D, itemAtlasTexture);

    glUniform1f(
        atlasRowsLocation,
        static_cast<float>(itemAtlasRows)
    );

    glUniform2f(scaleLocation, 0.055f, 0.065f);

    for (int slot = 0; slot < 6; slot++)
    {
        ItemType itemType = inventory.getItemTypeAtSlot(slot);

        if (itemType == ItemType::None)
        {
            continue;
        }

        Item item(itemType);

        glUniform1f(
            textureRowLocation,
            static_cast<float>(item.textureRow)
        );

        float slotX =
            -0.375f + static_cast<float>(slot) * 0.15f;

        glUniform2f(positionLocation, slotX, -0.85f);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // --------------------------------------------------------
    // Stack quantities
    // --------------------------------------------------------

    glUniform1i(drawModeLocation, 3);
    glBindTexture(GL_TEXTURE_2D, numberAtlasTexture);

    for (int slot = 0; slot < 6; slot++)
    {
        int amount = inventory.getAmountAtSlot(slot);

        // Hide quantities for empty slots and single items.
        if (amount <= 1)
        {
            continue;
        }

        float slotX =
            -0.375f + static_cast<float>(slot) * 0.15f;

        glUniform2f(scaleLocation, 0.018f, 0.030f);

        if (amount < 10)
        {
            glUniform1f(
                numberDigitLocation,
                static_cast<float>(amount)
            );

            glUniform2f(
                positionLocation,
                slotX + 0.040f,
                -0.885f
            );

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        else
        {
            int tensDigit = amount / 10;
            int onesDigit = amount % 10;

            glUniform1f(
                numberDigitLocation,
                static_cast<float>(tensDigit)
            );

            glUniform2f(
                positionLocation,
                slotX + 0.018f,
                -0.885f
            );

            glDrawArrays(GL_TRIANGLES, 0, 6);

            glUniform1f(
                numberDigitLocation,
                static_cast<float>(onesDigit)
            );

            glUniform2f(
                positionLocation,
                slotX + 0.048f,
                -0.885f
            );

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    glEnable(GL_DEPTH_TEST);
}


// ============================================================
// Draw the full inventory
//
// Bottom row: slots 0–5
// Top row: slots 6–11
// ============================================================

void UIRenderer::drawInventory(
    unsigned int inventoryTexture,
    unsigned int itemAtlasTexture,
    unsigned int numberAtlasTexture,
    const Inventory& inventory,
    int itemAtlasRows,
    float mouseX,
    float mouseY,
    int hoveredSlot
)
{
    glUseProgram(shaderProgram);
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(
        glGetUniformLocation(shaderProgram, "uiTexture"),
        0
    );

    const int mode =
        glGetUniformLocation(shaderProgram, "drawMode");

    const int scale =
        glGetUniformLocation(shaderProgram, "uiScale");

    const int position =
        glGetUniformLocation(shaderProgram, "uiPosition");

    const int row =
        glGetUniformLocation(shaderProgram, "textureRow");

    const int digit =
        glGetUniformLocation(shaderProgram, "numberDigit");

    glUniform1f(
        glGetUniformLocation(shaderProgram, "atlasRows"),
        static_cast<float>(itemAtlasRows)
    );

    // --------------------------------------------------------
    // Inventory background
    // --------------------------------------------------------

    glBindTexture(GL_TEXTURE_2D, inventoryTexture);
    glUniform1i(mode, 4);

    glUniform2f(
        scale,
        InventoryLayout::HALF_WIDTH,
        InventoryLayout::HALF_HEIGHT
    );

    glUniform2f(position, 0.0f, InventoryLayout::CENTER_Y);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // --------------------------------------------------------
    // Outline the slot under the mouse
    // --------------------------------------------------------

    if (hoveredSlot >= 0 && hoveredSlot < Inventory::SLOT_COUNT)
    {
        glUniform1i(mode, 2);

        const float x = InventoryLayout::slotX(hoveredSlot);
        const float y = InventoryLayout::slotY(hoveredSlot);

        const float half =
            InventoryLayout::CELL_SIZE * 0.5f - 0.003f;

        for (int side : {-1, 1})
        {
            // Top and bottom edges.
            glUniform2f(scale, half, 0.002f);
            glUniform2f(position, x, y + side * half);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Left and right edges.
            glUniform2f(scale, 0.002f, half);
            glUniform2f(position, x + side * half, y);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    // --------------------------------------------------------
    // Helper: draw one stack at any screen position
    // --------------------------------------------------------

    auto drawStack = [&](int slot, float x, float y)
        {
            const ItemType type = inventory.getItemTypeAtSlot(slot);
            const int amount = inventory.getAmountAtSlot(slot);

            if (type == ItemType::None || amount <= 0)
            {
                return;
            }

            const Item item(type);

            // Draw the item icon.
            glUniform1i(mode, 1);
            glBindTexture(GL_TEXTURE_2D, itemAtlasTexture);

            glUniform1f(row, static_cast<float>(item.textureRow));
            glUniform2f(scale, 0.055f, 0.065f);
            glUniform2f(position, x, y);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            if (amount <= 1)
            {
                return;
            }

            // Draw the quantity over the icon.
            glUniform1i(mode, 3);
            glBindTexture(GL_TEXTURE_2D, numberAtlasTexture);
            glUniform2f(scale, 0.018f, 0.030f);

            if (amount >= 10)
            {
                glUniform1f(digit, static_cast<float>(amount / 10));
                glUniform2f(position, x + 0.018f, y - 0.035f);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }

            glUniform1f(digit, static_cast<float>(amount % 10));

            glUniform2f(
                position,
                x + (amount >= 10 ? 0.048f : 0.040f),
                y - 0.035f
            );

            glDrawArrays(GL_TRIANGLES, 0, 6);
        };

    // --------------------------------------------------------
    // Draw stationary stacks
    // --------------------------------------------------------

    const int dragged = inventory.getDraggedSlot();

    for (int slot = 0; slot < Inventory::SLOT_COUNT; ++slot)
    {
        // The dragged stack is drawn at the mouse instead.
        if (slot == dragged)
        {
            continue;
        }

        drawStack(
            slot,
            InventoryLayout::slotX(slot),
            InventoryLayout::slotY(slot)
        );
    }

    // Draw the dragged stack last so it appears on top.
    if (dragged >= 0)
    {
        drawStack(dragged, mouseX, mouseY);
    }

    glEnable(GL_DEPTH_TEST);
}


// ============================================================
// Draw the crosshair
// ============================================================

void UIRenderer::drawCrosshair()
{
    glUseProgram(shaderProgram);
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(VAO);

    int scaleLocation =
        glGetUniformLocation(shaderProgram, "uiScale");

    int positionLocation =
        glGetUniformLocation(shaderProgram, "uiPosition");

    int drawModeLocation =
        glGetUniformLocation(shaderProgram, "drawMode");

    // Mode 2 draws solid white rectangles.
    glUniform1i(drawModeLocation, 2);

    // Vertical bar.
    glUniform2f(scaleLocation, 0.008f, 0.035f);
    glUniform2f(positionLocation, 0.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Horizontal bar.
    glUniform2f(scaleLocation, 0.025f, 0.010f);
    glUniform2f(positionLocation, 0.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glEnable(GL_DEPTH_TEST);
}

void UIRenderer::drawHand(
    unsigned int handTexture,
    unsigned int itemAtlasTexture,
    ItemType heldItem,
    int itemAtlasRows,
    int framebufferWidth,
    int framebufferHeight
)
{
    if (
        handTexture == 0 ||
        framebufferWidth <= 0 ||
        framebufferHeight <= 0
        )
    {
        return;
    }

    glUseProgram(shaderProgram);
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);

    glUniform1i(
        glGetUniformLocation(shaderProgram, "uiTexture"),
        0
    );

    int mode = glGetUniformLocation(shaderProgram, "drawMode");
    int scale = glGetUniformLocation(shaderProgram, "uiScale");
    int position = glGetUniformLocation(shaderProgram, "uiPosition");
    int row = glGetUniformLocation(shaderProgram, "textureRow");
    int rows = glGetUniformLocation(shaderProgram, "atlasRows");

    float halfHeight = 0.45f;

    float halfWidth =
        halfHeight *
        static_cast<float>(framebufferHeight) /
        static_cast<float>(framebufferWidth);

    float centerX = 1.0f - halfWidth;
    float centerY = -1.0f + halfHeight;

    glUniform1i(mode, 1);
    glUniform1f(row, 0.0f);
    glUniform1f(rows, 1.0f);

    glBindTexture(GL_TEXTURE_2D, handTexture);
    glUniform2f(scale, halfWidth, halfHeight);
    glUniform2f(position, centerX, centerY);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    if (
        heldItem != ItemType::None &&
        itemAtlasTexture != 0 &&
        itemAtlasRows > 0
        )
    {
        Item item(heldItem);

        if (
            item.textureRow >= 0 &&
            item.textureRow < itemAtlasRows
            )
        {
            glBindTexture(GL_TEXTURE_2D, itemAtlasTexture);

            glUniform1f(
                row,
                static_cast<float>(item.textureRow)
            );

            glUniform1f(
                rows,
                static_cast<float>(itemAtlasRows)
            );

            glUniform2f(
                position,
                centerX - halfWidth * 0.20f,
                centerY + halfHeight * 0.35f
            );

            glUniform2f(
                scale,
                halfWidth * 0.22f,
                halfHeight * 0.22f
            );

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    glEnable(GL_DEPTH_TEST);
}
