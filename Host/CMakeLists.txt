cmake_minimum_required(VERSION 3.20)
project(MilandrHostTestingApps)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(fmt EXCLUDE_FROM_ALL)

include_directories(include)
include_directories(ftdi/ftd2xx)
include_directories(ftdi/LibFT4222/inc)

if (WIN32)
    if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
        set(FT4222_DLL "${CMAKE_SOURCE_DIR}/ftdi/LibFT4222/dll/i386/LibFT4222.dll")
        set(FT4222_DLL_POSTFIX "")
        link_directories(ftdi/ftd2xx/i386)
        link_directories(ftdi/LibFT4222/dll/i386)
    else()
        set(FT4222_DLL "${CMAKE_SOURCE_DIR}/ftdi/LibFT4222/dll/amd64/LibFT4222-64.dll")
        set(FT4222_DLL_POSTFIX "-64")
        link_directories(ftdi/ftd2xx/amd64)
        link_directories(ftdi/LibFT4222/dll/amd64)
    endif()
endif()


add_library(lfs STATIC FtdiI2C.cpp FtdiSpi.cpp LFSmart.cpp common.cpp)
target_include_directories(lfs PRIVATE include)
target_include_directories(lfs PRIVATE .)
target_link_libraries(lfs PRIVATE fmt::fmt-header-only)
target_link_libraries(lfs PRIVATE ftd2xx LibFT4222${FT4222_DLL_POSTFIX})
add_custom_command(TARGET lfs POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${FT4222_DLL}"
        "$<TARGET_FILE_DIR:lfs>/.")


set(TARGET whoiam)
add_executable(${TARGET} whoiam.cpp)
target_link_libraries(${TARGET} PRIVATE fmt::fmt-header-only)
target_link_libraries(${TARGET} PRIVATE lfs)


set(TARGET iicwrite)
add_executable(${TARGET} iicwrite.cpp)
target_link_libraries(${TARGET} PRIVATE fmt::fmt-header-only)
target_link_libraries(${TARGET} PRIVATE lfs)

set(TARGET iicread)
add_executable(${TARGET} iicread.cpp)
target_link_libraries(${TARGET} PRIVATE fmt::fmt-header-only)
target_link_libraries(${TARGET} PRIVATE lfs)

set(TARGET iicreset)
add_executable(${TARGET} iicreset.cpp)
target_link_libraries(${TARGET} PRIVATE fmt::fmt-header-only)
target_link_libraries(${TARGET} PRIVATE lfs)


if (BUILD_TESTS)
    add_custom_command(TARGET lfs POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${FT4222_DLL}"
            "$<TARGET_FILE_DIR:lfs>/tests/.")
    enable_testing()
    add_subdirectory(tests)
endif()
