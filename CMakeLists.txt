cmake_minimum_required(VERSION 3.20)

# conan 2.0 required settings
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(CMAKE_C_STANDARD 99)
set(CONAN_HOST_PROFILE "auto-cmake")

# configure options
option(HORIAPI_BUILD_TESTS "Build horiapi tests" ON)
option(HORIAPI_BUILD_EXAMPLES "Build horiapi examples" OFF)
option(HORIAPI_BUILD_SHARED_LIBS "Build horiapi as shared library" OFF)

project(horiapi LANGUAGES C)
include("${CONAN_PROVIDER}")
find_package(hidapi REQUIRED)
find_package(argtable3 REQUIRED)
find_package(yaml REQUIRED)




add_subdirectory(src)


if (HORIAPI_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (HORIAPI_BUILD_TESTS)
    add_subdirectory(tests)
endif()
