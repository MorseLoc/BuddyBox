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
	int textureWidth;
	int textureHeight;
	int textureChannels;

	// Stores the OpenGL ID for the grass texture.
	unsigned int grassTexture = 0;

	unsigned char* textureData = stbi_load(
		"textures/grass.png",
		&textureWidth,
		&textureHeight,
		&textureChannels,
		STBI_rgb_alpha
	);

	// Check whether the image loaded successfully.
	if (textureData == nullptr)
	{
		std::cout << "Failed to load grass texture.\n";
	}
	else
	{
		std::cout << "Grass texture loaded successfully.\n";

		// Print the texture's width and height to the console.
		std::cout
			<< "Texture size: "
			<< textureWidth
			<< " x "
			<< textureHeight
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
			textureWidth,
			textureHeight,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			textureData
		);

		// Build smaller versions of the texture for distance rendering.
		glGenerateMipmap(GL_TEXTURE_2D);

		// We no longer need the original image data in normal memory.
		stbi_image_free(textureData);
	}

		// 1.6
		// These vertices create a cube.
		//
		// A cube has 6 faces.
		// Each face needs 2 triangles.
		// Each triangle needs 3 vertices.
		//
		// 6 faces × 2 triangles × 3 vertices = 36 vertices.

		float cubeVertices[] =
		{
			// FRONT FACE
			-0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,    1.0f, 1.0f,

			 0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,    0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,    0.0f, 0.0f,

			// BACK FACE
			-0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,    0.0f, 1.0f,

			 0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,    1.0f, 0.0f,

			// LEFT FACE
			-0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,    0.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,    1.0f, 1.0f,

			// RIGHT FACE
			 0.5f,  0.5f,  0.5f,    0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,    1.0f, 0.0f,

			 0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,    0.0f, 1.0f,

			 // TOP FACE
			 -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
			 -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,
			  0.5f,  0.5f,  0.5f,    1.0f, 0.0f,

			  0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
			  0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
			 -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,

			 // BOTTOM FACE
			 -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
			  0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
			  0.5f, -0.5f,  0.5f,    1.0f, 1.0f,

			  0.5f, -0.5f,  0.5f,    1.0f, 1.0f,
			 -0.5f, -0.5f,  0.5f,    0.0f, 1.0f,
			 -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
		};

		// 1.7
		// Create a Vertex Buffer Object (VBO).
		// A VBO is memory on the graphics card used to store our vertex coordinates.
		unsigned int VBO;

		//1.7.1
		// Ask OpenGL to create one buffer and store its ID inside VBO.
		glGenBuffers(1, &VBO);

		//1.7.2
		// Tell OpenGL that this is the buffer we currently want to work with.
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		//1.7.3
		// Copy our square vertex data from normal memory into GPU memory.
		glBufferData(
			GL_ARRAY_BUFFER,
			sizeof(cubeVertices),
			cubeVertices,
			GL_STATIC_DRAW
		);

		// 1.8
		// Create a Vertex Array Object (VAO).
		// The VAO remembers how our vertex data is organized.
		unsigned int VAO;

		// 1.8.1
		// Ask OpenGL to create one VAO and store its ID.
		glGenVertexArrays(1, &VAO);

		// 1.8.2
		// Tell OpenGL that this VAO is the one we are currently setting up.
		glBindVertexArray(VAO);

		// 1.8.3
		// Re-bind our VBO so the VAO knows which vertex buffer we are using.
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// 1.8.4
		// Tell OpenGL how to read each vertex.
		//
		// 0 = vertex attribute slot 0
		// 3 = each vertex has 3 numbers: X, Y, Z
		// GL_FLOAT = each number is a float
		// GL_FALSE = do not normalize the numbers
		// 3 * sizeof(float) = move forward 3 floats to reach the next vertex
		// (void*)0 = start reading at the beginning of each vertex
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			5 * sizeof(float),
			(void*)0
		);

		// 1.8.5
		// Turn on vertex attribute slot 0.
		glEnableVertexAttribArray(0);

		// 1.8.6
		// Tell OpenGL where the texture coordinates are stored.
		//
		// Attribute slot 1
		// 2 values per vertex: U and V
		// Each vertex contains 5 floats total
		// Texture data starts after the first 3 floats
		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			5 * sizeof(float),
			(void*)(3 * sizeof(float))
		);

		glEnableVertexAttribArray(1);

		// 1.9
		// Vertex shader:
		// Takes each vertex's position in the 3D world and converts it
		// into the correct position on our 2D screen.
		//
		// It also receives the texture coordinates (U and V)
		// and passes them to the fragment shader.
		const char* vertexShaderSource =
			"#version 330 core\n"

			// The XYZ position coming from our VAO/VBO.
			"layout (location = 0) in vec3 position;\n"

			// The UV texture coordinates coming from our VAO/VBO.
			"layout (location = 1) in vec2 textureCoordinate;\n"

			// Pass the texture coordinates to the fragment shader.
			"out vec2 texCoord;\n"

			// These three matrices control how the object is viewed.
			"uniform mat4 model;\n"
			"uniform mat4 view;\n"
			"uniform mat4 projection;\n"

			"void main()\n"
			"{\n"

			// Convert the vertex from 3D world space into screen space.
			"    gl_Position = projection * view * model * vec4(position, 1.0);\n"

			// Pass this vertex's texture coordinates to the fragment shader.
			"    texCoord = textureCoordinate;\n"

			"}\n";

		// 1.9.1
		// Create a vertex shader on the GPU.
		unsigned int vertexShader;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Give our shader source code to the GPU.
		glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);

		// Compile the shader into something the GPU can execute.
		glCompileShader(vertexShader);

		// 1.10
		// Fragment shader:
		// Controls the color of every pixel drawn on the block.
		//
		// The texture coordinates come from the vertex shader.
		// "blockTexture" is the texture image we want to draw.
		const char* fragmentShaderSource =
			"#version 330 core\n"

			// Receive the UV texture coordinates from the vertex shader.
			"in vec2 texCoord;\n"

			// The final color that will appear on the screen.
			"out vec4 finalColor;\n"

			// The texture image that C++ will give to this shader.
			"uniform sampler2D blockTexture;\n"

			"void main()\n"
			"{\n"

			// Look at the texture using the UV coordinates
			// and use that pixel as the final color.
			"    finalColor = texture(blockTexture, texCoord);\n"

			"}\n";

		// 1.10.1
		// Create a fragment shader on the GPU.
		unsigned int fragmentShader;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Give the fragment shader its source code.
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

		// Compile the fragment shader so the GPU can execute it.
		glCompileShader(fragmentShader);

		// 1.11
		// Create a shader program.
		// This combines the vertex shader and fragment shader into one usable program.
		unsigned int shaderProgram;
		shaderProgram = glCreateProgram();

		// 1.11.1
		// Attach both shaders to the program.
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);

		// 1.11.2
		// Link them together into one complete shader program.
		glLinkProgram(shaderProgram);

		// 1.11.3
		// The separate shader objects are no longer needed after linking.
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		//@Change SECTION 1.12 MOVED TO PLAYER.CPP

		// 1.12 (NEW)
		// Create a player object to represent the player in the world.
		// Creates the player using the default values from Player.cpp.
		Player player;

		// Create the game world.
		World world;

		// THIS IS WHERE YOU GIVE THE WORLD NAME OF THE FILE YOU WANT TO LOAD.
		if (!world.loadFromFile("test.world"))
		{
			std::cout << "Failed to load test.world\n";

			std::cout << "Blocks loaded: " << world.blocks.size() << "\n";
		}

		// Stores the camera's position in the 3D world.
		glm::vec3 cameraPosition = player.position;

		//Was added before the playerPosition variable, but is now replaced by it. It is kept here for reference.
		//glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);

		// 1.12.1
		// The direction the camera is looking.
		// Negative Z points forward into the screen.
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

		// 1.12.2
		// Defines which direction is "up" for the camera.
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		//@change SECTION 1.12.3 MOVED TO PLAYER.CPP

			// 1.12.3.1
			// Stores the time between the current frame and the last frame. (used for time-based movement, vice frame-based movement)
		float deltaTime = 0.0f;
		float lastFrame = 0.0f;

		// 1.12.4
		// Stores the camera's horizontal rotation.
		// -90 degrees means we initially look toward negative Z.
		float yaw = -90.0f;

		// 1.12.5
		// Stores the camera's vertical rotation.
		// 0 means we begin looking straight ahead.
		float pitch = 0.0f;

		// 1.12.6
		// Stores the mouse's previous position.
		// Since our window is 800 x 600, its center is 400 x 300.
		double lastMouseX = 400.0;
		double lastMouseY = 300.0;

		// 1.12.7
		// Controls how strongly mouse movement rotates the camera.
		float mouseSensitivity = 0.1f;

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

			//@change SECTION 1.13.2 MOVED TO PLAYER.CPP

			// 1.13.2 (NEW)
			// Tell the Player object to handle WASD movement.
			//
			// We give it:
			// window = lets Player.cpp check the keyboard
			// deltaTime = keeps movement speed consistent
			// cameraFront = tells the player which way "forward" is
			// cameraUp = helps calculate left and right
			player.move(
				window,
				deltaTime,
				cameraFront,
				cameraUp
			);

			// 1.13.2.1
			// Keep the camera attached to the player's position.
			cameraPosition = player.position;

			// 1.13.2.2
			// Get the mouse's current position.
			double mouseX;
			double mouseY;

			glfwGetCursorPos(window, &mouseX, &mouseY);

			// Measure how far the mouse moved since the previous frame.
			float mouseOffsetX = static_cast<float>(mouseX - lastMouseX);
			float mouseOffsetY = static_cast<float>(lastMouseY - mouseY);

			// Save the current position for the next frame.
			lastMouseX = mouseX;
			lastMouseY = mouseY;

			// Apply mouse sensitivity.
			mouseOffsetX *= mouseSensitivity;
			mouseOffsetY *= mouseSensitivity;

			// Change our horizontal and vertical camera rotation.
			yaw += mouseOffsetX;
			pitch += mouseOffsetY;

			// 1.13.2.2.1
			// Prevents the camera from flipping upside down.
			if (pitch > 89.0f)
			{
				pitch = 89.0f;
			}

			if (pitch < -89.0f)
			{
				pitch = -89.0f;
			}

			// 1.13.2.2.2
			// Convert yaw and pitch into a 3D direction.
			glm::vec3 direction;

			direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			direction.y = sin(glm::radians(pitch));
			direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

			// Normalize keeps the direction length equal to 1.
			cameraFront = glm::normalize(direction);

			// 1.13.2.3 REMOVED

			// 1.13.2.4
			// Create the VIEW matrix.
			// This represents the camera looking from cameraPosition
			// toward the direction stored in cameraFront.
			glm::mat4 view = glm::lookAt(
				cameraPosition,
				cameraPosition + cameraFront,
				cameraUp
			);

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
			glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

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

			// Use the VAO that describes our square's vertex data.
			glBindVertexArray(VAO);

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