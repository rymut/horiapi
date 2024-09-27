#pragma once

#include <horiapi/horiapi.h>

#define HORI_INTERNAL_REQUEST_SIZE 64
#define HORI_INTERNAL_RESPONSE_SIZE 64

#define HORI_INTERNAL_PACKET_HEADER_SIZE 5

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
    enum hori_report_id {
        HORI_REPORT_ID_PROFILE_REQUEST = 15,
        HORI_REPORT_ID_PROFILE_RESPONSE = 16,
    };

    enum hori_command_id {
        HORI_COMMAND_ID_ENTER_PROFILE = 1,
        HORI_COMMAND_ID_EXIT_PROFILE = 2,
        HORI_COMMAND_ID_WRITE_PROFILE = 3,
        HORI_COMMAND_ID_READ_PROFILE = 4,
        HORI_COMMAND_ID_PROFILE_ACK = 5,
        HORI_COMMAND_ID_ACK = 6,
        HORI_COMMAND_ID_SWITCH_PROFILE = 7,
        HORI_COMMAND_ID_PROFILE_SAVE = 8, /// store to eprom _EEPROM
        HORI_COMMAND_ID_READ_FIRMWARE_VERSION = 9,
        HORI_COMMAND_ID_READ_FIRMWARE_VERSION_ACK = 10,
        HORI_COMMAND_ID_READ_ACTIVE_PROFILE = 11,         // 15, 0, 0, 60, 11
        HORI_COMMAND_ID_READ_ACTIVE_PROFILE_ACK = 12,
        HORI_COMMAND_ID_SWITCH_ACTIVE_PROFILE = 13,
    };

    int hori_internal_send_command_exit_profile(hori_device_t* device);
    int hori_internal_send_command_enter_profile(hori_device_t* device);

    int hori_internal_get_firmware_version_str_size(const uint8_t* data, int data_size);
    /** @brief Parse firmware version

        @since 0.1.0
        @param data[in] The multibyte (UTF-8) string, can handle missing \0 at the end
        @param size[in] The length of @p data
        @param version[out] The pointer to firmware version structure, can be null

        @returns
            The function returns number of bytes form data used to parse firmware version,
            or -1 on error or invalid field value.
    */
    int hori_internal_parse_firmware_version(uint8_t const* data, int size, struct hori_firmware_version* version);

    /** @brief Parse firmware virsion string

        @since 0.1.0
        @param data[in] The multibyte (UTF-8) string
        @param data_size[in] The length of @p data
        @param version_str[out] The version string can be NULL
        @param version_str_size[in] The length of version string - must be 0 when @p version_str is NULL

        @returns
            The function return -1 on error or if version_str_size cannot hold version data,
            otherwise returns number of bytes written in version_str_size
     */
    int hori_internal_parse_firmware_version_str(char const* data, int data_size, char* version_str, int version_str_size);
#ifdef __cplusplus
}
#endif // __cplusplus
/**
 * HEADER:
 *  - byte 0:
 *     15 - write data to
 *  16 - read config report from gamepad
 /
int hori_internal_command_validate(uint8_t* bytes, int byte_size, uint8_t** payload) {
    if (payload) {
        *payload = NULL;
    }
    if (byte_size <= 0 || bytes == NULL) {
        return HORI_COMMAND_ID_UNKNOWN;
    }
    if (byte_size < 5) {
        return HORI_COMMAND_ID_UNKNOWN:
    }
    // 18 - bytes[0] - probably audio?
    //  1 - bytes[0] - probably gamepad
    const uint8_t magic = bytes[0];
    const uint8_t zero0 = bytes[1];
    const uint8_t zero1 = bytes[2];
    const uint8_t length = bytes[3];
    const uint8_t command = bytes[4];
    if (magic != 16 || zero0 != 0 || zero1 != 0) {
        return HORI_COMMAND_ID_UNKNOWN;
    }
    if (length + 4 < byte_size) {
        return HORI_COMMAND_ID_UNKNOWN;
    }
    if (command >= HORI_COMMAND_ID_PROFILE_ENTER && command <= HORI_COMMAND_ID_PROFILE_ACTIVE_SWITCH) {
        return HORI_COMMAND_ID_UNKNOWN;
    }
    if (payload && byte_size > 5) {
        *payload = bytes + 5;
    }
    return command;
}

/*
- Enter Profile = 1
- Exit Profile = 2
- Write Profile = 3
- Read Profile = 4
- 5 - ReadProfileAck
- 5 - ReadProfileComplete && 256 * command[6] + command[7] + command[8] == EveryPacketLength
- 5 - ReadAudioAck && 0 == command[6] && 40 = command[7] && 8 == command[8]
- 6 - Ack
- 7 - SwitchProfile
- 8 - WriteProfileToEEPRom
- 9 - ReadFirmwareVersion
- 10 - ReadFirmwareVersionAck
- 11 - ReadCurrentProfile
- 12 - ReadCurrentProfileAck
- 13 - ActiveSwitchProfile

*/
