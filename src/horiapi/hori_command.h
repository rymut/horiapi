#pragma once

#include <horiapi/horiapi.h>

enum hori_command_id {
    HORI_COMMAND_PROFILE_ENTER = 1,
    HORI_COMMAND_PROFILE_EXIT = 2,
    HORI_COMMAND_PROFILE_WRITE = 3,
    HORI_COMMAND_PROFILE_READ = 4,
    HORI_COMMAND_PROFILE_ACK = 5,
    HORI_COMMAND_ACK = 6,
    HORI_COMMAND_SWITCH_PROFILE = 7,
    HORI_COMMAND_PROFILE_SAVE = 8, /// store to eprom _EEPROM
    HORI_COMMAND_READ_FIRMWARE_VERSION = 9,
    HORI_COMMAND_READ_FIRMWARE_VERSION_ACK = 10,
    HORI_COMMAND_READ_ACTIVE_PROFILE = 11,         // 15, 0, 0, 60, 11
    HORI_COMMAND_READ_ACTIVE_PROFILE_ACK = 12,
    HORI_COMMAND_SWITCH_ACTIVE_PROFILE = 13,
};


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
        return HORI_COMMAND_UNKNOWN;
    }
    if (byte_size < 5) {
        return HORI_COMMAND_UNKNOWN:
    }
    // 18 - bytes[0] - probably audio?
    //  1 - bytes[0] - probably gamepad 
    const uint8_t magic = bytes[0];
    const uint8_t zero0 = bytes[1];
    const uint8_t zero1 = bytes[2];
    const uint8_t length = bytes[3];
    const uint8_t command = bytes[4];
    if (magic != 16 || zero0 != 0 || zero1 != 0) {
        return HORI_COMMAND_UNKNOWN;
    }
    if (length + 4 < byte_size) {
        return HORI_COMMAND_UNKNOWN;
    }
    if (command >= HORI_COMMAND_PROFILE_ENTER && command <= HORI_COMMAND_PROFILE_ACTIVE_SWITCH) {
        return HORI_COMMAND_UNKNOWN;
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
