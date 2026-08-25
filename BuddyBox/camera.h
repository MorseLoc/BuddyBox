#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct GLFWwindow;

class Camera
{
public:

	Camera();

	// Updates the direction the camera is looking
	// based on mouse movement.
	void update(
		GLFWwindow* window
	);

	// Updates only the camera's position
	// so it follows the player's newest position.
	void updatePosition(
		const glm::vec3& playerPosition
	);

	// Returns the matrix used to view the world.
	glm::mat4 getViewMatrix() const;

	// Gives Player.cpp the current camera directions.
	// Gives the camera's front and up vectors, as well as its position.
	const glm::vec3& getFront() const;
	const glm::vec3& getUp() const;
	const glm::vec3& getPosition() const;


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