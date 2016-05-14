
cmake_minimum_required (VERSION 3.0)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

set (SOURCE_FILES src/Tiger.cpp)
set (HEADER_FILES ${TIGER_SOURCE_DIR}/Tiger/include/Tiger.hpp)

add_library (Tiger ${SOURCE_FILES} ${HEADER_FILES})
    target_include_directories (Tiger PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                             $<INSTALL_INTERFACE:include>)
    target_compile_features (Tiger PRIVATE cxx_auto_type
                                           cxx_delegating_constructors
                                           cxx_nonstatic_member_init)
