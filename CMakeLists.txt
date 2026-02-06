cmake_minimum_required(VERSION 3.14)

cmake_policy(SET CMP0135 NEW)
project(app)

option(IS_STATIC "option static" OFF)

set(CMAKE_CXX_STANDARD 17 REQUIRED)
set(CMAKE_C_STANDARD 17 REQUIRED)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

FILE(GLOB_RECURSE MyCPPSources ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/include/*.hpp)

add_executable(app ${MyCPPSources})

target_compile_options(app PRIVATE
  $<$<CONFIG:Debug>:-g
    -Wall
    -Wextra
    -Wpedantic
    -Werror>
  $<$<CONFIG:Release>:-O3 -Os -s
    -Wall
    -Wextra
    -Wpedantic>
)

target_link_options(app PRIVATE
  $<$<CONFIG:Release>:-O3 -Os -s>
)

if(IS_STATIC)
    target_link_options(app PUBLIC "-static")
    message("building static")
endif()

#get Raylib
include(FetchContent)

set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylibLib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/third-party/raylib"
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
FetchContent_MakeAvailable(raylibLib)
target_compile_options(raylib PRIVATE -w)

target_include_directories(app PUBLIC include/)


#target_include_directories(app PUBLIC third-party/FrPr/fwk/include/)
target_link_libraries(app raylib)

