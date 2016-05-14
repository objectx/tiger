
cmake_minimum_required (VERSION 3.5)

include_directories ("${TIGER_SOURCE_DIR}/Catch/include")

add_executable (test-tiger "src/main.cpp")
    target_link_libraries (test-tiger tiger)
    target_compile_features (test-tiger PRIVATE cxx_auto_type)

add_test (NAME test-tiger COMMAND test-tiger -r compact)
