#include "hori_command.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdio.h>

#include "hori_device.h"

/** @brief get command payload offset for valid command buffer

    @since 0.1.0
    @param data[in] The array containing command response/request
    @param size[in] The size of the @p data parameter
    @param command_id[in] - command to get payload offset

    @returns
        The function returns -1 if @data does not contain valid @p command_id buffer,
        or offset to command payload, always less or equal to @p size
        (in case when command does not hold any payload)
 */
static int hori_internal_get_command_payload_offset(uint8_t const* data, int size, int command_id) {
    if (data == NULL) {
        return -1;
    }
    if (size < HORI_INTERNAL_PACKET_HEADER_SIZE) {
        return -1;
    }
    if (data[0] != HORI_REPORT_ID_PROFILE_RESPONSE) {
        return -1;
    }
    if (data[1] != 0 || data[2] != 0) {
        return -1;
    }
    if (data[3] > size - (HORI_INTERNAL_PACKET_HEADER_SIZE - 1)) {
        return -1;
    }
    if (data[4] != (uint8_t)command_id) {
        return -1;
    }
    return HORI_INTERNAL_PACKET_HEADER_SIZE;
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
    int payload_offset = hori_internal_get_command_payload_offset(response, response_size, HORI_COMMAND_ID_READ_FIRMWARE_VERSION_ACK);
    if (payload_offset == -1) {
        return -1;
    }
    uint8_t* payload = response + payload_offset;
    int payload_size = response_size - payload_offset;
    int version_str_size = hori_internal_get_firmware_version_str_size(payload, payload_size);
    if (version_str_size == -1) {
        return -1;
    }
    char* version_str = (char*)calloc(version_str_size, sizeof(char));
    struct hori_firmware_version* version = (struct hori_firmware_version*)calloc(1, sizeof(struct hori_firmware_version));
    if (version_str == NULL || version == NULL) {
        free(version_str);
        free(version);
        return -1;
    }
    if (-1 == hori_internal_parse_firmware_version_str(payload, payload_size, version_str, version_str_size)) {
        free(version_str);
        free(version);
        return -1;
    }
    if (-1 == hori_internal_parse_firmware_version(payload, payload_size, version)) {
        free(version_str);
        free(version);
        return -1;
    }

    free(device->firmware_version_str);
    device->firmware_version_str = version_str;
    free(device->firmware_version);
    device->firmware_version = version;
    return 1;
}

#include "hori_profile.h"
#define HORI_COMMAND_ID_READ_PROFILE_ACK_PAYLOAD_SIZE 55
#define HORI_COMMAND_ID_READ_PROFILE_OFFSET_INDEX 6
#define HORI_COMMAND_ID_READ_PROFILE_SIZE_INDEX 7
#define HORI_COMMAND_ID_READ_PROFILE_REMINING_INDEX 8

int hori_internal_read_profile(hori_device_t* device, int profile_id, struct hori_profile_config *profile) {
    if (profile_id < 1 || profile_id > 4) {
        // wrong profile
        return -1;
    }
    uint8_t read_profile[64] = { HORI_REPORT_ID_PROFILE_REQUEST, 0, 0, 60, HORI_COMMAND_ID_READ_PROFILE, profile_id, 0, 0, 0, 0 };
    uint8_t read_profile_ack[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    struct hori_profile_config profile_data;
    memset(&profile_data, 0, sizeof(profile_data));
    uint8_t* profile_buffer = (uint8_t*)&profile_data;
    int profile_remining_size = (int)sizeof(profile_data);
    for (int part = 0; part < ceil(HORI_PROFILE_CONFIG_SIZE / (1.0 * HORI_COMMAND_ID_READ_PROFILE_ACK_PAYLOAD_SIZE)); ++part) {
        if (profile_remining_size > HORI_COMMAND_ID_READ_PROFILE_ACK_PAYLOAD_SIZE) {
            profile_remining_size -= HORI_COMMAND_ID_READ_PROFILE_ACK_PAYLOAD_SIZE;
        }
        unsigned char offset = (HORI_COMMAND_ID_READ_PROFILE_ACK_PAYLOAD_SIZE * part) / 256;
        unsigned char size = (HORI_COMMAND_ID_READ_PROFILE_ACK_PAYLOAD_SIZE * part) % 256;
        unsigned char remining = profile_remining_size > HORI_COMMAND_ID_READ_PROFILE_ACK_PAYLOAD_SIZE ? HORI_COMMAND_ID_READ_PROFILE_ACK_PAYLOAD_SIZE : profile_remining_size;
        read_profile[HORI_COMMAND_ID_READ_PROFILE_OFFSET_INDEX] = offset;
        read_profile[HORI_COMMAND_ID_READ_PROFILE_SIZE_INDEX] = size;
        read_profile[HORI_COMMAND_ID_READ_PROFILE_REMINING_INDEX] = remining;
        if (-1 == hid_write(device->control, read_profile, sizeof(read_profile))) {
            return -1;
        }
        if (-1 == hori_internal_send_heartbeat(device)) {
            return -1;
        }
        memset(read_profile_ack, 0, sizeof(read_profile_ack));
        int response_size = hori_internal_read_control(device, read_profile_ack, HORI_INTERNAL_RESPONSE_SIZE);
        int payload_offset = hori_internal_get_command_payload_offset(read_profile_ack, response_size, HORI_COMMAND_ID_READ_PROFILE_ACK);
        if (payload_offset == -1) {
            return -1;
        }
        if (-1 == hori_internal_send_heartbeat(device)) {
            return -1;
        }
        offset = read_profile_ack[HORI_COMMAND_ID_READ_PROFILE_OFFSET_INDEX];
        size = read_profile_ack[HORI_COMMAND_ID_READ_PROFILE_SIZE_INDEX];
        remining = read_profile_ack[HORI_COMMAND_ID_READ_PROFILE_REMINING_INDEX];
        int profile_offset = ((int)offset) * 256 + size;
        memcpy(profile_buffer + profile_offset, read_profile_ack + 9, remining);
    }
    if (-1 == hori_internal_send_heartbeat(device)) {
        return -1;
    }

    if (-1 == hori_internal_is_valid_profile_config(&profile_data)) {
        return -1;
    }

    if (profile != NULL) {
        memcpy(profile, &profile_data, sizeof(profile_data));
    }
    return sizeof(profile_data);
}
