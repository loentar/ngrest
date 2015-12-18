cmake_minimum_required(VERSION 2.6)

project (ngrestdeployment CXX)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories("${PROJECT_SOURCE_DIR}/../../../core/server/src/")

FILE(GLOB NGRESTDEPLOYMENT_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/../../../core/server/src/*.cpp)

list(REMOVE_ITEM NGRESTDEPLOYMENT_SOURCES ${PROJECT_SOURCE_DIR}/../../../core/server/src/main.cpp)


add_executable(ngrestdeployment ${NGRESTDEPLOYMENT_SOURCES})

target_compile_features(ngrestdeployment PRIVATE cxx_range_for)
target_link_libraries(ngrestdeployment ngrestutils ngrestcommon ngrestjson ngrestengine)
