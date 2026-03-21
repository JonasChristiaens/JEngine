cmake_minimum_required(VERSION 3.26)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(Minigin)

if(EMSCRIPTEN)
  message("Using Emscripten!")
  set(TARGET_NAME minigin_web)
  set(TARGET_PARAM )
else()
  set(TARGET_NAME minigin)
  set(TARGET_PARAM WIN32)
endif()

# =========================
# Executable & Source Files
# =========================
add_executable(${TARGET_NAME} ${TARGET_PARAM}
  Minigin/Font.cpp
  Minigin/GameObject.cpp
  Minigin/Main.cpp
  Minigin/Minigin.cpp
  Minigin/Renderer.cpp
  Minigin/ResourceManager.cpp
  Minigin/Scene.cpp
  Minigin/SceneManager.cpp
  Minigin/Texture2D.cpp
  Minigin/Input/InputManager.cpp
  Minigin/Input/ControllerInput.cpp
  Minigin/Components/TransformComponent.cpp
  Minigin/Components/RenderComponent.cpp
  Minigin/Components/TextComponent.cpp
  Minigin/Components/RotateComponent.cpp
  Minigin/Components/FPSComponent.cpp
  Minigin/Components/CacheExerciseComponent.cpp
  Minigin/Components/SpriteAnimatorComponent.cpp
  Minigin/Components/PlayerAnimatorComponent.cpp
  Minigin/Components/HealthComponent.cpp
  Minigin/Components/ScoreComponent.cpp
  Minigin/Components/CollisionComponent.cpp
  Minigin/Components/PickupComponent.cpp
  Minigin/Commands/MoveCommand.cpp
  Minigin/Commands/ChangeHealthCommand.cpp
  Minigin/Commands/ChangeScoreCommand.cpp
  Minigin/Commands/ResetAchievementsCommand.cpp
  Minigin/Achievements/CSteamAchievements.cpp
  Minigin/Achievements/AchievementObserver.cpp
)

# Enable C++20 features
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)

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
  FetchContent_Declare(
      SDL3_ttf
      GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
      GIT_TAG release-3.2.2
      GIT_SHALLOW TRUE
  )
else()
  FetchContent_Declare(
      SDL3_ttf
      URL https://www.libsdl.org/projects/SDL_ttf/release/SDL3_ttf-devel-3.2.2-VC.zip
  )
endif()

FetchContent_MakeAvailable(SDL3_ttf)

if(NOT EMSCRIPTEN)
  list(PREPEND CMAKE_PREFIX_PATH "${sdl3_ttf_SOURCE_DIR}")
  find_package(SDL3_ttf CONFIG REQUIRED)
endif()

# ----- ImGui -----
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.92.5-docking
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(imgui)

# Compile ImGui as a static library
add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

# Set ImGui include directories
target_include_directories(imgui PUBLIC 
    ${imgui_SOURCE_DIR} 
    ${sdl3_SOURCE_DIR}/include
)

target_link_libraries(imgui PUBLIC SDL3::SDL3)

# ----- ImGui Plot -----
set(IMGUI_INCLUDE_DIR ${imgui_SOURCE_DIR})
FetchContent_Declare(
    imgui_plot
    GIT_REPOSITORY https://github.com/soulthreads/imgui-plot.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(imgui_plot)
target_link_libraries(imgui_plot PUBLIC imgui)

# ==============
# Optional Tools
# ==============
if (WIN32)
  find_package(VLD CONFIG)
endif()

# Add Steamworks API if available
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/steam")
find_package(Steamworks QUIET)

if(Steamworks_FOUND)
  target_compile_definitions(${TARGET_NAME} PUBLIC USE_STEAMWORKS=1)
  target_link_libraries(${TARGET_NAME} PUBLIC Steamworks::Steamworks)
endif()

# ===================
# Include Directories
# ===================
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory 
    "${CMAKE_CURRENT_SOURCE_DIR}/Data" 
    "$<TARGET_FILE_DIR:${TARGET_NAME}>/Data"
  )

target_include_directories("${TARGET_NAME}" PUBLIC 
  ${CMAKE_SOURCE_DIR}/Minigin/
  ${imgui_SOURCE_DIR}
  ${VLD_INCLUDE_DIR}
)

# ==============
# Link Libraries
# ==============
target_link_libraries("${TARGET_NAME}" PUBLIC 
  SDL3::SDL3 
  SDL3_ttf::SDL3_ttf
  glm::glm
  imgui
  imgui_plot
  ${VLD_LIBRARIES}
)

# Link xinput on Windows
if(WIN32)
  target_link_libraries("${TARGET_NAME}" PUBLIC xinput)
endif()

# ====================
# Copy DLL after build
# ====================
if(WIN32)
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:SDL3::SDL3>"
    "$<TARGET_FILE:SDL3_ttf::SDL3_ttf>"
    "$<TARGET_FILE_DIR:${TARGET_NAME}>"
)
if(Steamworks_FOUND AND Steamworks_RUNTIME_DLL)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${Steamworks_RUNTIME_DLL}"
        "$<TARGET_FILE_DIR:${TARGET_NAME}>"
    )
    # Generate steam_appid.txt for development
    file(GENERATE 
        OUTPUT "$<TARGET_FILE_DIR:${TARGET_NAME}>/steam_appid.txt" 
        CONTENT "480"
    )
endif()
endif()

# =================
# Compiler Warnings
# =================
if(MSVC)
  # Compile at warning level 4 and treat warnings as errors
  target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else()
  # Enable all common warnings and treat warnings as errors
  target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Werror)
endif()