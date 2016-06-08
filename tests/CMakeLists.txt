cmake_minimum_required(VERSION 2.6)
project (tests)

include(CheckIncludeFileCXX)

add_subdirectory(json)
check_include_file_cxx(json-c/json.h HAS_JSON_C)
if (HAS_JSON_C)
    add_subdirectory(json-benchmark)
endif()
add_subdirectory(deployment)
add_subdirectory(service)

