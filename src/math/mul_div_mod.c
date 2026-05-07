#include <stddef.h>
#include <stdint.h>

#include "math.h"

typedef struct {
    uint256_t quotient;
    uint256_t remainder;
} div_result_t;


static int uint256_is_zero(const uint256_t *value) {

    if (!value) {
        return 1;
    }

    for (size_t i = 0; i < UINT256_LIMBS; i++) {
        if (value->w[i] != 0) {
            return 0;
        }
    }

    return 1;

}


static int uint256_ge(const uint256_t *a, const uint256_t *b) {

    for (size_t i = UINT256_LIMBS; i > 0; i--) {
        size_t index = i - 1;

        if (a->w[index] > b->w[index]) {
            return 1;
        }

        if (a->w[index] < b->w[index]) {
            return 0;
        }
    }

    return 1;
}


static div_result_t uint256_divmod(const uint256_t *numerator, const uint256_t *denominator) {

    div_result_t result = {
        .quotient = {0},
        .remainder = {0}
    };
    
    if (!numerator || !denominator) {
        return result;
    }
    
    if (uint256_is_zero(denominator)) {
        return result;
    }

    for (int bit_index = UINT256_TOP_BIT_INDEX; bit_index >= 0; bit_index--) {
        
        uint64_t carry = 0;
        
        for (size_t j = 0; j < UINT256_LIMBS; j++) {
            
            uint64_t new_carry = result.remainder.w[j] >> UINT256_TOP_LIMB_BIT_INDEX;

            result.remainder.w[j] = (result.remainder.w[j] << 1) | carry;
            
            carry = new_carry;
        
        }

        size_t source_limb = (size_t)bit_index / UINT256_LIMB_BITS;
        size_t source_bit  = (size_t)bit_index % UINT256_LIMB_BITS;

        uint64_t bit_value = (numerator->w[source_limb] >> source_bit) & 1ULL;

        result.remainder.w[0] |= bit_value;

        if (uint256_ge(&result.remainder, denominator)) {
        
            uint256_t temp = {0};

            subtraction(&result.remainder, denominator, &temp);
            result.remainder = temp;
            
            result.quotient.w[source_limb] |= (1ULL << source_bit);
        }
    
    }
    
    return result;
}


evm_status_t multiplication(const uint256_t *a, const uint256_t *b, uint256_t *out) {
    
    if(!a || !b || !out) {
        return EVM_INTERNAL_ERROR;
    }

    __uint128_t total[UINT256_LIMBS * 2] = {0};
    
    for(size_t i = 0; i < UINT256_LIMBS; i++) {

        for(size_t j = 0; j < 4; j++) {

            __uint128_t n = (__uint128_t)a->w[i] * b->w[j];
            
            uint64_t low = (uint64_t)n;
            uint64_t carry = n >> 64;
            
            total[i+j] += low;
            total[i+j+1] += carry;
    
        }
    
    }

    for(size_t i = 0; i < (UINT256_LIMBS * 2) - 1; i++) {
        total[i + 1] += total[i] >> UINT256_LIMB_BITS;
        total[i] &= UINT64_MAX;
    }
    
    for (size_t i = 0; i < UINT256_LIMBS; i++) {
        out->w[i] = (uint64_t)total[i];
    }

    return EVM_OK;
    
}


evm_status_t division(const uint256_t *a, const uint256_t *b, uint256_t *out) {

    if(!a || !b || !out) {
        return EVM_INTERNAL_ERROR;
    }

    div_result_t result = uint256_divmod(a, b);
    *out = result.quotient;

    return EVM_OK;

}

evm_status_t modulo(const uint256_t *a, const uint256_t *b, uint256_t *out) {
   
    if(!a || !b || !out) {
        return EVM_INTERNAL_ERROR;
    }

    div_result_t result = uint256_divmod(a, b);
    *out = result.remainder;

    return EVM_OK;

}
