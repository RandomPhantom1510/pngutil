//
// Created by zaraksh on 5/24/21.)
//
#include "defs.h"
#include "tests.h"

bool IDATIntegrityOK(NODE* head) {

    NODE* cursor = head;
    short int flag = -1;
    // -1 = IDAT chunks not encounterd yet
    // 0 = First IDAT chunk encountered
     // 1 = First non-IDAT chunk after flag hit zero --> No more IDAT
     // chunks allowed.
    while(cursor != NULL) {

        if(cursor->type == IDAT_CHUNK && flag == -1) {flag = 0;}
        if(cursor->type == IDAT_CHUNK && flag == 0) {continue;}
        if(cursor->type != IDAT_CHUNK && flag == 0) {flag = 1;}
        if(cursor->type != IDAT_CHUNK && flag == 1) {return false;}

        cursor = cursor->next;
    }
    return true;
}

bool CRCCheckOK(NODE* head) {

    NODE* cursor = head;
    while(cursor != NULL) {
        // Check CRC on type and data fields.
        cursor = cursor->next;
    }
    return true;
}