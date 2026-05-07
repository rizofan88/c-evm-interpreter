#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stddef.h>

#include "types.h"

uint8_t *hex_to_bytes(const char *hex, size_t *array_len);
void print_bytes32(const uint8_t *bytes);
uint256_t bytes_to_uint256(const uint8_t *bytes);
uint8_t *uint256_to_bytes(uint256_t *uint256);
void print_uint256(const char *label, const uint256_t *x);

#endif
