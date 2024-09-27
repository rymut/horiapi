#include <horiapi/horiapi.h>

static struct hori_software_version api_version = {
    HORI_API_VERSION_MAJOR,
    HORI_API_VERSION_MINOR,
    HORI_API_VERSION_PATCH
};

const struct hori_software_version* HORI_API_CALL hori_version(void)
{
    return &api_version;
}

static const char* api_version_str = HORI_API_VERSION_STR;

const char* HORI_API_CALL hori_version_str(void)
{
    return api_version_str;
}
