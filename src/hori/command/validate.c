#include "validate.h"

#include <stdio.h>
#include <stdlib.h>
#include "../hori_yaml.h"

int hori_cli_command_validate(const char* input_file) {
    FILE* file = fopen(input_file, "r");
    struct hori_yaml_config config;
    hori_yaml_config_parse_file(&config, file);
    fclose(file);
    file = NULL;
    return EXIT_SUCCESS;
}
