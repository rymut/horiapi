project(utf8 LANGUAGES C)

add_library(${PROJECT_NAME} INTERFACE include/utf8/utf8.h)
target_include_directories(${PROJECT_NAME} INTERFACE include)
