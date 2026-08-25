# Creates a ready executable following the ApEngine project layout:
#   <dir>/include, <dir>/src, optional <dir>/assets (overrides engine assets)
# Usage: add_ap_project(mygame)
# Requires ApEngine targets (call after add_subdirectory of the engine).

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

    # Engine assets form the base layer, app assets overwrite them.
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
endfunction()
