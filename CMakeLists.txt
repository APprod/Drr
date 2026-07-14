cmake_minimum_required(VERSION 3.14)

cmake_policy(SET CMP0135 NEW)
cmake_policy(SET CMP0091 NEW)
project(app)

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
    endif()
    message("building static")
endif()

FILE(GLOB_RECURSE MyCPPSources ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/include/*.hpp)

add_executable(app ${MyCPPSources})


if (EMSCRIPTEN)
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    )
    #DEPENDS ${PROJECT_NAME}
else()
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    )
    #DEPENDS ${PROJECT_NAME}
endif()

if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang") AND NOT EMSCRIPTEN)
  target_compile_options(app PRIVATE
  $<$<CONFIG:Debug>:-g
    -Wall -Wextra -Wpedantic
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Woverloaded-virtual
    -Wformat=2
    -Wmisleading-indentation
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wnull-dereference
    -Wdouble-promotion
    -Wfloat-conversion
    -Wuseless-cast
    -Werror>
  $<$<CONFIG:Release>:-O3 -Os
    -Wall -Wextra -Wpedantic
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Woverloaded-virtual
    -Wformat=2
    -Wmisleading-indentation
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wnull-dereference
    -Wdouble-promotion
    -Wfloat-conversion
    -Wuseless-cast
    >
  )
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(app PRIVATE -Wchanges-meaning)
  endif()
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

if(IS_STATIC AND NOT EMSCRIPTEN)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_link_options(app PRIVATE -static-libgcc -static-libstdc++)
    endif()
endif()

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

target_include_directories(app PUBLIC include/)
target_include_directories(app PRIVATE "${CMAKE_SOURCE_DIR}/third-party/raylib")

target_link_libraries(app PRIVATE nlohmann_json::nlohmann_json raylib)

# Web Configurations
if (EMSCRIPTEN)
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_link_options(${PROJECT_NAME} 
    PRIVATE -sUSE_GLFW=3 
    PRIVATE --preload-file assets
    PRIVATE -sGL_ENABLE_GET_PROC_ADDRESS=1
    PRIVATE -sWASM=1
    PRIVATE -sMAX_WEBGL_VERSION=2
    PRIVATE -sALLOW_MEMORY_GROWTH=1
    PRIVATE --shell-file ${CMAKE_SOURCE_DIR}/web/minshell.html
    )
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()