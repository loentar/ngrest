project (ngrest_nginx_mod CXX)

#set(CMAKE_C_FLAGS "${APXS_CFLAGS} ${CMAKE_C_FLAGS}")
#set(CMAKE_SHARED_LINKER_FLAGS "${APXS_LDFLAGS} ${CMAKE_SHARED_LINKER_FLAGS}")

# remove -Wl,--no-undefined to allow ngx_* symbols be imported from nginx binary
STRING(REGEX REPLACE "-Wl,--no-undefined" "" CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS})

include_directories("${NGINX_SRC_PATH}/src/core" "${NGINX_SRC_PATH}/src/http" "${NGINX_SRC_PATH}/src/http/v2"
    "${NGINX_SRC_PATH}/src/http/modules" "${NGINX_SRC_PATH}/src/event" "${NGINX_SRC_PATH}/src/os/unix"
    "${NGINX_SRC_PATH}/objs")

FILE(GLOB NGREST_NGINX_MOD_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/../share/*.cpp)

add_library(ngrest_nginx_mod SHARED ${NGREST_NGINX_MOD_SOURCES})

set_target_properties(ngrest_nginx_mod PROPERTIES PREFIX "")
set_target_properties(ngrest_nginx_mod PROPERTIES OUTPUT_NAME "mod_ngrest")
set_target_properties(ngrest_nginx_mod PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SHARE_DIR}/modules/nginx"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SHARE_DIR}/modules/nginx"
)

target_link_libraries(ngrest_nginx_mod  ngrestutils ngrestcommon ngrestjson ngrestengine)
