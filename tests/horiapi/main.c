#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../src/horiapi/hori_command.h"

/* A test case that does nothing and succeeds. */
static void null_test_success(void** state) {
    (void)state; /* unused */
}


struct test_data_firmware_version {
    struct hori_firmware_version version;
    char* version_str;
    char* data;
    size_t data_size;
};
typedef struct test_data_firmware_version test_data_firmware_version_t;
#define sizeof_data(test_data) sizeof(test_data) - offsetof(test_data_firmware_version_t, data)
static char fw_ver_has_text_data[] = "0\0001\0002\0003\000test\000one\000";
static const test_data_firmware_version_t fw_ver_has_text = {
    {0, 1, 2, 3},
    "1.2.3 test one",
    fw_ver_has_text_data,
    sizeof(fw_ver_has_text_data)
};

static const test_data_firmware_version_t fw_ver_has_rev = {
    {1, 1, 2, 3},
    "1.2.3 (rev 1) test one",
    "1\0001\0002\0003\000\000",
};

static void parseFirmwareVersion_hasDoubleZeroAtEnd(void** state) {
    char data[] = "0\0001\0002\0003\000";
    struct hori_firmware_version expect = { 0, 1, 2, 3 };
    struct hori_firmware_version actual = { 0, { 0, 0, 0 } };
    int length = hori_internal_parse_firmware_version(data, sizeof(data), &actual);
    assert_int_equal(sizeof(data) - 1, length);
    assert_int_equal(expect.hardware_revision, actual.hardware_revision);
    assert_int_equal(expect.software_version.major, actual.software_version.major);
    assert_int_equal(expect.software_version.minor, actual.software_version.minor);
    assert_int_equal(expect.software_version.patch, actual.software_version.patch);
}

static void parseFirmwareVersion_hasOneZeroAtEnd(void** state) {
    char data[] = "0\0001\0002\0003";
    struct hori_firmware_version expect = { 0, 1, 2, 3 };
    struct hori_firmware_version actual = { 0, { 0, 0, 0 } };
    int length = hori_internal_parse_firmware_version(data, sizeof(data), &actual);
    assert_int_equal(sizeof(data), length);
    assert_int_equal(expect.hardware_revision, actual.hardware_revision);
    assert_int_equal(expect.software_version.major, actual.software_version.major);
    assert_int_equal(expect.software_version.minor, actual.software_version.minor);
    assert_int_equal(expect.software_version.patch, actual.software_version.patch);
}

static void parseFirmwareVersion_noZeroAtEnd(void** state) {
    char data[] = "0\0001\0002\0003";
    struct hori_firmware_version expect = { 0, 1, 2, 3 };
    struct hori_firmware_version actual = { 0, { 0, 0, 0 } };
    int length = hori_internal_parse_firmware_version(data, sizeof(data) - 1, &actual);
    assert_int_equal(sizeof(data) - 1, length);
    assert_int_equal(expect.hardware_revision, actual.hardware_revision);
    assert_int_equal(expect.software_version.major, actual.software_version.major);
    assert_int_equal(expect.software_version.minor, actual.software_version.minor);
    assert_int_equal(expect.software_version.patch, actual.software_version.patch);
}

static void parseFirmwareVersion_multiDigitsNumbers(void** state) {
    char data[] = "123\000124\0001234\00012341\000";
    struct hori_firmware_version expect = { 123, 124, 1234, 12341 };
    struct hori_firmware_version actual = { 0, { 0, 0, 0 } };
    int length = hori_internal_parse_firmware_version(data, sizeof(data), &actual);
    assert_int_equal(sizeof(data) - 1, length);
    assert_int_equal(expect.hardware_revision, actual.hardware_revision);
    assert_int_equal(expect.software_version.major, actual.software_version.major);
    assert_int_equal(expect.software_version.minor, actual.software_version.minor);
    assert_int_equal(expect.software_version.patch, actual.software_version.patch);
}


static void parseFirmwareVersion_multiDigitsNumbersWithLeadingZeros(void** state) {
    char data[] = "00123\000000124\00000001234\00000012341\000";
    struct hori_firmware_version expect = { 123, 124, 1234, 12341 };
    struct hori_firmware_version actual = { 0, { 0, 0, 0 } };
    int length = hori_internal_parse_firmware_version(data, sizeof(data), &actual);
    assert_int_equal(sizeof(data) - 1, length);
    assert_int_equal(expect.hardware_revision, actual.hardware_revision);
    assert_int_equal(expect.software_version.major, actual.software_version.major);
    assert_int_equal(expect.software_version.minor, actual.software_version.minor);
    assert_int_equal(expect.software_version.patch, actual.software_version.patch);
}


static void parseFirmwareVersion_multiDigitsNumbersWithoutTrim(void** state) {
    char data[] = "  123\000124   \000   1234   \000\t12341\r\n\000";
    struct hori_firmware_version expect = { 123, 124, 1234, 12341 };
    struct hori_firmware_version actual = { 0, { 0, 0, 0 } };
    int length = hori_internal_parse_firmware_version(data, sizeof(data), &actual);
    assert_int_equal(sizeof(data) - 1, length);
    assert_int_equal(expect.hardware_revision, actual.hardware_revision);
    assert_int_equal(expect.software_version.major, actual.software_version.major);
    assert_int_equal(expect.software_version.minor, actual.software_version.minor);
    assert_int_equal(expect.software_version.patch, actual.software_version.patch);
}


static void parseFirmwareVersion_onlyReviosionAndMajor(void** state) {
    char data[] = "123\000124\000";
    struct hori_firmware_version expect = { 123, 124, 0, 0 };
    struct hori_firmware_version actual = { 0, { 0, 0, 0 } };
    int length = hori_internal_parse_firmware_version(data, sizeof(data), &actual);
    assert_int_equal(sizeof(data) - 1, length);
    assert_int_equal(expect.hardware_revision, actual.hardware_revision);
    assert_int_equal(expect.software_version.major, actual.software_version.major);
    assert_int_equal(expect.software_version.minor, actual.software_version.minor);
    assert_int_equal(expect.software_version.patch, actual.software_version.patch);
}

static void parseFirmwareVersion_onlyRevisionAndMajorAndText(void** state) {
    char data[] = "123\000124\000testing\000";
    struct hori_firmware_version expect = { 123, 124, 0, 0 };
    struct hori_firmware_version actual = { 0, { 0, 0, 0 } };
    int length = hori_internal_parse_firmware_version(data, sizeof(data), &actual);
    assert_int_equal(8, length);
    assert_int_equal(expect.hardware_revision, actual.hardware_revision);
    assert_int_equal(expect.software_version.major, actual.software_version.major);
    assert_int_equal(expect.software_version.minor, actual.software_version.minor);
    assert_int_equal(expect.software_version.patch, actual.software_version.patch);
}

static void parseFirmwareVersion_emptyString(void** state) {
    char data[] = "";
    int length = hori_internal_parse_firmware_version(data, sizeof(data), NULL);
    assert_int_equal(-1, length);
}

static void parseFirmwareVersion_invalidParameterData(void** state) {
    int length = hori_internal_parse_firmware_version(NULL, 1, NULL);
    assert_int_equal(-1, length);
}

static void parseFirmwareVersion_invalidParameterDataSize(void** state) {
    int length = hori_internal_parse_firmware_version("", -1, NULL);
    assert_int_equal(-1, length);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(parseFirmwareVersion_hasDoubleZeroAtEnd),
        cmocka_unit_test(parseFirmwareVersion_hasOneZeroAtEnd),
        cmocka_unit_test(parseFirmwareVersion_noZeroAtEnd),
        cmocka_unit_test(parseFirmwareVersion_multiDigitsNumbers),
        cmocka_unit_test(parseFirmwareVersion_onlyReviosionAndMajor),
        cmocka_unit_test(parseFirmwareVersion_onlyRevisionAndMajorAndText),
        cmocka_unit_test(parseFirmwareVersion_emptyString),
        cmocka_unit_test(parseFirmwareVersion_invalidParameterData),
        cmocka_unit_test(parseFirmwareVersion_invalidParameterDataSize),
        cmocka_unit_test(parseFirmwareVersion_multiDigitsNumbersWithLeadingZeros),
        cmocka_unit_test(parseFirmwareVersion_multiDigitsNumbersWithoutTrim),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
