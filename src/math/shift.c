#include "math.h"

void uint256_shl(uint256_t *a, int n) {
    
    if(n == 0) { 
        return;
    }
    
    int k = n / 64;
    int s = n % 64;
    
    uint64_t temp[4] = {0};
    
    for(int i = 3; i >= 0; i--){
    
        if(i - k < 0) { 
            continue;
        }
        
        temp[i] = a->w[i - k] << s;
    
        if(s != 0 && i - k - 1 >= 0) {
            temp[i] |= a->w[i - k - 1] >> (64 - s);
        }
    }
    for(int i = 0; i < 4; i++) {
        a->w[i] = temp[i];
    }

}

void uint256_shr(uint256_t *a, int n) {
    
    if(n == 0) {
        return;
    }
    int k = n / 64;
    int s = n % 64;
    
    uint64_t temp[4] = {0};
   
    for(int i = 0; i < 4; i++) {
        
        if(i + k > 3) { 
            continue; 
        }
        
        temp[i] = a->w[i + k] >> s;
        
        if(s != 0 && i + k + 1 <= 3) {
            temp[i] |= a->w[i + k + 1] << (64 - s); 
        }
    }
    for(int i = 0; i < 4; i++) { 
        a->w[i] = temp[i];                           
    }

}

static int shift_too_large(const uint256_t *shift) {
    return shift->w[1] != 0 ||
           shift->w[2] != 0 ||
           shift->w[3] != 0 ||
           shift->w[0] > 255;
}

evm_status_t shift_left(const uint256_t *a, const uint256_t *b, uint256_t *out) { 
    
    if(!a || !b || !out) {
        return EVM_INTERNAL_ERROR;
    }

    if (shift_too_large(a)) {
        for (size_t i = 0; i < UINT256_LIMBS; i++) {
            out->w[i] = 0;
        }

        return EVM_OK;
    }

    *out = *b;
    uint256_shl(out, (int)a->w[0]);

    return EVM_OK;

}

evm_status_t shift_right(const uint256_t *a, const uint256_t *b, uint256_t *out) { 

    if(!a || !b || !out) {
        return EVM_INTERNAL_ERROR;
    }

    if (shift_too_large(a)) {
        for (size_t i = 0; i < UINT256_LIMBS; i++) {
            out->w[i] = 0;
        }

        return EVM_OK;
    }

    *out = *b;
    uint256_shr(out, (int)a->w[0]);

    return EVM_OK;
}
