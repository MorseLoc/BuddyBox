#include "ChunkMesh.h"

#include "World.h"

#include <glad/glad.h>

#include <tuple>


// ============================================================
// Constructor
// ============================================================

ChunkMesh::ChunkMesh()
{
    VAO = 0;
    VBO = 0;

    vertexCount = 0;


    glGenVertexArrays(
        1,
        &VAO
    );


    glGenBuffers(
        1,
        &VBO
    );
}


// ============================================================
// Destructor
// ============================================================

ChunkMesh::~ChunkMesh()
{
    if (VBO != 0)
    {
        glDeleteBuffers(
            1,
            &VBO
        );
    }


    if (VAO != 0)
    {
        glDeleteVertexArrays(
            1,
            &VAO
        );
    }
}


// ============================================================
// Build chunk
// ============================================================

void ChunkMesh::build(
    const World& world,
    int chunkX,
    int chunkY,
    int chunkZ
)
{
    std::vector<float> vertices;


    // Find this chunk's world-space starting position.

    int startX =
        chunkX * CHUNK_SIZE;

    int startY =
        chunkY * CHUNK_SIZE;

    int startZ =
        chunkZ * CHUNK_SIZE;


    // Check every possible block position inside the chunk.

    for (
        int x = startX;
        x < startX + CHUNK_SIZE;
        x++
        )
    {
        for (
            int y = startY;
            y < startY + CHUNK_SIZE;
            y++
            )
        {
            for (
                int z = startZ;
                z < startZ + CHUNK_SIZE;
                z++
                )
            {
                auto blockIterator =
                    world.blocks.find(
                        std::make_tuple(
                            x,
                            y,
                            z
                        )
                    );


                // No block here.
                if (
                    blockIterator ==
                    world.blocks.end()
                    )
                {
                    continue;
                }


                const Block& block =
                    blockIterator->second;


                // Front
                if (!world.isSolidAt(x, y, z + 1))
                {
                    addFace(
                        vertices,
                        x,
                        y,
                        z,
                        1,
                        block.textureRow
                    );
                }


                // Back
                if (!world.isSolidAt(x, y, z - 1))
                {
                    addFace(
                        vertices,
                        x,
                        y,
                        z,
                        2,
                        block.textureRow
                    );
                }


                // Left
                if (!world.isSolidAt(x - 1, y, z))
                {
                    addFace(
                        vertices,
                        x,
                        y,
                        z,
                        3,
                        block.textureRow
                    );
                }


                // Right
                if (!world.isSolidAt(x + 1, y, z))
                {
                    addFace(
                        vertices,
                        x,
                        y,
                        z,
                        4,
                        block.textureRow
                    );
                }


                // Top
                if (!world.isSolidAt(x, y + 1, z))
                {
                    addFace(
                        vertices,
                        x,
                        y,
                        z,
                        0,
                        block.textureRow
                    );
                }


                // Bottom
                if (!world.isSolidAt(x, y - 1, z))
                {
                    addFace(
                        vertices,
                        x,
                        y,
                        z,
                        5,
                        block.textureRow
                    );
                }
            }
        }
    }


    // Each vertex contains:
    //
    // XYZ
    // UV
    // face
    // texture row
    //
    // = 7 floats

    vertexCount =
        static_cast<int>(
            vertices.size() / 7
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
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW
    );


    // Position

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        7 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(
        0
    );


    // Texture UV

    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        7 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(
        1
    );


    // Cube face

    glVertexAttribPointer(
        2,
        1,
        GL_FLOAT,
        GL_FALSE,
        7 * sizeof(float),
        (void*)(5 * sizeof(float))
    );

    glEnableVertexAttribArray(
        2
    );


    // Block texture row

    glVertexAttribPointer(
        3,
        1,
        GL_FLOAT,
        GL_FALSE,
        7 * sizeof(float),
        (void*)(6 * sizeof(float))
    );

    glEnableVertexAttribArray(
        3
    );
}


// ============================================================
// Draw chunk
// ============================================================

void ChunkMesh::draw() const
{
    if (vertexCount == 0)
    {
        return;
    }


    glBindVertexArray(
        VAO
    );


    glDrawArrays(
        GL_TRIANGLES,
        0,
        vertexCount
    );
}

// ============================================================
// Add face
//
// Adds one visible block face to the chunk mesh.
//
// Each vertex contains:
//
// X, Y, Z
// U, V
// face number
// texture row
//
// = 7 floats per vertex
// ============================================================

void ChunkMesh::addFace(
    std::vector<float>& vertices,
    int x,
    int y,
    int z,
    int face,
    int textureRow
)
{
    // --------------------------------------------------------
    // The six faces of a cube
    //
    // Each face contains:
    // 2 triangles
    // 6 vertices
    //
    // Each vertex contains:
    // X, Y, Z, U, V
    // --------------------------------------------------------

    static const float faces[6][30] =
    {
        // ----------------------------------------------------
        // 0 = Top
        // ----------------------------------------------------
        {
            -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,   1.0f, 0.0f,

             0.5f,  0.5f,  0.5f,   1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,   0.0f, 1.0f
        },


        // ----------------------------------------------------
        // 1 = Front
        // ----------------------------------------------------
        {
            -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,   1.0f, 1.0f,

             0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,   0.0f, 0.0f
        },


        // ----------------------------------------------------
        // 2 = Back
        // ----------------------------------------------------
        {
            -0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,   0.0f, 1.0f,

             0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,   1.0f, 0.0f
        },


        // ----------------------------------------------------
        // 3 = Left
        // ----------------------------------------------------
        {
            -0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,   1.0f, 1.0f
        },


        // ----------------------------------------------------
        // 4 = Right
        // ----------------------------------------------------
        {
             0.5f,  0.5f,  0.5f,   0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,   1.0f, 0.0f,

             0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,   0.0f, 1.0f
        },


        // ----------------------------------------------------
        // 5 = Bottom
        // ----------------------------------------------------
        {
            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,   1.0f, 1.0f,

             0.5f, -0.5f,  0.5f,   1.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,   0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f
        }
    };


    // --------------------------------------------------------
    // Copy the selected face into our giant chunk vertex list
    // --------------------------------------------------------

    for (int vertex = 0; vertex < 6; vertex++)
    {
        int index =
            vertex * 5;


        // Position
        //
        // Add the block's world position here.
        // This means we no longer need a separate model matrix
        // for every block.

        vertices.push_back(
            faces[face][index] +
            static_cast<float>(x)
        );

        vertices.push_back(
            faces[face][index + 1] +
            static_cast<float>(y)
        );

        vertices.push_back(
            faces[face][index + 2] +
            static_cast<float>(z)
        );


        // Texture coordinates

        vertices.push_back(
            faces[face][index + 3]
        );

        vertices.push_back(
            faces[face][index + 4]
        );


        // Which cube face this is.

        vertices.push_back(
            static_cast<float>(face)
        );


        // Which block texture row this vertex uses.

        vertices.push_back(
            static_cast<float>(textureRow)
        );
    }
}