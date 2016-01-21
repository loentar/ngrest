cmake_minimum_required(VERSION 2.6)
project (ngrestcg CXX)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB NGRESTCG_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
FILE(GLOB NGRESTCG_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVERSION_FULL=\\\"${NGREST_VERSION}\\\"")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/templates DESTINATION "${PROJECT_CODEGEN_DIR}/")

add_executable(ngrestcg ${NGRESTCG_SOURCES})

target_link_libraries(ngrestcg ngrestutils ngrestjson ngrestxml ngrestcgparser)
