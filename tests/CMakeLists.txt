project(moro8-tests C)

include(ExternalProject)

# CMocka
set(CMOCKA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/cmocka")
set(CMOCKA_INCLUDE_DIR "${CMOCKA_SOURCE_DIR}/include")
set(CMOCKA_BINARY_DIR "${CMOCKA_SOURCE_DIR}/build")

ExternalProject_Add(cmocka
  PREFIX cmocka
  SOURCE_DIR ${CMOCKA_SOURCE_DIR}
  BINARY_DIR ${CMOCKA_BINARY_DIR}
  INSTALL_COMMAND ""
  CMAKE_ARGS
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DHAVE_SNPRINTF=${HAVE_SNPRINTF}
        -DHAVE_VSNPRINTF=${HAVE_VSNPRINTF}
        -DWITH_STATIC_LIB=true
)
ExternalProject_Get_Property(cmocka source_dir binary_dir)

add_library(cmocka_vendor STATIC SHARED IMPORTED)

if(MSVC)
  set(CMOCKA_LIBRARY "${CMOCKA_BINARY_DIR}/src/Release/cmocka-static.lib")
else()
  set(CMOCKA_LIBRARY "${CMOCKA_BINARY_DIR}/src/libcmocka-static.a")
endif()

add_dependencies(cmocka_vendor cmocka)

# libfs
#set(LIBFS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/libfs")
set(LIBFS_SOURCE_DIR "F:/libfs")
set(LIBFS_INCLUDE_DIR "${LIBFS_SOURCE_DIR}")
set(LIBFS_BINARY_DIR "${LIBFS_SOURCE_DIR}/build")

# moro8 tests
set(_TESTS
    test_moro8
    test_opcodes
)

message("cmocka_BINARY_DIR=${cmocka_BINARY_DIR}")
message("CMOCKA_STATIC_LIBRARY=${CMOCKA_STATIC_LIBRARY}")

include_directories(${CMOCKA_INCLUDE_DIR}
                    ${CMAKE_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${LIBFS_INCLUDE_DIR})
                    
foreach(_TEST ${_TESTS})
    add_cmocka_test(moro8-${_TEST}
                    SOURCES ${LIBFS_INCLUDE_DIR}/fs.c ${CMAKE_CURRENT_SOURCE_DIR}/${_TEST}.c
                    COMPILE_OPTIONS ${DEFAULT_C_COMPILE_FLAGS}
                    LINK_LIBRARIES ${CMOCKA_LIBRARY} ${MORO8_STATIC_LIB}
    )
    
    target_include_directories(moro8-${_TEST}
        PRIVATE 
            ${CMAKE_BINARY_DIR}
            ${cmocka_BINARY_DIR}
            ${LIBFS_INCLUDE_DIR})

    # Required for static linking CMocka
    if (WIN32)
    target_compile_options(moro8-${_TEST} PRIVATE "/MD$<$<CONFIG:Release>:>" "/MDd$<$<CONFIG:Debug>:>")
    endif()
endforeach()

# Copy test files
file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR} FILE_PERMISSIONS OWNER_READ OWNER_WRITE)
file(COPY test_opcodes DESTINATION ${CMAKE_CURRENT_BINARY_DIR} FILE_PERMISSIONS OWNER_READ OWNER_WRITE)
