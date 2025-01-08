project(hori LANGUAGES C)

find_package(Curses REQUIRED)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
    PRIVATE
    command/gamepad.c
    command/gamepad.h
    command/list.c
    command/list.h
    command/test.c
    command/list.h
    command/validate.c
    command/validate.h
    main.c
    hori_yaml.c
    hori_yaml.h
    hori_yaml_audio.c
    hori_yaml_audio.h
    hori_yaml_boolean.c
    hori_yaml_boolean.h
    hori_yaml_product.c
    hori_yaml_product.h
    hori_yaml_profile.c
    hori_yaml_profile.h
 "hori_yaml_button.h" "command/validate.c" "command/list.h" "command/test.h" "command/validate.h" "command/args.h" "tui/tui.h" "tui/tui.c")
target_link_libraries(${PROJECT_NAME} PRIVATE horiapi horiapi-internal argtable3::argtable3 yaml Curses::Curses)
