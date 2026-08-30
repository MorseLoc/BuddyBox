#pragma once

#include <glm/glm.hpp>


class Renderer
{
public:

    Renderer();


    void bindCube();


    // Draws one cube using a solid color.
    //
    // position = center of the cube
    // size     = width, height, depth
    // color    = red, green, blue
    // yaw      = rotation
    // opacity  = 1.0 solid, 0.0 invisible
    void drawColoredCube(
        const glm::vec3& position,
        const glm::vec3& size,
        const glm::vec3& color,
        float yaw = 0.0f,
        float opacity = 1.0f
    );


    void drawTexturedCube(
        const glm::vec3& position,
        const glm::vec3& size,
        unsigned int texture,
        int row,
        int rows,
        float yaw = 0.0f,
        bool useItemAtlas = false
    );

    // Draws a flat dropped-item sprite.
//
// Uses Itemdex instead of the normal
// six-column block texture atlas.
    void drawDroppedItem(
        const glm::vec3& position,
        unsigned int texture,
        int row,
        int rows,
        float yaw
    );

    unsigned int getShaderProgram() const;


private:

    unsigned int VBO;

    unsigned int VAO;

    unsigned int shaderProgram;
};