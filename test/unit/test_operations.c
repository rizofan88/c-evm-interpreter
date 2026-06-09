#include "test.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "evm.h"
#include "stack.h"
#include "types.h"
#include "opcode.h"
#include "operations.h"

static void init_test_evm(evm_t *evm) {
    memset(evm, 0, sizeof(*evm));
}

static void make_test_word(uint8_t out[UINT256_BYTES], uint8_t value) {
    memset(out, 0, UINT256_BYTES);
    out[UINT256_BYTES - 1] = value;
}

static void make_test_u16(uint8_t out[UINT256_BYTES], uint16_t value) {
    memset(out, 0, UINT256_BYTES);
    out[UINT256_BYTES - 2] = (uint8_t)(value >> 8);
    out[UINT256_BYTES - 1] = (uint8_t)(value);
}

static void make_test_max_uint256(uint8_t out[UINT256_BYTES]) {
    memset(out, 0xff, UINT256_BYTES);
}

static void test_calling_add_pops_two_values_and_pushes_sum(void) {
    
    evm_t evm;
    init_test_evm(&evm);
    
    uint8_t value[UINT256_BYTES];
    uint8_t value_two[UINT256_BYTES];
    uint8_t expected_res[UINT256_BYTES];
    
    make_test_word(value, 1);
    make_test_word(value_two, 2);
    make_test_word(expected_res, 3);


    TEST_ASSERT(push_one_to_stack(&evm, value) == EVM_OK);    
    TEST_ASSERT(push_one_to_stack(&evm, value_two) == EVM_OK);    
    TEST_ASSERT(evm.stack_pointer == 2);

    TEST_ASSERT(add(&evm) == EVM_OK);
    TEST_ASSERT( memcmp(evm.stack[0], expected_res, UINT256_BYTES) == 0 );

}

static void test_calling_add_without_values_on_stack_returns_underflow(void) {
    
    evm_t evm;
    init_test_evm(&evm);
    
    TEST_ASSERT(add(&evm) == EVM_STACK_UNDERFLOW);
    TEST_ASSERT(evm.stack_pointer == 0);

}

static void test_calling_add_with_insufficient_values_on_stack_returns_underflow(void) {
    
    evm_t evm;
    init_test_evm(&evm);

    uint8_t value[UINT256_BYTES];
    
    make_test_word(value, 1);

    TEST_ASSERT(push_one_to_stack(&evm, value) == EVM_OK);    
    TEST_ASSERT(evm.stack_pointer == 1); 
    TEST_ASSERT(add(&evm) == EVM_STACK_UNDERFLOW);
    TEST_ASSERT(evm.stack_pointer == 1);

}

static void test_sub_op_is_performed_in_right_order(void) {

    evm_t evm;
    init_test_evm(&evm);

    uint8_t minuend[UINT256_BYTES];
    uint8_t subtrahend[UINT256_BYTES];
    uint8_t difference[UINT256_BYTES];

    make_test_word(minuend, 3);
    make_test_word(subtrahend, 1);
    make_test_word(difference, 2);

    TEST_ASSERT(push_one_to_stack(&evm, subtrahend) == EVM_OK);
    TEST_ASSERT(push_one_to_stack(&evm, minuend) == EVM_OK);
    TEST_ASSERT(sub(&evm) == EVM_OK);

    TEST_ASSERT(memcmp(evm.stack[0], difference, UINT256_BYTES) == 0);

}

static void test_add_overflow_modulo_2_256(void) {
    
    evm_t evm;
    init_test_evm(&evm);

    uint8_t value[UINT256_BYTES];
    uint8_t value_two[UINT256_BYTES];
    uint8_t add_res[UINT256_BYTES];

    make_test_max_uint256(value);
    make_test_word(value_two, 1);
    make_test_word(add_res, 0);

    TEST_ASSERT(push_one_to_stack(&evm, value) == EVM_OK);
    TEST_ASSERT(push_one_to_stack(&evm, value_two) == EVM_OK);
    TEST_ASSERT(add(&evm) == EVM_OK);

    TEST_ASSERT(memcmp(evm.stack[0], add_res, UINT256_BYTES) == 0);

}

static void test_sub_underflow_modulo_2_256(void) {

    evm_t evm;
    init_test_evm(&evm);

    uint8_t minuend[UINT256_BYTES];
    uint8_t subtrahend[UINT256_BYTES];
    uint8_t difference[UINT256_BYTES];

    make_test_word(minuend, 0);
    make_test_word(subtrahend, 1);
    make_test_max_uint256(difference);

    TEST_ASSERT(push_one_to_stack(&evm, subtrahend) == EVM_OK);
    TEST_ASSERT(push_one_to_stack(&evm, minuend) == EVM_OK);
    TEST_ASSERT(sub(&evm) == EVM_OK);

    TEST_ASSERT(memcmp(evm.stack[0], difference, UINT256_BYTES) == 0);

}

static void test_add_carry_across_bytes(void) {
    
    evm_t evm;
    init_test_evm(&evm);

    uint8_t value[UINT256_BYTES];
    uint8_t value_two[UINT256_BYTES];
    uint8_t expected_res[UINT256_BYTES];

    make_test_word(value, 0xff);
    make_test_word(value_two, 1);
    make_test_u16(expected_res, 0x100);

    TEST_ASSERT(push_one_to_stack(&evm, value) == EVM_OK);
    TEST_ASSERT(push_one_to_stack(&evm, value_two) == EVM_OK);
    TEST_ASSERT(add(&evm) == EVM_OK);

    TEST_ASSERT(memcmp(evm.stack[0], expected_res, UINT256_BYTES) == 0);

}

static void test_sub_carry_across_bytes(void) {
    
    evm_t evm;
    init_test_evm(&evm);

    uint8_t minuend[UINT256_BYTES];
    uint8_t subtrahend[UINT256_BYTES];
    uint8_t difference[UINT256_BYTES];

    make_test_u16(minuend, 0x100);
    make_test_word(subtrahend, 0x01);
    make_test_word(difference, 0xff);

    TEST_ASSERT(push_one_to_stack(&evm, subtrahend) == EVM_OK);
    TEST_ASSERT(push_one_to_stack(&evm, minuend) == EVM_OK);
    TEST_ASSERT(sub(&evm) == EVM_OK);

    TEST_ASSERT(memcmp(evm.stack[0], difference, UINT256_BYTES) == 0);

}

static void test_div_and_mod_of_two_same_operands_are_correct(void) {
    
    evm_t evm;
    init_test_evm(&evm);

    uint8_t numerator[UINT256_BYTES];
    uint8_t denomenator[UINT256_BYTES];
    uint8_t quotient[UINT256_BYTES];

    uint8_t remainder[UINT256_BYTES];

    make_test_word(numerator, 10);
    make_test_word(denomenator, 5);
    make_test_word(quotient, 2);
    make_test_word(remainder, 0);

    TEST_ASSERT(push_one_to_stack(&evm, denomenator) == EVM_OK);
    TEST_ASSERT(push_one_to_stack(&evm, numerator) == EVM_OK);
    TEST_ASSERT(divi(&evm) == EVM_OK);

    TEST_ASSERT(memcmp(evm.stack[0], quotient, UINT256_BYTES) == 0);

    TEST_ASSERT(push_one_to_stack(&evm, denomenator) == EVM_OK);
    TEST_ASSERT(push_one_to_stack(&evm, numerator) == EVM_OK);
    TEST_ASSERT(mod(&evm) == EVM_OK);

    TEST_ASSERT(memcmp(evm.stack[1], remainder, UINT256_BYTES) == 0);

}

static void test_mul_return_result(void) {

    evm_t evm;
    init_test_evm(&evm);

    uint8_t value[UINT256_BYTES];
    uint8_t value_two[UINT256_BYTES];
    uint8_t expected_res[UINT256_BYTES];

    make_test_word(value, 0x02);
    make_test_word(value_two, 0x03);
    make_test_word(expected_res, 0x06);

    TEST_ASSERT(push_one_to_stack(&evm, value) == EVM_OK);
    TEST_ASSERT(push_one_to_stack(&evm, value_two) == EVM_OK);
    TEST_ASSERT(mul(&evm) == EVM_OK);

    TEST_ASSERT(memcmp(evm.stack[0], expected_res, UINT256_BYTES) == 0);

}

static void test_mul_overflow_modulo_2_256(void) {

    evm_t evm;
    init_test_evm(&evm);

    uint8_t value[UINT256_BYTES];
    uint8_t value_two[UINT256_BYTES];
    uint8_t expected_res[UINT256_BYTES];
    
    memset(value, 0, UINT256_BYTES);
    value[0] = 0x80;

    make_test_word(value_two, 0x02);
    make_test_word(expected_res, 0x00);

    TEST_ASSERT(push_one_to_stack(&evm, value) == EVM_OK);
    TEST_ASSERT(push_one_to_stack(&evm, value_two) == EVM_OK);
    TEST_ASSERT(mul(&evm) == EVM_OK);

    TEST_ASSERT(memcmp(evm.stack[0], expected_res, UINT256_BYTES) == 0);

}

static void test_division_by_zero_returns_zero(void) {

    evm_t evm;
    init_test_evm(&evm);

    uint8_t numerator[UINT256_BYTES];
    uint8_t denomenator[UINT256_BYTES];
    uint8_t quotient[UINT256_BYTES];
    
    make_test_word(numerator, 14);
    make_test_word(denomenator, 0);
    make_test_word(quotient, 0);
    
    TEST_ASSERT(push_one_to_stack(&evm, denomenator) == EVM_OK);
    TEST_ASSERT(push_one_to_stack(&evm, numerator) == EVM_OK);
    TEST_ASSERT(divi(&evm) == EVM_OK);

    TEST_ASSERT(memcmp(evm.stack[0], quotient, UINT256_BYTES) == 0);

}

static void test_modulo_by_zero_returns_zero(void) {

    evm_t evm;
    init_test_evm(&evm);

    uint8_t numerator[UINT256_BYTES];
    uint8_t denomenator[UINT256_BYTES];
    uint8_t remainder[UINT256_BYTES];
    
    make_test_word(numerator, 14);
    make_test_word(denomenator, 0);
    make_test_word(remainder, 0);
    
    TEST_ASSERT(push_one_to_stack(&evm, denomenator) == EVM_OK);
    TEST_ASSERT(push_one_to_stack(&evm, numerator) == EVM_OK);
    TEST_ASSERT(mod(&evm) == EVM_OK);

    TEST_ASSERT(memcmp(evm.stack[0], remainder, UINT256_BYTES) == 0);

}

static void test_shl_shifts_value_left(void) {

    evm_t evm;
    init_test_evm(&evm);

    uint8_t value[UINT256_BYTES];
    uint8_t shift[UINT256_BYTES];
    uint8_t expected_res[UINT256_BYTES];

    make_test_word(value, 0x02);
    make_test_word(shift, 0x01);
    make_test_word(expected_res, 0x04);

    TEST_ASSERT(push_one_to_stack(&evm, value) == EVM_OK);
    TEST_ASSERT(push_one_to_stack(&evm, shift) == EVM_OK);
    TEST_ASSERT(shl(&evm) == EVM_OK);

    TEST_ASSERT(evm.stack_pointer == 1);
    TEST_ASSERT(memcmp(evm.stack[0], expected_res, UINT256_BYTES) == 0);

}

static void test_shr_shifts_value_right(void) {

    evm_t evm;
    init_test_evm(&evm);

    uint8_t value[UINT256_BYTES];
    uint8_t shift[UINT256_BYTES];
    uint8_t expected_res[UINT256_BYTES];

    make_test_word(value, 0x04);
    make_test_word(shift, 0x01);
    make_test_word(expected_res, 0x02);

    TEST_ASSERT(push_one_to_stack(&evm, value) == EVM_OK);
    TEST_ASSERT(push_one_to_stack(&evm, shift) == EVM_OK);
    TEST_ASSERT(shr(&evm) == EVM_OK);

    TEST_ASSERT(evm.stack_pointer == 1);
    TEST_ASSERT(memcmp(evm.stack[0], expected_res, UINT256_BYTES) == 0);

}

int main(void) {
    
    RUN_TEST(test_calling_add_pops_two_values_and_pushes_sum);
    RUN_TEST(test_calling_add_without_values_on_stack_returns_underflow);
    RUN_TEST(test_calling_add_with_insufficient_values_on_stack_returns_underflow);
    RUN_TEST(test_sub_op_is_performed_in_right_order);
    RUN_TEST(test_sub_underflow_modulo_2_256);
    RUN_TEST(test_add_overflow_modulo_2_256);
    RUN_TEST(test_add_carry_across_bytes);
    RUN_TEST(test_sub_carry_across_bytes);
    RUN_TEST(test_div_and_mod_of_two_same_operands_are_correct);
    RUN_TEST(test_mul_return_result);
    RUN_TEST(test_mul_overflow_modulo_2_256);
    RUN_TEST(test_division_by_zero_returns_zero);
    RUN_TEST(test_modulo_by_zero_returns_zero);
    RUN_TEST(test_shl_shifts_value_left);
    RUN_TEST(test_shr_shifts_value_right);
        
    return 0;

}
