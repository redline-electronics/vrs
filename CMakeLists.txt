cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(vrs_impl VERSION 0.1.0 LANGUAGES CXX)

# Default to `Release` build type.
if(NOT CMAKE_BUILD_TYPE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Release")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>

    $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
    #$<$<COMPILE_LANGUAGE:CXX>:-Wsuggest-override>

    -Werror
    -Wall
    -Wextra
    #-Wcast-align
    #-Wconversion
    -Wsign-conversion

    -Wshadow
    #-Wlogical-op

    #-Wsuggest-final-types
    #-Wsuggest-final-methods
    #-pedantic
)

include_directories(SYSTEM third_party/spdlog/include)
add_subdirectory(src)
