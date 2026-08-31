cmake_minimum_required(VERSION 3.14)

cmake_policy(SET CMP0135 NEW)
cmake_policy(SET CMP0091 NEW)

project(ApEngine)

# Exposed to consumers: cmake helpers and bundled assets live at this repo's root.
set(APENGINE_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/../cmake" CACHE INTERNAL "ApEngine cmake helpers")
set(APENGINE_ASSETS_DIR "${CMAKE_CURRENT_LIST_DIR}/../assets" CACHE INTERNAL "ApEngine bundled assets")

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()
if(NOT DEFINED CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 17)
endif()
if(NOT DEFINED CMAKE_CXX_STANDARD_REQUIRED)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
if(NOT DEFINED CMAKE_C_STANDARD_REQUIRED)
    set(CMAKE_C_STANDARD_REQUIRED ON)
endif()

option(PROFILE "option profiling with Tracy" OFF)

if (EMSCRIPTEN)
    set(GRAPHICS GRAPHICS_API_OPENGL_ES3 CACHE STRING "" FORCE)
endif()


# Deps anchor to THIS repo's root (parent of engine/): they live in
# <ApEngine-repo>/third-party no matter who is the configure root.
set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/../_deps-cache" CACHE PATH "" FORCE) # Source files wont be redownloaded
include(FetchContent)
set(FETCHCONTENT_QUIET OFF CACHE BOOL "" FORCE)
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)

if(NOT TARGET raylib)
FetchContent_Declare(
    raylibLib
    URL https://github.com/raysan5/raylib/archive/refs/tags/6.0.tar.gz
    URL_HASH SHA256=2B3EE1E2120C7A0796B33062C7E9A694DD8A8CAA56A96319AC8C8ECF54A90D0B
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../third-party/raylib"
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
endif()

if(NOT TARGET nlohmann_json::nlohmann_json)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)
endif()

if (PROFILE)
    message("adding profiling")
    add_compile_definitions(TRACY_PROFILE=TRACY_PROFILE)
    FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG v0.13.1
        BINARY_DIR "${CMAKE_BINARY_DIR}/_deps/tracy-build"
        SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../third-party/tracy"
    )
    set(TRACY_ENABLE ON CACHE BOOL "" FORCE)
    set(TRACY_ON_DEMAND ON CACHE BOOL "" FORCE)
    set(TRACY_CALLSTACK ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(tracy)
else()
endif()

FILE(GLOB_RECURSE MyCPPSources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
add_library(ApEngine STATIC ${MyCPPSources})

target_include_directories(ApEngine PUBLIC include/)

if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang") AND NOT EMSCRIPTEN)
    target_compile_options(ApEngine PUBLIC
        $<$<CONFIG:Debug>:
            -g -gdwarf-4
            -Wall -Wextra -Wpedantic
            -Wshadow -Wnon-virtual-dtor -Wold-style-cast
            -Wcast-align -Woverloaded-virtual -Wformat=2
            -Wmisleading-indentation -Wnull-dereference
            -Wdouble-promotion -Wfloat-conversion -Werror
        >
        $<$<CONFIG:Release>:
            -O3
            -Wall -Wextra -Wpedantic
            -Wshadow -Wnon-virtual-dtor -Wold-style-cast
            -Wcast-align -Woverloaded-virtual -Wformat=2
            -Wmisleading-indentation -Wnull-dereference
            -Wdouble-promotion -Wfloat-conversion
        >
    )
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(ApEngine PUBLIC
            -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wuseless-cast
            -Wchanges-meaning
        )
  endif()
  target_link_options(ApEngine PUBLIC $<$<CONFIG:Release>:-s>)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options( ApEngine PUBLIC
    $<$<CONFIG:Debug>:
      /W4
      /WX
      /permissive->

    $<$<CONFIG:Release>:
      /O1
      /Ob2
      /W4
      /permissive->

    $<$<AND:$<CONFIG:Release>,$<BOOL:${PROFILE}>>:
      /Zi
      /Zo>
  )

    target_link_options(ApEngine PUBLIC
    $<$<AND:$<CONFIG:Release>,$<BOOL:${PROFILE}>>:
      /DEBUG>
  )
endif()



if (PROFILE)
    target_compile_definitions(ApEngine PUBLIC TRACY_ENABLE)
    target_link_libraries(ApEngine PUBLIC nlohmann_json::nlohmann_json raylib TracyClient)
else()
    target_link_libraries(ApEngine PUBLIC nlohmann_json::nlohmann_json raylib)
endif()
