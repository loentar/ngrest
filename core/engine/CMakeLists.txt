cmake_minimum_required(VERSION 2.6)
project (ngrestengine CXX)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB NGRESTENGINE_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
FILE(GLOB NGRESTENGINE_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

file(COPY ${NGRESTENGINE_HEADERS} DESTINATION ${PROJECT_INCLUDE_DIR}/ngrest/engine/)

add_library(ngrestengine SHARED ${NGRESTENGINE_SOURCES})

target_link_libraries(ngrestengine ngrestutils ngrestcommon ngrestjson)
