cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_COMPILER "clang++")

project(sandbox LANGUAGES CXX)

add_executable(
  sandbox
  ./src/sandbox/main.cpp
)

add_library(
  fengine STATIC
  ./src/fengine/src/app.cpp
  ./src/fengine/src/fecs.cpp
  ./src/fengine/src/window_sdl.cpp
  ./src/fengine/src/renderer_2d.cpp
  ./src/fengine/src/internal/shader/shader.cpp
)

target_include_directories(
  fengine PUBLIC
  ./src/fengine/include/
  ./src/fengine/src/internal/
)

target_link_libraries(
  fengine PUBLIC
  EnTT::EnTT
  glad
  glm::glm
  SDL3::SDL3
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
  sandbox PRIVATE
  fengine
)
