cmake_minimum_required(VERSION 2.6)
project (ngrestcgparser-cpp CXX)

set (CMAKE_MACOSX_RPATH 1)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB NGRESTCGPARSER_CPP_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
FILE(GLOB NGRESTCGPARSER_CPP_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_library(ngrestcgparser-cpp SHARED ${NGRESTCGPARSER_CPP_SOURCES})

set_target_properties(ngrestcgparser-cpp PROPERTIES PREFIX "")
set_target_properties(ngrestcgparser-cpp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SHARE_DIR}/codegen/parsers"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SHARE_DIR}/codegen/parsers"
)

target_link_libraries(ngrestcgparser-cpp ngrestcgparser)
