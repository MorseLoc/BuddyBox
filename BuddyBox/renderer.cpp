#include "Renderer.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// ============================================================
// Renderer constructor
//
// Creates:
//
// - Cube vertex data
// - VBO
// - VAO
// - Vertex shader
// - Fragment shader
// - Shader program
// ============================================================

Renderer::Renderer()
{
    // ========================================================
    // 1. Cube vertex data
    // ========================================================

    // Each normal cube vertex stores:
    //
    // X, Y, Z
    // U, V
    // face index
    //
    // = 6 floats
    //
    // Face numbers:
    //
    // 0 = top
    // 1 = front
    // 2 = back
    // 3 = left
    // 4 = right
    // 5 = bottom

    float cubeVertices[] =
    {
        // ----------------------------------------------------
        // Front
        // ----------------------------------------------------

        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,

         0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,


        // ----------------------------------------------------
        // Back
        // ----------------------------------------------------

        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 2.0f,
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 2.0f,
         0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 2.0f,

         0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 2.0f,
         0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 2.0f,
        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 2.0f,


        // ----------------------------------------------------
        // Left
        // ----------------------------------------------------

        -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 3.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 3.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 3.0f,

        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 3.0f,
        -0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 3.0f,
        -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 3.0f,


        // ----------------------------------------------------
        // Right
        // ----------------------------------------------------

         0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 4.0f,
         0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 4.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 4.0f,

         0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 4.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 4.0f,
         0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 4.0f,


         // ----------------------------------------------------
         // Top
         // ----------------------------------------------------

         -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 0.0f,
          0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,

          0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,
          0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,


         // ----------------------------------------------------
         // Bottom
         // ----------------------------------------------------

         -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 5.0f,
          0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 5.0f,
          0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 5.0f,

          0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 5.0f,
         -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 5.0f,
         -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 5.0f
    };


    // ========================================================
    // 2. Create VBO
    // ========================================================

    glGenBuffers(
        1,
        &VBO
    );


    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );


    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(cubeVertices),
        cubeVertices,
        GL_STATIC_DRAW
    );


    // ========================================================
    // 3. Create VAO
    // ========================================================

    glGenVertexArrays(
        1,
        &VAO
    );


    glBindVertexArray(
        VAO
    );


    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );


    // --------------------------------------------------------
    // Attribute 0
    //
    // Position:
    //
    // X, Y, Z
    // --------------------------------------------------------

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );


    glEnableVertexAttribArray(
        0
    );


    // --------------------------------------------------------
    // Attribute 1
    //
    // Texture coordinates:
    //
    // U, V
    // --------------------------------------------------------

    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );


    glEnableVertexAttribArray(
        1
    );


    // --------------------------------------------------------
    // Attribute 2
    //
    // Cube face index
    // --------------------------------------------------------

    glVertexAttribPointer(
        2,
        1,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(5 * sizeof(float))
    );


    glEnableVertexAttribArray(
        2
    );


    // --------------------------------------------------------
    // IMPORTANT
    //
    // Normal Renderer cubes do NOT contain attributes 3 or 4.
    //
    // Attribute 3 = ChunkMesh texture row
    // Attribute 4 = ChunkMesh light level
    //
    // They stay disabled on this VAO.
    // --------------------------------------------------------

    glDisableVertexAttribArray(
        3
    );


    glDisableVertexAttribArray(
        4
    );


    // ========================================================
    // 4. Vertex shader
    // ========================================================

    const char* vertexShaderSource =
        "#version 330 core\n"

        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec2 textureCoordinate;\n"
        "layout (location = 2) in float faceIndex;\n"

        // Chunk meshes use these.
        "layout (location = 3) in float vertexTextureRow;\n"
        "layout (location = 4) in float vertexLight;\n"


        "out vec2 texCoord;\n"
        "out float lightLevel;\n"


        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"


        // Old cube rendering uses this.
        "uniform float textureRow;\n"

        // Number of vertical atlas rows.
        "uniform float atlasRows;\n"

        // false = use textureRow uniform
        // true  = use vertexTextureRow attribute
        "uniform bool useVertexTextureRow;\n"

        // false = normal cube is fully lit
        // true  = use ChunkMesh light attribute
        "uniform bool useVertexLight;\n"

        // true when using the one-column Itemdex texture.
        "uniform bool useItemAtlas;\n"


        "void main()\n"
        "{\n"

        // Move vertex into screen space.
        "    gl_Position = projection * view * model * vec4(position, 1.0);\n"


        // ----------------------------------------------------
        // Horizontal atlas position
        //
        // Blocks use six horizontal face columns.
        //
        // Items use one horizontal column.
        // ----------------------------------------------------

        "    float atlasU;\n"

        "    if (useItemAtlas)\n"
        "    {\n"
        "        atlasU = textureCoordinate.x;\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        atlasU = (faceIndex + textureCoordinate.x) / 6.0;\n"
        "    }\n"


        // ----------------------------------------------------
        // Choose block texture row
        // ----------------------------------------------------

        "    float activeTextureRow;\n"


        "    if (useVertexTextureRow)\n"
        "    {\n"

        // Chunk mesh:
        // texture row comes directly from the vertex.
        "        activeTextureRow = vertexTextureRow;\n"

        "    }\n"
        "    else\n"
        "    {\n"

        // Normal cube:
        // texture row comes from the old uniform.
        "        activeTextureRow = textureRow;\n"

        "    }\n"


        // ----------------------------------------------------
        // Vertical atlas position
        // ----------------------------------------------------

        "    float atlasV = (activeTextureRow + (1.0 - textureCoordinate.y)) / atlasRows;\n"


        "    texCoord = vec2(atlasU, atlasV);\n"


        // ----------------------------------------------------
        // Lighting
        //
        // ChunkMesh stores values from 0 to 15.
        //
        // Convert that to 0.0 to 1.0.
        //
        // Normal Renderer cubes do not contain lighting data,
        // so they use full brightness instead.
        // ----------------------------------------------------

        "    if (useVertexLight)\n"
        "    {\n"
        "        lightLevel = vertexLight / 15.0;\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        lightLevel = 1.0;\n"
        "    }\n"


        "}\n";


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


    // ========================================================
    // 5. Fragment shader
    // ========================================================

    const char* fragmentShaderSource =
        "#version 330 core\n"

        "in vec2 texCoord;\n"
        "in float lightLevel;\n"


        "out vec4 finalColor;\n"


        "uniform sampler2D blockTexture;\n"


        "uniform bool useSolidColor;\n"

        "uniform vec3 solidColor;\n"

        "uniform float opacity;\n"


        "void main()\n"
        "{\n"


        "    if (useSolidColor)\n"
        "    {\n"

        // Colored overlays and debug cubes currently ignore
        // world lighting.
        "        finalColor = vec4(solidColor, opacity);\n"

        "    }\n"
        "    else\n"
        "    {\n"

        // Read the normal texture color.
        "        vec4 textureColor = texture(blockTexture, texCoord);\n"


        // ----------------------------------------------------
        // Lighting brightness
        //
        // We leave a small amount of ambient brightness
        // even when lightLevel is zero.
        //
        // 0 light  -> 15% brightness
        // 15 light -> 100% brightness
        // ----------------------------------------------------

        "        float brightness = 0.15 + (lightLevel * 0.85);\n"


        "        finalColor = vec4(\n"
        "            textureColor.rgb * brightness,\n"
        "            textureColor.a\n"
        "        );\n"

        "    }\n"


        "}\n";


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


    glDeleteShader(
        vertexShader
    );


    glDeleteShader(
        fragmentShader
    );
}


// ============================================================
// Bind cube
// ============================================================

void Renderer::bindCube()
{
    glBindVertexArray(
        VAO
    );
}


// ============================================================
// Get shader program
// ============================================================

unsigned int Renderer::getShaderProgram() const
{
    return shaderProgram;
}


// ============================================================
// Draw colored cube
// ============================================================

void Renderer::drawColoredCube(
    const glm::vec3& position,
    const glm::vec3& size,
    const glm::vec3& color,
    float yaw,
    float opacity
)
{
    glUseProgram(
        shaderProgram
    );


    // --------------------------------------------------------
    // Colored cubes do not use chunk texture rows.
    // --------------------------------------------------------

    int useVertexTextureRowLocation =
        glGetUniformLocation(
            shaderProgram,
            "useVertexTextureRow"
        );


    glUniform1i(
        useVertexTextureRowLocation,
        0
    );


    // --------------------------------------------------------
    // Colored cubes do not use chunk lighting.
    // --------------------------------------------------------

    int useVertexLightLocation =
        glGetUniformLocation(
            shaderProgram,
            "useVertexLight"
        );


    glUniform1i(
        useVertexLightLocation,
        0
    );


    // --------------------------------------------------------
    // Use solid color
    // --------------------------------------------------------

    int useSolidColorLocation =
        glGetUniformLocation(
            shaderProgram,
            "useSolidColor"
        );


    glUniform1i(
        useSolidColorLocation,
        1
    );


    int solidColorLocation =
        glGetUniformLocation(
            shaderProgram,
            "solidColor"
        );


    glUniform3f(
        solidColorLocation,
        color.r,
        color.g,
        color.b
    );


    // --------------------------------------------------------
    // Opacity
    // --------------------------------------------------------

    int opacityLocation =
        glGetUniformLocation(
            shaderProgram,
            "opacity"
        );


    glUniform1f(
        opacityLocation,
        opacity
    );


    // ========================================================
    // Model matrix
    // ========================================================

    glm::mat4 model =
        glm::mat4(1.0f);


    model =
        glm::translate(
            model,
            position
        );


    model =
        glm::rotate(
            model,
            glm::radians(yaw),
            glm::vec3(
                0.0f,
                1.0f,
                0.0f
            )
        );


    model =
        glm::scale(
            model,
            size
        );


    int modelLocation =
        glGetUniformLocation(
            shaderProgram,
            "model"
        );


    glUniformMatrix4fv(
        modelLocation,
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );


    // ========================================================
    // Draw
    // ========================================================

    glBindVertexArray(
        VAO
    );


    glDrawArrays(
        GL_TRIANGLES,
        0,
        36
    );


    // Turn solid color mode back off.

    glUniform1i(
        useSolidColorLocation,
        0
    );
}


// ============================================================
// Draw textured cube
//
// Used by things such as textured NPC body parts.
// ============================================================

void Renderer::drawTexturedCube(
    const glm::vec3& position,
    const glm::vec3& size,
    unsigned int texture,
    int row,
    int rows,
    float yaw,
    bool useItemAtlas
)
{
    glUseProgram(
        shaderProgram
    );


    // ========================================================
    // Item atlas mode
    //
    // false = normal 6-column block/NPC texture
    // true  = one-column Itemdex texture
    // ========================================================

    int useItemAtlasLocation =
        glGetUniformLocation(
            shaderProgram,
            "useItemAtlas"
        );


    glUniform1i(
        useItemAtlasLocation,
        useItemAtlas
    );


    // ========================================================
    // Normal cube texture mode
    // ========================================================

    int useSolidColorLocation =
        glGetUniformLocation(
            shaderProgram,
            "useSolidColor"
        );


    glUniform1i(
        useSolidColorLocation,
        0
    );


    // --------------------------------------------------------
    // This is an old-style cube, not a ChunkMesh.
    //
    // Its texture row comes from the textureRow uniform.
    // --------------------------------------------------------

    int useVertexTextureRowLocation =
        glGetUniformLocation(
            shaderProgram,
            "useVertexTextureRow"
        );


    glUniform1i(
        useVertexTextureRowLocation,
        0
    );


    // --------------------------------------------------------
    // Normal textured cubes do not contain
    // the ChunkMesh light attribute.
    //
    // Keep them fully lit.
    // --------------------------------------------------------

    int useVertexLightLocation =
        glGetUniformLocation(
            shaderProgram,
            "useVertexLight"
        );


    glUniform1i(
        useVertexLightLocation,
        0
    );


    // ========================================================
    // Bind texture
    // ========================================================

    glActiveTexture(
        GL_TEXTURE0
    );


    glBindTexture(
        GL_TEXTURE_2D,
        texture
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


    // ========================================================
    // Texture atlas settings
    // ========================================================

    int textureRowLocation =
        glGetUniformLocation(
            shaderProgram,
            "textureRow"
        );


    glUniform1f(
        textureRowLocation,
        static_cast<float>(row)
    );


    int atlasRowsLocation =
        glGetUniformLocation(
            shaderProgram,
            "atlasRows"
        );


    glUniform1f(
        atlasRowsLocation,
        static_cast<float>(rows)
    );


    // ========================================================
    // Model matrix
    // ========================================================

    glm::mat4 model =
        glm::mat4(1.0f);


    model =
        glm::translate(
            model,
            position
        );


    model =
        glm::rotate(
            model,
            glm::radians(yaw),
            glm::vec3(
                0.0f,
                1.0f,
                0.0f
            )
        );


    model =
        glm::scale(
            model,
            size
        );


    int modelLocation =
        glGetUniformLocation(
            shaderProgram,
            "model"
        );


    glUniformMatrix4fv(
        modelLocation,
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );


    // ========================================================
    // Draw cube
    // ========================================================

    glBindVertexArray(
        VAO
    );


    glDrawArrays(
        GL_TRIANGLES,
        0,
        36
    );


    // --------------------------------------------------------
    // Restore normal atlas mode
    //
    // Dropped items use the one-column Itemdex.
    //
    // Everything else in the world uses the normal
    // six-column texture atlas, so always reset this
    // when this draw call is finished.
    // --------------------------------------------------------

    glUniform1i(
        useItemAtlasLocation,
        0
    );
}


// ============================================================
// Draw dropped item
//
// Draws only the FRONT face of the cube geometry.
//
// Because the first 6 vertices in our cube VAO
// make one rectangle, we can reuse them as a
// flat item sprite.
// ============================================================

void Renderer::drawDroppedItem(
    const glm::vec3& position,
    unsigned int texture,
    int row,
    int rows,
    float yaw
)
{
    glUseProgram(
        shaderProgram
    );


    // --------------------------------------------------------
    // Use textured rendering
    // --------------------------------------------------------

    int useSolidColorLocation =
        glGetUniformLocation(
            shaderProgram,
            "useSolidColor"
        );


    glUniform1i(
        useSolidColorLocation,
        0
    );


    // --------------------------------------------------------
    // We are supplying one texture row manually.
    // --------------------------------------------------------

    int useVertexTextureRowLocation =
        glGetUniformLocation(
            shaderProgram,
            "useVertexTextureRow"
        );


    glUniform1i(
        useVertexTextureRowLocation,
        0
    );


    // --------------------------------------------------------
    // Dropped items do not use ChunkMesh lighting.
    //
    // Keep them fully lit for now.
    // --------------------------------------------------------

    int useVertexLightLocation =
        glGetUniformLocation(
            shaderProgram,
            "useVertexLight"
        );


    glUniform1i(
        useVertexLightLocation,
        0
    );


    // --------------------------------------------------------
    // Tell the shader this is Itemdex.
    //
    // Itemdex has ONE horizontal column.
    // --------------------------------------------------------

    int useItemAtlasLocation =
        glGetUniformLocation(
            shaderProgram,
            "useItemAtlas"
        );


    glUniform1i(
        useItemAtlasLocation,
        1
    );


    // --------------------------------------------------------
    // Bind Itemdex texture
    // --------------------------------------------------------

    glActiveTexture(
        GL_TEXTURE0
    );


    glBindTexture(
        GL_TEXTURE_2D,
        texture
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


    // --------------------------------------------------------
    // Choose the item's row
    // --------------------------------------------------------

    int textureRowLocation =
        glGetUniformLocation(
            shaderProgram,
            "textureRow"
        );


    glUniform1f(
        textureRowLocation,
        static_cast<float>(row)
    );


    int atlasRowsLocation =
        glGetUniformLocation(
            shaderProgram,
            "atlasRows"
        );


    glUniform1f(
        atlasRowsLocation,
        static_cast<float>(rows)
    );


    // --------------------------------------------------------
    // Build model matrix
    // --------------------------------------------------------

    glm::mat4 model =
        glm::mat4(1.0f);


    // Move sprite into the world.

    model =
        glm::translate(
            model,
            position
        );


    // Rotate around the Y axis.

    model =
        glm::rotate(
            model,
            glm::radians(yaw),
            glm::vec3(
                0.0f,
                1.0f,
                0.0f
            )
        );


    // Dropped items are smaller than blocks.

    model =
        glm::scale(
            model,
            glm::vec3(
                0.5f,
                0.5f,
                0.5f
            )
        );


    int modelLocation =
        glGetUniformLocation(
            shaderProgram,
            "model"
        );


    glUniformMatrix4fv(
        modelLocation,
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );


    // --------------------------------------------------------
    // Draw ONE face
    //
    // Vertices 0 through 5 are the front
    // rectangle of our existing cube.
    // --------------------------------------------------------

    glBindVertexArray(
        VAO
    );


    glDrawArrays(
        GL_TRIANGLES,
        0,
        6
    );


    // --------------------------------------------------------
    // Return the shader to normal block-atlas mode.
    //
    // Otherwise Itemdex mode leaks into the world.
    // --------------------------------------------------------

    glUniform1i(
        useItemAtlasLocation,
        0
    );
}