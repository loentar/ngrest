include(CheckIncludeFile)

find_program (APXS_PATH apxs)
if (APXS_PATH)
    add_subdirectory(apache2)
endif()
