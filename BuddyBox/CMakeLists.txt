# CMakeList.txt : CMake project for BuddyBox, include source and define
# project specific logic here.
#

# Add source to this project's executable.

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(BuddyBox
    "BuddyBox.cpp"
    "BuddyBox.h"
    "Block.cpp"
    "Block.h"
    "player.cpp"
    "player.h"
    "World.cpp"
    "World.h"
    "Renderer.cpp"
    "Renderer.h"
    "camera.cpp"
    "camera.h"
    "textureManager.cpp"
    "textureManager.h"
    "inventory.cpp"
    "inventory.h"
    "UIrenderer.cpp"
    "UIrenderer.h"
    "NPC.cpp"
    "NPC.h"
    "NPCrenderer.cpp"
    "NPCrenderer.h"
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/test.world"
    "${CMAKE_CURRENT_BINARY_DIR}/test.world"
    COPYONLY
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/inventory.txt"
    "${CMAKE_CURRENT_BINARY_DIR}/inventory.txt"
    COPYONLY
)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/textures")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/textures/artdex.png"
    "${CMAKE_CURRENT_BINARY_DIR}/textures/artdex.png"
    COPYONLY
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/textures/ScrollWheel.png"
    "${CMAKE_CURRENT_BINARY_DIR}/textures/ScrollWheel.png"
    COPYONLY
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/textures/NPCdex.png"
    "${CMAKE_CURRENT_BINARY_DIR}/textures/NPCdex.png"
    COPYONLY
)

target_link_libraries(BuddyBox PRIVATE glfw glad::glad glm::glm OpenGL::GL)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET BuddyBox PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
