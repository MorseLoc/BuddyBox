#include "Camera.h"

#include <GLFW/glfw3.h>
#include <cmath>

Camera::Camera()
{
	position = glm::vec3(
		0.0f,
		0.0f,
		0.0f
	);

	front = glm::vec3(
		0.0f,
		0.0f,
		-1.0f
	);

	up = glm::vec3(
		0.0f,
		1.0f,
		0.0f
	);

	yaw = -90.0f;
	pitch = 0.0f;

	lastMouseX = 400.0;
	lastMouseY = 300.0;

	mouseSensitivity = 0.2f;
}


void Camera::update(
	GLFWwindow* window
)
{
	// Get the mouse's current position.
	double mouseX;
	double mouseY;

	glfwGetCursorPos(
		window,
		&mouseX,
		&mouseY
	);

	// Measure how far the mouse moved since the previous frame.
	float mouseOffsetX =
		static_cast<float>(mouseX - lastMouseX);

	float mouseOffsetY =
		static_cast<float>(lastMouseY - mouseY);

	// Save the current mouse position for next frame.
	lastMouseX = mouseX;
	lastMouseY = mouseY;

	// Apply mouse sensitivity.
	mouseOffsetX *= mouseSensitivity;
	mouseOffsetY *= mouseSensitivity;

	// Update camera rotation.
	yaw += mouseOffsetX;
	pitch += mouseOffsetY;

	// Prevent the camera from flipping upside down.
	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	// Convert yaw and pitch into a direction.
	glm::vec3 direction;

	direction.x =
		cos(glm::radians(yaw)) *
		cos(glm::radians(pitch));

	direction.y =
		sin(glm::radians(pitch));

	direction.z =
		sin(glm::radians(yaw)) *
		cos(glm::radians(pitch));

	front = glm::normalize(direction);
}


glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(
		position,
		position + front,
		up
	);
}


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

void Camera::updatePosition(
	const glm::vec3& playerPosition
)
{
	position = playerPosition + glm::vec3(
		0.0f,
		0.7f,
		0.0f
	);
}