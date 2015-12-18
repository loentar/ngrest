cmake_minimum_required (VERSION 2.6)
project (ngrest)

include(CheckCXXCompilerFlag)

check_cxx_compiler_flag(-Wl,--no-undefined HAS_NO_UNDEFINED)
if (HAS_NO_UNDEFINED)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-undefined")
endif()

check_cxx_compiler_flag(-pedantic HAS_PEDANTIC)
if (HAS_PEDANTIC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-undefined")
endif()


set(PROJECT_DEPLOY_DIR ${PROJECT_BINARY_DIR}/deploy)
set(PROJECT_INCLUDE_DIR ${PROJECT_DEPLOY_DIR}/include)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_DEPLOY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_DEPLOY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_DEPLOY_DIR}/bin)

include_directories("${PROJECT_INCLUDE_DIR}")

add_subdirectory(core)
add_subdirectory(tests)

