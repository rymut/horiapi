#pragma once

/** @brief Read profile from device

    @param[in] device_id The device to connect to
    @param[in] profile_id The profile to read (-1 read all profiles)
    @param[in] output_file The file to write to (NULL write to stdout)

    @returns
        This function returns EXIT_FAILURE or EXIT_SUCCESS
  */
int hori_cli_command_get(int device_id, int profile_id, const char *output);

