cmake_minimum_required(VERSION 2.6)

project (favicon CXX)

set(FAVICON_HEADERS Favicon.h)

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(FAVICON_CODEGEN_DIR "${PROJECT_BINARY_DIR}/codegen")

PREPEND(FAVICON_HEADERS_PATHS ${PROJECT_SOURCE_DIR} ${FAVICON_HEADERS})

CODEGEN_FILES(FAVICON_CODEGEN_SOURCES ${FAVICON_CODEGEN_DIR} ${FAVICON_HEADERS})

add_custom_command(OUTPUT ${FAVICON_CODEGEN_SOURCES}
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ngrestcg -i "${PROJECT_SOURCE_DIR}" -o ${FAVICON_CODEGEN_DIR} -t service ${FAVICON_HEADERS}
    DEPENDS ${FAVICON_HEADERS_PATHS}
)

file(GLOB FAVICON_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)

list(APPEND FAVICON_SOURCES ${FAVICON_CODEGEN_SOURCES})

include_directories(${PROJECT_SOURCE_DIR})

add_library(favicon MODULE ${FAVICON_SOURCES})
if (APPLE) # cmake sets .so extension for modules under mac os x
    set_target_properties(favicon PROPERTIES SUFFIX ".dylib")
endif()

set_target_properties(favicon PROPERTIES PREFIX "")
set_target_properties(favicon PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SERVICES_DIR}"
)

target_link_libraries(favicon ngrestutils ngrestcommon ngrestjson ngrestengine)
