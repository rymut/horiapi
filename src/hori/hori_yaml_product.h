#pragma once

#include <yaml.h>

/** @brief Parse hori product

    @returns
        The function returns -1 when product is not valid
 */
int hori_yaml_parse_product(yaml_parser_t* parser, int *profile);

/** @brief Emit hori config audio

    @param emitter[in|out] The yaml emiter emitter to write to (cannot be NULL)
    @param audio[in] The audio structure to write (can be NULL)

    @returns
        The function returns -1 on error
  */
int hori_yaml_emit_product(yaml_emitter_t* emitter, int product);

