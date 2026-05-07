#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include <stdint.h>

#define u8 "%02" PRIx8 "\n"
#define u16 "%04" PRIx16 "\n"
#define u32 "%08" PRIx32 "\n"
#define u64 "%016" PRIx64 "\n"

#define UINT256_LIMBS 4
#define UINT256_BYTES 32
#define UINT256_LIMB_BITS 64
#define UINT256_BITS (UINT256_LIMBS * UINT256_LIMB_BITS)

#define UINT256_TOP_BIT_INDEX (UINT256_BITS - 1)
#define UINT256_TOP_LIMB_BIT_INDEX (UINT256_LIMB_BITS - 1)

#define EVM_STACK_LIMIT 1024

typedef struct {
    uint64_t w[4];
} uint256_t;

typedef enum {
    EVM_OK = 0,
    EVM_STACK_OVERFLOW,
    EVM_STACK_UNDERFLOW,
    EVM_INVALID_OPCODE,
    EVM_INVALID_BYTECODE,
    MALLOC_ERROR,
    EVM_INTERNAL_ERROR
} evm_status_t;


typedef struct {
    uint8_t stack[EVM_STACK_LIMIT][UINT256_BYTES];
    size_t stack_pointer;
    uint8_t *memory;
    size_t mem_size;
    size_t pc;
    int stopped;
} evm_t;

#endif 
