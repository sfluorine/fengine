cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_COMPILER "clang++")

project(sandbox LANGUAGES CXX)

add_executable(
  sandbox
  ./sandbox/main.cpp
)

add_library(
  fengine STATIC
  ./engine/src/app.cpp
  ./engine/src/fecs.cpp
  ./engine/src/window_sdl.cpp
  ./engine/src/renderer_2d.cpp
  ./engine/src/internal/shader/shader.cpp
)

target_include_directories(
  fengine PUBLIC
  ./engine/include/
  ./engine/src/internal/
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
