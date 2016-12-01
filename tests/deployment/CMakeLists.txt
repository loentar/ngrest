cmake_minimum_required(VERSION 2.6)

project (ngrestdeployment CXX)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories("${PROJECT_SOURCE_DIR}/../../../core/server/src/")

FILE(GLOB NGRESTDEPLOYMENT_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/../../../core/server/src/*.cpp)

list(REMOVE_ITEM NGRESTDEPLOYMENT_SOURCES ${PROJECT_SOURCE_DIR}/../../../core/server/src/main.cpp)


add_executable(ngrestdeployment ${NGRESTDEPLOYMENT_SOURCES})

set_target_properties(ngrestdeployment PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TESTS_OUTPUT_DIRECTORY}"
)

target_link_libraries(ngrestdeployment ngrestutils ngrestcommon ngrestjson ngrestengine)

# ws2_32 detection fails on mingw32
if (MINGW)
    target_link_libraries(ngrestdeployment ws2_32)
endif()
