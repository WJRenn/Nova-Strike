cmake_minimum_required(VERSION 3.24)
project(NovaStrike LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Fetch and statically link SFML 3.x (no system install needed)
include(FetchContent)
set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON)
FetchContent_MakeAvailable(SFML)

# Source files
set(SOURCES
    src/main.cpp
    src/app/Game.cpp
    src/gameplay/LevelManager.cpp
    src/ui/Leaderboard.cpp
    src/ui/Menu.cpp
    src/ui/MenuInfo.cpp
    src/ui/MenuOptions.cpp
    src/ui/MenuResults.cpp
    src/systems/CollisionSystem.cpp
    src/systems/SaveSystem.cpp
    src/entities/Spaceship.cpp
    src/entities/Bullet.cpp
    src/entities/enemies/Bomb.cpp
    src/entities/enemies/Enemy.cpp
    src/entities/enemies/Invader.cpp
    src/entities/enemies/InvaderTypes.cpp
    src/entities/enemies/Dragon.cpp
    src/entities/enemies/Monster.cpp
    src/entities/pickups/PickupFactory.cpp
)

# Headers
set(HEADERS
    src/app/Game.hpp
    src/config/GameConfig.hpp
    src/core/ResourceManager.hpp
    src/gameplay/LevelManager.hpp
    src/ui/Leaderboard.hpp
    src/ui/Menu.hpp
    src/input/PlayerInput.hpp
    src/systems/CollisionSystem.hpp
    src/systems/SaveSystem.hpp
    src/entities/Entity.hpp
    src/entities/Spaceship.hpp
    src/entities/Bullet.hpp
    src/entities/enemies/Bomb.hpp
    src/entities/enemies/Enemy.hpp
    src/entities/enemies/EnemyType.hpp
    src/entities/enemies/Invader.hpp
    src/entities/enemies/InvaderTypes.hpp
    src/entities/enemies/Dragon.hpp
    src/entities/enemies/Monster.hpp
    src/entities/pickups/Pickup.hpp
    src/entities/pickups/HealthPickup.hpp
    src/entities/pickups/MeteorHazard.hpp
    src/entities/pickups/PiercingPickup.hpp
    src/entities/pickups/ScorePickup.hpp
    src/entities/pickups/PickupFactory.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Allow project headers to be included from the src root instead of using
# fragile directory-relative paths such as ../../ResourceManager.hpp.
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files are UTF-8; tell MSVC not to interpret Chinese comments as GBK.
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

# Link SFML
target_link_libraries(${PROJECT_NAME} PRIVATE 
    SFML::Graphics 
    SFML::Window 
    SFML::System
)

# Copy assets to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# Set executable output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    OUTPUT_NAME "Nova Strike"
)
