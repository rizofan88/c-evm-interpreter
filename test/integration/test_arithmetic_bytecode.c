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

    while (!evm->stopped && evm->pc < len) {

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

static void test_evm_add_bytecode(void) {

    evm_t evm;
    test_init_evm(&evm);
    
    const char *input_hex = "6002600101";
    size_t bytecode_len = 0;
    uint8_t *bytecode = NULL;

    uint8_t result[UINT256_BYTES];
    memset(result, 0, UINT256_BYTES);
    result[31] = 0x03;

    bytecode = hex_to_bytes(input_hex, &bytecode_len);
    TEST_ASSERT(bytecode != NULL);

    evm_status_t status = test_evm_state(&evm, bytecode, bytecode_len);
    
    TEST_ASSERT(status == EVM_OK);
    TEST_ASSERT(evm.stack_pointer == 1);
    TEST_ASSERT(evm.pc == bytecode_len);
    TEST_ASSERT(memcmp(evm.stack[evm.stack_pointer - 1], result, UINT256_BYTES) == 0);

    free(bytecode);

}

static void test_evm_sub_bytecode(void) {

    evm_t evm;
    test_init_evm(&evm);
    
    const char *input_hex = "6002600303";
    size_t bytecode_len = 0;
    uint8_t *bytecode = NULL;

    uint8_t result[UINT256_BYTES];
    memset(result, 0, UINT256_BYTES);
    result[31] = 0x01;

    bytecode = hex_to_bytes(input_hex, &bytecode_len);
    TEST_ASSERT(bytecode != NULL);

    evm_status_t status = test_evm_state(&evm, bytecode, bytecode_len);
    
    TEST_ASSERT(status == EVM_OK);
    TEST_ASSERT(evm.stack_pointer == 1);
    TEST_ASSERT(evm.pc == bytecode_len);
    TEST_ASSERT(memcmp(evm.stack[evm.stack_pointer - 1], result, UINT256_BYTES) == 0);

    free(bytecode);

}

static void test_evm_mul_bytecode(void) {

    evm_t evm;
    test_init_evm(&evm);
    
    const char *input_hex = "6002600302";
    size_t bytecode_len = 0;
    uint8_t *bytecode = NULL;

    uint8_t result[UINT256_BYTES];
    memset(result, 0, UINT256_BYTES);
    result[31] = 0x06;

    bytecode = hex_to_bytes(input_hex, &bytecode_len);
    TEST_ASSERT(bytecode != NULL);

    evm_status_t status = test_evm_state(&evm, bytecode, bytecode_len);
    
    TEST_ASSERT(status == EVM_OK);
    TEST_ASSERT(evm.stack_pointer == 1);
    TEST_ASSERT(evm.pc == bytecode_len);
    TEST_ASSERT(memcmp(evm.stack[evm.stack_pointer - 1], result, UINT256_BYTES) == 0);

    free(bytecode);

}

static void test_evm_div_bytecode(void) {

    evm_t evm;
    test_init_evm(&evm);
    
    const char *input_hex = "6002600604";
    size_t bytecode_len = 0;
    uint8_t *bytecode = NULL;

    uint8_t result[UINT256_BYTES];
    memset(result, 0, UINT256_BYTES);
    result[31] = 0x03;

    bytecode = hex_to_bytes(input_hex, &bytecode_len);
    TEST_ASSERT(bytecode != NULL);

    evm_status_t status = test_evm_state(&evm, bytecode, bytecode_len);
    
    TEST_ASSERT(status == EVM_OK);
    TEST_ASSERT(evm.stack_pointer == 1);
    TEST_ASSERT(evm.pc == bytecode_len);
    TEST_ASSERT(memcmp(evm.stack[evm.stack_pointer - 1], result, UINT256_BYTES) == 0);

    free(bytecode);

}

static void test_evm_mod_bytecode(void) {

    evm_t evm;
    test_init_evm(&evm);
    
    const char *input_hex = "6002600606";
    size_t bytecode_len = 0;
    uint8_t *bytecode = NULL;

    uint8_t result[UINT256_BYTES];
    memset(result, 0, UINT256_BYTES);
    result[31] = 0x00;

    bytecode = hex_to_bytes(input_hex, &bytecode_len);
    TEST_ASSERT(bytecode != NULL);

    evm_status_t status = test_evm_state(&evm, bytecode, bytecode_len);
    
    TEST_ASSERT(status == EVM_OK);
    TEST_ASSERT(evm.stack_pointer == 1);
    TEST_ASSERT(evm.pc == bytecode_len);
    TEST_ASSERT(memcmp(evm.stack[evm.stack_pointer - 1], result, UINT256_BYTES) == 0);

    free(bytecode);

}

static void test_multiple_instructions_execute_as_expected(void) {

    evm_t evm;
    test_init_evm(&evm);
    
    const char *input_hex = "60026006016004026020046001600003";
    size_t bytecode_len = 0;
    uint8_t *bytecode = NULL;

    uint8_t result[UINT256_BYTES];
    memset(result, 0, UINT256_BYTES);
    result[31] = 0x01;

    uint8_t result_two[UINT256_BYTES];
    memset(result_two, 0xff, UINT256_BYTES);

    bytecode = hex_to_bytes(input_hex, &bytecode_len);
    TEST_ASSERT(bytecode != NULL);

    evm_status_t status = test_evm_state(&evm, bytecode, bytecode_len);
    
    TEST_ASSERT(status == EVM_OK);
    TEST_ASSERT(evm.stack_pointer == 2);
    TEST_ASSERT(evm.pc == bytecode_len);
    TEST_ASSERT(memcmp(evm.stack[0], result, UINT256_BYTES) == 0);
    TEST_ASSERT(memcmp(evm.stack[1], result_two, UINT256_BYTES) == 0);

    free(bytecode);

}

int main(void) {
    
    RUN_TEST(test_evm_add_bytecode);
    RUN_TEST(test_evm_sub_bytecode);
    RUN_TEST(test_evm_mul_bytecode);
    RUN_TEST(test_evm_div_bytecode);
    RUN_TEST(test_evm_mod_bytecode);
    RUN_TEST(test_multiple_instructions_execute_as_expected);
    
    return 0;

} 
