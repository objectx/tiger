
cmake_minimum_required (VERSION 3.14)

include (cotire OPTIONAL)

find_package (Catch2 REQUIRED)

add_executable (test-${PROJECT_NAME})

target_sources (test-${PROJECT_NAME} PRIVATE main.cpp)
set_source_files_properties (main.cpp PROPERTIES COTIRE_EXCLUDED YES)
target_sources (test-${PROJECT_NAME} PRIVATE default.cpp explicit.cpp put_hex.hpp fixture.hpp)

target_link_libraries (test-${PROJECT_NAME} PRIVATE ${PROJECT_NAME} Catch2::Catch2)
target_compile_features (test-${PROJECT_NAME} PRIVATE cxx_std_14)

if (COMMAND cotire)
    cotire (test-${PROJECT_NAME})
endif ()

add_test (NAME test-tiger COMMAND test-tiger -r compact)
