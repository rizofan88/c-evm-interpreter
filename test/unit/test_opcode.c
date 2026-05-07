#include "test.h"

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "evm.h"
#include "stack.h"
#include "types.h"
#include "opcode.h"

static void make_opcodes(size_t count,
                         uint8_t begin_opcode,
                         const evm_opcode_t *opcodes[count] ) 
{

    for (size_t i = 0; i < count; i++) {
        opcodes[i] = evm_get_opcode( (uint8_t)(begin_opcode + i) );
    }

}

static void test_get_opcode_returns_requested_opcode(void) {
    
    const evm_opcode_t *opcode = evm_get_opcode(0x00);

    TEST_ASSERT(opcode != NULL);

    TEST_ASSERT(opcode->byte == 0x00);
    TEST_ASSERT(strcmp(opcode->mnemonic, "STOP") == 0);
    TEST_ASSERT(opcode->imm_bytes == 0);

}

static void test_return_opcode_has_correct_mnemonic(void) {

    const evm_opcode_t *opcode = evm_get_opcode(0x01);

    TEST_ASSERT(opcode != NULL);
    TEST_ASSERT(strcmp(opcode->mnemonic, "ADD") == 0);

}

static void test_non_push_opcode_has_zero_immediate(void) {
    
    const evm_opcode_t *add = evm_get_opcode(0x01);
    const evm_opcode_t *pop = evm_get_opcode(0x50);
    const evm_opcode_t *stop = evm_get_opcode(0x00);

    TEST_ASSERT(add != NULL);
    TEST_ASSERT(pop != NULL);
    TEST_ASSERT(stop != NULL);

    TEST_ASSERT(add->imm_bytes == 0);
    TEST_ASSERT(pop->imm_bytes == 0);
    TEST_ASSERT(stop->imm_bytes == 0);

}

static void test_get_opcode_returns_expected_metadata(void) {
    
    const evm_opcode_t *mul = evm_get_opcode(0x02);
    const evm_opcode_t *sub = evm_get_opcode(0x03);
    const evm_opcode_t *mstore= evm_get_opcode(0x52);

    TEST_ASSERT(mul != NULL);
    TEST_ASSERT(sub != NULL);
    TEST_ASSERT(mstore != NULL);

    TEST_ASSERT(mul->byte == 0x02);
    TEST_ASSERT(sub->byte == 0x03);
    TEST_ASSERT(mstore->byte == 0x52);

    TEST_ASSERT(mul->imm_bytes == 0);
    TEST_ASSERT(sub->imm_bytes == 0);
    TEST_ASSERT(mstore->imm_bytes == 0);

    TEST_ASSERT(strcmp(mul->mnemonic, "MUL") == 0);
    TEST_ASSERT(strcmp(sub->mnemonic , "SUB") == 0);
    TEST_ASSERT(strcmp(mstore->mnemonic, "MSTORE") == 0);

}


static void test_get_opcode_range_returns_valid_opcode(void) {
    
    size_t count = 12;
    uint8_t begin_opcode = 0;

    const evm_opcode_t *opcodes[count];

    make_opcodes( count, begin_opcode, opcodes);

    for(size_t i = 0; i < 12; i++) {
        TEST_ASSERT(opcodes[i] != NULL);
        TEST_ASSERT(opcodes[i]->byte == (uint8_t)i);
    }

}

static void test_push_opcodes_have_correct_immediates(void) {
    
    size_t count = 32;
    uint8_t begin_opcode = 0x60;
    
    const evm_opcode_t *opcodes[count];

    make_opcodes(count, begin_opcode, opcodes);

    for(size_t i = 0; i < 32; i++) {
        TEST_ASSERT(opcodes[i] != NULL);
        TEST_ASSERT(opcodes[i]->byte == (uint8_t)(begin_opcode + i));
        TEST_ASSERT(opcodes[i]->imm_bytes == (uint8_t)i + 1);
    }

}

static void test_push_mnemonic_matches_immediate(void) {
    
    size_t count = 32;
    uint8_t begin_opcode = 0x60;
    
    const evm_opcode_t *opcodes[count];

    make_opcodes(count, begin_opcode, opcodes);

    for(size_t i = 0; i < 32; i++) {
        
        char expected[16];

        TEST_ASSERT(opcodes[i] != NULL);
        
        uint8_t imm_bytes = opcodes[i]->imm_bytes;
        const char *mnem = opcodes[i]->mnemonic;
        
        snprintf(expected, sizeof(expected), "PUSH%zu", i + 1);
        
        TEST_ASSERT(imm_bytes == (uint8_t)i + 1);
        TEST_ASSERT( strcmp(mnem, expected) == 0 );
    }

}

static void test_invalid_byte_returns_invalid_opcode(void) {

    const evm_opcode_t *opcode = evm_get_opcode(0xFE);

    TEST_ASSERT(opcode != NULL);
    TEST_ASSERT(strcmp(opcode->mnemonic, "INVALID") == 0);

}

static void test_opcodes_return_same_pointer(void) {
    
    const evm_opcode_t *first= evm_get_opcode(0x02);
    const evm_opcode_t *second= evm_get_opcode(0x02);

    TEST_ASSERT(first != NULL);
    TEST_ASSERT(second != NULL);

}


int main(void) {
   
    RUN_TEST(test_get_opcode_returns_requested_opcode); 
    RUN_TEST(test_return_opcode_has_correct_mnemonic); 
    RUN_TEST(test_non_push_opcode_has_zero_immediate); 
    RUN_TEST(test_get_opcode_returns_expected_metadata); 
    RUN_TEST(test_get_opcode_range_returns_valid_opcode); 
    RUN_TEST(test_push_opcodes_have_correct_immediates); 
    RUN_TEST(test_push_mnemonic_matches_immediate); 
    RUN_TEST(test_invalid_byte_returns_invalid_opcode); 
    RUN_TEST(test_opcodes_return_same_pointer); 

    return 0;

}
