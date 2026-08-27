#include "Camera.h"

#include <GLFW/glfw3.h>
#include <cmath>


// ============================================================
// Camera constructor
//
// Sets the camera's starting position, direction,
// rotation, and mouse sensitivity.
// ============================================================

Camera::Camera()
{
    // Camera starts at the origin.
    //
    // Its real gameplay position will later be updated
    // to follow the player.
    position = glm::vec3(
        0.0f,
        0.0f,
        0.0f
    );


    // Starting direction.
    //
    // Negative Z means the camera initially looks forward
    // into the BuddyBox world.
    front = glm::vec3(
        0.0f,
        0.0f,
        -1.0f
    );


    // Defines which direction counts as "up."
    up = glm::vec3(
        0.0f,
        1.0f,
        0.0f
    );


    // Horizontal camera rotation.
    yaw = -90.0f;


    // Vertical camera rotation.
    pitch = 0.0f;


    // Starting mouse position.
    //
    // These values match the center of the original
    // 800 x 600 BuddyBox window.
    lastMouseX = 400.0;
    lastMouseY = 300.0;


    // Controls how strongly mouse movement rotates the camera.
    mouseSensitivity = 0.2f;
}


// ============================================================
// Update camera rotation
//
// Reads mouse movement and converts it into
// camera yaw and pitch.
// ============================================================

void Camera::update(
    GLFWwindow* window
)
{
    // --------------------------------------------------------
    // Read mouse position
    // --------------------------------------------------------

    double mouseX;
    double mouseY;


    glfwGetCursorPos(
        window,
        &mouseX,
        &mouseY
    );


    // --------------------------------------------------------
    // Calculate mouse movement
    // --------------------------------------------------------

    // Horizontal mouse movement.
    float mouseOffsetX =
        static_cast<float>(
            mouseX - lastMouseX
            );


    // Vertical movement is reversed because window coordinates
    // increase downward, while looking upward should increase pitch.
    float mouseOffsetY =
        static_cast<float>(
            lastMouseY - mouseY
            );


    // Save this mouse position for the next frame.
    lastMouseX =
        mouseX;

    lastMouseY =
        mouseY;


    // Apply sensitivity.
    mouseOffsetX *=
        mouseSensitivity;

    mouseOffsetY *=
        mouseSensitivity;


    // --------------------------------------------------------
    // Update camera rotation
    // --------------------------------------------------------

    // Yaw controls left/right rotation.
    yaw +=
        mouseOffsetX;


    // Pitch controls up/down rotation.
    pitch +=
        mouseOffsetY;


    // Keep pitch slightly below 90 degrees.
    //
    // This prevents the camera from flipping upside down.
    if (pitch > 89.0f)
    {
        pitch = 89.0f;
    }


    if (pitch < -89.0f)
    {
        pitch = -89.0f;
    }


    // --------------------------------------------------------
    // Convert rotation into a direction vector
    // --------------------------------------------------------

    // Yaw and pitch are angles.
    //
    // These formulas convert those angles into an XYZ
    // direction that OpenGL can use.
    glm::vec3 direction;


    direction.x =
        std::cos(glm::radians(yaw)) *
        std::cos(glm::radians(pitch));


    direction.y =
        std::sin(glm::radians(pitch));


    direction.z =
        std::sin(glm::radians(yaw)) *
        std::cos(glm::radians(pitch));


    // Normalize so front always has a length of 1.
    front =
        glm::normalize(direction);
}


// ============================================================
// View matrix
//
// Creates the matrix that represents where the camera is
// and which direction it is looking.
// ============================================================

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(
        position,
        position + front,
        up
    );
}


// ============================================================
// Camera direction getters
// ============================================================

const glm::vec3& Camera::getFront() const
{
    return front;
}


const glm::vec3& Camera::getUp() const
{
    return up;
}


const glm::vec3& Camera::getPosition() const
{
    return position;
}


// ============================================================
// Follow player
//
// Moves the camera to the player's position,
// with a small vertical offset so the view comes
// from around the player's eye level.
// ============================================================

void Camera::updatePosition(
    const glm::vec3& playerPosition
)
{
    position =
        playerPosition +
        glm::vec3(
            0.0f,
            0.7f,
            0.0f
        );
}