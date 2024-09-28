#include "hori_command.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdio.h>

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

int hori_internal_parse_version_number(char const* data, int data_size) {
    if (data == NULL || data_size < 0) {
        return -1;
    }
    unsigned int result = 0;
    size_t index = 0;
    for (index = 0; index < data_size && isspace(data[index]) != 0; ++index);
    if (index >= data_size) {
        return -1;
    }
    for (; index < data_size && isdigit(data[index]) != 0 && index < data_size; ++index) {
        result = result * 10 + (data[index] - '0');
        if (result > INT_MAX) {
            return -1;
        }
    }
    for (; index < data_size && isspace(data[index]) != 0; ++index);
    for (; index < data_size && data[index] == 0; ++index);
    if (index < data_size) {
        return -1;
    }
    return (int)result;
}

int hori_internal_parse_firmware_version(uint8_t const* data, int data_size, struct hori_firmware_version* version) {
    if (data == NULL || data_size <= 0) {
        return -1;
    }

    int position = 0;
    int numbers[4] = { -1, -1, 0, 0 };
    char const* strings = data;
    int strings_size = data_size;
    for (size_t length = strnlen(strings, strings_size);
        length != 0 && position < 4 && strings_size > 0;
        position++, length = strnlen(strings, strings_size)) {
        int value = hori_internal_parse_version_number(strings, length);
        if (value == -1) {
            if (position < 2) {
                return -1;
            }
            break;
        }
        numbers[position] = value;
        strings += length + 1;
        strings_size -= length + 1;
    }

    if (numbers[1] == -1) {
        return -1;
    }
    if (version) {
        version->hardware_revision = numbers[0];
        version->software_version.major = numbers[1];
        version->software_version.minor = numbers[2];
        version->software_version.patch = numbers[3];
    }
    return strings_size <= 0 ? data_size : data_size - strings_size;
}

/** @brief Check if firmware version payload is valid

    @param data The payload of firmware version ack
    @since 0.1.0

    @returns
        Length of firmware version payload (always less than @p data_size),
        -1 if payload is not valid

    @note
        data is valid if it consists at least of 4 strings
        - number only positive
        - number - can contain leading zeros
        - number
        - number
        - any string ... repated
 */

#define HORI_VERSION_STR_REVISION_PREFIX "(rev "
#define HORI_VERSION_STR_REVISION_SUFFIX ")"
#define HORI_VERSION_STR_DIGITS_SEPARATOR "."
#define HORI_VERSION_STR_TEXT_SEPARATOR " "

static int hori_internal_number_digits(int value) {
    int digits = 0;
    if (value < 0) {
        digits += 1;
        value = -value;
    }
    return digits + log10(value) + 1;
}

int hori_internal_get_firmware_version_str_size(const uint8_t* data, int data_size) {
    if (data == NULL || data_size <= 0) {
        return -1;
    }
    struct hori_firmware_version version;
    int version_length = hori_internal_parse_firmware_version(data, data_size, &version);
    if (version_length == -1) {
        return -1;
    }
    int str_size = hori_internal_number_digits(version.software_version.major) + strlen(HORI_VERSION_STR_DIGITS_SEPARATOR) +
        hori_internal_number_digits(version.software_version.minor) + strlen(HORI_VERSION_STR_DIGITS_SEPARATOR) +
        hori_internal_number_digits(version.software_version.patch);
    if (version.hardware_revision != 0) {
        str_size += strlen(HORI_VERSION_STR_TEXT_SEPARATOR) + strlen(HORI_VERSION_STR_REVISION_PREFIX) + hori_internal_number_digits(version.hardware_revision) + strlen(HORI_VERSION_STR_REVISION_SUFFIX);
    }
    if (version_length == data_size) {
        return str_size;
    }
    const char* strings = data + version_length;
    int strings_size = data_size - version_length;
    for (size_t length = strnlen(strings, strings_size); length != 0 && strings_size > 0; length = strnlen(strings, strings_size)) {
        str_size += strlen(HORI_VERSION_STR_TEXT_SEPARATOR);
        str_size += length;
        strings += length + 1;
        strings_size -= length + 1;
    }
    return str_size + 1;
}

int hori_internal_parse_firmware_version_str(const uint8_t* data, int data_size, char* version_str, int version_str_size) {
    if (data == NULL || data_size <= 0) {
        return -1;
    }
    const int required_size = hori_internal_get_firmware_version_str_size(data, data_size);
    if (required_size == -1) {
        return -1;
    }
    if (version_str == NULL || version_str_size < required_size) {
        return -1;
    }
    struct hori_firmware_version version;
    int version_length = hori_internal_parse_firmware_version(data, data_size, &version);
    if (version_length == -1) {
        return -1;
    }
    version_str += sprintf(version_str, "%d", version.software_version.major);
    strcpy(version_str, HORI_VERSION_STR_DIGITS_SEPARATOR);
    version_str += strlen(HORI_VERSION_STR_DIGITS_SEPARATOR);

    version_str += sprintf(version_str, "%d", version.software_version.minor);
    strcpy(version_str, HORI_VERSION_STR_DIGITS_SEPARATOR);
    version_str += strlen(HORI_VERSION_STR_DIGITS_SEPARATOR);

    version_str += sprintf(version_str, "%d", version.software_version.patch);

    if (version.hardware_revision != 0) {
        strcpy(version_str, HORI_VERSION_STR_TEXT_SEPARATOR);
        version_str += strlen(HORI_VERSION_STR_TEXT_SEPARATOR);

        strcpy(version_str, HORI_VERSION_STR_REVISION_PREFIX);
        version_str += strlen(HORI_VERSION_STR_REVISION_PREFIX);

        version_str += sprintf(version_str, "%d", version.hardware_revision);
        strcpy(version_str, HORI_VERSION_STR_REVISION_SUFFIX);
        version_str += strlen(HORI_VERSION_STR_REVISION_SUFFIX);
    }

    const char* strings = data + version_length;
    int strings_size = data_size - version_length;
    for (size_t length = strnlen(strings, strings_size); length != 0 && strings_size > 0; length = strnlen(strings, strings_size)) {
        strcpy(version_str, HORI_VERSION_STR_TEXT_SEPARATOR);
        version_str += strlen(HORI_VERSION_STR_TEXT_SEPARATOR);

        strncpy(version_str, strings, length);
        version_str += length;

        strings += length + 1;
        strings_size -= length + 1;
    }
    *version_str = 0;
    return data_size - strings_size;
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
    free(device->firmware_version_str);
    //device->firmware_version_str = hori_internal_parse_firmware_version_str(response + HORI_INTERNAL_PACKET_HEADER_SIZE, payload_size);
    if (device->firmware_version == NULL) {
        return -1;
    }
    return strlen(device->firmware_version_str);
}

