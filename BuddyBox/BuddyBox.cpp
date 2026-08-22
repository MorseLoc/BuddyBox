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

	// 1.6
	// These are the 3 corner points of our triangle.
	// Each group of 3 numbers is X, Y, Z.
	float squareVertices[] =
	{
		// Triangle 1
		-0.5f,  0.5f, 0.0f,   // Top-left
		-0.5f, -0.5f, 0.0f,   // Bottom-left
		 0.5f,  0.5f, 0.0f,   // Top-right

		 // Triangle 2
		 0.5f,  0.5f, 0.0f,   // Top-right
		-0.5f, -0.5f, 0.0f,   // Bottom-left
		 0.5f, -0.5f, 0.0f,    // Bottom-right
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
		sizeof(squareVertices),
		squareVertices,
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
		3 * sizeof(float),
		(void*)0
	);

	// 1.8.5
	// Turn on vertex attribute slot 0.
	glEnableVertexAttribArray(0);

	// 1.9
	// Vertex shader:
	// Takes each vertex's position in the 3D world and converts it
	// into the correct position on our 2D screen.
	const char* vertexShaderSource =
		"#version 330 core\n"

		// The XYZ position coming from our VAO/VBO.
		"layout (location = 0) in vec3 position;\n"

		// These three matrices will control how the object is viewed.
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"

		"void main()\n"
		"{\n"

		// Convert the vertex from 3D world space into screen space.
		"    gl_Position = projection * view * model * vec4(position, 1.0);\n"

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
	// Controls the color of whatever we draw.
	//
	// Instead of hard-coding one color, "shapeColor" lets our
	// C++ code tell the shader what color to use.
	const char* fragmentShaderSource =
		"#version 330 core\n"
		"out vec4 finalColor;\n"
		"uniform vec4 shapeColor;\n"
		"void main()\n"
		"{\n"
		"    finalColor = shapeColor;\n"
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

	// 1.12
	// Stores the camera's position in the 3D world.
	//
	// X = left/right
	// Y = up/down
	// Z = forward/backward
	//
	// The camera starts 3 units away from the square.
	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);

	// 1.12.1
	// The direction the camera is looking.
	// Negative Z points forward into the screen.
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

	// 1.12.2
	// Defines which direction is "up" for the camera.
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// 1.12.3
	// Controls how fast the camera moves.
	float cameraSpeed = 0.05f;

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
		// CAMERA MOVEMENT
		// Checks which movement keys are currently being held.

		// W = move forward
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			cameraPosition += cameraSpeed * cameraFront;
		}

		// S = move backward
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			cameraPosition -= cameraSpeed * cameraFront;
		}

		// A = move left
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}

		// D = move right
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}

		// 1.13.1.1
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

		// 1.13.1.1.1
		// Prevents the camera from flipping upside down.
		if (pitch > 89.0f)
		{
			pitch = 89.0f;
		}

		if (pitch < -89.0f)
		{
			pitch = -89.0f;
		}

		// 1.13.1.1.2
		// Convert yaw and pitch into a 3D direction.
		glm::vec3 direction;

		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		// Normalize keeps the direction length equal to 1.
		cameraFront = glm::normalize(direction);

		// 1.13.1.2
		// Create the MODEL matrix.
		// This controls where the square exists in the world.
		glm::mat4 model = glm::mat4(1.0f);

		// 1.13.1.3
		// Create the VIEW matrix.
		// This represents the camera looking from cameraPosition
		// toward the direction stored in cameraFront.
		glm::mat4 view = glm::lookAt(
			cameraPosition,
			cameraPosition + cameraFront,
			cameraUp
		);

		// 1.13.1.4
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

		// 1.13.2
		// Choose the color OpenGL will use when clearing the screen.
		// Values are: Red, Green, Blue, Alpha.
		glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

		//1.13.3
		// Erase the previous frame and fill it with the color above.
		glClear(GL_COLOR_BUFFER_BIT);

		//1.13.3.1 CREATION OF TWO TRIANGLES TO FORM A SQUARE
		// Use the shader program we created.
		glUseProgram(shaderProgram);

		// 1.13.3.1.1
		// Send the MODEL matrix to the shader.
		int modelLocation = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(
			modelLocation,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

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

		//1.13.3.1.2 Two triangles are drawn to form a square. The first triangle is drawn in red, and the second triangle is drawn in blue.
		// Find the shader's shapeColor variable.
		int colorLocation = glGetUniformLocation(shaderProgram, "shapeColor");

		// Set the color to RED.
		glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);

		// Draw vertices 0, 1, and 2.
		// These make Triangle 1.
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Change the color to BLUE.
		glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f);

		// Draw vertices 3, 4, and 5.
		// These make Triangle 2.
		glDrawArrays(GL_TRIANGLES, 3, 3);

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