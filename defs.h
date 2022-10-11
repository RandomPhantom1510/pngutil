//
// Created by zaraksh on 5/24/21.
//

#ifndef PNGUTIL_DEFS_H
#define PNGUTIL_DEFS_H

#include <stdint.h>

#define BYTE uint8_t

#define IEND_CHUNK 0x49454e44
#define IDAT_CHUNK 49444154 // Writing the hex representation for this causes
                            // IDATIntegrityOK() in test.c to hangup. To Fix.

typedef struct NODE {
    uint32_t length;
    uint32_t type;
    uint32_t crc;
    BYTE* data;
    struct NODE* next;
} NODE;

#endif //PNGUTIL_DEFS_H
