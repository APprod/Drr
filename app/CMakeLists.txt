
FILE(GLOB_RECURSE MyCPPSources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
add_executable(app ${MyCPPSources})

target_include_directories(app PRIVATE include/)


target_link_libraries(app PRIVATE ApEngine)


if(CMAKE_CONFIGURATION_TYPES)
    set(ASSETS_DIR "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/assets")
else()
    set(ASSETS_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets")
endif()

add_custom_target(copy-assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets"
        "${ASSETS_DIR}"
    COMMENT "Copying assets to build directory"
)
add_dependencies(app copy-assets)

if(IS_STATIC AND NOT EMSCRIPTEN)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        if(MINGW)
            target_link_options(app PRIVATE -static)
        else()
            target_link_options(app PRIVATE -static-libgcc -static-libstdc++)
        endif()
    endif()
endif()

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