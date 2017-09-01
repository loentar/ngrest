cmake_minimum_required(VERSION 2.6)

project (ngrestjsonbenchmark CXX)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB NGRESTJSONBENCHMARK_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)

add_executable(ngrestjsonbenchmark ${NGRESTJSONBENCHMARK_SOURCES})

set_target_properties(ngrestjsonbenchmark PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TESTS_OUTPUT_DIRECTORY}"
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test.json DESTINATION "${TESTS_OUTPUT_DIRECTORY}/")

target_link_libraries(ngrestjsonbenchmark ngrestutils ngrestjson json-c)
