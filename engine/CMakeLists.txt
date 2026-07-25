
FILE(GLOB_RECURSE MyCPPSources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
add_library(ApEngine STATIC ${MyCPPSources})

target_include_directories(ApEngine PUBLIC include/)

if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang") AND NOT EMSCRIPTEN)
  target_compile_options( ApEngine PUBLIC
  $<$<CONFIG:Debug>:-g -gdwarf-4 
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
    -Werror
    >
  $<$<CONFIG:Release>:-O3
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
    target_compile_options( ApEngine PUBLIC -Wchanges-meaning)
  endif()
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
