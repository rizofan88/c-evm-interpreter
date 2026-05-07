#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "types.h"
#include "opcode.h"
#include "math.h"


void stack_printer(const evm_t *evm) {

    if (!evm) return;

    printf("\n");
    
    if(evm->stack_pointer == 0) {
        printf("STACK EMPTY\n");
        printf("-----------\n");
        printf("\n");
        return;
    }
    
    printf("STACK (next empty slot = [%zu])\n", evm->stack_pointer);
    printf("----------------------------------------------------------------------\n");

        for (size_t i = evm->stack_pointer; i > 0; i--) {

            size_t index = i - 1;

            printf("[%zu] 0x", index);

            for (size_t j = 0; j < 32; j++) {
                printf("%02x", evm->stack[index][j]);
            }

            printf("\n");

        }

    printf("\n");

}

evm_status_t push_to_stack(evm_t *evm, 
                           const evm_opcode_t *opcode, 
                           const uint8_t *bytes,
                           size_t len) 
{

    if(!evm || !opcode || !bytes) return EVM_INTERNAL_ERROR;

    if (evm->pc >= len) {
        return EVM_INVALID_BYTECODE;
    }
    
    uint8_t imm_bytes = opcode->imm_bytes;
    size_t remaining = len - evm->pc - 1; 
    
    if (imm_bytes > remaining) {
        return EVM_INVALID_BYTECODE;
    }
    
    if(evm->stack_pointer >= EVM_STACK_LIMIT) {
        return EVM_STACK_OVERFLOW;
    }
    
    uint8_t *slot = evm->stack[evm->stack_pointer];

    memset(slot, 0, 32);
    
    memcpy(
        slot + (32 - imm_bytes),
        bytes + evm->pc + 1,
        imm_bytes 
    );
    
    evm->stack_pointer ++;
    evm->pc += imm_bytes;

    return EVM_OK;
}

evm_status_t push_one_to_stack(evm_t *evm, const uint8_t *bytes) {

    if(!evm || !bytes) {
        return EVM_INTERNAL_ERROR;
    }

    if(evm->stack_pointer >= EVM_STACK_LIMIT) {
        return EVM_STACK_OVERFLOW;
    }
    
    uint8_t *slot = evm->stack[evm->stack_pointer];

    memcpy(
        slot,
        bytes,
        32 
    );
    
    evm->stack_pointer ++;
    
    return EVM_OK;
}

evm_status_t pop_from_stack(evm_t *evm, size_t items_to_pop) {

    if(!evm) {
        return EVM_INTERNAL_ERROR;
    }

    if(evm->stack_pointer < items_to_pop) {
        return EVM_STACK_UNDERFLOW;
    }

    evm->stack_pointer -= items_to_pop;

    return EVM_OK;

}
