cmake_minimum_required(VERSION 2.6)
project (ngrestcommon CXX)

set (CMAKE_MACOSX_RPATH 1)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB NGRESTCOMMON_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
FILE(GLOB NGRESTCOMMON_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

file(COPY ${NGRESTCOMMON_HEADERS} DESTINATION ${PROJECT_INCLUDE_DIR}/ngrest/common/)

add_definitions(-DNGREST_COMMON_DLL_EXPORTS)

add_library(ngrestcommon SHARED ${NGRESTCOMMON_SOURCES})

target_link_libraries(ngrestcommon ngrestutils)
