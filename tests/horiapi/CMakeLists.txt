project(horiapi_tests LANGUAGES C)
find_package(cmocka REQUIRED)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE "main.c")
target_link_libraries(${PROJECT_NAME} PRIVATE horiapi cmocka::cmocka)
