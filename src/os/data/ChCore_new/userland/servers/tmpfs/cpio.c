/*
 * Copyright (c) 2022 Institute of Parallel And Distributed Systems (IPADS)
 * ChCore-Lab is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan
 * PSL v1. You may obtain a copy of Mulan PSL v1 at:
 *     http://license.coscl.org.cn/MulanPSL
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
 * Mulan PSL v1 for more details.
 */

#include "cpio.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <chcore/assert.h>

#define cpio_info(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define cpio_zalloc(sz)     calloc(1, sz)
#define cpio_free(obj)      free(obj)

static u64 hex8_u64(const char s[8])
{
        u64 u = 0;

        for (int i = 0; i < 8; ++i) {
                u <<= 4;

                if (s[i] >= 'A' && s[i] <= 'F') {
                        u += s[i] - 'A' + 10;
                } else {
                        u += s[i] - '0';
                }
        }

        return u;
}

static int cpio_parse_header(const void* addr, struct cpio_header* header)
{
        const struct cpio_newc_header* newc = addr;

        // cpio_info("print in text: %s\n", addr);
        /* headers other than newc are not supported */
        chcore_bug_on(strncmp(newc->c_magic, "070701", 6));

        header->c_ino = hex8_u64(newc->c_ino);
        header->c_mode = hex8_u64(newc->c_mode);
        header->c_uid = hex8_u64(newc->c_uid);
        header->c_gid = hex8_u64(newc->c_gid);
        header->c_nlink = hex8_u64(newc->c_nlink);
        header->c_mtime = hex8_u64(newc->c_mtime);
        header->c_filesize = hex8_u64(newc->c_filesize);
        header->c_devmajor = hex8_u64(newc->c_devmajor);
        header->c_devminor = hex8_u64(newc->c_devminor);
        header->c_rdevmajor = hex8_u64(newc->c_rdevmajor);
        header->c_rdevminor = hex8_u64(newc->c_rdevminor);
        header->c_namesize = hex8_u64(newc->c_namesize);
        header->c_check = hex8_u64(newc->c_check);

        // cpio_info("header address is 0x%lx\n", (u64) header);
        return 0;
}

int cpio_extract_file(const void* addr, const char* dirat)
{
        return 0;
}

void cpio_init_g_files(void)
{
        g_files.head.next = NULL;
        g_files.tail = &g_files.head;
}

static struct cpio_file* cpio_alloc_file(void)
{
        return cpio_zalloc(sizeof(struct cpio_file));
}

static void cpio_add_file(struct cpio_file* f)
{
        f->next = NULL;
        g_files.tail->next = f;
        g_files.tail = f;
}

#define ALIGN4_UP(x) ((((u64)x) & (~3llu)) + ((!!(((u64)x) & 3)) << 2))

void cpio_extract(const void* addr, const char* dirat)
{
        const char* p = addr;
        struct cpio_file* f;
        int err;

        for (;;) {
                f = cpio_alloc_file();

                // printf("cpio_alloc_file returns %p\n", f);
                if (f == NULL) {
                        chcore_warn(
                                "cpio_alloc_file fails due to lack of memory.\n");
                        return;
                }

                chcore_bug_on(!f);

                err = cpio_parse_header(p, &f->header);
                chcore_bug_on(err);
                p += sizeof(struct cpio_newc_header);

                if (0 == strcmp(p, "TRAILER!!!")) {
                        cpio_free(f);
                        break;
                }

                f->name = p;
                p += f->header.c_namesize;
                p = (void*)ALIGN4_UP(p);

                f->data = p;
                p += f->header.c_filesize;
                p = (void*)ALIGN4_UP(p);

                cpio_add_file(f);
        }
}

int cpio_extract_single(const void* addr, const char* target,
                        int (*cpio_single_file_filler)(const void* start,
                                                       size_t size, void* data),
                        void* data)
{
        const char* p = addr;
        struct cpio_header header;
        const char* filename;

        /* Skip leading '/' */
        target = target + 1;

        for (;;) {
                int err;

                err = cpio_parse_header(p, &header);
                chcore_bug_on(err);
                p += sizeof(struct cpio_newc_header);

                if (0 == strcmp(p, "TRAILER!!!")) {
                        break;
                }

                filename = p;
                p += header.c_namesize;
                p = (void*)ALIGN4_UP(p);

                if (0 == strcmp(filename, target)) {
                        return cpio_single_file_filler(
                                p, header.c_filesize, data);
                }

                p += header.c_filesize;
                p = (void*)ALIGN4_UP(p);
        }

        return -ENOENT;
}
