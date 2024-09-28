#include "tests.h"
#include <stdlib.h>
#include "horiapi/horiapi.h"

#include "../src/horiapi/hori_command.h"

static void parseVersionNumber_invalidParameterDataSize(void** state) {
    int value = hori_internal_parse_version_number("0", -1);
    assert_int_equal(-1, value);
    value = hori_internal_parse_version_number("0", 0);
    assert_int_equal(-1, value);
}

static void parseVersionNumber_invalidParameterData(void** state) {
    int value = hori_internal_parse_version_number(NULL, 1);
    assert_int_equal(-1, value);
}

static void parseVersionNumber_invalidParameters(void** state) {
    int value = hori_internal_parse_version_number(NULL, -1);
    assert_int_equal(-1, value);
    value = hori_internal_parse_version_number(NULL, 0);
    assert_int_equal(-1, value);
}

#define HORI_VALUE(a) HORI_API_AS_STR_IMPL(a)

static void parseVersionNumber_handleIntMax(void** state) {
    const char data[] = HORI_VALUE(INT_MAX);
    int value = hori_internal_parse_version_number(data, sizeof(data));
    assert_int_equal(INT_MAX, value);
}
static void parseVersionNumber_handleOverIntMax(void** state) {
    const char data[] = HORI_VALUE(INT_MAX+1);
    int value = hori_internal_parse_version_number(data, sizeof(data));
    assert_int_equal(-1, value);
}

static void parseVersionNumber_handleZero(void** state) {
    const char data[] = "0";
    int value = hori_internal_parse_version_number(data, sizeof(data));
    assert_int_equal(0, value);
}

static void parseVersionNumber_handleLeftZeroFill(void** state) {
    const char data[] = "000123";
    int value = hori_internal_parse_version_number(data, sizeof(data));
    assert_int_equal(123, value);
}


static void parseVersionNumber_handleLeftPadding(void** state) {
    const char data[] = "  \r\t  123";
    int value = hori_internal_parse_version_number(data, sizeof(data));
    assert_int_equal(123, value);
}

static void parseVersionNumber_handleRightPadding(void** state) {
    const char data[] = "123  \r\t  ";
    int value = hori_internal_parse_version_number(data, sizeof(data));
    assert_int_equal(123, value);
}

static void parseVersionNumber_handlePadding(void** state) {
    const char data[] = "  \r\t  123  \r\t  ";
    int value = hori_internal_parse_version_number(data, sizeof(data));
    assert_int_equal(123, value);
}

static void parseVersionNumber_handleZeroFillAndPadding(void** state) {
    const char data[] = "  \r\t  000123  \r\t  ";
    int value = hori_internal_parse_version_number(data, sizeof(data));
    assert_int_equal(123, value);
}

static void parseVersionNumber_handleNegative(void** state) {
    const char data[] = "-123";
    int value = hori_internal_parse_version_number(data, sizeof(data));
    assert_int_equal(-1, value);
}

static void parseVersionNumber_handleEscZero(void** state) {
    const char data[] = "123\000123";
    int value = hori_internal_parse_version_number(data, sizeof(data));
    assert_int_equal(-1, value);
}

static void parseVersionNumber_handleRightEscZeroFill(void** state) {
    const char data[] = "123\000\000";
    int value = hori_internal_parse_version_number(data, sizeof(data));
    assert_int_equal(123, value);
}


static const struct CMUnitTest unit_tests[] = {
    cmocka_unit_test(parseVersionNumber_invalidParameterDataSize),
    cmocka_unit_test(parseVersionNumber_invalidParameterData),
    cmocka_unit_test(parseVersionNumber_invalidParameters),
    cmocka_unit_test(parseVersionNumber_handleIntMax),
    cmocka_unit_test(parseVersionNumber_handleOverIntMax),
    cmocka_unit_test(parseVersionNumber_handleZero),
    cmocka_unit_test(parseVersionNumber_handleLeftZeroFill),
    cmocka_unit_test(parseVersionNumber_handleLeftPadding),
    cmocka_unit_test(parseVersionNumber_handleRightPadding),
    cmocka_unit_test(parseVersionNumber_handlePadding),
    cmocka_unit_test(parseVersionNumber_handleZeroFillAndPadding),
    cmocka_unit_test(parseVersionNumber_handleNegative),
    cmocka_unit_test(parseVersionNumber_handleEscZero),
    cmocka_unit_test(parseVersionNumber_handleRightEscZeroFill),
};

static struct CMUnitTestGroup test_group = {
    "parseVersionNumber",
    sizeof(unit_tests) / sizeof(unit_tests[0]),
    unit_tests
};

const struct CMUnitTestGroup* hori_intrenal_parse_version_number_tests() {
    return &test_group;
}
