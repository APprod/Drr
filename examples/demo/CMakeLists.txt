
FILE(GLOB_RECURSE MyCPPSources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
add_executable(demo ${MyCPPSources})

target_include_directories(demo PRIVATE include/)


target_link_libraries(demo PRIVATE ApEngine)


if(CMAKE_CONFIGURATION_TYPES)
    set(ASSETS_DIR "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/assets")
else()
    set(ASSETS_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets")
endif()

add_custom_target(copy-assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "${ASSETS_DIR}"
    COMMENT "Copying assets to build directory"
)
add_dependencies(demo copy-assets)

# Web Configurations
if (EMSCRIPTEN)
    # Tell Emscripten to build an example.html file.
    set_target_properties(demo PROPERTIES SUFFIX ".html")
    target_link_options(demo 
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
    target_link_libraries(demo "-framework IOKit")
    target_link_libraries(demo "-framework Cocoa")
    target_link_libraries(demo "-framework OpenGL")
endif()