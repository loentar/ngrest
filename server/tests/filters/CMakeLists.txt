cmake_minimum_required(VERSION 2.6)

project (ngresttestfilter CXX)

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB NGRESTTESTFILTER_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)

include_directories(${PROJECT_SOURCE_DIR})

add_library(ngresttestfilter MODULE ${NGRESTTESTFILTER_SOURCES})

set_target_properties(ngresttestfilter PROPERTIES PREFIX "")
set_target_properties(ngresttestfilter PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_FILTERS_DIR}"
)

target_link_libraries(ngresttestfilter ngrestutils ngrestcommon ngrestengine)
if (APPLE) # cmake sets .so extension for modules under mac os x
    set_target_properties(ngresttestfilter PROPERTIES SUFFIX ".dylib")
endif()
