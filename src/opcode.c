#include <stdint.h>
#include <stddef.h>

#include "opcode.h"

static const evm_opcode_t opcode_table[] = {
    { 0x00, "STOP",    0 },
    { 0x01, "ADD",     0 },
    { 0x02, "MUL",     0 },
    { 0x03, "SUB",     0 },
    { 0x04, "DIV",     0 },
    { 0x05, "SDIV",    0 },
    { 0x06, "MOD",     0 },
    { 0x07, "SMOD",    0 },
    { 0x08, "ADDMOD",  0 },
    { 0x09, "MULMOD", 0 },
    { 0x0a, "EXP",     0 },
    { 0x0b, "SIGNEXTEND", 0 },

    { 0x10, "LT",     0 },
    { 0x11, "GT",     0 },
    { 0x12, "SLT",    0 },
    { 0x13, "SGT",    0 },
    { 0x14, "EQ",     0 },
    { 0x15, "ISZERO", 0 },
    { 0x16, "AND",    0 },
    { 0x17, "OR",     0 },
    { 0x18, "XOR",    0 },
    { 0x19, "NOT",    0 },

    { 0x1a, "BYTE",   0 },
    { 0x1b, "SHL",    0 },
    { 0x1c, "SHR",    0 },

    { 0x50, "POP",    0 },
    { 0x52, "MSTORE", 0 },
    
    { 0x60, "PUSH1",  1 },
    { 0x61, "PUSH2",  2 },
    { 0x62, "PUSH3",  3 },
    { 0x63, "PUSH4",  4 },
    { 0x64, "PUSH5",  5 },
    { 0x65, "PUSH6",  6 },
    { 0x66, "PUSH7",  7 },
    { 0x67, "PUSH8",  8 },
    { 0x68, "PUSH9",  9 },
    { 0x69, "PUSH10", 10 },
    { 0x6a, "PUSH11", 11 },
    { 0x6b, "PUSH12", 12 },
    { 0x6c, "PUSH13", 13 },
    { 0x6d, "PUSH14", 14 },
    { 0x6e, "PUSH15", 15 },
    { 0x6f, "PUSH16", 16 },
    { 0x70, "PUSH17", 17 },
    { 0x71, "PUSH18", 18 },
    { 0x72, "PUSH19", 19 },
    { 0x73, "PUSH20", 20 },
    { 0x74, "PUSH21", 21 },
    { 0x75, "PUSH22", 22 },
    { 0x76, "PUSH23", 23 },
    { 0x77, "PUSH24", 24 },
    { 0x78, "PUSH25", 25 },
    { 0x79, "PUSH26", 26 },
    { 0x7a, "PUSH27", 27 },
    { 0x7b, "PUSH28", 28 },
    { 0x7c, "PUSH29", 29 },
    { 0x7d, "PUSH30", 30 },
    { 0x7e, "PUSH31", 31 },
    { 0x7f, "PUSH32", 32 },
    
    { 0xFE, "INVALID", 0 }

};

const evm_opcode_t *evm_get_opcode(uint8_t opcode) {
    
    size_t len = sizeof(opcode_table) / sizeof(opcode_table[0]);

    for(size_t i = 0; i < len; i++) {

        if(opcode_table[i].byte == opcode){
            return &opcode_table[i];
        }
    }
    
    return NULL;

}

int evm_opcode_is_push(uint8_t byte) {
    return byte >= 0x60 && byte <= 0x7f;
}
