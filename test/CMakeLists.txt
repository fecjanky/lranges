cmake_minimum_required(VERSION 3.1.3)

enable_testing()

project(unit_test CXX)

set(SRC
        src/main.cpp
)


add_executable(sample_test  ${SRC})

if(CMAKE_BUILD_TYPE STREQUAL "COVERAGE")
	target_compile_definitions(sample_test PRIVATE POLY_VECTOR_COVERAGE_BUILD)
endif()

target_link_libraries(sample_test Transform Catch_lib)
set_target_properties(sample_test PROPERTIES LINKER_LANGUAGE CXX)
set_property(TARGET sample_test PROPERTY CXX_STANDARD 14)

target_include_directories(sample_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_test(NAME test_sample COMMAND sample_test WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
