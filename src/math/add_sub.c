#include <stddef.h>
#include <stdint.h>

#include "math.h"


evm_status_t addition(const uint256_t *a, const uint256_t *b, uint256_t *out) {

    if(!a || !b || !out) 
        return EVM_INTERNAL_ERROR;

    uint64_t carry = 0;

    for(size_t i = 0; i < UINT256_LIMBS; i++) {

        uint64_t x = a->w[i];
        uint64_t y = b->w[i];

        uint64_t temp = x + carry;
        uint64_t new_carry = (temp < x);

        uint64_t res = temp + y;
        new_carry |= (res < temp);

        carry = new_carry;
        out->w[i] = res;
    }

    return EVM_OK;

}


evm_status_t subtraction(const uint256_t *a, const uint256_t *b, uint256_t *out) {
    
    if(!a || !b || !out) {
        return EVM_INTERNAL_ERROR;
    }

    uint64_t borrow = 0;

    for(size_t i = 0; i < UINT256_LIMBS; i++) {
     
        uint64_t x = a->w[i];
        uint64_t y = b->w[i];
        
        uint64_t temp = x - borrow;
        uint64_t new_borrow = (x < borrow);

        uint64_t res = temp - y;
        new_borrow |= (temp < y);

        borrow = new_borrow;

        out->w[i] = res;
    }

    return EVM_OK;
}
