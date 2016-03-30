cmake_minimum_required(VERSION 2.6)
project (ngrestcgparser CXX)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB NGRESTCGPARSER_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
FILE(GLOB NGRESTCGPARSER_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

file(COPY ${NGRESTCGPARSER_HEADERS} DESTINATION ${PROJECT_INCLUDE_DIR}/ngrest/codegen/)

add_definitions(-DNGREST_CODEGENPARSER_DLL_EXPORTS)

add_library(ngrestcgparser SHARED ${NGRESTCGPARSER_SOURCES})

target_link_libraries(ngrestcgparser ngrestutils)
