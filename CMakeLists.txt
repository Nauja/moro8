cmake_minimum_required(VERSION 3.3)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
 
if (MSVC)
list(APPEND MORO8_COMPILE_FLAGS /W4 /WX)
else()
list(APPEND MORO8_COMPILE_FLAGS -Wall -Wextra -pedantic -Werror)
endif()

# Specify search path for CMake modules to be loaded by include() 
# and find_package()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

include(ExternalProject)

set(MORO8_VERSION_MAJOR 0)
set(MORO8_VERSION_MINOR 0)
set(MORO8_VERSION_PATCH 1)
set(MORO8_VERSION "${MORO8_VERSION_MAJOR}.${MORO8_VERSION_MINOR}.${MORO8_VERSION_PATCH}")

#find_package(Git)
if(Git_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} status
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        ERROR_VARIABLE RESULT_STRING
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    string(LENGTH "${RESULT_STRING}" LENGTH_RESULT_STRING)

    if(${LENGTH_RESULT_STRING} EQUAL 0)

        execute_process(
            COMMAND ${GIT_EXECUTABLE} log -1 --format=%H
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_COMMIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        string(SUBSTRING ${GIT_COMMIT_HASH} 0 7 GIT_COMMIT_HASH)
        set(VERSION_HASH ${GIT_COMMIT_HASH} )

        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-list HEAD --count
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE VERSION_REVISION
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

    endif()
endif()

project(moro8 VERSION ${MORO8_VERSION} LANGUAGES C)

include(ConfigureChecks)

option(MORO8_STATIC "Build a static library" ON)
option(MORO8_UNIT_TESTING "Unit Tests Enabled" ON)
option(MORO8_DOXYGEN "Docs Enabled" OFF)
option(MORO8_MINIMALIST "Strip some extra functions" OFF)
option(MORO8_EXTENDED_OPCODES "Enable extended opcodes specific to moro8" ON)
option(MORO8_WITH_PARSER "Enable moro8_print and moro8_parse functions" ON)
option(MORO8_WITH_HANDLERS "Enable support for custom opcode handlers" ON)
option(MORO8_WITH_CLI "Build the command-line interface" ON)

if (MORO8_MINIMALIST)
set(MORO8_UNIT_TESTING OFF)
set(MORO8_DOXYGEN OFF)
set(MORO8_WITH_PARSER OFF)
set(MORO8_WITH_HANDLERS OFF)
set(MORO8_WITH_CLI OFF)
endif(MORO8_MINIMALIST)

if (MORO8_UNIT_TESTING OR MORO8_DOXYGEN OR MORO8_WITH_CLI)
set(MORO8_WITH_PARSER ON)
set(MORO8_WITH_HANDLERS ON)
endif(MORO8_UNIT_TESTING OR MORO8_DOXYGEN OR MORO8_WITH_CLI)

# disallow in-source build
include(MacroEnsureOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build. Please create a separate build directory and run 'cmake /path/to/${PROJECT_NAME} [options]' there.")

message("Building for target : ${CMAKE_SYSTEM_NAME}")
message("PROJECT_VERSION: ${MORO8_VERSION}")
message("VERSION_HASH: ${VERSION_HASH}")
configure_file(moro8.h.cmake ${CMAKE_CURRENT_SOURCE_DIR}/moro8.h)

# moro8
set(MORO8_LIB moro8)

file(GLOB HEADERS moro8.h)
set(SOURCES moro8.c)

add_library("${MORO8_LIB}" SHARED "${HEADERS}" "${SOURCES}")
set_target_properties(${MORO8_LIB} PROPERTIES
        C_STANDARD 90)

target_compile_options("${MORO8_LIB}"
                        PRIVATE
                            ${MORO8_COMPILE_FLAGS}
                            -DHAVE_CONFIG_H
                            -DMORO8_WITH_CLI=Off)
                               
target_include_directories(${MORO8_LIB}
                            PRIVATE
                                ${CMAKE_CURRENT_BINARY_DIR}
                            PUBLIC
                                ${CMAKE_CURRENT_SOURCE_DIR})
if (MORO8_STATIC)
    
    set(MORO8_STATIC_LIB "${MORO8_LIB}-static")

    add_library(${MORO8_STATIC_LIB} STATIC "${HEADERS}" "${SOURCES}")
    set_target_properties(${MORO8_STATIC_LIB} PROPERTIES
            C_STANDARD 90
            OUTPUT_NAME "${MORO8_LIB}"
            PREFIX "")

    target_compile_options(${MORO8_STATIC_LIB}
                           PRIVATE
                               ${MORO8_COMPILE_FLAGS}
                               -DHAVE_CONFIG_H
                               -DMORO8_HIDE_SYMBOLS)
                               
    target_include_directories(${MORO8_STATIC_LIB}
                               PRIVATE
                                   ${CMAKE_CURRENT_BINARY_DIR}
                               PUBLIC
                                    ${CMAKE_CURRENT_SOURCE_DIR})
endif(MORO8_STATIC)

if (MORO8_WITH_CLI OR MORO8_UNIT_TESTING)
# libfs
set(LIBFS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/libfs")
set(LIBFS_INCLUDE_DIR "${LIBFS_SOURCE_DIR}")
set(LIBFS_BINARY_DIR "${LIBFS_SOURCE_DIR}/build")

ExternalProject_Add(libfs
  PREFIX libfs
  SOURCE_DIR ${LIBFS_SOURCE_DIR}
  BINARY_DIR ${LIBFS_BINARY_DIR}
  INSTALL_COMMAND ""
  CMAKE_ARGS
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DLIBFS_STATIC=On
        -DLIBFS_UNIT_TESTING=Off
        -DLIBFS_DOXYGEN=Off
)

add_library(libfs_vendor STATIC SHARED IMPORTED)

if(MSVC)
  set(LIBFS_LIBRARY "${LIBFS_BINARY_DIR}/${CMAKE_CFG_INTDIR}/libfs.lib")
else()
  set(LIBFS_LIBRARY "${LIBFS_BINARY_DIR}/libfs.a")
endif()

add_dependencies(libfs_vendor libfs)

message("LIBFS_BINARY_DIR=${LIBFS_BINARY_DIR}")
message("LIBFS_LIBRARY=${LIBFS_LIBRARY}")
endif(MORO8_WITH_CLI OR MORO8_UNIT_TESTING)

if (MORO8_WITH_CLI AND NOT MORO8_MINIMALIST)
    
    set(MORO8_CLI "moro8-cli")

    add_executable(${MORO8_CLI} "cli.c")
            
    target_compile_options(${MORO8_CLI}
                           PRIVATE
                               ${MORO8_COMPILE_FLAGS}
                               -DLIBFS_STATIC=On)

    target_link_libraries(${MORO8_CLI} PRIVATE ${LIBFS_LIBRARY} ${MORO8_STATIC_LIB})

    target_include_directories(${MORO8_CLI}
        PRIVATE 
            ${LIBFS_INCLUDE_DIR}
            ${LIBFS_BINARY_DIR})

    # Link libfs
    add_dependencies(${MORO8_CLI} libfs)

    set_target_properties(${MORO8_CLI} PROPERTIES
            C_STANDARD 90
            RUNTIME_OUTPUT_NAME moro8)

endif(MORO8_WITH_CLI AND NOT MORO8_MINIMALIST)

# include cmocka
if (MORO8_UNIT_TESTING)
    if (NOT MORO8_STATIC)
        message("Skip unit testing because MORO8_STATIC option is off")

    else()
        include(AddCMockaTest)
        add_subdirectory(tests)

    endif(NOT MORO8_STATIC)
endif (MORO8_UNIT_TESTING)

if (MORO8_DOXYGEN)
    add_subdirectory (docs)
endif (MORO8_DOXYGEN)