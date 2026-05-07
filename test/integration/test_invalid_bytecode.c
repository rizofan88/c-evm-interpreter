#include "test.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "evm.h"
#include "opcode.h"
#include "stack.h"
#include "operations.h"
#include "utils.h"

static void test_init_evm(evm_t *evm) {
    memset(evm, 0, sizeof(*evm));
}


evm_status_t test_evm_state(evm_t *evm, uint8_t *bytes, size_t len) {

    if (!bytes) {
        return EVM_INTERNAL_ERROR;
    }

    while (!evm->stopped && evm->pc < len){

        const evm_opcode_t *opcode = evm_get_opcode(bytes[evm->pc]);

        if (!opcode) {
            return EVM_INVALID_OPCODE;
        }

        evm_status_t status = EVM_OK;

        if (evm_opcode_is_push(opcode->byte)) {
            status = push_to_stack(evm, opcode, bytes, len);

        } else {
            switch(opcode->byte) {
                case 0x01: {
                    status = add(evm);
                    break;
                } 
                case 0x02: {
                    status = mul(evm);
                    break;

                } 
                case 0x03: {
                    status = sub(evm);
                    break;

                } 
                case 0x04: {
                    status = divi(evm);
                    break;
                } 
                case 0x06: {
                    status = mod(evm);
                    break;
                } 
                case 0x50: {
                    status = pop_from_stack(evm, 1);
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

        evm->pc++;
    }

    return EVM_OK;

}

static void test_operation_with_insufficient_stack_elements_returns_stack_underflow(void) {

    evm_t evm;
    test_init_evm(&evm);
    
    const char *input_hex = "600201";
    size_t bytecode_len = 0;
    uint8_t *bytecode = NULL;

    bytecode = hex_to_bytes(input_hex, &bytecode_len);
    TEST_ASSERT(bytecode != NULL);

    evm_status_t status = test_evm_state(&evm, bytecode, bytecode_len);

    TEST_ASSERT(status == EVM_STACK_UNDERFLOW);
    TEST_ASSERT(evm.stack_pointer == 1);
    TEST_ASSERT(evm.pc == bytecode_len - 1);

    free(bytecode);

}

static void test_operation_with_empty_stack_returns_stack_underflow(void) {

    evm_t evm;
    test_init_evm(&evm);
    
    const char *input_hex = "02";
    size_t bytecode_len = 0;
    uint8_t *bytecode = NULL;

    bytecode = hex_to_bytes(input_hex, &bytecode_len);
    TEST_ASSERT(bytecode != NULL);

    evm_status_t status = test_evm_state(&evm, bytecode, bytecode_len);

    TEST_ASSERT(status == EVM_STACK_UNDERFLOW);
    TEST_ASSERT(evm.stack_pointer == 0);
    TEST_ASSERT(evm.pc == 0);

    free(bytecode);

}

static void test_pop_with_no_stack_returns_stack_underflow(void) {

    evm_t evm;
    test_init_evm(&evm);
    
    const char *input_hex = "50";
    size_t bytecode_len = 0;
    uint8_t *bytecode = NULL;

    bytecode = hex_to_bytes(input_hex, &bytecode_len);
    TEST_ASSERT(bytecode != NULL);

    evm_status_t status = test_evm_state(&evm, bytecode, bytecode_len);

    TEST_ASSERT(status == EVM_STACK_UNDERFLOW);
    TEST_ASSERT(evm.stack_pointer == 0);
    TEST_ASSERT(evm.pc == 0);

    free(bytecode);

}

static void test_push_over_stack_limit_returns_stack_overflow(void) {

    evm_t evm;
    test_init_evm(&evm);
    
    size_t pushes = EVM_STACK_LIMIT + 1; 
    size_t hex_len = pushes * 4;

    char input_hex[hex_len + 1];
    char *instructions = "6002";

    memset(input_hex, 0, hex_len + 1);

    for(size_t i = 0; i < hex_len; i+=4) {
        memcpy(input_hex + i, instructions, 4);
    }

    input_hex[hex_len] = '\0';

    size_t bytecode_len = 0;
    uint8_t *bytecode = NULL;

    bytecode = hex_to_bytes(input_hex, &bytecode_len);

    TEST_ASSERT(bytecode != NULL);

    evm_status_t status = test_evm_state(&evm, bytecode, bytecode_len);

    TEST_ASSERT(status == EVM_STACK_OVERFLOW);
    TEST_ASSERT(evm.stack_pointer == EVM_STACK_LIMIT);
    TEST_ASSERT(evm.pc == (EVM_STACK_LIMIT * 2));

    free(bytecode);

}

static void test_evm_returns_error_on_invalid_bytecode(void) {

    evm_t evm;
    test_init_evm(&evm);
    
    const char *input_hex = "600260010";
    size_t bytecode_len = 0;
    uint8_t *bytecode = NULL;

    uint8_t result[UINT256_BYTES];
    memset(result, 0, UINT256_BYTES);
    result[31] = 0x03;

    bytecode = hex_to_bytes(input_hex, &bytecode_len);

    evm_status_t status = test_evm_state(&evm, bytecode, bytecode_len);
    
    TEST_ASSERT(bytecode == NULL); 
    TEST_ASSERT(status == EVM_INTERNAL_ERROR);
    TEST_ASSERT(evm.stack_pointer == 0);
    TEST_ASSERT(evm.pc == 0); 

    free(bytecode);

}

static void test_evm_returns_invalid_opcode_on_FE_opcode(void) {
    
    evm_t evm;
    test_init_evm(&evm);

    const char *input_hex = "6001612030fe";
    size_t bytecode_len = 0;
    uint8_t *bytecode = NULL;
    
    uint8_t last_element[UINT256_BYTES];
    
    memset(last_element, 0, UINT256_BYTES);

    last_element[31] = 0x30;
    last_element[30] = 0x20;

    bytecode = hex_to_bytes(input_hex, &bytecode_len);

    evm_status_t status = test_evm_state(&evm, bytecode, bytecode_len);
    
    TEST_ASSERT(status == EVM_INVALID_OPCODE);
    TEST_ASSERT(evm.pc == (bytecode_len - 1));
    TEST_ASSERT(evm.stack_pointer == 2);
    TEST_ASSERT(memcmp(evm.stack[evm.stack_pointer - 1], last_element, UINT256_BYTES) == 0);

}


int main(void) {
    
    RUN_TEST(test_operation_with_insufficient_stack_elements_returns_stack_underflow);
    RUN_TEST(test_pop_with_no_stack_returns_stack_underflow);
    RUN_TEST(test_operation_with_empty_stack_returns_stack_underflow);
    RUN_TEST(test_push_over_stack_limit_returns_stack_overflow);
    RUN_TEST(test_evm_returns_error_on_invalid_bytecode);
    RUN_TEST(test_evm_returns_invalid_opcode_on_FE_opcode);
    
    return 0;

} 
