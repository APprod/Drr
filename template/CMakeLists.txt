cmake_minimum_required(VERSION 3.14)

cmake_policy(SET CMP0135 NEW)
cmake_policy(SET CMP0091 NEW)
project(ApProjects)

option(IS_STATIC "option static" OFF)

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
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        if(MINGW)
            add_link_options(-static)
        else()
            add_link_options(-static-libgcc -static-libstdc++)
        endif()
    endif()
    message("building static")
endif()

# Engine ships its own dependency fetching (raylib/json anchored inside ApEngine/)
add_subdirectory(ApEngine)

