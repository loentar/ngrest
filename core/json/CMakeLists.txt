cmake_minimum_required(VERSION 2.6)
project (ngrestjson CXX)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB NGRESTJSON_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
FILE(GLOB NGRESTJSON_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

file(COPY ${NGRESTJSON_HEADERS} DESTINATION ${PROJECT_INCLUDE_DIR}/ngrest/json/)

add_library(ngrestjson SHARED ${NGRESTJSON_SOURCES})

target_compile_features(ngrestjson PRIVATE cxx_range_for)
target_link_libraries(ngrestjson ngrestutils ngrestcommon)
