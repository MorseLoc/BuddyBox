#include "Player.h"

#include <GLFW/glfw3.h>


// Creates a new Player and gives it default values.
Player::Player()
{
	// Starting position.
	position = glm::vec3(
		0.0f,
		1.4f,
		3.0f
	);


	// Player collision box.
	//
	// 0.6 blocks wide
	// 1.8 blocks tall
	// 0.6 blocks deep
	size = glm::vec3(
		0.6f,
		1.8f,
		0.6f
	);


	// Walking speed.
	// 3 world units per second.
	speed = 3.0f;
}


// Handles WASD movement.
void Player::move(
	GLFWwindow* window,
	float deltaTime,
	const glm::vec3& cameraFront,
	const glm::vec3& cameraUp
)
{
	// Create a flat forward direction.
	//
	// We ignore cameraFront.y so looking up or down
	// does not make the player fly.
	glm::vec3 flatFront = glm::normalize(
		glm::vec3(
			cameraFront.x,
			0.0f,
			cameraFront.z
		)
	);


	// Calculate the direction to the player's right.
	glm::vec3 rightDirection = glm::normalize(
		glm::cross(flatFront, cameraUp)
	);


	// W = forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		position += flatFront * speed * deltaTime;
	}


	// S = backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		position -= flatFront * speed * deltaTime;
	}


	// A = left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		position -= rightDirection * speed * deltaTime;
	}


	// D = right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		position += rightDirection * speed * deltaTime;
	}
}