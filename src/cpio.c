#include "cpio.h"
#include "uart.h"
#include "string.h"
#include "utils.h"

char *cpio_get_filename(struct cpio_newc_header *p) {
    return (char *)(p + 1);
}

struct cpio_newc_header *cpio_get_first_file() {
    struct cpio_newc_header *p = (struct cpio_newc_header *)(CPIO_BASE);
    if (strcmp(cpio_get_filename(p), "TRAILER!!!") == 0)
        return (struct cpio_newc_header *)0; 
    return p;
}

struct cpio_newc_header *cpio_get_next_file(struct cpio_newc_header *p) {
    uint32_t namesize = hex_to_int(p->c_namesize, 8);
    uint32_t filesize = hex_to_int(p->c_filesize, 8);
    namesize += (4 - (sizeof(struct cpio_newc_header) + namesize) % 4) % 4;
    filesize += (4 - filesize % 4) % 4;

    char *next_p = (char *)(p + 1);
    next_p += namesize + filesize;
    if (strcmp(cpio_get_filename((struct cpio_newc_header *)next_p), "TRAILER!!!") == 0)
        return (struct cpio_newc_header *)0; 
    return (struct cpio_newc_header *)next_p;
}

void *cpio_get_file_address(char *filename) {
    struct cpio_newc_header *p = cpio_get_first_file();
    while(p != 0) {
        if (strcmp(filename, cpio_get_filename(p)) == 0) {
            uint32_t namesize = hex_to_int(p->c_namesize, 8);
            namesize += (4 - (sizeof(struct cpio_newc_header) + namesize) % 4) % 4;
            return (void *)((char *)(p + 1) + namesize);
        }
        p = cpio_get_next_file(p);
    }
    return (void *)0;
}

void cpio_read_file(struct cpio_newc_header *p, uint32_t offset, char *buf, uint32_t size) {
    uint32_t namesize = hex_to_int(p->c_namesize, 8);
    namesize += (4 - (sizeof(struct cpio_newc_header) + namesize) % 4) % 4;
    char *start = (char *)(p + 1) + namesize + offset;
    strncpy(start, buf, size);
}

void ls() {
    uart_puts("Filesize   Filename\n");
    struct cpio_newc_header *p = cpio_get_first_file();
    while(p != 0) {
        uart_hex(hex_to_int(p->c_filesize, 8), 1);
        uart_puts(" ");
        uart_puts(cpio_get_filename(p));
        uart_puts("\n");
        p = cpio_get_next_file(p);
    }
}

void cat() {
    uart_puts("Filename: ");
    char filename[64];
    uart_gets(filename, 64);

    struct cpio_newc_header *p = cpio_get_first_file();
    while (p != 0) {
        if (strcmp(filename, cpio_get_filename(p)) == 0) {
            uint32_t filesize = hex_to_int(p->c_filesize, 8);
            uint32_t offset = 0;
            char buf[512];
            while (filesize > offset) {
                cpio_read_file(p, offset, buf, sizeof(buf) - 1);
                buf[sizeof(buf) - 1] = '\0';
                offset += sizeof(buf) - 1;
                uart_puts(buf);
            }
            return;
        }
        p = cpio_get_next_file(p);
    }
    uart_puts("File is not found.\n");
}