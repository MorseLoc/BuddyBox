#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// Forward declaration.
//
// Camera.h only needs to know that GLFWwindow exists.
// The full GLFW header is included in Camera.cpp.
struct GLFWwindow;


// ============================================================
// Camera
//
// Represents the player's view into the BuddyBox world.
//
// The camera stores:
// - Its position
// - The direction it is looking
// - Its up direction
// - Horizontal rotation (yaw)
// - Vertical rotation (pitch)
// - Previous mouse position
// - Mouse sensitivity
//
// Camera.cpp handles mouse-look and following the player.
// ============================================================

class Camera
{
public:
    // --------------------------------------------------------
    // Constructor
    // --------------------------------------------------------

    // Creates a camera with its default direction,
    // rotation, and mouse settings.
    Camera();


    // --------------------------------------------------------
    // Camera updates
    // --------------------------------------------------------

    // Reads mouse movement and updates
    // the direction the camera is looking.
    void update(
        GLFWwindow* window
    );


    // Moves the camera so it follows
    // the player's current position.
    void updatePosition(
        const glm::vec3& playerPosition
    );


    // --------------------------------------------------------
    // Camera information
    // --------------------------------------------------------

    // Creates the view matrix used to render
    // the world from this camera's point of view.
    glm::mat4 getViewMatrix() const;


    // Current direction the camera is looking.
    const glm::vec3& getFront() const;


    // Direction considered "up" by the camera.
    const glm::vec3& getUp() const;


    // Current camera position in the world.
    const glm::vec3& getPosition() const;


private:
    // --------------------------------------------------------
    // Position and direction
    // --------------------------------------------------------

    // Camera position in the 3D world.
    glm::vec3 position;


    // Normalized direction the camera is looking.
    glm::vec3 front;


    // Camera's upward direction.
    glm::vec3 up;


    // --------------------------------------------------------
    // Rotation
    // --------------------------------------------------------

    // Horizontal rotation in degrees.
    //
    // Turning left/right changes yaw.
    float yaw;


    // Vertical rotation in degrees.
    //
    // Looking up/down changes pitch.
    float pitch;


    // --------------------------------------------------------
    // Mouse input
    // --------------------------------------------------------

    // Mouse position from the previous frame.
    //
    // Camera.cpp compares these values with the new
    // mouse position to determine how far the mouse moved.
    double lastMouseX;
    double lastMouseY;


    // Controls how strongly mouse movement
    // changes camera rotation.
    float mouseSensitivity;
};