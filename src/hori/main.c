#include <stdio.h>
#include <Windows.h>

#pragma comment (lib, "Setupapi")
#pragma comment(lib, "Cfgmgr32")
#pragma comment(lib, "Hid")

#include <horiapi/horiapi.h>
#include <argtable3.h>

#include "../horiapi/hori_command.h"

#include "argtable3.h"

#define REG_EXTENDED 1
#define REG_ICASE (REG_EXTENDED << 1)

int main_list() {
    hori_enumeration_t* devices = hori_enumerate(HORI_PRODUCT_ANY, NULL);
    int i = 0;
    for (hori_enumeration_t* device = devices; device != NULL; device = device->next, i++) {
        printf("%d:\n", i);
        printf("\tpath: %s\n", device->path);
        printf("\tmodel: %s\n", device->device_config->firmware_name);
        wprintf(L"\tmanufacturer: %s\n", device->manufacturer_string);
        wprintf(L"\tproduct: %s\n", device->product_string);
        printf("\tcontroller: %d\n", device->device_config->device_config_mode);
        printf("\tstate: %d\n", device->state);
    }
    hori_free_enumerate(devices);
    return EXIT_SUCCESS;
}

int main_set() {
    return EXIT_SUCCESS;
}

int main_get() {
    return EXIT_SUCCESS;
}

#include "../horiapi/hori_time.h"

int main_test(int device_id, int wait_miliseconds) {
    char* device_path = NULL;
    hori_enumeration_t* devices = hori_enumerate(HORI_PRODUCT_ANY, NULL);
    if (devices == NULL) {
        printf("No devices found on the system");
        return EXIT_FAILURE;
    }
    int i = 0;
    for (hori_enumeration_t* device = devices; device != NULL; device = device->next, i++) {
        if (device_id == i) {
            device_path = strdup(device->path);
        }
    }
    hori_free_enumerate(devices);

    if (device_path == 0) {
        printf("Device id %d not found\n", device_id);
        return EXIT_FAILURE;
    }
    hori_device_t* device = hori_open_path(device_path, NULL);
    if (device == NULL) {
        printf("cannot open device %d\n", device_id);
        return EXIT_FAILURE;
    }
    if (hori_get_state(device) != HORI_STATE_CONFIG) {
        hori_set_state(device, HORI_STATE_CONFIG);
    }
    if (hori_get_state(device) != HORI_STATE_CONFIG) {
        hori_close(device);
        return EXIT_FAILURE;
    }
    double wait_seconds = wait_miliseconds < 0 ? -1 : wait_miliseconds / 1000.0;
    hori_clock_t start = hori_clock_now();
    for (double diff = 0; wait_seconds < 0 || diff < wait_seconds; diff = hori_clock_diff(hori_clock_now(), start)) {
        if (-1 == hori_send_heartbeat(device)) {
            printf("cannot send heartbeat\n");
            break;
        }
        printf("check status - %5.2lf\n", diff);
        hori_sleep_ms(100);
    }
    hori_close(device);
    device = NULL;
    return EXIT_SUCCESS;
}

int main_gamepad(int device_id, int wait_miliseconds) {
    char* device_path = NULL;
    hori_enumeration_t* devices = hori_enumerate(HORI_PRODUCT_ANY, NULL);
    if (devices == NULL) {
        printf("No devices found on the system");
        return EXIT_FAILURE;
    }
    int i = 0;
    for (hori_enumeration_t* device = devices; device != NULL; device = device->next, i++) {
        if (device_id == i) {
            device_path = strdup(device->path);
        }
    }
    hori_free_enumerate(devices);

    if (device_path == 0) {
        printf("Device id %d not found\n", device_id);
        return EXIT_FAILURE;
    }
    hori_device_t* device = hori_open_path(device_path, NULL);
    if (device == NULL) {
        printf("cannot open device %d\n", device_id);
        return EXIT_FAILURE;
    }
    if (hori_get_state(device) != HORI_STATE_CONFIG) {
        hori_set_state(device, HORI_STATE_CONFIG);
    }
    if (hori_get_state(device) != HORI_STATE_CONFIG) {
        hori_close(device);
        return EXIT_FAILURE;
    }

    hori_gamepad_t* gamepad = hori_make_gamepad();
    if (gamepad == NULL) {
        hori_close(device);
        return EXIT_FAILURE;
    }
    double wait_seconds = wait_miliseconds < 0 ? -1 : wait_miliseconds / 1000.0;
    hori_clock_t start = hori_clock_now();
    long long previous_buttons = 0;
    for (double diff = 0; wait_seconds < 0 || diff < wait_seconds; diff = hori_clock_diff(hori_clock_now(), start)) {
        // read gampade status
        if (hori_get_state(device) == HORI_STATE_CONFIG) {
            hori_send_heartbeat(device);
        }
        int result = hori_read_gamepad_timeout(device, gamepad, 150);
        if (result == -1) {
            printf("errror\n");
            break;
        }
        int buttons = hori_get_buttons(gamepad, 0);
        if (buttons == -1) {
            break;
        }
        long long allbuttons = buttons;
        buttons = hori_get_buttons(gamepad, 1);
        if (buttons != -1) {
            allbuttons = (allbuttons << sizeof(int) * 8) | buttons;
        }
        if (previous_buttons != allbuttons) {
            previous_buttons = allbuttons;
            printf("gamepad ");
            for (int i = 0; i < sizeof(allbuttons) * 8 - 1; i++) {
                if (i % 8 == 0) {
                    printf(" ");
                }
                printf("%d ", (allbuttons & (1LL << i)) != 0);
            }
            printf("\n");
        }
    }
    hori_free_gamepad(gamepad);
    hori_close(device);
    device = NULL;
    return EXIT_SUCCESS;
}

#include "hori_yaml.h"

int main_validate(const char *input_file) {
    hori_yaml_parse_file(input_file);
    return EXIT_SUCCESS;
}

#define ARG_CMD(cmd) arg_rex1(NULL, NULL, cmd, NULL, REG_ICASE, NULL);
#define ARG_HELP() arg_lit0("h", "help", "show help message")
#define ARG_DEVICE() arg_int1("d", "device", "<device>", "device number")
#define ARG_PROFILE() arg_int0("p", "profile", "<profile>", "profile number")
#define ARG_OUTPUT() arg_file0("o", "output", "<file>", "output file")
#define ARG_INPUT() arg_file1("i", "input", "<file>", "input file")

/** @brief Main entry point

    @param argc The number of elements in @p argv
    @param argv The array containing arguments

    @returns
        EXIT_FAILURE on error, EXIT_SUCCESS otherwise
 */
int main(int argc, char** argv)
{
    const char* progname = "hori";

    // syntax: [-h,--help] [-v,--version]
    struct arg_lit* main_help = ARG_HELP();
    struct arg_lit* main_version = arg_lit0("v", "version", "show version");
    struct arg_end* main_end = arg_end(2);
    void* main_argtable[] = { main_help, main_version, main_end };
    int main_errors = 0;

    // syntax: list [-h,--help]
    struct arg_rex* list_cmd = ARG_CMD("list");
    struct arg_lit* list_help = ARG_HELP();
    struct arg_end* list_end = arg_end(2);
    void* list_argtable[] = { list_cmd, list_help, list_end };
    int list_errors = 0;

    // syntax: get [-h,--help] [-d,--device=<DEVICE>] [-p,--profile=<PROFILE>] [-o,--output=<FILE>]
    struct arg_rex* get_cmd = ARG_CMD("get");
    struct arg_lit* get_help = ARG_HELP();
    struct arg_int* get_device = ARG_DEVICE();
    struct arg_int* get_profile = ARG_PROFILE();
    struct arg_file* get_output = ARG_OUTPUT();
    struct arg_end* get_end = arg_end(20);
    void* get_argtable[] = { get_cmd, get_help, get_device, get_profile, get_output, get_end };
    int get_errors = 0;

    // syntax: set [-h,--help] [-d,--device=<DEVICE>] [-p,--profile=<PROFILE>] [-i,--input=<FILE>]
    struct arg_rex* set_cmd = ARG_CMD("set");
    struct arg_lit* set_help = ARG_HELP();
    struct arg_int* set_device = ARG_DEVICE();
    struct arg_int* set_profile = ARG_PROFILE();
    struct arg_file* set_input = ARG_INPUT();
    struct arg_end* set_end = arg_end(20);
    void* set_argtable[] = { set_cmd, set_help, set_device, set_profile, set_input, set_end };
    int set_errors = 0;

    // syntax: test [-h,--help] [-d,--device=<DEVICE>] [-p,--profile=<PROFILE>] [-w,--wait=<miliseconds>] [-i,--input=<FILE>]
    struct arg_rex* test_cmd = ARG_CMD("test");
    struct arg_lit* test_help = ARG_HELP();
    struct arg_int* test_device = ARG_DEVICE();
    struct arg_int* test_profile = ARG_PROFILE();
    struct arg_int* test_wait = arg_int0("w", "wait", "<MILISECONDS>", "run test for miliseconds after connection (exclude connection time)");
    struct arg_file* test_input = ARG_INPUT();
    struct arg_end* test_end = arg_end(20);
    void* test_argtable[] = { test_cmd, test_help, test_device, test_profile, test_wait, test_input, test_end };
    int test_errors = 0;

    // syntax: gamepad [-h,--help] [-d,--device=<DEVICE>] [-w,--wait=<miliseconds>]
    struct arg_rex* gamepad_cmd = ARG_CMD("gamepad");
    struct arg_lit* gamepad_help = ARG_HELP();
    struct arg_int* gamepad_device = ARG_DEVICE();
    struct arg_int* gamepad_wait = arg_int0("w", "wait", "<MILISECONDS>", "run test for miliseconds after connection (exclude connection time)");
    struct arg_end* gamepad_end = arg_end(20);
    void* gamepad_argtable[] = { gamepad_cmd, gamepad_help, gamepad_device, gamepad_wait, gamepad_end };
    int gamepad_errors = 0;

    struct arg_rex* validate_cmd = ARG_CMD("validate");
    struct arg_lit* validate_help = ARG_HELP();
    struct arg_file* validate_input = ARG_INPUT();
    struct arg_end* validate_end = arg_end(20);
    void* validate_argtable[] = { validate_cmd, validate_help, validate_input, validate_end };
    int validate_errors = 0;

    int exitcode = EXIT_SUCCESS;

    if (arg_nullcheck(main_argtable) != 0 ||
        arg_nullcheck(list_argtable) != 0 ||
        arg_nullcheck(get_argtable) != 0 ||
        arg_nullcheck(set_argtable) != 0 ||
        arg_nullcheck(test_argtable) != 0 ||
        arg_nullcheck(gamepad_argtable) ||
        arg_nullcheck(validate_argtable))
    {
        printf("%s: insufficient memory\n", progname);
        exitcode = 1;
        goto exit;
    }

    test_device->ival[0] = 0;
    test_wait->ival[0] = -1;

    gamepad_device->ival[0] = 0;
    gamepad_wait->ival[0] = -1;

    main_errors = arg_parse(argc, argv, main_argtable);
    list_errors = arg_parse(argc, argv, list_argtable);
    get_errors = arg_parse(argc, argv, get_argtable);
    set_errors = arg_parse(argc, argv, set_argtable);
    test_errors = arg_parse(argc, argv, test_argtable);
    gamepad_errors = arg_parse(argc, argv, gamepad_argtable);
    validate_errors = arg_parse(argc, argv, validate_argtable);
    /* Execute the appropriate main<n> routine for the matching command line syntax */
    /* In this example program our alternate command line syntaxes are mutually     */
    /* exclusive, so we know in advance that only one of them can be successful.    */
    if (list_errors == 0) {
        if (list_help->count) {
            printf("show list help\n");
        }
        else {
            exitcode = main_list();
        }
    }
    else if (set_errors == 0) {
        if (set_help->count) {
            printf("show set help\n");
        }
        else {
            exitcode = main_set();
        }
    }
    else if (get_errors == 0) {
        if (get_help->count) {
            printf("show get help\n");
        }
        else {
            exitcode = main_get();
        }
    }
    else if (test_errors == 0) {
        if (test_help->count) {
            printf("show test help\n");
        }
        else {
            exitcode = main_test(*test_device->ival, *test_wait->ival);
        }
    }
    else if (gamepad_errors == 0) {
        if (test_help->count) {
            printf("show gampaed help\n");
        }
        else {
            exitcode = main_gamepad(*test_device->ival, *test_wait->ival);
        }
    }
    else if (validate_errors == 0) {
        if (validate_help->count) {
            printf("show validate help\n");
        }
        else {
            exitcode = main_validate(validate_input->filename[0]);
        }
    }
    else
    {
        if (list_cmd->count > 0)
        {
            if (list_help->count > 0) {
                printf("show list help\n");
            }
            else {
                printf("show list errors\n");
                arg_print_errors(stdout, list_end, progname);
            }
        }
        else if (set_cmd->count > 0)
        {
            if (set_help->count > 0) {
                printf("show set help\n");
            }
            else {
                printf("show set errors\n");
                arg_print_errors(stdout, set_end, progname);
            }
        }
        else if (get_cmd->count > 0)
        {
            if (get_help->count > 0) {
                printf("show get help\n");
            }
            else {
                printf("show get errors\n");
                arg_print_errors(stdout, get_end, progname);
            }
        }
        else if (test_cmd->count > 0)
        {
            if (test_help->count > 0) {
                printf("show test help\n");
            }
            else {
                printf("show test errors\n");
                arg_print_errors(stdout, test_end, progname);
            }
        }
        else
        {
            /* no correct cmd literals were given, so we cant presume which syntax was intended */
            printf("%s: missing <list|get|set|test> command.\n", progname);
            printf("usage 1: %s ", progname);  arg_print_syntax(stdout, main_argtable, "\n");
            printf("usage 2: %s ", progname);  arg_print_syntax(stdout, list_argtable, "\n");
            printf("usage 3: %s ", progname);  arg_print_syntax(stdout, get_argtable, "\n");
            printf("usage 3: %s ", progname);  arg_print_syntax(stdout, set_argtable, "\n");
            printf("usage 4: %s ", progname);  arg_print_syntax(stdout, test_argtable, "\n");
            printf("usage 5: %s ", progname);  arg_print_syntax(stdout, gamepad_argtable, "\n");
        }
    }

exit:
    /* deallocate each non-null entry in each argtable */
    arg_freetable(main_argtable, sizeof(main_argtable) / sizeof(main_argtable[0]));
    arg_freetable(list_argtable, sizeof(list_argtable) / sizeof(list_argtable[0]));
    arg_freetable(get_argtable, sizeof(get_argtable) / sizeof(get_argtable[0]));
    arg_freetable(set_argtable, sizeof(set_argtable) / sizeof(set_argtable[0]));
    arg_freetable(test_argtable, sizeof(test_argtable) / sizeof(test_argtable[0]));
    arg_freetable(gamepad_argtable, sizeof(gamepad_argtable) / sizeof(gamepad_argtable[0]));

    return exitcode;
}
int main1() {
    hori_device_t* driver = NULL;
    hori_enumeration_t* devices = hori_enumerate(HORI_PRODUCT_ANY, NULL);
    int i = 0;
    for (hori_enumeration_t* device = devices; device != NULL; device = device->next, i++) {
        printf("%2d: %s\n", i, device->path);
        driver = hori_open_path(device->path, NULL);
        if (driver == NULL) {
            continue;
        }
        if (hori_get_state(driver) != HORI_STATE_CONFIG) {
            hori_set_state(driver, HORI_STATE_CONFIG);
        }
        if (hori_get_state(driver) != HORI_STATE_CONFIG) {
            hori_close(driver);
            driver = NULL;
            continue;
        }
    }
    hori_free_enumerate(devices);
    int change = TRUE;
    if (driver) {
        printf("driver\n");
        for (int i = 0; i < 1000; i++) {
            if (-1 == hori_send_heartbeat(driver)) {
                printf("cannot send heartbeat\n");
                break;
            }
            const char* version_str = hori_get_firmware_version_str(driver);
            if (version_str == NULL) {
                printf("cannot get firmware\n");
                break;
            }
            printf("firmware version: %s\n", version_str);

            for (int profile_id = 4; profile_id >= 1; profile_id--) {
                hori_profile_t* profile = hori_get_profile(driver, profile_id);
                if (NULL == profile) {
                    printf("cannot read profile %d\n", profile_id);
                    continue;
                }
                if (profile_id == 4 && change) {
                    change = FALSE;
                    char name[] = "nowy";
                    hori_set_profile_name(profile, name, sizeof(name));
                    hori_set_profile(driver, profile_id, profile);
                }
                printf("%s\n", hori_get_profile_name(profile));
            }

            Sleep(1000);
        }
    }
    hori_close(driver);
    driver = NULL;
    //horiapi_xinput_reset(NULL);
    return EXIT_SUCCESS;
}
