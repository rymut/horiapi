#include "gamepad.h"

#include <stdio.h>
#include <stdlib.h>

#include <horiapi/horiapi.h>
#include <horiapi/hori_time.h>

int hori_cli_command_gamepad(int device_id, int wait_miliseconds) {
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
