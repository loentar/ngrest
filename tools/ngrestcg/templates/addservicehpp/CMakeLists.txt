cmake_minimum_required(VERSION 2.6)

project ($($service.!tolower) CXX)

set($($service.!toupper)_HEADERS $($service).hpp)

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set($($service.!toupper)_CODEGEN_DIR "${PROJECT_BINARY_DIR}/codegen")

PREPEND($($service.!toupper)_HEADERS_PATHS ${PROJECT_SOURCE_DIR} ${$($service.!toupper)_HEADERS})

CODEGEN_FILES($($service.!toupper)_CODEGEN_SOURCES ${$($service.!toupper)_CODEGEN_DIR} ${$($service.!toupper)_HEADERS})

add_custom_command(OUTPUT ${$($service.!toupper)_CODEGEN_SOURCES}
    COMMAND ${NGREST_BIN_PATH}ngrestcg -i "${PROJECT_SOURCE_DIR}" -o ${$($service.!toupper)_CODEGEN_DIR} -t service ${$($service.!toupper)_HEADERS}
    DEPENDS ${$($service.!toupper)_HEADERS_PATHS}
)

file(GLOB $($service.!toupper)_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.hpp)

list(APPEND $($service.!toupper)_SOURCES ${$($service.!toupper)_CODEGEN_SOURCES})

include_directories(${PROJECT_SOURCE_DIR})

add_library($($service.!tolower) MODULE ${$($service.!toupper)_SOURCES})

set_target_properties($($service.!tolower) PROPERTIES PREFIX "")
set_target_properties($($service.!tolower) PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SERVICES_DIR}"
)

target_link_libraries($($service.!tolower) ngrestutils ngrestcommon ngrestjson ngrestengine)
