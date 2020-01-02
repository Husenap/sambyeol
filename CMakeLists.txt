cmake_minimum_required(VERSION 3.15.0)
set(PROJECT_NAME "sambyeol")
project(${PROJECT_NAME} C CXX)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
message("-- CONFIGURATION TYPES: ${CMAKE_CONFIGURATION_TYPES}")
message("-- CMAKE VERSION: ${CMAKE_VERSION}")

include("cmake/compiler_features.cmake")
include("cmake/compiler_warnings.cmake")
include("cmake/cotire.cmake")
include("cmake/glm.cmake")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "predefined")
set_property(GLOBAL PROPERTY CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory("sambyeol_lib")
add_subdirectory("app")