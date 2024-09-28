project(horiapi_tests LANGUAGES C)
find_package(cmocka REQUIRED)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
    PRIVATE
        "hori_internal_parse_firmware_version_tests.c"
        "hori_internal_parse_version_number_tests.c"
        "main.c"
        "tests.h")
target_link_libraries(${PROJECT_NAME} PRIVATE horiapi cmocka::cmocka)
