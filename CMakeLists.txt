cmake_minimum_required(VERSION 3.1.3)

enable_testing()

project("LRanges" VERSION 0.1 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

include(config)
include(catch)

add_subdirectory(lranges)
add_subdirectory(test)

