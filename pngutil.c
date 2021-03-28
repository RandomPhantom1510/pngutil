// PNG file analyzer

/*
    TO DO:

    Argparsing using argp.h

*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define RED     "\033[0;31m" // These will be used to add coloured output to terminals.
#define GRN     "\033[0;32m" // This horribleness is being done because I don't want to deal with the external library
#define YLW     "\033[0;33m" // for coloured terminal output.
#define CLR      "\033[0m" 

#define BYTE uint8_t

typedef struct node {
    uint32_t length;
    uint32_t type;
    uint32_t crc;
    BYTE* data;
    struct node* next;
} node;

node* head = NULL; // This is used to make the linked list global

// Function prototypesv
void prnt();
bool IDATIntegrityOK();

int main(int argc, char const* argv[]) {

    // Help text
    if(argc != 2) {
        printf("Usage: %s <FILE>\n", argv[0]);
        return 4;
    }
 
    FILE* infile = fopen(argv[1], "r");
    if (infile == NULL) {return 1;}

    
    // Magic array to check magic signature
    BYTE sig[8] = {137,80,78,71,13,10,26,10}; // PNG magic numbers
    BYTE sigcheck[8];
    if (sigcheck == NULL) {fclose(infile);return 3;}
    fread(&sigcheck, sizeof(BYTE), 8, infile); // Assign first 8-bytes of infile to sigcheck[]
    // Check infile's magic signature
    if (memcmp(sig, sigcheck, sizeof(sig)) != 0) {printf("Aborted. Not a PNG file or file corrupted.\n");fclose(infile);return 2;}
    
    /* 
    PNG files begin with a magic no. - followed by a sequence of chunks.
    
    Chunks can be of 2 types -  0 - LENGTH-TYPE-DATA-CRC
                                1 - LENGTH-TYPE-CRC                     

    First chunk in the sequence will of type IHDR, the last will be of type IEND   
    
        where,  LENGTH = 4-byte unsigned int = no. of bytes in DATA. [0 >= LENGTH < 2^32].
                TYPE = 4-byte, where each byte is (65-90) OR (97-122) (i.e., a-z OR A-Z). Must be treated as binary values NOT character strings.
                DATA = LENGTH no. of bytes - i.e., [0,2^32).
                CRC = 4-byte CRC calculated on TYPE + DATA. Always present even for LENGTH = 0.

        therefore, each chunk has a total no. of 4+4+LENGTH+4 bytes = 12+LENGTH bytes therefore chunksize is always < 12+2^32

        Read each chunk and add it to a linked list.
    */

    BYTE buffer;
    while (true) { // This loop reads infile chunk-by-chunk and adds each chunk to a global linked list until IEND chunk is reached.
        
        node* newNode = (node*) malloc(sizeof(node));

        // Read LENGTH value; we have to buffer and then append to length hexdigit-by-hexdigit to account for endian-ness
        for(unsigned j = 0; j < 4; ++j) {
            fread(&buffer, 1, sizeof(BYTE), infile);
            newNode->length = (newNode->length<<8) | buffer;    // If length is deadbe and buffer is ef this makes sure that length
                                                                // ends up as deadbeef and not deadef
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

            node* cursor = head;

            while(cursor != NULL && cursor->next != NULL) {
                cursor = cursor->next;
            }

            cursor->next = newNode;

        } else {
            head = newNode;
        }

        if(newNode->type == 0x49454e44) {break;}
    }

    // Now we have a linked list of chunks, we can add new tests modularly.

    prnt();

    if(IDATIntegrityOK()) { // IDAT Integrity test makes sure that all IDAT chunks are consecutive.
        printf("%sIDAT Integrity test passed.%s\n", GRN, CLR);
    } else {
        printf("%sIDAT integrity test failed.%s\n", RED, CLR);
    }

    fclose(infile);
    return 0;
}

void prnt() {

    node *cursor = head;

    while(cursor != NULL) {
        printf("Length: %x\t", cursor->length);
        printf("Type: %c%c%c%c\t", (cursor->type & 0xff000000)>>24, (cursor->type & 0xff0000)>>16, (cursor->type & 0xff00)>>8, cursor->type & 0xff);
        printf("CRC: %x\n", cursor->crc);
        cursor = cursor->next;
    }

    return;
}

bool IDATIntegrityOK() { // Returns false if IDAT Integrity test fails
        
    node *cursor = head;
    short int flag = -1; // -1 = IDAT chunks not encounterd yet
                         // 0 = First IDAT chunk encountered
                         // 1 = First non-IDAT chunk after flag hit zero --> No more IDAT chunks allowed.
    while(cursor != NULL) {
        
        if(cursor->type == 49444154 && flag == -1) {flag = 0;}
        if(cursor->type == 49444154 && flag == 0) {continue;}
        if(cursor->type != 49444154 && flag == 0) {flag = 1;}
        if(cursor->type != 49444154 && flag == 1) {return false;}

        cursor = cursor->next;
    }
    return true;    
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
// "%c%c%c%c\t", (node->type & 0xff000000)>>24, (node->type & 0xff0000)>>16, (node->type & 0xff00)>>8, node->type & 0xff
