cmake_minimum_required(VERSION 3.14)

cmake_policy(SET CMP0135 NEW)
cmake_policy(SET CMP0091 NEW)
project(app)

option(IS_STATIC "option static" OFF)
option(PROFILE "option profiling with Tracy" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

if(IS_STATIC AND NOT EMSCRIPTEN)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)
    endif()
    message("building static")
endif()

# FILE(GLOB_RECURSE MyCPPSources ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/include/*.hpp)

# add_executable(app ${MyCPPSources})

if (EMSCRIPTEN)
    set(GRAPHICS GRAPHICS_API_OPENGL_ES3 CACHE STRING "" FORCE)
endif()

#get Raylib

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/_deps-cache" CACHE PATH "" FORCE) # Source files wont be redownloaded
include(FetchContent)
set(FETCHCONTENT_QUIET OFF CACHE BOOL "" FORCE)
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylibLib
    URL https://github.com/raysan5/raylib/archive/refs/tags/6.0.tar.gz
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/third-party/raylib"
    BINARY_DIR "${CMAKE_BINARY_DIR}/_deps/raylib-build"
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
FetchContent_MakeAvailable(raylibLib)

if(TARGET raylib)
    if (EMSCRIPTEN)
        target_compile_options(raylib PRIVATE -w)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(raylib PRIVATE -w)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(raylib PRIVATE -w)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(raylib PRIVATE /W0)
    endif()
endif()
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

if (PROFILE)
    message("adding profiling")
    add_compile_definitions(TRACY_PROFILE=TRACY_PROFILE)
    FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG v0.13.1
        BINARY_DIR "${CMAKE_BINARY_DIR}/_deps/tracy-build"
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/third-party/tracy"
    )
    set(TRACY_ENABLE ON CACHE BOOL "" FORCE)
    set(TRACY_ON_DEMAND ON CACHE BOOL "" FORCE)
    set(TRACY_CALLSTACK ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(tracy)
else()
endif()


add_subdirectory(engine)
add_subdirectory(app)