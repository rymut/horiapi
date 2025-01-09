#pragma once

/** @brief Gamepad test

    @param[in] device_id The device number
    @param[in] wait_miliseconds The interval to wait
    @param[in] enter_config_mode The gamepad will run in config mode
 */
int hori_cli_command_gamepad(int device_id, int wait_miliseconds, int enter_config_mode);

