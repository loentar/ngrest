cmake_minimum_required(VERSION 2.6)
project (ngrestcgparser-cpp CXX)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB NGRESTCGPARSER_CPP_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
FILE(GLOB NGRESTCGPARSER_CPP_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

file(COPY ${NGRESTCGPARSER_CPP_HEADERS} DESTINATION ${PROJECT_INCLUDE_DIR}/ngrest/utils/)

add_library(ngrestcgparser-cpp SHARED ${NGRESTCGPARSER_CPP_SOURCES})

set_target_properties(ngrestcgparser-cpp PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/codegen/parsers"
)

target_link_libraries(ngrestcgparser-cpp ngrestcgparser)
