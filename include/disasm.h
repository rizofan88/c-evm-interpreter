#ifndef DISASM_H
#define DISASM_H

#include <stdint.h>
#include <stddef.h>

void disasm(const uint8_t *code, size_t code_size);

#endif
