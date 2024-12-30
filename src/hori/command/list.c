#include "list.h"

#include <stdlib.h>
#include <stdio.h>

#include <horiapi/horiapi.h>

int hori_cli_command_list() {
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
