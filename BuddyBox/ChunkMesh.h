#pragma once

#include <vector>

struct World;


// ============================================================
// ChunkMesh
//
// Stores all visible block faces inside one 16x16x16 region.
//
// Instead of drawing every block separately,
// an entire chunk can be drawn with one OpenGL draw call.
// ============================================================

class ChunkMesh
{
public:

    static const int CHUNK_SIZE = 16;


    ChunkMesh();

    ~ChunkMesh();


    // Builds this chunk's mesh from the world.
    void build(
        const World& world,
        int chunkX,
        int chunkY,
        int chunkZ
    );


    // Draws the entire chunk.
    void draw() const;


private:

    unsigned int VAO;
    unsigned int VBO;

    int vertexCount;


    // Adds one exposed cube face to the mesh.
    void addFace(
        std::vector<float>& vertices,
        int x,
        int y,
        int z,
        int face,
        int textureRow
    );
};