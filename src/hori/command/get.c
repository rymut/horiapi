#include "get.h"

#include <horiapi/horiapi.h>
#include "../hori_yaml.h"

int hori_cli_command_get(int device_id, int profile_id, const char* output) {
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

    if (device_path == NULL) {
        printf("Device id %d not found\n", device_id);
        return EXIT_FAILURE;
    }
    hori_device_t* device = hori_open_path(device_path, NULL);
    free(device_path);
    device_path = NULL;

    if (device == NULL) {
        printf("cannot open device %d\n", device_id);
        return EXIT_FAILURE;
    }
    const int hori_state = HORI_STATE_CONFIG;
    if (hori_get_state(device) != hori_state) {
        hori_set_state(device, hori_state);
    }
    if (hori_get_state(device) != hori_state) {
        hori_close(device);
        printf("worong device state");
        return EXIT_FAILURE;
    }

    hori_device_config_t *device_config = hori_get_device_config(device);
    struct hori_yaml_config_list *list = NULL;
    struct hori_yaml_config_list** next = &list;
    const int profile_offset = profile_id < 0 ? 0 : profile_id;
    const int profile_count = profile_id < 0 ? 0xFF : 1;
    for (int p = 0; p < profile_count; ++p) {
        if (hori_get_state(device) == HORI_STATE_CONFIG) {
            hori_send_heartbeat(device);
        }
        // read profile 
        int profile_id = profile_offset + p;
        hori_profile_t *profile = hori_get_profile(device, profile_id);
        if (profile == NULL)
            break;
        struct hori_yaml_config_list *item = *next = hori_yaml_make_config_list(device_config, profile);
        if (item == NULL)
            continue;
        item->config.profile_id = profile_id;
        item->config.layout = device_config->device_config_mode;
        next = &item->next;
    }
    hori_close(device);
    device = NULL;

    size_t yaml_size = 0, string_size = 1024*1024*1;
    char* yaml_string = (char*)calloc(string_size, sizeof(char));
    if (yaml_string == NULL)
        return EXIT_FAILURE;
    hori_yaml_config_emit_string(list, yaml_string, string_size, &yaml_size);
//    hori_yaml_config_emit_file(list, stdout);
    
    hori_yaml_free_config_list(list);
    list = hori_yaml_config_parse_string(yaml_string, yaml_size, NULL);
    free(yaml_string);
    yaml_string = NULL;
    hori_yaml_free_config_list(list);
    list = NULL;
    return EXIT_SUCCESS;
}
