cmake_minimum_required(VERSION 2.6)
project (ngrestserver CXX)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB NGRESTSERVER_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
FILE(GLOB NGRESTSERVER_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_executable(ngrestserver ${NGRESTSERVER_SOURCES})

target_link_libraries(ngrestserver ngrestutils ngrestcommon ngrestjson ngrestengine)
