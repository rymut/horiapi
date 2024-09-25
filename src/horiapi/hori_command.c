#include "hori_command.h"

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "hori_device.h"


static int hori_internal_has_valid_command_response(uint8_t* buffer, int size, int command_id) {
    if (buffer == NULL) {
        return -1;
    }
    if (size < HORI_INTERNAL_PACKET_HEADER_SIZE) {
        return -1;
    }
    if (buffer[0] != HORI_REPORT_ID_PROFILE_RESPONSE) {
        return -1;
    }
    if (buffer[1] != 0 || buffer[2] != 0) {
        return -1;
    }
    if (buffer[3] > size - HORI_INTERNAL_PACKET_HEADER_SIZE) {
        return -1;
    }
    if (buffer[4] != (uint8_t)command_id) {
        return -1;
    }
    return size - HORI_INTERNAL_PACKET_HEADER_SIZE;
}

static int hori_internal_send_command(hori_device_t* device, int command_id) {
    uint8_t command[64] = { HORI_REPORT_ID_PROFILE_REQUEST, 0, 0, 60, command_id, 0 };
    return hori_internal_write_control(device, command, sizeof(command));
}

int hori_internal_switch_profile(hori_device_t* device, int profile) {
    if (profile < 0 || profile > 4) {
        return -1;
    }
    uint8_t command[HORI_INTERNAL_REQUEST_SIZE] = { HORI_REPORT_ID_PROFILE_REQUEST, 0, 0, 60, HORI_COMMAND_ID_SWITCH_PROFILE, (uint8_t)profile, 0 };
    return hori_internal_write_control(device, command, sizeof(command));
}

int hori_internal_exit_profile(hori_device_t* device)
{
    return hori_internal_send_command(device, HORI_COMMAND_ID_EXIT_PROFILE);
}

int hori_internal_send_command_enter_profile(hori_device_t* device)
{
    return hori_internal_send_command(device, HORI_COMMAND_ID_ENTER_PROFILE);
}

static uint8_t hori_internal_heartbeat[] = { HORI_REPORT_ID_PROFILE_REQUEST, 0xAA, 0x55, 0x5A, 0xA5 };

int hori_internal_send_heartbeat(hori_device_t* device)
{
    return hori_internal_write_control(device, hori_internal_heartbeat, sizeof(hori_internal_heartbeat));
}

/**
 @brief

 @param data - string text (might not end with \0)
 @param size - data size

 @return string containing version number or NULL if data does not contain valid version number
 */
char* hori_internal_parse_firmware_version(uint8_t* data, int size) {
    if (data == NULL || size <= 0) {
        return NULL;
    }
    // allocade maximum size
    char* version_str = (char*)calloc((size_t)size + 1, sizeof(char));
    if (version_str == NULL) {
        return NULL;
    }

    int position = 0;
    char* str = version_str;
    for (int length = strnlen(data, size); length != 0; position++, data += length, length = strnlen(data, size)) {
        if (version_str == str && position == 0 && length == 1 && data[0] == '0') {
            continue;
        }
        if (version_str != str) {
            *str = (position < 4) ? '.' : '-';
            str += 1;
        }
        memcpy(str, data, length);
        str += length;
    }

    if (str != version_str) {
        return version_str;
    }
    free(version_str);
    return NULL;
}

int hori_internal_read_firmware_version(hori_device_t* device) {
    uint8_t read_firmware_request[64] = { HORI_REPORT_ID_PROFILE_REQUEST, 0, 0, 60, HORI_COMMAND_ID_READ_FIRMWARE_VERSION, 0 };
    if (-1 == hori_internal_write_control(device, read_firmware_request, sizeof(read_firmware_request))) {
        return -1;
    }
    if (-1 == hori_internal_send_heartbeat(device)) {
        return -1;
    }
    uint8_t response[HORI_INTERNAL_RESPONSE_SIZE];
    int response_size = hori_internal_read_control(device, response, HORI_INTERNAL_RESPONSE_SIZE);
    int payload_size = hori_internal_has_valid_command_response(response, response_size, HORI_COMMAND_ID_READ_FIRMWARE_VERSION_ACK);
    if (payload_size == -1) {
        return -1;
    }
    free(device->firmware_version);
    device->firmware_version = hori_internal_parse_firmware_version(response + HORI_INTERNAL_PACKET_HEADER_SIZE, payload_size);
    if (device->firmware_version == NULL) {
        return -1;
    }
    return strlen(device->firmware_version);
}

