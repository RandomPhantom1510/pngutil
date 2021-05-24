//
// Created by zaraksh on 5/24/21.
//

#ifndef PNGUTIL_TESTS_H
#define PNGUTIL_TESTS_H

#include "defs.h"
#include <stdbool.h>
#include <stddef.h>

bool IDATIntegrityOK(NODE* head);
bool CRCCheckOK(NODE* head);

#endif //PNGUTIL_TESTS_H
