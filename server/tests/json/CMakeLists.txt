cmake_minimum_required(VERSION 2.6)

project (ngrestjsontest CXX)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB NGRESTJSONTEST_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)

add_executable(ngrestjsontest ${NGRESTJSONTEST_SOURCES})

set_target_properties(ngrestjsontest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TESTS_OUTPUT_DIRECTORY}"
)

target_link_libraries(ngrestjsontest ngrestutils ngrestjson)
