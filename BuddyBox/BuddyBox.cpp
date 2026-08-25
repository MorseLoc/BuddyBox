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

	// 1.5.1
	// Enables depth testing.
	//
	// This allows OpenGL to understand which surfaces are closer
	// to the camera and which surfaces are hidden behind them.
	//
	// This is necessary for proper 3D objects like cubes.
	glEnable(GL_DEPTH_TEST);


	// 1.5.2
	// Load the grass texture image from disk.
	int grassTextureWidth;
	int grassTextureHeight;
	int grassTextureChannels;

	// Stores the OpenGL ID for the grass texture.
	unsigned int grassTexture = 0;

	// Stores the OpenGL ID for the spawner texture.
	unsigned int spawnerTexture = 0;

	unsigned char* grassTextureData = stbi_load(
		"textures/grass.png",
		&grassTextureWidth,
		&grassTextureHeight,
		&grassTextureChannels,
		STBI_rgb_alpha
	);

	// Check whether the image loaded successfully.
	if (grassTextureData == nullptr)
	{
		std::cout << "Failed to load grass texture.\n";
	}
	else
	{
		std::cout << "Grass texture loaded successfully.\n";

		// Print the texture's width and height to the console.
		std::cout
			<< "Texture size: "
			<< grassTextureWidth
			<< " x "
			<< grassTextureHeight
			<< "\n";

		// 1.5.2.1 
		// Create an OpenGL texture object.
		glGenTextures(1, &grassTexture);

		// Make this the texture we are currently working with.
		glBindTexture(GL_TEXTURE_2D, grassTexture);

		// Use nearest-neighbor filtering so the pixel art stays crisp.
		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_MIN_FILTER,
			GL_NEAREST
		);

		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_MAG_FILTER,
			GL_NEAREST
		);

		// Repeat the texture if coordinates ever go outside 0 to 1.
		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_S,
			GL_REPEAT
		);

		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_T,
			GL_REPEAT
		);

		// 1.5.2.2
		// Upload the PNG pixel data into the OpenGL texture.
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			grassTextureWidth,
			grassTextureHeight,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			grassTextureData
		);

		// Build smaller versions of the texture for distance rendering.
		glGenerateMipmap(GL_TEXTURE_2D);

		// We no longer need the original image data in normal memory.
		stbi_image_free(grassTextureData);
	}

	// 1.5.3
	// Load the Spawner texture image from disk.
	int spawnerTextureWidth;
	int spawnerTextureHeight;
	int spawnerTextureChannels;

	unsigned char* spawnerTextureData = stbi_load(
		"textures/spawner.png",
		&spawnerTextureWidth,
		&spawnerTextureHeight,
		&spawnerTextureChannels,
		STBI_rgb_alpha
	);

	// Check whether the Spawner image loaded successfully.
	if (spawnerTextureData == nullptr)
	{
		std::cout << "Failed to load Spawner texture.\n";
	}
	else
	{
		std::cout << "Spawner texture loaded successfully.\n";

		// Create the OpenGL texture.
		glGenTextures(1, &spawnerTexture);

		// Work with the Spawner texture.
		glBindTexture(
			GL_TEXTURE_2D,
			spawnerTexture
		);

		// Keep pixel art crisp.
		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_MIN_FILTER,
			GL_NEAREST
		);

		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_MAG_FILTER,
			GL_NEAREST
		);

		// Allow the texture to repeat.
		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_S,
			GL_REPEAT
		);

		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_T,
			GL_REPEAT
		);

		// Upload the Spawner PNG to the GPU.
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			spawnerTextureWidth,
			spawnerTextureHeight,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			spawnerTextureData
		);

		glGenerateMipmap(GL_TEXTURE_2D);

		// Free the original image from normal memory.
		stbi_image_free(spawnerTextureData);
	}

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

		player.move(
			window,
			deltaTime,
			camera.getFront(),
			camera.getUp(),
			world
		);
	
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
			grassTexture
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

		// Draw all 36 vertices that make up the cube.
		// Now changed to draw all blocks in the world instead of just one cube.

		// Draw every block stored in the world.
		for (const auto& entry : world.blocks)
		{

			// Get the actual block stored at this grid position.
			const Block& block = entry.second;


			// Choose the correct texture for this block type.
			if (block.type == BlockType::Grass)
			{
				glBindTexture(
					GL_TEXTURE_2D,
					grassTexture
				);
			}
			else if (block.type == BlockType::Spawner)
			{
				glBindTexture(
					GL_TEXTURE_2D,
					spawnerTexture
				);
			}
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

	// 1.14 
	// After the loop exits (when the window is closed), the program cleans up by destroying the window and terminating GLFW.
	glfwDestroyWindow(window);
	glfwTerminate();

	// 1.15
	// ends the program with a return value of 0, indicating successful execution.
	return 0;
}