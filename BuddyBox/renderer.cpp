#include "Renderer.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ============================================================
// Renderer constructor
//
// Creates everything needed to draw BuddyBox blocks:
//
// - Cube vertex data
// - Vertex Buffer Object (VBO)
// - Vertex Array Object (VAO)
// - Vertex shader
// - Fragment shader
// - Final shader program
// ============================================================

Renderer::Renderer()
{
    // ========================================================
    // 1. Cube vertex data
    // ========================================================

    // Each vertex stores 6 values:
    //
    // X, Y, Z = position
    // U, V    = texture coordinates
    // Face    = which cube face this vertex belongs to
    //
    // A cube has:
    //
    // 6 faces
    // × 2 triangles per face
    // × 3 vertices per triangle
    //
    // = 36 total vertices
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
        // Front face
        // ----------------------------------------------------

        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,

         0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,


        // ----------------------------------------------------
        // Back face
        // ----------------------------------------------------

        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 2.0f,
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 2.0f,
         0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 2.0f,

         0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 2.0f,
         0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 2.0f,
        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 2.0f,


        // ----------------------------------------------------
        // Left face
        // ----------------------------------------------------

        -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 3.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 3.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 3.0f,

        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 3.0f,
        -0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 3.0f,
        -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 3.0f,


        // ----------------------------------------------------
        // Right face
        // ----------------------------------------------------

         0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 4.0f,
         0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 4.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 4.0f,

         0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 4.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 4.0f,
         0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 4.0f,


         // ----------------------------------------------------
         // Top face
         // ----------------------------------------------------

         -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 0.0f,
          0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,

          0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,
          0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,


         // ----------------------------------------------------
         // Bottom face
         // ----------------------------------------------------

         -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 5.0f,
          0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 5.0f,
          0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 5.0f,

          0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 5.0f,
         -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 5.0f,
         -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 5.0f
    };


    // ========================================================
    // 2. Create the Vertex Buffer Object
    // ========================================================

    // The VBO stores the cube's raw vertex data
    // inside graphics-card memory.
    glGenBuffers(
        1,
        &VBO
    );


    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );


    // Copy cubeVertices from normal RAM into GPU memory.
    //
    // GL_STATIC_DRAW means this data is not expected
    // to change frequently.
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(cubeVertices),
        cubeVertices,
        GL_STATIC_DRAW
    );


    // ========================================================
    // 3. Create the Vertex Array Object
    // ========================================================

    // The VAO remembers how each vertex is organized.
    glGenVertexArrays(
        1,
        &VAO
    );


    glBindVertexArray(
        VAO
    );


    // Attach our cube VBO to this VAO.
    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );


    // --------------------------------------------------------
    // Vertex attribute 0: position
    // --------------------------------------------------------

    // Reads:
    //
    // X, Y, Z
    //
    // from each group of 6 floats.
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
    // Vertex attribute 1: texture coordinates
    // --------------------------------------------------------

    // Reads:
    //
    // U, V
    //
    // starting after the first 3 position floats.
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
    // Vertex attribute 2: cube face
    // --------------------------------------------------------

    // Reads the final float in each vertex.
    //
    // The shader uses this value to choose which horizontal
    // section of the texture atlas belongs to this cube face.
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


    // ========================================================
    // 4. Vertex shader
    // ========================================================

    // The vertex shader runs once for every vertex.
    //
    // It:
    // - Transforms the vertex into camera/screen space
    // - Calculates the correct texture-atlas coordinates

    const char* vertexShaderSource =
        "#version 330 core\n"

        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec2 textureCoordinate;\n"
        "layout (location = 2) in float faceIndex;\n"

        "out vec2 texCoord;\n"

        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"

        "uniform float textureRow;\n"
        "uniform float atlasRows;\n"

        "void main()\n"
        "{\n"

        // Convert the cube vertex from model space
        // into its final position on screen.
        "    gl_Position = projection * view * model * vec4(position, 1.0);\n"

        // The atlas contains the six cube faces horizontally.
        //
        // faceIndex chooses which face section to use.
        "    float atlasU = (faceIndex + textureCoordinate.x) / 6.0;\n"

        // textureRow chooses which block type to use vertically.
        "    float atlasV = (textureRow + (1.0 - textureCoordinate.y)) / atlasRows;\n"

        "    texCoord = vec2(atlasU, atlasV);\n"

        "}\n";


    // Create the vertex shader object on the GPU.
    unsigned int vertexShader =
        glCreateShader(
            GL_VERTEX_SHADER
        );


    // Give OpenGL our shader source code.
    glShaderSource(
        vertexShader,
        1,
        &vertexShaderSource,
        nullptr
    );


    // Compile the shader.
    glCompileShader(
        vertexShader
    );


    // ========================================================
    // 5. Fragment shader
    // ========================================================

    // The fragment shader runs for each pixel produced
    // while drawing the cube.
    //
    // It reads the block texture and chooses the
    // final pixel color.

    const char* fragmentShaderSource =
        "#version 330 core\n"

        "in vec2 texCoord;\n"

        "out vec4 finalColor;\n"

        "uniform sampler2D blockTexture;\n"

        "uniform bool useSolidColor;\n"
        "uniform vec3 solidColor;\n"

        "void main()\n"
        "{\n"

        "    if (useSolidColor)\n"
        "    {\n"
        "        finalColor = vec4(solidColor, 1.0);\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        finalColor = texture(blockTexture, texCoord);\n"
        "    }\n"

        "}\n";

    // Create the fragment shader object.
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
    // 6. Create the shader program
    // ========================================================

    // A shader program combines the vertex shader
    // and fragment shader into one rendering pipeline.
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


    // Combine the shaders into the finished program.
    glLinkProgram(
        shaderProgram
    );


    // Once the program has been linked successfully,
    // the individual shader objects are no longer needed.
    glDeleteShader(
        vertexShader
    );


    glDeleteShader(
        fragmentShader
    );
}


// ============================================================
// Bind cube
//
// Activates the cube VAO so OpenGL knows which vertex data
// should be used by the next drawing command.
// ============================================================

void Renderer::bindCube()
{
    glBindVertexArray(
        VAO
    );
}


// ============================================================
// Get shader program
//
// Gives other parts of the game access to the block shader.
// ============================================================

unsigned int Renderer::getShaderProgram() const
{
    return shaderProgram;
}

void Renderer::drawColoredCube(
    const glm::vec3& position,
    const glm::vec3& size,
    const glm::vec3& color,
    float yaw
)
{
    glUseProgram(
        shaderProgram
    );


    // Tell the shader to ignore textures
    // and use a plain color instead.
    int useSolidColorLocation =
        glGetUniformLocation(
            shaderProgram,
            "useSolidColor"
        );

    glUniform1i(
        useSolidColorLocation,
        1
    );


    // Send the requested RGB color.
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


    // Start with a normal model matrix.
    glm::mat4 model =
        glm::mat4(1.0f);


    // Move the cube into position.
    model =
        glm::translate(
            model,
            position
        );

	// Rotate the cube around the Y axis.
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

    // Stretch the normal 1 x 1 x 1 cube
    // into the requested size.
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


    // Use the cube geometry already owned by Renderer.
    glBindVertexArray(
        VAO
    );


    glDrawArrays(
        GL_TRIANGLES,
        0,
        36
    );


    // IMPORTANT:
    // Turn solid-color mode back off so normal
    // world blocks keep using their textures.
    glUniform1i(
        useSolidColorLocation,
        0
    );
}

// ============================================================
// Draw textured cube
//
// Draws one cube using a texture atlas.
//
// The texture atlas is expected to contain:
// - 6 cube faces horizontally
// - One or more texture rows vertically
//
// This is used for textured NPC body parts.
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
    // Use the normal world shader.
    glUseProgram(
        shaderProgram
    );


    // --------------------------------------------------------
    // Use texture mode
    // --------------------------------------------------------

    // Make sure the shader does NOT use
    // the solid-color mode.
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
    // Bind texture
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
    // Tell shader which atlas row to use
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
    // Create model matrix
    // --------------------------------------------------------

    glm::mat4 model =
        glm::mat4(1.0f);


    // Move the cube into position.
    model =
        glm::translate(
            model,
            position
        );


    // Rotate the cube around the vertical Y axis.
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


    // Stretch the normal cube into the
    // requested body-part dimensions.
    model =
        glm::scale(
            model,
            size
        );


    // --------------------------------------------------------
    // Send model matrix to shader
    // --------------------------------------------------------

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
    // Draw cube
    // --------------------------------------------------------

    glBindVertexArray(
        VAO
    );


    glDrawArrays(
        GL_TRIANGLES,
        0,
        36
    );
}