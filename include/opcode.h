#ifndef OPCODE_H
#define OPCODE_H

#include <stdint.h>

typedef struct{
    uint8_t byte;
    const char *mnemonic;
    uint8_t imm_bytes;
} evm_opcode_t;

const evm_opcode_t *evm_get_opcode(uint8_t opcode);
int evm_opcode_is_push(uint8_t byte);

#endif
