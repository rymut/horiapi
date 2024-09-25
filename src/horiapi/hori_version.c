#include <horiapi/horiapi.h>

static struct hori_api_version api_version = {
    HORI_API_VERSION_MAJOR,
    HORI_API_VERSION_MINOR,
    HORI_API_VERSION_PATCH
};

const struct hori_api_version* HORI_API_CALL hid_version(void)
{
    return &api_version;
}

const char* HORI_API_CALL hid_version_str(void)
{
    return HORI_API_VERSION_STR;
}
