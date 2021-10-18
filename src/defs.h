//
// Created by zaraksh on 5/24/21.
//

#ifndef PNGUTIL_DEFS_H
#define PNGUTIL_DEFS_H

#include <stdint.h>

// These will be used to add colored output to terminals.
// This horribleness is being done because I don't want to deal with the
//      external library for coloured output.
#define RED     "\033[0;31m"
#define GRN     "\033[0;32m"
#define YLW     "\033[0;33m"
#define CLR      "\033[0m"

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
