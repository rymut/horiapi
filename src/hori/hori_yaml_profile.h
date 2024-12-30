#pragma once

#include <yaml.h>

#include <horiapi/hori_profile.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Parse hori product

    @returns
        The function returns -1 when product is not valid
 */
int hori_yaml_parse_profile(yaml_parser_t* parser, struct hori_profile* profile, int platform);

/** @brief Emit hori config audio

    @param emitter[in|out] The yaml emiter emitter to write to (cannot be NULL)
    @param audio[in] The audio structure to write (can be NULL)

    @returns
        The function returns -1 on error
  */
int hori_yaml_emit_profile(yaml_emitter_t* emitter, const struct hori_profile* profile, int platform);

#ifdef __cplusplus
}
#endif
