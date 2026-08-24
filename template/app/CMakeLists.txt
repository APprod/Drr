# Target name follows the app folder, so created projects need no renaming here.
get_filename_component(APP_TARGET ${CMAKE_CURRENT_SOURCE_DIR} NAME)
include(${APENGINE_CMAKE_DIR}/add_ap_project.cmake)
add_ap_project(${APP_TARGET})
