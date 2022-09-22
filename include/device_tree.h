#ifndef _DEVICE_TREE_H_
#define _DEVICE_TREE_H_

#include "stdint.h"

#define DTB_BASE        ((volatile uint64_t *)(0x040000))

#define FDT_BEGIN_NODE	0x01
#define FDT_END_NODE	0x02
#define FDT_PROP	    0x03
#define FDT_NOP		    0x04
#define FDT_END		    0x09

struct fdt_header {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
};

struct fdt_node_header {
	uint32_t tag;
	char name[0];
};
 
struct fdt_property_header {
	uint32_t tag;
	uint32_t len;
	uint32_t nameoff;
	char data[0];
};

void *parse_node(struct fdt_node_header *header);
void *parse_property(struct fdt_property_header *header, char *strings_base);
void parse_device_tree();

#endif