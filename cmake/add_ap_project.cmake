# Creates a ready executable following the ApEngine project layout:
#   <dir>/include, <dir>/src, optional <dir>/assets (overrides engine assets)
# Usage: add_ap_project(mygame)
# Requires ApEngine targets (call after add_subdirectory of the engine).

set(APENGINE_WEB_DIR "${APENGINE_CMAKE_DIR}/../web" CACHE PATH "ApEngine web support files")
function(add_ap_project target)
    FILE(GLOB_RECURSE MyCPPSources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
    add_executable(${target} ${MyCPPSources})

    target_include_directories(${target} PRIVATE include/)
    target_link_libraries(${target} PRIVATE ApEngine)

    if(CMAKE_CONFIGURATION_TYPES)
        set(ASSETS_DIR "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/assets")
    else()
        set(ASSETS_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets")
    endif()

    # Engine assets form the base layer, app assets adds on top of them.
    set(COPY_ARGS "")
    if(EXISTS "${APENGINE_ASSETS_DIR}")
        list(APPEND COPY_ARGS COMMAND ${CMAKE_COMMAND} -E copy_directory "${APENGINE_ASSETS_DIR}" "${ASSETS_DIR}")
    endif()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
        list(APPEND COPY_ARGS COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets" "${ASSETS_DIR}")
    endif()

    list(LENGTH COPY_ARGS copyArgsCount)
    if(copyArgsCount GREATER 0)
        add_custom_target(copy-assets-${target}
            ${COPY_ARGS}
            COMMENT "Copying assets to build directory"
        )
        add_dependencies(${target} copy-assets-${target})
    endif()
    
    # Web Configurations
    if (EMSCRIPTEN)
        # Tell Emscripten to build.
        set_target_properties(${target} PROPERTIES SUFFIX ".html")
        target_link_options(${target} 
        PRIVATE -sUSE_GLFW=3 
        PRIVATE --preload-file assets
        PRIVATE -sGL_ENABLE_GET_PROC_ADDRESS=1
        PRIVATE -sWASM=1
        PRIVATE -sMAX_WEBGL_VERSION=2
        PRIVATE -sALLOW_MEMORY_GROWTH=1
        PRIVATE --shell-file ${APENGINE_WEB_DIR}/minshell.html
        )
    endif()
    # Checks if OSX and links appropriate frameworks (Only required on MacOS)
    if (APPLE)
        target_link_libraries(${target} "-framework IOKit")
        target_link_libraries(${target} "-framework Cocoa")
        target_link_libraries(${target} "-framework OpenGL")
    endif()

endfunction()
