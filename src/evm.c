#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "evm.h"
#include "opcode.h"
#include "stack.h"
#include "operations.h"


evm_status_t evm_state(uint8_t *bytes, size_t len) {
   
    evm_t evm = {0};

    if (!bytes) {
        return EVM_INTERNAL_ERROR;
    }

    while (!evm.stopped && evm.pc < len) {

        const evm_opcode_t *opcode = evm_get_opcode(bytes[evm.pc]);

        if (!opcode) {
            return EVM_INVALID_OPCODE;
        }

        evm_status_t status = EVM_OK;

        if (evm_opcode_is_push(opcode->byte)) {
            status = push_to_stack(&evm, opcode, bytes, len);

        } else {

            switch(opcode->byte) {

                case 0x00: {
                    status = EVM_OK;
                    evm.stopped = 1;
                    break;
                }
                case 0x01: {
                    status = add(&evm);
                    break;
                } 
                case 0x02: {
                    status = mul(&evm);
                    break;

                } 
                case 0x03: {
                    status = sub(&evm);
                    break;

                } 
                case 0x04: {
                    status = divi(&evm);
                    break;
                } 
                case 0x06: {
                    status = mod(&evm);
                    break;
                } 
                case 0x50: {
                    status = pop_from_stack(&evm, 1);
                    break;
                }    
                default:
                    status = EVM_INVALID_OPCODE;
                    break;
            
            }
        
        }
       
        if(status != EVM_OK) {
            return status;
        }
       
        evm.pc++;
    }

    printf("\nExecution terminated successfully.\n");
    stack_printer(&evm);

    return EVM_OK;

}

