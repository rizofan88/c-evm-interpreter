#ifndef EVM_H
#define EVM_H

#include <stdint.h>
#include <stddef.h>

#include "types.h"

evm_status_t evm_state(uint8_t *bytes, size_t len);

#endif
