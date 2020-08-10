cmake_minimum_required(VERSION 3.16)
project(ZipInt_unit_test)

set(CMAKE_CXX_STANDARD 17)


if(UNIX)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra")
ENDIF(UNIX)

include_directories(../)

add_subdirectory(lib/googletest-release-1.10.0)

include_directories(
    lib/googletest-release-1.10.0/googletest/include
    lib/googletest-release-1.10.0/googlemock/include
)

set(SOURCE_TEST
    tests/TestUnsignedDefault.cpp
    tests/TestSignedDefault.cpp
    )

add_executable(ZipInt_unit_test ${SOURCE_TEST})

target_link_libraries(ZipInt_unit_test gtest gtest_main)

