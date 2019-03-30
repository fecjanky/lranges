cmake_minimum_required(VERSION 3.1.3)

project("header only project template" VERSION 0.1 LANGUAGES CXX)

# Sample project for header only library

add_library(Transform INTERFACE)

set(TRANSFORMLIB_CMAKE_LIB_DIR lib/cmake/Transform)
set(TRANSFORMLIB_CMAKE_INSTALL_INCLUDE_DIR include/Transform)
set(TRANSFORMLIB_CMAKE_INSTALL_SHARE_DIR share/Transform)

target_include_directories(Transform
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${TRANSFORMLIB_CMAKE_INSTALL_INCLUDE_DIR}>
)

# add visualizer to project that uses this project when MSVC is the generator
if(MSVC)
set(NATVIS_FILE Transform.natvis)
target_sources(Transform  
	INTERFACE 
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/visualizer/${NATVIS_FILE}>
		$<INSTALL_INTERFACE:${TRANSFORMLIB_CMAKE_INSTALL_SHARE_DIR}/${NATVIS_FILE}>
	INTERFACE 
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/transform.h>
		$<INSTALL_INTERFACE:${TRANSFORMLIB_CMAKE_INSTALL_INCLUDE_DIR}/transform.h>
)
endif()


include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/TransformConfigVersion.cmake"
    VERSION 0.1
    COMPATIBILITY AnyNewerVersion
)

# use standard fs layout for artifacts
install(TARGETS Transform
    EXPORT TransformTargets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime
)

install(EXPORT TransformTargets DESTINATION ${TRANSFORMLIB_CMAKE_LIB_DIR})

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/TransformConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/TransformConfig.cmake"
    INSTALL_DESTINATION ${TRANSFORMLIB_CMAKE_LIB_DIR}
)

install(FILES "${PROJECT_BINARY_DIR}/TransformConfigVersion.cmake"
              "${PROJECT_BINARY_DIR}/TransformConfig.cmake"
        DESTINATION ${TRANSFORMLIB_CMAKE_LIB_DIR})
		
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION ${TRANSFORMLIB_CMAKE_INSTALL_INCLUDE_DIR})

if(MSVC)
install(FILES  visualizer/${NATVIS_FILE} DESTINATION ${TRANSFORMLIB_CMAKE_INSTALL_SHARE_DIR})
endif()