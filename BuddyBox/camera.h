#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct GLFWwindow;

class Camera
{
public:

	Camera();

	// Updates the camera from mouse movement
	// and keeps it attached to the player.
	void update(
		GLFWwindow* window,
		const glm::vec3& playerPosition
	);

	// Returns the matrix used to view the world.
	glm::mat4 getViewMatrix() const;

	// Gives Player.cpp the current camera directions.
	const glm::vec3& getFront() const;
	const glm::vec3& getUp() const;

private:

	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;

	float yaw;
	float pitch;

	double lastMouseX;
	double lastMouseY;

	float mouseSensitivity;
};