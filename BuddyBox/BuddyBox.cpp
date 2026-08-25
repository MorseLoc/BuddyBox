// glad is included for loading OpenGL function pointers. 
// It is a library that helps manage OpenGL extensions and provides a
// simple way to load OpenGL functions at runtime.
#include <glad/glad.h>

// GLFW is a library for creating windows and contexts for OpenGL, Vulkan, and other graphics APIs. 
// It provides a simple API for creating windows, handling input, and managing OpenGL contexts. 
// The code snippet above demonstrates how to initialize GLFW, create a window, and enter a loop that polls for events until the window is closed.
#include <GLFW/glfw3.h>

// iostream is included for outputting error messages to the console. (std::cout is used to print messages indicating whether 
// GLFW initialization and window creation were successful or not.)
#include <iostream>

// glm is a header-only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications.
// It provides classes and functions for vector and matrix operations, which are commonly used in graphics programming.
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// this library is used for loading images in various formats (like PNG, JPEG, etc.) into memory.
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Block.h"
#include "Player.h"
#include "World.h"
#include "Renderer.h"
#include "Camera.h"
#include "textureManager.h"
#include "inventory.h"

// Stores mouse-wheel movement until the game loop handles it.
double scrollAmount = 0.0;


// GLFW calls this whenever the mouse wheel moves.
void scrollCallback(
	GLFWwindow* window,
	double xOffset,
	double yOffset
)
{
	scrollAmount += yOffset;
}

//1
int main()
{

	// 1.1
	//ends program if glfwInit() returns false (0), indicating that GLFW failed to initialize.
	if (!glfwInit())
	{
		std::cout << "GLFW failed to start.\n";
		return -1;
	}

	// 1.2
	// Creates a window with a width of 800 pixels,
	// a height of 600 pixels, and the title "BuddyBox".
	GLFWwindow* window = glfwCreateWindow(
		800,
		600,
		"BuddyBox",
		nullptr,
		nullptr
	);

	// 1.3
	//ends program if window creation fails (i.e., if glfwCreateWindow returns nullptr). 
	// It also calls glfwTerminate() to clean up GLFW resources before exiting.
	if (!window)
	{
		std::cout << "Window creation failed.\n";
		glfwTerminate();
		return -1;
	}

	// 1.4
	// Tells OpenGL that the BuddyBox window is the window we want to draw into.
	glfwMakeContextCurrent(window);
	//@IMPORTANT : This line enables V-Sync, which synchronizes the frame rate of the application with the refresh rate of the monitor. 
	//AKA controls speed (SWAP to 0 for max speed)
	glfwSwapInterval(1);

	// 1.4.1
	// Locks the mouse cursor inside the BuddyBox window.
	// The cursor becomes invisible so the mouse can control the camera.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// This tells GLFW to call the scrollCallback function whenever the mouse wheel is moved.
	glfwSetScrollCallback(
		window,
		scrollCallback
	);

	// 1.5
	// Loads modern OpenGL functions so BuddyBox can use them.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "GLAD failed to start.\n";
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	// This object will handle drawing everything in the BuddyBox world.
	Renderer renderer;

	// This object will handle loading and managing textures for the BuddyBox world.
	TextureManager textureManager;

	// 1.5.1
	// Enables depth testing.
	//
	// This allows OpenGL to understand which surfaces are closer
	// to the camera and which surfaces are hidden behind them.
	//
	// This is necessary for proper 3D objects like cubes.
	glEnable(GL_DEPTH_TEST);


	// 1.5.2 
	// Load the block texture atlas (artdex.png) using the TextureManager.
	if (!textureManager.loadAtlas("textures/artdex.png"))
	{
		std::cout << "Failed to load artdex.png\n";
	}
	
	unsigned int blockAtlasTexture =
		textureManager.getAtlasTexture();

		//SECTIONS 1.6 - 1.11 MOVED TO RENDERER.CPP

	// This is the shader program that will be used to draw everything in the world.
	unsigned int shaderProgram =
		renderer.getShaderProgram();

	//@Change SECTION 1.12 MOVED TO PLAYER.CPP

	// 1.12 (NEW)
	// Create a player object to represent the player in the world.
	// Creates the player using the default values from Player.cpp.
	Player player;

	// Create the game world.
	World world;

	// Create a camera object to represent the player's view in the world.
	Camera camera;

	// create an inventory object to represent the player's hotbar and selected block type.
	Inventory inventory;

	// THIS IS WHERE YOU GIVE THE WORLD NAME OF THE FILE YOU WANT TO LOAD.
	if (!world.loadFromFile("test.world"))
	{
		std::cout << "Failed to load test.world\n";

		std::cout << "Blocks loaded: " << world.blocks.size() << "\n";
	}

	//@change Camera section moved to camera.cpp
	//@change SECTION 1.12.3 MOVED TO PLAYER.CPP

		// 1.12.3.1
		// Stores the time between the current frame and the last frame. (used for time-based movement, vice frame-based movement)
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	// Store the previous state of the mouse buttons to detect clicks.
	bool leftMouseWasPressed = false;
	bool rightMouseWasPressed = false;

	//@change Camera section moved to camera.cpp
	// 1.13  
	// while loop that continues running until the window is closed. 
	// Inside the loop, glfwPollEvents() is called to process events such as keyboard and mouse input, window resizing, etc.
	while (!glfwWindowShouldClose(window))
	{
		// 1.13.1
		// Calculate how much time has passed since the previous frame.
		float currentFrame = static_cast<float>(glfwGetTime());

		deltaTime = currentFrame - lastFrame;

		lastFrame = currentFrame;

		// Prevent huge movement jumps if the game
		// temporarily pauses, such as while moving the window.
		if (deltaTime > 0.05f)
		{
			deltaTime = 0.05f;
		}

		//@change SECTION 1.13.2 MOVED TO PLAYER.CPP

		// 1.13.2 (NEW)
		// Tell the Player object to handle WASD movement.
		//
		// We give it:
		// window = lets Player.cpp check the keyboard
		// deltaTime = keeps movement speed consistent
		// cameraFront = tells the player which way "forward" is
		// cameraUp = helps calculate left and right

		//UPDATED TO USE CAMERA OBJECT INSTEAD OF CAMERA FRONT AND CAMERA UP
		camera.update(
			window
		);

		player.move(
			window,
			deltaTime,
			camera.getFront(),
			camera.getUp(),
			world
		);

		camera.updatePosition(
			player.position
		);

		// Check mouse buttons.
		bool leftMousePressed =
			glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

		bool rightMousePressed =
			glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;


		// Only trigger once when the button is first pressed.
		if (leftMousePressed && !leftMouseWasPressed)
		{
			int hitX, hitY, hitZ;
			int previousX, previousY, previousZ;

			if (world.raycastBlock(
				camera.getPosition(),
				camera.getFront(),
				5.0f,
				hitX,
				hitY,
				hitZ,
				previousX,
				previousY,
				previousZ
			))
			{
				world.removeBlock(
					hitX,
					hitY,
					hitZ
				);
			}
		}


		if (rightMousePressed && !rightMouseWasPressed)
		{
			int hitX, hitY, hitZ;
			int previousX, previousY, previousZ;

			if (world.raycastBlock(
				camera.getPosition(),
				camera.getFront(),
				5.0f,
				hitX,
				hitY,
				hitZ,
				previousX,
				previousY,
				previousZ
			))
			{
				Block block(inventory.getSelectedBlockType());

				// Create the player's collision-box edges.
				glm::vec3 playerMin =
					player.position - (player.size / 2.0f);

				glm::vec3 playerMax =
					player.position + (player.size / 2.0f);


				// Create the new block's edges.
				//
				// Blocks are centered on their grid position,
				// so each side extends 0.5 blocks outward.
				glm::vec3 blockMin(
					previousX - 0.5f,
					previousY - 0.5f,
					previousZ - 0.5f
				);

				glm::vec3 blockMax(
					previousX + 0.5f,
					previousY + 0.5f,
					previousZ + 0.5f
				);


				// Check whether the new block would overlap the player.
				bool overlapsPlayer =
					playerMax.x > blockMin.x &&
					playerMin.x < blockMax.x &&

					playerMax.y > blockMin.y &&
					playerMin.y < blockMax.y &&

					playerMax.z > blockMin.z &&
					playerMin.z < blockMax.z;


				// Only place the block if the player is not inside it.
				if (!overlapsPlayer)
				{
					world.placeBlock(
						previousX,
						previousY,
						previousZ,
						block
					);
				}
			}
		}


		// Remember mouse state for next frame.
		leftMouseWasPressed = leftMousePressed;
		rightMouseWasPressed = rightMousePressed;
	
		//Removed the following lines since cameraFront and cameraUp are now handled by the Camera class.

		//Removed cameraFront and cameraUp variables since they are now handled by the Camera class.

		// 1.13.2.3 REMOVED

		// 1.13.2.4
		// Create the VIEW matrix.
		// This represents the camera looking from cameraPosition
		// toward the direction stored in cameraFront.

		// Updated to use the Camera class's getViewMatrix() method 
		// instead of manually calculating the view matrix using cameraPosition, cameraFront, and cameraUp.
		glm::mat4 view =
			camera.getViewMatrix();

		// 1.13.2.5
		// Get the CURRENT size of the drawable window.
		int windowWidth;
		int windowHeight;

		glfwGetFramebufferSize(
			window,
			&windowWidth,
			&windowHeight
		);

		// Prevent problems if the window is minimized.
		if (windowHeight == 0)
		{
			windowHeight = 1;
		}

		// Tell OpenGL to use the entire available window.
		glViewport(
			0,
			0,
			windowWidth,
			windowHeight
		);

		// Create perspective using the CURRENT window shape.
		glm::mat4 projection = glm::perspective(
			glm::radians(45.0f),
			static_cast<float>(windowWidth) /
			static_cast<float>(windowHeight),
			0.1f,
			100.0f
		);

		// 1.13.3
		// Choose the color OpenGL will use when clearing the screen.
		// Values are: Red, Green, Blue, Alpha.
		//THIS IS THE SKY
		glClearColor(0.55f, 0.70f, 0.90f, 1.0f);

		// Clear both the old colors and the old depth information.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//1.13.3.1 CREATION OF TWO TRIANGLES TO FORM A SQUARE
		// Use the shader program we created.
		glUseProgram(shaderProgram);

		// Tell OpenGL we want to use texture unit 0.
		glActiveTexture(GL_TEXTURE0);

		// Bind the grass texture to texture unit 0.
		glBindTexture(
			GL_TEXTURE_2D,
			blockAtlasTexture
		);

		// Find the blockTexture variable inside the shader.
		int textureLocation = glGetUniformLocation(
			shaderProgram,
			"blockTexture"
		);

		// Tell the shader that blockTexture should use texture unit 0.
		glUniform1i(
			textureLocation,
			0
		);

		// 1.13.3.1.1
		// Send the MODEL matrix to the shader.
		int modelLocation = glGetUniformLocation(shaderProgram, "model");

		// Send the VIEW matrix to the shader.
		int viewLocation = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(
			viewLocation,
			1,
			GL_FALSE,
			glm::value_ptr(view)
		);

		// Send the PROJECTION matrix to the shader.
		int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(
			projectionLocation,
			1,
			GL_FALSE,
			glm::value_ptr(projection)
		);

		// Use the VAO that describes our square's vertex data. REMOVED 
		// This VAO was created in Renderer.cpp.
		renderer.bindCube();

		//REMOVED KEPT FOR REFERENCE:
		// 1.13.3.1.2
		//int colorLocation = glGetUniformLocation(shaderProgram, "shapeColor");

		// Give our first cube a temporary green color.
		//glUniform4f(
		//	colorLocation,
		//	0.2f,
		//	0.8f,
		//	0.3f,
		//	1.0f
		//);

		// Atlas texture row and block count locations in the shader.
		int textureRowLocation =
			glGetUniformLocation(
				shaderProgram,
				"textureRow"
			);

		int atlasRowsLocation =
			glGetUniformLocation(
				shaderProgram,
				"atlasRows"
			);

		glUniform1f(
			atlasRowsLocation,
			static_cast<float>(
				textureManager.getBlockCount()
				)
		);

		// Draw all 36 vertices that make up the cube.
		// Now changed to draw all blocks in the world instead of just one cube.

		// Draw every block stored in the world.
		for (const auto& entry : world.blocks)
		{

			// Get the actual block stored at this grid position.
			const Block& block = entry.second;

			// This block's texture row in the atlas is sent to the shader.
			glUniform1f(
				textureRowLocation,
				static_cast<float>(
					block.textureRow
					)
			);

			// Get the grid coordinate.
			int x = std::get<0>(entry.first);
			int y = std::get<1>(entry.first);
			int z = std::get<2>(entry.first);

			// Start with a fresh model matrix for this block.
			glm::mat4 model = glm::mat4(1.0f);

			// Move the cube to this block's grid position.
			model = glm::translate(
				model,
				glm::vec3(
					static_cast<float>(x),
					static_cast<float>(y),
					static_cast<float>(z)
				)
			);

			// Send this block's position to the shader.
			glUniformMatrix4fv(
				modelLocation,
				1,
				GL_FALSE,
				glm::value_ptr(model)
			);

			// Draw this block.
			glDrawArrays(
				GL_TRIANGLES,
				0,
				36
			);
		}

		//1.13.4
		// Display the frame we just created.
		glfwSwapBuffers(window);

		//1.13.5
		// Check for things like mouse, keyboard, resizing, and closing.
		glfwPollEvents();
	}

	// Move through the hotbar with the mouse wheel.
	if (scrollAmount > 0.0)
	{
		inventory.cycleSlot(-1);
	}
	else if (scrollAmount < 0.0)
	{
		inventory.cycleSlot(1);
	}

	// We handled this scroll.
	scrollAmount = 0.0;

	// 1.14 
	// After the loop exits (when the window is closed), the program cleans up by destroying the window and terminating GLFW.
	glfwDestroyWindow(window);
	glfwTerminate();

	// 1.15
	// ends the program with a return value of 0, indicating successful execution.
	return 0;
}