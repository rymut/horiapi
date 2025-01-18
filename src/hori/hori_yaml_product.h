#pragma once

#include <yaml.h>

#include <horiapi/horiapi.h>

/** @brief Parse product node

    @param[in] document The yaml document
    @param[in] node_index The yaml document node index
    @param[in] context The hori context (when NULL uses default context)

    @returns
        This function returns -1 on error, otherwise function returns product id @see hori_product
 */
int hori_yaml_parse_product(yaml_document_t* document, int node_index, const hori_context_t* context);

/** @brief Emit hori config audio

    @param emitter[in|out] The yaml emiter emitter to write to (cannot be NULL)
    @param audio[in] The audio structure to write (can be NULL)

    @returns
        The function returns -1 on error
  */
int hori_yaml_emit_product(yaml_emitter_t* emitter, int product);

