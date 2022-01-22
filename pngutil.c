// TODO: Argument parsing using argp.h
// TODO: CRC validity check

/* This program analyzes PNG files according to the Second Edition (10 Nov
 * 2003) specification. https://www.w3.org/TR/PNG/
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "tests.h"

void prnt(NODE* head);

int main(int argc, char const* argv[]) {

    printf("pngutil by Z. Rahman\n--------------------\n");

    if(argc != 2) {
        printf("Usage: %s <FILE>\n", argv[0]);
        return 4;
    }

    FILE* infile = fopen(argv[1], "r");
    if (infile == NULL) {return 1;}


    // PNG magic numbers must match first 8 bytes of file.
    BYTE sig[8] = {137,80,78,71,13,10,26,10};
    BYTE sigcheck[8];
    if (sigcheck == NULL) {fclose(infile); return 3;}
    fread(&sigcheck, sizeof(BYTE), 8, infile);

    char ic;
    if (memcmp(sig, sigcheck, sizeof(sig)) != 0) {
        printf("Not a PNG file or file unreadable."
                                "Continue anyway? [y/n]\n");
        scanf("%c", &ic);
        if (ic == 'y' || ic == 'Y'){}
	else {
		fclose(infile);
        	return 2;
	}
    }

    // LENGTH (4-byte unsigned int) = no. of bytes in DATA.
    //                          i.e., [0 >= LENGTH < 2^32].
    // TYPE (4-byte int ) ASCII a-z OR A-Z = type of chunk
    // TYPE must be treated as binary values NOT character strings.

    // DATA = LENGTH no. of bytes of data.
    // CRC = 4-byte CRC calculated on TYPE + DATA. Always present
    // even for LENGTH = 0.

    // Reads PNG file chunk-by-chunk and add it to a linked list.
    NODE* head = NULL;
    BYTE buffer;

    while (true) {

        NODE* newNode = (NODE*) malloc(sizeof(NODE));

	    // If length is 'deadbe' and buffer is 'ef' this makes sure
	    // that length ends up as deadbeef and not deadef
        for(unsigned j = 0; j < 4; ++j) {
            fread(&buffer, 1, sizeof(BYTE), infile);
            newNode->length = (newNode->length<<8) | buffer;
        }

        // Read TYPE value
        for(unsigned i = 0; i < 4; ++i) {
            fread(&buffer, 1, sizeof(BYTE), infile);
            newNode->type = (newNode->type<<8) | buffer;
        }

        // Allocate LENGTH bytes of memory for data
        newNode->data = malloc(newNode->length * sizeof(BYTE));
        if (newNode->data == NULL) {fclose(infile);}

        // Populate DATA
        for(unsigned j = 0; j < newNode->length; ++j) {
            fread(&buffer, 1, sizeof(BYTE), infile);
            *(newNode->data) = (*(newNode->data)<<8) | buffer;
        }

        // Read CRC value
        for(unsigned i = 0; i < 4; ++i) {
            fread(&buffer, 1, sizeof(BYTE), infile);
            newNode->crc = (newNode->crc<<8) | buffer;
        }

        newNode->next = NULL;

        if (head != NULL) {
            NODE* cursor = head;

            while(cursor != NULL && cursor->next != NULL) {
                cursor = cursor->next;
            }
            cursor->next = newNode;

        } else {head = newNode;}

        if(newNode->type == IEND_CHUNK) {break;}
    }

    prnt(head);

    // IDAT Integrity test makes sure that all IDAT chunks are in order
    if(IDATIntegrityOK(head)) {
        printf("%sIDAT Integrity test passed.%s\n", GRN, CLR);
    } else {
        printf("%sIDAT integrity test failed.%s\n", RED, CLR);
    }

    // CRC check makes sure that CRC integrity is in order.
    if (CRCCheckOK(head)) {
        printf("%sCRC check passed.%s\n", GRN, CLR);
    } else {
        printf("%sCRC check failed.%s\n", RED, CLR);
    }

    fclose(infile);
    return 0;
}

void prnt(NODE* head) {

    NODE* cursor = head;

    while(cursor != NULL) {
        printf("Length: %x\t", cursor->length);
        printf("Type: %c%c%c%c\t\n",
				 (cursor->type & 0xff000000)>>24,
				 (cursor->type & 0xff0000)>>16,
				 (cursor->type & 0xff00)>>8,
				  cursor->type & 0xff
				// See note at bottom of file
	      );
        //printf("CRC: %08x\n", cursor->crc);
        cursor = cursor->next;
    }

    return;
}

/*      Error Codes

    1 - File IO error
    2 - File signature test failed
    3 - Runtime memory error
    4 - Incorrect usage
*/


/*      Hex equivalents of common chunk types

                IHDR    49484452
                PLTE    504c5445
                IDAT    49444154
                IEND    49454e44

                iCCP    69434350
                cHRM    6348524d
                bKGD    624b4744
                pHYs    70485973
                gAMA    67414d41
                tIME    74494d45
                tEXt    74455874
                iTXt    69545874
                zTXt    7a545874
*/

// This is the correct way to print char equivalents of chunk types
// "%c%c%c%c\t",
//              (NODE->type & 0xff000000)>>24,
//              (NODE->type & 0xff0000)>>16,
//		        (NODE->type & 0xff00)>>8,
//		        NODE->type & 0xff
