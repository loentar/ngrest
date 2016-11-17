cmake_minimum_required(VERSION 2.6)

project (ngresttestservice CXX)

set(NGRESTTESTSERVICE_HEADERS TestService.h)

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(NGRESTTESTSERVICE_CODEGEN_DIR "${PROJECT_BINARY_DIR}/codegen")

PREPEND(NGRESTTESTSERVICE_HEADERS_PATHS ${PROJECT_SOURCE_DIR} ${NGRESTTESTSERVICE_HEADERS})

CODEGEN_FILES(NGRESTTESTSERVICE_CODEGEN_SOURCES ${NGRESTTESTSERVICE_CODEGEN_DIR} ${NGRESTTESTSERVICE_HEADERS})

add_custom_command(OUTPUT ${NGRESTTESTSERVICE_CODEGEN_SOURCES}
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ngrestcg -x -i "${PROJECT_SOURCE_DIR}" -o ${NGRESTTESTSERVICE_CODEGEN_DIR} -t service ${NGRESTTESTSERVICE_HEADERS}
    DEPENDS ${NGRESTTESTSERVICE_HEADERS_PATHS}
)

file(GLOB NGRESTTESTSERVICE_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)

list(APPEND NGRESTTESTSERVICE_SOURCES ${NGRESTTESTSERVICE_CODEGEN_SOURCES})

include_directories(${PROJECT_SOURCE_DIR})

add_library(ngresttestservice MODULE ${NGRESTTESTSERVICE_SOURCES})
if (APPLE) # cmake sets .so extension for modules under mac os x
    set_target_properties(ngresttestservice PROPERTIES SUFFIX ".dylib")
endif()

set_target_properties(ngresttestservice PROPERTIES PREFIX "")
set_target_properties(ngresttestservice PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SERVICES_DIR}"
)

target_link_libraries(ngresttestservice ngrestutils ngrestcommon ngrestjson ngrestengine)

add_custom_command(TARGET ngresttestservice POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_CURRENT_SOURCE_DIR}/test_client ${TESTS_OUTPUT_DIRECTORY})

add_custom_command(TARGET ngresttestservice POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_CURRENT_SOURCE_DIR}/test_server_client ${TESTS_OUTPUT_DIRECTORY})
