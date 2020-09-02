
cmake_minimum_required (VERSION 3.14)

find_package (doctest REQUIRED)
find_package (fmt REQUIRED)

set (test_ test-${PROJECT_NAME})
add_executable (${test_})
target_link_libraries (${test_} PRIVATE ${PROJECT_NAME} doctest::doctest fmt::fmt)
target_compile_definitions (${test_} PRIVATE DOCTEST_CONFIG_SUPER_FAST_ASSERTS=1)
target_compile_features (${test_} PRIVATE cxx_std_14)

target_sources (${test_}
                PRIVATE default.cpp
                        tiger2.cpp
                        to_string.hpp
                        fixture.hpp
                        main.cpp)
set_source_files_properties (main.cpp PROPERTIES SKIP_PRECOMPILE_HEADERS YES)
target_precompile_headers (${test_}
                           PRIVATE <doctest/doctest.h>
                                   <fmt/format.h>)

add_test (NAME test-tiger COMMAND ${test_} -r compact)
