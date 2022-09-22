#include "device_tree.h"
#include "uart.h"
#include "string.h"
#include "utils.h"

void *parse_node(struct fdt_node_header *header) {
    // uart_puts("node: ");
    // uart_puts(&(header->name[0]));
    // uart_puts("\n");

    uint32_t name_len = strlen(&(header->name[0])) + 1;
    if (name_len % 4 != 0) {
        name_len = (4 - name_len % 4) + name_len;
    }
    return (void *)((char *)header + sizeof(header->tag) + name_len);
}

void *parse_property(struct fdt_property_header *header, char *strings_base) {
    // uart_puts("  property: ");
    // uart_puts(strings_base + convert_endian(header->nameoff));
    // uart_puts(" = ");

    uint32_t len = convert_endian(header->len);
    // for (int i = 0; i < len; i++) {
    //     uart_putc(header->data[i]);
    // }
    // uart_puts(";\n");

    if (len % 4 != 0) {
        len += 4 - len % 4;
    }
    return (void *)((char *)header + len + sizeof(header->tag) + sizeof(header->len) + sizeof(header->nameoff));
}

void parse_device_tree(void (*callback_func)(char *node_name, char *property_name, void *property_value)) {
    struct fdt_header *header = (struct fdt_header *)(*DTB_BASE);
    uint32_t off_dt_struct = convert_endian(header->off_dt_struct);
    uint32_t off_dt_strings = convert_endian(header->off_dt_strings);

    void *strings_base = (void *)((uint64_t)header + off_dt_strings);
    void *p = (void *)((uint64_t)header + off_dt_struct);

    char *node_name;
    char *property_name;
    char *property_value;
    while (1) {
        uint32_t tag = convert_endian(*(uint32_t *)p);
        if (tag == FDT_BEGIN_NODE) {
            struct fdt_node_header *node_p = (struct fdt_node_header *)p;
            node_name = &(node_p->name[0]);
            p = parse_node(node_p);
        } else if (tag == FDT_PROP) {
            struct fdt_property_header *property_p = (struct fdt_property_header *)p;
            property_name = strings_base + convert_endian(property_p->nameoff);
            property_value = &(property_p->data[0]);
            p = parse_property(property_p, strings_base);
            callback_func(node_name, property_name, property_value);
        } else if (tag != FDT_END) {
            p = (char *)p + sizeof(tag);
        } else {
            break;
        }
    }
}