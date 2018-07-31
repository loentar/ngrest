cmake_minimum_required (VERSION 2.8.4)
project ($(project.name))

include(CheckCXXCompilerFlag)

check_cxx_compiler_flag(-std=gnu++11 HAS_CXX11)
if (HAS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
endif()

if (NOT APPLE)
    check_cxx_compiler_flag(-Wl,--no-undefined HAS_NO_UNDEFINED)
    if (HAS_NO_UNDEFINED)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-undefined")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
    endif()
endif()

check_cxx_compiler_flag(-pedantic HAS_PEDANTIC)
if (HAS_PEDANTIC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
endif()

check_cxx_compiler_flag(-Wall HAS_WALL)
if (HAS_WALL)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

if (CMAKE_BUILD_TYPE MATCHES "DEBUG")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
endif()

set(PROJECT_DEPLOY_DIR ${PROJECT_BINARY_DIR}/deploy)
set(PROJECT_INCLUDE_DIR ${PROJECT_DEPLOY_DIR}/include)
set(PROJECT_SHARE_DIR ${PROJECT_DEPLOY_DIR}/share/ngrest)
set(PROJECT_SERVICES_DIR ${PROJECT_SHARE_DIR}/services)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_DEPLOY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_DEPLOY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_DEPLOY_DIR}/bin)

FUNCTION(PREPEND var prefix)
   SET(listVar "")
   FOREACH(f ${ARGN})
      LIST(APPEND listVar "${prefix}/${f}")
   ENDFOREACH(f)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)

FUNCTION(CODEGEN_FILES outVar prefix)
   SET(listVar "${prefix}/ServiceGroupImpl.h" "${prefix}/ServiceGroupImpl.cpp")
   FOREACH(file ${ARGN})
      get_filename_component(fileBase "${file}" NAME_WE)
      LIST(APPEND listVar "${prefix}/${fileBase}Wrapper.h")
      LIST(APPEND listVar "${prefix}/${fileBase}Wrapper.cpp")
   ENDFOREACH(file)
   SET(${outVar} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(CODEGEN_FILES)

include_directories("${PROJECT_INCLUDE_DIR}")

if (NOT "$ENV{NGREST_HOME}" STREQUAL "")
  set(NGREST_HOME "$ENV{NGREST_HOME}")
else()
  set(NGREST_HOME "$ENV{HOME}/.ngrest/ngrest-build/deploy")
endif()

include_directories("${NGREST_HOME}/include")
if (MINGW)
    link_directories("${NGREST_HOME}/bin")
else()
    link_directories("${NGREST_HOME}/lib")
endif()

set(NGREST_BIN_PATH "${NGREST_HOME}/bin/")

# services deployed from this project

##include <addprojectservice/CMakeLists.txt>
