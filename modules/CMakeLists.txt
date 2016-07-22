include(CheckIncludeFile)

find_program (APXS_PATH apxs)
if (APXS_PATH)
    add_subdirectory(apache2)
endif()

if ("$ENV{NGINX_PATH}" STREQUAL "")
    message(Skipping Nginx module compilation: no Nginx source path provided)
else()
    set(NGINX_SRC_PATH "$ENV{NGINX_PATH}")
    add_subdirectory(nginx)
endif()
