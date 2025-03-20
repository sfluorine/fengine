cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_COMPILER "clang++")

project(game LANGUAGES CXX)

add_executable(
  game
  ./src/main.cpp
  ./src/window_sdl.cpp
  ./src/renderer_2d.cpp
  ./src/misc/shader.cpp
)

add_library(
  fengine STATIC
  ./src/fengine/app.cpp
  ./src/fengine/fecs.cpp
)

target_include_directories(
  fengine PUBLIC
  ./src/fengine/
)

target_link_libraries(
  fengine PUBLIC
  EnTT::EnTT
)

set(SDL_SHARED OFF)
set(SDL_STATIC ON)
set(SDL_TESTS OFF)
set(SDL_EXAMPLES OFF)

add_subdirectory(./vendor/entt/)
add_subdirectory(./vendor/glad/)
add_subdirectory(./vendor/glm/)
add_subdirectory(./vendor/SDL3/)

target_link_libraries(
  game PRIVATE
  fengine
  glad
  glm::glm
  SDL3::SDL3
)
