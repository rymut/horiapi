#include "hori_xinput_win32.h"

#include <inttypes.h>

#include <Windows.h>

HANDLE OpenDeviceInterface(const wchar_t* path, BOOL readOnly)
{
    DWORD desired_access = readOnly == TRUE ? 0 : (GENERIC_WRITE | GENERIC_READ);
    DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;

    HANDLE handle = CreateFileW(path, desired_access, share_mode, 0, OPEN_EXISTING, 0, 0);

    return handle;
}

// check if corntorelr is connected as player 1, 2, 3, 4
/** @brief Get player id for xinput device

    @returns
        This function returns -1 on error (wrong path, wrong device), or 
 */
int hori_win32_xinput_device_get_player_id(const wchar_t* path) {
    if (path == NULL)
        return -1;
    
    HANDLE handle = OpenDeviceInterface(path, FALSE);
    if (handle == INVALID_HANDLE_VALUE)
        return -1;

    uint8_t gamepadStateRequest0101[3] = { 0x01, 0x01, 0x00 };
    uint8_t ledStateData[3] = { 0, 0,0 };
    DWORD len = 0;

    // https://github.com/nefarius/XInputHooker/issues/1
    // https://gist.github.com/mmozeiko/b8ccc54037a5eaf35432396feabbe435
    DWORD IOCTL_XUSB_GET_LED_STATE = 0x8000E008;

    DeviceIoControl(handle,
        IOCTL_XUSB_GET_LED_STATE,
        gamepadStateRequest0101,
        3,
        ledStateData,
        3,
        &len,
        NULL);

    CloseHandle(handle);
    handle = NULL;

    if (len != sizeof(ledStateData)) {
        return -1;
    }

    // https://www.partsnotincluded.com/xbox-360-controller-led-animations-info/
    // https://github.com/paroj/xpad/blob/5978d1020344c3288701ef70ea9a54dfc3312733/xpad.c#L1382-L1402
    const uint8_t kInvalidXInputUserId = 0xff;
    const uint8_t xinputLedToUserId[] =
    {
        kInvalidXInputUserId,   // All off
        kInvalidXInputUserId,   // All blinking, then previous setting
        0,                      // 1 flashes, then on
        1,                      // 2 flashes, then on
        2,                      // 3 flashes, then on
        3,                      // 4 flashes, then on
        0,                      // 1 on
        1,                      // 2 on
        2,                      // 3 on
        3,                      // 4 on
        kInvalidXInputUserId,   // Rotate
        kInvalidXInputUserId,   // Blink, based on previous setting
        kInvalidXInputUserId,   // Slow blink, based on previous setting
        kInvalidXInputUserId,   // Rotate with two lights
        kInvalidXInputUserId,   // Persistent slow all blink
        kInvalidXInputUserId,   // Blink once, then previous setting
    };

    const uint8_t ledState = ledStateData[2];
    if (ledState >= sizeof(xinputLedToUserId)) {
        return -1;
    }
    const uint8_t userId = xinputLedToUserId[ledState];
    if (userId != kInvalidXInputUserId) {
        return -1;
    }
    return userId;
}
