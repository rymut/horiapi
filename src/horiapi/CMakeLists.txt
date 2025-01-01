project(horiapi LANGUAGES C)

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
    PUBLIC
        "../../include/horiapi/horiapi.h"
    PRIVATE
        "hori_alignof.h"
        "hori_assert.h"
        "hori_command.c"
        "hori_command.h"
        "hori_context.c"
        "hori_context.h"
        "hori_device.h"
        "hori_device.c"
        "hori_endian.h"
        "hori_gamepad.c"
        "hori_gamepad.h"
        "hori_macros.h"
        "hori_profile.h"
        "hori_profile.c"
        "hori_state.c"
        "hori_time.h"
        "hori_utf.c"
        "hori_utf.h"
        "hori_version.c"
        "horiapi.c"
        "horiapi_reset_xinput.c"
        "hori_xinput.h"
)
if (WIN32)
target_sources(${PROJECT_NAME}
    PRIVATE
    
        "hori_device_win32.h"
        "hori_device_win32.c"
        "hori_string_win32.c"
        "hori_string_win32.h"
        "hori_time_win32.h"
        "hori_utf_win32.c"
        "hori_xinput_win32.h"
        "hori_xinput_win32.c"
)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE hidapi::hidapi utf8)
target_include_directories(${PROJECT_NAME} PUBLIC ../../include)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER $<TARGET_PROPERTY:${PROJECT_NAME},INTERFACE_SOURCES>)

add_library(${PROJECT_NAME}-internal INTERFACE)
target_include_directories(${PROJECT_NAME}-internal INTERFACE ..)

#include(GNUInstallDirs)
#install(TARGETS ${PROJECT_NAME} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/horiapi)

