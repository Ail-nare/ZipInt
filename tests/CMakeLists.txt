cmake_minimum_required(VERSION 3.16)
project(ZipInt_unit_test)

set(CMAKE_CXX_STANDARD 17)


if(UNIX)
    # Those are warning flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra")

    # This is an optimisation flag
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

    # This is an optimisation flag (may include loss of precision, but doesn't affect ZipInt)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
ENDIF(UNIX)

include_directories(../)
include_directories(../lib/fl-lib/)

add_subdirectory(lib/googletest-release-v1.10.x)

include_directories(
    lib/googletest-release-1.10.x/googletest/include
)

set(SOURCE_TEST
    tests/TestUnsignedDefault.cpp
    tests/TestSignedDefault.cpp
    tests/TestContainer.cpp
    tests/TestRead.cpp
)

add_executable(ZipInt_unit_test ${SOURCE_TEST})

target_link_libraries(ZipInt_unit_test gtest gtest_main)

