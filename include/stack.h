#ifndef STACK_H
#define STACK_H

#include "types.h"
#include "opcode.h"

void stack_printer(const evm_t *evm);
evm_status_t push_to_stack(evm_t *evm, 
                           const evm_opcode_t *opcode,
                           const uint8_t *bytes, 
                           size_t len);
evm_status_t push_one_to_stack(evm_t *evm, const uint8_t *bytes);
evm_status_t pop_from_stack(evm_t *evm, size_t items_to_pop);

#endif
