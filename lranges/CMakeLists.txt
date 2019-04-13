cmake_minimum_required(VERSION 3.1.3)

project("header only project template" VERSION 0.1 LANGUAGES CXX)

# Sample project for header only library

add_library(LRanges INTERFACE)

set(LRangesLIB_CMAKE_LIB_DIR lib/cmake/LRanges)
set(LRangesLIB_CMAKE_INSTALL_INCLUDE_DIR include/LRanges)
set(LRangesLIB_CMAKE_INSTALL_SHARE_DIR share/LRanges)

target_include_directories(LRanges
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${LRangesLIB_CMAKE_INSTALL_INCLUDE_DIR}>
)

# add headers to project that uses this project when MSVC is the generator
if(MSVC)
target_sources(LRanges  
    INTERFACE 
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/lranges.h>
        $<INSTALL_INTERFACE:${LRangesLIB_CMAKE_INSTALL_INCLUDE_DIR}/lranges.h>
)
endif()


include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/LRangesConfigVersion.cmake"
    VERSION 0.1
    COMPATIBILITY AnyNewerVersion
)

# use standard fs layout for artifacts
install(TARGETS LRanges
    EXPORT LRangesTargets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime
)

install(EXPORT LRangesTargets DESTINATION ${LRangesLIB_CMAKE_LIB_DIR})

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/LRangesConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/LRangesConfig.cmake"
    INSTALL_DESTINATION ${LRangesLIB_CMAKE_LIB_DIR}
)

install(FILES "${PROJECT_BINARY_DIR}/LRangesConfigVersion.cmake"
              "${PROJECT_BINARY_DIR}/LRangesConfig.cmake"
        DESTINATION ${LRangesLIB_CMAKE_LIB_DIR})

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION ${LRangesLIB_CMAKE_INSTALL_INCLUDE_DIR})

