#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class Skybox
{
public:
    // Uses a built-in sky if no image path is supplied.
    bool initialize(const char* imagePath = nullptr);

    void draw(
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& sunDirection,
        float daylight
    ) const;

    // Call before destroying the OpenGL window.
    void cleanup();

private:
    GLuint program = 0;
    GLuint vao = 0;
    GLuint texture = 0;

    GLint viewLocation = -1;
    GLint projectionLocation = -1;
    GLint imageLocation = -1;

    GLint sunLocation = -1;
    GLint daylightLocation = -1;
};