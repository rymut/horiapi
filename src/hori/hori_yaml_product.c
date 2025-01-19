#include "hori_yaml_product.h"

int hori_yaml_parse_product(yaml_document_t* document, int node_index, const hori_context_t* context) {
    if (document == NULL)
        return -1;
    if (context == NULL)
        context = hori_context();
    if (context == NULL || context->devices == NULL)
        return -1;
    yaml_node_t* product_node = yaml_document_get_node(document, node_index);
    if (product_node == NULL)
        return -1;
    if (product_node->type != YAML_SCALAR_NODE)
        return -1;
    int is_string = hori_yaml_compare_string(product_node->tag, YAML_STR_TAG);
    int is_int = hori_yaml_compare_string(product_node->tag, YAML_INT_TAG);
    if (!(is_string || is_int))
        return -1;
    for (hori_device_config_t* device = context->devices; device != NULL; device = device->next) {
        if (!product_node->data.scalar.value)
            continue;
        if (is_string) {
            if (hori_yaml_compare(device->firmware_name, strlen(device->firmware_name), product_node->data.scalar.value, product_node->data.scalar.length)) {
                return device->product;
            }
        }
        if (is_int) {
            int value = atoi(product_node->data.scalar.value);
            return value < 0 ? -1 : value;
        }
    }
    return -1;
}

int hori_yaml_emit_product(yaml_emitter_t* emitter, int product) {
    return -1;
}
