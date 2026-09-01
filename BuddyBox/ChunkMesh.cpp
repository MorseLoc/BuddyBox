#include "ChunkMesh.h"

#include "World.h"
#include "lighting.h"

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
//
// Builds all visible block faces inside this chunk.
//
// Lighting is sampled from the AIR CELL
// directly beside each visible block face.
//
// This means one block can have different lighting
// on different sides.
// ============================================================

void ChunkMesh::build(
    const World& world,
    const Lighting& lighting,
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


    // Check every possible block position
    // inside the chunk.

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


                // ------------------------------------------------
                // Front
                //
                // Sample the air cell at:
                //
                // x, y, z + 1
                // ------------------------------------------------

                if (
                    !world.isSolidAt(
                        x,
                        y,
                        z + 1
                    )
                    )
                {
                    int skyLight =
                        lighting.getSkyLight(
                            x,
                            y,
                            z + 1
                        );


                    int blockLight =
                        lighting.getBlockLight(
                            x,
                            y,
                            z + 1
                        );


                    addFace(
                        vertices,
                        x,
                        y,
                        z,
                        1,
                        block.textureRow,
                        skyLight,
                        blockLight
                    );
                }


                // ------------------------------------------------
                // Back
                //
                // Sample the air cell at:
                //
                // x, y, z - 1
                // ------------------------------------------------

                if (
                    !world.isSolidAt(
                        x,
                        y,
                        z - 1
                    )
                    )
                {
                    int skyLight =
                        lighting.getSkyLight(
                            x,
                            y,
                            z - 1
                        );


                    int blockLight =
                        lighting.getBlockLight(
                            x,
                            y,
                            z - 1
                        );


                    addFace(
                        vertices,
                        x,
                        y,
                        z,
                        2,
                        block.textureRow,
                        skyLight,
                        blockLight
                    );
                }


                // ------------------------------------------------
                // Left
                //
                // Sample the air cell at:
                //
                // x - 1, y, z
                // ------------------------------------------------

                if (
                    !world.isSolidAt(
                        x - 1,
                        y,
                        z
                    )
                    )
                {
                    int skyLight =
                        lighting.getSkyLight(
                            x - 1,
                            y,
                            z
                        );


                    int blockLight =
                        lighting.getBlockLight(
                            x - 1,
                            y,
                            z
                        );


                    addFace(
                        vertices,
                        x,
                        y,
                        z,
                        3,
                        block.textureRow,
                        skyLight,
                        blockLight
                    );
                }


                // ------------------------------------------------
                // Right
                //
                // Sample the air cell at:
                //
                // x + 1, y, z
                // ------------------------------------------------

                if (
                    !world.isSolidAt(
                        x + 1,
                        y,
                        z
                    )
                    )
                {
                    int skyLight =
                        lighting.getSkyLight(
                            x + 1,
                            y,
                            z
                        );


                    int blockLight =
                        lighting.getBlockLight(
                            x + 1,
                            y,
                            z
                        );


                    addFace(
                        vertices,
                        x,
                        y,
                        z,
                        4,
                        block.textureRow,
                        skyLight,
                        blockLight
                    );
                }


                // ------------------------------------------------
                // Top
                //
                // Sample the air cell at:
                //
                // x, y + 1, z
                // ------------------------------------------------

                if (
                    !world.isSolidAt(
                        x,
                        y + 1,
                        z
                    )
                    )
                {
                    int skyLight =
                        lighting.getSkyLight(
                            x,
                            y + 1,
                            z
                        );


                    int blockLight =
                        lighting.getBlockLight(
                            x,
                            y + 1,
                            z
                        );


                    addFace(
                        vertices,
                        x,
                        y,
                        z,
                        0,
                        block.textureRow,
                        skyLight,
                        blockLight
                    );
                }


                // ------------------------------------------------
                // Bottom
                //
                // Sample the air cell at:
                //
                // x, y - 1, z
                // ------------------------------------------------

                if (
                    !world.isSolidAt(
                        x,
                        y - 1,
                        z
                    )
                    )
                {
                    int skyLight =
                        lighting.getSkyLight(
                            x,
                            y - 1,
                            z
                        );


                    int blockLight =
                        lighting.getBlockLight(
                            x,
                            y - 1,
                            z
                        );


                    addFace(
                        vertices,
                        x,
                        y,
                        z,
                        5,
                        block.textureRow,
                        skyLight,
                        blockLight
                    );
                }
            }
        }
    }


    // ========================================================
    // Vertex format
    //
    // Each vertex contains:
    //
    // X
    // Y
    // Z
    //
    // U
    // V
    //
    // face
    //
    // texture row
    //
    // sky light
    //
    // block light
    //
    // = 9 floats
    // ========================================================

    vertexCount =
        static_cast<int>(
            vertices.size() / 9
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


    // ========================================================
    // Vertex attribute 0
    //
    // Position:
    //
    // X Y Z
    // ========================================================

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        9 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(
        0
    );


    // ========================================================
    // Vertex attribute 1
    //
    // Texture coordinates:
    //
    // U V
    // ========================================================

    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        9 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(
        1
    );


    // ========================================================
    // Vertex attribute 2
    //
    // Cube face number
    // ========================================================

    glVertexAttribPointer(
        2,
        1,
        GL_FLOAT,
        GL_FALSE,
        9 * sizeof(float),
        (void*)(5 * sizeof(float))
    );

    glEnableVertexAttribArray(
        2
    );


    // ========================================================
    // Vertex attribute 3
    //
    // Block texture atlas row
    // ========================================================

    glVertexAttribPointer(
        3,
        1,
        GL_FLOAT,
        GL_FALSE,
        9 * sizeof(float),
        (void*)(6 * sizeof(float))
    );

    glEnableVertexAttribArray(
        3
    );


    // ========================================================
    // Vertex attribute 4
    //
    // Sky light
    //
    // 0 - 15
    // ========================================================

    glVertexAttribPointer(
        4,
        1,
        GL_FLOAT,
        GL_FALSE,
        9 * sizeof(float),
        (void*)(7 * sizeof(float))
    );

    glEnableVertexAttribArray(
        4
    );


    // ========================================================
    // Vertex attribute 5
    //
    // Block light
    //
    // 0 - 15
    // ========================================================

    glVertexAttribPointer(
        5,
        1,
        GL_FLOAT,
        GL_FALSE,
        9 * sizeof(float),
        (void*)(8 * sizeof(float))
    );

    glEnableVertexAttribArray(
        5
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
// sky light
// block light
//
// = 9 floats per vertex
// ============================================================

void ChunkMesh::addFace(
    std::vector<float>& vertices,
    int x,
    int y,
    int z,
    int face,
    int textureRow,
    int skyLight,
    int blockLight
)
{
    // --------------------------------------------------------
    // The six faces of a cube
    //
    // Each face contains:
    //
    // 2 triangles
    // 6 vertices
    //
    // The temporary face data contains:
    //
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
    // Copy the selected face into our giant
    // chunk vertex list.
    // --------------------------------------------------------

    for (
        int vertex = 0;
        vertex < 6;
        vertex++
        )
    {
        int index =
            vertex * 5;


        // ----------------------------------------------------
        // Position
        // ----------------------------------------------------

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


        // ----------------------------------------------------
        // Texture coordinates
        // ----------------------------------------------------

        vertices.push_back(
            faces[face][index + 3]
        );


        vertices.push_back(
            faces[face][index + 4]
        );


        // ----------------------------------------------------
        // Cube face number
        // ----------------------------------------------------

        vertices.push_back(
            static_cast<float>(
                face
                )
        );


        // ----------------------------------------------------
        // Block texture row
        // ----------------------------------------------------

        vertices.push_back(
            static_cast<float>(
                textureRow
                )
        );


        // ----------------------------------------------------
        // Sky light
        // ----------------------------------------------------

        vertices.push_back(
            static_cast<float>(
                skyLight
                )
        );


        // ----------------------------------------------------
        // Block light
        // ----------------------------------------------------

        vertices.push_back(
            static_cast<float>(
                blockLight
                )
        );
    }
}