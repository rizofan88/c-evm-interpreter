#include "test.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "evm.h"
#include "stack.h"
#include "types.h"
#include "opcode.h"
#include "utils.h"

static void test_conversion_string_to_bytes(void) {
   
    char *bytecode = "6040610aff";
    size_t bytecode_len = 0;
    uint8_t *bytes = NULL;

    uint8_t expected[] = {0x60, 0x40, 0x61, 0x0a, 0xff};

    bytes = hex_to_bytes(bytecode, &bytecode_len);
    
    TEST_ASSERT(bytes != NULL);
    TEST_ASSERT(bytecode_len == strlen(bytecode) / 2);
    TEST_ASSERT(memcmp(bytes, expected, bytecode_len) == 0);

    free(bytes);

}

static void test_hex_to_bytes_handles_uppercase_hex(void) {
   
    char *bytecode = "6040610AFF";
    size_t bytecode_len = 0;
    uint8_t *bytes = NULL;

    uint8_t expected[] = {0x60, 0x40, 0x61, 0x0a, 0xff};

    bytes = hex_to_bytes(bytecode, &bytecode_len);
    
    TEST_ASSERT(bytes != NULL);
    TEST_ASSERT(bytecode_len == strlen(bytecode) / 2);
    TEST_ASSERT(memcmp(bytes, expected, bytecode_len) == 0);

    free(bytes);

}

static void test_hex_to_bytes_reverts_on_null_pointers(void) {
    
    char *null_bytecode = NULL;
    size_t bytecode_len = 0;

    TEST_ASSERT(hex_to_bytes(null_bytecode, &bytecode_len) == NULL);

    char *bytecode = "6040";
    size_t *null_len = NULL;
    
    TEST_ASSERT(hex_to_bytes(bytecode, null_len) == NULL);

}

static void test_hex_to_bytes_uneven_byte_length_returns_null(void) {

    char *bytecode = "6040610af";
    size_t bytecode_len = 0;

    TEST_ASSERT(hex_to_bytes(bytecode, &bytecode_len) == NULL);

}

static void test_bytes_to_uint256_converts_32_byte_big_endian_value(void) {

    const uint8_t bytes[UINT256_BYTES] = {
        0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
        0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
        0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44
    };

    uint256_t u256 = bytes_to_uint256(bytes);

    TEST_ASSERT(u256.w[3] == 0x1111111111111111ULL);
    TEST_ASSERT(u256.w[2] == 0x2222222222222222ULL);
    TEST_ASSERT(u256.w[1] == 0x3333333333333333ULL);
    TEST_ASSERT(u256.w[0] == 0x4444444444444444ULL);

}

static void test_uint256_to_bytes_converts_32_byte_big_endian_value(void) {

    const uint8_t bytes[UINT256_BYTES] = {
        0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
        0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
        0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44
    };

    uint256_t u256 = bytes_to_uint256(bytes);

    TEST_ASSERT(u256.w[3] == 0x1111111111111111ULL);
    TEST_ASSERT(u256.w[2] == 0x2222222222222222ULL);
    TEST_ASSERT(u256.w[1] == 0x3333333333333333ULL);
    TEST_ASSERT(u256.w[0] == 0x4444444444444444ULL);
    
    uint8_t *converted_bytes = uint256_to_bytes(&u256);

    TEST_ASSERT(converted_bytes != NULL);
    TEST_ASSERT(memcmp(bytes, converted_bytes, UINT256_BYTES) == 0);
    
    free(converted_bytes);

}

static void test_max_uint256_round_trip_preserves_value(void) {
    
    uint8_t original[UINT256_BYTES];
    memset(original, 0xff, UINT256_BYTES);

    uint256_t u256 = bytes_to_uint256(original);
    uint8_t *converted = uint256_to_bytes(&u256);

    TEST_ASSERT(memcmp(original, converted, UINT256_BYTES) == 0);

}


int main(void) {
    
    RUN_TEST(test_conversion_string_to_bytes);
    RUN_TEST(test_hex_to_bytes_handles_uppercase_hex);
    RUN_TEST(test_hex_to_bytes_reverts_on_null_pointers);
    RUN_TEST(test_hex_to_bytes_uneven_byte_length_returns_null);
    RUN_TEST(test_bytes_to_uint256_converts_32_byte_big_endian_value);
    RUN_TEST(test_uint256_to_bytes_converts_32_byte_big_endian_value);
    RUN_TEST(test_max_uint256_round_trip_preserves_value);

    return 0;

}
