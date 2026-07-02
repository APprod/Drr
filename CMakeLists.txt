cmake_minimum_required(VERSION 3.14)

cmake_policy(SET CMP0135 NEW)
project(app)

option(IS_STATIC "option static" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)


if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

FILE(GLOB_RECURSE MyCPPSources ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/include/*.hpp)

add_executable(app ${MyCPPSources})


if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
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
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(app PRIVATE
  $<$<CONFIG:Debug>:/W4
    /WX
    /permissive->
  $<$<CONFIG:Release>:/O1
    /Ob2
    /W4
    /permissive->
  )
endif()

if(IS_STATIC)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_link_options(app PRIVATE -static)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set_property(TARGET app PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
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

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(raylib PRIVATE -w)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(raylib PRIVATE /W0)
endif()

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

target_include_directories(app PUBLIC include/)

target_link_libraries(app PRIVATE nlohmann_json::nlohmann_json raylib)


