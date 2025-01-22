#include "hori_yaml_product.h"

#include "hori_yaml_utils.h"

// @todo bug in emit - product is emited as STR and not INT
int hori_yaml_parse_product(yaml_document_t* document, int node_index, const hori_context_t* context) {
    if (context == NULL)
        context = hori_context();
    if (document == NULL)
        return -1;
    if (context == NULL || context->devices == NULL)
        return -1;
    yaml_node_t* product_node = yaml_document_get_node(document, node_index);
    if (product_node == NULL)
        return -1;
    if (product_node->type != YAML_SCALAR_NODE)
        return -1;
    int value = 0;
    if (hori_yaml_int(&value, product_node->data.scalar.value, product_node->data.scalar.length)) {
        return value < 0 ? -1 : value;
    }
    for (hori_device_config_t* device = context->devices; device != NULL; device = device->next) {
        if (!product_node->data.scalar.value)
            continue;
        if (hori_yaml_compare(device->firmware_name, strlen(device->firmware_name), product_node->data.scalar.value, product_node->data.scalar.length))
            return device->product;
    }
    return -1;
}

int hori_yaml_emit_product(yaml_emitter_t* emitter, int product) {
    return -1;
}
