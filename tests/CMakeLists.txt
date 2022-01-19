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

message("CMOCKA_BINARY_DIR=${CMOCKA_BINARY_DIR}")
message("CMOCKA_LIBRARY=${CMOCKA_LIBRARY}")

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
ExternalProject_Get_Property(libfs source_dir binary_dir)

add_library(libfs_vendor STATIC SHARED IMPORTED)

if(MSVC)
  set(LIBFS_LIBRARY "${LIBFS_BINARY_DIR}/src/Release/libfs-static.lib")
else()
  set(LIBFS_LIBRARY "${LIBFS_BINARY_DIR}/src/liblibfs-static.a")
endif()

add_dependencies(libfs_vendor libfs)

# moro8 tests
set(_TESTS
    test_array_memory
    test_moro8
    test_opcodes
)

include_directories(${CMOCKA_INCLUDE_DIR}
                    ${LIBFS_INCLUDE_DIR}
                    ${CMAKE_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR})
                    
foreach(_TEST ${_TESTS})
    add_cmocka_test(moro8-${_TEST}
                    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${_TEST}.c
                    COMPILE_OPTIONS ${DEFAULT_C_COMPILE_FLAGS}
                    LINK_LIBRARIES ${CMOCKA_LIBRARY} ${LIBFS_LIBRARY} ${MORO8_STATIC_LIB}
    )
    
    target_include_directories(moro8-${_TEST}
        PRIVATE 
            ${CMAKE_BINARY_DIR}
            ${CMOCKA_BINARY_DIR}
            ${LIBFS_BINARY_DIR})

    # Link libfs
    add_dependencies(moro8-${_TEST} libfs)

endforeach()

# Copy test files
file(COPY test_opcodes DESTINATION ${CMAKE_CURRENT_BINARY_DIR} FILE_PERMISSIONS OWNER_READ OWNER_WRITE)
