#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "tests.h"


int main(void) {

    struct CMUnitTestGroup* tests = hori_intrenal_parse_firmware_version_tests();
    return _cmocka_run_group_tests(tests->name, tests->unit_tests, tests->unit_tests_count, NULL, NULL);
}
