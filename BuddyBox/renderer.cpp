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
    // Normal Renderer cubes do NOT contain attribute 3.
    //
    // Attribute 3 is only used by ChunkMesh.
    //
    // We disable it here so the normal Renderer keeps using
    // the uniform textureRow instead.
    // --------------------------------------------------------

    glDisableVertexAttribArray(
        3
    );


    // ========================================================
    // 4. Vertex shader
    // ========================================================

    const char* vertexShaderSource =
        "#version 330 core\n"

        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec2 textureCoordinate;\n"
        "layout (location = 2) in float faceIndex;\n"

        // Chunk meshes use this.
        "layout (location = 3) in float vertexTextureRow;\n"


        "out vec2 texCoord;\n"


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


        "void main()\n"
        "{\n"

        // Move vertex into screen space.
        "    gl_Position = projection * view * model * vec4(position, 1.0);\n"


        // ----------------------------------------------------
        // Horizontal atlas position
        // ----------------------------------------------------

        "    float atlasU = (faceIndex + textureCoordinate.x) / 6.0;\n"


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


        "out vec4 finalColor;\n"


        "uniform sampler2D blockTexture;\n"


        "uniform bool useSolidColor;\n"

        "uniform vec3 solidColor;\n"

        "uniform float opacity;\n"


        "void main()\n"
        "{\n"


        "    if (useSolidColor)\n"
        "    {\n"

        "        finalColor = vec4(solidColor, opacity);\n"

        "    }\n"
        "    else\n"
        "    {\n"

        "        finalColor = texture(blockTexture, texCoord);\n"

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
    float yaw
)
{
    glUseProgram(
        shaderProgram
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


    // IMPORTANT:
    //
    // This is an old-style cube, not a ChunkMesh.
    //
    // Therefore its texture row comes from the
    // textureRow uniform.

    int useVertexTextureRowLocation =
        glGetUniformLocation(
            shaderProgram,
            "useVertexTextureRow"
        );


    glUniform1i(
        useVertexTextureRowLocation,
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
}