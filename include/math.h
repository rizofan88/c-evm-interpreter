#ifndef MATH_H
#define MATH_H

#include "types.h"

evm_status_t addition(const uint256_t *a, const uint256_t *b, uint256_t *out);
evm_status_t subtraction(const uint256_t *a, const uint256_t *b, uint256_t *out);
evm_status_t multiplication(const uint256_t *a, const uint256_t *b, uint256_t *out);
evm_status_t division(const uint256_t *a, const uint256_t *b, uint256_t *out);
evm_status_t modulo(const uint256_t *a, const uint256_t *b, uint256_t *out);

#endif
