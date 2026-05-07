#include <stdlib.h>

#include "operations.h"
#include "types.h"
#include "math.h"
#include "opcode.h"
#include "utils.h"
#include "stack.h"

typedef evm_status_t (*math_op_t)(
    const uint256_t *a,
    const uint256_t *b,
    uint256_t *out
);

static evm_status_t stack_op(evm_t *evm, math_op_t op) {

    if (!evm || !op) {
        return EVM_INTERNAL_ERROR;
    }

    if (evm->stack_pointer < 2) {
        return EVM_STACK_UNDERFLOW;
    }

    uint256_t a = bytes_to_uint256(evm->stack[evm->stack_pointer - 1]);
    uint256_t b = bytes_to_uint256(evm->stack[evm->stack_pointer - 2]);
    uint256_t result;

    evm_status_t status = op(&a, &b, &result);
    if (status != EVM_OK) {
        return status;
    }

    uint8_t *to_push = uint256_to_bytes(&result);
    if (!to_push) {
        return MALLOC_ERROR;
    }

    status = pop_from_stack(evm, 2);
    if (status != EVM_OK) {
        free(to_push);
        return status;
    }

    status = push_one_to_stack(evm, to_push);

    free(to_push);
    return status;

}

evm_status_t add(evm_t *evm) {
    return stack_op(evm, addition);
}

evm_status_t mul(evm_t *evm) {
    return stack_op(evm, multiplication);
}

evm_status_t sub(evm_t *evm) {
    return stack_op(evm, subtraction);
}

evm_status_t divi(evm_t *evm) {
    return stack_op(evm, division);
}

evm_status_t mod(evm_t *evm) {
    return stack_op(evm, modulo);
}
