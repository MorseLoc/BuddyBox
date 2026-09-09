#include "Skybox.h"

#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace
{
    GLuint compileSkyShader(GLenum type, const char* source)
    {
        GLuint shader = glCreateShader(type);

        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            char message[2048];

            glGetShaderInfoLog(
                shader, sizeof(message), nullptr, message
            );

            std::cout << "Sky shader error:\n" << message << "\n";

            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }
}

bool Skybox::initialize(const char* imagePath)
{
    cleanup();

    // Build a cube directly in the shader.
    // It needs a VAO but no vertex buffer.
    const char* vertexSource = R"(
        #version 330 core

        out vec3 skyDirection;

        uniform mat4 skyView;
        uniform mat4 skyProjection;

        const vec3 corners[8] = vec3[8](
            vec3(-1, -1, -1),
            vec3( 1, -1, -1),
            vec3( 1,  1, -1),
            vec3(-1,  1, -1),
            vec3(-1, -1,  1),
            vec3( 1, -1,  1),
            vec3( 1,  1,  1),
            vec3(-1,  1,  1)
        );

        const int indices[36] = int[36](
            0, 1, 2, 2, 3, 0,
            4, 6, 5, 6, 4, 7,
            0, 3, 7, 7, 4, 0,
            1, 5, 6, 6, 2, 1,
            3, 2, 6, 6, 7, 3,
            0, 4, 5, 5, 1, 0
        );

        void main()
        {
            vec3 position = corners[indices[gl_VertexID]];
            skyDirection = position;

            vec4 clipPosition =
                skyProjection * skyView * vec4(position, 1.0);

            // Place the sky at the far end of the depth buffer.
            gl_Position = clipPosition.xyww;
        }
    )";

    const char* fragmentSource = R"(
        #version 330 core

        in vec3 skyDirection;
        out vec4 finalColor;

        uniform sampler2D skyImage;
        uniform bool hasImage;

        const float PI = 3.14159265359;

        void main()
        {
            vec3 direction = normalize(skyDirection);

            if (hasImage)
            {
                // Map a 360-degree panorama around the camera.
                float u =
                    atan(direction.z, direction.x) / (2.0 * PI)
                    + 0.5;

                float v =
                    acos(clamp(direction.y, -1.0, 1.0)) / PI;

                finalColor = vec4(
                    texture(skyImage, vec2(u, v)).rgb,
                    1.0
                );

                return;
            }

            // Default sky colors.
            vec3 horizon = vec3(0.72, 0.86, 1.0);
            vec3 overhead = vec3(0.16, 0.43, 0.82);
            vec3 below = vec3(0.35, 0.44, 0.55);

            vec3 color;

            if (direction.y >= 0.0)
            {
                color = mix(
                    horizon,
                    overhead,
                    smoothstep(0.0, 1.0, direction.y)
                );
            }
            else
            {
                color = mix(
                    horizon,
                    below,
                    smoothstep(0.0, 0.7, -direction.y)
                );
            }

            // A small sun fixed in the sky.
            vec3 sunDirection = normalize(vec3(0.4, 0.65, -0.6));

            float sun = smoothstep(
                0.9992,
                0.9996,
                dot(direction, sunDirection)
            );

            color = mix(color, vec3(1.0, 0.96, 0.78), sun);

            finalColor = vec4(color, 1.0);
        }
    )";

    GLuint vertex = compileSkyShader(
        GL_VERTEX_SHADER, vertexSource
    );

    GLuint fragment = compileSkyShader(
        GL_FRAGMENT_SHADER, fragmentSource
    );

    if (!vertex || !fragment)
    {
        if (vertex) glDeleteShader(vertex);
        if (fragment) glDeleteShader(fragment);

        return false;
    }

    program = glCreateProgram();

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char message[2048];

        glGetProgramInfoLog(
            program, sizeof(message), nullptr, message
        );

        std::cout << "Skybox link error:\n" << message << "\n";

        cleanup();
        return false;
    }

    glGenVertexArrays(1, &vao);

    viewLocation = glGetUniformLocation(program, "skyView");
    projectionLocation = glGetUniformLocation(
        program, "skyProjection"
    );
    imageLocation = glGetUniformLocation(program, "hasImage");

    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "skyImage"), 0);

    if (imagePath)
    {
        int width;
        int height;
        int channels;

        // The panorama's top edge represents the sky overhead.
        stbi_set_flip_vertically_on_load(false);

        unsigned char* pixels = stbi_load(
            imagePath,
            &width,
            &height,
            &channels,
            STBI_rgb_alpha
        );

        if (pixels)
        {
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGBA8,
                width,
                height,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                pixels
            );

            glTexParameteri(
                GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR
            );
            glTexParameteri(
                GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR
            );

            // Wrap horizontally around the world.
            glTexParameteri(
                GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT
            );
            glTexParameteri(
                GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE
            );

            stbi_image_free(pixels);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else
        {
            std::cout
                << "Sky image could not load; using built-in sky.\n";
        }
    }

    glUseProgram(0);
    return true;
}

void Skybox::draw(
    const glm::mat4& view,
    const glm::mat4& projection
) const
{
    if (!program)
    {
        return;
    }

    // Keep camera rotation, but remove camera movement.
    glm::mat4 rotationOnly = glm::mat4(glm::mat3(view));

    // Draw inside the cube, without writing depth.
    GLboolean cullingEnabled = glIsEnabled(GL_CULL_FACE);

    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    glUseProgram(program);

    glUniformMatrix4fv(
        viewLocation, 1, GL_FALSE, glm::value_ptr(rotationOnly)
    );
    glUniformMatrix4fv(
        projectionLocation, 1, GL_FALSE, glm::value_ptr(projection)
    );
    glUniform1i(imageLocation, texture != 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    // Restore the settings used by BuddyBox's world renderer.
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    if (cullingEnabled)
    {
        glEnable(GL_CULL_FACE);
    }
}

void Skybox::cleanup()
{
    if (texture) glDeleteTextures(1, &texture);
    if (vao) glDeleteVertexArrays(1, &vao);
    if (program) glDeleteProgram(program);

    texture = 0;
    vao = 0;
    program = 0;
}