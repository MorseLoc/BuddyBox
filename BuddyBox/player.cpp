#include "Player.h"

#include <GLFW/glfw3.h>

#include "World.h"

#include <cmath>


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

	// The player starts with no vertical movement.
	verticalVelocity = 0.0f;

	// We will let collision determine whether
	// the player is actually standing on something.
	grounded = false;
}

// Checks whether the player's hitbox would overlap
// any solid block in the world.
bool Player::collidesWithWorld(
	const glm::vec3& testPosition,
	const World& world
) const
{
	// Calculate the outer edges of the player's hitbox.
	glm::vec3 playerMin = testPosition - (size / 2.0f);
	glm::vec3 playerMax = testPosition + (size / 2.0f);

	// Convert those edges into block-grid coordinates.
	int minX = static_cast<int>(floor(playerMin.x + 0.5f));
	int maxX = static_cast<int>(floor(playerMax.x + 0.5f));

	int minY = static_cast<int>(floor(playerMin.y + 0.5f));
	int maxY = static_cast<int>(floor(playerMax.y + 0.5f));

	int minZ = static_cast<int>(floor(playerMin.z + 0.5f));
	int maxZ = static_cast<int>(floor(playerMax.z + 0.5f));

	// Check every grid cell the player's hitbox touches.
	for (int x = minX; x <= maxX; x++)
	{
		for (int y = minY; y <= maxY; y++)
		{
			for (int z = minZ; z <= maxZ; z++)
			{
				if (world.isSolidAt(x, y, z))
				{
					return true;
				}
			}
		}
	}

	return false;
}

// Handles WASD movement.
void Player::move(
	GLFWwindow* window,
	float deltaTime,
	const glm::vec3& cameraFront,
	const glm::vec3& cameraUp,
	const World& world
)
{
	// Create a flat forward direction.
	//
	// This removes the up/down part of cameraFront
	// so looking up or down does not make the player fly.
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


	// Start with no movement.
	glm::vec3 movement = glm::vec3(
		0.0f,
		0.0f,
		0.0f
	);


	// W = forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		movement += flatFront;
	}


	// S = backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		movement -= flatFront;
	}


	// A = left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		movement -= rightDirection;
	}


	// D = right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		movement += rightDirection;
	}


	// If the player is trying to move,
	// normalize the direction so diagonal movement
	// is not faster than straight movement.
	if (glm::length(movement) > 0.0f)
	{
		movement = glm::normalize(movement);

		// Apply walking speed and frame time.
		movement *= speed * deltaTime;
	}


	// Try moving on the X axis first.
	glm::vec3 testPosition = position;

	testPosition.x += movement.x;

	if (!collidesWithWorld(testPosition, world))
	{
		position.x = testPosition.x;
	}


	// Try moving on the Z axis separately.
	testPosition = position;

	testPosition.z += movement.z;

	if (!collidesWithWorld(testPosition, world))
	{
		position.z = testPosition.z;
	}

	// GRAVITY
	//
	// Gravity constantly pulls the player's
	// vertical velocity downward.
	const float gravity = -20.0f;

	verticalVelocity += gravity * deltaTime;


	// Calculate where gravity wants to move the player.
	glm::vec3 verticalTestPosition = position;

	verticalTestPosition.y += verticalVelocity * deltaTime;


	// Check whether that vertical movement would
	// collide with a solid block.
	if (!collidesWithWorld(verticalTestPosition, world))
	{
		// Nothing is blocking us.
		// Allow the vertical movement.
		position.y = verticalTestPosition.y;

		grounded = false;
	}
	else
	{
		// Something stopped our vertical movement.
		verticalVelocity = 0.0f;

		// For now, assume a downward collision
		// means we landed on the ground.
		grounded = true;
	}
}
