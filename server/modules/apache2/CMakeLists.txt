project (ngrest_apache2_mod CXX)

set (CMAKE_MACOSX_RPATH 1)

execute_process(COMMAND ${APXS_PATH} -q CFLAGS EXTRA_INCLUDES
    OUTPUT_VARIABLE APXS_CFLAGS
)
execute_process(COMMAND ${APXS_PATH} -q LDFLAGS
    OUTPUT_VARIABLE APXS_LDFLAGS
)

STRING(REGEX REPLACE "\n" "" APXS_CFLAGS "${APXS_CFLAGS}")
STRING(REGEX REPLACE ";;" " " APXS_CFLAGS "${APXS_CFLAGS}")
STRING(REGEX REPLACE "\n" "" APXS_LDFLAGS "${APXS_LDFLAGS}")

if (APPLE)
    set(APXS_CFLAGS "${APXS_CFLAGS} -I/usr/include/apr-1")
    set(CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup ${CMAKE_SHARED_LINKER_FLAGS}")
endif ()

set(CMAKE_C_FLAGS "${APXS_CFLAGS} ${CMAKE_C_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${APXS_LDFLAGS} ${CMAKE_SHARED_LINKER_FLAGS}")

# remove -Wl,--no-undefined to allow ap_* symbols be imported from httpd binary
STRING(REGEX REPLACE "-Wl,--no-undefined" "" CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS})

FILE(GLOB NGREST_APACHE2_MOD_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/../share/*.cpp)

add_library(ngrest_apache2_mod SHARED ${NGREST_APACHE2_MOD_SOURCES})

set_target_properties(ngrest_apache2_mod PROPERTIES PREFIX "")
set_target_properties(ngrest_apache2_mod PROPERTIES OUTPUT_NAME "mod_ngrest")
set_target_properties(ngrest_apache2_mod PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SHARE_DIR}/modules/apache2"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SHARE_DIR}/modules/apache2"
)

target_link_libraries(ngrest_apache2_mod ngrestcgparser apr-1 ngrestutils ngrestcommon ngrestjson ngrestengine)
