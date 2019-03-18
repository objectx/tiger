
cmake_minimum_required (VERSION 3.14)

target_include_directories (${PROJECT_NAME}
                            PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                   $<INSTALL_INTERFACE:include>)
target_sources (${PROJECT_NAME} PUBLIC Tiger.hpp)
