#pragma once

#include <glm/glm.hpp>

// ============================================================
// Renderer
//
// Owns the OpenGL objects used to draw BuddyBox blocks.
//
// Renderer.cpp is responsible for:
// - Creating the cube vertex data
// - Creating the VBO and VAO
// - Creating and linking the block shaders
//
// Other parts of the game can:
// - Bind the cube for drawing
// - Access the finished shader program
// ============================================================

class Renderer
{
public:
    // Creates the renderer and sets up its OpenGL resources.
    Renderer();


    // Activates the cube VAO so OpenGL can draw block geometry.
    void bindCube();

    // Draws one cube using a solid color.
    //
    // position = center of the cube
    // size     = width, height, depth
    // color    = red, green, blue
    void drawColoredCube(
        const glm::vec3& position,
        const glm::vec3& size,
        const glm::vec3& color
    );

    // Returns the shader program used to draw blocks.
    unsigned int getShaderProgram() const;


private:
    // Vertex Buffer Object.
    //
    // Stores the cube's raw vertex data in GPU memory.
    unsigned int VBO;


    // Vertex Array Object.
    //
    // Stores the layout of the cube's vertex data.
    unsigned int VAO;


    // Combined vertex + fragment shader program.
    unsigned int shaderProgram;
};