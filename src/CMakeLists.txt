
cmake_minimum_required (VERSION 3.14)

target_sources (${PROJECT_NAME} PRIVATE Tiger.cpp)

target_compile_features (${PROJECT_NAME} PRIVATE cxx_std_14)

if (FORCE_RUNTIME_BYTEORDER_CHECKING)
    target_compile_definitions (${PROJECT_NAME} PRIVATE FORCE_RUNTIME_BYTEORDER_CHECKING=1)
endif ()
