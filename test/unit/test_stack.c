#include "test.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "evm.h"
#include "stack.h"
#include "types.h"
#include "opcode.h"


static void init_test_evm(evm_t *evm) {
    memset(evm, 0, sizeof(*evm));
}

static void make_test_word(uint8_t out[UINT256_BYTES], uint8_t value) {
    memset(out, 0, UINT256_BYTES);
    out[UINT256_BYTES - 1] = value;
}

static void test_new_evm_stack_starts_empty(void) {
    evm_t evm;
    init_test_evm(&evm);

    TEST_ASSERT(evm.stack_pointer == 0);
}

static void test_push_one_increments_stack_pointer(void) {
    evm_t evm;
    init_test_evm(&evm);

    uint8_t value[UINT256_BYTES];
    make_test_word(value, 42);

    TEST_ASSERT(push_one_to_stack(&evm, value) == EVM_OK);
    TEST_ASSERT(evm.stack_pointer == 1);
}

static void test_push_one_copies_full_32_byte_word(void) {
    evm_t evm;
    uint8_t value[UINT256_BYTES];

    init_test_evm(&evm);

    for (size_t i = 0; i < UINT256_BYTES; i++) {
        value[i] = (uint8_t)i;
    }

    TEST_ASSERT(push_one_to_stack(&evm, value) == EVM_OK);
    TEST_ASSERT(evm.stack_pointer == 1);
    TEST_ASSERT(memcmp(evm.stack[0], value, UINT256_BYTES) == 0);
}

static void test_push_one_multiple_values_preserve_order(void) {

    evm_t evm;
    uint8_t a[UINT256_BYTES];
    uint8_t b[UINT256_BYTES];

    init_test_evm(&evm);

    memset(a, 0xaa, UINT256_BYTES);
    memset(b, 0xbb, UINT256_BYTES);

    TEST_ASSERT(push_one_to_stack(&evm, a) == EVM_OK);
    TEST_ASSERT(push_one_to_stack(&evm, b) == EVM_OK);

    TEST_ASSERT(evm.stack_pointer == 2);
    TEST_ASSERT(memcmp(evm.stack[0], a, UINT256_BYTES) == 0);
    TEST_ASSERT(memcmp(evm.stack[1], b, UINT256_BYTES) == 0);

}

static void test_push_to_stack_push1(void) {

    evm_t evm;
    
    const uint8_t code[] = {0x60, 0x01};
    const evm_opcode_t op = { 0x60, "PUSH1", 1 };
    
    init_test_evm(&evm);
    
    TEST_ASSERT(push_to_stack(&evm, &op, code, sizeof(code)) == EVM_OK);
    TEST_ASSERT(evm.pc == 1);
    TEST_ASSERT(evm.stack_pointer == 1);
}

static void test_push_to_stack_push2(void) {

    evm_t evm;
    
    const uint8_t code[] = {0x61, 0x01, 0x02};
    const evm_opcode_t op = { 0x61, "PUSH2", 2 };
    
    uint8_t value[UINT256_BYTES];
    memset(value, 0, UINT256_BYTES);
    
    value[UINT256_BYTES - 2] = 0x01;
    value[UINT256_BYTES - 1]  = 0x02;
    
    init_test_evm(&evm);

    
    TEST_ASSERT(push_to_stack(&evm, &op, code, sizeof(code)) == EVM_OK);
    TEST_ASSERT(memcmp(evm.stack[0], value, UINT256_BYTES) == 0);
    TEST_ASSERT(evm.pc == 2);
    TEST_ASSERT(evm.stack_pointer == 1);

}

static void test_push_to_stack_push32(void) {

    evm_t evm;
      
    init_test_evm(&evm);

    uint8_t value[UINT256_BYTES];
    memset(value, 0, UINT256_BYTES);
   
    uint8_t code[UINT256_BYTES + 1];
    code[0] = 0x7f;
    
    const evm_opcode_t op = { 0x7f, "PUSH32", 32 };

    for(size_t i = 1; i < (UINT256_BYTES + 1); i++) {
        code[i] = (uint8_t)i;
        value[i - 1] = (uint8_t)i;
    }
    
    TEST_ASSERT(push_to_stack(&evm, &op, code, sizeof(code)) == EVM_OK);
    TEST_ASSERT(memcmp(evm.stack[0], value, UINT256_BYTES) == 0);
    TEST_ASSERT(evm.pc == 32);
    TEST_ASSERT(evm.stack_pointer == 1);

}

static void test_push_to_stack_pc_past_bytecode_returns_invalid_bytecode(void) {

    evm_t evm;
    init_test_evm(&evm);

    const uint8_t code[] = {0x60}; 
    const evm_opcode_t op = {0x60, "PUSH1", 1};

    evm.pc = sizeof(code); 

    TEST_ASSERT(push_to_stack(&evm, &op, code, sizeof(code)) == EVM_INVALID_BYTECODE);

}

static void test_push32_with_not_enough_immediate_bytes_returns_invalid_bytecode(void) {

    evm_t evm;
    init_test_evm(&evm);
    evm_opcode_t op = {0x7f, "PUSH32", 32};
    uint8_t code[] = {0x7f, 0x01, 0x02};

    TEST_ASSERT(push_to_stack(&evm, &op, code, sizeof(code)) == EVM_INVALID_BYTECODE);

}

static void test_pushing_to_stack_limit_returns_overflow(void) {

    evm_t evm;
    init_test_evm(&evm);

    uint8_t value[UINT256_BYTES];
    make_test_word(value, 42);
    
    for(int i = 0; i < EVM_STACK_LIMIT; i++) {
        TEST_ASSERT(push_one_to_stack(&evm, value) == EVM_OK);
    }
    
    TEST_ASSERT(evm.stack_pointer == EVM_STACK_LIMIT);
    TEST_ASSERT(push_one_to_stack(&evm, value) == EVM_STACK_OVERFLOW);
    TEST_ASSERT(evm.stack_pointer == EVM_STACK_LIMIT);

}

static void test_push_with_non_zero_pc(void) {

    evm_t evm;
    init_test_evm(&evm);

    evm_opcode_t op = {0x60, "PUSH1", 1};
    uint8_t code[] = {
                        0x00, 
                        0x60, 0x01 };
    evm.pc = 1;
    
    TEST_ASSERT(push_to_stack(&evm, &op, code, sizeof(code)) == EVM_OK); 
    TEST_ASSERT(evm.pc == 2);

}

static void test_push_null_to_stack_evm_detects_internal_error(void) {

    evm_t evm;
    init_test_evm(&evm);

    evm_opcode_t op = {0x60, "PUSH1", 1};
    uint8_t code[] = {0x60, 0x01};

    TEST_ASSERT(push_to_stack(NULL, &op, code, sizeof(code)) == EVM_INTERNAL_ERROR);
    TEST_ASSERT(push_to_stack(&evm, NULL, code, sizeof(code)) == EVM_INTERNAL_ERROR);
    TEST_ASSERT(push_to_stack(&evm, &op, NULL, sizeof(code)) == EVM_INTERNAL_ERROR);
    TEST_ASSERT(push_one_to_stack(&evm, NULL) == EVM_INTERNAL_ERROR);
    TEST_ASSERT(push_one_to_stack(NULL, code) == EVM_INTERNAL_ERROR);
    TEST_ASSERT(evm.pc == 0);

}

static void test_pc_advances_by_imm_bytes_only(void) {

    evm_t evm;
    init_test_evm(&evm);
    
    evm_opcode_t op = {0x62, "PUSH3", 3};
    uint8_t code[] = {0x62, 0x01, 0x02, 0x03};

    TEST_ASSERT(push_to_stack(&evm, &op, code, sizeof(code)) == EVM_OK);
    TEST_ASSERT(evm.pc == 3);

}

static void test_pop_one_decrements_stack_pointer(void) {

    evm_t evm;
    init_test_evm(&evm);

    uint8_t value[UINT256_BYTES];
    make_test_word(value, 42);

    size_t to_pop = 1;

    TEST_ASSERT(push_one_to_stack(&evm, value) == EVM_OK);

    TEST_ASSERT(evm.stack_pointer == 1);
    TEST_ASSERT(pop_from_stack(&evm, to_pop) == EVM_OK);
    TEST_ASSERT(evm.stack_pointer == 0);

}

static void test_pop_empty_stack_returns_underflow(void) {

    evm_t evm;
    init_test_evm(&evm);

    size_t to_pop = 1;

    TEST_ASSERT(evm.stack_pointer == 0);
    TEST_ASSERT(pop_from_stack(&evm, to_pop) == EVM_STACK_UNDERFLOW);
    TEST_ASSERT(evm.stack_pointer == 0);

}

static void test_pop_more_items_than_available_returns_underflow(void) {

    evm_t evm;
    init_test_evm(&evm);

    uint8_t value[UINT256_BYTES];
    make_test_word(value, 42);

    size_t to_pop = 2;

    TEST_ASSERT(push_one_to_stack(&evm, value) == EVM_OK);

    TEST_ASSERT(evm.stack_pointer == 1);
    TEST_ASSERT(pop_from_stack(&evm, to_pop) == EVM_STACK_UNDERFLOW);
}

int main(void) {

    RUN_TEST(test_new_evm_stack_starts_empty);
    RUN_TEST(test_push_one_copies_full_32_byte_word);
    RUN_TEST(test_push_one_multiple_values_preserve_order);
    RUN_TEST(test_push_one_increments_stack_pointer);
    
    RUN_TEST(test_push_to_stack_push1);
    RUN_TEST(test_push_to_stack_push2);
    RUN_TEST(test_push_to_stack_push32);
    
    RUN_TEST(test_push_to_stack_pc_past_bytecode_returns_invalid_bytecode);
    RUN_TEST(test_push32_with_not_enough_immediate_bytes_returns_invalid_bytecode);
    RUN_TEST(test_pc_advances_by_imm_bytes_only);
    
    RUN_TEST(test_pushing_to_stack_limit_returns_overflow);
    RUN_TEST(test_push_with_non_zero_pc);
    RUN_TEST(test_push_null_to_stack_evm_detects_internal_error);

    RUN_TEST(test_pop_one_decrements_stack_pointer);
    RUN_TEST(test_pop_empty_stack_returns_underflow);
    RUN_TEST(test_pop_more_items_than_available_returns_underflow);

    return 0;

}
