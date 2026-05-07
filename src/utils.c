#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include "utils.h"
#include "math.h"
#include "types.h"

static int hex_char_to_value(char c) {

    if(c >= '0' && c <= '9') {
        return c - '0';
    }

    if(c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }

    if(c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }

    return -1;
}


uint8_t *hex_to_bytes(const char *hex, size_t *array_len) {

    if(!hex || !array_len) {
        return NULL;
    }

    size_t len = strlen(hex);

    if(len % 2 != 0)
        return NULL;

    size_t n_bytes = len / 2;

    uint8_t *bytes = malloc(sizeof(uint8_t)*n_bytes);
    
    if(!bytes) {
        return NULL;
    }
    
    for(size_t i = 0; i < n_bytes; i++) {

        char high = hex[2*i];
        char low = hex[2*i + 1];

        int high_value = hex_char_to_value(high);
        int low_value = hex_char_to_value(low);


        if(high_value < 0 || low_value < 0) {
            free(bytes);
            return NULL;
        }
        
        bytes[i] = (high_value << 4) | low_value;
    }

    *array_len = n_bytes;
    return bytes;

}


uint256_t bytes_to_uint256(const uint8_t *bytes) {
    
    uint256_t uint256 = {0};

    for(int i = 0; i < 4; i++) {
     
        uint64_t v = 0;
     
        for(int j = 0; j < 8; j++){
            v = (v << 8) | bytes[i * 8 + j];
        }
     
        uint256.w[3 - i] = v;

    }

    return uint256;

}


uint8_t *uint256_to_bytes(uint256_t *uint256) {
    
    uint8_t *bytes = malloc(sizeof(uint8_t) * 32);
    
    if(!bytes) {
        return NULL;
    }
    
    memset(bytes, 0, UINT256_BYTES);

    for(int i = 0; i < 4; i++) {
        
        for(int j = 0; j < 8; j++) {

            uint8_t byte = 0;
            uint64_t x = uint256->w[i] >> (8 * j);
            byte = (uint8_t)(x & 0xFF);

            bytes[UINT256_BYTES - 1 - (i * 8 + j)] = byte;
        }
    }

    return bytes;

}


void print_bytes32(const uint8_t *bytes) {
  
    printf("RESULT: \n");
    printf("----------------------------------------------------------------\n");

    for(int x = 0; x < 32; x++){
        printf("%02x", bytes[x]);
    }

    printf("\n");

}


void print_uint256(const char *label, const uint256_t *x) {

    printf("%s: 0x%016llx%016llx%016llx%016llx\n",
           label,
           (unsigned long long)x->w[3],
           (unsigned long long)x->w[2],
           (unsigned long long)x->w[1],
           (unsigned long long)x->w[0]);

}
