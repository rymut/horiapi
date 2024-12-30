#pragma once

#include <yaml.h>

#include "hori_yaml.h"

/** @brief Parse hori config audio
 */
int hori_yaml_parse_audio(yaml_parser_t* parser, struct hori_config_audio* config);

/** @brief Emit hori config audio

    @param emitter[in|out] The yaml emiter emitter to write to (cannot be NULL)
    @param audio[in] The audio structure to write (can be NULL)

    @returns
        The function returns -1 on error
  */
int hori_yaml_emit_audio(yaml_emitter_t* emitter, struct hori_config_audio* audio);
