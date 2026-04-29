cmake_minimum_required(VERSION 3.26)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(JEngine)

option(USE_STEAMWORKS "Enable Steamworks features" OFF)

if(EMSCRIPTEN)
  message("Using Emscripten!")
  set(GAME_TARGET bomberman_web)
  set(GAME_TARGET_PARAM )
else()
  set(GAME_TARGET bomberman)
  set(GAME_TARGET_PARAM WIN32)
endif()

# ============
# FetchContent
# ============
include(FetchContent)

# ----- GLM -----
FetchContent_Declare(
  glm
  URL https://github.com/g-truc/glm/releases/download/1.0.3/glm-1.0.3.zip
)
FetchContent_MakeAvailable(glm)

# ----- SDL3 -----
if(EMSCRIPTEN)
  FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.0
    GIT_SHALLOW TRUE
  )
else()
  FetchContent_Declare(
    SDL3
    URL https://www.libsdl.org/release/SDL3-devel-3.4.0-VC.zip
  )
endif()
FetchContent_MakeAvailable(SDL3)

if(NOT EMSCRIPTEN)
  list(PREPEND CMAKE_PREFIX_PATH "${sdl3_SOURCE_DIR}")
  find_package(SDL3 CONFIG REQUIRED)
endif()

# ----- SDL3_ttf -----
if(EMSCRIPTEN)
  set(SDLTTF_VENDORED ON CACHE BOOL "Use vendored dependencies for SDL_ttf" FORCE)
  set(SDLTTF_USE_VENDORED_FREETYPE ON CACHE BOOL "" FORCE)
  set(SDLTTF_USE_VENDORED_HARFBUZZ ON CACHE BOOL "" FORCE)

  FetchContent_Declare(
    SDL3_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG release-3.2.0
    GIT_SHALLOW TRUE
  )
else()
  FetchContent_Declare(
    SDL3_ttf
    URL https://www.libsdl.org/projects/SDL_ttf/release/SDL3_ttf-devel-3.2.0-VC.zip
  )
endif()
FetchContent_MakeAvailable(SDL3_ttf)

if(NOT EMSCRIPTEN)
  list(PREPEND CMAKE_PREFIX_PATH "${sdl3_ttf_SOURCE_DIR}")
  find_package(SDL3_ttf CONFIG REQUIRED)
endif()

# =========================
# Engine library (Minigin)
# =========================
add_library(minigin STATIC
  # Core
  Minigin/Core/Minigin.cpp

  # Scene
  Minigin/Scene/GameObject.cpp
  Minigin/Scene/Scene.cpp
  Minigin/Scene/SceneManager.cpp

  # Rendering
  Minigin/Rendering/Renderer.cpp
  Minigin/Rendering/Texture2D.cpp

  # Resources
  Minigin/Resources/ResourceManager.cpp
  Minigin/Resources/Font.cpp

  # Input
  Minigin/Input/InputManager.cpp
  Minigin/Input/ControllerInput.cpp

  # Components
  Minigin/Components/TransformComponent.cpp
  Minigin/Components/RenderComponent.cpp
  Minigin/Components/TextComponent.cpp
  Minigin/Components/RotateComponent.cpp
  Minigin/Components/FPSComponent.cpp
  Minigin/Components/CacheExerciseComponent.cpp
  Minigin/Components/SpriteAnimatorComponent.cpp
  Minigin/Components/CollisionComponent.cpp
  Minigin/Components/DelayedEventComponent.cpp

  # Audio
  Minigin/Audio/ServiceLocator.cpp
  Minigin/Audio/SoundServiceSdlMixer.cpp
  Minigin/Audio/AudioEventObserver.cpp
)

target_compile_features(minigin PUBLIC cxx_std_20)

target_include_directories(minigin PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/Minigin
  ${CMAKE_CURRENT_SOURCE_DIR}/Minigin/Core
  ${CMAKE_CURRENT_SOURCE_DIR}/Minigin/Scene
  ${CMAKE_CURRENT_SOURCE_DIR}/Minigin/Components
  ${CMAKE_CURRENT_SOURCE_DIR}/Minigin/Rendering
  ${CMAKE_CURRENT_SOURCE_DIR}/Minigin/Resources
  ${CMAKE_CURRENT_SOURCE_DIR}/Minigin/Input
  ${CMAKE_CURRENT_SOURCE_DIR}/Minigin/Audio
  ${CMAKE_CURRENT_SOURCE_DIR}/Minigin/EventQueue
  ${CMAKE_CURRENT_SOURCE_DIR}/Minigin/Commands
)

target_link_libraries(minigin PUBLIC
  SDL3::SDL3
  SDL3_ttf::SDL3_ttf
  glm::glm
)

# =========================
# Game executable (Bomberman)
# =========================
add_executable(${GAME_TARGET} ${GAME_TARGET_PARAM}
  Bomberman/Main.cpp
  Bomberman/Observers/BombEventObserver.cpp
  Bomberman/Commands/SpawnBombCommand.cpp
  Bomberman/Commands/MoveCommand.cpp
  Bomberman/Commands/ChangeHealthCommand.cpp
  Bomberman/Commands/ChangeScoreCommand.cpp
  Bomberman/Commands/ResetAchievementsCommand.cpp
  Bomberman/Components/PlayerAnimatorComponent.cpp
  Bomberman/Components/HealthComponent.cpp
  Bomberman/Components/ScoreComponent.cpp
  Bomberman/Components/PickupComponent.cpp
  Bomberman/Achievements/AchievementObserver.cpp
  Bomberman/Achievements/CSteamAchievements.cpp
)

target_compile_features(${GAME_TARGET} PUBLIC cxx_std_20)
target_link_libraries(${GAME_TARGET} PRIVATE minigin)
target_include_directories(${GAME_TARGET} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Bomberman
)

if(USE_STEAMWORKS)
  target_compile_definitions(${GAME_TARGET} PRIVATE USE_STEAMWORKS=1)
endif()