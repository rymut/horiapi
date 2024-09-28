#include <horiapi/horiapi.h>
#include <stdlib.h>
#include <Windows.h>

#pragma comment (lib, "Setupapi")
#pragma comment(lib, "Cfgmgr32")
#pragma comment(lib, "Hid")

int main() {
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
    if (driver) {
        printf("driver\n");
        for (int i = 0; i < 1000; i++) {
            if (-1 == hori_send_heartbeat(driver)) {
                printf("cannot send heartbeat\n");
                break;
            }
            const char *version_str = hori_get_firmware_version_str(driver);
            if (version_str == NULL) {
                printf("cannot get firmware\n");
                break;
            }
            printf("firmware version: %s\n", version_str);
            Sleep(1000);
        }
    }
    hori_close(driver);
    driver = NULL;
    //horiapi_xinput_reset(NULL);
    return EXIT_SUCCESS;
}
