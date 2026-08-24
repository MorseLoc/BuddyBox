#pragma once

#include <glm/glm.hpp>

// Forward declaration.
// This tells C++ that GLFWwindow exists without needing
// to include all of GLFW inside Player.h.
struct GLFWwindow;

// Forward declaration.
struct World;


// Represents the player in the world.
struct Player
{
	// The player's position in the 3D world.
	//
	// X = left/right
	// Y = up/down
	// Z = forward/backward
	glm::vec3 position;


	// The size of the player's invisible collision box.
	//
	// X = width
	// Y = height
	// Z = depth
	glm::vec3 size;


	// How fast the player walks.
	// Measured in world units per second.
	float speed;


	// Constructor.
	// Gives a new player its starting values.
	Player();

	// Checks whether the player's collision box
	// would overlap any solid block in the world.
	bool collidesWithWorld(
		const glm::vec3& testPosition,
		const World& world
	) const;

	// Handles the player's WASD movement.
	//
	// window = lets us check which keys are pressed
	// deltaTime = keeps movement speed independent of FPS
	// cameraFront = tells us which way the player is facing
	// cameraUp = helps calculate left and right
	void move(
		GLFWwindow* window,
		float deltaTime,
		const glm::vec3& cameraFront,
		const glm::vec3& cameraUp,
		const World& world
	);
};