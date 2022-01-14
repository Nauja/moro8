cmake_minimum_required(VERSION 3.3)
 
# Specify search path for CMake modules to be loaded by include() 
# and find_package()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

set(MORO8_VERSION_MAJOR 0)
set(MORO8_VERSION_MINOR 1)
set(MORO8_VERSION_PATCH 0)
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
option(MORO8_MINIMALIST "Build a minimalist version of this library" OFF)
option(MORO8_WITH_PARSER "Enable moro8_print and moro8_parse functions" ON)
option(MORO8_WITH_HOOKS "Enable support for hook functions" ON)

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
target_compile_options("${MORO8_LIB}"
                        PRIVATE
                            ${DEFAULT_C_COMPILE_FLAGS}
                            -DHAVE_CONFIG_H)
                               
target_include_directories(${MORO8_LIB}
                            PRIVATE
                                ${CMAKE_CURRENT_BINARY_DIR}
                            PUBLIC
                                ${CMAKE_CURRENT_SOURCE_DIR})
if (MORO8_STATIC)
    
    set(MORO8_STATIC_LIB "${MORO8_LIB}-static")

    add_library(${MORO8_STATIC_LIB} STATIC "${HEADERS}" "${SOURCES}")
    set_target_properties(${MORO8_STATIC_LIB} PROPERTIES OUTPUT_NAME "${MORO8_LIB}")
    set_target_properties(${MORO8_STATIC_LIB} PROPERTIES PREFIX "")

    target_compile_options(${MORO8_STATIC_LIB}
                           PRIVATE
                               ${DEFAULT_C_COMPILE_FLAGS}
                               -DHAVE_CONFIG_H
                               -DMORO8_HIDE_SYMBOLS)
                               
    target_include_directories(${MORO8_STATIC_LIB}
                               PRIVATE
                                   ${CMAKE_CURRENT_BINARY_DIR}
                               PUBLIC
                                    ${CMAKE_CURRENT_SOURCE_DIR})
endif()

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
    add_subdirectory ("docs")
endif (MORO8_DOXYGEN)