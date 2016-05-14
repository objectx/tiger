
cmake_minimum_required (VERSION 3.5)

set (SOURCE_FILES Tiger.cpp)
set (HEADER_FILES ${TIGER_SOURCE_DIR}/include/Tiger.hpp)

add_library (tiger ${SOURCE_FILES} ${HEADER_FILES})
    target_include_directories (tiger PUBLIC $<BUILD_INTERFACE:${TIGER_SOURCE_DIR}/include>
                                             $<INSTALL_INTERFACE:include>)
    target_compile_features (tiger PRIVATE cxx_auto_type
                                           cxx_delegating_constructors
                                           cxx_nonstatic_member_init)
