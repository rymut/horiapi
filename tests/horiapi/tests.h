#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
struct CMUnitTest;

struct CMUnitTestGroup {
    const char* name;
    size_t unit_tests_count;
    const struct CMUnitTest* unit_tests;
};

const struct CMUnitTestGroup* hori_intrenal_parse_firmware_version_tests();
