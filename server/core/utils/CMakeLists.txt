cmake_minimum_required(VERSION 2.6)
project (ngrestutils CXX)

set (CMAKE_MACOSX_RPATH 1)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB NGRESTUTILS_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
FILE(GLOB NGRESTUTILS_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

file(COPY ${NGRESTUTILS_HEADERS} DESTINATION ${PROJECT_INCLUDE_DIR}/ngrest/utils/)

add_definitions(-DNGREST_UTILS_DLL_EXPORTS)

add_library(ngrestutils SHARED ${NGRESTUTILS_SOURCES})

if (HAS_DL)
    target_link_libraries(ngrestutils dl)
endif()
