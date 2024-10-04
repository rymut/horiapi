#pragma once

#include <horiapi/horiapi.h>

#define HORI_INTERNAL_REQUEST_SIZE 64
#define HORI_INTERNAL_RESPONSE_SIZE 64

// size with command_id
#define HORI_INTERNAL_PACKET_HEADER_SIZE 5

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
    enum hori_report_id {
        HORI_REPORT_ID_PROFILE_REQUEST = 15,
        // this depends on controller
        HORI_REPORT_ID_PROFILE_RESPONSE = 16,
    };

    enum hori_command_id {
        HORI_COMMAND_ID_ENTER_PROFILE = 1,
        HORI_COMMAND_ID_EXIT_PROFILE = 2,
        HORI_COMMAND_ID_WRITE_PROFILE = 3,
        HORI_COMMAND_ID_READ_PROFILE = 4,
        /** @brief Response to @see HORI_COMMAND_ID_READ_PROFILE, @see HORI_COMMAND_ID_WRITE_PROFILE */
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

    struct hori_packet_header {
        unsigned char report_id;
        unsigned char zeros[2];
        /** @brief How long is the message */
        unsigned char remining;
        /** @brief Command Id @see hori_command_id */
        unsigned char command_id;
    };

    struct hori_packet_payload_profile {
        unsigned char profile_id;
        unsigned char block;
        unsigned char position;
        unsigned char size;
        unsigned char data[55];
    };

    struct hori_packet {
        struct hori_packet_header header;
        union {
            unsigned char unknown[59];
            struct hori_packet_payload_profile profile;
        } payload;
    };


    int hori_internal_send_command_exit_profile(hori_device_t* device);
    int hori_internal_send_command_enter_profile(hori_device_t* device);

    /** @brief Parse unsigned number

        @summary
            Number can be left padded by 0, or both side padded by whitespaces

        @since 0.1.0
        @param data[in] the array containing value to parse
        @param data_size[in] number of bytes to parse

        @returns
            This function returns parsed value or -1 when invalid arguments are passed or
            data contains other characters than digits (padded with whitespace and \0) or not
            data_size characters were parsed.
      */
    int hori_internal_parse_version_number(char const* data, int data_size);


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
    int hori_internal_parse_firmware_version_str(const uint8_t* data, int data_size, char* version_str, int version_str_size);

    /** @brief Read firmware version from device

        @since 0.1.0
        @param device[in|out] device from which read the version and store values

        @return
            This function returns -1 on error or zero value on success
     */
    int hori_internal_read_firmware_version(hori_device_t* device);

    /** @brief Read profile from device

        @since 0.1.0
        @param device[in|out] The device from which read the profile
        @param profile_id[in] The profile number (1,2,3,4 only)

        @note
            Device must be in HORI_MODE_CONFIG to be able to read profile
      */

    int hori_internal_read_profile(hori_device_t* device, int profile_id, struct hori_profile_config* profile);

    /** @brief Read profile memory
     */
    int hori_internal_read_profile_memory(hori_device_t* device, int profile_id, int address, uint8_t* data, int size);

    /** @brief Write profile to device (using difference method)

        @param device[in|out] The device handle
        @param profile_id[in] Profile id
        @param profile_config Profile configuration to write

        @returns
            This function returns number of bytes written (byte difference) or -1 on error
      */
    int hori_internal_write_profile_difference(hori_device_t* device, int profile_id, struct hori_profile_config* profile_config);

    /** @brief Write profile memory to device at specified offest and size

        @param device[in|out] The device handle
        @param profile_id Profile Id
        @param
        @param data Memory to write (at least
     */
    int hori_internal_write_profile_memory(hori_device_t* device, int profile_id, int offset, uint8_t* data, int size);
#ifdef __cplusplus
}
#endif // __cplusplus
/**
 * HEADER generic pad
 *  - byte 0:
 *     15 - write data to -
 *  16 - read config report from gamepad

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
