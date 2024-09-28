#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>

#include "tests.h"


int main(void) {

    struct CMUnitTestGroup* tests[] = {
        hori_intrenal_parse_firmware_version_tests(),
        hori_intrenal_parse_version_number_tests(),
    };
    int sum = 0;
    for (size_t group = 0; group < sizeof(tests) / sizeof(tests[0]); ++group) {
        struct CMUnitTestGroup* test_group = tests[group];
        sum += _cmocka_run_group_tests(test_group->name, test_group->unit_tests, test_group->unit_tests_count, NULL, NULL);
    }
    if (sum == 0) {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;

}
