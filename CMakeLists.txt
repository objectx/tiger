#
# Copyright (c) 2016-2019 Masashi Fujita
#

cmake_minimum_required (VERSION 3.14)

project (tiger
         LANGUAGES CXX C
         VERSION 1.0.0.0)

option (FORCE_RUNTIME_BYTEORDER_CHECKING "Dynamically checks the byte-order." OFF)

include (${CMAKE_BINARY_DIR}/conan_paths.cmake)

function (setvar_default var_)
    if (NOT DEFINED ${var_})
        set (${var_} ${ARGN} PARENT_SCOPE)
    endif ()
endfunction ()

setvar_default (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
setvar_default (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
setvar_default (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

enable_testing ()

add_library (${PROJECT_NAME} STATIC)

add_subdirectory (include)
add_subdirectory (src)
add_subdirectory (test)
