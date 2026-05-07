#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "opcode.h"
#include "disasm.h"


static void print_bytes_to_hex(const uint8_t *byte_array, size_t len) {
    for(size_t i = 0; i < len; i++){
        printf("%02X", byte_array[i]);
    }
}

void disasm(const uint8_t *code, size_t code_size) {

    if(!code || code_size == 0) return;

    size_t pc = 0;
    
    printf("\n%-9s%-10s%-15s%s\n",
           "PC", "OPCODE", "MNEMONIC", "IMMEDIATE");

    printf("------------------------------------------------------------\n");

    while(pc < code_size) {

        const evm_opcode_t *op = evm_get_opcode(code[pc]);
        
        if (!op) {
            printf("%-6zu   0x%02X      %-12s   %s\n", pc, code[pc], "INVALID", "");
            pc++;
            continue;
        }

        size_t imm_len = op->imm_bytes;
        size_t remaining = code_size - pc - 1;

        if (imm_len > remaining) {
            imm_len = remaining;
        }

        printf("%-6zu   0x%02X      %-12s   ", pc, code[pc], op->mnemonic);
        print_bytes_to_hex(&code[pc + 1], imm_len);
        printf("\n");

        pc += 1 + imm_len;
    }

}

