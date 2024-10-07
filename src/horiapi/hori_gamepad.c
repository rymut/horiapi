#include "horiapi.h"

hori_gamepad_t* hori_make_gamepad() {
    return NULL;
}

void hori_free_gamepad(hori_gamepad_t* gamepad) {
    free(gamepad);
}

int hori_read_gamepad(hori_device_t* device, hori_gamepad_t* gamepad) {
    return -1;
}

int hori_read_gamepad_timeout(hori_device_t* device, hori_gamepad_t* gamepad, int miliseconds) {
    return -1;
}

