cmake_minimum_required(VERSION 3.24)
project(PingPong)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch Raylib
include(FetchContent)
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG c04e57399acfc860a68ac0c11ff6d1c77da9cfda
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

add_executable(PingPong main.cpp)

# Include the 'header' directory as requested by the user
target_include_directories(PingPong PRIVATE header)
# Also include raylib headers from the fetched content
target_link_libraries(PingPong PRIVATE raylib)
