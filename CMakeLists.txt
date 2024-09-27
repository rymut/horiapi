cmake_minimum_required(VERSION 3.20)

# conan 2.0 required settings
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(CMAKE_CXX_STANDARD 14)
set(CONAN_HOST_PROFILE "auto-cmake")

# download conan provider
set(CONAN_PROVIDER "${CMAKE_BINARY_DIR}/conan_provider.cmake")
set(CONAN_PROVIDER_GITHUB "rymut/cmake-conan")
if(NOT EXISTS "${CONAN_PROVIDER}")
  message(STATUS "Downloading conan_provider.cmake from https://github.com/${CONAN_PROVIDER_GITHUB}")
  file(DOWNLOAD "https://raw.githubusercontent.com/${CONAN_PROVIDER_GITHUB}/develop2/conan_provider.cmake"
                "${CONAN_PROVIDER}"
                TLS_VERIFY ON)
endif()

# configure options
option(HORIAPI_BUILD_TESTS "Build horiapi tests" ON)
option(HORIAPI_BUILD_EXAMPLES "Build horiapi examples" OFF)
option(HORIAPI_BUILD_SHARED_LIBS "Build horiapi as shared library" OFF)

project(horiapi LANGUAGES C)
include("${CONAN_PROVIDER}")
find_package(hidapi REQUIRED)
find_package(cargs REQUIRED)

add_library(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PUBLIC
        "include/horiapi/horiapi.h"
    PRIVATE
        "src/horiapi/hori_command.c"
        "src/horiapi/hori_command.h"
        "src/horiapi/hori_context.c"
        "src/horiapi/hori_context.h"
        "src/horiapi/hori_device.h"
        "src/horiapi/hori_device.c"
        "src/horiapi/hori_device_win32.h"
        "src/horiapi/hori_endian.h"
        "src/horiapi/hori_profile.h"
        "src/horiapi/hori_state.c"
        "src/horiapi/hori_time.h"
        "src/horiapi/hori_version.c"
        "src/horiapi/horiapi.c"
        "src/horiapi/horiapi_reset_xinput.c"
        "src/hori/main.c"
)
target_include_directories(${PROJECT_NAME} PUBLIC "include" "src/horiapi" PRIVATE "include/horiapi")
target_link_libraries(${PROJECT_NAME} PRIVATE hidapi::hidapi)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER $<TARGET_PROPERTY:${PROJECT_NAME},INTERFACE_SOURCES>)

add_executable(hori)
target_sources(hori PRIVATE "src/hori/main.c")
target_link_libraries(hori PRIVATE horiapi cargs::cargs)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/horiapi)

if (HORIAPI_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (HORIAPI_BUILD_TESTS)
    add_subdirectory(tests)
endif()
