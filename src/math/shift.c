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
