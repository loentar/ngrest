cmake_minimum_required(VERSION 2.6)

project (crudservice CXX)

set(CRUDSERVICE_HEADERS Crud.h)

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CRUDSERVICE_CODEGEN_DIR "${PROJECT_BINARY_DIR}/codegen")

PREPEND(CRUDSERVICE_HEADERS_PATHS ${PROJECT_SOURCE_DIR} ${CRUDSERVICE_HEADERS})

CODEGEN_FILES(CRUDSERVICE_CODEGEN_SOURCES ${CRUDSERVICE_CODEGEN_DIR} ${CRUDSERVICE_HEADERS})

add_custom_command(OUTPUT ${CRUDSERVICE_CODEGEN_SOURCES}
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ngrestcg -i "${PROJECT_SOURCE_DIR}" -o ${CRUDSERVICE_CODEGEN_DIR} -t service ${CRUDSERVICE_HEADERS}
    DEPENDS ${CRUDSERVICE_HEADERS_PATHS}
)

file(GLOB CRUDSERVICE_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)

list(APPEND CRUDSERVICE_SOURCES ${CRUDSERVICE_CODEGEN_SOURCES})

include_directories(${PROJECT_SOURCE_DIR})

add_library(crudservice MODULE ${CRUDSERVICE_SOURCES})
if (APPLE) # cmake sets .so extension for modules under mac os x
    set_target_properties(crudservice PROPERTIES SUFFIX ".dylib")
endif()

set_target_properties(crudservice PROPERTIES PREFIX "")
set_target_properties(crudservice PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SERVICES_DIR}"
)

target_link_libraries(crudservice ngrestutils ngrestcommon ngrestjson ngrestengine)
