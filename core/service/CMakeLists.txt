cmake_minimum_required(VERSION 2.6)
project (ngrestservice CXX)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB NGRESTSERVICE_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
FILE(GLOB NGRESTSERVICE_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_executable(ngrestservice ${NGRESTSERVICE_SOURCES})

target_compile_features(ngrestservice PRIVATE cxx_range_for)
target_link_libraries(ngrestservice ngrestutils ngrestjson)
