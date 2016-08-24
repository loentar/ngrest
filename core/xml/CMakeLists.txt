cmake_minimum_required(VERSION 2.6)
project (ngrestxml CXX)

set (CMAKE_MACOSX_RPATH 1)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB NGRESTXML_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
FILE(GLOB NGRESTXML_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

file(COPY ${NGRESTXML_HEADERS} DESTINATION ${PROJECT_INCLUDE_DIR}/ngrest/xml/)

add_definitions(-DNGREST_XML_DLL_EXPORTS)

add_library(ngrestxml SHARED ${NGRESTXML_SOURCES})

target_link_libraries(ngrestxml ngrestutils)
if (HAS_DL)
    target_link_libraries(ngrestxml dl)
endif()
