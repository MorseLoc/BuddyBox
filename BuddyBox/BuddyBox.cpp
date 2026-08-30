// ============================================================
// BuddyBox.cpp
//
// Main entry point for the game.
//
// This file is responsible for:
// - Starting GLFW and OpenGL
// - Creating the game window
// - Creating the major game systems
// - Running the main game loop
// - Handling block breaking / placing
// - Updating NPCs
// - Drawing chunk meshes, NPCs, and UI
// - Shutting the game down cleanly
// ============================================================


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <memory>
#include <set>
#include <tuple>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// STB_IMAGE_IMPLEMENTATION must exist in exactly one .cpp file.
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


#include "Block.h"
#include "Player.h"
#include "World.h"
#include "Renderer.h"
#include "Camera.h"
#include "textureManager.h"
#include "inventory.h"
#include "UIRenderer.h"
#include "NPC.h"
#include "NPCRenderer.h"
#include "ChunkMesh.h"


// ============================================================
// Mouse wheel input
// ============================================================

double scrollAmount = 0.0;


void scrollCallback(
	GLFWwindow* window,
	double xOffset,
	double yOffset
)
{
	scrollAmount += yOffset;
}


// ============================================================
// Main
// ============================================================

int main()
{
	// --------------------------------------------------------
	// 1. Start GLFW
	// --------------------------------------------------------

	if (!glfwInit())
	{
		std::cout
			<< "GLFW failed to start.\n";

		return -1;
	}


	// --------------------------------------------------------
	// 2. Create window
	// --------------------------------------------------------

	GLFWwindow* window =
		glfwCreateWindow(
			800,
			600,
			"BuddyBox",
			nullptr,
			nullptr
		);


	if (!window)
	{
		std::cout
			<< "Window creation failed.\n";

		glfwTerminate();

		return -1;
	}


	glfwMakeContextCurrent(
		window
	);


	glfwSwapInterval(
		1
	);


	glfwSetInputMode(
		window,
		GLFW_CURSOR,
		GLFW_CURSOR_DISABLED
	);


	glfwSetScrollCallback(
		window,
		scrollCallback
	);


	// --------------------------------------------------------
	// 3. Load OpenGL
	// --------------------------------------------------------

	if (!gladLoadGLLoader(
		(GLADloadproc)glfwGetProcAddress
	))
	{
		std::cout
			<< "GLAD failed to start.\n";

		glfwDestroyWindow(
			window
		);

		glfwTerminate();

		return -1;
	}


	// --------------------------------------------------------
	// 4. Create major game systems
	// --------------------------------------------------------

	Renderer renderer;

	NPCRenderer npcRenderer;

	UIRenderer uiRenderer;

	TextureManager textureManager;

	Player player;

	World world;

	Camera camera;

	Inventory inventory;


	// Stores every NPC currently alive.
	std::vector<NPC> npcs;


	std::map<
		std::tuple<int, int, int>,
		std::unique_ptr<ChunkMesh>
	> chunkMeshes;

	// --------------------------------------------------------
	// 5. Chunk helper
	// --------------------------------------------------------

	// Converts a world block coordinate into its chunk coordinate.
	//
	// This version also works with negative coordinates.
	auto getChunkCoordinate =
		[](int blockCoordinate)
		{
			if (blockCoordinate >= 0)
			{
				return
					blockCoordinate /
					ChunkMesh::CHUNK_SIZE;
			}

			return
				(
					blockCoordinate -
					(ChunkMesh::CHUNK_SIZE - 1)
					)
				/
				ChunkMesh::CHUNK_SIZE;
		};


	// Rebuilds all chunk meshes from the current World data.
	//
	// For now this is deliberately simple.
	// Later we can rebuild only the chunk that changed.
	auto rebuildAllChunks =
		[&]()
		{
			// Delete the old GPU chunk meshes.
			chunkMeshes.clear();


			// Find all chunks that currently contain blocks.
			std::set<
				std::tuple<int, int, int>
			> usedChunks;


			for (const auto& entry : world.blocks)
			{
				int blockX =
					std::get<0>(
						entry.first
					);

				int blockY =
					std::get<1>(
						entry.first
					);

				int blockZ =
					std::get<2>(
						entry.first
					);


				int chunkX =
					getChunkCoordinate(
						blockX
					);

				int chunkY =
					getChunkCoordinate(
						blockY
					);

				int chunkZ =
					getChunkCoordinate(
						blockZ
					);


				usedChunks.insert(
					std::make_tuple(
						chunkX,
						chunkY,
						chunkZ
					)
				);
			}


			// Build one mesh for each used chunk.
			for (
				const auto& chunkPosition :
				usedChunks
				)
			{
				int chunkX =
					std::get<0>(
						chunkPosition
					);

				int chunkY =
					std::get<1>(
						chunkPosition
					);

				int chunkZ =
					std::get<2>(
						chunkPosition
					);


				auto chunk =
					std::make_unique<
					ChunkMesh
					>();


				chunk->build(
					world,
					chunkX,
					chunkY,
					chunkZ
				);


				chunkMeshes[
					std::make_tuple(
						chunkX,
						chunkY,
						chunkZ
					)
				] = std::move(chunk);
			}
		};

	// --------------------------------------------------------
// Rebuild one specific chunk
// --------------------------------------------------------

	auto rebuildChunk =
		[&](int chunkX, int chunkY, int chunkZ)
		{
			auto chunk =
				std::make_unique<ChunkMesh>();

			chunk->build(
				world,
				chunkX,
				chunkY,
				chunkZ
			);

			chunkMeshes[
				std::make_tuple(
					chunkX,
					chunkY,
					chunkZ
				)
			] = std::move(chunk);
		};


	// --------------------------------------------------------
	// Rebuild the chunk containing a changed block
	// and its six neighboring chunks.
	//
	// Neighbor chunks matter because changing a block along a
	// chunk edge can expose/hide a face in the next chunk.
	// --------------------------------------------------------

	auto rebuildChunksAroundBlock =
		[&](int blockX, int blockY, int blockZ)
		{
			int chunkX =
				getChunkCoordinate(blockX);

			int chunkY =
				getChunkCoordinate(blockY);

			int chunkZ =
				getChunkCoordinate(blockZ);


			// Always rebuild the chunk containing
			// the changed block.
			rebuildChunk(
				chunkX,
				chunkY,
				chunkZ
			);


			// Find where the block sits inside
			// its 16 x 16 x 16 chunk.
			int localX =
				blockX -
				chunkX * ChunkMesh::CHUNK_SIZE;

			int localY =
				blockY -
				chunkY * ChunkMesh::CHUNK_SIZE;

			int localZ =
				blockZ -
				chunkZ * ChunkMesh::CHUNK_SIZE;


			// Only rebuild neighboring chunks when
			// the changed block touches that boundary.

			if (localX == 0)
			{
				rebuildChunk(
					chunkX - 1,
					chunkY,
					chunkZ
				);
			}

			if (localX ==
				ChunkMesh::CHUNK_SIZE - 1)
			{
				rebuildChunk(
					chunkX + 1,
					chunkY,
					chunkZ
				);
			}


			if (localY == 0)
			{
				rebuildChunk(
					chunkX,
					chunkY - 1,
					chunkZ
				);
			}

			if (localY ==
				ChunkMesh::CHUNK_SIZE - 1)
			{
				rebuildChunk(
					chunkX,
					chunkY + 1,
					chunkZ
				);
			}


			if (localZ == 0)
			{
				rebuildChunk(
					chunkX,
					chunkY,
					chunkZ - 1
				);
			}

			if (localZ ==
				ChunkMesh::CHUNK_SIZE - 1)
			{
				rebuildChunk(
					chunkX,
					chunkY,
					chunkZ + 1
				);
			}
		};

	// --------------------------------------------------------
	// 6. Initialize rendering systems
	// --------------------------------------------------------

	if (!uiRenderer.initialize())
	{
		std::cout
			<< "Failed to initialize UI renderer.\n";


		glfwDestroyWindow(
			window
		);

		glfwTerminate();

		return -1;
	}


	glEnable(
		GL_DEPTH_TEST
	);

	// Enable transparency / alpha blending.
	glEnable(
		GL_BLEND
	);


	// Normal alpha blending:
	//
	// finalColor =
	//     source * alpha +
	//     background * (1 - alpha)
	glBlendFunc(
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA
	);


	// --------------------------------------------------------
	// 7. Load textures
	// --------------------------------------------------------

	if (!textureManager.loadAtlas(
		"textures/artdex.png"
	))
	{
		std::cout
			<< "Failed to load artdex.png\n";
	}


	unsigned int blockAtlasTexture =
		textureManager.getAtlasTexture();


	unsigned int scrollWheelTexture =
		textureManager.loadTexture(
			"textures/ScrollWheel.png"
		);


	unsigned int npcAtlasTexture =
		textureManager.loadTexture(
			"textures/NPCdex.png"
		);


	unsigned int shaderProgram =
		renderer.getShaderProgram();


	// --------------------------------------------------------
	// 8. Load inventory
	// --------------------------------------------------------

	if (!inventory.loadFromFile(
		"inventory.txt"
	))
	{
		std::cout
			<< "Failed to load inventory.txt\n";
	}


	// --------------------------------------------------------
	// 9. Load world
	// --------------------------------------------------------

	if (!world.loadFromFile(
		"test.world"
	))
	{
		std::cout
			<< "Failed to load test.world\n";
	}
	else
	{
		std::cout
			<< "Blocks loaded: "
			<< world.blocks.size()
			<< "\n";


		// Build the world's initial chunk meshes.
		rebuildAllChunks();


		std::cout
			<< "Chunk meshes built: "
			<< chunkMeshes.size()
			<< "\n";
	}


	// --------------------------------------------------------
	// 10. Game-loop variables
	// --------------------------------------------------------

	float deltaTime =
		0.0f;


	float lastFrame =
		0.0f;


	bool leftMouseWasPressed =
		false;

	// How long the player has continuously
// held left click while breaking.
	float blockBreakTimer =
		0.0f;

	// 0.0 = just started breaking
// 1.0 = fully broken
	float blockBreakProgress =
		0.0f;

	// Coordinates of the block currently being broken.
	int breakingBlockX = 0;
	int breakingBlockY = 0;
	int breakingBlockZ = 0;


	// true when the player is currently breaking a block.
	bool isBreakingBlock =
		false;

	bool rightMouseWasPressed =
		false;


	// ========================================================
	// 11. Main game loop
	// ========================================================

	while (!glfwWindowShouldClose(
		window
	))
	{
		// ----------------------------------------------------
		// Frame timing
		// ----------------------------------------------------

		float currentFrame =
			static_cast<float>(
				glfwGetTime()
				);


		deltaTime =
			currentFrame -
			lastFrame;


		lastFrame =
			currentFrame;


		if (deltaTime > 0.05f)
		{
			deltaTime =
				0.05f;
		}


		// ----------------------------------------------------
		// Camera update
		// ----------------------------------------------------

		camera.update(
			window
		);


		// ----------------------------------------------------
		// Player update
		// ----------------------------------------------------

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


		// ----------------------------------------------------
// Active block updates
//
// Only blocks that actually have ongoing behavior
// are updated every frame.
// ----------------------------------------------------

		for (const auto& position : world.activeBlocks)
		{
			auto blockIt =
				world.blocks.find(
					position
				);


			// Safety check in case the block no longer exists.
			if (blockIt == world.blocks.end())
			{
				continue;
			}


			Block& block =
				blockIt->second;


			int x =
				std::get<0>(
					position
				);

			int y =
				std::get<1>(
					position
				);

			int z =
				std::get<2>(
					position
				);


			block.update(
				deltaTime,

				glm::vec3(
					static_cast<float>(x),
					static_cast<float>(y),
					static_cast<float>(z)
				),

				npcs
			);
		}


		// ----------------------------------------------------
		// NPC updates
		// ----------------------------------------------------

		for (NPC& npc : npcs)
		{
			npc.update(
				deltaTime,
				world
			);
		}


		// ----------------------------------------------------
		// Mouse input
		// ----------------------------------------------------

		bool leftMousePressed =
			glfwGetMouseButton(
				window,
				GLFW_MOUSE_BUTTON_LEFT
			)
			==
			GLFW_PRESS;

		bool rightMousePressed =
			glfwGetMouseButton(
				window,
				GLFW_MOUSE_BUTTON_RIGHT
			)
			==
			GLFW_PRESS;


		// ----------------------------------------------------
// Break block
// ----------------------------------------------------

		if (leftMousePressed)
		{
			int hitX;
			int hitY;
			int hitZ;

			int previousX;
			int previousY;
			int previousZ;


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
				// ------------------------------------------------
				// Start breaking a block
				// ------------------------------------------------

				if (!isBreakingBlock)
				{
					breakingBlockX =
						hitX;

					breakingBlockY =
						hitY;

					breakingBlockZ =
						hitZ;


					blockBreakTimer =
						0.0f;

					blockBreakProgress =
						0.0f;


					isBreakingBlock =
						true;
				}


				// ------------------------------------------------
				// Player aimed at a different block
				// ------------------------------------------------

				if (
					hitX != breakingBlockX ||
					hitY != breakingBlockY ||
					hitZ != breakingBlockZ
					)
				{
					breakingBlockX =
						hitX;

					breakingBlockY =
						hitY;

					breakingBlockZ =
						hitZ;


					// Restart breaking progress.
					blockBreakTimer =
						0.0f;

					blockBreakProgress =
						0.0f;
				}


				// ------------------------------------------------
				// Count breaking time
				// ------------------------------------------------

				blockBreakTimer +=
					deltaTime;


				// Find the block currently being broken.
				auto blockIt =
					world.blocks.find(
						std::make_tuple(
							hitX,
							hitY,
							hitZ
						)
					);


				if (blockIt != world.blocks.end())
				{
					Block& block =
						blockIt->second;


					// How long this particular block
					// takes to break.
					float requiredBreakTime =
						block.durability *
						player.breakSpeed;


					// Convert breaking time into a
					// value from 0.0 to 1.0.
					blockBreakProgress =
						blockBreakTimer /
						requiredBreakTime;


					// Safety clamp.
					if (blockBreakProgress > 1.0f)
					{
						blockBreakProgress =
							1.0f;
					}


					// ------------------------------------------------
					// Block finished breaking
					// ------------------------------------------------

					if (
						blockBreakTimer >=
						requiredBreakTime
						)
					{
						world.removeBlock(
							hitX,
							hitY,
							hitZ
						);


						rebuildChunksAroundBlock(
							hitX,
							hitY,
							hitZ
						);


						// Start fresh for the next block.
						blockBreakTimer =
							0.0f;

						blockBreakProgress =
							0.0f;

						isBreakingBlock =
							false;
					}
				}
			}
			else
			{
				// ------------------------------------------------
				// Mouse is held, but no block is targeted
				// ------------------------------------------------

				blockBreakTimer =
					0.0f;

				blockBreakProgress =
					0.0f;

				isBreakingBlock =
					false;
			}
		}
		else
		{
			// ------------------------------------------------
			// Left mouse button released
			// ------------------------------------------------

			blockBreakTimer =
				0.0f;

			blockBreakProgress =
				0.0f;

			isBreakingBlock =
				false;
		}


		// ----------------------------------------------------
		// Place block
		// ----------------------------------------------------


		// ----------------------------------------------------
		// Place block
		// ----------------------------------------------------

		if (
			rightMousePressed &&
			!rightMouseWasPressed
			)
		{
			int hitX;
			int hitY;
			int hitZ;

			int previousX;
			int previousY;
			int previousZ;


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
				Block block(
					inventory.getSelectedBlockType()
				);


				glm::vec3 playerMin =
					player.position -
					(player.size / 2.0f);


				glm::vec3 playerMax =
					player.position +
					(player.size / 2.0f);


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


				bool overlapsPlayer =
					playerMax.x > blockMin.x &&
					playerMin.x < blockMax.x &&

					playerMax.y > blockMin.y &&
					playerMin.y < blockMax.y &&

					playerMax.z > blockMin.z &&
					playerMin.z < blockMax.z;


				if (!overlapsPlayer)
				{
					world.placeBlock(
						previousX,
						previousY,
						previousZ,
						block
					);


					rebuildChunksAroundBlock(
						previousX,
						previousY,
						previousZ
					);
				}
			}
		}


		leftMouseWasPressed =
			leftMousePressed;


		rightMouseWasPressed =
			rightMousePressed;


		// ----------------------------------------------------
		// Camera matrices
		// ----------------------------------------------------

		glm::mat4 view =
			camera.getViewMatrix();


		int windowWidth;
		int windowHeight;


		glfwGetFramebufferSize(
			window,
			&windowWidth,
			&windowHeight
		);


		if (windowHeight == 0)
		{
			windowHeight =
				1;
		}


		glViewport(
			0,
			0,
			windowWidth,
			windowHeight
		);


		glm::mat4 projection =
			glm::perspective(
				glm::radians(
					45.0f
				),

				static_cast<float>(
					windowWidth
					)
				/
				static_cast<float>(
					windowHeight
					),

				0.1f,
				100.0f
			);


		// ----------------------------------------------------
		// Clear frame
		// ----------------------------------------------------

		glClearColor(
			0.42f,
			0.75f,
			1.0f,
			1.0f
		);


		glClear(
			GL_COLOR_BUFFER_BIT |
			GL_DEPTH_BUFFER_BIT
		);


		// ----------------------------------------------------
		// Prepare world rendering
		// ----------------------------------------------------

		glUseProgram(
			shaderProgram
		);


		glActiveTexture(
			GL_TEXTURE0
		);


		glBindTexture(
			GL_TEXTURE_2D,
			blockAtlasTexture
		);


		int textureLocation =
			glGetUniformLocation(
				shaderProgram,
				"blockTexture"
			);


		glUniform1i(
			textureLocation,
			0
		);


		int useSolidColorLocation =
			glGetUniformLocation(
				shaderProgram,
				"useSolidColor"
			);


		glUniform1i(
			useSolidColorLocation,
			0
		);


		int modelLocation =
			glGetUniformLocation(
				shaderProgram,
				"model"
			);


		int viewLocation =
			glGetUniformLocation(
				shaderProgram,
				"view"
			);


		int projectionLocation =
			glGetUniformLocation(
				shaderProgram,
				"projection"
			);


		int atlasRowsLocation =
			glGetUniformLocation(
				shaderProgram,
				"atlasRows"
			);


		int useVertexTextureRowLocation =
			glGetUniformLocation(
				shaderProgram,
				"useVertexTextureRow"
			);


		// ----------------------------------------------------
		// Camera uniforms
		// ----------------------------------------------------

		glUniformMatrix4fv(
			viewLocation,
			1,
			GL_FALSE,
			glm::value_ptr(
				view
			)
		);


		glUniformMatrix4fv(
			projectionLocation,
			1,
			GL_FALSE,
			glm::value_ptr(
				projection
			)
		);


		glUniform1f(
			atlasRowsLocation,

			static_cast<float>(
				textureManager.getBlockCount()
				)
		);


		// ----------------------------------------------------
		// Chunk model matrix
		// ----------------------------------------------------

		// Chunk vertices are already stored in world space,
		// so they do not need individual translation matrices.
		glm::mat4 chunkModel =
			glm::mat4(
				1.0f
			);


		glUniformMatrix4fv(
			modelLocation,
			1,
			GL_FALSE,
			glm::value_ptr(
				chunkModel
			)
		);


		// Chunk vertices contain their own texture-row data.
		glUniform1i(
			useVertexTextureRowLocation,
			1
		);


		// ----------------------------------------------------
		// Draw world chunk meshes
		// ----------------------------------------------------

		for (const auto& entry : chunkMeshes)
		{
			entry.second->draw();
		}

		// ----------------------------------------------------
// Breaking block visual
// ----------------------------------------------------

		if (isBreakingBlock)
		{
			// The overlay becomes darker as the block
			// gets closer to breaking.
			float overlayOpacity =
				blockBreakProgress *
				0.55f;


			renderer.drawColoredCube(
				glm::vec3(
					static_cast<float>(breakingBlockX),
					static_cast<float>(breakingBlockY),
					static_cast<float>(breakingBlockZ)
				),

				// Slightly larger than the real block
				// so the surfaces do not fight each other.
				glm::vec3(
					1.01f,
					1.01f,
					1.01f
				),

				// Black overlay.
				glm::vec3(
					0.0f,
					0.0f,
					0.0f
				),

				0.0f,

				overlayOpacity
			);
		}

		// ----------------------------------------------------
		// Draw NPCs
		// ----------------------------------------------------

		for (const NPC& npc : npcs)
		{
			npcRenderer.drawNPC(
				npc,
				renderer,
				npcAtlasTexture
			);
		}


		// ----------------------------------------------------
		// Draw UI
		// ----------------------------------------------------

		uiRenderer.drawHotbar(
			scrollWheelTexture,
			blockAtlasTexture,
			inventory.getSelectedSlot(),
			inventory,
			textureManager.getBlockCount()
		);


		uiRenderer.drawCrosshair();


		// ----------------------------------------------------
		// Finish frame
		// ----------------------------------------------------

		glfwSwapBuffers(
			window
		);


		glfwPollEvents();


		// ----------------------------------------------------
		// Mouse-wheel hotbar selection
		// ----------------------------------------------------

		if (scrollAmount > 0.0)
		{
			inventory.cycleSlot(
				-1
			);
		}
		else if (scrollAmount < 0.0)
		{
			inventory.cycleSlot(
				1
			);
		}


		scrollAmount =
			0.0;
	}


	// ========================================================
	// 12. Shutdown
	// ========================================================

	// Delete chunk OpenGL objects while the OpenGL context
	// still exists.
	chunkMeshes.clear();


	glfwDestroyWindow(
		window
	);


	glfwTerminate();


	return 0;
}