#ifndef _CPIO_H_
#define _CPIO_H_

#include "stdint.h"

#define CPIO_BASE 0x8000000

struct cpio_newc_header {
    char c_magic[6];
    char c_ino[8];
    char c_mode[8];
    char c_uid[8];
    char c_gid[8];
    char c_nlink[8];
    char c_mtime[8];
    char c_filesize[8];
    char c_devmajor[8];
    char c_devminor[8];
    char c_rdevmajor[8];
    char c_rdevminor[8];
    char c_namesize[8];
    char c_check[8];
};

char *cpio_get_filename(struct cpio_newc_header *p);
struct cpio_newc_header *cpio_get_first_file();
struct cpio_newc_header *cpio_get_next_file(struct cpio_newc_header *p);
void *cpio_get_file_address(char *filename);
void cpio_read_file(struct cpio_newc_header *p, uint32_t offset, char *buf, uint32_t size);
void ls();
void cat();

#endif